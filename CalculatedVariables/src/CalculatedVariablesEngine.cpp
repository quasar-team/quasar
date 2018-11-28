/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * CalculatedVariablesEngine.cpp
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ASNodeManager.h>
#include <LogIt.h>

#include <CalculatedVariable.h>
#include <CalculatedVariablesLogComponentId.h>
#include <CalculatedVariablesChangeListener.h>
#include <CalculatedVariablesEngine.h>
#include <ParserVariableRequestUserData.h>

#include <Utils.h>

namespace CalculatedVariables
{

void Engine::initialize()
{
    logComponentId = Log::getComponentHandle("CalcVars");
}

void Engine::registerVariableForCalculatedVariables(AddressSpace::ChangeNotifyingVariable* variable)
{
    LOG(Log::TRC, logComponentId) << "Putting on list of ParserVariables: " << variable->nodeId().toString().toUtf8();
    s_parserVariables.emplace_back(variable);
    variable->addChangeListener(ChangeListener(s_parserVariables.back())); // using back() because we just added it a line above
}

double* CalculatedVariables::Engine::parserVariableRequestHandler(const char* name, void* userData)
{
    ParserVariableRequestUserData* requestUserData = static_cast<ParserVariableRequestUserData*> (userData);
    CalculatedVariable* requestor = requestUserData->requestor;

    LOG(Log::TRC, logComponentId) <<
            "muparser asks for this variable: " << name <<
            " while instantiating: " << requestor->nodeId().toString().toUtf8();
    decltype(s_parserVariables)::iterator it = std::find_if(
            std::begin(s_parserVariables),
            std::end(s_parserVariables),
            [name](const ParserVariable& variable){return variable.name()==name;});
    if (it == std::end(s_parserVariables))
    {
        LOG(Log::ERR, logComponentId) << "Variable " << name << " can't be found. Formula error most likely?";
        throw std::runtime_error("Couldnt find formula variable. The exact error has been logged.");
    }
    else
    {
        CalculatedVariable* cv = dynamic_cast<CalculatedVariable*> (it->notifyingVariable());
        // Is this the first variable that requestor requests?
        if (requestor->valueVariables().size() + requestor->statusVariables().size() == 0)
        { // yes it's the first one. if the PV which is being requested is bound to another CV then inherit the synchronization domain, otherwise created new one

            if (!cv)
            { // it's probably a plain ChangeNotifyingVariable so it doesn't have anything to inherit from.
                if (it->synchronizer())
                    throw_runtime_error_with_origin("logic-error: havent expected any synchronizer here");
                it->synchronizer().reset(new Synchronizer());
                ++s_numSynchronizers;
                LOG(Log::TRC, logComponentId) << "Created new synchronization domain for PV " << it->name() << ": " << it->synchronizer().get();
            }
            else
            {
                if (it->synchronizer())
                    throw_runtime_error_with_origin("logic-error: havent expected any synchronizer here");
                LOG(Log::TRC, logComponentId) << "Trying to inherit synchronization domain for PV " << it->name() << " because it is bound to a CalculatedVariable";
                if (cv->valueVariables().size() > 0) // we will inherit from value vars
                    it->synchronizer() = cv->valueVariables().front()->synchronizer();
                else if (cv->statusVariables().size() > 0) // we will inherit from status vars
                    it->synchronizer() = cv->statusVariables().front()->synchronizer();
                else
                {
                    // nothing to inherit from
                    it->synchronizer().reset(new Synchronizer());
                    ++s_numSynchronizers;
                    LOG(Log::TRC, logComponentId) << "Impossible to inherit so created new synchronization domain for PV " << it->name() << ": " << it->synchronizer().get();
                }
            }
        }
        else
        { // no it is not the  first variable that we're adding - likely we will need to merge synchronization domains
            SharedSynchronizer source, target;
            if (requestor->valueVariables().size()>0)
                target = requestor->valueVariables().front()->synchronizer();
            else
                target = requestor->statusVariables().front()->synchronizer();
            if (!cv)
            { // it's probably a plain ChangeNotifyingVariable so just assign our synchronizer to it
                if (it->synchronizer())
                    throw_runtime_error_with_origin("logic-error: havent expected any synchronizer here");
                it->synchronizer() = target;
                LOG(Log::TRC, logComponentId) << "Assigned inherited synchro domain for: " << it->name() << ": " << it->synchronizer().get();
            }
            else
            {
                if (cv->valueVariables().size()>0)
                    source = cv->valueVariables().front()->synchronizer();
                else if (cv->statusVariables().size()>0)
                    source = cv->statusVariables().front()->synchronizer();
                else
                    throw_runtime_error_with_origin("case not implemented");
                // here we do real merge
                for (ParserVariable& pv: s_parserVariables)
                    if (pv.synchronizer() == source)
                        pv.synchronizer() = target;
                it->synchronizer() = target;
                s_numSynchronizers--;
                LOG(Log::TRC, logComponentId) << "Trying merging. it.synchronizer=" << it->synchronizer().get();
            }
        }


        if (requestUserData->type == ParserVariableRequestUserData::Type::Value)
            requestor->addDependentVariableForValue(&(*it));
        else if (requestUserData->type == ParserVariableRequestUserData::Type::Status)
            requestor->addDependentVariableForStatus(&(*it));
        else
            throw_runtime_error_with_origin("Enum value not handled. Report to quasar-developers.");
        it->addNotifiedVariable(requestor);
        return it->valuePtr();
    }
}

void Engine::instantiateCalculatedVariable(
        AddressSpace::ASNodeManager* nm,
        UaNodeId parentNodeId,
        const Configuration::CalculatedVariable& config)
{
    CalculatedVariable* calculatedVariable = new CalculatedVariable(
            nm->makeChildNodeId(parentNodeId, config.name().c_str()),
            config.name().c_str(),
            nm->getNameSpaceIndex(),
            nm,
            config.value(),
            config.isBoolean(),
            config.status().present(),
            config.status().present() ? *config.status() : "");

    nm->addNodeAndReference( parentNodeId, calculatedVariable, OpcUaId_Organizes);
    if (config.initialValue().present())
    {
        UaVariant variant (*config.initialValue());
        calculatedVariable->setValue(
                nullptr /*session*/,
                UaDataValue(variant, OpcUa_Good, UaDateTime::now(), UaDateTime::now()),
                OpcUa_False /*check access level*/
                );
    }
    registerVariableForCalculatedVariables(calculatedVariable);
    s_numCalculatedVariables++;
    LOG(Log::TRC, logComponentId) << "Instantiated Calculated Variable: " << calculatedVariable->nodeId().toString().toUtf8();
}

void Engine::printInstantiationStatistics()
{
    for (ParserVariable& pv : s_parserVariables)
    {
        LOG(Log::TRC, logComponentId) << "PV: " << pv.name() << " synchronizer: " << pv.synchronizer();
    }
    LOG(Log::INF, logComponentId) <<
            " #ParserVariables: " << s_parserVariables.size() <<
            " #CalculatedVariables: " << s_numCalculatedVariables <<
            " #Synchronizers: " << s_numSynchronizers;
}

/* This can be called at the end of instantiation step
 * (when no new CalculatedVariables are expected to be added).
 * It removes all ParserVariables that aren't used (and respective onChange callbacks)/
 * It will likely boost performance and reduce memory usage.
 */
void Engine::optimize()
{
    size_t numOptimized = 0;
    decltype(s_parserVariables)::iterator it;
    for (it = std::begin(s_parserVariables); it!=std::end(s_parserVariables); it++)
    {
        if (it->notifiedVariables().size() == 0)
        {
            if (it->notifyingVariable()->changeListenerSize() == 1)
            {
                it->notifyingVariable()->removeAllChangeListeners();
                it = s_parserVariables.erase(it);
                numOptimized++;
            }
            else
            {
                LOG(Log::WRN, logComponentId) << "Need to selectively remove change listeners - not implemented. Skipping optimization for this ParserVariable.";
                continue;
            }
        }
    }
    LOG(Log::INF, logComponentId) << "Optimized(suppresed) " << numOptimized << " ParserVariables not used in any formulas.";

}

Log::LogComponentHandle logComponentId = Log::INVALID_HANDLE;
std::list <ParserVariable> Engine::s_parserVariables;
size_t Engine::s_numSynchronizers = 0;
size_t Engine::s_numCalculatedVariables = 0;



} /* namespace CalculatedVariables */



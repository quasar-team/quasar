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

#include <boost/regex.hpp>

#include <regex>

namespace CalculatedVariables
{

void Engine::initialize()
{
    logComponentId = Log::getComponentHandle("CalcVars");
}

ParserVariable& Engine::registerVariableForCalculatedVariables(AddressSpace::ChangeNotifyingVariable* variable)
{
    LOG(Log::TRC, logComponentId) << "Putting on list of ParserVariables: " << variable->nodeId().toString().toUtf8();
    s_parserVariables.emplace_back(variable);
    variable->addChangeListener(ChangeListener(s_parserVariables.back())); // using back() because we just added it a line above
    return s_parserVariables.back();
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


std::string CalculatedVariables::Engine::applyFormula (
        const Configuration::CalculatedVariable& config,
        const std::string& parentObjectAddress
        )
{
    // Note (Piotr): it's of course be preferred to use std::string::const_iterator here,
    // but we need to be compatible with gcc 4.8 (CC7) and there C++11 std::string is not complete,
    // e.g. without const_iterator support.
    boost::regex cvSubstitutionRegex ("\\$([A-Za-z0-9_]+)(?:(?:\\()(\\S+)(?:\\)))?");
    boost::match_results<std::string::iterator> matched;
    std::string formulaInWork (config.value());
    bool matchedAnything (false);
    do
    {
        matchedAnything = boost::regex_search(
                formulaInWork.begin(),
                formulaInWork.end(),
                matched,
                cvSubstitutionRegex);
        if (matchedAnything)
        {
            if (matched.size() < 2 || matched.size() > 3)
                throw std::runtime_error("Unexpected parsing of regular expression "); // TODO improve it
            std::string matchedAtom = matched[1];
            if (matchedAtom == "_")
            {
                LOG(Log::INF, logComponentId) << "Before expanding $_, formulaInWork=" << formulaInWork;
                std::string::iterator from = matched[0].first;
                std::string::iterator to = matched[0].second;
                formulaInWork.replace(from, to, parentObjectAddress);
                LOG(Log::INF, logComponentId) << "After expanding $_, formulaInWork=" << formulaInWork;
            }
            else if (matchedAtom == "applyGenericFormula")
            {
                if (matched.size() != 3)
                    throw_runtime_error_with_origin("$applyGenericFormula requires an argument");
                std::string formulaId = matched[2];
                LOG(Log::INF, logComponentId) << "Formula to expand is: " << formulaId;
                try
                {
                    formulaInWork = s_genericFormulas.at(formulaId);
                    LOG(Log::INF, logComponentId) << "Used formula " << formulaInWork << " in expansion";
                }
                catch (std::out_of_range &e)
                {
                    LOG(Log::ERR, logComponentId) << "Formula named " << formulaId << " was referenced but never declared.";
                    throw std::runtime_error("applyGenericFormula error. The detailed error was logged to CalculatedVars LogIt component");
                }
            }
            else
                throw std::runtime_error("Invalid dollar expression: "+matched[0].str());
        }
    }
    while (matchedAnything);

    return formulaInWork;
}

void Engine::loadGenericFormulas (
        const Configuration::Configuration::CalculatedVariableGenericFormula_sequence& config)
{
    for (const Configuration::CalculatedVariableGenericFormula& formula : config)
    {
        // TODO: make sure it's unique!
        s_genericFormulas.emplace(formula.name(), formula.formula());
    }
}

void Engine::instantiateCalculatedVariable(
        AddressSpace::ASNodeManager* nm,
        UaNodeId parentNodeId,
        const Configuration::CalculatedVariable& config)
{
    LOG(Log::INF) << "Engine::instantiateCalculatedVariable  parentNodeId=" << parentNodeId.toString().toUtf8() << " value="<<config.value();
    // check if see any magic expression in the formula
    std::string expandedFormula = applyFormula(
            config,
            parentNodeId.toString().toUtf8());

    CalculatedVariable* calculatedVariable = new CalculatedVariable(
            nm->makeChildNodeId(parentNodeId, config.name().c_str()),
            config.name().c_str(),
            nm->getNameSpaceIndex(),
            nm,
            expandedFormula,
            config.isBoolean(),
            config.status().present(),
            config.status().present() ? *config.status() : "");

    UaStatus status = nm->addNodeAndReference( parentNodeId, calculatedVariable, OpcUaId_Organizes);
    if (!status.isGood())
        throw_runtime_error_with_origin(std::string("While adding Calculated Variable to address space:")+status.toString().toUtf8());
    ParserVariable& pv = registerVariableForCalculatedVariables(calculatedVariable);
    pv.setIsConstant(calculatedVariable->isConstant());
    calculatedVariable->setNotifiedVariable(&pv);

    if (config.initialValue().present())
    {
        UaVariant variant (*config.initialValue());
        calculatedVariable->setValue(
                nullptr /*session*/,
                UaDataValue(variant, OpcUa_Good, UaDateTime::now(), UaDateTime::now()),
                OpcUa_False /*check access level*/
                );
    }
    else
        calculatedVariable->update();

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
    for (it = std::begin(s_parserVariables); it!=std::end(s_parserVariables); )
    {
        if (it->notifiedVariables().size() == 0)
        {
            if (it->notifyingVariable()->changeListenerSize() <= 1)
            {
                it->notifyingVariable()->removeAllChangeListeners();
                CalculatedVariable* cv = dynamic_cast<CalculatedVariable*> (it->notifyingVariable());
                if (cv)
                    cv->setNotifiedVariable(nullptr);
                LOG(Log::TRC, logComponentId) << "Optimizing out: " << it->name();
                it = s_parserVariables.erase(it);
                numOptimized++;
                continue;
            }
            else
                LOG(Log::WRN, logComponentId) << "Need to selectively remove change listeners - not implemented. Skipping optimization for this ParserVariable.";
        }
        it++;
    }
    LOG(Log::INF, logComponentId) << "Optimized(suppresed) " << numOptimized << " ParserVariables not used in any formulas.";

}

void Engine::dfsAndSetSynchronizer(ParserVariable& pv, SharedSynchronizer& synchronizer)
{
    LOG(Log::TRC, logComponentId) << "traverse pv adjacent: " << pv.name() << " new_s=" << pv.synchronizer();
    pv.synchronizer() = synchronizer;
    // first try going towards "ancestors" in the calculation graph
    CalculatedVariable* notifyingCalculatedVariable = dynamic_cast<CalculatedVariable*> (pv.notifyingVariable());
    if (notifyingCalculatedVariable)
    {
        for (ParserVariable* variable : notifyingCalculatedVariable->valueVariables())
        {
            if (!variable->synchronizer() && !variable->isConstant())
                dfsAndSetSynchronizer(*variable, synchronizer);
        }
        for (ParserVariable* variable : notifyingCalculatedVariable->statusVariables())
        {
            if (!variable->synchronizer() && !variable->isConstant())
                dfsAndSetSynchronizer(*variable, synchronizer);
        }
    }
    // then try to go towards "descendants" in the calculation graph
    for (CalculatedVariable* cv : pv.notifiedVariables())
    {
        if (cv->notifiedVariable())
            if (! cv->notifiedVariable()->synchronizer() && !cv->notifiedVariable()->isConstant())
                dfsAndSetSynchronizer(*cv->notifiedVariable(), synchronizer);
    }
}

void Engine::setupSynchronization()
{
    LOG(Log::TRC, logComponentId) << "In setupSynchronization";
    decltype(s_parserVariables)::reverse_iterator it;
    for (it = s_parserVariables.rbegin(); it != s_parserVariables.rend(); it++)
    {
        if (it->notifiedVariables().size() < 1)
        {
            LOG(Log::TRC, logComponentId) << "Skipping PV because it notifies nothing: " << it->name();
            continue;
        }
        if (it->synchronizer())
        {
            LOG(Log::TRC, logComponentId) << "Skipping PV because it already has synchronization domain: " << it->name();
            continue;
        }
        if (it->isConstant())
        {
            LOG(Log::TRC, logComponentId) << "Skipping PV because it is constant. PV:" << it->name();
            continue;
        }
        SharedSynchronizer synchronizer (new Synchronizer());
        s_numSynchronizers++;
        it->synchronizer() = synchronizer;
        LOG(Log::TRC, logComponentId) << "Added new synchronizer to: " << it->name();
        dfsAndSetSynchronizer(*it, synchronizer);

    }
}

Log::LogComponentHandle logComponentId = Log::INVALID_HANDLE;
std::list <ParserVariable> Engine::s_parserVariables;
size_t Engine::s_numSynchronizers = 0;
size_t Engine::s_numCalculatedVariables = 0;
std::map<std::string, std::string> Engine::s_genericFormulas;


} /* namespace CalculatedVariables */



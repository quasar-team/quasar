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

#include <boost/xpressive/xpressive.hpp>

#define LOG_AND_THROW_ERROR(FORMULA,ERROR) \
    { \
    LOG(Log::ERR, "CalcVars") << "When instantiating " << FORMULA << " error: " << ERROR; \
    throw std::runtime_error(ERROR); \
    }

using namespace boost::xpressive;

namespace CalculatedVariables
{

const std::string DashSignVariableRepr {"__dash__"};
const std::string SlashSignVariableRepr {"__slash__"};

std::string escapeSpecialCharactersInFormula (const std::string& inputFormula);
std::string escapeSpecialCharactersInParserVariableName (const std::string& input);
static std::string replaceAll (const std::string& input, const std::string& from, const std::string& to);

void Engine::initialize()
{
    logComponentId = Log::getComponentHandle("CalcVars");
}

ParserVariable& Engine::registerVariableForCalculatedVariables(AddressSpace::ChangeNotifyingVariable* variable)
{
    LOG(Log::TRC, logComponentId) << "Putting on list of ParserVariables: " << variable->nodeId().toString().toUtf8();
    /* see if we have to do some substitutions of minus sign, etc. */
    s_parserVariables.emplace_back(
        variable,
        escapeSpecialCharactersInParserVariableName(variable->nodeId().toString().toUtf8())); // might be different from the variable name! (OPCUA-2456)
    variable->addChangeListener(ChangeListener(s_parserVariables.back())); // using back() because we just added it a line above
    return s_parserVariables.back();
}

void Engine::registerConstantForCalculatedVariables( const std::string& name, double value)
{
	LOG(Log::TRC, logComponentId) << "Putting *const* on list of ParserVariables: " << name << ", value=" << value;
    s_parserConstants.emplace(name, value);
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
        LOG(Log::ERR, logComponentId) << "Variable " << name << " can't be found. Formula error most likely? (While instantiating '" << requestor->nodeId().toString().toUtf8() << "')";
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

static std::string elaborateParent(
        const std::string& input,
        unsigned int levels,
        const std::string& thisFormulaAddress)
{
    // the assumption is that for every level to go up we find the dot and cut off at such place
    LOG(Log::TRC, logComponentId) << "Elaborating parent of address='" << input << "' for numLevels='" << levels << "'";
    std::size_t cutOffIndex = std::string::npos;
    while (levels > 0)
    {
        cutOffIndex = input.rfind('.', cutOffIndex);
        if (cutOffIndex == std::string::npos)
            LOG_AND_THROW_ERROR(thisFormulaAddress, "Not enough levels to go up!");
        levels--;
        if (levels>0)
        {
            if (cutOffIndex>0)
                cutOffIndex--;
            else
                LOG_AND_THROW_ERROR(thisFormulaAddress, "Not enough levels to go up!");
        }
    }
    std::string result = input.substr(0, cutOffIndex);
    LOG(Log::TRC, logComponentId) << "Elaboration parent result: '" << result << "'";
    return result;
}

static std::string replaceAll (const std::string& input, const std::string& from, const std::string& to)
{
    std::string replica (input);
    std::string::size_type pos;
    do
    {
        pos = replica.find(from);
        if (pos != std::string::npos)
        {
            replica.replace(pos, from.length(), to);
        }
    }
    while (pos != std::string::npos);
    return replica;
}

std::string escapeSpecialCharactersInFormula (const std::string& inputFormula)
{
    std::string dashReplaced { replaceAll(inputFormula, "\\-", DashSignVariableRepr) };
    return replaceAll(dashReplaced, "\\/", SlashSignVariableRepr);
}

std::string escapeSpecialCharactersInParserVariableName (const std::string& input)
{
    std::string dashReplaced { replaceAll(input, "-", DashSignVariableRepr) };
    return replaceAll(dashReplaced, "/", SlashSignVariableRepr);
}

std::string CalculatedVariables::Engine::elaborateFormula (
    const std::string& rawFormula,
    const Configuration::CalculatedVariable& config,
    const std::string& parentObjectAddress)
{
    // We use this one just to print some debug info.
    const std::string thisFormulaAddress = parentObjectAddress+"."+config.name();

    // Note (Piotr): it's of course be preferred to use std::string::const_iterator here,
    // but we need to be compatible with gcc 4.8 (CC7) and there C++11 std::string is not complete,
    // e.g. without const_iterator support.

    // Comment for the regex below:
    // Will match two "kinds" of expressions:
    // $xxxxxxxxx
    // $xxxxxxxxx(yyyyy)
    // Note: ?: is the non-captured group so in the end we get between 2 and 3 capture groups total.
    basic_regex<std::string::iterator> cvSubstitutionRegex = basic_regex<std::string::iterator>::compile("\\$([A-Za-z0-9_]+)(?:(?:\\()([^ \r\n\t()]+)(?:\\)))?");

    match_results<std::string::iterator> matched;
    std::string formulaInWork (rawFormula);
    bool matchedAnything (false);
    do
    {
        matchedAnything = regex_search(
            formulaInWork.begin(),
            formulaInWork.end(),
            matched,
            cvSubstitutionRegex);
        if (matchedAnything)
        {
            std::string operation = matched[1];
            bool argumentPresent = matched[2].matched;
            std::string argument = matched[2];
            if (operation == "_" || operation == "thisObjectAddress")
            {
                if (argumentPresent)
                    LOG_AND_THROW_ERROR(thisFormulaAddress, "$"+operation+" expression does not take arguments!");
                LOG(Log::TRC, logComponentId) << "Before expanding $_, formulaInWork=" << formulaInWork;
                formulaInWork.replace(/*from*/ matched[0].first, /*to*/ matched[0].second, escapeSpecialCharactersInParserVariableName(parentObjectAddress));
                LOG(Log::TRC, logComponentId) << "After expanding $_, formulaInWork=" << formulaInWork;
            }
            else if (operation == "applyGenericFormula")
            {
                if (!argumentPresent)
                    LOG_AND_THROW_ERROR(thisFormulaAddress, "$applyGenericFormula expects a single argument -- formula id");
                std::string formulaId = matched[2];
                try
                {
                    formulaInWork.replace(
                            /*from*/ matched[0].first,
                            /*to*/ matched[0].second,
                            s_genericFormulas.at(formulaId));
                }
                catch (std::out_of_range &e)
                    LOG_AND_THROW_ERROR(thisFormulaAddress, "Generic Formula id='"+formulaId+"' was referenced but never declared.");
                LOG(Log::TRC, logComponentId) << "After expanding $applyGenericFormula, formulaInWork=" << formulaInWork;
            }
            else if (operation == "parentObjectAddress")
            {
                if (!argumentPresent)
                    LOG_AND_THROW_ERROR(thisFormulaAddress, "$"+operation+" expression requires an argument");
                basic_regex<std::string::iterator> argumentFormat =
                    basic_regex<std::string::iterator>::compile("^numLevelsUp=(\\d+)$");
                match_results<std::string::iterator> myMatchResults;
                if (!regex_match(argument, myMatchResults, argumentFormat))
                    LOG_AND_THROW_ERROR(thisFormulaAddress, "Argument did not fit the expected syntax, for example: $parentObjectAddress(numLevelsUp=2)");
                unsigned int numLevelsUp = std::stoi(myMatchResults[1]);
                LOG(Log::TRC, logComponentId) << "Before expanding parentObjectAddress, formulaInWork=" << formulaInWork << ", levels=" << numLevelsUp;
                formulaInWork.replace(
                      /*from*/ matched[0].first,
                      /*to*/ matched[0].second,
                      escapeSpecialCharactersInParserVariableName(elaborateParent(parentObjectAddress, numLevelsUp, thisFormulaAddress)));
                LOG(Log::TRC, logComponentId) << "After expanding parentObjectAddress, formulaInWork=" << formulaInWork;
            }
            else
                LOG_AND_THROW_ERROR(thisFormulaAddress, "Invalid dollar expression: "+matched[0].str());
        }
    }
    while (matchedAnything);
    formulaInWork = escapeSpecialCharactersInFormula(formulaInWork);
    return formulaInWork;
}

void Engine::loadGenericFormulas (
        const Configuration::Configuration::CalculatedVariableGenericFormula_sequence& config)
{
    for (const Configuration::CalculatedVariableGenericFormula& formula : config)
    {
        bool insertionHappened = s_genericFormulas.emplace(formula.name(), formula.formula()).second;
        if (!insertionHappened)
        {
            // TODO: one day we could understand how to get line info
            LOG(Log::ERR, logComponentId) << "Couldn't store generic formula with name='" << formula.name() << "', probably it was already defined!";
            throw_runtime_error_with_origin("CalculatedVariables generic formulas error -- exact error was logged in the CalcVars LogIt");
        }
    }
}

void Engine::instantiateCalculatedVariable(
        AddressSpace::ASNodeManager* nm,
        UaNodeId parentNodeId,
        const Configuration::CalculatedVariable& config)
{
    // check if see any magic expression in the formula
    LOG(Log::TRC, logComponentId) << "Value formula before elaboration: " <<  config.value();
    std::string elaboratedValueFormula = elaborateFormula(
        config.value(),
        config,
        parentNodeId.toString().toUtf8());
    LOG(Log::TRC, logComponentId) << "Value formula after elaboration: " << elaboratedValueFormula;

    std::string elaboratedStatusFormula;
    if (config.status().present())
    {
        LOG(Log::TRC, logComponentId) << "Status formula before elaboration: " <<  config.status();
        elaboratedStatusFormula = elaborateFormula(
            config.status().get(), 
            config,
            parentNodeId.toString().toUtf8());
        LOG(Log::TRC, logComponentId) << "Status formula after elaboration: " << elaboratedStatusFormula;
    }

    CalculatedVariable* calculatedVariable = new CalculatedVariable(
        nm->makeChildNodeId(parentNodeId, config.name().c_str()),
        config.name().c_str(),
        nm->getNameSpaceIndex(),
        nm,
        elaboratedValueFormula,
        config.isBoolean(),
        config.status().present(),
        elaboratedStatusFormula);

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
        if (it->notifiedVariables().size() == 0) // i.e. there is no formula 
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

bool Engine::isConstantDefined (const std::string& id)
{
    return s_parserConstants.count(id) > 0;
}

double Engine::getValueOfConstant (const std::string& id)
{
    return s_parserConstants.at(id);
}

Log::LogComponentHandle logComponentId = Log::INVALID_HANDLE;
std::list <ParserVariable> Engine::s_parserVariables;
std::map <std::string, double> Engine::s_parserConstants;
size_t Engine::s_numSynchronizers = 0;
size_t Engine::s_numCalculatedVariables = 0;
std::map<std::string, std::string> Engine::s_genericFormulas;


} /* namespace CalculatedVariables */

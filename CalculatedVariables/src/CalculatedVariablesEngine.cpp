/*
 * CalculatedVariablesEngine.cpp
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#include <CalculatedVariable.h>
#include <CalculatedVariablesLogComponentId.h>
#include <CalculatedVariablesChangeListener.h>
#include <ASNodeManager.h>
#include <LogIt.h>
#include "../include/CalculatedVariablesEngine.h"

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
    CalculatedVariable* requestor = static_cast<CalculatedVariable*> (userData);

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
        requestor->addDependentVariable(&(*it));
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
            config.isBoolean());

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
    LOG(Log::TRC, logComponentId) << "Instantiated Calculated Variable: " << calculatedVariable->nodeId().toString().toUtf8();
}

Log::LogComponentHandle logComponentId = Log::INVALID_HANDLE;
std::list <ParserVariable> Engine::s_parserVariables;

} /* namespace CalculatedVariables */



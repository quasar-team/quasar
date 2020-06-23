/*
 * FreeVariablesEngine.cpp
 *
 *  Created on: 23 Jun 2020
 *      Author: pnikiel
 */

#include <ASNodeManager.h>
#include <FreeVariablesEngine.h>
#include <ChangeNotifyingVariable.h>
#include <Utils.h>
#include <CalculatedVariablesEngine.h>

namespace AddressSpace
{

void FreeVariablesEngine::instantiateFreeVariable(
    AddressSpace::ASNodeManager* nm,
    UaNodeId                     parentNodeId,
    const                        Configuration::FreeVariable& config
    )
{
    AddressSpace::ChangeNotifyingVariable* freeVariable = new AddressSpace::ChangeNotifyingVariable (
      nm->makeChildNodeId(
          parentNodeId,
          config.name().c_str()),
          config.name().c_str(),
      nm->getNameSpaceIndex(),
      UaVariant(),
      OpcUa_AccessLevels_CurrentReadOrWrite,
      nm);

    // type-dependent business
    UaVariant initialValue;
    if (config.type() == "String")
    {
        freeVariable->setDataType(UaNodeId(OpcUaType_String, 0));
        initialValue.setString( config.initialValue().present()? config.initialValue().get().c_str() : "");
    }
    else if (config.type() == "Double")
    {
        freeVariable->setDataType(UaNodeId(OpcUaType_Double, 0));
        initialValue.setDouble( config.initialValue().present()? std::stod(config.initialValue().get()) : 0.0 ); // TODO std::stod doesn't catch very well
        CalculatedVariables::Engine::registerVariableForCalculatedVariables(freeVariable);
    }
    else
        throw_runtime_error_with_origin("Free variable: given type seems invalid: '"+config.type()+"'");
    freeVariable->setValue(/*session*/0, UaDataValue(initialValue, OpcUa_Good, UaDateTime::now(), UaDateTime::now()), /* check access*/ OpcUa_False);
    // add to address space

    nm->addNodeAndReference( parentNodeId, freeVariable, OpcUaId_Organizes );
}

} /* namespace AddressSpace */

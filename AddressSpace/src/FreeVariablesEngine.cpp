/* © Copyright CERN, 2020.  All rights not expressly granted are reserved.
 * FreeVariablesEngine.cpp
 *
 *  Created on: 23 Jun 2020
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

#include <stdint.h>
#include <string>
#include <map>

#include <Configuration.hxx>
#include <ASNodeManager.h>
#include <FreeVariablesEngine.h>
#include <ChangeNotifyingVariable.h>
#include <Utils.h>
#include <CalculatedVariablesEngine.h>
#include <LogIt.h>

namespace AddressSpace
{

static const std::map<std::string, UaNodeId> g_freeVariableTypeToBuiltInType = {
        {"Boolean", UaNodeId(OpcUaType_Boolean, 0)},
        {"Byte",    UaNodeId(OpcUaType_Byte, 0)},
        {"SByte",   UaNodeId(OpcUaType_SByte, 0)},
        {"UInt16",  UaNodeId(OpcUaType_UInt16, 0)},
        {"Int16",   UaNodeId(OpcUaType_Int16, 0)},
        {"UInt32",  UaNodeId(OpcUaType_UInt32, 0)},
        {"Int32",   UaNodeId(OpcUaType_Int32, 0)},
        {"UInt64",  UaNodeId(OpcUaType_UInt64, 0)},
        {"Int64",   UaNodeId(OpcUaType_Int64, 0)},
        {"Double",  UaNodeId(OpcUaType_Double, 0)},
        {"Float",   UaNodeId(OpcUaType_Float, 0)},
        {"String",  UaNodeId(OpcUaType_String, 0)}
};

//! Convert from textual representation of type into its OPC-UA type definition identifier.
static UaNodeId freeVariableTypeToBuiltInType (const std::string& type)
{
    const auto it = g_freeVariableTypeToBuiltInType.find(type);
    if(it != g_freeVariableTypeToBuiltInType.end())
        return it->second;
    throw_runtime_error_with_origin("FreeVariable requested type is not supported: '" + type + "'");
}

template<typename T>
static T veryIntelligentWrapperOfConversionFunction (const std::string& str, T(*convFunc)(const std::string&, std::size_t*) )
{
    size_t pos; // we'll use it to detect potential garbage at the end of the string.

    auto result = (*convFunc)(str, &pos);
    if (pos != str.length())
        throw_runtime_error_with_origin("didn't manage to convert full literal '" + str + "' into a number of chosen type, garbage somewhere?");
    return result;
}

template<typename T, typename Twider>
static T veryIntelligentWrapperOfConversionFunctionWithRangeCheck (
        const std::string& str,
        Twider(*convFunc)(const std::string&, std::size_t*, int base))
{
    T rangeMin = std::numeric_limits<T>::min();
    T rangeMax = std::numeric_limits<T>::max();
    size_t pos; // we'll use it to detect potential garbage at the end of the string.

    Twider convertedNumber;
    try
    {
        convertedNumber = (*convFunc)(str, &pos, /*base*/10);
    }
    catch (const std::exception& e)
    {
        LOG(Log::ERR) << "While converting '" << str << "' into integer, exception: " << e.what();
        throw;
    }
    if (pos != str.length())
        throw_runtime_error_with_origin("didn't manage to convert full literal '" + str + "' into a number of chosen type, garbage somewhere?");
    if (convertedNumber < rangeMin || convertedNumber > rangeMax)
        throw_runtime_error_with_origin("value '" + str + "' out of allowed range '" + std::to_string(rangeMin) + "," + std::to_string(rangeMax) + "'");
    return convertedNumber;

}

//! The version for integer does range check, the version for floating points does not do it.
template<typename T, typename Twider> // T here will be e.g. OpcUa_UInt16 and Twider wider will be uint32_t for which a standard conversion function exists
static void initializeIntegerNumeric (
        UaVariant& out,
        void (UaVariant::*setter)(T),
        Twider(*convFunc)(const std::string&, std::size_t*, int base),
        const Configuration::FreeVariable::initialValue_optional& configInitialValue)
{
    T initialValue (0);
    if (configInitialValue.present())
        initialValue = veryIntelligentWrapperOfConversionFunctionWithRangeCheck<T, Twider>( configInitialValue.get(), convFunc );
    (out.*setter)(initialValue);
}

template<typename T> // T here will be e.g. OpcUa_Float or OpcUa_Double
static void initializeFloatingPointNumeric (
        UaVariant& out,
        void (UaVariant::*setter)(T),
        T(*convFunc)(const std::string&, std::size_t*),
        const Configuration::FreeVariable::initialValue_optional& configInitialValue)
{
    T initialValue (0);
    if (configInitialValue.present())
        initialValue = veryIntelligentWrapperOfConversionFunction( configInitialValue.get(), convFunc );
    (out.*setter)(initialValue);
}

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

    // data type
    freeVariable->setDataType(freeVariableTypeToBuiltInType(config.type()));

    // type-dependent business, the order is as per Oracle.AllQuasarDataTypes
    UaVariant initialValue;
    if (config.type() == "Boolean")
    {
        OpcUa_Boolean value = OpcUa_False; // default if not present
        if (config.initialValue().present())
        {
            std::string initialValueAsString = config.initialValue().get();
            if (initialValueAsString != "true" && initialValueAsString != "false") // booleans are from discrete set.
            {
                throw_runtime_error_with_origin("While instantiating FreeVariable '"
                        + config.name() + "' under '"
                        + parentNodeId.toString().toUtf8()
                        + " boolean literal is wrong '" + initialValueAsString + "'. Allowed values are 'true' or 'false'.");
            }
            value = initialValueAsString == "true" ? OpcUa_True : OpcUa_False;
        }
        initialValue.setBool( value );
    }
    else if (config.type() == "Byte")
        initializeIntegerNumeric( initialValue, &UaVariant::setByte, &std::stoul, config.initialValue());
    else if (config.type() == "SByte")
        initializeIntegerNumeric( initialValue, &UaVariant::setSByte, &std::stol, config.initialValue());
    else if (config.type() == "UInt16")
        initializeIntegerNumeric( initialValue, &UaVariant::setUInt16, &std::stoul, config.initialValue());
    else if (config.type() == "Int16")
        initializeIntegerNumeric( initialValue, &UaVariant::setInt16, &std::stol, config.initialValue());
    else if (config.type() == "UInt32")
        initializeIntegerNumeric( initialValue, &UaVariant::setUInt32, &std::stoul, config.initialValue());
    else if (config.type() == "Int32")
        initializeIntegerNumeric( initialValue, &UaVariant::setInt32, &std::stol, config.initialValue());
    else if (config.type() == "UInt64")
        initializeIntegerNumeric( initialValue, &UaVariant::setUInt64, &std::stoul, config.initialValue());
    else if (config.type() == "Int64")
        initializeIntegerNumeric( initialValue, &UaVariant::setInt64, &std::stol, config.initialValue());
    else if (config.type() == "Double")
        initializeFloatingPointNumeric( initialValue, &UaVariant::setDouble, &std::stod, config.initialValue());
    else if (config.type() == "Float")
        initializeFloatingPointNumeric( initialValue, &UaVariant::setFloat, &std::stof, config.initialValue());
    else if (config.type() == "String")
        initialValue.setString( config.initialValue().present()? config.initialValue().get().c_str() : "");
    else
        throw_runtime_error_with_origin("Free variable: given type seems invalid: '"+config.type()+"'");

    // if declared as numeric data type, make it available as input for calculated variables.
    if (config.type() == "Byte"   || config.type() == "SByte" || config.type() == "UInt16" || config.type() == "Int16" ||
        config.type() == "UInt32" || config.type() == "Int32" || config.type() == "UInt64" || config.type() == "Int64" ||
        config.type() == "Float"  || config.type() == "Double")
        CalculatedVariables::Engine::registerVariableForCalculatedVariables(freeVariable);

    // set the initial value (important do it after registration for calc vars, not to lose the initial update)
    freeVariable->setValue(/*session*/0, UaDataValue(initialValue, OpcUa_Good, UaDateTime::now(), UaDateTime::now()), /* check access*/ OpcUa_False);

    // access level
    switch(config.accessLevel())
    {
        case ::Configuration::accessLevel::value::R: freeVariable->setAccessLevel(OpcUa_AccessLevels_CurrentRead); break;
        case ::Configuration::accessLevel::value::RW: freeVariable->setAccessLevel(OpcUa_AccessLevels_CurrentReadOrWrite); break;
        case ::Configuration::accessLevel::value::W: freeVariable->setAccessLevel(OpcUa_AccessLevels_CurrentWrite); break;
        default: throw std::logic_error("enum case not supported; check FreeVariable XSD");
    }

    // add to address space
    UaStatus status = nm->addNodeAndReference( parentNodeId, freeVariable, OpcUaId_Organizes );
    if (!status.isGood())
        throw_runtime_error_with_origin("When adding the free variable '" + config.name() + "' to the address space: " + status.toString().toUtf8());
}

} /* namespace AddressSpace */

/*
 * CalculatedVariablesChangeListener.cpp
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#include <LogIt.h>

#include <CalculatedVariablesLogComponentId.h>
#include <CalculatedVariablesChangeListener.h>
#include <ParserVariable.h>

#include <ChangeNotifyingVariable.h>

namespace CalculatedVariables
{

ChangeListener::ChangeListener(ParserVariable& variable):
        m_variable(variable)
{
}

void CalculatedVariables::ChangeListener::operator ()(
        AddressSpace::ChangeNotifyingVariable& fromWhere,
        const UaDataValue&                     newValue)
{
    LOG(Log::TRC, logComponentId) << "ChangeListener fired, fromWhere=" << fromWhere.nodeId().toString().toUtf8();
    if (newValue.value())
    {
        UaVariant variant (*newValue.value());
        OpcUa_Double value;
        if (variant.toDouble(value) == OpcUa_Good)
            {
                LOG(Log::TRC, logComponentId) << "new value is: " << value;
                m_variable.setValue(value, ParserVariable::State::Good);
            }
        else
        {
                LOG(Log::WRN, logComponentId) << "didnt manage to convert the value to double";
                m_variable.setValue(0, ParserVariable::State::Bad);
        }
    }
    else
    {
        LOG(Log::WRN, logComponentId) << "passed null value";
        m_variable.setValue(0, ParserVariable::State::Bad);
    }
}

} /* namespace CalculatedVariables */



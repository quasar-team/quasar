/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * CalculatedVariablesChangeListener.cpp
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
    if (newValue.statusCode() == OpcUa_BadWaitingForInitialData)
        m_variable.setValue(0, ParserVariable::WaitingInitialData);
    else if ((newValue.statusCode() & 0x80000000) != 0)
        m_variable.setValue(0, ParserVariable::State::Bad);
    else if (newValue.value())
    {
        UaVariant variant (*newValue.value());
        if (variant.isEmpty())
        {
            LOG(Log::DBG, logComponentId) << "Variant empty, probably storing NULL.";
            m_variable.setValue(0, ParserVariable::State::Bad);
        }
        else
        {
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
    }
    else
    {
        LOG(Log::WRN, logComponentId) << "passed null value";
        m_variable.setValue(0, ParserVariable::State::Bad);
    }
}

} /* namespace CalculatedVariables */



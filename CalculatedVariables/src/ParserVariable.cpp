/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * ParserVariable.cpp
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
#include <ChangeNotifyingVariable.h>

#include <CalculatedVariablesLogComponentId.h>
#include <ParserVariable.h>
#include <CalculatedVariable.h>

#include <mutex> // for lock_guard

namespace CalculatedVariables
{

ParserVariable::ParserVariable(AddressSpace::ChangeNotifyingVariable* notifyingVariable):
        m_notifyingVariable(notifyingVariable),
        m_value(0),
        m_state(State::WaitingInitialData),
        m_isConstant(false)
{
    LOG(Log::TRC, logComponentId) << "Created ParserVariable for: " << name();
}

std::string ParserVariable::name() const
{
    return m_notifyingVariable->nodeId().toString().toUtf8();
}

void ParserVariable::setValue(double v, State state)
{
    if (m_synchronizer)
        this->setValueSynchronized(v, state);
    else
        this->setValueNonSynchronized(v, state);
}

void ParserVariable::setValueNonSynchronized(double v, State state)
{
    m_value = v;
    m_state = state;
    for (CalculatedVariable* notifiedVariable : m_notifiedVariables)
    {
        LOG(Log::TRC, logComponentId) << "Notifying variable " << notifiedVariable->nodeId().toString().toUtf8();
        notifiedVariable->update();
    }
}

void ParserVariable::setValueSynchronized(double v, State state)
{
    std::lock_guard<Synchronizer> lock (*m_synchronizer);
    this->setValueNonSynchronized(v, state);
}

void ParserVariable::addNotifiedVariable(CalculatedVariable* notifiedVariable)
{
    LOG(Log::TRC, logComponentId) << "To ParseVariable bound to: " << name() << " adding notified variable: " << notifiedVariable->nodeId().toString().toUtf8();
    m_notifiedVariables.push_back(notifiedVariable);

}



} /* namespace CalculatedVariables */


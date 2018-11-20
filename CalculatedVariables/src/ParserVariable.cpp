/*
 * ParserVariable.cpp
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#include <LogIt.h>
#include <ChangeNotifyingVariable.h>

#include <CalculatedVariablesLogComponentId.h>
#include <ParserVariable.h>
#include <CalculatedVariable.h>

namespace CalculatedVariables
{

ParserVariable::ParserVariable(AddressSpace::ChangeNotifyingVariable* notifyingVariable):
        m_notifyingVariable(notifyingVariable),
        m_value(0),
        m_state(State::WaitingInitialData)
{
    LOG(Log::TRC, logComponentId) << "Created ParserVariable for: " << name();
}

std::string ParserVariable::name() const
{
    return m_notifyingVariable->nodeId().toString().toUtf8();
}

void ParserVariable::setValue(double v, State state)
{
    m_value = v;
    m_state = state;
    for (CalculatedVariable* notifiedVariable : m_notifiedVariables)
    {
        LOG(Log::TRC, logComponentId) << "Notifying variable " << notifiedVariable->nodeId().toString().toUtf8();
        notifiedVariable->update();
    }
}

void ParserVariable::addNotifiedVariable(CalculatedVariable* notifiedVariable)
{
    LOG(Log::TRC, logComponentId) << "To ParseVariable bound to: " << name() << " adding notified variable: " << notifiedVariable->nodeId().toString().toUtf8();
    m_notifiedVariables.push_back(notifiedVariable);
}

} /* namespace CalculatedVariables */



/*
 * CalculatedVariablesChangeListener.cpp
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#include <LogIt.h>

#include <CalculatedVariablesLogComponentId.h>
#include <CalculatedVariablesChangeListener.h>


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
    LOG(Log::TRC, logComponentId) << "ChangeListener fired.";
}

} /* namespace CalculatedVariables */



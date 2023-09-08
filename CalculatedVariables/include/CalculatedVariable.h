/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * CalculatedVariable.h
 *
 *  Created on: 19 Nov 2018
 *      Author: pnikiel, Paris Moschovakos
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

#ifndef CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLE_H_
#define CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLE_H_

#include <ChangeNotifyingVariable.h>
#include <muParser.h>
#include <ParserVariableRequestUserData.h>

namespace CalculatedVariables
{

class ParserVariable;

class CalculatedVariable: public AddressSpace::ChangeNotifyingVariable
{
public:

    CalculatedVariable(
        const UaNodeId&    nodeId,
        const UaString&    name,
        OpcUa_UInt16       browseNameNameSpaceIndex,
        NodeManagerConfig* pNodeConfig,
        const std::string& formula,
        bool               isBoolean,
        bool               hasStatusFormula,
        const std::string& statusFormula,
        UaMutexRefCounted* pSharedMutex = NULL,
        bool               autoUpdateEnabled = true);

    // Allows the user to enable or disable automatic updates of the calculated variable.
    // When automatic updates are disabled, the update() method will do nothing, and
    // the user must manually trigger updates by calling triggerRecalculation().
    void setAutoUpdate(bool enabled) { m_autoUpdateEnabled = enabled; };
    void triggerRecalculation();
    void update();

    void addDependentVariableForValue(ParserVariable* variable);
    void addDependentVariableForStatus(ParserVariable* variable);

    std::list<ParserVariable*>& valueVariables() { return m_valueVariables; }
    std::list<ParserVariable*>& statusVariables() { return m_statusVariables; }

    void setNotifiedVariable( ParserVariable* var) { m_notifiedVariable = var; }
    ParserVariable* notifiedVariable() const { return m_notifiedVariable; }

    bool isConstant () const { return m_valueVariables.size() + m_statusVariables.size() == 0; }

private:
    void initializeParser(
            mu::Parser& parser,
            const std::string& formula,
            ParserVariableRequestUserData::Type formulaType);

    /* Value-Formula part */
    mu::Parser m_valueParser;
    std::list<ParserVariable*> m_valueVariables;

    // True if the output should be boolean instead of double (e.g. when logical operators are used in formula)
    bool m_isBoolean;

    /* Status-Formula part */
    bool m_hasStatusFormula;
    mu::Parser m_statusParser;
    // Points to ParserVariables which are used by statusFormula
    std::list<ParserVariable*> m_statusVariables;

    // Keeping this reference is necessary to efficiently construct synchronization graph
    ParserVariable* m_notifiedVariable;

    // Performs an update of the calculated variable. This method is called by update() and triggerRecalculation()
    void calculate();

    // Flag to indicate whether automatic updates are enabled.
    bool m_autoUpdateEnabled;

};

}



#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLE_H_ */

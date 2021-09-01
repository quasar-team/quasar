/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * ParserVariable.h
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

#ifndef CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLE_H_
#define CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLE_H_

#include <string>
#include <list>

#include <boost/thread/recursive_mutex.hpp>

// forward-decls
namespace AddressSpace
{
class ChangeNotifyingVariable;
}

namespace CalculatedVariables
{

typedef boost::recursive_mutex Synchronizer;
typedef std::shared_ptr<Synchronizer> SharedSynchronizer;

class CalculatedVariable;

class ParserVariable
{
public:
    enum State
    {
        WaitingInitialData,
        Good,
        Bad
    };

    ParserVariable(AddressSpace::ChangeNotifyingVariable* notifyingVariable, const std::string& name);

    double value() const { return m_value; }
    double* valuePtr()  { return &m_value; }
    State state() const { return m_state; }

    void setValue(double v, State state);

    //! Will match our address space counterpart address
    std::string name() const;

    void addNotifiedVariable( CalculatedVariable* notifiedVariable );
    std::list<CalculatedVariable*> notifiedVariables() { return m_notifiedVariables; }

    AddressSpace::ChangeNotifyingVariable* notifyingVariable() { return m_notifyingVariable; }

    SharedSynchronizer& synchronizer() { return m_synchronizer; }

    bool isConstant() const { return m_isConstant; }
    void setIsConstant(bool v) { m_isConstant = v; }

private:
    //! Ptr to our Address Space counterpart, will notify us on change
    AddressSpace::ChangeNotifyingVariable* const m_notifyingVariable;

    const std::string m_name;

    //! This is the current numerical value
    double                                 m_value;

    State m_state;

    //! List of all variables that should be recomputed in case this one changes value.
    std::list<CalculatedVariable*>         m_notifiedVariables;

    SharedSynchronizer m_synchronizer;

    bool m_isConstant;

    void setValueNonSynchronized(double v, State state);
    void setValueSynchronized(double v, State state);

};

} /* namespace CalculatedVariables */

#endif /* CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLE_H_ */

/*
 * ParserVariable.h
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#ifndef CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLE_H_
#define CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLE_H_

#include <string>
#include <list>

// forward-decls
namespace AddressSpace
{
class ChangeNotifyingVariable;
}

namespace CalculatedVariables
{
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

    ParserVariable(AddressSpace::ChangeNotifyingVariable* notifyingVariable);

    double value() const { return m_value; }
    double* valuePtr()  { return &m_value; }
    void setValue(double v);

    //! Will match our address space counterpart address
    std::string name() const;

    void addNotifiedVariable( CalculatedVariable* notifiedVariable );

    const AddressSpace::ChangeNotifyingVariable* notifyingVariable() const { return m_notifyingVariable; }

private:
    //! Ptr to our Address Space counterpart, will notify us on change
    const AddressSpace::ChangeNotifyingVariable* m_notifyingVariable;

    //! This is the current numerical value
    double                                 m_value;

    State m_state;

    //! List of all variables that should be recomputed in case this one changes value.
    std::list<CalculatedVariable*>         m_notifiedVariables;

};

} /* namespace CalculatedVariables */

#endif /* CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLE_H_ */

/*
 * CalculatedVariablesChangeListener.h
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#ifndef CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESCHANGELISTENER_H_
#define CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESCHANGELISTENER_H_

class UaDataValue;

namespace AddressSpace
{
class ChangeNotifyingVariable;
}

namespace CalculatedVariables
{

class ParserVariable;

class ChangeListener
{
public:
    ChangeListener(ParserVariable& variable);

    void operator()(
            AddressSpace::ChangeNotifyingVariable& fromWhere,
            const UaDataValue&                     newValue);
private:
    ParserVariable& m_variable;
};

} /* namespace CalculatedVariables */

#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESCHANGELISTENER_H_ */

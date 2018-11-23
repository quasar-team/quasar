/*
 * ParserVariableRequestUserData.h
 *
 *  Created on: 22 Nov 2018
 *      Author: pnikiel
 */

#ifndef CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLEREQUESTUSERDATA_H_
#define CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLEREQUESTUSERDATA_H_

namespace CalculatedVariables
{

class CalculatedVariable;

struct ParserVariableRequestUserData
{
    enum Type
    {
        Value,
        Status
    };
    Type type;
    CalculatedVariable* requestor;
};

}



#endif /* CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLEREQUESTUSERDATA_H_ */

/*
 * CalculatedVariable.h
 *
 *  Created on: 19 Nov 2018
 *      Author: pnikiel
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
        UaMutexRefCounted* pSharedMutex = NULL);

    void update();

    void addDependentVariableForValue(ParserVariable* variable) { m_parserVariables.push_back(variable); }
    void addDependentVariableForStatus(ParserVariable* variable) { m_statusVariables.push_back(variable); }

private:
    void initializeParser(
            mu::Parser& parser,
            const std::string& formula,
            ParserVariableRequestUserData::Type formulaType);

    /* Value-Formula part */
    mu::Parser m_parser;
    std::list<ParserVariable*> m_parserVariables;

    //! True if the output should be boolean instead of double (e.g. when logical operators are used in formula)
    bool m_isBoolean;

    /* Status-Formula part */
    bool m_hasStatusFormula;
    mu::Parser m_statusParser;
    //! Points to ParserVariables which are used by statusFormula
    std::list<ParserVariable*> m_statusVariables;
};

}



#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLE_H_ */

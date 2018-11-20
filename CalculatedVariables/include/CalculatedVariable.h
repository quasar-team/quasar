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
        UaMutexRefCounted* pSharedMutex = NULL);

    void update();

    void addDependentVariable(ParserVariable* variable) { m_parserVariables.push_back(variable); }

private:
    mu::Parser m_parser;
    std::list<ParserVariable*> m_parserVariables;

};

}



#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLE_H_ */

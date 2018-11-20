/*
 * CalculatedVariablesEngine.h
 *
 *  Created on: 20 Nov 2018
 *      Author: pnikiel
 */

#ifndef CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESENGINE_H_
#define CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESENGINE_H_

#include <list>

#include <uanodeid.h>

#include <Configuration.hxx>
#include <ParserVariable.h>

// forward-decls
namespace AddressSpace
{
class ASNodeManager;
class ChangeNotifyingVariable;
}

namespace CalculatedVariables
{

class Engine
{
public:
    static void initialize();

    static void instantiateCalculatedVariable(
            AddressSpace::ASNodeManager* nm,
            UaNodeId parentNodeId,
            const Configuration::CalculatedVariable& config
            );

    static void registerVariableForCalculatedVariables( AddressSpace::ChangeNotifyingVariable* variable);

    //! userData should be the 'this' of a CalculatedVariable this is being requested
    static double* parserVariableRequestHandler(const char* name, void* userData);
private:
    static std::list <ParserVariable> s_parserVariables;
};

} /* namespace CalculatedVariables */

#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESENGINE_H_ */

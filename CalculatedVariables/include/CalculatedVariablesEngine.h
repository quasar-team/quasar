/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * CalculatedVariablesEngine.h
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

    static ParserVariable& registerVariableForCalculatedVariables( AddressSpace::ChangeNotifyingVariable* variable);
    static void registerConstantForCalculatedVariables( const std::string& name, double value);

    //! userData should be the 'this' of a CalculatedVariable this is being requested
    static double* parserVariableRequestHandler(const char* name, void* userData);

    static void printInstantiationStatistics ();

    static void setupSynchronization();

    static void optimize ();

    //! Perform a dfs, each found node is bound to particular synchronization domain
    static void dfsAndSetSynchronizer(ParserVariable& pv, SharedSynchronizer& synchronizer);

    //! Resolves all dollar operators until a formuls is free of them.
    static std::string elaborateFormula (
            const std::string& rawFormula,
            const Configuration::CalculatedVariable& config,
            const std::string& parentObjectAddress
            );

    static void loadGenericFormulas (
            const Configuration::Configuration::CalculatedVariableGenericFormula_sequence& config);

    static bool isConstantDefined (const std::string& id);
    static double getValueOfConstant (const std::string& id);

private:
    static std::list <ParserVariable> s_parserVariables;
    static std::map <std::string, double> s_parserConstants;
    static size_t s_numSynchronizers;
    static size_t s_numCalculatedVariables;
    static std::map<std::string, std::string> s_genericFormulas;
};

} /* namespace CalculatedVariables */

#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESENGINE_H_ */

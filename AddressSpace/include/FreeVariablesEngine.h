/*
 * FreeVariablesEngine.h
 *
 *  Created on: 23 Jun 2020
 *      Author: pnikiel
 */

#ifndef ADDRESSSPACE_INCLUDE_FREEVARIABLESENGINE_H_
#define ADDRESSSPACE_INCLUDE_FREEVARIABLESENGINE_H_

#include <uanodeid.h>
#include <Configuration.hxx>

namespace AddressSpace
{

class ASNodeManager; // forward-decl

class FreeVariablesEngine
{
public:

    static void instantiateFreeVariable(
        AddressSpace::ASNodeManager* nm,
        UaNodeId                     parentNodeId,
        const                        Configuration::FreeVariable& config);

};

} /* namespace AddressSpace */

#endif /* ADDRESSSPACE_INCLUDE_FREEVARIABLESENGINE_H_ */

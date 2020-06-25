/* © Copyright CERN, 2020.  All rights not expressly granted are reserved.
 * FreeVariablesEngine.h
 *
 *  Created on: 23 Jun 2020
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

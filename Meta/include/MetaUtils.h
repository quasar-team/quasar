/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * MetaUtils.h
 *
 *  Created on: Aug 18, 2015
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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

#ifndef META_INCLUDE_METAUTILS_H_
#define META_INCLUDE_METAUTILS_H_

#include <ASNodeManager.h>
#include "DServer.h"

namespace MetaUtils
{

void assertNodeAdded(const UaStatus& status, const UaNodeId& parentNodeId, const UaNodeId& childNodeId);

template <typename TChild, typename TParent>
void linkChildNodeToParent(TChild* childNode, TParent* parentNode, AddressSpace::ASNodeManager *nm)
{
    const UaNodeId parentNodeId = parentNode->nodeId();
    const UaStatus s = nm->addNodeAndReference( parentNodeId, childNode, OpcUaId_HasComponent);
    assertNodeAdded(s, parentNodeId, childNode->nodeId());
}

template<typename THandlerObject, typename TAddressSpaceNode>
void linkHandlerObjectAndAddressSpaceNode(THandlerObject* handlerObject, TAddressSpaceNode* addressSpaceNode)
{
	addressSpaceNode->linkDevice( handlerObject );
    handlerObject->linkAddressSpace( addressSpaceNode );
}
void setDServer(Device::DServer*);
void increaseSessionCounter();
void decreaseSessionCounter();

std::string calculateRemainingCertificateValidity(void);


} // namespace MetaUtils


#endif /* META_INCLUDE_METAUTILS_H_ */

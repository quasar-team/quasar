/* © Copyright CERN, PNPI, 2015.  All rights not expressly granted are reserved.
 * ASNodeManager.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *      Author: Viatcheslav Filimonov <vfilimon@cern.ch>
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

#ifndef ASNODEMANAGER_H_
#define ASNODEMANAGER_H_

#include <iostream>
#include <boost/function.hpp>

#include <nodemanagerbase.h>

#include <ASUtils.h>


namespace AddressSpace

{

    class ASNodeManager : public NodeManagerBase
    {
	UA_DISABLE_COPY(ASNodeManager);
    public:
	ASNodeManager();
	virtual ~ASNodeManager();

	virtual UaStatus afterStartUp();
	virtual UaStatus beforeShutDown();

#ifndef BACKEND_OPEN62541
    UaObject * getInstanceDeclarationObjectType (OpcUa_UInt32 typeId);

    virtual IOManager* getIOManager(UaNode* pUaNode, OpcUa_Int32 attributeId) const;
#endif

    UaNodeId makeChildNodeId (const UaNodeId &parent, const UaString& childName);

    const UaNodeId getTypeNodeId (unsigned int numericalType);
    void setAfterStartupDelegate( boost::function<UaStatus ()> afterStartUpDelegate );

	UaStatus addUnreferencedNode( UaNode* node ) { m_unreferencedNodes.push_back(node); return OpcUa_Good; }
	const std::list<UaNode*>& getUnreferencedNodes () const { return m_unreferencedNodes; }

  private:
    UaStatus createTypeNodes();
    boost::function<UaStatus ()> m_afterStartUpDelegate;
	std::list<UaNode*> m_unreferencedNodes;
  };





}



#endif /* ASNODEMANAGER_H_ */

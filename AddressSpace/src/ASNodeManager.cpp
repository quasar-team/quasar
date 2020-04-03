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

#include <iostream>
#include <stdexcept>
#include <boost/xpressive/xpressive.hpp>

#include <ASNodeManager.h>
#include <ASInformationModel.h>
#include <ASSourceVariable.h>


using namespace std;

namespace AddressSpace

{

ASNodeManager::ASNodeManager () :
		NodeManagerBase("OPCUASERVER", OpcUa_False, 1000),
		m_afterStartUpDelegate(0)
{

}

ASNodeManager::~ASNodeManager()
{
	for (auto it = m_unreferencedNodes.begin(); it != m_unreferencedNodes.end(); it++)
	{
		(*it)->releaseReference();
	}
	m_unreferencedNodes.clear();

}



UaStatus ASNodeManager::createTypeNodes ()
{
	ASInformationModel::createNodesOfTypes (this);
	return OpcUa_Good;
}

UaStatus ASNodeManager::afterStartUp()
{
	UaStatus status;
	status = createTypeNodes();
	if (!status.isGood())
		return status;
	if (m_afterStartUpDelegate)
	{
		status = m_afterStartUpDelegate();
		if (!status.isGood())
			return status;
	}

	return OpcUa_Good;
}

UaStatus ASNodeManager::beforeShutDown()
{
	return OpcUa_Good;
}

	UaNodeId ASNodeManager::makeChildNodeId (const UaNodeId &parent, const UaString& childName )
	{
		/* Bear in mind that parent nodeId maybe string identifier, maybe anything else ... */
		switch (parent.identifierType())
		{
		case OpcUa_IdentifierType_Numeric:
			/* This we can safely resolve only for ObjectFolder */
			if ((parent.namespaceIndex()==0) && (parent.identifierNumeric()==OpcUaId_ObjectsFolder))
			{
				int ns = this->getNameSpaceIndex();
			    return UaNodeId (childName, ns);
			}
			else
				throw std::runtime_error ("CASUtils::makeChildNodeId: numeric identifier which can't have children.");
			break;
		case OpcUa_IdentifierType_String:
			return UaNodeId (UaString(parent.identifierString())+UaString(".")+childName, parent.namespaceIndex());
			break;
		default:
			throw std::runtime_error ("CASUtils::makeChildNodeId: not applicable for this identifier type");
		}
	}

	const UaNodeId ASNodeManager::getTypeNodeId (unsigned int numericalType)
	{
#ifdef BACKEND_OPEN62541
		return UaNodeId(UA_NS0ID_BASEOBJECTTYPE, 0 );
#else
		return UaNodeId (numericalType, getNameSpaceIndex());
#endif // BACKEND_OPEN62541
	}

#ifndef BACKEND_OPEN62541
	  IOManager* ASNodeManager::getIOManager(UaNode* pUaNode, OpcUa_Int32 attributeId) const
	  {

		  if ( attributeId==OpcUa_Attributes_Value)
		  {
			  ASSourceVariable *sv = dynamic_cast<ASSourceVariable*>(pUaNode);
			  if (sv == pUaNode)
			  {
				  return sv->getIOManager();
			  }
		  }

		  return NodeManagerBase::getIOManager (pUaNode, attributeId);
	  }
#endif // BACKEND_OPEN62541



	void ASNodeManager::setAfterStartupDelegate( std::function<UaStatus ()> afterStartUpDelegate )
	{
		m_afterStartUpDelegate = afterStartUpDelegate;
	}


}

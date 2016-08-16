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
	UaStatus s;
	s = createTypeNodes();
	if (!s.isGood())
		return s;
	if (! m_afterStartUpDelegate.empty())
	{
		s = m_afterStartUpDelegate();
		if (!s.isGood())
			return s;
	}
	
	return OpcUa_Good;
}

UaStatus ASNodeManager::beforeShutDown()
{
	return OpcUa_Good;
}

/* Inspired on OPCUA CANopen server */
/* Probably get rid of this */
//UaObject* ASNodeManager::getInstanceDeclarationObjectType (OpcUa_UInt32 typeId)
//{
//	// Try to find the instance declaration node with the numeric identifier
//	// and the namespace index of this node manager
//	UaNode* pNode = findNode (UaNodeId(typeId, getNameSpaceIndex()));
//	if (pNode != NULL && pNode->nodeClass()==OpcUa_NodeClass_ObjectType)
//		return (UaObject*) pNode;
//	else
//		throw std::logic_error (std::string(__func__)+": no satisfying object found, given typeId="+CUtils::toString(typeId));
//
//}



///* This is rather temporary */
//OpcUa_Int32  ASNodeManager::findUaVariables(OpcUa::BaseObjectType *pParObj,std::string& pattern,UaControlVariableSet& storage)
//{
//	OpcUa_Int32 nMatched =  0;
//	UaReference *pRefList = const_cast<UaReference*>( pParObj->pTargetNodes() );
//	UaString fPatten = UaString("%1.%2").arg(pParObj->getKey().toString()).arg(pattern.c_str());
//	std::string fullpatten = fPatten.toUtf8();
//	while (pRefList) {
//		nMatched = nMatched + findUaNodeVariables(pRefList->pTargetNode(),fullpatten,storage);
//		pRefList = pRefList->pNextForwardReference();
//	}
//	return nMatched;
//}

/** @brief find set of variables starting from node
	* @param pNode pointer to starting node
	* @param pattern searching pattern
	* @param storage return found variables
	* @return number of finding variables
	*/
//	OpcUa_Int32  ASNodeManager::findUaNodeVariables(UaNode *pNode,std::string & pattern,UaControlVariableSet& storage)
//	{
//		OpcUa_Int32 nMatched =  0;
//		boost::xpressive::sregex expression = boost::xpressive::sregex::compile(pattern);
//
//		std::cout << "pattern=" << pattern << std::endl;
//
//		if (pNode->nodeClass() == OpcUa_NodeClass_Variable) {
//			UaNodeId nodeid = pNode->getKey();
//			if (nodeid.identifierType()  == OpcUa_IdentifierType_String) {
//				boost::xpressive::smatch what;
//				string sName = nodeid.toString().toUtf8();
//				std::cout << "Comparing " << sName << std::endl;
//				if (boost::xpressive::regex_match(sName,what,expression))
//				{
//					cout << "matched" << endl;
//					nMatched = 1;
//					storage.push_back((UaControlVariable *)pNode);
//				}
//			}
//		}
//		else {
//			if (pNode->nodeClass() ==  OpcUa_NodeClass_Object) {
//				UaReference *pRefList = (UaReference *)pNode->getUaReferenceLists()->pTargetNodes();
//				while (pRefList) {
//					nMatched = nMatched + findUaNodeVariables(pRefList->pTargetNode(),pattern,storage);
//					pRefList = pRefList->pNextForwardReference();
//				}
//			}
//		}
//		return nMatched;
//	}

	// TODO This function actually could be a template parametrized as a type
	// Then you would do findNode<UaControlVariable>(name)
	// TODO Actually should by called findNodeByStringId

//	UaControlVariable * ASNodeManager::findUaControlVariable (std::string & fullName)
//	{
//		UaNode* node = getNode (UaNodeId (fullName.c_str(), getNameSpaceIndex()));
//
//		if (node && node == dynamic_cast<UaControlVariable*>(node) )
//		{
//			/* Alright, we deal with existing UaControlVariable or its descendant */
//			return (UaControlVariable*) node;
//		}
//		else
//			return 0;
//	}

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

	  
	  
	void ASNodeManager::setAfterStartupDelegate( boost::function<UaStatus ()> afterStartUpDelegate )
	{
		m_afterStartUpDelegate = afterStartUpDelegate;
	}


}

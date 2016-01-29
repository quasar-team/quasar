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
#include <boost/xpressive/xpressive.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
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

	/*
	// improve this
	OpcUa_Int32  findUaVariables(OpcUa::BaseObjectType *pParObj,std::string& pattern,UaControlVariableSet& storage);
	OpcUa_Int32  findUaNodeVariables(UaNode *pNode,std::string & pattern,UaControlVariableSet& storage);
	*/

	//	/* get rid of this */
	//	UaControlVariable * findUaControlVariable (std::string & fullName);

	UaNodeId makeChildNodeId (const UaNodeId &parent, const UaString& childName);



	template<typename T>
	    T* findByStringId (const std::string & stringId)
	{
	    UaNode* node = getNode (UaNodeId (stringId.c_str(), getNameSpaceIndex()));
	    if (node && node == dynamic_cast<T*>(node) )
	    {
		/* Alright, we deal with given type or its descendant */
		return (T*) node;
	    }
	    else
		return 0;
	}



	template<typename T>
	    unsigned int findVariablesByRegex (UaNode* startNode, const boost::xpressive::sregex & expression, std::vector<T*> &storage)
	{
	    if (!startNode)
		return 0; 
	    UaNodeId id = startNode->nodeId();
	    if (startNode->nodeClass() == OpcUa_NodeClass_Variable)
	    {
		if (id.identifierType() == OpcUa_IdentifierType_String)
		{

		    std::string sId = UaString(id.identifierString()).toUtf8();


		    boost::xpressive::smatch what;
		    if (boost::xpressive::regex_match( sId , what, expression ))
		    {
			std::cout << "name matched:" << sId << std::endl;
			T* t = dynamic_cast<T*>(startNode);
			if (t == startNode) /* this type itself or its descendant */
			{
			    std::cout << "type also matched" << std::endl;
			    storage.push_back( t );
			    return 1;
			}
			else
			    return 0; /* type mismatch */
		    }
		    else
			return 0; /* name mismatch */
		}
		else
		    return 0; /* strange identifier type */
	    }
	    else if (startNode->nodeClass() == OpcUa_NodeClass_Object)
	    {
	      #ifndef BACKEND_OPEN62541
		unsigned int nMatched=0;
		UaReference *pRefList = const_cast<UaReference *>(startNode->getUaReferenceLists()->pTargetNodes());
		while (pRefList)
		{
		    nMatched = nMatched + findVariablesByRegex( pRefList->pTargetNode(), expression, storage);
		    pRefList = pRefList->pNextForwardReference();
		}
		return nMatched;
#else // BACKEND_OPEN62541

#endif // BACKEND_OPEN62541
		
	    }
	    else
		return 0;
	    
	}

	template<typename T>
	    unsigned int findVariablesByPattern (UaNode* startNode, const std::string & pattern, std::vector<T*> &storage)
	{
	    if (!startNode)
		return 0; 
	    boost::xpressive::sregex expression;
	    try
	    {
		expression = boost::xpressive::sregex::compile( pattern );
	    }
	    catch (boost::xpressive::regex_error &e)
	    {
		ABORT_MESSAGE(CONCAT3(" REGEX Expression is wrong:",pattern,e.what()));
	    }
	    return findVariablesByRegex<T> (startNode, expression, storage);
	}

	template<typename T>
	    unsigned int findAllByRegex (UaNode* startNode, OpcUa_NodeClass nodeClass, const boost::xpressive::sregex & expression, std::vector<T*> &storage)
	{
	    if (!startNode)
		return 0; 

	    UaNodeId id = startNode->nodeId();
	    unsigned int numAdded=0;
	    if (startNode->nodeClass() == nodeClass)
	    {
		if (id.identifierType() == OpcUa_IdentifierType_String)
		{
		    std::string sId = UaString(id.identifierString()).toUtf8();
		    boost::xpressive::smatch what;
		    if (boost::xpressive::regex_match( sId , what, expression ))
		    {
			//std::cout << "name matched:" << sId << std::endl;
			T* t = dynamic_cast<T*>(startNode);
			if (t == startNode) /* this type itself or its descendant */
			{
			    //	std::cout << "type also matched" << std::endl;
			    storage.push_back( t );
			    numAdded++;
			}
			//else
			//	std::cout << "type didnt match:" << typeid(startNode).name() << std::endl;
		    }
		}
	    }
	    if (startNode->nodeClass() == OpcUa_NodeClass_Object)
	    {
		unsigned int nMatched=0;
		#ifdef BACKEND_OPEN62541
		BOOST_FOREACH( const UaNode::ReferencedTarget &target, *(startNode->referencedTargets()) )
		{
		    nMatched = nMatched + findAllByRegex( target.target, nodeClass, expression, storage);
		}
		#else // BACKEND_OPEN62541
		UaReference *pRefList = const_cast<UaReference *>(startNode->getUaReferenceLists()->pTargetNodes());
		while (pRefList)
		{
		    nMatched = nMatched + findAllByRegex( pRefList->pTargetNode(), nodeClass, expression, storage);
		    pRefList = pRefList->pNextForwardReference();
		}

		#endif // BACKEND_OPEN62541
		return nMatched;
	    }
	    else
		return numAdded;
	}

	template<typename T>
	    unsigned int findAllByPattern (UaNode* startNode, OpcUa_NodeClass nodeClass, const std::string & pattern, std::vector<T*> &storage)
	{
	    if (!startNode)
		return 0; 
	    boost::xpressive::sregex expression;
	    try
	    {
		expression = boost::xpressive::sregex::compile( pattern );
	    }
	    catch (boost::xpressive::regex_error &e)
	    {
		ABORT_MESSAGE(CONCAT3(" REGEX Expression is wrong:",pattern,e.what()));
	    }
	    return findAllByRegex<T> (startNode, nodeClass, expression, storage);
	}

	const UaNodeId getTypeNodeId (unsigned int numericalType);
	void setAfterStartupDelegate( boost::function<UaStatus ()> afterStartUpDelegate );

    private:
	UaStatus createTypeNodes();
	boost::function<UaStatus ()> m_afterStartUpDelegate;
    };





}



#endif /* ASNODEMANAGER_H_ */

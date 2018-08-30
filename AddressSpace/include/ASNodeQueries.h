/* © Copyright CERN, 2015-2016.  All rights not expressly granted are reserved.
 * ASNodeQueries.h
 *
 *  Created on: 03 Dec 2015
 *      Author: Piotr Nikiel <piotr@nikiel.info>
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

#ifndef __ASNODEQUERIES_H__
#define __ASNODEQUERIES_H__

#include <ASNodeManager.h>
#include <boost/xpressive/xpressive.hpp>

namespace AddressSpace
{

    template<typename T>
        T* findByStringId (ASNodeManager *nm,   const std::string & stringId)
    {

        UaNode* node = nm->getNode ( UaNodeId(stringId.c_str(), nm->getNameSpaceIndex()));
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
        unsigned int findAllByRegex (const ASNodeManager *nm, UaNode* startNode, OpcUa_NodeClass nodeClass, const boost::xpressive::sregex & expression, std::vector<T*> &storage)
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
                        //      std::cout << "type also matched" << std::endl;
                        storage.push_back( t );
                        numAdded++;
                    }
                    //else
                    //  std::cout << "type didnt match:" << typeid(startNode).name() << std::endl;
                }
            }
        }
        if (startNode->nodeClass() == OpcUa_NodeClass_Object)
        {
            unsigned int nMatched=0;
#ifdef BACKEND_OPEN62541
    for(const UaNode::ReferencedTarget &target : *(startNode->referencedTargets()) )
	{
	  nMatched = nMatched + findAllByRegex(nm, target.target, nodeClass, expression, storage);
	}
#else // BACKEND_OPEN62541
            UaReference *pRefList = const_cast<UaReference *>(startNode->getUaReferenceLists()->pTargetNodes());
            while (pRefList)
            {
                nMatched = nMatched + findAllByRegex(nm, pRefList->pTargetNode(), nodeClass, expression, storage);
                pRefList = pRefList->pNextForwardReference();
            }

#endif // BACKEND_OPEN62541
            return nMatched;
        }
        else
            return numAdded;
    }

    template<typename T>
        unsigned int findAllByPattern (const ASNodeManager *nm, UaNode* startNode, OpcUa_NodeClass nodeClass, const std::string & pattern, std::vector<T*> &storage)
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
        return findAllByRegex<T> (nm, startNode, nodeClass, expression, storage);
    }

    template<typename T>
        unsigned int findAllObjectsByPatternInNodeManager (ASNodeManager *nm, const std::string & pattern, std::vector<T*> &storage)
    {
        boost::xpressive::sregex expression;
        try
        {
            expression = boost::xpressive::sregex::compile( pattern );
        }
        catch (boost::xpressive::regex_error &e)
        {
            ABORT_MESSAGE(CONCAT3(" REGEX Expression is wrong:",pattern,e.what()));
        }
        int numReferenced = findAllByRegex<T> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, expression, storage);
        int numUnreferenced = 0;
        auto& unreferencedNodes = nm->getUnreferencedNodes();
        for (auto it = unreferencedNodes.begin(); it!=unreferencedNodes.end(); it++)
        {
            UaNodeId id = (*it)->nodeId();
            if ((*it)->nodeClass() == OpcUa_NodeClass_Object)
            {
                if (id.identifierType() == OpcUa_IdentifierType_String)
                {
                    std::string sId = UaString(id.identifierString()).toUtf8();
                    boost::xpressive::smatch what;
                    if (boost::xpressive::regex_match( sId , what, expression ))
                    {
                        //std::cout << "name matched:" << sId << std::endl;
                        T* t = dynamic_cast<T*>((*it));
                        if (t == *it) /* this type itself or its descendant */
                        {
                            //  std::cout << "type also matched" << std::endl;
                            storage.push_back( t );
                            numUnreferenced++;
                        }
                    }
                }
            }
        }
        return numReferenced + numUnreferenced;

    }

}

#endif // __ASNODEQUERIES_H__

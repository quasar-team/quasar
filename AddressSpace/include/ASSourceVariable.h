/* © Copyright CERN, 2015. All rights not expressly granted are reserved.
 * ASSourceVariable.h
 *
 *  Created on: Sep 16, 2014
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

#ifndef ASSOURCEVARIABLE_H_
#define ASSOURCEVARIABLE_H_

#ifndef BACKEND_OPEN62541

#include <opcua_basedatavariabletype.h>
#include <iomanager.h>
#include <SourceVariables.h>
#include <ASSourceVariableIoManager.h>

namespace AddressSpace
{


class ASSourceVariable: public OpcUa::BaseDataVariableType

{
public:

	ASSourceVariable (
			const UaNodeId&    nodeId,
	        const UaString&    name,
	        OpcUa_UInt16       browseNameNameSpaceIndex,
	        const UaVariant&   initialValue,
	        OpcUa_Byte         accessLevel,
	        NodeManagerConfig* pNodeConfig,
	        const UaNode*	   pParentObjectNode,
	        ASSourceVariableJobId readJobId,
	        ASSourceVariableJobId writeJobId,
	        UaMutexRefCounted* pSharedMutex = NULL):
	        	OpcUa::BaseDataVariableType (nodeId, name, browseNameNameSpaceIndex, initialValue, accessLevel, pNodeConfig, pSharedMutex),
	        	m_readOperationJobId(readJobId),
	        	m_writeOperationJobId(writeJobId),
	        	m_parentObjectNode(pParentObjectNode),
	        	m_ioManager(new ASSourceVariableIoManager(m_readOperationJobId, m_writeOperationJobId, m_parentObjectNode) )
	        {}
	virtual ~ASSourceVariable ()
	{
		delete m_ioManager;
	};

	IOManager* getIOManager () const { return m_ioManager; }


private:
	ASSourceVariableJobId m_readOperationJobId;
	ASSourceVariableJobId m_writeOperationJobId;
	const UaNode* m_parentObjectNode;
	ASSourceVariableIoManager *m_ioManager;


};

}

#endif // BACKEND_OPEN62541


#endif /* ASSOURCEVARIABLE_H_ */

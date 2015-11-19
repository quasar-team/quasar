/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASDelegatingVariable.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Piotr Nikiel
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

#ifndef ASDELEGATINGVARIABLE_H_
#define ASDELEGATINGVARIABLE_H_

#include <opcua_basedatavariabletype.h>

namespace AddressSpace
{

template<typename ObjectType>
class ASDelegatingVariable: public OpcUa::BaseDataVariableType

{
public:

	ASDelegatingVariable (
			const UaNodeId&    nodeId,
	        const UaString&    name,
	        OpcUa_UInt16       browseNameNameSpaceIndex,
	        const UaVariant&   initialValue,
	        OpcUa_Byte         accessLevel,
	        NodeManagerConfig* pNodeConfig,
	        UaMutexRefCounted* pSharedMutex = NULL):
	        	OpcUa::BaseDataVariableType (nodeId, name, browseNameNameSpaceIndex, initialValue, accessLevel, pNodeConfig, pSharedMutex),
	        	m_write(0),
	        	m_object(0) {}
	virtual ~ASDelegatingVariable () {};

	UaStatus setValue(Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel = OpcUa_True)
	{
		if (m_object && m_write && pSession!=0)
		{
			// TODO Consider whether we should update cache after that or not?
			UaStatus status =  (m_object->*m_write) (pSession, dataValue, checkAccessLevel);
			if (status.isGood())
				OpcUa::BaseDataVariableType::setValue (pSession, dataValue, checkAccessLevel);
			return status;
		}
		else
		{
			//cout << "This variable is not delegated" << endl;
			return BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
		}
	}
	UaStatus assignHandler (ObjectType* object, UaStatus (ObjectType::*method)(Session*, const UaDataValue&, OpcUa_Boolean))
	{
		m_object = object;
		m_write = method;
		return OpcUa_Good;
	}
private:
	UaStatus (ObjectType::* m_write)(Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel);
	ObjectType* m_object;
};

}
#endif /* ASDELEGATINGVARIABLE_H_ */

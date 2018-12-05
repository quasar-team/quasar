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

#include <ChangeNotifyingVariable.h>

namespace AddressSpace
{

template<typename ObjectType>
class ASDelegatingVariable: public ChangeNotifyingVariable

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
	        	ChangeNotifyingVariable (nodeId, name, browseNameNameSpaceIndex, initialValue, accessLevel, pNodeConfig, pSharedMutex),
	        	m_write(0),
	        	m_object(0) {}
	virtual ~ASDelegatingVariable () {};

	UaStatus setValue(Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel = OpcUa_True)
	{
		if (m_object && m_write && pSession!=0)
		{
			UaStatus status =  (m_object->*m_write) (pSession, dataValue, checkAccessLevel);
			if (status.isGood())
				ChangeNotifyingVariable::setValue (pSession, dataValue, checkAccessLevel);
			return status;
		}
		else
		{
			return ChangeNotifyingVariable::setValue(pSession, dataValue, checkAccessLevel);
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

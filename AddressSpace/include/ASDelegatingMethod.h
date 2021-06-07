/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASDelegatingVariable.h
 *
 *  Created on: Dec 31, 2016
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

#ifndef ADDRESSSPACE_INCLUDE_ASDELEGATINGMETHOD_H_
#define ADDRESSSPACE_INCLUDE_ASDELEGATINGMETHOD_H_

#include <LogIt.h>

namespace AddressSpace
{
template<typename ObjectType>
class ASDelegatingMethod: public OpcUa::BaseMethod

{

public:
	ASDelegatingMethod(
			const UaNodeId &  	nodeId,
			const UaString &  	name,
			OpcUa_UInt16  	browseNameNameSpaceIndex,
			UaMutexRefCounted *  	pSharedMutex = 0
			) : OpcUa::BaseMethod (
					nodeId,
					name,
					browseNameNameSpaceIndex,
					pSharedMutex),
					m_object(0),
					m_method(0)
	{

	}

	UaStatus call (
			MethodManagerCallback* pCallback,
			OpcUa_UInt32           callbackHandle,
			const UaVariantArray&  inputArguments
			)
	{
		// TODO: missing arguments
		if (m_object && m_method)
		{
			return (m_object->*m_method)( pCallback, callbackHandle, inputArguments );
		}
		else
		{
			LOG(Log::ERR) << "Method was called but handler isn't set.";
			return OpcUa_BadNotImplemented;
		}
	}


	void assignHandler(
			ObjectType* object,
			UaStatus (ObjectType::*method)(MethodManagerCallback* pCallback, OpcUa_UInt32  callbackHandle, const UaVariantArray&  inputArguments)
			)
	{
		m_method = method;
		m_object = object;
	}

private:

	// this is address space object to which the call will be routed
	ObjectType* m_object;
	UaStatus (ObjectType::*m_method)(MethodManagerCallback* pCallback, OpcUa_UInt32  callbackHandle, const UaVariantArray&  inputArguments) ;


};

}


#endif /* ADDRESSSPACE_INCLUDE_ASDELEGATINGMETHOD_H_ */

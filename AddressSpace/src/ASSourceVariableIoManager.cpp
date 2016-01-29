/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
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

#ifndef BACKEND_OPEN62541

#include <ASSourceVariableIoManager.h>
#include <LogIt.h>

using namespace std;

namespace AddressSpace
{

UaStatus ASSourceVariableIoManager::beginTransaction (
    IOManagerCallback*       pCallback,
    const ServiceContext&    serviceContext,
    OpcUa_UInt32             hTransaction,
    OpcUa_UInt32             totalItemCountHint,
    OpcUa_Double             maxAge,
    OpcUa_TimestampsToReturn timestampsToReturn,
    TransactionType          transactionType,
    OpcUa_Handle&            hIOManagerContext)
{
	m_callback = pCallback;
	m_transaction = hTransaction;
	return OpcUa_Good;
}

UaStatus ASSourceVariableIoManager::beginRead (
    OpcUa_Handle        hIOManagerContext,
    OpcUa_UInt32        callbackHandle,
    VariableHandle*     pVariableHandle,
    OpcUa_ReadValueId*  pReadValueId)
{
	m_callbackHandle = callbackHandle;
	LOG(Log::DBG) << "beginRead op=" << m_readOperationJobId << " cbkHandle=" <<callbackHandle << endl;
	if (m_readOperationJobId == ASSOURCEVARIABLE_NOTHING)
		return OpcUa_BadUserAccessDenied;
	else
		return SourceVariables_spawnIoJobRead (
				m_readOperationJobId,
				m_callback,
				m_transaction,
				m_callbackHandle,
				m_variableParentNode
				);

}

UaStatus ASSourceVariableIoManager::beginWrite (
    OpcUa_Handle        hIOManagerContext,
    OpcUa_UInt32        callbackHandle,
    VariableHandle*     pVariableHandle,
    OpcUa_WriteValue*   pWriteValue)
{
	m_callbackHandle = callbackHandle;
	LOG(Log::DBG) << "beginWrite op=" << m_writeOperationJobId << " cbkHandle=" <<callbackHandle << endl;
	if (m_writeOperationJobId == ASSOURCEVARIABLE_NOTHING)
		return OpcUa_BadUserAccessDenied;
	else
		return SourceVariables_spawnIoJobWrite (
				m_writeOperationJobId,
				m_callback,
				m_transaction,
				m_callbackHandle,
				m_variableParentNode,
				pWriteValue
				);
}

UaStatus ASSourceVariableIoManager::finishTransaction (
    OpcUa_Handle        hIOManagerContext)
{
	return OpcUa_Good;
}



}

#endif //  BACKEND_OPEN62541

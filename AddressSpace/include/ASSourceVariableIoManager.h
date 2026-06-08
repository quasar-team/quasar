/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASSourceVariableIoManager.h
 *
 *  Created on: Sep 16, 2014
 *      Author: pnikiel
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

#ifndef ASSOURCEVARIABLEIOMANAGER_H_
#define ASSOURCEVARIABLEIOMANAGER_H_

#ifndef BACKEND_OPEN62541

#include <iomanager.h>
#include <SourceVariables.h>

namespace AddressSpace
{

class ASSourceVariableIoManager: public IOManager

{
public:

	ASSourceVariableIoManager (ASSourceVariableJobId readOp, ASSourceVariableJobId writeOp, const UaNode* variableParentNode):
		m_readOperationJobId (readOp),
		m_writeOperationJobId (writeOp),
		m_variableParentNode (variableParentNode),
		m_callback(0),
		m_transaction(0),
		m_callbackHandle(0)
{}
	virtual ~ASSourceVariableIoManager () {}

    virtual UaStatus beginTransaction (
        IOManagerCallback*       pCallback,
        const ServiceContext&    serviceContext,
        OpcUa_UInt32             hTransaction,
        OpcUa_UInt32             totalItemCountHint,
        OpcUa_Double             maxAge,
        OpcUa_TimestampsToReturn timestampsToReturn,
        TransactionType          transactionType,
        OpcUa_Handle&            hIOManagerContext) override;

    virtual UaStatus beginStartMonitoring(
        OpcUa_Handle,
        OpcUa_UInt32,
        IOVariableCallback*,
        VariableHandle*,
        MonitoringContext&) override
    {
    	return OpcUa_Bad;
    }

    virtual UaStatus beginModifyMonitoring(
        OpcUa_Handle,
        OpcUa_UInt32,
        OpcUa_UInt32,
        MonitoringContext&) override
    {
    	return OpcUa_Bad;
    }

    virtual UaStatus beginStopMonitoring(
        OpcUa_Handle,
        OpcUa_UInt32,
        OpcUa_UInt32) override
    {
    	return OpcUa_Bad;
    }

    virtual UaStatus beginRead (
        OpcUa_Handle        hIOManagerContext,
        OpcUa_UInt32        callbackHandle,
        VariableHandle*     pVariableHandle,
        OpcUa_ReadValueId*  pReadValueId) override;

    virtual UaStatus beginWrite (
        OpcUa_Handle        hIOManagerContext,
        OpcUa_UInt32        callbackHandle,
        VariableHandle*     pVariableHandle,
        OpcUa_WriteValue*   pWriteValue) override;

    virtual UaStatus finishTransaction (
        OpcUa_Handle        hIOManagerContext) override;

private:
    ASSourceVariableJobId m_readOperationJobId;
    ASSourceVariableJobId m_writeOperationJobId;
    const UaNode * m_variableParentNode;
	IOManagerCallback*  m_callback;
	OpcUa_UInt32 m_transaction;
	OpcUa_UInt32 m_callbackHandle;

};


}


#endif // BACKEND_OPEN62541

#endif /* ASSOURCEVARIABLEIOMANAGER_H_ */

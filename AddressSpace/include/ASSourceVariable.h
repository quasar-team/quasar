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
	}

	ASSourceVariable (const ASSourceVariable&) = delete;
	ASSourceVariable& operator= (const ASSourceVariable&) = delete;

	IOManager* getIOManager () const { return m_ioManager; }


private:
	ASSourceVariableJobId m_readOperationJobId;
	ASSourceVariableJobId m_writeOperationJobId;
	const UaNode* m_parentObjectNode;
	ASSourceVariableIoManager *m_ioManager;


};

}

#else

#include <functional>
#include <memory>
#include <mutex>
#include <opcua_basedatavariabletype.h>
#include <QuasarThreadPool.h>
#include <SourceVariables.h>

namespace AddressSpace
{

class ASSourceVariable: public OpcUa::BaseDataVariableType
{
public:
	typedef std::function<UaStatus(UaVariant&, UaDateTime&)> ReadFn;
	typedef std::function<UaStatus(const UaVariant&)> WriteFn;
	typedef std::function<std::mutex*()> MutexFn;

	ASSourceVariable (
			const UaNodeId&    nodeId,
			const UaString&    name,
			OpcUa_UInt16       browseNameNameSpaceIndex,
			const UaVariant&   initialValue,
			OpcUa_Byte         accessLevel,
			NodeManagerConfig* pNodeConfig,
			ReadFn             readFn,
			bool               readAsynchronous,
			MutexFn            readMutexFn,
			WriteFn            writeFn,
			bool               writeAsynchronous,
			MutexFn            writeMutexFn,
			UaMutexRefCounted* pSharedMutex = NULL):
				OpcUa::BaseDataVariableType (nodeId, name, browseNameNameSpaceIndex, initialValue, accessLevel, pNodeConfig, pSharedMutex),
				m_readFn(readFn),
				m_writeFn(writeFn),
				m_readMutexFn(readMutexFn),
				m_writeMutexFn(writeMutexFn),
				m_readAsynchronous(readAsynchronous),
				m_writeAsynchronous(writeAsynchronous)
			{}

	virtual UaDataValue value (Session* session) override
	{
		if (m_readFn)
		{
			UaVariant variant;
			UaDateTime sourceTime;
			UaStatus status = m_readFn(variant, sourceTime);
			if (status.isNotGood() || variant.isEmpty())
				return OpcUa::BaseDataVariableType::value(session);
			return UaDataValue(variant, status.statusCode(), sourceTime, UaDateTime::now());
		}
		else
			return OpcUa::BaseDataVariableType::value(session);
	}

	virtual UaStatus setValue (Session* session, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel) override
	{
		if (m_writeFn)
			return m_writeFn(UaVariant(*dataValue.value()));
		else
			return OpcUa::BaseDataVariableType::setValue(session, dataValue, checkAccessLevel);
	}

	virtual OpcUa_Boolean handlesIo () const override { return OpcUa_True; }

	virtual void beginRead (AsyncReadHandle handle) override
	{
		if (!m_readFn)
		{
			handle.complete(OpcUa::BaseDataVariableType::value(nullptr));
			return;
		}
		std::mutex* mutex = nullptr;
		if (m_readMutexFn)
		{
			mutex = m_readMutexFn();
			if (!mutex)
			{
				handle.complete(badDataValue(OpcUa_BadInternalError));
				return;
			}
		}
		ReadFn readFn = m_readFn;
		UaString address = nodeId().toString();
		auto work = [readFn, address]() -> UaDataValue
		{
			UaVariant variant;
			UaDateTime sourceTime;
			UaStatus status;
			try
			{
				status = readFn(variant, sourceTime);
			}
			catch (const std::exception& e)
			{
				LOG(Log::ERR) << "At variable " << address.toUtf8() << " an exception was thrown from the read delegate: " << e.what();
				status = OpcUa_BadInternalError;
			}
			catch (...)
			{
				LOG(Log::ERR) << "At variable " << address.toUtf8() << " a non-standard exception was thrown from the read delegate";
				status = OpcUa_BadInternalError;
			}
			return UaDataValue(variant, status.statusCode(), sourceTime, UaDateTime::now());
		};
		if (m_readAsynchronous)
		{
			Quasar::ThreadPool* pool = SourceVariables_getThreadPool();
			std::shared_ptr<AsyncReadHandle> sharedHandle(new AsyncReadHandle(std::move(handle)));
			UaStatus dispatchStatus = pool
				? pool->addJob(
					[work, sharedHandle](){ sharedHandle->complete(work()); },
					std::string("read sourcevariable of node ") + address.toUtf8(),
					mutex)
				: UaStatus(OpcUa_BadOutOfService);
			if (!dispatchStatus.isGood())
				sharedHandle->complete(badDataValue(dispatchStatus.statusCode()));
		}
		else
		{
			if (mutex)
			{
				std::unique_lock<std::mutex> lock(*mutex);
				handle.complete(work());
			}
			else
				handle.complete(work());
		}
	}

	virtual void beginWrite (const UaDataValue& dataValue, AsyncWriteHandle handle) override
	{
		if (!m_writeFn)
		{
			handle.complete(OpcUa::BaseDataVariableType::setValue(nullptr, dataValue, OpcUa_True));
			return;
		}
		std::mutex* mutex = nullptr;
		if (m_writeMutexFn)
		{
			mutex = m_writeMutexFn();
			if (!mutex)
			{
				handle.complete(OpcUa_BadInternalError);
				return;
			}
		}
		WriteFn writeFn = m_writeFn;
		UaString address = nodeId().toString();
		UaVariant variant(*dataValue.value());
		auto work = [writeFn, address, variant]() -> UaStatus
		{
			try
			{
				return writeFn(variant);
			}
			catch (const std::exception& e)
			{
				LOG(Log::ERR) << "At variable " << address.toUtf8() << " an exception was thrown from the write delegate: " << e.what();
				return OpcUa_BadInternalError;
			}
			catch (...)
			{
				LOG(Log::ERR) << "At variable " << address.toUtf8() << " a non-standard exception was thrown from the write delegate";
				return OpcUa_BadInternalError;
			}
		};
		if (m_writeAsynchronous)
		{
			Quasar::ThreadPool* pool = SourceVariables_getThreadPool();
			std::shared_ptr<AsyncWriteHandle> sharedHandle(new AsyncWriteHandle(std::move(handle)));
			UaStatus dispatchStatus = pool
				? pool->addJob(
					[work, sharedHandle](){ sharedHandle->complete(work()); },
					std::string("write sourcevariable of node ") + address.toUtf8(),
					mutex)
				: UaStatus(OpcUa_BadOutOfService);
			if (!dispatchStatus.isGood())
				sharedHandle->complete(dispatchStatus);
		}
		else
		{
			if (mutex)
			{
				std::unique_lock<std::mutex> lock(*mutex);
				handle.complete(work());
			}
			else
				handle.complete(work());
		}
	}

private:
	static UaDataValue badDataValue (OpcUa_StatusCode status)
	{
		return UaDataValue(UaVariant(), status, UaDateTime::now(), UaDateTime::now());
	}

	ReadFn m_readFn;
	WriteFn m_writeFn;
	MutexFn m_readMutexFn;
	MutexFn m_writeMutexFn;
	bool m_readAsynchronous;
	bool m_writeAsynchronous;
};

}

#endif // BACKEND_OPEN62541


#endif /* ASSOURCEVARIABLE_H_ */

<?xml version="1.0" encoding="UTF-8"?>
<!-- © Copyright CERN, 2015.                                                       -->
<!-- All rights not expressly granted are reserved.                                -->
<!-- This file is part of Quasar.                                                  -->
<!--                                                                               -->
<!-- Quasar is free software: you can redistribute it and/or modify                -->     
<!-- it under the terms of the GNU Lesser General Public Licence as published by   -->     
<!-- the Free Software Foundation, either version 3 of the Licence.                -->     
<!-- Quasar is distributed in the hope that it will be useful,                     -->     
<!-- but WITHOUT ANY WARRANTY; without even the implied warranty of                -->     
<!--                                                                               -->
<!-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 -->
<!-- GNU Lesser General Public Licence for more details.                           -->
<!--                                                                               -->
<!-- You should have received a copy of the GNU Lesser General Public License      -->
<!-- along with Quasar.  If not, see <http://www.gnu.org/licenses/>                -->
<!--                                                                               -->
<!-- Created:   Sep 2014                                                           -->
<!-- Authors:                                                                      -->
<!--   Piotr Nikiel <piotr@nikiel.info>                                            -->

<xsl:transform version="2.0" 
xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:d="http://cern.ch/quasar/Design" 
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd "

>
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"/>
	<xsl:param name="xsltFileName"/>

	
		<xsl:template  name="ReadJob">
	<xsl:param name="className"/>
	<xsl:param name="variableName"/>
	
	class IoJob_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="$variableName"/> : public UaThreadPoolJob
	{
		public:
		IoJob_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="$variableName"/> (
			IOManagerCallback *callback,
			OpcUa_UInt32 hTransaction,
			OpcUa_UInt32 callbackHandle,
			const UaNode* parentObjectNode
		):
			m_callback(callback),
			m_hTransaction(hTransaction),
			m_callbackHandle (callbackHandle),
			m_parentObjectNode (parentObjectNode)
		{}
		virtual void execute ()
		{
			LOG(Log::DBG) &lt;&lt; "Executing IoJob read: className=<xsl:value-of select="$className"/> varName=<xsl:value-of select="$variableName"/>" &lt;&lt; "hTransaction:"&lt;&lt;m_hTransaction&lt;&lt;" cbkhandle "&lt;&lt; m_callbackHandle&lt;&lt;endl;
			UaStatus s;
			<xsl:value-of select="@dataType"/> value;
			UaDateTime sourceTime;
			// Obtain Device Logic object
			const <xsl:value-of select="fnc:ASClassName($className)"/> *addressSpaceObject;
			addressSpaceObject = dynamic_cast&lt;const <xsl:value-of select="fnc:ASClassName($className)"/>* &gt; ( m_parentObjectNode );
			if (addressSpaceObject == m_parentObjectNode)
			{ /* OK. Proper cast. */
				Device::<xsl:value-of select="fnc:DClassName($className)"/> *device = addressSpaceObject->getDeviceLink();
				if (device != 0)
				{
					/* OK - Device Logic linked. */
					<xsl:choose >
					<xsl:when test="@addressSpaceReadUseMutex='of_this_operation'">
					device->lockVariableRead_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceReadUseMutex='of_this_variable'">
					device->lockVariable_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceReadUseMutex='of_containing_object'">
					device->lock();
					</xsl:when>
					<xsl:when test="@addressSpaceReadUseMutex='of_parent_of_containing_object'">
					device->getParent()->lock();
					</xsl:when>
					</xsl:choose>
					try
					{
						s = device->read<xsl:value-of select="fnc:capFirst($variableName)"/> (value, sourceTime );
					}
					catch (...)
					{
						/* TODO -- how to signalize error from here */
						LOG(Log::ERR) &lt;&lt; "An exception was thrown from read<xsl:value-of select="fnc:capFirst($variableName)"/>" &lt;&lt; std::endl;
						s = OpcUa_BadInternalError;
					}
					<xsl:choose >
					<xsl:when test="@addressSpaceReadUseMutex='of_this_operation'">
					device->unlockVariableRead_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceReadUseMutex='of_this_variable'">
					device->unlockVariable_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceReadUseMutex='of_containing_object'">
					device->unlock();
					</xsl:when>
					<xsl:when test="@addressSpaceReadUseMutex='of_parent_of_containing_object'">
					device->getParent()->unlock();
					</xsl:when>
					</xsl:choose>
				}
				else
					s = OpcUa_BadInternalError;
			}
			else
				s = OpcUa_BadInternalError;
			UaDataValue result (UaVariant(value), s.statusCode(), sourceTime, UaDateTime::now());
			// get appropriate object
			s = m_callback->finishRead (
				m_hTransaction,
				m_callbackHandle,
				result
			);
			LOG(Log::DBG) &lt;&lt; "After finishRead status:" &lt;&lt; s.toString().toUtf8() &lt;&lt; endl;
			
		}
		private:
			IOManagerCallback *m_callback;
			OpcUa_UInt32 m_hTransaction;
			OpcUa_UInt32 m_callbackHandle;
			const UaNode* m_parentObjectNode;
	};
	
	</xsl:template>

	<xsl:template  name="WriteJob">
	<xsl:param name="className"/>
	
	class IoJob_<xsl:value-of select="$className"/>_WRITE_<xsl:value-of select="@name"/> : public UaThreadPoolJob
	{
		public:
		IoJob_<xsl:value-of select="$className"/>_WRITE_<xsl:value-of select="@name"/> (
			IOManagerCallback *callback,
			OpcUa_UInt32 hTransaction,
			OpcUa_UInt32 callbackHandle,
			const UaNode* parentObjectNode,
			OpcUa_WriteValue* writeValue
		):
			m_callback(callback),
			m_hTransaction(hTransaction),
			m_callbackHandle (callbackHandle),
			m_parentObjectNode (parentObjectNode),
			m_variant (writeValue->Value.Value)
	
		{

		}
		virtual void execute ()
		{
		    LOG(Log::DBG) &lt;&lt; "Executing IoJob write: className=<xsl:value-of select="$className"/> varName=<xsl:value-of select="@name"/>" &lt;&lt; "hTransaction:"&lt;&lt;m_hTransaction&lt;&lt;" cbkhandle "&lt;&lt; m_callbackHandle&lt;&lt;endl;
			UaStatus s;
			
			<xsl:value-of select="@dataType"/> value;
			<xsl:choose>
			<xsl:when test="@dataType='UaVariant'">
			value = m_variant;
			if (true)
			{
			</xsl:when>
			<xsl:otherwise>
			if (m_variant.type() == <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>)
			{
				<xsl:choose>
				<xsl:when test="@dataType='UaString'">
				m_variant = value;
				</xsl:when>
				<xsl:otherwise>
				m_variant.<xsl:value-of select="fnc:dataTypeToVariantConverter(@dataType)"/> ( value );
				</xsl:otherwise>
				</xsl:choose>
				
			</xsl:otherwise>
			</xsl:choose>

				// Obtain Device Logic object
				const <xsl:value-of select="fnc:ASClassName($className)"/> *addressSpaceObject;
				addressSpaceObject = dynamic_cast&lt;const <xsl:value-of select="fnc:ASClassName($className)"/>* &gt; ( m_parentObjectNode );
				if (addressSpaceObject == m_parentObjectNode)
				{ /* OK. Proper cast. */
					Device::<xsl:value-of select="fnc:DClassName($className)"/> *device = addressSpaceObject->getDeviceLink();
					if (device != 0)
					{
					<xsl:choose >
					<xsl:when test="@addressSpaceWriteUseMutex='of_this_operation'">
					device->lockVariableWrite_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceWriteUseMutex='of_this_variable'">
					device->lockVariable_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceWriteUseMutex='of_containing_object'">
					device->lock();
					</xsl:when>
					<xsl:when test="@addressSpaceWriteUseMutex='of_parent_of_containing_object'">
					device->getParent()->lock();
					</xsl:when>
					</xsl:choose>
						/* OK - Device Logic linked. */
						try
						{
							s = device->write<xsl:value-of select="fnc:capFirst(@name)"/> (value );
						}
						catch (...)
						{
							/* TODO -- how to signalize error from here */
							LOG(Log::ERR) &lt;&lt; "An exception was thrown from write<xsl:value-of select="fnc:capFirst(@name)"/>" &lt;&lt; std::endl;
							s = OpcUa_BadInternalError;
						}
					<xsl:choose >
					<xsl:when test="@addressSpaceWriteUseMutex='of_this_operation'">
					device->unlockVariableWrite_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceWriteUseMutex='of_this_variable'">
					device->unlockVariable_<xsl:value-of select="@name"/> ();
					</xsl:when>
					<xsl:when test="@addressSpaceWriteUseMutex='of_containing_object'">
					device->unlock();
					</xsl:when>
					<xsl:when test="@addressSpaceWriteUseMutex='of_parent_of_containing_object'">
					device->getParent()->unlock();
					</xsl:when>
					</xsl:choose>
						
					}
					else
						s = OpcUa_BadInternalError;
				}
				else
					s = OpcUa_BadInternalError;
			}
			else
			    s = OpcUa_BadDataEncodingInvalid;
			UaDataValue result (UaVariant(value), s.statusCode(), UaDateTime::now(), UaDateTime::now());
			// get appropriate object
			s = m_callback->finishWrite (
				m_hTransaction,
				m_callbackHandle,
				s
			);
			LOG(Log::TRC) &lt;&lt; "After finishWrite status:" &lt;&lt; s.toString().toUtf8() &lt;&lt; endl;
			
		}
		private:
			IOManagerCallback *m_callback;
			OpcUa_UInt32 m_hTransaction;
			OpcUa_UInt32 m_callbackHandle;
			const UaNode* m_parentObjectNode;
			OpcUa_WriteValue* m_writeValue;
			UaVariant m_variant;
	};
	
	</xsl:template>

	<xsl:template match="/">	
	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToSourceVariablesBody.xslt','Piotr Nikiel')"/>    

	#ifndef BACKEND_OPEN62541


	#include &lt;iomanager.h&gt;
	#include &lt;SourceVariables.h&gt;
	#include &lt;iostream&gt;
	#include &lt;stdexcept&gt;
	#include &lt;LogIt.h&gt;
	
	<xsl:for-each select="/d:design/d:class">
	<xsl:if test="count(d:sourcevariable)>0">
	#include &lt;<xsl:value-of select="fnc:ASClassName(@name)"/>.h&gt;
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)">
	#include &lt;<xsl:value-of select="fnc:DClassName(@name)"/>.h&gt;
	</xsl:if>
	</xsl:if>
	</xsl:for-each>
	
	using namespace std;

	namespace AddressSpace
	{

	/* The thread pool should be initialized by Meta while reading the config file, using function: 
	    SourceVariables_initSourceVariablesThreadPool */
	static UaThreadPool *sourceVariableThreads = 0; 
	void SourceVariables_initSourceVariablesThreadPool (unsigned int minThreads, unsigned int maxThreads)
	{
	    LOG(Log::DBG) &lt;&lt; "Initializing source variables thread pool to min=" &lt;&lt; minThreads  &lt;&lt; " max=" &lt;&lt; maxThreads &lt;&lt; " threads";
		sourceVariableThreads = new UaThreadPool (minThreads, maxThreads);
	}


		<xsl:for-each select="/d:design/d:class">
		<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
		<xsl:for-each select="d:sourcevariable">
			<xsl:if test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous' ">
			<xsl:call-template name="ReadJob">
			<xsl:with-param name="className"><xsl:value-of select="$className"/></xsl:with-param>
			<xsl:with-param name="variableName"><xsl:value-of select="@name"/></xsl:with-param>
			</xsl:call-template>
			</xsl:if>
			
			<xsl:if test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous'">
			<xsl:call-template name="WriteJob">
			<xsl:with-param name="className"><xsl:value-of select="$className"/></xsl:with-param>

			</xsl:call-template>

			</xsl:if>
		</xsl:for-each>
		</xsl:for-each>

UaStatus SourceVariables_spawnIoJobRead (		
		ASSourceVariableJobId jobId,
		IOManagerCallback *callback,
		OpcUa_UInt32 hTransaction,
		OpcUa_UInt32        callbackHandle,
		const UaNode *parentNode
	)
	{
		if (! sourceVariableThreads)
			throw std::runtime_error("Attempted Source Variable operation, but Source Variable threads are not up.");
		switch (jobId)
		{
		<xsl:for-each select="/d:design/d:class">
		<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
		<xsl:for-each select="d:sourcevariable">
			<xsl:if test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous'">
			
			case ASSOURCEVARIABLE_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="@name"/>:
			{
				<xsl:choose>
				<xsl:when test="@addressSpaceRead='asynchronous'">
				IoJob_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="@name"/> *job =
				new IoJob_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="@name"/> (
				callback,
				hTransaction,
				callbackHandle,
				parentNode
				); 
				sourceVariableThreads-&gt;addJob (job);
				</xsl:when>
				<xsl:when test="@addressSpaceRead='synchronous'">
				IoJob_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="@name"/> job (
				callback,
				hTransaction,
				callbackHandle,
				parentNode
				); 
				job.execute();
				</xsl:when>
				<xsl:otherwise>
				<xsl:message terminate="yes">Something got wrong.</xsl:message>
				</xsl:otherwise>
				</xsl:choose>
			}
				return OpcUa_Good;

			</xsl:if>
		</xsl:for-each>
		</xsl:for-each>
					default:
				return OpcUa_Bad;
		}
	}
	
	UaStatus SourceVariables_spawnIoJobWrite (		
		ASSourceVariableJobId jobId,
		IOManagerCallback *callback,
		OpcUa_UInt32 hTransaction,
		OpcUa_UInt32        callbackHandle,
		const UaNode *parentNode,
		OpcUa_WriteValue*   pWriteValue
	)
	{
		if (! sourceVariableThreads)
			throw std::runtime_error("Attempted Source Variable operation, but Source Variable threads are not up.");
		switch (jobId)
		{
		<xsl:for-each select="/d:design/d:class">
		<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
		<xsl:for-each select="d:sourcevariable">
			<xsl:if test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous'">
			
			case <xsl:value-of select="fnc:sourceVariableWriteJobId($className,@name)"/>:
			{
				<xsl:choose>
				<xsl:when test="@addressSpaceWrite='asynchronous'">
			
				IoJob_<xsl:value-of select="$className"/>_WRITE_<xsl:value-of select="@name"/> *job =
				new IoJob_<xsl:value-of select="$className"/>_WRITE_<xsl:value-of select="@name"/> (
				callback,
				hTransaction,
				callbackHandle,
				parentNode,
				pWriteValue
				); 
				sourceVariableThreads-&gt;addJob (job);
				</xsl:when>
				<xsl:when test="@addressSpaceWrite='synchronous'">
				
				IoJob_<xsl:value-of select="$className"/>_WRITE_<xsl:value-of select="@name"/> job (
				callback,
				hTransaction,
				callbackHandle,
				parentNode,
				pWriteValue
				); 
				job.execute();
				
				</xsl:when>
				<xsl:otherwise>
				<xsl:message terminate="yes">Something got wrong.</xsl:message>
				</xsl:otherwise>
				</xsl:choose>
				
				
			}
				return OpcUa_Good;

			</xsl:if>
		</xsl:for-each>
		</xsl:for-each>
					default:
				return OpcUa_Bad;
		}
	}

	<!-- Iterate for all sourcevariables -->
	<xsl:for-each select="/d:design/d:class">
	<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
	<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceRead='asynchronous'">

		</xsl:if>
	</xsl:for-each>
	</xsl:for-each>
	
	}

	#endif // BACKEND_OPEN62541
	
	</xsl:template>
	


</xsl:transform>

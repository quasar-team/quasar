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




	<xsl:template match="/">	
	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToSourceVariablesHeader.xslt','Piotr Nikiel')"/>    
	#ifndef __IOMANAGERS_H__
	#define __IOMANAGERS_H__

	#ifndef BACKEND_OPEN62541
	
	#include &lt;iomanager.h&gt;
	#include &lt;uathreadpool.h&gt;
	#include &lt;uabasenodes.h&gt;

	namespace AddressSpace
	{
	
	<!-- Generate IO JOB directory -->
	enum ASSourceVariableJobId
	{
		ASSOURCEVARIABLE_NOTHING
	<!-- Iterate for all sourcevariables -->
	<xsl:for-each select="/d:design/d:class">
	<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
	<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous'">
			,<xsl:value-of select="fnc:sourceVariableReadJobId($className,@name)"/>
		</xsl:if>
		<xsl:if test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous'">
			,<xsl:value-of select="fnc:sourceVariableWriteJobId($className,@name)"/>
		</xsl:if>
	</xsl:for-each>
	</xsl:for-each>
	};
	
	void SourceVariables_initSourceVariablesThreadPool (unsigned int minThreads=0, unsigned int maxThreads=10);
	
	UaStatus SourceVariables_spawnIoJobRead (
		ASSourceVariableJobId jobId,
		IOManagerCallback *callback,
		OpcUa_UInt32 hTransaction,
		OpcUa_UInt32        callbackHandle,
		const UaNode *parentNode
		
		);
		
	UaStatus SourceVariables_spawnIoJobWrite (
		ASSourceVariableJobId jobId,
		IOManagerCallback *callback,
		OpcUa_UInt32 hTransaction,
		OpcUa_UInt32        callbackHandle,
		const UaNode *parentNode,
		OpcUa_WriteValue*   pWriteValue
		
		
		);
		

	}

#endif // BACKEND_OPEN62541


#endif // include guard

	</xsl:template>
	


</xsl:transform>

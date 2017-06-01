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
<!-- Created:   Jun 2014                                                           -->
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
	<xsl:param name="className"/>
	<xsl:param name="xsltFileName"/>


	
	<xsl:template name="cachevariables_setgetters">
	
	<!--  dataType needed for this -->
	UaStatus get<xsl:value-of select="fnc:capFirst(@name)"/> (<xsl:value-of select="@dataType"/> &amp;) const ;
	UaStatus <xsl:value-of select="fnc:varSetter(@name, @dataType, 'true')"/> ; 
	<!-- If value is guaranteed to never be null (config initialization or initialValue specified) then we can have a plain getter. -->
	<!-- OPCUA-209  -->
	<xsl:choose>
		<xsl:when test="@nullPolicy='nullForbidden'">
			/* short getter (possible because nullPolicy=nullForbidden) */
			<xsl:value-of select="@dataType"/> get<xsl:value-of select="fnc:capFirst(@name)"/> () const;
		</xsl:when>
		<xsl:when test="@nullPolicy='nullAllowed'">
			/* null-setter (possible because nullPolicy=nullAllowed) */
			UaStatus <xsl:value-of select="fnc:varSetter(@name, 'null', 'true')"/> ;  
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="yes">nullPolicy not present (class='<xsl:value-of select="$className"/>' variable='<xsl:value-of select="@name"/>')</xsl:message>
		</xsl:otherwise>
	</xsl:choose>
	</xsl:template>
	
	<xsl:template name="cachevariables_delegates">
	<xsl:if test="@addressSpaceWrite='delegated' or @addressSpaceWrite='regular'">
	//! This function will be called when OPC UA write request comes. Never directly call it.
	<xsl:value-of select="fnc:delegateWrite(@name,$className,'header')"/>;
	</xsl:if>
	</xsl:template>
	
	<xsl:template  name="classHeader">

	
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
		/* forward declaration */
		namespace Device{ class <xsl:value-of select="fnc:DClassName($className)"/>; }
		</xsl:if>

namespace AddressSpace 
{
		//! Fully auto-generated class to represent <xsl:value-of select="@name"/> in the OPC UA AddressSpace
		class <xsl:value-of select="fnc:ASClassName(@name)"/>: public OpcUa::BaseObjectType
		{
		UA_DISABLE_COPY(<xsl:value-of select="fnc:ASClassName(@name)"/>);
		public:
<!-- *************************************************** -->
<!-- CONSTRUCTOR *************************************** -->
<!-- *************************************************** -->
		//! Constructor. Used in Configurator.cpp. You NEVER use it directly.
		AS<xsl:value-of select="@name"/> (
			UaNodeId parentNodeId,
			const UaNodeId&amp; typeNodeId,
			ASNodeManager *nm,
			const Configuration::<xsl:value-of select="@name"/> &amp;config);
<!-- *************************************************** -->
<!-- DESTRUCTOR **************************************** -->
<!-- *************************************************** -->
		/*dtor*/
		~AS<xsl:value-of select="@name"/> ();
<!-- *************************************************** -->
<!-- SETTERS/GETTERS *********************************** -->
<!-- *************************************************** -->
		/* setters and getters for variables */
		<xsl:for-each select="d:cachevariable">
		<xsl:call-template name="cachevariables_setgetters"/>
		</xsl:for-each>


		
		
<!-- *************************************************** -->
<!-- DELEGATES ***************************************** -->
<!-- *************************************************** -->
		/* delegators for cachevariables  */
		<xsl:for-each select="d:cachevariable">
		<xsl:call-template name="cachevariables_delegates"/>
		</xsl:for-each>
		
<!-- *************************************************** -->
<!-- DELEGATES FOR METHODS ***************************** -->
<!-- *************************************************** -->
	<xsl:for-each select="d:method">
	UaStatus call<xsl:value-of select="fnc:capFirst(@name)"/> (
			MethodManagerCallback* pCallback,
			OpcUa_UInt32           callbackHandle,
			const UaVariantArray&amp;  inputArguments) ;
	</xsl:for-each>
		
<!-- *************************************************** -->
<!-- LINK/UNLINK DEVICE LOGIC ************************** -->
<!-- *************************************************** -->
		/* Device Logic setter (if requested) */
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
		void linkDevice( Device::<xsl:value-of select="fnc:DClassName($className)"/> *deviceLink);
		void unlinkDevice ();
		Device::<xsl:value-of select="fnc:DClassName($className)"/> * getDeviceLink () const { return m_deviceLink; }
		</xsl:if>
		/* OPC UA Type Information provider for this class. */
		virtual UaNodeId typeDefinitionId () const { return m_typeNodeId; }
		
		<xsl:if test="d:method">
		/* Call handler defined because at least one method is declared */
		virtual UaStatus beginCall ( 
			 	MethodManagerCallback *  	pCallback, 
			 	const ServiceContext &amp; 	serviceContext,
			 	OpcUa_UInt32  	callbackHandle, 
				MethodHandle *  	pMethodHandle, 
				const UaVariantArray &amp;  	inputArguments  
		);
		</xsl:if>

		
private:
		UaNodeId m_typeNodeId;
		/* Variables */
		<xsl:for-each select="child::d:cachevariable">
		<xsl:choose>
		<xsl:when test="@addressSpaceWrite='delegated'">ASDelegatingVariable&lt;AS<xsl:value-of select="$className"/>&gt; </xsl:when>
		<xsl:otherwise>OpcUa::BaseDataVariableType </xsl:otherwise>
		</xsl:choose>
		* m_<xsl:value-of select="@name" />;
		</xsl:for-each>
		
		<xsl:for-each select="d:sourcevariable">
		ASSourceVariable * m_<xsl:value-of select="@name"/>;
		</xsl:for-each>
		
		/* Methods */
		<xsl:for-each select="d:method">
		ASDelegatingMethod&lt;AS<xsl:value-of select="$className"/>&gt; * m_<xsl:value-of select="@name" />;
		</xsl:for-each>
		
		/* Device Logic link (if requested) */
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
		Device::<xsl:value-of select="fnc:DClassName($className)"/> *m_deviceLink;
		</xsl:if>
		
	
		
		};

}		
	</xsl:template>

	<xsl:template match="/">	
	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToClassHeader.xslt','Piotr Nikiel')"/>
	<xsl:if test="not(/d:design/d:class[@name=$className])">
		<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' not found</xsl:message>
	</xsl:if>
	
#ifndef __<xsl:value-of select="fnc:ASClassName($className)"/>__H__
#define __<xsl:value-of select="fnc:ASClassName($className)"/>__H__	


#include &lt;opcua_baseobjecttype.h&gt;
#include &lt;opcua_basedatavariabletype.h&gt;

#include &lt;methodhandleuanode.h&gt; 
#include &lt;ASDelegatingMethod.h&gt;

#include &lt;Configuration.hxx&gt;

#include &lt;ASNodeManager.h&gt;
#include &lt;ASDelegatingVariable.h&gt;


#include &lt;ASSourceVariable.h&gt;

	<xsl:for-each select="/d:design/d:class[@name=$className]">
	<xsl:call-template name="classHeader"/>
	</xsl:for-each>
	

#endif // include guard

	</xsl:template>
	


</xsl:transform>

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
<!-- Created:   Jul 2014                                                           -->
<!-- Authors:                                                                      -->
<!--   Piotr Nikiel <piotr@nikiel.info>                                            -->

<xsl:transform version="2.0" xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"/>
	<xsl:param name="className"/>
	<xsl:strip-space elements="*"/>


	<xsl:template name="cachevariables_delegates">
		<xsl:if test="@addressSpaceWrite='delegated'">
			/* Note: never directly call this function. */
			<!--  get rid of it 
			<xsl:value-of select="fnc:delegateWrite(@name,fnc:DClassName($className),'body')" />
			-->
			UaStatus <xsl:value-of select="fnc:DClassName($className)"/>::write<xsl:value-of select="fnc:capFirst(@name)"/> ( const <xsl:value-of select="@dataType"/> &amp; v)
			{
			return OpcUa_BadNotImplemented;
			}
		</xsl:if>
	</xsl:template>

	<xsl:template name="sourcevariables_operations">
		<xsl:if test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous'">
			/* ASYNCHRONOUS !! -- this executes as separate thread (job from ThreadPool) */
			UaStatus <xsl:value-of select="fnc:DClassName($className)"/>::read<xsl:value-of select="fnc:capFirst(@name)"/> (
			<xsl:value-of select="@dataType"/> &amp;value,
			UaDateTime &amp;sourceTime
			)
			{
				sourceTime = UaDateTime::now();
				return OpcUa_BadNotImplemented;
			}
		</xsl:if>
		
		<xsl:if test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous'">
			/* ASYNCHRONOUS !! -- this executes as separate thread (job from ThreadPool) */
			UaStatus <xsl:value-of select="fnc:DClassName($className)"/>::write<xsl:value-of select="fnc:capFirst(@name)"/> (
			<xsl:value-of select="@dataType"/> &amp;value
			)
			{
				return OpcUa_BadNotImplemented;
			}
		</xsl:if>
	</xsl:template>
	
	<xsl:template name="method_handler">
		UaStatus <xsl:value-of select="fnc:DClassName($className)"/>::call<xsl:value-of select="fnc:capFirst(@name)"/> (
		<xsl:for-each select="d:argument">
			<xsl:value-of select="fnc:fixDataTypePassingMethod(@dataType)"/><xsl:text> </xsl:text><xsl:value-of select="@name"/><xsl:if test="position() &lt; (count(../d:argument)+count(../d:returnvalue))">,</xsl:if><xsl:text>
			</xsl:text>
		</xsl:for-each>
		<xsl:for-each select="d:returnvalue">
			<xsl:value-of select="@dataType"/> &amp; <xsl:value-of select="@name"/><xsl:if test="position() &lt; count(../d:returnvalue)">,
			</xsl:if>
		</xsl:for-each>
		 )
		{
		     return OpcUa_BadNotImplemented;
		}
	</xsl:template>

	<xsl:template name="deviceBody">


    // 1111111111111111111111111111111111111111111111111111111111111111111111111
    // 1     GENERATED CODE STARTS HERE AND FINISHES AT SECTION 2              1
    // 1     Users don't modify this code!!!!                                  1
    // 1     If you modify this code you may start a fire or a flood somewhere,1
    // 1     and some human being may possible cease to exist. You don't want  1
    // 1     to be charged with that!                                          1 
    // 1111111111111111111111111111111111111111111111111111111111111111111111111

	 
		
		

		
    // 2222222222222222222222222222222222222222222222222222222222222222222222222
    // 2     SEMI CUSTOM CODE STARTS HERE AND FINISHES AT SECTION 3            2
    // 2     (code for which only stubs were generated automatically)          2
    // 2     You should add the implementation but dont alter the headers      2
    // 2     (apart from constructor, in which you should complete initializati2
    // 2     on list)                                                          2 
    // 2222222222222222222222222222222222222222222222222222222222222222222222222
    
    /* sample ctr */
    <xsl:value-of select="fnc:DClassName(@name)"/>::<xsl:value-of select="fnc:DClassName(@name)"/> (
    const Configuration::<xsl:value-of select="@name"/> &amp; config,
    <xsl:value-of select="fnc:Parent_DClassName($className)"/> * parent
		):
			<xsl:value-of select="fnc:Base_DClassName(@name)"/>( config, parent)
			
		/* fill up constructor initialization list here */
		{
			/* fill up constructor body here */
		}
		
	/* sample dtr */
		<xsl:value-of select="fnc:DClassName($className)" />::~<xsl:value-of select="fnc:DClassName($className)" /> ()
		{
		}
    
    /* delegators for cachevariables and externalvariables */
		<xsl:for-each select="d:cachevariable">
			<xsl:call-template name="cachevariables_delegates" />
		</xsl:for-each>

		<xsl:for-each select="d:sourcevariable">
			<xsl:call-template name="sourcevariables_operations" />
		</xsl:for-each>
    
		<xsl:for-each select="d:method">
			<xsl:call-template name="method_handler"/>
		</xsl:for-each>
		
    // 3333333333333333333333333333333333333333333333333333333333333333333333333
    // 3     FULLY CUSTOM CODE STARTS HERE                                     3
    // 3     Below you put bodies for custom methods defined for this class.   3
    // 3     You can do whatever you want, but please be decent.               3
    // 3333333333333333333333333333333333333333333333333333333333333333333333333
	 
	 </xsl:template>
	 
	
	 	<xsl:template match="/">	
	 	<xsl:if test="not(/d:design/d:class[@name=$className])">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' not found. If you are running this through generateDevice*.sh scripts, maybe you forgot to put name of the class as a parameter?</xsl:message>
		</xsl:if>
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)!='true'">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' hasnt got device logic</xsl:message>
		</xsl:if>
		<xsl:value-of select="fnc:headerStubGenerated(/,'using transform designToDeviceBody.xslt','Piotr Nikiel')"/>

		
#include &lt;boost/foreach.hpp&gt;		
		
#include &lt;Configuration.hxx&gt;

#include &lt;<xsl:value-of select="fnc:DClassName($className)"/>.h&gt;
#include &lt;<xsl:value-of select="fnc:ASClassName($className)"/>.h&gt;

	<xsl:for-each select="/d:design/d:class[@name=$className]">
	<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
	<xsl:for-each select="d:hasobjects">
	<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
	<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
	#include &lt;<xsl:value-of select="fnc:DClassName(@class)"/>.h&gt;
	</xsl:if>
	</xsl:for-each>
	</xsl:if>
	</xsl:for-each>



namespace Device
{

	<xsl:for-each select="/d:design/d:class[@name=$className]">
	<xsl:call-template name="deviceBody"/>
	</xsl:for-each>
	
	
}


	</xsl:template>
	 
	 
	 
	 
	 
	 
</xsl:transform>

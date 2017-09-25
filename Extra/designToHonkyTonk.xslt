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
<!-- Created:   Oct 2014                                                           -->
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
	<xsl:output method="text"></xsl:output>
	 	<xsl:param name="xsltFileName"/>

<xsl:template name="hasObjects">
	<xsl:param name="parentNodeId"/>
	<xsl:param name="parentDevice"/>
	<xsl:param name="configuration" />

	BOOST_FOREACH( Device::<xsl:value-of select="fnc:DClassName(@class)"/> *d<xsl:value-of select="fnc:DClassName(@class)"/> , <xsl:value-of select="$parentDevice"/>-&gt;<xsl:value-of select="lower-case(@class)"/>s())
		{

			<xsl:variable name="class" select="@class"/>

			<xsl:for-each select="/d:design/d:class[@name=$class]/d:cachevariable">
			d<xsl:value-of select="fnc:DClassName($class)"/>-&gt;getAddressSpaceLink()->set<xsl:value-of select="fnc:capFirst(@name)"/> (
			<xsl:choose>
			<!-- if numeric dataType -->
			<xsl:when test="@dataType='OpcUa_Double' or @dataType='OpcUa_Float' or
							@dataType='OpcUa_UInt64' or @dataType='OpcUa_Int64' or 
							@dataType='OpcUa_UInt32' or @dataType='OpcUa_Int32' or 
							@dataType='OpcUa_UInt16' or @dataType='OpcUa_Int16' ">
			 rand()
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Boolean'">
			 (rand()&amp;0x01)?OpcUa_True:OpcUa_False
			</xsl:when>
			<xsl:when test="@dataType='UaString'">
				UaString("test%1").arg(UaString::number(rand()))
			</xsl:when>
			<xsl:when test="@dataType='UaVariant'">
				UaString("UaVariant__%1").arg(UaString::number(rand()))
			</xsl:when>
			<xsl:otherwise>
			<xsl:message terminate="yes">
			Randomizer not defined for dataType=<xsl:value-of select="@dataType"/>
			</xsl:message>
			</xsl:otherwise>
			</xsl:choose>
			, OpcUa_Good, UaDateTime::now() );
			</xsl:for-each>

			<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
			
			
			
			
			<xsl:for-each select="/d:design/d:class[@name=$class]/d:hasobjects">
			<xsl:if test="fnc:classHasDeviceLogic(/,@class)='true'">
			<xsl:call-template name="hasObjects">
			<xsl:with-param name="parentDevice">d<xsl:value-of select="fnc:DClassName($className)"/></xsl:with-param>
			</xsl:call-template>
			</xsl:if>
			</xsl:for-each>

		}

	
	</xsl:template>


	<xsl:template match="/">	
	#include &lt;stdlib.h&gt;
	#include &lt;DRoot.h&gt;
	
	#include &lt;boost/foreach.hpp&gt;
	
	<xsl:for-each select="/d:design/d:class">
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
	#include &lt;<xsl:value-of select="fnc:DClassName(@name)"/>.h&gt;
	#include &lt;<xsl:value-of select="fnc:ASClassName(@name)"/>.h&gt;
	</xsl:if>
	</xsl:for-each>
	
	void honkyTonk ()
	{
	
	Device::DRoot *root = Device::DRoot::getInstance();

	<xsl:for-each select="/d:design/d:root/d:hasobjects">
	
	<xsl:if test="fnc:classHasDeviceLogic(/,@class)='true'">

	<xsl:call-template name="hasObjects">
	<xsl:with-param name="parentDevice">root</xsl:with-param>
	</xsl:call-template>
	
	</xsl:if>

	</xsl:for-each>

	}
	</xsl:template>



</xsl:transform>

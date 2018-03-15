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
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:output indent="yes" method="xml"/>
	<xsl:param name="metaXsdPath"/>
	
	<xsl:template name="dataTypeToXsdType">
	<xsl:param name="dataType"/>
	<xsl:choose>
	<xsl:when test="$dataType='UaString'">xs:string</xsl:when>
	<xsl:when test="$dataType='OpcUa_SByte'">xs:byte</xsl:when>
	<xsl:when test="$dataType='OpcUa_Byte'">xs:unsignedByte</xsl:when>
	<xsl:when test="$dataType='OpcUa_Int16'">xs:short</xsl:when>
	<xsl:when test="$dataType='OpcUa_UInt16'">xs:unsignedShort</xsl:when>
	<xsl:when test="$dataType='OpcUa_Int32'">xs:int</xsl:when>
	<xsl:when test="$dataType='OpcUa_UInt32'">xs:unsignedInt</xsl:when>
	<xsl:when test="$dataType='OpcUa_Int64'">xs:long</xsl:when>
	<xsl:when test="$dataType='OpcUa_UInt64'">xs:unsignedLong</xsl:when>
	<xsl:when test="$dataType='OpcUa_Boolean'">xs:boolean</xsl:when>
	<xsl:when test="$dataType='OpcUa_Double'">xs:double</xsl:when>
	<xsl:when test="$dataType='OpcUa_Float'">xs:float</xsl:when>
	<xsl:when test="$dataType='UaByteString'"><xsl:message terminate="yes">ERROR: it is not allowed to initialize a variable of ByteString type from configuration due to ambiguity of input format. If you need this feature, please do ask quasar team for implementation.</xsl:message></xsl:when>
	<xsl:when test="$dataType='UaVariant'"><xsl:message terminate="yes">ERROR: it is not allowed to initialize a variable of variable type (UaVariant) from configuration.</xsl:message></xsl:when>
	<xsl:otherwise><xsl:message terminate="yes">ERROR: unknown type <xsl:value-of select="$dataType"/></xsl:message></xsl:otherwise>
	</xsl:choose>
	
	</xsl:template>

	<xsl:template match="d:class">	
	<!--  for every class we create a complexType -->
	<xsl:element name="xs:complexType">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>

	<xs:sequence>
	<xs:choice minOccurs="0" maxOccurs="unbounded">
		<xs:element name="item" type="tns:ITEM" >
		</xs:element>
		<xs:element name="regexpr" type="tns:REGEXPR" >
		</xs:element>
	

		<xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
		<xs:element name="{@class}" type="tns:{@class}"  />
		</xsl:for-each>


	</xs:choice>
	</xs:sequence>
	<!-- name of the object of this class is mandatory and not configurable. -->
	<xs:attribute name="name" type="xs:string" use="required"/>
	<!-- now go over all cache-variables that shall be initialized from config file -->
	
	<xsl:for-each select="child::d:cachevariable">
		<xsl:if test="@initializeWith='configuration'">
			<!--  <xs:attribute name="{@name}" type="{@xsdType}" use="required"/>-->
			<xsl:element name="xs:attribute">
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="use">required</xsl:attribute>
				<xsl:attribute name="type">
				<xsl:call-template name="dataTypeToXsdType">
				<xsl:with-param name="dataType" select="@dataType"/>
				</xsl:call-template>
				</xsl:attribute>
			
				<!-- OPCUA-458 Try carrying documentation over from Design file to Configuration schema -->
				<xsl:if test="d:documentation">
				<xsl:element name="xs:annotation">
				<xsl:element name="xs:documentation">
				<xsl:value-of select="d:documentation"/>
				</xsl:element>
				</xsl:element>
				</xsl:if>
				
			</xsl:element>
		</xsl:if>
	</xsl:for-each>

	<!-- also config entries shall create attributes in xsd -->
	<xsl:for-each select="d:configentry">
		<xsl:element name="xs:attribute">
			<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
			<xsl:attribute name="use">required</xsl:attribute>
			<xsl:attribute name="type">
			<xsl:call-template name="dataTypeToXsdType">
			<xsl:with-param name="dataType" select="@dataType"/>
			</xsl:call-template>
			</xsl:attribute>
			
			<!-- OPCUA-458 Try carrying documentation over from Design file to Configuration schema -->
			<xsl:if test="d:documentation">
			<xsl:element name="xs:annotation">
			<xsl:element name="xs:documentation">
			<xsl:value-of select="d:documentation"/>
			</xsl:element>
			</xsl:element>
			</xsl:if>
			
		</xsl:element>
	</xsl:for-each>	

	
	</xsl:element>
	

	
	
	</xsl:template>

	
	<xsl:template match="/">
	<xs:schema targetNamespace="http://cern.ch/quasar/Configuration" 
	xmlns:tns="http://cern.ch/quasar/Configuration" 
	xmlns:xi="http://www.w3.org/2003/XInclude"
	elementFormDefault="qualified">
	
	<xi:include href="{$metaXsdPath}" xpointer="xmlns(xs=http://www.w3.org/2001/XMLSchema) xpointer(/xs:schema/node())">
	</xi:include>
	
	<xs:complexType name="REGEXPR">
		<xs:attribute name="name" type="xs:string" use="required" />
		<xs:attribute name="value" type="xs:string" use="required" />
	</xs:complexType>

	<xs:complexType name="ITEM">
		<xs:attribute name="name" type="xs:string" use="required" />
		<xs:attribute name="value" type="xs:string" use="required" />
		<xs:attribute name="when" type="xs:string" use="optional" />
		<xs:attribute name="status" type="xs:string" use="optional" />
	</xs:complexType>	
	<xsl:apply-templates/>
	
	<xs:complexType name="Configuration">
		<xs:sequence>
		    <xs:choice minOccurs="0" maxOccurs="1"><xs:element name="StandardMetaData" type="tns:StandardMetaData"></xs:element></xs:choice>
			<xs:choice minOccurs="0" maxOccurs="unbounded"  >
				<xsl:for-each select="/d:design/d:root/d:hasobjects[@instantiateUsing='configuration']">
				<xs:element name="{@class}" type="tns:{@class}" />
				</xsl:for-each>
			</xs:choice>
		</xs:sequence>
	</xs:complexType>	
	
	<xs:element name="configuration" type="tns:Configuration" />
	
	</xs:schema>
	</xsl:template>

</xsl:transform>

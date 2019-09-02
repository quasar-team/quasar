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
xmlns:fnc="http://cern.ch/quasar/Functions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:output indent="yes" method="xml"/>
	<xsl:param name="metaXsdPath"/>
	
	
<xsl:function name="fnc:dataTypeToXsdType">
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
	
</xsl:function>

<xsl:template name="configRestrictionGeneric">
    <xsl:element name="xs:simpleType">
        <xsl:element name="xs:restriction">
            <xsl:attribute name="base"><xsl:value-of select="fnc:dataTypeToXsdType(@dataType)" /></xsl:attribute>
            <xsl:for-each select="d:configRestriction/d:restrictionByPattern">
                <xsl:element name="xs:pattern">
                    <xsl:attribute name="value">
                    <xsl:value-of select="@pattern" />
                </xsl:attribute>
                </xsl:element>
            </xsl:for-each>
            <xsl:for-each select="d:configRestriction/d:restrictionByEnumeration/d:enumerationValue">
                <xsl:element name="xs:enumeration">
                    <xsl:attribute name="value">
                    <xsl:value-of select="@value" />
                </xsl:attribute>
                </xsl:element>
            </xsl:for-each>
            <xsl:for-each select="d:configRestriction/d:restrictionByBounds">
                <xsl:if test="@minInclusive">
                    <xs:minInclusive value="{@minInclusive}"/>
                </xsl:if>
                <xsl:if test="@maxInclusive">
                    <xs:maxInclusive value="{@maxInclusive}"/>
                </xsl:if>
                <xsl:if test="@minExclusive">
                    <xs:minExclusive value="{@minExclusive}"/>
                </xsl:if>
                <xsl:if test="@maxExclusive">
                    <xs:maxExclusive value="{@maxExclusive}"/>
                </xsl:if>
            </xsl:for-each>
        </xsl:element>
    </xsl:element>
</xsl:template>



<xsl:template match="d:class">	
	<!--  for every class we create a complexType -->
	<xsl:element name="xs:complexType">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>

	<xs:sequence>
	<xs:choice minOccurs="0" maxOccurs="unbounded">
		<xs:element name="CalculatedVariable" type="tns:CalculatedVariable" />

		<xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
		<xs:element name="{@class}" type="tns:{@class}"  />
		</xsl:for-each>
	</xs:choice>
	<!-- here we go through all the config entries and filter out the ones which are arrays. 
	they become elements. -->
	<xsl:for-each select="d:configentry | d:cachevariable[@initializeWith='configuration']">
		<xsl:if test="d:array">
			<xsl:variable name="minimumSize">
				<xsl:choose>
					<xsl:when test="d:array/@minimumSize"><xsl:value-of select="d:array/@minimumSize"/></xsl:when>
					<xsl:otherwise>0</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="maximumSize">
				<xsl:choose>
					<xsl:when test="d:array/@maximumSize"><xsl:value-of select="d:array/@maximumSize"/></xsl:when>
					<xsl:otherwise>unbounded</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>			
			<xs:element name="{@name}">
				<xs:complexType>
		    			<xs:sequence minOccurs="{$minimumSize}" maxOccurs="{$maximumSize}">
                            <xsl:choose>
                            <xsl:when test="d:configRestriction">
                                <xs:element name="value">
                                    <xsl:call-template name="configRestrictionGeneric"/>
                                </xs:element>
                            </xsl:when>
                            <xsl:otherwise>
                                <xs:element name="value" type="{fnc:dataTypeToXsdType(@dataType)}"/>
                            </xsl:otherwise>
                            </xsl:choose>
		    			</xs:sequence>
	    		</xs:complexType>
			</xs:element>
		</xsl:if>
	</xsl:for-each>
    	
	</xs:sequence>
	<!-- name of the object of this class is mandatory and not configurable. -->
	<xs:attribute name="name" type="tns:ObjectName" use="required"/>
	<!-- now go over all cache-variables that shall be initialized from config file -->
	
	<xsl:for-each select="child::d:cachevariable">
		<xsl:if test="@initializeWith='configuration'">
		<xsl:choose>
		<xsl:when test="d:array">
		<!-- array cache var: these are elements and not attributes, so that we can use
		complex types. Do not generate them as attributes here therefore -->			
		</xsl:when>
		<xsl:otherwise>
	    <!-- scalar cache var as attribute -->
				
			<!--  <xs:attribute name="{@name}" type="{@xsdType}" use="required"/>-->
			<xsl:element name="xs:attribute">
				<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
				<xsl:attribute name="use">required</xsl:attribute>
				
                <xsl:if test="not(d:configRestriction)">
                    <xsl:attribute name="type"><xsl:value-of select="fnc:dataTypeToXsdType(@dataType)"/></xsl:attribute>
                </xsl:if>
			
				<!-- OPCUA-458 Try carrying documentation over from Design file to Configuration schema -->
				<xsl:if test="d:documentation">
				<xsl:element name="xs:annotation">
				<xsl:element name="xs:documentation">
				<xsl:value-of select="d:documentation"/>
				</xsl:element>
				</xsl:element>
				</xsl:if>
                
                <xsl:if test="d:configRestriction">
                    <xsl:call-template name="configRestrictionGeneric"/>
                </xsl:if>
                	
			</xsl:element>
		</xsl:otherwise>
		</xsl:choose>
			
		</xsl:if>
	</xsl:for-each>

	<!-- also config entries shall create attributes in xsd -->
	<xsl:for-each select="d:configentry">
	<xsl:choose>
		<xsl:when test="d:array">
		<!-- we have a config entry array, and this is needed in the xsd as an 
		element and not an attribute here. Each array has different bounds and 
		different base type. Therefore we need to generate one specific xsd-type for 
		each configuration entry !  
		-->
		</xsl:when>
		<xsl:otherwise>
		<!-- we have a config entry scalar, which is an attribute -->
			<xsl:element name="xs:attribute">
			<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
            <xsl:choose>
                <xsl:when test="@defaultValue">
                    <xsl:attribute name="use">optional</xsl:attribute>
                    <xsl:attribute name="default"><xsl:value-of select="@defaultValue"/></xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="use">required</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:if test="not(d:configRestriction)">
                <xsl:attribute name="type"><xsl:value-of select="fnc:dataTypeToXsdType(@dataType)"/></xsl:attribute>
            </xsl:if>
            
			
			<!-- OPCUA-458 Try carrying documentation over from Design file to Configuration schema -->
			<xsl:if test="d:documentation">
			<xsl:element name="xs:annotation">
			<xsl:element name="xs:documentation">
			<xsl:value-of select="d:documentation"/>
			</xsl:element>
			</xsl:element>
			</xsl:if>
            
            <xsl:if test="d:configRestriction">
                <xsl:call-template name="configRestrictionGeneric"/>
            </xsl:if>
			
		</xsl:element>
		</xsl:otherwise>
	</xsl:choose>
	</xsl:for-each>	
	</xsl:element>
</xsl:template>
<!-- template class end -->

	
<xsl:template match="/">
	<xs:schema targetNamespace="http://cern.ch/quasar/Configuration" 
	xmlns:tns="http://cern.ch/quasar/Configuration" 
	xmlns:xi="http://www.w3.org/2003/XInclude"
	elementFormDefault="qualified">
	
	<xi:include href="{$metaXsdPath}" xpointer="xmlns(xs=http://www.w3.org/2001/XMLSchema) xpointer(/xs:schema/node())">
	</xi:include>
	
	<xs:simpleType name="ObjectName">
		<xs:restriction base="xs:string">
			<xs:minLength value="1"/>
		</xs:restriction>
	</xs:simpleType>

	<xs:complexType name="CalculatedVariable">
		<xs:attribute name="name" type="xs:string" use="required" />
		<xs:attribute name="value" type="xs:string" use="required" />
        <xs:attribute name="initialValue" type="xs:double" use="optional" />
        <xs:attribute name="isBoolean" type="xs:boolean" use="optional" default="false" />
        <xs:attribute name="status" type="xs:string" use="optional" />

	</xs:complexType>	
    
    <xs:complexType name="CalculatedVariableGenericFormula">
          <xs:attribute name="name" type="tns:ObjectName" use="required"/>
          <xs:attribute name="formula" type="xs:string" use="required"/>
    </xs:complexType>
    
	<xsl:apply-templates/>
	
	<xs:complexType name="Configuration">
		<xs:sequence>
		    <xs:choice minOccurs="0" maxOccurs="1">
                <xs:element name="StandardMetaData" type="tns:StandardMetaData"/>
            </xs:choice>
            <xs:choice minOccurs="0" maxOccurs="unbounded">
                <xs:element name="CalculatedVariableGenericFormula" type="tns:CalculatedVariableGenericFormula"/>
            </xs:choice>
			<xs:choice minOccurs="0" maxOccurs="unbounded"  >
				<xsl:for-each select="/d:design/d:root/d:hasobjects[@instantiateUsing='configuration']">
				<xs:element name="{@class}" type="tns:{@class}" />
				</xsl:for-each>
			</xs:choice>
            <xs:choice minOccurs="0" maxOccurs="unbounded">
                <xs:element name="CalculatedVariable" type="tns:CalculatedVariable"/>
            </xs:choice>
		</xs:sequence>
	</xs:complexType>	
	
	<xs:element name="configuration" type="tns:Configuration" />
	
	</xs:schema>
</xsl:template>

</xsl:transform>

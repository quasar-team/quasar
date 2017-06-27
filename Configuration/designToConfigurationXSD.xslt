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



	<xsl:output indent="yes" method="xml"></xsl:output>

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
	<!-- <![CDATA[insert config data complex type]]> -->
	<xs:choice minOccurs="0" maxOccurs="1">
           	<xs:element name="ConfigurationData" type="tns:CONFIGDATATYPE"/>
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
	
	<xi:include href="../Meta/config/Meta.xsd" xpointer="xmlns(xs=http://www.w3.org/2001/XMLSchema) xpointer(/xs:schema/node())">
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
	
	
	<!-- ===start config data type=== -->
	<!-- this should be included from --> 
	<!-- <xs:include schemaLocation="xxxConfigurationData.xsd"/> -->
	<!-- but that does not work. solution ? --> 
		<!-- scalars --> 
	<xs:complexType name="boolType_t">
		<xs:annotation>
			<xs:documentation>a scalar calibration coefficient, bool
			</xs:documentation>
		</xs:annotation>
		<xs:attribute name="value" type="xs:boolean" />
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>
	<xs:complexType name="int16Type_t">
		<xs:annotation>
			<xs:documentation>a scalar calibration coefficient, int16
			</xs:documentation>
		</xs:annotation>
		<xs:attribute name="value" type="xs:short" />
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>
	<xs:complexType name="int32Type_t">
		<xs:annotation>
			<xs:documentation>a scalar calibration coefficient, int32
			</xs:documentation>
		</xs:annotation>
		<xs:attribute name="value" type="xs:integer" />
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>
	<xs:complexType name="int64Type_t">
		<xs:annotation>
			<xs:documentation>a scalar calibration coefficient, int64
			</xs:documentation>
		</xs:annotation>
		<xs:attribute name="value" type="xs:long" />
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>
	<xs:complexType name="floatType_t">
		<xs:annotation>
			<xs:documentation>a scalar calibration coefficient, float
			</xs:documentation>
		</xs:annotation>
		<xs:attribute name="value" type="xs:float" />
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>
	<xs:complexType name="doubleType_t">
		<xs:annotation>
			<xs:documentation>a scalar calibration coefficient, double
			</xs:documentation>
		</xs:annotation>
		<xs:attribute name="value" type="xs:double" />
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>

	<!-- arrays -->
	<xs:complexType name="boolArrayType_t">
		<xs:annotation>
			<xs:documentation>an array of calibration coefficients, bool
			</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="value" type="xs:boolean" maxOccurs="unbounded"></xs:element>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>

	<!-- byte does not work -->

	<xs:complexType name="int16ArrayType_t">
		<xs:annotation>
			<xs:documentation>an array of calibration coefficients, int16
			</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="value" type="xs:short" maxOccurs="unbounded"></xs:element>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>

	<xs:complexType name="int32ArrayType_t">
		<xs:annotation>
			<xs:documentation>an array of calibration coefficients, int32
			</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="value" type="xs:integer" maxOccurs="unbounded"></xs:element>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>

	<xs:complexType name="int64ArrayType_t">
		<xs:annotation>
			<xs:documentation>an array of calibration coefficients, int64
			</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="value" type="xs:long" maxOccurs="unbounded"></xs:element>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>

	<xs:complexType name="floatArrayType_t">
		<xs:annotation>
			<xs:documentation>an array of calibration coefficients, float
			</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="value" type="xs:float" maxOccurs="unbounded"></xs:element>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>

	<xs:complexType name="doubleArrayType_t">
		<xs:annotation>
			<xs:documentation>an array of calibration coefficients, double
			</xs:documentation>
		</xs:annotation>
		<xs:sequence>
			<xs:element name="value" type="xs:double" maxOccurs="unbounded"></xs:element>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" />
	</xs:complexType>



	<!-- sequence of arrays is unordered: they can have any succession -->
	<xs:complexType xmlns:xs="http://www.w3.org/2001/XMLSchema" name="CONFIGDATATYPE">
		<xs:sequence>
			<xs:choice maxOccurs="unbounded" minOccurs="0">
				<!-- scalars -->
				<xs:element name="boolScalar"   type="tns:boolType_t"   minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="int16Scalar"  type="tns:int16Type_t"  minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="int32Scalar"  type="tns:int32Type_t"  minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="int64Scalar"  type="tns:int64Type_t"  minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="floatScalar"  type="tns:floatType_t"  minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="doubleScalar" type="tns:doubleType_t" minOccurs="0" maxOccurs="unbounded" />
				
				<!-- arrays -->
				<xs:element name="boolArray" type="tns:boolArrayType_t"
					minOccurs="0" maxOccurs="unbounded" />
				<!-- -->
				<xs:element name="int16Array" type="tns:int16ArrayType_t"
					minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="int32Array" type="tns:int32ArrayType_t"
					minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="int64Array" type="tns:int64ArrayType_t"
					minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="doubleArray" type="tns:doubleArrayType_t"
					minOccurs="0" maxOccurs="unbounded" />
				<xs:element name="floatArray" type="tns:floatArrayType_t"
					minOccurs="0" maxOccurs="unbounded" />
			</xs:choice>
		</xs:sequence>
		<xs:attribute name="name" type="xs:string" use="required"/>
	</xs:complexType>
	<!-- ===end config data type=== -->
	
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

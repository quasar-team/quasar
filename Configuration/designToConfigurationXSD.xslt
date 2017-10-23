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
	<xsl:output indent="yes" method="xml"></xsl:output>

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
	<xsl:otherwise><xsl:message terminate="yes">ERROR: unknown type '<xsl:value-of select="$dataType"/>'</xsl:message></xsl:otherwise>
	</xsl:choose>
	</xsl:function>



	<xsl:template match="d:class">	
	<!--  for every class we create a complexType -->
	<xsl:element name="xs:complexType">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>

	<xs:sequence>
	
	
	<xsl:for-each select="d:cachevariable">
		<xsl:if test="d:array">
			<xsl:variable name="minimumSize">
				<xsl:choose>
					<xsl:when test="d:array[@minimumSize]"><xsl:value-of select="d:array/@minimumSize"/></xsl:when>
					<xsl:otherwise>0</xsl:otherwise>						
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="maximumSize">
				<xsl:choose>
					<xsl:when test="d:array[@maximumSize]"><xsl:value-of select="d:array/@maximumSize"/></xsl:when>
					<xsl:otherwise>unbounded</xsl:otherwise>						
				</xsl:choose>
			</xsl:variable>
				
			<xs:element name="{@name}">
				<xs:complexType>
					<xs:sequence>
						<xs:element name="value" type="{fnc:dataTypeToXsdType(@dataType)}" minOccurs="{$minimumSize}" maxOccurs="unbounded"/>
					</xs:sequence>
				</xs:complexType>
			</xs:element>
		</xsl:if>
	</xsl:for-each>
	
	<xs:choice minOccurs="0" maxOccurs="unbounded">
		<xs:element name="item" type="tns:ITEM" ></xs:element>
		<xs:element name="regexpr" type="tns:REGEXPR" ></xs:element>
		
		<xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
		<xs:element name="{@class}" type="tns:{@class}"  />
		</xsl:for-each>

	</xs:choice>
	
	
		<!-- here we go through all the config entries and filter out the ones which are arrays. 
		they become elements. No AS get/sets are needed -->
		<xsl:for-each select="child::d:configentry">
		<xsl:choose>
			<xsl:when test="d:array">
			<!-- look for the array base types and give different element 
			complex types accordingly -->
			<xsl:choose>
			<xsl:when test="@dataType='OpcUa_Boolean'">
				<xs:element name="{@name}" type="tns:BoolArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Byte'">
			<xs:element name="{@name}" type="tns:ByteArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_SByte'">
				<xs:element name="{@name}" type="tns:SByteArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Int16'">
				<xs:element name="{@name}" type="tns:Int16ArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_UInt16'">
				<xs:element name="{@name}" type="tns:UInt16ArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Int32'">
				<xs:element name="{@name}" type="tns:Int32ArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_UInt32'">
				<xs:element name="{@name}" type="tns:UInt32ArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Int64'">
				<xs:element name="{@name}" type="tns:Int64ArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_UInt64'">
				<xs:element name="{@name}" type="tns:UInt64ArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Float'">
				<xs:element name="{@name}" type="tns:FloatArrayType">  </xs:element>
			</xsl:when>
			<xsl:when test="@dataType='OpcUa_Double'">
				<xs:element name="{@name}" type="tns:DoubleArrayType">  </xs:element>
			</xsl:when>
			<!-- no string arrays for now -->
		
			</xsl:choose>	
			</xsl:when>
		</xsl:choose>
		</xsl:for-each>
	
	
	
		<!-- here we go through all cachevariables which are populated
		by configuration and filter out the ones which are arrays
		to make them elements with complex type and not attributes -->
		<xsl:for-each select="child::d:cachevariable">
		<xsl:if test="@initializeWith='configuration'">
		<xsl:choose>
		<xsl:when test="d:array">

		<!-- look for the array base types and give different element 
		complex types accordingly -->
		<xsl:choose>
		<xsl:when test="@dataType='OpcUa_Boolean'">
			<xs:element name="{@name}" type="tns:BoolArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Byte'">
			<xs:element name="{@name}" type="tns:ByteArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_SByte'">
			<xs:element name="{@name}" type="tns:SByteArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Int16'">
			<xs:element name="{@name}" type="tns:Int16ArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_UInt16'">
			<xs:element name="{@name}" type="tns:UInt16ArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Int32'">
			<xs:element name="{@name}" type="tns:Int32ArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_UInt32'">
			<xs:element name="{@name}" type="tns:UInt32ArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Int64'">
			<xs:element name="{@name}" type="tns:Int64ArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_UInt64'">
			<xs:element name="{@name}" type="tns:UInt64ArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Float'">
			<xs:element name="{@name}" type="tns:FloatArrayType">  </xs:element>
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Double'">
			<xs:element name="{@name}" type="tns:DoubleArrayType">  </xs:element>
		</xsl:when>
		<!-- no string arrays for now -->
		
		</xsl:choose>

<!-- base array type 
<xs:element name="{@dataType}"></xs:element>
-->


		</xsl:when>
		</xsl:choose>
		</xsl:if>
		</xsl:for-each>
		
	
	</xs:sequence>
		
	<!-- name of the object of this class is mandatory and not configurable. -->
	<xs:attribute name="name" type="xs:string" use="required"/>
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
				<xsl:attribute name="type"><xsl:value-of select="fnc:dataTypeToXsdType(@dataType)"/></xsl:attribute>
				
				<!-- OPCUA-458 Try carrying documentation over from Design file to Configuration schema -->
				<xsl:if test="d:documentation">
				<xsl:element name="xs:annotation">
				<xsl:element name="xs:documentation">
				<xsl:value-of select="d:documentation"/>
				</xsl:element>
				</xsl:element>
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
		element and not an attribute here. therefore generate nothing -->
		
		</xsl:when>
		<xsl:otherwise>
		<!-- we have a config entry scalar, which is an attribute -->
			<xsl:element name="xs:attribute">
			<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
			<xsl:attribute name="use">required</xsl:attribute>
			<xsl:attribute name="type"><xsl:value-of select="fnc:dataTypeToXsdType(@dataType)"/></xsl:attribute>
			
			<!-- OPCUA-458 Try carrying documentation over from Design file to Configuration schema -->
			<xsl:if test="d:documentation">
			<xsl:element name="xs:annotation">
			<xsl:element name="xs:documentation">
			<xsl:value-of select="d:documentation"/>
			</xsl:element>
			</xsl:element>
			</xsl:if>
			
		</xsl:element>
		</xsl:otherwise>
	</xsl:choose>
	
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
		
    <!-- for all these array types we should generate fixed array bounds here to 
    be safe, from Design.xml -->
	<xs:complexType name="BoolArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:boolean"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="ByteArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:byte"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="SByteArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:int"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="Int16ArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:short"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="UInt16ArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:int"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="Int32ArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:int"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="UInt32ArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:long"></xs:element>
    		<!-- use next bigger xs type as container -->
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="Int64ArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:long"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="UInt64ArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:long"></xs:element>
    		<!-- strictly speaking this is a fishy fix, but xs types are poorer than C++ types anyway and compromises have to be made.
    		nevertheless the signedness is just a type rule, and the user can cast a signed to an unsigned herself. This will
    		matter for nanoseconds timestamps obvioulsy where 64bit unsigned ints are needed --> 
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="FloatArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:float"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<xs:complexType name="DoubleArrayType">
    	<xs:sequence>
    		<xs:choice minOccurs="1" maxOccurs="unbounded">
    		<xs:element name="value" type="xs:double"></xs:element>
    		</xs:choice>
    	</xs:sequence>
	</xs:complexType>
	<!-- no string array type right now -->
	
	
	
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

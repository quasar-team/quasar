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

<xsl:transform  xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xmlns:d="http://cern.ch/quasar/Design"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">

<!-- Returns C++ class name of AddressSpace class with given Design name -->
<xsl:function name="fnc:ASClassName">
<xsl:param name="name"/>
<xsl:value-of select="concat('AS',$name)"/>
</xsl:function>

<!--  Returns C++ class name of Device class with given Design name -->
<xsl:function name="fnc:DClassName">
<xsl:param name="name"/>
<xsl:value-of select="concat('D',$name)"/>
</xsl:function>

<!--  Returns C++ class name of Device base class with given Design name -->
<xsl:function name="fnc:Base_DClassName">
<xsl:param name="name"/>
<xsl:value-of select="concat('Base_D',$name)"/>
</xsl:function>

<!--  Returns C++ class name of Parent class with given Design name -->
<xsl:function name="fnc:Parent_DClassName">
<xsl:param name="name"/>
<xsl:value-of select="concat('Parent_D',$name)"/>
</xsl:function>

<!-- Capitalize first letter (useful for creating setCamelCase from camelCase variable name) -->
<xsl:function name="fnc:capFirst">
<xsl:param name="name"/>
<xsl:value-of select="concat(upper-case(substring($name,1,1)),substring($name,2))"/>
</xsl:function>

<xsl:function name="fnc:varSetter">
<xsl:param name="name"/>
<xsl:param name="dataType"/>
<xsl:param name="forHeader"/>

<!--  task OPCUA-231: current datetime (UaDateTime::now()) should be default argument for time -->
<xsl:variable name="srcTime">const UaDateTime &amp; srcTime <xsl:if test="$forHeader='true'">= UaDateTime::now()</xsl:if></xsl:variable>
<!--  task OPCUA-167: simple datatypes (bools, integers, floats) shall be passed by value rather than by reference -->
<!--  task OPCUA-209: null setters for non-uavariant variables -->
<xsl:choose>
<xsl:when test="$dataType='OpcUa_Double' or $dataType='OpcUa_Float' or $dataType='OpcUa_UInt32' or $dataType='OpcUa_Int32' or $dataType='OpcUa_UInt64' or $dataType='OpcUa_Int64' or $dataType='OpcUa_Boolean'">
<!-- simple dataTypes -->
<xsl:value-of select="concat('set',fnc:capFirst($name),' (const ',$dataType,' value, OpcUa_StatusCode statusCode,', $srcTime,')' )"/>
</xsl:when>
<xsl:when test="$dataType='null'">
<!-- skip data argument for null setter -->
<xsl:value-of select="concat('setNull',fnc:capFirst($name),' (OpcUa_StatusCode statusCode,', $srcTime,')' )"/>
</xsl:when>
<xsl:otherwise>
<xsl:value-of select="concat('set',fnc:capFirst($name),' (const ',$dataType,' &amp; value, OpcUa_StatusCode statusCode,', $srcTime,')' )"/>
</xsl:otherwise>
</xsl:choose>
</xsl:function>

<xsl:function name="fnc:delegateWriteName">
<xsl:param name="name"/>
write<xsl:value-of select="fnc:capFirst($name)"/> 
</xsl:function>

<!--  Generates header line of function declaration (where=header) or definition(where=body)
	  Class name has to be Full C++ class name (e.g. ASDevice) and not just design class name. -->
<xsl:function name="fnc:delegateWrite">
<xsl:param name="name"/>
<xsl:param name="className"/>
<xsl:param name="where"/>
/* NOTE: This function is not intended to be used by human being. */ 
UaStatus 
<xsl:if test="$where='body'">
<xsl:value-of select="$className"/>::
</xsl:if>
<xsl:value-of select="fnc:delegateWriteName($name)"/> (Session* pSession, const UaDataValue&amp; dataValue, OpcUa_Boolean checkAccessLevel
<xsl:if test="$where='header'">
 = OpcUa_True
 </xsl:if>
 )
</xsl:function>

<xsl:function name="fnc:typeNumericId">
<xsl:param name="className"/>
AS_TYPE_<xsl:value-of select="upper-case($className)"/>
</xsl:function>

<xsl:function name="fnc:sourceVariableReadJobId">
<xsl:param name="className"/>
<xsl:param name="variableName"/>
ASSOURCEVARIABLE_<xsl:value-of select="$className"/>_READ_<xsl:value-of select="$variableName"/>
</xsl:function>

<xsl:function name="fnc:sourceVariableWriteJobId">
<xsl:param name="className"/>
<xsl:param name="variableName"/>
ASSOURCEVARIABLE_<xsl:value-of select="$className"/>_WRITE_<xsl:value-of select="$variableName"/>
</xsl:function>

<!-- Counts how many times given class is a child either to another class or to root -->
<xsl:function name="fnc:getCountParentClassesAndRoot">
<xsl:param name="xmlRoot"/>
<xsl:param name="className"/>
<xsl:value-of select="count($xmlRoot/d:design/d:class/d:hasobjects[@class=$className])+count($xmlRoot/d:design/d:root/d:hasobjects[@class=$className])"/>
</xsl:function>

<!--  Get parent type name for given class. This only makes sense if value of getCountParentClassesAndRoot for this class is 1 -->
<xsl:function name="fnc:getParentClass">
	<xsl:param name="xmlRoot"/>
	<xsl:param name="className" />
	<xsl:choose>
		<xsl:when test="fnc:getCountParentClassesAndRoot($xmlRoot,$className)!=1">
			__ERROR__
		</xsl:when>
		<xsl:otherwise>
			<xsl:choose>
				<xsl:when test="count($xmlRoot/d:design/d:root/d:hasobjects[@class=$className])=1">Root</xsl:when>
				<xsl:otherwise><xsl:value-of select="$xmlRoot/d:design/d:class/d:hasobjects[@class=$className]/../@name"/></xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>

</xsl:function>

<!--  Get parent device class name for given class. This only makes sense if value of getCountParentClassesAndRoot for this class is 1 -->
<xsl:function name="fnc:getParentDeviceClass">
	<xsl:param name="xmlRoot"/>
	<xsl:param name="className" />
	<xsl:choose>
		<xsl:when test="fnc:getCountParentClassesAndRoot($xmlRoot,$className)!=1">
			__ERROR__
		</xsl:when>
		<xsl:otherwise>
			<xsl:choose>
				<xsl:when test="count($xmlRoot/d:design/d:root/d:hasobjects[@class=$className])=1">DRoot</xsl:when>
				<xsl:otherwise><xsl:value-of select="fnc:DClassName($xmlRoot/d:design/d:class/d:hasobjects[@class=$className]/../@name)"/></xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>

</xsl:function>

<xsl:function name="fnc:dataTypeToBuiltinType">
<xsl:param name="dataType"/>
<xsl:choose>
<xsl:when test="$dataType='OpcUa_Double'">OpcUaType_Double</xsl:when>
<xsl:when test="$dataType='OpcUa_Float'">OpcUaType_Float</xsl:when>
<xsl:when test="$dataType='OpcUa_Byte'">OpcUaType_Byte</xsl:when>
<xsl:when test="$dataType='OpcUa_SByte'">OpcUaType_SByte</xsl:when>
<xsl:when test="$dataType='OpcUa_Int16'">OpcUaType_Int16</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt16'">OpcUaType_UInt16</xsl:when>
<xsl:when test="$dataType='OpcUa_Int32'">OpcUaType_Int32</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt32'">OpcUaType_UInt32</xsl:when>
<xsl:when test="$dataType='OpcUa_Int64'">OpcUaType_Int64</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt64'">OpcUaType_UInt64</xsl:when>
<xsl:when test="$dataType='OpcUa_Boolean'">OpcUaType_Boolean</xsl:when>
<xsl:when test="$dataType='UaString'">OpcUaType_String</xsl:when>
<xsl:when test="$dataType='UaByteString'">OpcUaType_ByteString</xsl:when>
<xsl:when test="$dataType='UaVariant'">OpcUaType_Variant</xsl:when>
<xsl:otherwise><xsl:message terminate="yes">Sorry, this dataType='<xsl:value-of select="$dataType"/>' is unknown.</xsl:message></xsl:otherwise>
</xsl:choose>
</xsl:function>

<!-- This returns a method name of UaVariant class that will provide conversion TO given type (FROM conversion is available as a constructor) -->
<xsl:function name="fnc:dataTypeToVariantConverter">
<xsl:param name="dataType"/>
<xsl:choose>
<xsl:when test="$dataType='OpcUa_Double'">toDouble</xsl:when>
<xsl:when test="$dataType='OpcUa_Float'">toFloat</xsl:when>
<xsl:when test="$dataType='OpcUa_Byte'">toByte</xsl:when>
<xsl:when test="$dataType='OpcUa_SByte'">toSByte</xsl:when>
<xsl:when test="$dataType='OpcUa_Int16'">toInt16</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt16'">toUInt16</xsl:when>
<xsl:when test="$dataType='OpcUa_Int32'">toInt32</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt32'">toUInt32</xsl:when>
<xsl:when test="$dataType='OpcUa_Int64'">toInt64</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt64'">toUInt64</xsl:when>
<xsl:when test="$dataType='OpcUa_Boolean'">toBool</xsl:when>
<xsl:when test="$dataType='UaByteString'">toByteString</xsl:when>
<xsl:when test="$dataType='UaString'"><xsl:message terminate="yes">Internal error - for UaString use toString() conversion</xsl:message></xsl:when>
<xsl:otherwise><xsl:message terminate="yes">Sorry, this dataType='<xsl:value-of select="$dataType"/>' is unknown.</xsl:message></xsl:otherwise>
</xsl:choose>
</xsl:function>

<!-- This returns a method name of UaVariant class that will provide conversion TO given type (FROM conversion is available as a constructor) -->
<xsl:function name="fnc:dataTypeToVariantSetter">
<xsl:param name="dataType"/>
<xsl:choose>
<xsl:when test="$dataType='OpcUa_Double'">setDouble</xsl:when>
<xsl:when test="$dataType='OpcUa_Float'">setFloat</xsl:when>
<xsl:when test="$dataType='OpcUa_Byte'">setByte</xsl:when>
<xsl:when test="$dataType='OpcUa_SByte'">setSByte</xsl:when>
<xsl:when test="$dataType='OpcUa_Int16'">setInt16</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt16'">setUInt16</xsl:when>
<xsl:when test="$dataType='OpcUa_Int32'">setInt32</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt32'">setUInt32</xsl:when>
<xsl:when test="$dataType='OpcUa_Int64'">setInt64</xsl:when>
<xsl:when test="$dataType='OpcUa_UInt64'">setUInt64</xsl:when>
<xsl:when test="$dataType='OpcUa_Boolean'">setBool</xsl:when>
<xsl:when test="$dataType='UaString'">setString</xsl:when>
<xsl:when test="$dataType='UaByteString'">setByteString</xsl:when>
<xsl:otherwise><xsl:message terminate="yes">Sorry, this dataType='<xsl:value-of select="$dataType"/>' is unknown.</xsl:message></xsl:otherwise>
</xsl:choose>
</xsl:function>

<!-- Some OPC-UA data types shall not be used in Base Device classes (i.e. UaString which is a typically Address Space construct).  -->
<xsl:function name="fnc:dataTypeToBaseDeviceType">
<xsl:param name="dataType"/>
<xsl:choose>
<xsl:when test="$dataType='UaString'">const std::string &amp;</xsl:when>
<xsl:otherwise><xsl:value-of select="$dataType"/></xsl:otherwise>
</xsl:choose>
</xsl:function>

<!-- When the type is of POD type, just returns the type, otherwise adds a const reference. Should be used to generate argument passing into custom code.  -->
<xsl:function name="fnc:fixDataTypePassingMethod">
<xsl:param name="dataType"/>
<xsl:choose>
<xsl:when test="$dataType='UaString' or $dataType='UaByteString' or $dataType='UaVariant'">const <xsl:value-of select="$dataType"/> &amp; </xsl:when>
<xsl:otherwise><xsl:value-of select="$dataType"/></xsl:otherwise>
</xsl:choose>

</xsl:function>

<!-- This returns true if given hasObjects relations points to a singleton, that is, exactly 1 object -->
<!-- The context node is some hasObject -->
<xsl:function name="fnc:isHasObjectsSingleton">
<xsl:param name="currentHasObjects"/>
<xsl:choose>
<xsl:when test="$currentHasObjects/@minOccurs='1' and $currentHasObjects/@maxOccurs='1'">true</xsl:when>
<xsl:otherwise>false</xsl:otherwise>
</xsl:choose>
</xsl:function>
 
 <!-- Checks whether given class has device logic -->
<xsl:function name="fnc:classHasDeviceLogic">
<xsl:param name="xmlRoot"/>
<xsl:param name="className"/>
<xsl:choose>
<xsl:when test="($className='Root') or ($xmlRoot/d:design/d:class[@name=$className]/d:devicelogic)">true</xsl:when>
<xsl:otherwise>false</xsl:otherwise>
</xsl:choose>
</xsl:function>
 
  <!-- Checks whether given class has device logic with own mutex -->
<xsl:function name="fnc:classDeviceLogicHasMutex">
<xsl:param name="xmlRoot"/>
<xsl:param name="className"/>
<xsl:choose>
<xsl:when test="($className='Root') or ($xmlRoot/d:design/d:class[@name=$className]/d:devicelogic/d:mutex)">true</xsl:when>
<xsl:otherwise>false</xsl:otherwise>
</xsl:choose>
</xsl:function>
 
<xsl:function name="fnc:headerFullyGenerated">
<xsl:param name="xmlRoot"/>
<xsl:param name="xsltInfo"/>
<xsl:param name="authors" />
/*  © Copyright CERN, 2015. All rights not expressly granted are reserved.
    Authors(from Quasar team): <xsl:value-of select="$authors"/>
        
    This file is part of Quasar.
   
    Quasar is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public Licence as published by
    the Free Software Foundation, either version 3 of the Licence.
    Quasar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public Licence for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Quasar.  If not, see &lt;http://www.gnu.org/licenses/&gt;.
    
    This file was completely generated by Quasar (additional info: <xsl:value-of select="$xsltInfo"/>) 
    on <xsl:value-of select="current-dateTime()"/>
 */

</xsl:function>

<xsl:function name="fnc:headerStubGenerated">
<xsl:param name="xmlRoot"/>
<xsl:param name="xsltInfo"/>
<xsl:param name="authors" />
/*  © Copyright CERN<xsl:if test="$xmlRoot/d:design/@author">, <xsl:value-of select="$xmlRoot/d:design/@author"/></xsl:if>, 2015. All rights not expressly granted are reserved.
        
    The stub of this file was generated by Quasar (additional info: <xsl:value-of select="$xsltInfo"/>) 
    on <xsl:value-of select="current-dateTime()"/>
    
    Quasar is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public Licence as published by
    the Free Software Foundation, either version 3 of the Licence.
    Quasar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public Licence for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Quasar.  If not, see &lt;http://www.gnu.org/licenses/&gt;.
    
    <xsl:if test="$xmlRoot/d:design/@author">The stub of this file was made complete by <xsl:value-of select="$xmlRoot/d:design/@author"/></xsl:if>
    
 */

</xsl:function>
 
</xsl:transform>

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

<xsl:transform version="2.0" xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"></xsl:output>
	 <xsl:param name="className"/>
	 	<xsl:param name="xsltFileName"/>

	
		<xsl:template name="classBody">
		  <xsl:variable name="singleVariableNode"><xsl:if test="/d:design/d:class[@name=$className]/@singleVariableNode='true'">true</xsl:if></xsl:variable>
	
#include &lt;<xsl:value-of select="fnc:ASClassName(@name)"/>.h&gt;

<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
#include &lt;<xsl:value-of select="fnc:DClassName($className)"/>.h&gt;
</xsl:if>

namespace AddressSpace
{
<!-- *************************************************** -->
<!-- CONSTRUCTOR *************************************** -->
<!-- *************************************************** -->
/*ctr*/
AS<xsl:value-of select="@name"/>::AS<xsl:value-of select="@name"/> (
	UaNodeId parentNodeId,
	const UaNodeId&amp; typeNodeId,
	ASNodeManager *nm,
	const Configuration::<xsl:value-of select="@name"/> &amp; config):
	OpcUa::BaseObjectType ( 
		/*nodeId*/
		nm->makeChildNodeId(parentNodeId,(<xsl:if test="$singleVariableNode='true'">"__single_item_node__"+</xsl:if>config.name()).c_str()), /*name*/(<xsl:if test="$singleVariableNode='true'">"__single_item_node__"+</xsl:if>config.name()).c_str(), nm->getNameSpaceIndex(), nm),
	m_typeNodeId (typeNodeId)
<!--  check number of variables -->

<xsl:for-each select="d:cachevariable">,
<xsl:variable name="variableName"><xsl:choose><xsl:when test="$singleVariableNode='true'">config.name().c_str()</xsl:when><xsl:otherwise>"<xsl:value-of select="@name"/>"</xsl:otherwise></xsl:choose></xsl:variable>
														  
m_<xsl:value-of select="@name"/> (new 

		<xsl:choose>
		<xsl:when test="@addressSpaceWrite='delegated'">ASDelegatingVariable&lt;AS<xsl:value-of select="$className"/>&gt; </xsl:when>
		<xsl:otherwise>OpcUa::BaseDataVariableType </xsl:otherwise>
		</xsl:choose>


		(nm->makeChildNodeId(
		<xsl:choose>
		  <xsl:when test="$singleVariableNode='true'">parentNodeId</xsl:when>
		  <xsl:otherwise>this->nodeId()</xsl:otherwise>
		</xsl:choose>,
		UaString(<xsl:value-of select="$variableName"/>)), UaString(<xsl:value-of select="$variableName"/>), nm->getNameSpaceIndex(), UaVariant(), 
<!--  now depends if write access is forbidden or not -->
<xsl:choose>
<xsl:when test="@addressSpaceWrite='forbidden'">OpcUa_AccessLevels_CurrentRead</xsl:when>
<xsl:otherwise>OpcUa_AccessLevels_CurrentReadOrWrite</xsl:otherwise>
</xsl:choose>
, nm))
</xsl:for-each>

<!--  sourcevariables -->
<xsl:for-each select="d:sourcevariable">,
<xsl:variable name="variableName"><xsl:choose><xsl:when test="$singleVariableNode='true'">config.name().c_str()</xsl:when><xsl:otherwise>"<xsl:value-of select="@name"/>"</xsl:otherwise></xsl:choose></xsl:variable>
m_<xsl:value-of select="@name"/> (new ASSourceVariable(
  nm->makeChildNodeId(
   <xsl:choose>
     <xsl:when test="$singleVariableNode='true'">parentNodeId</xsl:when>
     <xsl:otherwise>this->nodeId()</xsl:otherwise>
   </xsl:choose>,
  UaString(<xsl:value-of select="$variableName"/>)), UaString(<xsl:value-of select="$variableName"/>), nm->getNameSpaceIndex(), UaVariant(), 
  <xsl:choose>
  <xsl:when test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous' ">OpcUa_AccessLevels_CurrentRead</xsl:when>
  <xsl:when test="@addressSpaceRead='forbidden'">0</xsl:when>
  <xsl:otherwise><xsl:message terminate="yes">Something went wrong.</xsl:message></xsl:otherwise>
  </xsl:choose>
  |
  <xsl:choose>
  <xsl:when test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous' ">OpcUa_AccessLevels_CurrentWrite</xsl:when>
  <xsl:when test="@addressSpaceWrite='forbidden'">0</xsl:when>
  <xsl:otherwise><xsl:message terminate="yes">Something went wrong.</xsl:message></xsl:otherwise>
  </xsl:choose> ,
  nm, this,
  <xsl:choose>
  <xsl:when test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous'"><xsl:value-of select="fnc:sourceVariableReadJobId($className,@name)"/></xsl:when>
  <xsl:otherwise>ASSOURCEVARIABLE_NOTHING</xsl:otherwise> 
  </xsl:choose>,
   <xsl:choose>
  <xsl:when test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous'"><xsl:value-of select="fnc:sourceVariableWriteJobId($className,@name)"/></xsl:when>
  <xsl:otherwise>ASSOURCEVARIABLE_NOTHING</xsl:otherwise> 
  </xsl:choose>

  ))
</xsl:for-each>

<!-- methods -->
<xsl:for-each select="d:method">,
m_<xsl:value-of select="@name"/> (new ASDelegatingMethod&lt;AS<xsl:value-of select="$className"/>&gt;(
  nm->makeChildNodeId( this->nodeId(), UaString( "<xsl:value-of select="@name"/>" ) ),
  UaString( "<xsl:value-of select="@name"/>" ),
  nm->getNameSpaceIndex()))

</xsl:for-each>

<!-- initialize device link to zero? -->
<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">,
m_deviceLink (0)
</xsl:if>

{

UaStatus s;
UaVariant v;


				<xsl:choose>
		<xsl:when test="@singleVariableNode='true'">s = nm->addUnreferencedNode( this );</xsl:when>
		<xsl:otherwise>s = nm->addNodeAndReference( parentNodeId, this, OpcUaId_HasComponent);</xsl:otherwise>
		</xsl:choose>
		if (!s.isGood())
		{
			std::cout &lt;&lt; "While addNodeAndReference from " &lt;&lt; parentNodeId.toString().toUtf8() &lt;&lt; " to " &lt;&lt; this-&gt;nodeId().toString().toUtf8() &lt;&lt; " : " &lt;&lt; std::endl;
			ASSERT_GOOD(s);
			}


<xsl:for-each select="d:cachevariable">
<xsl:if test="@nullPolicy='nullForbidden'">
m_<xsl:value-of select="@name"/>-&gt;setDataType(UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0
</xsl:if>
<xsl:choose>
	<xsl:when test="@initializeWith='valueAndStatus'">
		<xsl:choose>
			<xsl:when test="@initialValue">
				v.<xsl:value-of select="fnc:dataTypeToVariantSetter(@dataType)"/>
				<xsl:choose>
					<xsl:when test="@dataType='UaString'">
						( "<xsl:value-of select="@initialValue"/>" );
					</xsl:when>
					<xsl:otherwise>
						 ( <xsl:value-of select="@initialValue"/> );
					</xsl:otherwise>
				</xsl:choose>
						
			</xsl:when>
			<xsl:otherwise>
				v.clear ();
			</xsl:otherwise>
		</xsl:choose>
		m_<xsl:value-of select="@name"/>-&gt;setValue(/*pSession*/0, UaDataValue(UaVariant( v ),
		<xsl:if test="not(@initialStatus)"><xsl:message terminate="yes">For valueAndStatus initializer initialStatus must be given.(at class='<xsl:value-of select="$className"/>' variable='<xsl:value-of select="@name"/>')</xsl:message></xsl:if> 
		<xsl:value-of select="@initialStatus"/>, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);
	</xsl:when>
	<xsl:when test="@initializeWith='configuration'">
		v.<xsl:value-of select="fnc:dataTypeToVariantSetter(@dataType)"/> ( 
		<xsl:choose>
			<xsl:when test="@dataType='UaString'"> config.<xsl:value-of select="@name" />().c_str() </xsl:when>
			<xsl:otherwise> config.<xsl:value-of select="@name" />() </xsl:otherwise>
		</xsl:choose>
		);
		m_<xsl:value-of select="@name"/>-&gt;setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);
	</xsl:when> 
	<xsl:otherwise>
		<xsl:message terminate="yes">
			Unintelligible option of initializeWith: <xsl:value-of select="@initializeWith"/>
		</xsl:message>
	</xsl:otherwise>
</xsl:choose>
s = nm->addNodeAndReference(
	<xsl:choose>
	  <xsl:when test="$singleVariableNode='true'">parentNodeId</xsl:when>
	  <xsl:otherwise>this</xsl:otherwise>
	</xsl:choose>,
	m_<xsl:value-of select="@name"/>, OpcUaId_HasComponent);
if (!s.isGood())
{
	std::cout &lt;&lt; "While addNodeAndReference from " &lt;&lt; this->nodeId().toString().toUtf8() &lt;&lt; " to " &lt;&lt; m_<xsl:value-of select="@name"/>-&gt;nodeId().toString().toUtf8() &lt;&lt; " : " &lt;&lt; std::endl;
	ASSERT_GOOD(s);
}


<xsl:if test="@addressSpaceWrite='delegated'">
	m_<xsl:value-of select="@name"/>-&gt;assignHandler(this, &amp;<xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:delegateWriteName(@name)"/>);
</xsl:if>

</xsl:for-each>

<xsl:for-each select="d:sourcevariable">

s = nm->addNodeAndReference(
    <xsl:choose>
      <xsl:when test="$singleVariableNode='true'">parentNodeId</xsl:when>
      <xsl:otherwise>this</xsl:otherwise>
    </xsl:choose>,
    m_<xsl:value-of select="@name"/>, OpcUaId_HasComponent);
if (!s.isGood())
{
	std::cout &lt;&lt; "While addNodeAndReference from " &lt;&lt; this->nodeId().toString().toUtf8() &lt;&lt; " to " &lt;&lt; m_<xsl:value-of select="@name"/>-&gt;nodeId().toString().toUtf8() &lt;&lt; " : " &lt;&lt; std::endl;
	ASSERT_GOOD(s);
}
</xsl:for-each>

<xsl:for-each select="d:method">
<xsl:variable name="methodName"><xsl:value-of select="@name"/></xsl:variable>

	<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
	m_<xsl:value-of select="@name"/>-&gt;assignHandler( this, &amp;<xsl:value-of select="fnc:ASClassName($className)"/>::call<xsl:value-of select="fnc:capFirst(@name)"/> ); 
	</xsl:if>


<xsl:if test="d:argument">
{

UaPropertyMethodArgument * prop = new UaPropertyMethodArgument ( 
	nm->makeChildNodeId( m_<xsl:value-of select="$methodName"/>-&gt;nodeId(), "args" ),
	OpcUa_AccessLevels_CurrentRead,
	<xsl:value-of select="count(d:argument)"/>,
	UaPropertyMethodArgument::INARGUMENTS
	);

unsigned int argCounter = 0;
<xsl:for-each select="d:argument">


	{
		UaUInt32Array dimensions;
		prop-&gt;setArgument( argCounter, UaString("<xsl:value-of select="@name"/>"), UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0), -1, dimensions, UaLocalizedText("en_US", "<xsl:value-of select="@name"/>") );
	}

    
    argCounter++;
    

</xsl:for-each>
	s = nm->addNodeAndReference(
	m_<xsl:value-of select="$methodName"/>,
    prop, 
    OpcUaId_HasProperty);
    ASSERT_GOOD(s);
}
</xsl:if>

<xsl:if test="d:returnvalue">
{
UaPropertyMethodArgument * propReturn = new UaPropertyMethodArgument ( 
	nm->makeChildNodeId( m_<xsl:value-of select="$methodName"/>-&gt;nodeId(), "return_values" ),
	OpcUa_AccessLevels_CurrentRead,
	<xsl:value-of select="count(d:returnvalue)"/>,
	UaPropertyMethodArgument::OUTARGUMENTS
	);
	<xsl:for-each select="d:returnvalue">
	{
		UaUInt32Array dimensions;
		propReturn-&gt;setArgument( <xsl:value-of select="position()-1"/>, UaString("<xsl:value-of select="@name"/>"), UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0), -1, dimensions, UaLocalizedText("en_US", "<xsl:value-of select="@name"/>") );
	}	
	</xsl:for-each>
	
	
s = nm->addNodeAndReference(
	m_<xsl:value-of select="$methodName"/>,
    propReturn, 
    OpcUaId_HasProperty);
}
ASSERT_GOOD(s);
</xsl:if>

s = nm->addNodeAndReference(
	this,
    m_<xsl:value-of select="@name"/>, OpcUaId_HasComponent);
if (!s.isGood())
{
	std::cout &lt;&lt; "While addNodeAndReference from " &lt;&lt; this->nodeId().toString().toUtf8() &lt;&lt; " to " &lt;&lt; m_<xsl:value-of select="@name"/>-&gt;nodeId().toString().toUtf8() &lt;&lt; " : " &lt;&lt; std::endl;
	ASSERT_GOOD(s);
}

</xsl:for-each>

}

<!-- *************************************************** -->
<!-- DESTRUCTOR **************************************** -->
<!-- *************************************************** -->

AS<xsl:value-of select="@name"/>::~AS<xsl:value-of select="@name"/> ()
{
	<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
	if (m_deviceLink != 0)
	{
		PRINT("deviceLink not zero!!");
		PRINT_LOCATION();
		
	}
	</xsl:if>
}

<!-- *************************************************** -->
<!-- SETTERS/GETTERS *********************************** -->
<!-- *************************************************** -->

<!-- switch method signature according to scalar or array for cachevariables 
	scalars have one value, arrays have a vector -->  
	
<!--  SETTER -->
<xsl:for-each select="d:cachevariable">

<!--  we have to see the attributes inside the array element -->
<xsl:variable name="baseType" select= "@dataType"/>
<xsl:variable name="baseName" select= "@name"/>

<xsl:choose>
<xsl:when test="d:array">
	<!-- found array signature -->
	<xsl:for-each select="d:array">
		<!-- also generate methods for min and max size -->
int <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="$baseName"/>_minimumSize()
{ 
return ( <xsl:value-of select="@minimumSize"/> ); 
}
int <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="$baseName"/>_maximumSize()
{ 
return ( <xsl:value-of select="@maximumSize"/> ); 
}
	
<!-- the following code is a bit xslt heavy-handed but at least it is clear ;-) -->
UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:varSetterArray($baseName,$baseType,'false')"/>
{ 
	<xsl:if test="@minimumSize &gt; @maximumSize">
	<xsl:message terminate="yes">
		Illegal array boundaries: minimumSize= <xsl:value-of select= "@minimumSize"/> can not be greater than maximumSize= <xsl:value-of select= "@maximumSize"/>!
	</xsl:message>
	</xsl:if>
	<xsl:if test="@minimumSize &lt; 0">
	<xsl:message terminate="yes">
		Illegal array boundaries for <xsl:value-of select="$baseName"/> : minimumSize= <xsl:value-of select= "@minimumSize"/> can not be negative !
	</xsl:message>
	</xsl:if>
	int min = <xsl:value-of select="@minimumSize"/>;
	int max = <xsl:value-of select="@maximumSize"/>;
    // make sure the design size constraints are respected during runtime
    OpcUa_Int32 dim = value.size();   
    if ( dim &lt; min || dim &gt; max){
    	PRINT("ERROR: set " &lt;&lt; this->getDeviceLink()->getFullName() &lt;&lt; " <xsl:value-of select="$baseName"/> "&lt;&lt; " runtime array size " &lt;&lt; dim &lt;&lt; " out of bounds!" );
        return( OpcUa_BadOutOfRange );
    }
    UaVariant v; 
	UaUInt32Array arrayDimensions;    
	OpcUa_Int32 valueRank = 1; // only support 1-dim arrays 
	<xsl:choose>
		<xsl:when test="$baseType='OpcUa_Boolean'">
			// boolean array
    		UaBoolArray ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setBoolArray( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Boolean, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_Byte'">
			// byte array
    		UaByteArray ua; 
    		ua.resize( dim ); // bastardo 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setByteArray( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Byte, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_SByte'">
			// signed byte array
    		UaSByteArray ua; 
    		ua.create( dim );  
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setSByteArray( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_SByte, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_Int16'">
			// int16 array
    		UaInt16Array ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setInt16Array( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Int16, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_UInt16'">
			// uint16 array
    		UaUInt16Array ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setUInt16Array( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_UInt16, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_Int32'">
			// int32 array
    		UaInt32Array ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setInt32Array( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Int32, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_UInt32'">
			// uint32 array
    		UaUInt32Array ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setUInt32Array( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_UInt32, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_Int64'">
			// int64 array
    		UaInt64Array ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setInt64Array( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Int64, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_UInt64'">
			// uint64 array
    		UaUInt64Array ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setUInt64Array( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_UInt64, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_Float'">
			// float array
    		UaFloatArray ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setFloatArray( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Float, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='OpcUa_Double'">
			// double array
    		UaDoubleArray ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	ua[ i ] = value[ i ];
   			v.setDoubleArray( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_Double, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:when test="$baseType='UaString'">
			// string array
    		UaStringArray ua; 
    		ua.create( dim ); 
    		for ( int i = 0; i &lt; dim; i++ )	{
    		        ua[ i ] = *(value[ i ].toOpcUaString()); // aha
    		}
   			v.setStringArray( ua );
   			v.arrayDimensions( arrayDimensions );
    		m_<xsl:value-of select="$baseName"/>-&gt;setValueRank( valueRank );
    		m_<xsl:value-of select="$baseName"/>-&gt;setArrayDimensions( arrayDimensions );
			m_<xsl:value-of select="$baseName"/>-&gt;setDataType( UaNodeId( OpcUaId_String, /* system namespace */ 0 ));
		</xsl:when>
		<xsl:otherwise>
			// debug: error with dataType = <xsl:value-of select= "$baseType"/>
			<xsl:message terminate="yes"> 
			Illegal or unknown array base type <xsl:value-of select= "$baseType"/> !
			</xsl:message>
		</xsl:otherwise>		
	</xsl:choose>
	return m_<xsl:value-of select="$baseName"/>-&gt;setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
}
	</xsl:for-each> <!-- array element -->
</xsl:when> <!-- test array -->

<xsl:otherwise>
	<!-- found scalar signature -->
UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:varSetter(@name,@dataType,'false')"/>
{ 
	UaVariant v = value;
	v.<xsl:value-of select="fnc:dataTypeToVariantSetter(@dataType)"/>( value );
	return m_<xsl:value-of select="@name"/>-&gt;setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
}
</xsl:otherwise> 
</xsl:choose>  <!-- scalar/array switch SETTER -->



<!-- GETTER -->
<xsl:choose>
<xsl:when test="d:array">
	<!-- found array signature -->
	

UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::get<xsl:value-of select="fnc:capFirst(@name)"/> ( vector &lt;<xsl:value-of select="@dataType"/> &gt; &amp; r) const 
{
	// get the variant, extract the array correspecting the type and put it into the vector
	UaVariant v ( * (m_<xsl:value-of select="@name"/>-&gt;value (/* session */ 0).value())) ;
	if ( !v.isArray() ){
	   	PRINT("ERROR: set " &lt;&lt; this->getDeviceLink()->getFullName() &lt;&lt; " <xsl:value-of select="$baseName"/> "&lt;&lt; " runtime array not found " );
		std::cout &lt;&lt; __FILE__ &lt;&lt; " " &lt;&lt; __LINE__ &lt;&lt; " is array= false " &lt;&lt; std::endl;
        return( OpcUa_BadIndexRangeNoData );
	} 
	<xsl:choose>
		<xsl:when test="@dataType='OpcUa_Boolean'">
			UaBooleanArray ua;
		    v.toBoolArray( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Byte'">
			UaByteArray ua;
		    v.toByteArray( ua );
			int dim = ua.size(); // bastardo ! size in bytes.. come on
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_SByte'">
			UaSByteArray ua;
		    v.toSByteArray( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Int16'">
			UaInt16Array ua;
		    v.toInt16Array( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_UInt16'">
			UaUInt16Array ua;
		    v.toUInt16Array( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Int32'">
			UaInt32Array ua;
		    v.toInt32Array( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_UInt32'">
			UaUInt32Array ua;
		    v.toUInt32Array( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Int64'">
			UaInt64Array ua;
		    v.toInt64Array( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_UInt64'">
			UaUInt64Array ua;
		    v.toUInt64Array( ua );
			int dim = ua.length();
		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Float'">
			UaFloatArray ua;
		    v.toFloatArray( ua );
 			int dim = ua.length();
 		</xsl:when>
		<xsl:when test="@dataType='OpcUa_Double'">
			UaDoubleArray ua;
		    v.toDoubleArray( ua );
			int dim = ua.length(); 
 		</xsl:when>
		<xsl:when test="@dataType='UaString'">
			UaStringArray ua;
		    v.toStringArray( ua );
			int dim = ua.length();
 		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="yes"> 
			Illegal or unknown array base type <xsl:value-of select= "@dataType"/> !
			</xsl:message>
		</xsl:otherwise>		
	</xsl:choose>
	r.clear();
	for ( int i = 0; i &lt; dim; i++ ){
	    r.push_back( ua[ i ]);
	}
    return OpcUa_Good;
}
</xsl:when>
<xsl:otherwise>
	<!-- found scalar signature -->
UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::get<xsl:value-of select="fnc:capFirst(@name)"/> (<xsl:value-of select="@dataType"/> &amp; r) const 
{
	UaVariant v (* (m_<xsl:value-of select="@name" />-&gt;value(/*session*/0).value()));
	<xsl:choose>
		<xsl:when test="@dataType='UaString'">
		if (v.type() == OpcUaType_String)
		{
			r = v.toString(); 
			return OpcUa_Good;
		}
		else
			return OpcUa_Bad; 
		</xsl:when>
		<xsl:when test="@dataType='UaVariant'">
		r = v;
		return OpcUa_Good;
		</xsl:when>
		<xsl:otherwise>return v.<xsl:value-of select="fnc:dataTypeToVariantConverter(@dataType)"/> ( r ); </xsl:otherwise>
	</xsl:choose>
}

<xsl:if test="@nullPolicy='nullForbidden'">
	/* short getter (possible because this variable will never be null) */
	<xsl:value-of select="@dataType"/><xsl:text> </xsl:text><xsl:value-of select="fnc:ASClassName($className)"/>::get<xsl:value-of select="fnc:capFirst(@name)"/> () const
	{
		UaVariant v (* m_<xsl:value-of select="@name"/>-&gt;value (0).value() );
		<xsl:value-of select="@dataType"/> v_value;
		<xsl:choose>
			<xsl:when test="@dataType='UaString'">v_value = v.toString(); </xsl:when>
			<xsl:otherwise>v.<xsl:value-of select="fnc:dataTypeToVariantConverter(@dataType)"/> ( v_value ); </xsl:otherwise>
		</xsl:choose>
		return v_value;
	}
</xsl:if>

<xsl:if test="@nullPolicy='nullAllowed'">
    /* null-setter (possible because nullPolicy='nullAllowed') */
    UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:varSetter(@name,'null','false')"/>
	{
		UaVariant v;
		return m_<xsl:value-of select="@name"/>-&gt;setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
	}
</xsl:if>

</xsl:otherwise>
</xsl:choose>   <!-- scalar/array switch GETTER -->

</xsl:for-each> <!-- cache variable -->



<!-- *************************************************** -->
<!-- DELEGATES ***************************************** -->
<!-- *************************************************** -->
/* generate delegates (if requested) */
<xsl:for-each select="d:cachevariable">
<xsl:if test="@addressSpaceWrite='delegated' or @addressSpaceWrite='regular'">
<xsl:value-of select="fnc:delegateWrite(@name,fnc:ASClassName($className),'body')"/>
	{
	
	<xsl:if test="@dataType != 'UaVariant'">
	/* ensure that data type passed by OPC UA client matches specification */
	UaVariant v (*dataValue.value());
	if (v.type() != <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)" /> )
	{
	<xsl:if test="@nullPolicy='nullForbidden'">
		return OpcUa_BadDataEncodingInvalid;
	</xsl:if>
	if (v.type() != OpcUaType_Null)
		return OpcUa_BadDataEncodingInvalid; // now we know it is neither the intended datatype nor NULL

	}
	</xsl:if>
	
	<xsl:choose>
		<xsl:when test="@addressSpaceWrite='regular'">
			return OpcUa_Good;
		</xsl:when>
		<xsl:when test="@addressSpaceWrite='delegated'">			
			<xsl:choose>
				<xsl:when test="@dataType='UaString'">
					v_value = v.toString();
				</xsl:when>
				<xsl:when test="@dataType='UaVariant'">
					v_value = *dataValue.value();
				</xsl:when>
				<xsl:otherwise>
				
					<xsl:choose>
					<xsl:when test="d:array">
					// array point2
					vector&lt;<xsl:value-of select="@dataType" />&gt; v_value;
					// call the array method to fill the vector
					// no size check for a change, since methods are written by the user anyway
					
					<xsl:choose>
					<xsl:when test="@dataType='OpcUa_Boolean'">
					    UaBoolArray ua;
   						v.toBoolArray( ua );
    					int dim = ua.length();
    				</xsl:when>

					<xsl:when test="@dataType='OpcUa_Byte'">
					    UaByteArray ua;
   						v.toByteArray( ua );
    					int dim = ua.size(); // miststueck
    				</xsl:when>
					<xsl:when test="@dataType='OpcUa_SByte'">
					    UaSByteArray ua;
   						v.toSByteArray( ua );
    					int dim = ua.length(); // miststueck miststueck !!
    				</xsl:when>
  
					<xsl:when test="@dataType='OpcUa_Int16'">
					    UaInt16Array ua;
   						v.toInt16Array( ua );
    					int dim = ua.length();
    				</xsl:when>
    				
					<xsl:when test="@dataType='OpcUa_UInt16'">
					    UaUInt16Array ua;
   						v.toUInt16Array( ua );
    					int dim = ua.length();
    				</xsl:when>
    				
					<xsl:when test="@dataType='OpcUa_Int32'">
					    UaInt32Array ua;
   						v.toInt32Array( ua );
    					int dim = ua.length();
    				</xsl:when>
    				
					<xsl:when test="@dataType='OpcUa_UInt32'">
					    UaUInt32Array ua;
   						v.toUInt32Array( ua );
    					int dim = ua.length();
    				</xsl:when>
    				
 					<xsl:when test="@dataType='OpcUa_Int64'">
					    UaInt64Array ua;
   						v.toInt64Array( ua );
    					int dim = ua.length();
    				</xsl:when>
    				
					<xsl:when test="@dataType='OpcUa_UInt64'">
					    UaUInt64Array ua;
   						v.toUInt64Array( ua );
    					int dim = ua.length();
    				</xsl:when>
 
					<xsl:when test="@dataType='OpcUa_Float'">
					    UaFloatArray ua;
   						v.toFloatArray( ua );
    					int dim = ua.length();
    				</xsl:when>    					

 					<xsl:when test="@dataType='OpcUa_Double'">
					    UaDoubleArray ua;
   						v.toDoubleArray( ua );
    					int dim = ua.length();
    				</xsl:when>    					
    				</xsl:choose>
    					v_value.clear();
    					for ( int i = 0; i &lt; dim; i++ ) {
        					v_value.push_back( ua[ i ]);
    					}
    					
    				</xsl:when>
					<xsl:otherwise>
					// scalar point2
					<xsl:value-of select="@dataType" /> v_value;
					v.<xsl:value-of select="fnc:dataTypeToVariantConverter(@dataType)" />( v_value );
					</xsl:otherwise>
					</xsl:choose>
				
				</xsl:otherwise> 
				
			</xsl:choose>

			/* if device logic type specified, then generate calling functions */
			<xsl:choose>
				<xsl:when test="fnc:classHasDeviceLogic(/,$className)='true'">
					if (m_deviceLink != 0)
					{
					 return m_deviceLink-><xsl:value-of select="fnc:delegateWriteName(@name)" /> (v_value);
					}
					else
						return OpcUa_Bad;
				</xsl:when>
				<xsl:otherwise>
					return OpcUa_BadNotImplemented;
				</xsl:otherwise>
			</xsl:choose>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message terminate="yes">
				Something went wrong. Please notify Piotr.
			</xsl:message>
		</xsl:otherwise>
	</xsl:choose>

	}
</xsl:if>
</xsl:for-each>

<!-- *************************************************** -->
<!-- METHODS ******************************************* -->
<!-- *************************************************** -->
<xsl:if test="d:method">
		/* Call handler defined because at least one method is declared */
		UaStatus <xsl:value-of select="fnc:ASClassName(@name)"/>::beginCall ( 
			 	MethodManagerCallback *  	pCallback, 
			 	const ServiceContext &amp; 	serviceContext,
			 	OpcUa_UInt32  	callbackHandle, 
				MethodHandle *  	pMethodHandle, 
				const UaVariantArray &amp;  	inputArguments  
		)
		{
		<xsl:choose>
		<xsl:when test="fnc:classHasDeviceLogic(/,$className)='true'">
		
		
		MethodHandleUaNode* upper = dynamic_cast&lt;MethodHandleUaNode*&gt; ( pMethodHandle );
		if (!upper)
		{
			return OpcUa_BadInternalError;
		}
		
		ASDelegatingMethod&lt; <xsl:value-of select="fnc:ASClassName(@name)"/>  &gt; * impl = 
		  static_cast&lt; ASDelegatingMethod&lt; <xsl:value-of select="fnc:ASClassName(@name)"/> &gt; * &gt; ( upper-&gt;pUaMethod() );
		if (impl != 0)
		{
		  return impl->call(pCallback, callbackHandle, inputArguments);		  
		}
		else
		  return OpcUa_BadInternalError;
		
		</xsl:when>
		<xsl:otherwise>
		return OpcUa_BadInternalError;
		</xsl:otherwise>
		</xsl:choose> 
		}
</xsl:if>

<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
	<xsl:for-each select="d:method">
	UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::call<xsl:value-of select="fnc:capFirst(@name)"/> (
		MethodManagerCallback* pCallback,
			OpcUa_UInt32           callbackHandle,
			const UaVariantArray&amp;  inputArguments) 
	{

	  
	    <xsl:for-each select="d:argument">
	    	<!-- TODO: implement strict checking? -->
	    	<xsl:value-of select="@dataType"/> arg_<xsl:value-of select="@name"/> ;
	    	<xsl:choose>
	    	<xsl:when test="@dataType='UaString'"> 
	    	
	    	arg_<xsl:value-of select="@name"/> = (UaVariant(inputArguments[<xsl:value-of select="position()-1"/>])).toString();   
	    	</xsl:when>
	    	<xsl:when test="@dataType='UaVariant'">
	    	arg_<xsl:value-of select="@name"/> = inputArguments[<xsl:value-of select="position()-1"/>];
	    	</xsl:when>
	    	<xsl:otherwise>
	    	if ((UaVariant(inputArguments[<xsl:value-of select="position()-1"/>])).<xsl:value-of select="fnc:dataTypeToVariantConverter(@dataType)"/>( arg_<xsl:value-of select="@name"/> ) != OpcUa_Good )
	    		return OpcUa_BadDataEncodingInvalid; 
	    	</xsl:otherwise>
	    	</xsl:choose>

	    </xsl:for-each>
	    
	    <xsl:for-each select="d:returnvalue">
	    	<xsl:value-of select="@dataType"/> rv_<xsl:value-of select="@name"/> ;
	    </xsl:for-each>
	    
	    UaStatus stat = getDeviceLink()-&gt;call<xsl:value-of select="fnc:capFirst(@name)"/> (
	    <xsl:for-each select="d:argument">
	    	arg_<xsl:value-of select="@name"/><xsl:if test="position() &lt; (count(../d:argument)+count(../d:returnvalue))">,</xsl:if>
	    </xsl:for-each>
	    <xsl:for-each select="d:returnvalue">
	    	rv_<xsl:value-of select="@name"/><xsl:if test="position() &lt; count(../d:returnvalue)">,</xsl:if>
	    </xsl:for-each>
	    );
	    UaStatusCodeArray   	inputArgumentResults;
	    UaDiagnosticInfos   	inputArgumentDiag;
	    UaVariantArray       	outputArguments;
	    
	    <xsl:if test="d:returnvalue">
	    UaVariant helper;
	    outputArguments.create( <xsl:value-of select="count(d:returnvalue)"/> );
	    <xsl:for-each select="d:returnvalue">
	    	<xsl:choose>
	    		<xsl:when test="@dataType='OpcUa_Boolean'">
	    		    <!--  we do this because OpcUa_Boolean decays to char and not C++ bool. -->
	    			helper.setBool( rv_<xsl:value-of select="@name"/> );
	    		</xsl:when>
	    		<xsl:when test="@dataType='UaByteString'">
	    			helper.setByteString( rv_<xsl:value-of select="@name"/>, /*detach*/false );
	    		</xsl:when>
	    		<xsl:otherwise>
	    			helper = rv_<xsl:value-of select="@name"/>;
	    		</xsl:otherwise>
	    	</xsl:choose>
	    	
	    	helper.copyTo( &amp;outputArguments[<xsl:value-of select="position()-1"/>] );
	    </xsl:for-each>
	    </xsl:if>
	    
	    pCallback->finishCall( callbackHandle, inputArgumentResults, inputArgumentDiag, outputArguments, stat );
	    return OpcUa_Good;
	}
	</xsl:for-each>
</xsl:if>

<!-- *************************************************** -->
<!-- LINK/UNLINK DEVICE LOGIC ************************** -->
<!-- *************************************************** -->
/* generate device logic link */

		<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
		void <xsl:value-of select="fnc:ASClassName(@name)"/>::linkDevice( Device::<xsl:value-of select="fnc:DClassName(@name)"/> *deviceLink)
		{
			if (m_deviceLink != 0)
			{
				/* This is an error -- device can be linked at most in the object's lifetime. */
				//TODO After error handling is discussed, abort in smarter way
				abort();
				
			}
			else
				m_deviceLink = deviceLink;
		}
		
		
		void <xsl:value-of select="fnc:ASClassName(@name)"/>::unlinkDevice ()
		{
			m_deviceLink = 0;
		}
		</xsl:if>

}

	</xsl:template>
	
	<xsl:template match="/">	
    <xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToClassBody.xslt','Piotr Nikiel')"/>
	#include &lt;iostream&gt;

	
	<xsl:if test="not(/d:design/d:class[@name=$className])">
		<xsl:message terminate="yes">Class not found.</xsl:message>
	</xsl:if>
	<xsl:for-each select="/d:design/d:class[@name=$className]">
	<xsl:call-template name="classBody"/>
	</xsl:for-each>
	</xsl:template>



</xsl:transform>

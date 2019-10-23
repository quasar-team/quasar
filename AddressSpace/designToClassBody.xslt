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
	 
	 	<xsl:param name="xsltFileName"/>

	
		<xsl:template name="classBody">
        <xsl:param name="className"/>
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
		<xsl:otherwise>ChangeNotifyingVariable</xsl:otherwise>
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

<!-- constructor body starts here -->
{

UaStatus s;
UaVariant v;


		<xsl:choose>
		<xsl:when test="@singleVariableNode='true'">s = nm->addUnreferencedNode( this );</xsl:when>
		<xsl:otherwise>s = nm->addNodeAndReference( parentNodeId, this, OpcUaId_HasComponent);</xsl:otherwise>
		</xsl:choose>
		if (!s.isGood())
		{
			LOG(Log::ERR) &lt;&lt; "While addNodeAndReference from " &lt;&lt; parentNodeId.toString().toUtf8() &lt;&lt; " to " &lt;&lt; this-&gt;nodeId().toString().toUtf8() &lt;&lt; " : ";
			ASSERT_GOOD(s);
		}


<xsl:for-each select="d:cachevariable">
	<xsl:if test="@nullPolicy='nullForbidden'">
	m_<xsl:value-of select="@name"/>-&gt;setDataType(UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0
	</xsl:if>
    <xsl:if test="d:array">
        m_<xsl:value-of select="@name"/>-&gt;setValueRank( 1 );
        {
            UaUInt32Array arrayDimensions;
            arrayDimensions.create(1);
            m_<xsl:value-of select="@name"/>-&gt;setArrayDimensions(arrayDimensions);    
        }
    </xsl:if>
<xsl:choose>
	<xsl:when test="@initializeWith='valueAndStatus'">
		<xsl:choose>
		<xsl:when test="d:array">
			<xsl:if test="@initialValue">
				<xsl:message terminate="yes">ERROR (at class=<xsl:value-of select="$className"/> variable=<xsl:value-of select="@name"/>): An array can't be initialized with initialValue initializer.</xsl:message>
			</xsl:if>
			
		</xsl:when>
		<xsl:otherwise> <!-- not an array -->
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
		</xsl:otherwise>
		</xsl:choose>

	</xsl:when>
	<xsl:when test="@initializeWith='configuration'">
	
	<xsl:choose>
	<xsl:when test="d:array">
	// found array signature: have to put the vector into the variant
	{ // scope 
			unsigned int dim = config.<xsl:value-of select="@name" />().value().size();
			
 		   	// make sure the design size constraints are respected during runtime
			unsigned int min = <xsl:value-of select="@name"/>_minimumSize();
			unsigned int max = <xsl:value-of select="@name"/>_maximumSize();
 		    if ( dim &lt; min || dim &gt; max )
 		    {
 		    	throw_runtime_error_with_origin("Size of configuration data supplied to the constructor is out of bounds. Size is "+boost::lexical_cast&lt;std::string&gt;(dim)+" and bounds are ["+boost::lexical_cast&lt;std::string&gt;(min)+","+boost::lexical_cast&lt;std::string&gt;(max)+"]");
 		   	}
 		   	std::vector &lt;<xsl:value-of select="@dataType"/>&gt; vect;
 		   	<xsl:choose>
				<xsl:when test="@dataType='UaString'">
				vect.assign(dim, UaString());
				std::transform( 
					config.<xsl:value-of select="@name" />().value().begin(), 
					config.<xsl:value-of select="@name" />().value().end(), 
					vect.begin(), 
					[](const std::string&amp; x){ return x.c_str(); }  );

				</xsl:when>
				<xsl:otherwise>
				vect = config.<xsl:value-of select="@name" />().value();
				</xsl:otherwise>
			</xsl:choose>
			
			UaVariant v;
			<xsl:value-of select="fnc:convertVectorToUaVariant('vect', 'v', @dataType)"/>
			m_<xsl:value-of select="@name"/>-&gt;setDataType( UaNodeId(<xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0) );
			m_<xsl:value-of select="@name"/>-&gt;setValue(/*pSession*/0, UaDataValue( v , OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);			

	} // scope
	
	</xsl:when>
	<xsl:otherwise>
		// found scalar signature: can simply load the variant with the scalar
		// fnc:dataTypeToVariantSetter(@dataType)
		v.<xsl:value-of select="fnc:dataTypeToVariantSetter(@dataType)"/> ( 
		<xsl:choose>
			<xsl:when test="@dataType='UaString'"> config.<xsl:value-of select="@name" />().c_str() </xsl:when>
			<xsl:otherwise> config.<xsl:value-of select="@name" />() </xsl:otherwise>
		</xsl:choose>
		);
		m_<xsl:value-of select="@name"/>-&gt;setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);
		
	</xsl:otherwise>
	</xsl:choose>
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

<xsl:if test="fnc:isQuasarDataTypeNumeric(@dataType)='true' and not(d:array)">
    CalculatedVariables::Engine::registerVariableForCalculatedVariables( m_<xsl:value-of select="@name"/> );
</xsl:if>

</xsl:for-each>

<xsl:for-each select="d:sourcevariable">
m_<xsl:value-of select="@name"/>-&gt;setDataType(UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0 )); // assumption: BuiltInTypeId matches numeric address of the type in namespace0
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
		OpcUa_Int32 valueRank = -1; // scalar by default
		<xsl:if test="d:array">
			valueRank = 1;
		</xsl:if>
		prop-&gt;setArgument( argCounter, UaString("<xsl:value-of select="@name"/>"), UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0), valueRank, dimensions, UaLocalizedText("en_US", "<xsl:value-of select="@name"/>") );
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
		OpcUa_Int32 valueRank = -1; // scalar by default
		<xsl:if test="d:array">
			valueRank = 1;
		</xsl:if>
		propReturn-&gt;setArgument( <xsl:value-of select="position()-1"/>, UaString("<xsl:value-of select="@name"/>"), UaNodeId( <xsl:value-of select="fnc:dataTypeToBuiltinType(@dataType)"/>, 0), valueRank, dimensions, UaLocalizedText("en_US", "<xsl:value-of select="@name"/>") );
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
		LOG(Log::ERR) &lt;&lt; "deviceLink not zero!!";	
	}
	</xsl:if>
}

<!-- *************************************************** -->
<!-- SETTERS/GETTERS *********************************** -->
<!-- *************************************************** -->
/* generate setters and getters (if requested) */
<xsl:for-each select="d:cachevariable">

<!--  we have to see the attributes inside the array element -->
<xsl:variable name="variableDataType" select= "@dataType"/>
<xsl:variable name="variableName" select= "@name"/>

<xsl:choose>
<xsl:when test="d:array">
	<!-- found array signature -->
	<xsl:for-each select="d:array">
		<!-- also generate methods for min and max size -->
OpcUa_UInt32 <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="$variableName"/>_minimumSize()
{ 
	<xsl:choose>
	<xsl:when test="@minimumSize">
	return <xsl:value-of select="@minimumSize"/>; 
	</xsl:when>
	<xsl:otherwise>
	return 0; 
	</xsl:otherwise>
	</xsl:choose>
}
OpcUa_UInt32 <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="$variableName"/>_maximumSize()
{ 
	<xsl:choose>
	<xsl:when test="@maximumSize">
	return <xsl:value-of select="@maximumSize"/>; 
	</xsl:when>
	<xsl:otherwise>
	return INT_MAX; 
	</xsl:otherwise>
	</xsl:choose>
}
	
<!-- the following code is a bit xslt heavy-handed but at least it is clear ;-) -->
UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:varSetterArray($variableName,$variableDataType,'false')"/>
{ 
	unsigned int min = <xsl:value-of select="$variableName"/>_minimumSize();
	unsigned int max = <xsl:value-of select="$variableName"/>_maximumSize();
 
    // make sure the design size constraints are respected during runtime
    OpcUa_UInt32 dim = value.size();   
    if ( dim &lt; min || dim &gt; max )
    {
    	LOG(Log::ERR) &lt;&lt; "Attempted to set an array of size " &lt;&lt; dim &lt;&lt; " which is out of Design bounds!";
        return OpcUa_BadIndexRangeInvalid;
    } 
	 	
 	UaVariant v;
 	<xsl:value-of select="fnc:convertVectorToUaVariant('value', 'v', $variableDataType)"/>
	return m_<xsl:value-of select="$variableName"/>-&gt;setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
}
	</xsl:for-each> <!-- array element -->
</xsl:when> <!-- test array -->

<xsl:otherwise>
	<!-- found scalar signature -->
UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:varSetter(@name,@dataType,'false')"/>
{
<xsl:choose>
	<xsl:when test="@dataType='UaVariant'">
		return m_<xsl:value-of select="@name"/>-&gt;setValue (0, UaDataValue (value, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
	</xsl:when>
	<xsl:otherwise>
		UaVariant v;
		<xsl:choose>
			<xsl:when test="@dataType='UaByteString'">
				v.setByteString(const_cast&lt;UaByteString &amp;&gt;(value), /*detach*/ OpcUa_False ); // this const_case should be safe because we don't detach the value
			</xsl:when>
			<xsl:otherwise>
				v.<xsl:value-of select="fnc:dataTypeToVariantSetter(@dataType)"/>( value );
			</xsl:otherwise>
		</xsl:choose>
		return m_<xsl:value-of select="@name"/>-&gt;setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
	</xsl:otherwise>
</xsl:choose>	
}
</xsl:otherwise> 
</xsl:choose>



<!-- GETTER -->
<xsl:choose>
<xsl:when test="d:array">
	<!-- found array signature -->

UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::get<xsl:value-of select="fnc:capFirst(@name)"/> ( std::vector &lt;<xsl:value-of select="@dataType"/> &gt; &amp; r) const 
{
	UaVariant v ( * (m_<xsl:value-of select="@name"/>-&gt;value (/* session */ 0).value())) ;
	if ( !v.isArray() )
	{
        return OpcUa_BadIndexRangeNoData;
	} 
	<xsl:value-of select="fnc:convertUaVariantToVector('v', 'r', @dataType)"/>
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
</xsl:otherwise>
</xsl:choose>

<xsl:if test="@nullPolicy='nullForbidden'">
<xsl:choose>
<xsl:when test="d:array">
	/* short getter (possible because this variable will never be null) */
std::vector&lt;<xsl:value-of select="@dataType"/>&gt; <xsl:value-of select="fnc:ASClassName($className)"/>::get<xsl:value-of select="fnc:capFirst(@name)"/> () const
{
	UaVariant variant (* m_<xsl:value-of select="@name"/>-&gt;value (0).value() );
	std::vector&lt;<xsl:value-of select="@dataType"/>&gt; vector;
	<xsl:value-of select="fnc:convertUaVariantToVector('variant', 'vector', @dataType)"/>
	return vector;
}	

</xsl:when>
<xsl:otherwise>
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
</xsl:otherwise>
</xsl:choose>
</xsl:if>

<xsl:if test="@nullPolicy='nullAllowed'">
    /* null-setter (possible because nullPolicy='nullAllowed') */
    UaStatus <xsl:value-of select="fnc:ASClassName($className)"/>::<xsl:value-of select="fnc:varSetter(@name,'null','false')"/>
{
UaVariant v;

return m_<xsl:value-of select="@name"/>-&gt;setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}
</xsl:if>


</xsl:for-each>

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
				
				// cache delegated: must distinguish between scalar and array
				<xsl:choose>
					<xsl:when test="d:array">
					std::vector&lt;UaString&gt; v_value;
					<xsl:value-of select="fnc:convertUaVariantToVector('v', 'v_value', @dataType)"/>					
					</xsl:when>
					<xsl:otherwise> 
					UaString v_value;
					v_value = v.toString();
					</xsl:otherwise>
				</xsl:choose>
				
					
				</xsl:when>
				<xsl:when test="@dataType='UaVariant'">
					UaVariant v_value = *dataValue.value();
				</xsl:when>
				<xsl:otherwise>
					<xsl:choose>
					<xsl:when test="d:array">
					std::vector&lt;<xsl:value-of select="@dataType" />&gt; v_value;
					<xsl:value-of select="fnc:convertUaVariantToVector('v', 'v_value', @dataType)"/>
    				</xsl:when>
					<xsl:otherwise>
					// scalar
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

	    if (inputArguments.length() != <xsl:value-of select="count(d:argument)"/>)
            return OpcUa_BadArgumentsMissing;
      
	    <xsl:for-each select="d:argument">
            <xsl:variable name="argumentIndex"><xsl:value-of select="position()-1"/></xsl:variable>
	    	<!-- TODO: implement strict checking? -->
	    	<xsl:value-of select="fnc:quasarDataTypeToCppType(@dataType,d:array)"/> arg_<xsl:value-of select="@name"/> ;
            <xsl:choose>
                <xsl:when test="d:array">
                    {
                        <xsl:variable name="input">inputArguments[<xsl:value-of select="$argumentIndex"/>]</xsl:variable>
                        UaStatus conversionStatus = <xsl:value-of select="fnc:convertUaVariantToVector($input,concat('arg_',@name),@dataType)"/>
                        if (!conversionStatus.isGood())
                            return conversionStatus;
                    }
                </xsl:when>
                <xsl:otherwise>
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
                </xsl:otherwise>
            </xsl:choose>

	    </xsl:for-each>
        
        <xsl:if test="@executionSynchronicity='asynchronous'">
        #ifdef BACKEND_OPEN62541
        #error asynchronous method execution is not available for open62541 backend
        #endif
        
        AddressSpace::SourceVariables_getThreadPool()-&gt;addJob(  [this,callbackHandle,pCallback<xsl:for-each select="d:argument">,arg_<xsl:value-of select="@name"/></xsl:for-each>](){
        </xsl:if>
	    
	    <xsl:for-each select="d:returnvalue">
	    	<xsl:value-of select="fnc:quasarDataTypeToCppType(@dataType,d:array)"/> rv_<xsl:value-of select="@name"/> ;
	    </xsl:for-each>
	    
        UaStatusCodeArray       inputArgumentResults;
        UaDiagnosticInfos       inputArgumentDiag;
        UaVariantArray          outputArguments;
        
        try
        {

	    UaStatus stat = getDeviceLink()-&gt;call<xsl:value-of select="fnc:capFirst(@name)"/> (
	    <xsl:for-each select="d:argument">
	    	arg_<xsl:value-of select="@name"/><xsl:if test="position() &lt; (count(../d:argument)+count(../d:returnvalue))">,</xsl:if>
	    </xsl:for-each>
	    <xsl:for-each select="d:returnvalue">
	    	rv_<xsl:value-of select="@name"/><xsl:if test="position() &lt; count(../d:returnvalue)">,</xsl:if>
	    </xsl:for-each>
	    );

	    
	    <xsl:if test="d:returnvalue">
	    UaVariant helper;
	    outputArguments.create( <xsl:value-of select="count(d:returnvalue)"/> );
	    <xsl:for-each select="d:returnvalue">
        <xsl:choose>
            <xsl:when test="d:array">
                <xsl:variable name="input">rv_<xsl:value-of select="@name"/></xsl:variable>
                <xsl:value-of select="fnc:convertVectorToUaVariant($input,'helper',@dataType)"/>
            </xsl:when>
            <xsl:otherwise>
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
            </xsl:otherwise>
        </xsl:choose>

	    	
	    	helper.copyTo( &amp;outputArguments[<xsl:value-of select="position()-1"/>] );
	    </xsl:for-each>
	    </xsl:if>
	    
	    pCallback->finishCall( callbackHandle, inputArgumentResults, inputArgumentDiag, outputArguments, stat );
	    return (OpcUa_StatusCode)OpcUa_Good;
        
        }
        catch (std::exception&amp; e)
        {
            LOG(Log::ERR) &lt;&lt; "method call of method <xsl:value-of select="@name"/> thrown an exception (should have been handled in the method body...): " &lt;&lt; e.what();
            UaStatus badStatus = OpcUa_BadInternalError;
            pCallback->finishCall( callbackHandle, inputArgumentResults, inputArgumentDiag, outputArguments, badStatus );
            return OpcUa_BadInternalError;
        }
  
        <xsl:if test="@executionSynchronicity='asynchronous'">
        }, std::string("method call of method <xsl:value-of select="@name"/> on object ")+this-&gt;nodeId().toString().toUtf8());
        return OpcUa_Good;
        </xsl:if>
        

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
	#include &lt;climits&gt;
	
	#include &lt;boost/lexical_cast.hpp&gt;
	
	#include &lt;ArrayTools.h&gt;
	#include &lt;Utils.h&gt;
    #include &lt;ChangeNotifyingVariable.h&gt;
    #include &lt;CalculatedVariablesEngine.h&gt;

    #include &lt;SourceVariables.h&gt;
	
	<xsl:for-each select="/d:design/d:class">
	<xsl:call-template name="classBody">
    <xsl:with-param name="className"><xsl:value-of select="@name"/></xsl:with-param>
    </xsl:call-template>
	</xsl:for-each>
	</xsl:template>



</xsl:transform>

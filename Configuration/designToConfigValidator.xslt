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
	<xsl:output method="text"></xsl:output>
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:param name="xsltFileName"/>


	
	<xsl:template name="validateHasObjects">
	<!-- This template should be run on hasObjects context node -->
	<xsl:param name="containingClass"/>
	<xsl:param name="onlyHeader"/>
	bool validate<xsl:value-of select="$containingClass"/>Has<xsl:value-of select="@class"/> ( Device::<xsl:value-of select="fnc:DClassName($containingClass)"/> *object )
	<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
	<xsl:choose>
	<xsl:when test="$onlyHeader='true'">
	;
	</xsl:when>
	<xsl:otherwise>
	{
	    <!-- Skip validation if given class doesnt have device logic. -->
		<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
	    size_t size = object-><xsl:value-of select="lower-case(@class)"/>s().size();
	    (void)size; /* prevent unused variable warning if bounds are not given in design file */
	    <xsl:if test="@minOccurs">
	    if ( size &lt; <xsl:value-of select="@minOccurs"/>  )
		{
			throw std::runtime_error ("Configuration error: <xsl:value-of select="$containingClass"/> should have no less than <xsl:value-of select="@minOccurs"/><xsl:text> </xsl:text><xsl:value-of select="@class"/>, but it has "+Utils::toString(size) + " (for "+object-&gt;getFullName()+")");
		}
	    </xsl:if>
	    <xsl:if test="@maxOccurs">
	    if ( size &gt; <xsl:value-of select="@maxOccurs"/>  )
		{
			throw std::runtime_error ("Configuration error: <xsl:value-of select="$containingClass"/> should have no more than <xsl:value-of select="@minOccurs"/><xsl:text> </xsl:text><xsl:value-of select="@class"/>, but it has "+Utils::toString(size) + " (for "+object-&gt;getFullName()+")");
		}
	    </xsl:if>
	    
	    BOOST_FOREACH( Device::<xsl:value-of select="fnc:DClassName(@class)"/> *child, object-><xsl:value-of select="lower-case(@class)"/>s())
	    {
	        validate<xsl:value-of select="@class"/>( child );
	    }    
	    </xsl:if>
	    return true;
	}
	</xsl:otherwise>
	</xsl:choose>
	

	</xsl:template>

    <xsl:template name="validateClass">
    <xsl:param name="onlyHeader"/>
    <xsl:variable name="containingClass"><xsl:value-of select="@name"/></xsl:variable>
    <xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
    <xsl:call-template name="validateHasObjects">
    <xsl:with-param name="containingClass"><xsl:value-of select="$containingClass"/></xsl:with-param>
    <xsl:with-param name="onlyHeader"><xsl:value-of select="$onlyHeader"/></xsl:with-param>
    </xsl:call-template>
    </xsl:for-each>
	<!-- This template should be run on class context node -->
	<xsl:variable name="class"><xsl:value-of select="@name"/></xsl:variable>
	bool validate<xsl:value-of select="$class"/> ( Device::<xsl:value-of select="fnc:DClassName($class)"/> * object )<xsl:choose><xsl:when test="$onlyHeader='true'">;
	</xsl:when>
	<xsl:otherwise>
	{
		<xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
		<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
		validate<xsl:value-of select="$class"/>Has<xsl:value-of select="@class"/>( object );		
		</xsl:for-each>
		return true;
	}
	</xsl:otherwise>
	</xsl:choose>
    </xsl:template>


	
	<xsl:template match="/">
    <xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToConfigValidator.xslt','Piotr Nikiel')"/>
	#include &lt;iostream&gt;
	#include &lt;boost/foreach.hpp&gt;
	
	#include &lt;ASUtils.h&gt;
	#include &lt;ASInformationModel.h&gt;

	#include &lt;Utils.h&gt;
	
	#include &lt;DRoot.h&gt;
	
	#include &lt;Configurator.h&gt;
	#include &lt;Configuration.hxx&gt;

	
<!-- *************************************************** -->
<!-- HEADERS OF ALL DECLARED CLASSES ******************* -->
<!-- *************************************************** -->
	<xsl:for-each select="/d:design/d:class">
	#include "<xsl:value-of select="fnc:ASClassName(@name)"/>.h"
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
	#include "<xsl:value-of select="fnc:DClassName(@name)"/>.h"
	</xsl:if>
	</xsl:for-each>
	
<!-- *************************************************** -->
<!-- CONFIGURATOR MAIN ********************************* -->
<!-- *************************************************** -->	
	using namespace std;
	
	// HEADERS OF VALIDATOR
	<!-- define all validators -->
	<xsl:for-each select="/d:design/d:class">
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
	<xsl:call-template name="validateClass">
	<xsl:with-param name="onlyHeader">true</xsl:with-param>
	</xsl:call-template>
	</xsl:if>
	</xsl:for-each>
	
	// BODIES OF VALIDATOR
	<xsl:for-each select="/d:design/d:class">
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
	<xsl:call-template name="validateClass">
	<xsl:with-param name="onlyHeader">false</xsl:with-param>
	</xsl:call-template>
	</xsl:if>
	</xsl:for-each>
	
	// ROOT VALIDATOR

	<xsl:for-each select="/d:design/d:root/d:hasobjects[@instantiateUsing='configuration']">
	<xsl:call-template name="validateHasObjects">
	<xsl:with-param name="onlyHeader">false</xsl:with-param>
	<xsl:with-param name="containingClass">Root</xsl:with-param>
	</xsl:call-template>
	</xsl:for-each>
	
	bool validateRoot (Device::DRoot * root)
	{
	<xsl:for-each select="/d:design/d:root/d:hasobjects[@instantiateUsing='configuration']">	
	validateRootHas<xsl:value-of select="@class"/>( root );
	</xsl:for-each>
	return true; /* it is assumed that improper config would throw */
	}
	

<!-- ****************************** -->
<!-- CONFIG VALIDATOR ************* -->
<!-- ****************************** -->
bool validateDeviceTree ()
{
	Device::DRoot *deviceRoot = Device::DRoot::getInstance();
	return validateRoot( deviceRoot );
	
}
	
</xsl:template>



</xsl:transform>

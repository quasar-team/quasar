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

	<xsl:template name="createDeviceLogic">
	<xsl:param name="configItem"/>
	<xsl:param name="parentDevice"/>
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
	
	Device::<xsl:value-of select="fnc:DClassName(@name)"/> *d<xsl:value-of select="fnc:DClassName(@name)"/> = new Device::<xsl:value-of select="fnc:DClassName(@name)"/> (<xsl:value-of select="$configItem"/>
	<xsl:if test="fnc:getCountParentClassesAndRoot(/,@name)=1">
	, <xsl:value-of select="$parentDevice"/>
	</xsl:if>
	);
	a<xsl:value-of select="fnc:ASClassName(@name)"/>-&gt;linkDevice( d<xsl:value-of select="fnc:DClassName(@name)"/> );
	d<xsl:value-of select="fnc:DClassName(@name)"/>-&gt;linkAddressSpace( a<xsl:value-of select="fnc:ASClassName(@name)"/>, a<xsl:value-of select="fnc:ASClassName(@name)"/>-&gt;nodeId().toString().toUtf8() );
	<xsl:value-of select="$parentDevice"/>-&gt;add (  d<xsl:value-of select="fnc:DClassName(@name)"/> );
	
	</xsl:if>
	</xsl:template>

	
	
	<xsl:template name="hasObjects">
	<xsl:param name="parentNodeId"/>
	<xsl:param name="parentDevice"/>
	<xsl:param name="configuration" />
	<xsl:param name="containingClass" />
	<xsl:choose>
	<xsl:when test="@instantiateUsing='configuration'">
	BOOST_FOREACH(const Configuration::<xsl:value-of select="@class"/> &amp; <xsl:value-of select="@class"/>config, <xsl:value-of select="$configuration"/>.<xsl:value-of select="@class"/><xsl:if test="$containingClass=@class">1</xsl:if>())
	<!-- this funny thing with adding 1 above is when a class has hasObjects towards itself- xsdcxx then renames access method with 1 at the end in order not to confuse it with the constructor -->
		{
			<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
			<!-- If contained class doesn't have device logic then there is nothing to add -->
			<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
			Device::<xsl:value-of select="fnc:DClassName($containedClass)"/>* newObject =
			</xsl:if>
			configure<xsl:value-of select="@class"/> (
				<xsl:value-of select="@class"/>config,
				nm,
				<xsl:value-of select="$parentNodeId"/><xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">,
				<xsl:choose>
				<xsl:when test="(fnc:getCountParentClassesAndRoot(/,@class)=1) and (fnc:classHasDeviceLogic(/,$containingClass)='true')">
				<xsl:value-of select="$parentDevice"/>
				</xsl:when>
				<xsl:otherwise>
				/*parent not existing */ 0
				</xsl:otherwise>
				</xsl:choose>
			      </xsl:if>
				);
			
			<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
			<xsl:choose>
			<xsl:when test="fnc:classHasDeviceLogic(/,$containingClass)='true'">
			<xsl:value-of select="$parentDevice"/>-&gt;add( newObject );
			</xsl:when>
			<xsl:otherwise>
			Device::<xsl:value-of select="fnc:DClassName($containedClass)"/>::registerOrphanedObject( newObject );
			</xsl:otherwise>
			</xsl:choose>
			</xsl:if>
			
		}
	</xsl:when>
	<xsl:when test="@instantiateUsing='design'">
	/* Experimental code: instantiating from design. */
	<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
	<xsl:for-each select="d:object">
	{
				Configuration::<xsl:value-of select="$className"/> c ("<xsl:value-of select="@name"/>");
				AddressSpace::<xsl:value-of select="fnc:ASClassName($className)"/> *a<xsl:value-of select="fnc:ASClassName($className)"/> = 
				new AddressSpace::AS<xsl:value-of select="$className"/>(
				<xsl:value-of select="$parentNodeId"/>, 
				nm->getTypeNodeId(AddressSpace::ASInformationModel::<xsl:value-of select="fnc:typeNumericId($className)"/>), 
				nm,
				c);
				<xsl:variable name="className"><xsl:value-of select="$className"/></xsl:variable>
				<xsl:for-each select="/d:design/d:class[@name=$className]">
				<xsl:call-template name="createDeviceLogic">
				<xsl:with-param name="configItem">c</xsl:with-param>
				<xsl:with-param name="parentDevice"><xsl:value-of select="$parentDevice"/></xsl:with-param>
				</xsl:call-template>
				</xsl:for-each>
	}
	</xsl:for-each>
	
	</xsl:when>	
	</xsl:choose>
	</xsl:template>

	<xsl:template name="configureHeader">
	<xsl:param name="class"/>
	<xsl:choose>
	<xsl:when test="fnc:classHasDeviceLogic(/,$class)='true'">Device::<xsl:value-of select="fnc:DClassName($class)"/>*</xsl:when>
	<xsl:otherwise>void</xsl:otherwise> 
	</xsl:choose>
	configure<xsl:value-of select="$class"/>( const Configuration::<xsl:value-of select="$class"/> &amp; config,
					AddressSpace::ASNodeManager *nm,
					UaNodeId parentNodeId
					<xsl:if test="fnc:classHasDeviceLogic(/,$class)='true'">
					,Device::<xsl:value-of select="fnc:Parent_DClassName($class)"/> * parentDevice
					</xsl:if>
		);

	</xsl:template>

	<xsl:template name="configureObject">
	<xsl:param name="class"/>
	//! Called to create every single instance of <xsl:value-of select="$class"/><xsl:text> 
	</xsl:text>
	<xsl:choose>
	<xsl:when test="fnc:classHasDeviceLogic(/,$class)='true'">Device::<xsl:value-of select="fnc:DClassName($class)"/>*</xsl:when>
	<xsl:otherwise>void</xsl:otherwise> 
	</xsl:choose>
	configure<xsl:value-of select="$class"/>( const Configuration::<xsl:value-of select="$class"/> &amp; config,
					AddressSpace::ASNodeManager *nm,
					UaNodeId parentNodeId
					<xsl:if test="fnc:classHasDeviceLogic(/,$class)='true'">
					,Device::<xsl:value-of select="fnc:Parent_DClassName($class)"/> * parentDevice
					</xsl:if>
				
		)
	{
		AddressSpace::<xsl:value-of select="fnc:ASClassName($class)"/> *asItem = 
				new AddressSpace::<xsl:value-of select="fnc:ASClassName($class)"/>(
				parentNodeId, 
				nm->getTypeNodeId(AddressSpace::ASInformationModel::<xsl:value-of select="fnc:typeNumericId($class)"/>), 
				nm, 
				<xsl:value-of select="@class"/>config);

		<xsl:if test="fnc:classHasDeviceLogic(/,$class)='true'">
 		  Device::<xsl:value-of select="fnc:DClassName(@name)"/> *dItem = new Device::<xsl:value-of select="fnc:DClassName(@name)"/> (
		  config,
		  parentDevice
		);
		asItem-&gt;linkDevice( dItem );
		dItem-&gt;linkAddressSpace( asItem, asItem-&gt;nodeId().toString().toUtf8() );
 		
		</xsl:if>
		
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:hasobjects">
		<xsl:call-template name="hasObjects">
		<xsl:with-param name="containingClass"><xsl:value-of select="$class"/></xsl:with-param>
		<xsl:with-param name="parentDevice">dItem</xsl:with-param>
		<xsl:with-param name="parentNodeId">asItem->nodeId()</xsl:with-param>
		<xsl:with-param name="configuration">config</xsl:with-param>
		</xsl:call-template>
		</xsl:for-each>
		
		<xsl:if test="fnc:classHasDeviceLogic(/,$class)='true'">
		return dItem;
		</xsl:if>
	
	}
	</xsl:template>

	
	<xsl:template match="/">

	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToConfigurator.xslt','Piotr Nikiel')"/>
	#include &lt;iostream&gt;
	#include &lt;boost/foreach.hpp&gt;
	
	#include &lt;ASUtils.h&gt;
	#include &lt;ASInformationModel.h&gt;
	#include &lt;ASNodeQueries.h&gt;
	
	#include &lt;DRoot.h&gt;
	
	#include &lt;Configurator.h&gt;
	#include &lt;Configuration.hxx&gt;

	#include &lt;meta.h&gt;

	#include &lt;LogIt.h&gt;
	
<!-- *************************************************** -->
<!-- HEADERS OF ALL DECLARED CLASSES ******************* -->
<!-- *************************************************** -->
	<xsl:for-each select="/d:design/d:class">
	#include &lt;<xsl:value-of select="fnc:ASClassName(@name)"/>.h&gt;
	<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
	#include &lt;<xsl:value-of select="fnc:DClassName(@name)"/>.h&gt;
	</xsl:if>
	</xsl:for-each>
	
	<xsl:for-each select="/d:design/d:class">
	<xsl:variable name="class"><xsl:value-of select="@name"/></xsl:variable>
	<xsl:call-template name="configureHeader">
	<xsl:with-param name="class"><xsl:value-of select="$class"/></xsl:with-param>
	</xsl:call-template>
	</xsl:for-each>
	
	<xsl:for-each select="/d:design/d:class">
	<xsl:variable name="class"><xsl:value-of select="@name"/></xsl:variable>
	<xsl:call-template name="configureObject">
	<xsl:with-param name="class"><xsl:value-of select="$class"/></xsl:with-param>
	</xsl:call-template>
	</xsl:for-each>
	
<!-- *************************************************** -->
<!-- CONFIGURATION DECORATION ************************** -->
<!-- *************************************************** -->	
	bool runConfigurationDecoration(Configuration::Configuration&amp; theConfiguration, ConfigXmlDecoratorFunction&amp; configXmlDecoratorFunction)
	{
		if(configXmlDecoratorFunction.empty()) return true;
		
		if(configXmlDecoratorFunction(theConfiguration))
		{
			return true;
		}
		else
		{
			std::cout &lt;&lt; "Error: device specific configuration decoration failed, check logs for details" &lt;&lt; std::endl;
		}
		return false;
	}	
	
<!-- *************************************************** -->
<!-- CONFIGURATOR MAIN ********************************* -->
<!-- *************************************************** -->	
	bool configure (std::string fileName, AddressSpace::ASNodeManager *nm, ConfigXmlDecoratorFunction configXmlDecoratorFunction)
    {
	
	std::auto_ptr&lt;Configuration::Configuration&gt; theConfiguration;
	
	try
	{
	    theConfiguration = Configuration::configuration(fileName);
	} 
	catch (xsd::cxx::tree::parsing&lt;char&gt; &amp;exception)
	{
        LOG(Log::ERR) &lt;&lt; "Configuration: Failed when trying to open the configuration, with general error message: " &lt;&lt; exception.what();
		BOOST_FOREACH( const xsd::cxx::tree::error&lt;char&gt; &amp;error, exception.diagnostics() )
		{
			LOG(Log::ERR) &lt;&lt; "Configuration: Problem at " &lt;&lt; error.id() &lt;&lt; ":" &lt;&lt; error.line() &lt;&lt; ": " &lt;&lt; error.message();
		}
	    throw std::runtime_error("Configuration: failed to load configuration. The exact problem description should have been logged.");
	}
	
	
	UaNodeId rootNode = UaNodeId(OpcUaId_ObjectsFolder, 0);
	Device::DRoot *deviceRoot = Device::DRoot::getInstance();

	configureMeta( *theConfiguration.get(), nm, rootNode );	
	if(!runConfigurationDecoration(*theConfiguration, configXmlDecoratorFunction)) return false;
	
	<xsl:for-each select="/d:design/d:root/d:hasobjects[@instantiateUsing='configuration']">

        <xsl:call-template name="hasObjects">
        <xsl:with-param name="containingClass">Root</xsl:with-param>
        <xsl:with-param name="parentDevice">deviceRoot</xsl:with-param>
        <xsl:with-param name="parentNodeId">rootNode</xsl:with-param>
        <xsl:with-param name="configuration">(*theConfiguration)</xsl:with-param>
        </xsl:call-template>

	</xsl:for-each>
	
	<xsl:for-each select="/d:design/d:root/d:hasobjects[@instantiateUsing='design']">
	<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
	<xsl:for-each select="d:object">
	{
				Configuration::<xsl:value-of select="$className"/> c ("<xsl:value-of select="@name"/>");
				AddressSpace::<xsl:value-of select="fnc:ASClassName($className)"/> *a<xsl:value-of select="fnc:ASClassName($className)"/> = 
				new AddressSpace::AS<xsl:value-of select="$className"/>(
				rootNode, 
				nm->getTypeNodeId(AddressSpace::ASInformationModel::<xsl:value-of select="fnc:typeNumericId($className)"/>), 
				nm,
				c);
				<xsl:variable name="className"><xsl:value-of select="$className"/></xsl:variable>
				<xsl:for-each select="/d:design/d:class[@name=$className]">
				<xsl:call-template name="createDeviceLogic">
				<xsl:with-param name="configItem">c</xsl:with-param>
				<xsl:with-param name="parentDevice">deviceRoot</xsl:with-param>
				</xsl:call-template>
				</xsl:for-each>
	}
	</xsl:for-each>
	</xsl:for-each>
	return true;
}

<!-- *************************************************** -->
<!-- DECONFIGURATOR ************************************ -->
<!-- *************************************************** -->
	void unlinkAllDevices (AddressSpace::ASNodeManager *nm)
	{
	        unsigned int totalObjectsNumber = 0;
		<xsl:for-each select="/d:design/d:class">
		<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
		{
			std::vector&lt; AddressSpace::<xsl:value-of select="fnc:ASClassName(@name)"/> * &gt; objects;
			std::string pattern (".*");
			AddressSpace::findAllObjectsByPatternInNodeManager&lt;AddressSpace::<xsl:value-of select="fnc:ASClassName(@name)"/>&gt; (nm, pattern, objects);
			totalObjectsNumber += objects.size();
			BOOST_FOREACH(AddressSpace::<xsl:value-of select="fnc:ASClassName(@name)"/> *a, objects)
			{
				a-&gt;unlinkDevice();
			}
		}
		</xsl:if>
		</xsl:for-each>
		LOG(Log::INF) &lt;&lt; "Total number of unlinked objects: " &lt;&lt; totalObjectsNumber;
	}
	
	</xsl:template>



</xsl:transform>

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
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"/>
	<xsl:param name="className"/>
	<xsl:strip-space elements="*"/>



	<xsl:template name="deviceBody">
	 
		void <xsl:value-of select="fnc:Base_DClassName(@name)"/>::linkAddressSpace( AddressSpace::<xsl:value-of select="fnc:ASClassName(@name)"/> *addressSpaceLink, const std::string &amp; stringAddress)
		{
			if (m_addressSpaceLink != 0)
			{
				/* signalize nice error from here. */
				abort();
			}
			else
				m_addressSpaceLink = addressSpaceLink;	
			m_stringAddress.assign( stringAddress );
		}
		
		/* add/remove */
		<xsl:for-each select="d:hasobjects">
		<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
		void <xsl:value-of select="fnc:Base_DClassName($className)"/>::add ( <xsl:value-of select="fnc:DClassName(@class)"/> *device)
		{
			m_<xsl:value-of select="@class"/>s.push_back (device);
		}
		</xsl:if>
		</xsl:for-each>
		
		//! Disconnects AddressSpace part from the Device logic, and does the same for all children
		//! Returns number of unlinked objects including self
		unsigned int <xsl:value-of select="fnc:Base_DClassName($className)"/>::unlinkAllChildren ()
		{
			unsigned int objectCounter=1;  // 1 is for self
			m_addressSpaceLink = 0;
			/* Fill up: call unlinkAllChildren on all children */
			<xsl:for-each select="d:hasobjects">
			<xsl:variable name="childClassName"><xsl:value-of select="@class"/></xsl:variable>
			<xsl:if test="fnc:classHasDeviceLogic(/,$childClassName)='true'">
			BOOST_FOREACH( <xsl:value-of select="fnc:DClassName(@class)"/> *d, m_<xsl:value-of select="@class"/>s )
			{
				objectCounter += d->unlinkAllChildren();
			}
			</xsl:if>
			</xsl:for-each>
			return objectCounter;
			
		}
		
		
		/* find methods for children */
		<xsl:for-each select="d:hasobjects">
		<xsl:variable name="class"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:cachevariable[@isKey='true']">
		
		<xsl:value-of select="fnc:DClassName($class)"/> * <xsl:value-of select="fnc:Base_DClassName($className)"/>::get<xsl:value-of select="$class"/>By<xsl:value-of select="fnc:capFirst(@name)"/> (<xsl:value-of select="fnc:dataTypeToBaseDeviceType(@dataType)"/> key) const
		{
			BOOST_FOREACH( <xsl:value-of select="fnc:DClassName($class)"/> *test, m_<xsl:value-of select="$class"/>s )
			{
				if (test-&gt;<xsl:value-of select="@name"/>() == key)
					return test;
			}
			return 0;
		}
		</xsl:for-each>
		
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:configentry[@isKey='true']">
		
		<xsl:value-of select="fnc:DClassName($class)"/> * <xsl:value-of select="fnc:Base_DClassName($className)"/>::get<xsl:value-of select="$class"/>By<xsl:value-of select="fnc:capFirst(@name)"/> (<xsl:value-of select="fnc:dataTypeToBaseDeviceType(@dataType)"/> key) const
		{
			BOOST_FOREACH( <xsl:value-of select="fnc:DClassName($class)"/> *test, m_<xsl:value-of select="$class"/>s )
			{
				if (test-&gt;<xsl:value-of select="@name"/>() == key)
					return test;
			}
			return 0;
		}
		
		</xsl:for-each><!-- for-each configEntry -->
		
		<xsl:if test="fnc:isHasObjectsSingleton(.)='true'">
		// This function is generated because the hasObjects links to exactly 1 object
		<xsl:value-of select="fnc:DClassName(@class)"/> * <xsl:value-of select="fnc:Base_DClassName($className)"/>::<xsl:value-of select="lower-case(@class)"/>() const
		{
		  if (m_<xsl:value-of select="@class"/>s.size() != 1)
		    throw std::runtime_error( "Configuration error: <xsl:value-of select="$className"/> should have exactly 1 <xsl:value-of select="@class"/> " );
		  return m_<xsl:value-of select="@class"/>s.at(0);
		}
		</xsl:if>
		</xsl:for-each><!-- for-each hasObjects -->
		
		


    
    /* Constructor */
    <xsl:value-of select="fnc:Base_DClassName(@name)"/>::<xsl:value-of select="fnc:Base_DClassName(@name)"/> (
    const Configuration::<xsl:value-of select="@name"/> &amp; config,
		 <xsl:value-of select="fnc:Parent_DClassName($className)"/> * parent
		):

		 	m_parent(parent),
			m_addressSpaceLink(0),
			m_stringAddress("**NB**")	
			<xsl:for-each select="d:cachevariable[@isKey='true']">
			, m_<xsl:value-of select="@name"/> ( config.<xsl:value-of select="@name"/>() )
			</xsl:for-each>
			<xsl:for-each select="d:configentry[@isKey='true' or @storedInDeviceObject='true']">
			, m_<xsl:value-of select="@name"/> ( config.<xsl:value-of select="@name"/>() )
			</xsl:for-each>
            
		{

		}
		
		<xsl:value-of select="fnc:Base_DClassName($className)" />::~<xsl:value-of select="fnc:Base_DClassName($className)" /> ()
		{
			/* remove children */
			<xsl:for-each select="/d:design/d:class[@name=$className]/d:hasobjects">
			<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
			<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
			BOOST_FOREACH( <xsl:value-of select="fnc:DClassName(@class)"/> *d, m_<xsl:value-of select="@class"/>s )
			{
				delete d;
			}
			</xsl:if>
			</xsl:for-each>
		}
    
	 </xsl:template>
	 
	
	 	<xsl:template match="/">	
	 	<xsl:if test="not(/d:design/d:class[@name=$className])">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' not found. If you are running this through generateDevice*.sh scripts, maybe you forgot to put name of the class as a parameter?</xsl:message>
		</xsl:if>
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)!='true'">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' hasnt got device logic</xsl:message>
		</xsl:if>
	 	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToDeviceBaseBody.xslt','Piotr Nikiel')"/>

		
#include &lt;boost/foreach.hpp&gt;		
		
#include &lt;Configuration.hxx&gt;


#include &lt;<xsl:value-of select="fnc:Base_DClassName($className)"/>.h&gt;



	<xsl:for-each select="/d:design/d:class[@name=$className]">
	<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
	<xsl:for-each select="d:hasobjects">
	<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
	<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
	#include &lt;<xsl:value-of select="fnc:DClassName(@class)"/>.h&gt;
	</xsl:if>
	</xsl:for-each>
	</xsl:if>
	</xsl:for-each>



namespace Device
{

	<xsl:for-each select="/d:design/d:class[@name=$className]">
	<xsl:call-template name="deviceBody"/>
	
	std::list&lt;<xsl:value-of select="fnc:DClassName($className)"/>*&gt; Base_<xsl:value-of select="fnc:DClassName($className)"/>::s_orphanedObjects;
	
	</xsl:for-each>
	
	
}


	</xsl:template>
	 
	 
	 
	 
	 
	 
</xsl:transform>

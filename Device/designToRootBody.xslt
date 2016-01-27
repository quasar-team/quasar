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
	<xsl:output method="text"></xsl:output>
	 
	
	 	<xsl:template match="/">
	 	 <xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToRootBody.xslt','Piotr Nikiel')"/>
	 	#include &lt;iostream&gt;
	 	#include &lt;boost/foreach.hpp&gt;
		#include &lt;DRoot.h&gt;
		#include &lt;ASUtils.h&gt;
		


        <xsl:for-each select="/d:design/d:class">
        <xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
        <xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
        #include &lt;<xsl:value-of select="fnc:DClassName($className)"/>.h&gt;
        </xsl:if>
        </xsl:for-each>
		
		namespace Device
		{
		
		DRoot::DRoot() 
		{
			if (m_instance != 0)
				abort(); /* Logic error inside program. DRoot can be instantiated just once */
			m_instance = this;
		}

		DRoot::~DRoot()
		{
			<xsl:for-each select="/d:design/d:root/d:hasobjects">
			<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
			<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
			for (auto it = m_children<xsl:value-of select="@class"/>.begin(); it!=m_children<xsl:value-of select="@class"/>.end();it++)
				delete (*it);
			m_children<xsl:value-of select="@class"/>.clear();
			</xsl:if>
			</xsl:for-each>
			<xsl:for-each select="/d:design/d:class">
            <xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
             {
             auto objects = <xsl:value-of select="fnc:Base_DClassName(@name)"/>::orphanedObjects ();
             BOOST_FOREACH( <xsl:value-of select="fnc:DClassName(@name)"/>* object, objects )
             {
                  delete object;
                  
             }
             objects.clear();
        
             }
            </xsl:if>
            </xsl:for-each>
		}

		DRoot* DRoot::getInstance()
		{
			if (m_instance == 0)
			{
				ABORT_MESSAGE ("DRoot: instance not yet created. ");
			}
			return m_instance;
		}

		void DRoot::unlinkAllChildren () const
		{
			unsigned int objectCounter = 0;
			<xsl:for-each select="/d:design/d:root/d:hasobjects">
			<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
			<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
			for (auto it = m_children<xsl:value-of select="@class"/>.begin(); it!=m_children<xsl:value-of select="@class"/>.end();it++)
				objectCounter += (*it)->unlinkAllChildren();
			</xsl:if>
			</xsl:for-each>
			<xsl:for-each select="/d:design/d:class">
			<xsl:if test="fnc:classHasDeviceLogic(/,@name)='true'">
			 {
			 auto objects = <xsl:value-of select="fnc:DClassName(@name)"/>::orphanedObjects ();
			 BOOST_FOREACH( <xsl:value-of select="fnc:DClassName(@name)"/>* object, objects )
			 {
			      objectCounter += object->unlinkAllChildren();
			 }
		
			 }
			</xsl:if>
			</xsl:for-each>
			std::cout &lt;&lt; "DRoot::unlinkAllChildren(): " &lt;&lt; objectCounter &lt;&lt; " objects unlinked " &lt;&lt; std::endl;
		}
		
		/* find methods for children */
		<xsl:for-each select="/d:design/d:root/d:hasobjects">
		<xsl:variable name="class"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:cachevariable[@isKey='true']">
		
		<xsl:value-of select="fnc:DClassName($class)"/> * DRoot::get<xsl:value-of select="$class"/>By<xsl:value-of select="fnc:capFirst(@name)"/> (
		<xsl:choose>
		<xsl:when test="@dataType='UaString'">std::string </xsl:when>
		<xsl:otherwise><xsl:value-of select="@dataType"/></xsl:otherwise>
		</xsl:choose>
		key) const
		{
			BOOST_FOREACH( <xsl:value-of select="fnc:DClassName($class)"/> *test, m_children<xsl:value-of select="$class"/> )
			{
				if (test-&gt;<xsl:value-of select="@name"/>() == key)
					return test;
			}
			return 0;
		}
		</xsl:for-each>
		</xsl:for-each>
		
		/* Singleton's instance. */
		DRoot* DRoot::m_instance = 0;
		
		
	 	}
	 	

	</xsl:template>
	 
	 
	 	 
	 
</xsl:transform>

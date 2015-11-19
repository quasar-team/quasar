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
<!-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 -->
<!-- GNU Lesser General Public Licence for more details.                           -->
<!--                                                                               -->
<!-- You should have received a copy of the GNU Lesser General Public License      -->
<!-- along with Quasar.  If not, see <http://www.gnu.org/licenses/>                -->
<!--                                                                               -->
<!-- Created:      Jun 2014                                                        -->
<!-- Authors:                                                                      -->

<xsl:transform version="2.0"
	xmlns:xml="http://www.w3.org/XML/1998/namespace" xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:d="http://cern.ch/quasar/Design" xmlns:fnc="http://cern.ch/quasar/MyFunctions"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"></xsl:output>


	<xsl:template match="/">
	 <xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToRootHeader.xslt','Piotr Nikiel')"/>
	#ifndef DROOT_H_
	#define DROOT_H_
	

	#include &lt;opcua_platformdefs.h&gt;
	
	#include &lt;vector&gt;
	namespace Device
	{
	
	<xsl:for-each select="/d:design/d:root/d:hasobjects">
	<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
	<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
	class <xsl:value-of select="fnc:DClassName(@class)"/>;
	</xsl:if>
	</xsl:for-each>
	
	class DRoot
	{
	public:
		/* yes, its a singleton */
		static DRoot * getInstance ();
		
		DRoot ();
		virtual ~DRoot ();
		
		/* To gracefully quit */
		void unlinkAllChildren () const;
		
		/* For constructing the tree of devices and for browsing children. */
		<xsl:for-each select="/d:design/d:root/d:hasobjects">
		<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
		void add (<xsl:value-of select="fnc:DClassName(@class)"/> *d) { m_children<xsl:value-of select="@class"/>.push_back (d); }
		const std::vector&lt;<xsl:value-of select="fnc:DClassName(@class)"/> * &gt; &amp; <xsl:value-of select="lower-case(@class)"/>s () const { return m_children<xsl:value-of select="@class"/>; }
		</xsl:if> 
		</xsl:for-each>
		
		
		/* find methods for children */
		<xsl:for-each select="/d:design/d:root/d:hasobjects">
		<xsl:variable name="class"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:cachevariable[@isKey='true']">
		
		<xsl:value-of select="fnc:DClassName($class)"/> * get<xsl:value-of select="$class"/>By<xsl:value-of select="fnc:capFirst(@name)"/> (
		<xsl:choose>
		<xsl:when test="@dataType='UaString'">std::string </xsl:when>
		<xsl:otherwise><xsl:value-of select="@dataType"/></xsl:otherwise>
		</xsl:choose>
		 key) const;
		
		</xsl:for-each>
		</xsl:for-each>
		
		/* Here you can put your custom code, e.g. for browsing objects which are children of root elements */
		
	private:
		static DRoot *m_instance; /* it's a singleton class */
		
		// disable copy
		DRoot (const DRoot &amp;);
		DRoot&amp; operator= (const DRoot &amp;);
		
		/* Pointers to children */
		<xsl:for-each select="/d:design/d:root/d:hasobjects">
		<xsl:variable name="className"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)='true'">
		std::vector&lt;<xsl:value-of select="fnc:DClassName(@class)"/> *&gt; m_children<xsl:value-of select="@class"/>;
		</xsl:if>
		</xsl:for-each>
			
	};

	}
	#endif // include guard
	</xsl:template>






</xsl:transform>

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
	<xsl:include href="CommonFunctions.xslt" />
	<xsl:param name="detailLevel"/>



	
	<xsl:template name="NodesClass">

	
	<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
	class_<xsl:value-of select="@name"/> [shape=rect,margin=0,label=&lt;
		&lt;table border="1" cellspacing="0" &gt;
	
		&lt;tr&gt; 
		&lt;td align="center" border="1" &gt;
		&lt;b&gt; 
		
		&lt;font point-size="18" &gt;
		<xsl:value-of select="@name"/>
		&lt;/font&gt;
		<xsl:if test="$detailLevel>=2">
		&lt;font point-size="10" color="blue" &gt;
        DL=<xsl:value-of select="count(d:devicelogic)"/>
        &lt;/font&gt;
		</xsl:if>
		
		&lt;/b&gt; 
		&lt;/td&gt;  
		&lt;/tr&gt;
		
				&lt;tr&gt; 
		&lt;td align="center" border="1" &gt;

	<xsl:if test="count(d:cachevariable)+count(d:sourcevariable)+count(d:configentry)>0">

		&lt;table border="0"&gt; 		
	<xsl:for-each select="d:cachevariable">
		&lt;tr&gt;
		<xsl:if test="$detailLevel>=1">
		&lt;td align="left" &gt;
		<xsl:choose>
		<xsl:when test="@isKey='true'">
		&lt;font point-size="10" color="blue" &gt;CV,K&lt;/font&gt;
		</xsl:when>
		<xsl:otherwise>
		&lt;font point-size="10" color="blue" &gt;CV&lt;/font&gt;
		</xsl:otherwise>
		</xsl:choose>
		&lt;/td&gt;
		</xsl:if>
		&lt;td align="left" &gt;
		<xsl:value-of select="@name"/>
		&lt;/td&gt;
		&lt;td align="left" &gt;
		: <xsl:value-of select="@dataType"/>
		&lt;/td&gt;
		&lt;/tr&gt;
	</xsl:for-each>
	<xsl:for-each select="d:sourcevariable">
		&lt;tr&gt;
		<xsl:if test="$detailLevel>=1">
		&lt;td align="left" &gt;
		&lt;font point-size="10" color="darkgreen" &gt;SV&lt;/font&gt;
		&lt;/td&gt;
		</xsl:if>
		&lt;td align="left" &gt;
		<xsl:value-of select="@name"/>
		&lt;/td&gt;
		&lt;td align="left" &gt;
		: <xsl:value-of select="@dataType"/>
		&lt;/td&gt;
		&lt;/tr&gt;
	</xsl:for-each>
	<xsl:for-each select="d:configentry">
		&lt;tr&gt;
		<xsl:if test="$detailLevel>=1">
		&lt;td align="left" &gt;
		<xsl:choose>
		<xsl:when test="@isKey='true'">
		&lt;font point-size="10" color="red" &gt;CE,K&lt;/font&gt;
		</xsl:when>
		<xsl:otherwise>
		&lt;font point-size="10" color="red" &gt;CE&lt;/font&gt;
		</xsl:otherwise>
		</xsl:choose>
		&lt;/td&gt;
		</xsl:if>
		&lt;td align="left" &gt;
		<xsl:value-of select="@name"/>
		&lt;/td&gt;
		&lt;td align="left" &gt;
		: <xsl:value-of select="@dataType"/>
		&lt;/td&gt;
		&lt;/tr&gt;
	</xsl:for-each>
		&lt;/table&gt;  
		</xsl:if>

				&lt;/td&gt;  
		&lt;/tr&gt;
		
		&lt;/table&gt;&gt;]
	

	
	</xsl:template>
	
	<xsl:template name="EdgesHasObjects">
	<xsl:param name="parentClassName"/>
	<xsl:variable name="cardinality">
	<xsl:choose>
	<xsl:when test="@minOccurs='1' and @maxOccurs='1'">1</xsl:when>
	<xsl:otherwise><xsl:choose><xsl:when test="@minOccurs"><xsl:value-of select="@minOccurs"/></xsl:when><xsl:otherwise>0</xsl:otherwise></xsl:choose>..<xsl:choose><xsl:when test="@maxOccurs"><xsl:value-of select="@maxOccurs"/></xsl:when><xsl:otherwise>*</xsl:otherwise></xsl:choose></xsl:otherwise>
	</xsl:choose>
	</xsl:variable>
	
	class_<xsl:value-of select="$parentClassName"/>
	-&gt; 
	<xsl:choose>
	<xsl:when test="@instantiateUsing='configuration'">
	class_<xsl:value-of select="@class"/> [arrowtail=odiamond,dir=back,label="<xsl:value-of select="$cardinality"/>"]
	</xsl:when>
	<xsl:when test="@instantiateUsing='design'">
	class_<xsl:value-of select="@class"/> [arrowtail=odiamond,dir=back,label="<xsl:value-of select="count(d:object)"/>,nc"]
	</xsl:when>
	
	</xsl:choose>
	</xsl:template>
	
	<xsl:template match="/">
	
	digraph  {
		graph [fontname="Arial"]
		<xsl:for-each select="/d:design/d:class">
		<xsl:call-template name="NodesClass"/>
		</xsl:for-each>
		
		<!-- all classes drawn, now	their hasobjects can be drawn too -->
		
		<xsl:for-each select="/d:design/d:class">
		<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
		
		<xsl:for-each select="d:hasobjects">
		<xsl:call-template name="EdgesHasObjects">
		<xsl:with-param name="parentClassName"><xsl:value-of select="$className"/></xsl:with-param>
		</xsl:call-template>
		</xsl:for-each>
		
		</xsl:for-each>


		class_ROOT [label="ROOT"]
		<xsl:for-each select="/d:design/d:root/d:hasobjects">
		<xsl:call-template name="EdgesHasObjects">
		<xsl:with-param name="parentClassName">ROOT</xsl:with-param>
		</xsl:call-template>
		</xsl:for-each>
	}
	
	</xsl:template>



</xsl:transform>

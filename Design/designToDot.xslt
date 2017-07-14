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

	<xsl:function name="fnc:simplifyQuasarType">
	  <xsl:param name="quasarTypeName"/><xsl:choose><xsl:when test="not(contains($quasarTypeName,'OpcUa'))"><xsl:value-of select="$quasarTypeName"/></xsl:when><xsl:otherwise><xsl:value-of select="substring-after($quasarTypeName,'OpcUa_')"/></xsl:otherwise></xsl:choose></xsl:function>

	<xsl:function name="fnc:getArrowForSourceVariable">
	  <xsl:param name="this"/><xsl:choose><xsl:when test="$this/@addressSpaceRead!='forbidden' and $this/@addressSpaceWrite!='forbidden'">&#8596;</xsl:when><xsl:when test="$this/@addressSpaceRead='forbidden' and $this/@addressSpaceWrite!='forbidden'">&#8594;</xsl:when><xsl:when test="$this/@addressSpaceRead!='forbidden' and $this/@addressSpaceWrite='forbidden'">&#8592;</xsl:when><xsl:otherwise>&#9949;</xsl:otherwise></xsl:choose></xsl:function>

	<xsl:function name="fnc:getArrowForCacheVariable">
	  <xsl:param name="this"/><xsl:choose><xsl:when test="$this/@addressSpaceWrite='forbidden'">&#8592;</xsl:when><xsl:otherwise>&#8596;</xsl:otherwise></xsl:choose></xsl:function>


	<xsl:template name="NodesClass">

	
	<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
	class_<xsl:value-of select="@name"/> [shape=none,margin=0,label=&lt;
		&lt;table border="1" cellspacing="0" cellpadding="2" cellborder="0" &gt;
			&lt;tr &gt; 
			        &lt;td width="0"&gt;&lt;/td&gt;
				&lt;td width="0"&gt;&lt;/td&gt;
				&lt;td colspan="2"&gt;
					&lt;b&gt;
					&lt;font point-size="18" &gt;<xsl:value-of select="@name"/>&lt;/font&gt;
					<xsl:if test="$detailLevel>=2">
						&lt;font point-size="10" color="blue" &gt;DL=<xsl:value-of select="count(d:devicelogic)"/>&lt;/font&gt;
					</xsl:if>
					&lt;/b&gt; 
				&lt;/td&gt;  
			&lt;/tr&gt;
			
			&lt;hr/&gt;

			<xsl:choose >
			  <xsl:when test="count(d:cachevariable)+count(d:sourcevariable)+count(d:configentry)=0">
			    &lt;tr&gt; &lt;td colspan="3"&gt; &lt;/td&gt; &lt;/tr&gt;
			  </xsl:when>
			  <xsl:otherwise>

	 		<xsl:for-each select="d:cachevariable">
				&lt;tr&gt;
				<!-- don't indent the lines below: you will break the graphical layout -->
				&lt;td align="left" width="0"&gt;<xsl:if test="$detailLevel>=3">&lt;font point-size="16"&gt;<xsl:value-of select="fnc:getArrowForCacheVariable(.)"/>&lt;/font&gt;</xsl:if>&lt;/td&gt;
				&lt;td align="left" width="0"&gt;<xsl:if test="$detailLevel>=1">&lt;font point-size="10" color="blue" &gt;CV<xsl:if test="@isKey='true'">,K</xsl:if>&lt;/font&gt;</xsl:if>&lt;/td&gt;
				&lt;td align="left" &gt;<xsl:value-of select="@name"/>&lt;/td&gt;
				&lt;td align="left" &gt;: <xsl:value-of select="fnc:simplifyQuasarType(@dataType)"/>&lt;/td&gt;
				&lt;/tr&gt;
			</xsl:for-each>  	
			
			<xsl:for-each select="d:sourcevariable">
				&lt;tr&gt;
				<!-- don't indent the lines below: you will break the graphical layout -->
				&lt;td align="left" width="0"&gt;<xsl:if test="$detailLevel>=3">&lt;font point-size="16"&gt;<xsl:value-of select="fnc:getArrowForSourceVariable(.)"/>&lt;/font&gt;</xsl:if>&lt;/td&gt;
				&lt;td align="left"&gt;<xsl:if test="$detailLevel>=1">&lt;font point-size="10" color="darkgreen" &gt;SV&lt;/font&gt;</xsl:if>&lt;/td&gt;
				&lt;td align="left" &gt;<xsl:value-of select="@name"/>&lt;/td&gt;
				&lt;td align="left" &gt;: <xsl:value-of select="fnc:simplifyQuasarType(@dataType)"/>&lt;/td&gt;
				&lt;/tr&gt;
			</xsl:for-each>

			<xsl:for-each select="d:configentry">
				&lt;tr&gt;
				<!-- don't indent the lines below: you will break the graphical layout -->
				&lt;td align="left" width="0"&gt;&lt;/td&gt;
				&lt;td align="left"&gt;<xsl:if test="$detailLevel>=1">&lt;font point-size="10" color="red" &gt;&#x2008;CE<xsl:if test="@isKey='true'">,K</xsl:if>&lt;/font&gt;</xsl:if>&lt;/td&gt;
				&lt;td align="left" &gt;<xsl:value-of select="@name"/>&lt;/td&gt;
				&lt;td align="left" &gt;: <xsl:value-of select="fnc:simplifyQuasarType(@dataType)"/>&lt;/td&gt;
				&lt;/tr&gt;
			</xsl:for-each>
			</xsl:otherwise>
			</xsl:choose>
			
			<xsl:choose >
			  <xsl:when test="count(d:method)=0">
			    <!-- don't draw the bar in case no methods -->
			    <!-- &lt;tr&gt; &lt;td colspan="3" height="0"&gt;&lt;/td&gt;&lt;/tr&gt; -->
			  </xsl:when>
			  <xsl:otherwise>
			    &lt;hr/&gt;
	   		    <xsl:for-each select="d:method">
			      <!-- don't indent the lines below: you will break the graphical layout -->
			      &lt;tr&gt;
			      &lt;td align="left" width="4"&gt;<xsl:if test="$detailLevel>=1">&lt;font point-size="10" color="blue" &gt;M&lt;/font&gt;</xsl:if>&lt;/td&gt;
			      &lt;td align="left"&gt;<xsl:value-of select="@name"/>( <xsl:for-each select="d:argument"><xsl:value-of select="fnc:simplifyQuasarType(@dataType)"/><xsl:if test="position() &lt; count(../d:argument)">,</xsl:if></xsl:for-each> )&lt;/td&gt;
			      &lt;td align="left" &gt;: <xsl:choose><xsl:when test="count(d:returnvalue) > 0"><xsl:for-each select="d:returnvalue"><xsl:value-of select="fnc:simplifyQuasarType(@dataType)"/><xsl:if test="position() &lt; count(../d:returnvalue)">,</xsl:if></xsl:for-each></xsl:when><xsl:otherwise>void</xsl:otherwise></xsl:choose>&lt;/td&gt;  
			      &lt;/tr&gt;	
			    </xsl:for-each> 			    
			  </xsl:otherwise>
			</xsl:choose>

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

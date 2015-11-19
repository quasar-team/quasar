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
<!-- Created:   Jan 2015                                                           -->
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
<xsl:param name="remove_makeSetGet" required="no"/>
<xsl:param name="add_nullPolicy" required="no"/>
<xsl:param name="convert_to_hasDeviceLogic" required="no"/>

  <!-- Identity transform. -->
  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>


	<xsl:template match="d:cachevariable">
	<xsl:copy>
		<xsl:for-each select="@*">
		<xsl:if test="$remove_makeSetGet='' or ($remove_makeSetGet='yes' and name()!='makeSetGet')">
				<xsl:copy>
				    <xsl:apply-templates select="@*|node()"/>
				</xsl:copy>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:if test="$add_nullPolicy!='' and not(@nullPolicy)">
			<xsl:attribute name="nullPolicy"><xsl:value-of select="$add_nullPolicy"/></xsl:attribute>
		</xsl:if>
		<xsl:apply-templates select="node()"/>
	</xsl:copy> 

	</xsl:template> 



	<xsl:template match="d:class">
	<xsl:variable name="className"><xsl:value-of select="@name"/></xsl:variable>
		<xsl:copy>
			<xsl:for-each select="@*">
				<xsl:choose>
					<xsl:when
						test="$convert_to_hasDeviceLogic='yes' and name()='deviceLogicTypeName'">
						<xsl:if test="current()!=fnc:DClassName($className)">
						  <xsl:message terminate="yes">Device Logic class name declared in deviceLogicTypeName of class '<xsl:value-of select="$className"/>' attribute is not standard ('<xsl:value-of select="current()"/>'). Can't convert automatically.</xsl:message>
						</xsl:if>
						<xsl:element name="d:devicelogic">
						  <xsl:if test="/d:design/d:class[@name=$className]/@deviceLogicHasMutex">
							  <xsl:element name="d:mutex">
							  </xsl:element>
						  </xsl:if>
						</xsl:element>
					</xsl:when>
					<xsl:when test="$convert_to_hasDeviceLogic='yes' and name()='deviceLogicHasMutex'">
					  <!-- get rid of deviceLogicHasMutex which is now deprecated -->
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates select="." />
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
			<xsl:apply-templates></xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	

	<xsl:template match="/">
	<!-- do some checks on input params -->
	<xsl:if test="$remove_makeSetGet!='' and $remove_makeSetGet!='yes'">
		<xsl:message terminate="yes">Param 'remove_makeSetGet' can be either empty or 'yes'; no other values allowed. [given value is <xsl:value-of select="$remove_makeSetGet"/>]</xsl:message>
	</xsl:if>
	
	<xsl:if test="$add_nullPolicy!='' and $add_nullPolicy!='nullAllowed' and $add_nullPolicy!='nullForbidden'">
		<xsl:message terminate="yes">Param 'add_nullPolicy' can be either empty or 'nullAllowed' or 'nullForbidden'; no other values allowed. [given value is <xsl:value-of select="$add_nullPolicy"/>]</xsl:message>
	</xsl:if>
	
	<xsl:if test="$convert_to_hasDeviceLogic!='' and $convert_to_hasDeviceLogic!='yes'">
	   <xsl:message terminate="yes">Param 'convert_to_hasDeviceLogic' can be either empty or 'yes'; no other values allowed. [given value is <xsl:value-of select="$remove_makeSetGet"/>]</xsl:message>
	</xsl:if>
	
	<xsl:apply-templates/>
	</xsl:template>

</xsl:transform>

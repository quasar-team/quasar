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

<!--
This XSLT is not used to generate any useful content.
Its sole purpose is to validate the design file as a 2nd line of defence, that is, to find
errors which weren't possible to be found using schema-conformance check (or were very difficult to be done that way).

The validation is successful when the XSLT processor finishes without error.
The output is not relevant.

If the processor finishes with error (most likely due to a message with terminate="yes") then the validation wasn't successful.
 
 -->
<xsl:transform version="2.0" xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
<xsl:include href="../Design/CommonFunctions.xslt" />

<xsl:template name="validateArraySize">
<xsl:param name="className"/>
<xsl:param name="variableName"/>
	<xsl:if test="d:array/@minimumSize and d:array/@maximumSize">
		<xsl:if test="d:array/@minimumSize > d:array/@maximumSize">
			<xsl:message terminate="yes">ERROR (at class=<xsl:value-of select="$className"/> variable=<xsl:value-of select="$variableName"/>) Both minimumSize and maximumSize given but minimumSize is larger than maximumSize. </xsl:message>
		</xsl:if>
	</xsl:if>
</xsl:template>

<xsl:template name="cachevariable">
<xsl:param name="className"/>
	<xsl:if test="@nullPolicy!='nullAllowed' and @initializeWith!='configuration' and not(@initialValue)">
		<xsl:message terminate="yes">ERROR (at class='<xsl:value-of select="$className"/>' variable='<xsl:value-of select="@name"/>'): these settings are invalid. When nullPolicy doesn't allow NULL, for valueAndStatus initializer you have to have initialValue attribute. </xsl:message>
	</xsl:if>
	<xsl:if test="@dataType='UaVariant' and @initialValue">
		<xsl:message terminate="yes">ERROR (at class='<xsl:value-of select="$className"/>' variable='<xsl:value-of select="@name"/>'): these settings are invalid. You can't specify initialValue for UaVariant dataType (reason: quasar won't be able to deduce type). </xsl:message>
	</xsl:if>
	<xsl:if test="d:array">
		<xsl:call-template name="validateArraySize">
		<xsl:with-param name="className"><xsl:value-of select="$className"/></xsl:with-param>
		<xsl:with-param name="variableName"><xsl:value-of select="@name"/></xsl:with-param>
		</xsl:call-template>
	</xsl:if>
    <xsl:if test="@isKey">
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)!='true'">
				<xsl:message terminate="yes">ERROR: Class '<xsl:value-of select="$className"/>' hasnt got device logic - isKey can't be used</xsl:message>
		</xsl:if>
    </xsl:if>
    <xsl:if test="d:array and @isKey">
        <xsl:message terminate="yes">ERROR: There's no support for isKey for arrays. Fix your design.</xsl:message>
    </xsl:if>
</xsl:template>

<xsl:template match="d:class">
	<xsl:variable name="className">
		<xsl:value-of select="@name" />
	</xsl:variable>
	<xsl:if test="@singleVariableNode='true' and (count(d:cachevariable)+count(d:sourcevariable)!=1)">
<xsl:message terminate="yes">ERROR (at class='<xsl:value-of select="$className"/>' When singleVariableNode attribute is set to true, the class requires exactly one variable (cachevariable or sourcevariable).</xsl:message>
	</xsl:if>
	<xsl:if test="@singleVariableNode='true' and (count(d:hasobjects)!=0)">
<xsl:message terminate="yes">ERROR (at class='<xsl:value-of select="$className"/>' When singleVariableNode attribute is set to true, the class must not have any children (=d:hasobjects).</xsl:message>

	</xsl:if>
	<xsl:for-each select="d:cachevariable">
		<xsl:call-template name="cachevariable">
		<xsl:with-param name="className"><xsl:value-of select="$className"/></xsl:with-param>
		</xsl:call-template>
	</xsl:for-each>
    
    <xsl:for-each select="d:configentry[@defaultValue]">
        <xsl:if test="d:array">
            <xsl:message terminate="yes">
                ERROR (at class='<xsl:value-of select="$className"/>' configentry='<xsl:value-of select="@name"/>'):
                defaultValue can not be used for array config-entries.
            </xsl:message>
        </xsl:if>
    </xsl:for-each>
    
</xsl:template>


<xsl:template match="/">
<xsl:apply-templates/>
</xsl:template>


</xsl:transform>

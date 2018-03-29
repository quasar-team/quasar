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
<!-- Created:   Apr 2015                                                           -->
<!-- Authors:                                                                      -->
<!--   Piotr Nikiel <piotr@nikiel.info>                                            -->

<xsl:transform version="2.0"
xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:output method="text"></xsl:output>
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:param name="projectBinaryDir"/>
	
	<xsl:template name="commands">

	add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/AddressSpace/include/<xsl:value-of select="fnc:ASClassName(@name)"/>.h ${PROJECT_BINARY_DIR}/AddressSpace/src/<xsl:value-of select="fnc:ASClassName(@name)"/>.cpp 
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND python quasar.py generate asclass <xsl:value-of select="@name"/> --project_binary_dir <xsl:value-of select="$projectBinaryDir"/> 
	DEPENDS ${DESIGN_FILE} ${PROJECT_SOURCE_DIR}/AddressSpace/designToClassHeader.xslt ${PROJECT_SOURCE_DIR}/AddressSpace/designToClassBody.xslt Configuration.hxx validateDesign
	)	
	
	</xsl:template>

	
	<xsl:template match="/">
	<xsl:for-each select="/d:design/d:class">
	<xsl:call-template name="commands"/>
	</xsl:for-each>
	
	set(ADDRESSSPACE_CLASSES 
	<xsl:for-each select="/d:design/d:class">
	<xsl:value-of select="$projectBinaryDir"/>/AddressSpace/src/<xsl:value-of select="fnc:ASClassName(@name)"/>.cpp
	</xsl:for-each>
	)
	
	set(ADDRESSSPACE_HEADERS
	<xsl:for-each select="/d:design/d:class">
	<xsl:value-of select="$projectBinaryDir"/>/AddressSpace/include/<xsl:value-of select="fnc:ASClassName(@name)"/>.h
	</xsl:for-each>
	)
	

	
	</xsl:template>
</xsl:transform>

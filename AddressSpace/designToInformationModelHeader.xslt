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
<!-- Created:   Jun 2014                                                           -->
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
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"/>
	<xsl:param name="className"/>

	<xsl:template match="/">
	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToInformationModelHeader.xslt','Piotr Nikiel')"/>	
	
#ifndef __ASINFORMATIONMODEL__H__
#define __ASINFORMATIONMODEL__H__	
	
#include &lt;ASNodeManager.h&gt;	
	
namespace AddressSpace
{
	
	
	class ASInformationModel
	{
	public:

	enum
	{
	AS_TYPE_STANDARDMETADATA = 1000,
	AS_TYPE_LOG,
	AS_TYPE_GENERALLOGLEVEL,
	AS_TYPE_SOURCEVARIABLESTHREADPOOL,	
	AS_TYPE_COMPONENTLOGLEVEL,
	AS_TYPE_COMPONENTLOGLEVELS,
	AS_TYPE_QUASAR,
	AS_TYPE_SERVER
	<xsl:for-each select="/d:design/d:class">
		, <xsl:value-of select="fnc:typeNumericId(@name)"/>
	</xsl:for-each>
	};
	
	static void createNodesOfTypes (ASNodeManager *nm);
	};
}

#endif // __ASINFORMATIONMODEL__H__

	</xsl:template>
	


</xsl:transform>

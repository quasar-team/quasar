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

<xsl:transform version="2.0" xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/MyFunctions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"></xsl:output>
	 <xsl:param name="className"/>
	
	<xsl:template match="/">	
	<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToInformationModelBody.xslt','Piotr Nikiel')"/>    
	#include &lt;iostream&gt;
	#include &lt;ASUtils.h&gt;
	#include &lt;ASInformationModel.h&gt;
	

	namespace AddressSpace
	{
	
	void ASInformationModel::createNodesOfTypes (ASNodeManager *nm)
	{
	#ifndef BACKEND_OPEN62541

		UaObjectTypeSimple * type;
		UaStatus addStatus;
		<xsl:for-each select="/d:design/d:class">
		type = new UaObjectTypeSimple ("<xsl:value-of select="@name"/>", UaNodeId(<xsl:value-of select="fnc:typeNumericId(@name)"/>, nm->getNameSpaceIndex()), /* locale id*/UaString(""), /*is abstract*/OpcUa_False);
		addStatus = nm->addNodeAndReference(OpcUaId_BaseObjectType, type, OpcUaId_HasSubtype);
		if (!addStatus.isGood())
		{
			std::cout &lt;&lt; "While adding a type definition node: ";
			ASSERT_GOOD(addStatus);
		}
		</xsl:for-each>
	#endif // BACKEND_OPEN62541
	}

	
	}
	
	
	</xsl:template>



</xsl:transform>

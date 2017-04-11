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

<xsl:transform version="2.0"
	xmlns:xml="http://www.w3.org/XML/1998/namespace" xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:d="http://cern.ch/quasar/Design" xmlns:fnc="http://cern.ch/quasar/MyFunctions"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
	<xsl:include href="../Design/CommonFunctions.xslt" />
	<xsl:output method="text"></xsl:output>
	<xsl:param name="className" />

	<xsl:template name="cachevariables_delegates">
		<xsl:if test="@addressSpaceWrite='delegated'">
			/* Note: never directly call this function. */

			
			UaStatus write<xsl:value-of select="fnc:capFirst(@name)"/> ( const <xsl:value-of select="@dataType"/> &amp; v);
		</xsl:if>
	</xsl:template>

	<xsl:template name="sourcevariables_operations">
		<xsl:if test="@addressSpaceRead='asynchronous' or @addressSpaceRead='synchronous' ">
			/* ASYNCHRONOUS !! */
			UaStatus read<xsl:value-of select="fnc:capFirst(@name)"/> (
			<xsl:value-of select="@dataType"/> &amp;value,
			UaDateTime &amp;sourceTime
			);
		</xsl:if>
		
		<xsl:if test="@addressSpaceWrite='asynchronous' or @addressSpaceWrite='synchronous'">
			/* ASYNCHRONOUS !! */
			UaStatus write<xsl:value-of select="fnc:capFirst(@name)"/> (
			<xsl:value-of select="@dataType"/> &amp;value
			);
		</xsl:if>
	</xsl:template>
	
	<xsl:template name="method_handler">
		UaStatus call<xsl:value-of select="fnc:capFirst(@name)"/> ( 
		<xsl:for-each select="d:argument">
			<xsl:value-of select="fnc:fixDataTypePassingMethod(@dataType)"/><xsl:text> </xsl:text><xsl:value-of select="@name"/><xsl:if test="position() &lt; (count(../d:argument)+count(../d:returnvalue))">,</xsl:if><xsl:text>
			</xsl:text>
		</xsl:for-each>
		<xsl:for-each select="d:returnvalue">
			<xsl:value-of select="@dataType"/> &amp; <xsl:value-of select="@name"/><xsl:if test="position() &lt; count(../d:returnvalue)">,
			</xsl:if>
		</xsl:for-each>
		) ;
	</xsl:template>

	<xsl:template name="deviceHeader">

		class
		<xsl:value-of select="fnc:DClassName($className)" />
		: public <xsl:value-of select="fnc:Base_DClassName($className)" />
		{

		public:
		/* sample constructor */
		explicit <xsl:value-of select="fnc:DClassName($className)" /> (
		const Configuration::<xsl:value-of select="@name"/> &amp; config, 
		 <xsl:value-of select="fnc:Parent_DClassName($className)"/> * parent
		) ;
		/* sample dtr */
		~<xsl:value-of select="fnc:DClassName($className)" /> ();


		

		/* delegators for
		cachevariables and sourcevariables */
		<xsl:for-each select="d:cachevariable">
			<xsl:call-template name="cachevariables_delegates" />
		</xsl:for-each>
		
		<xsl:for-each select="d:sourcevariable">
			<xsl:call-template name="sourcevariables_operations"/>
		</xsl:for-each>
		
		/* delegators for methods */
		<xsl:for-each select="d:method">
			<xsl:call-template name="method_handler"/>
		</xsl:for-each>
		
		private:
		/* Delete copy constructor and assignment operator */
		<xsl:value-of select="fnc:DClassName($className)" />( const <xsl:value-of select="fnc:DClassName($className)" /> &amp; );
		<xsl:value-of select="fnc:DClassName($className)" />&amp; operator=(const <xsl:value-of select="fnc:DClassName($className)" /> &amp;other);
		
	// ----------------------------------------------------------------------- *
	// -     CUSTOM CODE STARTS BELOW THIS COMMENT.                            *
	// -     Don't change this comment, otherwise merge tool may be troubled.  *
	// ----------------------------------------------------------------------- *
	
	public:
	
	private:
	
	
	};
	
	
	</xsl:template>


	<xsl:template match="/">
		<xsl:if test="not(/d:design/d:class[@name=$className])">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' not found. If you are running this through generateDevice*.sh scripts, maybe you forgot to put name of the class as a parameter?</xsl:message>
		</xsl:if>
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)!='true'">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' hasnt got device logic</xsl:message>
		</xsl:if>
		<xsl:value-of select="fnc:headerStubGenerated(/,'using transform designToDeviceHeader.xslt','Piotr Nikiel')"/>



		#ifndef __<xsl:value-of select="fnc:DClassName($className)" />__H__
		#define __<xsl:value-of select="fnc:DClassName($className)" />__H__

		#include &lt;vector&gt;
		#include &lt;boost/thread/mutex.hpp&gt;

		#include &lt;statuscode.h&gt;
		#include &lt;uadatetime.h&gt;
		#include &lt;session.h&gt;
	
		
		#include &lt;DRoot.h&gt;
		#include &lt;Configuration.hxx&gt;

		#include &lt;<xsl:value-of select="fnc:Base_DClassName($className)" />.h&gt;


		namespace Device
		{
		

		<xsl:for-each select="/d:design/d:class[@name=$className]">
			<xsl:call-template name="deviceHeader" />
		</xsl:for-each>


		}

		#endif // include guard
	</xsl:template>






</xsl:transform>

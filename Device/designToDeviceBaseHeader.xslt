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

	<xsl:template name="deviceHeader">

		class
		<xsl:value-of select="fnc:Base_DClassName($className)" />
		{

		public:
		/* Constructor */
		explicit <xsl:value-of select="fnc:Base_DClassName($className)" /> (
		const Configuration::<xsl:value-of select="@name"/> &amp; config,
		<xsl:value-of select="fnc:Parent_DClassName($className)"/> * parent
				) ;
		
		private:
		/* No copy constructors or assignment operators */
		<xsl:value-of select="fnc:Base_DClassName($className)" /> (const <xsl:value-of select="fnc:Base_DClassName($className)" /> &amp; other);
		<xsl:value-of select="fnc:Base_DClassName($className)" /> &amp; operator=(const <xsl:value-of select="fnc:Base_DClassName($className)" /> &amp; other);
		
		public:
		
		/* sample dtr */
		virtual ~<xsl:value-of select="fnc:Base_DClassName($className)" /> ();
		
		<xsl:value-of select="fnc:Parent_DClassName($className)"/> * getParent () const { return m_parent; } 	

		/* add/remove for handling device structure */
		<xsl:for-each select="d:hasobjects">
		<xsl:variable name="containedClass"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:if test="fnc:classHasDeviceLogic(/,$containedClass)='true'">
		void add ( <xsl:value-of select="fnc:DClassName(@class)"/> *);
		const std::vector&lt;<xsl:value-of select="fnc:DClassName(@class)"/> * &gt; &amp; <xsl:value-of select="lower-case(@class)"/>s () const { return m_<xsl:value-of select="@class"/>s; }
		<xsl:if test="fnc:isHasObjectsSingleton(.)='true'">
		<xsl:value-of select="fnc:DClassName(@class)"/> * <xsl:value-of select="lower-case(@class)"/>() const;
		</xsl:if>
		</xsl:if>
		</xsl:for-each>
		
		/* to safely quit */
		unsigned int unlinkAllChildren ();


		void linkAddressSpace (AddressSpace::<xsl:value-of select="fnc:ASClassName($className)" /> * as, const std::string &amp; stringAddress);
		AddressSpace::<xsl:value-of select="fnc:ASClassName($className)"/> * getAddressSpaceLink () const { return m_addressSpaceLink; }
		
		
		/* find methods for children */
		<xsl:for-each select="d:hasobjects">
		<xsl:variable name="class"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:cachevariable[@isKey='true']">
		/* Returns 0 when element not found */
		<xsl:value-of select="fnc:DClassName($class)"/> * get<xsl:value-of select="$class"/>By<xsl:value-of select="fnc:capFirst(@name)"/> (
		  <xsl:value-of select="fnc:dataTypeToBaseDeviceType(@dataType)"/> key) const;
		</xsl:for-each>
		</xsl:for-each>
		
		<xsl:for-each select="d:hasobjects">
		<xsl:variable name="class"><xsl:value-of select="@class"/></xsl:variable>
		<xsl:for-each select="/d:design/d:class[@name=$class]/d:configentry[@isKey='true']">
		/* Returns 0 when element not found */
		<xsl:value-of select="fnc:DClassName($class)"/> * get<xsl:value-of select="$class"/>By<xsl:value-of select="fnc:capFirst(@name)"/> (
          <xsl:value-of select="fnc:dataTypeToBaseDeviceType(@dataType)"/> key) const;	
		</xsl:for-each>
		</xsl:for-each>
		
		/* getters for values which are keys */
		<xsl:for-each select="d:cachevariable[@isKey='true']">
		const 
		<xsl:choose>
		<xsl:when test="@dataType='UaString'">std::string</xsl:when>
		<xsl:otherwise><xsl:value-of select="@dataType"/></xsl:otherwise>
		</xsl:choose>
		<xsl:text> </xsl:text><xsl:value-of select="@name"/> () { return m_<xsl:value-of select="@name"/>; }
		</xsl:for-each>
		<xsl:for-each select="d:configentry[@isKey='true' or @storedInDeviceObject='true']">
		const 
		<xsl:choose>
		<xsl:when test="@dataType='UaString'">std::string</xsl:when>
		<xsl:otherwise><xsl:value-of select="@dataType"/></xsl:otherwise>
		</xsl:choose>
		<xsl:text> </xsl:text><xsl:value-of select="@name"/> () { return m_<xsl:value-of select="@name"/>; }
		</xsl:for-each>
		
		
		/* mutex operations */
		<xsl:if test="fnc:classDeviceLogicHasMutex(/,$className)='true'">
		void lock () { m_lock.lock(); }
		void unlock () { m_lock.unlock(); }
		</xsl:if>
		
		/* variable-wise locks */
		<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceWriteUseMutex='of_this_variable' or @addressSpaceReadUseMutex='of_this_variable'">
		void lockVariable_<xsl:value-of select="@name"/> () { m_lockVariable_<xsl:value-of select="@name"/>.lock(); }
		void unlockVariable_<xsl:value-of select="@name"/> () { m_lockVariable_<xsl:value-of select="@name"/>.unlock(); } 
		</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceWriteUseMutex='of_this_operation'">
		void lockVariableWrite_<xsl:value-of select="@name"/> () { m_lockVariable_write_<xsl:value-of select="@name"/>.lock(); }
		void unlockVariableWrite_<xsl:value-of select="@name"/> () { m_lockVariable_write_<xsl:value-of select="@name"/>.unlock(); } 		
		</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceReadUseMutex='of_this_operation'">
		void lockVariableRead_<xsl:value-of select="@name"/> () { m_lockVariable_read_<xsl:value-of select="@name"/>.lock(); }
		void unlockVariableRead_<xsl:value-of select="@name"/> () { m_lockVariable_read_<xsl:value-of select="@name"/>.unlock(); } 	
		</xsl:if>
		</xsl:for-each>
		
		/* query address-space for full name (mostly for debug purposes) */
		std::string getFullName() const { return m_stringAddress; };
		
		static std::list &lt; <xsl:value-of select="fnc:DClassName($className)"/> * &gt; s_orphanedObjects;
		static void registerOrphanedObject( <xsl:value-of select="fnc:DClassName($className)"/> * object ) { s_orphanedObjects.push_back( object ); }
        static std::list &lt; <xsl:value-of select="fnc:DClassName($className)"/>* &gt; &amp; orphanedObjects() { return  s_orphanedObjects; }

		private:
		
		Parent_<xsl:value-of select="fnc:DClassName($className)"/> * m_parent;
	
		public:	  // TODO: reconsider this! shoudln't be public!
		AddressSpace::
		<xsl:value-of select="fnc:ASClassName($className)" />
		*m_addressSpaceLink;
		


		private:
		std::string m_stringAddress;
		<xsl:for-each select="d:hasobjects">
		std::vector &lt; <xsl:value-of select="fnc:DClassName(@class)"/> * &gt; m_<xsl:value-of select="@class"/>s;
		</xsl:for-each>
		/* if any of our cachevariables has isKey=true then we shall keep its copy here for find functions  (it is const, either way) */
		<xsl:for-each select="d:cachevariable[@isKey='true']">
		const 
		<xsl:choose>
			<xsl:when test="@dataType='UaString'">std::string </xsl:when>
			<xsl:otherwise><xsl:value-of select="@dataType"/></xsl:otherwise>
		</xsl:choose> m_<xsl:value-of select="@name"/> ;
		</xsl:for-each>
		
		/* if any of our config entries has isKey=true then we shall keep its copy here for find functions  (it is const, either way) */
		<xsl:for-each select="d:configentry[@isKey='true' or @storedInDeviceObject='true']">
		const 
		<xsl:choose>
			<xsl:when test="@dataType='UaString'">std::string </xsl:when>
			<xsl:otherwise><xsl:value-of select="@dataType"/></xsl:otherwise>
		</xsl:choose> m_<xsl:value-of select="@name"/> ;
		</xsl:for-each>

		/* object-wise lock */
		<xsl:if test="fnc:classDeviceLogicHasMutex(/,$className)='true'">
		boost::mutex m_lock;
		</xsl:if>
		
		/* variable-wise locks */
		<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceWriteUseMutex='of_this_variable' or @addressSpaceReadUseMutex='of_this_variable'">
		boost::mutex m_lockVariable_<xsl:value-of select="@name"/> ;
		</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceWriteUseMutex='of_this_operation'">
		boost::mutex m_lockVariable_write_<xsl:value-of select="@name"/> ;
		</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select="d:sourcevariable">
		<xsl:if test="@addressSpaceReadUseMutex='of_this_operation'">
		boost::mutex m_lockVariable_read_<xsl:value-of select="@name"/> ;
		</xsl:if>
		</xsl:for-each>

		
	
	
	};
	
	
	</xsl:template>


	<xsl:template match="/">
		<xsl:if test="not(/d:design/d:class[@name=$className])">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' not found. If you are running this through generateDevice*.sh scripts, maybe you forgot to put name of the class as a parameter?</xsl:message>
		</xsl:if>
		<xsl:if test="fnc:classHasDeviceLogic(/,$className)!='true'">
				<xsl:message terminate="yes">Class '<xsl:value-of select="$className"/>' hasnt got device logic</xsl:message>
		</xsl:if>
		<xsl:value-of select="fnc:headerFullyGenerated(/, 'using transform designToDeviceBaseHeader.xslt','Piotr Nikiel')"/>

		#ifndef __<xsl:value-of select="fnc:Base_DClassName($className)" />__H__
		#define __<xsl:value-of select="fnc:Base_DClassName($className)" />__H__

		#include &lt;vector&gt;
		#include &lt;string&gt;
		#include &lt;list&gt;
		#include &lt;boost/thread/mutex.hpp&gt;

		
		#include &lt;DRoot.h&gt;
		#include &lt;Configuration.hxx&gt;
		
		#include &lt;opcua_platformdefs.h&gt;

		/* forward decl for AddressSpace */
		namespace AddressSpace { class
		<xsl:value-of select="fnc:ASClassName($className)" />
		; }
		/* forward decl for Parent */
		<xsl:if test="fnc:getCountParentClassesAndRoot(/,$className)=1">
		<xsl:if test="fnc:classHasDeviceLogic(/,fnc:getParentClass(/,$className))='true'">
		#include &lt;<xsl:value-of select="fnc:getParentDeviceClass(/,$className)"/>.h&gt;
		</xsl:if>
		</xsl:if>

		namespace Device
		{

		<xsl:choose>
		  <xsl:when test="fnc:getCountParentClassesAndRoot(/,$className)=1 and fnc:classHasDeviceLogic(/,fnc:getParentClass(/,$className))='true'">
		    typedef <xsl:value-of select="fnc:getParentDeviceClass(/,$className)"/> Parent_<xsl:value-of select="fnc:DClassName($className)"/> ;
		  </xsl:when>
		  <xsl:otherwise>
		      typedef struct{/*No exact Parent of the class*/} Parent_<xsl:value-of select="fnc:DClassName($className)"/> ;
		  </xsl:otherwise>
		</xsl:choose>

		
		class <xsl:value-of select="fnc:DClassName($className)"/>;
		/* forward declarations (comes from design.class.hasObjects) */
		<xsl:for-each select="/d:design/d:class[@name=$className]/d:hasobjects">
		class <xsl:value-of select="fnc:DClassName(@class)"/>;
		</xsl:for-each>

		<xsl:for-each select="/d:design/d:class[@name=$className]">
			<xsl:call-template name="deviceHeader" />
		</xsl:for-each>


		}

		#endif // include guard
	</xsl:template>






</xsl:transform>

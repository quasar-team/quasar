<xsl:transform version="2.0" xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/Functions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
    <xsl:output indent="yes" method="xml"/>
    
    <xsl:function name="fnc:estimateFunctionReadWrite">
    <xsl:param name="this"/>
    <xsl:if test="name($this)='d:cachevariable'">
        <xsl:choose>
            <xsl:when test="$this/@addressSpaceWrite='forbidden'">read-only</xsl:when>
            <xsl:otherwise>read+write</xsl:otherwise>
        </xsl:choose>
    </xsl:if>
    <xsl:if test="name($this)='d:sourcevariable'">
        <xsl:choose>
            <xsl:when test="$this/@addressSpaceWrite='forbidden' and $this/@addressSpaceRead='forbidden'">none</xsl:when>
            <xsl:when test="$this/@addressSpaceWrite='forbidden' and $this/@addressSpaceRead!='forbidden'">read-only</xsl:when>
            <xsl:when test="$this/@addressSpaceWrite!='forbidden' and $this/@addressSpaceRead='forbidden'">write-only</xsl:when>
            <xsl:when test="$this/@addressSpaceWrite!='forbidden' and $this/@addressSpaceRead!='forbidden'">read+write</xsl:when>
        </xsl:choose>
    </xsl:if>
        
    </xsl:function>
    
    <xsl:template match="/">
    
    <html>
    
    <h1>OPC-UA address space documentation for <xsl:value-of select="/d:design/@projectShortName"/></h1>
    
    
    Jump to:
    <xsl:for-each select="/d:design/d:class">
    <a href="#class_{@name}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
    </xsl:for-each>
    
    <xsl:for-each select="/d:design/d:class">
    <xsl:sort select="@name"/>
    <a id="class_{@name}"><h2><xsl:value-of select="@name"/></h2></a>
    <xsl:if test="d:documentation">
        <div style="background-color:#eeeeff"><font color="blue">DOC</font><xsl:text> </xsl:text>
            <xsl:copy-of select="d:documentation"/>
        </div>
        <br/>
    </xsl:if>
    
    <xsl:choose>
        <xsl:when test="d:cachevariable | d:sourcevariable | d:method">
        <xsl:if test="d:cachevariable | d:sourcevariable">
        Variables:
        <ul>
        <xsl:for-each select="d:cachevariable | d:sourcevariable">
            <li>
                <b><xsl:value-of select="@name"/></b> (dataType=<xsl:value-of select="@dataType"/>;
                <xsl:choose>
                    <xsl:when test="name()='d:cachevariable'">cache-variable</xsl:when>
                    <xsl:otherwise>source-variable</xsl:otherwise>           
                </xsl:choose>; access=<xsl:value-of select="fnc:estimateFunctionReadWrite(.)"/>)
                <br/>
                <xsl:if test="d:documentation">
                    <div style="background-color:#eeeeff"><font color="blue">DOC</font><xsl:text> </xsl:text>
                    <xsl:copy-of select="d:documentation"/>
                    </div>
                </xsl:if>
                
            </li>
        </xsl:for-each>
        </ul>
        </xsl:if>

        <xsl:if test="d:method">
        Methods:
        <ul>
            <xsl:for-each select="d:method">
            <li>
                <b><xsl:value-of select="@name"/></b>
                <xsl:if test="d:documentation">
                    <div style="background-color:#eeeeff"><font color="blue">DOC</font><xsl:text> </xsl:text>
                    <xsl:copy-of select="d:documentation"/>
                    </div>
                </xsl:if>
                <xsl:if test="d:argument">
                    <br/>
                    Arguments:
                    <ul>
                        <xsl:for-each select="d:argument">
                            <li>
                                <xsl:value-of select="@name"/> (<xsl:value-of select="@dataType"/> <xsl:if test="d:array">[]</xsl:if>)
                                <xsl:if test="d:documentation">
                                    <div style="background-color:#eeeeff"><font color="blue">DOC</font><xsl:text> </xsl:text>
                                    <xsl:copy-of select="d:documentation"/>
                                    </div>
                                </xsl:if>
                            </li>
                        </xsl:for-each>
                    </ul>
                </xsl:if>
                <xsl:if test="d:returnvalue">
                    <br/>
                    Return values:
                    <ul>
                        <xsl:for-each select="d:returnvalue">
                            <li>
                                <xsl:value-of select="@name"/> (<xsl:value-of select="@dataType"/> <xsl:if test="d:array">[]</xsl:if>)
                                    <xsl:if test="d:documentation">
                                        <div style="background-color:#eeeeff"><font color="blue">DOC</font><xsl:text> </xsl:text>
                                        <xsl:copy-of select="d:documentation"/>
                                        </div>
                                    </xsl:if>
                            </li>
                        </xsl:for-each>
                    </ul>
                </xsl:if>
            </li>
            </xsl:for-each>
        </ul>
        </xsl:if>

            
        </xsl:when>
        <xsl:otherwise>
        This quasar class has no address-space components.
        </xsl:otherwise>
        
    </xsl:choose>
    
    <xsl:if test="d:hasobjects">
        Classes possible to be children objects:
        <xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
            <a href="#class_{@class}"><xsl:value-of select="@class"/></a>
        </xsl:for-each>   
    </xsl:if>
    
    </xsl:for-each>


    
    
    </html>
    
    
    </xsl:template>
    
    </xsl:transform>
    
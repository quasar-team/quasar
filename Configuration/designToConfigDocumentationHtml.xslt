<xsl:transform version="2.0" xmlns:xml="http://www.w3.org/XML/1998/namespace" 
xmlns:xs="http://www.w3.org/2001/XMLSchema" 
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:d="http://cern.ch/quasar/Design"
xmlns:fnc="http://cern.ch/quasar/Functions"
xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform schema-for-xslt20.xsd ">
    <xsl:output indent="yes" method="xml"/>
    
    <xsl:template match="/">
    
    <html>
    
    <h1>Configuration documentation for your Quasar server</h1>
    
    
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
        <xsl:when test="d:configentry | d:cachevariable[@initializeWith='configuration']">
        Configuration attributes:
        <ul>
        <xsl:for-each select="d:configentry | d:cachevariable[@initializeWith='configuration']">
            <li>
                <xsl:value-of select="@name"/>
                <xsl:if test="d:documentation">
                    <div style="background-color:#eeeeff"><font color="blue">DOC</font><xsl:text> </xsl:text>
                    <xsl:copy-of select="d:documentation"/>
                    </div>
                </xsl:if>
            </li>
        </xsl:for-each>
        </ul>
            
        </xsl:when>
        <xsl:otherwise>
        This class(XML element) has no configuration attributes.
        </xsl:otherwise>
        
    </xsl:choose>
    
    Possible children:
    <xsl:for-each select="d:hasobjects[@instantiateUsing='configuration']">
    <a href="#class_{@class}"><xsl:value-of select="@class"/></a>
    </xsl:for-each>
    
    </xsl:for-each>


    
    
    </html>
    
    
    </xsl:template>
    
    </xsl:transform>
    
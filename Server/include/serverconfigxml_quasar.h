/******************************************************************************
** serverconfigxml.h
**
** Copyright (C) 2008-2009 Unified Automation GmbH. All Rights Reserved.
** Web: http://www.unified-automation.com
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Project: C++ OPC Server SDK sample code
**
** Description: Configuration management class for the OPC Server.
**
******************************************************************************/
#ifndef SERVERCONFIGXML_H
#define SERVERCONFIGXML_H

#include "serverconfigdata.h"
#include "xmldocument.h"
#include "uapkiprovider.h"

// Default configuration if not present in the XML file
#define CONFIGXML_APPLICATIONURI   "UnifiedAutomation/UaDemoserver"
#define CONFIGXML_MANUFACTURERNAME "Unified Automation GmbH"
#define CONFIGXML_APPLICATIONNAME  "OpcDemoServer"
#define CONFIGXML_SOFTWAREVERSION  "1.0.0"
#define CONFIGXML_BUILDNUMBER      "232"
#define CONFIGXML_SERVERURI        "[NodeName]/UnifiedAutomation/UaDemoserver"
#define CONFIGXML_SERVERNAME       "OpcDemoServer@[NodeName]"
#define CONFIGXML_SERVERTRACEFILE  "[ApplicationPath]/SrvTrace.log"
// Default security settings if no entry in XML file
// 1 -> Basic128Rsa15 / SignAndEncrypt 0 -> None / None
#define CONFIG_DEFAULT_SECURE   1

/** ServerConfigXml 
 *  Derived from ServerConfig. 
 *  It is not possible to make plane copies of this class.
 */
class ServerConfigXml: public ServerConfigData
{
    UA_DISABLE_COPY(ServerConfigXml);
public:
    /* construction. */
    ServerConfigXml(const UaString& sXmlFileName, const UaString& sApplicationPath);
    /** destruction */
    virtual ~ServerConfigXml();

    /*  Load the configuration from the config file.
     *  First method called after creation of ServerConfig. Must create all NodeManagers
     *  before method startUp is called.
     *  @return               Error code.
     */
    UaStatus loadConfiguration();

    /* Save the configuration to the config file. */
    UaStatus saveConfiguration();

    inline UaString sXmlFileName() const {return m_sXmlFileName;}

private:
    UaString              m_sXmlFileName;
};

/** UaEndpointXml
 *  Derived from UaEndpoint.
 */
class UaEndpointXml: public UaEndpoint
{
public:
    /** construction. */
    UaEndpointXml(){};
    /** destruction */
    virtual ~UaEndpointXml(){};

    /*  Init Endpoint with XML configuration.
     *  @param xmlElement       a reference to the xml Element.
     *  @param sApplicationPath the path to the application.
     *  @param pServerConfig    a pointer to the server config struct.
     *  @return                 the OPC UA Status code.
     */
    UaStatus setXmlConfig(UaXmlElement &xmlElement, const UaString& sApplicationPath, ServerConfig* pServerConfig, bool& updatedConfiguration);
};

#endif // SERVERCONFIGXML_H

#ifndef BACKEND_OPEN62541
/******************************************************************************
** serverconfigxml.cpp
**
** Copyright (C) 2008-2011 Unified Automation GmbH. All Rights Reserved.
** Web: http://www.unified-automation.com
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Project: C++ OPC Server SDK sample code
**
** Description: Configuration management class for the OPC Server.
**
******************************************************************************/

#include <version_coremodule.h>

/* Support for UA Toolkit API 1.3, 1.4 and 1.5 by pnikiel and damiron */
/* Note the API is different in these versions wrt to server configuration*/

// Defines storing UA SDK version are unfortunately called differently in 1.5.x compared to previous versions ...
#ifndef CPP_SDK_MAJOR
#define CPP_SDK_MAJOR PROD_MAJOR
#define CPP_SDK_MINOR PROD_MINOR
#define CPP_SDK_MINOR2 PROD_PATCH
#endif

#define UA_API_VERSION (CPP_SDK_MAJOR * 100 \
                               + CPP_SDK_MINOR * 10 \
                               + CPP_SDK_MINOR2 )


#if UA_API_VERSION < 140       // serverconfig is implemented by files coming from quasar
#include <serverconfigxml_quasar.h>

#ifdef _WIN32
	# include <winsock2.h>	 
#endif
#include "opcua_core.h"
#include "servermanager.h"
#include "srvtrace.h"
#include "uaunistring.h"

#if OPCUA_SUPPORT_PKI
#include "uadir.h"
#include "uapkicertificate.h"
#include "uapkirevocationlist.h"
#endif // OPCUA_SUPPORT_PKI

/* ----------------------------------------------------------------------------
    Begin Class    ServerConfigXml
-----------------------------------------------------------------------------*/

/** construction
 * @param sXmlFileName     the file name of the XML file.
 * @param sApplicationPath the path to the application.
 */
ServerConfigXml::ServerConfigXml(const UaString& sXmlFileName, const UaString& sApplicationPath)
:ServerConfigData(sApplicationPath)
{
    m_sXmlFileName                 = sXmlFileName;

    // Default Build Info settings
    m_sApplicationUri   = CONFIGXML_APPLICATIONURI;
    m_sManufacturerName = CONFIGXML_MANUFACTURERNAME;
    m_sApplicationName  = CONFIGXML_APPLICATIONNAME;
    m_sSoftwareVersion  = CONFIGXML_SOFTWAREVERSION;
    m_sBuildNumber      = CONFIGXML_BUILDNUMBER;

    // Default ServerDescription settings
    m_sServerUri        = CONFIGXML_SERVERURI;
    m_sServerName       = CONFIGXML_SERVERNAME;
}

/** destruction */
ServerConfigXml::~ServerConfigXml()
{
}

/** Load the configuration from the config file and generate node managers.
 *  First method called after creation of ServerConfig. Must create all NodeManagers
 *  before method startUp is called.
 *  @return               Error code.
 */
UaStatus ServerConfigXml::loadConfiguration()
{
    if ( m_isConfigLoaded != OpcUa_False )
    {
        return OpcUa_Good;
    }

    UaStatus ret;
    UaString sNodeName;
    char szHostName[256];
    if ( 0 == gethostname(szHostName, 256) )
    {
        sNodeName = szHostName;
    }

    /*****************************************************
    Configuration Option
      This method can be used to load server specific 
      settings from a file and to overwrite the default 
      settings from the constructor
    ******************************************************/
    /*****************************************************
    The sample implementation uses a XML file to load the 
    configuration parameters
    ******************************************************/
    UaXmlDocument * pXmlDoc = new UaXmlDocument;
    if ( 0 != pXmlDoc->loadXmlFile( (char*)m_sXmlFileName.toUtf8() ) )
    {
        delete pXmlDoc;
        return OpcUa_BadInternalError;
    }

    const char*  strName = NULL;
    UaXmlValue   value;
    UaVariant    uvValue;
    bool         xmlParserError = true;
    OpcUa_Int32  nValue;
    OpcUa_UInt32 uValue;
    bool updatedConfiguration = false;

    //-----------------------------------------------------------
    // Read general UaServerConfig settings
    strName = pXmlDoc->getNodeName();
    if( (strName != NULL) && (strcmp("OpcServerConfig", strName) == 0) )
    {
        if ( 0 == pXmlDoc->getChild("UaServerConfig") )
        {
            xmlParserError = false;
            if ( 0 == pXmlDoc->getChild("MaxRequestAge") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxRequestAge = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxSessionCount") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxSessionCount = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MinSessionTimeout") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_minSessionTimeout = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxSessionTimeout") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxSessionTimeout = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxBrowseContinuationPoints") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxBrowseContinuationPoints = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxHistoryContinuationPoints") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxHistoryContinuationPoints = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MinPublishingInterval") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_minPublishingInterval = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxPublishingInterval") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxPublishingInterval = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MinKeepAliveInterval") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_minKeepAliveInterval = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MinSubscriptionLifetime") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_minSubscriptionLifetime = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxSubscriptionLifetime") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxSubscriptionLifetime = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxRetransmitionQueueSize") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxRetransmitionQueueSize = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxNotificationsPerPublish") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxNotificationsPerPublish = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxDataQueueSize") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_iMaxDataQueueSize = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxEventQueueSize") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_iMaxEventQueueSize = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxSubscriptionCount") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxSubscriptionCount = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxMonitoredItemCount") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxMonitoredItemCount = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MaxMonitoredItemPerSubscriptionCount") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                    {
                        m_maxMonitoredItemPerSubscriptionCount = nValue;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("MinSupportedSampleRate") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    OpcUa_Double tempDouble;
                    uvValue.setString(value.pValue());
                    if ( OpcUa_IsGood(uvValue.toDouble(tempDouble)) )
                    {
                        m_minSupportedSampleRate = tempDouble;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("AvailableLocaleIds") )
            {
                UaXmlElementList uaXmlElementList;
                if ( 0 == pXmlDoc->getChildElements("LocaleId", uaXmlElementList) )
                {
                    m_localeIdArray.create((OpcUa_UInt32)uaXmlElementList.size());
                    UaXmlElementListIterator it;
                    OpcUa_UInt32 i = 0;
                    for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                    {
                        it->getContent(value);
                        if ( value.pValue() != NULL )
                        {
                            UaString sTemp(value.pValue());
                            sTemp.copyTo(&m_localeIdArray[i]);
                            i++;
                        }
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("AvailableServerProfiles") )
            {
                UaXmlElementList uaXmlElementList;
                if ( 0 == pXmlDoc->getChildElements("ServerProfileUri", uaXmlElementList) )
                {
                    m_serverProfileArray.create((OpcUa_UInt32)uaXmlElementList.size());
                    UaXmlElementListIterator it;
                    OpcUa_UInt32 i = 0;
                    for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                    {
                        it->getContent(value);
                        if ( value.pValue() != NULL )
                        {
                            UaString sTemp(value.pValue());
                            sTemp.copyTo(&m_serverProfileArray[i]);
                            i++;
                        }
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("IsAuditActivated") )
            {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_isAuditActivated = OpcUa_True;
                        }
                        else
                        {
                            m_isAuditActivated = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("RedundancySettings") )
            {
                if ( 0 == pXmlDoc->getChild("RedundancySupport") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sRedundancySupport = value.pValue();
                        if ( sRedundancySupport == "Cold" )
                        {
                            m_redundancySupport = OpcUa_RedundancySupport_Cold;
                        }
                        else if ( sRedundancySupport == "Warm" )
                        {
                            m_redundancySupport = OpcUa_RedundancySupport_Warm;
                        }
                        else if ( sRedundancySupport == "Hot" )
                        {
                            m_redundancySupport = OpcUa_RedundancySupport_Hot;
                        }
                        else if ( sRedundancySupport == "Transparent" )
                        {
                            m_redundancySupport = OpcUa_RedundancySupport_Transparent;
                        }
                        else
                        {
                            m_redundancySupport = OpcUa_RedundancySupport_None;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                UaXmlElementList uaXmlElementList;
                if ( 0 == pXmlDoc->getChildElements("ServerUri", uaXmlElementList) )
                {
                    m_serverUriArray.create((OpcUa_UInt32)uaXmlElementList.size());
                    UaXmlElementListIterator it;
                    OpcUa_UInt32 i = 0;
                    for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                    {
                        it->getContent(value);
                        if ( value.pValue() != NULL )
                        {
                            UaString sTemp(value.pValue());
                            sTemp.copyTo(&m_serverUriArray[i]);
                            i++;
                        }
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("AdditionalServerEntries") )
            {
                UaXmlElementList uaXmlElementList;
                if ( 0 == pXmlDoc->getChildElements("ApplicationDescription", uaXmlElementList) )
                {
                    m_serverDescriptions.create((OpcUa_UInt32)uaXmlElementList.size());
                    UaXmlElementListIterator it;
                    OpcUa_UInt32 i = 0;
                    for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                    {
                        UaXmlElement xmlElement = *it;
                        if ( 0 == xmlElement.getChild("ApplicationUri") )
                        {
                            xmlElement.getContent(value);
                            UaString sValue(value.pValue());
                            replaceNodeName(sValue, sNodeName);
                            sValue.copyTo(&m_serverDescriptions[i].ApplicationUri);
                            xmlElement.getParentNode();
                        }
                        if ( 0 == xmlElement.getChild("ProductUri") )
                        {
                            xmlElement.getContent(value);
                            UaString sValue(value.pValue());
                            sValue.copyTo(&m_serverDescriptions[i].ProductUri);
                            xmlElement.getParentNode();
                        }
                        if ( 0 == xmlElement.getChild("ApplicationName") )
                        {
                            xmlElement.getContent(value);
                            UaLocalizedText ltValue("", value.pValue());
                            ltValue.copyTo(&m_serverDescriptions[i].ApplicationName);
                            xmlElement.getParentNode();
                        }
                        if ( 0 == xmlElement.getChild("ApplicationType") )
                        {
                            xmlElement.getContent(value);
                            if ( value.pValue() != NULL )
                            {
                                UaString sApplicationType = value.pValue();
                                if ( sApplicationType == "ClientAndServer" )
                                {
                                    m_serverDescriptions[i].ApplicationType = OpcUa_ApplicationType_ClientAndServer;
                                }
                                else
                                {
                                    m_serverDescriptions[i].ApplicationType = OpcUa_ApplicationType_Server;
                                }
                            }
                            xmlElement.getParentNode();
                        }
                        if ( 0 == xmlElement.getChild("GatewayServerUri") )
                        {
                            xmlElement.getContent(value);
                            UaString sValue(value.pValue());
                            sValue.copyTo(&m_serverDescriptions[i].GatewayServerUri);
                            xmlElement.getParentNode();
                        }
                        if ( 0 == xmlElement.getChild("DiscoveryProfileUri") )
                        {
                            xmlElement.getContent(value);
                            UaString sValue(value.pValue());
                            sValue.copyTo(&m_serverDescriptions[i].DiscoveryProfileUri);
                            xmlElement.getParentNode();
                        }
                        UaXmlElementList uaXmlElementList2;
                        if ( 0 == xmlElement.getChildElements("DiscoveryUrl", uaXmlElementList2) )
                        {
                            if ( uaXmlElementList2.size() > 0 )
                            {
                                UaStringArray tempDiscoveryUrls;
                                tempDiscoveryUrls.create((OpcUa_UInt32)uaXmlElementList2.size());
                                UaXmlElementListIterator it2;
                                OpcUa_UInt32 j = 0;
                                for ( it2 = uaXmlElementList2.begin(); it2 != uaXmlElementList2.end(); ++it2)
                                {
                                    it2->getContent(value);
                                    if ( value.pValue() != NULL )
                                    {
                                        UaString sTemp(value.pValue());
                                        replaceNodeName(sTemp, sNodeName);
                                        sTemp.copyTo(&tempDiscoveryUrls[j]);
                                        j++;
                                    }
                                }
                                m_serverDescriptions[i].NoOfDiscoveryUrls = tempDiscoveryUrls.length();
                                m_serverDescriptions[i].DiscoveryUrls     = tempDiscoveryUrls.detach();
                            }
                        }
                        i++;
                    }
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("AvailableSamplingRates") )
            {
                UaXmlElementList uaXmlElementList;
                if ( 0 == pXmlDoc->getChildElements("SamplingRate", uaXmlElementList) )
                {
                    m_availableSamplingRates.create((OpcUa_UInt32)uaXmlElementList.size());
                    UaXmlElementListIterator it;
                    OpcUa_UInt32 i = 0;
                    for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                    {
                        it->getContent(value);
                        if ( value.pValue() != NULL )
                        {
                            uvValue.setString(value.pValue());
                            if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                            {
                                m_availableSamplingRates[i] = nValue;
                                i++;
                            }
                        }
                    }
                }
                pXmlDoc->getParentNode();
            }
            else
            {
                m_availableSamplingRates.create(8);
                m_availableSamplingRates[0] = 50;
                m_availableSamplingRates[1] = 100;
                m_availableSamplingRates[2] = 250;
                m_availableSamplingRates[3] = 500;
                m_availableSamplingRates[4] = 1000;
                m_availableSamplingRates[5] = 2000;
                m_availableSamplingRates[6] = 5000;
                m_availableSamplingRates[7] = 10000;
            }
            if ( 0 == pXmlDoc->getChild("UserIdentityTokens") )
            {
                if ( 0 == pXmlDoc->getChild("EnableAnonymous") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bEnableAnonymous = OpcUa_True;
                        }
                        else
                        {
                            m_bEnableAnonymous = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("EnableUserPw") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bEnableUserPw = OpcUa_True;
                        }
                        else
                        {
                            m_bEnableUserPw = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("ApplicationUri") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sApplicationUri = value.pValue();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("ManufacturerName") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sManufacturerName = value.pValue();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("ApplicationName") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sApplicationName = value.pValue();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("SoftwareVersion") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sSoftwareVersion = value.pValue();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("BuildNumber") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sBuildNumber = value.pValue();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("ServerUri") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sServerUri = value.pValue();
                    replaceNodeName(m_sServerUri, sNodeName);
                }
                pXmlDoc->getParentNode();
            }
            else
            {   // Default name can also use the place holder
                replaceNodeName(m_sServerUri, sNodeName);
            }
            if ( 0 == pXmlDoc->getChild("ServerName") )
            {
                pXmlDoc->getContent(value);
                if ( value.pValue() != NULL )
                {
                    m_sServerName = value.pValue();
                    replaceNodeName(m_sServerName, sNodeName);
                }
                pXmlDoc->getParentNode();
            }
            else
            {   // Default name can also use the place holder
                replaceNodeName(m_sServerName, sNodeName);
            }
            if ( 0 == pXmlDoc->getChild("Serializer") )
            {
                if ( 0 == pXmlDoc->getChild("MaxAlloc") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxAlloc = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxStringLength") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxStringLength = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxByteStringLength") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxByteStringLength = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxArrayLength") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxArrayLength = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxMessageSize") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxMessageSize = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("StackThreadPoolSettings") )
            {
                if ( 0 == pXmlDoc->getChild("Enabled") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bStackThreadPoolEnabled = OpcUa_True;
                        }
                        else
                        {
                            m_bStackThreadPoolEnabled = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MinThreads") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMinStackThreads = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxThreads") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxStackThreads = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxJobs") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_iMaxStackThreadJobs = nValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("BlockOnAdd") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bStackThreadBlockOnAdd = OpcUa_True;
                        }
                        else
                        {
                            m_bStackThreadBlockOnAdd = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("Timeout") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                        {
                            m_nStackThreadTimeout = nValue;
                            if ( m_nStackThreadTimeout == 0 )
                            {
                                m_nStackThreadTimeout = OPCUA_INFINITE;
                            }
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("Trace") )
            {
                if ( 0 == pXmlDoc->getChild("UaStackTraceEnabled") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bTraceEnabled = OpcUa_True;
                        }
                        else
                        {
                            m_bTraceEnabled = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("UaStackTraceLevel") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sTraceLevel = value.pValue();
                        if ( sTraceLevel == "ERROR" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_ERROR;
                        }
                        else if ( sTraceLevel == "WARNING" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_WARNING;
                        }
                        else if ( sTraceLevel == "SYSTEM" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_SYSTEM;
                        }
                        else if ( sTraceLevel == "INFO" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_INFO;
                        }
                        else if ( sTraceLevel == "DEBUG" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_DEBUG;
                        }
                        else if ( sTraceLevel == "CONTENT" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_CONTENT;
                        }
                        else if ( sTraceLevel == "ALL" )
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_ALL;
                        }
                        else
                        {
                            m_uTraceLevel = OPCUA_TRACE_OUTPUT_LEVEL_NONE;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("UaAppTraceEnabled") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bSdkTraceEnabled = OpcUa_True;
                        }
                        else
                        {
                            m_bSdkTraceEnabled = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("UaAppTraceLevel") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sTraceLevel = value.pValue();
                        if ( sTraceLevel == "Errors" )
                        {
                            m_uSdkTraceLevel = 1;
                        }
                        else if ( sTraceLevel == "Warning" )
                        {
                            m_uSdkTraceLevel = 2;
                        }
                        else if ( sTraceLevel == "Info" )
                        {
                            m_uSdkTraceLevel = 3;
                        }
                        else if ( sTraceLevel == "InterfaceCall" )
                        {
                            m_uSdkTraceLevel = 4;
                        }
                        else if ( sTraceLevel == "CtorDtor" )
                        {
                            m_uSdkTraceLevel = 5;
                        }
                        else if ( sTraceLevel == "ProgramFlow" )
                        {
                            m_uSdkTraceLevel = 6;
                        }
                        else if ( sTraceLevel == "Data" )
                        {
                            m_uSdkTraceLevel = 7;
                        }
                        else
                        {
                            m_uSdkTraceLevel = 0;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("UaAppTraceMaxEntries") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_uMaxTraceEntries = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("UaAppTraceMaxBackup") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_uMaxBackupFiles = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("UaAppTraceFile") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        m_sTraceFile = value.pValue();
                        replaceApplicationPath(m_sTraceFile, m_sApplicationPath);
                    }
                    pXmlDoc->getParentNode();
                }
                else
                {   // Default name can also use the place holder
                    replaceApplicationPath(m_sTraceFile, m_sApplicationPath);
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("ThreadPoolSettings") )
            {
                if ( 0 == pXmlDoc->getChild("MinSizeTransactionManager") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_minSizeTransactionManager = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxSizeTransactionManager") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_maxSizeTransactionManager = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MinSizeSubscriptionManager") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_minSizeSubscriptionManager = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("MaxSizeSubscriptionManager") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_maxSizeSubscriptionManager = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                pXmlDoc->getParentNode();
            }
            if ( 0 == pXmlDoc->getChild("DiscoveryRegistration") )
            {
                if ( 0 == pXmlDoc->getChild("AutomaticCertificateExchange") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        UaString sBool = value.pValue();
                        if ( sBool == "true" )
                        {
                            m_bAutomaticCertificateExchange = OpcUa_True;
                        }
                        else
                        {
                            m_bAutomaticCertificateExchange = OpcUa_False;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("DiscoveryServerCertificateFile") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        m_sDiscoveryServerCertificateFile = value.pValue();
                        replaceApplicationPath(m_sDiscoveryServerCertificateFile, sApplicationPath());
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("DiscoveryServerStoreName") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        m_sDiscoveryServerStoreName = value.pValue();
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("DiscoveryServerCertificateName") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        m_sDiscoveryServerCertificateName = value.pValue();
                    }
                    pXmlDoc->getParentNode();
                }
                if ( 0 == pXmlDoc->getChild("RegistrationInterval") )
                {
                    pXmlDoc->getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        uvValue.setString(value.pValue());
                        if ( OpcUa_IsGood(uvValue.toUInt32(uValue)) )
                        {
                            m_nRegistrationInterval = uValue;
                        }
                    }
                    pXmlDoc->getParentNode();
                }
                UaXmlElementList uaXmlElementList;
                if ( 0 == pXmlDoc->getChildElements("Url", uaXmlElementList) )
                {
                    m_discoveryUrlArray.create((OpcUa_UInt32)uaXmlElementList.size());
                    UaXmlElementListIterator it;
                    OpcUa_UInt32 i = 0;
                    for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                    {
                        it->getContent(value);
                        if ( value.pValue() != NULL )
                        {
                            UaString sTemp(value.pValue()); 
                            sTemp.copyTo(&m_discoveryUrlArray[i]);
                        }
                    }
                }
                pXmlDoc->getParentNode();
            }
        }
    }

    //-----------------------------------------------------------
    // Read UaServerConfig endpoint descriptions
    if ( xmlParserError == false )
    {
        if ( 0 == pXmlDoc->getChild("RejectedCertificatesDirectory") )
        {
            pXmlDoc->getContent(value);
            if ( value.pValue() != NULL )
            {
                m_sRejectedCertificateDirectory = value.pValue();
                replaceApplicationPath(m_sRejectedCertificateDirectory, m_sApplicationPath);
            }
            pXmlDoc->getParentNode();
        }
        UaXmlElementList uaXmlElementList;
        if ( 0 == pXmlDoc->getChildElements("UaEndpoint", uaXmlElementList) )
        {
            m_uaEndpointArray.create((OpcUa_UInt32)uaXmlElementList.size());
            UaXmlElementListIterator it;
            OpcUa_UInt32 i = 0;
            OpcUa_UInt32 successCount = 0;
            for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
            {
                bool tempUpdatedConfiguration = false;
                UaEndpointXml *pUaEndpointXml = new UaEndpointXml;
                ret = pUaEndpointXml->setXmlConfig(*it, m_sApplicationPath, this, tempUpdatedConfiguration);

                if ( tempUpdatedConfiguration )
                {
                    updatedConfiguration = true;
                }

                if ( ret.isGood() )
                {
                    m_uaEndpointArray[i] = pUaEndpointXml;
                    successCount++;
                }
                else
                {
                    delete pUaEndpointXml;
                }
                i++;
            }
            if ( successCount == 0 )
            {
                ret = OpcUa_BadConfigurationError;
            }
            m_uaEndpointArray.resize(successCount);
        }
        else
        {
            xmlParserError = true;
        }
    }

    if ( xmlParserError )
    {
        ret = OpcUa_BadInternalError;
    }
    
    if ( ret.isGood() && updatedConfiguration )
    {
        pXmlDoc->saveXmlFile((char*)m_sXmlFileName.toUtf8());
    }

    pXmlDoc->freeXmlDocument();
    delete pXmlDoc;
    /*****************************************************
    Configuration Option
    ******************************************************/

    /*******************************************************
     Initialize derived class
    ********************************************************/
    if ( ret.isGood() )
    {
        m_isConfigLoaded = OpcUa_True;
        ret = afterLoadConfiguration();
    }

    return ret;
}

/** Save the configuration to the config file.
 *  @return Error code.
 */
UaStatus ServerConfigXml::saveConfiguration()
{
    UaStatus ret;

    return ret;
}
/* ----------------------------------------------------------------------------
    End Class    ServerConfigXml
-----------------------------------------------------------------------------*/


/** Init Endpoint with XML configuration.
 *  @param xmlElement           a reference to the xml Element.
 *  @param sApplicationPath     the path to the application.
 *  @param pServerConfig        a pointer to the server config struct.
 *  @param updatedConfiguration indicates if the thumbprint is updated in the ServerConfig.xml
 *  @return                     the OPC UA Status code.
 */
UaStatus UaEndpointXml::setXmlConfig(UaXmlElement &xmlElement, const UaString& sApplicationPath, ServerConfig* pServerConfig, bool& updatedConfiguration)
{
    UaStatus    ret;
    UaXmlValue  value;
    const char* strName = NULL;

    updatedConfiguration = false;

    UaString sNodeName;
    char szHostName[256];
    if ( 0 == gethostname(szHostName, 256) )
    {
        sNodeName = szHostName;
    }

    // Validate if the xml element has the right name
    strName = xmlElement.getNodeName();
    if( (strName == NULL) && (strcmp("UaEndpoint", strName) != 0) )
    {
        TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - Is not a UaEndpoint tag");
        return OpcUa_BadInvalidArgument;
    }

    /********************************************/
    // Get the Endpoint URL
    if ( 0 == xmlElement.getChild("Url") )
    {
        xmlElement.getContent(value);
        UaString sUrl = value.pValue();
        if ( (sUrl.find("/") < 0) && (sUrl.length() < 6) )
        {
            // If only the port is configured we create a localhost URL
            UaString sTemp = "opc.tcp://localhost:";
            sTemp = sTemp + sUrl;
            sUrl = sTemp;
        }
        else
        {
            ServerConfigXml::replaceNodeName(sUrl, sNodeName);
        }
        setEndpointUrl(sUrl, OpcUa_True);
        xmlElement.getParentNode();
    }
    else
    {
        TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element Url is missing");
        return OpcUa_BadInvalidArgument;
    }
    if ( 0 == xmlElement.getChild("StackUrl") )
    {
        xmlElement.getContent(value);
        UaString sUrl = value.pValue();
        setStackEndpointUrl(sUrl);
        xmlElement.getParentNode();
    }
    /********************************************/

    if ( 0 == xmlElement.getChild("Redundancy") )
    {
        xmlElement.getContent(value);
        UaString sBool = value.pValue();
        if ( sBool == "true" )
        {
            setIsTransparentRedundant(OpcUa_True);
        }
        xmlElement.getParentNode();
    }

    if ( 0 == xmlElement.getChild("IsVisible") )
    {
        xmlElement.getContent(value);
        UaString sBool = value.pValue();
        if ( sBool == "false" )
        {
            setIsVisible(OpcUa_False);
        }
        else
        {
            setIsVisible(OpcUa_True);
        }
        xmlElement.getParentNode();
    }

    if ( 0 == xmlElement.getChild("IsDiscoveryUrl") )
    {
        xmlElement.getContent(value);
        UaString sBool = value.pValue();
        if ( sBool == "false" )
        {
            setIsDiscoveryUrl(OpcUa_False);
        }
        else
        {
            setIsDiscoveryUrl(OpcUa_True);
        }
        xmlElement.getParentNode();
    }

#if OPCUA_SUPPORT_PKI
    /********************************************/
    // Get PKI configuration and load certificate
    UaString     sRevocationListLocation;
    UaString     sTrustListLocation;
    UaString     sServerCertificate;
    UaString     sPrivateKey;
    UaByteString serverCertificate;
    UaByteString serverPrivateKey;
    OpcUa_Boolean isOpenSSLStore = OpcUa_True;
#if OPCUA_SUPPORT_PKI_WIN32
    WindowsStoreLocation windowsStoreLocation = Location_LocalMachine;
#endif // OPCUA_SUPPORT_PKI_WIN32

    UaString serverUri;
    UaLocalizedTextArray ltServerName;
    pServerConfig->getServerInstanceInfo(serverUri, ltServerName);

    if ( 0 == xmlElement.getChild("CertificateStore") )
    {
        if ( 0 == xmlElement.getChild("OpenSSLStore") )
        {
            if ( 0 == xmlElement.getChild("CertificateTrustListLocation") )
            {
                xmlElement.getContent(value);
                sTrustListLocation = value.pValue();
                ServerConfigXml::replaceApplicationPath(sTrustListLocation, sApplicationPath);
                xmlElement.getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element CertificateTrustListLocation is missing");
                return OpcUa_BadInvalidArgument;
            }
            if ( 0 == xmlElement.getChild("CertificateRevocationListLocation") )
            {
                xmlElement.getContent(value);
                sRevocationListLocation = value.pValue();
                ServerConfigXml::replaceApplicationPath(sRevocationListLocation, sApplicationPath);
                xmlElement.getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element CertificateRevocationListLocation is missing");
                return OpcUa_BadInvalidArgument;
            }
            if ( 0 == xmlElement.getChild("ServerCertificate") )
            {
                xmlElement.getContent(value);
                sServerCertificate = value.pValue();
                ServerConfigXml::replaceApplicationPath(sServerCertificate, sApplicationPath);
                xmlElement.getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element ServerCertificate is missing");
                return OpcUa_BadInvalidArgument;
            }
            if ( 0 == xmlElement.getChild("ServerPrivateKey") )
            {
                xmlElement.getContent(value);
                sPrivateKey = value.pValue();
                ServerConfigXml::replaceApplicationPath(sPrivateKey, sApplicationPath);
                xmlElement.getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element ServerPrivateKey is missing");
                return OpcUa_BadInvalidArgument;
            }
            xmlElement.getParentNode();
        }
#if OPCUA_SUPPORT_PKI_WIN32
        else if ( 0 == xmlElement.getChild("WindowsStore") )
        {
            isOpenSSLStore = OpcUa_False;
#ifdef _WIN32
            if ( 0 == xmlElement.getChild("StoreLocation") )
            {
                xmlElement.getContent(value);
                UaString sTemp = value.pValue();
                if ( sTemp == "CurrentUser" )
                {
                    windowsStoreLocation = Location_CurrentUser;
                }
                else
                {
                    windowsStoreLocation = Location_LocalMachine;
                }
                xmlElement.getParentNode();
            }
#endif // _WIN32
            if ( 0 == xmlElement.getChild("StoreName") )
            {
                xmlElement.getContent(value);
                sTrustListLocation = value.pValue();
                xmlElement.getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element CertificateTrustListLocation is missing");
                return OpcUa_BadInvalidArgument;
            }
            if ( 0 == xmlElement.getChild("ServerCertificateThumbprint") )
            {
                xmlElement.getContent(value);
                sServerCertificate = value.pValue();
                xmlElement.getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element ServerCertificate is missing");
                return OpcUa_BadInvalidArgument;
            }
            xmlElement.getParentNode();
        }
#endif /* OPCUA_SUPPORT_PKI_WIN32 */
        else
        {
            TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element for a valid store like OpenSSLStore is missing");
            return OpcUa_BadInvalidArgument;
        }
        xmlElement.getParentNode();
    }
    else
    {
        TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element CertificateStore is missing");
        return OpcUa_BadInvalidArgument;
    }

    // Check if we have server certificate and private key
    OpcUa_Boolean certAvailable = OpcUa_True;
    if ( isOpenSSLStore != OpcUa_False )
    {
        FILE* pFile = fopen( sServerCertificate.toUtf8(), "r");
        if ( pFile != NULL )
        {
            fclose(pFile);
        }
        else
        {
            certAvailable = OpcUa_False;
        }
        pFile = fopen( sPrivateKey.toUtf8(), "r");
        if ( pFile != NULL )
        {
            fclose(pFile);
        }
        else
        {
            certAvailable = OpcUa_False;
        }
    }
    else
    {
#if OPCUA_SUPPORT_PKI_WIN32
        UaByteArray thumbprint = UaByteArray::fromHex(sServerCertificate);
        UaPkiCertificate tempServerCert = UaPkiCertificate::fromWindowsStore(windowsStoreLocation, sTrustListLocation, thumbprint);
        if ( tempServerCert.isValid() == false )
        {
            certAvailable = OpcUa_False;
        }
#endif // OPCUA_SUPPORT_PKI_WIN32
    }

    if ( certAvailable == OpcUa_False )
    {
        OpcUa_Boolean bCreateCertificate = OpcUa_False;
        UaString sCommonName;
        UaString sOrganization;
        UaString sOrganizationUnit;
        UaString sLocality;
        UaString sState;
        UaString sCountry;
        UaString sYearsValidFor;

        if ( 0 == xmlElement.getChild("CertificateStore") )
        {
            if ( 0 == xmlElement.getChild("GenerateCertificate") )
            {
                xmlElement.getContent(value);
                if ( value.pValue() != NULL )
                {
                    UaString sBool = value.pValue();
                    if ( sBool == "true" )
                    {
                        bCreateCertificate = OpcUa_True;
                    }
                    else
                    {
                        bCreateCertificate = OpcUa_False;
                    }
                }
                xmlElement.getParentNode();
            }
            if ( 0 == xmlElement.getChild("CertificateSettings") )
            {
                if ( 0 == xmlElement.getChild("CommonName") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sCommonName = value.pValue();
                        // Replace placeholder [ServerName]
                        ServerConfigXml::replaceServerName(sCommonName, &ltServerName[0].Text);
                        // Replace placeholder [NodeName]
                        ServerConfigXml::replaceNodeName(sCommonName, sNodeName);
                    }
                    xmlElement.getParentNode();
                }
                if ( 0 == xmlElement.getChild("Organization") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sOrganization = value.pValue();
                    }
                    xmlElement.getParentNode();
                }
                if ( 0 == xmlElement.getChild("OrganizationUnit") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sOrganizationUnit = value.pValue();
                    }
                    xmlElement.getParentNode();
                }
                if ( 0 == xmlElement.getChild("Locality") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sLocality = value.pValue();
                    }
                    xmlElement.getParentNode();
                }
                if ( 0 == xmlElement.getChild("State") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sState = value.pValue();
                    }
                    xmlElement.getParentNode();
                }
                if ( 0 == xmlElement.getChild("Country") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sCountry = value.pValue();
                    }
                    xmlElement.getParentNode();
                }
                if ( 0 == xmlElement.getChild("YearsValidFor") )
                {
                    xmlElement.getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        sYearsValidFor = value.pValue();
                    }
                    xmlElement.getParentNode();
                }
                xmlElement.getParentNode();
            }
            xmlElement.getParentNode();
        }
        if ( bCreateCertificate != OpcUa_False )
        {
            TRACE1_INFO(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=0x%lx] - loadCertificate failed - try to create one", ret.statusCode());

            if ( isOpenSSLStore != OpcUa_False )
            {
                // First make sure the directories exist
                UaDir dirHelper("");
                UaUniString usServerCertificatePath(dirHelper.filePath(UaDir::fromNativeSeparators(sServerCertificate.toUtf16())));
                dirHelper.mkpath(usServerCertificatePath);
                UaUniString usPrivateKeyPath(dirHelper.filePath(UaDir::fromNativeSeparators(sPrivateKey.toUtf16())));
                dirHelper.mkpath(usPrivateKeyPath);
            }

            // Create the certificate
            UaPkiRsaKeyPair keyPair ( 1024 );
            UaPkiPrivateKey IssuerPrivateKey;
            UaPkiPublicKey  SubjectPublicKey;
            UaPkiIdentity   identity;
            UaPkiCertificateCollection trusted, untrusted;
            
            identity.commonName       = sCommonName;
            identity.organization     = sOrganization;
            identity.organizationUnit = sOrganizationUnit;
            identity.locality         = sLocality;
            identity.state            = sState;
            identity.country          = sCountry;
            identity.domainComponent  = sNodeName;

            UaPkiCertificateInfo info;
            info.URI       = serverUri;
            info.DNS       = sNodeName;
            OpcUa_Int32 validTime = 3600*24*365*5; // 5 Years
            if (sYearsValidFor.length() > 0)
            {
                UaVariant uvValue;
                uvValue.setString(sYearsValidFor);
                OpcUa_Int32 nValue;
                if ( OpcUa_IsGood(uvValue.toInt32(nValue)) )
                {
                    if ( nValue > 20 )
                    {
                        nValue = 20;
                    }
                    validTime = 3600*24*365*nValue;
                }
            }
            info.validTime = validTime;

            IssuerPrivateKey = keyPair.privateKey();
            SubjectPublicKey = keyPair.publicKey();

            // create a self signed certificate
            UaPkiCertificate cert ( info, identity, SubjectPublicKey, identity, IssuerPrivateKey );

            if ( isOpenSSLStore != OpcUa_False )
            {
                // encoded to DER format
                cert.toDERFile ( sServerCertificate.toUtf8() );
                cert.validate( trusted, untrusted );
                
                keyPair.toPEMFile ( sPrivateKey.toUtf8(), 0 );
                UaByteArray DERData = keyPair.toDER();

                // Create revocation list
                FILE* pFile = fopen( sRevocationListLocation.toUtf8(), "r");
                if ( pFile != NULL )
                {
                    fclose(pFile);
                }
                else
                {
                    UaDir dirHelper("");
                    UaUniString usRevocationListPath(dirHelper.filePath(UaDir::fromNativeSeparators(sRevocationListLocation.toUtf16())));
                    dirHelper.mkpath(usRevocationListPath);
                    UaPkiRevocationList revocationList(info.validTime, identity, 1);
                    revocationList.sign(keyPair.privateKey());
                    revocationList.toPEMFile ( sRevocationListLocation.toUtf8() );
                }
            }
            else
            {
#if OPCUA_SUPPORT_PKI_WIN32
                UaByteArray thumbprint = cert.thumbPrint();
                sServerCertificate = thumbprint.toHex();
                if ( 0 == xmlElement.getChild("CertificateStore") )
                {
                    if ( 0 == xmlElement.getChild("WindowsStore") )
                    {
                        if ( 0 == xmlElement.getChild("ServerCertificateThumbprint") )
                        {
                            xmlElement.setContent((char*)sServerCertificate.toUtf8());
                            xmlElement.getParentNode();
                            updatedConfiguration = true;
                        }
                        xmlElement.getParentNode();
                    }
                    xmlElement.getParentNode();
                }
                cert.toWindowsStoreWithPrivateKey(windowsStoreLocation, sTrustListLocation, keyPair);
#endif // OPCUA_SUPPORT_PKI_WIN32
            }
        }
        else
        {
            ret = OpcUa_BadInternalError;
            TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInternalError] - Certificate not available");
            return ret;
        }
    }

    if ( isOpenSSLStore != OpcUa_False )
    {
        UaPkiProviderOpenSSL pkiProvider(
            sRevocationListLocation,
            sTrustListLocation);

        ret = pkiProvider.openCertificateStore();
        if ( ret.isNotGood() )
        {
            TRACE1_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=0x%lx] - openCertificateStore failed", ret.statusCode());
            return ret;
        }

        ret = pkiProvider.loadCertificate(
            sServerCertificate,
            serverCertificate);
        if ( ret.isNotGood() )
        {
            TRACE1_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=0x%lx] - loadCertificate failed", ret.statusCode());
            return ret;
        }

        UaString pw;
        ret = pkiProvider.loadPrivateKey(
            sPrivateKey,
            OpcUa_Crypto_Encoding_PEM,
            pw,
            serverPrivateKey);
        if ( ret.isNotGood() )
        {
            TRACE1_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=0x%lx] - loadPrivateKey failed", ret.statusCode());
            return ret;
        }
        pkiProvider.closeCertificateStore();

        setPkiConfiguration(serverCertificate, serverPrivateKey, &pkiProvider);
    }
    else
    {
#if OPCUA_SUPPORT_PKI_WIN32
        OpcUa_UInt32 windowsStoreLocationFlag = WIN32_PKI_MACHINESTORE;
        if ( windowsStoreLocation == Location_CurrentUser )
        {
            windowsStoreLocationFlag = WIN32_PKI_USERSTORE;
        }
        else
        {
            windowsStoreLocationFlag = WIN32_PKI_MACHINESTORE;
        }
        UaPkiProviderWindowsStore pkiProvider(windowsStoreLocationFlag, sTrustListLocation);

        // Use option to load certificate with SDK PKI classes
        // Other option is to use the PKI provider to load the certificates
        // This option does not work if the certificates are not in the "My" store since this store his hardcoded in loadPrivateKey
        UaPkiRsaKeyPair keyPair;
        UaByteArray thumbprint = UaByteArray::fromHex(sServerCertificate);
        UaPkiCertificate cert = UaPkiCertificate::fromWindowsStoreWithPrivateKey(windowsStoreLocation, sTrustListLocation, thumbprint, keyPair);
        if ( cert.isValid() == false )
        {
            TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadCertificateInvalid] - loadCertificate from Windows store failed");
            return OpcUa_BadCertificateInvalid;
        }

        UaByteArray certData;
        certData = cert.toDER();
        serverCertificate.setByteString(certData.size(), (OpcUa_Byte*)certData.data());

        UaByteArray certDataPrivate;
        certDataPrivate = keyPair.toDER();
        serverPrivateKey.setByteString(certDataPrivate.size(), (OpcUa_Byte*)certDataPrivate.data());

        setPkiConfiguration(serverCertificate, serverPrivateKey, &pkiProvider);
#else
        TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInternalError] - windows certificate store configured but not supported");
        return OpcUa_BadInternalError;
#endif /* OPCUA_SUPPORT_PKI_WIN32 */
    }
    /********************************************/
#else
    OpcUa_ReferenceParameter(sApplicationPath);
    OpcUa_ReferenceParameter(pServerConfig);

    UaByteString serverCertificate(ServerConfigData::s_defaultCertificateLength, ServerConfigData::s_pDefaultCertificateData);
    UaByteString serverPrivateKey(ServerConfigData::s_defaultPrivateKeyLength, ServerConfigData::s_pDefaultPrivateKeyData);

    UaPkiProviderNoSecurity pkiProvider;
    setPkiConfiguration(serverCertificate, serverPrivateKey, &pkiProvider);
#endif // OPCUA_SUPPORT_PKI

    /********************************************/
    // Get the Endpoint Serializer Type
    if ( 0 == xmlElement.getChild("SerializerType") )
    {
        xmlElement.getContent(value);
        if ( (value.pValue() != NULL) && (strcmp("Binary", value.pValue()) == 0) )
        {
            setSerializerType(OpcUa_Endpoint_SerializerType_Binary);
        }
        else if ( (value.pValue() != NULL) && (strcmp("Xml", value.pValue()) == 0) )
        {
            setSerializerType(OpcUa_Endpoint_SerializerType_Xml);
        }
        else
        {
            TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element SerializerType is not Binary or Xml");
            return OpcUa_BadInvalidArgument;
        }
        xmlElement.getParentNode();
    }
    else
    {
        // Set default binary
        setSerializerType(OpcUa_Endpoint_SerializerType_Binary);
    }
    /********************************************/

    /********************************************/
    // Get the Endpoint SecuritySetting
    UaXmlElementList uaXmlElementListSettings;
    if ( 0 == xmlElement.getChildElements("SecuritySetting", uaXmlElementListSettings) )
    {
        UaXmlElementListIterator itSettings;
        for ( itSettings = uaXmlElementListSettings.begin(); itSettings != uaXmlElementListSettings.end(); ++itSettings)
        {
            UaEndpointSecuritySetting securitySetting;

            /********************************************/
            // Get the Endpoint Security Policy
            if ( 0 == (*itSettings).getChild("SecurityPolicy") )
            {
                (*itSettings).getContent(value);
                securitySetting.setSecurityPolicy(value.pValue());
                (*itSettings).getParentNode();
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element SecurityPolicy is missing");
                return OpcUa_BadInvalidArgument;
            }
            /********************************************/

            /********************************************/
            // Get the Endpoint Message Security Mode
            UaXmlElementList uaXmlElementList;
            if ( 0 == (*itSettings).getChildElements("MessageSecurityMode", uaXmlElementList) )
            {
                UaXmlElementListIterator it;
                for ( it = uaXmlElementList.begin(); it != uaXmlElementList.end(); ++it)
                {
                    (*it).getContent(value);
                    if ( value.pValue() != NULL )
                    {
                        if ( (strcmp("None", value.pValue()) == 0) )
                        {
                            securitySetting.addMessageSecurityMode(OPCUA_ENDPOINT_MESSAGESECURITYMODE_NONE);
                        }
                        else if ( (strcmp("Sign", value.pValue()) == 0) )
                        {
                            securitySetting.addMessageSecurityMode(OPCUA_ENDPOINT_MESSAGESECURITYMODE_SIGN);
                        }
                        else if ( (strcmp("SignAndEncrypt", value.pValue()) == 0) )
                        {
                            securitySetting.addMessageSecurityMode(OPCUA_ENDPOINT_MESSAGESECURITYMODE_SIGNANDENCRYPT);
                        }
                        else
                        {
                            TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element MessageSecurityMode is not None, Sign or SignAndEncrypt");
                            return OpcUa_BadInvalidArgument;
                        }
                    }
                    else
                    {
                        TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element in list MessageSecurityMode is empty");
                        return OpcUa_BadInvalidArgument;
                    }
                }
            }
            else
            {
                TRACE0_ERROR(SERVER_UI, "Error: UaEndpointXml::setXmlConfig [ret=OpcUa_BadInvalidArgument] - XML Element MessageSecurityMode is missing");
                return OpcUa_BadInvalidArgument;
            }
            /********************************************/

            addSecuritySetting(securitySetting);
        }
    }
    else
    {
        UaEndpointSecuritySetting securitySetting;
        OpcUa_UInt32 configDefaultSecure = CONFIG_DEFAULT_SECURE;
        if ( configDefaultSecure == 0 )
        {
            securitySetting.setSecurityPolicy(OpcUa_SecurityPolicy_None);
            securitySetting.addMessageSecurityMode(OPCUA_ENDPOINT_MESSAGESECURITYMODE_NONE);
        }
        else
        {
            securitySetting.setSecurityPolicy(OpcUa_SecurityPolicy_Basic128Rsa15);
            securitySetting.addMessageSecurityMode(OPCUA_ENDPOINT_MESSAGESECURITYMODE_SIGNANDENCRYPT);
        }
        addSecuritySetting(securitySetting);
    }
    /********************************************/

    return ret;
}


#endif // UA_API_VERSION < 140

#endif //  BACKEND_OPEN62541

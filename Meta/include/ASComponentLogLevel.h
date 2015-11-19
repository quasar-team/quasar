/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASComponentLogLevel.h
 *
 *  Created on: Aug 18, 2015
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASComponentLogLevel__H__
#define __ASComponentLogLevel__H__

#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

#include <Configuration.hxx>

#include <ASNodeManager.h>
#include <ASDelegatingVariable.h>
#include <ASSourceVariable.h>

/* forward declaration */
namespace Device {
class DComponentLogLevel;
}


namespace AddressSpace
{
//! Fully auto-generated class to represent ComponentLogLevel in the OPC UA AddressSpace
class ASComponentLogLevel: public OpcUa::BaseObjectType
{
    UA_DISABLE_COPY(ASComponentLogLevel);
public:

    //! Constructor. Used in Configurator.cpp. You NEVER use it directly.
    ASComponentLogLevel (UaNodeId parentNodeId, const UaNodeId& typeNodeId, ASNodeManager *nm, const std::string& componentName, const std::string& logLevel);
    ~ASComponentLogLevel ();

    /* setters and getters for variables */
    UaStatus getLogLevel (UaString &) const ;
    UaStatus setLogLevel (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime = UaDateTime::now()) ;

    /* short getter (possible because nullPolicy=nullForbidden) */
    UaString getLogLevel () const;

    /* delegators for cachevariables  */

    //! This function will be called when OPC UA write request comes. Never directly call it.

    /* NOTE: This function is not intended to be used by human being. */
    UaStatus writeLogLevel (Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel= OpcUa_True);

    /* Device Logic setter (if requested) */
    void linkDevice( Device::DComponentLogLevel *deviceLink);
    void unlinkDevice ();
    Device::DComponentLogLevel * getDeviceLink () const {return m_deviceLink;}

    /* OPC UA Type Information provider for this class. */
    virtual UaNodeId typeDefinitionId () const {return m_typeNodeId;}

private:
    UaNodeId m_typeNodeId;

    /* Variables */
    ASDelegatingVariable<ASComponentLogLevel> * m_logLevel;


    /* Device Logic link (if requested) */
    Device::DComponentLogLevel *m_deviceLink;
};

}

#endif // include guard

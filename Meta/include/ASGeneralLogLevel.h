/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASGeneralLogLevel.h
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

#ifndef __ASGeneralLogLevel__H__
#define __ASGeneralLogLevel__H__


#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

#include <Configuration.hxx>

#include <ASNodeManager.h>
#include <ASDelegatingVariable.h>
#include <ASSourceVariable.h>


/* forward declaration */
namespace Device {
class DGeneralLogLevel;
}


namespace AddressSpace
{
//! Fully auto-generated class to represent GeneralLogLevel in the OPC UA AddressSpace
class ASGeneralLogLevel: public OpcUa::BaseObjectType
{
    UA_DISABLE_COPY(ASGeneralLogLevel);
public:

    ASGeneralLogLevel (UaNodeId parentNodeId, const UaNodeId& typeNodeId, ASNodeManager *nm, const std::string& logLevel);
    virtual ~ASGeneralLogLevel ();

    UaStatus getLogLevel (UaString &) const ;
    UaStatus setLogLevel (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime = UaDateTime::now()) ;

    UaString getLogLevel () const;

    UaStatus writeLogLevel (Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel= OpcUa_True);

    void linkDevice( Device::DGeneralLogLevel *deviceLink);
    void unlinkDevice ();
    Device::DGeneralLogLevel * getDeviceLink () const {return m_deviceLink;}

    /* OPC UA Type Information provider for this class. */
    virtual UaNodeId typeDefinitionId () const {return m_typeNodeId;}

private:
    UaNodeId m_typeNodeId;
    ASDelegatingVariable<ASGeneralLogLevel> * m_logLevel;

    Device::DGeneralLogLevel *m_deviceLink;
};

}



#endif // include guard


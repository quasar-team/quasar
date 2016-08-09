/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * ASServer.h
 *
 *  Created on: Nov 5, 2015
 * 		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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

#ifndef __ASServer__H__
#define __ASServer__H__


#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

#include <Configuration.hxx>

#include <ASNodeManager.h>
#include <ASDelegatingVariable.h>
#include <ASSourceVariable.h>


/* forward declaration */
namespace Device {
class DServer;
}


namespace AddressSpace
{
//! Fully auto-generated class to represent Server in the OPC UA AddressSpace
class ASServer: public OpcUa::BaseObjectType
{
    UA_DISABLE_COPY(ASServer);
public:



    //! Constructor. Used in Configurator.cpp. You NEVER use it directly.
    ASServer (
        UaNodeId parentNodeId,
        const UaNodeId& typeNodeId,
        ASNodeManager *nm,
        const Configuration::Server &config);



    /*dtor*/
    ~ASServer ();



    /* setters and getters for variables */



    UaStatus getConnectedClientCount (OpcUa_UInt32 &) const ;
    UaStatus setConnectedClientCount (const OpcUa_UInt32 value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime = UaDateTime::now()) ;
    /* short getter (possible because nullPolicy=nullForbidden) */
    OpcUa_UInt32 getConnectedClientCount () const;

    UaStatus getRemainingCertificateValidity(UaString &) const ;
    UaStatus setRemainingCertificateValidity(const UaString& value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime = UaDateTime::now()) ;
    /* short getter (possible because nullPolicy=nullForbidden) */
    UaString getRemainingCertificateValidity() const;



    /* delegators for cachevariables  */




    /* Device Logic setter (if requested) */

    void linkDevice( Device::DServer *deviceLink);
    void unlinkDevice ();
    Device::DServer * getDeviceLink () const {
        return m_deviceLink;
    }

    /* OPC UA Type Information provider for this class. */
    virtual UaNodeId typeDefinitionId () const {
        return m_typeNodeId;
    }
private:
    UaNodeId m_typeNodeId;
    /* Variables */
    OpcUa::BaseDataVariableType
    * m_connectedClientCount;
    OpcUa::BaseDataVariableType
    * m_remainingCertificateValidity;


    /* Device Logic link (if requested) */

    Device::DServer *m_deviceLink;




};

}



#endif // include guard


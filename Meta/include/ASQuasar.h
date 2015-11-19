/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * ASQuasar.h
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

#ifndef __ASQuasar__H__
#define __ASQuasar__H__


#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

#include <Configuration.hxx>

#include <ASNodeManager.h>
#include <ASDelegatingVariable.h>
#include <ASSourceVariable.h>


/* forward declaration */
namespace Device {
class DQuasar;
}


namespace AddressSpace
{
//! Fully auto-generated class to represent Quasar in the OPC UA AddressSpace
class ASQuasar: public OpcUa::BaseObjectType
{
    UA_DISABLE_COPY(ASQuasar);
public:



    //! Constructor. Used in Configurator.cpp. You NEVER use it directly.
    ASQuasar (
        UaNodeId parentNodeId,
        const UaNodeId& typeNodeId,
        ASNodeManager *nm,
        const Configuration::Quasar &config);



    /*dtor*/
    ~ASQuasar ();



    /* setters and getters for variables */



    UaStatus getVersion (UaString &) const ;
    UaStatus setVersion (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime = UaDateTime::now()) ;



    /* short getter (possible because nullPolicy=nullForbidden) */
    UaString getVersion () const;




    /* delegators for cachevariables  */




    /* Device Logic setter (if requested) */

    void linkDevice( Device::DQuasar *deviceLink);
    void unlinkDevice ();
    Device::DQuasar * getDeviceLink () const {
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
    * m_version;


    /* Device Logic link (if requested) */

    Device::DQuasar *m_deviceLink;




};

}



#endif // include guard


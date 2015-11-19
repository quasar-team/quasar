/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASStandardMetaData.h
 *
 *  Created on: Aug 18, 2015
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
 *      Author: Piotr Nikiel <piotr@nikiel.info>
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
#ifndef __ASStandardMetaData__H__
#define __ASStandardMetaData__H__


#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

#include <Configuration.hxx>

#include <ASNodeManager.h>
#include <ASDelegatingVariable.h>
#include <ASSourceVariable.h>


/* forward declaration */
namespace Device {
class DStandardMetaData;
}


namespace AddressSpace
{
//! Fully auto-generated class to represent StandardMetaData in the OPC UA AddressSpace
class ASStandardMetaData: public OpcUa::BaseObjectType
{
    UA_DISABLE_COPY(ASStandardMetaData);
public:



    //! Constructor. Used in Configurator.cpp. You NEVER use it directly.
    ASStandardMetaData (
        UaNodeId parentNodeId,
        const UaNodeId& typeNodeId,
        ASNodeManager *nm,
        const Configuration::StandardMetaData &config);



    /*dtor*/
    ~ASStandardMetaData ();



    /* setters and getters for variables */




    /* delegators for cachevariables  */




    /* Device Logic setter (if requested) */

    void linkDevice( Device::DStandardMetaData *deviceLink);
    void unlinkDevice ();
    Device::DStandardMetaData * getDeviceLink () const {
        return m_deviceLink;
    }

    /* OPC UA Type Information provider for this class. */
    virtual UaNodeId typeDefinitionId () const {
        return m_typeNodeId;
    }
private:
    UaNodeId m_typeNodeId;
    /* Variables */


    /* Device Logic link (if requested) */

    Device::DStandardMetaData *m_deviceLink;




};

}



#endif // include guard


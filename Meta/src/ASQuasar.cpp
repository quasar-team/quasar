/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * ASQuasar.cpp
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
#include <iostream>
#include <ASQuasar.h>
#include <DQuasar.h>

namespace AddressSpace
{



/*ctr*/
ASQuasar::ASQuasar (
    UaNodeId parentNodeId,
    const UaNodeId& typeNodeId,
    ASNodeManager *nm,
    const Configuration::Quasar & config):
    OpcUa::BaseObjectType (
        /*nodeId*/nm->makeChildNodeId(parentNodeId,"Quasar"), "Quasar", nm->getNameSpaceIndex(), nm),
    m_typeNodeId (typeNodeId)


    ,
    m_version (new

               OpcUa::BaseDataVariableType


               (nm->makeChildNodeId(this->nodeId(),UaString("version")), UaString("version"), nm->getNameSpaceIndex(), UaVariant(

                    UaString( "dummy version" )

                ),

                OpcUa_AccessLevels_CurrentRead
                , nm))



    ,
    m_deviceLink (0)


{
    UaStatus s;
    UaVariant v;

    s = nm->addNodeAndReference( parentNodeId, this, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << parentNodeId.toString().toUtf8() << " to " << this->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }

    v.setString
    ( "dummy version" );

    m_version->setValue(/*pSession*/0, UaDataValue(UaVariant( v ),
                                    OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);

    s = nm->addNodeAndReference(this, m_version, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_version->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }



}





ASQuasar::~ASQuasar ()
{

    if (m_deviceLink != 0)
    {
        PRINT("deviceLink not zero!!");
        PRINT_LOCATION();

    }

}




/* generate setters and getters (if requested) */


UaStatus ASQuasar::setVersion (const UaString & value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{
    UaVariant v;

    v.setString( value );



    return m_version->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}

UaStatus ASQuasar::getVersion (UaString & r) const
{
    UaVariant v (* (m_version->value(/*session*/0).value()));

    if (v.type() == OpcUaType_String)
    {
        r = v.toString();
        return OpcUa_Good;
    }
    else
        return OpcUa_Bad;

}


/* short getter (possible because this variable will never be null) */
UaString ASQuasar::getVersion () const
{
    UaVariant v (* m_version->value (0).value() );
    UaString v_value;
    v_value = v.toString();
    return v_value;
}





/* generate delegates (if requested) */





/* generate device logic link */


void ASQuasar::linkDevice( Device::DQuasar *deviceLink)
{
    if (m_deviceLink != 0)
    {
        /* This is an error -- device can be linked at most in the object's lifetime. */
        //TODO After error handling is discussed, abort in smarter way
        abort();

    }
    else
        m_deviceLink = deviceLink;
}


void ASQuasar::unlinkDevice ()
{
    m_deviceLink = 0;
}


}


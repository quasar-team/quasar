/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASSourceVariableThreadPool.cpp
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
#include <iostream>




#include <ASSourceVariableThreadPool.h>


#include <DSourceVariableThreadPool.h>


namespace AddressSpace
{



/*ctr*/
ASSourceVariableThreadPool::ASSourceVariableThreadPool (
    UaNodeId parentNodeId,
    const UaNodeId& typeNodeId,
    ASNodeManager *nm,
    const size_t& min,
	const size_t& max):
    OpcUa::BaseObjectType (
    /*nodeId*/nm->makeChildNodeId(parentNodeId,"SourceVariableThreadPool"), "SourceVariableThreadPool", nm->getNameSpaceIndex(), nm),
    m_typeNodeId (typeNodeId)


    ,
    m_minThreads (new

                  OpcUa::BaseDataVariableType


                  (nm->makeChildNodeId(this->nodeId(),UaString("minThreads")), UaString("minThreads"), nm->getNameSpaceIndex(), UaVariant(

                       static_cast<OpcUa_UInt32>(min)
                   ),

                   OpcUa_AccessLevels_CurrentRead
                   , nm))



    ,
    m_maxThreads (new

                  OpcUa::BaseDataVariableType


                  (nm->makeChildNodeId(this->nodeId(),UaString("maxThreads")), UaString("maxThreads"), nm->getNameSpaceIndex(), UaVariant(

                		  static_cast<OpcUa_UInt32>(max)
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

    v.setUInt32 (
    		static_cast<OpcUa_UInt32>(min)
    );
    m_minThreads->setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);

    s = nm->addNodeAndReference(this, m_minThreads, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_minThreads->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }


    v.setUInt32 (
    		static_cast<OpcUa_UInt32>(max)
    );
    m_maxThreads->setValue(/*pSession*/0, UaDataValue(UaVariant( v ), OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);

    s = nm->addNodeAndReference(this, m_maxThreads, OpcUaId_HasComponent);
    if (!s.isGood())
    {
        std::cout << "While addNodeAndReference from " << this->nodeId().toString().toUtf8() << " to " << m_maxThreads->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(s);
    }



}





ASSourceVariableThreadPool::~ASSourceVariableThreadPool ()
{

    if (m_deviceLink != 0)
    {
        PRINT("deviceLink not zero!!");
        PRINT_LOCATION();

    }

}




/* generate setters and getters (if requested) */


UaStatus ASSourceVariableThreadPool::setMinThreads (const OpcUa_UInt32 value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{
    UaVariant v;

    v.setUInt32( value );



    return m_minThreads->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}

UaStatus ASSourceVariableThreadPool::getMinThreads (OpcUa_UInt32 & r) const
{
    UaVariant v (* (m_minThreads->value(/*session*/0).value()));
    return v.toUInt32 ( r );
}


/* short getter (possible because this variable will never be null) */
OpcUa_UInt32 ASSourceVariableThreadPool::getMinThreads () const
{
    UaVariant v (* m_minThreads->value (0).value() );
    OpcUa_UInt32 v_value;
    v.toUInt32 ( v_value );
    return v_value;
}


UaStatus ASSourceVariableThreadPool::setMaxThreads (const OpcUa_UInt32 value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{
    UaVariant v;

    v.setUInt32( value );



    return m_maxThreads->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;

}

UaStatus ASSourceVariableThreadPool::getMaxThreads (OpcUa_UInt32 & r) const
{
    UaVariant v (* (m_maxThreads->value(/*session*/0).value()));
    return v.toUInt32 ( r );
}


/* short getter (possible because this variable will never be null) */
OpcUa_UInt32 ASSourceVariableThreadPool::getMaxThreads () const
{
    UaVariant v (* m_maxThreads->value (0).value() );
    OpcUa_UInt32 v_value;
    v.toUInt32 ( v_value );
    return v_value;
}





/* generate delegates (if requested) */





/* generate device logic link */


void ASSourceVariableThreadPool::linkDevice( Device::DSourceVariableThreadPool *deviceLink)
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


void ASSourceVariableThreadPool::unlinkDevice ()
{
    m_deviceLink = 0;
}


}


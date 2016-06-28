/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * ASServer.cpp
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
#include <ASServer.h>
#include <DServer.h>

#include <ASNodeManager.h>

namespace AddressSpace
{



/*ctr*/
ASServer::ASServer ( UaNodeId parentNodeId,
		const UaNodeId& typeNodeId,
		ASNodeManager *nm,
		const Configuration::Server & config): OpcUa::BaseObjectType (
				/*nodeId*/ nm->makeChildNodeId(parentNodeId,"Server"),
				"Server", nm->getNameSpaceIndex(), nm),
				m_typeNodeId (typeNodeId),
				m_connectedClientCount ( new OpcUa::BaseDataVariableType( nm->makeChildNodeId(this->nodeId(), UaString("connectedClientCount")),
											UaString("connectedClientCount"),
											nm->getNameSpaceIndex(),
											UaVariant(OpcUa_UInt32( 0 )),
										OpcUa_AccessLevels_CurrentRead, nm)),
				m_certValidityRemaining ( new OpcUa::BaseDataVariableType( nm->makeChildNodeId(this->nodeId(), UaString("m_certValidityRemaining")),
											UaString("m_certValidityRemaining"),
											nm->getNameSpaceIndex(),
											UaVariant(OpcUa_UInt32( 0 )),
										OpcUa_AccessLevels_CurrentRead, nm)),
		m_deviceLink (0)

{
}





ASServer::~ASServer ()
{

    if (m_deviceLink != 0)
    {
        PRINT("deviceLink not zero!!");
        PRINT_LOCATION();

    }

}




/* generate setters and getters (if requested) */


UaStatus ASServer::setConnectedClientCount (const OpcUa_UInt32 value, OpcUa_StatusCode statusCode,const UaDateTime & srcTime )
{
    UaVariant v;
    v.setUInt32( value );
    return m_connectedClientCount->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
}

UaStatus ASServer::getConnectedClientCount (OpcUa_UInt32 & r) const
{
    UaVariant v (* (m_connectedClientCount->value(/*session*/0).value()));
    return v.toUInt32 ( r );
}


/* short getter (possible because this variable will never be null) */
OpcUa_UInt32 ASServer::getConnectedClientCount () const
{
    UaVariant v (* m_connectedClientCount->value (0).value() );
    OpcUa_UInt32 v_value;
    v.toUInt32 ( v_value );
    return v_value;
}

UaStatus ASServer::setCertValidityRemaining (const std::string value, OpcUa_StatusCode statusCode, const UaDateTime & srcTime )
{
    UaVariant v;
    v.setString( value.c_str() );
    return m_certValidityRemaining->setValue (0, UaDataValue (v, statusCode, srcTime, UaDateTime::now()), /*check access*/OpcUa_False  ) ;
}

void ASServer::getCertValidityRemaining ( std::string & s) const
{
    UaVariant v (* (m_certValidityRemaining->value(/*session*/0).value()));
    s = v.toString().toUtf8();
}





/* generate delegates (if requested) */





/* generate device logic link */


void ASServer::linkDevice( Device::DServer *deviceLink)
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


void ASServer::unlinkDevice ()
{
    m_deviceLink = 0;
}



/// add and connect the BaseDataVariables, which are initialized using variants to the specified parent node
/// this does not work in the constructor, that is why the code is separated into here
void ASServer::connectStandardMetaVariables( AddressSpace::ASNodeManager *nm,
		UaVariant v_connectedClientCount,
		UaVariant v_certValidityRemaining,
		UaNode *parentNode )
{
	m_connectedClientCount->setValue(/*pSession*/0, UaDataValue( v_connectedClientCount, OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);
	UaStatus s = nm->addNodeAndReference( parentNode, m_connectedClientCount, OpcUaId_HasComponent);
	if (!s.isGood())
	{
		LOG(Log::ERR) << "While connectVariable from "
				<< parentNode->nodeId().toString().toUtf8() << " to "
				<< m_connectedClientCount->browseName().unqualifiedName().toUtf8();
		ASSERT_GOOD(s);
	}

	m_certValidityRemaining->setValue(/*pSession*/0, UaDataValue( v_certValidityRemaining, OpcUa_Good, UaDateTime::now(), UaDateTime::now() ), /*check access level*/OpcUa_False);
	s = nm->addNodeAndReference( parentNode, m_certValidityRemaining, OpcUaId_HasComponent);
	if (!s.isGood())
	{
		LOG(Log::ERR) << "While connectVariable from "
				<< parentNode->nodeId().toString().toUtf8() << " to "
				<< m_certValidityRemaining->browseName().unqualifiedName().toUtf8();
		ASSERT_GOOD(s);
	}
}

} // namespace

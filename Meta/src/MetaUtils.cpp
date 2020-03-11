/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * MetaUtils.h
 *
 *  Created on: Aug 18, 2015
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
 *		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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
#include "MetaUtils.h"
#include "Certificate.h"

// debug
#define BACKEND_UATOOLKIT 1

// serverconfig is implemented by ua toolkit in 1.5.5 and later
#ifdef BACKEND_UATOOLKIT
#include <servermanager.h>
#include <serverconfig.h>

#endif

using std::string;

Device::DServer* g_dServer = 0;

void MetaUtils::assertNodeAdded(const UaStatus& status, const UaNodeId& parentNodeId, const UaNodeId& childNodeId)
{
    if (!status.isGood())
    {
        std::cout << "While addNodeAndReference from " << parentNodeId.toString().toUtf8() << " to " << childNodeId.toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(status);
    }
}


void MetaUtils::setDServer(Device::DServer* ser)
{
	g_dServer = ser;
}

string MetaUtils::calculateRemainingCertificateValidity(void)
{
	std::vector<std::string> vfn = MetaUtils::readCertAndPkeyFilenameFromServerConfig();
	if ( vfn.size() < 2 ){
		vfn[0] = Certificate::DEFAULT_PUBLIC_CERT_FILENAME;
		vfn[1] = Certificate::DEFAULT_PRIVATE_CERT_FILENAME;
		LOG(Log::WRN) << "could not find cert file names in server config Certificate Store, taking defaults instead.";
	}
	Certificate::Instance( vfn )->init();
	return Certificate::Instance()->remainingTime();
}

// return the first certificate filename
// we can have several enpoints, but ignore them for now, just take the first
// code is disabled anyway for now
std::vector<string> MetaUtils::readCertAndPkeyFilenameFromServerConfig( void ){
	std::vector<string> vfn;

#if BACKEND_UATOOLKIT
	// need the server config
	ServerConfig* serverConfig = ServerManager().getServerConfig();
	UaStatus stat = serverConfig->loadConfiguration();
	UaString sRejectedCertificateDirectory;
	OpcUa_UInt32 nRejectedCertificateCount;
	UaEndpointArray uaEndpointArray;
	serverConfig->getEndpointConfiguration(
			sRejectedCertificateDirectory,
			nRejectedCertificateCount,
			uaEndpointArray);

	for ( OpcUa_UInt32 idx=0; idx<uaEndpointArray.length(); idx++ ) {
		LOG(Log::DBG) << "Opened endpoint: " << uaEndpointArray[idx]->sEndpointUrl().toUtf8();

		CertificateStoreConfiguration *pstore = uaEndpointArray[idx]->pEndpointCertificateStore();
		LOG(Log::DBG) << "certificate count : " << pstore->certificateCount();
		for ( OpcUa_UInt32 k = 0; k < pstore->certificateCount(); k++ ){
			if ( pstore->getCertificate( k )->isCertificateAvailable() ){
				LOG(Log::DBG) << "certificate location : " <<
						pstore->getCertificate( k )->m_sCertificateLocation.toUtf8();

				LOG(Log::DBG) << "priv key location : " <<
						pstore->getCertificate( k )->m_sPrivateKeyLocation.toUtf8();

				vfn.push_back( pstore->getCertificate( k )->m_sCertificateLocation.toUtf8() );
				vfn.push_back( pstore->getCertificate( k )->m_sPrivateKeyLocation.toUtf8() );
			} else {
				LOG(Log::WRN) << "certificate " << k << " is not available from Certificate Store";
			}
		}
		// we can do much more with the cert store at this point, and provide certain functionality
		// in the OPCUA address space as well. Up to the developer.
	}
#endif
	return( vfn );
}




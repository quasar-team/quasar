/*
 * Certificate.h
 *
 *  Created on: Jun 29, 2016
 *      Author: mludwig
 */

#ifndef META_SRC_CERTIFICATE_H_
#define META_SRC_CERTIFICATE_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <ctime>
#include <cstdlib>
#include <stdint.h>


#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/asn1.h>

using namespace std;

/// load
class Certificate {
public:
    /// behavior:
    /// NONE: don't attempt to read any certificates, do nothing. Validity is unknown.
    /// TRY:  if cert file exists, load it, otherwise report and continue
    /// TRYCA: like TRY, but with CA and a chain
    /// CERT: load a crt from file, abort if problem
    /// CERTCA: like CERT, but with CA load and interrogation
	/// we can discuss this ;-)
	enum behaviour_t { BEHAVIOR_NONE, BEHAVIOR_TRY, BEHAVIOR_TRYCA, BEHAVIOR_CERT, BEHAVIOR_CERTCA };

	enum status_t { STATUS_OK, STATUS_FAILED, STATUS_UNKNOWN };

	const static std::string DEFAULT_PUBLIC_CERT_FILENAME;
	const static std::string DEFAULT_PRIVATE_CERT_FILENAME;

	static Certificate* Instance( string certfn, string privkeyfn, enum behaviour_t beh );
	static Certificate* Instance( );

	virtual ~Certificate();
	int init( void );
	string remainingTime( void );
	void setTypeDER( void ) { m_type = SSL_FILETYPE_ASN1; }
	void setTypePEM( void ) { m_type = SSL_FILETYPE_PEM; }

private:
	Certificate( string certfn, string privkeyfn, enum behaviour_t beh  );	// singleton
	Certificate( Certificate const&);                            // copy constructor absent (i.e. cannot call it - linker will fail).
	Certificate& operator=(Certificate const&);  		// assignment operator absent (i.e. cannot call it - linker will fail).
	static Certificate* _pInstance;

	int remainingDays( void )  const;
	int remainingHours( void ) const;
	int remainingMins( void ) const;
	int remainingSecs( void ) const;
	int loadPrivateKeyFromFile( void);
	int loadCertificateFromFile( void );
	void remainingValidityTime( void );

	/**
	 * checks that certificate file name is valid and file exists and can be opened.
	 * returns:
	 * 0=OK
	 * -1: file i/o error, not found
	 * -2: filename must have a length of at least 5 characters, like i.e. "a.pem"
	 */
	int validateCertificateFilename(const std::string& certificateFilename) const;
	time_t _timeASN1toTIME_T( ASN1_TIME* time );

	const string m_certfn;
	const string m_privkeyfn;
	const behaviour_t m_behaviour;
	int m_type;

	SSL *m_ssl;
	std::time_t m_time_end;
	status_t m_status;

	int64_t m_remaining_validity_in_seconds;
};

#endif /* META_SRC_CERTIFICATE_H_ */

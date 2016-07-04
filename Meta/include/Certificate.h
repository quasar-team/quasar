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
#include <sys/time.h>


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

	static Certificate* Instance( string certfn, string privkeyfn, enum behaviour_t beh )	{
		if (!_pInstance)   // Only allow one instance of class to be generated.
			_pInstance = new Certificate( certfn, privkeyfn, beh );
		return _pInstance;
	}
	static Certificate* Instance( void );

	virtual ~Certificate();
	int init( void );
	string remainingTime( void );
	void setTypeDER( void ) { _type = SSL_FILETYPE_ASN1; }
	void setTypePEM( void ) { _type = SSL_FILETYPE_PEM; }

private:
	Certificate( string certfn, string privkeyfn, enum behaviour_t beh  );	// singleton
	Certificate( Certificate const&){};                                 // copy constructor is private
	Certificate& operator=(Certificate const&){ return *_pInstance; };  // assignment operator is private
	static Certificate* _pInstance;

	int remainingDays( void )  { return _remainingdays; }
	int remainingHours( void ) { return _remaininghours; }
	int remainingMins( void )  { return _remainingmins; }
	int remainingSecs( void )  { return _remainingsecs; }
	int loadPrivateKeyFromFile( void);
	int loadCertificateFromFile( void );
	void remainingValidityTime( void );
	time_t _timeASN1toTIME_T( ASN1_TIME* time );

	SSL_CTX *_ssl_ctx;
	SSL *_ssl;
	int _type;
	X509 *_x509crt;
	time_t *_ptimeNotBefore; // valid not before
	time_t *_ptimeNotAfter;  // valid not after
	std::time_t _time_end;
	std::time_t _time_beg;
	int _valid;
	behaviour_t _behaviour;
	status_t _status;

	ASN1_TIME *_nbefore;
	ASN1_TIME *_nafter;
	int _totaldays;
	int _totalhours;
	int _totalmins;
	int _totalsecs;

	int _remainingdays;
	int _remaininghours;
	int _remainingmins;
	int _remainingsecs;

	string _certfn;
	string _privkeyfn;
	string _csrfn;
	int _validdays;
};

#endif /* META_SRC_CERTIFICATE_H_ */

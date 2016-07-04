/*
 * Certificate.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: mludwig
 */

#include <iomanip>

#include <Certificate.h>

#include <LogIt.h>
#include <LogLevels.h>

// singleton
Certificate* Certificate::_pInstance = NULL;


Certificate::Certificate( string certfn, string privkeyfn, enum behaviour_t beh  ) {
	// _type = SSL_FILETYPE_PEM;
	_type = SSL_FILETYPE_ASN1; // DER
	_x509crt = NULL;
	_ptimeNotBefore = NULL;
	_ptimeNotAfter = NULL;
	_time_end = NULL;
	_time_beg = NULL;
	_valid = false;
	_totaldays = 0;
	_totalhours = 0;
	_totalmins = 0;
	_totalsecs = 0;
	_remainingdays = 0;
	_remaininghours = 0;
	_remainingmins = 0;
	_remainingsecs = 0;
	_certfn = certfn;
	_privkeyfn = privkeyfn;
	_csrfn = "server.csr"; // certificate signing request filename: intermediate step
	_validdays = 3;        // for self-signed certificates from scratch
	_behaviour = beh;
	_nafter = NULL;
	_nbefore = NULL;
	_ssl_ctx = NULL;
	_ssl = NULL;
	if ( _behaviour != BEHAVIOR_NONE ) {
		_ssl_ctx = SSL_CTX_new( SSLv23_method() );
		_ssl = SSL_new( _ssl_ctx );
		SSL_load_error_strings();            /* readable error messages */
		SSL_library_init();                  /* initialize library */
	}
}

Certificate::~Certificate() {
	SSL_shutdown( _ssl );
}


/// loads one private key
// returns: 0=OK
// -1: file i/o error, not found
// -2: filename must have a length of at least 5 characters, like i.e. "a.pem"
int Certificate::loadPrivateKeyFromFile( void ) {
	string fname = _privkeyfn;
	if ( fname.length() < 5 ) {
		LOG(Log::ERR) << "refuse to open certificate " << fname << " : filename seems to short ( \"a.der\" at least )";
		return(-2);
	}
	ifstream ftest;
	string line;
	ftest.open ( fname.c_str() );
	if ( ftest.is_open() )	{
		while ( getline (ftest ,line) )	{
			// cout << __FILE__ << " " << __LINE__ << " line= " << line << endl;
		}
		ftest.close();
	} else {
		LOG(Log::ERR) << "unable to open file  " << fname;
		return(-1);
	}
	_ssl_ctx = SSL_CTX_new( SSLv23_method() );
	_ssl = SSL_new( _ssl_ctx );
	SSL_use_PrivateKey_file( _ssl, fname.c_str(), _type );
	return( 0 );
}

/// loads one PEM file with one certificate, no chain, no CA
// returns: 0=OK
// -1: file i/o error, not found
// -2: filename must have a length of at least 5 characters, like i.e. "a.pem"
int Certificate::loadCertificateFromFile( void ) {
	string fname = _certfn;
	if ( fname.length() < 5 ) {
		LOG(Log::ERR) << "refuse to open certificate " << fname << " : filename seems to short ( \"a.der\" at least )";
		return(-2);
	}
	ifstream ftest;
	string line;
	ftest.open ( fname.c_str() );
	if ( ftest.is_open() )	{
		while ( getline (ftest ,line) )	{
			// cout << __FILE__ << " " << __LINE__ << " line= " << line << endl;
		}
		ftest.close();
	} else {
		LOG(Log::ERR) << "unable to open file  " << fname;
		return(-1);
	}
	_ssl_ctx = SSL_CTX_new( SSLv23_method() );
	_ssl = SSL_new( _ssl_ctx );
	SSL_use_certificate_file( _ssl, fname.c_str(), _type );
	// or like this, including a CA
	// SSL_CTX_use_certificate_chain_file( _ssl_ctx, fname.c_str() );
	_x509crt = SSL_get_certificate( _ssl );

	// validity format is here:
	// https://github.com/openssl/openssl/commit/f48b83b4fb7d6689584cf25f61ca63a4891f5b11
	_valid = _x509crt->valid;   // shows false, without CA, as it seems
	_nbefore = _x509crt->cert_info->validity->notBefore;
	_nafter  = _x509crt->cert_info->validity->notAfter;

	// in fact these are strings in UTC format, need to convert them into time_t to become useful
	_time_end = _timeASN1toTIME_T( _nafter );
	_time_beg = _timeASN1toTIME_T( _nbefore );

	int64_t tdiff = ( int64_t ) difftime( _time_end, _time_beg );
	_totaldays  = (int)  ( tdiff/86400 );
	_totalhours = (int) (( tdiff - _totaldays * 86400 )/3600);
	_totalmins  = (int) (( tdiff - _totaldays * 86400 - _totalhours * 3600 )/60);
	_totalsecs  = (int)  ( tdiff - _totaldays * 86400 - _totalhours * 3600 - _totalmins * 60 );

	remainingValidityTime();
	LOG(Log::INF) << " certificate remaining time= " << remainingDays() << "days "
			<< remainingHours() << ":"
			<< remainingMins() << ":"
			<< remainingSecs();
	return( 0 );
}


void Certificate::remainingValidityTime( void ){
	time_t t = time( 0 );
	struct tm * now = localtime( &t );
	ASN1_TIME asn1now;
	asn1now.type = V_ASN1_UTCTIME;
	stringstream ss;
	string fmon = "";
	if ( now->tm_mon +1 < 10 ) fmon = "0";
	string fday = "";
	if ( now->tm_mday < 10 ) fday = "0";
	string fhour = "";
	if ( now->tm_hour < 10 ) fhour = "0";
	string fmin = "";
	if ( now->tm_min < 10 ) fmin = "0";
	string fsec = "";
	if ( now->tm_sec < 10 ) fsec = "0";

	ss << now->tm_year - 100
			<< fmon << now->tm_mon +1
			<< fday << now->tm_mday
			<< fhour << now->tm_hour
			<< fmin << now->tm_min
			<< fsec << now->tm_sec << "Z";

	string asn1str = ss.str();
	//cout << __FILE__ << " " << __LINE__ << " asn1str= " << asn1str << endl;
	asn1now.data = (unsigned char *) ( asn1str.c_str() );
	std::time_t time_now = _timeASN1toTIME_T( &asn1now );

	int64_t idiff = ( int64_t ) difftime( _time_end, time_now ); // seconds as int
	//cout << __FILE__ << " " << __LINE__ << " remaining time idiff= " << idiff  << endl;
	_remainingdays = (int64_t) ( idiff / 86400 );
	_remaininghours = (int64_t) ((int64_t) ( idiff - _remainingdays * 86400 ) / 3600);
	_remainingmins  = (int64_t) ((int64_t) ( idiff - _remainingdays * 86400 - _remaininghours * 3600 )/60);
	_remainingsecs  = (int64_t) ((int64_t) ( idiff - _remainingdays * 86400 - _remaininghours * 3600 - _remainingmins * 60 ));
}


// validity format is here:
// https://github.com/openssl/openssl/commit/f48b83b4fb7d6689584cf25f61ca63a4891f5b11
time_t Certificate::_timeASN1toTIME_T( ASN1_TIME* time ){
	struct tm t;
	string str = string( (const char *) time->data );
	size_t i = 0;
	/*
	    OPCServerCertificate.cpp 150 str= 160822103554Z
	    OPCServerCertificate.cpp 153 V_ASN1_UTCTIME
	    OPCServerCertificate.cpp 150 str= 160606103554Z
	    OPCServerCertificate.cpp 153 V_ASN1_UTCTIME
	 */
	if (time->type == V_ASN1_UTCTIME) {/* two digit year */
		//cout << __FILE__ << " " << __LINE__ << " V_ASN1_UTCTIME" << endl;
		t.tm_year  = std::atoi( str.substr(0, 2).c_str() ) + 2000;
		//cout << __FILE__ << " " << __LINE__ << " tm_year= " << t.tm_year << endl;
		i = 2;
	} else if (time->type == V_ASN1_GENERALIZEDTIME) {/* four digit year */
		//cout << __FILE__ << " " << __LINE__ << " V_ASN1_GENERALIZEDTIME" << endl;
		t.tm_year  = std::atoi( str.substr(0, 4).c_str() );
		//cout << __FILE__ << " " << __LINE__ << "tm_year= " << t.tm_year << endl;
		i = 4;
	}
	t.tm_mon   = std::atoi( str.substr(i, 2).c_str() ) - 1; // -1 since January is 0 not 1.
	i += 2;
	t.tm_mday  = std::atoi( str.substr(i, 2).c_str() );
	i += 2;
	t.tm_hour  = std::atoi( str.substr(i, 2).c_str() );
	i += 2;
	t.tm_min  = std::atoi( str.substr(i, 2).c_str() );
	i += 2;
	t.tm_sec  = std::atoi( str.substr(i, 2).c_str() );
	return mktime(&t);
}

// ===== PUBLIC ======

// convenience singleton constructor
/* static */ Certificate* Certificate::Instance( void )	{
	if ( _pInstance ) {
		return _pInstance;
	} else {
		LOG(Log::WRN) << "coding error: constructor usage for certificate, override with NONE/defaults";
		string certfn = "./PKI/CA/certs/certificate.der";
		string privkeyfn = "./PKI/CA/private/server_priv.pem";
		behaviour_t beh = BEHAVIOR_NONE;
		_pInstance = new Certificate( certfn, privkeyfn, beh );
		return _pInstance;
	}
}


int Certificate::init( void ){
	int ret = 0;
	switch ( _behaviour ) {
	case BEHAVIOR_NONE: {
		_status = Certificate::STATUS_OK;
		return( 0 );
		break;
	}
	case BEHAVIOR_TRY: {
		ret = Certificate::loadCertificateFromFile();
		if( ret < 0 ) {
			_status = Certificate::STATUS_FAILED;
		} else {
			// try to open opc secure channel with that certificate, report warning if unsuccessful but continue
			// ...
			_status = Certificate::STATUS_OK;
		}
		break;
	}
	case BEHAVIOR_TRYCA:
	case BEHAVIOR_CERT:
	case BEHAVIOR_CERTCA:{
		_status = Certificate::STATUS_UNKNOWN;
		break;
	}
	} // switch
	return( ret );
}

string Certificate::remainingTime( void ){
	if ( _behaviour == BEHAVIOR_NONE )
		return( "SSL NOT INITIALIZED" );
	if (( _behaviour == BEHAVIOR_TRY ) && ( _status == Certificate::STATUS_FAILED ))
		return( "Not Applicable: No Certificate Found" );

	if ( _status == Certificate::STATUS_FAILED ){
		LOG(Log::ERR) << "certificate status failed, a problem occurred";
		return( "CERTIFICATE STATUS FAILED" );
	}
	if ( _status == Certificate::STATUS_UNKNOWN ){
		return( "CERTIFICATE STATUS UNKNOWN" );
	}
	remainingValidityTime();
	stringstream xx;
	xx << remainingDays() << string("days ") <<
			setfill('0') << setw(2) << remainingHours() << ":" <<
			setfill('0') << setw(2) << remainingMins() << ":" <<
			setfill('0') << setw(2) << remainingSecs();
	return( xx.str() );
}

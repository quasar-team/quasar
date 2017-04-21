/*
 * Certificate.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: mludwig
 */

#include <iomanip>

#include <Certificate.h>
#include <boost/date_time.hpp>

#include <LogIt.h>
#include <LogLevels.h>

// singleton
Certificate* Certificate::_pInstance = NULL;

const std::string Certificate::DEFAULT_PUBLIC_CERT_FILENAME("./PKI/CA/certs/OpcUaServer.der");
const std::string Certificate::DEFAULT_PRIVATE_CERT_FILENAME("./PKI/CA/private/OpcUaServer.pem");

Certificate* Certificate::Instance( string certfn, string privkeyfn, enum behaviour_t beh )
{
	if (!_pInstance)   // Only allow one instance of class to be generated.
		_pInstance = new Certificate( certfn, privkeyfn, beh );
	return _pInstance;
}

Certificate* Certificate::Instance( )
{
	if(!_pInstance)
	{
		LOG(Log::WRN) << "No Certfiicate singleton instance has been instantiated, programming error. Returning [NULL]";
	}
	return _pInstance;
}

Certificate::Certificate( string certfn, string privkeyfn, enum behaviour_t beh  )
:m_certfn(certfn), m_privkeyfn(privkeyfn), m_behaviour(beh), m_ssl(NULL), m_time_end(NULL), m_status(STATUS_UNKNOWN), m_remaining_validity_in_seconds(0)
{
	setTypeDER();

	if (m_behaviour != BEHAVIOR_NONE )
	{
		SSL_load_error_strings();            /* readable error messages */
		SSL_library_init();                  /* initialize library */
	}
}

Certificate::~Certificate()
{
	if(m_ssl) SSL_shutdown( m_ssl );
}

int Certificate::validateCertificateFilename(const std::string& certificateFilename) const
{
	if ( m_privkeyfn.length() < 5 )
	{
		LOG(Log::ERR) << "refuse to open certificate " << m_privkeyfn << " : filename seems to short ( \"a.der\" at least )";
		return -2;
	}
	ifstream ftest;
	string line;
	ftest.open ( m_privkeyfn.c_str() );
	if ( ftest.is_open() )	{
		while ( getline (ftest ,line) )	{
			// cout << __FILE__ << " " << __LINE__ << " line= " << line << endl;
		}
		ftest.close();
	} else {
		LOG(Log::ERR) << "unable to open file  " << m_privkeyfn;
		return -1;
	}

	return 0;
}

/// loads one private key
int Certificate::loadPrivateKeyFromFile( void )
{
	const int certificateFileCheckResult = validateCertificateFilename(m_privkeyfn);
	if(certificateFileCheckResult < 0) return certificateFileCheckResult;

	m_ssl = SSL_new( SSL_CTX_new( SSLv23_method() ) );
	SSL_use_PrivateKey_file( m_ssl, m_privkeyfn.c_str(), m_type);
	return( 0 );
}

/// loads one PEM file with one certificate, no chain, no CA
int Certificate::loadCertificateFromFile( void )
{
	const int certificateFileCheckResult = validateCertificateFilename(m_certfn);
	if(certificateFileCheckResult < 0) return certificateFileCheckResult;

	m_ssl = SSL_new( SSL_CTX_new( SSLv23_method() ) );
	SSL_use_certificate_file( m_ssl, m_certfn.c_str(), m_type);
	// or like this, including a CA
	// SSL_CTX_use_certificate_chain_file( _ssl_ctx, fname.c_str() );
	X509 *x509crt = SSL_get_certificate( m_ssl );

	// validity format is here:
	// https://github.com/openssl/openssl/commit/f48b83b4fb7d6689584cf25f61ca63a4891f5b11

	// in fact these are strings in UTC format, need to convert them into time_t to become useful
	m_time_end = _timeASN1toTIME_T(  X509_get_notAfter(x509crt) );
	
	remainingValidityTime();
	LOG(Log::INF) << " certificate remaining time= " << remainingDays() << "days "
			<< remainingHours() << ":"
			<< remainingMins() << ":"
			<< remainingSecs();
	return( 0 );
}


void Certificate::remainingValidityTime( void ){
	const tm now = boost::posix_time::to_tm(boost::posix_time::second_clock::local_time());
//	time_t t = time( 0 );
//	struct tm * now = localtime( &t );
	ASN1_TIME asn1now;
	asn1now.type = V_ASN1_UTCTIME;
	stringstream ss;
	string fmon = "";
	if ( now.tm_mon +1 < 10 ) fmon = "0";
	string fday = "";
	if ( now.tm_mday < 10 ) fday = "0";
	string fhour = "";
	if ( now.tm_hour < 10 ) fhour = "0";
	string fmin = "";
	if ( now.tm_min < 10 ) fmin = "0";
	string fsec = "";
	if ( now.tm_sec < 10 ) fsec = "0";

	ss << now.tm_year - 100
			<< fmon << now.tm_mon +1
			<< fday << now.tm_mday
			<< fhour << now.tm_hour
			<< fmin << now.tm_min
			<< fsec << now.tm_sec << "Z";

	string asn1str = ss.str();
	//cout << __FILE__ << " " << __LINE__ << " asn1str= " << asn1str << endl;
	asn1now.data = (unsigned char *) ( asn1str.c_str() );
	std::time_t time_now = _timeASN1toTIME_T( &asn1now );

	m_remaining_validity_in_seconds = ( int64_t ) difftime( m_time_end, time_now ); // seconds as int
}

int Certificate::remainingDays( void ) const
{
	return m_remaining_validity_in_seconds / 86400;
}

int Certificate::remainingHours( void ) const
{
	return (m_remaining_validity_in_seconds - (remainingDays()*86400) ) / 3600;
}

int Certificate::remainingMins( void ) const
{
	return (m_remaining_validity_in_seconds - (remainingDays()*86400) - (remainingHours()*3600) ) / 60;
}

int Certificate::remainingSecs( void ) const
{
	return m_remaining_validity_in_seconds - (remainingDays()*86400) - (remainingHours()*3600) - (remainingMins()*60);;
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

int Certificate::init( void ){
	int ret = 0;
	switch ( m_behaviour ) {
	case BEHAVIOR_NONE: {
		m_status = Certificate::STATUS_OK;
		break;
	}
	case BEHAVIOR_TRY: {
		ret = Certificate::loadCertificateFromFile();
		m_status = ret < 0 ? Certificate::STATUS_FAILED : Certificate::STATUS_OK;
		break;
	}
	case BEHAVIOR_TRYCA:
	case BEHAVIOR_CERT:
	case BEHAVIOR_CERTCA:{
		m_status = Certificate::STATUS_UNKNOWN;
		break;
	}
	} // switch
	return( ret );
}

string Certificate::remainingTime( void ){
	if ( m_behaviour == BEHAVIOR_NONE )
		return( "SSL NOT INITIALIZED" );
	if (( m_behaviour == BEHAVIOR_TRY ) && ( m_status == Certificate::STATUS_FAILED ))
		return( "Not Applicable: No Certificate Found" );

	if ( m_status == Certificate::STATUS_FAILED ){
		LOG(Log::ERR) << "certificate status failed, a problem occurred";
		return( "CERTIFICATE STATUS FAILED" );
	}
	if ( m_status == Certificate::STATUS_UNKNOWN ){
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

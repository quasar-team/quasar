/*
 * ConfigurationDataReader.h
 *
 *  Created on: May 11, 2017
 *      Author: mludwig
 */

#ifndef CONFIGURATIONDATAREADER_H_
#define CONFIGURATIONDATAREADER_H_

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "ConfigurationData.hxx"

using namespace std;
namespace ns_configurationDataReader {

class ConfigurationDataReader
{
public:
	ConfigurationDataReader( string filename );
	virtual ~ConfigurationDataReader();

#if 0
	int readBool( string arname, bool &v );
	int readInt16( string arname, int16_t &v );
	int readInt32( string arname, int32_t &v );
	int readInt64( string arname, int64_t &v );
	int readFloat( string arname, float &v );
	int readDouble( string arname, double &v );


	int readBoolArray( string arname, vector<bool> &v );
	int readInt16Array( string arname, vector<int16_t> &v );
	int readInt32Array( string arname, vector<int32_t> &v );
	int readInt64Array( string arname, vector<int64_t> &v );
	int readFloatArray( string arname, vector<float> &v );
	int readDoubleArray( string arname, vector<double> &v );
#endif


	int readScalar( string arname, bool &v );
	int readScalar( string arname, int16_t &v );
	int readScalar( string arname, int32_t &v );
	int readScalar( string arname, int64_t &v );
	int readScalar( string arname, float &v );
	int readScalar( string arname, double &v );

	int readArray( string arname, vector<bool> &v );
	int readArray( string arname, vector<int16_t> &v );
	int readArray( string arname, vector<int32_t> &v );
	int readArray( string arname, vector<int64_t> &v );
	int readArray( string arname, vector<float> &v );
	int readArray( string arname, vector<double> &v );

private:
	auto_ptr<configurationData_t> model;
};

} /* namespace ns_configurationDataReader */

#endif /* CONFIGURATIONDATAREADER_H_ */

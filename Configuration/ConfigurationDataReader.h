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

/*
 * ConfigurationDataReader.cpp
 *
 *  Created on: May 11, 2017
 *      Author: mludwig
 */

#include "ConfigurationDataReader.h"


using namespace std;

namespace ns_configurationDataReader {

// read in model
ConfigurationDataReader::ConfigurationDataReader( string filename ) {
	try
	{
		model = myConfigurationData ( filename.c_str());
	}
	catch (const xml_schema::exception& e)
	{
		cerr << e << endl;
		std::ostringstream os;
		os << __FILE__ << " " << __LINE__ << " could not find model: " << e << endl;
		throw std::runtime_error( os.str() );
	}
}




int ConfigurationDataReader::readScalar( string scname, bool &v ){

	for (configurationData_t::boolScalar_const_iterator scalar ( model->boolScalar().begin() );
			scalar != model->boolScalar().end();
			++scalar) {
		ostringstream s;
		s << scalar->name();
		if ( s.str() == scname ) {
			v = scalar->value().get();
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readScalar: could not find " << scname << endl;
	return -1;
}
int ConfigurationDataReader::readScalar( string scname, int16_t &v ){
	for (configurationData_t::int16Scalar_const_iterator scalar ( model->int16Scalar().begin() );
			scalar != model->int16Scalar().end();
			++scalar) {
		ostringstream s;
		s << scalar->name();
		if ( s.str() == scname ) {
			v = scalar->value().get();
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readScalar: could not find " << scname << endl;
	return -1;
}
int ConfigurationDataReader::readScalar( string scname, int32_t &v ){
	for (configurationData_t::int32Scalar_const_iterator scalar ( model->int32Scalar().begin() );
			scalar != model->int32Scalar().end();
			++scalar) {
		ostringstream s;
		s << scalar->name();
		if ( s.str() == scname ) {
			v = scalar->value().get();
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readScalar: could not find " << scname << endl;
	return -1;
}
int ConfigurationDataReader::readScalar( string scname, int64_t &v ){
	for (configurationData_t::int64Scalar_const_iterator scalar ( model->int64Scalar().begin() );
			scalar != model->int64Scalar().end();
			++scalar) {
		ostringstream s;
		s << scalar->name();
		if ( s.str() == scname ) {
			v = scalar->value().get();
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readScalar: could not find " << scname << endl;
	return -1;
}
int ConfigurationDataReader::readScalar( string scname, float &v ){
	for (configurationData_t::floatScalar_const_iterator scalar ( model->floatScalar().begin() );
			scalar != model->floatScalar().end();
			++scalar) {
		ostringstream s;
		s << scalar->name();
		if ( s.str() == scname ) {
			v = scalar->value().get();
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readScalar: could not find " << scname << endl;
	return -1;
}
int ConfigurationDataReader::readScalar( string scname, double &v ){
	for (configurationData_t::doubleScalar_const_iterator scalar ( model->doubleScalar().begin() );
			scalar != model->doubleScalar().end();
			++scalar) {
		ostringstream s;
		s << scalar->name();
		if ( s.str() == scname ) {
			v = scalar->value().get();
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readScalar: could not find " << scname << endl;
	return -1;
}

int ConfigurationDataReader::readArray( string arname, vector<bool> &v ){
	for (configurationData_t::boolArray_const_iterator array (model->boolArray().begin() );
			array != model->boolArray().end();
			++array) {
		ostringstream s;
		s << array->name();
		if ( s.str() == arname ) {
			for (configurationData_t::boolArray_type::value_const_iterator val (array->value().begin() );
					val != array->value().end();
					++val)	{
				v.push_back( (bool) *val );
			}
			return 0 ;
		}
	}
	cout << "ConfigurationDataReader::readArray: could not find " << arname << endl;
	return -1;
}
int ConfigurationDataReader::readArray( string arname, vector<int16_t> &v ){
	for (configurationData_t::int16Array_const_iterator array (model->int16Array().begin() );
			array != model->int16Array().end();
			++array) {
		ostringstream s;
		s << array->name();
		if ( s.str() == arname ) {
			for (configurationData_t::int16Array_type::value_const_iterator val (array->value().begin() );
					val != array->value().end();
					++val)	{
				v.push_back( (int16_t) *val );
			}
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readArray: could not find " << arname << endl;
	return -1;
}
int ConfigurationDataReader::readArray( string arname, vector<int32_t> &v ){
	for (configurationData_t::int32Array_const_iterator array (model->int32Array().begin() );
			array != model->int32Array().end();
			++array) {
		ostringstream s;
		s << array->name();
		if ( s.str() == arname ) {
			for (configurationData_t::int32Array_type::value_const_iterator val (array->value().begin() );
					val != array->value().end();
					++val)	{
				v.push_back( (int32_t) *val );
			}
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readArray: could not find " << arname << endl;
	return -1;
}
int ConfigurationDataReader::readArray( string arname, vector<int64_t> &v ){
	for (configurationData_t::int64Array_const_iterator array (model->int64Array().begin() );
			array != model->int64Array().end();
			++array) {
		ostringstream s;
		s << array->name();
		if ( s.str() == arname ) {
			for (configurationData_t::int64Array_type::value_const_iterator val (array->value().begin() );
					val != array->value().end();
					++val)	{
				v.push_back( (int64_t) *val );
			}
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readArray: could not find " << arname << endl;
	return -1;
}
int ConfigurationDataReader::readArray( string arname, vector<float> &v ){
	for (configurationData_t::floatArray_const_iterator array (model->floatArray().begin() );
			array != model->floatArray().end();
			++array) {
		ostringstream s;
		s << array->name();
		if ( s.str() == arname ) {
			for (configurationData_t::floatArray_type::value_const_iterator val (array->value().begin() );
					val != array->value().end();
					++val)	{
				v.push_back( (float) *val );
			}
			return 0;
		}
	}
	cout << "ConfigurationDataReader::readArray: could not find " << arname << endl;
	return -1;
}
int ConfigurationDataReader::readArray( string arname, vector<double> &v ){
	for (configurationData_t::doubleArray_const_iterator array (model->doubleArray().begin() );
			array != model->doubleArray().end();
			++array) {
		ostringstream s;
		s << array->name();
		if ( s.str() == arname ) {
			for (configurationData_t::doubleArray_type::value_const_iterator val (array->value().begin() );
					val != array->value().end();
					++val)	{
				v.push_back( (double) *val );
			}
		}
		return 0;
	}
	cout << "ConfigurationDataReader::readArray: could not find " << arname << endl;
	return -1;
}




ConfigurationDataReader::~ConfigurationDataReader() {
}

} /* namespace ns_configurationData */

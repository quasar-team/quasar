/*
 * ArrayUtils.h
 *
 *  Created on: Jan 15, 2018
 *      Author: mludwig
 */

#ifndef QUASAR_COMMON_SRC_ARRAYUTILS_H_
#define QUASAR_COMMON_SRC_ARRAYUTILS_H_

#include <vector>

#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

class ArrayTools {
public:
	ArrayTools();
	virtual ~ArrayTools();
	//static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Boolean>  value ); // unsigned char == OpcUa_Byte
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Byte>   value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_SByte>  value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Int16>  value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_UInt16> value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Int32>  value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_UInt32> value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Int64>  value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_UInt64> value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Float>  value );
	static UaVariant convertVectorToUaVariant( std::vector <OpcUa_Double> value );
	static UaVariant convertVectorToUaVariant( std::vector <UaString>     value );


	// static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Boolean> &vect ); // unsigned char == OpcUa_Byte
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Byte> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_SByte> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Int16> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_UInt16> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Int32> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_UInt32> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Int64> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_UInt64> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Float> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Double> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <UaString> &vect );
};

#endif /* QUASAR_COMMON_SRC_ARRAYUTILS_H_ */

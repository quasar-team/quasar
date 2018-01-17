/*
 * ArrayUtils.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: mludwig
 */

#include <vector>
#include "../include/ArrayTools.h"

ArrayTools::ArrayTools() {
	// TODO Auto-generated constructor stub

}

ArrayTools::~ArrayTools() {
	// TODO Auto-generated destructor stub
}


// same signature as as Boolean: unsigned char. Can't overload
// /* static */ UaVariant ArrayUtils::convertVectorToUaVariant( std::vector <OpcUa_Boolean> value )
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Byte> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaByteArray ua;
    // ua.create( value.size() );
    ua.resize( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setByteArray( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_SByte> value )
{
	UaVariant v;
	UaUInt32Array arrayDimensions;
	UaSByteArray ua;
	ua.create( value.size() );
	for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
	v.setSByteArray( ua );
	v.arrayDimensions( arrayDimensions );
	return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Int16> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaInt16Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setInt16Array( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_UInt16> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaUInt16Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setUInt16Array( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Int32> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaInt32Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setInt32Array( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_UInt32> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaUInt32Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setUInt32Array( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Int64> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaInt64Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setInt64Array( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_UInt64> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaUInt64Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setUInt64Array( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Float> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaDoubleArray ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setDoubleArray( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Double> value )
{
    UaVariant v;
    UaUInt32Array arrayDimensions;
    UaDoubleArray ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setDoubleArray( ua );
    v.arrayDimensions( arrayDimensions );
    return( v );
}
/* static */ UaVariant ArrayTools::convertVectorToUaVariant( std::vector <UaString> value )
{
	UaVariant v;
	UaUInt32Array arrayDimensions;
	UaStringArray ua;
	ua.create( value.size() );
	for ( unsigned int i = 0; i < value.size(); i++ )	{
		UaString uaString = (*(value[ i ].toOpcUaString()));
        uaString.detach( &ua[i] ); 		// basically does: ua[ i ] = value[ i ];
	}
    v.setStringArray( ua, /* detach */ true  );
    v.arrayDimensions( arrayDimensions );
    return( v );
}


/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Byte> &vect ){
	UaByteArray ua;
	v.toByteArray( ua );
	vect.clear();
	for ( int i = 0; i < ua.size(); i++ ) { // caramba
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_SByte> &vect ){
	UaSByteArray ua;
	v.toSByteArray( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Int16> &vect ){
	UaInt16Array ua;
	v.toInt16Array( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_UInt16> &vect ){
	UaUInt16Array ua;
	v.toUInt16Array( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Int32> &vect ){
	UaInt32Array ua;
	v.toInt32Array( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_UInt32> &vect ){
	UaUInt32Array ua;
	v.toUInt32Array( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Int64> &vect ){
	UaInt64Array ua;
	v.toInt64Array( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_UInt64> &vect ){
	UaUInt64Array ua;
	v.toUInt64Array( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Float> &vect ){
	UaFloatArray ua;
	v.toFloatArray( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Double> &vect ){
	UaDoubleArray ua;
	v.toDoubleArray( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <UaString> &vect ){
	UaStringArray ua;
	v.toStringArray( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( ua[ i ]);
	}
}



/* © Copyright CERN, PNPI, 2015.  All rights not expressly granted are reserved.
 * ASNodeManager.h
 *
 *  Created on: Jun 20, 2014
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *      Author: Viatcheslav Filimonov <vfilimon@cern.ch>
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
/*
 * ArrayUtils.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: mludwig
 */

#include <vector>
#include "../include/ArrayTools.h"


using namespace AddressSpace;

ArrayTools::ArrayTools() {}
ArrayTools::~ArrayTools() {}

/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Boolean> value, UaVariant &v )
{
    UaBooleanArray ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setBoolArray( ua );
}

// OpcUa_Byte has same signature as OpcUa_Boolean: unsigned char. Add a dummy to cheat
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Byte> value, UaVariant &v, bool dummy )
{
    UaByteArray ua;
    ua.resize( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setByteArray( ua );
}

/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_SByte> value, UaVariant &v  )
{
	UaSByteArray ua;
	ua.create( value.size() );
	for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
	v.setSByteArray( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Int16> value, UaVariant &v  )
{
    UaInt16Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setInt16Array( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_UInt16> value, UaVariant &v  )
{
    UaUInt16Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setUInt16Array( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Int32> value, UaVariant &v  )
{
    UaInt32Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setInt32Array( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_UInt32> value , UaVariant &v )
{
    UaUInt32Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setUInt32Array( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Int64> value , UaVariant &v )
{
    UaInt64Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setInt64Array( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_UInt64> value, UaVariant &v  )
{
    UaUInt64Array ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setUInt64Array( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Float> value, UaVariant &v  )
{
    UaFloatArray ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setFloatArray( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <OpcUa_Double> value, UaVariant &v  )
{
    UaDoubleArray ua;
    ua.create( value.size() );
    for ( unsigned int i = 0; i < value.size(); i++ )	ua[ i ] = value[ i ];
    v.setDoubleArray( ua );
}
/* static */ void ArrayTools::convertVectorToUaVariant( std::vector <UaString> value, UaVariant &v  )
{
	UaStringArray ua;
	ua.create( value.size() );
	for ( unsigned int i = 0; i < value.size(); i++ )	{
		UaString uaString = (*(value[ i ].toOpcUaString()));
        uaString.detach( &ua[i] ); 		// basically does: ua[ i ] = value[ i ];
	}
    v.setStringArray( ua, /* detach */ true  );
}


/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Boolean> &vect ){
	UaBooleanArray ua;
	v.toBoolArray( ua );
	vect.clear();
	for ( unsigned int i = 0; i < ua.length(); i++ ) {
		vect.push_back( (bool) ua[ i ]);
	}
}

// cheat overloading
/* static */ void ArrayTools::convertUaVariantToVector( UaVariant v, std::vector <OpcUa_Byte> &vect, bool dummy ){
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



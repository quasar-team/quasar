/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * ArrayTools.h
 *
 *  Created on: Jan 15, 2018
 *      Author: mludwig
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

#ifndef QUASAR_COMMON_SRC_ARRAYUTILS_H_
#define QUASAR_COMMON_SRC_ARRAYUTILS_H_

#include <vector>

#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

namespace AddressSpace
{


class ArrayTools {
public:
	ArrayTools();
	virtual ~ArrayTools();

	static void convertVectorToUaVariant( std::vector <OpcUa_Boolean>  value, UaVariant &v );
	static void convertVectorToUaVariant( std::vector <OpcUa_Byte>  value, UaVariant &v, bool dummy ); // cheat overloading
	static void convertVectorToUaVariant( std::vector <OpcUa_SByte>  value, UaVariant &v  );
	static void convertVectorToUaVariant( std::vector <OpcUa_Int16>  value, UaVariant &v  );
	static void convertVectorToUaVariant( std::vector <OpcUa_UInt16> value, UaVariant &v  );
	static void convertVectorToUaVariant( std::vector <OpcUa_Int32>  value , UaVariant &v );
	static void convertVectorToUaVariant( std::vector <OpcUa_UInt32> value , UaVariant &v );
	static void convertVectorToUaVariant( std::vector <OpcUa_Int64>  value, UaVariant &v  );
	static void convertVectorToUaVariant( std::vector <OpcUa_UInt64> value, UaVariant &v  );
	static void convertVectorToUaVariant( std::vector <OpcUa_Float>  value , UaVariant &v );
	static void convertVectorToUaVariant( std::vector <OpcUa_Double> value, UaVariant &v  );
	static void convertVectorToUaVariant( std::vector <UaString>     value , UaVariant &v );

	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Boolean> &vect );
	static void convertUaVariantToVector(  UaVariant v, std::vector <OpcUa_Byte> &vect, bool dummy ); // cheat overloading
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

}
#endif /* QUASAR_COMMON_SRC_ARRAYUTILS_H_ */

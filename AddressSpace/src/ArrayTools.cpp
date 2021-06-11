/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * ArrayTools.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: mludwig
 *      Author: Piotr Nikiel
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

#include <ArrayTools.h>
#include <algorithm>

namespace AddressSpace
{

namespace ArrayTools
{

/** This templatized function is applicable to "simple types" like arrays of ints, where an assignment operator does well... */
template<typename Type, typename ArrayType>
static void vectorToUaVariant( const std::vector<Type>& input, UaVariant& output, void (UaVariant::*setterFunction)(ArrayType& array, OpcUa_Boolean detach) )
{
    ArrayType array;
    array.create( input.size() );
    for (unsigned int i=0; i<input.size(); ++i)
        array[i] = input[i];
    (output.*setterFunction)(array, /*detach*/ OpcUa_False); // TODO @pnikiel: one day we should switch to detach=true, after testing...
}

/** This templatized function is applicable to "complex types" like arrays of UaByteString, where an assignment operator to the stack type is not defined... */
template<typename Type, typename ArrayType>
static void vectorToUaVariantByCopyTo( const std::vector<Type>& input, UaVariant& output, void (UaVariant::*setterFunction)(ArrayType& array, OpcUa_Boolean detach) )
{
    ArrayType array;
    array.create( input.size() );
    for (unsigned int i=0; i<input.size(); ++i)
        input[i].copyTo(&array[i]);
    (output.*setterFunction)(array, /*detach*/ OpcUa_False); // TODO @pnikiel: one day we should switch to detach=true, after testing...
}

void convertBooleanVectorToUaVariant( const std::vector<OpcUa_Boolean>& input, UaVariant& output )
{
    vectorToUaVariant (input, output, &UaVariant::setBoolArray);
}

void convertByteVectorToUaVariant( const std::vector <OpcUa_Byte>& input, UaVariant &output )
{
    UaByteArray array;
    if (input.size() > 0)
    {
        array = UaByteArray( reinterpret_cast<const char*>(&input[0]), input.size() );
    }
    output.setByteArray(array, /*detach*/ OpcUa_False);
}

void convertVectorToUaVariant( const std::vector <OpcUa_SByte>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setSByteArray);
}

void convertVectorToUaVariant( const std::vector <OpcUa_Int16>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setInt16Array);
}

void convertVectorToUaVariant( const std::vector <OpcUa_UInt16>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setUInt16Array);
}

void convertVectorToUaVariant( const std::vector <OpcUa_Int32>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setInt32Array);
}

void convertVectorToUaVariant( const std::vector <OpcUa_UInt32>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setUInt32Array);
}

void convertVectorToUaVariant( const std::vector <OpcUa_Int64>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setInt64Array);
}

void convertVectorToUaVariant( const std::vector <OpcUa_UInt64>& input, UaVariant &output  )
{
    vectorToUaVariant (input, output, &UaVariant::setUInt64Array);
}

void convertVectorToUaVariant( const std::vector <OpcUa_Float>& input, UaVariant &output  )
{
    vectorToUaVariant (input, output, &UaVariant::setFloatArray);
}

void convertVectorToUaVariant( const std::vector <OpcUa_Double>& input, UaVariant &output )
{
    vectorToUaVariant (input, output, &UaVariant::setDoubleArray);
}

void convertVectorToUaVariant( const std::vector< UaByteString>& input, UaVariant& output )
{
    vectorToUaVariantByCopyTo ( input, output, &UaVariant::setByteStringArray);
}

void convertVectorToUaVariant( const std::vector <UaVariant>& input, UaVariant& output )
{
    vectorToUaVariantByCopyTo ( input, output, &UaVariant::setVariantArray);
}

void convertVectorToUaVariant( const std::vector <UaString>& input, UaVariant& output)
{
    vectorToUaVariantByCopyTo ( input, output, &UaVariant::setStringArray);
}

template<typename Type, typename ArrayType>
static  UaStatus uaVariantToVector( const UaVariant& input, std::vector<Type>& output, OpcUa_StatusCode (UaVariant::*getterFunction)(ArrayType& array) const )
{
    ArrayType array;
    UaStatus status = (input.*getterFunction)(array);
    if (!status.isGood())
        return status;
    output.assign( array.length(), Type());
    for (unsigned int i=0; i<array.length(); ++i)
        output[i] = array[i]; // TODO: might this fail?
    return OpcUa_Good;
}

UaStatus convertUaVariantToBooleanVector( const UaVariant& input, std::vector <OpcUa_Boolean> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toBoolArray);
}

UaStatus convertUaVariantToByteVector( const UaVariant& input, std::vector <OpcUa_Byte> &output )
{
    UaByteArray array;
    UaStatus status = input.toByteArray(array);
    if (!status.isGood())
        return status;
    output.assign( array.size(), 0);
    std::copy( array.data(), array.data()+array.size(), output.begin());
    return OpcUa_Good;
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_SByte> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toSByteArray);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_Int16> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toInt16Array);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_UInt16> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toUInt16Array);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_Int32> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toInt32Array);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_UInt32> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toUInt32Array);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_Int64> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toInt64Array);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_UInt64> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toUInt64Array);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_Float> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toFloatArray);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <OpcUa_Double> &output )
{
    return uaVariantToVector(input, output, &UaVariant::toDoubleArray);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <UaString> &output )
{
    return uaVariantToVector<UaString, UaStringArray>(input, output, &UaVariant::toStringArray);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <UaByteString>& output )
{
    return uaVariantToVector<UaByteString, UaByteStringArray>(input, output, &UaVariant::toByteStringArray);
}

UaStatus convertUaVariantToVector( const UaVariant& input, std::vector <UaVariant>& output )
{
    return uaVariantToVector<UaVariant, UaVariantArray>(input, output, &UaVariant::toVariantArray);
}

std::vector<UaString> convertStdStringsToUaStrings( const std::vector<std::string>& input )
{
    std::vector<UaString> output (input.size());
    std::transform( input.begin(), input.end(), output.begin(), [](const std::string& x) {
        return x.c_str();
    }  );
    return output;
}

}
}

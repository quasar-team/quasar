/*
 * LogItInternalUtils.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: bfarnham
 */

#include <sstream>
#include <cstring>
#include <stdexcept>

using std::ostringstream;

union ConversionStructure
{
	uint64_t m_64BitKey;
	uint8_t m_8Bytes[8];

	ConversionStructure()
	{
		std::memset(this, 0, sizeof(ConversionStructure));
	}

	ConversionStructure(const char* charArray, const size_t sz)
	{
		std::memset(this, 0, sizeof(ConversionStructure));
		std::memcpy(this, charArray, sz);
	}
};

uint64_t to64BitKey(const std::string& stringValue)
{
	if(stringValue.length() > 8)
	{
		ostringstream msg;
		msg << "max component length is [8] bytes, received ["<<stringValue.length()<<"], rejecting...";
		throw std::runtime_error(msg.str());
	}

	// create and initialize converter - dump string as bytes into union
	ConversionStructure converter(stringValue.c_str(), stringValue.length());
	return converter.m_64BitKey;
}

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

/**
 * Provides collision-free identifier generation for log component strings.
 * Simply maps the 8 (or fewer) characters to a 64 bit number.
 */
union CharactersToUniqueKey
{
	uint64_t m_64BitKey;
	uint8_t m_8Chars[8];

	CharactersToUniqueKey(const char* charArray, const size_t sz)
	{
		std::memset(this, 0, sizeof(CharactersToUniqueKey));
		std::memcpy(this, charArray, sz);
	}
};

uint64_t to64BitKey(const std::string& stringValue)
{
	if(stringValue.length() < 1 || stringValue.length() > 8)
	{
		ostringstream msg;
		msg << "logging component length must be between 1 and 8 characters, received log component ["<<stringValue<<"] with length ["<<stringValue.length()<<"], rejecting...";
		throw std::runtime_error(msg.str());
	}

	// create and initialize converter - dump string as bytes into union
	CharactersToUniqueKey keyGenerator(stringValue.c_str(), stringValue.length());
	return keyGenerator.m_64BitKey;
}

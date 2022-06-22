/* © Copyright CERN, 2015. All rights not expressly granted are reserved. */
/*
 * Utils.h
 *
 *  Created on: Oct 22, 2014
 *      Author: Piotr Nikiel <piotr@nikiel.info>
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

#ifndef UTILS_H_
#define UTILS_H_

#include <sstream>
#include <stdexcept>

#define throw_runtime_error_with_origin(MSG) throw std::runtime_error(std::string("At ")+__FILE__+":"+Utils::toString(__LINE__)+" "+MSG)

class Utils
{
public:
template<typename T>
static std::string toString (const T t)
{
        std::ostringstream oss;
        oss << t;
        return oss.str ();
}

template<typename T>
static std::string toHexString (const T t)
{
        std::ostringstream oss;
        oss << std::hex << (unsigned long)t << std::dec;
        return oss.str ();
}

static unsigned int fromHexString (const std::string &s)
{
	unsigned int x;
	std::istringstream iss (s);
	iss >> std::hex >> x;
	if (iss.bad())
		throw std::runtime_error ("Given string '"+s+"' not convertible from hex to uint");
	return x;
}

};

namespace Quasar
{
class TermColors
/** These are simple ANSI control codes. Implemented as static methods in case we had to deal with runtime platform dependencies */
{
public:
	static std::string ForeBold () { return "\033[1m"; }
	static std::string ForeRed () { return "\033[1;31m"; }
	static std::string ForeGreen () { return "\033[1;32m"; }
	static std::string ForeYellow () { return "\033[1;33m"; }
	static std::string ForeBlue () { return "\033[1;34m"; }
	static std::string StyleReset () { return "\033[0m"; }
};
}


#endif /* UTILS_H_ */

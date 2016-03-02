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


#endif /* UTILS_H_ */

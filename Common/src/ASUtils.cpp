/* © Copyright CERN, 2015. All rights not expressly granted are reserved. */
/*
 * ASUtils.cpp
 *
 *  Created on: Jun 12, 2014
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

#include <stdlib.h>
#include <stdexcept>
#include <iostream>


#include "ASUtils.h"

using namespace std;

void abort_with_message(const char* file, int line, const char* message)
{
	std::cout << "ABORT WITH MESSAGE:" << std::endl;
	std::cout << message << std::endl;
	std::cout << "At " << file << ":" << line << std::endl;
	abort ();
}
void abort_with_message(const char* file, int line, const std::string& msg)
{
  abort_with_message(file, line, msg.c_str());
}
namespace AddressSpace
{



}

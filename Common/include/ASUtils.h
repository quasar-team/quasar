/* © Copyright CERN, 2015. All rights not expressly granted are reserved. */
/*
 * ASUtils.h
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

#ifndef ASUTILS_H_
#define ASUTILS_H_

#include <stdlib.h>
#include <iostream>
#include <string>

void abort_with_message(const char* file, int line, const char* message);
void abort_with_message(const char* file, int line, const std::string& message);


#define ASSERT_GOOD(status) { if (!(status).isGood()) { abort_with_message(__FILE__, __LINE__, status.toString().toUtf8()); } }

#define ABORT_MESSAGE(message) { abort_with_message(__FILE__, __LINE__, message); }
#define CONCAT2(s1,s2) ((" " +std::string(s1)+" "+std::string(s2)+" ").c_str())
#define CONCAT3(s1,s2,s3) (CONCAT2(s1,CONCAT2(s2,s3)))

#ifndef __GNUC__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#define PRINT(t) std::cout << t << std::endl;
#define PRINT_LOCATION() std::cout << __PRETTY_FUNCTION__ << std::endl


#endif /* ASUTILS_H_ */

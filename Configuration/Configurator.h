/* © Copyright CERN, 2015. All rights not expressly granted are reserved. */
/*
 * Configurator.h
 *
 *  Created on: Jun 6, 2014
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
 *
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_


#include <string>

#include <ASNodeManager.h>
#include <Configuration.hxx>

typedef boost::function<bool (Configuration::Configuration&)> ConfigXmlDecoratorFunction;
bool configure (std::string fileName,
        AddressSpace::ASNodeManager *nm, ConfigXmlDecoratorFunction
        configXmlDecoratorFunction = ConfigXmlDecoratorFunction()); // 'empty' function by default.

void unlinkAllDevices (AddressSpace::ASNodeManager *nm);

/* The body for that one is generated in ConfigValidator.cpp */
bool validateDeviceTree ();


#endif /* CONFIGURATOR_H_ */

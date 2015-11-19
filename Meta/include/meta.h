/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * meta.h
 *
 *  Created on: Aug 18, 2015
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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

#pragma once

#include <DRoot.h>
#include <ASNodeManager.h>
#include <DStandardMetaData.h>

Device::DStandardMetaData* configureMeta(Configuration::Configuration & config, AddressSpace::ASNodeManager *nm, UaNodeId parentNodeId);
void destroyMeta (AddressSpace::ASNodeManager *nm);

/* (C) Copyright CERN, 2020.  All rights not expressly granted are reserved.
 * ASCommon.h
 *
 *  Created on: 03 Nov 2020
 *      Author: pnikiel
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

namespace AddressSpace
{

/** The intent is to simplify the AS class API by overloading. Should map to simplest possible C++
type (potentially a void struct) that can be passed as an argument. */
struct QuasarNullDataType {};

const std::string SingleItemNodeObjectPrefix ("__single_item_node__");

}

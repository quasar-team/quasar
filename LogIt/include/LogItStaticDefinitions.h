/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogIt.h
 *
 *  Created on: Jan 8, 2018
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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
#ifndef LOGIT_INCLUDE_LOGITSTATICDEFINITIONS_H_
#define LOGIT_INCLUDE_LOGITSTATICDEFINITIONS_H_

namespace Log
{
	typedef size_t LogComponentHandle;

	const size_t g_sMaxComponentIdCount = 512;
	const LogComponentHandle INVALID_HANDLE = 999999999;
}

#endif /* LOGIT_INCLUDE_LOGITSTATICDEFINITIONS_H_ */

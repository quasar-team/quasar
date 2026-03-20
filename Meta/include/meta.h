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
#include <ASNodeQueries.h>
#include <Configuration.hxx>
#include <LogIt.h>
#include <ASNodeQueries.h>

namespace Meta
{
	void initializeMeta(AddressSpace::ASNodeManager *nm);
	void configureMeta(Configuration::Configuration & config);

	template<typename AddressSpaceType>
	AddressSpaceType* findStandardMetaDataChildObject(AddressSpace::ASNodeManager *nm, const std::string& childName)
	{
		const std::string fullChildName = "StandardMetaData."+childName;
		std::vector<AddressSpaceType*> children;
		AddressSpace::findAllObjectsByPatternInNodeManager(nm, fullChildName, children);

		if(children.size() != 1)
		{
			LOG(Log::ERR) << __FUNCTION__ << " searched AS for objects matching ["<<fullChildName<<"] found ["<<children.size()<<"] expected exactly [1], this is a configuration error - exiting!";
			std::exit(1);
		}

		return children[0];
	};
} // namespace Meta


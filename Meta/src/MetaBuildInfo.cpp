/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * BuildMetaInfo.cpp
 *
 *  Created on: Nov 11, 2018
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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
#include "MetaBuildInfo.h"
#include "MetaBuildInfoGenerated.h" // contains constants; contents generated by meta_build_info.py at build time.

using std::string;

string BuildMetaInfo::getBuildTime()
{
	return GEND_BUILD_TIME;
}

string BuildMetaInfo::getBuildHost()
{
	return GEND_BUILD_HOST;
}

std::string BuildMetaInfo::getCommitID()
{
	return GEND_COMMIT_ID;
}

string BuildMetaInfo::getToolkitLibs()
{
	return GEND_TOOLKIT_LIBS;
}

/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * MetaBuildInfo.h
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

#ifndef BUILD_META_INFO_INCLUDE_VERSION_H_
#define BUILD_META_INFO_INCLUDE_VERSION_H_

#include <string>
namespace BuildMetaInfo
{

std::string getBuildTime();
std::string getBuildHost();
std::string getCommitID();
std::string getToolkitLibs();

} // namespace BuildMetaInfo
#endif /* BUILD_META_INFO_INCLUDE_VERSION_H_ */

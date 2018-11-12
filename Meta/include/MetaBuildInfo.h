/*
 * BuildMetaInfo.h
 *
 *  Created on: Mar 25, 2016
 *      Author: bfarnham
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

/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * meta.cpp
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

#include <meta.h>
#include <LogIt.h>
#include <ConfigurationDecorationUtils.h>
#include <ASStandardMetaData.h>
#include <ASBuildInformation.h>
#include <ASQuasar.h>
#include <ASServer.h>
#include <ASSourceVariableThreadPool.h>
#include "MetaBuildInfo.h"
#include "QuasarVersion.h"
#include "metaBackwardsCompatibilityUtils.h"

using std::begin;
using std::end;

void initializeBuildInformation(AddressSpace::ASNodeManager *nm)
{
    auto buildInformation = Meta::findStandardMetaDataChildObject<AddressSpace::ASBuildInformation>(nm, "BuildInformation");
    LOG(Log::INF) << __FUNCTION__ << " found StandardMetaData object ["<<std::hex<<buildInformation<<"]. Populating...";

    buildInformation->setBuildHost(BuildMetaInfo::getBuildHost().c_str(), OpcUa_Good);
    buildInformation->setBuildTimestamp(BuildMetaInfo::getBuildTime().c_str(), OpcUa_Good);
    buildInformation->setCommitID(BuildMetaInfo::getCommitID().c_str(), OpcUa_Good);
    buildInformation->setToolkitLibs(BuildMetaInfo::getToolkitLibs().c_str(), OpcUa_Good);
}

void intializeQuasar(AddressSpace::ASNodeManager *nm)
{
    auto quasar = Meta::findStandardMetaDataChildObject<AddressSpace::ASQuasar>(nm, "Quasar");
    LOG(Log::INF) << __FUNCTION__ << " found StandardMetaData object ["<<std::hex<<quasar<<"]. Populating...";

    quasar->setVersion(QUASAR_VERSION_STR, OpcUa_Good);
}

void initializeServer(AddressSpace::ASNodeManager *nm)
{
    auto server = Meta::findStandardMetaDataChildObject<AddressSpace::ASServer>(nm, "Server");
    LOG(Log::INF) << __FUNCTION__ << " found StandardMetaData object ["<<std::hex<<server<<"]. Populating...";

    server->setRemainingCertificateValidity("uninitialized", OpcUa_Good);
}

void Meta::initializeMeta(AddressSpace::ASNodeManager *nm)
{
    LOG(Log::INF) << __FUNCTION__ << " called";
 
    initializeBuildInformation(nm);
    intializeQuasar(nm);
    initializeServer(nm);
}

Configuration::StandardMetaData& getStandardMetaData(Configuration::Configuration & parent)
{
    if(!parent.StandardMetaData().present())
    {
        LOG(Log::INF) << __FUNCTION__ << " parent does not contain a StandardMetaData element; adding one";
        Configuration::StandardMetaData standardMetaData;
        standardMetaData.name("StandardMetaData");
        parent.StandardMetaData(standardMetaData);
        // preserve content order for optional elements (as well as arrays). Arg 0 below indicates 0th position
        const xml_schema::content_order orderingElement(Configuration::Configuration::StandardMetaData_id, 0);
        parent.content_order().push_back(orderingElement);
    }
    return parent.StandardMetaData().get();
}

Configuration::Log& getLog(Configuration::StandardMetaData& parent)
{
    if(parent.Log().empty())
    {
        LOG(Log::INF) << __FUNCTION__ << " parent does not contain a Log element; adding one";
        Configuration::Log log;
        log.name("Log");
        Configuration::DecorationUtils::push_back(parent, parent.Log(), log, Configuration::StandardMetaData::Log_id);        
    }
    return parent.Log().front();
}

Configuration::SourceVariableThreadPool& getSourceVariableThreadPool(Configuration::StandardMetaData& parent)
{
    if(parent.SourceVariableThreadPool().empty())
    {
        LOG(Log::INF) << __FUNCTION__ << " parent does not contain a SourceVariableThreadPool element; adding one";
        Configuration::SourceVariableThreadPool sourceVariableThreadPool("10", "1");
        sourceVariableThreadPool.name("SourceVariableThreadPool");
        Configuration::DecorationUtils::push_back(parent, parent.SourceVariableThreadPool(), sourceVariableThreadPool, Configuration::StandardMetaData::SourceVariableThreadPool_id);                
    }
    return parent.SourceVariableThreadPool().front();
}

Configuration::LogLevel& getGeneralLogLevel(Configuration::Log& parent)
{
    if(parent.LogLevel().empty())
    {
        const Log::LOG_LEVEL level = Log::getNonComponentLogLevel();
        LOG(Log::INF) << __FUNCTION__ << " parent does not contain a GeneralLogLevel element; adding one";
        Configuration::LogLevel generalLogLevel("GeneralLogLevel", Log::logLevelToString(level));
        Configuration::DecorationUtils::push_back(parent, parent.LogLevel(), generalLogLevel, Configuration::Log::LogLevel_id);
    }
    return parent.LogLevel().front();
}

Configuration::ComponentLogLevels& getComponentLogLevels(Configuration::Log& parent)
{
    if(parent.ComponentLogLevels().empty())
    {
        LOG(Log::INF) << __FUNCTION__ << " parent does not contain a ComponentLogLevels element; adding one";
        Configuration::ComponentLogLevels componentLogLevels;
        componentLogLevels.name("ComponentLogLevels");
        Configuration::DecorationUtils::push_back(parent, parent.ComponentLogLevels(), componentLogLevels, Configuration::Log::ComponentLogLevels_id);
    }
    return parent.ComponentLogLevels().front();
}

Configuration::LogLevel& getComponentLogLevel(const std::string& name, const Log::LogComponentHandle& componentHandle, Configuration::ComponentLogLevels& parent)
{
    const auto matchLogLevelByNameFn = [name](Configuration::LogLevel& logLevel) { return name == logLevel.name(); };

	// find level in configuration
    auto pos = std::find_if(begin(parent.LogLevel()), end(parent.LogLevel()), matchLogLevelByNameFn);
    if(pos != end(parent.LogLevel())) return *pos;

    // not found - add. First retrieve component log level (as string)
    Log::LOG_LEVEL level;
    if(!Log::getComponentLogLevel(componentHandle, level))
    {
        LOG(Log::ERR) << __FUNCTION__ << " programming error - unknown logging component name ["<<name<<"], handle ["<<componentHandle<<"]. Defaulting to [INF]";
        level = Log::INF;
    }

    LOG(Log::INF) << __FUNCTION__ << " LogLevel element name ["<<name<<"] not found in ComponentLogLevels, will create and add to config";
    Configuration::DecorationUtils::push_back(
        parent,
        parent.LogLevel(),
        Configuration::LogLevel(name, Log::logLevelToString(level)),
        Configuration::ComponentLogLevels::LogLevel_id);

    // should find it now
    pos = std::find_if(begin(parent.LogLevel()), end(parent.LogLevel()), matchLogLevelByNameFn);
    if(pos != end(parent.LogLevel())) return *pos;    

    // otherwise error
    std::ostringstream err;
    err << __FUNCTION__ << " programming error - failed to find LogLevel with name ["<<name<<"] in ComponentLogLevels";
    throw std::runtime_error(err.str());
}

void validateComponentLogLevels( const Configuration::ComponentLogLevels& componentLogLevels )
{
	std::list<std::string> checkedComponentNames;

	for( const Configuration::LogLevel& logLevelConfig: componentLogLevels.LogLevel() )
	{
		std::string name(logLevelConfig.name());
		// check for duplicates
		if (std::find(begin(checkedComponentNames), end(checkedComponentNames), name) != end(checkedComponentNames))
		{
			std::ostringstream err;
			err << __FUNCTION__ << " Configuration contains ComponentLogLevel name [" << name << "] more than once: mis-configuration.";
            LOG(Log::ERR) << err.str() << " Throwing exception";
			throw std::runtime_error(err.str());
		}
		checkedComponentNames.push_back( name );
	}
}

void configureComponentLogLevel(const Log::LogComponentHandle& componentHandle, const std::string componentName, Configuration::ComponentLogLevels& parent)
{
    auto& logLevelConfig = getComponentLogLevel(componentName, componentHandle, parent);

    // parse config logLevel string to a LogIt log level.
    const std::string levelString(logLevelConfig.logLevel());
    Log::LOG_LEVEL level;
    if(!Log::logLevelFromString(levelString, level))
    {
        LOG(Log::WRN) << __FUNCTION__ << " failed to parse logLevel string ["<<levelString<<"] to valid level ID for logging component [nm:"<<componentName<<", hdl:"<<componentHandle<<"]. Ignoring";
        return;
    }

    // set component (should not fail)
    if(Log::setComponentLogLevel(componentHandle, level))
    {
        LOG(Log::INF) << __FUNCTION__ << " set logging component [nm:"<<componentName<<", hdl:"<<componentHandle<<"] to threshold ["<<levelString<<"]";
    }
    else
    {
        LOG(Log::ERR) << __FUNCTION__ << " programming error: failed to set logging component [nm:"<<componentName<<", hdl:"<<componentHandle<<"] to threshold ["<<levelString<<"]";
    }    
}

void configureComponentLogLevels(Configuration::Log& parent)
{
    auto& componentLogLevels = getComponentLogLevels(parent);
    Meta::BackwardsCompatibilityUtils::convertOldComponentLogLevel(componentLogLevels);
    validateComponentLogLevels(componentLogLevels);

    // Loop round only those logging components registered with LogIt. 
    // Config _may_ contain other named logging components
    // But if LogIt doesn't know them - can only ignore them here.
    // Perhaps the unknown components will be registered later on 
    //
    // Fine: e.g. some shared libray, which registers its own logging 
    // component, might be dynamically loaded later or something.
    for(const auto& registeredComponent : Log::getComponentLogsList())
    {
        const Log::LogComponentHandle& componentHandle = registeredComponent.first;
        const std::string componentName = Log::getComponentName(componentHandle);
        configureComponentLogLevel(componentHandle, componentName, componentLogLevels);
    }
}

void configureGeneralLogLevel(Configuration::Log& parent)
{
    Meta::BackwardsCompatibilityUtils::convertOldGeneralLogLevel(parent);

    auto& generalLogLevel = getGeneralLogLevel(parent);
    Log::LOG_LEVEL level;
    if(Log::logLevelFromString(generalLogLevel.logLevel(), level))
    {
        Log::setNonComponentLogLevel(level);
        LOG(Log::INF) << __FUNCTION__ << " set GeneralLogLevel to threshold ["<<Log::logLevelToString(level)<<"]";
    }
    else
    {
        LOG(Log::ERR) << __FUNCTION__ << "  failed to parse logLevel string ["<<generalLogLevel.logLevel()<<"] to valid level ID for GeneralLogLevel";
    }    
}

void configureLog(Configuration::StandardMetaData& parent)
{
    auto& log = getLog(parent);
    configureGeneralLogLevel(log);
    configureComponentLogLevels(log);
}

void configureSourceVariableThreadPool(Configuration::StandardMetaData& parent)
{
    getSourceVariableThreadPool(parent);
}

void configureStandardMetaData(Configuration::Configuration & parent)
{
    auto& standardMetaData = getStandardMetaData(parent);
    configureLog(standardMetaData);
    configureSourceVariableThreadPool(standardMetaData);
}

void Meta::configureMeta(Configuration::Configuration & config)
{
    LOG(Log::INF) << __FUNCTION__ << " called";
    configureStandardMetaData(config);
}
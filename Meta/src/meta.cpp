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
#include <sstream>
#include <iostream>
#include <list>
#include <boost/foreach.hpp>

#include <LogIt.h>
#include <LogLevels.h>

#include <ASUtils.h>
#include <ASInformationModel.h>
#include <ASNodeManager.h>
#include <ASNodeQueries.h>

#include <MetaUtils.h>

#include <ASStandardMetaData.h>
#include <DStandardMetaData.h>
#include <ASLog.h>
#include <DRoot.h>
#include <ASGeneralLogLevel.h>
#include <ASComponentLogLevel.h>
#include <DGeneralLogLevel.h>
#include <DComponentLogLevel.h>
#include <ASComponentLogLevels.h>
#include <ASSourceVariableThreadPool.h>
#include <DSourceVariableThreadPool.h>
#include <ASQuasar.h>
#include <DQuasar.h>
#include <ASServer.h>
#include <DServer.h>

using std::string;

const string getComponentLogLevelFromConfig(const ComponentAttributes& component, const Configuration::ComponentLogLevels& config)
{
	string result = Log::logLevelToString(component.getLevel());

	BOOST_FOREACH(const Configuration::ComponentLogLevel & componentLogLevelConfig, config.ComponentLogLevel())
	{
		const string configuredComponentName = componentLogLevelConfig.componentName();
		if(component.getName() == configuredComponentName)
		{
			result = componentLogLevelConfig.logLevel();
		}
	}

	LOG(Log::INF) << "configuration for logging component ["<<component.getName()<<"] using value ["<<result<<"]";
	return result;
}

/**
 * This function ensures that:
 * 1) all specified component log levels are registered in the server (to prevent config file typos, etc)
 * 2) that all specified component log levels are present at most once (to not have conflicting levels)
 *
 * @return true, when component log levels are sane, false otherwise
 */
bool validateComponentLogLevels( const Configuration::ComponentLogLevels& logLevels )
{
	std::list<std::string> checkedComponentNames;
	const std::list<ComponentAttributes> registeredComponents (Log::getComponentLogsList());
	BOOST_FOREACH( const Configuration::ComponentLogLevel &logLevel, logLevels.ComponentLogLevel() )
	{
		std::string name (logLevel.componentName());

		/* 1) validate that the component is registered by querying its id basing on name */
//      this nice lambda code works only with fully C++11 compatbicle compiler...
//		std::find_if( components.begin(), components.end(),
//				[name](ComponentAttributes &ca){ return name == ca.getName(); }
//		);

		// have to use sth gcc 4.4.7 compatible
		bool found = false;
		BOOST_FOREACH( const ComponentAttributes& ca, registeredComponents)
		{
			if (name == ca.getName())
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::cout << "Component Log Level named '" << name << "' is unknown (not registered)." << std::endl;
			return false;
		}

		/* 2) now check for duplicates */
		if (std::find(checkedComponentNames.begin(), checkedComponentNames.end(), name) != checkedComponentNames.end())
		{
			std::cout << "Component Log Level named '" << name << "' is given more than once. " << std::endl;
			return false;
		}
		checkedComponentNames.push_back( name );

	}

	return true;
}

const Configuration::ComponentLogLevels getComponentLogLevels(const Configuration::Log & config)
{
	if(config.ComponentLogLevels().present())
	{
		LOG(Log::INF) << "StandardMetaData.Log.ComponentLogLevels configuration found in the configuration file, configuring StandardMetaData.Log.ComponentLogLevels from the configuration file";
		return config.ComponentLogLevels().get();
	}
	else
	{
		LOG(Log::INF) << "no StandardMetaData.Log.ComponentLogLevels configuration found in the configuration file, configuring StandardMetaData.Log.ComponentLogLevels with default values";
		return Configuration::ComponentLogLevels();
	}
}

const string getGeneralLogLevelFromConfig(const Configuration::Log & config)
{
	// default to value used by logger.
	string result = Log::logLevelToString(Log::getNonComponentLogLevel());

	if( config.GeneralLogLevel().present() )
	{
		result = config.GeneralLogLevel().get().logLevel();
	}

	LOG(Log::INF) << "general non-component log level will be ["<<result<<"]";
	return result;
}

void configureGeneralLogLevel(const string& logLevel, AddressSpace::ASNodeManager *nm, AddressSpace::ASLog* parent)
{
    AddressSpace::ASGeneralLogLevel *asGeneralLogLevel = new AddressSpace::ASGeneralLogLevel(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_GENERALLOGLEVEL), nm, logLevel);

    Device::DGeneralLogLevel* dGeneralLogLevel = new Device::DGeneralLogLevel (logLevel);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dGeneralLogLevel, asGeneralLogLevel);
}

void configureSourceVariableThreadPool(const Configuration::SourceVariableThreadPool& config, AddressSpace::ASNodeManager *nm,  AddressSpace::ASStandardMetaData* parent)
{
    AddressSpace::ASSourceVariableThreadPool *asSourceVariableThreadPool = new AddressSpace::ASSourceVariableThreadPool(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_SOURCEVARIABLESTHREADPOOL), nm, config.minThreads(), config.maxThreads());

    Device::DSourceVariableThreadPool* dSourceVariableThreadPool = new Device::DSourceVariableThreadPool(config.minThreads(), config.maxThreads());
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dSourceVariableThreadPool, asSourceVariableThreadPool);
}

void configureQuasar(const Configuration::Quasar& config, AddressSpace::ASNodeManager *nm,  AddressSpace::ASStandardMetaData* parent, Device::DRoot * deviceParent)
{
    AddressSpace::ASQuasar *asQuasar = new AddressSpace::ASQuasar(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_QUASAR), nm, config);

    Device::DQuasar* dQuasar = new Device::DQuasar(config, deviceParent);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dQuasar, asQuasar);
    dQuasar->updateVersion();
}

void configureServer(const Configuration::Server& config, AddressSpace::ASNodeManager *nm,  AddressSpace::ASStandardMetaData* parent, Device::DRoot * deviceParent)
{
    AddressSpace::ASServer *asServer = new AddressSpace::ASServer(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_SERVER), nm, config);

    Device::DServer* dServer = new Device::DServer(config, deviceParent);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dServer, asServer);
    MetaUtils::setDServer(dServer);

    dServer->updateRemainingCertificateValidity(MetaUtils::calculateRemainingCertificateValidity());
}

void configureComponentLogLevel(const ComponentAttributes& component, const string& logLevel, AddressSpace::ASNodeManager *nm, AddressSpace::ASComponentLogLevels* parent)
{
    AddressSpace::ASComponentLogLevel *asComponentLogLevel = new AddressSpace::ASComponentLogLevel(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_COMPONENTLOGLEVEL), nm, component.getName(), logLevel);

    Device::DComponentLogLevel* dComponentLogLevel = new Device::DComponentLogLevel (component.getId(), logLevel);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dComponentLogLevel, asComponentLogLevel);
}

const Configuration::Log getLogConfig(const Configuration::StandardMetaData & config)
{
	if( config.Log().present() )
	{
		LOG(Log::INF) << "StandardMetaData.Log configuration found in the configuration file, configuring StandardMetaData.Log from the configuration file";
		return config.Log().get();
	}
	else
	{
		LOG(Log::INF) << "no StandardMetaData.Log configuration found in the configuration file, configuring StandardMetaData.Log with default values";
		return Configuration::Log();
	}
}

const Configuration::SourceVariableThreadPool getSourceVariableThreadPoolConfig(const Configuration::StandardMetaData & config)
{
	if( config.SourceVariableThreadPool().present() )
	{
		LOG(Log::INF) << "StandardMetaData.SourceVariableThreadPool configuration found in the configuration file, configuringStandardMetaData.SourceVariableThreadPool from the configuration file";
		return config.SourceVariableThreadPool().get();
	}
	else
	{
		LOG(Log::INF) << "no StandardMetaData.SourceVariableThreadPool configuration found in the configuration file, configuring StandardMetaData.SourceVariableThreadPool with default values";
		unsigned int min = 1;
		unsigned int max = 10;
		return Configuration::SourceVariableThreadPool(min, max);
	}
}

const Configuration::Quasar getQuasarConfig(const Configuration::StandardMetaData & config)
{
	if( config.Quasar().present() )
	{
		LOG(Log::INF) << "StandardMetaData.Quasar configuration found in the configuration file, configuringStandardMetaData.Quasar from the configuration file";
		return config.Quasar().get();
	}
	else
	{
		LOG(Log::INF) << "no StandardMetaData.Quasar configuration found in the configuration file, configuring StandardMetaData.Quasar with default values";
		return Configuration::Quasar();
	}
}

const Configuration::Server getServerConfig(const Configuration::StandardMetaData & config)
{
	if( config.Server().present() )
	{
		LOG(Log::INF) << "StandardMetaData.Server configuration found in the configuration file, configuringStandardMetaData.Server from the configuration file";
		return config.Server().get();
	}
	else
	{
		LOG(Log::INF) << "no StandardMetaData.Server configuration found in the configuration file, configuring StandardMetaData.Server with default values";
		return Configuration::Server();
	}
}

void configureComponentLogLevels(const Configuration::ComponentLogLevels& config, AddressSpace::ASNodeManager* nm, AddressSpace::ASLog *parent)
{
    AddressSpace::ASComponentLogLevels* asComponentLogLevels = new AddressSpace::ASComponentLogLevels(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_COMPONENTLOGLEVELS), nm);

    BOOST_FOREACH(const ComponentAttributes& component, Log::getComponentLogsList())
    {
    	const string componentLogLevel = getComponentLogLevelFromConfig(component, config);
    	configureComponentLogLevel(component, componentLogLevel, nm, asComponentLogLevels);
    }
}

void configureLog(const Configuration::Log & config, AddressSpace::ASNodeManager *nm, AddressSpace::ASStandardMetaData* parent)
{
    AddressSpace::ASLog *asLog = new AddressSpace::ASLog(parent->nodeId(),nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_LOG), nm, config);

    configureGeneralLogLevel(getGeneralLogLevelFromConfig(config), nm, asLog);
    const Configuration::ComponentLogLevels componentLogLevels = getComponentLogLevels(config);
    if (!validateComponentLogLevels(componentLogLevels))
    	abort(); // TODO: Ben: pass the information back in more human way, perhaps?
    configureComponentLogLevels(componentLogLevels, nm, asLog);
}

const Configuration::StandardMetaData getMetaConfig(const Configuration::Configuration& config)
{
	if( config.StandardMetaData().present() )
	{
		LOG(Log::INF) << "meta configuration found in the configuration file, configuring StandardMetaData from the configuration file";
		return config.StandardMetaData().get();
	}
	else
	{
		LOG(Log::INF) << "no Meta configuration found in the configuration file, configuring StandardMetaData with default values";
		return Configuration::StandardMetaData ();
	}
}

Device::DStandardMetaData* configureMeta( const Configuration::StandardMetaData & config,AddressSpace::ASNodeManager *nm, UaNodeId parentNodeId, Device::DRoot * parent)
{
    AddressSpace::ASStandardMetaData *asMeta = new AddressSpace::ASStandardMetaData(parentNodeId, nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_STANDARDMETADATA), nm, config);
    UaStatus s = nm->addNodeAndReference( parentNodeId, asMeta, OpcUaId_HasComponent);
    MetaUtils::assertNodeAdded(s, parentNodeId, asMeta->nodeId());

    Device::DStandardMetaData *dMeta = new Device::DStandardMetaData (config, parent);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dMeta, asMeta);

    configureLog(getLogConfig(config), nm, asMeta);
    configureSourceVariableThreadPool(getSourceVariableThreadPoolConfig(config), nm, asMeta);
    configureQuasar(getQuasarConfig(config), nm, asMeta, parent);
	configureServer(getServerConfig(config), nm, asMeta, parent);
	
    return dMeta;
}

Device::DStandardMetaData* configureMeta(Configuration::Configuration & config, AddressSpace::ASNodeManager *nm, UaNodeId parentNodeId)
{
	return configureMeta(getMetaConfig(config), nm, parentNodeId, Device::DRoot::getInstance());
}

void destroyMeta (AddressSpace::ASNodeManager *nm)
{
	{
		std::vector< AddressSpace::ASStandardMetaData * > objects;
		std::string pattern (".*");
		AddressSpace::findAllByPattern<AddressSpace::ASStandardMetaData> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, pattern, objects);
		BOOST_FOREACH(AddressSpace::ASStandardMetaData *a, objects)
		{
			a->unlinkDevice();
		}
	}

	{
		std::vector< AddressSpace::ASGeneralLogLevel * > objects;
		std::string pattern (".*");
		AddressSpace::findAllByPattern<AddressSpace::ASGeneralLogLevel> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, pattern, objects);
		BOOST_FOREACH(AddressSpace::ASGeneralLogLevel *a, objects)
		{
			a->unlinkDevice();
		}
	}
	
	{
		std::vector< AddressSpace::ASQuasar * > objects;
		std::string pattern (".*");
		AddressSpace::findAllByPattern<AddressSpace::ASQuasar> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, pattern, objects);
		BOOST_FOREACH(AddressSpace::ASQuasar *a, objects)
		{
			a->unlinkDevice();
		}
	}
	
	{
		std::vector< AddressSpace::ASServer * > objects;
		std::string pattern (".*");
		AddressSpace::findAllByPattern<AddressSpace::ASServer> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, pattern, objects);
		BOOST_FOREACH(AddressSpace::ASServer *a, objects)
		{
			a->unlinkDevice();
		}
	}

	{
		std::vector< AddressSpace::ASSourceVariableThreadPool * > objects;
		std::string pattern (".*");
		AddressSpace::findAllByPattern<AddressSpace::ASSourceVariableThreadPool> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, pattern, objects);
		BOOST_FOREACH(AddressSpace::ASSourceVariableThreadPool *a, objects)
		{
			a->unlinkDevice();
		}
	}

	{
		std::vector< AddressSpace::ASComponentLogLevel * > objects;
		std::string pattern (".*");
		AddressSpace::findAllByPattern<AddressSpace::ASComponentLogLevel> (nm, nm->getNode(UaNodeId(OpcUaId_ObjectsFolder, 0)), OpcUa_NodeClass_Object, pattern, objects);
		BOOST_FOREACH(AddressSpace::ASComponentLogLevel *a, objects)
		{
			a->unlinkDevice();
		}
	}

}

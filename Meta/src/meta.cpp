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

#include <Certificate.h>

#include <QUASARFrameworkVersion.h>

using std::string;
using namespace std;
using namespace AddressSpace;


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
 * 3) that at least one component log level is registered and exists
 *
 * @return true, when component log levels are sane, false otherwise
 */
bool validateComponentLogLevels( const Configuration::ComponentLogLevels& logLevels )
{
	std::list<std::string> checkedComponentNames;
	const std::list<ComponentAttributes> registeredComponents (Log::getComponentLogsList());

	if ( !registeredComponents.size() ) {
		LOG(Log::ERR) << "validateComponentLogLevels: did not find any registered Component Log Levels";
		return false;
	}

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
#if 0
	if(config.ComponentLogLevels().present())
	{
		LOG(Log::INF) << "StandardMetaData.Log.ComponentLogLevels configuration found in the configuration file, configuring StandardMetaData.Log.ComponentLogLevels from the configuration file";
		return config.ComponentLogLevels().get();
	}
	else
#endif
	{
		LOG(Log::INF) << " configuring StandardMetaData.Log.ComponentLogLevels with fixed format default values";
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
    MetaUtils::linkChildNodeToParent(asGeneralLogLevel, parent, nm);

    Device::DGeneralLogLevel* dGeneralLogLevel = new Device::DGeneralLogLevel (logLevel);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dGeneralLogLevel, asGeneralLogLevel);
    UaVariant v_logLevel; v_logLevel.setString( logLevel.c_str() );
    asGeneralLogLevel->connectStandardMetaVariables( nm, v_logLevel, asGeneralLogLevel );
}

void configureSourceVariableThreadPool(const Configuration::SourceVariableThreadPool& config, AddressSpace::ASNodeManager *nm,  AddressSpace::ASStandardMetaData* parent)
{
    AddressSpace::ASSourceVariableThreadPool *asSourceVariableThreadPool = new AddressSpace::ASSourceVariableThreadPool(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_SOURCEVARIABLESTHREADPOOL), nm, config.minThreads(), config.maxThreads());
    MetaUtils::linkChildNodeToParent(asSourceVariableThreadPool, parent, nm);

    Device::DSourceVariableThreadPool* dSourceVariableThreadPool = new Device::DSourceVariableThreadPool(config.minThreads(), config.maxThreads());
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dSourceVariableThreadPool, asSourceVariableThreadPool);
    MetaUtils::setDSourceVariableThreadPool( dSourceVariableThreadPool );

    UaVariant v_minThreads; v_minThreads.setUInt32( 0 );
    UaVariant v_maxThreads; v_maxThreads.setUInt32( 10 );
    asSourceVariableThreadPool->connectStandardMetaVariables( nm, v_minThreads, v_maxThreads, asSourceVariableThreadPool );
}


void configureQuasar(const Configuration::Quasar& config, AddressSpace::ASNodeManager *nm,  AddressSpace::ASStandardMetaData* parent, Device::DRoot * deviceParent)
{
    AddressSpace::ASQuasar *asQuasar = new AddressSpace::ASQuasar(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_QUASAR), nm, config);
    MetaUtils::linkChildNodeToParent(asQuasar, parent, nm);

    Device::DQuasar* dQuasar = new Device::DQuasar(config, deviceParent);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dQuasar, asQuasar);
    MetaUtils::setDQuasar( dQuasar );

    UaVariant v_version; v_version.setString( QUASAR_VERSION_STR );
    asQuasar->connectStandardMetaVariables( nm,	v_version, asQuasar );
}

void configureServer(const Configuration::Server& config, AddressSpace::ASNodeManager *nm,  AddressSpace::ASStandardMetaData* parent, Device::DRoot * deviceParent)
{
    AddressSpace::ASServer *asServer = new AddressSpace::ASServer(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_SERVER), nm, config);
    MetaUtils::linkChildNodeToParent(asServer, parent, nm);

    Device::DServer* dServer = new Device::DServer(config, deviceParent);
    MetaUtils::linkHandlerObjectAndAddressSpaceNode(dServer, asServer);
    MetaUtils::setDServer(dServer);

    UaVariant v_connectedClientCount; v_connectedClientCount.setUInt32( 0 );
    UaVariant v_certValidityRemaining; v_certValidityRemaining.setString( "remaining certificate time unknown" );
    asServer->connectStandardMetaVariables( nm,
    		v_connectedClientCount,
			v_certValidityRemaining,
			asServer );

    // certificate
    string certfn = "./PKI/CA/certs/certificate.der";
    string privkeyfn = "./PKI/CA/private/server_priv.pem";
    Certificate::Instance( certfn, privkeyfn, Certificate::BEHAVIOR_TRY );
    Certificate::Instance()->init();
}

void addComponentLogLevel(const ComponentAttributes& component, const string& logLevel, AddressSpace::ASNodeManager *nm, AddressSpace::ASComponentLogLevels* parent)
{
	// have to hand-add each component, getting the parent right
    AddressSpace::ASComponentLogLevel *asComponentLogLevel = new AddressSpace::ASComponentLogLevel(parent->nodeId(),
    		nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_COMPONENTLOGLEVEL), nm, component.getName(), logLevel);
    MetaUtils::linkChildNodeToParent(asComponentLogLevel, parent, nm);

    UaVariant v_logLevel; v_logLevel.setString("ERR");
    asComponentLogLevel->connectStandardMetaVariables( nm, v_logLevel, asComponentLogLevel );
}


const Configuration::Log getLogConfig(const Configuration::StandardMetaData & config)
{
#if 0
	if( config.Log().present() )
	{
		LOG(Log::INF) << "StandardMetaData.Log configuration found in the configuration file, configuring StandardMetaData.Log from the configuration file";
		return config.Log().get();
	}
	else
#endif
		// use a fixed format, see Configuration/Configuration.xsd and Meta/config/Meta.xsd
		LOG(Log::INF) << "configuring StandardMetaData.Log with fixed format default values";
		return Configuration::Log();
}


const Configuration::SourceVariableThreadPool getSourceVariableThreadPoolConfig(const Configuration::StandardMetaData & config)
{
#if 0
	if( config.SourceVariableThreadPool().present() )
	{
		LOG(Log::INF) << "StandardMetaData.SourceVariableThreadPool configuration found in the configuration file, configuringStandardMetaData.SourceVariableThreadPool from the configuration file";
		return config.SourceVariableThreadPool().get();
	}
	else
#endif
	{
		LOG(Log::INF) << "configuring StandardMetaData.SourceVariableThreadPool with fixed format default values";
		Configuration::SourceVariableThreadPool::minThreads_type minThreads = 0;
		Configuration::SourceVariableThreadPool::maxThreads_type maxThreads = 10;
		return Configuration::SourceVariableThreadPool(minThreads, maxThreads);
	}
}

const Configuration::Quasar getQuasarConfig(const Configuration::StandardMetaData & config)
{
#if 0
	if( config.Quasar().present() )
	{
		LOG(Log::INF) << "StandardMetaData.Quasar configuration found in the configuration file, configuringStandardMetaData.Quasar from the configuration file";
		return config.Quasar().get();
	}
	else
#endif
	{
		LOG(Log::INF) << "configuring StandardMetaData.Quasar with fixed format default values";
		Configuration::Quasar::version_type version = QUASAR_VERSION_STR;
		return Configuration::Quasar( version );
	}
}

const Configuration::Server getServerConfig(const Configuration::StandardMetaData & config)
{
#if 0
	if( config.Server().present() )
	{
		LOG(Log::INF) << "StandardMetaData.Server configuration found in the configuration file, configuringStandardMetaData.Server from the configuration file";
		return config.Server().get();
	}
	else
#endif
	{
		LOG(Log::INF) << "configuring StandardMetaData.Server with fixed format default values";
		Configuration::Server::connectedClientCount_type connectedClientCount = 0;
		Configuration::Server::certValidityRemaining_type certValidityRemaining = "unknown";
		return Configuration::Server( connectedClientCount, certValidityRemaining );
	}
}

void configureComponentLogLevels(const Configuration::ComponentLogLevels& config, AddressSpace::ASNodeManager* nm, AddressSpace::ASLog *parent)
{
    AddressSpace::ASComponentLogLevels* asComponentLogLevels = new AddressSpace::ASComponentLogLevels(parent->nodeId(), nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_COMPONENTLOGLEVELS), nm);
    MetaUtils::linkChildNodeToParent(asComponentLogLevels, parent, nm);

    // set up a fixed format list as "smallest common denominator". More specific log components can be added later,
    // probably using LogIt again
    std::list<ComponentAttributes> attList;
    attList.push_back( ComponentAttributes( 0, "AS_DISCO",   Log::ERR ) );
    attList.push_back( ComponentAttributes( 1, "AS_UPDATES", Log::ERR ) );
    attList.push_back( ComponentAttributes( 2, "AS_WRITES",  Log::ERR ) );
    attList.push_back( ComponentAttributes( 3, "STATES",     Log::ERR ) );
    BOOST_FOREACH(const ComponentAttributes& component, attList )
    {
    	// the log level for each of them might be overwritten by config
    	const string componentLogLevel = getComponentLogLevelFromConfig(component, config);
    	addComponentLogLevel(component, componentLogLevel, nm, asComponentLogLevels);
    }
}


void configureLog(const Configuration::Log & config, AddressSpace::ASNodeManager *nm, AddressSpace::ASStandardMetaData* parent)
{
    AddressSpace::ASLog *asLog = new AddressSpace::ASLog(parent->nodeId(),nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_LOG), nm, config);
    MetaUtils::linkChildNodeToParent(asLog, parent, nm);
    configureGeneralLogLevel(getGeneralLogLevelFromConfig(config), nm, asLog);

    const Configuration::ComponentLogLevels componentLogLevels = Configuration::ComponentLogLevels(); // empty
    configureComponentLogLevels(componentLogLevels, nm, asLog);
}

const Configuration::StandardMetaData getMetaConfig(const Configuration::Configuration& config)
{
#if 0
	if( config.StandardMetaData().present() )
	{
		LOG(Log::INF) << "meta configuration found in the configuration file, configuring StandardMetaData from the configuration file";
		return config.StandardMetaData().get();
	}
	else
#endif
	{
		LOG(Log::INF) << "configuring StandardMetaData with fixed format default values";
		return Configuration::StandardMetaData ();
	}
}

Device::DStandardMetaData* configureMeta( const Configuration::StandardMetaData & config,AddressSpace::ASNodeManager *nm, UaNodeId parentNodeId, Device::DRoot * parent)
{
	const UaNodeId unid =	nm->getTypeNodeId(AddressSpace::ASInformationModel::AS_TYPE_STANDARDMETADATA);

	AddressSpace::ASStandardMetaData *asMeta = new AddressSpace::ASStandardMetaData(parentNodeId, unid, nm, config);
	UaStatus s = nm->addNodeAndReference( parentNodeId, asMeta, /* implementation specific type data */ OpcUaId_HasComponent );
	if ( s != UA_STATUSCODE_GOOD ){
		LOG(Log::ERR) << "could not add StandardMetaData node, exiting...";
		exit(-1);
	}
	MetaUtils::assertNodeAdded(s, parentNodeId, asMeta->nodeId());
	Device::DStandardMetaData *dMeta = new Device::DStandardMetaData ( config, parent );
	MetaUtils::linkHandlerObjectAndAddressSpaceNode( dMeta, asMeta );

	configureServer(getServerConfig(config), nm, asMeta, parent);
	configureSourceVariableThreadPool(getSourceVariableThreadPoolConfig(config), nm, asMeta);
	configureQuasar(getQuasarConfig(config), nm, asMeta, parent);
	configureLog( getLogConfig(config), nm, asMeta);
    return dMeta;
}

Device::DStandardMetaData* configureMeta(Configuration::Configuration & config, AddressSpace::ASNodeManager *nm, UaNodeId parentNodeId)
{
	return configureMeta(getMetaConfig(config), nm, parentNodeId, Device::DRoot::getInstance());
}

// update nodes in standard meta data where the information does NOT come from the client
// this can be called i.e. by the QuasarServer at 1Hz
void updateStandardMetaData( AddressSpace::ASNodeManager *nm ){

	Device::DServer *dserver = MetaUtils::getDServer();
	//LOG(Log::INF) << " updating StandardMetaData.Server";

	// some fake animation for now. This needs to be connected correctly
	int cc = rand();
	dserver->setConnectedClientCount( cc );


	dserver->setCertValidityRemaining( Certificate::Instance()->remainingTime() );

	// quasar has just the version string, does not need to be updated.

	// get some open6 version
	// cout << (char *) UA_ServerConfig_standard.buildInfo.softwareVersion.data << endl;


	// variable thread pool:
	// some clarification needed: do we need this dynamically updatable?
	// Device::DSourceVariableThreadPool *dsvtp = MetaUtils::getDSourceVariableThreadPool();
	// LOG(Log::INF) << " updating " << dsvtp->getFullName();

	// logging: GeneralLogLevel and ComponentLogLevels: this information comes from the client

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


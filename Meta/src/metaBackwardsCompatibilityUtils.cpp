/* © Copyright CERN, 2022.  All rights not expressly granted are reserved.
 * metaBackwardsCompatibility.cpp
 *
 *  Created on: Jul 19, 2022
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
#include <metaBackwardsCompatibilityUtils.h>
#include <ConfigurationDecorationUtils.h>

using std::begin;
using std::end;

void Meta::BackwardsCompatibilityUtils::convertOldGeneralLogLevel(Configuration::Log& parent)
{
    // if no old-skool GeneralLogLevel child elements: nothing to do
    auto& oldLogLevels = parent.GeneralLogLevel();
    if(oldLogLevels.empty()) return;

    // if has a new-skool <LogLevel name='GeneralLogLevel...> child element then ignore old-skool <GeneralLogLevel ...> elements: favour the new
    auto& newLogLevels = parent.LogLevel();
    const auto matchNewLogLevelByNameFn = [](Configuration::LogLevel& logLevel) { return "GeneralLogLevel" == logLevel.name(); };
    if(std::find_if(begin(newLogLevels), end(newLogLevels), matchNewLogLevelByNameFn) != end(newLogLevels))
    {
        Configuration::DecorationUtils::clear(parent, oldLogLevels, Configuration::Log::GeneralLogLevel_id);
    }

    // convert old-skool GeneralLogLevel to new-skool LogLevel element
    for(Configuration::GeneralLogLevel& oldLogLevel : oldLogLevels)
    {
        Configuration::LogLevel newGeneralLogLevel("GeneralLogLevel", oldLogLevel.logLevel());
        Configuration::DecorationUtils::push_back(parent, newLogLevels, newGeneralLogLevel, Configuration::Log::LogLevel_id);        
    }

    Configuration::DecorationUtils::clear(parent, oldLogLevels, Configuration::Log::GeneralLogLevel_id); // discard all old
}

void Meta::BackwardsCompatibilityUtils::convertOldComponentLogLevel(Configuration::ComponentLogLevels& parent)
{
    // if no old-skool ComponentLogLevel child elements: nothing to do
    auto& oldLogLevels = parent.ComponentLogLevel();
    if(oldLogLevels.empty()) return;

    // if has new-skool <LogLevel name='XYZ...> child elements, then remove any matching old-skool <ComponentLogLevel ...> elements: favour the new
    auto& newLogLevels = parent.LogLevel();
    for(Configuration::LogLevel& newLogLevel : newLogLevels)
    {
        const std::string& componentName(newLogLevel.name());
        const auto matchOldLogLevelByNameFn = [componentName](Configuration::ComponentLogLevel& oldLogLevel) { return oldLogLevel.componentName() == componentName; };
        oldLogLevels.erase(std::remove_if(begin(oldLogLevels), end(oldLogLevels), matchOldLogLevelByNameFn));
    }

    // convert old-skool ComponentLogLevel elements to new-skool LogLevel elements
    for(Configuration::ComponentLogLevel& oldLogLevel : oldLogLevels)
    {
        Configuration::LogLevel newLogLevel(oldLogLevel.componentName(), oldLogLevel.logLevel());
        Configuration::DecorationUtils::push_back(parent, newLogLevels, newLogLevel, Configuration::Log::LogLevel_id);        
    }

    Configuration::DecorationUtils::clear(parent, oldLogLevels, Configuration::ComponentLogLevels::ComponentLogLevel_id); // discard all old
}
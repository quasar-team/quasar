/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ComponentAttributes.cpp
 *
 *  Created on: Aug 18, 2015
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

#include "ComponentAttributes.h"

ComponentAttributes::ComponentAttributes(const Log::LogComponentHandle& handle, const std::string& name, const Log::LOG_LEVEL& initialLogLevel/*=INF*/)
:m_handle(handle), m_name(name), m_level(initialLogLevel)
{}

std::string ComponentAttributes::getName() const
{
    return m_name;
}

Log::LogComponentHandle ComponentAttributes::getHandle() const
{
	return m_handle;
}

Log::LOG_LEVEL ComponentAttributes::getLevel() const
{
    return m_level;
}

void ComponentAttributes::setLevel(const Log::LOG_LEVEL& level)
{
    m_level = level;
}

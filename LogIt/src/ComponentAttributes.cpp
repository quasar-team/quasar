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

ComponentAttributes::ComponentAttributes(const uint32_t& id, const std::string& name, const Log::LOG_LEVEL& level/*=INF*/)
:m_id(id), m_name(name), m_level(level)
{}

ComponentAttributes::ComponentAttributes(const ComponentAttributes& obj)
:m_id(obj.m_id), m_name(obj.m_name), m_level(obj.m_level)
{}

ComponentAttributes& ComponentAttributes::operator=(const ComponentAttributes& obj)
{
    if(&obj == this) return *this;

    m_id = obj.m_id;
    m_name = obj.m_name;
    m_level = obj.m_level;

    return *this;
}

uint32_t ComponentAttributes::getId() const
{
    return m_id;
}

std::string ComponentAttributes::getName() const
{
    return m_name;
}

Log::LOG_LEVEL ComponentAttributes::getLevel() const
{
    return m_level;
}

void ComponentAttributes::setLevel(const Log::LOG_LEVEL& level)
{
    m_level = level;
}

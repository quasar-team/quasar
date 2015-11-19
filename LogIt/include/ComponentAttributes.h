/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ComponentAttributes.h
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

#ifndef COMPONENTATTRIBUTES_H_
#define COMPONENTATTRIBUTES_H_

#include <string>
#include <stdint.h>
#include "LogLevels.h"

class ComponentAttributes
{
public:
	ComponentAttributes(const uint32_t& id, const std::string& name, const Log::LOG_LEVEL& level = Log::INF);
    ComponentAttributes(const ComponentAttributes& obj);
    ComponentAttributes& operator=(const ComponentAttributes& obj);

    uint32_t getId() const;
    std::string getName() const;

    Log::LOG_LEVEL getLevel() const;
    void setLevel(const Log::LOG_LEVEL& level);

private:
    uint32_t m_id;
    std::string m_name;
    Log::LOG_LEVEL m_level;
};

#endif /* COMPONENTATTRIBUTES_H_ */

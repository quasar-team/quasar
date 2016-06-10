/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogSinks.cpp
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
#include "LogSinks.h"

using std::vector;

LogSinks::LogSinks()
{}

/**
 * Clean up sink memory.
 */
LogSinks::~LogSinks()
{
    for(vector<LogSinkInterface*>::iterator it = m_sinks.begin(); it!= m_sinks.end(); ++it)
    {
        delete *it;
    }
}

/**
 * Ownership of the sink is transferred to this object (deleted in the ~dtor)
 */
void LogSinks::addSink(LogSinkInterface* sink)
{
    m_sinks.push_back(sink);
}

void LogSinks::logMessage(const std::string& logMsg)
{
    for(vector<LogSinkInterface*>::iterator it = m_sinks.begin(); it!= m_sinks.end(); ++it)
    {
        (*it)->logMessage(logMsg);
    }
}

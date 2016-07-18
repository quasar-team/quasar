/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogRecord.cpp
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

#include "LogRecord.h"
#include "LogItInstance.h"
#include "LogIt.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/format.hpp>

//namespace boost = boost_1_54_0;

using boost::posix_time::time_facet;
using boost::posix_time::microsec_clock;

using std::string;

const string g_sTimestampFormat = "%Y-%m-%d %H:%M.%s";

LogRecord::LogRecord(const string& file, const int& line, const Log::LOG_LEVEL& level)
{
    initializeStream(file, line, level)<<"] ";
}

LogRecord::LogRecord(const string& file, const int& line, const Log::LOG_LEVEL& level, const uint32_t& componentId)
{
    initializeStream(file, line, level)<<", "<<Log::componentIdToString(componentId)<<"] ";
}

LogRecord::~LogRecord()
{
    m_stream.flush();
    LogItInstance::getInstance()->m_logSinksInstance.logMessage(m_stream.str());
}

std::ostringstream& LogRecord::initializeStream(const string& file, const int& line, const Log::LOG_LEVEL& level)
{
    m_stream.imbue(std::locale(m_stream.getloc(), new time_facet(g_sTimestampFormat.c_str())));
    m_stream << microsec_clock::local_time() << " ["<<stripDirectory(file)<<":"<<line<<", "<<Log::logLevelToString(level);
    return m_stream;
}

std::ostream& LogRecord::getStream()
{
    return m_stream;
}

const string LogRecord::stripDirectory(const string& file)
{
    const size_t trailingSlashPosition = file.find_last_of("/\\");

    if(trailingSlashPosition == std::string::npos) return file;

    if(trailingSlashPosition+1 >= file.length()) return file;

    return file.substr(trailingSlashPosition+1);
}

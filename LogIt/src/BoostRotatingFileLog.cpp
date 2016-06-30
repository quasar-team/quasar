/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * BoostRotatingFileLog.cpp
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
#include "BoostRotatingFileLog.h"

#include <boost/move/move.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/log/core/core.hpp>

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/common.hpp>

#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>

namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

using sinks::text_ostream_backend;
using sinks::text_file_backend;

const std::string fileNamePattern = "file_%5N.log";
const std::string logDirectory = "log";
const size_t maxLogFileSizeBytes = 5000000;
const size_t maxLogFileCount = 10;

bool BoostRotatingFileLog::initialize()
{
    addFileSink();
    return true;
}

void BoostRotatingFileLog::logMessage(const std::string& logEntry)
{
    boost::log::record record = m_boostLogger.open_record();

    if(record)
    {
        boost::log::record_ostream stream(record);
        stream << logEntry;
        stream.flush();
		m_boostLogger.push_record(boost::move(record));
    }
}

void BoostRotatingFileLog::addFileSink()
{
    boost::shared_ptr<text_file_backend> fileBackend = boost::make_shared<text_file_backend>(
            keywords::file_name = fileNamePattern,
            keywords::rotation_size = maxLogFileSizeBytes,
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0) );

    typedef sinks::synchronous_sink<text_file_backend> sink_t;
    boost::shared_ptr<sink_t> fileSink(new sink_t(fileBackend));
    fileSink->locked_backend()->auto_flush(true);

    // manage logfile rolling and deleting oldest file
    fileSink->locked_backend()->set_file_collector(sinks::file::make_collector(
            keywords::target = logDirectory,
            keywords::max_size = maxLogFileCount * maxLogFileSizeBytes));

    // takes account of files already in the target log directory (otherwise old log files are overwritten).
    fileSink->locked_backend()->scan_for_files();

    boost::log::core::get()->add_sink(fileSink);
}

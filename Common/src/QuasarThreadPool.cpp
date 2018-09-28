/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * QuasarThreadPool.cpp
 *
 *  Created on: 4 May 2018
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

#include <algorithm>

#include <QuasarThreadPool.h>
#include <LogIt.h>

namespace Quasar
{

ThreadPool::ThreadPool (unsigned int maxThreads, unsigned int maxJobs):
        m_quit(false),
        m_maxJobs(maxJobs)
{
    m_workers.reserve(maxThreads);
    for (unsigned int i=0; i<maxThreads; ++i)
        m_workers.emplace_back( [this](){this->work();} );
}

ThreadPool::~ThreadPool ()
{
    LOG(Log::INF) << "Stopping threadpool - this might take some time.";
    m_quit = true;
    m_conditionVariable.notify_all();
    for (std::thread &t : m_workers)
        t.join();
    LOG(Log::INF) << "Stopped the threadpool";
    // all threads are stopped now, but are all jobs flushed?
    while (!m_pendingJobs.empty())
    {
        ThreadPoolJob* job = m_pendingJobs.front();
        m_pendingJobs.pop();
        LOG(Log::WRN) << "Removing unfinished job: " << job->describe();
        delete job;
    }
}

void ThreadPool::work()
{
    while (!m_quit)
    {
        std::unique_lock<std::mutex>lock (m_accessLock);
        if (! m_pendingJobs.empty())
        {
            ThreadPoolJob *job = m_pendingJobs.front();
            m_pendingJobs.pop();
            unsigned int size = m_pendingJobs.size();
            lock.unlock();
            LOG(Log::TRC) << "Removed job from the threadpool, current number of jobs is:" << size;
            try
            {
                job->execute();
            }
            catch (...)
            {
                LOG(Log::ERR) << "Job '" << job->describe() << "' has thrown an unhandled exception. The job description was '" + job->describe() + "'";
            }

            delete job;
        }
        else
        {
            m_conditionVariable.wait(lock);
        }
    }
}

UaStatus ThreadPool::addJob (ThreadPoolJob* job)
{
    {
        std::lock_guard<std::mutex>lock (m_accessLock);
        if (m_pendingJobs.size() >= m_maxJobs)
        {
            LOG(Log::ERR) << "The threadpool is already full (it has limit of " << m_maxJobs << " jobs. Cant add new jobs. Enlarge the threadpool";
            return OpcUa_BadResourceUnavailable;
        }
        m_pendingJobs.push(job);
    }
    m_conditionVariable.notify_one();
    LOG(Log::TRC) << "Added new job to threadpool, current number of jobs is:" << m_pendingJobs.size();
    return OpcUa_Good;
}

UaStatus ThreadPool::addJob (const std::function<void()>& functor, const std::string& description)
{
    class StdFunctionJob: public ThreadPoolJob
    {
    public:
        StdFunctionJob (
                const std::function<void()>& functor,
                const std::string& description) :
                    m_functor(functor),
                    m_description(description) {}
        virtual void execute() { m_functor(); }
        virtual std::string describe() const { return m_description; }
    private:
        const std::function<void()> m_functor;
        const std::string m_description;

    };
    StdFunctionJob *job = new StdFunctionJob (functor, description);
    return this->addJob (job);
}

}



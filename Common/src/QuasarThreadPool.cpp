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
    LOG(Log::TRC) << "Stopping threadpool";
    m_quit = true;
    m_conditionVariable.notify_all();
    std::for_each( m_workers.begin(), m_workers.end(), [](std::thread &t){t.join();} );
    LOG(Log::TRC) << "Stopped the threadpool";
}

void ThreadPool::work()
{
    while (!m_quit)
    {
        std::unique_lock<std::mutex>lock (m_accessLock);
        if (m_pendingJobs.size()>0)
        {
            ThreadPoolJob *job = m_pendingJobs.front();
            m_pendingJobs.pop_front();
            unsigned int size = m_pendingJobs.size();
            lock.unlock();
            LOG(Log::TRC) << "Removed job from the threadpool, current number of jobs is:" << size;
            try
            {
                job->execute();
            }
            catch (...)
            {
                LOG(Log::ERR) << "Job has thrown an unhandled exception.";
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
            LOG(Log::ERR) << "The threadpool is already full, cant add new jobs. Enlarge the threadpool";
            return OpcUa_BadResourceUnavailable;
        }
        m_pendingJobs.push_back(job);
    }
    m_conditionVariable.notify_one();
    LOG(Log::TRC) << "Added new job to threadpool, current number of jobs is:" << m_pendingJobs.size();
    return OpcUa_Good;
}

UaStatus ThreadPool::addJob (const std::function<void()>& functor)
{
    class StdFunctionJob: public ThreadPoolJob
    {
    public:
        StdFunctionJob ( const std::function<void()>& functor ) : m_functor(functor) {}
        virtual void execute() { m_functor(); }
    private:
        const std::function<void()> m_functor;

    };
    StdFunctionJob *job = new StdFunctionJob (functor);
    return this->addJob (job);
}

}



/*
 * QuasarThreadPool.cpp
 *
 *  Created on: 4 May 2018
 *      Author: pnikiel
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
    std::lock_guard<std::mutex>lock (m_accessLock);
    if (m_pendingJobs.size() >= m_maxJobs)
    {
        LOG(Log::ERR) << "The threadpool is already full, cant add new jobs. Enlarge the threadpool";
        return OpcUa_BadResourceUnavailable;
    }
    m_pendingJobs.push_back(job);
    m_conditionVariable.notify_one();
    LOG(Log::TRC) << "Added new job to threadpool, current number of jobs is:" << m_pendingJobs.size();
    return OpcUa_Good;
}

}



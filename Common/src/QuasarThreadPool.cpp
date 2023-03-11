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
        m_maxJobs(maxJobs),
        m_jobsAcceptedCounter(0),
        m_jobsFinishedCounter(0)
{
    m_threadPoolLogId = Log::getComponentHandle("ThreadPool");
    if (m_threadPoolLogId == Log::INVALID_HANDLE)
        throw std::logic_error("ThreadPool initialized before ThreadPool LogIt handle is initialized");
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
    for (auto it = std::begin(m_pendingJobs); it != std::end(m_pendingJobs); )
    {
        /* Users should be aware what was left unexecuted, that's why we just don't to list.clear() */
        LOG(Log::WRN) << "Removing unfinished job: " << (*it)->describe();
        it = m_pendingJobs.erase (it);
    }
}

/** This method finds the next job suitable to be dealt by the next available worker.
 * It can finish with two potential outcomes:
 * -- there is no suitable job to execute
 *      returns nullptr for the job, does not change the job list
 * -- there is a suitable job to execute (either w/o a mutex or with a mutex that is free)
 *      locks that specifix mutex (if applicable)
 *      returns the job ptr and the lock
 *      removes that job from the list
 */
ThreadPool::Duty ThreadPool::findSomeDuty ()
{
    LOG(Log::TRC, m_threadPoolLogId) << "--> Find some duty";
    for (auto iter = std::begin(m_pendingJobs); iter != std::end(m_pendingJobs); iter++)
    {
        if (! (*iter)->associatedMutex() ) // no synchro domain
        {
            Duty duty;
            duty.job = std::move(*iter);
            m_pendingJobs.erase(iter);
            LOG(Log::TRC, m_threadPoolLogId) << "<-- Removed job from the threadpool, current number of jobs is:" << m_pendingJobs.size();
            return duty;
        }
        else // there is a synchro domain, dunno if free?
        {
            Duty duty;
            /* can we grab it ? */
            std::unique_lock<std::mutex> lock (*(*iter)->associatedMutex(), std::try_to_lock);
            if (!lock.owns_lock())
            {
                LOG(Log::TRC, m_threadPoolLogId) << "<-- Could not lock mutex [" << (*iter)->associatedMutex() << "] for job [" << (*iter)->describe() << "]";
                continue;
            }
            /* so, we own the lock... */
            duty.lock = std::move(lock);
            duty.job = std::move(*iter);
            m_pendingJobs.erase(iter);
            LOG(Log::TRC, m_threadPoolLogId) << "<-- Removed job [" << duty.job->describe() << "] from the threadpool, current #jobs is:" << m_pendingJobs.size();
            return duty;
        }
    }
    LOG(Log::TRC, m_threadPoolLogId) << "<-- No executable duty";
    return Duty(); // by default no job, i.e. can't find anything to do now.
}

void ThreadPool::work()
{
    while (!m_quit)
    {
        Duty duty;
        { /* synchro block for the internals */
            std::unique_lock<std::mutex>lock (m_accessLock);
            duty = findSomeDuty();
        }
        if (!duty.job)
        {
            std::unique_lock<std::mutex>lock (m_accessLock);
            m_conditionVariable.wait_for(lock, std::chrono::milliseconds(100));
            continue;
        }
        /* So, we found a job to execute */
        try
        {
            duty.job->execute();
        }
        catch (...)
        {
            LOG(Log::ERR) << "Job '" << duty.job->describe() <<
                "' has thrown an undeterminate exception. The job description was '" + duty.job->describe() + "'";
        }
        if (duty.job->associatedMutex())
        {
            std::unique_lock<std::mutex>lock (m_accessLock);
            //! Piotr: This line is super important: unlocking of the associated mutex
            //! MUST happen within m_accessLock context
            duty.job->associatedMutex()->unlock();
        }
        m_jobsFinishedCounter++;
    }
}

UaStatus ThreadPool::addJob (std::unique_ptr<ThreadPoolJob> && job)
{
    {
        std::lock_guard<std::mutex>lock (m_accessLock);
        if (m_pendingJobs.size() >= m_maxJobs)
        {
            LOG(Log::ERR) << "The threadpool is already full (it has limit of " << m_maxJobs << " jobs. Cant add new jobs. Enlarge the threadpool";
            return OpcUa_BadResourceUnavailable;
        }
        m_pendingJobs.push_back(std::move(job));
        LOG(Log::TRC) << "Added new job [" << m_pendingJobs.back()->describe() << "] to threadpool, current number of jobs is:" << m_pendingJobs.size();
    }
    
    m_conditionVariable.notify_one();
    m_jobsAcceptedCounter++;
    return OpcUa_Good;
}

UaStatus ThreadPool::addJob (const std::function<void()>& functor, const std::string& description, std::mutex* mutex)
{
    class StdFunctionJob: public ThreadPoolJob
    {
    public:
        StdFunctionJob (
                const std::function<void()>& functor,
                const std::string& description,
                std::mutex* mutex = nullptr) :
                    m_functor(functor),
                    m_description(description),
                    m_mutex(mutex) {}
        virtual void execute() { m_functor(); }
        virtual std::string describe() const { return m_description; }
        virtual std::mutex* associatedMutex() const { return m_mutex; }
    private:
        const std::function<void()> m_functor;
        const std::string m_description;
        std::mutex* m_mutex;

    };
    // make_unique would be much better, but officially we're still not C++14... 
    return this->addJob (std::unique_ptr<ThreadPoolJob> (new StdFunctionJob (functor, description, mutex)));
}

size_t ThreadPool::getNumPendingJobs ()
{
    std::lock_guard<std::mutex>lock (m_accessLock);
    return m_pendingJobs.size();
}

}



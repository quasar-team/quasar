/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * QuasarThreadPool.h
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

#ifndef COMMON_INCLUDE_QUASARTHREADPOOL_H_
#define COMMON_INCLUDE_QUASARTHREADPOOL_H_

#include <mutex>
#include <vector>
#include <list>
#include <thread>
#include <map>
#include <condition_variable>
#include <functional>
#include <atomic>

#include <LogIt.h>
#include <statuscode.h>

namespace Quasar
{

class ThreadPoolJob
{
public:
    virtual ~ThreadPoolJob() {};

    virtual void execute() = 0;

    virtual std::string describe() const = 0;

    // Can be nullptr if this job is not protected by any mutex.
    virtual std::mutex* associatedMutex() const = 0;
};

class ThreadPool
{
public:
    ThreadPool (unsigned int maxThreads, unsigned int maxJobs);
    ~ThreadPool ();

    UaStatus addJob (std::unique_ptr<ThreadPoolJob> && job);
    UaStatus addJob (const std::function<void()>& functor, const std::string& description, std::mutex* mutex = nullptr);

    void notifyExternalEvent () { m_conditionVariable.notify_one(); };

    //! How many jobs are buffered for execution ?
    size_t getNumPendingJobs ();

    size_t getNumJobsAccepted () { return m_jobsAcceptedCounter.load(); }
    size_t getNumJobsFinished () { return m_jobsFinishedCounter.load(); }


private:
    void work();

    std::mutex m_accessLock;
    bool m_quit;
    std::vector<std::thread> m_workers;
    std::list<std::unique_ptr<ThreadPoolJob>> m_pendingJobs;

    const unsigned int m_maxJobs;

    // this is the notification business for conditional variable notification
    std::condition_variable m_conditionVariable;

    struct Duty
    {
        std::unique_ptr<ThreadPoolJob> job;
        std::unique_lock<std::mutex> lock;
        Duty() : job(nullptr) {};
    };

    //! Search for a job that can be presently executed, if found remove it from the list.
    Duty findSomeDuty ();

    std::atomic_size_t m_jobsAcceptedCounter;
    std::atomic_size_t m_jobsFinishedCounter;

    Log::LogComponentHandle m_threadPoolLogId;

};

}

#endif /* COMMON_INCLUDE_QUASARTHREADPOOL_H_ */

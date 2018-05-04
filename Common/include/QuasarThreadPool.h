/*
 * QuasarThreadPool.h
 *
 *  Created on: 4 May 2018
 *      Author: pnikiel
 */

#ifndef COMMON_INCLUDE_QUASARTHREADPOOL_H_
#define COMMON_INCLUDE_QUASARTHREADPOOL_H_

#include <mutex>
#include <vector>
#include <thread>
#include <list>
#include <condition_variable>

#include <statuscode.h>

namespace Quasar
{

class ThreadPoolJob
{
public:
    virtual ~ThreadPoolJob() {};

    virtual void execute() = 0;
};

class ThreadPool
{
public:
    ThreadPool (unsigned int maxThreads, unsigned int maxJobs);
    ~ThreadPool ();

    UaStatus addJob (ThreadPoolJob* job);

private:
    void work();

    std::mutex m_accessLock;
    bool m_quit;
    std::vector<std::thread> m_workers;
    std::list<ThreadPoolJob*> m_pendingJobs;

    const unsigned int m_maxJobs;

    // this is the notification business for conditional variable notification
    std::condition_variable m_conditionVariable;

};

}

#endif /* COMMON_INCLUDE_QUASARTHREADPOOL_H_ */

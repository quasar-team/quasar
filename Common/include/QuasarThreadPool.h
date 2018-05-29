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
#include <thread>
#include <list>
#include <queue>
#include <condition_variable>
#include <functional>

#include <statuscode.h>

namespace Quasar
{

class ThreadPoolJob
{
public:
    virtual ~ThreadPoolJob() {};

    virtual void execute() = 0;

    virtual std::string describe() const = 0;
};

class ThreadPool
{
public:
    ThreadPool (unsigned int maxThreads, unsigned int maxJobs);
    ~ThreadPool ();

    UaStatus addJob (ThreadPoolJob* job);
    UaStatus addJob (const std::function<void()>& functor, const std::string& description);

private:
    void work();

    std::mutex m_accessLock;
    bool m_quit;
    std::vector<std::thread> m_workers;
    std::queue<ThreadPoolJob*, std::list<ThreadPoolJob*> > m_pendingJobs;

    const unsigned int m_maxJobs;

    // this is the notification business for conditional variable notification
    std::condition_variable m_conditionVariable;

};

}

#endif /* COMMON_INCLUDE_QUASARTHREADPOOL_H_ */

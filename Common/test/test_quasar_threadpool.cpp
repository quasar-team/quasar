/*
 * test_quasar_threadpool.cpp
 *
 *  Created on: 7 May 2018
 *      Author: pnikiel
 */


#include <QuasarThreadPool.h>
#include <iostream>

#include <LogIt.h>



class MyJob: public Quasar::ThreadPoolJob
{
public:
    virtual void execute()
    {
        std::cout << "Starting job" << std::endl;
        double x = M_PI;
        for (unsigned int i=0; i<50E6; ++i)
            x *= 1.00000001;
        std::cout << "Finishing job, output=" << x << std::endl;
    }
    virtual std::string describe() const { return ""; }
};

// check if the CPU load scales to 300% (if you have at least 3 cores)

int main ()
{
    Log::initializeLogging(Log::WRN);
    Quasar::ThreadPool threadPool (3, 1E6);
    for (int i=0; i<100E3; ++i)
        threadPool.addJob(new MyJob());
    usleep(30E6);
}

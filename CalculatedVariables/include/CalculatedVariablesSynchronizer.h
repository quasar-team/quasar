/*
 * CalculatedVariableSynchronizer.h
 *
 *  Created on: 27 Nov 2018
 *      Author: pnikiel
 */

#ifndef CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESSYNCHRONIZER_H_
#define CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESSYNCHRONIZER_H_

#include <boost/thread/mutex.hpp>

namespace CalculatedVariables
{

class Synchronizer
{
public:
    boost::mutex& mutex() { return m_mutex; }

private:
    boost::mutex m_mutex;
};

typedef std::shared_ptr<Synchronizer> SharedSynchronizer;

}

#endif /* CALCULATEDVARIABLES_INCLUDE_CALCULATEDVARIABLESSYNCHRONIZER_H_ */

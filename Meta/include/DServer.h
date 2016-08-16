/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * DServer.h
 *
 *  Created on: Nov 5, 2015
 * 		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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
#ifndef __DServer__H__
#define __DServer__H__

#include <vector>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DServer.h>


namespace Device
{




class
    DServer
    : public Base_DServer
{

public:
    /* sample constructor */
    explicit DServer ( const Configuration::Server & config

                       , DRoot * parent

                     ) ;
    /* sample dtr */
    ~DServer ();
	void updateConnectedClientCount(unsigned int count);
	void updateRemainingCertificateValidity(const std::string& remainingValidity);



    /* delegators for
    cachevariables and sourcevariables */


private:
    /* Delete copy constructor and assignment operator */
    DServer( const DServer & );
    DServer& operator=(const DServer &other);

    // ----------------------------------------------------------------------- *
    // -     CUSTOM CODE STARTS BELOW THIS COMMENT.                            *
    // -     Don't change this comment, otherwise merge tool may be troubled.  *
    // ----------------------------------------------------------------------- *

public:

private:


};





}

#endif // include guard

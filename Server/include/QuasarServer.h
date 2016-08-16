/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * QuasarServer.h
 *
 *  Created on: Nov 6, 2015
 * 		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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

#ifndef QUASARSERVER_H_
#define QUASARSERVER_H_

#include "BaseQuasarServer.h"

/*
 * Example class. This class overrides functionality from BaseQuasarServer in order to make the logic fit an specific implementation.
 * THIS CLASS SHOULD BE MODIFIED BY THE FINAL USER to contain his custom logic, or taken as an example to create his own separate file.
 */
class QuasarServer : public BaseQuasarServer{
public:
    QuasarServer();
    virtual ~QuasarServer();
    //Main loop of the application logic.
    virtual void mainLoop();
    //Method for initialising LogIt. Can be overided for a specific implementation, but a default initialization is already provided.
    virtual void initializeLogIt();
    /*
     * Method for initialising Custom Modules, to be overwritten by the final user
     * return: When the return is 0, the execution will continue normally. When the return is different than 0 it will exit the server execution.
     */
    virtual void initialize();
    //Method for deinitialising Custom Modules, to be overwritten by the final user
    virtual void shutdown();
private:
    //Disable copy-constructor and assignment-operator
    QuasarServer( const QuasarServer& server );
    void operator=( const QuasarServer& server );

};

#endif /* QUASARSERVER_H_ */

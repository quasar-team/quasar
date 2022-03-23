/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * main.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *      Author: Damian Abalo Miron <damian.abalo@cern.ch>
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

#include "QuasarServer.h"

int main (int argc, char *argv[])
{   
    try
    {
        QuasarServer quasarServer ;
        return quasarServer.startApplication(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
	return -1;
    }

}

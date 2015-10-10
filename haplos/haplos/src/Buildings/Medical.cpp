//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
//
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include "Medical.h"
#include "Building.h"


Medical::Medical(int i, int x, int y, int capacity, int visitorCapacity, int patientCapcity) : Building('M', i, x, y, capacity, visitorCapacity){
    this->maxPatientCapacity= patientCapcity;
    this->currentPatientCapacity=0;

}

int Medical::getMaxPatientCapacity(){
    return this->maxPatientCapacity;
}

int Medical::getCurrentPatientCapacity(){
    return this->currentPatientCapacity;
}

void Medical::setCurrentPatientCapacity(int p){
    this->currentPatientCapacity=p;
}

void Medical::removePatient(Person *p){
    currentPatients.erase(p->getID());
}

void Medical::addPatient(Person *p){
    currentPatients[p->getID()] = p;
}

std::unordered_map<int, Person *> Medical::getPatients(){
    return currentPatients;
    
}

int Medical::getTotalNumberOfPeople(){
    return currentPatients.size()+Building::getTotalNumberOfPeople();
}

std::string Medical::toString(){
    std::string returnString=Building::toString();
    returnString += ","+std::to_string(maxPatientCapacity)+","+std::to_string(currentPatientCapacity);
    return returnString;
    
}

Medical::~Medical(){
    
}
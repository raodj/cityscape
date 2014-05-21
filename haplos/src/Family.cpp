//------------------------------------------------------------
//
// This file is part of HAPLOS availabe off the website at
// <http://pc2lab.cec.miamiOH.edu/haplos>
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

#include "Family.h"
#include <vector>

using namespace std;

Family::Family() {
    hasAdult=false;
    numberOfPeople=0;
}

Person* Family::getPerson(int id){
    return &members[id];
    
}

int Family::getNumberOfPeople(){
    return numberOfPeople;
    
}

Person* Family::getAllPersons(){
    return &members[0];
}
void Family::addPerson(Person newPerson){
    members.push_back(newPerson);
    if(newPerson.getAge()>17){
        hasAdult=true;
    }
    numberOfPeople++;
    
}

void Family::setLocation(int x, int y){
    for(std::vector< Person >::iterator it = members.begin(); it!= members.end(); ++it){
        it->setLocation(x, y);
    }
}

bool Family::getHasAdult(){
    return hasAdult;
}
Family::~Family() {
	// TODO Auto-generated destructor stub
}

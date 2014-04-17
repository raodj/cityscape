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

#include "Location.h"
#include <cstdlib>

using namespace std;

Location::Location(){
	numberOfPeople=0;
	maxPopulation=0;
	coordinates[0]=-1;
	coordinates[1]=-1;
}
Location::Location(int x, int y, float m) {
	// TODO Auto-generated constructor stub
	numberOfPeople=0;
	maxPopulation=m;
	coordinates[0]=x;
	coordinates[1]=y;
}

int Location::getCurrentPopulation() const {
	return numberOfPeople;
}

bool Location::isFull(){
	if (numberOfPeople>=(int)maxPopulation) {
		return true;
	}
	else{
		return false;
	}
}
float Location::getMaxPopulation() const{
	return maxPopulation;
}
int* Location::getCoordinates(){
	return coordinates;
}
void Location::addPerson(){
	numberOfPeople++;
}

void Location::removePerson(){
	numberOfPeople--;
}

Location &Location::operator = (const Location &p){
	if (this!=&p) {
		numberOfPeople=p.numberOfPeople;
		maxPopulation=p.maxPopulation;
		coordinates[0]=p.coordinates[0];
		coordinates[1]=p.coordinates[1];
	}
	return *this;
}
Location::~Location() {
	// TODO Auto-generated destructor stub

}


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
#include "Daycare.h"
#include "Building.h"

Daycare::Daycare(int i, int x, int y, int capacity, int visitorCapacity, int childCapacity) : Building('D', i, x, y, capacity, visitorCapacity){
    this->maxChildCapacity=childCapacity;
    this->childCapacity=0;
    this->currentChildren.reserve(this->maxChildCapacity);
}

Daycare::Daycare(const Daycare &d) : Building(d){
    this->childCapacity = d.childCapacity;
    this->maxChildCapacity = d.maxChildCapacity;
    this->currentChildren.clear();
    this->currentChildren.insert(d.currentChildren.begin(), d.currentChildren.end());
}

Daycare &Daycare::operator = (const Daycare &d){
    if (this!=&d) {
        Building::operator = (d);
        this->childCapacity = d.childCapacity;
        this->maxChildCapacity = d.maxChildCapacity;
        this->currentChildren.clear();
        this->currentChildren.insert(d.currentChildren.begin(), d.currentChildren.end());
        
    }
    return *this;
}

int Daycare::getChildCapacity(){
    return childCapacity;
}

int Daycare::getMaxChildCapacity(){
    return maxChildCapacity;
}

void Daycare::removeChild(int pID){
    currentChildren.erase(pID);
}

void Daycare::addChild(int pID, int familyID){
    currentChildren[pID] = familyID;
}

std::unordered_map<int, int>Daycare::getChildren(){
    return currentChildren;
    
}

int Daycare::getTotalNumberOfPeople(){
    return currentChildren.size()+Building::getTotalNumberOfPeople();
}


void Daycare::setChildCapacity(int c){
    childCapacity=c;
}

std::string Daycare::exportString(){
    std::string returnString = "*";
    returnString += std::string(1, getType())+"\n";
    returnString += std::to_string(getID())+"\n";
    returnString += std::to_string(getMaxCapacity())+"\n";
    returnString += std::to_string(getMaxVisitorCapacity())+"\n";
    int *l = getLocation();
    returnString += std::to_string(l[0])+","+std::to_string(l[1])+"\n";
    returnString += std::to_string(this->maxChildCapacity)+"\n";
    return returnString;
}


Daycare::~Daycare(){
    
}

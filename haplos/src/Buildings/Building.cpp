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

#include "Building.h"

using namespace std;

Building:: Building(){
    type = 'U';
    idNumber = -1;
    location[0] = -1;
    location[1] = -1;
    maxCapacity = -1;
    currentCapacity = 0;
    visitorCapacity = 0;
    currentEmployees.clear();
    currentVisitors.clear();
}

Building::Building(char t, int i, int x, int y, int capacity, int visitorCapacity){
    this->type = t;
    this->idNumber = i;
    this->location[0] = x;
    this->location[1] = y;
    this->maxCapacity = capacity;
    this->currentCapacity = 0;
    this->visitorCapacity = visitorCapacity;
    currentEmployees.clear();
    currentVisitors.clear();
}

Building::Building(const Building &b){
    this->type = b.type;
    this->idNumber = b.idNumber;
    this->location[0] = b.location[0];
    this->location[1] = b.location[1];
    this->maxCapacity = b.maxCapacity;
    this->currentCapacity = b.currentCapacity;
    this->visitorCapacity = b.visitorCapacity;
    this->currentEmployees.clear();
    this->currentVisitors.clear();
    this->currentEmployees.insert(b.currentEmployees.begin(), b.currentEmployees.end());
    this->currentVisitors.insert(b.currentVisitors.begin(), b.currentVisitors.end());

}

int Building::getID(){
    return idNumber;
}

int Building::getMaxCapacity(){
    return maxCapacity;
}

int Building::getCurrentCapacity(){
    return currentCapacity;
}

void Building::setCurrentCapacity(int c){
    currentCapacity=c;
}

int Building::getMaxVisitorCapacity(){
    return visitorCapacity;
}

void Building::setMaxVisitorCapacity(int v){
    visitorCapacity = v;
}
int Building::getCurrentVisitorCapacity(int time){
    return currentVisitorCapacity[time];
}

void Building::setCurrentVisitorCapacity(int time, int c){
    currentVisitorCapacity[time]=c;
}

void Building::addVisitorTimeSlot(int startTime, int endTime){
    for(int i = startTime; i<endTime; i++){
        currentVisitorCapacity[i]++;
    }
}

std::unordered_map<int, Person *>Building::getVisitors(){
    return currentVisitors;
}

std::unordered_map<int, Person *>Building::getEmployees(){
    return currentEmployees;
}

void Building::addVisitor(Person *p){
    currentVisitors[p->getID()] = p;
}

void Building::removeVisitor(Person *p){
    currentVisitors.erase(p->getID());
}

void Building::addEmployee(Person *p){
    currentEmployees[p->getID()] = p;
}

void Building::removeEmployee(Person *p){
    currentEmployees.erase(p->getID());
}

int Building::getTotalNumberOfPeople(){
    return currentEmployees.size() + currentVisitors.size();
}

void Building::setID(int id){
    this->idNumber= id;
}
void Building::setLocation(int x, int y){
    this->location[0]=x;
    this->location[1]=y;
}

int* Building::getLocation(){
    //std::cout<<"Location: "<<location[0]<<","<<location[1]<<std::endl;
	return location;
}

char Building::getType() const{
    return type;
}
std::string Building::toString(){
    std::string returnString="ID: "+std::to_string(this->idNumber)+
                             "\n Type: "+std::string(1, this->type)+
                             "\n Capacity: "+std::to_string(this->maxCapacity)+
                             "\n Visitor Capacity: "+std::to_string(this->visitorCapacity)+
                             "\n Location: "+std::to_string(this->location[0])+","+std::to_string(this->location[1]);
    return returnString;
}

std::string Building::exportString(){
    std::string returnString = "*";
    returnString += std::string(1, this->type)+"\n";
    returnString += std::to_string(this->idNumber)+"\n";
    returnString += std::to_string(this->maxCapacity)+"\n";
    returnString += std::to_string(this->visitorCapacity)+"\n";
    returnString += std::to_string(this->location[0])+","+std::to_string(this->location[1])+"\n";
    return returnString;
}

Building::~Building(){
    
    
}


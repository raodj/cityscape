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
#include <sstream>
#include "Person.h"
#include "Schedule/Schedule.h"
#include "Schedule/TimeSlot.h"

Person::Person(){
	age=-1;
	gender='n';
    currentLocation=-1;
    schedule = Schedule();
}

Person::Person(int a, char g, int b, int id_number, int scheduleType, bool goToJobLocation) {
	age=a;
	gender=g;
    currentLocation = b;
    id_num = id_number;
    schedule = Schedule(scheduleType, goToJobLocation);
}

void Person::setLocation(int b){
    currentLocation=b;
}
Person::Person(const Person &p){
    this->id_num=p.id_num;
	this->age=p.age;
	this->gender=p.gender;
	this->currentLocation=p.currentLocation;
    this->schedule = p.schedule;
}

Person &Person::operator = (const Person &p){
	if (this!=&p) {
		this->age=p.age;
		this->gender=p.gender;
        this->id_num=p.id_num;
        this->currentLocation=p.currentLocation;
        this->schedule = p.schedule;
	}
	return *this;
}

char Person::getGender(){
	return gender;
}

void Person::setGender(char g){
	gender=g;
}

int Person::getAge(){
	return age;
}

int Person::getLocation(){
	return currentLocation;
}

int Person::getID(){
    return id_num;
}

void Person::setSchedule(Schedule s){
    schedule = Schedule(s);
}

Schedule* Person::getSchedule(){
    return &schedule;
}
std::string Person::toString(){
    std::ostringstream outputString;
    outputString << "\"Person ID:\",\"" << this->id_num << '"' << std::endl;
    outputString << "\"Age:\",\"" << this->age <<'"' << std::endl;
    outputString << "\"Schedule:\"" << std::endl << schedule.toString() << std::endl;
    return outputString.str();
}

TimeSlot* Person::getCurrentTimeSlot(){
    return schedule.getCurrentTimeSlot();
}

TimeSlot* Person::updateToNextTimeStep(){
    TimeSlot* t = schedule.getNextLocation();
    currentLocation = t->getLocation();
    return t;
}

void Person::setCustomAttribute(std::string key, std::string value){
    customAttributes[key] = value;
}

std::string Person::getCustomAttribute(std::string key){
    return customAttributes[key];
}

std::string Person::exportPerson(){
    std::ostringstream outputString;
    outputString << "*";
    outputString << id_num<< std::endl;
    outputString << age<< std::endl;
    outputString << gender<< std::endl;
    outputString << std::to_string(schedule.getScheduleType())<<std::endl;
    outputString << schedule.exportSchedule();
    return outputString.str();
}

Person::~Person() {
	// TODO Auto-generated destructor stub
    
}



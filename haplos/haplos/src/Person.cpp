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

Person::Person(){
	age=-1;
	gender='n';
	location[0]=-1;
	location[1]=-1;
    schedule = Schedule();
}

Person::Person(int a, char g, int x, int y, int id_number, int scheduleType) {
	age=a;
	gender=g;
	location[0]=x;
	location[1]=y;
    id_num = id_number;
    schedule = Schedule(scheduleType);
}

void Person::setLocation(int x, int y){

	location[0]=x;
	location[1]=y;
}
Person::Person(const Person &p){
    id_num=p.id_num;
	age=0;
	gender=' ';
	age=p.age;
	gender=p.gender;
	location[0]=p.location[0];
	location[1]=p.location[1];
    schedule= p.schedule;
}

Person &Person::operator = (const Person &p){
	if (this!=&p) {
		age=0;
		gender=' ';
		age=p.age;
		gender=p.gender;
        id_num=p.id_num;
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

int* Person::getLocation(){
	return location;
}

int Person::getID(){
    return id_num;
}

Schedule* Person::getSchedule(){
    return &schedule;
}
std::string Person::toString(){
    std::ostringstream outputString;
    outputString << "\n\n\"Person ID:\",\"" << this->id_num << '"' << std::endl;
    outputString << "\"Age:\",\"" << this->age <<'"' << std::endl;
    outputString << "\"Schedule:\"" << std::endl << schedule.toString() << std::endl;
    return outputString.str();
}

Person::~Person() {
	// TODO Auto-generated destructor stub
    
}



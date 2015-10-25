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

#include "School.h"
#include "Building.h"
#include <math.h>

School::School(int i, int x, int y, int capacity) : Building('S', i, x, y, capacity, 0){
    //Determine Grade Capacity
    //20% of School are not Teachers (teachers can have up to 30 students)
    int avalibleTeachers = (int)capacity * 0.20;
    //Schools very rarely offer all grades, split at elementry, middle, and high
    schoolType = (int)rand() % 2;
    
    int currentGrade =0;
    int endGrade=0;
    int startGrade=0;
    
    switch(schoolType){
        case 0:
            //Elementry
            //Grades 0-5
            endGrade=5;
            startGrade=0;
            currentGrade=0;
            break;
        case 1:
            //Middle
            //Grades 6-8
            endGrade=8;
            startGrade=6;
            currentGrade=6;
            break;
        case 2:
            //High School
            //Grades 9-12
            endGrade=12;
            startGrade=9;
            currentGrade=9;
            break;
        default:
            std::cout<<"Invalid School Type"<<std::endl;
            break;
    }
    
    this->studentCapacity = 0;
    this->studentMaxCapacity=avalibleTeachers*30;
    setMaxVisitorCapacity(studentMaxCapacity*2);

    
    
    //7:00AM (42) - 10:00AM (60) (needs to be chagned to a 15 minute scale)
    this->schoolStart=(int)rand() % 18 + 42;
    
    //6 (36) to 8 (48) hours of school (needs to be changed to a hour scale)
    this->schoolEnd= schoolStart+ ((int)rand() % 12 + 36);
    
}


School::School(int i, int x, int y, int capacity, int forceSchoolType) : Building('S', i, x, y, capacity, 0){
    //Determine Grade Capacity
    //20% of School are not Teachers (teachers can have up to 30 students)
    int avalibleTeachers = (int)ceil(capacity * 0.20);
    //Schools very rarely offer all grades, split at elementry, middle, and high
    schoolType =forceSchoolType;
    //std::cout<<"Avalaible teachers: "<<avalibleTeachers<<std::endl;
    int currentGrade =0;
    endGrade=0;
    startGrade=0;
    
    
    switch(schoolType){
        case 0:
            //Elementry
            //Grades 0-5
            endGrade=5;
            startGrade=0;
            currentGrade=0;
            break;
        case 1:
            //Middle
            //Grades 6-8
            endGrade=8;
            startGrade=6;
            currentGrade=6;
            break;
        case 2:
            //High School
            //Grades 9-12
            endGrade=12;
            startGrade=9;
            currentGrade=9;
            break;
        default:
            std::cout<<"Invalid School Type"<<std::endl;
            break;
    }

    this->studentCapacity = 0;
    this->studentMaxCapacity=avalibleTeachers*30;
    setMaxVisitorCapacity(studentMaxCapacity*2);
    //7:00AM (42) - 10:00AM (60) (needs to be chagned to a 15 minute scale)
    this->schoolStart=(int)rand() % 18 + 42;
    
    //6 (36) to 8 (48) hours of school (needs to be changed to a hour scale)
    this->schoolEnd= schoolStart+ ((int)rand() % 12 + 36);
    //std::cout<<studentCapacity<<"/"<<studentMaxCapacity<<std::endl;
    //std::cout<<"ID Number: "<<this->getID()<<std::endl;
    //std::cout<<"\tStart Time: "<<schoolStart<<std::endl;
    //std::cout<<"\tEnd Time: "<<schoolEnd <<std::endl;
}


School::School(int i, int x, int y, int capacity, int visitorCapacity, int childMax,  int forceSchoolType, int schoolStart, int schoolEnd) : Building('S', i, x, y, capacity, visitorCapacity){
    schoolType =forceSchoolType;
    this->studentCapacity = 0;
    this->studentMaxCapacity=childMax;
    this->schoolStart = schoolStart;
    this->schoolEnd = schoolEnd;
}


int School::getSchoolStartTime(){
    return schoolStart;
}

int School::getSchoolEndTime(){
    return schoolEnd;
}

int School::getStudentMaxCapacity(){
    return studentMaxCapacity;
}

int School::getStudentCapacity(){
    return studentCapacity;
}

bool School::hasGradeAvaliable(int age, int numberOfStudents){
    int grade=age-5;
   // std::cout<<"Start Time: "<<schoolStart<<" "<<this->getID()<<std::endl;
    //std::cout<<"Capacity: "<<studentCapacity<<" "<<studentMaxCapacity<<std::endl;
    if(endGrade>=grade && startGrade<=grade){
        if(studentMaxCapacity-studentCapacity>=numberOfStudents){
            //Has Grade Avaliable
                return true;
        }
    }
    return false;
}


void School::assignStudentToSchool(int grade){
    //std::cout<<"Grade: "<<grade<<std::endl;
    studentCapacity++;
    
}

int School::getTotalNumberOfPeople(){
    return currentStudents.size()+Building::getTotalNumberOfPeople();
}

void School::removeStudent(Person *p){
    currentStudents.erase(p->getID());
}

void School::addStudent(Person *p){
    currentStudents[p->getID()] = p;
}

std::unordered_map<int, Person *> School::getStudents(){
    return currentStudents;
    
}

std::string School::exportString(){
    std::string returnString = "*";
    returnString += std::string(1, getType())+"\n";
    returnString += std::to_string(getID())+"\n";
    returnString += std::to_string(getMaxCapacity())+"\n";
    returnString += std::to_string(getMaxVisitorCapacity())+"\n";
    int *l = getLocation();
    returnString += std::to_string(l[0])+","+std::to_string(l[1])+"\n";
    returnString += std::to_string(schoolType)+"\n";
    returnString += std::to_string(studentMaxCapacity)+"\n";
    returnString += std::to_string(schoolStart)+"\n"+std::to_string(schoolEnd)+"\n";

    return returnString;
}

School::~School(){
    
}
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

#ifndef __haplos__School__
#define __haplos__School__

#include <iostream>
#include "Building.h"

class School : public Building{
    /*Class for Representing a School Building.*/
    public:
        /** The default constructor for this class.
         
         \param[in] id ID number of Building
         \param[in] x row number of building
         \param[in] y col number of building
         \param[in] capacity max number of employees.
         \param[in] gradeCapacity max number of students they can have in a given grade.
         */
        School(int i, int x, int y, int capacity);
    
        /** The alternative constructor for this class.
         
         \param[in] id ID number of Building
         \param[in] x row number of building
         \param[in] y col number of building
         \param[in] capacity max number of employees.
         \param[in] gradeCapacity max number of students they can have in a given grade.
         \param[in] forceSchoolType 0=Elementry, 1= Middle, 2= High School
         */
        School(int i, int x, int y, int capacity, int forceSchoolType);
    
        School(int i, int x, int y, int capacity, int visitorCapacity, int childMax,  int forceSchoolType, int schoolStart, int schoolEnd) ;

    
        /**Get time of day when school will start (based on a 144 hour time units)
         \return Time of School Starting
         */
        int getSchoolStartTime();
    
        /*Get time of day when school will end (baesd on 144 hour time units)
         \return Time of School Ending
         */
        int getSchoolEndTime();
    
        /*Get the max capacity of the school for students.
         \return Time of School Ending
         */
        int getStudentMaxCapacity();
    
        /*Get the current capacity of the school for students.
         \return Time of School Ending
         */
        int getStudentCapacity();
    
        /*Returns true if it has a avaliable student slot for a given grade
         */
        bool hasGradeAvaliable(int grade, int numberOfStudents);
    
        /*Add a Student to a grade
         */
        void assignStudentToSchool(int grade);
    
        void addStudent(Person *p);
    
        void removeStudent(Person *p);
    
        std::unordered_map<int, Person *> getStudents();
    
        using Building::getTotalNumberOfPeople;
        int getTotalNumberOfPeople();
    
        std::string exportString();
    
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~School();
    private:
        int studentCapacity;
        int studentMaxCapacity;
        int schoolStart;
        int schoolEnd;
        int startGrade;
        int endGrade;
        int schoolType;
        std::unordered_map <int, Person *> currentStudents;
    
    
};
#endif /* defined(__haplos__School__) */

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

#ifndef __haplos__Building__
#define __haplos__Building__

#include "../Person.h"

#include <iostream>
#include <unordered_map>


class Person;
class Schedule;
class Building {
    /*Class for Representing a Unclassified Building*/
    public:
        /** The default constructor for this class. All values would have to be manually set.
         */
        Building();
        /** Class with All main Paramaters
         
         \param[in] type Type of Building ('H'= Home, 'B' = Business, 'S'=School)
         \param[in] id ID number of Building
         \param[in] x row number of building
         \param[in] y col number of building
         \param[in] capacity max capacity of building (not including visitors)
         \param[in] number of visitors allowed in building (not including those in capacity)
         */
        Building(char type, int idNumber, int x, int y, int capacity, int visitorCapacity);
    
        /** Copy Constructor
         \param[in] b building to copy.
         */
        Building(const Building &b);
        /**Get ID of Building
         \return ID of Building
         */
        int getID();
    
        /**Get max capacity of Building
         \return max capacity of Building
         */
        int getMaxCapacity();
    
        /**Get current capacity of Building
         \return current capacity of Building
         */
        int getCurrentCapacity();
    
        /**Set Current Capacity to c
         \param[in] c new current capacity;
         */
        void setCurrentCapacity(int c);
    
        /**Get max visitor capacity of Building
         \return max visitor capacity of Building
         */
        int getMaxVisitorCapacity();
    
        /**Get current visitor capacity of Building at a specific timeslot
         \param[in] time  time slot to get visitor capacity
         \return current visitor capacity of Building
         */
        int getCurrentVisitorCapacity(int time);
    
        /**Set current visitor capacity of Building
         \param[in] time time slot to set visitor capacity
         \param[in] c new visitor capacity to change to.
         */
        void setCurrentVisitorCapacity(int time, int c);
    
        /**Set ID of Building
         \param[in] id id of building
         */
        void setID(int i);
    
        /**Set location of Building
         \param[in] x 
         \param[in] y
         */
        void setLocation(int x, int y);
    
        void addVisitor(Person *p);
    
        void removeVisitor(Person *p);
    
        void addEmployee(Person *p);
    
        void removeEmployee(Person *p);
    
        void addVisitorTimeSlot(int startTime, int endTime);
    
        std::unordered_map<int, Person *> getEmployees();
    
        std::unordered_map<int, Person *> getVisitors();
        /** Return the coordinates of the current location of entity
         
         \return An array consisting of [rows, cols]
         */
    
        int* getLocation();
    
        /**Return the Char representing the type of building
         \return the Char repreenting the type of building
         */
        char getType() const;
    
        int getTotalNumberOfPeople();

    
        std::string exportString();
    
        /** Returns a Human Readable Version on Information about the building.
         
         \return Human Readable String of Information about the Person
         */
        std::string toString();

        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Building();
    private:
        char type;
        int maxCapacity;
        int currentCapacity;
        int visitorCapacity;
        int currentVisitorCapacity[1008]; //Visitor Capacity at each time incriment for a week
        std::unordered_map<int, Person *>currentEmployees;
        std::unordered_map<int, Person *>currentVisitors;
        int idNumber;
        int location[2];
    
};

#endif /* defined(__haplos__Building__) */

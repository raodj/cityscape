
#ifndef FAMILY_H_
#define FAMILY_H_

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

#include <vector>
#include <unordered_map>
#include "Person.h"
#include "Buildings/Building.h"
#include "Buildings/School.h"
#include "Buildings/TransportHub.h"
#include "Buildings/Daycare.h"

class Building;
class Schoool;
class Person;

class Family {
    /** A class specifically for repersenting a family of indviduals. */
    public:
        /** The default constructor for this class.
         
         \param[in] size Size of family.
         */
        Family();
    
        Family(Building *home, Daycare *daycare);
    
    
        Family(const Family &f);
    
        /** Get a member of a family.
         \param id  id of family memember to retrive.
         \return An array of memembers in the family.
         */
        Person* getPerson(int id);
        
        int getNumberOfPeople();
        
        /** Get All memembers of the family
         \return Vector of all family memembers
         */
        Person* getAllPersons();
        
        /**Add new Person to Family
         \param newPerson Person to be added
         */
        void addPerson(Person newPerson);
        
        /**Sets starting location for family
         \param b ID of where family is
         */
        void setLocation(int b);
    
        /**Assign a Home Number to a Family
          \param[in] n pointer of new Home Building.
         */
        void setHome(Building *n);
    
        /**Assign a Daycare Number to a Family
         \param[in] n pointer of new Home Building.
         */
        void setDaycare(Daycare *d);
    
        /** Get home number
         \return pointer of home location.
         */
        Building* getHome();
    
        /** Get Daycare number
         \return pointer of Daycare location.
         */
        Daycare* getDaycare();
    
        /** Get if Family has an Adult.
         \return True if family has at least 1 adult in it, false if otherwise.
         */
        bool getHasAdult();
    
        /** Get if Family has an Young Child (Younger than 14).
         \return True if family has at least 1 young child in it, false if otherwise.
         */
        int getHasYoungChild();
    
        /** Get if Family has an School Child.
         \return True if family has at least 1 school child in it, false if otherwise.
         */
        bool getHasSchoolChild();
    
        /** Get if Family has an Young School Child.
         \return True if family has at least 1 young school child in it, false if otherwise.
         */
        bool getHasYoungSchoolChild();
    
        Person* getChildCareAdult();
    
        std::string toString();
    
        std::string exportFamily();
    
        void updateToNextTimeStep(std::unordered_map<int, Building*> *allBuildings);
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Family();
        
    private:
        std::vector < Person > members; //Vector containing all members in the family
        int numberOfPeople; //Number of People in the Family
        bool hasAdult;  //Has an person 18 and older in the family
        int hasYoungChild; //Has Child that is under the age of 14 and needs to have adult supervision
        bool hasYoungSchoolChild;
        bool hasSchoolChild;
        int childCareAdultPos;
        Building *homeNumber; //Pointer to Home Building.
        Daycare *daycare;  //Pointer to Daycare for Family.
    
    
};

#endif /* FAMILY_H_ */

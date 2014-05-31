
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

#import <vector>
#import "Person.h"
class Family {
    /** A class specifically for repersenting a family of indviduals. */
    public:
        /** The default constructor for this class.
         
         \param[in] size Size of family.
         */
        Family();
        
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
         \param x X Location of Family.
         \param y Y Location of family.
         */
        void setLocation(int x, int y);
    
        /**Assign a Home Number to a Family
          \param[in] n Number of Home Building.
         */
        int setHomeNumber(int n);
    
        /** Get if Family has an Adult.
         \return True if family has at least 1 adult in it, false if otherwise.
         */
        bool getHasAdult();
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Family();
        
    private:
        std::vector < Person > members;
        int numberOfPeople;
        bool hasAdult;
        int homeNumber;
    
    
};

#endif /* FAMILY_H_ */

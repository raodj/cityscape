
#ifndef LOCATION_H_
#define LOCATION_H_

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

#include <unordered_map>
#include "Person.h"
#include "Buildings/Building.h"
#include "Buildings/Medical.h"
#include "Buildings/School.h"

#include "Family.h"
class Person;
class Family;
class Location {
    /** A class specifically for handeling set area of land.
     */
public:
    
    /** The default constructor for this class.
     */
	Location();
    
    /** The alternative constructor for this class.
     
     \param[in] x Row location in grid.
     \param[in] y Col locaiton in grid.
     \param[in] m Max capacity of location.
     \param[in] d Density of Locaiton
     
     */
	Location(int x, int y, float m, float d);
    
    /** Adds a person to the location.
     
     \param p Person to add to Location
     */
	void addPerson(Person *p);
    
    /** Add a Entire Family to Location.
     
     \param f Family to be added to location.
     */
    void addFamily(Family *f);
    
    /** Add a Building to Location.
     
     \param b Building to be added to location.
     */
    Building* addBuilding(Building *b);
    
    /** Return the current number of people at location.
     
     \return The number of people currently at location.
     */
	int getCurrentPopulation() const;
    
    /** Return the coordinates of location.
     
     \return An array containing the coordinates of the location [row, col]
     */
	int* getCoordinates();
    
    /** Copy Constructor
     */
	Location &operator=(const Location &p);
    
    /** Return the maximumn capacity for a location.
     
     \return The maximumn copacity for a location.
     */
	float getMaxPopulation() const;
    
    /** Return the density for a location.
     
     \return The density for a location.
     */
    float getDensity() const;
    
    /** Return the number of buildings in a location.
     
     \return The number of buildings in a location.
     */
    int getNumberOfBuildings() const;
    
    /** Check to see if location is currently full.
     
     \return True if location is full, false if location is not.
     */
	bool isFull();
    
    /** Checks to see if a there exist a building has a avalible slot for person in a given visitor type.
     \param[in] vistorType type of visitor (E=Employee, V=Standard Visitor, P=Patient)
     \param[in] startTime start time of slot (required for standard visitor)
     \param[in] endTime end time of slot (required for standard visitor
     \return pointer to building that has avaliable visitor slot
     */
    Building* hasAvaliableBuilding(char visitorType, int startTime, int endTime);
    
    
    /** Removes a person from location.
     \param idNum ID Number of Person to Remove
     */
	void removePerson(int idNum);
    
    void printTemp();
    
    /**
     The destructor.
     
     Currently the destructor does not have any specific task to
     perform in this class.  However, it is defined for adherence
     with conventions and for future extensions.
     */
	virtual ~Location();
private:
	int numberOfPeople;
	int coordinates [2];
	float maxPopulation;
    float density;
    std::unordered_map<int,Person*> people;
    std::unordered_map<int, Building*> buildings;
    Building *tmp;

};


#endif /* LOCATION_H_ */

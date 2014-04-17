
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
     
     */
	Location(int x, int y, float m);
    
    /** Adds a person to the location.
     */
	void addPerson();
    
    /** Return the current number of people at location.
     
     \return The number of people currently at location.
     */
	int getCurrentPopulation() const;
    
    /** Return the coordinates of location.
     
     \return An array containing the coordinates of the location [row, col]
     */
	int* getCoordinates();
    
	Location &operator=(const Location &p);
    
    /** Return the maximumn capacity for a location.
     
     \return The maximumn copacity for a location.
     */
	float getMaxPopulation() const;
    
    /** Check to see if location is currently full.
     
     \return True if location is full, false if location is not.
     */
	bool isFull();
    
    /** Removes a person from location.
     */
	void removePerson();
    
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

};


#endif /* LOCATION_H_ */

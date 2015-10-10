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

#ifndef __haplos__TransportHub__
#define __haplos__TransportHub__
#include "Building.h"
#include <iostream>
class TransportHub : public Building {
    /*Class for Representing a Business Building*/
public:
    
    TransportHub();
    
    /** The default constructor for this class.
     
     \param[in] buildID ID number of Building
     \param[in] x row number of building
     \param[in] y col number of building
     */
    TransportHub(int buildID, int x, int y);
    
    void addPrivateTransport(int homeNumber, Person *p);
    
    void removePrivateTransport(int homeNumber, Person *p);
    
    std::unordered_map<int, Person *> getPrivateTransport(int homeNumber);
    
    using Building::getTotalNumberOfPeople;
    int getTotalNumberOfPeople();
    
    /**
     The destructor.
     
     Currently the destructor does not have any specific task to
     perform in this class.  However, it is defined for adherence
     with conventions and for future extensions.
     */
    virtual ~TransportHub();
private:
    std::unordered_map<int, std::unordered_map<int, Person *>> privateTransport;

    
};
#endif /* defined(__haplos__TransportHub__) */

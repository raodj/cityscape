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

#ifndef __haplos__Medical__
#define __haplos__Medical__

#include <iostream>
#include "Building.h"

class Medical : public Building {
    /*Class for Representing a Medical Building (Doctor's Office, Hospital, etc).*/
    public:
        /** The default constructor for this class.
         
         \param[in] id ID number of Building
         \param[in] x row number of building
         \param[in] y col number of building
         \param[in] capacity max number of employees.
         \param[in] visitorCapacity max number of visitors a can have in a given hour.
         \param[in] patientCapacity Max number of patients a medical building can have.
         */
        Medical(int i, int x, int y, int capacity, int visitorCapcity, int patientCapcity);
    
    
        Medical(const Medical &m);
        
        Medical &operator=(const Medical &m);
    
        /** Return the Max Capacity for Patients
         \return max capacity for patients
         */
        int getMaxPatientCapacity();
    
        /** Return current number of Patients at Medical Building
         \return current number of visitors at Medical Building.
         */
        int getCurrentPatientCapacity();
    
        /** Set the value for Current Patient Capacity
         \param[in] p new current patient capacity
         */
        void setCurrentPatientCapacity(int p);
    
        void addPatient(int pID, int familyID);
    
        void removePatient(int pID);
    
        std::unordered_map<int, int> getPatients();
    
        using Building::getTotalNumberOfPeople;
        int getTotalNumberOfPeople();
    
        /** Returns a Human Readable Version on Information about the building.
         
         \return Human Readable String of Information about the Person
         */
        std::string toString();
    
        std::string exportString();
    
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Medical();
    private:
        int maxPatientCapacity = 0;
        int currentPatientCapacity = 0;
        std::unordered_map<int, int> currentPatients;

};

#endif /* defined(__haplos__Medical__) */
//
//  Medical.h
//  haplos
//
//  Created by Emily on 5/30/14.
//  Copyright (c) 2014 Emily. All rights reserved.
//

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
        virtual ~Medical();
    private:
        int maxPatientCapacity;
        int currentPatientCapacity;

};

#endif /* defined(__haplos__Medical__) */
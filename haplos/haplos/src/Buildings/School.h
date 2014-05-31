//
//  School.h
//  haplos
//
//  Created by Emily on 5/30/14.
//  Copyright (c) 2014 Emily. All rights reserved.
//

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
         \param[in] visitorCapacity max number of visitors a can have in a given hour.
         \param[in] gradeCapacity max number of students they can have in a given grade.
         */
        School(int i, int x, int y, int capacity, int visitorCapacity, int gradeCapacity[13]);
    
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~School();
    private:
        int maxVisitorCapacity;
        int *gradeCapacity;
    
    
};
#endif /* defined(__haplos__School__) */

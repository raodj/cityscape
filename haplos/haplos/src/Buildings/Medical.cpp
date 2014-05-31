//
//  Medical.cpp
//  haplos
//
//  Created by Emily on 5/30/14.
//  Copyright (c) 2014 Emily. All rights reserved.
//

#include "Medical.h"
#include "Building.h"


Medical::Medical(int i, int x, int y, int capacity, int visitorCapacity, int patientCapcity) : Building('M', i, x, y, capacity){
    this->maxVisitorCapacity= visitorCapacity;
    this->maxPatientCapacity= patientCapcity;
}

Medical::~Medical(){
    
}
//
//  School.cpp
//  haplos
//
//  Created by Emily on 5/30/14.
//  Copyright (c) 2014 Emily. All rights reserved.
//

#include "School.h"
#include "Building.h"

School::School(int i, int x, int y, int capacity, int visitorCapacity, int *gradeCapacity) : Building('S', i, x, y, capacity, visitorCapacity){
    this->gradeCapacity= gradeCapacity;
}

School::~School(){
    
}
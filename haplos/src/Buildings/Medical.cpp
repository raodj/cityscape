//
//  Medical.cpp
//  haplos
//
//  Created by Emily on 5/30/14.
//  Copyright (c) 2014 Emily. All rights reserved.
//

#include "Medical.h"
#include "Building.h"


Medical::Medical(int i, int x, int y, int capacity, int visitorCapacity, int patientCapcity) : Building('M', i, x, y, capacity, visitorCapacity){
    this->maxPatientCapacity= patientCapcity;
    this->currentPatientCapacity=0;

}

int Medical::getMaxPatientCapacity(){
    return this->maxPatientCapacity;
}

int Medical::getCurrentPatientCapacity(){
    return this->currentPatientCapacity;
}

void Medical::setCurrentPatientCapacity(int p){
    this->currentPatientCapacity=p;
}

std::string Medical::toString(){
    std::string returnString=Building::toString();
    returnString += ","+std::to_string(maxPatientCapacity)+","+std::to_string(currentPatientCapacity);
    return returnString;
    
}

Medical::~Medical(){
    
}
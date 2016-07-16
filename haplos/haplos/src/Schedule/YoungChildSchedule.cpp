/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   YoungChildSchedule.cpp
 * Author: yeshwanth
 * 
 * Created on July 15, 2016, 6:20 PM
 */

#include "YoungChildSchedule.h"

YoungChildSchedule::YoungChildSchedule() {
}

YoungChildSchedule::YoungChildSchedule(Person *p, Family *f, int radiusLimit) {
    currentSchedule = p->getSchedule();
    childCareAdultSchedule = f->getChildCareAdult()->getSchedule();
    daycareLocaiton = f->getDaycare();
    i=0;
    child_AtDaycare=false;

}
    

YoungChildSchedule::ChildState YoungChildSchedule::changefrom_AT_DAYCARE(ChildState & state, TimeSlot *slot) {
    
    if(slot->getLocation() == daycareLocaiton->getID() && slot->getVisitorType()== 'V' && child_AtDaycare){
    // We add a timeslot that signifies that the child will remain at the dayCare only
    // until the end of that time slot and with the end of the timeSlot, the Child
    // will leave the dayCare with the adult.
    currentSchedule->addTimeSlot(TimeSlot(daycareLocaiton->getID(), slot->getEndTime(), 'D'));
    child_AtDaycare = false;
    state = WITH_ADULT;
    return state;
    } 
    return state;
}


YoungChildSchedule::ChildState YoungChildSchedule::changefrom_WITH_ADULT(ChildState & state, TimeSlot *slot, TimeSlot *nextSlot) {
    //Employed Adult Responsible for Child Care
    // If the Employed Child Care Adult is in the dayCare &&
    // if the Child Care Adult is A Visitor/ visiting &&
    // The Child Care Adult has a schedule to follow in the next slot &&
    // That next slot is that of work.
    if(slot->getLocation()==daycareLocaiton->getID() && slot->getVisitorType()== 'V' && nextSlot != NULL && nextSlot->getVisitorType()=='E') {
        child_AtDaycare = true;
        state = AT_DAYCARE;
        return state;
    } else {
        currentSchedule->addTimeSlot(TimeSlot(slot->getLocation(), slot->getEndTime(), slot->getVisitorType()));
    }
    return state;
}

bool YoungChildSchedule::isAdultUnEmployed() {
    if(childCareAdultSchedule->getScheduleType()==4)
        return true;
    else
        return false;
}

void YoungChildSchedule::generateSchedule() {
    // The default state where child is with Adult in the start.
    state = WITH_ADULT;
    // While the childCare adult is present at some location.
    while(childCareAdultSchedule->getLocationAt(i)!= NULL){
        TimeSlot *slot = childCareAdultSchedule->getLocationAt(i);
        TimeSlot *nextSlot = NULL;
        int advance =1;
        // I Guess Conidition is to skip the traveling Schedule in the Child Care Adult Schedule.
        while(childCareAdultSchedule->getLocationAt(i+advance)!= NULL && (childCareAdultSchedule->getLocationAt(i+advance)->getVisitorType() == 'T' ||childCareAdultSchedule->getLocationAt(i+advance)->getVisitorType() == 'C' || childCareAdultSchedule->getLocationAt(i+advance)->getVisitorType() == 'W')){
            advance++;
        }
        
         nextSlot=childCareAdultSchedule->getLocationAt(i+advance);
         
         if(isAdultUnEmployed()) {
             state = WITH_ADULT;
         } else {
             // This indicates the adult is employed
             switch(state) {
                case WITH_ADULT:
                     state = changefrom_WITH_ADULT(state, slot, nextSlot);
                     break;
                 
                case AT_DAYCARE:
                    state = changefrom_AT_DAYCARE(state, slot);
                    break;
             }
         }
         i++;
    }
    
//    if(currentSchedule->getPlan()->size() == 0){
//        std::cout<<"Child Plan is Size Zero"<<std::endl;
//        std::cout<< "Adult's Schedule: "<<std::endl;
//        std::cout<<childCareAdultSchedule->toString()<<std::endl;
//    }

}
    
    YoungChildSchedule::~YoungChildSchedule() {
}


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ScheduleGenerator.h"
#include "../Location.h"
#include "../Population.h"
#include "../Family.h"
#include <map>
#include <vector>

#include "generateYoungSchoolAgedChildSchedule.h"



YoungSchoolAgedChildSchedule::YoungSchoolAgedChildSchedule(Person &p, 
        Family &f, int radiusLimit, bool goToSchool, std::unordered_map<int, Building*> &allBuildings) {
    currentSchedule = p.getSchedule();
    childCareAdultSchedule = f.getChildCareAdult()->getSchedule();
    attendingSchool= static_cast<School* >(allBuildings.at(currentSchedule->getJobLocation()));
    attendingSchool->assignStudentToSchool(0);
    currentSchedule->setGoToJobLocation(goToSchool);
    schoolStartTime = (goToSchool ? attendingSchool->getSchoolStartTime() : 99999);
    schoolEndTime = (goToSchool ? attendingSchool->getSchoolEndTime(): 99999);
    i=0;
    day = 0;
    Child_atSchool=false; // the var expresses if the child is at school or not
    previousSlot = NULL;
    nextSlot = NULL;
}

//std::ostream& operator<<(std::ostream& os, 
//        const YoungSchoolAgedChildSchedule::ChildState& cs) {
//    const std::string EnumNames[] = {"WithAdult", "WithChild" };
//    return (os << EnumNames[cs]);
//}

bool checkif_Adult_AtSchool(TimeSlot &slot, Schedule *currentSchedule) {
    return slot.getLocation() == currentSchedule->getJobLocation();
}

//The method is called when the child's current state is With the Adult.
YoungSchoolAgedChildSchedule::ChildState YoungSchoolAgedChildSchedule::changefrom_WITH_ADULT(ChildState& state, TimeSlot &slot) {

    // School has started and the child is not at School
    if(slot.getEndTime() >= schoolStartTime  && !Child_atSchool){
        //School Starting
        if(slot.getEndTime() != schoolStartTime){
        // Visited School Before School Started.
            if(slot.getEndTime() > schoolStartTime){
                schoolSlotStartTime = slot.getEndTime();
                            
            }
            state = BEFORE_SCHOOL;
            return state;
        }           
    }
    
    if(!checkif_Adult_AtSchool(slot, currentSchedule) && !Child_atSchool) {
        // if the adult is not at school, and the child is not at school,
        // then the child is with the adult.
        return (state = WITH_ADULT);
    }
    return state;
}


YoungSchoolAgedChildSchedule::ChildState YoungSchoolAgedChildSchedule::changefrom_BEFORE_SCHOOL(ChildState& state, TimeSlot &slot) {
    // Add a time slot for time at school
    currentSchedule->addTimeSlot(TimeSlot(currentSchedule->getJobLocation(),
                                                          schoolSlotStartTime,
                                                         'S'));
    // Change the child's state.
    Child_atSchool=true; 
    state = AT_SCHOOL;
    return state;
    
}

YoungSchoolAgedChildSchedule::ChildState YoungSchoolAgedChildSchedule::changefrom_AT_SCHOOL(ChildState& state, TimeSlot &slot) {
    // if the Adult is at School location
        
        // Already at school and School Time is not Over.
            if(Child_atSchool && slot.getEndTime() >= schoolEndTime) {
                // checking if it past school time.
                if(slot.getEndTime() > schoolEndTime){
                    // After School Visitng slot is updated.
                    // If the nextslot of the adult is not that of the school again,
                    // then it implies school time is over and the child is leaving with the 
                    // parent.
                    if(nextSlot != NULL && nextSlot->getLocation() != currentSchedule->getJobLocation())
                        currentSchedule->addTimeSlot(TimeSlot(currentSchedule->getJobLocation(),
                                                              slot.getEndTime(),
                                                              'V'));                        
                        //Update the child State
                        state = AFTER_SCHOOL;
                        return state;
                }
            }

    return state;   //STUB
}


int YoungSchoolAgedChildSchedule::getTimeSlotsPerHour() {
    return 6;
}

int YoungSchoolAgedChildSchedule::to_TimeSlot(const int days) {
    return days * 24 * getTimeSlotsPerHour();
}


void YoungSchoolAgedChildSchedule::AdvanceSchoolSchedule(int &day, int &schoolStartTime, int &schoolEndTime, 
        int nextCycleDayCount, int INFINITE_VAL) {
    
    if(day<4) {
        schoolStartTime += to_TimeSlot(nextCycleDayCount);
        schoolEndTime += to_TimeSlot(nextCycleDayCount);
    }else{
        schoolStartTime = INFINITE_VAL;
        schoolEndTime = INFINITE_VAL;
    }
    day++;
}

YoungSchoolAgedChildSchedule::ChildState YoungSchoolAgedChildSchedule::changefrom_AFTER_SCHOOL(ChildState& state, TimeSlot &slot) {
    // Advance school times to next possible times
    AdvanceSchoolSchedule(day, schoolStartTime, schoolEndTime, 1, 99999);
    // The leaves the school with adult
    Child_atSchool=false;
    // Change State of child to with ChildCareAdult
    state = WITH_ADULT;
    return state;   
}


void YoungSchoolAgedChildSchedule::generateSchedule() {
    
    // Initially the child starts from home.
    state = WITH_ADULT;
    
    // We check until there is no specified location for a time slot.
    while(childCareAdultSchedule->getLocationAt(i)!= NULL) {
        nextSlot = childCareAdultSchedule->getLocationAt(i+1);
        TimeSlot *slot =childCareAdultSchedule->getLocationAt(i);
        switch(state) {
            case WITH_ADULT:
                state = changefrom_WITH_ADULT(state, *slot);
                break;
                
            case BEFORE_SCHOOL:
                state = changefrom_BEFORE_SCHOOL(state, *slot);
                break;
                
            case AT_SCHOOL:
                state = changefrom_AT_SCHOOL(state, *slot);
                break;
                
            case AFTER_SCHOOL:
                state = changefrom_AFTER_SCHOOL(state, *slot);
                break;
                      
        }
        previousSlot = slot;
        i++;
    }
}


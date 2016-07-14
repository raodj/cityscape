/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   generateYoungSchoolAgedChildSchedule.h
 * Author: yeshwanth
 *
 * Created on July 5, 2016, 2:58 PM
 */
#include "ScheduleGenerator.h"
#include "../Policy.h"
#include "../Location.h"
#include "../Population.h"
#include "../Family.h"
#include <map>
#include <vector>


#ifndef GENERATEYOUNGSCHOOLAGEDCHILDSCHEDULE_H
#define GENERATEYOUNGSCHOOLAGEDCHILDSCHEDULE_H

class YoungSchoolAgedChildSchedule {    
public:
    enum ChildState {WITH_ADULT, BEFORE_SCHOOL, AT_SCHOOL, AFTER_SCHOOL };

      
    Schedule *currentSchedule;
    Schedule *childCareAdultSchedule;
    School *attendingSchool;
    std::unordered_map<int, Building*> *allBuildings;
    int schoolStartTime;
    int schoolEndTime;
    
    int schoolSlotStartTime ;
    ChildState state;
    bool Child_atSchool;
    
    int i;
    int day;
    TimeSlot *previousSlot;
    TimeSlot *nextSlot;
    // Constructor
    YoungSchoolAgedChildSchedule();
    YoungSchoolAgedChildSchedule(Person *p, Family *f, int radiusLimit,
                                bool goToSchool, std::unordered_map<int, Building*> *allBuildingscopy);

    //By default he should be at home initially.
    void generateSchedule();
    ChildState changefrom_WITH_ADULT(ChildState& state, TimeSlot *slot);
    ChildState changefrom_BEFORE_SCHOOL(ChildState& state, TimeSlot *slot);
    ChildState changefrom_AT_SCHOOL(ChildState& state, TimeSlot *slot);
    ChildState changefrom_AFTER_SCHOOL(ChildState& state, TimeSlot *slot);
    
    int getTimeSlotsPerHour();
    int to_TimeSlot(const int days);
    void AdvanceSchoolSchedule(int &day, int &schoolStartTime, int &schoolEndTime, 
        int nextCycleDayCount, int INFINITE_VAL);
    
};


#endif /* GENERATEYOUNGSCHOOLAGEDCHILDSCHEDULE_H */


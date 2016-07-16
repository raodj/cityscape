/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   YoungChildSchedule.h
 * Author: yeshwanth
 *
 * Created on July 15, 2016, 6:20 PM
 */

#include "ScheduleGenerator.h"
#include "../Location.h"
#include "../Population.h"
#include "../Family.h"
#include "TimeSlot.h"


#ifndef YOUNGCHILDSCHEDULE_H
#define YOUNGCHILDSCHEDULE_H

class YoungChildSchedule {
public:
    enum ChildState {WITH_ADULT, AT_DAYCARE };
    ChildState state;
    
    Schedule *currentSchedule;
    Schedule *childCareAdultSchedule;
    Daycare  *daycareLocaiton;
    
    int i;
    bool child_AtDaycare;
    
    
    YoungChildSchedule();
    YoungChildSchedule(Person *p, Family *f, int radiusLimit);
    
    void generateSchedule();
    bool isAdultUnEmployed();
    ChildState changefrom_WITH_ADULT(ChildState & state, TimeSlot *slot, TimeSlot *nextSlot);
    ChildState changefrom_AT_DAYCARE(ChildState & state, TimeSlot *slot);
    
    
    
    virtual ~YoungChildSchedule();
private:

};

#endif /* YOUNGCHILDSCHEDULE_H */


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SchoolAgedChildSchedule.cpp
 * Author: yeshwanth
 * 
 * Created on July 18, 2016, 5:06 PM
 */

#include "SchoolAgedChildSchedule.h"
#include "ScheduleGenerator.h"

#include <stdio.h>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

SchoolAgedChildSchedule::SchoolAgedChildSchedule() {
}


SchoolAgedChildSchedule::SchoolAgedChildSchedule(Person *p, Family *f, int radiusLimit,
        double *transportProbablities,
        int *transportRadiusLimits, int *transportRates,
        double *schoolDayVisitorTypeProbablities,
        double *weekendVisitorTypeProbablities,
        bool goToSchool, std::unordered_map<int, Building*> *allBuildings, 
        std::vector< std::vector < Location > > *densityData) {
    // Older School Aged Child (On own after school)
    currentSchedule = p->getSchedule();
    home =f->getHome();
    currentSchedule->setGoToJobLocation(goToSchool);

    //Set Probablities for where they can go
    //double workProb=0;
    //double outProb=0.2;
    //double homeProb=0.8;
    workProb = schoolDayVisitorTypeProbablities[0];
    outProb = schoolDayVisitorTypeProbablities[1];
    homeProb = schoolDayVisitorTypeProbablities[2];

    /*
     * We get the location of the attending school.
     * Assign Student to that School.
     * Get the Location of the school
     * Get the location of home.
     */
    attendingSchool= static_cast<School* >(allBuildings->at(currentSchedule->getJobLocation()));
    attendingSchool->assignStudentToSchool(0);
    schoolLoc = attendingSchool->getLocation();
    homeLoc = home->getLocation();
    
    dayTime=0;  // Time for current day (Midnight = 0, 11:59=144
    trueTime=0; // Actual HAPLOS Time (Monday at Midnight = 0, till Sunday at 11:59 = 1008)
    totalTimeSpentAtHome=0; // Total Time spent at home for the given day
    maxTimeAway = 96;
    totalTimeSpentAway = 0;
    crewfew = 132; 
    visitorType = 'H';
    sleepTimeNeeded = 48;
    transportType = 'T';
    
    this->transportProbablities = transportProbablities;
    this->transportRadiusLimits = transportRadiusLimits;
    this->transportRates = transportRates;
    this->radiusLimit = radiusLimit;
    this->goToSchool = goToSchool;
    this->densityData = densityData;

    // Get School start and End times for reference
    schoolStart = (goToSchool ? dayTime+attendingSchool->getSchoolStartTime() : 99999);
    schoolEnd = (goToSchool ? dayTime+attendingSchool->getSchoolEndTime() : 99999);
    // Add One Hour for Sleep for Previous Day
    timeUpOnMonday = schoolStart;
    
    travelTime = 0;
    travelTimeToHome = 0;
    travelTimeToSchool = 0;
        
}

SchoolAgedChildSchedule::~SchoolAgedChildSchedule() {
}

/**
 * The purpose of computeTransportSpecifics is to make use of the multiple need to compute the
 * transportType, transportRate and travelTime, thus effectively reducing the code length
 * and increasing understandability.
 * @param transportType The type of transport that will be used used ( public,pvt,walking)
 * @param transportRate The cost associated with the transport type that will be chosen.
 * @param travelTime The time taken to travel
 * @param travelTimeToAPlace The travel time to a particular destination
 * @param timeLimit The time within which the possible transportation should occur
 * @param transportProbablities The probability with choosing a particular transportation type
 * @param transportRadiusLimits The radius of operation of each transportation type 
 * @param transportRates  The rates of all transportTypes (public, pvt,walking)
 * @param currentSchedule The schedule of the current individual
 * @param startLoc  The starting location from where the individual must travel.
 * @param destination The destination that is to be reached with the transportType
 * @param visitorType The purpose of visit at a location.
 * @param endTime The HAPLOS time which is nothing but trueTime + dayTime
 */
void SchoolAgedChildSchedule::computeTransportSpecifics(char transportType, int transportRate, int travelTime, int travelTimeToAPlace,
                            int timeLimit, double *transportProbablities, int *transportRadiusLimits,
                            int *transportRates, Schedule *currentSchedule, Building *startLoc,
                            Building *destination, int visitorType, int endTime) {
    
    transportType = determineTransportationType(travelTimeToAPlace, timeLimit,
                                                transportProbablities, transportRadiusLimits, transportRates);

    transportRate = getTransportRate(transportType, transportRates);

    int transportID = -1;
                            
    travelTime= addMoveTo(currentSchedule, startLoc, destination, visitorType, 
                            endTime, transportType, transportRate, transportID);
    
}

void SchoolAgedChildSchedule::determinePlace_basedOn_Prob_AfterSchool(Building* lastPlace, int* lastPlaceLoc) {
    switch(distribution(generator)){
        case 0:{
            //Home
            //update time to reflect staying at home once home
            //**std::cout<<"\t\t\t Staying Home"<<std::endl;
            totalTimeSpentAtHome += crewfew - dayTime;
            if(lastPlace != home){
                computeTransportSpecifics(transportType, transportRate, travelTime,
                    travelTimeToHome, crewfew - dayTime,
                    transportProbablities, transportRadiusLimits,
                    transportRates, currentSchedule, lastPlace, home,
                    visitorType, trueTime+dayTime);
            
                visitorType='H';
                lastPlace= home;
                lastPlaceLoc =lastPlace->getLocation();
            }
            totalTimeSpentAtHome-=travelTime;
            totalTimeSpentAway+=travelTime;
            if(totalTimeSpentAtHome > sleepTimeNeeded){
                totalTimeSpentAway=0;
            }
            dayTime =crewfew;
            // Update state of the child to ATHOME
            state = ATHOME;
            break;
        }
        case 1:
            //Job (Should not Happen)
            break;
        case 2:
        {
            //Out
            //**std::cout<< "\t\t\tFind Place To Go Out (After)" <<std::endl;
            int timeSpentOut=1;
            if(crewfew-dayTime-travelTimeToHome>1){
                timeSpentOut=(int)rand() % (crewfew-dayTime-travelTimeToHome)+1;
            }
                        
            //**std::cout<<"\t\t\tTime To Spend Out: "<<timeSpentOut<<std::endl;
            //(radiusLimit<timeSpentOut-1?radiusLimit:timeSpentOut-1)
            Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                            home->getLocation()[1], 'V', radiusLimit, 
                            trueTime+dayTime, trueTime+dayTime+timeSpentOut,
                            1, transportRate);
            
            if(lastPlace_tmp==NULL){
                //No avaliable place to go so just return back to home
                //**std::cout<<"\t\t After School: No Places found to go out :("<<std::endl;
                //**std::cout<<"\t\t\tTime Frame: "<<trueTime+dayTime<<"-"<<trueTime+dayTime+timeSpentOut<<std::endl;
                if(lastPlace_tmp != home){
                //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                    computeTransportSpecifics(transportType, transportRate, travelTime,
                            travelTimeToHome, crewfew - dayTime,
                            transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace, home,
                            visitorType, trueTime+dayTime);
                    
                    visitorType='H';
                    lastPlace=home;
                    lastPlaceLoc =lastPlace->getLocation();
                    // Update state of the child to ATHOME
                    state = ATHOME;
                }
                            
                //update time to reflect staying at home once home
                totalTimeSpentAtHome += crewfew - dayTime;
                if(totalTimeSpentAtHome >=sleepTimeNeeded){
                    totalTimeSpentAway=0;
                }
                dayTime =crewfew;
            } else{
                //Found a Place to go Visit
                if(lastPlace != lastPlace_tmp || visitorType!='V') {
                    //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                    int *lastPlaceLoc_tmp =lastPlace_tmp->getLocation();
                    //**std::cout<<"Last Place Tmp: "<<lastPlace_tmp->getID()<<std::endl;
                    int travelTimeToNext = calculateTravelTime(lastPlaceLoc[0],
                                            lastPlaceLoc[1], lastPlaceLoc_tmp[0],
                                            lastPlaceLoc_tmp[1], 1);
                    travelTimeToHome = calculateTravelTime(lastPlaceLoc_tmp[0],
                                            lastPlaceLoc_tmp[1], homeLoc[0],
                                            homeLoc[1], 1);
                    computeTransportSpecifics(transportType, transportRate, travelTime,
                            travelTimeToNext, crewfew - dayTime-travelTimeToHome,
                            transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace, lastPlace_tmp,
                            visitorType, trueTime+dayTime);
                                
                    lastPlace=lastPlace_tmp;
                    lastPlaceLoc =lastPlace->getLocation();
                }
                visitorType='V';
                dayTime+=timeSpentOut;
                totalTimeSpentAway+=timeSpentOut;
                totalTimeSpentAway+= travelTime;
                dayTime+=travelTime;
                totalTimeSpentAtHome=0;
                lastPlace->addVisitorTimeSlot(trueTime+dayTime,trueTime+dayTime+timeSpentOut);
                // Update state of the child to ATHOME
                state = OUTSIDE;
            }                        
            break;
        }
    }
     travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
}


void SchoolAgedChildSchedule::determinePlace_basedOn_Prob(Building *lastPlace, int *lastPlaceLoc) {
    switch(distribution(generator)){
        case 0:
        // Home
        travelTimeToSchool = calculateTravelTime(homeLoc[0],
                                                homeLoc[1],
                                                schoolLoc[0],
                                                schoolLoc[1],
                                                1);
        if(lastPlace != home) {                            
        /*Signature of the computeTransportSpecifics.
            computeTransportSpecifics(char transportType,int transportRate,int travelTime,int travelTimeToAPlace,
            int timeLimit,double transportProbablities,int transportRadiusLimits,
            int transportRates,Schedule *currentSchedule,Building *startLoc,
            Building *destination, int visitorType,int trueTime+dayTime)*/
            
            computeTransportSpecifics(transportType, transportRate, travelTime,
            travelTimeToHome, schoolStart-1-travelTimeToSchool-dayTime,
            transportProbablities, transportRadiusLimits,
            transportRates, currentSchedule, lastPlace, home,
            visitorType, trueTime+dayTime);

        lastPlace=home;
        lastPlaceLoc =lastPlace->getLocation();
        
        // Update state of the child to ATHOME
        state = ATHOME;
                            
        visitorType='H';
        }
        dayTime = schoolStart-travelTimeToSchool-1;
        break;
        
        case 1:
        //Job (Should not Happen)
        break;
        
        case 2:
        // Out
        int timeSpentAway = (trueTime+dayTime+schoolStart)-(trueTime+dayTime);
        Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                home->getLocation()[1], 'H', 
                (radiusLimit<timeSpentAway-1? radiusLimit : timeSpentAway-1),
                trueTime+dayTime, trueTime+dayTime+schoolStart, 1, 
                transportRate);
        // If there is no place to go out, we just return home.
        if(lastPlace_tmp==NULL){
            if(lastPlace != home) {
                
                travelTimeToSchool = calculateTravelTime(homeLoc[0],
                                    homeLoc[1], schoolLoc[0], schoolLoc[1], 1);
            
                transportType = determineTransportationType(travelTimeToHome,
                            schoolStart-1-travelTimeToSchool-dayTime,
                            transportProbablities, transportRadiusLimits, 
                            transportRates);
                            transportRate = getTransportRate(transportType, transportRates);
                                
                travelTime = addMoveTo(currentSchedule, lastPlace, home,
                            visitorType, trueTime+dayTime, transportType,
                            transportRate, -1);
                lastPlace=home;
                lastPlaceLoc =lastPlace->getLocation();
                                
                visitorType='H';
                // Update state of child to ATHOME
                state = ATHOME;
            }
            dayTime = schoolStart-travelTimeToSchool-1;
        } else {
                // IF there is a place to go out and not the same as
                // last place, we go there
                if(lastPlace != lastPlace_tmp) {
                    int *lastPlaceLoc_tmp =lastPlace_tmp->getLocation();
                    travelTimeToSchool = calculateTravelTime(lastPlaceLoc_tmp[0],
                            lastPlaceLoc_tmp[1], schoolLoc[0], schoolLoc[1],
                            1);
                    
                    int travelTimeToNext = calculateTravelTime(lastPlaceLoc[0],
                        lastPlaceLoc[1], lastPlaceLoc_tmp[0], 
                        lastPlaceLoc_tmp[1], 1);
            
                    computeTransportSpecifics(transportType, transportRate, travelTime,
                    travelTimeToNext, schoolStart-1-travelTimeToSchool-dayTime,
                    transportProbablities, transportRadiusLimits,
                    transportRates, currentSchedule, lastPlace, lastPlace_tmp,
                    visitorType, trueTime+dayTime);

                    lastPlace=lastPlace_tmp;
                    lastPlaceLoc =lastPlace->getLocation();
                }
            dayTime+=(trueTime+dayTime+schoolStart)-(trueTime+dayTime);
            totalTimeSpentAway+=(trueTime+dayTime+schoolStart)-(trueTime+dayTime);
            totalTimeSpentAtHome=0;
            lastPlace->addVisitorTimeSlot(trueTime+dayTime,trueTime+dayTime+schoolStart);
            visitorType='V';
            // Update state of the child to OUTSIDE
            state = OUTSIDE;
        }
        totalTimeSpentAtHome=0;
        break;
    }
}


void SchoolAgedChildSchedule::generateBeforeSchoolSchedule(Building *lastPlace, int *lastPlaceLoc) {
    // We check if the totalTime spent at home is less than sleep time
    // needed because if it isn't we add additional time needed to rest
    // and prepare schedule based on the accordingly.
    if(totalTimeSpentAtHome<sleepTimeNeeded){
    //**std::cout<<"\t"<<"Adding Addtional Sleep Time"<<std::endl;
    // We calculate the deficit in Sleep Time.
        int timeNeeded = sleepTimeNeeded-totalTimeSpentAtHome;
        // If there is enough time before School Start time, we make use
        // of it by sleeping.
        if(dayTime+timeNeeded<schoolStart-1){
            //Progress Time to Earliest Possible Activty time
            dayTime+=timeNeeded;
            totalTimeSpentAway=0;
            // Update State of the child to AT school!
            state = ATSCHOOL;
        } else{                   
            dayTime=schoolStart-1;
            // We consider the insufficient time before school start 
            // time as time spent away.
            totalTimeSpentAway+=schoolStart-1-dayTime;
            // Update State of the child to AT school!
            state = ATSCHOOL;
            }
    }
    // Add Predicted Time Spent at School to Away Time
    // Essentially the entire time spent at school is considered as time
    // spentAway.
    totalTimeSpentAway+= (schoolEnd - schoolStart)+1;
    
    // If the child has spent sufficient time sleeping/at home and 
    // child has not exceeded the limit on time spent outside and
    // there is time before school starts, we consider this time to be
    // useful for some activity before school hours.
    if (totalTimeSpentAtHome>=sleepTimeNeeded && dayTime<schoolStart-1 &&
       totalTimeSpentAway<maxTimeAway){
        determinePlace_basedOn_Prob(lastPlace, lastPlaceLoc);
    }
}

void SchoolAgedChildSchedule::goingToSchoolSchedule(Building *lastPlace, int *lastPlaceLoc) {
    //**std::cout<<"\tGoing to school"<<std::endl;
    // .Advance time to Start of School
    // If child did not have enough sleep or if child has exceed the max
    // time the child can spend time or if there isn't time before 
    // school, we advance time to start of School
    totalTimeSpentAtHome=0;
    travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                            lastPlaceLoc[1], schoolLoc[0], schoolLoc[1], 1);
    
    
    computeTransportSpecifics(transportType, transportRate, travelTime,
                    travelTimeToSchool, schoolStart-1-dayTime,
                    transportProbablities, transportRadiusLimits,
                    transportRates, currentSchedule, lastPlace, attendingSchool,
                    visitorType, trueTime+schoolStart);
    
    dayTime+=schoolEnd-schoolStart;
    visitorType='S';
    totalTimeSpentAtHome=0;
    lastPlace=attendingSchool;
    lastPlaceLoc =lastPlace->getLocation();
    // Update state of the child, incase the child stayed at home.
    state = ATSCHOOL;
}

void SchoolAgedChildSchedule::generateAfterSchoolSchedule(Building *lastPlace, 
        int *lastPlaceLoc) {
    int travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
    // While there is still time before curfew time and permission to 
    // spend  more time away
    while(dayTime<crewfew-travelTimeToHome-1 && totalTimeSpentAway<maxTimeAway) {
                travelTime = 0;
                determinePlace_basedOn_Prob_AfterSchool(lastPlace, lastPlaceLoc);
    }

    if(lastPlace != home){
        transportType = determineTransportationType(travelTimeToHome,
                            travelTimeToHome, transportProbablities,
                            transportRadiusLimits, transportRates);
                
                
        transportRate = getTransportRate(transportType, transportRates);
                
        travelTime = addMoveTo(currentSchedule, lastPlace, home,
                                visitorType, trueTime+dayTime, transportType,
                                transportRate, -1);
        lastPlace = home;
        lastPlaceLoc =lastPlace->getLocation();
                
        visitorType='H';
        totalTimeSpentAway+= travelTime;
        // Update state of the child to ATHOME
        state = ATHOME;
                
    }
}
void SchoolAgedChildSchedule::generateWeekDaySchedule(Building *lastPlace, int *lastPlaceLoc) {

    SchoolAgedChildSchedule::generateBeforeSchoolSchedule(lastPlace, lastPlaceLoc);
    
    SchoolAgedChildSchedule::goingToSchoolSchedule(lastPlace, lastPlaceLoc);
    
    SchoolAgedChildSchedule::generateAfterSchoolSchedule(lastPlace, lastPlaceLoc);
    
}


void SchoolAgedChildSchedule::determinePlace_basedOn_Prob_WeekEnd(Building *lastPlace, int *lastPlaceLoc, int &timeSpentOut) {
    switch(distribution(generator)){
        case 0:
        {
            //Home
            //update time to reflect staying at home once home
            int travelTime=0;
            if (lastPlace!=home){
                // Update Schedule to Reflect Change of Location
                //**std::cout<<"\tStay Home Case: "<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                computeTransportSpecifics(transportType, transportRate, travelTime,
                    travelTimeToHome, crewfew-dayTime-1, transportProbablities, 
                    transportRadiusLimits, transportRates, currentSchedule, lastPlace, 
                    home, visitorType, trueTime+dayTime);
                
                lastPlace=home;
                lastPlaceLoc =lastPlace->getLocation();
                                
            }
            dayTime+=travelTime;
            timeSpentOut=(int)rand() % (crewfew-dayTime)+1;
            totalTimeSpentAtHome+= timeSpentOut;
                            
            totalTimeSpentAway+=timeSpentOut+travelTime;
            if(totalTimeSpentAtHome>=sleepTimeNeeded){
                totalTimeSpentAway=0;
            }
            visitorType='H';
            dayTime +=timeSpentOut;
            // Update state of the child to ATHOME
            state = ATHOME;
            break;
        }
        case 1:
            //Job (Should not Happen)
            break;
            
        case 2:
            //Out
            ////**std::cout<< "\t\t\tFind Place To Go Out (After)" <<std::endl;
            timeSpentOut=0;
            //**std::cout<<"\t\t\tcrewfew-dayTime-travelTimeToHome-radiusLimit-1: "<<crewfew-dayTime-travelTimeToHome-radiusLimit-1<<std::endl;
            if(crewfew-dayTime-travelTimeToHome-radiusLimit-1>2){
                timeSpentOut=(int)rand() % (crewfew-dayTime-travelTimeToHome-radiusLimit-1)+2;
            }
                            
            if(timeSpentOut+totalTimeSpentAway+travelTimeToHome>maxTimeAway-radiusLimit-1){
                //**std::cout<<"\t\t\tOver Max Time Away: "<<maxTimeAway-totalTimeSpentAway-travelTimeToHome-radiusLimit-1<<std::endl;
                timeSpentOut= maxTimeAway-totalTimeSpentAway-travelTimeToHome-radiusLimit-1;
            }
            int radiusLimitTemp = radiusLimit;
            if(radiusLimit> timeSpentOut) {
                radiusLimit = timeSpentOut-1;
            }
                            
            //**std::cout<<"\t\t\tMax Time Out: "<<maxTimeAway<<std::endl;
            Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                        home->getLocation()[1], 'V', radiusLimit,
                                        trueTime+dayTime, trueTime+dayTime+timeSpentOut,
                                        1, transportRate);
            
            radiusLimit = radiusLimitTemp;
            int travelTime=0;
            if(lastPlace_tmp==NULL){
            //No avaliable place to go so just return back to home
            //**std::cout<<"\t\t\tNo Places found to go out :(";
            //update time to reflect staying at home once home
                totalTimeSpentAtHome+= crewfew - dayTime;
                totalTimeSpentAway+=crewfew - dayTime;
                if(lastPlace != home) {
                    //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                    computeTransportSpecifics(transportType, transportRate, travelTime,
                        travelTimeToHome, crewfew-dayTime-1, transportProbablities, 
                        transportRadiusLimits, transportRates, currentSchedule, lastPlace, 
                        home, visitorType, trueTime+dayTime);

                                    
                    lastPlace=home;
                    lastPlaceLoc =lastPlace->getLocation();
                                    
                }
                visitorType='H';
                dayTime+=travelTime;
                totalTimeSpentAway+=travelTime;
                if(totalTimeSpentAtHome>=sleepTimeNeeded){
                    totalTimeSpentAway=0;
                }
                dayTime =crewfew;
                // Update State of the child to ATHOME
                state = ATHOME;
            } else{
                // There is a place to go outside
                //**std::cout<<"\tVisitor Case: "<<visitorType<<" "<<lastPlace->getID()<<"->"<<lastPlace_tmp->getID()<<" "<<trueTime+dayTime<<std::endl;
                if(lastPlace != lastPlace_tmp || visitorType!= 'V') {
                    int *lastPlaceLoc_tmp =lastPlace_tmp->getLocation();
                    int travelTimeToNext = calculateTravelTime(lastPlaceLoc[0],
                            lastPlaceLoc[1], lastPlaceLoc_tmp[0], lastPlaceLoc_tmp[1],
                            1);
                    
                    travelTimeToHome = calculateTravelTime(lastPlaceLoc_tmp[0],
                                        lastPlaceLoc_tmp[1], homeLoc[0],
                                        homeLoc[1], 1);
                    computeTransportSpecifics(transportType, transportRate, travelTime,
                        travelTimeToNext, crewfew-dayTime-1, transportProbablities, 
                        transportRadiusLimits, transportRates, currentSchedule, lastPlace, 
                        lastPlace_tmp, visitorType, trueTime+dayTime);     
                    
                }
                //**std::cout<<"\t\tTime Spent Out: "<<timeSpentOut<<std::endl;
                lastPlace=lastPlace_tmp;
                lastPlaceLoc =lastPlace->getLocation();
                                
                dayTime+=travelTime;
                totalTimeSpentAway+=travelTime;
                visitorType='V';
                totalTimeSpentAtHome=0;
                totalTimeSpentAway+=timeSpentOut;
                lastPlace->addVisitorTimeSlot(trueTime+dayTime,trueTime+dayTime+timeSpentOut);
                dayTime+=timeSpentOut;
                // Update State of the child to OUTSIDE
                state = OUTSIDE;                
            }
                            
            break;
    }
}

void SchoolAgedChildSchedule::generateWeekEndSchedule(Building *lastPlace, int *lastPlaceLoc, const int &day) {
    state = ATHOME;
    //Weekend
    //**std::cout<<"\tNon-School Day"<<std::endl;
    // Add Sleeping Time if Needed
    // If child spent enough time at home/sleeping.
    if(totalTimeSpentAtHome<sleepTimeNeeded){
        int timeNeeded = sleepTimeNeeded-totalTimeSpentAtHome;
        // Progress Time to Earliest Possible Activty time
        dayTime += timeNeeded;
    }
    int sleepIfOutAllNight = 144-crewfew + timeUpOnMonday;
    if(sleepIfOutAllNight<sleepTimeNeeded && day==6){
        crewfew = crewfew-(sleepTimeNeeded-sleepIfOutAllNight);
    }
    
    // While there is still time left before the curfew time.
            while(dayTime<crewfew-travelTimeToHome-1) {
                travelTime = 0;
                travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
                int timeSpentOut=1;
                if(totalTimeSpentAway+travelTimeToHome+radiusLimit+1>=maxTimeAway){
                    //**std::cout<<"\tForce Home ("<<trueTime+dayTime<<"): "<<dayTime<<std::endl;
                    //**std::cout<<"\t\tTime Need to be Home: "<<sleepTimeNeeded-totalTimeSpentAtHome<<std::endl;
                    //**std::cout<<"\t\tTime Already Spent at Home: "<< totalTimeSpentAtHome<<std::endl;
                    //Been out too Long Need to Get Sleep
                    int timeNeedToBeHome= sleepTimeNeeded-totalTimeSpentAtHome;
                    int travelTime = 0;
                    if(lastPlace != home){
                        //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                        transportType = determineTransportationType(travelTimeToHome,
                                                                    travelTimeToHome,
                                                                    transportProbablities,
                                                                    transportRadiusLimits,
                                                                    transportRates);
                        transportRate = getTransportRate(transportType, transportRates);
                        
                        travelTime=addMoveTo(currentSchedule,
                                             lastPlace,
                                             home,
                                             visitorType,
                                             trueTime+dayTime,
                                             transportType,
                                             transportRate,
                                             -1);
                    }
                    lastPlace=home;
                    lastPlaceLoc =lastPlace->getLocation();
                    
                    dayTime+=timeNeedToBeHome+travelTime;
                    totalTimeSpentAtHome+=timeNeedToBeHome;
                    totalTimeSpentAway+=travelTime+timeNeedToBeHome;
                    if(totalTimeSpentAtHome >=sleepTimeNeeded){
                        totalTimeSpentAway=0;
                    }
                    visitorType='H';
                    
                } else {
                    determinePlace_basedOn_Prob_WeekEnd(lastPlace, lastPlaceLoc, timeSpentOut);
                }
                travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
            }
            if(lastPlace != home){
                //**std::cout<<"\tCrewfew: "<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                transportType = determineTransportationType(travelTimeToHome,
                                                            travelTimeToHome,
                                                            transportProbablities,
                                                            transportRadiusLimits,
                                                            transportRates);
                transportRate = getTransportRate(transportType, transportRates);
                
                travelTime=addMoveTo(currentSchedule,
                                     lastPlace,
                                     home,
                                     visitorType,
                                     trueTime+dayTime,
                                     transportType,
                                     transportRate,
                                     -1);
                lastPlace = home;
                lastPlaceLoc =lastPlace->getLocation();
                
                visitorType='H';
                totalTimeSpentAway+=travelTime;
            }

}



void SchoolAgedChildSchedule::generateSchedule() {
    for(int day=0; day< 7 ; day++){

        state = ATHOME;
        
        //Kids should Always be home at the start of the day (Curfew at 23:00 (138))
        Building *lastPlace = home;
        //**std::cout<<"Starting New Day "<<day<<std::endl;
        // Update trueTime to reflect a full day has passed
        trueTime+=dayTime;
        dayTime=0;
        int *lastPlaceLoc =lastPlace->getLocation();
        travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                               lastPlaceLoc[1],
                                               homeLoc[0],
                                               homeLoc[1],
                                               1);
        
        visitorType='H';
        // If it a weekday and child Attends School
        if(day < 5 && goToSchool) {            
            generateWeekDaySchedule(lastPlace, lastPlaceLoc);
        } else
            generateWeekEndSchedule(lastPlace, lastPlaceLoc, day);
        
        totalTimeSpentAtHome+=144-dayTime;
        totalTimeSpentAway+= 144-dayTime;
        if(totalTimeSpentAtHome>=sleepTimeNeeded){
            totalTimeSpentAway=0;
        }
        dayTime=144;
    }    
    //Check for Completely Empty Schedule
    if(currentSchedule->peekNextLocation()==NULL){
        //**std::cout<<"Didn't go Anywhere :("<<std::endl;
        currentSchedule->addTimeSlot(TimeSlot(home->getID(), 1008, 'H'));
    }
}

int SchoolAgedChildSchedule::calculateTravelTime(int start_x, int start_y, int end_x, int end_y,  int transportRate ){
    if(std::abs(start_x-end_x)>std::abs(start_y-end_y)){
        return std::ceil(std::abs(start_x-end_x)/(double)transportRate);
    }else{
        return std::ceil(std::abs(start_y-end_y)/(double)transportRate);
    }
}

/**
 * The determineTranportation Type function, determines the type of transportat-
 * ion to be used based on the probabilities of transportation modes, along with
 * distance to be covered and the time within which the distance needs to be 
 * covered.
 * @param distance distance to be covered
 * @param timeLimit timiLimit within which the distance has to be covered
 * @param transportProbablities the probabilities of various transport modes.
 * @param transportRadiusLimits the Limits within which the transport modes operate
 * @param transportRates the Rate/Cost for each transport Mode
 * @return 
 */
char SchoolAgedChildSchedule::determineTransportationType(int distance, int timeLimit,
        double *transportProbablities, int *transportRadiusLimits, 
        int *transportRates) {
    double publicTransportProb = transportProbablities[0];
    double privateTransportProb = transportProbablities[1];
    double walkingTransportProb = transportProbablities[2];
    double tmp_walkingTransportProb = walkingTransportProb;
    
    // Given the case that there is some distance that is to be covered.
    if(distance != -1){
        // If the distance to be covered is greater than radius of operation of
        // public transport systems, we use a privateTransport such as car.
        if(distance>transportRadiusLimits[0]){
            // Public Transport Distance Limit (Have to Use Private Transport)
            return 'C';
        }else{
            // If public transport is indeed viable, but before that we check
            // if it is too far to walk.
            if(distance>transportRadiusLimits[2]){
                // If it is indeed too far to walk, we make use of both walking
                // and public transport.
                publicTransportProb = walkingTransportProb + publicTransportProb;
                // the reason we assign waslkingTransportProb to 0 is because
                // while choosing between the transport medium to be picked,
                walkingTransportProb=0;
            }
            // The reason why we do not return 'W' if distance < transportRadiusLimit[2]
            // is because the timeLimit also has to be considered.
        }
    }
    // We consider the timeLimit also while choosing the TransportationType.
    if(timeLimit != -1){
        // We check if we can use publicTransport and make it within the time
        // limit, if we can't we use Private Transport.
        if(distance*transportRates[0] > timeLimit){
            return 'C';
        }else{
            // Public Transport is Viable
            // We check if the distance can be covered within the time Limit by
            // walking, else we use public Transport which also involves walking
            if(distance*transportRates[2] > timeLimit){
                publicTransportProb = tmp_walkingTransportProb + publicTransportProb;
                walkingTransportProb=0;
            }
        }
    }
    // Create a distribution to choose Transportation Type
    std::discrete_distribution<int> distribution{publicTransportProb, privateTransportProb,walkingTransportProb};
    
    int transportType = distribution(generator);
    switch(transportType){
        case 0:
            // Public Transport
            return 'T';
            break;
        case 1:
            // Private Transport
            return 'C';
            break;
        case 2:
            // Walking
            return 'W';
            break;
        default:
            return '\0';
            break;
    }
} // End of determineTransportationType


/**
 * The getTransportRate method receives the chosen transportType as input and 
 * returns the corresponding rate for the transportType.
 * @param transportType The chosen transportation Type to reach the dest.
 * @param transportRates The rate assigned in the config file for each transportation mode.
 *                       It is value of 1 for public and pvt transportation and value of 2 for walking.
 * @return 
 */
int SchoolAgedChildSchedule::getTransportRate(char transportType, 
        int *transportRates) {
    switch(transportType){
        // T stands for publicTransport.
        case 'T':
            return transportRates[0]; // Rate of 1 is returned.
            break;
        // C stands for Car emphasizing pvt transport.    
        case 'C':
            return transportRates[1]; // Rate of 1 is returned.
            break;
        // W Stands for Walking
        case 'W':
            return transportRates[2]; // Rate of 2 is returned.
            break;
        default:
            return 1;
    }
} // end of getTransportRate


/**
 * The addMoveTo function calculates the totalTime to reach the destination
 * from the source and also addsTimeSlot periodically with Nearest Transportatio
 * -n Hub from the current location. The current loc start location is iterativ
 * -ely incremented/ decremented until the destination is reached. Until then,
 * at every Transportation Hub in-between, a TimeSlot is added. The timeUnit bet
 * -ween every consecutive Transport Hub that is encountered is 1 timeUnit. 
 * At the end of the function, the totalTimeTaken to reach the destination from 
 * source is returned.
 * @param s The current Schedule
 * @param start The source/current loc
 * @param end The destination
 * @param visitorType Describes what the purpose is
 * @param endTime Summation of trueTime + dayTime (i.e. the HAPLOS time )
 * @param transportType The type of transportation that is to be used
 * @param transportRate The rate of the transportation used
 * @param transportID The transportID which for now is assigned -1.
 * @return 
 */
int SchoolAgedChildSchedule::addMoveTo(Schedule *s, Building *start, Building *end, char visitorType, int endTime, char transportType,
                                 int transportRate, int transportID){
    int *startLocation = start->getLocation();
    int *endLocation = end->getLocation();
    // Travel Time between the Start and EndLocation.
    int totalTravelTime = calculateTravelTime(startLocation[0], startLocation[1], endLocation[0], endLocation[1],transportRate);
    int travelTime = 1;
    
    // We add a time slot to the schedule with startLoc Building ID, endTime
    // and VisitorType.
    s->addTimeSlot(TimeSlot(start->getID(), endTime, visitorType));
    
    // While the start and end location are not the same
    while(startLocation[0] != endLocation[0] and startLocation[1] != endLocation[1]){
        // Increment the travelTime with trueTime + dayTime i.e. endTime
        // The increment is because it's the time to reach the transportation 
        // which is nearest to the start Location/ recent TransportHub.
        // Consider it like a bus moving through several stops before reaching
        // it's destination. At every strop we add a TimeSlot. The time taken
        // between each consequtive stop is 1 time unit.
        endTime+=travelTime;
        // We add a schedule with the Transportation hub's building ID
        // and updated endTime.
        s->addTimeSlot(TimeSlot(densityData->at(startLocation[0]).at(startLocation[1]).getTransportHub()->getID(), endTime, transportType));
        
        // According the StartLocation with respect to the endLocation, we
        // increment or decrement a value. We do this until both the StartLoc
        // and endLoc are the same.
        if(startLocation[0]< endLocation[0]){
            startLocation[0] += 1;
        }else{
            if(startLocation[0]>endLocation[0]){
                startLocation[0]-= 1;
            }
        }
        
        if(startLocation[1]< endLocation[1]){
            startLocation[1] += 1;
        }else{
            if(startLocation[1]>endLocation[1]){
                startLocation[1]-= 1;
            }
        }
    }
    
    return totalTravelTime;
    
} // End of addMoveTo function.

Building* SchoolAgedChildSchedule::findAvaliableBuilding(int x, int y, 
        char typeOfVisitor, int radius, int startTime, int endTime,
        int numberOfVisitors, int transportRate){
    int maxX=densityData->size()-1;
    int maxY=densityData[0].size()-1;
    int r=0; //Current Radius
    int travelTime = 0;
    //Adjusted Radius range for X,Y Values (Never go beyond or below Grid)
    int adjusted_actual_radMinY=y;
    int adjusted_actual_radMaxY=y;
    int adjusted_actual_radMinX=x;
    int adjusted_actual_radMaxX=x;
    //Actual Radius Range Values (Can go Beyond Grid)
    int actual_radMinY=y;
    int actual_radMaxY=y;
    int actual_radMinX=x;
    int actual_radMaxX=x;
    ////**std::cout<<x<<","<<y<<std::endl;
    Building* b=densityData->at(x).at(y).hasAvaliableBuilding(typeOfVisitor, startTime, endTime, numberOfVisitors);
    
    if(b!=NULL){
        // Found
        return b;
    }else{
        // if(typeOfVisitor=='E'){
        //     std::cout<<"Starting Locaiton: "<<x<<" "<<y<<std::endl;
        // }
        while(actual_radMaxY<maxY||actual_radMaxX<maxX||actual_radMinY>0||actual_radMinX>0) {
            // if(typeOfVisitor=='E'){
            // std::cout<<"----Next---"<<std::endl;
            // }
            r++;
            //Update Radius ranges
            adjusted_actual_radMinY=(y-r>0)?y-r:0;
            adjusted_actual_radMaxY=(y+r<maxY)?y+r:maxY;
            adjusted_actual_radMinX=(x-r>0)?x-r:0;
            adjusted_actual_radMaxX=(x+r<maxX)?x+r:maxX;
            actual_radMinY=y-r;
            actual_radMaxY=y+r;
            actual_radMinX=x-r;
            actual_radMaxX=x+r;
            
            for(int j=adjusted_actual_radMinY; j<adjusted_actual_radMaxY; j++){
                //Check Bottom
                if(actual_radMaxX<=maxX){
                    //Actual is Still in Grid and has not gone off Edge
                    b=densityData->at(actual_radMaxX).at(j).hasAvaliableBuilding(typeOfVisitor, startTime, endTime,numberOfVisitors);
                    // if(typeOfVisitor=='E'){
                    //  std::cout<<"Bottom: "<<actual_radMaxX<<" "<<j<<std::endl;
                    // }
                    if(b!=NULL){
                        // Found
                        return b;
                    }
                }else{
                    //Gone Off Bottom Edge of Grid
                    ////**std::cout<<"-Off Bottom Side: "<<actual_radMaxX<<std::endl;
                }
                // Check Top
                if(actual_radMinX>=0){
                    //Actual is Still in Grid and has not gone off Edge
                    
                    b=densityData->at(actual_radMinX).at(j).hasAvaliableBuilding(typeOfVisitor, startTime, endTime,numberOfVisitors);
                    // if(typeOfVisitor=='E'){
                    //     std::cout<<"Top: "<<actual_radMinX<<" "<<j<<std::endl;
                    // }
                    if(b!=NULL){
                        // Found
                        return b;
                    }
                }else{
                    //Gone Off Top Edge of Grid
                    ////**std::cout<<"-Off Top Side "<<actual_radMinX<<std::endl;
                }
            }
            
            for(int i=adjusted_actual_radMinX; i<=adjusted_actual_radMaxX;i++){
                //Right
                if(actual_radMaxY<=maxY){
                    //Actual is Still in Grid and has not gone off Edge
                    
                    b=densityData->at(i).at(actual_radMaxY).hasAvaliableBuilding(typeOfVisitor, startTime, endTime,numberOfVisitors);
                    // if(typeOfVisitor=='E'){
                    //    std::cout<<"Right: "<<i<<" "<<actual_radMaxY<<std::endl;
                    //}
                    if(b!=NULL){
                        // Found
                        return b;
                    }
                }else{
                    //Gone Off Right Side of Grid
                    // std::cout<<"-Off Right Side "<<actual_radMaxY<<std::endl;
                }
                //Left
                if(actual_radMinY>=0){
                    //Actual is Still in Grid and has not gone off Edge
                    
                    b=densityData->at(i).at(actual_radMinY).hasAvaliableBuilding(typeOfVisitor, startTime, endTime, numberOfVisitors);
                    // if(typeOfVisitor=='E'){
                    //     std::cout<<"Left: "<<i<<" "<<actual_radMinY<<std::endl;
                    // }
                    if(b!=NULL){
                        // Found
                        return b;
                    }
                }else{
                    //Gone Off Left Side of Grid
                    //::cout<<"-Off Left Side "<<actual_radMinY<<std::endl;
                }
            }
        }
        ////**std::cout<<actual_radMinX<<"-"<<actual_radMaxX<<" "<<actual_radMinY<<"-"<<actual_radMaxY<<std::endl;
        
        travelTime = calculateTravelTime(x, y, x+r, y+r,  transportRate);
        //Check for Radius Limit
        if( (radius>=0 && r+1 >radius) || startTime+travelTime>endTime){
            ////**std::cout<<"Hit Radius Limit"<<std::endl;
            return NULL;
        }
    }
    return NULL;
}
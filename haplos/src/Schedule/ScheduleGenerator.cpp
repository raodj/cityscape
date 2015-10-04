//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
//
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include "ScheduleGenerator.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <utility>


ScheduleGenerator::ScheduleGenerator(std::vector< std::vector < Location > > *d, std::unordered_map<int, Building*> *a,
                                     std::default_random_engine g, bool p){
    this->progressDisplay = p;
    this->densityData = d;
    this->allBuildings = a;
    this->generator = g;
    
}


void ScheduleGenerator::generateSchedules(Population &pop, int *radiusLimit, double *transportProbablities,
                                          int *transportRadiusLimits,int *transportRate ) {
    //Creating Schedules for Families
    float oldRatio=0;
    //**std::cout << "Generating Schedules for Population" << std::endl;
    if(progressDisplay){
        printf("Percentage Complete: %3d%%", 0 );
    }
    fflush(stdout);
    for ( int i =0; i<pop.getNumberOfFamilies(); i++ ) {
        Family *currentFamily = pop.getFamily(i);
        assignJobSchoolLocations(currentFamily);
        //Process Child Care Adult First
        Person *childcareAdult = currentFamily->getChildCareAdult();
        
        generatePersonSchedule(currentFamily, childcareAdult, radiusLimit, transportProbablities, transportRadiusLimits, transportRate);
        
        for(int p=0; p<currentFamily->getNumberOfPeople(); p++){
            Person *p1 = currentFamily->getPerson(p);
            
            if(p1->getID()!=childcareAdult->getID()){
                generatePersonSchedule(currentFamily, p1, radiusLimit, transportProbablities, transportRadiusLimits, transportRate);
            }
            
        } //End of Person Loop
        
        
        //Calculate Percent Complete
        if(progressDisplay){
            float ratio = i/(float)pop.getNumberOfFamilies();
            if ( 100*(ratio-oldRatio) > 1 ) {
                //Update Percent Complete Only if there is a Change
                printf("\r");
                printf("Percentage Complete: %3d%%", (int)(ratio*100) );
                oldRatio=ratio;
                fflush(stdout);
            }
        }
    } //End of Family Loop
    //Print out 100% Complete
    if(progressDisplay){
        printf("\r");
        printf("Percentage Complete: %3d%%", 100 );
        fflush(stdout);
    }
    //**std::cout << std::endl <<"Population Successfully Assigned Locations" << std::endl;
    
}




//Private Methods

void ScheduleGenerator::generatePersonSchedule(Family *currentFamily, Person *p1, int *radiusLimit,
                                               double *transportProbablities,int *transportRadiusLimits,
                                               int *transportRates){
    Schedule *currentSchedule = p1->getSchedule();
    int childModification = -1;
    bool youngChildModification=false;
    if(currentFamily->getChildCareAdult() == p1){
        //Non-Working Adult Assigned  or First Employeed Adult Modify Schedules to get Children from Schools/Daycares as needed
        if(currentFamily->getHasYoungChild()>0){
            youngChildModification=true;
        }
        if(currentFamily->getHasYoungSchoolChild()){
            childModification=1;
            
        }else{
            if(currentFamily->getHasSchoolChild()){
                childModification=0;
            }
        }
        
        
    }
    
    switch(currentSchedule->getScheduleType()){
        case 0:
            ////**std::cout<<"Generating Young Child Schedule"<<std::endl;
            generateYoungChildSchedule(p1, currentFamily, radiusLimit[0]);
            break;
        case 1:
            ////**std::cout<<"Generating Young School Aged Child Schedule"<<std::endl;
            generateYoungSchoolAgedChildSchedule(p1, currentFamily, radiusLimit[1]);
            break;
        case 2:
            ////**std::cout<<"Generating School Aged Child Schedule"<<std::endl;
            if(p1->getAge()<16){
                generateSchoolAgedChildSchedule(p1, currentFamily, radiusLimit[2], transportProbablities, transportRadiusLimits, transportRates);
            }else{
                generateSchoolAgedChildSchedule(p1, currentFamily, radiusLimit[3], transportProbablities, transportRadiusLimits, transportRates);
            }
            break;
        case 3:
            ////**std::cout<<"Generating Employeed Adult Schedule"<<std::endl;
            generateEmployeedAdultSchedule(p1, currentFamily, childModification, youngChildModification, radiusLimit[4], transportProbablities, transportRadiusLimits, transportRates);
            break;
        case 4:
            ////**std::cout<<"Generating UnEmployeed Schedule"<<std::endl;
            generateUnemployeedAdultSchedule(p1, currentFamily, (childModification>-1?true:false), radiusLimit[5], transportProbablities, transportRadiusLimits, transportRates);
            break;
        default:
            //**std::cout<<"ERROR: Unknown Schedule Type"<<std::endl;
            break;
    }
    
    
}

void ScheduleGenerator::generateYoungChildSchedule(Person* p, Family *f, int radiusLimit){
    //Yougn Child (Follow Adult always or day care)
    Schedule *currentSchedule = p->getSchedule();
    Schedule *childCareAdultSchedule = f->getChildCareAdult()->getSchedule();
    Daycare  *daycareLocaiton = f->getDaycare();
    //**std::cout<<"---------------------- Young Child: "<<p->getID()<<" -------------------------"<<std::endl;
    //**std::cout<<childCareAdultSchedule->getScheduleType()<<std::endl;
    //Set Schedule Based on Child Care Adult
    int i=0;
    bool atDaycare=false;
    while(childCareAdultSchedule->getLocationAt(i)!=NULL){
        TimeSlot *slot =childCareAdultSchedule->getLocationAt(i);
        TimeSlot *nextSlot = NULL;
        int advance =1;
        while(childCareAdultSchedule->getLocationAt(i+advance)!= NULL && (childCareAdultSchedule->getLocationAt(i+advance)->getVisitorType() == 'T' ||childCareAdultSchedule->getLocationAt(i+advance)->getVisitorType() == 'C' || childCareAdultSchedule->getLocationAt(i+advance)->getVisitorType() == 'W')){
            advance++;
        }
        nextSlot=childCareAdultSchedule->getLocationAt(i+advance);
        
        if(childCareAdultSchedule->getScheduleType()!=4){
            //Employeed Adult Responsible for Child Care
            if(slot->getLocation()==daycareLocaiton->getID() && slot->getVisitorType()== 'V' && nextSlot != NULL && nextSlot->getVisitorType()=='E'){
                //**std::cout<<"\t At Daycare or Parent is at Work: "<<slot->getEndTime()<<std::endl;
                //**std::cout<<"\t\tNot at Daycare Already"<<std::endl;
                atDaycare=true;
            }else{
                if(slot->getLocation() == daycareLocaiton->getID() && slot->getVisitorType()== 'V' && atDaycare){
                    //**std::cout<<"\t\tGetting Picked Up from Daycare"<<std::endl;
                    currentSchedule->addTimeSlot(TimeSlot(daycareLocaiton->getID(), slot->getEndTime(), 'D'));
                    atDaycare=false;
                }else{
                    //**std::cout<<"Other Time Slot" <<slot->getEndTime()<<std::endl;
                    if(!atDaycare){
                        //**std::cout<<"Not currently attending Daycare"<<std::endl;
                        currentSchedule->addTimeSlot(TimeSlot(slot->getLocation(), slot->getEndTime(), slot->getVisitorType()));
                    }
                }
            }
        }else{
            //Unemployeed Adult Responsible for Child Care
            //**std::cout<<"Not currently attending Daycare"<<std::endl;
            currentSchedule->addTimeSlot(TimeSlot(slot->getLocation(), slot->getEndTime(), slot->getVisitorType()));
        }
        i++;
    }
}

void ScheduleGenerator::generateYoungSchoolAgedChildSchedule(Person *p, Family *f, int radiusLimit){
    //Older School Aged Child (On own after school)
    Schedule *currentSchedule = p->getSchedule();
    Schedule *childCareAdultSchedule = f->getChildCareAdult()->getSchedule();
    School *attendingSchool= static_cast<School* >(allBuildings->at(currentSchedule->getJobLocation()));
    attendingSchool->assignStudentToSchool(0);
    int schoolStartTime = attendingSchool->getSchoolStartTime();
    int schoolEndTime = attendingSchool->getSchoolEndTime();
    //**std::cout<<"---------------------- Young School: "<<p->getID()<<" ----------------------"<<std::endl;
    //**std::cout<<"School Location: "<<attendingSchool->getID()<<std::endl;
    //**std::cout<<"School Time: " <<schoolStartTime <<" - "<<schoolEndTime<<std::endl;
    //Get School start and End times for reference
    int i=0;
    int day = 0;
    bool atSchool=false;
    TimeSlot *previousSlot = NULL;
    TimeSlot *nextSlot = NULL;
    while(childCareAdultSchedule->getLocationAt(i)!=NULL){
        nextSlot = childCareAdultSchedule->getLocationAt(i+1);
        TimeSlot *slot =childCareAdultSchedule->getLocationAt(i);
        
        int startTimeForSlot = 0;
        char visitorTypeForPrevious = 'H';
        if(previousSlot!=NULL){
            startTimeForSlot = previousSlot->getEndTime();
            visitorTypeForPrevious= previousSlot->getVisitorType();
        }
        
        if(slot->getLocation()==currentSchedule->getJobLocation()){
            //At School Location
            if(atSchool && slot->getEndTime()>=schoolEndTime){
                //Already at School and School Time is Over
                //**std::cout<<"\t\tLeaving School"<<std::endl;
                //**std::cout<<"\t\t\tS "<<currentSchedule->getJobLocation()<<": "<<slot->getEndTime()<<std::endl;
                
                if(slot->getEndTime()>schoolEndTime){
                    //Add After School Visiting Slot
                    //**std::cout<<"\t\t\tVisited School Afterwards"<<std::endl;
                    ////**std::cout<<nextSlot->getLocation()<<std::endl;
                    ////**std::cout<<currentSchedule->getJobLocation()<<std::endl;
                    if(nextSlot != NULL && nextSlot->getLocation() != currentSchedule->getJobLocation()){
                        currentSchedule->addTimeSlot(TimeSlot(currentSchedule->getJobLocation(),
                                                              slot->getEndTime(),
                                                              'V'));
                    }
                }
                //Advance school times to next possible times
                if(day<4){
                    schoolStartTime+=144;
                    schoolEndTime+=144;
                }else{
                    schoolStartTime = 99999;
                    schoolEndTime = 99999;
                }
                day++;
                //**std::cout<<"\t\tNext School Times: "<<schoolStartTime<<" - "<<schoolEndTime<<std::endl;
                atSchool=false;
            }else{
                //Not Already At School or School Time is not Over
                if(slot->getEndTime() >= schoolStartTime  && !atSchool){
                    //School Starting
                    //**std::cout<<"\t\tAt to School"<<std::endl;
                    int schoolSlotStartTime =schoolEndTime;
                    if(slot->getEndTime()!=schoolStartTime){
                        //Visited School Before Going to School
                        //**std::cout<<"\t\tVisited School before school started."<<std::endl;
                        if(slot->getEndTime()>schoolStartTime){
                            schoolSlotStartTime=slot->getEndTime();
                            
                        }else{
                            if(previousSlot->getLocation()== slot->getLocation()){
                                //Was already visiting this location prior remove previous time slot
                                //**std::cout<<"\t\t\t Previous Time Slot due to Duplicate"<<std::endl;
                                currentSchedule->removeTimeSlot(-1);
                            }
                            currentSchedule->addTimeSlot(TimeSlot(slot->getLocation(),
                                                                  schoolStartTime,
                                                                  'V'));
                        }
                    }
                    currentSchedule->addTimeSlot(TimeSlot(currentSchedule->getJobLocation(),
                                                          schoolSlotStartTime,
                                                          'S'));
                    atSchool=true;
                    if(slot->getEndTime()>=schoolEndTime){
                        //Already at School and School Time is Over
                        //**std::cout<<"\t\tLeaving School"<<std::endl;
                        //**std::cout<<"\t\t\tS "<<currentSchedule->getJobLocation()<<": "<<slot->getEndTime()<<std::endl;
                        
                        if(slot->getEndTime()>schoolEndTime){
                            //Add After School Visiting Slot
                            currentSchedule->addTimeSlot(TimeSlot(currentSchedule->getJobLocation(),
                                                                  slot->getEndTime(),
                                                                  'V'));
                        }
                        //Advance school times to next possible times
                        if(day<4){
                            schoolStartTime+=144;
                            schoolEndTime+=144;
                        }else{
                            schoolStartTime = 99999;
                            schoolEndTime = 99999;
                        }
                        day++;
                        //**std::cout<<"\t\tNext School Times: "<<schoolStartTime<<" - "<<schoolEndTime<<std::endl;
                        atSchool=false;
                    }
                }else{
                    if(!atSchool){
                        //Not At School just copy the time slot
                        //**std::cout<<"\t\tNot At School (Not Correct Time)"<<std::endl;
                        //**std::cout<<"\t\t\t"<<slot->getVisitorType()<<" "<<slot->getLocation()<<": "<<slot->getEndTime()<<std::endl;
                        currentSchedule->addTimeSlot(TimeSlot(slot->getLocation(), slot->getEndTime(), slot->getVisitorType()));
                    }
                    
                }
            }
            
        }else{
            if(!atSchool){
                //**std::cout<<"\t\tNot At School (Not At Location)"<<std::endl;
                //**std::cout<<"\t\t\t"<<slot->getVisitorType()<<" "<<slot->getLocation()<<": "<<slot->getEndTime()<<std::endl;
                currentSchedule->addTimeSlot(TimeSlot(slot->getLocation(), slot->getEndTime(), slot->getVisitorType()));
            }
        }
        previousSlot=slot;
        i++;
    }
    
}

void ScheduleGenerator::generateSchoolAgedChildSchedule(Person *p, Family *f, int radiusLimit, double *transportProbablities,
                                                        int *transportRadiusLimits, int *transportRates){
    //Older School Aged Child (On own after school)
    Schedule *currentSchedule = p->getSchedule();
    Building *home =f->getHome();
    
    //Set Probablities for where they can go
    double workProb=0;
    double outProb=0.2;
    double homeProb=0.8;
    
    ////**std::cout<<"\tGenerating Schedule"<<std::endl;
    //School Aged Child (On own after school)
    std::discrete_distribution<int> distribution{homeProb,workProb,outProb};
    
    School *attendingSchool= static_cast<School* >(allBuildings->at(currentSchedule->getJobLocation()));
    attendingSchool->assignStudentToSchool(0);
    int *schoolLoc = attendingSchool->getLocation();
    int *homeLoc = home->getLocation();
    
    int dayTime=0;  //Time for current day (Midnight = 0, 11:59=144
    int trueTime=0; //Actual HAPLOS Time (Monday at Midnight = 0, till Sunday at 11:59 = 1008)
    int totalTimeSpentAtHome=0; //Total Time spent at home for the given day
    int maxTimeAway = 96;
    int totalTimeSpentAway = 0;
    int crewfew = 132;
    int visitorType = 'H';
    int sleepTimeNeeded = 48;
    char transportType = 'T';
    int transportRate = 1;
    
    //Get School start and End times for reference
    int schoolStart = dayTime+attendingSchool->getSchoolStartTime();
    int schoolEnd = dayTime+attendingSchool->getSchoolEndTime();
    //Add One Hour for Sleep for Previous Day
    int timeUpOnMonday = schoolStart;
    
    //**std::cout<<"---------------------- School Child: "<<p->getID()<<" ----------------------"<<std::endl;
    //**std::cout<<"School Location: "<<attendingSchool->getID()<<" "<<schoolStart<<"-"<<schoolEnd<<std::endl;
    int travelTime = 0;
    int travelTimeToHome = 0;
    int travelTimeToSchool = 0;
    //Create Schedule
    for(int day=0; day< 7 ; day++){
        //Kids should Always be home at the start of the day (Curfew at 23:00 (138))
        Building *lastPlace = home;
        //**std::cout<<"Starting New Day "<<day<<std::endl;
        //Update trueTime to reflect a full day has passed
        trueTime+=dayTime;
        dayTime=0;
        int *lastPlaceLoc =lastPlace->getLocation();
        travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                               lastPlaceLoc[1],
                                               homeLoc[0],
                                               homeLoc[1],
                                               1);
        
        visitorType='H';
        if(day<5){
            //**std::cout<<"\t"<<"School Day"<<std::endl;
            //School Day
            //Add Sleeping Time if Needed
            travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                                     lastPlaceLoc[1],
                                                     schoolLoc[0],
                                                     schoolLoc[1],
                                                     1);
            if(totalTimeSpentAtHome<sleepTimeNeeded){
                //**std::cout<<"\t"<<"Adding Addtional Sleep Time"<<std::endl;
                int timeNeeded = sleepTimeNeeded-totalTimeSpentAtHome;
                if(dayTime+timeNeeded<schoolStart-1){
                    //Progress Time to Earliest Possible Activty time
                    dayTime+=timeNeeded;
                    totalTimeSpentAway=0;
                }else{
                    dayTime=schoolStart-1;
                    totalTimeSpentAway+=schoolStart-1-dayTime;
                }
            }
            //Add Predicted Time Spent at School to Away Time
            totalTimeSpentAway+= (schoolEnd - schoolStart)+1;
            
            //**std::cout<<"\tCheck Before School Activty"<<std::endl;
            if (totalTimeSpentAtHome>=sleepTimeNeeded && dayTime<schoolStart-1 &&
                totalTimeSpentAway<maxTimeAway){
                //Activity before School
                //**std::cout<<"\t\tCan go out Still"<<std::endl;
                
                switch(distribution(generator)){
                    case 0:
                        //Home
                        travelTimeToSchool = calculateTravelTime(homeLoc[0],
                                                                 homeLoc[1],
                                                                 schoolLoc[0],
                                                                 schoolLoc[1],
                                                                 1);
                        if(lastPlace != home) {
                            
                            transportType = determineTransportationType(travelTimeToHome,
                                                                        schoolStart-1-travelTimeToSchool-dayTime,
                                                                        transportProbablities,
                                                                        transportRadiusLimits,
                                                                        transportRates);
                            transportRate = getTransportRate(transportType, transportRates);
                            
                            travelTime= addMoveTo(currentSchedule,
                                                  lastPlace,
                                                  home,
                                                  visitorType,
                                                  trueTime+dayTime,
                                                  transportType,
                                                  transportRate,
                                                  -1);
                            lastPlace=home;
                            lastPlaceLoc =lastPlace->getLocation();
                            
                            visitorType='H';
                        }
                        dayTime = schoolStart-travelTimeToSchool-1;
                        break;
                    case 1:
                        //Job (Should not Happen)
                        ////**std::cout<<"ERROR: KID GOT A JOB"<<std::endl;
                        break;
                    case 2:
                        //Out
                        //**std::cout<< "\t\tFind Place To Go Out (Before)" <<std::endl;
                        int timeSpentAway = (trueTime+dayTime+schoolStart)-(trueTime+dayTime);
                        Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                                                      home->getLocation()[1],
                                                                      'H',
                                                                      (radiusLimit<timeSpentAway-1? radiusLimit : timeSpentAway-1),
                                                                      trueTime+dayTime,
                                                                      trueTime+dayTime+schoolStart,
                                                                      1,
                                                                      transportRate);
                        if(lastPlace_tmp==NULL){
                            //No avaliable place to go so just return back to home
                            //**std::cout<<"\t\tBefore School: No Places found to go out :("<<std::endl;
                            //**std::cout<<"\t\t\tTime Frame: "<<trueTime+dayTime<<"-"<<trueTime+dayTime+schoolStart<<std::endl;
                            if(lastPlace != home) {
                                travelTimeToSchool = calculateTravelTime(homeLoc[0],
                                                                         homeLoc[1],
                                                                         schoolLoc[0],
                                                                         schoolLoc[1],
                                                                         1);
                                transportType = determineTransportationType(travelTimeToHome,
                                                                            schoolStart-1-travelTimeToSchool-dayTime,
                                                                            transportProbablities,
                                                                            transportRadiusLimits,
                                                                            transportRates);
                                transportRate = getTransportRate(transportType, transportRates);
                                
                                travelTime = addMoveTo(currentSchedule,
                                                       lastPlace,
                                                       home,
                                                       visitorType,
                                                       trueTime+dayTime,
                                                       transportType,
                                                       transportRate,
                                                       -1);
                                lastPlace=home;
                                lastPlaceLoc =lastPlace->getLocation();
                                
                                visitorType='H';
                            }
                            dayTime = schoolStart-travelTimeToSchool-1;
                        }else{
                            if(lastPlace != lastPlace_tmp) {
                                int *lastPlaceLoc_tmp =lastPlace_tmp->getLocation();
                                travelTimeToSchool = calculateTravelTime(lastPlaceLoc_tmp[0],
                                                                         lastPlaceLoc_tmp[1],
                                                                         schoolLoc[0],
                                                                         schoolLoc[1],
                                                                         1);
                                int travelTimeToNext = calculateTravelTime(lastPlaceLoc[0],
                                                                           lastPlaceLoc[1],
                                                                           lastPlaceLoc_tmp[0],
                                                                           lastPlaceLoc_tmp[1],
                                                                           1);
                                transportType = determineTransportationType(travelTimeToNext,
                                                                            schoolStart-1-travelTimeToSchool-dayTime,
                                                                            transportProbablities,
                                                                            transportRadiusLimits,
                                                                            transportRates);
                                transportRate = getTransportRate(transportType, transportRates);
                                travelTime = addMoveTo(currentSchedule,
                                                       lastPlace,
                                                       lastPlace_tmp,
                                                       visitorType,
                                                       trueTime+dayTime,
                                                       transportType,
                                                       transportRate,
                                                       -1);
                                lastPlace=lastPlace_tmp;
                                lastPlaceLoc =lastPlace->getLocation();
                            }
                            dayTime+=(trueTime+dayTime+schoolStart)-(trueTime+dayTime);
                            totalTimeSpentAway+=(trueTime+dayTime+schoolStart)-(trueTime+dayTime);
                            totalTimeSpentAtHome=0;
                            lastPlace->addVisitorTimeSlot(trueTime+dayTime,trueTime+dayTime+schoolStart);
                            visitorType='V';
                            
                        }
                        totalTimeSpentAtHome=0;
                        break;
                }
            }
            
            //**std::cout<<"\tGoing to school"<<std::endl;
            //Advance time to Start of School
            totalTimeSpentAtHome=0;
            travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                                     lastPlaceLoc[1],
                                                     schoolLoc[0],
                                                     schoolLoc[1],
                                                     1);
            transportType = determineTransportationType(travelTimeToSchool,
                                                        schoolStart-1-dayTime,
                                                        transportProbablities,
                                                        transportRadiusLimits,
                                                        transportRates);
            
            transportRate = getTransportRate(transportType, transportRates);
            dayTime=schoolStart;
            
            //Go To School (No Need to update totalAway Time due to its already been taken into Account)
            travelTime = addMoveTo(currentSchedule,
                                   lastPlace,
                                   attendingSchool,
                                   visitorType,
                                   trueTime+dayTime,
                                   transportType,
                                   transportRate,
                                   -1);
            dayTime+=schoolEnd-schoolStart;
            visitorType='S';
            totalTimeSpentAtHome=0;
            lastPlace=attendingSchool;
            lastPlaceLoc =lastPlace->getLocation();
            //**std::cout<<"\tAfter School Activty"<<std::endl;
            int travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
            
            while(dayTime<crewfew-travelTimeToHome-1 && totalTimeSpentAway<maxTimeAway){
                travelTime = 0;
                //**std::cout<<"\t\tCan go out Still"<<std::endl;
                //Activity After School
                switch(distribution(generator)){
                    case 0:{
                        //Home
                        //update time to reflect staying at home once home
                        //**std::cout<<"\t\t\t Staying Home"<<std::endl;
                        totalTimeSpentAtHome+= crewfew - dayTime;
                        if(lastPlace != home){
                            //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                            transportType = determineTransportationType(travelTimeToHome,
                                                                        crewfew - dayTime,
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
                        
                        break;}
                    case 1:
                        //Job (Should not Happen)
                        break;
                    case 2:
                        //Out
                        //**std::cout<< "\t\t\tFind Place To Go Out (After)" <<std::endl;
                        int timeSpentOut=1;
                        if(crewfew-dayTime-travelTimeToHome>1){
                            timeSpentOut=(int)rand() % (crewfew-dayTime-travelTimeToHome)+1;
                        }
                        
                        //**std::cout<<"\t\t\tTime To Spend Out: "<<timeSpentOut<<std::endl;
                        //(radiusLimit<timeSpentOut-1?radiusLimit:timeSpentOut-1)
                        Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                                                      home->getLocation()[1],
                                                                      'V',
                                                                      radiusLimit,
                                                                      trueTime+dayTime,
                                                                      trueTime+dayTime+timeSpentOut,
                                                                      1,
                                                                      transportRate);
                        if(lastPlace_tmp==NULL){
                            //No avaliable place to go so just return back to home
                            //**std::cout<<"\t\t After School: No Places found to go out :("<<std::endl;
                            //**std::cout<<"\t\t\tTime Frame: "<<trueTime+dayTime<<"-"<<trueTime+dayTime+timeSpentOut<<std::endl;
                            if(lastPlace_tmp != home){
                                //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                                transportType = determineTransportationType(travelTimeToHome,
                                                                            crewfew - dayTime,
                                                                            transportProbablities,
                                                                            transportRadiusLimits,
                                                                            transportRates);
                                
                                transportRate = getTransportRate(transportType, transportRates);
                                
                                travelTime = addMoveTo(currentSchedule,
                                                       lastPlace,
                                                       home,
                                                       visitorType,
                                                       trueTime+dayTime,
                                                       transportType,
                                                       transportRate,
                                                       -1);
                                visitorType='H';
                                lastPlace=home;
                                lastPlaceLoc =lastPlace->getLocation();
                            }
                            
                            //update time to reflect staying at home once home
                            totalTimeSpentAtHome+= crewfew - dayTime;
                            if(totalTimeSpentAtHome >=sleepTimeNeeded){
                                totalTimeSpentAway=0;
                            }
                            dayTime =crewfew;
                        }else{
                            //Found a Place to go Visit
                            if(lastPlace != lastPlace_tmp || visitorType!='V') {
                                //**std::cout<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                                int *lastPlaceLoc_tmp =lastPlace_tmp->getLocation();
                                //**std::cout<<"Last Place Tmp: "<<lastPlace_tmp->getID()<<std::endl;
                                int travelTimeToNext = calculateTravelTime(lastPlaceLoc[0],
                                                                           lastPlaceLoc[1],
                                                                           lastPlaceLoc_tmp[0],
                                                                           lastPlaceLoc_tmp[1],
                                                                           1);
                                travelTimeToHome = calculateTravelTime(lastPlaceLoc_tmp[0],
                                                                       lastPlaceLoc_tmp[1],
                                                                       homeLoc[0],
                                                                       homeLoc[1],
                                                                       1);
                                transportType = determineTransportationType(travelTimeToNext,
                                                                            crewfew - dayTime-travelTimeToHome,
                                                                            transportProbablities,
                                                                            transportRadiusLimits,
                                                                            transportRates);
                                
                                
                                transportRate = getTransportRate(transportType, transportRates);
                                travelTime=addMoveTo(currentSchedule,
                                                     lastPlace,
                                                     lastPlace_tmp,
                                                     visitorType,
                                                     trueTime+dayTime,
                                                     transportType,
                                                     transportRate,
                                                     -1);
                                
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
                        }
                        
                        break;
                }
                
                travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
            } //End of While
            if(lastPlace != home){
                // //**std::cout<<"\tCrewfew: "<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                transportType = determineTransportationType(travelTimeToHome,
                                                            travelTimeToHome,
                                                            transportProbablities,
                                                            transportRadiusLimits,
                                                            transportRates);
                
                
                transportRate = getTransportRate(transportType, transportRates);
                
                travelTime = addMoveTo(currentSchedule,
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
                totalTimeSpentAway+= travelTime;
                
            }
        }
        else{
            //Weekend
            //**std::cout<<"\tNon-School Day"<<std::endl;
            //Add Sleeping Time if Needed
            if(totalTimeSpentAtHome<sleepTimeNeeded){
                int timeNeeded = sleepTimeNeeded-totalTimeSpentAtHome;
                //Progress Time to Earliest Possible Activty time
                dayTime+=timeNeeded;
            }
            int sleepIfOutAllNight = 144-crewfew + timeUpOnMonday;
            if(sleepIfOutAllNight<sleepTimeNeeded && day==6){
                crewfew = crewfew-(sleepTimeNeeded-sleepIfOutAllNight);
            }
            
            while(dayTime<crewfew-travelTimeToHome-1){
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
                    
                }else{
                    switch(distribution(generator)){
                        case 0:{
                            //Home
                            //update time to reflect staying at home once home
                            int travelTime=0;
                            if (lastPlace!=home){
                                //Update Schedule to Reflect Change of Location
                                //**std::cout<<"\tStay Home Case: "<<visitorType<<" "<<lastPlace->getID()<<" "<<trueTime+dayTime<<std::endl;
                                transportType = determineTransportationType(travelTimeToHome,
                                                                            crewfew-dayTime-1,
                                                                            transportProbablities,
                                                                            transportRadiusLimits,
                                                                            transportRates);
                                transportRate = getTransportRate(transportType, transportRates);
                                
                                travelTime= addMoveTo(currentSchedule,
                                                      lastPlace,
                                                      home,
                                                      visitorType,
                                                      trueTime+dayTime,
                                                      transportType,
                                                      transportRate,
                                                      -1);
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
                            break;
                        }
                        case 1:
                            //Job (Should not Happen)
                            break;
                        case 2:
                            //Out
                            ////**std::cout<< "\t\t\tFind Place To Go Out (After)" <<std::endl;
                            timeSpentOut=0;
                            //**std::cout<<"\t\t\tcrewfew-dayTime-travelTimeToHome: "<<crewfew-dayTime-travelTimeToHome<<std::endl;
                            if(crewfew-dayTime-travelTimeToHome-radiusLimit-1>2){
                                timeSpentOut=(int)rand() % (crewfew-dayTime-travelTimeToHome-radiusLimit-1)+2;
                            }
                            
                            if(timeSpentOut+totalTimeSpentAway+travelTimeToHome>maxTimeAway-radiusLimit-1){
                                //**std::cout<<"\t\t\tOver Max Time Away: "<<maxTimeAway-totalTimeSpentAway-travelTimeToHome-radiusLimit-1<<std::endl;
                                timeSpentOut= maxTimeAway-totalTimeSpentAway-travelTimeToHome-radiusLimit-1;
                            }
                            int radiusLimitTemp = radiusLimit;
                            if(radiusLimit> timeSpentOut){
                                radiusLimit = timeSpentOut-1;
                            }
                            
                            //**std::cout<<"\t\t\tMax Time Out: "<<maxTimeAway<<std::endl;
                            Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                                                          home->getLocation()[1],
                                                                          'V',
                                                                          radiusLimit,
                                                                          trueTime+dayTime,
                                                                          trueTime+dayTime+timeSpentOut,
                                                                          1,
                                                                          transportRate);
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
                                    transportType = determineTransportationType(travelTimeToHome,
                                                                                crewfew-dayTime-1,
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
                            }else{
                                //**std::cout<<"\tVisitor Case: "<<visitorType<<" "<<lastPlace->getID()<<"->"<<lastPlace_tmp->getID()<<" "<<trueTime+dayTime<<std::endl;
                                if(lastPlace != lastPlace_tmp || visitorType!= 'V') {
                                    int *lastPlaceLoc_tmp =lastPlace_tmp->getLocation();
                                    int travelTimeToNext = calculateTravelTime(lastPlaceLoc[0],
                                                                               lastPlaceLoc[1],
                                                                               lastPlaceLoc_tmp[0],
                                                                               lastPlaceLoc_tmp[1],
                                                                               1);
                                    travelTimeToHome = calculateTravelTime(lastPlaceLoc_tmp[0],
                                                                           lastPlaceLoc_tmp[1],
                                                                           homeLoc[0],
                                                                           homeLoc[1],
                                                                           1);
                                    
                                    transportType = determineTransportationType(travelTimeToNext,
                                                                                crewfew-dayTime-1-travelTimeToHome,
                                                                                transportProbablities,
                                                                                transportRadiusLimits,
                                                                                transportRates);
                                    transportRate = getTransportRate(transportType, transportRates);
                                    travelTime=addMoveTo(currentSchedule,
                                                         lastPlace,
                                                         lastPlace_tmp,
                                                         visitorType,
                                                         trueTime+dayTime,
                                                         transportType,
                                                         transportRate,
                                                         -1);
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
                                
                            }
                            
                            break;
                    }
                }
                travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                       lastPlaceLoc[1],
                                                       homeLoc[0],
                                                       homeLoc[1],
                                                       1);
                
            } //End of While
            
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
        totalTimeSpentAtHome+=144-dayTime;
        totalTimeSpentAway+= 144-dayTime;
        if(totalTimeSpentAtHome>=sleepTimeNeeded){
            totalTimeSpentAway=0;
        }
        dayTime=144;
    }
    
}


void ScheduleGenerator::generateEmployeedAdultSchedule(Person *p, Family *f, int schoolChildModification, bool youngChildModification, int radiusLimit, double *transportProbablities,
                                                       int *transportRadiusLimits, int *transportRates){
    //Working Adult
    Schedule *currentSchedule = p->getSchedule();
    Building *home =f->getHome();
    Building *lastPlace = home;
    Building *jobLocation=allBuildings->at(currentSchedule->getJobLocation());
    Building *dayCareLocation = f->getDaycare();
    
    double workProb=0.90;
    double outProb=0.05;
    double homeProb=0.05;
    
    std::discrete_distribution<int> distribution{homeProb,workProb,outProb};
    std::discrete_distribution<int> no_work_left_distribution{0.5,0,0.5};
    
    //Generate a schedule for everyone else
    int dayTime=0;  //Time for current day (Midnight = 0, 11:59=144
    int trueTime=0; //Actual HAPLOS Time (Sunday at Midnight = 0, till Saturday at 11:59 = 1008)
    int totalTimeSpentAtHome=0; //Total Time spent at home for the given day
    int totalTimeSpentAway = 0; //Once it hits 16 hours (96) force go home
    int dailyTotalTimeAtJob = 0; //Total time spent at job in a given day (0-14 Hours, 0-84 HAPLOS Time)
    int totalTimeSpentAtJob = ((int)rand() % 300)+60; //Total time Spent at work during the week 10-60 hours (60-360)
    bool kidsAtSchool = false;
    bool kidsAtDaycare = false;
    int transportRate =1;
    char transportType = 'T';
    int *homeLoc = home->getLocation();
    int *jobLoc = jobLocation->getLocation();
    int crewfew = (schoolChildModification>=0 || youngChildModification) ? 120 : 144;
    bool goneToWork = false;
    //If Need to Take care of Child give a little more buffer in case kid needs to be taken to school
    int maxTimeAway = ((youngChildModification || schoolChildModification)? 90 : 96);
    int fullDay=144;
    ////**std::cout<<totalTimeSpentAtJob<<std::endl;
    std::vector <std::pair<int, School*> >schoolTimes;
    //**std::cout<<"---------------------- Employeed: "<<p->getID()<<" "<<schoolChildModification<<" "<<(youngChildModification?"True":"False")<<" ----------------------"<<std::endl;
    //**std::cout<<"\tTest: "<<currentSchedule->getJobLocation()<<std::endl;
    //**std::cout<<"\tJob Location: "<<jobLocation->getID()<<" ("<<currentSchedule->getJobLocation()<<")"<<std::endl;
    //Get Children School Start Times If Needed
    if(schoolChildModification>-1){
        schoolTimes=getSchoolTimes(f);
    }else{
        kidsAtSchool=true;
    }
    char visitorType='H';
    int travelTime = 0;
    for(int day=0; day< 7 ; day++){
        int currentPlaceInSchoolTimes = 0;
        std::pair<int, School*> nextSchoolTime=std::make_pair(9999,nullptr);
        int travelTimeToSchool = 0;
        int travelTimeSchoolFromJob = 0;
        int travelTimeToDayCare =0;
        int travelTimeToJob =0;
        transportRate =1;
        
        //Reset School Time to first one if a week day.
        //Reset to first School Time if Needed
        if(schoolChildModification>-1 && schoolTimes.size()>0 && day<5){
            nextSchoolTime=schoolTimes.at(0);
            nextSchoolTime.first+=trueTime;
        }
        
        dailyTotalTimeAtJob=0;
        //Determine how long needs to be spent at Job today
        if(totalTimeSpentAtJob>0 ){
            if(totalTimeSpentAtJob<84){
                //Less than 14 hours needed
                //**std::cout<<"totalTimeSpentAtJob: "<<totalTimeSpentAtJob<<std::endl;
                dailyTotalTimeAtJob=((int)rand() % totalTimeSpentAtJob)+1;
            }else{
                //More than 14 hours needed
                dailyTotalTimeAtJob=((int)rand() % 84)+1;
            }
        }
        ////**std::cout<<"\t"<<dailyTotalTimeAtJob<<std::endl;
        //**std::cout<<"---------Starting New Day "<<day<<"-----------"<<std::endl;
        //**std::cout<<"-- Daily Job Total: "<<dailyTotalTimeAtJob<<std::endl;
        int *lastPlaceLoc =lastPlace->getLocation();
        
        int travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                   lastPlaceLoc[1],
                                                   homeLoc[0],
                                                   homeLoc[1],
                                                   1);
        //**std::cout<<"-- Daytime: "<<dayTime<<std::endl;
        //**std::cout<<"-- Crewfew: "<<crewfew<<std::endl;
        //**std::cout<<"-- travelTimeToHome: "<<travelTimeToHome<<std::endl;
        while(dayTime<crewfew-travelTimeToHome-1){
            int maxTimeOut=0;
            travelTime=0;
            if(crewfew != fullDay && ( crewfew-dayTime-travelTimeToHome )< (maxTimeAway-totalTimeSpentAway-travelTimeToHome)){
                maxTimeOut=crewfew-dayTime-travelTimeToHome;
            }else{
                maxTimeOut=maxTimeAway-totalTimeSpentAway-travelTimeToHome;
            }
            if(maxTimeOut<0){
                maxTimeOut=0;
            }
            //**std::cout<<"\ttotalTimeSpendAway"<<totalTimeSpentAway<<std::endl;
            //**std::cout<<"\tmaxTimeOut: "<<maxTimeOut<<std::endl;
            //**std::cout<<"\tmaxTimeAway: "<<maxTimeAway<<std::endl;
            
            travelTimeToSchool = 0;
            travelTimeSchoolFromJob = 0;
            travelTimeToDayCare = 0;
            travelTimeToJob =0;
            if(nextSchoolTime.second != nullptr){
                int *schoolLocation = nextSchoolTime.second->getLocation();
                travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                                         lastPlaceLoc[1],
                                                         schoolLocation[0],
                                                         schoolLocation[1],
                                                         1);
                
                travelTimeSchoolFromJob = calculateTravelTime(jobLoc[0],
                                                              jobLoc[1],
                                                              schoolLocation[0],
                                                              schoolLocation[1],
                                                              1);
            }
            
            travelTimeToJob = calculateTravelTime(lastPlaceLoc[0],
                                                  lastPlaceLoc[1],
                                                  jobLoc[0],
                                                  jobLoc[1],
                                                  1);
            
            if(dayCareLocation != NULL){
                int *dayLoc = dayCareLocation->getLocation();
                
                travelTimeToDayCare = calculateTravelTime(lastPlaceLoc[0],
                                                          lastPlaceLoc[1],
                                                          dayLoc[0],
                                                          dayLoc[1],
                                                          1);
            }
            //Repeat until day is over
            
            //Check if child needs to go to School
            //**std::cout<<"--- Next School Time: "<<nextSchoolTime.first+1-trueTime-travelTimeToSchool<<std::endl;
            //**std::cout<<"--- Day Time: "<<dayTime<<std::endl;
            if(schoolChildModification>-1 && dayTime>=nextSchoolTime.first-1-trueTime-travelTimeToSchool && nextSchoolTime.second!=nullptr && day<5){
                //Get From School
                //**std::cout<<"\tGet Kid from School"<<std::endl;
                if(lastPlace != nextSchoolTime.second || visitorType!='V'){
                    transportType = determineTransportationType(travelTimeToSchool,
                                                                travelTimeToSchool,
                                                                transportProbablities,
                                                                transportRadiusLimits,
                                                                transportRates);
                    transportRate = getTransportRate(transportType, transportRates);
                    
                    travelTime=addMoveTo(currentSchedule,
                                         lastPlace,
                                         nextSchoolTime.second,
                                         visitorType,
                                         trueTime+dayTime,
                                         transportType,
                                         transportRate,
                                         -1);
                }
                lastPlace=nextSchoolTime.second;
                if(currentPlaceInSchoolTimes<schoolTimes.size()-1){
                    currentPlaceInSchoolTimes++;
                    nextSchoolTime=schoolTimes.at(currentPlaceInSchoolTimes);
                    nextSchoolTime.first+=day*fullDay;
                    if(currentPlaceInSchoolTimes==schoolTimes.size()/2){
                        //**std::cout<<"**Kids At School"<<std::endl;
                        kidsAtSchool=true;
                    }else{
                        //**std::cout<<"**Kids At Not School"<<std::endl;
                        kidsAtSchool=false;
                    }
                    
                }else{
                    //**std::cout<<"**Kids At Not School"<<std::endl;
                    nextSchoolTime=schoolTimes.at(0);
                    currentPlaceInSchoolTimes=0;
                    nextSchoolTime.first+=(day+1)*fullDay;
                    kidsAtSchool=false;
                }
                
                int *schoolLocation = nextSchoolTime.second->getLocation();
                transportRate =1;
                travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                                         lastPlaceLoc[1],
                                                         schoolLocation[0],
                                                         schoolLocation[1],
                                                         transportRate);
                
                //**std::cout<<"\t\tSchool ID: "<<lastPlace->getID()<<std::endl;
                //**std::cout<<"\t\tNext School Time: "<<nextSchoolTime.first<<std::endl;
                //**std::cout<<"\t\tDay Time: "<<dayTime<<std::endl;
                totalTimeSpentAtHome=0;
                dayTime+=travelTime+1;
                totalTimeSpentAway+=travelTime+1;
                visitorType='V';
            }else{
                //Child does not need to go to school right now
                if(totalTimeSpentAway+travelTimeToHome+1>=maxTimeOut){
                    //Been out too Long Need to Get Sleep
                    //**std::cout<<"\tForce Home ("<<trueTime+dayTime<<"): "<<dayTime<<std::endl;
                    //**std::cout<<"\t\tTime Need to be Home: "<<48-totalTimeSpentAtHome<<std::endl;
                    //**std::cout<<"\t\tTime Already Spent at Home: "<< totalTimeSpentAtHome<<std::endl;
                    
                    
                    transportType = determineTransportationType(travelTimeToHome,
                                                                travelTimeToHome,
                                                                transportProbablities,
                                                                transportRadiusLimits,
                                                                transportRates);
                    
                    transportRate = getTransportRate(transportType, transportRates);
                    
                    if(youngChildModification && kidsAtDaycare){
                        //Pick Up Young Child at Daycare
                        //**std::cout<<"\t\tPicking Up Kid At Daycare: " <<dailyTotalTimeAtJob<<std::endl;
                        if(lastPlace != f->getDaycare() || visitorType!= 'V'){
                            travelTime= addMoveTo(currentSchedule,
                                                  lastPlace,
                                                  f->getDaycare(),
                                                  visitorType,
                                                  trueTime+dayTime,
                                                  transportType,
                                                  transportRate,
                                                  -1);
                        }
                        lastPlace=f->getDaycare();
                        dayTime+=travelTime+1;
                        totalTimeSpentAway+=travelTime+1;
                        
                        visitorType='V';
                        kidsAtDaycare=false;
                        
                    }
                    
                    int timeNeedToBeHome= 48-totalTimeSpentAtHome;
                    if(lastPlace != home){
                        travelTime= addMoveTo(currentSchedule,
                                              lastPlace,
                                              home,
                                              visitorType,
                                              trueTime+dayTime,
                                              transportType,
                                              transportRate,
                                              -1);
                    }
                    totalTimeSpentAway+=travelTime;
                    dayTime+=travelTime;
                    
                    visitorType='H';
                    lastPlace=home;
                    if(nextSchoolTime.first-1>dayTime+timeNeedToBeHome+trueTime){
                        totalTimeSpentAtHome+= timeNeedToBeHome;
                        dayTime +=timeNeedToBeHome;
                        totalTimeSpentAway=0;
                    }else{
                        //Need to take a break to take kid to school
                        //**std::cout<<"\t\tNeed to Break to take Child to Schoool"<<std::endl;
                        //**std::cout<<"\t\t\tNext School Time: "<<nextSchoolTime.first<<std::endl;
                        //**std::cout<<"\t\t\tTrue Time: "<<trueTime<<std::endl;
                        //**std::cout<<"\t\t\tDay Time: "<<dayTime<<std::endl;
                        //**std::cout<<"\t\t\tCal: "<<(nextSchoolTime.first-trueTime)-dayTime;
                        if((nextSchoolTime.first-trueTime)-dayTime>0){
                            totalTimeSpentAtHome+=(nextSchoolTime.first-trueTime)-dayTime;
                        }else{
                            totalTimeSpentAtHome+=1;
                        }
                        //**std::cout<<"\t\t\tTotal Time Spent At Home: "<<totalTimeSpentAtHome<<std::endl;
                        if (nextSchoolTime.first-trueTime-travelTimeToSchool>dayTime){
                            dayTime=nextSchoolTime.first-trueTime-travelTimeToSchool;
                        }
                    }
                    visitorType='H';
                    
                }else{
                    
                    int timeLeft=(youngChildModification ? crewfew-dayTime-travelTimeToHome-travelTimeToDayCare+1 :
                                  crewfew-dayTime-travelTimeToHome);
                    //Check for if need to go to Job
                    //**std::cout<<"\t\t***Daily Total Time At Job: "<<dailyTotalTimeAtJob<<" Time Left: "<<timeLeft<<"***"<<std::endl;
                    //**std::cout<<kidsAtSchool<<" "<<schoolChildModification<<std::endl;
                    int timeSpentAtJob = 0;
                    //Force Job if time is running out and kids are at school or
                    if((dailyTotalTimeAtJob>(timeLeft/2) && timeLeft>2 && kidsAtSchool) ||
                       (dailyTotalTimeAtJob>0 && (schoolChildModification>-1 || youngChildModification) && kidsAtSchool)){
                        if(youngChildModification && !kidsAtDaycare){
                            //**std::cout<<"\t\tDropping Kid At Daycare: " <<trueTime+dayTime<<std::endl;
                            travelTime=0;
                            transportType = determineTransportationType(travelTimeToDayCare,
                                                                        timeLeft,
                                                                        transportProbablities,
                                                                        transportRadiusLimits,
                                                                        transportRates);
                            transportRate = getTransportRate(transportType, transportRates);
                            
                            if(f->getDaycare()!= lastPlace || (f->getDaycare()==lastPlace && visitorType!='V')){
                                travelTime= addMoveTo(currentSchedule,
                                                      lastPlace,
                                                      f->getDaycare(),
                                                      visitorType,
                                                      trueTime+dayTime,
                                                      transportType,
                                                      transportRate,
                                                      -1);
                                //Drop Young Child off at Daycare first
                                lastPlace=f->getDaycare();
                                
                                
                            }
                            dayTime+=travelTime+1;
                            totalTimeSpentAway+=travelTime+1;
                            visitorType='V';
                            kidsAtDaycare=true;
                            timeLeft=crewfew-dayTime-travelTimeToHome;
                        }
                        
                        //Force to go to Job until no longer able to or no time is left
                        //**std::cout<<"\tForce Job("<<trueTime+dayTime<<"): "<<dayTime<<std::endl;
                        
                        //Assume Max Time Can Be Spent
                        timeSpentAtJob += (dailyTotalTimeAtJob>timeLeft)? timeLeft : dailyTotalTimeAtJob;
                        //**std::cout<<"\t\tTime Spent at Job: " <<timeSpentAtJob<<std::endl;
                        if(dayTime+timeSpentAtJob+travelTimeToJob>nextSchoolTime.first-1-trueTime-travelTimeSchoolFromJob){
                            //**std::cout<<"\t\t\tNext School Time: " <<nextSchoolTime.first<<std::endl;
                            //**std::cout<<"\t\t\tTravel Time to Job: "<<travelTimeToJob<<std::endl;
                            //**std::cout<<"\t\t\tTravel Time To School: " <<travelTimeSchoolFromJob<<std::endl;
                            //**std::cout<<"\t\t\ttrueTime: "<<trueTime<<std::endl;
                            //**std::cout<<"\t\t\tdayTime: "<<dayTime<<std::endl;
                            //Need to take break to pick up child from school
                            timeSpentAtJob =(nextSchoolTime.first-1-trueTime-travelTimeToJob-travelTimeSchoolFromJob)-dayTime;
                            
                        }
                        //Check that Force Home isn't going to kick in
                        if(timeSpentAtJob+totalTimeSpentAway+travelTimeToHome+travelTimeToJob>maxTimeAway){
                            //**std::cout<<"\t\t\tMax Time Away: "<<maxTimeAway<<std::endl;
                            //**std::cout<<"\t\t\tTotal Time Spent Away: "<<totalTimeSpentAway<<std::endl;
                            //**std::cout<<"Travel Time to Home: "<<travelTimeToHome<<std::endl;
                            timeSpentAtJob=maxTimeAway-totalTimeSpentAway-travelTimeToHome;
                        }
                        
                        
                        if(timeSpentAtJob <= 0){
                            timeSpentAtJob = 1;
                        }
                        
                        //**std::cout<<"\t\tTime Spent at Job: " <<timeSpentAtJob<<std::endl;
                        //**std::cout<<"\t\tTime Already Spent at Home: "<< totalTimeSpentAtHome<<std::endl;
                        if(lastPlace != jobLocation || visitorType!='E'){
                            transportType = determineTransportationType(travelTimeToJob,
                                                                        travelTimeToJob,
                                                                        transportProbablities,
                                                                        transportRadiusLimits,
                                                                        transportRates);
                            transportRate = getTransportRate(transportType, transportRates);
                            
                            
                            travelTime= addMoveTo(currentSchedule,
                                                  lastPlace,
                                                  jobLocation,
                                                  visitorType,
                                                  trueTime+dayTime,
                                                  transportType,
                                                  transportRate,
                                                  -1);
                            lastPlace=jobLocation;
                        }
                        visitorType='E';
                        totalTimeSpentAtHome=0;
                        totalTimeSpentAway+=timeSpentAtJob+travelTime;
                        totalTimeSpentAtJob-=timeSpentAtJob;
                        dailyTotalTimeAtJob-=timeSpentAtJob;
                        dayTime+=timeSpentAtJob+travelTime;
                        
                        
                    }else{
                        int activity=0; //Activity Type
                        
                        //Determine which Distribution to use
                        if(dailyTotalTimeAtJob<=0 ||
                           !kidsAtSchool ||
                           (youngChildModification && totalTimeSpentAway<maxTimeAway-2) ){
                            //No work Left to do at job
                            activity = no_work_left_distribution(generator);
                        }else{
                            //Work still to do at job
                            activity = distribution(generator);
                        }
                        
                        int timeSpentAtLocation=0;
                        //Pick Activity
                        switch(activity){
                            case 0:{
                                //Home
                                //**std::cout<<"\tHome ("<<trueTime+dayTime<<")"<<std::endl;
                                //update time to reflect staying at home once home
                                
                                //**std::cout<<"\t\tdayTime: "<<dayTime<<std::endl;
                                
                                if(youngChildModification && kidsAtDaycare){
                                    //Pick Young Child off at Daycare
                                    //**std::cout<<"\t\tPicking Up Kid At Daycare: " <<dailyTotalTimeAtJob<<std::endl;
                                    
                                    if(lastPlace!= f->getDaycare() || visitorType!= 'V'){
                                        transportType = determineTransportationType(travelTimeToDayCare,
                                                                                    maxTimeAway-totalTimeSpentAway-travelTimeToHome-1,
                                                                                    transportProbablities,
                                                                                    transportRadiusLimits,
                                                                                    transportRates);
                                        transportRate = getTransportRate(transportType, transportRates);
                                        
                                        travelTime= addMoveTo(currentSchedule,
                                                              lastPlace,
                                                              f->getDaycare(),
                                                              visitorType,
                                                              trueTime+dayTime, transportType, transportRate, -1);
                                    }
                                    
                                    lastPlace=f->getDaycare();
                                    dayTime+=travelTime+1;
                                    totalTimeSpentAway+=travelTime+1;
                                    visitorType='V';
                                    kidsAtDaycare=false;
                                    
                                }
                                //**std::cout<<"\t\tMax Time: "<<maxTimeOut<<std::endl;
                                timeSpentAtLocation=1;
                                if(schoolChildModification>-1){
                                    //**std::cout<<"\t\t\tNext School Times: "<<nextSchoolTime.first<<std::endl;
                                    //**std::cout<<"\t\t\tTrue Time: "<<trueTime<<std::endl;
                                    //**std::cout<<"\t\t\tDay Time: "<<dayTime<<std::endl;
                                    if((nextSchoolTime.first-1-trueTime)-dayTime>1){
                                        timeSpentAtLocation=(int)rand() % ((nextSchoolTime.first-1-trueTime)-dayTime)+1;
                                    }
                                }else{
                                    timeSpentAtLocation=((int)rand() % (maxTimeOut>timeLeft && timeLeft>0? timeLeft : maxTimeOut))+1;
                                    
                                }
                                totalTimeSpentAtHome+= timeSpentAtLocation;
                                //**std::cout<<"\t\tTime Spent Here: "<<timeSpentAtLocation<<std::endl;
                                
                                
                                //**std::cout<<"\t\tCurrent Time Spent out Total: "<<totalTimeSpentAway<<std::endl;
                                if (lastPlace!=home){
                                    transportType = determineTransportationType(travelTimeToHome,
                                                                                maxTimeAway-totalTimeSpentAway-travelTimeToHome-1,
                                                                                transportProbablities,
                                                                                transportRadiusLimits,
                                                                                transportRates);
                                    transportRate = getTransportRate(transportType, transportRates);
                                    
                                    //Update Schedule to Reflect Change of Location
                                    travelTime= addMoveTo(currentSchedule,
                                                          lastPlace,
                                                          home,
                                                          visitorType,
                                                          trueTime+dayTime,
                                                          transportType,
                                                          transportRate,
                                                          -1);
                                    lastPlace=home;
                                }
                                totalTimeSpentAway+=travelTime;
                                //Check if time Spent Home is 8 hours or longer
                                if(totalTimeSpentAtHome>=48){
                                    totalTimeSpentAway=0;
                                }else{
                                    totalTimeSpentAway+= timeSpentAtLocation;
                                }
                                visitorType='H';
                                break;
                            }
                            case 1:{
                                //Job
                                //**std::cout<<"\tGoing to Job "<<visitorType<<std::endl;
                                if(youngChildModification && !kidsAtDaycare){
                                    if(f->getDaycare()!= lastPlace || visitorType != 'V'){
                                        //**std::cout<<"\t\tDropping Kid At Daycare: " <<dailyTotalTimeAtJob<<std::endl;
                                        //**std::cout<<"\t\t\tDayCare ID: "<<f->getDaycare()->getID()<<std::endl;
                                        transportType = determineTransportationType(travelTimeToDayCare,
                                                                                    maxTimeAway-totalTimeSpentAway-travelTimeToHome-1,
                                                                                    transportProbablities,
                                                                                    transportRadiusLimits,
                                                                                    transportRates);
                                        transportRate = getTransportRate(transportType, transportRates);
                                        
                                        travelTime= addMoveTo(currentSchedule,
                                                              lastPlace,
                                                              f->getDaycare(),
                                                              visitorType,
                                                              trueTime+dayTime,
                                                              transportType,
                                                              transportRate,
                                                              -1);
                                        //Drop Young Child off at Daycare first
                                        lastPlace=f->getDaycare();
                                        dayTime+=travelTime+1;
                                        totalTimeSpentAway+=travelTime+1;
                                    }
                                    visitorType='V';
                                    kidsAtDaycare=true;
                                }
                                int maxTimeCanBeSpentAtJob = (dailyTotalTimeAtJob>(maxTimeAway-totalTimeSpentAway-travelTimeToHome-1)? maxTimeAway-totalTimeSpentAway-travelTimeToHome-1 : dailyTotalTimeAtJob);
                                if(dayTime+maxTimeCanBeSpentAtJob+trueTime>nextSchoolTime.first-1
                                   && nextSchoolTime.second!=nullptr){
                                    //Need to take break to pick up child from school
                                    timeSpentAtLocation = (nextSchoolTime.first-1)-dayTime-trueTime;
                                    
                                }else{
                                    //**std::cout<<"\t\t Max for Time Out: "<<maxTimeCanBeSpentAtJob<<std::endl;
                                    timeSpentAtLocation = ((int)rand() % maxTimeCanBeSpentAtJob)+1;
                                    if(!youngChildModification){
                                        timeSpentAtLocation++;
                                    }
                                }
                                
                                
                                if(lastPlace != jobLocation || visitorType!='E'){
                                    transportType = determineTransportationType(travelTimeToJob,
                                                                                maxTimeAway-totalTimeSpentAway-travelTimeToHome-1,
                                                                                transportProbablities,
                                                                                transportRadiusLimits,
                                                                                transportRates);
                                    transportRate = getTransportRate(transportType, transportRates);
                                    
                                    travelTime= addMoveTo(currentSchedule,
                                                          lastPlace,
                                                          jobLocation,
                                                          visitorType,
                                                          trueTime+dayTime,
                                                          transportType,
                                                          transportRate,
                                                          -1);
                                    lastPlace=jobLocation;
                                    
                                }
                                totalTimeSpentAtHome=0;
                                totalTimeSpentAway+=timeSpentAtLocation+travelTime;
                                totalTimeSpentAtJob-=timeSpentAtLocation;
                                dailyTotalTimeAtJob-=timeSpentAtLocation;
                                goneToWork=true;
                                visitorType='E';
                                
                                break;
                            }
                            case 2:{
                                //Out
                                //**std::cout<<"\tVisitor ("<<trueTime+dayTime<<"): "<<dayTime<<std::endl;
                                //**std::cout<<"\t\tCurrent Time Spent out Total: "<<totalTimeSpentAway<<std::endl;
                                timeSpentAtLocation=1;
                                if(dayTime+(maxTimeOut)+trueTime>nextSchoolTime.first-1-travelTimeToSchool
                                   && nextSchoolTime.second!=nullptr){
                                    //Need to take break to pick up child from school
                                    timeSpentAtLocation = (nextSchoolTime.first-1)-dayTime-trueTime-travelTimeToSchool;
                                    
                                }else{
                                    //**std::cout<<"\t\t Max for Time Out: "<<maxTimeOut<<std::endl;
                                    //**std::cout<<"\t\tTime Left: "<<timeLeft<<std::endl;
                                    timeSpentAtLocation = ((int)rand() %(maxTimeOut>timeLeft && timeLeft>1? timeLeft-1 : maxTimeOut-1))+1;
                                }
                                //**std::cout<<"\t\tTime Spent at Location:"<<timeSpentAtLocation<<std::endl;
                                //Update Schedule To reflect moving of location
                                Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                                                              home->getLocation()[1], 'V', radiusLimit,
                                                                              trueTime+dayTime,
                                                                              trueTime+dayTime+timeSpentAtLocation,
                                                                              1,
                                                                              transportRate);
                                
                                if(lastPlace_tmp==NULL){
                                    //No avaliable place to go so just return back to home
                                    //**std::cout<<"\t\t\tNo Places found to go out :("<<std::endl;
                                    //update time to reflect staying at home once home
                                    
                                    if(lastPlace != home){
                                        //Wasn't at Home Needs to Go Home
                                        transportType = determineTransportationType(travelTimeToHome,
                                                                                    maxTimeAway-totalTimeSpentAway-travelTimeToHome-1,
                                                                                    transportProbablities,
                                                                                    transportRadiusLimits,
                                                                                    transportRates);
                                        transportRate = getTransportRate(transportType, transportRates);
                                        travelTime= addMoveTo(currentSchedule,
                                                              lastPlace,
                                                              home,
                                                              visitorType,
                                                              trueTime+dayTime,
                                                              transportType,
                                                              transportRate,
                                                              -1);
                                        lastPlace=home;
                                    }
                                    //Update Time Spent at Home
                                    totalTimeSpentAway+=travelTime;
                                    totalTimeSpentAtHome+=timeSpentAtLocation;
                                    //Check if time Spent Home is 8 hours or longer
                                    if(totalTimeSpentAtHome>=48){
                                        totalTimeSpentAway=0;
                                    }else{
                                        totalTimeSpentAway+=timeSpentAtLocation+travelTime;
                                    }
                                    visitorType='H';
                                    
                                }else{
                                    //Found Location
                                    //**std::cout<<"\t\t\tLocation Found"<<std::endl;
                                    if(lastPlace != lastPlace_tmp || visitorType!='V') {
                                        //**std::cout<<"\t\t\t\tMoving to New Location"<<std::endl;
                                        int *newLocation =lastPlace_tmp->getLocation();
                                        int travelTimeToNewLocation = calculateTravelTime(lastPlaceLoc[0],
                                                                                          lastPlaceLoc[1],
                                                                                          newLocation[0],
                                                                                          newLocation[1],
                                                                                          1);
                                        
                                        transportType = determineTransportationType(travelTimeToNewLocation,
                                                                                    maxTimeAway-totalTimeSpentAway-travelTimeToHome-1,
                                                                                    transportProbablities,
                                                                                    transportRadiusLimits,
                                                                                    transportRates);
                                        transportRate = getTransportRate(transportType, transportRates);
                                        
                                        travelTime= addMoveTo(currentSchedule,
                                                              lastPlace,
                                                              lastPlace_tmp,
                                                              visitorType,
                                                              trueTime+dayTime,
                                                              transportType,
                                                              transportRate,
                                                              -1);
                                        lastPlace=lastPlace_tmp;
                                    }
                                    //**std::cout<<"\t\t\t\tLocation Updated"<<std::endl;
                                    travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                                           lastPlaceLoc[1],
                                                                           homeLoc[0],
                                                                           homeLoc[1],
                                                                           transportRate);
                                    if(timeSpentAtLocation-travelTimeToHome-1>0){
                                        timeSpentAtLocation=timeSpentAtLocation-travelTimeToHome-1;
                                    }else{
                                        timeSpentAtLocation=1;
                                    }
                                    
                                    lastPlace->addVisitorTimeSlot(trueTime+dayTime,timeSpentAtLocation);
                                    totalTimeSpentAtHome=0;
                                    totalTimeSpentAway+=timeSpentAtLocation+travelTime;
                                    visitorType='V';
                                    //**std::cout<<"\t\tNew Last Place: "<<lastPlace->getID()<<std::endl;
                                    
                                    break;
                                }// End Of Null Check Else
                            } //End of Case
                        } //End of Switch Case
                        dayTime+=timeSpentAtLocation+travelTime;
                    } // End of Everything Else Check
                } //End of Job Check
            }//End of School Time If
            lastPlaceLoc =lastPlace->getLocation();
            travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                   lastPlaceLoc[1],
                                                   homeLoc[0],
                                                   homeLoc[1],
                                                   transportRate);
            travelTime=0;
            //**std::cout<<"\t\tLoop Done"<<std::endl;
        }//End of While
        travelTime=0;
        if(crewfew != fullDay){
            //**std::cout<<"\tHandeling Crewfew"<<std::endl;
            if(youngChildModification && kidsAtDaycare){
                //Pick Young Child off at Daycare
                //**std::cout<<"\t\tPicking Up Kid At Daycare: " <<dailyTotalTimeAtJob<<std::endl;
                
                if(lastPlace!=f->getDaycare() || visitorType!='V'){
                    transportType = determineTransportationType(travelTimeToDayCare,
                                                                travelTimeToDayCare,
                                                                transportProbablities,
                                                                transportRadiusLimits,
                                                                transportRates);
                    transportRate = getTransportRate(transportType, transportRates);
                    travelTime= addMoveTo(currentSchedule,
                                          lastPlace,
                                          f->getDaycare(),
                                          visitorType,
                                          trueTime+dayTime,
                                          transportType,
                                          transportRate,
                                          -1);
                    
                    travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                           lastPlaceLoc[1],
                                                           homeLoc[0],
                                                           homeLoc[1],
                                                           transportRate);
                    
                }
                dayTime+=travelTime;
                lastPlace=f->getDaycare();
                dayTime++;
                totalTimeSpentAway+=travelTime+1;
                visitorType='V';
                kidsAtDaycare=false;
                
            }
            
            if(lastPlace != home){
                //**std::cout<<"\t\tVisitor Type: "<<visitorType<<std::endl;
                transportType = determineTransportationType(travelTimeToHome,
                                                            travelTimeToHome,
                                                            transportProbablities,
                                                            transportRadiusLimits,
                                                            transportRates);
                transportRate = getTransportRate(transportType, transportRates);
                travelTime= addMoveTo(currentSchedule,
                                      lastPlace,
                                      home,
                                      visitorType,
                                      trueTime+dayTime,
                                      transportType,
                                      transportRate,
                                      -1);
                lastPlace=home;
                visitorType='H';
                
            }
            totalTimeSpentAway+=travelTime;
            dayTime+=travelTime;
            if(dayTime<fullDay+30) {
                int timeSpentAtLocation = fullDay-dayTime+30;
                //**std::cout<<"\t\tTime Spent At Location: "<<timeSpentAtLocation<<std::endl;
                totalTimeSpentAtHome+= timeSpentAtLocation;
                //**std::cout<<"\t\tTotal Time Spent At Home: "<<totalTimeSpentAtHome<<std::endl;
                dayTime+=timeSpentAtLocation;
            }
            if(totalTimeSpentAtHome<48){
                //**std::cout<<"\t\tAddtional Time Spent at Home: "<<48-totalTimeSpentAtHome<<std::endl;
                dayTime+=48-totalTimeSpentAtHome;
            }
            //**std::cout<<"goneToWork: "<<goneToWork<<std::endl;
            
            if(!goneToWork && day==2  && schoolChildModification>-1){
                //**std::cout<<"\t\tNever gone to Work yet, forcing"<<std::endl;
                //Avoid Never Going to Work Loop Problem by staying home until school starts
                dayTime = nextSchoolTime.first - travelTimeToSchool - trueTime-1;
            }
            
            totalTimeSpentAway=0;
        }
        trueTime+=dayTime;
        if(dayTime>fullDay){
            //**std::cout<<"\tDealing with Bleed Over"<<std::endl;
            //**std::cout<<"\t\tTime Before Dealing with: "<<dayTime<<std::endl;
            //Day Bleed Over to Next Day
            if(schoolTimes.size()>0){
                //Make sure bleed over doesn't go past first school time for kids
                if(dayTime-fullDay>nextSchoolTime.first-1){
                    dayTime = nextSchoolTime.first- trueTime - travelTimeToSchool-1;
                }else{
                    dayTime=dayTime-fullDay;
                }
            }else{
                dayTime=dayTime-fullDay;
            }
            
            //Avoid Double Count
            trueTime=trueTime-dayTime;
            
            //**std::cout<<"\t\tTime At End of bleed over: "<<dayTime<<std::endl;
        }else{
            //Day Didn't bleed Over
            dayTime=0;
        }
    }//For Loop
    
    
    //Check for Completely Empty Schedule
    if(currentSchedule->peekNextLocation()==NULL){
        //**std::cout<<"Didn't go Anywhere :("<<std::endl;
        currentSchedule->addTimeSlot(TimeSlot(lastPlace->getID(), trueTime+dayTime, 'H'));
    }
    
}

void ScheduleGenerator::generateUnemployeedAdultSchedule(Person *p, Family *f, bool childModification, int radiusLimit,
                                                         double *transportProbablities, int *transportRadiusLimits,
                                                         int *transportRates){
    //Non-working Adult
    //**std::cout<<"---------------------- Unemployeed: "<<p->getID()<<" "<<(childModification?"True":"False")<<" ----------------------"<<std::endl;
    
    Schedule *currentSchedule = p->getSchedule();
    Building *home =f->getHome();
    Building *lastPlace = home;
    
    double workProb=0;
    double outProb=0.5;
    double homeProb=0.5;
    
    std::discrete_distribution<int> distribution{homeProb,workProb,outProb};
    
    //Generate a schedule for everyone else
    int dayTime=0;  //Time for current day (Midnight = 0, 11:59=144
    int trueTime=0; //Actual HAPLOS Time (Sunday at Midnight = 0, till Saturday at 11:59 = 1008)
    int totalTimeSpentAtHome=0; //Total Time spent at home for the given day
    int totalTimeSpentAway = 0; //Once it hits 16 hours (96) force go home
    std::vector <std::pair<int, School*> >schoolTimes;
    char visitorType = 'H';
    int crewfew = (childModification) ? 120 : 144;
    int transportRate = 1;
    int transportType = 'T';
    
    int fullDay = 144;
    //If Need to Take care of Child give a little more buffer in case kid needs to be taken to school
    int maxTimeAway = ((childModification)? 94 : 96);
    //Check if Child Modifications Need to Be made
    if(childModification){
        schoolTimes=getSchoolTimes(f);
    }
    int travelTime;
    int *homeLoc = home->getLocation();
    int travelTimeToSchool = 0;
    int travelTimeToHome  = 0;
    //Start of Week Loop
    for(int day=0; day< 7 ; day++){
        int currentPlaceInSchoolTimes = 0;
        std::pair<int, School*> nextSchoolTime=std::make_pair(9999,nullptr);
        //**std::cout<<schoolTimes.size()<<std::endl;
        //Reset to first School Time if Needed
        
        if(childModification && schoolTimes.size()>0 && day<5){
            ////**std::cout<<"\t\tSetting Inital School Time: "<<std::endl;
            nextSchoolTime=schoolTimes.at(0);
            nextSchoolTime.first+=trueTime;
            ////**std::cout<<nextSchoolTime.first<<std::endl;
        }
        //**std::cout<<"--New Day "<<day<<std::endl;
        //**std::cout<<"\tDayTime At Start: "<<dayTime<<std::endl;
        //**std::cout<<"\tTrue Time At Start: " << trueTime<<std::endl;
        //While there is still time left in the day
        while(dayTime<crewfew){
            int *lastPlaceLoc =lastPlace->getLocation();
            travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                   lastPlaceLoc[1],
                                                   homeLoc[0],
                                                   homeLoc[1],
                                                   1);
            travelTimeToSchool = 0;
            if(nextSchoolTime.second != nullptr){
                int *schoolLocation = nextSchoolTime.second->getLocation();
                travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                                         lastPlaceLoc[1],
                                                         schoolLocation[0],
                                                         schoolLocation[1],
                                                         1);
            }
            travelTime=0;
            //**std::cout<<"Travel Time to Home: "<<travelTimeToHome<<std::endl;
            //**std::cout<<"Travel time to School: "<<travelTimeToSchool<<std::endl;
            //Check that Child doesn't need to be taken to School
            if(childModification && dayTime+travelTimeToSchool>=nextSchoolTime.first-1-trueTime&& nextSchoolTime.second!=nullptr && day<5){
                //Child Needs to be taken to School
                //**std::cout<<"Picking up Kid: "<<trueTime+dayTime<<std::endl;
                //**std::cout<<"\tPick Up Time: "<<nextSchoolTime.first-1-trueTime<<" "<<dayTime<<std::endl;
                //**std::cout<<"\tcurrentPlaceInSchoolTimes: "<<currentPlaceInSchoolTimes<<std::endl;
                //Leave Previous Location
                if(lastPlace != nextSchoolTime.second){
                    transportType = determineTransportationType(travelTimeToSchool,
                                                                travelTimeToSchool,
                                                                transportProbablities,
                                                                transportRadiusLimits,
                                                                transportRates);
                    transportRate = getTransportRate(transportType, transportRates);
                    
                    travelTime=addMoveTo(currentSchedule,
                                         lastPlace,
                                         nextSchoolTime.second,
                                         visitorType,
                                         trueTime+dayTime,
                                         transportType,
                                         transportRate,
                                         -1);
                }
                
                lastPlace=nextSchoolTime.second;
                if(currentPlaceInSchoolTimes<schoolTimes.size()-1){
                    currentPlaceInSchoolTimes++;
                    nextSchoolTime=schoolTimes.at(currentPlaceInSchoolTimes);
                    nextSchoolTime.first+=day*fullDay;
                    
                }else{
                    nextSchoolTime=schoolTimes.at(0);
                    currentPlaceInSchoolTimes=0;
                    nextSchoolTime.first+=(day+1)*fullDay;
                }
                dayTime+=travelTime;
                totalTimeSpentAway+=travelTime;
                visitorType='V';
                totalTimeSpentAtHome=0;
                dayTime++;
            }else{
                if(totalTimeSpentAway+travelTimeToHome+1>=maxTimeAway || dayTime+travelTimeToHome+1>=crewfew){
                    //**std::cout<<"\tForce Home ("<<trueTime+dayTime<<"): "<<dayTime<<std::endl;
                    //**std::cout<<"\t\tTime Need to be Home: "<<48-totalTimeSpentAtHome<<std::endl;
                    //**std::cout<<"\t\tTime Already Spent at Home: "<< totalTimeSpentAtHome<<std::endl;
                    //Been out too Long Need to Get Sleep
                    int timeNeedToBeHome= (48-totalTimeSpentAtHome>1? 48-totalTimeSpentAtHome : 1);
                    if(lastPlace != home){
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
                        dayTime+=travelTime;
                        totalTimeSpentAway+=travelTime;
                    }
                    lastPlace=home;
                    if(nextSchoolTime.first-1>dayTime+timeNeedToBeHome+trueTime){
                        totalTimeSpentAtHome+= timeNeedToBeHome;
                        dayTime +=timeNeedToBeHome;
                        totalTimeSpentAway=0;
                    }else{
                        //Need to take a break to take kid to school
                        if(nextSchoolTime.first-travelTimeToSchool-dayTime-trueTime>0){
                            totalTimeSpentAtHome+=nextSchoolTime.first-travelTimeToSchool-dayTime-trueTime;
                        }
                        if(nextSchoolTime.first-travelTimeToSchool-trueTime>dayTime){
                            dayTime=nextSchoolTime.first-travelTimeToSchool-trueTime;
                        }
                    }
                    visitorType='H';
                    
                }else{
                    int timeSpentAtLocation=0;
                    //Can go out
                    switch(distribution(generator)){
                        case 0:
                            //Home
                            //**std::cout<<"\tHome ("<<trueTime+dayTime<<")"<<std::endl;
                            //**std::cout<<"\t\tDay Time: "<<dayTime<<" Crewfew: "<<crewfew<<std::endl;
                            //**std::cout<<"\t\tSchool Time: "<<nextSchoolTime.first<<" TrueTime: "<<trueTime<<std::endl;
                            
                            //update time to reflect staying at home once home
                            if (lastPlace!=home){
                                //Update Schedule to Reflect Change of Location
                                transportType = determineTransportationType(travelTimeToHome,
                                                                            maxTimeAway-totalTimeSpentAway-1,
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
                                lastPlace=home;
                            }
                            visitorType='H';
                            dayTime+=travelTime;
                            totalTimeSpentAway+=travelTime;
                            if(childModification){
                                if((nextSchoolTime.first-trueTime)-dayTime != 0){
                                    timeSpentAtLocation=(int)rand() % ((nextSchoolTime.first-trueTime)-dayTime)+1;
                                }
                                else{
                                    timeSpentAtLocation = 0;
                                }
                            }else{
                                //**std::cout<<"\t\tDay Time: "<<dayTime<<" Crewfew: "<<crewfew<<std::endl;
                                //**std::cout<<((int)rand() % (crewfew-dayTime))<<std::endl;
                                timeSpentAtLocation=((int)rand() % (crewfew-dayTime))+1;
                                
                            }
                            totalTimeSpentAtHome+= timeSpentAtLocation;
                            //**std::cout<<"\t\tTime Spent Here: "<<timeSpentAtLocation<<std::endl;
                            
                            //Check if time Spent Home is 8 hours or longer
                            if(totalTimeSpentAtHome>=48){
                                totalTimeSpentAway=0;
                            }else{
                                totalTimeSpentAway+= timeSpentAtLocation;
                            }
                            ////**std::cout<<"\t\tCurrent Time Spent out Total: "<<totalTimeSpentAway<<std::endl;
                            break;
                        case 1:
                            //Job (Should not Happen)
                            break;
                        case 2:
                            //Out
                            //**std::cout<<"\tVisitor ("<<trueTime+dayTime<<"): "<<dayTime<<std::endl;
                            //**std::cout<<"\t\tCurrent Time Spent out Total: "<<totalTimeSpentAway<<std::endl;
                            timeSpentAtLocation=1;
                            if(dayTime+(maxTimeAway-totalTimeSpentAway)+trueTime>nextSchoolTime.first-1
                               && nextSchoolTime.second!=nullptr){
                                //Need to take break to pick up child from school
                                timeSpentAtLocation = (nextSchoolTime.first-1)-dayTime-trueTime;
                                
                            }else{
                                ////**std::cout<<"\t\t Max for Time Out: "<<96-totalTimeSpentAway<<std::endl;
                                timeSpentAtLocation = ((int)rand() % (maxTimeAway
                                                                      -totalTimeSpentAway
                                                                      -travelTimeToHome))+1;
                            }
                            
                            if(dayTime+timeSpentAtLocation+travelTimeToHome+1>crewfew){
                                timeSpentAtLocation=crewfew-dayTime-travelTimeToHome-1;
                            }
                            
                            //**std::cout<<"\t\tTime Spent at Location:"<<timeSpentAtLocation<<std::endl;
                            //Update Schedule To reflect moving of location
                            int tempRadiusLimit = timeSpentAtLocation-1;
                            Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                                                          home->getLocation()[1],
                                                                          'V',
                                                                          tempRadiusLimit,
                                                                          trueTime+dayTime,
                                                                          trueTime+dayTime+timeSpentAtLocation,
                                                                          1,
                                                                          transportRate);
                            
                            if(lastPlace_tmp==NULL){
                                //No avaliable place to go so just return back to home
                                //**std::cout<<"\t\t\tNo Places found to go out :("<<std::endl;
                                //update time to reflect staying at home once home
                                
                                if(lastPlace != home){
                                    //Wasn't at Home Needs to Go Home
                                    transportType = determineTransportationType(travelTimeToHome,
                                                                                maxTimeAway-totalTimeSpentAway-1-timeSpentAtLocation,
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
                                //Update Time Spent at Home
                                totalTimeSpentAtHome+=timeSpentAtLocation;
                                dayTime+=travelTime;
                                visitorType='H';
                                lastPlace=home;
                                //Check if time Spent Home is 8 hours or longer
                                if(totalTimeSpentAtHome>=48){
                                    totalTimeSpentAway=0;
                                }else{
                                    totalTimeSpentAway+=timeSpentAtLocation+travelTime;
                                }
                                
                            }else{
                                //Found Location
                                if(lastPlace != lastPlace_tmp || visitorType!='V') {
                                    int *lastPlace_tmp_loc = lastPlace_tmp->getLocation();
                                    int travelTimeToNewLocation = calculateTravelTime(lastPlaceLoc[0],
                                                                                      lastPlaceLoc[1],
                                                                                      lastPlace_tmp_loc[0],
                                                                                      lastPlace_tmp_loc[1],
                                                                                      1);
                                    
                                    transportType = determineTransportationType(travelTimeToNewLocation,
                                                                                maxTimeAway-totalTimeSpentAway-1-timeSpentAtLocation,
                                                                                transportProbablities,
                                                                                transportRadiusLimits,
                                                                                transportRates);
                                    transportRate = getTransportRate(transportType, transportRates);
                                    travelTime=addMoveTo(currentSchedule,
                                                         lastPlace,
                                                         lastPlace_tmp,
                                                         visitorType,
                                                         trueTime+dayTime,
                                                         transportType,
                                                         transportRate,
                                                         -1);
                                    
                                }
                                lastPlace=lastPlace_tmp;
                                travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                                       lastPlaceLoc[1],
                                                                       homeLoc[0],
                                                                       homeLoc[1],
                                                                       transportRate);
                                if(timeSpentAtLocation-travelTimeToHome-1>0){
                                    timeSpentAtLocation=timeSpentAtLocation-travelTimeToHome-1;
                                }else{
                                    timeSpentAtLocation=1;
                                }
                                
                                lastPlace->addVisitorTimeSlot(trueTime+dayTime,timeSpentAtLocation);
                                totalTimeSpentAtHome=0;
                                totalTimeSpentAway+=timeSpentAtLocation+travelTime;
                                dayTime+=travelTime;
                                visitorType='V';
                                //**std::cout<<"\t\tNew Last Place: "<<lastPlace->getID()<<std::endl;
                                
                                break;
                            } // End Of Null Check Else
                            
                    } //End of Switch
                    //Advance Clock
                    dayTime+=timeSpentAtLocation;
                    //**std::cout<<"\t\tTrue Time at End of Loop: "<<trueTime<<std::endl;
                    //**std::cout<<"\t\tTime at End of Loop: "<<dayTime<<std::endl;
                }//End of Can Do Something else
            }//End of No Child to Pick Up Else
        } //End of While Day
        //End of Day (Update True Time)
        travelTime=0;
        if(crewfew != fullDay){
            //**std::cout<<"\tHandeling Crewfew"<<std::endl;
            if(lastPlace != home){
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
                lastPlace=home;
                visitorType='H';
                
            }
            dayTime+=travelTime;
            totalTimeSpentAway+=travelTime;
            if(dayTime<fullDay+30) {
                int timeSpentAtLocation = fullDay-dayTime+30;
                //**std::cout<<"\t\tTime Spent At Location: "<<timeSpentAtLocation<<std::endl;
                totalTimeSpentAtHome+= timeSpentAtLocation;
                //**std::cout<<"\t\tTotal Time Spent At Home: "<<totalTimeSpentAtHome<<std::endl;
                dayTime+=timeSpentAtLocation;
            }
            if(totalTimeSpentAtHome<48){
                //**std::cout<<"\t\tAddtional Time Spent at Home: "<<48-totalTimeSpentAtHome<<std::endl;
                dayTime+=48-totalTimeSpentAtHome;
                totalTimeSpentAway=0;
            }
            
            
        }
        
        trueTime+=dayTime;
        if(dayTime>fullDay){
            //**std::cout<<"\tDealing with Bleed Over"<<std::endl;
            //**std::cout<<"\t\tTime Before Dealing with: "<<dayTime<<std::endl;
            //Day Bleed Over to Next Day
            if(schoolTimes.size()>0){
                //Make sure bleed over doesn't go past first school time for kids
                if(dayTime-fullDay>nextSchoolTime.first-1){
                    dayTime = nextSchoolTime.first;
                }else{
                    dayTime=dayTime-fullDay;
                }
            }else{
                dayTime=dayTime-fullDay;
            }
            
            //Avoid Double Count
            trueTime=trueTime-dayTime;
            
            //**std::cout<<"\t\tTime At End of bleed over: "<<dayTime<<std::endl;
        }else{
            //Day Didn't bleed Over
            dayTime=0;
        }
    }//For Loop
    
    
    //Check for Completely Empty Schedule
    if(currentSchedule->peekNextLocation()==NULL){
        //**std::cout<<"Didn't go Anywhere :("<<std::endl;
        currentSchedule->addTimeSlot(TimeSlot(lastPlace->getID(), trueTime+dayTime, 'H'));
    }
    
    
}

//Private Helper Methods

struct sort_pair{
    bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right){
        return left.first < right.first;
    }
    
};

char ScheduleGenerator::determineTransportationType(int distance, int timeLimit, double *transportProbablities, int *transportRadiusLimits, int *transportRates){
    double publicTransportProb = transportProbablities[0];
    double privateTransportProb = transportProbablities[1];
    double walkingTransportProb = transportProbablities[2];
    
    if(distance != -1){
        if(distance>transportRadiusLimits[0]){
            //Public Transport Distance Limit (Have to Use Private Transport)
            return 'C';
        }else{
            //Public Transport is Viable
            if(distance>transportRadiusLimits[2]){
                //Too far to walk
                publicTransportProb = walkingTransportProb + publicTransportProb;
                walkingTransportProb=0;
            }
        }
    }
    
    if(timeLimit != -1){
        if(distance*transportRates[0] > timeLimit){
            //Public Transport Time Limit (Have to Use Private Transport)
            return 'C';
        }else{
            //Public Transport is Viable
            if(distance*transportRates[2] > timeLimit){
                //Walking takes too much time
                publicTransportProb = walkingTransportProb + publicTransportProb;
                walkingTransportProb=0;
            }
        }
    }
    std::discrete_distribution<int> distribution{publicTransportProb, privateTransportProb,walkingTransportProb};
    
    int transportType = distribution(generator);
    switch(transportType){
        case 0:
            //Public Transport
            return 'T';
            break;
        case 1:
            //Private Transport
            return 'C';
            break;
        case 2:
            //Walking
            return 'W';
            break;
        default:
            return '\0';
            break;
    }
}

int ScheduleGenerator::getTransportRate (char transportType, int *transportRates){
    switch(transportType){
        case 'T':
            return transportRates[0];
            break;
        case 'C':
            return transportRates[1];
            break;
        case 'W':
            return transportRates[2];
            break;
        default:
            return 1;
    }
}

bool ScheduleGenerator::sort_schoolTimes(const std::pair<int, School*>& firstElem, const std::pair<int, School*>& secondElem) {
    return firstElem.first < secondElem.first;
    
}

std::vector <std::pair<int, School*> > ScheduleGenerator::getSchoolTimes(Family *f){
    std::vector <std::pair<int, School*> > schoolTimes;
    std::map <std::string,int > existingTimes;
    for(int fm=0; fm<f->getNumberOfPeople(); fm++){
        Person *p1 = f->getPerson(fm);
        Schedule *sc = p1->getSchedule();
        if(sc->getScheduleType()==1 || sc->getScheduleType()==2){
            School *attendingSchool= static_cast<School* >(allBuildings->at(sc->getJobLocation()));
            //**std::cout<<sc->getJobLocation()<<std::endl;
            //**std::cout<<"Type: "<<attendingSchool->getType()<<std::endl;;
            std::string key =std::to_string(attendingSchool->getSchoolStartTime())+"_"+std::to_string(attendingSchool->getID());
            if(existingTimes[key]!=1){
                existingTimes[key]=1;
                schoolTimes.push_back(std::make_pair(attendingSchool->getSchoolStartTime(), attendingSchool));
                schoolTimes.push_back(std::make_pair(attendingSchool->getSchoolEndTime(), attendingSchool));
            }
        }
    }

    std::sort(schoolTimes.begin(), schoolTimes.end(), [](const std::pair<int, School*>& firstElem, const std::pair<int, School*>& secondElem) {
        return firstElem.first < secondElem.first;
        
    });
    return schoolTimes;
}

int ScheduleGenerator::addMoveTo(Schedule *s, Building *start, Building *end, char visitorType, int endTime, char transportType,
                                 int transportRate, int transportID){
    int *startLocation = start->getLocation();
    int *endLocation = end->getLocation();
    int totalTravelTime = calculateTravelTime(startLocation[0], startLocation[1], endLocation[0], endLocation[1],transportRate);
    int travelTime = 1;
    s->addTimeSlot(TimeSlot(start->getID(), endTime, visitorType));
    
    while(startLocation[0] != endLocation[0] and startLocation[1] != endLocation[1]){
        endTime+=travelTime;
        s->addTimeSlot(TimeSlot(densityData->at(startLocation[0]).at(startLocation[1]).getTransportHub()->getID(), endTime, transportType));
        
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
    
}

int ScheduleGenerator::calculateTravelTime(int start_x, int start_y, int end_x, int end_y,  int transportRate ){
    if(std::abs(start_x-end_x)>std::abs(start_y-end_y)){
        return std::ceil(std::abs(start_x-end_x)/(double)transportRate);
    }else{
        return std::ceil(std::abs(start_y-end_y)/(double)transportRate);
    }
}



/** Checks to see if a there exist a building has a avalible slot for person in a given visitor type.
 \param[in] vistorType type of visitor (E=Employee, V=Standard Visitor, P=Patient)
 \param[in] radius radius limit (-1 means no limit).
 \param[in] startTime start time of slot (required for standard visitor)
 \param[in] endTime end time of slot (required for standard visitor
 \return pointer to building that has avaliable visitor slot
 */
Building* ScheduleGenerator::findAvaliableBuilding(int x, int y, char typeOfVisitor, int radius, int startTime, int endTime, int numberOfVisitors, int transportRate){
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
    //std::cout<<x<<","<<y<<std::endl;
    Building* b=densityData->at(x).at(y).hasAvaliableBuilding(typeOfVisitor, startTime, endTime, numberOfVisitors);
    
    if(b!=NULL){
        // Found
        return b;
    }else{
        /*if(typeOfVisitor=='E'){
         std::cout<<"Starting Locaiton: "<<x<<" "<<y<<std::endl;
         }*/
        while(actual_radMaxY<maxY||actual_radMaxX<maxX||actual_radMinY>0||actual_radMinX>0) {
            /*if(typeOfVisitor=='E'){
             std::cout<<"----Next---"<<std::endl;
             }*/
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
                    /*if(typeOfVisitor=='E'){
                     std::cout<<"Bottom: "<<actual_radMaxX<<" "<<j<<std::endl;
                     }*/
                    if(b!=NULL){
                        // Found
                        return b;
                    }
                }else{
                    //Gone Off Bottom Edge of Grid
                    //std::cout<<"-Off Bottom Side: "<<actual_radMaxX<<std::endl;
                }
                // Check Top
                if(actual_radMinX>=0){
                    //Actual is Still in Grid and has not gone off Edge
                    
                    b=densityData->at(actual_radMinX).at(j).hasAvaliableBuilding(typeOfVisitor, startTime, endTime,numberOfVisitors);
                    /*if(typeOfVisitor=='E'){
                     std::cout<<"Top: "<<actual_radMinX<<" "<<j<<std::endl;
                     }*/
                    if(b!=NULL){
                        // Found
                        return b;
                    }
                }else{
                    //Gone Off Top Edge of Grid
                    //std::cout<<"-Off Top Side "<<actual_radMinX<<std::endl;
                }
            }
            
            for(int i=adjusted_actual_radMinX; i<=adjusted_actual_radMaxX;i++){
                //Right
                if(actual_radMaxY<=maxY){
                    //Actual is Still in Grid and has not gone off Edge
                    
                    b=densityData->at(i).at(actual_radMaxY).hasAvaliableBuilding(typeOfVisitor, startTime, endTime,numberOfVisitors);
                    /*if(typeOfVisitor=='E'){
                     std::cout<<"Right: "<<i<<" "<<actual_radMaxY<<std::endl;
                     }*/
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
                    /*if(typeOfVisitor=='E'){
                     std::cout<<"Left: "<<i<<" "<<actual_radMinY<<std::endl;
                     }*/
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
        //std::cout<<actual_radMinX<<"-"<<actual_radMaxX<<" "<<actual_radMinY<<"-"<<actual_radMaxY<<std::endl;
        
        travelTime = calculateTravelTime(x, y, x+r, y+r,  transportRate);
        //Check for Radius Limit
        if( (radius>=0 && r+1 >radius) || startTime+travelTime>endTime){
            //std::cout<<"Hit Radius Limit"<<std::endl;
            return NULL;
        }
    }
    return NULL;
}

void ScheduleGenerator::assignJobSchoolLocations(Family *f){
    Building *home =f->getHome();
    for(int p=0; p<f->getNumberOfPeople(); p++){
        Schedule *currentSchedule =f->getPerson(p)->getSchedule();
        //**std::cout<<"Person ID: "<<f->getPerson(p)->getID()<<std::endl;
        if(currentSchedule->getScheduleType()==1 || currentSchedule->getScheduleType()==2){
            //Set School
            Building *schoolLocation=findAvaliableBuilding(home->getLocation()[0], home->getLocation()[1], 'S', -1, f->getPerson(p)->getAge(), 0, 1, 1);
            if(schoolLocation==NULL){
                ////**std::cout<<"No School Location Found For School Aged Child"<<std::endl;
                ////**std::cout<<p->toString()<<std::endl;
            }else{
                ////**std::cout<<"School Found"<<std::endl;
                currentSchedule->setJobLocation(schoolLocation->getID());
            }
            
        }
        if(currentSchedule->getScheduleType()==0 && f->getDaycare()==NULL){
            //Young Child that needs to go to DayCare
            Building *daycareLocation=findAvaliableBuilding(home->getLocation()[0], home->getLocation()[1], 'D', -1, f->getPerson(p)->getAge(), 0, f->getHasYoungChild(), 1);
            if(daycareLocation==NULL){
                //**std::cout<<"No Daycare"<<std::endl;
                //**std::cout<<f->getPerson(p)->toString()<<std::endl;
            }else{
                ////**std::cout<<"School Found"<<std::endl;
                Daycare* d = static_cast<Daycare* >(daycareLocation);
                
                f->setDaycare(d);
            }
        }
        if(currentSchedule->getScheduleType()==3){
            //Employeed
            //**std::cout<<"ID: "<<home->getID()<<std::endl;
            //**std::cout<<"Loc: "<<home->getLocation()[0]<<","<<home->getLocation()[1]<<std::endl;
            Building *workLocation=findAvaliableBuilding(home->getLocation()[0], home->getLocation()[1], 'E', -1,  0, 0, 1, 1);
            if(workLocation==NULL){
                //**std::cout<<"-----No Work Location Found"<<std::endl;
            }else{
                currentSchedule->setJobLocation(workLocation->getID());
            }
            
        }
        
    }
    
    
}


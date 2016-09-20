/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   generateEmployedAdultSchedule.cpp
 * Author: yeshwanth
 * 
 * Created on August 25, 2016, 12:29 AM
 */

#include "ScheduleGenerator.h"
#include "generateEmployedAdultSchedule.h"
#include <random>
#include <vector>
#include <map>
generateEmployedAdultSchedule::generateEmployedAdultSchedule() {
}

generateEmployedAdultSchedule::generateEmployedAdultSchedule(Person *p, 
                            Family *f, int schoolChildModification,
                            bool youngChildModification, int radiusLimit,
                            double *transportProbablities,
                            int *transportRadiusLimits, int *transportRates,
                            double *visitorTypeProbablities_work,
                            double *visitorTypeProbablities_noWork,
                            bool goToWork, std::unordered_map<int, Building*> *allBuildingsTemp,
                            std::vector< std::vector < Location > > *densityData,
                            std::default_random_engine generator) {
    
    this->f = f;
    this->schoolChildModification = schoolChildModification;
    
    // Working Adult
    currentSchedule = p->getSchedule();
    currentSchedule->setGoToJobLocation(goToWork);
    allBuildings = allBuildingsTemp;
    // We get the home, lastPlace, jobLocation and dayCareLocation.
    home =f->getHome();
    lastPlace = home;
    jobLocation=static_cast<Business* >allBuildings->at(currentSchedule->getJobLocation());
    dayCareLocation = f->getDaycare();
    
    //double workProb=0.90;
    //double outProb=0.05;
    //double homeProb=0.05;
    
    //For MidWorldData
    //Schedule_Adult_Employeed_Work_Home_Probablity=0.50
    //Schedule_Adult_Employeed_Work_Out_Probablity=0.17
    //Schedule_Adult_Employeed_Work_Job_Probablity=0.33
    workProb_work = visitorTypeProbablities_work[0];
    outProb_work = visitorTypeProbablities_work[1];
    homeProb_work = visitorTypeProbablities_work[2];
    
//    MidWorldData
//    Schedule_Adult_Employeed_No_Work_Home_Probablity=0.5
//    Schedule_Adult_Employeed_No_Work_Out_Probablity=0.5
//    Schedule_Adult_Employeed_No_Work_Job_Probablity=0
    
    workProb_nowork = visitorTypeProbablities_noWork[0];
    outProb_nowork = visitorTypeProbablities_noWork[1];
    homeProb_nowork = visitorTypeProbablities_noWork[2];
    
    

    
    //Generate a schedule for everyone else
    dayTime=0;  //Time for current day (Midnight = 0, 11:59=144
    trueTime=0; //Actual HAPLOS Time (Sunday at Midnight = 0, till Saturday at 11:59 = 1008)
    totalTimeSpentAtHome=0; //Total Time spent at home for the given day
    totalTimeSpentAway = 0; //Once it hits 16 hours (96) force go home
    dailyTotalTimeAtJob = 0; //Total time spent at job in a given day (0-14 Hours, 0-84 HAPLOS Time)
    totalTimeSpentAtJob = (goToWork ? (((int)rand() % 300)+60) : 0); //Total time Spent at work during the week 10-60 hours (60-360)
    kidsAtSchool = false;
    kidsAtDaycare = false;
    transportRate =1;
    transportType = 'T';
    this->transportProbablities = transportProbablities;
    this->transportRadiusLimits = transportRadiusLimits;
    this->transportRates = transportRates;
    this->densityData = densityData;
    this->generator = generator;
    this->youngChildModification = youngChildModification;
    homeLoc = home->getLocation();
    jobLoc = jobLocation->getLocation();
    crewfew = (schoolChildModification>=0 || youngChildModification) ? 120 : 144;
    goneToWork = false;
    
    //If Need to Take care of Child give a little more buffer in case kid needs to be taken to school
    maxTimeAway = ((youngChildModification || schoolChildModification)? 90 : 96);
    fullDay=144;
    
    // Get Children School Start Times If Needed
    if(schoolChildModification>-1){
        schoolTimes=getSchoolTimes(f);
    }else{
        kidsAtSchool=true;
    }

    visitorType='H';
    travelTime = 0;
    
    
}

generateEmployedAdultSchedule::~generateEmployedAdultSchedule() {
}



void generateEmployedAdultSchedule::determineTimeToSpendAtJob() {
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
}

void generateEmployedAdultSchedule::determineTravelTimeToHome(int &lastPlaceLoc,
        int &travelTimeToHome) {
    
    lastPlaceLoc =lastPlace.getLocation();
        
    travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                   lastPlaceLoc[1],
                                                   homeLoc[0],
                                                   homeLoc[1],
                                                   1);
    
}

void generateEmployedAdultSchedule::calculateAllTravelTimes(int &travelTimeToHome,int &travelTimeToSchool,
        int &travelTimeSchoolFromJob, int &travelTimeToDayCare,int &travelTimeToJob,int &lastPlaceLoc) {
    
    travelTimeToSchool = 0;
    travelTimeSchoolFromJob = 0;
    travelTimeToDayCare = 0;
    travelTimeToJob =0;
    
    
    if(nextSchoolTime.second != nullptr) {
        int *schoolLocation = nextSchoolTime.second->getLocation();
        travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                        lastPlaceLoc[1], schoolLocation[0],
                                        schoolLocation[1],1);
                
        travelTimeSchoolFromJob = calculateTravelTime(jobLoc[0],
                                                    jobLoc[1],schoolLocation[0],
                                                    schoolLocation[1],1);
    }
    travelTimeToJob = calculateTravelTime(lastPlaceLoc[0], lastPlaceLoc[1],
                                            jobLoc[0], jobLoc[1], 1);
        
        // If the child attends dayCare, take that into account. 
        if(dayCareLocation != NULL){
            int *dayLoc = dayCareLocation->getLocation();
                
            travelTimeToDayCare = calculateTravelTime(lastPlaceLoc[0],
                                                    lastPlaceLoc[1],
                                                    dayLoc[0],dayLoc[1],1);
        }
    
}

void generateEmployedAdultSchedule::calculateMaxTime(int &maxTimeOut, int &travelTimeToHome) {
    maxTimeOut = 0;
    travelTime = 0;
        
    // The time left for the curfew is lesser than the time that can be spent away
    if(crewfew != fullDay && ( crewfew-dayTime-travelTimeToHome )< (maxTimeAway-totalTimeSpentAway-travelTimeToHome)){
        maxTimeOut=crewfew-dayTime-travelTimeToHome;
    }else{
        // if the time that can be spent away is lesser than the time 
        // left until the curfew time, then,
        // maxTimeOut is the time left until maxTimeAway can be reached.
        maxTimeOut=maxTimeAway-totalTimeSpentAway-travelTimeToHome;
    }
    if(maxTimeOut < 0){
        maxTimeOut=0;
    }
}

void generateEmployedAdultSchedule::ChildNeedsToGoToSchool(int &day,int &lastPlaceLoc) {
    // Get From School
    //**std::cout<<"\tGet Kid from School"<<std::endl;
    if(lastPlace != nextSchoolTime.second || visitorType!='V'){
        
        void computeTransportSpecifics(transportType, transportRate, travelTime, 
					travelTimeToSchool,
                            		travelTimeToSchool, 
					transportProbablities, 
					transportRadiusLimits,
                            		transportRates,
					currentSchedule,
					lastPlace,
                            		nextSchoolTime.second, 
					visitorType, 
					trueTime+dayTime);
        
//        transportType = determineTransportationType(travelTimeToSchool,
//                            travelTimeToSchool, transportProbablities,
//                            transportRadiusLimits, transportRates);
//        
//        transportRate = getTransportRate(transportType, transportRates);
//                    
//        travelTime = addMoveTo(currentSchedule, lastPlace, nextSchoolTime.second,
//                                visitorType, trueTime+dayTime, transportType,
//                                        transportRate, -1);
                }
                // Update the lastPlace 
                lastPlace=nextSchoolTime.second;
                // currentPlaceInSchoolTime is like the ptr of which day it
                // currently is in to update the and correctly assign the 
                // school timings.
                if(currentPlaceInSchoolTimes<schoolTimes.size()-1){
                    // What does currentPlaceInSchoolTimes exactly mean ?
                    currentPlaceInSchoolTimes++;
                    nextSchoolTime=schoolTimes.at(currentPlaceInSchoolTimes);
                    nextSchoolTime.first+=day*fullDay;
                    // Why are we dividing it by 2 ? 
                    if(currentPlaceInSchoolTimes==schoolTimes.size()/2){
                        //**std::cout<<"**Kids At School"<<std::endl;
                        kidsAtSchool=true;
                    }else{
                        //**std::cout<<"**Kids At Not School"<<std::endl;
                        kidsAtSchool=false;
                    }
                    
                }else{
                    // Resets the school time values !
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
            
}

void generateEmployedAdultSchedule::PickUpChildFromDayCare() {
    //**std::cout<<"\t\tPicking Up Kid At Daycare: " <<dailyTotalTimeAtJob<<std::endl;
    if(lastPlace != f->getDaycare() || visitorType!= 'V') {
        travelTime= addMoveTo(currentSchedule, lastPlace,
                f->getDaycare(), visitorType, trueTime+dayTime, transportType,
                transportRate, -1);
    }
    lastPlace=f->getDaycare();
    dayTime+=travelTime+1;
    totalTimeSpentAway+=travelTime+1;
                        
    visitorType='V';
    kidsAtDaycare=false;
}

void generateEmployedAdultSchedule::NeedToSleep(int &travelTimeToHome) {
    transportType = determineTransportationType(travelTimeToHome,
                                    travelTimeToHome,transportProbablities,
                                    transportRadiusLimits,transportRates);
                    
        transportRate = getTransportRate(transportType, transportRates);
                    
                    
        if(youngChildModification && kidsAtDaycare) {
            //Pick Up Young Child at Daycare
            PickUpChildFromDayCare();                
        }
        int timeNeedToBeHome= 48-totalTimeSpentAtHome;
        if(lastPlace != home){
            travelTime= addMoveTo(currentSchedule,
                                lastPlace,home,visitorType,trueTime+dayTime,
                                transportType,transportRate,-1);
        }
        totalTimeSpentAway+=travelTime;
        dayTime+=travelTime;
                    
        visitorType='H';
        lastPlace=home;
        if(nextSchoolTime.first-1>dayTime+timeNeedToBeHome+trueTime) {
            totalTimeSpentAtHome+= timeNeedToBeHome;
            dayTime +=timeNeedToBeHome;
            totalTimeSpentAway=0;
         } else {
                //Need to take a break to take kid to school
                if((nextSchoolTime.first-trueTime)-dayTime>0){
                    totalTimeSpentAtHome+=(nextSchoolTime.first-trueTime)-dayTime;
                } else {
                        totalTimeSpentAtHome+=1;
                }
                //**std::cout<<"\t\t\tTotal Time Spent At Home: "<<totalTimeSpentAtHome<<std::endl;
                if (nextSchoolTime.first-trueTime-travelTimeToSchool>dayTime) {
                    dayTime=nextSchoolTime.first-trueTime-travelTimeToSchool;
                }
        }
        visitorType='H';
    
}

void generateEmployedAdultSchedule::DroppingKidAtDayCare(int &timeLeft, int &travelTimeToHome) {
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

void generateEmployedAdultSchedule::TimeLeftToDoStuff(int &travelTimeToHome) {
    int timeLeft=(youngChildModification ? crewfew-dayTime-travelTimeToHome-travelTimeToDayCare+1 :
                                  crewfew-dayTime-travelTimeToHome);
                    //Check for if need to go to Job
                    //**std::cout<<"\t\t***Daily Total Time At Job: "<<dailyTotalTimeAtJob<<" Time Left: "<<timeLeft<<"***"<<std::endl;
                    //**std::cout<<kidsAtSchool<<" "<<schoolChildModification<<std::endl;
                    int timeSpentAtJob = 0;
                    //Force Job if time is running out and kids are at school or
                    if((dailyTotalTimeAtJob>(timeLeft/2) && timeLeft>2 && kidsAtSchool) ||
                       (dailyTotalTimeAtJob>0 && (schoolChildModification>-1 || youngChildModification) && kidsAtSchool)) {
                        
                        if(youngChildModification && !kidsAtDaycare){
                            DroppingKidAtDayCare(timeLeft, travelTimeToHome);   
                        }
                        
                    }
}

void generateEmployedAdultSchedule::ChildDoesntNeedToGoToSchool(int &travelTimeToHome, int &maxTimeOut) {
    //Child does not need to go to school right now
    if(totalTimeSpentAway+travelTimeToHome+1>=maxTimeOut) {
        //Been out too Long Need to Get Sleep
         NeedToSleep(travelTimeToHome);        
    } else {
        TimeLeftToDoStuff(travelTimeToHome);
    }
}

void generateEmployedAdultSchedule::scheduleBeforeCurfewTime(int &travelTimeToHome,int &travelTimeToSchool,
        int &travelTimeSchoolFromJob, int &travelTimeToDayCare,int &travelTimeToJob,int &maxTimeOut, int &lastPlaceLoc, int &day) {
    // while there is still time left before curfew, the foll. activities repeat
    while(dayTime<crewfew-travelTimeToHome-1){
        
        // Calculates max time out that can be spent.
        calculateMaxTime(maxTimeOut, travelTimeToHome);
        
        // Calculates all necessary travel Times for the schedule.       
        calculateAllTravelTimes(travelTimeToHome,travelTimeToSchool,
        travelTimeSchoolFromJob, travelTimeToDayCare, travelTimeToJob, maxTimeOut);
        
        // Check if child needs to go to school
        // The check conditions explained:
        // Child goes to school and
        // the daytime is greater than schoolTime ( is it school starttime or school end time ?) and
        // the nextSchoolTime loc is not null ()
        if(schoolChildModification>-1 && dayTime>=nextSchoolTime.first-1-trueTime-travelTimeToSchool &&
                    nextSchoolTime.second!=nullptr && day<5){
            
                ChildNeedsToGoToSchool(day, lastPlaceLoc);
        } else {
            // Child does not need to go to school right now
            ChildDoesntNeedToGoToSchool(travelTimeToHome, maxTimeOut);
        }
 
    }
}

void generateEmployedAdultSchedule::generateSchedule() {
    
    if(schoolChildModification>-1){
        schoolTimes=getSchoolTimes(f);
    }else{
        kidsAtSchool=true;
    }
    
    for(int day=0; day< 7 ; day++){
        currentPlaceInSchoolTimes = 0;
        
        nextSchoolTime=std::make_pair(99999,nullptr);
        
        travelTimeToSchool = 0;
        travelTimeSchoolFromJob = 0;
        travelTimeToDayCare =0;
        travelTimeToJob =0;
        transportRate =1;
        int *lastPlaceLoc;
        int travelTimeToHome;
        
        //Pre curfew variables.
        int maxTimeOut = 0;
        
        // Reset School Time to first one if a week day.
        // Reset to first School Time if Needed
        if(schoolChildModification>-1 && schoolTimes.size()>0 && day<5){
            nextSchoolTime=schoolTimes.at(0);
            nextSchoolTime.first+=trueTime;
        }
        
        // Calculates the amount of time to be spent at work at this day
        determineTimeToSpendAtJob();
        
        // Determine travel time to Home
        determineTravelTimeToHome(*lastPlaceLoc, travelTimeToHome);
        
        // While there is time before the curfew time
        scheduleBeforeCurfewTime(travelTimeToHome,travelTimeToSchool,
                                travelTimeSchoolFromJob, travelTimeToDayCare,
                                travelTimeToJob,maxTimeOut, *lastPlaceLoc, day);

        
                
    }
}

std::vector <std::pair<int, School*> > generateEmployedAdultSchedule::getSchoolTimes(Family *f){
    std::vector <std::pair<int, School*> > schoolTimes;
    std::map <std::string,int > existingTimes;
    std::unordered_map< int , Person> *people =f->getAllPersons();
    for(auto fm = people->begin(); fm != people->end(); fm++){
        Person *p1 = &(fm->second);
        Schedule *sc = p1->getSchedule();
        if((sc->getScheduleType()==1 || sc->getScheduleType()==2) && sc->getGoToJobLocation() == true){
            //std::cout<<sc->getJobLocation()<<std::endl;

            School *attendingSchool= static_cast<School* >(allBuildings->at(sc->getJobLocation()));
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

int generateEmployedAdultSchedule::calculateTravelTime(int & start_x, int & start_y,
                               int & end_x, int & end_y,  int & transportRate ){
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
char generateEmployedAdultSchedule::determineTransportationType(int distance, int timeLimit,
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
int generateEmployedAdultSchedule::getTransportRate(char transportType, 
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
int generateEmployedAdultSchedule::addMoveTo(Schedule *s, Building *start, Building *end, char visitorType, int endTime, char transportType,
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
void ScheduleGenerator::computeTransportSpecifics(char transportType, int transportRate, int travelTime, int travelTimeToAPlace,
                            int timeLimit, double *transportProbablities, int *transportRadiusLimits,
                            int *transportRates, Schedule *currentSchedule, Building *startLoc,
                            Building *destination, int visitorType, int endTime) {
    transportType = determineTransportationType(travelTimeToAPlace, timeLimit,
                                                transportProbablities, transportRadiusLimits, transportRates);
    
//    char tmp_transportType = *transportType;
    transportRate = getTransportRate(transportType, transportRates);
    
//    int tmp_transportRate = *transportRate;
      int transportID = -1;
//    char tmp_visitorType = *visitorType;
                            
    travelTime= addMoveTo(currentSchedule, startLoc, destination, visitorType, 
                            endTime, transportType, transportRate, transportID);
    
}
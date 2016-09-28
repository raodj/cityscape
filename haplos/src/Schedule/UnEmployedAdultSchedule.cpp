/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UnEmployedAdultSchedule.cpp
 * Author: yeshwanth
 * 
 * Created on September 27, 2016, 12:42 AM
 */

#include "UnEmployedAdultSchedule.h"

UnEmployedAdultSchedule::UnEmployedAdultSchedule() {
}

UnEmployedAdultSchedule::UnEmployedAdultSchedule(Person *p,
                            Family *f, bool childModification, int radiusLimit,
                            double *transportProbablities, int *transportRadiusLimits,
                            int *transportRates, double *visitorTypeProbablities,
                            std::unordered_map<int, Building*> *allBuildingsTemp,
                            std::vector< std::vector < Location > > *densityData,
                            std::default_random_engine generator) {
    
    currentSchedule = p->getSchedule();

    home =f->getHome();
    lastPlace = home;
   
    this->allBuildings = allBuildingsTemp;
    
    workProb=visitorTypeProbablities[0];
    outProb=visitorTypeProbablities[1];
    homeProb=visitorTypeProbablities[2];
    
    this->childModification = childModification;
    
    //Generate a schedule for everyone else
    dayTime=0;  //Time for current day (Midnight = 0, 11:59=144
    trueTime=0; //Actual HAPLOS Time (Sunday at Midnight = 0, till Saturday at 11:59 = 1008)
    totalTimeSpentAtHome=0; //Total Time spent at home for the given day
    totalTimeSpentAway = 0; //Once it hits 16 hours (96) force go home

    visitorType = 'H';
    crewfew = (childModification) ? 120 : 144;
    transportRate = 1;
    transportType = 'T';
    
    fullDay = 144;
    //If Need to Take care of Child give a little more buffer in case kid needs to be taken to school
    maxTimeAway = ((childModification)? 94 : 96);
    //Check if Child Modifications Need to Be made
    if(childModification){
        schoolTimes=getSchoolTimes(f);
    }
    int travelTime;
    int *homeLoc = home->getLocation();
    int travelTimeToSchool = 0;
    int travelTimeToHome  = 0;
    
    this->transportProbablities = transportProbablities;
    this->transportRadiusLimits = transportRadiusLimits;
    this->transportRates = transportRates;
    this->densityData = densityData;
    this->generator = generator;
    this->radiusLimit = radiusLimit;
}

void UnEmployedAdultSchedule::AssignInitialSchoolTimings(std::pair<int, School*> & nextSchoolTime) {
    // Setting initial school value 
    nextSchoolTime=schoolTimes.at(0);
    nextSchoolTime.first+=trueTime;

}

void UnEmployedAdultSchedule::CalculateTravelTimeToHomenSchool(std::pair<int, School*> & nextSchoolTime,
                                            int & lastPlaceLoc) {
    
        travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],
                                                lastPlaceLoc[1],
                                                homeLoc[0],
                                                homeLoc[1],
                                                1);
        travelTimeToSchool = 0;
        if(nextSchoolTime.second != nullptr){
            // If child needs to go to school, calculate travel time to School
            int *schoolLocation = nextSchoolTime.second->getLocation();
            travelTimeToSchool = calculateTravelTime(lastPlaceLoc[0],
                                                        lastPlaceLoc[1],
                                                        schoolLocation[0],
                                                        schoolLocation[1],
                                                        1);
        }
}

void UnEmployedAdultSchedule::ChildNeedsToBePickedUpFromSchool(std::pair<int, School*> & nextSchoolTime,
                                                             int & day, size_t & currentPlaceInSchoolTimes) {
    //Child Needs to be taken to School

    //Leave Previous Location
    if(lastPlace != nextSchoolTime.second){
        // Compute transport specifics to the school loc
        computeTransportSpecifics(transportType, transportRate, travelTime, travelTimeToSchool,
                            travelTimeToSchool, transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace,
                            nextSchoolTime.second, int visitorType, trueTime+dayTime);
    }
    // Update the last location           
    lastPlace=nextSchoolTime.second;
    // When the currentPlaceInSchoolTimes hits the half way mark
    // it means that the school end timings start.
    if(currentPlaceInSchoolTimes < schoolTimes.size()-1) {
        currentPlaceInSchoolTimes++;
        nextSchoolTime=schoolTimes.at(currentPlaceInSchoolTimes);
        nextSchoolTime.first+=day*fullDay;                
    } else {
        // we reset the values for next school timings
        nextSchoolTime=schoolTimes.at(0);
        currentPlaceInSchoolTimes=0;
        nextSchoolTime.first+=(day+1)*fullDay;
    }
    // Update all necessary values
    dayTime += travelTime;
    totalTimeSpentAway+=travelTime;
    visitorType='V';
    totalTimeSpentAtHome=0;
    dayTime++;
}

void UnEmployedAdultSchedule::BeenOutTooLong(std::pair<int, School*> & nextSchoolTime) {
    
    // Been out too long, need to get some sleep.
    // Calculate the time needed to be at home.
    int timeNeedToBeHome= (48-totalTimeSpentAtHome>1? 48-totalTimeSpentAtHome : 1);
    // If not at home, go home
    if(lastPlace != home){
        computeTransportSpecifics(transportType, transportRate, travelTime, travelTimeToHome,
                            travelTimeToHome, transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace,
                            home, visitorType, trueTime+dayTime);

        dayTime += travelTime;
        totalTimeSpentAway += travelTime;
    }
    // Update last place loc
    lastPlace=home;
    // if there is enough time to stay at home before the next school time
    // then spend time at home until then.
    if(nextSchoolTime.first-1>dayTime+timeNeedToBeHome+trueTime){
        totalTimeSpentAtHome+= timeNeedToBeHome;
        dayTime +=timeNeedToBeHome;
        totalTimeSpentAway=0;
    } else{
        // Need to take a break to take kid to school
        if(nextSchoolTime.first-travelTimeToSchool-dayTime-trueTime>0){
            totalTimeSpentAtHome+=nextSchoolTime.first-travelTimeToSchool-dayTime-trueTime;
        }
        if(nextSchoolTime.first-travelTimeToSchool-trueTime>dayTime){
            dayTime=nextSchoolTime.first-travelTimeToSchool-trueTime;
        }
    }
    visitorType='H'; 
}

void UnEmployedAdultSchedule::SpendTimeAtHome(std::pair<int, School*> & nextSchoolTime, int & timeSpentAtLocation) {

                            
    //update time to reflect staying at home once home.
    // If not at home, travel home.
    if (lastPlace!=home){
        computeTransportSpecifics(transportType, transportRate, travelTime, travelTimeToHome,
                            maxTimeAway-totalTimeSpentAway-1, transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace,
                            home, visitorType, trueTime+dayTime);
        

        lastPlace=home;
    }
    
    visitorType='H';
    dayTime+=travelTime;
    totalTimeSpentAway+=travelTime;
    if(childModification){
        if((nextSchoolTime.first-trueTime)-dayTime != 0){
            timeSpentAtLocation=(int)rand() % ((nextSchoolTime.first-trueTime)-dayTime)+1;
        } else{
            timeSpentAtLocation = 0;
        }
    } else {
        timeSpentAtLocation=((int)rand() % (crewfew-dayTime))+1;
    }
    totalTimeSpentAtHome+= timeSpentAtLocation;
                            
    //Check if time Spent Home is 8 hours or longer
    if(totalTimeSpentAtHome>=48){
        totalTimeSpentAway=0;
    }else{
        totalTimeSpentAway+= timeSpentAtLocation;
    }
    
}

void UnEmployedAdultSchedule::CalcTimeSpentAtLocation(std::pair<int, School*> & nextSchoolTime, int & timeSpentAtLocation) {
    if(dayTime+(maxTimeAway-totalTimeSpentAway)+trueTime>nextSchoolTime.first-1
                    && nextSchoolTime.second!=nullptr){
        //Need to take break to pick up child from school
        timeSpentAtLocation = (nextSchoolTime.first-1)-dayTime-trueTime;
                                
    }else{
        ////**std::cout<<"\t\t Max for Time Out: "<<96-totalTimeSpentAway<<std::endl;
        timeSpentAtLocation = ((int)rand() % (maxTimeAway -totalTimeSpentAway
                                                -travelTimeToHome))+1;
    }
                            
    if(dayTime+timeSpentAtLocation+travelTimeToHome+1>crewfew){
        timeSpentAtLocation=crewfew-dayTime-travelTimeToHome-1;
    }
}

void UnEmployedAdultSchedule::NoPlaceToGo(int &timeSpentAtLocation) {
    
    if(lastPlace != home){
        computeTransportSpecifics(transportType, transportRate, travelTime, travelTimeToHome,
                            maxTimeAway-totalTimeSpentAway-1-timeSpentAtLocation, transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace,
                            home, visitorType, trueTime+dayTime);
    }
    //Update Time Spent at Home
    totalTimeSpentAtHome+=timeSpentAtLocation;
    dayTime+=travelTime;
    visitorType='H';
    lastPlace=home;
    
    // Check if time Spent Home is 8 hours or longer
    if(totalTimeSpentAtHome>=48){
        totalTimeSpentAway=0;
    } else {
        totalTimeSpentAway+=timeSpentAtLocation+travelTime;
    }
}

void UnEmployedAdultSchedule::FoundPlaceToGo(Building & lastPlace_tmp, int &timeSpentAtLocation,
                                        int & lastPlaceLoc) {
    //Found Location
    if(lastPlace != lastPlace_tmp || visitorType!='V') {
        int *lastPlace_tmp_loc = lastPlace_tmp.getLocation();
        int travelTimeToNewLocation = calculateTravelTime(lastPlaceLoc[0],
                                                lastPlaceLoc[1],lastPlace_tmp_loc[0],
                                                lastPlace_tmp_loc[1], 1);
            
        computeTransportSpecifics(transportType, transportRate, travelTime, travelTimeToNewLocation,
                            maxTimeAway-totalTimeSpentAway-1-timeSpentAtLocation, transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace,
                            lastPlace_tmp, visitorType, trueTime+dayTime);
    }
        
    // Update the last place
    lastPlace=lastPlace_tmp;
    // Calculate travel Time to Home
    travelTimeToHome = calculateTravelTime(lastPlaceLoc[0],lastPlaceLoc[1],
                                                homeLoc[0],homeLoc[1],
                                                transportRate);
    // Calculate the timeSpentAtLocation
    if(timeSpentAtLocation-travelTimeToHome-1>0){
        timeSpentAtLocation=timeSpentAtLocation-travelTimeToHome-1;
    } else {
        timeSpentAtLocation=1;
    }
                                
    lastPlace->addVisitorTimeSlot(trueTime+dayTime,timeSpentAtLocation);
    totalTimeSpentAtHome=0;
    totalTimeSpentAway+=timeSpentAtLocation+travelTime;
    dayTime+=travelTime;
    visitorType='V';
                                 
}

void UnEmployedAdultSchedule::SpendTimeOutside(std::pair<int, School*> & nextSchoolTime, int &timeSpentAtLocation,
                                        int & lastPlaceLoc) {
    
    timeSpentAtLocation=1;
    CalcTimeSpentAtLocation(nextSchoolTime, timeSpentAtLocation);
                            
    //**std::cout<<"\t\tTime Spent at Location:"<<timeSpentAtLocation<<std::endl;
    //Update Schedule To reflect moving of location
    int tempRadiusLimit = timeSpentAtLocation-1;
    Building *lastPlace_tmp=findAvaliableBuilding(home->getLocation()[0],
                                        home->getLocation()[1],'V',
                                        tempRadiusLimit,trueTime+dayTime,
                                        trueTime+dayTime+timeSpentAtLocation,
                                        1,transportRate);
                            
    if(lastPlace_tmp == NULL){
        //No avaliable place to go so just return back to home
        NoPlaceToGo(timeSpentAtLocation);
    } else {
        FoundPlaceToGo(lastPlace_tmp, timeSpentAtLocation, lastPlaceLoc);
    } // End Of Null Check Else
}

void UnEmployedAdultSchedule::NoChildToPickUp(std::pair<int, School*> & nextSchoolTime, int & lastPlaceLoc) {
    
    // Check if adult has been out too long.
    if(totalTimeSpentAway+travelTimeToHome+1>=maxTimeAway || dayTime+travelTimeToHome+1>=crewfew) {
        // Been out too Long Need to Get Sleep
        BeenOutTooLong(nextSchoolTime);
    }else{
        int timeSpentAtLocation=0;
        //Can go out
        switch(distribution(generator)){
            case 0:
                //Home
                SpendTimeAtHome(nextSchoolTime, timeSpentAtLocation);
                break;
                
            case 1:
                //Job (Should not Happen)
                break;
            case 2:
                //Out
                SpendTimeOutside(nextSchoolTime, timeSpentAtLocation, lastPlaceLoc);
                break;            
                            
        } //End of Switch
        //Advance Clock
        dayTime+=timeSpentAtLocation;
    }//End of Can Do Something else
}

void UnEmployedAdultSchedule::ScheduleBeforeCurfewTime(std::pair<int, School*> & nextSchoolTime, int & day,
                                        size_t & currentPlaceInSchoolTimes) {
    //While there is still time left in the day
    while(dayTime<crewfew){
        // Get the location of the last place 
        int *lastPlaceLoc =lastPlace->getLocation();
        // Calculate the travel Time to Home and School
        CalculateTravelTimeToHomenSchool(lastPlaceLoc, nextSchoolTime);
        
        travelTime = 0;
        
        //Check that Child doesn't need to be taken to School
        if(childModification && dayTime+travelTimeToSchool>=nextSchoolTime.first-1-trueTime&& nextSchoolTime.second!=nullptr && day<5){
                ChildNeedsToBePickedUpFromSchool(nextSchoolTime, day, currentPlaceInSchoolTimes);
        } else {
            NoChildToPickUp(nextSchoolTime, lastPlaceLoc);
        }
    } // end of While day < curfew
}

void UnEmployedAdultSchedule::CurfewNotEqualsFullDay() {
    //**std::cout<<"\tHandeling Crewfew"<<std::endl;
    if(lastPlace != home){
        computeTransportSpecifics(transportType, transportRate, travelTime, int travelTimeToHome,
                            travelTimeToHome, transportProbablities, transportRadiusLimits,
                            transportRates, currentSchedule, lastPlace,
                            home, visitorType, trueTime+dayTime);

        lastPlace=home;
        visitorType='H';
                
    }
    // update values
    dayTime+=travelTime;
    totalTimeSpentAway+=travelTime;
    // This is actually the forcing of the person to be at home for the rest of the day and then some 
    // in the morning (6AM in this case). This is trying to make sure you don't have 
    // adults with children waking up at 3AM and going places with the kids since that is not typical.
    if(dayTime<fullDay+30) {
        int timeSpentAtLocation = fullDay-dayTime+30;
        totalTimeSpentAtHome+= timeSpentAtLocation;
        dayTime+=timeSpentAtLocation;
    }
    // Calculate time spent at home
    if(totalTimeSpentAtHome<48){
        dayTime+=48-totalTimeSpentAtHome;
        totalTimeSpentAway=0;
    }
}

void UnEmployedAdultSchedule::DealingWithDayCarriedOver(std::pair<int, School*> & nextSchoolTime) {

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
           
}

void UnEmployedAdultSchedule::CheckNextForCompletelyEmptySchedule() {
    //Check for Completely Empty Schedule
    if(currentSchedule->peekNextLocation()== NULL) {
        currentSchedule->addTimeSlot(TimeSlot(lastPlace->getID(), 1008, 'H'));
    }
}

void UnEmployedAdultSchedule::GenerateSchedule() {
    
    for(int day=0; day< 7 ; day++){
        
        size_t currentPlaceInSchoolTimes = 0;
        // Adding a pair that would mark the end of the segment.
        std::pair<int, School*> nextSchoolTime=std::make_pair(9999,nullptr);

        // Assign school timings according to the day
        if(childModification && schoolTimes.size()>0 && day<5){
            AssignInitialSchoolTimings(nextSchoolTime);
        }
        // All activities before the curfew time 
        ScheduleBeforeCurfewTime(nextSchoolTime, day, currentPlaceInSchoolTimes);
        
        // If curfew is not the full day and there is time left
        CurfewNotEqualsFullDay();
        
        trueTime+=dayTime;
        
        if(dayTime>fullDay){
            DealingWithDayCarriedOver(nextSchoolTime);
        } else{
            //Day Didn't bleed Over
            dayTime=0;
        }   
        
    }
    
    CheckNextForCompletelyEmptySchedule();
    
}


std::vector <std::pair<int, School*> > UnEmployedAdultSchedule::getSchoolTimes(Family *f){
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


int UnEmployedAdultSchedule::calculateTravelTime(int start_x, int start_y, int end_x, int end_y,  int transportRate ){
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
char UnEmployedAdultSchedule::determineTransportationType(int distance, int timeLimit,
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
int UnEmployedAdultSchedule::getTransportRate(char transportType, 
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
int UnEmployedAdultSchedule::addMoveTo(Schedule *s, Building *start, Building *end, char visitorType, int endTime, char transportType,
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
void UnEmployedAdultSchedule::computeTransportSpecifics(char transportType, int transportRate, int travelTime, int travelTimeToAPlace,
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


Building* UnEmployedAdultSchedule::findAvaliableBuilding(int x, int y, char typeOfVisitor, int radius, int startTime, int endTime, int numberOfVisitors, int transportRate){
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
    
    if(b!= NULL){
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
                    if(b!= NULL){
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
                    if(b!= NULL){
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
                    if(b!= NULL){
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
                    if(b!= NULL){
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

UnEmployedAdultSchedule::~UnEmployedAdultSchedule() {
}


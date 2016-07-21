/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SchoolAgedChildSchedule.h
 * Author: yeshwanth
 *
 * Created on July 18, 2016, 5:06 PM
 */

#include <vector>
#include "../Location.h"
#include "../Population.h"
#include "../Family.h"


#ifndef SCHOOLAGEDCHILDSCHEDULE_H
#define SCHOOLAGEDCHILDSCHEDULE_H

class SchoolAgedChildSchedule {
public:
    
    //Older School Aged Child (On own after school)
    Schedule *currentSchedule;
    Building *home;
    
    double workProb;
    double outProb;
    double homeProb;
    
    enum ChildState { ATHOME, ATSCHOOL, OUTSIDE };
    ChildState state;
    
    std::discrete_distribution<int> distribution{homeProb,workProb,outProb};
    std::default_random_engine generator;
    
    std::vector< std::vector < Location > > *densityData;
    
    School *attendingSchool;

    int *schoolLoc;
    int *homeLoc;
    
    int dayTime;  // Time for current day (Midnight = 0, 11:59=144
    int trueTime; // Actual HAPLOS Time (Monday at Midnight = 0, till Sunday at 11:59 = 1008)
    int totalTimeSpentAtHome; // Total Time spent at home for the given day
    int maxTimeAway;
    int totalTimeSpentAway;
    int crewfew;
    int visitorType;
    int sleepTimeNeeded;
    char transportType;
    int transportRate;
    
    double *transportProbablities;
    int *transportRadiusLimits;
    int *transportRates;
    int radiusLimit;
    
    bool goToSchool;
    int schoolStart;
    int schoolEnd;
    
    //Add One Hour for Sleep for Previous Day
    int timeUpOnMonday;
    
    int travelTime;
    int travelTimeToHome;
    int travelTimeToSchool;
    
    
    void generateSchedule();
    void generateWeekDaySchedule(Building *lastPlace, int *lastPlaceLoc);
    void generateWeekEndSchedule(Building *lastPlace, int *lastPlaceLoc, const int &day);
    void generateBeforeSchoolSchedule(Building *lastPlace, int *lastPlaceLoc);
    void generateAfterSchoolSchedule(Building *lastPlace, int *lastPlaceLoc);
    void goingToSchoolSchedule(Building *lastPlace, int *lastPlaceLoc);
    
    void determinePlace_basedOn_Prob(Building *lastPlace, int *lastPlaceLoc);
    void determinePlace_basedOn_Prob_AfterSchool(Building *lastPlace, int *lastPlaceLoc);
    void determinePlace_basedOn_Prob_WeekEnd(Building *lastPlace, int *lastPlaceLoc, int &timeSpentOut);
    
    int calculateTravelTime(int start_x,
                                int start_y,
                                int end_x,
                                int end_y,
                                int transportRate );
    
    void computeTransportSpecifics(char transportType,
                                        int transportRate,
					int travelTime, 
					int travelTimeToAPlace,
                            		int  timeLimit, 
					double *transportProbablities, 
					int *transportRadiusLimits,
                            		int *transportRates,
					Schedule *currentSchedule,
					Building *startLoc,
                            		Building *destination, 
					int visitorType, 
					int endTime);
    
    char determineTransportationType(int distance, int timeLimit,
        double *transportProbablities, int *transportRadiusLimits, 
        int *transportRates);
    
    int getTransportRate(char transportType, 
        int *transportRates);
    
    int addMoveTo(Schedule *s, Building *start, Building *end, 
        char visitorType, int endTime, char transportType,
        int transportRate, int transportID);
    
    Building* findAvaliableBuilding(int x, int y, 
        char typeOfVisitor, int radius, int startTime, int endTime,
        int numberOfVisitors, int transportRate);
    
    SchoolAgedChildSchedule();
    SchoolAgedChildSchedule(Person *p, Family *f, int radiusLimit,
        double *transportProbablities,
        int *transportRadiusLimits, int *transportRates,
        double *schoolDayVisitorTypeProbablities,
        double *weekendVisitorTypeProbablities,
        bool goToSchool, std::unordered_map<int, Building*>  *allBuildings, 
        std::vector< std::vector < Location > > *densityData);
    virtual ~SchoolAgedChildSchedule();
private:

};

#endif /* SCHOOLAGEDCHILDSCHEDULE_H */


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   generateEmployedAdultSchedule.h
 * Author: yeshwanth
 *
 * Created on August 25, 2016, 12:29 AM
 */

#ifndef GENERATEEMPLOYEDADULTSCHEDULE_H
#define GENERATEEMPLOYEDADULTSCHEDULE_H

class generateEmployedAdultSchedule {
public:
    Family *f;
    int schoolChildModification;
    bool youngChildModification;
    std::unordered_map<int, Building*> *allBuildings;
    Schedule *currentSchedule;

    Building *home;
    Building *lastPlace;
    Building *jobLocation;
    Building *dayCareLocation;
    
    double workProb_work;
    double outProb_work;
    double homeProb_work;
    
    
    double workProb_nowork;
    double outProb_nowork;
    double homeProb_nowork;
    
    std::discrete_distribution<int> distribution{homeProb_work,workProb_work,outProb_work};
    std::discrete_distribution<int> no_work_left_distribution{homeProb_nowork,workProb_nowork,outProb_nowork};
    
    std::default_random_engine generator;
    
    
    
    //Generate a schedule for everyone else
    int dayTime;
    int trueTime;
    int totalTimeSpentAtHome;
    int totalTimeSpentAway;
    int dailyTotalTimeAtJob;
    int totalTimeSpentAtJob;
    bool kidsAtSchool;
    bool kidsAtDaycare;
    int transportRate;
    char transportType;
    int *homeLoc;
    int *jobLoc;
    int crewfew;
    bool goneToWork;
    

    int maxTimeAway;
    int fullDay;

    std::vector <std::pair<int, School*> >schoolTimes;
    std::vector< std::vector < Location > > *densityData;
    
    char visitorType;
    int travelTime;
    
    size_t currentPlaceInSchoolTimes = 0;
        
    std::pair<int, School*> nextSchoolTime;
    int travelTimeToSchool;
    int travelTimeSchoolFromJob;
    int travelTimeToDayCare;
    int travelTimeToJob;

    double *transportProbablities;
    int *transportRadiusLimits;
    int *transportRates;
    int radiusLimit;
    
    generateEmployedAdultSchedule();
    generateEmployedAdultSchedule(Person *p, Family *f, int schoolChildModification,
                                    bool youngChildModification, int radiusLimit,
                                    double *transportProbablities,
                                    int *transportRadiusLimits, int *transportRates,
                                    double *visitorTypeProbablities_work,
                                    double *visitorTypeProbablities_noWork,
                                    bool goToWork, 
                                    std::unordered_map<int, Building*> *allBuildingsTemp,
                                    std::vector< std::vector < Location > > *densityData);

    
   // virtual ~generateEmployedAdultSchedule();
    
    void generateSchedule();
    
    void determineTimeToSpendAtJob();
    void determineTravelTimeToHome(int* &lastPlace, int &travelTimeToHome);
    void scheduleBeforeCurfewTime(int &travelTimeToHome,int &travelTimeToSchool,
        int &travelTimeSchoolFromJob, int &travelTimeToDayCare,int &travelTimeToJob,int &maxTimeOut, int lastPlaceLoc[], int &day);
    
    void calculateAllTravelTimes(int &travelTimeToHome,int &travelTimeToSchool,
        int &travelTimeSchoolFromJob, int &travelTimeToDayCare,int &travelTimeToJob,int lastPlaceLoc[]);
    
    void calculateMaxTime(int &maxTimeOut, int &travelTimeToHome);
    
    void ChildNeedsToGoToSchool(int &day,int lastPlaceLoc[]);
    void ChildDoesntNeedToGoToSchool(int &travelTimeToHome, int &maxTimeOut, int lastPlaceLoc[]);
    
    
    void NeedToSleep(int &travelTimeToHome);
    void TimeLeftToDoStuff(int &travelTimeToHome, int &maxTimeOut, int lastPlaceLoc[]);

    void PickUpChildFromDayCare();
    void DroppingKidAtDayCare(int &timeLeft, int &travelTimeToHome);
    void goBackToJobLoc(int &timeSpentAtJob);
    void determineActivity(int &travelTimeToHome, int &timeLeft, int &maxTimeOut, int lastPlaceLoc[]);
    int determineDistribution(int &travelTimeToHome, int &maxTimeOut);
    void selectActivity(int &activity, int &travelTimeToHome, int &timeLeft, int &maxTimeOut, int lastPlaceLoc[]);
    
    void GoingHome(int &timeSpentAtLocation, int &travelTimeToHome, int &timeLeft, int &maxTimeOut);
    void GoingToJob(int &timeSpentAtLocation, int &travelTimeToHome);
    void ChanceOfGoingOutside(int &timeSpentAtLocation, int &maxTimeOut, int &travelTimeToHome,
                                                            int &timeLeft, int lastPlaceLoc[]);
    int CalculateTimeSpentAtLocation(int &timeSpentAtLocation, int &maxTimeOut, int &timeLeft);
    
    void NoPlaceToGoOut(int &timeSpentAtLocation, int &travelTimeToHome);
    void FoundPlaceToGoOut(int &timeSpentAtLocation, int &travelTimeToHome, int lastPlaceLoc[], Building* lastPlace_tmp);
    
    void CurfewNotEqualsFullDay(int &travelTimeToHome, int lastPlaceLoc[], int &day);
    
    void DealingWithDayCarriedOver();
    
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
    
    int calculateTravelTime(int start_x, int start_y, int end_x, 
                                        int end_y,  int transportRate );
    
    std::vector <std::pair<int, School*> > getSchoolTimes(Family *f);
    
    Building* findAvaliableBuilding(int x, int y, char typeOfVisitor, int radius, int startTime, int endTime, int numberOfVisitors, int transportRate);
    
    
private:

};

#endif /* GENERATEEMPLOYEDADULTSCHEDULE_H */


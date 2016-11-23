/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UnEmployedAdultSchedule.h
 * Author: yeshwanth
 *
 * Created on September 27, 2016, 12:42 AM
 */

#ifndef UNEMPLOYEDADULTSCHEDULE_H
#define UNEMPLOYEDADULTSCHEDULE_H

#include "ScheduleGenerator.h"

class UnEmployedAdultSchedule {
public:
    
    Schedule *currentSchedule;

    Building *home;
    Building *lastPlace;
    
    double workProb;
    double outProb;
    double homeProb;
    
    std::unordered_map<int, Building*> *allBuildings;
    
    
    std::discrete_distribution<int> distribution{homeProb,workProb,outProb};
    
    //Generate a schedule for everyone else
    int dayTime;  //Time for current day (Midnight = 0, 11:59=144
    int trueTime; //Actual HAPLOS Time (Sunday at Midnight = 0, till Saturday at 11:59 = 1008)
    int totalTimeSpentAtHome; //Total Time spent at home for the given day
    int totalTimeSpentAway; //Once it hits 16 hours (96) force go home
    std::vector <std::pair<int, School*> >schoolTimes;
    char visitorType;
    int crewfew;
    int transportRate;
    int transportType;
    
    int fullDay;
    //If Need to Take care of Child give a little more buffer in case kid needs to be taken to school
    int maxTimeAway;

    int travelTime;
    int *homeLoc;
    int travelTimeToSchool;
    int travelTimeToHome;
    
    bool childModification;
    
    double *transportProbablities;
    int *transportRadiusLimits;
    int *transportRates;
    int radiusLimit;
    
    std::vector< std::vector < Location > > *densityData;
    std::default_random_engine generator;
    
    UnEmployedAdultSchedule();
    
    UnEmployedAdultSchedule(Person *p,
                            Family *f, bool childModification, int radiusLimit,
                            double *transportProbablities, int *transportRadiusLimits,
                            int *transportRates, double *visitorTypeProbablities,
                            std::unordered_map<int, Building*> *allBuildingsTemp,
                            std::vector< std::vector < Location > > *densityData);
    
    
    void GenerateSchedule();
    
    void AssignInitialSchoolTimings(std::pair<int, School*> & nextSchoolTime);
    void ScheduleBeforeCurfewTime(std::pair<int, School*> & nextSchoolTime, int & day, size_t & currentPlaceInSchoolTimes);
    
    void CalculateTravelTimeToHomenSchool(std::pair<int, School*> & nextSchoolTime,
                                            int lastPlaceLoc[]);
    void ChildNeedsToBePickedUpFromSchool(std::pair<int, School*> & nextSchoolTime,
                                            int & day, size_t & currentPlaceInSchoolTimes);
    
    void NoChildToPickUp(std::pair<int, School*> & nextSchoolTime, int lastPlaceLoc[]);
    
    void BeenOutTooLong(std::pair<int, School*> & nextSchoolTime);
    
    void SpendTimeAtHome(std::pair<int, School*> & nextSchoolTime, int & timeSpentAtLocation);
    void SpendTimeOutside(std::pair<int, School*> & nextSchoolTime, int &timeSpentAtLocation,
                                        int lastPlaceLoc[]);
    
    void CalcTimeSpentAtLocation(std::pair<int, School*> & nextSchoolTime, int & timeSpentAtLocation);
    
    void NoPlaceToGo(int &timeSpentAtLocation);
    void FoundPlaceToGo(Building * lastPlace_tmp, int &timeSpentAtLocation,
                                        int lastPlaceLoc[]);
    
    void CurfewNotEqualsFullDay();
    void DealingWithDayCarriedOver(std::pair<int, School*> & nextSchoolTime);
    
    void CheckNextForCompletelyEmptySchedule();
    
    void computeTransportSpecifics(char transportType, int transportRate, int travelTime, int travelTimeToAPlace,
                            int timeLimit, double *transportProbablities, int *transportRadiusLimits,
                            int *transportRates, Schedule *currentSchedule, Building *startLoc,
                            Building *destination, int visitorType, int endTime);
    
    char determineTransportationType(int distance, int timeLimit,
                            double *transportProbablities, int *transportRadiusLimits, 
                            int *transportRates);
    
    int getTransportRate(char transportType, int *transportRates);
    
    int addMoveTo(Schedule *s, Building *start, Building *end, char visitorType, int endTime, char transportType,
                                 int transportRate, int transportID);
    
    Building* findAvaliableBuilding(int x, int y, char typeOfVisitor, int radius, int startTime, int endTime, int numberOfVisitors, int transportRate);
    
    std::vector <std::pair<int, School*> > getSchoolTimes(Family *f);
    int calculateTravelTime(int start_x, int start_y, int end_x, int end_y,  int transportRate );
    
    virtual ~UnEmployedAdultSchedule();
private:

};

#endif /* UNEMPLOYEDADULTSCHEDULE_H */


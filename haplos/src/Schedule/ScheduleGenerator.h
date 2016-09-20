#ifndef __haplos__ScheduleGenerator__
#define __haplos__ScheduleGenerator__
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

#include <vector>
#include "../Location.h"
#include "../Population.h"
#include "../Family.h"

class ScheduleGenerator {
    public:
        ScheduleGenerator();
        ScheduleGenerator(std::vector< std::vector < Location > > *densityData,
                          std::unordered_map<int, Building*> *allBuildings,
                          std::default_random_engine generator,
                          bool progressDisplay);
        ScheduleGenerator(const ScheduleGenerator &p);

        void generateSchedules(Population &pop,
                               int *radiusLimit,
                               double *transportProbablities,
                               int *transportRadiusLimits,
                               int *transportRate,
                               double *olderSchoolChildSchoolDayVisitorProbablities,
                               double *olderSchoolChildWeekendVisitorProbablities,
                               double *adultNoworkVisitorProbablites,
                               double *adultWorkVisitorProbablities,
                               double *adultUnemployeedVisitorProbablities);
        void generatePersonSchedule(Family *currentFamily,
                                    Person *p1,
                                    int *radiusLimit,
                                    double *transportProbablities,
                                    int *transportRadiusLimits,
                                    int *transportRates,
                                    double *primaryVisitorTypeProb,
                                    double *secondaryVisitorTypeProbs,
                                    bool specialLocationFlag,
                                    std::default_random_engine generator);
    private:

        //Schedue Types
        void generateYoungChildSchedule(Person* p,
                                        Family *f,
                                        int radiusLimit);
        void generateYoungSchoolAgedChildSchedule(Person *p,
                                                  Family *f,
                                                  int radiusLimit,
                                                  bool goToSchool);
        void generateSchoolAgedChildSchedule(Person *p,
                                             Family *f,
                                             int radiusLimit,
                                             double *transportProbablities,
                                             int *transportRadiusLimits,
                                             int *transportRates,
                                             double *schoolDayVistorTypeProbablities,
                                             double *weekendVistorTypeProbablities,
                                             bool goToSchool);
    
        void generateEmployeedAdultSchedule(Person *p, Family *f, int schoolChildModification,
                                            bool youngChildModification, int radiusLimit,
                                            double *transportProbablities, int *transportRadiusLimits,
                                            int *transportRates, double *visitorTypeProbablities_work,
                                            double *visitorTypeProbablities_noWork,
                                            bool goToWork);
        void generateUnemployeedAdultSchedule(Person *p, Family *f, bool childModification, int radiusLimit,
                                              double *transportProbablities, int *transportRadiusLimits,
                                              int *transportRates, double *visitorTypeProbablities);
        //Helper Methods

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

        char determineTransportationType(int distance,
                                         int timeLimit,
                                         double *transportProbablities,
                                         int *transportRadiusLimits,
                                         int *transportRates);
        int getTransportRate (char transportType,
                              int *transportRates);
        bool sort_schoolTimes(const std::pair<int, School*>& firstElem,
                              const std::pair<int, School*>& secondElem);
        std::vector <std::pair<int, School*> > getSchoolTimes(Family *f);
        int addMoveTo(Schedule *s,
                      Building *start,
                      Building *end,
                      char visitorType,
                      int endTime,
                      char transportType,
                      int transportRate,
                      int transportID);
        int calculateTravelTime(int start_x,
                                int start_y,
                                int end_x,
                                int end_y,
                                int transportRate );
        Building* findAvaliableBuilding(int x,
                                        int y,
                                        char typeOfVisitor,
                                        int radius,
                                        int startTime,
                                        int endTime,
                                        int numberOfVisitors,
                                        int transportRate);
        void assignJobSchoolLocations(Family *f);
    
        //Variables
        std::vector< std::vector < Location > > *densityData;
        std::unordered_map<int, Building*> *allBuildings;
        bool progressDisplay;
        std::default_random_engine generator;


	
};

#endif /* defined(__haplos__ScheduleGenerator__) */



#ifndef POPULATION_H_
#define POPULATION_H_
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
#include "Person.h"
#include "Family.h"
#include "Buildings/Building.h"
#include "Schedule/Schedule.h"
#include <vector>
#include <unordered_map>
#include <random>
class Population {
    /** A class specifically repersenting a population.
     */
    public:
        Population();
        /** The default constructor for this class.
         
         \param[in] size Size of population to create.
         \param[in] age2024Prob Probablity of someone being age 20-24.
         \param[in] age2534Prob Probablity of someone being age 25-34.
         \param[in] age3549Prob Probablity of someone being age 35-49.
         \param[in] age5064Prob Probablity of someone being age 50-64.
         \param[in] age65OlderProb Probablity of someone being age 65 or older.
         \param[in] maleProb Probablity of a someone being a male.
         \param[in] familysize1Prob Probably of creating a family of size 1.
         \param[in] familysize1Prob Probably of creating a family of size 2.
         \param[in] familysize1Prob Probably of creating a family of size 3.
         \param[in] familysize1Prob Probably of creating a family of size 4.
         \param[in] familysize1Prob Probably of creating a family of size 5.
         \param[in] familysize1Prob Probably of creating a family of size 6.
         \param[in] familysize1Prob Probably of creating a family of size 7.
         \param[in] popSeed Seed used to generate Population (-1 will use a random seed based on time)

         */
        Population(int size, double *ageProbablities, double *familySizeProbablities,
                   double maleProbablity, double *scheduleProbablities, int popSeed);
    
        Population(const Population &p);

        /** Generates population

        \param[in] progressDisplay Display progress of creating population.
         
         */
        void generatePopulation(bool progressDisplay);
        /** Display Statistics about population to Console.
         */
        void displayStatistics(std::string fileLocation);
    
        /** Set the home location of a family and move all members to that location.
         \param[in] home Location family should be set to.
         \param[in] family Family to assign location to.
        */
        void setHomeLocationOfFamily(Building *home, int family);

        /** Get a Specific Family
          \param[in] family Family to retrive
         */
        Family* getFamily(int family);
    
    
        /** Get Number of People in Population
         
         \return Number of People in Population
         */
        int getPopulationSize();
    
        /** Get Number of Families Created
         
         \return Number of Families in Population
         */
        int getNumberOfFamilies();
    
        /** Get Number of Employeed Adults Created
         
         \return Number of of Employeed Adults in Population
         */
        int getNumberOfEmployeedAdults();
    
    
        /** Get Number of Students Per Grade Created
         
         \return Array containing the Students Per Grade Created
         */
        int* getNumberOfStudentsPerGrade();
    
    
        int getNumberOfChildrenDaycare();
    
        void updateToNextTimeStep(std::unordered_map<int, Building*> *allBuildings);
    
    
        void exportPopulation(std::string fileLocation);
    
        void importPopulation(std::string fileLocation, std::unordered_map<int, Building*> *allBuildings);
    
        std::string returnFirstTenFamiliesInfo(std::string fileLocation);
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Population();
    private:
        /** Set gender for person in the population.
         \return 'f' for female, 'm' for male.
         */
        char determineGender();
    
        /** Set age for person in the population.
         \param[in] forceAdult forces the creation of adult.
         \return an array consisting of the age of person and the agegroup the person falls into.
         */
        int* determineAge(bool forceAdult, int *returnArray);
    
        /** Set Schedule Type for person in the population.
            \param[in] ageGroup 0= 5 or younger, 1= 5-13, 2=14-17, 3=18-24, 4=25-44, 5=45-64, 6=65 or older
            \return 'E'=Employeed Schedule, 'U'= Unemployeed Schedule, 'S' = School schedule,
                    'Y'= Young Child Schedule
         */
        int determineScheduleType(int ageGroup);
    
        /** Set age for population of person in population.
         */
        int generateFamilySize();
    
        void updateStatistics(int age, int scheduleType, char gender);
    
        int size;   //Size of Population to Generate
    
        //Distributions
        double *ageProbablities;    //Age Distribution
        double *familySizeProbablites;  //Family Size Distrbution
        double *scheduleProbablities;   //Schedule Type Distribution
        double maleProbablity;
    
        //Statistics Helpers
        int numberOfPeopleAges[7]={};
        int numberOfMales=0;
        int numberOfFamiliesSizes[7]={};
        int numberOfPeopleAssignedSchedule[11]={};
        int numberOfStudentsAssignedGrade[13]={};
        int numberOfChildrenDaycare=0;
        std::default_random_engine generator;

        std::unordered_map < int, Family > families;    //Families in Population.
};


#endif /* POPULATION_H_ */

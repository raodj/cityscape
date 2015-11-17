//------------------------------------------------------------
//
// This file is part of HAPLOS availabe off the website at
// <http://pc2lab.cec.miamiOH.edu/haplos>
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

#ifndef __haplos__BuildingGenerator__
#define __haplos__BuildingGenerator__
#include "../Population.h"
#include "../Location.h"
#include "Business.h"

#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class BuildingGenerator{
    public:
        BuildingGenerator(std::default_random_engine generator,
                          int numberOfBuildings,
                          std::vector< School > *schoolBuildings,
                          std::vector< Daycare > *daycareBuildings,
                          std::vector< Medical > *medicalBuildings,
                          std::vector< Business > *businessBuildings,
                          std::vector< Building > *otherBuildings,
                          std::unordered_map<int, Building*> *allBuildings,
                          std::vector< std::vector < Location > > *densityData,
                          bool progressDisplay);
        void generateBuildings(double businessSizeProbablities[6], double hospitalSizeProbablities[6],
                               double schoolSizeProbablities[6], std::vector<double> locationProbablties,
                               int const width, int const numberOfEmployeedAdults, int* numberOfStudentsPerGrade,
                               int numberChildrenDaycare );
        void assignHomes(Population &pop);
        void displayBuildingStatistics(double businessSizeProbablities[6], double hospitalSizeProbablities[6],
                                       double schoolSizeProbablities[6], double daycareSizeProbablities[6],
                                       std::string fileLocation);
    
    void exportBuildings(std::string fileLocation);

    void importBuildings(std::string fileLocation);
    private:
        void updateStatistics(char buildingType, int capacity);

        std::default_random_engine generator;
        bool progressDisplay = false;
        std::vector< School > *schoolBuildings;
        std::vector< Daycare > *daycareBuildings;
        std::vector< Medical > *medicalBuildings;
        std::vector< Building > *otherBuildings;
        std::vector< Business > *businessBuildings;

        std::unordered_map<int, Building*> *allBuildings;
        std::vector< std::vector < Location > > *densityData;
        int totalBusinessSize[6] = {0,0,0,0,0,0};
        int totalHospitalSize[6] = {0,0,0,0,0,0};
        int totalSchoolSize[6] = {0,0,0,0,0,0};
        int totalDaycareSize[6] = {0,0,0,0,0,0};
        int totalSchools = 0;
        int totalHospitals = 0;
        int totalBusinesses = 0;
        int totalDaycares = 0;
        int numberOfBuildings = 0;
    
    
};

#endif /* defined(__haplos__BuildingGenerator__) */

#ifndef HAPLOS_CPP
#define HAPLOS_CPP

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

#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

#include "SedacReader.h"
#include "ImageGeneration/ImageGen.h"
#include "Location.h"
#include "Population.h"
#include "ImageGeneration/XFigImageGenerator.h"
#include "Files/ConfigFile.h"
#include "Files/ImageFileGenerator.h"

#include "Buildings/Medical.h"
#include "Buildings/School.h"
#include "Buildings/Business.h"
#include "Buildings/Daycare.h"

using namespace std;

std::string const outputFolder="output/";
//std::string configFile="examples/config/VirginaData.hapl";
//std::string configFile="examples/config/USAData.hapl";
std::string configFile="examples/config/MicroWorldData.hapl";
std::vector< School > schoolBuildings;
std::vector< Business > businessBuildings;
std::vector< Daycare > daycareBuildings;
std::vector< Medical > medicalBuildings;
std::vector< Building > otherBuildings;
std::unordered_map<int, Building*> allBuildings;

std::vector< std::vector < Location > > densityData;

std::default_random_engine generator;

ConfigFile configuration;
bool progressDisplay=true;

int totalBusinessSize[6];
int totalHospitalSize[6];
int totalSchoolSize[6];
int totalDaycareSize[6];
int totalSchools=0;
int totalHospitals=0;
int totalBusinesses=0;
int totalDaycares=0;
int numberOfBuildings=0;

#include "buildingHelper.pcpp"
#include "buildingGeneratorHelper.pcpp"
#include "scheduleGeneratorHelper.pcpp"



int main(int argc, char* argv[]) {
    //Display Awesome Logo
    std::cout << " _    _          _____  _      ____   _____" <<std::endl;
    std::cout << "| |  | |   /\\   |  __ \\| |    / __ \\ / ____|" <<std::endl;
    std::cout << "| |__| |  /  \\  | |__) | |   | |  | | (___" << std::endl;
    std::cout << "|  __  | / /\\ \\ |  ___/| |   | |  | |\\___ \\" << std::endl;
    std::cout << "| |  | |/ ____ \\| |    | |___| |__| |____) |" << std::endl;
    std::cout << "|_|  |_/_/    \\_\\_|    |______\\____/|_____/" <<std::endl;
    bool produceImages=true;
    //Command Line Paramaters
    if(argc>0){
        //Do Not Use Default
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if ((arg == "--configFile") || (arg=="-c")) {
                //Configuration File Path
                 if (i + 1 < argc) {
                     configFile=argv[++i];
                 }else{
                     std::cerr << "--configFile option requires one argument." << std::endl;
                 }
            }else{
                if((arg=="--noImages")||(arg=="-ni")){
                    //No Images Produced
                    produceImages=false;
                }
                if((arg=="--noProgressDisplay")|| (arg=="-npd")){
                    progressDisplay=false;
                }
            }
        }
    }
    
    //Read in Configuration File
    configuration= ConfigFile(configFile);
    
    //Read in Density Data
    SedacReader sr = SedacReader();
    densityData = sr.readFile(configuration.getSedacFileLocation(),
                              configuration["Density_File_Total_Population"],
                              configuration["Total_Population"]);
    
    //Extract out Pure Density Data
    std::vector<double> pureDensity;
    pureDensity.resize(densityData.size() * densityData[0].size());
    for( int x=0; x < densityData.size(); x++ ){
        for( int y=0; y < densityData[0].size(); y++ ){
            if(densityData.at(x).at(y).getMaxPopulation()==0){
                pureDensity[x*y]=0;
                
            }else{
                pureDensity[x*y]=densityData.at(x).at(y).getDensity();
            }
        }
    }

    //Age Probablities
    double ageProbablities [7]={ configuration["Age_5-Younger_Probablity"],
                                 configuration["Age_5-13_Probablity"],
                                 configuration["Age_14-17_Probablity"],
                                 configuration["Age_18-24_Probablity"],
                                 configuration["Age_25-44_Probablity"],
                                 configuration["Age_45-64_Probablity"],
                                 configuration["Age_65-Older_Probablity"]};
    
    //Family Size Probablties
    double familySizeProbablities [7]={ configuration["Family_Size_1_Probablity"],
                                        configuration["Family_Size_2_Probablity"],
                                        configuration["Family_Size_3_Probablity"],
                                        configuration["Family_Size_4_Probablity"],
                                        configuration["Family_Size_5_Probablity"],
                                        configuration["Family_Size_6_Probablity"],
                                        configuration["Family_Size_7_Probablity"]};
    
    double businessSizeProbablities[6]={ configuration["Business_Size_0-4_Probablity"],
                                         configuration["Business_Size_5-9_Probablity"],
                                         configuration["Business_Size_10-19_Probablity"],
                                         configuration["Business_Size_20-99_Probablity"],
                                         configuration["Business_Size_100-499_Probablity"],
                                         configuration["Business_Size_500_Probablity"]};
    
    double hospitalSizeProbablities[6]={ configuration["Hospital_Size_0-4_Probablity"],
                                         configuration["Hospital_Size_5-9_Probablity"],
                                         configuration["Hospital_Size_10-19_Probablity"],
                                         configuration["Hospital_Size_20-99_Probablity"],
                                         configuration["Hospital_Size_100-499_Probablity"],
                                         configuration["Hospital_Size_500_Probablity"]};
    
    double schoolSizeProbablities[6]={ configuration["School_Size_0-4_Probablity"],
                                       configuration["School_Size_5-9_Probablity"],
                                       configuration["School_Size_10-19_Probablity"],
                                       configuration["School_Size_20-99_Probablity"],
                                       configuration["School_Size_100-499_Probablity"],
                                       configuration["School_Size_500_Probablity"]};
   
    double daycareSizeProbablities[6]={ configuration["Daycare_Size_0-4_Probablity"],
                                        configuration["Daycare_Size_5-9_Probablity"],
                                        configuration["Daycare_Size_10-19_Probablity"],
                                        configuration["Daycare_Size_20-99_Probablity"],
                                        configuration["Daycare_Size_100-499_Probablity"],
                                        configuration["Daycare_Size_500_Probablity"]};
    
    double scheduleTypeProbablities[11]={ configuration["Schedule_Young_Children_5-Younger_Probablity"],
                                          configuration["Schedule_School_Children_5-13_Probablity"],
                                          configuration["Schedule_School_Children_14-17_Probablity"],
                                          configuration["Schedule_Employeed_Adults_18-24_Probablity"],
                                          configuration["Schedule_Unemployeed_Adults_18-24_Probablity"],
                                          configuration["Schedule_Employeed_Adults_25-44_Probablity"],
                                          configuration["Schedule_Unemployeed_Adults_25-44_Probablity"],
                                          configuration["Schedule_Employeed_Adults_45-64_Probablity"],
                                          configuration["Schedule_Unemployeed_Adults_45-64_Probablity"],
                                          configuration["Schedule_Employeed_Adults_65-Older_Probablity"],
                                          configuration["Schedule_Unemployeed_Adults_65-Older_Probablity"]};
        
    generator.seed(time(0));
        
    //Create Population
    Population pop = Population(configuration["Total_Population"], ageProbablities, familySizeProbablities, configuration["Male_Probablity"], scheduleTypeProbablities, progressDisplay, configuration["Population_Seed"]);
   
    //Assign Locations to Population
    assignHomes(pop);
    generateBuildings(businessSizeProbablities, hospitalSizeProbablities, schoolSizeProbablities, pureDensity, densityData.size(), pop.getNumberOfEmployeedAdults(), pop.getNumberOfStudentsPerGrade(), pop.getNumberOfChildrenDaycare());

        
    //Generate Schedules
    generateSchedules(pop);
    /*int allBuildings=0;
    for(int i=0;i<densityData.size();i++){
        for(int j=0;j<densityData[0].size();j++){
            allBuildings+=densityData.at(i).at(j).getNumberOfBuildings(NULL);
        }
    }*/
    if(produceImages){
        #ifdef HAVE_MAGICK
            ImageGen ig(outputFolder);
            ig.createPNGImage(densityData, densityData.size(),
                          densityData[0].size());
        #endif
        
        // Generate image in XFig file format. This should be an
        // option indicated by the user.

        std::vector< std::vector < int > > imageData;
        std::vector< std::vector < int > > buildingData;
        imageData.resize(densityData.size());
        buildingData.resize(densityData.size());
        for(int i = 0; i<densityData.size(); i++){
            imageData.at(i).resize(densityData[0].size());
            buildingData.at(i).resize(densityData[0].size());
        }
        for( int x=0; x < densityData.size(); x++ ){
            for( int y=0; y < densityData[0].size(); y++ ){
                if(densityData.at(x).at(y).getMaxPopulation()==0){
                    imageData.at(x).at(y)=-1;
                    buildingData.at(x).at(y)=-1;


                }else{
                    imageData.at(x).at(y)=densityData.at(x).at(y).getCurrentPopulation();
                    buildingData.at(x).at(y)=densityData.at(x).at(y).getNumberOfBuildings(NULL);

                }
            }
        }
        XFigImageGenerator xfig;
        std::cout<<"Generating Population Image" << std::endl;
        xfig.createImage(outputFolder + "haplos_population.fig", imageData,
                 imageData.size(), imageData[0].size(), configuration["Max_Population_In_Area"]);
        
        std::cout<<"Generating Building Image" << std::endl;
        xfig.createImage(outputFolder + "haplos_building.fig", buildingData,
                         buildingData.size(), buildingData[0].size(), 100);
    }
    
    //Create Output Directory
    mkdir("output", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer [80];
    strftime(buffer,80,"output/haplos_%Y%m%d_%H%M%S",ltm);
    std::string saveLocationPath(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    strftime(buffer,80,"output/haplos_%Y%m%d_%H%M%S/imageFiles",ltm);
    std::string imageFileLocationPath(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    strftime(buffer,80,"output/haplos_%Y%m%d_%H%M%S/familyData",ltm);
    std::string familyFileLocationPath(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    //Create Files
    pop.displayStatistics(saveLocationPath);
    displayBuildingStatistics(businessSizeProbablities, hospitalSizeProbablities, schoolSizeProbablities, daycareSizeProbablities, saveLocationPath);
    
    //Display 10 Families Entirely (Useful for Schedule Testing).
    pop.returnFirstTenFamiliesInfo(familyFileLocationPath);
    
    //Generate Sample Image Generation Files
    ImageFileGenerator imgGen = ImageFileGenerator(&densityData, imageFileLocationPath);
    imgGen.makeBuildingFile("building_All.hapi", NULL);
    imgGen.makeBuildingFile("building_Daycares.hapi", 'D');
    imgGen.makeBuildingFile("building_Schools.hapi", 'S');
    imgGen.makeBuildingFile("building_Businesss.hapi", 'B');
    imgGen.makeBuildingFile("building_Homes.hapi", 'H');
    imgGen.makeBuildingFile("building_Medical.hapi", 'M');

    imgGen.makePopFile("pop.hapi");
    
    return 0;
}



#endif

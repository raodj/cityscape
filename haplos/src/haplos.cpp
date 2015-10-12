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
#include "Files/TimelineFile.h"
#include "Files/ImageFileGenerator.h"
#include "Schedule/ScheduleGenerator.h"
#include "Buildings/BuildingGenerator.h"
#include "Policy.h"


#include "Buildings/Medical.h"
#include "Buildings/School.h"
#include "Buildings/Business.h"
#include "Buildings/Daycare.h"

using namespace std;

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


int main(int argc, char* argv[]) {
    //Display Awesome Logo
    std::cout << " _    _          _____  _      ____   _____" <<std::endl;
    std::cout << "| |  | |   /\\   |  __ \\| |    / __ \\ / ____|" <<std::endl;
    std::cout << "| |__| |  /  \\  | |__) | |   | |  | | (___" << std::endl;
    std::cout << "|  __  | / /\\ \\ |  ___/| |   | |  | |\\___ \\" << std::endl;
    std::cout << "| |  | |/ ____ \\| |    | |___| |__| |____) |" << std::endl;
    std::cout << "|_|  |_/_/    \\_\\_|    |______\\____/|_____/" <<std::endl;
    
    //Create Policy Object
    Policy policy =  Policy();
    
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
    std::string outputFolder = configuration.getOutputFileLocation();

    //Read in Density Data
    SedacReader sr = SedacReader();
    densityData = sr.readFile(configuration.getSedacFileLocation(),
                              configuration["Density_File_Total_Population"],
                              configuration["Total_Population"]);
    
    //Read in Timeline File
    TimelineFile tl = TimelineFile(configuration.getTimelineFileLocation(), configuration.getCustomFileTypes());
    
    //Extract out Pure Density Data
    std::vector<double> pureDensity;
    pureDensity.resize(densityData.size() * densityData[0].size());
    for( int x=0; x < densityData.size(); x++ ){
        for( int y=0; y < densityData[0].size(); y++ ){
            densityData.at(x).at(y).getTransportHub()->setID(numberOfBuildings);
            allBuildings[numberOfBuildings] = densityData.at(x).at(y).getTransportHub();
            numberOfBuildings++;
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
    double familySizeProbablities [7] = { configuration["Family_Size_1_Probablity"],
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
    
    double hospitalSizeProbablities[6] = { configuration["Hospital_Size_0-4_Probablity"],
                                         configuration["Hospital_Size_5-9_Probablity"],
                                         configuration["Hospital_Size_10-19_Probablity"],
                                         configuration["Hospital_Size_20-99_Probablity"],
                                         configuration["Hospital_Size_100-499_Probablity"],
                                         configuration["Hospital_Size_500_Probablity"]};
    
    double schoolSizeProbablities[6] = { configuration["School_Size_0-4_Probablity"],
                                       configuration["School_Size_5-9_Probablity"],
                                       configuration["School_Size_10-19_Probablity"],
                                       configuration["School_Size_20-99_Probablity"],
                                       configuration["School_Size_100-499_Probablity"],
                                       configuration["School_Size_500_Probablity"]};
   
    double daycareSizeProbablities[6] = { configuration["Daycare_Size_0-4_Probablity"],
                                        configuration["Daycare_Size_5-9_Probablity"],
                                        configuration["Daycare_Size_10-19_Probablity"],
                                        configuration["Daycare_Size_20-99_Probablity"],
                                        configuration["Daycare_Size_100-499_Probablity"],
                                        configuration["Daycare_Size_500_Probablity"]};
    
    double scheduleTypeProbablities[11] = { configuration["Schedule_Young_Children_5-Younger_Probablity"],
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
    
    int radiusLimits[6] = { (int)configuration["Schedule_Young_Children_Radius_Limit"],
                            (int)configuration["Schedule_School_Children_5-13_Radius_Limit"],
                            (int)configuration["Schedule_School_Children_14-15_Radius_Limit"],
                            (int)configuration["Schedule_School_Children_14-15_Radius_Limit"],
                            (int)configuration["Schedule_Employeed_Radius_Limit"],
                            (int)configuration["Schedule_Unemployeed_Radius_Limit"]};
    
    double transportProbablities[3] = { configuration["Public_Transport_Percentage"],
                                        configuration["Private_Transport_Percentage"],
                                        configuration["Walking_Transport_Percentage"]};
    
    int transportRate[3] = { (int)configuration["Public_Transport_Rate"],
                              (int)configuration["Private_Transport_Rate"],
                              (int)configuration["Walking_Transport_Rate"]};
    
    int transportRadius[2] = { (int)configuration["Max_Public_Transport_Distance"],
                               (int)configuration["Max_Walking_Distance"]};
    
    double adultWorkVisitorProbablities[3] = { configuration["Schedule_Adult_Employeed_Work_Job_Probablity"],
                                            configuration["Schedule_Adult_Employeed_Work_Out_Probablity"],
                                            configuration["Schedule_Adult_Employeed_Work_Home_Probablity"]};
    
    double adultNoWorkVisitorProbablities[3] = { configuration["Schedule_Adult_Employeed_No_Work_Job_Probablity"],
                                              configuration["Schedule_Adult_Employeed_No_Work_Out_Probablity"],
                                              configuration["Schedule_Adult_Employeed_No_Work_Home_Probablity"]};
    
    double adultUnemployeedVisitorProbablities[3]  = { configuration["Schedule_Adult_UnEmployeed_Job_Probablity"],
                                                       configuration["Schedule_Adult_UnEmployeed_Out_Probablity"],
                                                       configuration["Schedule_Adult_UnEmployeed_Probablity"]};
    
    double olderSchoolSchoolDayProbablities[3] = { configuration["Schedule_Older_School_School_Day_Job_Probablity"],
                                                configuration["Schedule_Older_School_School_Day_Out_Probablity"],
                                                configuration["Schedule_Older_School_School_Day_Home_Probablity"]};
    
    double olderSchoolWeekendProbablities[3] = { configuration["Schedule_Older_School_Weekend_Job_Probablity"],
                                              configuration["Schedule_Older_School_Weekend_Out_Probablity"],
                                              configuration["Schedule_Older_School_Weekend_Home_Probablity"]};
                      


    //Set Seed
    generator.seed(time(0));

    //Create Population
    Population pop = Population(configuration["Total_Population"],
                                ageProbablities,
                                familySizeProbablities,
                                configuration["Male_Probablity"],
                                scheduleTypeProbablities,
                                progressDisplay,
                                configuration["Population_Seed"]);
   
    //Assign Locations to Population
    
    BuildingGenerator buildingGen = BuildingGenerator(generator,
                                                      numberOfBuildings,
                                                      &schoolBuildings,
                                                      &daycareBuildings,
                                                      &medicalBuildings,
                                                      &businessBuildings,
                                                      &otherBuildings,
                                                      &allBuildings,
                                                      &densityData,
                                                      progressDisplay);
    
    buildingGen.assignHomes(pop);
    buildingGen.generateBuildings(businessSizeProbablities,
                                  hospitalSizeProbablities,
                                  schoolSizeProbablities,
                                  pureDensity,
                                  densityData.size(),
                                  pop.getNumberOfEmployeedAdults(),
                                  pop.getNumberOfStudentsPerGrade(),
                                  pop.getNumberOfChildrenDaycare());

    //Generate Schedules
    ScheduleGenerator scheduleGen = ScheduleGenerator (&densityData,
                                                       &allBuildings,
                                                       generator,
                                                       progressDisplay);
    scheduleGen.generateSchedules(pop,
                                  radiusLimits,
                                  transportProbablities,
                                  transportRadius,
                                  transportRate,
                                  olderSchoolSchoolDayProbablities,
                                  olderSchoolWeekendProbablities,
                                  adultWorkVisitorProbablities,
                                  adultNoWorkVisitorProbablities,
                                  adultUnemployeedVisitorProbablities);
    
    policy.setupCustomAttributes(&pop);
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
                    buildingData.at(x).at(y)=densityData.at(x).at(y).getNumberOfBuildings('\0');

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
    
    std::cout<<"Output Folder: "<< outputFolder<<std::endl;
    std::string temp =outputFolder+"haplos_%Y%m%d_%H%M%S";
    //Create Output Directory
    char *outputPath = (char*)((temp).c_str());
    std::string temp2 =outputFolder+"haplos_%Y%m%d_%H%M%S/imageFiles";

    char *imageOutputPath = (char*)((temp2).c_str());
    std::string temp3 =outputFolder+"haplos_%Y%m%d_%H%M%S/familyData";
    char *familyDataPath = (char*)((temp3).c_str());
    //mkdir("output", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer [300];
    strftime(buffer,300,outputPath,ltm);
    std::string saveLocationPath(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    strftime(buffer,300,imageOutputPath,ltm);
    std::string imageFileLocationPath(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    strftime(buffer,300,familyDataPath,ltm);
    std::string familyFileLocationPath(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    //Create Files
    pop.displayStatistics(saveLocationPath);
    buildingGen.displayBuildingStatistics(businessSizeProbablities,
                              hospitalSizeProbablities,
                              schoolSizeProbablities,
                              daycareSizeProbablities,
                              saveLocationPath);
    
    //Display 10 Families Entirely (Useful for Schedule Testing).
    pop.returnFirstTenFamiliesInfo(familyFileLocationPath);
    vector<std::string> headerInformation;


    
    headerInformation.push_back(std::to_string(configuration["Lower_Left_Longitude"])+","
                                +std::to_string(configuration["Lower_Left_Laditude"]));
    headerInformation.push_back(std::to_string(configuration["Cellsize_Width"])+","
                           +std::to_string(configuration["Cellsize_Height"]));
    
    int currentTime = 0;
    std::cout<<"Simulation Running"<<std::endl;
    while(currentTime<configuration["Length_Of_Simulation"]){
      //  std::cout<<"Current Time: "<<currentTime<<std::endl;
        //Update Population
        pop.updateToNextTimeStep(&allBuildings);
        policy.updatePopulation(&pop, &allBuildings, currentTime);
        //Generate Any Images Needed
        std::vector<std::string> files =tl.getFilesToProduceAt(currentTime);
        if(!files.empty()){
            ImageFileGenerator imgGen = ImageFileGenerator(&densityData, imageFileLocationPath);
            for(std::vector<std::string>::iterator i = files.begin(); i!= files.end(); i++){
                char type = '\0';
                bool customFile = false;
                if(std::strcmp(i->c_str(), "population_density") != 0){
                    if(std::strcmp(i->c_str(), "all_buildings") == 0){
                        type='\0';
                    }else{
                        if(std::strcmp(i->c_str(), "medical_buildings") == 0){
                            type='M';
                        }else{
                            if(std::strcmp(i->c_str(), "business_buildings") == 0){
                                type='B';
                            }else{
                                if(std::strcmp(i->c_str(), "school_buildings") == 0){
                                    type='S';
                                }else{
                                    if(std::strcmp(i->c_str(), "home_buildings") == 0){
                                        type='H';
                                    }else{
                                        customFile =true;
                                    }
                                }
                            }
                        }
                    }
                    if(!customFile){
                        imgGen.makeBuildingFile(i->c_str(), i->append("_"+std::to_string(currentTime)+".hapi"), type, headerInformation);
                    }else{
                        imgGen.makeCustomFile(i->c_str(), i->append("_"+std::to_string(currentTime)+".hapi"), headerInformation,
                                              &policy);
                    }

                }
                   else{
                       imgGen.makePopFile(i->c_str(), i->append("_"+std::to_string(currentTime)+".hapi"), headerInformation);

                   }

            }
        }
        
        

        currentTime++;
    }
    //Generate Sample Image Generation Files
    //std::cout<<"Generating Image Files."<<std::endl;
    /*ImageFileGenerator imgGen = ImageFileGenerator(&densityData, imageFileLocationPath);
    imgGen.makeBuildingFile("building_All.hapi", '\0', headerInformation);
    imgGen.makeBuildingFile("building_Daycares.hapi", 'D', headerInformation);
    imgGen.makeBuildingFile("building_Schools.hapi", 'S', headerInformation);
    imgGen.makeBuildingFile("building_Businesss.hapi", 'B', headerInformation);
    imgGen.makeBuildingFile("building_Homes.hapi", 'H', headerInformation);
    imgGen.makeBuildingFile("building_Medical.hapi", 'M', headerInformation);

    imgGen.makePopFile("pop.hapi", headerInformation);
    */
    std::ofstream completeFile;
    completeFile.open(outputFolder+"/complete.txt");
    completeFile.close();

    std::cout<<"Simulation Complete"<<std::endl;
    
    return 0;
}



#endif

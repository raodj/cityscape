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
#include "Haplos.h"


#include "Buildings/Medical.h"
#include "Buildings/School.h"
#include "Buildings/Business.h"
#include "Buildings/Daycare.h"

using namespace std;

Haplos::Haplos(std::string configFileLocation, bool produceImages, bool progressDisplay, bool exportFiles){
    //Display Awesome Logo
    std::cout << " _    _          _____  _      ____   _____" <<std::endl;
    std::cout << "| |  | |   /\\   |  __ \\| |    / __ \\ / ____|" <<std::endl;
    std::cout << "| |__| |  /  \\  | |__) | |   | |  | | (___" << std::endl;
    std::cout << "|  __  | / /\\ \\ |  ___/| |   | |  | |\\___ \\" << std::endl;
    std::cout << "| |  | |/ ____ \\| |    | |___| |__| |____) |" << std::endl;
    std::cout << "|_|  |_/_/    \\_\\_|    |______\\____/|_____/" <<std::endl;
    
    this->produceImages=produceImages;
    this->progressDisplay=progressDisplay;
    this->exportFiles = exportFiles;
    
    //Read in Configuration File
    configuration= ConfigFile(configFileLocation);
    outputFolder = configuration.getOutputFileLocation();
}
void Haplos::runSimulation(Policy *p){
    
    //Age Probablities
    double *ageProbablities =configuration.getAgeProbablities();
    
    //Family Size Probablties
    double *familySizeProbablities = configuration.getFamilySizeProbablities();
    
    //Building Probablities
    double *businessSizeProbablities = configuration.getBusinessSizeProbablities();
    double *hospitalSizeProbablities = configuration.getHospitalSizeProbablities();
    double *schoolSizeProbablities = configuration.getSchoolSizeProbablities();
    double *daycareSizeProbablities = configuration.getDaycareSizeProbablities();
    
    //Schedule Probablities
    double *scheduleTypeProbablities = configuration.getScheduleTypeProbablities();
    
    //Transportation Probablities
    int *radiusLimits = configuration.getRadiusLimits();
    double *transportProbablities = configuration.getTransportProbablities();
    int *transportRate = configuration.getTransportRate();
    int *transportRadius = configuration.getTransportRadius();
    
    //Visitation Probablities
    double *adultWorkVisitorProbablities = configuration.getAdultWorkVisitorProbablities();
    double *adultNoWorkVisitorProbablities = configuration.getAdultNoWorkVisitorProbablities();
    double *adultUnemployeedVisitorProbablities = configuration.getAdultUnemployeedVisitorProbablities();
    double *olderSchoolSchoolDayProbablities = configuration.getOlderSchoolSchoolDayProbablities();
    double *olderSchoolWeekendProbablities = configuration.getOlderSchoolWeekendProbablities();
    
    //Set Seed
    generator.seed(time(0));
    
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
   Population pop = Population(configuration["Total_Population"],
                                ageProbablities,
                                familySizeProbablities,
                                configuration["Male_Probablity"],
                                scheduleTypeProbablities,
                                configuration["Population_Seed"]);
    
    
    //Generate Schedules
    ScheduleGenerator scheduleGen = ScheduleGenerator (&densityData,
                                     &allBuildings,
                                     generator,
                                     progressDisplay);
    
    //Create Policy Object
    p->setConfigFile(&configuration);
    
    std::cout<<"Population Import File: "<<configuration.getPopulationImport()<<std::endl;
    std::cout<<"Get Building Import File: "<<configuration.getBuildingImport()<<std::endl;
    if(configuration.getPopulationImport() ==  "-1" || configuration.getBuildingImport() == "-1"){
        //Create Population
        pop.generatePopulation(progressDisplay);
        
        //Assign Locations to Population
        buildingGen.assignHomes(pop);
        buildingGen.generateBuildings(businessSizeProbablities,
                                      hospitalSizeProbablities,
                                      schoolSizeProbablities,
                                      pureDensity,
                                      densityData.size(),
                                      pop.getNumberOfEmployeedAdults(),
                                      pop.getNumberOfStudentsPerGrade(),
                                      pop.getNumberOfChildrenDaycare());

      
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
        
    }else{
        buildingGen.importBuildings(configuration.getBuildingImport());
        pop.importPopulation(configuration.getPopulationImport(), &allBuildings);
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
    std::string saveLocationPath_tmp(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    strftime(buffer,300,imageOutputPath,ltm);
    std::string imageFileLocationPath_tmp(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    strftime(buffer,300,familyDataPath,ltm);
    std::string familyFileLocationPath_tmp(buffer);
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    
    saveLocationPath=saveLocationPath_tmp;
    imageFileLocationPath=imageFileLocationPath_tmp;
    familyFileLocationPath=familyFileLocationPath_tmp;
    //Create Files
    pop.displayStatistics(saveLocationPath);
    if(exportFiles){
        pop.exportPopulation(saveLocationPath);
        buildingGen.exportBuildings(saveLocationPath);
    }
    buildingGen.displayBuildingStatistics(businessSizeProbablities,
                                          hospitalSizeProbablities,
                                          schoolSizeProbablities,
                                          daycareSizeProbablities,
                                          saveLocationPath);
    
    //Display 10 Families Entirely (Useful for Schedule Testing).
    //pop.returnFirstTenFamiliesInfo(familyFileLocationPath);

    policy.setupCustomAttributes(pop);

    //Prepare Headers for Image Generator Files
    vector<std::string> headerInformation;
    headerInformation.push_back(std::to_string(configuration["Lower_Left_Longitude"])+","
                                +std::to_string(configuration["Lower_Left_Laditude"]));
    headerInformation.push_back(std::to_string(configuration["Cellsize_Width"])+","
                                +std::to_string(configuration["Cellsize_Height"]));

    /*for(auto it = schoolBuildings.begin(); it != schoolBuildings.end(); it++){
        std::cout<<"Address of School Building: "<<&(*it)<<std::endl;
        std::cout<<"Address of All Building: "<<&(*(allBuildings[it->getID()]))<<std::endl;
    }*/
    
    int currentTime = 0;
    std::cout<<"Simulation Running"<<std::endl;
    while(currentTime<configuration["Length_Of_Simulation"]){
        //  std::cout<<"Current Time: "<<currentTime<<std::endl;
        //Update Population
        pop.updateToNextTimeStep(&allBuildings);
        p->updatePopulation(pop, allBuildings, currentTime, &scheduleGen);
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
        if(currentTime%144 == 0){
            std::cout<<"Day: "<<(currentTime/144)<<std::endl;
        }
    }
    
    std::ofstream completeFile;
    completeFile.open(outputFolder+"/complete.txt");
    completeFile.close();
    
    std::cout<<"Simulation Complete"<<std::endl;

}


#endif

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

#include "SedacReader.h"
#include "ImageGeneration/ImageGen.h"
#include "Location.h"
#include "Population.h"
#include "ImageGeneration/XFigImageGenerator.h"
#include "ConfigFile.h"
#include "Buildings/Medical.h"
#include "Buildings/School.h"
#include "Buildings/Business.h"
using namespace std;

std::string const outputFolder="output/";
std::string const configFile="examples/config/USAData.hapl";
//std::string const configFile="examples/config/MicroWorldData.hapl";
std::vector< std::vector < Location > > densityData;
std::default_random_engine generator;
ConfigFile configuration;

int totalBusinessSize[6];
int totalHospitalSize[6];
int totalSchoolSize[6];
int totalSchools=0;
int totalHospitals=0;
int totalBusinesses=0;
int numberOfBuildings=0;

void assignHomes(Population &pop){
    //Assign Families Locations
    int x=0;
    int y=0;
    int notAssigned=0;
    float oldRatio=0;
    std::cout << "Assigning Locations to Population" << std::endl;
    printf("Percentage Complete: %3d%%", 0 );
    fflush(stdout);
    
    for ( int i =0; i<pop.getNumberOfFamilies(); i++ ) {
        while (densityData.at(x).at(y).isFull()) {
            //Move to Next Location in column
            x++;
            if(x>=densityData.size()&&y<densityData[0].size()){
                //Move to next Row
                x=0;
                y++;
            }
            else{
                if (y>=densityData[0].size()-1) {
                    //No More Locations Avaliable
                    notAssigned++;
                    break;
                }
            }
        }
        
        if (y>=densityData[0].size()-1) {
            //No More Locations Avaliable
            break;
        }
        //Set Location of Family
        Building home= Building('H', numberOfBuildings, x, y, pop.getFamily(i)->getNumberOfPeople());
        pop.setLocationOfFamily(x, y, i);
        densityData.at(x).at(y).addFamily(pop.getFamily(i));
        densityData.at(x).at(y).addBuilding(&home);
        numberOfBuildings++;
        
        //Calculate Percent Complete
        float ratio = i/(float)pop.getNumberOfFamilies();
        
        if ( 100*(ratio-oldRatio) > 1 ) {
            //Update Percent Complete Only if there is a Change
            printf("\r");
            printf("Percentage Complete: %3d%%", (int)(ratio*100) );
            oldRatio=ratio;
            fflush(stdout);
        }
    }
    //Print out 100% Complete
    printf("\r");
    printf("Percentage Complete: %3d%%", 100 );
    fflush(stdout);
    
    std::cout << std::endl <<"Population Successfully Assigned Locations" << std::endl;
    
}

void placeLocation(){
    
}

void generateBuildings(double businessSizeProbablities[6], double hospitalSizeProbablities[6], double schoolSizeProbablities[6], std::vector<double> locationProbablties, int const width, int const numberOfEmployeedAdults){
    //Set Random Generator Seed
    generator.seed(time(0));
    //Set Distribution
    std::discrete_distribution<int> businessSizeDistribution{ businessSizeProbablities[0],
                                                  businessSizeProbablities[1],
                                                  businessSizeProbablities[2],
                                                  businessSizeProbablities[3],
                                                  businessSizeProbablities[4],
                                                  businessSizeProbablities[5]};
    std::discrete_distribution<int> locationDistribution(locationProbablties.begin(), locationProbablties.end());
    
    //Set Tailies for Print out
    int totalBuinessPopulation=0;
    while(numberOfEmployeedAdults>totalBuinessPopulation){
        int capacity = -1;
        bool hospital=false;
        bool school=false;
        switch(businessSizeDistribution(generator)){
            case 0:{
                std::discrete_distribution<int> hospitalSizeDistribution0{1-hospitalSizeProbablities[0],hospitalSizeProbablities[0]};
                capacity = (int)rand() % 4 + 1;
                if(hospitalSizeDistribution0(generator)){
                    totalHospitals++;
                    totalHospitalSize[0]++;
                    hospital=true;
                }
                else{
                    std::discrete_distribution<int> schoolSizeDistribution0{ 1-schoolSizeProbablities[0], schoolSizeProbablities[0]};
                    if(schoolSizeDistribution0(generator)){
                        totalSchools++;
                        totalSchoolSize[0]++;
                        school=true;
                    }else{
                        totalBusinesses++;
                        totalBusinessSize[0]++;
                    }
                }
                break;
            }
            case 1:{
                std::discrete_distribution<int> hospitalSizeDistribution1{1-hospitalSizeProbablities[1],hospitalSizeProbablities[1]};
                capacity = (int)rand() % 4 + 5;
                if(hospitalSizeDistribution1(generator)){
                    totalHospitals++;
                    totalHospitalSize[1]++;
                    hospital=true;
                }
                else{
                    std::discrete_distribution<int> schoolSizeDistribution1{1-schoolSizeProbablities[1],schoolSizeProbablities[1]};
                    if(schoolSizeDistribution1(generator)){
                        totalSchools++;
                        totalSchoolSize[1]++;
                        school=true;
                    }else{
                        totalBusinesses++;
                        totalBusinessSize[1]++;
                    }
                }
                break;
            }
            case 2:{
                std::discrete_distribution<int> hospitalSizeDistribution2{1-hospitalSizeProbablities[2],hospitalSizeProbablities[2]};
                capacity = (int)rand() % 9 + 10;
                if(hospitalSizeDistribution2(generator)){
                    totalHospitals++;
                    totalHospitalSize[2]++;
                    hospital=true;
                }
                else{
                    std::discrete_distribution<int> schoolSizeDistribution2{1-schoolSizeProbablities[2],schoolSizeProbablities[2]};
                    if(schoolSizeDistribution2(generator)){
                        totalSchools++;
                        totalSchoolSize[2]++;
                        school=true;
                    }else{
                        totalBusinesses++;
                        totalBusinessSize[2]++;
                    }
                }
                break;
            }
            case 3:{
                std::discrete_distribution<int> hospitalSizeDistribution3{1-hospitalSizeProbablities[3],hospitalSizeProbablities[3]};
                capacity = (int)rand() % 79 + 20;
                if(hospitalSizeDistribution3(generator)){
                    totalHospitals++;
                    totalHospitalSize[3]++;
                    hospital=true;
                }
                else{
                    std::discrete_distribution<int> schoolSizeDistribution3{1-schoolSizeProbablities[3],schoolSizeProbablities[3]};
                    if(schoolSizeDistribution3(generator)){
                        totalSchools++;
                        totalSchoolSize[3]++;
                        school=true;
                    }else{
                        totalBusinesses++;
                        totalBusinessSize[3]++;
                    }
                }
                break;
            }
            case 4:{
                std::discrete_distribution<int> hospitalSizeDistribution4{1-hospitalSizeProbablities[4],hospitalSizeProbablities[4]};
                capacity = (int)rand() % 399 + 100;
                if(hospitalSizeDistribution4(generator)){
                    totalHospitals++;
                    totalHospitalSize[4]++;
                    hospital=true;
                }
                else{
                    std::discrete_distribution<int> schoolSizeDistribution4{1-schoolSizeProbablities[4],schoolSizeProbablities[4]};
                    if(schoolSizeDistribution4(generator)){
                        totalSchools++;
                        totalSchoolSize[4]++;
                        school=true;
                    }else{
                        totalBusinesses++;
                        totalBusinessSize[4]++;
                    }
                }
                break;
            }
            case 5:{
                std::discrete_distribution<int> hospitalSizeDistribution5{1-hospitalSizeProbablities[5],hospitalSizeProbablities[5]};
                capacity = 500;
                if(hospitalSizeDistribution5(generator)){
                    totalHospitals++;
                    totalHospitalSize[5]++;
                    hospital=true;
                }
                else{
                    std::discrete_distribution<int> schoolSizeDistribution5{1-schoolSizeProbablities[5],schoolSizeProbablities[5]};
                    if(schoolSizeDistribution5(generator)){
                        totalSchools++;
                        totalSchoolSize[5]++;
                        school=true;
                    }else{
                        totalBusinesses++;
                        totalBusinessSize[5]++;
                    }
                }
                break;
            }
        }
        int location = locationDistribution(generator);
        Building newBuilding;
        if(school){
            newBuilding = School(numberOfBuildings, location%width, location/width, capacity, 0, NULL);
        }else{
            if(hospital){
                newBuilding = Medical(numberOfBuildings, location%width, location/width, capacity, 0, 0);
            }else{
                newBuilding=Business(numberOfBuildings,location%width, location/width, capacity, 0);
            }
        }
        densityData.at(location%width).at(location/width).addBuilding(&newBuilding);
        totalBuinessPopulation+=capacity;
        numberOfBuildings++;
    }
}

void displayBuildingStatistics(double businessSizeProbablities[6], double hospitalSizeProbablities[6], double schoolSizeProbablities[6]){
    std::cout << "--------Buildings--------" << std::endl;
    std::cout << "Total Buildings Generated: " << numberOfBuildings << std::endl;
    std::cout << "----Businesses----" << std::endl;
    std::cout <<"Total Generated: " << totalBusinesses << std::endl;
	std::cout << "Employee Capacity 1-4:     \t" << totalBusinessSize[0] << " \t"<<(totalBusinessSize[0]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[0] << ")" << std::endl;
    std::cout << "Employee Capacity 5-9:     \t" << totalBusinessSize[1] << " \t"<<(totalBusinessSize[1]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[1] << ")" << std::endl;
    std::cout << "Employee Capacity 10-19:   \t" << totalBusinessSize[2] << " \t"<<(totalBusinessSize[2]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[2] << ")" << std::endl;
    std::cout << "Employee Capacity 20-99:   \t" << totalBusinessSize[3] << " \t"<<(totalBusinessSize[3]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[3] << ")" << std::endl;
    std::cout << "Employee Capacity 100-499: \t" << totalBusinessSize[4] << " \t"<<(totalBusinessSize[4]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[4] << ")" << std::endl;
    std::cout << "Employee Capacity 500:     \t" << totalBusinessSize[5] << " \t"<<(totalBusinessSize[5]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[5] << ")" << std::endl;
    
    std::cout << "----Hospitals----" << std::endl;
    std::cout <<"Total Generated: " << totalHospitals << std::endl;
	std::cout << "Employee Capacity 1-4:     \t" << totalHospitalSize[0] << " \t"<<(totalHospitalSize[0]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[0] << ")" << std::endl;
    std::cout << "Employee Capacity 5-9:     \t" << totalHospitalSize[1] << " \t"<<(totalHospitalSize[1]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[1] << ")" << std::endl;
    std::cout << "Employee Capacity 10-19:   \t" << totalHospitalSize[2] << " \t"<<(totalHospitalSize[2]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[2] << ")" << std::endl;
    std::cout << "Employee Capacity 20-99:   \t" << totalHospitalSize[3] << " \t"<<(totalHospitalSize[3]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[3] << ")" << std::endl;
    std::cout << "Employee Capacity 100-499: \t" << totalHospitalSize[4] << " \t"<<(totalHospitalSize[4]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[4] << ")" << std::endl;
    std::cout << "Employee Capacity 500:     \t" << totalHospitalSize[5] << " \t"<<(totalHospitalSize[5]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[5] << ")" << std::endl;
    
    std::cout << "----Schools----" << std::endl;
    std::cout <<"Total Generated: " << totalSchools << std::endl;
	std::cout << "Employee Capacity 1-4:     \t" << totalSchoolSize[0] << " \t"<<(totalSchoolSize[0]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[0] << ")" << std::endl;
    std::cout << "Employee Capacity 5-9:     \t" << totalSchoolSize[1] << " \t"<<(totalSchoolSize[1]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[1] << ")" << std::endl;
    std::cout << "Employee Capacity 10-19:   \t" << totalSchoolSize[2] << " \t"<<(totalSchoolSize[2]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[2] << ")" << std::endl;
    std::cout << "Employee Capacity 20-99:   \t" << totalSchoolSize[3] << " \t"<<(totalSchoolSize[3]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[3] << ")" << std::endl;
    std::cout << "Employee Capacity 100-499: \t" << totalSchoolSize[4] << " \t"<<(totalSchoolSize[4]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[4] << ")" << std::endl;
    std::cout << "Employee Capacity 500:     \t" << totalSchoolSize[5] << " \t"<<(totalSchoolSize[5]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[5] << ")" << std::endl;
}

int main() {
    //Display Awesome Logo
    std::cout << " _    _          _____  _      ____   _____" <<std::endl;
    std::cout << "| |  | |   /\\   |  __ \\| |    / __ \\ / ____|" <<std::endl;
    std::cout << "| |__| |  /  \\  | |__) | |   | |  | | (___" << std::endl;
    std::cout << "|  __  | / /\\ \\ |  ___/| |   | |  | |\\___ \\" << std::endl;
    std::cout << "| |  | |/ ____ \\| |    | |___| |__| |____) |" << std::endl;
    std::cout << "|_|  |_/_/    \\_\\_|    |______\\____/|_____/" <<std::endl;
    
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
        
        
        
    //Create Population
    Population pop = Population(configuration["Total_Population"], ageProbablities, familySizeProbablities, configuration["Male_Probablity"], scheduleTypeProbablities);
   
    //Assign Locations to Population
    assignHomes(pop);
    generateBuildings(businessSizeProbablities, hospitalSizeProbablities, schoolSizeProbablities, pureDensity, densityData.size(), pop.getNumberOfEmployeedAdults());

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
                buildingData.at(x).at(y)=densityData.at(x).at(y).getNumberOfBuildings();

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
    pop.displayStatistics();
    displayBuildingStatistics(businessSizeProbablities, hospitalSizeProbablities, schoolSizeProbablities);
    return 0;
}



#endif

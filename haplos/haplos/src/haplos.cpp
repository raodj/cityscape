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
#include "Buildings/Business.h"
using namespace std;

std::string const outputFolder="output/";
std::string const configFile="examples/config/USAData.hapl";
//std::string const configFile="examples/config/MicroWorldData.hapl";
std::vector< std::vector < Location > > densityData;
std::default_random_engine generator;
ConfigFile configuration;

int totalBusinessSize0=0;
int totalBusinessSize1=0;
int totalBusinessSize2=0;
int totalBusinessSize3=0;
int totalBusinessSize4=0;
int totalBusinessSize5=0;
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

void generateBuildings(double businessSizeProbablities[6], std::vector<double> locationProbablties, int const width, int const numberOfEmployeedAdults){
    //Set Random Generator Seed
    generator.seed(time(0));
    //Set Distribution
    std::discrete_distribution<int> sizeDistribution{ businessSizeProbablities[0],
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
        switch(sizeDistribution(generator)){
            case 0:
                capacity = (int)rand() % 4 + 1;
                totalBusinessSize0++;
                break;
            case 1:
                capacity = (int)rand() % 4 + 5;
                totalBusinessSize1++;
                break;
            case 2:
                capacity = (int)rand() % 9 + 10;
                totalBusinessSize2++;
                break;
            case 3:
                capacity = (int)rand() % 79 + 20;
                totalBusinessSize3++;
                break;
            case 4:
                capacity = (int)rand() % 399 + 100;
                totalBusinessSize4++;
                break;
            case 5:
                totalBusinessSize5++;
                capacity = 500;
                break;
        }
        int location = locationDistribution(generator);
        Business newBusiness= Business(numberOfBuildings,location%width, location/width, capacity, 0);
        densityData.at(location%width).at(location/width).addBuilding(&newBusiness);
        totalBuinessPopulation+=capacity;
        totalBusinesses++;
        numberOfBuildings++;
    }
}

void displayBuildingStatistics(double businessSizeProbablities[6]){
    std::cout << "--------Buildings--------" << std::endl;
    std::cout << "----Businesses----" << std::endl;
    std::cout <<"Total Generated: " << totalBusinesses << std::endl;
	std::cout << "Employee Capacity 1-4:     \t" << totalBusinessSize0 << " \t"<<(totalBusinessSize0/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[0] << ")" << std::endl;
    std::cout << "Employee Capacity 5-9:     \t" << totalBusinessSize1 << " \t"<<(totalBusinessSize1/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[1] << ")" << std::endl;
    std::cout << "Employee Capacity 10-19:   \t" << totalBusinessSize2 << " \t"<<(totalBusinessSize2/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[2] << ")" << std::endl;
    std::cout << "Employee Capacity 20-99:   \t" << totalBusinessSize3 << " \t"<<(totalBusinessSize3/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[3] << ")" << std::endl;
    std::cout << "Employee Capacity 100-499: \t" << totalBusinessSize4 << " \t"<<(totalBusinessSize4/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[4] << ")" << std::endl;
    std::cout << "Employee Capacity 500:     \t" << totalBusinessSize5 << " \t"<<(totalBusinessSize5/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[5] << ")" << std::endl;
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
    generateBuildings(businessSizeProbablities, pureDensity, densityData.size(), pop.getNumberOfEmployeedAdults());

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
    displayBuildingStatistics(businessSizeProbablities);
    return 0;
}



#endif

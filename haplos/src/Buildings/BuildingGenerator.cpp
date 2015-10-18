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

#include "BuildingGenerator.h"
#include "Building.h"
#include "School.h"
#include "Medical.h"
#include "Business.h"
#include "Daycare.h"
#include "../Location.h"

using namespace std;


BuildingGenerator::BuildingGenerator(std::default_random_engine g, int nb,
                                     std::vector< School > *sb, std::vector< Daycare > *db, std::vector< Medical > *mb,
                                     std::vector< Business > *bb, std::vector< Building > *ob,
                                     std::unordered_map<int, Building*> *ab, std::vector< std::vector < Location > > *d,
                                     bool pd){
    generator = g;
    schoolBuildings = sb;
    daycareBuildings = db;
    medicalBuildings = mb;
    businessBuildings = bb;
    otherBuildings = ob;
    allBuildings = ab;
    densityData = d;
    totalSchools=0;
    totalHospitals=0;
    totalBusinesses=0;
    totalDaycares=0;
    numberOfBuildings=nb;
    progressDisplay=pd;
    
    for(int i = 0; i<6 ; i++){
        totalBusinessSize[i] = 0;
        totalHospitalSize[i] = 0;
        totalSchoolSize[i] = 0;
        totalDaycareSize[i] = 0;
    }
}


void BuildingGenerator::generateBuildings(double businessSizeProbablities[6], double hospitalSizeProbablities[6],
                                          double schoolSizeProbablities[6], std::vector<double> locationProbablties,
                                          int const width, int const numberOfEmployeedAdults, int* numberOfStudentsPerGrade,
                                          int numberChildrenDaycare ){
    std::cout<< "Generating Buildings" <<std::endl;
    
    //Set Random Generator Seed
    int seed = time(0);
    generator.seed(seed);
    std::cout<<"Building Seed: "<<seed<<std::endl;
    //Set Distribution
    std::discrete_distribution<int> businessSizeDistribution{ businessSizeProbablities[0],
        businessSizeProbablities[1],
        businessSizeProbablities[2],
        businessSizeProbablities[3],
        businessSizeProbablities[4],
        businessSizeProbablities[5]};
    std::discrete_distribution<int> locationDistribution(locationProbablties.begin(), locationProbablties.end());
    
    std::vector<double> stuff=locationDistribution.probabilities();
    std::cout<<"Distributions Created"<<std::endl;
    //Set Tailies for Print out
    int totalBuinessPopulation=0;
    
    int elementrySchoolPopulation = numberOfStudentsPerGrade[0]+numberOfStudentsPerGrade[1]+numberOfStudentsPerGrade[2]+numberOfStudentsPerGrade[3]+numberOfStudentsPerGrade[4]+numberOfStudentsPerGrade[5];
    int middleSchoolPopulation=numberOfStudentsPerGrade[6]+numberOfStudentsPerGrade[7]+numberOfStudentsPerGrade[8];
    int highSchoolPopulation = numberOfStudentsPerGrade[9]+numberOfStudentsPerGrade[10]+numberOfStudentsPerGrade[11]+numberOfStudentsPerGrade[12];
    
    std::cout<<"Populations Created"<<std::endl;
    // std::cout<<"Elementry: "<<elementrySchoolPopulation<<std::endl;
    // std::cout<<"Middle: " << middleSchoolPopulation<<std::endl;
    // std::cout<<"High: "<<highSchoolPopulation<<endl<<std::endl;
    
    int eleTotal=0;
    int middleTotal=0;
    int highTotal=0;
    //Force Generate Enough Schools for Kids
    std::cout<<"Generating Schools"<<std::endl;
    while(elementrySchoolPopulation+middleSchoolPopulation+highSchoolPopulation>0){
        int capacity = 0;
        switch(businessSizeDistribution(generator)){
                /*case 0:
                 //4 (Anything less than 4 seems kind of silly)
                 capacity = 4;
                 totalSchoolSize[0]++;
                 
                 break;
                 case 1:
                 //5-9
                 capacity = (int)rand() % 4 + 5;
                 totalSchoolSize[1]++;
                 
                 break;*/
            case 2:
                //10-19
                capacity = (int)rand() % 9 + 10;
                break;
            case 3:
                //20-99
                capacity = (int)rand() % 79 + 20;
                break;
            case 4:
                //100-499
                capacity = (int)rand() % 399 + 100;
                break;
            case 5:
                //500
                capacity=500;
                break;
            default:
                //Need a minimum of 6 teachers (0.20 will not be teachers)
                capacity = (int)rand() % 9 + 10;
                break;
                
        }
        int location = locationDistribution(generator);
        if(elementrySchoolPopulation>0){
            //Make Elementry School
            // std::cout<<"Making Elementry: "<<location%width<<", "<<location/width<<"("<<capacity<<")"<<std::endl;
            schoolBuildings->push_back(School(numberOfBuildings, location%width, location/width, capacity, 10, 0));
            elementrySchoolPopulation-=capacity;
            if(elementrySchoolPopulation<0){
                elementrySchoolPopulation=0;
            }
            eleTotal++;
        }else{
            if(middleSchoolPopulation>0){
                //Make Middle School
                //  std::cout<<"Making Middle: "<<location%width<<", "<<location/width<<"("<<capacity<<")"<<std::endl;
                schoolBuildings->push_back(School(numberOfBuildings, location%width, location/width, capacity, 10, 1));
                middleSchoolPopulation-=capacity;
                if(middleSchoolPopulation<0){
                    middleSchoolPopulation=0;
                }
                middleTotal++;
            }else{
                if(highSchoolPopulation>0){
                    //Make High School
                    //  std::cout<<"Making High: "<<location%width<<", "<<location/width<<"("<<capacity<<")"<<std::endl;
                    schoolBuildings->push_back(School(numberOfBuildings, location%width, location/width, capacity, 10, 2));
                    highSchoolPopulation-=capacity;
                    if(highSchoolPopulation<0){
                        highSchoolPopulation=0;
                    }
                    highTotal++;
                }
            }
        }
        totalBuinessPopulation+=capacity;
        updateStatistics('S', capacity);
    }
    for(int i = 0; i< schoolBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({schoolBuildings->at(i).getID(), &(schoolBuildings->at(i))});
        densityData->at(schoolBuildings->at(i).getLocation()[0]).at(schoolBuildings->at(i).getLocation()[1]).addBuilding(&(schoolBuildings->at(i)));
    }
    std::cout<<"\tNumber of Elementries: "<<eleTotal<<endl;
    std::cout<<"\tNumber of Middle:  "<<middleTotal<<endl;
    std::cout<<"\tNumber of High:  "<<highTotal<<std::endl;
    std::cout<<"Generate Daycares"<<std::endl;
    std::cout<<"\tNumber of Young Children: "<<numberChildrenDaycare<<std::endl;
    while(numberChildrenDaycare>0){
        int capacity = 0;
        switch(businessSizeDistribution(generator)){
            case 0:
                //4
                capacity = 4;
                break;
            case 1:
                //5-9
                capacity = (int)rand() % 4 + 5;
                break;
            case 2:
                //10-19
                capacity = (int)rand() % 9 + 10;
                break;
                /*case 3: Anything greater than 20 seems kind of Silly
                 //20-99
                 capacity = (int)rand() % 79 + 20;
                 totalDaycareSize[3]++;
                 break;
                 case 4:
                 //100-499
                 capacity = (int)rand() % 399 + 100;
                 totalDaycareSize[4]++;
                 break;
                 case 5:
                 //500
                 capacity=500;
                 totalDaycareSize[5]++;
                 break;*/
            default:
                //Genrate a midsize Daycare
                capacity = (int)rand() % 4 + 5;
                break;
                
        }
        int location = locationDistribution(generator);
        //std::cout<<"Creating Daycare "<< location%width<<","<<location/width<<std::endl;
        daycareBuildings->push_back(Daycare(numberOfBuildings, location%width, location/width, 10, 10, capacity));
        numberChildrenDaycare-=capacity;
        if(numberChildrenDaycare<0){
            numberChildrenDaycare=0;
        }
        updateStatistics('D', capacity);

    }
    for(int i = 0; i< daycareBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({daycareBuildings->at(i).getID(), &(daycareBuildings->at(i))});
        densityData->at(daycareBuildings->at(i).getLocation()[0]).at(daycareBuildings->at(i).getLocation()[1]).addBuilding(&(daycareBuildings->at(i)));
    }
    std::cout<<"\tNumber Generated: "<<totalDaycares<<std::endl;
    
    std::cout<<"Generating Businesses"<<std::endl;
    //Create Businesses
    while(numberOfEmployeedAdults>totalBuinessPopulation){
        int capacity = -1;
        bool hospital=false;
        // bool school=false;
        switch(businessSizeDistribution(generator)){
            case 0:{
                //Building Size 0-4
                std::discrete_distribution<int> hospitalSizeDistribution0{1-hospitalSizeProbablities[0],hospitalSizeProbablities[0]};
                capacity = (int)rand() % 4 + 1;
                if(hospitalSizeDistribution0(generator)){
                    //Make Medical
                    hospital=true;
                }
                break;
            }
            case 1:{
                //Building Size 5-9
                std::discrete_distribution<int> hospitalSizeDistribution1{1-hospitalSizeProbablities[1],hospitalSizeProbablities[1]};
                capacity = (int)rand() % 4 + 5;
                if(hospitalSizeDistribution1(generator)){
                    //Make Medical
                    hospital=true;
                }
                break;
            }
            case 2:{
                //Building Size 10-19
                std::discrete_distribution<int> hospitalSizeDistribution2{1-hospitalSizeProbablities[2],hospitalSizeProbablities[2]};
                capacity = (int)rand() % 9 + 10;
                if(hospitalSizeDistribution2(generator)){
                    //Make Medical
                    hospital=true;
                }
                break;
            }
            case 3:{
                //Building Size 20-99
                std::discrete_distribution<int> hospitalSizeDistribution3{1-hospitalSizeProbablities[3],hospitalSizeProbablities[3]};
                capacity = (int)rand() % 79 + 20;
                if(hospitalSizeDistribution3(generator)){
                    //Make Medical
                    hospital=true;
                }
                break;
            }
            case 4:{
                //Building Size 100-499
                std::discrete_distribution<int> hospitalSizeDistribution4{1-hospitalSizeProbablities[4],hospitalSizeProbablities[4]};
                capacity = (int)rand() % 399 + 100;
                if(hospitalSizeDistribution4(generator)){
                    //Make Medical
                    hospital=true;
                }
                break;
            }
            case 5:{
                //Building Size  500
                std::discrete_distribution<int> hospitalSizeDistribution5{1-hospitalSizeProbablities[5],hospitalSizeProbablities[5]};
                capacity = 500;
                if(hospitalSizeDistribution5(generator)){
                    //Make Medical
                    hospital=true;
                }
                break;
            }
        }
        int location = locationDistribution(generator);
        
        if(hospital){
            //Medical was made
            medicalBuildings->push_back(Medical(numberOfBuildings, location%width, location/width, capacity, 10, 0));
            //allBuildings.insert({numberOfBuildings, &medicalBuildings.at(medicalBuildings.size()-1)});
            updateStatistics('M', capacity);
        }else{
            //Normal business was made
            businessBuildings->push_back(Business(numberOfBuildings,location%width, location/width, capacity, 10));
            //allBuildings.insert({numberOfBuildings, &businessBuildings.at(businessBuildings.size()-1)});
            //std::cout<<numberOfBuildings<<": "<<newBuilding.getLocation()[0]<<" "<<newBuilding.getLocation()[1]<<std::endl;
            updateStatistics('B', capacity);

        }
        totalBuinessPopulation+=capacity;
    }
    
    for(int i = 0; i< medicalBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({medicalBuildings->at(i).getID(), &(medicalBuildings->at(i))});
        densityData->at(medicalBuildings->at(i).getLocation()[0]).at(medicalBuildings->at(i).getLocation()[1]).addBuilding(&(medicalBuildings->at(i)));
    }
    
    for(int i = 0; i< businessBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({businessBuildings->at(i).getID(), &(businessBuildings->at(i))});
        densityData->at(businessBuildings->at(i).getLocation()[0]).at(businessBuildings->at(i).getLocation()[1]).addBuilding(&(businessBuildings->at(i)));
    }
    
    std::cout<<"Buildings Generated: "<<numberOfBuildings<<std::endl;
    
}

void BuildingGenerator::assignHomes(Population &pop){
    //Assign Families Locations
    int x=0;
    int y=0;
    int notAssigned=0;
    float oldRatio=0;
    std::cout << "Assigning Homes to Population" << std::endl;
    if(progressDisplay){
        printf("Percentage Complete: %3d%%", 0 );
    }
    fflush(stdout);
    for ( int i =0; i<pop.getNumberOfFamilies(); i++ ) {
        //Create all Homes (Temporary)
        otherBuildings->push_back(Building('H', -1, -1, -1, 0, 0));
    }
    
    for ( int i =0; i<pop.getNumberOfFamilies(); i++ ) {
        while (densityData->at(x).at(y).isFull()) {
            //Move to Next Location in column
            x++;
            if(x>=densityData->size()&&y<densityData->at(0).size()){
                //Move to next Row
                x=0;
                y++;
            }
            else{
                if (y>=densityData->at(0).size()-1) {
                    //No More Locations Avaliable
                    notAssigned++;
                    break;
                }
            }
        }
        
        if (y>=densityData->at(0).size()-1) {
            //No More Locations Avaliable
            std::cout<<"No Locations Avaliable"<<std::endl;
            break;
        }
        //Set Location of Family
        otherBuildings->at(i).setCurrentCapacity(pop.getFamily(i)->getNumberOfPeople());
        otherBuildings->at(i).setLocation(x,y);
        otherBuildings->at(i).setID(numberOfBuildings);
        allBuildings->insert({numberOfBuildings, &otherBuildings->at(i)});
        densityData->at(x).at(y).addFamily(pop.getFamily(i));
        densityData->at(x).at(y).addBuilding(&(otherBuildings->at(i)));
        pop.setHomeLocationOfFamily(&(otherBuildings->at(i)), i);
        
        numberOfBuildings++;
        
        //Calculate Percent Complete
        if(progressDisplay){
            float ratio = i/(float)pop.getNumberOfFamilies();
            if ( 100*(ratio-oldRatio) > 1 ) {
                //Update Percent Complete Only if there is a Change
                printf("\r");
                printf("Percentage Complete: %3d%%", (int)(ratio*100) );
                oldRatio=ratio;
                fflush(stdout);
            }
        }
    }
    
    
    //Print out 100% Complete
    if(progressDisplay){
        printf("\r");
        printf("Percentage Complete: %3d%%", 100 );
        fflush(stdout);
    }
    std::cout << std::endl <<"Population Successfully Assigned Locations " << std::endl;
    
}


void BuildingGenerator::displayBuildingStatistics(double businessSizeProbablities[6], double hospitalSizeProbablities[6], double schoolSizeProbablities[6], double daycareSizeProbablities[6], std::string fileLocation){
    std::ostringstream outputString;
    
    outputString << "--------Buildings--------" << std::endl;
    outputString << "Total Buildings Generated: " << numberOfBuildings << std::endl;
    outputString << "----Businesses----" << std::endl;
    outputString <<"Total Generated: " << totalBusinesses << std::endl;
    outputString << "Employee Capacity 1-4:     \t" << totalBusinessSize[0] << " \t"<<(totalBusinessSize[0]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[0] << ")" << std::endl;
    outputString << "Employee Capacity 5-9:     \t" << totalBusinessSize[1] << " \t"<<(totalBusinessSize[1]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[1] << ")" << std::endl;
    outputString << "Employee Capacity 10-19:   \t" << totalBusinessSize[2] << " \t"<<(totalBusinessSize[2]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[2] << ")" << std::endl;
    outputString << "Employee Capacity 20-99:   \t" << totalBusinessSize[3] << " \t"<<(totalBusinessSize[3]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[3] << ")" << std::endl;
    outputString << "Employee Capacity 100-499: \t" << totalBusinessSize[4] << " \t"<<(totalBusinessSize[4]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[4] << ")" << std::endl;
    outputString << "Employee Capacity 500:     \t" << totalBusinessSize[5] << " \t"<<(totalBusinessSize[5]/(double)totalBusinesses) << "\t(Expected " << businessSizeProbablities[5] << ")" << std::endl;
    
    outputString << "----Hospitals----" << std::endl;
    outputString <<"Total Generated: " << totalHospitals << std::endl;
    outputString << "Employee Capacity 1-4:     \t" << totalHospitalSize[0] << " \t"<<(totalHospitalSize[0]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[0] << ")" << std::endl;
    outputString << "Employee Capacity 5-9:     \t" << totalHospitalSize[1] << " \t"<<(totalHospitalSize[1]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[1] << ")" << std::endl;
    outputString << "Employee Capacity 10-19:   \t" << totalHospitalSize[2] << " \t"<<(totalHospitalSize[2]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[2] << ")" << std::endl;
    outputString << "Employee Capacity 20-99:   \t" << totalHospitalSize[3] << " \t"<<(totalHospitalSize[3]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[3] << ")" << std::endl;
    outputString << "Employee Capacity 100-499: \t" << totalHospitalSize[4] << " \t"<<(totalHospitalSize[4]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[4] << ")" << std::endl;
    outputString << "Employee Capacity 500:     \t" << totalHospitalSize[5] << " \t"<<(totalHospitalSize[5]/(double)totalHospitals) << "\t(Expected " << hospitalSizeProbablities[5] << ")" << std::endl;
    
    outputString << "----Schools----" << std::endl;
    outputString <<"Total Generated: " << totalSchools << std::endl;
    outputString << "Employee Capacity 1-4:     \t" << totalSchoolSize[0] << " \t"<<(totalSchoolSize[0]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[0] << ")" << std::endl;
    outputString << "Employee Capacity 5-9:     \t" << totalSchoolSize[1] << " \t"<<(totalSchoolSize[1]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[1] << ")" << std::endl;
    outputString << "Employee Capacity 10-19:   \t" << totalSchoolSize[2] << " \t"<<(totalSchoolSize[2]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[2] << ")" << std::endl;
    outputString << "Employee Capacity 20-99:   \t" << totalSchoolSize[3] << " \t"<<(totalSchoolSize[3]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[3] << ")" << std::endl;
    outputString << "Employee Capacity 100-499: \t" << totalSchoolSize[4] << " \t"<<(totalSchoolSize[4]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[4] << ")" << std::endl;
    outputString << "Employee Capacity 500:     \t" << totalSchoolSize[5] << " \t"<<(totalSchoolSize[5]/(double)totalSchools) << "\t(Expected " << schoolSizeProbablities[5] << ")" << std::endl;
    
    outputString << "----Daycares----" << std::endl;
    outputString <<"Total Generated: " << totalDaycares << std::endl;
    outputString << "Employee Capacity 1-4:     \t" << totalDaycareSize[0] << " \t"<<(totalDaycareSize[0]/(double)totalDaycares) << "\t(Expected " << daycareSizeProbablities[0] << ")" << std::endl;
    outputString << "Employee Capacity 5-9:     \t" << totalDaycareSize[1] << " \t"<<(totalDaycareSize[1]/(double)totalDaycares) << "\t(Expected " << daycareSizeProbablities[1] << ")" << std::endl;
    outputString << "Employee Capacity 10-19:   \t" << totalDaycareSize[2] << " \t"<<(totalDaycareSize[2]/(double)totalDaycares) << "\t(Expected " << daycareSizeProbablities[2] << ")" << std::endl;
    outputString << "Employee Capacity 20-99:   \t" << totalDaycareSize[3] << " \t"<<(totalDaycareSize[3]/(double)totalDaycares) << "\t(Expected " << daycareSizeProbablities[3] << ")" << std::endl;
    outputString << "Employee Capacity 100-499: \t" << totalDaycareSize[4] << " \t"<<(totalDaycareSize[4]/(double)totalDaycares) << "\t(Expected " << daycareSizeProbablities[4] << ")" << std::endl;
    outputString << "Employee Capacity 500:     \t" << totalDaycareSize[5] << " \t"<<(totalDaycareSize[5]/(double)totalDaycares) << "\t(Expected " << daycareSizeProbablities[5] << ")" << std::endl;
    if(fileLocation==""){
        std::cout<< outputString.str();
    }else{
        ofstream buildingStatsFile;
        buildingStatsFile.open(fileLocation+"/buildingStatistics.txt");
        buildingStatsFile << outputString.str();
        buildingStatsFile.close();
        
    }
}

void BuildingGenerator::exportBuildings(std::string fileLocation){
        std::ostringstream outputString;
        
        for(auto i = schoolBuildings->begin(); i != schoolBuildings->end(); i++){
            outputString << i->exportString();
        }
        
        for(auto i = businessBuildings->begin(); i != businessBuildings->end(); i++){
            outputString << i->exportString();
        }
        
        for(auto i = daycareBuildings->begin(); i != daycareBuildings->end(); i++){
            outputString << i->exportString();
        }
    
        for(auto i = medicalBuildings->begin(); i != medicalBuildings->end(); i++){
            outputString << i->exportString();
        }
        
        for(auto i = otherBuildings->begin(); i != otherBuildings->end(); i++){
            outputString << i->exportString();
        }
        
        ofstream buildingExportFile;
        buildingExportFile.open(fileLocation+"/buildingExport.hpb");
        buildingExportFile << outputString.str();
        buildingExportFile.close();
}

void BuildingGenerator::importBuildings(std::string fileLocation){
    std::ifstream in(fileLocation);
    std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::stringstream ss(s);
    std::string item;
    std::getline(ss, item, '*');
    while (std::getline(ss, item, '*')) {
        //Process Building Element
        std::stringstream rows(item);
        std::string r;
        std::getline(rows, r, '\n');
        char type=(r.c_str())[0];
        std::getline(rows, r, '\n');
        int id = std::stoi(r);
        std::getline(rows, r, '\n');
        int maxCap = std::stoi(r);
        std::getline(rows, r, '\n');
        int maxVis = std::stoi(r);
        std::string loc;
        std::getline(rows, r, '\n');
        std::stringstream locationString(r);
        std::getline(locationString, loc, ',');
        int loc_x = std::stoi(loc);
        std::getline(locationString, loc, ',');
        int loc_y = std::stoi(loc);
        switch(type){
            case 'M':{
                //Medical
                std::getline(rows, r, '\n');
                int patCap = std::stoi(r);
                medicalBuildings->push_back(Medical(id, loc_x, loc_y, maxCap, maxVis, patCap));
                break;}
            case 'S':{
                //School
                std::getline(rows, r, '\n');
                int schoolType = std::stoi(r);
                std::getline(rows, r, '\n');
                int studCap = std::stoi(r);
                std::getline(rows, r, '\n');
                int startTime = std::stoi(r);
                std::getline(rows, r, '\n');
                int endTime = std::stoi(r);
                schoolBuildings->push_back(School(id, loc_x, loc_y, maxCap, maxVis, studCap, schoolType, startTime, endTime));
                break;}
            case 'B':{
                //Business
                businessBuildings->push_back(Business(id, loc_x, loc_y, maxCap, maxVis));
                break;
            case 'D':
                //Daycare
                std::getline(rows, r, '\n');
                int childCap = std::stoi(r);
                daycareBuildings->push_back(Daycare(id, loc_x, loc_y, maxCap, maxVis, childCap));
                break;}
            default:{
                //Standard Building
                otherBuildings->push_back(Building(type,id, loc_x, loc_y, maxCap, maxVis));
                break;
            }
        }
        updateStatistics(type, maxCap);
    }

    for(int i = 0; i< otherBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({otherBuildings->at(i).getID(), &(otherBuildings->at(i))});
        densityData->at(otherBuildings->at(i).getLocation()[0]).at(otherBuildings->at(i).getLocation()[1]).addBuilding(&(otherBuildings->at(i)));
        allBuildings->insert({otherBuildings->at(i).getID(),
            &(otherBuildings->at(i))});
    }
    
    for(int i = 0; i< daycareBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({daycareBuildings->at(i).getID(), &(daycareBuildings->at(i))});
        densityData->at(daycareBuildings->at(i).getLocation()[0]).at(daycareBuildings->at(i).getLocation()[1]).addBuilding(&(daycareBuildings->at(i)));
        allBuildings->insert({daycareBuildings->at(i).getID(),
            &(daycareBuildings->at(i))});

    }
    
    for(int i = 0; i< schoolBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({schoolBuildings->at(i).getID(), &(schoolBuildings->at(i))});
        densityData->at(schoolBuildings->at(i).getLocation()[0]).at(schoolBuildings->at(i).getLocation()[1]).addBuilding(&(schoolBuildings->at(i)));
        allBuildings->insert({schoolBuildings->at(i).getID(),
            &(schoolBuildings->at(i))});
    }
    
    for(int i = 0; i< medicalBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({medicalBuildings->at(i).getID(), &(medicalBuildings->at(i))});
        densityData->at(medicalBuildings->at(i).getLocation()[0]).at(medicalBuildings->at(i).getLocation()[1]).addBuilding(&(medicalBuildings->at(i)));
        allBuildings->insert({medicalBuildings->at(i).getID(),
            &(medicalBuildings->at(i))});
    }
    
    for(int i = 0; i< businessBuildings->size();i++){
        //Set up Pointers
        allBuildings->insert({businessBuildings->at(i).getID(), &(businessBuildings->at(i))});
        densityData->at(businessBuildings->at(i).getLocation()[0]).at(businessBuildings->at(i).getLocation()[1]).addBuilding(&(businessBuildings->at(i)));
        allBuildings->insert({businessBuildings->at(i).getID(),
            &(businessBuildings->at(i))});
    }
    
    std::cout<<"\tImported Buildings Successfully"<<std::endl;
    std::cout<<"\tBusinesses: "<<businessBuildings->size()<<std::endl;
    std::cout<<"\tDaycares: "<<daycareBuildings->size()<<std::endl;
    std::cout<<"\tMedical: "<<medicalBuildings->size()<<std::endl;
    std::cout<<"\tSchools: "<<schoolBuildings->size()<<std::endl;
    std::cout<<"\tOther Buildigns: "<<otherBuildings->size()<<std::endl;
    std::cout<<"\tTotal: "<<allBuildings->size()<<std::endl;
}

void BuildingGenerator::updateStatistics(char buildingType, int capacity){
    int capacityLevel = 0;
    numberOfBuildings++;
    if(capacity > 4 && capacity < 10){
        capacityLevel = 1;
    }else{
        if(capacity > 9 && capacity < 20){
            capacityLevel = 2;
        }else{
            if(capacityLevel > 19 && capacity < 100){
                capacityLevel = 3;
            }else{
                if(capacityLevel > 99 && capacity < 500){
                    capacityLevel = 4;
                }else{
                    if (capacityLevel > 499){
                        capacityLevel = 5;
                    }
                }
            }
        }
    }
    
    switch(buildingType){
        case 'M':
            totalHospitalSize[capacityLevel]++;
            totalHospitals++;
            break;
        case 'B':
            totalBusinessSize[capacityLevel]++;
            totalBusinesses++;
            break;
        case 'S':
            totalSchoolSize[capacityLevel]++;
            totalSchools++;
            break;
        case 'D':
            totalDaycareSize[capacityLevel]++;
            totalDaycares++;
            break;
    }
}

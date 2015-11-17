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

#include "Population.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Person.h"
#include "Family.h"
#include "Buildings/Building.h"
#include <vector>
#include <ctime>

using namespace std;

Population::Population(){
    
}

Population::Population(int s, double *ageProbablities, double *familySizeProbablites, double mProb, double
    *scheduleProbablities, int popSeed) {
    numberOfMales=0;
    
	size=s;
    //Initalize random number generator
   // std::default_random_engine generator;   //Random Generator for Distributions
    int seed = time(0);
    
    //Forcing Population Seed for Stable Population for Testing
    if(popSeed!=-1){
        seed = popSeed;
    }
    
    generator.seed(seed);
    std::cout<< "Population Seed: " << seed << std::endl;
    //Set Age Probablities
    this->ageProbablities=ageProbablities;

    //Set Family Size Probablities
    this->familySizeProbablites=familySizeProbablites;

    //Set Schedule Probablities
    this->scheduleProbablities=scheduleProbablities;

    //Set Probablity of Male (It is Assumed Probablity of Female is 1-MaleProbablity)
    maleProbablity=mProb;
}

Population::Population(const Population &p){
    this->numberOfMales=p.numberOfMales;
    this->size=p.size;
    this->generator = p.generator;
    //Set Age Probablities
    this->ageProbablities=p.ageProbablities;
    //Set Family Size Probablities
    this->familySizeProbablites=p.familySizeProbablites;
    //Set Schedule Probablities
    this->scheduleProbablities=p.scheduleProbablities;
    //Set Probablity of Male (It is Assumed Probablity of Female is 1-MaleProbablity)
    this->maleProbablity=p.maleProbablity;
    this->families = p.families;
    this->numberOfChildrenDaycare = p.numberOfChildrenDaycare;
}


void Population::generatePopulation(bool progressDisplay){
    //std::cout << "Creating Population of size: " << size << std::endl;
    int i=0;
    float oldRatio=0;
    int numberOfFamilies = 0;
    //Reserve Space for Familes to prevent space resassignment midway (Avg Family size is 3)
    //families.reserve(size/3);
    
    if(progressDisplay){
        printf("Percentage Complete: %3d%%", 0 );
        fflush(stdout);
    }
    
    //Initialize Arrays
    for(int i = 0; i < 13;i++){
        numberOfStudentsAssignedGrade[i]=0;
        
    }
    
    //Initialize Arrays
    for(int i = 0; i < 11;i++){
        numberOfPeopleAssignedSchedule[i]=0;
        
    }
    //Initialize Arrays
    for(int i = 0; i < 7;i++){
        numberOfFamiliesSizes[i]=0;
        numberOfPeopleAges[i]=0;
    }
    
    while( i < size){
        int familySize= generateFamilySize();
        // std::cout<< "Family Size: "<< familySize << std::endl;
        if(familySize > size-i ){
            familySize = size-i;
        }
        
        families.insert({numberOfFamilies, Family(NULL, NULL, numberOfFamilies)});
        Family *newFamily = &families[numberOfFamilies];
        
        for( int b = 0; b<familySize; b++ ){
            //Always first Person in Family must be an Adult
            int ageInformation[2];
            determineAge(((b == 0) ? true : false ), ageInformation);
            int scheduleType = determineScheduleType(ageInformation[1]);
            char gender =determineGender();
            Person newPerson= Person(ageInformation[0],
                                     gender,
                                     -1,
                                     i++,
                                     scheduleType,
                                     true);
            updateStatistics(ageInformation[0], scheduleType, gender);
            newFamily->addPerson(newPerson);
        }
        
        
        if(newFamily->getHasYoungChild()>0 && newFamily->getChildCareAdult()->getSchedule()->getScheduleType()!=4){
            //std::cout<<"Family Needs Day Care"<<std::endl;
            numberOfChildrenDaycare++;
        }
        numberOfFamilies++;
        if(progressDisplay){
            float ratio = i/(float)size;
            if(100*(ratio-oldRatio)>1){
                // Show the percentage complete.
                printf("\r");
                printf("Percentage Complete: %3d%%", (int)(ratio*100) );
                oldRatio=ratio;
                fflush(stdout);
            }
        }
        
    }
    if(progressDisplay){
        printf("\r");
        printf("Percentage Complete: %3d%%", 100 );
        fflush(stdout);
    }
    std::cout<< std::endl;
    std::cout<<"Total of "<<i<<" People Generated."<<std::endl;
    
}

int Population::getPopulationSize(){
    return size;
    
}

int Population::getNumberOfFamilies(){
    return families.size();
    
}

void Population::setHomeLocationOfFamily(Building *home, int family){
    families.at(family).setHome(home);
    int location = families.at(family).getHome()->getID();
    families.at(family).setLocation(location);

}

Family* Population::getFamily(int family){
    //std::cout<<"------Family Data--------"<<std::endl;
    //std::cout<<families.at(family).toString()<<std::endl;
    return &families.at(family);
}

int Population::getNumberOfEmployeedAdults(){
    return numberOfPeopleAssignedSchedule[3]+numberOfPeopleAssignedSchedule[5]
           +numberOfPeopleAssignedSchedule[7]+numberOfPeopleAssignedSchedule[9];
}

int* Population::getNumberOfStudentsPerGrade(){
    return numberOfStudentsAssignedGrade;
}


int Population::getNumberOfChildrenDaycare(){
    return numberOfChildrenDaycare;
    
}

char Population::determineGender(){
	//Generate random number
	double x = 1.0*rand()/RAND_MAX;
	if ( x<maleProbablity ) {
		return 'm';
	}
	else {
		return 'f';
	}

}

int Population::generateFamilySize(){
    std::discrete_distribution<int> distribution{familySizeProbablites[0],
                                                 familySizeProbablites[1],
                                                 familySizeProbablites[2],
                                                 familySizeProbablites[3],
                                                 familySizeProbablites[4],
                                                 familySizeProbablites[5],
                                                 familySizeProbablites[6]};
    int size=distribution(generator);
    numberOfFamiliesSizes[size]++;
    return size+1;
}


int *Population::determineAge(bool forceAdult, int *returnArray){
    int ageGroup=-1;
    int *ageInformation = returnArray;
    if(forceAdult){
        //Adult Only (Using Uniform Distribution for Now)
        int age=(int)rand() % 82 + 18;
        if(age<25){
            ageGroup=3;
        }
        else{
            if(age<45){
                ageGroup=4;
            }
            else{
                if(age<65){
                    ageGroup=5;
                }
                else{
                    ageGroup=6;
                }
            }
        }
        ageInformation[0]=age;
        ageInformation[1]=ageGroup;
    }
    else{
        //Any age can be selected
        
        //Create Distribution
        std::discrete_distribution<int> distribution{ ageProbablities[0],
            ageProbablities[1],
            ageProbablities[2],
            ageProbablities[3],
            ageProbablities[4],
            ageProbablities[5],
            ageProbablities[6]};

        
        switch(distribution(generator)){
            case 0:
                //4 and Younger
                ageInformation[0]=(int)rand() % 4;
                ageInformation[1]=0;
                break;
            case 1:
                //5-13
                ageInformation[0]= (int)rand() % 8 + 5;
                ageInformation[1]= 1;
                break;
            case 2:
                //14-17
                ageInformation[0]=(int)rand() % 3 + 14;
                ageInformation[1]=2;
                break;
            case 3:
                //18-24
                ageInformation[0]= (int)rand() % 6 + 18;
                ageInformation[1]=3;
                break;
            case 4:
                //25-44
                ageInformation[0]= (int)rand() % 19 + 25;
                ageInformation[1]=4;
                break;
            case 5:
                //45-64
                ageInformation[0]= (int)rand() % 19 + 45;
                ageInformation[1]=5;
                break;
            case 6:
                //65-Older
                ageInformation[0]= (int)rand() % 35 +65;
                ageInformation[1]=6;
                break;
        }
    }
    if(ageInformation[0]>100){
        std::cout<<"TOO OLD: "+std::to_string(ageInformation[0])+" "+std::to_string(ageInformation[1])<<std::endl;
    }
    if(ageInformation[0]<0){
        std::cout<<"TOO YOUNG: "+std::to_string(ageInformation[0])+" "+std::to_string(ageInformation[1])<<std::endl;
    }
    
    return ageInformation;
}

int Population::determineScheduleType(int ageGroup){
    double adultScheduleProbablities[2];
    switch(ageGroup){
        case 0:
            //Young Child Schedule
            return 0;
            break;
        case 1:
            //School Aged Child Schedule (5-13)
            return 1;
            break;
        case 2:
            //School Aged Child Schedule (14-17)
            return 2;
            break;
        case 3:
            //Adult Schedule (18-24)
            adultScheduleProbablities[0]=scheduleProbablities[3];
            adultScheduleProbablities[1]=scheduleProbablities[4];
            break;
        case 4:
            //Adult Schedule (25-44)
            adultScheduleProbablities[0]=scheduleProbablities[5];
            adultScheduleProbablities[1]=scheduleProbablities[6];
            break;
        case 5:
            //Adult Schedule (45-64)
            adultScheduleProbablities[0]=scheduleProbablities[7];
            adultScheduleProbablities[1]=scheduleProbablities[8];
            break;
        case 6:
            //Adult Schedule (65-Older)
            adultScheduleProbablities[0]=scheduleProbablities[9];
            adultScheduleProbablities[1]=scheduleProbablities[10];
            break;
    }
    
    //Create Distributon for Adult Schedule
    std::discrete_distribution<int> distribution{adultScheduleProbablities[0],adultScheduleProbablities[1]};
   
    //Select Adult Schedule
    int scheduleType =distribution(generator);
    
    if(scheduleType==1){
        //Unemployeed
        return 4;
    }
    else{
        //Employeed
        return 3;
    }
    return 0;
}

void Population::displayStatistics(std::string fileLocation){
    std::ostringstream outputString;

    //Display Family Information
    cout<< std::fixed<< std::setprecision(5);
    outputString<< std::fixed << std::setprecision(5);
    
    outputString <<"--------Family--------" << std::endl;
    outputString << "Total Number of Families: " << getNumberOfFamilies() << std::endl;
    outputString << "Size 1:  \t" << numberOfFamiliesSizes[0] << " \t" << (numberOfFamiliesSizes[0]/(double)getNumberOfFamilies()) << "\t(Expected " << familySizeProbablites[0] << ")" << std::endl;
    outputString<< "Size 2:  \t" << numberOfFamiliesSizes[1] << " \t" << (numberOfFamiliesSizes[1]/(double)getNumberOfFamilies()) << "\t(Expected " <<familySizeProbablites[1] << ")" << std::endl;
    outputString << "Size 3:  \t" << numberOfFamiliesSizes[2] << " \t" << (numberOfFamiliesSizes[2]/(double)getNumberOfFamilies()) << "\t(Expected " << familySizeProbablites[2] << ")" << std::endl;
    outputString << "Size 4:  \t" << numberOfFamiliesSizes[3] << " \t" << (numberOfFamiliesSizes[3]/(double)getNumberOfFamilies()) << "\t(Expected " << familySizeProbablites[3] << ")" << std::endl;
    outputString << "Size 5:  \t" << numberOfFamiliesSizes[4] << " \t" << (numberOfFamiliesSizes[4]/(double)getNumberOfFamilies()) << "\t(Expected " << familySizeProbablites[4] << ")" << std::endl;
    outputString << "Size 6:  \t" << numberOfFamiliesSizes[5] << " \t" << (numberOfFamiliesSizes[5]/(double)getNumberOfFamilies()) << "\t(Expected " << familySizeProbablites[5] << ")" << std::endl;
    outputString << "Size 7:  \t" << numberOfFamiliesSizes[6] << " \t" << (numberOfFamiliesSizes[6]/(double)getNumberOfFamilies()) << "\t(Expected " << familySizeProbablites[6] << ")" << std::endl;
    
    //Display Population Information
    outputString <<"--------Population--------" << std::endl;
	outputString << "----Gender----" << std::endl;
	outputString << "Males:   \t" << numberOfMales <<" \t" << (numberOfMales/(double)size) << "\t(Expected " << maleProbablity << ")" <<std::endl;
	outputString << "Females: \t" << size-numberOfMales <<" \t" << ((size-numberOfMales)/(double)size) << "\t(Expected " << 1-maleProbablity << ")" << std::endl;
	outputString << "----Age----"<<endl;
	outputString << "Under Age 5: \t" <<numberOfPeopleAges[0]<< " \t"<<(numberOfPeopleAges[0]/(double)size) << "\t(Expected " << ageProbablities[0] << ")" << std::endl;
	outputString << "Age 5-13:    \t" <<numberOfPeopleAges[1]<< " \t"<< (numberOfPeopleAges[1]/(double)size) << "\t(Expected " << ageProbablities[1] << ")" << std::endl;
	outputString << "Age 14-17:   \t" <<numberOfPeopleAges[2]<< " \t"<< (numberOfPeopleAges[2]/(double)size) << "\t(Expected " << ageProbablities[2] << ")" << std::endl;
	outputString << "Age 18-24:   \t" <<numberOfPeopleAges[3]<< " \t"<< (numberOfPeopleAges[3]/(double)size) << "\t(Expected " << ageProbablities[3] << ")" << std::endl;
	outputString << "Age 25-44:   \t" <<numberOfPeopleAges[4]<< " \t"<< (numberOfPeopleAges[4]/(double)size) << "\t(Expected " << ageProbablities[4] << ")" << std::endl;
	outputString << "Age 45-64:   \t" <<numberOfPeopleAges[5] << " \t" << (numberOfPeopleAges[5]/(double)size) << "\t(Expected " << ageProbablities[5] << ")" << std::endl;
	outputString << "Over Age 65: \t" <<numberOfPeopleAges[6] << " \t" << (numberOfPeopleAges[6]/(double)size) << "\t(Expected " << ageProbablities[6] << ")" << std::endl;
    outputString << "Total:       \t" <<to_string(numberOfPeopleAges[0]+numberOfPeopleAges[1]+
                                      numberOfPeopleAges[2]+numberOfPeopleAges[3]+
                                      numberOfPeopleAges[4]+numberOfPeopleAges[5]+
                                      numberOfPeopleAges[6]) << std::endl;
    //Display Schedule Information
    outputString <<"--------Schedules--------" << std::endl;
    outputString << "Young Child: \t"<< numberOfPeopleAssignedSchedule[0] << " \t" << (numberOfPeopleAssignedSchedule[0]/(double)numberOfPeopleAges[0]) <<"\t(Expected " <<scheduleProbablities[0] <<")" <<endl;
    double expected=(ageProbablities[1]*scheduleProbablities[1])+(ageProbablities[2]*scheduleProbablities[2]);
    outputString << "School Schedule: \t"<< numberOfPeopleAssignedSchedule[1]+numberOfPeopleAssignedSchedule[2] << " \t"<< ((numberOfPeopleAssignedSchedule[1]+numberOfPeopleAssignedSchedule[2])/(double)numberOfPeopleAges[1]+numberOfPeopleAges[2]) <<"\t(Expected " <<expected <<")" <<endl;
    
    expected=(ageProbablities[3]*scheduleProbablities[3])+
                           (ageProbablities[4]*scheduleProbablities[5])+
                           (ageProbablities[5]*scheduleProbablities[7])+
                           (ageProbablities[6]*scheduleProbablities[9]);
    
    outputString << "Employeed Schedule: \t"<< numberOfPeopleAssignedSchedule[3]+numberOfPeopleAssignedSchedule[5]+numberOfPeopleAssignedSchedule[7]+numberOfPeopleAssignedSchedule[9] << " \t"<< ((numberOfPeopleAssignedSchedule[3]+numberOfPeopleAssignedSchedule[5]+numberOfPeopleAssignedSchedule[7]+numberOfPeopleAssignedSchedule[9])/((double)numberOfPeopleAges[3]+numberOfPeopleAges[4]+numberOfPeopleAges[5]+numberOfPeopleAges[6])) <<"\t(Expected " <<expected <<")" <<endl;
   
    expected=(ageProbablities[3]*scheduleProbablities[4])+
             (ageProbablities[4]*scheduleProbablities[6])+
             (ageProbablities[5]*scheduleProbablities[8])+
             (ageProbablities[6]*scheduleProbablities[10]);
    
    outputString << "Unemployeed Schedule: \t"<< numberOfPeopleAssignedSchedule[4]+numberOfPeopleAssignedSchedule[6]+numberOfPeopleAssignedSchedule[8]+numberOfPeopleAssignedSchedule[10] << " \t"<< ((numberOfPeopleAssignedSchedule[4]+numberOfPeopleAssignedSchedule[6]+numberOfPeopleAssignedSchedule[8]+numberOfPeopleAssignedSchedule[10])/((double)numberOfPeopleAges[3]+numberOfPeopleAges[4]+numberOfPeopleAges[5]+numberOfPeopleAges[6])) <<"\t(Expected " <<expected <<")" <<endl;
    
    outputString << "----Schedule Break Down----" << std::endl;
    outputString << "Young Child (0-4): \t"<< numberOfPeopleAssignedSchedule[0] << " \t" << (numberOfPeopleAssignedSchedule[0]/(double)numberOfPeopleAges[0]) <<"\t(Expected " <<scheduleProbablities[0] <<")" <<endl;
    outputString << "School Schedule (5-13): \t"<< numberOfPeopleAssignedSchedule[1] << " \t"<< (numberOfPeopleAssignedSchedule[1]/(double)numberOfPeopleAges[1]) <<"\t(Expected " <<scheduleProbablities[1] <<")" <<std::endl;
    outputString << "School Schedule (14-17): \t"<< numberOfPeopleAssignedSchedule[2] << " \t"<< (numberOfPeopleAssignedSchedule[2]/(double)numberOfPeopleAges[2]) <<"\t(Expected " <<scheduleProbablities[2] <<")" <<std::endl;
    outputString << "Employeed Schedule (18-24): \t"<< numberOfPeopleAssignedSchedule[3] << " \t"<< (numberOfPeopleAssignedSchedule[3]/(double)numberOfPeopleAges[3]) <<"\t(Expected " <<scheduleProbablities[3] <<")" <<std::endl;
    outputString << "Unemployeed Schedule (18-24): \t"<< numberOfPeopleAssignedSchedule[4] << " \t"<< (numberOfPeopleAssignedSchedule[4]/(double)numberOfPeopleAges[3]) <<"\t(Expected " <<scheduleProbablities[4] <<")" <<std::endl;
    outputString << "Employeed Schedule (25-44): \t"<< numberOfPeopleAssignedSchedule[5] << " \t"<< (numberOfPeopleAssignedSchedule[5]/(double)numberOfPeopleAges[4]) <<"\t(Expected " <<scheduleProbablities[5] <<")" <<std::endl;
    outputString << "Unemployeed Schedule (25-44): \t"<< numberOfPeopleAssignedSchedule[6] << " \t"<< (numberOfPeopleAssignedSchedule[6]/(double)numberOfPeopleAges[4]) <<"\t(Expected " <<scheduleProbablities[6] <<")" <<std::endl;
    outputString << "Employeed Schedule (45-64): \t"<< numberOfPeopleAssignedSchedule[7] << " \t"<< (numberOfPeopleAssignedSchedule[7]/(double)numberOfPeopleAges[5]) <<"\t(Expected " <<scheduleProbablities[7] <<")" <<std::endl;
    outputString << "Unemployeed Schedule (45-64): \t"<< numberOfPeopleAssignedSchedule[8] << " \t"<< (numberOfPeopleAssignedSchedule[8]/(double)numberOfPeopleAges[5]) <<"\t(Expected " <<scheduleProbablities[8] <<")" <<std::endl;
    outputString << "Employeed Schedule (65-Older): \t"<< numberOfPeopleAssignedSchedule[9] << " \t"<< (numberOfPeopleAssignedSchedule[9]/(double)numberOfPeopleAges[6]) <<"\t(Expected " <<scheduleProbablities[9] <<")" <<std::endl;
    outputString << "Unemployeed Schedule (65-Older): \t"<< numberOfPeopleAssignedSchedule[10] << " \t"<< (numberOfPeopleAssignedSchedule[10]/(double)numberOfPeopleAges[6]) <<"\t(Expected " <<scheduleProbablities[10] <<")" <<std::endl;
    outputString << "Total:                           \t"<<numberOfPeopleAssignedSchedule[0]+numberOfPeopleAssignedSchedule[2]+numberOfPeopleAssignedSchedule[3]+numberOfPeopleAssignedSchedule[4]+
                                                        numberOfPeopleAssignedSchedule[5]+numberOfPeopleAssignedSchedule[6]+numberOfPeopleAssignedSchedule[7]+numberOfPeopleAssignedSchedule[8]+
                                                        numberOfPeopleAssignedSchedule[9]+numberOfPeopleAssignedSchedule[10]+numberOfPeopleAssignedSchedule[1] << std::endl;
    
     if(fileLocation!=""){
         ofstream buildingStatsFile;
         buildingStatsFile.open(fileLocation+"/populationStatistics.txt");
         buildingStatsFile << outputString.str();
         buildingStatsFile.close();
     }
    
    //std::cout<< outputString.str();
}

std::string Population::returnFirstTenFamiliesInfo(std::string fileLocation){
    
    std::string returnString="FIRST 10 FAMILY INFORMATION\n";
    int n=getNumberOfFamilies();
    for(int i=0; i<n; i++){
        std::ostringstream outputString;
        outputString << getFamily(i)->toString();
        returnString+=outputString.str();

        if(fileLocation!=""){
            ofstream buildingStatsFile;
            buildingStatsFile.open(fileLocation+"/Family_"+std::to_string(i)+".csv");
            buildingStatsFile << outputString.str();
            buildingStatsFile.close();
            
        }
        
    }
    
   
    return returnString;
}


void Population::updateToNextTimeStep(std::unordered_map<int, Building*> *allBuildings){
    //std::cout<<"Updating Population"<<std::endl;
    for (auto i = families.begin(); i!= families.end(); i++){
        i->second.updateToNextTimeStep(allBuildings);
    }
}


void Population::exportPopulation(std::string fileLocation){
    std::ostringstream outputString;

    for (auto i = families.begin(); i!= families.end(); i++){
        outputString<<i->second.exportFamily();
    }
    
    if(fileLocation!=""){
        ofstream exportFile;
        exportFile.open(fileLocation+"/populationExport.hpe");
        exportFile << outputString.str();
        exportFile.close();
    }
    
}

void Population::importPopulation(std::string fileLocation, std::unordered_map<int, Building*> *allBuildings){
    std::ifstream in(fileLocation);
    std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::stringstream ss(s);
    std::string item;
    int numberOfFamilies = 0;
    std::getline(ss, item, '%');
    while (std::getline(ss, item, '%')) {
        //std::cout<<item<<std::endl;
        std::stringstream rows(item);
        std::string r;
        std::getline(rows, r, '\n');
        int homeID = std::stoi(r);
        std::getline(rows, r, '\n');
        int daycareID = std::stoi(r);
        //std::cout<<"Home ID: "<<homeID<<std::endl;
        //std::cout<<"Daycare ID: "<<daycareID<<std::endl;
        Family newFamily = Family(allBuildings->at(homeID), (daycareID != -1 ? static_cast<Daycare*> (allBuildings->at(daycareID)) : NULL), numberOfFamilies);
        std::getline(rows, r, '*');
        while(std::getline(rows, r, '*')){
            std::string p;
            //std::cout<<r<<std::endl;
            std::stringstream rs(r);
            std::getline(rs, p, '\n');
            int personId = std::stoi(p);
            std::getline(rs, p, '\n');
            int age = std::stoi(p);
            std::getline(rs, p, '\n');
            char gender = (p.c_str())[0];
            std::getline(rs, p, '\n');
            int schType = std::stoi(p);
            Person newPerson = Person(age, gender, -1, personId, schType, true);
            std::string s;
            std::getline(rs, s, '#');
            Schedule tmpSch = Schedule(schType, true);
            while(std::getline(rs, s, '#')){
                //Read in Schedule Element
                std::string jl;
                std::stringstream schStream(s);
                std::getline(schStream, jl, '\n');
                int jobLocationID = std::stoi(jl);
                std::string t;
                tmpSch.setJobLocation(jobLocationID);
                while(std::getline(schStream, t, '\n')){
                    //Read in Time Slot Elements
                    std::string ts;
                    std::stringstream timeStream(t);
                    std::getline(timeStream, ts, ',');
                    int buildingID = std::stoi(ts);
                    std::getline(timeStream, ts, ',');
                    char vistType = (ts.c_str())[0];
                    std::getline(timeStream, ts, ',');
                    int endTime = std::stoi(ts);
                    tmpSch.addTimeSlot(TimeSlot(buildingID, endTime, vistType));
                }
            }
            newPerson.setSchedule(tmpSch);
            TimeSlot *firstLoc = tmpSch.getCurrentTimeSlot();
            newFamily.addPerson(newPerson);
            allBuildings->at(firstLoc->getLocation())->addVisitor(newPerson.getID(), newFamily.getID());
            updateStatistics(age, schType, gender);

        }

        families.insert({numberOfFamilies, newFamily});
        numberOfFamilies++;
        numberOfFamiliesSizes[newFamily.getNumberOfPeople()-1]++;
        
    }
    
}
void Population::updateStatistics(int age, int scheduleType, char gender){
    int ageGroup = 0;
    if(gender == 'm'){
        numberOfMales++;
    }
    if(age>4 && age <14){
        ageGroup = 1;
    }else{
        if(age>13 && age <18){
            ageGroup =2;
        }else{
            if(age>17 && age <25){
                ageGroup = 3;
            }else{
                if(age > 24 && age < 45){
                    ageGroup = 4;
                }else{
                    if(age > 44 && age < 65){
                        ageGroup = 5;
                    }else{
                        if(age > 64){
                            ageGroup = 6;
                        }
                    }
                }
            }
        }
    }
    if(age<18 && age>4){
        numberOfStudentsAssignedGrade[age-5]++;
    }
    numberOfPeopleAges[ageGroup]++;

    if(scheduleType!= 3 && scheduleType != 4){
        numberOfPeopleAssignedSchedule[scheduleType]++;
    }else{
        if(scheduleType==3){
                //Employeed Schedule
                switch(ageGroup){
                    case 3:
                        numberOfPeopleAssignedSchedule[3]++;
                        break;
                    case 4:
                        numberOfPeopleAssignedSchedule[5]++;
                        break;
                    case 5:
                        numberOfPeopleAssignedSchedule[7]++;
                        break;
                    case 6:
                        numberOfPeopleAssignedSchedule[9]++;
                        break;
                    default:
                        break;
                }
        }else{
            //Employeed Schedule
            switch(ageGroup){
                case 3:
                    numberOfPeopleAssignedSchedule[4]++;
                    break;
                case 4:
                    numberOfPeopleAssignedSchedule[6]++;
                    break;
                case 5:
                    numberOfPeopleAssignedSchedule[8]++;
                    break;
                case 6:
                    numberOfPeopleAssignedSchedule[10]++;
                    break;
                default:
                    break;
            }
        }
    }
}

Population::~Population() {
	// TODO Auto-generated destructor stub
}


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

static std::default_random_engine generator;   //Random Generator for Distributions


Population::Population(int s, double *ageProbablities, double *familySizeProbablites, double mProb, double
    *scheduleProbablities, bool progressDisplay, int popSeed) {
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
    
    std::cout << "Creating Population of size: " << size << std::endl;
    int i=0;
    float oldRatio=0;
    
    //Reserve Space for Familes to prevent space resassignment midway (Avg Family size is 3)
    families.reserve(size/3);
    
    if(progressDisplay){
        printf("Percentage Complete: %3d%%", 0 );
        fflush(stdout);
    }
    
    //Initialize Arrays
    for(int i = 0; i < 13;i++){
        numberOfStudentsAssignedGrade[i]=0;

    }
    
    while( i < size ){
        int familySize= generateFamilySize();
       // std::cout<< "Family Size: "<< familySize << std::endl;

        Family newFamily = Family();
        for( int b = 0; b<familySize; b++ ){
            //Always first Person in Family must be an Adult
            int *ageInformation=determineAge(((b == 0) ? true : false ));
            Person newPerson= Person(ageInformation[0], determineGender(), -1, -1, i++, determineScheduleType(ageInformation[1]).getScheduleType());
            if(ageInformation[0]<18){
                numberOfStudentsAssignedGrade[ageInformation[0]-5]++;
            }
            newFamily.addPerson(newPerson);
        }
  
        if(newFamily.getHasYoungChild() && newFamily.getNonWorkingAdult()==nullptr){
            std::cout<<"Family Needs Day Care"<<std::endl;
            numberOfChildrenDaycare++;
        }
        
        families.push_back(newFamily);

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
}

int Population::getNumberOfFamilies(){
    return families.size();
    
}

void Population::setHomeLocationOfFamily(Building *home, int family){
    families.at(family).setHome(home);
    int* location = families.at(family).getHome()->getLocation();
    families.at(family).setLocation(location[0], location[1]);
}

Family* Population::getFamily(int family){
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
	//double x =1;
	if ( x<maleProbablity ) {
        numberOfMales++;
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
   // std::cout<< "Family Size (In Generator): "<< size << std::endl;
    numberOfFamiliesSizes[size]++;
    return size+1;
}


int *Population::determineAge(bool forceAdult){
    int ageGroup=-1;
    int *ageInformation=new int[2];
    if(forceAdult){
        //Adult Only (Using Uniform Distribution for Now)
        int age=(int)rand() % 82 + 18;
        if(age<25){
            numberOfPeopleAges[3]++;
            ageGroup=3;
        }
        else{
            if(age<45){
                numberOfPeopleAges[4]++;
                ageGroup=4;
            }
            else{
                if(age<65){
                    numberOfPeopleAges[5]++;
                    ageGroup=5;
                }
                else{
                    numberOfPeopleAges[6]++;
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
                numberOfPeopleAges[0]++;
                ageInformation[0]=(int)rand() % 4;
                ageInformation[1]=0;
                break;
            case 1:
                //5-13
                numberOfPeopleAges[1]++;
                ageInformation[0]= (int)rand() % 8 + 5;
                ageInformation[1]= 1;
                break;
            case 2:
                //14-17
                numberOfPeopleAges[2]++;
                ageInformation[0]=(int)rand() % 3 + 14;
                ageInformation[1]=2;
                break;
            case 3:
                //18-24
                numberOfPeopleAges[3]++;
                ageInformation[0]= (int)rand() % 6 + 18;
                ageInformation[1]=3;
                break;
            case 4:
                //25-44
                numberOfPeopleAges[4]++;
                ageInformation[0]= (int)rand() % 19 + 25;
                ageInformation[1]=4;
                break;
            case 5:
                //45-64
                numberOfPeopleAges[5]++;
                ageInformation[0]= (int)rand() % 19 + 45;
                ageInformation[1]=5;
                break;
            case 6:
                //65-Older
            
                numberOfPeopleAges[6]++;
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

Schedule Population::determineScheduleType(int ageGroup){
    double adultScheduleProbablities[2];
    Schedule tmp = Schedule();
    switch(ageGroup){
        case 0:
            //Young Child Schedule
            numberOfPeopleAssignedSchedule[0]++;
            tmp.setScheduleType(0);
            return tmp;
            break;
        case 1:
            //School Aged Child Schedule (5-13)
            numberOfPeopleAssignedSchedule[1]++;
            tmp.setScheduleType(1);
            return tmp;
            break;
        case 2:
            //School Aged Child Schedule (14-17)
            numberOfPeopleAssignedSchedule[2]++;
            tmp.setScheduleType(2);
            return tmp;
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
        switch(ageGroup){
            case 3:
                //18-24
                numberOfPeopleAssignedSchedule[4]++;
                break;
            case 4:
                //25-44
                numberOfPeopleAssignedSchedule[6]++;
                break;
            case 5:
                //45-64
                numberOfPeopleAssignedSchedule[8]++;
                break;
            case 6:
                //65-Older
                numberOfPeopleAssignedSchedule[10]++;
                break;
        }
        tmp.setScheduleType(4);
    }
    else{
        //Employeed
        switch(ageGroup){
            case 3:
                //18-24
                numberOfPeopleAssignedSchedule[3]++;
                break;
            case 4:
                //25-44
                numberOfPeopleAssignedSchedule[5]++;
                break;
            case 5:
                //45-64
                numberOfPeopleAssignedSchedule[7]++;
                break;
            case 6:
                //65-Older
                numberOfPeopleAssignedSchedule[9]++;
                break;
        }
        tmp.setScheduleType(3);
    }
    return tmp;
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
    int n=10;
    if(getNumberOfFamilies()<10){
        n=getNumberOfFamilies();
    }
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

Population::~Population() {
	// TODO Auto-generated destructor stub
}


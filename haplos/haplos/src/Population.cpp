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
#include "Person.h"
#include "Family.h"
#include <vector>

using namespace std;

Population::Population(int s, double ageProbablities[], double familySizeProbablites[], double mProb) {
	size=s;

    //Set Age Probablities
    age5To13Probablity=ageProbablities[0];
    age14To17Probablity=ageProbablities[0]+ageProbablities[1];
    age18To24Probablity=age14To17Probablity+ageProbablities[2];
    age25To44Probablity=age18To24Probablity+ageProbablities[3];
    age45To64Probablity=age25To44Probablity+ageProbablities[4];
    age65OrOlderProbablity=age45To64Probablity+ageProbablities[5];
    
    //Set Family Size Probablities
    familySize2Probablity=familySizeProbablites[0];
    familySize3Probablity=familySizeProbablites[0]+familySizeProbablites[1];
    familySize4Probablity=familySize3Probablity+familySizeProbablites[2];
    familySize5Probablity=familySize4Probablity+familySizeProbablites[3];
    familySize6Probablity=familySize5Probablity+familySizeProbablites[4];
    familySize7Probablity=familySize6Probablity+familySizeProbablites[5];

    
    //Set Probablity of Male (It is Assumed Probablity of Female is 1-MaleProbablity)
    maleProbablity=mProb;
    
    std::cout << "Creating Population of size: " << size << std::endl;
    int i=0;
    float oldRatio=0;
    
    //Reserve Space for Familes to prevent space resassignment midway (Avg Family size is 3)
    families.reserve(size/3);
    
    printf("Percentage Complete: %3d%%", 0 );
    fflush(stdout);
   
    while( i < size ){
        //Create A New Family
        int familySize= generateFamilySize();
        Family newFamily = Family();
        for( int b = 0; b<familySize-1; b++ ){
            Person newPerson= Person(determineAge(false), determineGender(), -1, -1, i);
            newFamily.addPerson(newPerson);
        }
        
        //Check that family has an Adult, if not create one.
        if(!newFamily.getHasAdult()){
            Person newPerson= Person(determineAge(true), determineGender(), -1, -1, i);
            newFamily.addPerson(newPerson);

        }
        else{
            Person newPerson= Person(determineAge(false), determineGender(), -1, -1, i);
            newFamily.addPerson(newPerson);
        }
        
        families.push_back(newFamily);
        i+=familySize;
        
        float ratio = i/(float)size;
        if(100*(ratio-oldRatio)>1){
            // Show the percentage complete.
            printf("\r");
            printf("Percentage Complete: %3d%%", (int)(ratio*100) );
            oldRatio=ratio;
            fflush(stdout);
        }

        numberOfFamilies++;
    }
    printf("\r");
    printf("Percentage Complete: %3d%%", 100 );
    fflush(stdout);

}

int Population::getNumberOfFamilies(){
    return numberOfFamilies;
    
}
void Population::setLocationOfFamily(int x, int y, int family){
    families.at(family).setLocation(x, y);
}

Family* Population::getFamily(int family){
    return &families.at(family);
}


char Population::determineGender(){
	//Generate random number
	double x = 1.0*rand()/RAND_MAX;
	//double x =1;
	if ( x<maleProbablity ) {
		return 'm';
	}
	else {
		return 'f';
	}

}


int Population::generateFamilySize(){
    double x= 1.0*rand()/RAND_MAX;
    if( x>familySize7Probablity ){
        return 7;
    }
    if( x>familySize6Probablity ){
        return 6;
    }
    if( x>familySize5Probablity ){
        return 5;
    }
    if( x>familySize4Probablity ){
        return 4;
    }
    if( x>familySize3Probablity ){
        return 3;
    }
    if( x>familySize2Probablity ){
        return 2;
    }
    return 1;
}


int Population::determineAge(bool forceAdult){

	double x = 1.0*rand()/RAND_MAX;
    
    if(forceAdult){
        return (int)rand() % 82 + 18;
    }
    
    if( x>age65OrOlderProbablity ){
        //65-100
        return (int)rand() % 35 +65;
    }
    if( x>age45To64Probablity ){
        //45-64
        return (int)rand() % 19 + 45;
    }
    if( x>age25To44Probablity ){
        //25-44
        return (int)rand() % 19 + 25;
    }
    if( x>age18To24Probablity){
        //18-24
        return (int)rand() % 6 + 18;
    }
    if( x>age14To17Probablity ){
        //14-17
        return (int)rand() % 3 + 14;
    }
    if(x>age5To13Probablity){
        //5-13
        return (int)rand() % 8 + 5;
    }
    else{
        //0-5
        return (int)rand() % 5;
    }
}

void Population::displayStatistics(){
	//Gender Counters
    int numOfFemales=0;
	int numOfMales=0;
    
    //Age Counters
	int age0=0;
	int age1=0;
	int age2=0;
	int age3=0;
	int age4=0;
	int age5=0;
    int age6=0;
    
    //Family Counters
    int fsize1=0;
    int fsize2=0;
    int fsize3=0;
    int fsize4=0;
    int fsize5=0;
    int fsize6=0;
    int fsize7=0;

    //Get Demographic Information
	for (int i=0;i<numberOfFamilies;i++){
        Person* members=families[i].getAllPersons();
        int numberOfPeople= families[i].getNumberOfPeople();
        //Get Population Demographics
        for(int p=0; p<numberOfPeople;p++){

            if (members[p].getGender()=='f') {
                numOfFemales++;
            }
            else {
                numOfMales++;
            }
            
            
            int age = members[p].getAge();
            if(age<5){
                age0++;
            }
            if(5<=age && age<14){
                age1++;
            }
            if(14<=age && age<18){
                age2++;
            }
            if(18<=age && age<25){
                age3++;
            }
            if(25<=age && age<45){
                age4++;
            }
            if(45<=age && age<65){
                age5++;
            }
            if(65<=age){
                age6++;
            }
        }
        
        //Get Family Demographics
        switch(numberOfPeople){
            case 1:
                fsize1++;
                break;
            case 2:
                fsize2++;
                break;
            case 3:
                fsize3++;
                break;
            case 4:
                fsize4++;
                break;
            case 5:
                fsize5++;
                break;
            case 6:
                fsize6++;
                break;
            case 7:
                fsize7++;
                break;
            default:
                //Invalid Family
                break;
            
        }
	}
    cout<< std::fixed<< std::setprecision(5);
    std::cout <<"--------Family--------" << std::endl;
    std::cout << "Total Number of Families: " << numberOfFamilies << std::endl;
    std::cout << "Size 1:  \t" << fsize1 << " \t" << (fsize1/(double)numberOfFamilies) << "\t(Expected " << familySize2Probablity << ")" << std::endl;
    std::cout << "Size 2:  \t" << fsize2 << " \t" << (fsize2/(double)numberOfFamilies) << "\t(Expected " << familySize3Probablity-familySize2Probablity << ")" << std::endl;
    std::cout << "Size 3:  \t" << fsize3 << " \t" << (fsize3/(double)numberOfFamilies) << "\t(Expected " << familySize4Probablity-familySize3Probablity << ")" << std::endl;
    std::cout << "Size 4:  \t" << fsize4 << " \t" << (fsize4/(double)numberOfFamilies) << "\t(Expected " << familySize5Probablity-familySize4Probablity << ")" << std::endl;
    std::cout << "Size 5:  \t" << fsize5 << " \t" << (fsize5/(double)numberOfFamilies) << "\t(Expected " << familySize6Probablity-familySize5Probablity << ")" << std::endl;
    std::cout << "Size 6:  \t" << fsize6 << " \t" << (fsize6/(double)numberOfFamilies) << "\t(Expected " << familySize7Probablity-familySize6Probablity << ")" << std::endl;
    std::cout << "Size 7:  \t" << fsize7 << " \t" << (fsize7/(double)numberOfFamilies) << "\t(Expected " << 1-familySize7Probablity << ")" << std::endl;
    
    std::cout <<"--------Population--------" << std::endl;
	std::cout << "----Gender----" << std::endl;
	std::cout << "Males:   \t" << numOfMales <<" \t" << (numOfMales/(double)size) << "\t(Expected " << maleProbablity << ")" <<std::endl;
	std::cout << "Females: \t" << numOfFemales <<" \t" << (numOfFemales/(double)size) << "\t(Expected " << 1-maleProbablity << ")" << std::endl;
	std::cout << "----Age----"<<endl;

	std::cout << "Under Age 5: \t" <<age0<< " \t"<<(age0/(double)size) << "\t(Expected " << age5To13Probablity << ")" << std::endl;
	std::cout << "Age 5-13:    \t" <<age1<< " \t"<< (age1/(double)size) << "\t(Expected " << age14To17Probablity-age5To13Probablity << ")" << std::endl;
	std::cout << "Age 14-17:   \t" <<age2<< " \t"<< (age2/(double)size) << "\t(Expected " << age18To24Probablity-age14To17Probablity << ")" << std::endl;
	std::cout << "Age 18-24:   \t" <<age3<< " \t"<< (age3/(double)size) << "\t(Expected " << age25To44Probablity-age18To24Probablity << ")" << std::endl;
	std::cout << "Age 25-44:   \t" <<age4<< " \t"<< (age4/(double)size) << "\t(Expected " << age45To64Probablity-age25To44Probablity << ")" << std::endl;
	std::cout << "Age 45-64:   \t" <<age5 << " \t" << (age5/(double)size) << "\t(Expected " << age65OrOlderProbablity-age45To64Probablity << ")" << std::endl;
	std::cout << "Over Age 65: \t" <<age6 << " \t" << (age6/(double)size) << "\t(Expected " << 1-age65OrOlderProbablity << ")" << std::endl;

}
Population::~Population() {
	// TODO Auto-generated destructor stub
}


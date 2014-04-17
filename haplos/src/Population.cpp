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
#include "Person.h"
#include <vector>

using namespace std;

Population::Population(int s, double a2024Prob, double a2534Prob, double a3549Prob, double a5064Prob, double a65OlderProb, double mProb) {
	// TODO Auto-generated constructor stub
	size=s;
    age2024Prob=a2024Prob;
    age2534Prob=a2534Prob;
    age3549Prob=a3549Prob;
    age5064Prob=a5064Prob;
    age65OlderProb=a65OlderProb;
    maleProb=mProb;
    
    std::cout << "Creating Population of size: " << s << std::endl;
	for (int i=0;i<s;i++) {
		population.push_back(Person(determineAge(), determineGender(), -1, -1));
	}
}

void Population::setLocationOfPerson(int x, int y, int person){
	population.at(person).setLocation(x, y);
}

char Population::determineGender(){
	//double femaleProb=0.511796988;
	//Generate random number
	double x = 1.0*rand()/RAND_MAX;
	//double x =1;
	if (x<maleProb) {
		return 'm';
	}
	else {
		return 'f';
	}

}
int Population::determineAge(){

	double x = 1.0*rand()/RAND_MAX;
    
	if (x<age2024Prob) {
		//20-24
		return 1;
	}else{
		if (x<age2534Prob) {
			//25-34
			return 2;
		}
        else {
			if (x<age3549Prob) {
				//35-49
				return 3;
			}
            else {
				if (x<age5064Prob) {
					//50-64
					return 4;
				}
				else {
					if (x<age65OlderProb) {
						//Older 65
						return 5;
					}
					else {
						//Under 20
						return 0;
					}
				}
			}
		}
	}
}

void Population::displayStatistics(){

	int numOfFemales=0;
	int numOfMales=0;
	int age0=0;
	int age1=0;
	int age2=0;
	int age3=0;
	int age4=0;
	int age5=0;
	for (int i=0;i<size;i++) {
		if (population[i].getGender()=='f') {
			numOfFemales++;
		}
		else {
			numOfMales++;
		}
        
		switch (population[i].getAge()) {
			case 0:
				age0++;
				break;
			case 1:
				age1++;
				break;
			case 2:
				age2++;
				break;
			case 3:
				age3++;
				break;
			case 4:
				age4++;
				break;
			case 5:
				age5++;
				break;
		}
	}
	cout<<"----Gender----"<<endl;
	cout<<"Males: "<<numOfMales<<" "<<(numOfMales/(double)size)<<endl;
	cout<<"Females: "<<numOfFemales<<" "<<(numOfFemales/(double)size)<<endl;
	cout<<"----Age----"<<endl;

	cout<<"Under 20: "<<age0<<" "<<(age0/(double)size)<<endl;
	cout<<"Under 20-24: "<<age1<<" "<<(age1/(double)size)<<endl;
	cout<<"Under 25-34: "<<age2<<" "<<(age2/(double)size)<<endl;
	cout<<"Under 35-49: "<<age3<<" "<<(age3/(double)size)<<endl;
	cout<<"Under 50-64: "<<age4<<" "<<(age4/(double)size)<<endl;
	cout<<"Under 65 or Older: "<<age5<<" "<<(age5/(double)size)<<endl;


}
Population::~Population() {
	// TODO Auto-generated destructor stub
}


/*
 * Population.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: schmidee
 */

#include "Population.h"
#include <cstdlib>
#include <iostream>
#include "Person.h"

using namespace std;

Population::Population(int s) {
	// TODO Auto-generated constructor stub
	population=new Person[s];
	size=s;
	cout<<"Creating Population of size: "<<s<<endl;
	for(int i=0;i<s;i++){
		population[i]= Person(determineAge(), determineGender(), -1, -1);
	}
}

void Population::setLocationOfPerson(int x, int y, int person){
	population[person].setLocation(x, y);
}

char Population::determineGender(){
	double maleProb=0.488203012;
	//double femaleProb=0.511796988;
	//Generate random number
	//double x = 1.0*rand()/RAND_MAX;
	double x =1;
	if(x<maleProb){
		return 'm';
	}
	else{
		return 'f';
	}

}
int Population::determineAge(){
	double age2024Prob=0.066147942;
	double age2534Prob=0.188365123;
	double age3549Prob=0.390763094;
	double age5064Prob=0.593538823;
	double age65OlderProb=0.734137309;
	double x = 1.0*rand()/RAND_MAX;
	if(x<age2024Prob){
		//20-24
		return 1;
	}else{
		if(x<age2534Prob){
			//25-34
			return 2;
		}else{
			if(x<age3549Prob){
				//35-49
				return 3;
			}else{
				if(x<age5064Prob){
					//50-64
					return 4;
				}
				else{
					if(x<age65OlderProb){
						//Older 65
						return 5;
					}
					else{
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
	for(int i=0;i<size;i++){
		if(population[i].getGender()=='f'){
			numOfFemales++;
		}
		else{
			numOfMales++;
		}
		switch(population[i].getAge()){
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


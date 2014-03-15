/*
 * Location.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: schmidee
 */

#include "Location.h"
#include <cstdlib>

using namespace std;

Location::Location(){
	numberOfPeople=0;
	maxPopulation=0;
	coordinates= new int[2];
	coordinates[0]=-1;
	coordinates[1]=-1;
}
Location::Location(int x, int y, float m) {
	// TODO Auto-generated constructor stub
	numberOfPeople=0;
	maxPopulation=m;
	coordinates= new int[2];
	coordinates[0]=x;
	coordinates[1]=y;
}

int Location::getCurrentPopulation() const {
	return numberOfPeople;
}

bool Location::isFull(){
	if(numberOfPeople>=(int)maxPopulation){
		return true;
	}
	else{
		return false;
	}
}
float Location::getMaxPopulation(){
	return maxPopulation;
}
int* Location::getCoordinates(){
	return coordinates;
}
void Location::addPerson(){
	numberOfPeople++;
}

void Location::removePerson(){
	numberOfPeople--;
}

Location &Location::operator = (const Location &p){
	if(this!=&p){
		numberOfPeople=p.numberOfPeople;
		maxPopulation=p.maxPopulation;
		int *coordinates=p.coordinates;
	}
	return *this;
}
Location::~Location() {
	// TODO Auto-generated destructor stub

}


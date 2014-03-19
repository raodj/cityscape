/*
 * Population.h
 *
 *  Created on: Mar 4, 2014
 *      Author: schmidee
 */

#ifndef POPULATION_H_
#define POPULATION_H_
#include "Person.h"

//using namespace std;

class Population {
public:
	Population(int size);
	void displayStatistics();
	void setLocationOfPerson(int x, int y, int person);
	virtual ~Population();
private:
	char determineGender();
	int determineAge();
	int size;
	Person *population;
};


#endif /* POPULATION_H_ */

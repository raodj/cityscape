/*
 * Location.h
 *
 *  Created on: Mar 4, 2014
 *      Author: schmidee
 */

#ifndef LOCATION_H_
#define LOCATION_H_

using namespace std;

class Location {
public:
	Location();
	Location(int x, int y, float m);
	void addPerson();
	int getCurrentPopulation() const;
	int* getCoordinates();
	Location &operator=(const Location &p);
	float getMaxPopulation();
	bool isFull();
	void removePerson();
	virtual ~Location();
private:
	int numberOfPeople;
	int* coordinates;
	float maxPopulation;

};


#endif /* LOCATION_H_ */

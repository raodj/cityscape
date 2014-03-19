/*
 * Family.cpp
 *
 *  Created on: Mar 18, 2014
 *      Author: schmidee
 */

#include "Family.h"

using namespace std;

Family::Family(int s) {
	// TODO Auto-generated constructor stub
    members= new Person[s];
    numberOfPeople=s;
}

Family::~Family() {
	// TODO Auto-generated destructor stub
    delete[] members;
}

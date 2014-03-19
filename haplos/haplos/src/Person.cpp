/*
 * Person.cpp
 *
 *  Created on: Feb 2, 2013
 *      Author: schmidee
 */

#include "Person.h"


int age=0;
char gender=' ';

Person::Person(){
	age=-1;
	gender='n';
	location[0]=-1;
	location[1]=-1;
}

Person::Person(int a, char g, int x, int y) {
	// TODO Auto-generated constructor stub
	age=a;
	gender=g;
	location[0]=x;
	location[1]=y;
}

void Person::setLocation(int x, int y){

	location[0]=x;
	location[1]=y;
}
Person::Person(const Person &p){
	age=0;
	gender=' ';
	age=p.age;
	gender=p.gender;
	location[0]=p.location[0];
	location[1]=p.location[1];
}

Person &Person::operator = (const Person &p){
	if(this!=&p){
		age=0;
		gender=' ';
		age=p.age;
		gender=p.gender;
	}
	return *this;
}

char Person::getGender(){
	return gender;
}

void Person::setGender(char g){
	gender=g;
}

int Person::getAge(){
	return age;
}

int* Person::getLocation(){
	return location;
}

Person::~Person() {
	// TODO Auto-generated destructor stub
}



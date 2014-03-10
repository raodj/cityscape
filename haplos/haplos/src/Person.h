/*
 * Person.h
 *
 *  Created on: Feb 2, 2013
 *      Author: schmidee
 */

#ifndef PERSON_H_
#define PERSON_H_
using namespace std;


class Person {
public:
	Person();
	Person(int age , char gender, int x, int y);
	virtual ~Person();
	Person(const Person &p);
	Person &operator=(const Person &p);
	char getGender(void);
	void setGender(char);
	int* getLocation();
	void setLocation(int x, int y);
	int getAge(void);
private:
	int age;
	int gender;
	int* location;
};

#endif /* PERSON_H_ */

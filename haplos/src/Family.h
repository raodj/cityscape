/*
 * Family.h
 *
 *  Created on: Mar 18, 2014
 *      Author: schmidee
 */

#ifndef FAMILY_H_
#define FAMILY_H_

namespace std {

class Family {
public:
	Family(int size);
    People* getMemebers();
    
	virtual ~Family();
    
private:
    Person *members;
    int numberOfPeople;
    
    
};

} /* namespace std */
#endif /* FAMILY_H_ */

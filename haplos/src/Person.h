

#ifndef PERSON_H_
#define PERSON_H_
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
#include "Schedule/Schedule.h"
#include "Schedule/TimeSlot.h"

#include <vector>
#include <unordered_map>
class Schedule;
class Person {
    /** A class specifically repersenting a single person.
     */
public:
    /** The default constructor for this class.
     */
	Person();
    /** The alternative constructor for this class.
     
     \param[in] age Age of person.
     \param[in] gender Gender of person.
     \param[in] b building id of where person current is
     \param[in] id_num Unique ID of person.
     \param[in] scheduleType Type of schedule assigned (0=young child, 1= school aged child, 2=older school aged child, 3=working adult, 4=non-working adult)
     */
	Person(int age , char gender, int b, int id_num, int scheduleType, bool goToJobLocation);
    /** The copy constructor for this class.
     
     \param[in] p Another person.
     */
	Person(const Person &p);
    
	Person &operator=(const Person &p);
	
    /** Return the gender of person.
     
     \return 'f' if Female and 'm' if male.
     */
    char getGender(void);
    
    /** Set gender of person.
     
     \param[in] g Gender of person ('f' for female and 'm' for male).
     */
	void setGender(char g);
    
    /** Return the coordinates of the current location of entity
     
     \return An array consisting of [rows, cols]
     */
	int getLocation();
    
    /** Set the locaton of a person.
     
     \param[in] b building of id where person is
     */
	void setLocation(int b);
    
    /** Return the age of a person.
     
     \return Age of the person.
     */
	int getAge(void);
    
    /** Return the ID number of a person.
     
     \return ID Number of the person.
     */
    int getID(void);
    
    /** Set the Schedule of a person.
     
     \param[in] s is the new schedule
     */
    void setSchedule(Schedule s);
    
    /** Return the Schedule of a person.
     
     \return Pointer to the schedule object
     */
    Schedule* getSchedule();
    
    /** Set the Custom Attribute of a person.
     
     \param[in] key the name of the custom attribute to set.
     \param[in] value the value of the custom attribute.
     */
    void setCustomAttribute(std::string key, std::string value);
    
    /** Return the value of the custom attribute
     
     \param[in] key the custom attribute to retrive.
     \return The value of the custom attribute.
     */
    std::string getCustomAttribute(std::string key);
    
    /** Returns a Human Readable Version on Information about the Person and Schedule.
     
     \return Human Readable String of Information about the Person
     */
    
    std::string toString();
    
    std::string exportPerson();
    
    /** Returns a CSV version of Informationabout the Person. This inclues ID number, 
        Age, Gender and Schedule Type.
     
     \return A CSV string containg information about the person.
     */
    std::string toCSV();
    
    TimeSlot* getCurrentTimeSlot();
    
    TimeSlot* updateToNextTimeStep();

    /**
     The destructor.
     
     Currently the destructor does not have any specific task to
     perform in this class.  However, it is defined for adherence
     with conventions and for future extensions.
     */
    virtual ~Person();
private:
	int age;
	char gender;
    int id_num;
    int currentLocation;
    Schedule schedule;
    std::unordered_map<std::string, std::string> customAttributes;
};

#endif /* PERSON_H_ */

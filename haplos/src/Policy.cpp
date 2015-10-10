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
#include "Policy.h"
#include "Population.h"
#include <iterator>
Policy::Policy(){

}

//Required Method
void Policy::setupCustomAttributes(Population *p){
    int numberOfFamilies = p->getNumberOfFamilies();
    int infectedID = 55;
    
    for (int i = 0; i < numberOfFamilies; i++){
        Family *f = p->getFamily(i);
        int numberOfPeople = f->getNumberOfPeople();
        for(int j =0 ; j < numberOfPeople; j++){
            Person *p = f->getPerson(j);
            if(p->getID() == infectedID){
                p->setCustomAttribute("infected", "0");

            }else{
                p->setCustomAttribute("infected", "");
            }
            
            p->setCustomAttribute("immune", "0");
        }
        
    }
}

//Required Method
void Policy::updatePopulation(Population *p, std::unordered_map<int, Building*> *allBuildings, int currentTime){
    int numberOfFamilies = p->getNumberOfFamilies();
    for (int i = 0; i < numberOfFamilies; i++){
        Family *f = p->getFamily(i);
        int numberOfPeople = f->getNumberOfPeople();
        for(int j =0 ; j < numberOfPeople; j++){
            Person *p = f->getPerson(j);
            if(p->getCustomAttribute("infected") != ""){
                TimeSlot *t = p->getCurrentTimeSlot();
                std::unordered_map<int, Person *> contacts = getPossibleContacts(t, f->getHome()->getID(), allBuildings);
                if(contacts.size()>1){
                    std::unordered_map<int, Person *>::iterator it = contacts.find(p->getID());
                    if(std::next(it) != contacts.end() && std::next(it)->second->getCustomAttribute("immune") != "1"
                       && std::next(it)->second->getCustomAttribute("infected") == "" ){
                        std::next(it)->second->setCustomAttribute("infected", std::to_string(currentTime));

                    }
                   
                    /*(std::prev(it) != contacts->begin()){
                        std::prev(it)->second->setCustomAttribute("infected", currentTime)
                        
                    }*/
                }
                
                if(std::stoi(p->getCustomAttribute("infected"))+432 <= currentTime){
                    p->setCustomAttribute("immune", "1");
                    p->setCustomAttribute("infected", "");
                    
                }
                
            }
            
        }
        
    }

}

//Required Method
int Policy::getCustomFileTypeData(Location *l, std::string fileType){
    std::unordered_map<int,Person*> *people = l->getPeople();
    int total = 0;
    if(fileType == "immune"){
        for(auto p = people->begin(); p != people->end(); p++){
            if(p->second->getCustomAttribute("immune") == "1"){
                total++;
            }
        }
    }else{
        for(auto p = people->begin(); p != people->end(); p++){
            if(p->second->getCustomAttribute("infected") != ""){
                total++;
            }
        }
    }
    return total;
}


void Policy::scheduleModification(Family f){
    
    
}

std::unordered_map<int, Person *> Policy::getPossibleContacts(TimeSlot *t, int homeNumber, std::unordered_map<int, Building*> *allBuildings){
    int locationID =t->getLocation();
    char visType = t->getVisitorType();
    std::unordered_map<int, Person *> returnVector;
    if(visType == 'V' || visType == 'T' || visType == 'H'){
        returnVector = allBuildings->at(locationID)->getVisitors();
    }
    
    if(visType == 'E' || visType == 'W'){
        returnVector = allBuildings->at(locationID)->getEmployees();
    }
    
    if(visType == 'C'){
        returnVector = (static_cast<TransportHub *>(allBuildings->at(locationID)))->getPrivateTransport(homeNumber);

    }
    
    if(visType == 'S'){
        returnVector = (static_cast<School *>(allBuildings->at(locationID)))->getStudents();

    }
    
    if(visType == 'D'){
        returnVector = (static_cast<Daycare *>(allBuildings->at(locationID)))->getChildren();

    }
    
    if(visType == 'P'){
        returnVector= (static_cast<Medical *>(allBuildings->at(locationID)))->getPatients();
    }

    return returnVector;
    
}


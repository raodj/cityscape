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
#include <cstdlib>
#include <algorithm>

Policy::Policy(){
    //Set Policy Seed
    int seed = time(0);
    generator.seed(seed);
    std::cout<<"Policy Seed: "<<seed<<std::endl;

}

//Required Method
void Policy::setupCustomAttributes(Population *p){
    int numberOfFamilies = p->getNumberOfFamilies();
    int infectiousID = rand() % p->getPopulationSize();
    std::cout<<"Initial Infection: "<<infectiousID<<std::endl;
    for (int i = 0; i < numberOfFamilies; i++){
        Family *f = p->getFamily(i);
        int numberOfPeople = f->getNumberOfPeople();
        for(int j =0 ; j < numberOfPeople; j++){
            Person *p = f->getPerson(j);
            if(p->getID() == infectiousID){
                setExposedStatus(p, 0);

            }else{
                p->setCustomAttribute("exposed", "");

            }
            p->setCustomAttribute("infectious", "");
            p->setCustomAttribute("immune", "0");
        }
        
    }
}


void Policy::determineExposedStatus(Person *p, int currentTime){
    //SEIR Model from Nsoesie etc (2012)
    
    //Transmission Probablity Per Second of Exposure
    double t = 0.000046;
    
    //1 Haplos Time Unit = 10 Minutes
    int haplosUnitToSeconds = 36000;
    
    double exposedProbablity  = std::pow((1 - t),haplosUnitToSeconds);
    double exposed =  (double)(rand() / (double)RAND_MAX);
    if(exposed<exposedProbablity){
        //Person was exposed successfully
        //Duration of Exposed State (1,2,3 Days)
        setExposedStatus(p, currentTime);
    }
}

void Policy::setExposedStatus(Person *p, int currentTime){
    std::discrete_distribution<int> exposedToinfectious{0.3,0.5,0.2};
    int haplosDay = 144;
    int blah = exposedToinfectious(generator);
    int exposedLength = (blah*haplosDay)+currentTime;
    p->setCustomAttribute("exposed", std::to_string(exposedLength));
}

void Policy::setInfectiousStatus(Person *p, int currentTime){
    //SEIR Model from Nsoesie etc (2012)

    //Duration of Infectious State (3,4,5,6 Days)
    std::discrete_distribution<int> infectiousToImmune{0.3,0.4,0.2,0.1};
    
    int haplosDay = 144;
    int blah =infectiousToImmune(generator);
    int infectionLength = ((blah+3)*haplosDay)+currentTime;
    p->setCustomAttribute("infectious", std::to_string(infectionLength));
    
}

//Required Method
void Policy::updatePopulation(Population *p, std::unordered_map<int, Building*> *allBuildings, int currentTime){
    int numberOfFamilies = p->getNumberOfFamilies();
    for (int i = 0; i < numberOfFamilies; i++){
        Family *f = p->getFamily(i);
        int numberOfPeople = f->getNumberOfPeople();
        for(int j =0 ; j < numberOfPeople; j++){
            Person *p = f->getPerson(j);
            if(p->getCustomAttribute("infectious") != ""){
                //Determine if Person has infectious other people
                TimeSlot *t = p->getCurrentTimeSlot();
                std::unordered_map<int, Person *> contacts = getPossibleContacts(t, f->getHome()->getID(), allBuildings);
                if(contacts.size()>2){
                    //std::cout<<"Contact Size: "<<contacts.size()<<std::endl;
                    int numberOfContacts = 1;
                    int currentContact = 0;
                    while (currentContact < numberOfContacts){
                       // std::cout<<"CurrentContacts: "<<currentContact<<std::endl;
                        auto randomContact = std::next(std::begin(contacts), (rand() % contacts.size()-1));
                        while(randomContact->second->getID() == p->getID()){
                            //std::cout<< (rand() % (contacts.size()-1))<< std::endl;
                            randomContact = std::next(std::begin(contacts), (rand() % (contacts.size()-1)));
                            //std::cout<<randomContact->second->getID()<<" "<<p->getID()<<std::endl;
                        }

                        ///std::cout<<p->getID()<<" "<<randomContact->second->getID()<<std::endl;
                        if(randomContact->second->getCustomAttribute("immune") != "1"
                           && randomContact->second->getCustomAttribute("exposed") == "" ){
                                //Update Infection Status of Contact
                                determineExposedStatus(randomContact->second, currentTime);
                        }
                        currentContact++;
                    }
                }
                //std::cout<<"Done"<<std::endl;
                //Update Infected Status if Needed
                //std::cout<<std::stoi(p->getCustomAttribute("infectious")) <<" "<<currentTime<< std::endl;
                if(std::stoi(p->getCustomAttribute("infectious")) <= currentTime){
                    p->setCustomAttribute("immune", "1");
                }
                
            }//end of Infectious check
            
            //Updated Exposed Status if Needed
            if(p->getCustomAttribute("exposed")!= "" && p->getCustomAttribute("infectious") == ""){
                if(std::stoi(p->getCustomAttribute("exposed")) <= currentTime){
                    setInfectiousStatus(p, currentTime);
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
            if(p->second->getCustomAttribute(fileType) != ""){
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
    std::unordered_map<int, Person *> tmp;
    
    if(visType == 'V' || visType == 'H' || visType == 'T' || visType == 'S' || visType == 'D' || visType =='P' || visType =='P'){
        tmp = allBuildings->at(locationID)->getVisitors();
        returnVector.insert(tmp.begin(), tmp.end());
    }
   
    
    if(visType == 'E' || visType == 'V' || visType =='S' || visType == 'D' || visType == 'P' || visType == 'W'){
        tmp =  allBuildings->at(locationID)->getEmployees();
        returnVector.insert(tmp.begin(), tmp.end());
    }
    
    if(visType == 'C'){
        tmp = (static_cast<TransportHub *>(allBuildings->at(locationID)))->getPrivateTransport(homeNumber);
        returnVector.insert(tmp.begin(), tmp.end());
    }
    
    if(visType == 'S' || allBuildings->at(locationID)->getType() == 'S' ){
        tmp = (static_cast<School *>(allBuildings->at(locationID)))->getStudents();
        returnVector.insert(tmp.begin(), tmp.end());

    }
    
    if(visType == 'D' || allBuildings->at(locationID)->getType() == 'D' ){
        tmp = (static_cast<Daycare *>(allBuildings->at(locationID)))->getChildren();
        returnVector.insert(tmp.begin(), tmp.end());

    }
    
    if(visType == 'P' || allBuildings->at(locationID)->getType() == 'M' ){
        tmp= (static_cast<Medical *>(allBuildings->at(locationID)))->getPatients();
        returnVector.insert(tmp.begin(), tmp.end());

    }

    return returnVector;
    
}


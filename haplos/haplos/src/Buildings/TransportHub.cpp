//------------------------------------------------------------
//
// This file is part of HAPLOS availabe off the website at
// <http://pc2lab.cec.miamiOH.edu/haplos>
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

#include <algorithm>
#include "TransportHub.h"
#include "Building.h"

TransportHub::TransportHub() : Building(){
    
}


TransportHub::TransportHub(int i, int x, int y) : Building('T', i, x, y, -1, -1){

}

TransportHub::TransportHub(const TransportHub &t) : Building(t){
    this->privateTransport.clear();
    if(t.privateTransport.size()>0){
        this->privateTransport.insert(t.privateTransport.begin(), t.privateTransport.end());
    }
}

TransportHub &TransportHub::operator = (const TransportHub &t){
    if (this!=&t) {
        Building::operator = (t);
        this->privateTransport.clear();
        if(t.privateTransport.size()>0){
            this->privateTransport.insert(t.privateTransport.begin(), t.privateTransport.end());
        }
    }
    return *this;
}

int TransportHub::getTotalNumberOfPeople(){
    int total =0;
    
    for(std::unordered_map<int, std::vector<int>>::iterator i = privateTransport.begin(); i != privateTransport.end(); i++){
        
        total += (*i).second.size();
    }
    
    return total+Building::getTotalNumberOfPeople();
}

void TransportHub::removePrivateTransport(int pID, int familyID){
    std::vector<int>::iterator it;
    it = std::find(privateTransport[familyID].begin(), privateTransport[familyID].end(), pID);
    if(it != privateTransport[familyID].end()){
        privateTransport[familyID].erase(it);
    }
}

void TransportHub::addPrivateTransport(int pID, int familyID){
    privateTransport[familyID].push_back(pID);
}

std::vector<int> TransportHub::getPrivateTransport(int familyID){
    return privateTransport[familyID];
}

TransportHub::~TransportHub(){
    
}

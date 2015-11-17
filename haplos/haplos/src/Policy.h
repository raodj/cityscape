#ifndef POLICY_H_
#define POLICY_H_
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

#include "Population.h"
#include "Files/ConfigFile.h"
#include "Schedule/Schedule.h"
#include "Schedule/ScheduleGenerator.h"
#include "Family.h"
#include "Person.h"
#include "Location.h"
#include <random>
#include <cstdlib>

class Policy {
    
public:
    Policy();
    void setConfigFile(ConfigFile *configuration);
    void setupCustomAttributes(Population &p);
    void updatePopulation(Population &p,
                          std::unordered_map<int, Building*> &allBuildings,
                          int currentTime,
                          ScheduleGenerator *scheduleGen);
    int getCustomFileTypeData(Location *l, std::string fileType);
    std::unordered_map<int, int> getPossibleContacts(TimeSlot *t,
                                                     int familyID,
                                                     std::unordered_map<int, Building*> &allBuildings);
    void modifySchedule(Family *f, Person *p, ScheduleGenerator *scheduleGenerator, int currentTIme);
    void revertToOldSchedule(Family *f, Person *p, int currentTime);
    
private:
    void setExposedStatus(Person *p, int currentTime);
    void setInfectiousStatus(Person *p, int currentTime);
    void determineExposedStatus(Person *p, int currentTime);
    std::default_random_engine generator;
    std::unordered_map<int, Schedule> oldSchedules;
    ConfigFile *configuration;

};


#endif /* POLICY_H_ */

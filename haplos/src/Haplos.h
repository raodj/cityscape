
#ifndef HAPLOS_H_
#define HAPLOS_H_

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


#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

#include "SedacReader.h"
#include "ImageGeneration/ImageGen.h"
#include "Location.h"
#include "Population.h"
#include "ImageGeneration/XFigImageGenerator.h"
#include "Files/ConfigFile.h"
#include "Files/TimelineFile.h"
#include "Files/ImageFileGenerator.h"
#include "Schedule/ScheduleGenerator.h"
#include "Buildings/BuildingGenerator.h"
#include "Policy.h"


#include "Buildings/Medical.h"
#include "Buildings/School.h"
#include "Buildings/Business.h"
#include "Buildings/Daycare.h"

class Haplos {
public:
    Haplos(std::string configFileLocation, bool produceImages, bool progressDisplay, bool exportFiles, Policy p);
private:
    Policy policy;
    std::vector< School > schoolBuildings;
    std::vector< Business > businessBuildings;
    std::vector< Daycare > daycareBuildings;
    std::vector< Medical > medicalBuildings;
    std::vector< Building > otherBuildings;
    std::unordered_map<int, Building*> allBuildings;
    
    std::vector< std::vector < Location > > densityData;
    
    std::default_random_engine generator;
    std::string outputFolder;
    std::string imageFileLocationPath;
    std::string familyFileLocationPath;
    std::string saveLocationPath;
    ConfigFile configuration;
    bool produceImages;
    bool progressDisplay;
    bool exportFiles;
    int numberOfBuildings=0;


};

#endif /* HAPLOS_H_ */

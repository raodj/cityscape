#ifndef HAPLOS_TIMELINE_FILE_H
#define HAPLOS_TIMELINE_FILE_H

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
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <locale>

class TimelineFile {

    public:
    
        /** The default constructor for this class.
         
         */
        TimelineFile();
        /** Constructor with File Location Set
         
         \param[in] FileLocation Location of Timeline File
         */
        TimelineFile(const std::string& fileLocation, std::string customFileTypes);
    
        TimelineFile(const TimelineFile &p);

    
        std::vector<std::string> getFilesToProduceAt(int time);
    
        std::string displayTimeLineInformation();
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~TimelineFile();
    
    private:
        void getFilesToReturn(std::vector<std::string> filesToProduce, std::unordered_map<std::string, bool> *fileProduced);
        std::string lowerCaseString(std::string s);
        std::string timeLineLocation;
        int timeStepInterval = 0;
        int dayInterval = 0;
        int weekInterval = 0;
        int monthInterval = 0;
        int yearInterval = 0;
        std::vector<std::string> allowedFileTypes { "all_buildings",
                                                    "daycare_buildings",
                                                    "medical_buildings",
                                                    "school_buildings",
                                                    "business_buildings",
                                                    "homes",
                                                    "population_density"};
        std::vector<std::string> fileTimeStep;
        std::vector<std::string> fileDay;
        std::vector<std::string> fileWeek;
        std::vector<std::string> fileMonth;
        std::vector<std::string> fileYear;
        std::map<int, std::vector<std::string>> singleFiles;


    
    
};

#endif

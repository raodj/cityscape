#ifndef HAPLOS_IMAGE_FILE_CPP
#define HAPLOS_IMAGE_FILE_CPP

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

#include "ImageFileGenerator.h"
#include <climits>
#include <sys/types.h>
#include <sys/stat.h>

ImageFileGenerator::ImageFileGenerator(){

}

ImageFileGenerator::ImageFileGenerator(std::vector< std::vector < Location > > *locationData, std::string outputLocation){
    this->locationData= locationData;
    this->outputLocation=outputLocation;
}


bool ImageFileGenerator::makeCustomFile(std::string fileType, std::string fileName, std::vector<std::string> headerInformation, Policy *policy){
    std::ostringstream outputString;
    int maxVal=0;
    int minVal=INT_MAX;
    int  total = 0;
    for (size_t x=0; x < locationData->size(); x++) {
        for (size_t y=0; y < locationData->at(0).size(); y++) {
            int amount=policy->getCustomFileTypeData(&(locationData->at(x).at(y)), fileType);
            total += amount;
            if (amount>=maxVal){
                maxVal=amount;
            }
            if(minVal>=amount){
                minVal=amount;
            }
            
            if(y!=0){
                outputString<<",";
            }
            outputString<<amount;
        }
        outputString<<"\n";
    }
    
    //Make Folder for Files if Needed
    struct stat info;
    std::string folderLocation =outputLocation+"/"+fileType;
    if(stat(folderLocation.c_str(), &info) != 0){
        mkdir(folderLocation.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    std::ofstream buildingStatsFile;
    buildingStatsFile.open(folderLocation+"/"+fileName);
    
    //Add Header Information
    for(std::vector<std::string>::iterator it = headerInformation.begin(); it != headerInformation.end(); ++it) {
        buildingStatsFile<<*it<<"\n";
    }
    buildingStatsFile<<total<<"\n";
    //Add Data
    buildingStatsFile << minVal<<","<<maxVal<<"\n"<<outputString.str();
    
    
    buildingStatsFile.close();
    return true;
    
}



bool ImageFileGenerator::makePopFile(std::string folderName, std::string fileName, std::vector<std::string> headerInformation){
    std::ostringstream outputString;
    int maxVal=0;
    int minVal=INT_MAX;
    int  total = 0;
    for (size_t x = 0; x < locationData->size(); x++) {
        for (size_t y=0; y < locationData->at(0).size(); y++) {
            int amount=locationData->at(x).at(y).getCurrentPopulation();
            total += amount;
            if (amount>=maxVal){
                maxVal=amount;
            }
            if(minVal>=amount){
                minVal=amount;
            }
            
            if(y!=0){
                outputString<<",";
            }
            outputString<<amount;
        }
        outputString<<"\n";
    }
    
    //Make Folder for Files if Needed
    struct stat info;
    std::string folderLocation =outputLocation+"/"+folderName;
    if(stat(folderLocation.c_str(), &info) != 0){
        mkdir(folderLocation.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    std::ofstream buildingStatsFile;
    buildingStatsFile.open(folderLocation+"/"+fileName);

    //Add Header Information
    for(std::vector<std::string>::iterator it = headerInformation.begin(); it != headerInformation.end(); ++it) {
        buildingStatsFile<<*it<<"\n";
    }
    buildingStatsFile<<total<<"\n";
    //Add Data
    buildingStatsFile << minVal<<","<<maxVal<<"\n"<<outputString.str();
    
    
    buildingStatsFile.close();
    return true;
    
}

bool ImageFileGenerator::makeBuildingFile(std::string folderName, std::string fileName, char type, std::vector<std::string> headerInformation){
    std::ostringstream outputString;
    int maxVal=0;
    int minVal=INT_MAX;
    for (size_t x = 0; x < locationData->size(); x++) {
        for (size_t y = 0; y < locationData->at(0).size(); y++) {
            int amount=locationData->at(x).at(y).getNumberOfBuildings(type);
            if (amount>=maxVal){
                maxVal=amount;
            }
            if(minVal>=amount){
                minVal=amount;
            }
            
            if(y!=0){
                outputString<<",";
            }
            outputString<<amount;
        }
        outputString<<"\n";
    }

    
    //Make Folder for Files if Needed
    struct stat info;
    std::string folderLocation =outputLocation+"/"+folderName;
    if(stat(folderLocation.c_str(), &info) != 0){
        mkdir(folderLocation.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    std::ofstream buildingStatsFile;
    buildingStatsFile.open(folderLocation+"/"+fileName);

    //Add Header Information
    for(std::vector<std::string>::iterator it = headerInformation.begin(); it != headerInformation.end(); ++it) {
        buildingStatsFile<<*it<<"\n";
    }
    //Add Data
    buildingStatsFile << minVal<<","<<maxVal<<"\n"<<outputString.str();
    
    buildingStatsFile.close();
    return true;
}
#endif

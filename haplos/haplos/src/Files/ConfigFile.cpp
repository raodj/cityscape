#ifndef HAPLOS_CONFIG_FILE_CPP
#define HAPLOS_CONFIG_FILE_CPP

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

#include "ConfigFile.h"
#include <iostream>
#include <fstream>
#include <sstream>

ConfigFile::ConfigFile(){

}

ConfigFile::ConfigFile(const std::string& fileLocation) :
    configFileLocation(fileLocation) {
    std::cout << "Loading config file: " << configFileLocation << std::endl;
    std::ifstream infile(configFileLocation);
    if (!infile.good()) {
        std::cerr << "Unable to Open Configuration File." << std::endl;
        exit(0);
        return;
    }
    std::string line="";
    sedacFileLocation="";
    outputFileLocation="";
    timelineFileLocation="";
    customFileTypes="";
    populationImport="";
    buildingImport="";
    while (getline(infile, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.at(0) != '#') {
            // Not Comment
            int equal_pos = line.find_first_of("=");
            if (sedacFileLocation.empty()) {
                //SEDAC File not set
                sedacFileLocation = line.substr(equal_pos+1);
            } else {
                if(timelineFileLocation.empty()){
                    timelineFileLocation=line.substr(equal_pos+1);
                }else{
                    if(outputFileLocation.empty()){
                        outputFileLocation = line.substr(equal_pos+1);
                    }else{
                        if(buildingImport.empty()){
                            buildingImport = line.substr(equal_pos+1);
                        }else{
                            if(populationImport.empty()){
                                populationImport = line.substr(equal_pos+1);
                            }else{
                                if(customFileTypes.empty()){
                                    customFileTypes = line.substr(equal_pos+1);
                                }else{
                                    addVariable(line.substr(0, equal_pos),
                                                std::atof(line.substr(equal_pos + 1).c_str()));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
        
    //Initialize Helper Arrays
    olderSchoolWeekendProbablities[0] = variables["Schedule_Older_School_Weekend_Job_Probablity"];
    olderSchoolWeekendProbablities[1] = variables["Schedule_Older_School_Weekend_Out_Probablity"];
    olderSchoolWeekendProbablities[2] =  variables["Schedule_Older_School_Weekend_Home_Probablity"];
        
    olderSchoolSchoolDayProbablities[0] = variables["Schedule_Older_School_School_Day_Job_Probablity"];
    olderSchoolSchoolDayProbablities[1] = variables["Schedule_Older_School_School_Day_Out_Probablity"];
    olderSchoolSchoolDayProbablities[2] = variables["Schedule_Older_School_School_Day_Home_Probablity"];
        
    adultUnemployeedVisitorProbablities[0] = variables["Schedule_Adult_UnEmployeed_Job_Probablity"];
    adultUnemployeedVisitorProbablities[1] = variables["Schedule_Adult_UnEmployeed_Out_Probablity"];
    adultUnemployeedVisitorProbablities[2] = variables["Schedule_Adult_UnEmployeed_Probablity"];
        
    adultNoWorkVisitorProbablities[0] = variables["Schedule_Adult_Employeed_No_Work_Job_Probablity"];
    adultNoWorkVisitorProbablities[1] = variables["Schedule_Adult_Employeed_No_Work_Out_Probablity"];
    adultNoWorkVisitorProbablities[2] = variables["Schedule_Adult_Employeed_No_Work_Home_Probablity"];
    
    adultWorkVisitorProbablities[0] = variables["Schedule_Adult_Employeed_Work_Job_Probablity"];
    adultWorkVisitorProbablities[1] = variables["Schedule_Adult_Employeed_Work_Out_Probablity"];
    adultWorkVisitorProbablities[2] = variables["Schedule_Adult_Employeed_Work_Home_Probablity"];

    transportRadius[0] = (int)variables["Max_Public_Transport_Distance"];
    transportRadius[1] = (int)variables["Max_Walking_Distance"];
        
    transportRate[0] = (int)variables["Public_Transport_Rate"];
    transportRate[1] = (int)variables["Private_Transport_Rate"];
    transportRate[2] = (int)variables["Walking_Transport_Rate"];

    transportProbablities[0] = variables["Public_Transport_Percentage"];
    transportProbablities[1] = variables["Private_Transport_Percentage"];
    transportProbablities[2] = variables["Walking_Transport_Percentage"];
        
    radiusLimits[0] = (int)variables["Schedule_Young_Children_Radius_Limit"];
    radiusLimits[1] = (int)variables["Schedule_School_Children_5-13_Radius_Limit"];
    radiusLimits[2] = (int)variables["Schedule_School_Children_14-15_Radius_Limit"];
    radiusLimits[3] = (int)variables["Schedule_School_Children_14-15_Radius_Limit"];
    radiusLimits[4] = (int)variables["Schedule_Employeed_Radius_Limit"];
    radiusLimits[5] = (int)variables["Schedule_Unemployeed_Radius_Limit"];
        
    scheduleTypeProbablities[0] = variables["Schedule_Young_Children_5-Younger_Probablity"];
    scheduleTypeProbablities[1] = variables["Schedule_School_Children_5-13_Probablity"];
    scheduleTypeProbablities[2] = variables["Schedule_School_Children_14-17_Probablity"];
    scheduleTypeProbablities[3] = variables["Schedule_Employeed_Adults_18-24_Probablity"];
    scheduleTypeProbablities[4] = variables["Schedule_Unemployeed_Adults_18-24_Probablity"];
    scheduleTypeProbablities[5] = variables["Schedule_Employeed_Adults_25-44_Probablity"];
    scheduleTypeProbablities[6] = variables["Schedule_Unemployeed_Adults_25-44_Probablity"];
    scheduleTypeProbablities[7] = variables["Schedule_Employeed_Adults_45-64_Probablity"];
    scheduleTypeProbablities[8] = variables["Schedule_Unemployeed_Adults_45-64_Probablity"];
    scheduleTypeProbablities[9] = variables["Schedule_Employeed_Adults_65-Older_Probablity"];
    scheduleTypeProbablities[10] = variables["Schedule_Unemployeed_Adults_65-Older_Probablity"];
        
    daycareSizeProbablities[0] = variables["Daycare_Size_0-4_Probablity"];
    daycareSizeProbablities[1] = variables["Daycare_Size_5-9_Probablity"];
    daycareSizeProbablities[2] = variables["Daycare_Size_10-19_Probablity"];
    daycareSizeProbablities[3] = variables["Daycare_Size_20-99_Probablity"];
    daycareSizeProbablities[4] = variables["Daycare_Size_100-499_Probablity"];
    daycareSizeProbablities[5] = variables["Daycare_Size_500_Probablity"];

    schoolSizeProbablities[0] = variables["School_Size_0-4_Probablity"];
    schoolSizeProbablities[1] = variables["School_Size_5-9_Probablity"];
    schoolSizeProbablities[2] = variables["School_Size_10-19_Probablity"];
    schoolSizeProbablities[3] = variables["School_Size_20-99_Probablity"];
    schoolSizeProbablities[4] = variables["School_Size_100-499_Probablity"];
    schoolSizeProbablities[5] = variables["School_Size_500_Probablity"];
        
    hospitalSizeProbablities[0] = variables["Hospital_Size_0-4_Probablity"];
    hospitalSizeProbablities[1] = variables["Hospital_Size_5-9_Probablity"];
    hospitalSizeProbablities[2] = variables["Hospital_Size_10-19_Probablity"];
    hospitalSizeProbablities[3] = variables["Hospital_Size_20-99_Probablity"];
    hospitalSizeProbablities[4] = variables["Hospital_Size_100-499_Probablity"];
    hospitalSizeProbablities[5] = variables["Hospital_Size_500_Probablity"];

    businessSizeProbablities[0] = variables["Business_Size_0-4_Probablity"];
    businessSizeProbablities[1] = variables["Business_Size_5-9_Probablity"];
    businessSizeProbablities[2] = variables["Business_Size_10-19_Probablity"];
    businessSizeProbablities[3] = variables["Business_Size_20-99_Probablity"];
    businessSizeProbablities[4] = variables["Business_Size_100-499_Probablity"];
    businessSizeProbablities[5] = variables["Business_Size_500_Probablity"];
        
    familySizeProbablities[0] = variables["Family_Size_1_Probablity"];
    familySizeProbablities[1] = variables["Family_Size_2_Probablity"];
    familySizeProbablities[2] = variables["Family_Size_3_Probablity"];
    familySizeProbablities[3] = variables["Family_Size_4_Probablity"];
    familySizeProbablities[4] = variables["Family_Size_5_Probablity"];
    familySizeProbablities[5] = variables["Family_Size_6_Probablity"];
    familySizeProbablities[6] = variables["Family_Size_7_Probablity"];
        
    ageProbablities[0] = variables["Age_5-Younger_Probablity"];
    ageProbablities[1] = variables["Age_5-13_Probablity"];
    ageProbablities[2] = variables["Age_14-17_Probablity"];
    ageProbablities[3] = variables["Age_18-24_Probablity"];
    ageProbablities[4] = variables["Age_25-44_Probablity"];
    ageProbablities[5] = variables["Age_45-64_Probablity"];
    ageProbablities[6] = variables["Age_65-Older_Probablity"];


    std::cout << "Configuration File Loaded Successfully." << std::endl;
  //  std::cout << *this << std::endl;
    
}

void
ConfigFile::addVariable(const std::string& nameOfVariable,
			double valueOfVariable){
    variables[nameOfVariable] = valueOfVariable;
}


void
ConfigFile::displayVariables(std::ostream& os) const {
    for (std::unordered_map<std::string, double>::const_iterator i = variables.cbegin(); (i != variables.cend()); ++i){
        os << " [" << i->first << ", "
	   << i->second << "]" << std::endl;
    }
}

std::string
ConfigFile::getSedacFileLocation() const {
    return sedacFileLocation;
}

std::string
ConfigFile::getOutputFileLocation() const {
    return outputFileLocation;
}

std::string
ConfigFile::getTimelineFileLocation() const {
    return timelineFileLocation;
}

std::string
ConfigFile::getCustomFileTypes() const {
    return customFileTypes;
}

std::string
ConfigFile::getBuildingImport() const {
    return buildingImport;
}

std::string
ConfigFile::getPopulationImport() const {
    return populationImport;
}

double* ConfigFile::getAgeProbablities(){
    return ageProbablities;
}

double* ConfigFile::getFamilySizeProbablities(){
    return familySizeProbablities;
}

double* ConfigFile::getBusinessSizeProbablities(){
    return businessSizeProbablities;
}

double* ConfigFile::getHospitalSizeProbablities(){
    return hospitalSizeProbablities;
}

double* ConfigFile::getSchoolSizeProbablities(){
    return schoolSizeProbablities;
}

double* ConfigFile::getDaycareSizeProbablities(){
    return daycareSizeProbablities;
}

double* ConfigFile::getScheduleTypeProbablities(){
    return scheduleTypeProbablities;
}

int* ConfigFile::getRadiusLimits(){
    return radiusLimits;
}

double* ConfigFile::getTransportProbablities(){
    return transportProbablities;
}

int* ConfigFile::getTransportRate(){
    return transportRate;
}

int* ConfigFile::getTransportRadius(){
    return transportRadius;
}

double* ConfigFile::getAdultWorkVisitorProbablities(){
    return adultWorkVisitorProbablities;
}

double* ConfigFile::getAdultNoWorkVisitorProbablities(){
    return adultNoWorkVisitorProbablities;
}

double* ConfigFile::getAdultUnemployeedVisitorProbablities(){
    return adultUnemployeedVisitorProbablities;
}

double* ConfigFile::getOlderSchoolWeekendProbablities(){
    return olderSchoolWeekendProbablities;
}

double* ConfigFile::getOlderSchoolSchoolDayProbablities(){
    return olderSchoolSchoolDayProbablities;
}

ConfigFile::~ConfigFile() {
    // TODO Auto-generated destructor stub
}

std::ostream& operator<<(std::ostream& os, const ConfigFile& cf) {
    cf.displayVariables(os);
    return os;
}

#endif

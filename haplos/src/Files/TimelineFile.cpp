#ifndef HAPLOS_TIMELINE_FILE_CPP
#define HAPLOS_TIMELINE_FILE_CPP

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

#include "TimelineFile.h"


TimelineFile::TimelineFile(){
    
}

TimelineFile::TimelineFile(const std::string& fileLocation, std::string customFileTypes):
timeLineLocation(fileLocation) {
    //std::cout << "Loading timeline file: " << timeLineLocation << std::endl;
    std::ifstream infile(timeLineLocation);
    std::locale loc;
    std::string line="";

    if (!infile.good()) {
        std::cerr << "Unable to Open Timeline File." << std::endl;
        return;
    }
    
    //Process Custom File Types
    std::istringstream fileTypesSS(customFileTypes);
    std::string temp;
    //singleFiles[std::atoi(tokens.at(0).c_str())] = std::vector<std::string>;
    while(std::getline(fileTypesSS, temp, ',')){
        allowedFileTypes.push_back(this->lowerCaseString(temp));
    }
    
    //Process Time Line File
    while (getline(infile, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.at(0) != '#') {
            // Not Comment
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf), end;
            std::vector<std::string> tokens(beg, end);

            if(std::isalpha(line.at(0), loc)){
                //Output at Specific Intervals
                std::istringstream filesToProduce(tokens.at(2));
                switch(line.at(0)){
                    case 'T':
                        //Timestep
                        //std::cout<<"Time Step Interval: "<<std::endl;
                        timeStepInterval = std::atoi(tokens.at(1).c_str());
                        while(std::getline(filesToProduce, temp, ',')){
                            fileTimeStep.push_back(this->lowerCaseString(temp));
                        }
                        //std::cout<<"\tInterval "<<timeStepInterval<<std::endl;
                        break;
                    case 'D':
                        //Day
                        //std::cout<<"Day Interval: "<<std::endl;
                        dayInterval = std::atoi(tokens.at(1).c_str());
                        while(std::getline(filesToProduce, temp, ',')){
                            fileDay.push_back(this->lowerCaseString(temp));
                        }
                        //std::cout<<"\tInterval "<<dayInterval<<std::endl;
                        break;
                    case 'W':
                        //Week
                        //std::cout<<"Week Interval: "<<std::endl;
                        weekInterval = std::atoi(tokens.at(1).c_str());
                        while(std::getline(filesToProduce, temp, ',')){
                            fileWeek.push_back(this->lowerCaseString(temp));
                        }
                        //std::cout<<"\tInterval "<<dayInterval<<std::endl;
                    case 'M':
                        //Month
                        //std::cout<<"Month Interval: "<<std::endl;
                        monthInterval = std::atoi(tokens.at(1).c_str());
                        while(std::getline(filesToProduce, temp, ',')){
                            fileMonth.push_back(this->lowerCaseString(temp));
                        }
                        //std::cout<<"\tInterval "<<monthInterval<<std::endl;
                        break;
                    case 'Y':
                        //Year
                        //std::cout<<"Year Interval: "<<std::endl;
                        yearInterval = std::atoi(tokens.at(1).c_str());
                        while(std::getline(filesToProduce, temp, ',')){
                            fileYear.push_back(this->lowerCaseString(temp));
                        }
                        //std::cout<<"\tInterval "<<yearInterval<<std::endl;
                        break;
                    default:
                        std::cout<<"Invalid Timeline Interval Type: "<<line.at(0)<<std::endl;
                }
            }else{
                //One time interval
                //std::cout<<"One Time File: "<<std::endl;
                std::istringstream filesToProduce(tokens.at(1));
                //singleFiles[std::atoi(tokens.at(0).c_str())] = std::vector<std::string>;
                while(std::getline(filesToProduce, temp, ',')){
                    singleFiles[std::atoi(tokens.at(0).c_str())].push_back(this->lowerCaseString(temp));
                }
                //std::cout<<"\tInterval: "<<tokens.at(0)<<std::endl;
                //std::cout<<"\tFile: "<<tokens.at(1)<<std::endl;
            }
        }
    }
    
    //std::cout<<"Results from Load"<<std::endl;
    //std::cout<<this->displayTimeLineInformation()<<std::endl;
    //std::cout << "Timeline File Loaded Successfully." << std::endl;
    
}

std::vector<std::string> TimelineFile::getFilesToProduceAt(int time){
    //std::cout<<"Time: "<<time<<std::endl;
    std::unordered_map<std::string, bool> filesProduced;
    for(std::vector<std::string>::iterator it = allowedFileTypes.begin(); it != allowedFileTypes.end(); ++it){
        filesProduced[*it] = false;
    }
    std::vector<std::string> returnVector;
    
    if(time%timeStepInterval==0){
        //std::cout<<"Creating Time"<<std::endl;
        this->getFilesToReturn(fileTimeStep, &filesProduced);
    }
    
    if(time%(dayInterval*144) == 0){
        //std::cout<<"Creating Day"<<std::endl;
        this->getFilesToReturn(fileDay, &filesProduced);
    }
    
    if(time%(weekInterval*1008)==0){
        //std::cout<<"Creating Week"<<std::endl;
        this->getFilesToReturn(fileWeek, &filesProduced);
        
    }
    
    if(time%(monthInterval*4320)==0){
        //std::cout<<"Creating Month"<<std::endl;
        this->getFilesToReturn(fileMonth, &filesProduced);
    }
    
    if(time%(yearInterval*52560)==0){
        //std::cout<<"Creating Year"<<std::endl;
        this->getFilesToReturn(fileYear, &filesProduced);

    }
    if(singleFiles.count(time)>0){
        //std::cout<<"Creating Single"<<std::endl;
        this->getFilesToReturn(singleFiles[time], &filesProduced);

    }
    //std::cout<<"Returning Files: "<<std::endl;
    for(std::vector<std::string>::iterator it = allowedFileTypes.begin(); it != allowedFileTypes.end(); ++it){
        if(filesProduced[*it]){
            //std::cout<<*it<<",";
            returnVector.push_back(*it);
        }
    }
    //std::cout<<std::endl;
    
    return returnVector;
}

void TimelineFile::getFilesToReturn(std::vector<std::string> filesToProduce, std::unordered_map<std::string, bool> *fileProduced){
    for(std::vector<std::string>::iterator it = filesToProduce.begin(); it != filesToProduce.end(); ++it){
        if(fileProduced->count(*it)>0){
            if(!fileProduced->at(*it)){
                fileProduced->at(*it) = true;
            }
        }else{
            //std::cout<<"Invalid File Type: "<<*it<<std::endl;
        }
    }
}

std::string TimelineFile::lowerCaseString(std::string s){
    std::locale loc;
    std::string returnString;
    for (std::string::size_type i=0; i<s.length(); ++i)
        returnString += std::tolower(s[i],loc);
    return returnString;
}

std::string TimelineFile::displayTimeLineInformation(){
    std::string returnString = "";
    
    for(std::map<int, std::vector<std::string>>::iterator ii = singleFiles.begin(); ii!=singleFiles.end();ii++){
        returnString+=std::to_string((*ii).first)+"\n";
        for(std::vector<std::string>::iterator it = (*ii).second.begin(); it!= (*ii).second.end(); ++it){
            returnString+="\t"+*it+"\n";
        }
    }
    return returnString;
}

TimelineFile::~TimelineFile(){
    // TODO Auto-generated destructor stub

}

#endif

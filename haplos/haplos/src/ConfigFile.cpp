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

using namespace std;

ConfigFile::ConfigFile(string FileLocation) {
    configFileLocation = FileLocation;
    std::cout << "Loading Configuration file from : " << configFileLocation << std::endl;
    ifstream infile;
    infile.open (configFileLocation.c_str());
    
    if (infile.fail()) {
        std::cout << "Unable to Open Configuration File." << std::endl;
        sedacFileLocation="";
    }else{
        std::string line="";
        sedacFileLocation="";

        while (infile) {
            getline(infile,line);
            if (!line.empty()) {
                if (line.at(0)!='#') {
                //Not Comment
                    int equal_pos = line.find_first_of("=");

                    if(sedacFileLocation.empty()){
                        //SEDAC File not set
                        sedacFileLocation=line.substr(equal_pos+1);
                    }else{
                        addVariable(line.substr(0, equal_pos), std::atof(line.substr(equal_pos+1).c_str()));
                    }
                }
                //Comment
            }
            //Blank Line
        }
    }
    std::cout<< "Configuration File Loaded Successfully."<<std::endl;
    displayVariables();
}


void ConfigFile::addVariable(string nameOfVariable, double valueOfVariable){
    variables[nameOfVariable]= valueOfVariable;
}
double ConfigFile::getVariable(string variableName){
    return variables[variableName];
}

void ConfigFile::displayVariables(){
    for (std::tr1::unordered_map<std::string, double>::iterator i = variables.begin();i != variables.end(); ++i){
        std::cout << " [" << i->first << ", ";
        std::cout << i->second << "]"<< std::endl;
    }
    
}
string ConfigFile::getSedacFileLocation(){
    return sedacFileLocation;
    
}

ConfigFile::~ConfigFile() {
	// TODO Auto-generated destructor stub
}
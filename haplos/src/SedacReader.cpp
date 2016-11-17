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

#include "SedacReader.h"
#include "Location.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

SedacReader::SedacReader() {
	// TODO Auto-generated constructor stub

}

SedacReader::~SedacReader() {
	// TODO Auto-generated destructor stub
}

std::vector<std::vector<Location> > SedacReader::readFile(std::string fileName, int sedacPopulationSize, int actualPopulationSize){
        std::vector< std::vector<Location> > densityData;
		int cols;
		int rows;
        std::string line;
        std::ifstream infile;
		infile.open (fileName.c_str());
        int buildingID= 0;
    
		if (infile.fail()) {
            std::cout << "Unable to Open SEDAC Data File at" << fileName.c_str() << std::endl;
            exit(0);
			return densityData;
		}
		//Get Number of Rows
		getline(infile,line);
		unsigned pos = line.find_last_of(" ");
		cols = std::stoi(line.substr(pos+1).c_str());
    
		//Get Number of cols
		getline(infile,line);
		pos = line.find_last_of(" ");
		rows= std::atoi(line.substr(pos+1).c_str());
    
		//Ignore These Lines
		getline(infile,line);
		getline(infile,line);
		getline(infile,line);

		//Get No Data Value
		getline(infile,line);
		pos = line.find_last_of(" ");
        std::cout << "Starting Processing" << std::endl;
		densityData.resize(rows);
		for(int i =0; i < rows; i++){
			densityData.at(i).resize(cols);
		}

                std::cout << "Data Allocated (rows: " << rows
                          << ", cols: " << cols << ")" << std::endl;
		int currentRow=0;
    
		//Get all the Data
        densityData.resize(rows);
        for(int i = 0; i<rows; i++){
            densityData.at(i).resize(cols);
        }
		while (!infile.eof()) { // To get you all the lines.
			//Process Line
			getline(infile,line);
		    string buf; // Have a buffer string
		    stringstream ss(line); // Insert the string into a stream

		    vector<std::string> tokens; // Create vector to hold our words

		    while (ss >> buf) {
		        tokens.push_back(buf);
		    }
		    vector<std::string>::iterator i;
		    int currentColumn=0;
		    for (vector<std::string>::iterator i = tokens.begin();i != tokens.end();++i) {
		    	if (currentColumn<cols) {
                    float density=(std::atof((*i).c_str()))/sedacPopulationSize;
					densityData.at(currentRow).at(currentColumn) = Location(currentRow,
                                                                            currentColumn,
                                                                            ceil(density*actualPopulationSize),
                                                                            density,
                                                                            buildingID);
                    buildingID++;
                    
		    		currentColumn++;
		    	}
                else {
		    		break;
		    	}
		    }
			currentRow++;
		}
        std::cout << "Done reading Data" << std::endl;
		infile.close();
		return densityData;

}


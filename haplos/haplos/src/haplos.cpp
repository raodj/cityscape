#ifndef HAPLOS_CPP
#define HAPLOS_CPP

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

#include "SedacReader.h"
#include "ImageGen.h"
#include "Location.h"
#include "Population.h"
#include "XFigImageGenerator.h"
#include "ConfigFile.h"

using namespace std;

string const outputFolder="output/";
string const configFile="examples/config/USAData.hapl";
int main() {
    vector< vector < Location > > densityData;
    std::cout << "-----HAPLOS-----" << std::endl;
    ConfigFile configuration= ConfigFile(configFile);
    SedacReader sr = SedacReader();
    densityData = sr.readFile(configuration.getSedacFileLocation());
    
    //Age Probablities
    double ageProbablities [6]={ configuration["Age_5-13_Probablity"],
                                 configuration["Age_14-17_Probablity"],
                                 configuration["Age_18-24_Probablity"],
                                 configuration["Age_25-44_Probablity"],
                                 configuration["Age_45-64_Probablity"],
                                 configuration["Age_65-Older_Probablity"]};
    
    //Family Size Probablties
    double familySizeProbablities [6]={ configuration["Family_Size_1_Probablity"],
                                        configuration["Family_Size_2_Probablity"],
                                        configuration["Family_Size_3_Probablity"],
                                        configuration["Family_Size_4_Probablity"],
                                        configuration["Family_Size_5_Probablity"],
                                        configuration["Family_Size_6_Probablity"]};
    
    //Create Population
    Population pop = Population(configuration["Total_Population"], ageProbablities, familySizeProbablities, configuration["Male_Probablity"]);
   
    //Assign Familes Locations
    int x=0;
    int y=0;
    int notAssigned=0;
    float oldRatio=0;
    std::cout << "Assigning Locations to Population" << std::endl;
    printf("Percentage Complete: %3d%%", 0 );
    fflush(stdout);
    
    for ( int i =0; i<pop.getNumberOfFamilies(); i++ ) {
        while (densityData.at(x).at(y).isFull()) {
            //Move to Next Location in column
            x++;
            if(x>=densityData.size()&&y<densityData[0].size()){
                //Move to next Row
                x=0;
                y++;
            }
            else{
                if (y>=densityData[0].size()-1) {
                    //No More Locations Avaliable
                    notAssigned++;
                    break;
                }
            }
        }
        
        if (y>=densityData[0].size()-1) {
            //No More Locations Avaliable
            break;
        }
        //Set Location of Family
        pop.setLocationOfFamily(x, y, i);
        densityData.at(x).at(y).addFamily(pop.getFamily(i));
        
        //Calculate Percent Complete
        float ratio = i/(float)pop.getNumberOfFamilies();
        
        if ( 100*(ratio-oldRatio) > 1 ) {
            //Update Percent Complete Only if there is a Change
            printf("\r");
            printf("Percentage Complete: %3d%%", (int)(ratio*100) );
            oldRatio=ratio;
            fflush(stdout);
        }
    }
    //Print out 100% Complete
    printf("\r");
    printf("Percentage Complete: %3d%%", 100 );
    fflush(stdout);
    
    std::cout << "Population Successfully Assigned Locations" << std::endl;

    #ifdef HAVE_MAGICK
        ImageGen ig(outputFolder);
        ig.createPNGImage(densityData, densityData.size(),
                      densityData[0].size());
    #endif
    // Generate image in XFig file format. This should be an
    // option indicated by the user.
    XFigImageGenerator xfig;
    xfig.createImage(outputFolder + "haplos.fig", densityData,
		     densityData.size(), densityData[0].size());
    pop.displayStatistics();

    return 0;
}

#endif

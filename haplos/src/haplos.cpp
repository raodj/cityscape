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

using namespace std;

// The following two lines should be command-line arguments processed
// using the ArgParser in MUSE.
string sedacDataFile = "data/usap00ag.asc";
string outputFolder  = "output/";

int main() {
    vector<vector<Location>> densityData;
    int const numberOfPeople = 283230000; //281424000;
    cout << "-----HAPLOS-----" << endl;
    SedacReader sr;
    densityData = sr.readFile(sedacDataFile);
    Population pop(numberOfPeople);
    int x=0;
    int y=0;
    int notAssigned=0;
    cout<<"Assigning Locations to Population"<<endl;
    for(int i =0; i<numberOfPeople;i++){
	while(densityData[x][y].isFull()){
	    x++;
	    if(x>=densityData.size()&&y<=densityData[0].size()){
		x=0;
		y++;
	    }
	    else{
		if(y>=densityData[0].size()){
		    notAssigned++;
		    break;
		}
	    }
	    // cout << x << " " << y << endl;
	}

	if(y>=densityData[0].size()){
	    break;
	}
	pop.setLocationOfPerson(x, y, i);
	densityData[x][y].addPerson();
    }
    cout<<"Not Assigned: "<<notAssigned<<endl;
    cout<<"Population Assigned Locations"<<endl;

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

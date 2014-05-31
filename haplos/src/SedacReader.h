
#ifndef SEDACREADER_H_
#define SEDACREADER_H_
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
#include <string>
#include <vector>
#include "Location.h"



class SedacReader {
    /** A class specifically for reading and processing SEDAC data.
     
     */
public:
    /** The default constructor for this class.s
     */
	SedacReader();
    
    /** Read in a SEDAC file and return a vector of locations.
        \param[in] filename Location of SEDAC File to be read.
        \param[in] sedacPopulationSize Size of total population in SEDAC File.
        \param[in] actualPopulationSize Size of Population being Generated.
     */
    std::vector<std::vector<Location> > readFile(std::string filename, int sedacPopulationSize, int actualPopulationSize);
	virtual ~SedacReader();
};


#endif /* SEDACREADER_H_ */

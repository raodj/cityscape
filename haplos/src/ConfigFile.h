#ifndef __haplos__ConfigFile__
#define __haplos__ConfigFile__
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

// Mavricks downgraded G++ compiler, thus doesn't include the tr1 libraries, using BOOST Libraries for now
#include <boost/tr1/unordered_map.hpp>

using namespace std;

class ConfigFile {
    /** A class specifically for Parsing ConfigFile     */
public:
    /** The default constructor for this class.
     
     \param[in] FileLocation Location of ConfigFile
     */
	ConfigFile(string FileLocation);
    
    /** Add variable to variable map.
     
     \param[in] nameOfVariable Name of variable to add.
     \param[in] valueOfVariable Value of variable to add.
     */
    void addVariable(string nameOfVariable, double valueOfVariable);
    
    /** Return variable from variable map.
     
     \param[in] nameOfVariable Name of variable to retrive.
     \return value of variable.
     */
    double getVariable(string variableName);
    
    /** Helper Method to display variables set.
     */
    void displayVariables();
    /** Return SEDAC file location.
     
     \param[in] nameOfVariable Name of variable to retrive.
     \return path to SEDAC file.
     */
    
    string getSedacFileLocation();
    
    /**
     The destructor.
     
     Currently the destructor does not have any specific task to
     perform in this class.  However, it is defined for adherence
     with conventions and for future extensions.
     */
	virtual ~ConfigFile();
    
private:
    std::string configFileLocation;
    std::string sedacFileLocation;
    std::tr1::unordered_map<string, double> variables;
    
    
};


#endif /* defined(__haplos__ConfigFile__) */

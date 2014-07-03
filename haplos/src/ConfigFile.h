#ifndef HAPLOS_CONFIG_FILE_H
#define HAPLOS_CONFIG_FILE_H

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
#include <unordered_map>

/** A class specifically for Parsing ConfigFile.

    This class essentially encapsulates a simple HashMap to ease
    look-up of various configuration variables that: <ul>

    <li>can loaded from a configuration file or</li>
    <li>Directly added to the hash map in this class.</li>

    </ul>

    A configuration file has the following convention:

    <ul>

    <li>All lines beginning with a '#' (pound/hash sign) are treated
    as comments and they are ignored.</li>

    <li>Other lines are assumed to be in the format <variable>=<value>
    and the variable is used as the key to store the value into a hash
    map.</li>

    </ul>

    This class can be used in the following manner:

    \begincode

    ConfigFile config("../MicroWorldData.hapl");
    std::cout << config["Male_Probability"] << std::endl;
    
    \endcode
*/
class ConfigFile {
    /** Stream insertion operator for ConfigFile.

	Convenience operator to write the current set of variables and
	their values to a given output stream.

	\param[out] os The output stream to which the values are to be
	written.

	\param[in] cf The configuration file whose values are to be
	written.
    */
    friend std::ostream& operator<<(std::ostream& os, const ConfigFile& cf);
public:
     /** The default constructor for this class.
      
      */
    ConfigFile();
    /** Constructor with File Location Set
	
	\param[in] FileLocation Location of ConfigFile
    */
    ConfigFile(const std::string& fileLocation);
    
    /** Add variable to variable map.
	
	\param[in] nameOfVariable Name of variable to add.  If the
	variable already exists in the configuration, then its value
	is overwritten.
	
	\param[in] valueOfVariable Value of variable to add.
    */
    void addVariable(const std::string& nameOfVariable, double valueOfVariable);
    
    /** Return variable from variable map.
     
	\param[in] nameOfVariable Name of variable to retrive.
     
	\return value of variable.
    */    
    double operator[](const std::string& variableName) const {
        return variables.at(variableName);
    }
    
    /** Helper Method to display variables set.

	\param[out] os The output stream to which the variable
	information is to be written.
    */
    void displayVariables(std::ostream& os) const;
    
    /** Return SEDAC file location.
	
	\return path to SEDAC file.
    */
    std::string getSedacFileLocation() const;
    
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
    std::unordered_map<std::string, double> variables;
};

#endif

#ifndef HAPLOS_H_
#define HAPLOS_H_

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
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

#include "SedacReader.h"
#include "ImageGeneration/ImageGen.h"
#include "Location.h"
#include "Population.h"
#include "ImageGeneration/XFigImageGenerator.h"
#include "Files/ConfigFile.h"
#include "Files/TimelineFile.h"
#include "Files/ImageFileGenerator.h"
#include "Schedule/ScheduleGenerator.h"
#include "Buildings/BuildingGenerator.h"
#include "Policy.h"

#include "Buildings/Medical.h"
#include "Buildings/School.h"
#include "Buildings/Business.h"
#include "Buildings/Daycare.h"

class Haplos {
public:
    /** Convenience constructor to initialize Haplos from command-line
        arguments.

        This is a convenience constructor that enables initialization
        of Haplos from command-line arguments.  This is the preferred
        constructor for Haplos and performs the following tasks: <ol>

        <li>It uses an ArgParser object to parse the command-line
        arguments and udpate internal configuration instance
        variables.</li>

        <li>If the command-line processing is successful, then it
        processes the configuration file specified as command-line
        argument.</li>
        
        </ol>

        \note If any errors/failures occurr then the constructor
        reports a suitable error message and terminates the program.
        
        \param[in,out] argc The number of command-line arguments to be
        parsed.

        \param[in,out] argv The actual command-line arguments to be
        used to initialize this object.

        \param[out] success This flag is set to true to indicate the
        processing was completed successfully.  On errors this flag is
        set to false.
    */
    Haplos(int& argc, char *argv[], bool& success);

    /** Generate the model using data from a given configuration file.

        This method is a top-level method that must be used to
        generate a model (from scratch) using information from a
        configuration file (spplied as command-line argument).
    */
    void generateModel();
    
    /** Top-level method to run a simulation using generated model.

        This method is the top-level method for running a simulation
        using the current model.

        \note A valid model must be available for simulation.  The
        model can be generated or loaded from a data file.
        
        \param[in] p The policy object to be used during simulation.
     */
    void runSimulation(Policy *p);

    /** Determine if simulation is enabled.

        \return This method returns true if simulation is enabled by
        the user via a command-line argument.
    */
    bool shouldSimulate() const { return simulate; }
    
protected:
    /** Convenience method to parse command-line arguments into
        instance variables in this class.

        This method uses an ArgParser object to parse the command-line
        arguments and udpate internal configuration instance
        variables.
        
        \param[in,out] argc The number of command-line arguments to be
        parsed.

        \param[in,out] argv The actual command-line arguments to be
        used to initialize this object.

        \return This method returns ture if the command-line arguments
        were successfully processed.
    */
    bool parseCmdLineArgs(int& argc, char *argv[]);
    
private:
    Policy policy;
    std::vector< School > schoolBuildings;
    std::vector< Business > businessBuildings;
    std::vector< Daycare > daycareBuildings;
    std::vector< Medical > medicalBuildings;
    std::vector< Building > otherBuildings;
    std::unordered_map<int, Building*> allBuildings;
    
    std::vector< std::vector < Location > > densityData;
    
    std::default_random_engine generator;
    std::string outputFolder           = "";
    std::string imageFileLocationPath  = "";
    std::string familyFileLocationPath = "";
    std::string saveLocationPath       = "";
    std::string configFilePath         = "";

    ConfigFile configuration;
    
    bool produceImages    = true;
    bool progressDisplay  = true;
    bool exportFiles      = true;
    bool simulate         = false;
    int numberOfBuildings = 0;

    // The instance variable shared between generation phase and
    // simulation phase in Haplos
    Population pop;
    ScheduleGenerator scheduleGen;
    TimelineFile tl;
};

#endif /* HAPLOS_H_ */

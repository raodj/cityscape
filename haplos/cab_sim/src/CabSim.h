#ifndef CAB_SIM_H
#define CAB_SIM_H

//---------------------------------------------------------------------------
//
// Copyright (c) PC2Lab Development Team
// All rights reserved.
//
// This file is part of free(dom) software -- you can redistribute it
// and/or modify it under the terms of the GNU General Public
// License (GPL)as published by the Free Software Foundation, either
// version 3 (GPL v3), or (at your option) a later version.
//
// The software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Miami University and PC2Lab makes no representations or warranties
// about the suitability of the software, either express or implied,
// including but not limited to the implied warranties of
// merchantability, fitness for a particular purpose, or
// non-infringement.  Miami University and PC2Lab is not be liable for
// any damages suffered by licensee as a result of using, result of
// using, modifying or distributing this software or its derivatives.
//
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
//
// Authors: Dhananjai M. Rao       raodm@miamiOH.edu
//
//---------------------------------------------------------------------------

#include "OSMData.h"

class CabSim {
public:
    /** Destructor.
        
        Currently the destructor for this class does not have much
        operation to perform and is merely peresent to adhere to
        conventions and serve as a place holder for future changes.
    */
    ~CabSim();

    /** The main interface method for this class that should be used
        to start and run the PHOLD Simulation.  The command-line
        arguments to the program should be passed-in as the arguments
        to this method.  These values are typically the default values
        passed to a standard C/C++ main() function.
 
        \param[in] argc The number of command-line arguments.
 
        \param[in] argv The actual command-line arguments.

        \return This method returns 0 (zero) on successful simulation.
        On errors, it returns a non-zero value.
    */    
    static int run(int argc, char** argv);

protected:
    /** The default constructor.
        
        The default constructor is intentionally protected as this
        class is not meant to be directly instantiated.  Instead the
        CabSim::run() static method should be used to run the
        simulation.  The constructor initializes the simulation
        configuration variables to default initial values.
    */
    CabSim();

    /** Helper method to process command-line arguments (if any).
 
        This method uses the ArgParser utility class to parse any
        command-line arguments supplied to the simulation and updates
        the configuration (several private instance variables)
        variables in this class.  The configuration variables are used
        by various method in the class to create various agents and
        initialize the simulation to match the supplied configuration.
 
        \param[in] argc The number of command-line arguments.
         
        \param[in] argv The actual command-line arguments.

        \return This method returns true if the command-line arguments
        were successfully processed.
    */
    bool processArgs(int argc, char** argv);

private:
    /** The generated model containing rings, building, street, and
        population data to be used for simulation.  The data is loaded
        from the processArgs method.
    */
    OSMData model;
};

#endif

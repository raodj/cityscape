#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

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

/** Class to encapsulate and process simulation-time configuration

    This class is used by CabSim to load and process simulation-time
    configuration information supplied by the modeler. The
    simulation-time configuration file is somewhat involved and has
    several options.  Accordingly, this class has been introduced to
    streamline management of the configuration information.
*/
class SimConfig {
public:
    /** The main helper method to load and process the simulation
        configuration file.
        
        \param[in] configPath The path to the configuration file for
        use during simulation.

        \param[in,out] model The model (already loaded) to be updated
        based on the configuration information specified by the user.
        
        \return This method returns true if the configuration file was
        successfully processed.  On errors this method returns false.
    */
    bool processConfig(const std::string& configPath, OSMData& model);

    /** Covenience method to return any extra taxis setup for a given
        ring.

        \param[in] ringIndex The index number associated with the
        ring.
        
        \return This method returns any extra taxis specified for a
        ring, or zero if no extra taxis have been set for given ring.
    */
    int getExtraTaxis(const int ringIndex) const;
    
    /** The total number of taxis to simulate in the model */
    int numTaxis;

    /** Map of rings and additional taxis to be added to the ring.
        This unordered map is populated with data from pop-ring-taxis
        directive in simulation configuration file.  The key is the
        ring ID and the value stored is the extra taxis to be added to
        the ring.  These entries are useful to add extra taxis to
        non-populated rings such as airports, railway stations etc.
    */
    std::unordered_map<int, int> ringExtraTaxis;
    
protected:
    /** Distribute a given number of people to buildings in a given
        poulation ring.

        \param[in,out] model The model whose buildings and population
        ring is to be updated.

        \param[in] ringIndex The index of the population ring whose
        population is to be increased.

        \param[in] pop The extra people to be distributed to various
        buildings in this population ring.
    */
    void addRingPop(OSMData& model, const int ringIndex, int pop) const;

private:
};

#endif

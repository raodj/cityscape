#ifndef CAB_SIM_CPP
#define CAB_SIM_CPP

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

#include "CabSim.h"
#include "ArgParser.h"
#include "PopGrid.h"
#include "Simulation.h"

CabSim::CabSim() {
    // Currently nothing else to be done.
}

CabSim::~CabSim() {
    // Currently nothing else to be done.
}

int
CabSim::run(int argc, char** argv) {
    // Create an instance of cab sim for use
    CabSim sim;
    // First check and process command-line arguments and load the
    // necessary information into memory.
    if (!sim.processArgs(argc, argv)) {
        return 1;  // Error processing args.
    }
    // Everything went well.
    return 0;
}

bool
CabSim::processArgs(int argc, char** argv) {
    // Temoprary variables to hold parsed argument values
    std::string modelPath = "Unspecifed";
    // Make the arg_record with MUSE's argument parser
    ArgParser::ArgRecord arg_list[] =  {
        {"--model", "The generated model to be used for simulation",
         &modelPath, ArgParser::STRING },
        {"", "", NULL, ArgParser::INVALID}
    };

    // Let the kernel initialize using any additional command-line
    // arguments.
    try {
        muse::Simulation::initializeSimulation(argc, argv);
    } catch (std::exception& exp) {
        std::cerr << "Exiting simulation due to initialization error: "
                  << exp.what() << std::endl;
        return false;
    }

    // Use the MUSE argument parser to parse command-line arguments
    // and update instance variables
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);

    // Check and load the model data
    if (model.loadModel(modelPath) != 0) {
        std::cerr << "Error loading model from " << modelPath << std::endl;
        return false;
    }
    // Next use the model information to create a grid of population
    // rings for further using during simulation.
    PopGrid& popGrid = PopGrid::get();
    popGrid.setupGrid(model.popRings);
    // Everything went well so far.
    return true;
}

#endif

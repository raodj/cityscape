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
#include "Utilities.h"
#include "XFigHelper.h"
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
    sim.distributeTaxis(sim.simConfig.numTaxis);
    // Register the agents on this parallel process
    sim.createAgents(sim.simConfig.numTaxis);
    // Finally run the simulation.
    sim.simulate();
    // Everything went well.
    return 0;
}

bool
CabSim::processArgs(int argc, char** argv) {
    // Temoprary variables to hold parsed argument values
    std::string modelPath = "Unspecifed", configPath = "Unspecified";
    // Make the arg_record with MUSE's argument parser
    ArgParser::ArgRecord arg_list[] =  {
        {"--model", "Path to generated model to be used for simulation",
         &modelPath, ArgParser::STRING },
        {"--config", "Path to simulation-time configuration file",
         &configPath, ArgParser::STRING },
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
    // rings for further use during simulation.
    PopGrid& popGrid = PopGrid::get();
    popGrid.setupGrid(model.popRings);

    // Next process the simulation-time configuration file.
    if (!simConfig.processConfig(configPath, model)) {
        std::cerr << "Error processing simulation-time configuration from "
                  << configPath << std::endl;
        return false;
    }
    // Everything went well so far.
    return true;
}

void
CabSim::distributeTaxis(const int numTaxis) const {
    XFigHelper xfig;
    xfig.setOutput("init_taxis.fig");
    // Get list of population rings sorted based on population.
    long total = 0, taxisLeft = numTaxis;
    const OSMData::InfoVec ringsInfo = model.getSortedPopRingInfo(0, total);
    const double pop = total;  // Convert to double for convenience.
    // To make taxi distribution in rings faster, we create a full
    // list of buildings sorted on their square footage.
    const OSMData::InfoVec bldInfo = model.getSortedBldInfo(-1, 1, total);
    // Distribute taxis proportional to the population in each ring
    bool firstBld = true;
    for (const OSMData::Info ri : ringsInfo) {
        // Compute taxis to be assigned to this ring.
        const int taxisInRing = simConfig.getExtraTaxis(ri.first) +
            std::min<int>(taxisLeft, std::ceil(numTaxis * ri.second / pop));
        if (taxisInRing > 0) {
            // Distribute taxis to buildings in this ring.
            initTaxiLocations(ri.first, taxisInRing, bldInfo, xfig, firstBld);
            firstBld = false;
            // Track number of taxis already assigned.
            taxisLeft -= taxisInRing;
        }
    }
    // Check to ensure all the taxis were assigned.
    if (taxisLeft > 0) {
        std::cerr << "Warning: Unassigned taxis in model "
                  << numTaxis << std::endl;
    }
}

void
CabSim::initTaxiLocations(int ring, const int numTaxis,
                          const OSMData::InfoVec& bldInfo, XFigHelper& xfig,
                          bool firstBld, const int mapSize) const {
    // Sanity check on pre-conditions.
    ASSERT((ring >= 0) && (ring < (int) model.popRings.size()));
    ASSERT(numTaxis > 0);
    // Compute xfig clip bounds for further reference
    int xClip = 0, yClip = 0, taxisLeft = numTaxis;
    const Point tlCoords = PopGrid::get().getMinTopLeft();
    getXYValues(tlCoords.second, tlCoords.first, xClip, yClip,
                mapSize * XFIG_SCALE, false);
    // Get the total square footage for the ring
    const double totSqFt = model.popRings[ring].bldsTotSqFt;
    // Assign taxis to the entry way of buildings.
    for (const OSMData::Info& bi : bldInfo) {
        if (bi.third != ring) {
            continue;  // This building is not ring
        }
        // Compute taxis to be assigned to this building.
        const int bldTaxis =
            std::max<int>(1, std::ceil(numTaxis * bi.second / totSqFt));
        ASSERT(bldTaxis > 0);
        // Get the building for reference in loop below.
        const Building& bld = model.buildingMap.at(bi.first);
        // Generate entry for each taxi
        for (int i = 0; (i < bldTaxis); i++) {
            int xfigX, xfigY;
            getXYValues(bld.wayLat, bld.wayLon, xfigX, xfigY,
                        mapSize * XFIG_SCALE, false);
            xfigX -= xClip;
            xfigY -= yClip;
            // Add some random jitter to distribute nearby taxis
            // so they are some what visible.
            if (i > 0) {
                unsigned int seed = i;
                xfigX += 500 - (rand_r(&seed) % 1000);
                xfigY += 500 - (rand_r(&seed) % 1000);
            }
            xfig.drawOval(xfigX, xfigY, 30, 30, MAGENTA, MAGENTA, 20);
            // Print the building information as well.
            //  bld.write(std::cout, firstBld);
            firstBld = false;
        }
        // Track number of taxis already assigned.
        taxisLeft -= bldTaxis;
        // Break out of loop as soon as possible
        if (taxisLeft <= 0) {
            break;  // No more taxis left to assign.
        }
    }
    // Check to ensure all the taxis were assigned.
    if (taxisLeft > 0) {
        std::cerr << "Warning: Unassigned taxis in ring #" << ring << " = "
                  << taxisLeft << std::endl;
    }
}

void
CabSim::createAgents(const int numTaxis) {
    // Set the MUSE kernel we have been working with.
    muse::Simulation* kernel = muse::Simulation::getSimulator();
    const int rank           = kernel->getSimulatorID();
    // Get list of population rings sorted based on population.
    long total = 0, taxisLeft = numTaxis;
    const OSMData::InfoVec ringsInfo = model.getSortedPopRingInfo(0, total);
    const double pop    = total;  // Convert to double for convenience.
    // To make taxi distribution in rings faster, we create a full
    // list of buildings sorted on their square footage.
    const OSMData::InfoVec bldInfo = model.getSortedBldInfo(-1, 1, total);    
    // Get the information about the parallel configuration.
    const PopGrid& grid = PopGrid::get();
    const int compNodes = kernel->getNumberOfProcesses();
    // Register every compNodes'th agent on this process
    for (size_t i = 0, registerRing = rank; (i < ringsInfo.size()); i++) {
        // Compute taxis to be initial assigned to this ring/agent
        const OSMData::Info& ri = ringsInfo.at(i);
        const int taxisInRing = simConfig.getExtraTaxis(ri.first) +
            std::min<int>(taxisLeft, std::ceil(numTaxis * ri.second / pop));
        // If this ring is to be registered, then do so
        if (i == registerRing) {
            // Create and register an Area agent that manages taxis.
            Area *area = new Area(i, grid.ringGridCoords.at(i),
                                  taxisInRing, bldInfo);
            kernel->registerAgent(area);  // register with MUSE kernel
            registerRing += compNodes;    // Update next ring to register
        }
        // Track number of taxis already assigned.
        taxisLeft -= taxisInRing;
    }

    // Check to ensure all the taxis were assigned.
    if (taxisLeft > 0) {
        std::cerr << "Warning: Unassigned taxis in model "
                  << taxisLeft << std::endl;
    }
}

void
CabSim::simulate() {
    // Convenient local reference to simulation kernel
    muse::Simulation* const kernel = muse::Simulation::getSimulator();
    // Setup start and end time of the simulation
    kernel->setStartTime(0);
    kernel->setStopTime(1);
    // Finally start the simulation here!!
    kernel->start();
    // Now we finalize the simulation to make sure it cleans up.
    muse::Simulation::finalizeSimulation();
}

#endif

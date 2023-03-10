#ifndef SIM_CONFIG_CPP
#define SIM_CONFIG_CPP

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

#include <fstream>
#include "SimConfig.h"
#include "Utilities.h"

/** A convenience macro used in processConfig method to check if
    sufficient tokens are present for a command and if not report an
    error.
*/
#define CHK_TOKENS(line, tokens, minSize)                   \
    if (tokens.size() < minSize) {                          \
        std::cerr << "Invalid line: '" << line << "'.\n";   \
        continue;                                           \
    }

bool
SimConfig::processConfig(const std::string& configPath, OSMData& model) {
    // Open configuration file and ensure it is valid.
    std::ifstream config(configPath);
    if (!config.good()) {
        std::cerr << "Error reading from configuration file "
                  << configPath << std::endl;
        return false;
    }
    // Process line-by-line of the configuration file.
    std::string line;
    while (std::getline(config, line)) {
        line = trim(line);  // remove leading/trailing blanks
        if (line.empty() || (line[0] == '#')) {
            continue;  // blank or comment line
        }
        // Break the line into words for convenient processing
        const std::vector<std::string> tokens = split(line);
        CHK_TOKENS(line, tokens, 2);
        // First get the command to be processed which is the first
        // word of the line.
        if (tokens[0] == "taxis") {
            numTaxis = std::stoi(tokens.at(1));
        } else if (tokens[0] == "pop-ring") {
            CHK_TOKENS(line, tokens, 3);
            const int ringIndex  = std::stoi(tokens.at(1));
            const int population = std::stoi(tokens.at(2));
            addRingPop(model, ringIndex, population);
        } else if (tokens[0] == "pop-ring-taxis") {
            CHK_TOKENS(line, tokens, 3);
            ringExtraTaxis[std::stoi(tokens.at(1))] = std::stoi(tokens.at(2));
        } else {
            std::cerr << "Unknown option line: '" << line << "'. Ignored.\n";
        }
    }
    // Everything went well
    return true;
}

void
SimConfig::addRingPop(OSMData& model, const int ringIndex, int pop) const {
    // Validate pre-conditions
    ASSERT((ringIndex >= 0) && (ringIndex < (int) model.popRings.size()));
    ASSERT(pop > 0);
    // Add population to the population ring's population.
    PopRing& pr = model.popRings.at(ringIndex);
    pr.setPopulation(pr.getPopulation() + pop);
    // Distribute population to buildings proportional to sq.footage
    const double sqFtPerPerson = std::floor(pr.homesTotSqFt / pop);
    for (auto bldEntry : model.buildingMap) {
        if (bldEntry.second.attributes == ringIndex) {
            continue;  // building is not in ring of interest
        }
        // Update population of this building.
        Building& bld = bldEntry.second;
        // Compute building's sq. footage
        const int levels  = (bld.levels > 1 ? bld.levels : 1);
        const int bldSqFt = bld.sqFootage * levels;
        const int bldPop  = std::min<int>(pop, std::ceil(bldSqFt /
                                                         sqFtPerPerson));
        // Assign pending population to the building.
        bld.population   += bldPop;
        pop              -= bldPop;  // track people left to assign
    }
    std::cout << "In ring index #" << ringIndex << ", unassigned: "
              << pop << std::endl;
}

int
SimConfig::getExtraTaxis(const int ringIndex) const {
    const auto entry = ringExtraTaxis.find(ringIndex);
    return (entry != ringExtraTaxis.end() ? entry->second : 0);
}

#endif

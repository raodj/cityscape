#ifndef OPTIONS_CPP
#define OPTIONS_CPP

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

#include <iostream>
#include "Options.h"
#include "Utilities.h"

// The process-wide unique instance of the unordered map that manages
// the global options.
NameValueMap Options::globalOptions;

void
Options::add(const std::string& option, const std::string& value) {
    ASSERT(!option.empty());
    globalOptions[option] = value;
}

std::string
Options::get(const std::string& option, const std::string& defaultValue) {
    // Try to find the option in our set of options.
    NameValueMap::const_iterator entry = globalOptions.find(option);
    // Return its value if found, otherwise return default value
    return (entry != globalOptions.end() ? entry->second : defaultValue);
}

int
Options::get(const std::string& option, const int defaultValue) {
    int value = defaultValue;  // The value to be returned.    

    // Try to find the option in our set of options.
    NameValueMap::const_iterator entry = globalOptions.find(option);
    // Try to parse it out as an integer, if found
    if (entry != globalOptions.end()) {
        try {
            value = std::stoi(entry->second);
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Unable to convert value for " << option
                      << entry->second << " to integer\n";
        }
    }
    // Now value is either default value or the value specified for option
    return value;
}

double
Options::get(const std::string& option, const double defaultValue) {
    double value = defaultValue;  // The value to be returned.    

    // Try to find the option in our set of options.
    NameValueMap::const_iterator entry = globalOptions.find(option);
    // Try to parse it out as an integer, if found
    if (entry != globalOptions.end()) {
        try {
            value = std::stod(entry->second);
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Unable to convert value for " << option
                      << entry->second << " to double\n";
        }
    }
    // Now value is either default value or the value specified for option
    return value;
}

#endif

#ifndef STOPWATCH_H
#define STOPWATCH_H

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

#include <chrono>

/**
 * A simple stopwatch class to track elsapsed time for different
 * operations.  Each instance of this class is MT-safe.
 */
class Stopwatch {
public:
    /**
     * Start/restart the stopwatch.  This basically sets the startTime
     * for this stopwatch.
     */
    void start();

    /**
     * Returns the current elapsed time in milliseconds
     *
     * \return The elapsed time since start was last called.
     */
    std::chrono::milliseconds elapsed();

private:
    /**
     * The time when this stopwatch was started.
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> stTime;
};

#endif

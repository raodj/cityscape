#ifndef OPTIONS_H
#define OPTIONS_H

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

#include <unordered_map>
#include <string>

/** A shortcut to a pair that holds the key and value */
using NameValueMap = std::unordered_map<std::string, std::string>;

/** A simple class to manage options specified by the user in input
    configuration file.  This class encapsulates a set of name--value
    pairs, with the name being a string value for options.  These
    options are used at various points in the model generation process
    to customize model generation.  These options are needed primarily
    to compensate for some missing information in open street maps
    data. These options enable the user to # improve the overall
    quality of the generated model.  These options would have to
    change based on the general properties of the city being analyzed.
    An instance of this options class is used by the ModelGenerator

    \note This class has been implemented as a singleton -- that is,
    there is only one global instance of this class.  Most of the
    methods in this class are static so that they can be readily used.
*/
class Options {
public:
    /** Convenience method to add an option to the global set of
        options encapsulated by this class.  Note that this method is
        rather simple and no checks are made on either the option or
        the value.

        \param[in] option The name of the option for which a value is
        being added.

        \param[in] value The value to be set for this option.
    */
    static void add(const std::string& option, const std::string& value);

    /** Get the value for a given option as a string.

        \param[in] option The option whose value is to be returned.

        \param[in] defaultValue If the option is not found, then this
        default value is returned by this method.

        \return The value set for the option, if found.  If the option
        specified was not found (that is, a value has not been set)
        then the default value is returned.
    */
    static std::string get(const std::string& option,
                           const std::string& defaultValue = "");

    /** Get the value for a given option as an integer.

        \param[in] option The option whose value is to be returned.

        \param[in] defaultValue If the option is not found, then this
        default value is returned by this method.

        \return The value set for the option, if found.  If the option
        specified was not found (that is, a value has not been set)
        then the default value is returned.
    */
    static int get(const std::string& option, const int defaultValue = 0);

    /** Get the value for a given option as a double.

        \param[in] option The option whose value is to be returned.

        \param[in] defaultValue If the option is not found, then this
        default value is returned by this method.

        \return The value set for the option, if found.  If the option
        specified was not found (that is, a value has not been set)
        then the default value is returned.
    */    
    static double get(const std::string& option, const double defaultValue = 0);

protected:
    /** The default and only constructor.  The constructor is
        protected to ensure that this class is not directly
        instantiated.

        This class does not have much initialization to be done.
    */
    Options() {}

    /** The destructor.

        A simple destructor (this class does not use any dynamic
        memory) just to be consistent with coding conventions.
    */
    ~Options() {}

private:
    /** The process-wide unique instance of the unordered map that
        manages the global options.
    */
    static NameValueMap globalOptions;
};

#endif

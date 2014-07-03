#ifndef IMAGE_GENERATOR_H
#define IMAGE_GENERATOR_H

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

#include <string>
#include <vector>
#include <exception>

#include "../Location.h"

/** Base class for image generation.

    The base class provides a consistent interface for generating an
    image of the model for HAPLOS in different file formats.  This
    class is not meant to be directly used.  Instead one of the
    derived classes must be instantiated and used.  Refer to the
    documentation on the derived classes for details on their use.
*/
class ImageGenerator {
public:
    /**
       The destructor.

       Currently the destructor does not have any specific task to
       perform in this class.  However, it is defined for adherence
       with conventions and for future extensions.
    */
    virtual ~ImageGenerator();

    /** The interface method to be used for generating images to the
        given data file.

	\param[in] outFileName The output file to which the image is
	to be written.  If the file exists then this method overwrites
	the data in the file.

	\param[in] data The data for the model to be written to a
	given XFig file.

	\param[in] rows The number of rows.

	\param[in] cols The number of cols.

	\exception std::exception This method may throw exceptions on
	errors.
    */
    virtual void createImage(const std::string& outFileName,
			     const std::vector< std::vector< int > >& data,
			     const int rows, const int cols, const double maxValue)
	throw(std::exception) = 0;
    
protected:
    /** The default constructor for this class.

	<p>The constructor is protected to ensure that this class is
	never directly instantiated.  Instead one of the derived
	classes must be used for generating the desired image.</p>

	The constructor in this class does not have any specific tasks
	to perform.
    */
    ImageGenerator();
};

#endif

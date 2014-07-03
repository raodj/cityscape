#ifndef XFIG_IMAGE_GENERATOR_H
#define XFIG_IMAGE_GENERATOR_H

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

#include "ImageGenerator.h"

// Forward declaration to keep compiler happy and fast
class XFigHelper;

/** A class specifically designed to help generation of images in
    simple XFig file format.
    
*/
class XFigImageGenerator : public ImageGenerator {
public:
    XFigImageGenerator() {}
    ~XFigImageGenerator() {}
    
    void createImage(const std::string& outFileName,
		     const std::vector< std::vector< int > >& data,
		     const int rows, const int cols, const double maxValue)
	throw(std::exception);

private:
    /**
       Generates a range of colors with a given set of colors to aid
       in generating charts with well defined gradations so that
       colors can be easily recognized and mapped to corresponding
       values.

       \param[in,out] xfig The XFig file to which the colors are to be
       written.

       \return The number of color codes added by this method to the
       given XFig file.
    */
    int generateColorScale(XFigHelper& xfig);
};

#endif

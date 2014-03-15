#ifndef XFIG_IMAGE_GENERATOR_CPP
#define XFIG_IMAGE_GENERATOR_CPP

//------------------------------------------------------------
//
// This file is part of HAPLOS availabe off the website at
// <http://pc2lab.cec.miamiOH.edu/haplos>
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

#include "XFigImageGenerator.h"
#include "XFigHelper.h"

#define MIN_COLOR_CODE 33

void
XFigImageGenerator::createImage(const std::string& outFileName,
				const std::vector<std::vector<Location>>& data,
				const int rows, const int cols)
    throw(std::exception) {
    // Create a XFigHelper to help generating an image.
    XFigHelper xfig;
    xfig.setOutput(outFileName); // Setup output file.
    std::cout << std::string(70, '-')
	      << "\nGenerating XFig image to: " << outFileName << std::endl;
    // Generate well defined colors for plotting points
    const int maxColors = generateColorScale(xfig);
    const int radius    = 5, scale = 10;
    // Iterate over the points and generate circles...
    for(size_t r = 0; (r < data.size()); r++) {
	for(size_t c = 0; (c < data.size()); c++) {
	    // Compute color code for 
	    if (data[r][c].getCurrentPopulation() <= 0) {
		int colorCode = MIN_COLOR_CODE +
		    data[r][c].getCurrentPopulation();
		colorCode     = std::min(maxColors, colorCode);
		xfig.drawOval(c * scale, r * scale,
			      radius, radius, colorCode, colorCode);
	    }
	}
    }
    std::cout << "Done generating XFig image.\n"
	      << "You may view the file using xfig or generate other image "
	      << "formats\n(PNG, JPG, GIF, PDF, SVG etc.) using fig2dev.\n"
	      << std::string(70, '-') << std::endl;
}

int
XFigImageGenerator::generateColorScale(XFigHelper& xfig) {
    // Dump custom color codes for further use.
    int colorCode = MIN_COLOR_CODE;
    for(int red = 0; (red < 256); red += 85) {
        for(int blue = 0; (blue < 256); blue += 85) {
            for(int green = 0; (green < 256); green += 85) {
		xfig.addColor(colorCode, red, green, blue);
                colorCode++;
            }
        }
    }
    return colorCode;
}

#endif

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
int redColor=33;
int blueColor=34;
int greenColor=35;
int yellowColor=36;
int orangeColor=37;
void XFigImageGenerator::createImage(const std::string& outFileName,
		const std::vector<std::vector<Location>>& data, const int rows,
		const int cols) throw (std::exception) {
	// Create a XFigHelper to help generating an image.
	XFigHelper xfig;
	xfig.setOutput(outFileName); // Setup output file.
	std::cout << std::string(70, '-') << "\nGenerating XFig image to: "
			<< outFileName << std::endl;
	// Generate well defined colors for plotting points
	const int maxColors = generateColorScale(xfig);
	const int radius = 4, scale = 8;
	// Iterate over the points and generate circles...
	for (size_t r = 0; (r < rows); r++) {
		for (size_t c = 0; (c < cols); c++) {
			// Compute color code for
			if (data[r][c].getCurrentPopulation() <= 0) {
				if(data[r][c].getMaxPopulation()>0){
					//Land but Unpopulated
					int colorCode = MIN_COLOR_CODE
							+ data[r][c].getCurrentPopulation();
					colorCode = std::min(maxColors, colorCode);
					xfig.drawOval(c * scale, r * scale, radius, radius, MIN_COLOR_CODE,
							MIN_COLOR_CODE);
				}
				else{
					//Not Land
					xfig.drawOval(c * scale, r * scale, radius, radius, maxColors,
							maxColors);
				}
			}
			else{
				//Populated
				int colorCode=MIN_COLOR_CODE;
				if((data[r][c].getCurrentPopulation()/500.0)<0.25){
					colorCode=blueColor;
				}
				if((data[r][c].getCurrentPopulation()/500.0)<1.0&&(data[r][c].getCurrentPopulation()/500.0)>=0.25){
					colorCode=greenColor;
				}
				if((data[r][c].getCurrentPopulation()/500.0)<10.0&&(data[r][c].getCurrentPopulation()/500.0)>=1.0){
					colorCode=yellowColor;
				}
				if((data[r][c].getCurrentPopulation()/500.0)<20.0&&(data[r][c].getCurrentPopulation()/500.0)>=10.0){
					colorCode=orangeColor;
				}
				if((data[r][c].getCurrentPopulation()/500.0)>=20.0){
					colorCode=redColor;
				}
				xfig.drawOval(c * scale, r * scale, radius, radius, colorCode,
						colorCode);
			}
		}
	}
	std::cout << "Done generating XFig image.\n"
			<< "You may view the file using xfig or generate other image "
			<< "formats\n(PNG, JPG, GIF, PDF, SVG etc.) using fig2dev.\n"
			<< std::string(70, '-') << std::endl;
}

int XFigImageGenerator::generateColorScale(XFigHelper& xfig) {
	// Dump custom color codes for further use.
	int colorCode = MIN_COLOR_CODE;
	xfig.addColor(colorCode, 0, 0, 0);
	colorCode++;
	redColor=colorCode;
	xfig.addColor(colorCode, 255, 0, 0);
	colorCode++;
	blueColor=colorCode;
	xfig.addColor(colorCode, 0, 0, 255);
	colorCode++;
	greenColor=colorCode;
	xfig.addColor(colorCode, 0, 255, 0);
	colorCode++;
	yellowColor=colorCode;
	xfig.addColor(colorCode, 255, 255, 0);
	colorCode++;
	orangeColor=colorCode;
	xfig.addColor(colorCode, 255, 150, 0);
	colorCode++;
	xfig.addColor(colorCode, 255, 255, 255);
	return colorCode;
}

#endif

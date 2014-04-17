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

#include "ImageGen.h"
#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>

#ifdef HAVE_MAGICK
#include <Magick++.h>

using namespace Magick;
using namespace std;


ImageGen::ImageGen(string outputFolder) {
	// TODO Auto-generated constructor stub

}
/*void ImageGen::createSVGImage(vector<vector<double> > data, int rows, int cols){
	cout<<"Generating Image"<<endl;
	cout<<rows<<" "<<cols<<endl;
	board.setUnit(Board::UPoint);
	int truei=0;
	int truej=0;
	double maxValue=0;
	float cubeSize=0.001;
	for(int i =0; i < rows;i++){
		truej=0;
		for(int j=0;j<cols;j++){
			//cout<<"On Point: "<<i<<" "<<j<<endl;
			createPoint(i*cubeSize,j*cubeSize, data[i][j]);
		}
	}
	cout<<"Done creating objects"<<endl;
	board.scale(100);
	//board.saveSVG( "image1.svg");
	cout<<"Image Created"<<endl;

}*/

void ImageGen::createPNGImage(vector<vector<Location> > data, int rows, int cols){
    std::cout << "Creating Image" << std::endl;
    std::string size = to_string(cols)+"x"+to_string(rows);
    std::cout << "String Size: " << size << std::endl;
	Image image(size, "white" );
    std::cout << "Image Initialzied " << std::endl;
	  for (int i=0; i<rows;i++) {
	  		for (int j=0; j<cols;j++) {
	  			if (data[i][j].getCurrentPopulation()<=0) {
	  					if (data[i][j].getMaxPopulation()>0) {
	  						//Unpopulated Land
		  					image.pixelColor(j, i, ColorRGB(0,0,0));
	  					}

	  				}
                else {
	  					float precentage= data[i][j].getCurrentPopulation()/500.0;
	  					if(precentage>1){
	  						precentage=1;
	  					}
	  					int hueMax=250;
	  					float hue= precentage*hueMax;
	  					hue=hueMax-hue;
	  					double *bob =hsv2rgb(hue,1,1);
	  					//cout<<bob[0]<<" "<<bob[1]<<" "<<bob[2]<<endl;

	  					//cout<<to_string(c->red())<<" "<<to_string(c->green())<<" "<<to_string(c->blue())<<endl;
	  					image.pixelColor(j, i, ColorRGB(bob[0],bob[1],bob[2]));
	  				}
	  		}
	  	}
    std::cout << "Done making Image writing" << std::endl;
    image.write( "red_pixel.png" );

}

/*void ImageGen::createPoint(float x, float y, float value){
	if(value<=0){
		board.setPenColorRGBi( 0, 0, 0 );
	}else{
		float precentage= value/500.0;
		if(precentage>1){
			precentage=1;
		}
		int hueMax=250;
		float hue= precentage*hueMax;
		hue=hueMax-hue;
		int *color =hsv2rgb(hue,1,1);
		board.setPenColorRGBf(color[0],color[1], color[2]);
		//board.setPenColorRGBf(222,0, 0);

	}
	board.fillRectangle(x, y, 0.001,0.001);

}*/

double* ImageGen::hsv2rgb(double h, double s, double v)
{
	double      hh, p, q, t, ff;
	long        i;
	static double out[3];
    
    //Invalid Saturation, set to Black
	if (s <= 0.0) {
		out[0] = h;
		out[1] = s;
		out[2] = v;
	}
    
	hh = h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
        case 0:
            out[0] = v;
            out[1] = t;
            out[2] = p;
            break;
            
        case 1:
            out[0] = q;
            out[1] = v;
            out[2] = p;
            break;
            
        case 2:
            out[0] = p;
            out[1] = v;
            out[2] = t;
            break;

        case 3:
            out[0] = p;
            out[1] = q;
            out[2] = v;
            break;
            
        case 4:
            out[0] = t;
            out[1] = p;
            out[2] = v;
            break;
            
        default:
            out[0] = v;
            out[1] = p;
            out[2]= q;
            break;
	}
	return out;
}

ImageGen::~ImageGen() {
	// TODO Auto-generated destructor stub
}

#endif

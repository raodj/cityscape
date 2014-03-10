/*
 * ImageGen.h
 *
 *  Created on: Mar 2, 2014
 *      Author: schmidee
 */

#ifndef IMAGEGEN_H_
#define IMAGEGEN_H_
#include <string>
#include <vector>
#include "Location.h"

using namespace std;

class ImageGen {
	public:
		ImageGen(string outputFolder);
		void createSVGImage(vector<vector<Location> > data, int rows, int cols);
		void createPNGImage(vector<vector<Location> > data, int rows, int cols);
		virtual ~ImageGen();
	private:
		double*  hsv2rgb(double h, double s, double v);
		void createPoint(float x, float y, float value);

};

#endif /* IMAGEGEN_H_ */

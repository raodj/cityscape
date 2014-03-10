/*
 * sedacReader.h
 *
 *  Created on: Mar 2, 2014
 *      Author: schmidee
 */

#ifndef SEDACREADER_H_
#define SEDACREADER_H_
#include <string>
#include <vector>
#include "Location.h"



using namespace std;


class SedacReader {
public:
	SedacReader();
	vector<vector<Location> > readFile(string filename);
	virtual ~SedacReader();
};


#endif /* SEDACREADER_H_ */

//============================================================================
// Name        : haplos.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include "SedacReader.h"
#include "ImageGen.h"
#include "Location.h"
#include "Population.h"
#include <vector>
using namespace std;

string sedacDataFile="data/usap00ag.asc";
string outputFolder="output/";
//double** densityData;


int main() {
	vector<vector<Location> > densityData;
	int const numberOfPeople=283230000; //281424000;
	cout << "-----HAPLOS-----" << endl;
	SedacReader *sr = new SedacReader();
	densityData=sr->readFile(sedacDataFile);
	Population *pop = new Population(numberOfPeople);
	int x=0;
	int y=0;
	int notAssigned=0;
	cout<<"Assigning Locations to Population"<<endl;
	for(int i =0; i<numberOfPeople;i++){
		while(densityData[x][y].isFull()){
			x++;
			if(x>=densityData.size()&&y<=densityData[0].size()){
				x=0;
				y++;
			}
			else{
				if(y>=densityData[0].size()){
					notAssigned++;
					break;
				}
			}
			cout<<x<<" "<<y<<endl;
		}

		if(y>=densityData[0].size()){
			break;
		}
		pop->setLocationOfPerson(x, y, i);
		densityData[x][y].addPerson();
	}
	cout<<"Not Assigned: "<<notAssigned<<endl;
	cout<<"Population Assigned Locations"<<endl;
	ImageGen *ig = new ImageGen(outputFolder);
	ig->createPNGImage(densityData, densityData.size(), densityData[0].size());
	pop->displayStatistics();
	return 0;
}



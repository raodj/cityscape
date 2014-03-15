/*
 * sedacReader.cpp
 *
 *  Created on: Mar 2, 2014
 *      Author: schmidee
 */

#include "SedacReader.h"
#include "Location.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

SedacReader::SedacReader() {
	// TODO Auto-generated constructor stub

}

SedacReader::~SedacReader() {
	// TODO Auto-generated destructor stub
}

vector<vector<Location> > SedacReader::readFile(string fileName){
		vector<vector<Location> > densityData;
		int cols;
		int rows;
		cout<<fileName.c_str()<<endl;
	    string line;
		ifstream infile;
		infile.open (fileName.c_str());
		if(infile.fail()){
			cout<<"Unable to Open SEDAC Data File."<<endl;
			return densityData;
		}
		//Get Number of Rows
		getline(infile,line);
		unsigned pos = line.find_last_of(" ");
		cols = std::stoi(line.substr(pos+1).c_str());
		cols=2750; 	//Cut out Random Few islands off the cost of alaska
		//Get Number of cols
		getline(infile,line);
		pos = line.find_last_of(" ");
		rows=std::atoi(line.substr(pos+1).c_str());
		//cout<<"Number of Rows: "<<rows<<" Number of Columns: "<<cols<<endl;
		//Ignore These Lines
		getline(infile,line);
		getline(infile,line);
		getline(infile,line);

		//Get No Data Value
		getline(infile,line);
		pos = line.find_last_of(" ");
		int noDataValue=std::atoi(line.substr(pos+1).c_str());
		cout<<"Starting Processing"<<endl;
		//densityData = new double*[rows];
		densityData.resize(rows);
		for(int i =0; i < rows; i++){
			densityData[i].resize(cols);
		}
		cout<<"Data Allocated"<<endl;
		int currentRow=0;
		bool actualData=false;
		float maxVal=0;
		//Get all the Data
		//double totalSpace=0;
		while(!infile.eof()) // To get you all the lines.
		{
			//Process Line
			getline(infile,line);
			//cout<<line<<endl;
		    string buf; // Have a buffer string
		    stringstream ss(line); // Insert the string into a stream

		    vector<string> tokens; // Create vector to hold our words

		    while (ss >> buf){
		        tokens.push_back(buf);
		    }
		   // cout<<tokens.size()<<endl;
		    vector<string>::iterator i;
		    int currentColumn=0;
		    for (vector<string>::iterator i = tokens.begin();i != tokens.end();++i){
		    	if(currentColumn<cols){
		    		//totalSpace+=(std::atof((*i).c_str()))*1000;
					densityData[currentRow][currentColumn]= Location(currentRow, currentColumn, (std::atof((*i).c_str())));
					//densityData[currentRow][currentColumn].operator =(new Location(currentRow, currentColumn, (std::atof((*i).c_str())*1000)));
		    		currentColumn++;
		    	}else{
		    		break;
		    	}
		    }
			currentRow++;
		}
	//	cout<<"Total Space: "<<totalSpace<<endl;
		cout<<"Done reading Data"<<endl;
		infile.close();
		return densityData;

}


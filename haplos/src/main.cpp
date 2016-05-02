#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include "Haplos.h"
#include "Policy.h"

using namespace std;

int main(int argc, char* argv[]) {

    std::string configFile;
    bool produceImages =false;
    bool progressDisplay = false;
    bool exportFiles = false;
    
    //Command Line Paramaters
    if(argc>0){
        //Do Not Use Default
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if ((arg == "--configFile") || (arg=="-c")) {
                //Configuration File Path
                if (i + 1 < argc) {
                    configFile=argv[++i];
                }else{
                    std::cerr << "--configFile option requires one argument." << std::endl;
                }
            }else{
                if((arg=="--noImages")||(arg=="-ni")){
                    //No Images Produced
                    produceImages=false;
                }
                if((arg=="--noProgressDisplay")|| (arg=="-npd")){
                    progressDisplay=false;
                }
                if((arg=="--export")|| (arg=="-e")){
                    exportFiles=true;
                }
                
            }
        }
    }
    Policy p = Policy();
    Haplos hap = Haplos(configFile, produceImages, progressDisplay, exportFiles);
    hap.runSimulation(&p);
    return 0;
}

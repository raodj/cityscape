"""
Schedule Test Main Class

Executes both Family and Person Specific Tests on all outputted Families in the 
familyData folder of a HAPLOS output.

How to Run:
python main.py HAPLOS_FOLDER_PATH

HAPLOS_FOLDER_PATH: Path to Folder Containing Output from a HAPLOS Run
"""	

import StringProcessor
import os
import sys
import glob
import subprocess
import time

noFails = True;
subprocess.call(["clear"])
#folderPath="../output/"+sys.argv[1]+"/familyData/";
#files=os.listdir(folderPath) 
runCount= 0;
maxRuns = 50000000;
subprocess.call("rm -rf ../output/", shell=True); 
subprocess.call("mkdir ../output/", shell=True); 
os.chdir("..")
subprocess.call("pwd", shell=True);
subprocess.call("make", shell=True);
os.chdir("tests");

while(noFails and maxRuns >= runCount ):
	subprocess.call(["clear"])
	print "---Executing Run "+str(runCount)+"---"
	print "Rerunning HAPLOS"
	subprocess.call("../haplos -c ../examples/config/MicroWorldData.hapl -ni -npd > ../output/commandLineOutput.txt 2>&1", shell=True);
	print "HAPLOS Complete"
	folders=os.listdir("../output/");
	if not os.path.exists("../output/complete.txt"):
		print "Program never completed. Exiting.\n";
		sys.exit()

	for folder in folders:
		if not folder.startswith('.') and folder != "commandLineOutput.txt" and folder != "complete.txt":
			fail = False
			files=os.listdir("../output/"+folder+"/familyData");
			for file in files:
				print "Processing File: "+file;
				fail = StringProcessor.processFamily("../output/"+folder+"/familyData/"+file)
				if fail:
					noFails = False
			if not noFails: 
				print "Failure Occurred"
				sys.exit();
			#time.sleep(3);
			print "Test Succcessfull Cleaning Up Directory: "+folder
			subprocess.call("rm -rf ../output/"+folder, shell=True)
			subprocess.call("rm -rf ../output/complete.txt", shell=True)

				
	runCount= runCount + 1;
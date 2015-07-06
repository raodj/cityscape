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
folderPath="../svn/haplos/haplos/output/"+sys.argv[1]+"/familyData/";
files=os.listdir(folderPath)
for file in files:
	print "Processing File: "+file;
	StringProcessor.processFamily(folderPath+file)
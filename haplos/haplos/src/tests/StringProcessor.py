"""
HAPLOS String Parser
"""
import csv
import re
import Person
import TestFamily
import TestPerson
	
"""
processFamily
fileLocation: Path to Family File

Process Family File in order to create Person Objects. Executes Tests on Family and on
each individual person.
"""
def processFamily(fileLocation):
	fail = False;
	with open(fileLocation, 'rb') as csvfile:
		#familyFile = csv.reader(filePath);
		#for row in csvfile:
		#	print row
		data=csvfile.read().strip()
		csvfile.close()

		partSplit= re.split("\n\n", data)
		genInfo = partSplit.pop(0).strip()
		temp = genInfo.split("\n")
		genInfo=[];
		for line in temp:
			genInfo.append(line.split(","))
		
		family ={}
		for part in partSplit:
			part = part.strip()
			newPerson =processPerson(part)
			family[int(newPerson.id)] = newPerson
		tempFail = TestFamily.runFamilyTests(family, genInfo[0][1])
		if  not tempFail:
			noFail=True
		childCareAdultInfo = genInfo[2][1].split(" ");
		for person in family.keys():
			tempFail= TestPerson.runPersonTests(family[person], genInfo[0][1], family[int(childCareAdultInfo[0][1:])])
			if  not tempFail:
				fail=True
	return fail
		
"""
processPerson
personString: String in HAPLOS Person format

Helper Method to Generate a Person Object from a String
"""	
def processPerson(personString):
	newPerson=Person.Person(personString)
	return newPerson
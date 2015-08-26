"""
Family Specific Tests
"""
familyTestFailed=False

"""
runFamilyTests
family: List of Persons in Family
number: Number of Family

Runs all Family Specific Tests
"""
def runFamilyTests(family, number):
	global familyTestFailed
	if  hasAdultTest(family) is False and familyTestFailed is False:
		print "**Test Failed: Family "+number+" Does not Have An Adult."
		familyTestFailed=True;
	if familyTestFailed:
		return False
	return True

"""
hasAdultTest
family: List of Persons in Family

Test to make sure family has at least one adult
"""
def hasAdultTest(family):
	for person in family.keys():
		if family[person].age>=18:
			return True
	return False
	
	

"""
Person Specific Tests
"""
import EmployedAdultTests
import UnemployedAdultTests
import SchoolChildTests
import YoungSchoolChildTests
import YoungChildTests


noExtraTimeSlotsFail = False
"""
runPersonTests
person: Person Object
familyNumber: Family Number Person Belongs to
childCareAdult: Adult responsible for Child Care

Run Specific Person Tests based on Schedule
"""
def runPersonTests(person, familyNumber, childCareAdult):
	global noExtraTimeSlotsFail
	
	#Non Schedule Specific Tests	
	if noExtraTimeSlotsFail == False and noExtraTimeSlots(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Has Extra Time Slots("+person.type+")"
		noExtraTimeSlotsFail=True;
		
	#Run Schedule Specific Tests
	if person.type == 'Employed Adult':
		EmployedAdultTests.runAllTests(person, familyNumber)
	else:
		if person.type=="Unemployed Adult":
			UnemployedAdultTests.runAllTests(person, familyNumber)
		else:
			if person.type == "School Child":
				SchoolChildTests.runAllTests(person, familyNumber)
			else:
				if person.type == "Young School Child":
					YoungSchoolChildTests.runAllTests(person, familyNumber, childCareAdult)
				else:
					if person.type == "Young Child": 
						YoungChildTests.runAllTests(person, familyNumber, childCareAdult)
					else:
						print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Has Invalid Schedule Type"
						
def noExtraTimeSlots(person):
	schedule=person.schedule
	lastLocation= "-999999";
	lastVisitorType = 'N';
	
	for timeSlot in schedule:
		#print timeSlot[0]+" "+timeSlot[1]+" "+timeSlot[2]
		#print "Last One: " +lastVisitorType+ " " +lastLocation
		if timeSlot[1]==lastVisitorType and timeSlot[0] == lastLocation:
			print "Test Failed on "+timeSlot[0]+" "+timeSlot[1]+" "+timeSlot[2];
			return False
		lastVisitorType = timeSlot[1]
		lastLocation = timeSlot[0]
	return True;

					
		
	
"""
Person Specific Tests
"""
import EmployedAdultTests
import UnemployedAdultTests
import SchoolChildTests
import YoungSchoolChildTests
import YoungChildTests


noExtraTimeSlotsFail = False
noTimeSlotsSequencialFail = False
"""
runPersonTests
person: Person Object
familyNumber: Family Number Person Belongs to
childCareAdult: Adult responsible for Child Care

Run Specific Person Tests based on Schedule
"""
def runPersonTests(person, familyNumber, childCareAdult):
	global noExtraTimeSlotsFail
	global noTimeSlotsSequencialFail
	#Non Schedule Specific Tests	
	if noExtraTimeSlotsFail == False and noExtraTimeSlots(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Has Extra Time Slots("+person.type+")"
		noExtraTimeSlotsFail=True;
	
	if noTimeSlotsSequencialFail == False and timeSlotsSequencial(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Time Slots are not Sequencial("+person.type+")"
		noTimeSlotsSequencialFail=True;
	
	tempFail = True	
	#Run Schedule Specific Tests
	if person.type == 'Employed Adult':
		tempFail = EmployedAdultTests.runAllTests(person, familyNumber)
	else:
		if person.type=="Unemployed Adult":
			tempFail = UnemployedAdultTests.runAllTests(person, familyNumber)
		else:
			if person.type == "School Child":
				tempFail = SchoolChildTests.runAllTests(person, familyNumber)
			else:
				if person.type == "Young School Child":
					tempFail = YoungSchoolChildTests.runAllTests(person, familyNumber, childCareAdult)
				else:
					if person.type == "Young Child": 
						tempFail = YoungChildTests.runAllTests(person, familyNumber, childCareAdult)
					else:
						print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Has Invalid Schedule Type"
						return False
	if not tempFail or noExtraTimeSlotsFail or noTimeSlotsSequencialFail:
		return False
	return True
						
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


def timeSlotsSequencial(person):
	schedule=person.schedule
	lastTimeSlot = 0;
	for timeSlot in schedule:
		if int(timeSlot[2]) < lastTimeSlot:
			print "Test Failed on "+timeSlot[0]+" "+timeSlot[1]+" "+timeSlot[2]
			print "Previous Time Slot Stopped At: "+str(lastTimeSlot)
			return False
		lastTimeSlot = int(timeSlot[2])
	return True;		
	
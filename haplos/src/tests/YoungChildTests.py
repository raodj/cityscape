"""
Young Child Specific Tests
"""
youngChildDoesNotGoToJobFailed=False
YoungChildFollowAdultFail = False

"""
runAllTests 
person: Person Object
familyNumber: Family Number Person Belongs to
childCareAdult: Adult who is responsible for child care

Runs all Tests relating to a Young Child 
"""
def runAllTests(person, familyNumber, childCareAdult):
	global youngChildDoesNotGoToJobFailed
	global YoungChildFollowAdultFail
	if youngChildDoesNotGoToJobFailed==False and doNotGoToJob(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Goes to Work, but is a Young Child."
		youngChildDoesNotGoToJobFailed=True
	
	if YoungChildFollowAdultFail == False and followsAdult(person, childCareAdult) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Doesn't Follow Adult (Young)."
		YoungChildFollowAdultFail=True

	if youngChildDoesNotGoToJobFailed or YoungChildFollowAdultFail:
		return False
	return True
"""
doNotGoToJob 
person: Person Object

Checks to make sure Young Child does not go to job
"""			
def doNotGoToJob(person):
	schedule=person.schedule
	for timeSlot in schedule:
		if timeSlot[1]=='E':
			return False;
	return True;
"""
followsAdult 
person: Person Object

Checks to make sure Young Child follows childCareAdult's schedule unless they are at work.
If they are at work they are at a daycare location.
"""	
def followsAdult(person, childCareAdult):
	schedule=person.schedule
	childCareSchedule = childCareAdult.schedule
	placeInSchedule = 0
	atSchool =False;
	atSchoolTill = 0;
	for timeSlot in schedule:
		if not atSchool:
			if timeSlot[1] == 'D':
				atSchool=True
				atSchoolTill = int(timeSlot[2]) 
			else:
				if timeSlot != childCareSchedule[placeInSchedule]:
					print timeSlot
					print childCareSchedule[placeInSchedule]
					return False
			placeInSchedule+=1
		else:
			while int(childCareSchedule[placeInSchedule][2])<=atSchoolTill:
				placeInSchedule+=1
					
	return True		
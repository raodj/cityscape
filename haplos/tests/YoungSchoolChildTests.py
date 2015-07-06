"""
Young School Child Specific Tests
"""

YoungSchoolChildGoesToSchoolFail = False
YoungToSchoolOnWeekdaysOnlyFail = False
YoungToSchoolFollowAdultFail = False

"""
runAllTests 
person: Person Object
familyNumber: Family Number Person Belongs to
childCareAdult: Adult who is responsible for child care

Runs all Tests relating to a Young School Child 
"""
def runAllTests(person, familyNumber, childCareAdult):	
	global YoungToSchoolOnWeekdaysOnlyFail
	global YoungSchoolChildGoesToSchoolFail
	global YoungToSchoolFollowAdultFail
	if  YoungSchoolChildGoesToSchoolFail == False and goesToSchool(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Never Goes to School (Young)."
		YoungSchoolChildGoesToSchoolFail=True
	
	if  YoungToSchoolOnWeekdaysOnlyFail == False and goesToSchoolOnWeekdaysOnly(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Doesn't Go to School Only on Weekdays (Young)."
		YoungToSchoolOnWeekdaysOnlyFail=True
	if  YoungToSchoolFollowAdultFail == False and followsAdult(person, childCareAdult) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Doesn't Follow Adult (Young)."
		YoungToSchoolFollowAdultFail=True
		
"""
goesToSchool
person: Person Object

Tests that Person goes to School At least Once
"""			
def goesToSchool(person):
	schedule=person.schedule
	for timeSlot in schedule:
		if timeSlot[1]=='S':
			return True;
	return False;

"""
goesToSchoolOnWeekdaysOnly
person: Person Object

Tests that Person goes to school only on weekdays
"""		
def goesToSchoolOnWeekdaysOnly(person):
	day = 0
	goneToSchool=False
	weekend=False
	schedule=person.schedule

	for timeSlot in schedule:
		time = timeSlot[3]
		if time[0] != str(day):
			#New Day
			if goneToSchool==False and weekend==False:
				print "Failed Never went to School on Day"+str(day)
				return False
			else:
				goneToSchool=False
				if day + 1 >4:
					weekend=True
				day=day+1
		if timeSlot[1] == 'S':
			if weekend==False:
				goneToSchool=True
			else:
				print "Went to School on a weekend "+str(day)
				return False
	return True
	
"""
followsAdult 
person: Person Object

Checks to make sure Young Child follows childCareAdult's schedule unless they are at work.
If they are at work they are at a School location.
"""	
def followsAdult(person, childCareAdult):
	schedule=person.schedule
	childCareSchedule = childCareAdult.schedule
	placeInSchedule = 0
	atSchool =False;
	atSchoolTill = 0;
	for timeSlot in schedule:
		if not atSchool:
			if timeSlot[1] == 'S':
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
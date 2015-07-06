"""
School Child  Specific Tests

"""

SchoolChildGoesToSchoolFail = False
SchoolChildToSchoolOnWeekdaysOnlyFail = False
SchoolChildToSchool8HourMinFail = False

"""
runAllTests 
person: Person Object
familyNumber: Family Number Person Belongs to

Runs all Tests relating to a School Child Schedule
"""
def runAllTests(person, familyNumber):
	global SchoolChildGoesToSchoolFail
	global SchoolChildToSchoolOnWeekdaysOnlyFail
	global SchoolChildToSchool8HourMinFail
	if SchoolChildGoesToSchoolFail == False and goesToSchool(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Never Goes to School."
		SchoolChildGoesToSchoolFail=True
	
	if SchoolChildToSchoolOnWeekdaysOnlyFail == False and goesToSchoolOnWeekdaysOnly(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Doesn't Go to School Only on Weekdays."
		SchoolChildToSchoolOnWeekdaysOnlyFail=True	
		
	if SchoolChildToSchool8HourMinFail== False and atHomeFor8HoursMin(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" isn't Home for 8 Hours (School Child)."
		SchoolChildToSchool8HourMinFail=True

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
atHomeFor8HoursMin
person: Person Object

Tests that person is at home at least for 8 continuous hours every 24 hours 
"""	
def atHomeFor8HoursMin(person):
	day=1
	schedule=person.schedule
	lastTimeSlot= 0
	totalTimeSpentOut=0;
	for timeSlot in schedule:
		time = timeSlot[3]
		if timeSlot[1]!='H':
			totalTimeSpentOut=(int(timeSlot[2])-lastTimeSlot)+totalTimeSpentOut;
		else: 
			#print "Home: "+ str(timeSlot[2])+" "+str((int(timeSlot[2])-lastTimeSlot))
			if (int(timeSlot[2])-lastTimeSlot)>=48:
				#Did spend enough time at home
				totalTimeSpentOut=0;
			else:
				#Didn't spend enough time at home
				totalTimeSpentOut=(int(timeSlot[2])-lastTimeSlot)+totalTimeSpentOut;
		if time[0] == str(day):
			day=day+1
		if(totalTimeSpentOut>144):
			print "**Fail: Day "+str(day-1)+" Total: "+str(totalTimeSpentOut)+" "+timeSlot[3]
			return False
		lastTimeSlot=int(timeSlot[2])
	return True;
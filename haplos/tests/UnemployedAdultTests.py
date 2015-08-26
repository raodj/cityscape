"""
Unemployed Adult Specific Tests
"""
unemployedDoesNotGoToJobFailed=False
unemployedAtHomeFor8HoursMinFailed=False

"""
runAllTests 
person: Person Object
familyNumber: Family Number Person Belongs to

Runs all Tests relating to a Unemployed Adult
"""
def runAllTests(person, familyNumber):
	global unemployedDoesNotGoToJobFailed
	global unemployedAtHomeFor8HoursMinFailed
	if  unemployedDoesNotGoToJobFailed==False and doNotGoToJob(person) == False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Goes to Work But Unemployeed."
		unemployedDoesNotGoToJobFailed=True;
	
	if unemployedAtHomeFor8HoursMinFailed==False and atHomeFor8HoursMin(person) == False  :
		print "**Test Failed: Unemployeed Person "+person.id+" in Family "+familyNumber+" Not Home for 8 Hours."
		unemployedAtHomeFor8HoursMinFailed=True;
		
	if unemployedDoesNotGoToJobFailed or unemployedAtHomeFor8HoursMinFailed:
		return False
	return True
		
"""
doNotGoToJob 
person: Person Object

Checks to make sure Unemployed Adult does not go to job
"""		
def doNotGoToJob(person):
	schedule=person.schedule
	for timeSlot in schedule:
		if timeSlot[1]=='E':
			return False;
	return True;

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
"""
Employed Adult Specific Tests
"""
employedGoesToJobFailed = False
employedAtHomeFor8HoursMinFailed=False

"""
runAllTests 
person: Person Object
familyNumber: Family Number Person Belongs to

Runs all Tests relating to a Employed Adult
"""
def runAllTests(person, familyNumber):
	global employedGoesToJobFailed
	global employedAtHomeFor8HoursMinFailed

	if employedGoesToJobFailed==False and goesToJob(person)== False:
		print "**Test Failed: Person "+person.id+" in Family "+familyNumber+" Never Goes to Work."
		employedGoesToJobFailed=True	
	
	if employedAtHomeFor8HoursMinFailed==False and atHomeFor8HoursMin(person) == False  :
		print "**Test Failed: Employeed Person "+person.id+" in Family "+familyNumber+" Not Home for 8 Hours."
		employedAtHomeFor8HoursMinFailed=True;
	if employedGoesToJobFailed or employedAtHomeFor8HoursMinFailed:
		return False
	return True

"""
goesToJob
person: Person Object

Tests that Person goes to Job at least once.
"""			
def goesToJob(person):
	schedule=person.schedule
	#print schedule
	for timeSlot in schedule:
		#print timeSlot[1]
		if timeSlot[1]=='E':
			return True;
	return False;

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
			print "**Fail: Day "+str(day-1)+" Total: "+str(totalTimeSpentOut)+" "+timeSlot[3] + " " + timeSlot[2]
			return False
		lastTimeSlot=int(timeSlot[2])
	return True;


"""
atHomeFor8HoursMin
person: Person Object

Tests that person has been at home for 8 hours total every 24 hours. This is a 
modification to the Normal Test due to adults can get interruped during sleeping to 
have to drop children off at school.
"""
"""
def atHomeFor8HoursMin(person):
	day=1
	schedule=person.schedule
	lastTimeSlot= 0
	timeSpentAtHome = 0;
	for timeSlot in schedule:
		time = timeSlot[3]
		if timeSlot[1]=='H':
			#print "Home: "+ str(timeSlot[2])+" "+str((int(timeSlot[2])-lastTimeSlot))
			timeSpentAtHome=timeSpentAtHome+int(timeSlot[2])-lastTimeSlot;
		if time[0] == str(day):
			leftOver = int(timeSlot[2])%144;
			timeSpentAthome = timeSpentAtHome - leftOver;
			if(timeSpentAtHome<48):
				print "**Fail: Day "+str(day-1)+" Total: "+str(timeSpentAtHome)+" "+timeSlot[3]+ " " + timeSlot[2]
				return False
			timeSpentAtHome=leftOver;
			day=day+1
		lastTimeSlot=int(timeSlot[2])
	return True;
"""
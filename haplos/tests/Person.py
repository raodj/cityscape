"""
Person Class

Attributes:
id: ID of person
age: Age of person
type: Type of Schedule class a person has
schedule: Schedule of Person
"""	
class Person:

	"""
	Constructor
	inputString: String containing output of a single person from HAPLOS
	"""	
	def __init__(self, inputString):
		newLineSplit = inputString.replace('"','').split("\n")
		processed=[];
		for line in newLineSplit:
			processed.append(line.split(","))
		self.id=processed[0][1]
		self.age=processed[1][1]
		self.type=processed[3][1]
		#print processed
		processed.pop(0)
		processed.pop(0)
		processed.pop(0)
		processed.pop(0)
		processed.pop(0)
		self.schedule=processed

	"""
	display

	Helper method to print a person's details to the screen
	"""	
	def display(self):
		print "ID: "+self.id
		print "Age: "+self.age
		print "Type: "+self.type
		print "----------"
		print self.schedule
		
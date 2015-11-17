import os
import sys
import numpy
import random
import png


file=sys.argv[1]
width = int(sys.argv[2])
height = int(sys.argv[3])
numberOfPeople = int(sys.argv[4])

matrix = numpy.zeros((width, height));

f = open(file, 'w')
max = 0;
for num in range(0, numberOfPeople):
	x = int(random.triangular(0, width, width/2));
	y = int(random.triangular(0, height, height/2))
	matrix[x][y] += 1;
	if matrix[x][y] > max:
		max = matrix[x][y]

f.write("ncols         "+str(height)+"\n");
f.write("nrows         "+str(width)+"\n");
f.write("xllcorner     -1\n");
f.write("yllcorner     -1\n");
f.write("cellsize      0.0416666666667\n");
f.write("NODATA_value  -9999\n");

numpy.savetxt(f, matrix,'%1i' ,delimiter=" ")
#for w in range(0, width-1):
#	f.write(' '.join(map(str, matrix[w]))+"\n");
f.close
print int(max)
f = open("test.png", "wb");
p = []

for w in range(0, width):
	tmp = ()
	for h in range (0, height):
		x=0;
		if int(matrix[w][h]) != 0:
			x = float(int(matrix[w][h])-0)/int(max)
		t= (255-(x*255), 255-(x*255), 255-(x*255));
		tmp=tmp+(int(255-(x*255)), int(255-(x*255)), int(255-(x*255)), )
	p.append(tmp)

w = png.Writer(width, height)
w.write(f, p)
f.close()		
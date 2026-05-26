#
# This file requires gnuplot 5.0
#

# Assume input data file
dataFile = "Taxi_Trips_ByArea_Matrix.tsv"

# Set input and output file formats
set terminal pdfcairo enhanced color size 4in, 4in
set output "Taxi_Trips_ByArea_Matrix.pdf"
# set datafile separator '\t'

set xrange [0:77]
set yrange [0:77]
set cblabel "Rides (in millions)"

# Plot trips for each of the 77 areas
plot dataFile matrix with image

# End of script



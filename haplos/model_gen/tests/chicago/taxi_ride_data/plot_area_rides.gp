#
# This file requires gnuplot 5.0
#

# Assume input data file
dataFile = "Taxi_Trips_ByArea.tsv"
infoFile = "../area_info.tsv"

# Set input and output file formats
set terminal pdfcairo enhanced color size 8in, 3in
set output "Taxi_Trips_ByArea.pdf"
set datafile separator '\t'

# Setup properties for histograms
set boxwidth 0.4 absolute
set style fill solid 0.50

# Set range to 77 areas
set xrange[0:78]
set xtics 5 format " " nomirror
set x2tics 5 nomirror

# Named constant to improve readability
million = 1000000.0
red = '#d95319'
yellow = '#edb120'

# The average number of trips per area (for reference)
avgTrips = 1209158.0 / million
sd       = 4447313.0 / million
oneSD    = (avgTrips + sd)

set ylabel "Taxi rides\n(in millions)" offset 9, 0 font " Bold"

# Set lines for average and avarege+sd
set arrow nohead from 0, avgTrips to 78, avgTrips front lc rgb yellow
set label sprintf("Avg: %.2f", avgTrips) at 65, avgTrips + 1 tc rgb yellow font " Bold,8"

set arrow nohead from 0, oneSD to 78, oneSD front lc rgb red
set label sprintf("Avg+SD: %.2f", oneSD) at 65, oneSD + 1 tc rgb red font " Bold,8"

# Increase margins at bottom to accommodate area name
set bmargin 9

# Plot trips for each of the 77 areas
plot dataFile using ($1-0.2):($3/million) with boxes title "Pickups",\
     dataFile using ($2+0.2):($3/million) with boxes title "Drop-offs",\
     infoFile using 1:(0):2 with labels rotate right offset 0, -0.1 font ",8" notitle

# End of script



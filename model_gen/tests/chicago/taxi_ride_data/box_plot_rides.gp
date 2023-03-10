#
# This file requires gnuplot 5.0
#

# Assume input data file
dataFile = "Taxi_Trips_StartTimes.tsv"

# Set input and output file formats
set terminal pdfcairo enhanced color size 6.5in, 9in
set output "Taxi_Trips_StartTimes.pdf"
set datafile separator '\t'

# The year that we would like to plot
Year = 2014

# Convenience function to get the data
getData(day, time) = sprintf("< grep '/%d ..:..:.. ..\t%d\t%d\t' %s", Year, day, time, dataFile)
# print getData(0, 0)

# Setup properties for boxplot
set boxwidth 0.75 absolute
set style fill solid 0.50 border lt -1
unset key
set style increment default
set pointsize 0.25
set style data boxplot
set bars 0.5

# Plot chart for 7 days of the week (sunday through saturday)
set multiplot layout 7, 1 title "Summary of daily traffic from 2014" font " Bold"
DayName = "Sunday Monday Tuesday Wednesday Thursday Friday Saturday"

# The data is in 15 minute intervals -- 4 intervals per hour == 96 tics
set xrange [0:96]

# To address issue with box plotting versus different x-tics we
# manually generate xtics.
set xtics (0, 0)
do for [blk=0:24] {
    set xtics add (sprintf("%d", blk) blk * 4)
}

do for [day=0:6] {

    # Set title for this chart to enable easy identification of day of week
    set title word(DayName, day + 1)  offset 0, -2 font " Bold"
    
    # Total number of minutes in a week. Here we skip over the minunutes
    # in previous days for convenient iteration in the plot command below
    startDayMins = (day + 0) * 60 * 24
    endDayMins   = (day + 1) * 60 * 24 - 1

    # Plot trips in 15 minute intervals for the given day.
    plot for [timeOfDay=startDayMins:endDayMins:15] \
        getData((timeOfDay / 1440), (timeOfDay % 1440))\
        using ((timeOfDay - startDayMins) / 15):4\
        lc ((timeOfDay / 1440) + 1) notitle

}

# Finish multiplot (otherwise chart will not be drawn)
unset multiplot

# End of script



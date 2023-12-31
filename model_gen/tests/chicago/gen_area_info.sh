#!/bin/bash

# A simple shell script to generate area summary information from the
# ring information csv file generated by model_gen.
#
# NOTE: Script makes implicit assumptions about column order in CSV file
#
scriptDir="$( dirname "${BASH_SOURCE[0]}" )"
csv="${scriptDir}/bld_info_data_2022.csv"

# The above csv file is generated via:
# echo "ID, population, totSqFootage, buildings, Sqft/person, info" > "bld_info_data_2022.csv"
# grep "area_numbe" bld_info_2023.txt >> bld_info_data_2022.csv

# Print generic header
echo -e "#AreaID\tName\tTotPop\tTotSqFt\tTotBlds"

# Print information for each area
for area in `seq 1 77`
do
    # Setup our area identification key
    key="area_numbe='${area}'"
    # Get the area name from the data file.
    name=`grep ${key} ${csv} | tail -1 | cut -d"'" -f10`
    # Extract total population by adding up all entries for this area.
    totPop=`grep ${key} ${csv} | cut -d"," -f2 | paste -sd+ | bc`
    # Extract total sqFoot by adding up all entries for this area.
    totSqFt=`grep ${key} ${csv} | cut -d"," -f3 | paste -sd+ | bc`
    # Extract total buildings by adding up all entries for this area.
    totBlds=`grep ${key} ${csv} | cut -d"," -f4 | paste -sd+ | bc`
    # Print the information
    echo -e "${area}\t${name}\t${totPop}\t${totSqFt}\t${totBlds}"
done

# End of script

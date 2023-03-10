#!/bin/bash

# This is the file that contains all taxi ride data. This file is
# downloaded from the chicago taxi trips portal as indicated further
# below in this script.
FullTaxiRideFile="Taxi_Trips.tsv"

# This is a script to run the riskanayzer to generate a TSV of risk
# scores for a given year (as command-line argument).
year=2018
if [ $# -gt 1 ]; then
    # Override default year with command-line argument.
    year="$1"
fi

echo "Running risk analysis for year ${year}"

# First download the giant taxi cab ride data from the chicago portal
# The following file can be ~100 GB and will take a while depending on your
# internet connection speed.
if [ ! -e "${FullTaxiRideFile}" ]; then
    echo "Did not find full taxi data file ${FullTaxiRideFile}"
    echo "This is a large file and you can download it separately"
    echo "via the following command (it is about 100 GB and takes a while"
    echo "to download based on your internet speed)"
    echo "wget https://data.cityofchicago.org/api/views/wrvz-psew/rows.tsv?accessType=DOWNLOAD&bom=true"
    exit 1
fi

# We also need the chicago census tracts shape files for anaysis. This
# needs to be downloaded just once.
CensusTractDir="chicago_census_tracts_2010"
if [ ! -d "${CensusTractDir}" ]; then
    echo "Downloading Chicago 2010 census tract shape files..."
    mkdir "${CensusTractDir}"
    if [ $? -ne 0 ]; then
        echo "Error creating directory ${CensusTractDir}"
    fi

    cd "${CensusTractDir}"
    # Download the census tract file.
    wget "https://data.cityofchicago.org/api/geospatial/5jrd-6zik?method=export&format=Shapefile" -O "ChicagoCensusTract_2010.zip"
    # The download comes as a zip. We need to unzip it.
    unzip "ChicagoCensusTract_2010.zip"
fi

# Now we just extract the year of data out of the file to make the
# overall processing a bit easier for us.
TaxiTripsYear="Taxi_Trips_${year}.txt"

# Perform the data extraction only if the target file doesn't exist
if [ ! -e "${TaxiTripsYear}" ]; then
    echo "Extracting trips for year ${year} into ${TaxiTripsYear}"
    head -1 "${FullTaxiRideFile}" > "${TaxiTripsYear}"
    grep "../../${year}" "${FullTaxiRideFile}" >> "${TaxiTripsYear}"
else
    echo "Reusing data in exisiting file ${TaxiTripsYear}"
fi


# Now run the anaysis with the necessary data.
/usr/bin/time ../../../riskanalyzer --model ../chicago_model.txt --best-time --shape "${CensusTractDir}/geo_export_*.shp --dbf "${CensusTractDir}/geo_export_*.dbf" --taxi-rides "${TaxiTripsYear}" --batch-size 8000 --start-date "01/01/${Year}" --end-date "12/31/${year}" --node-summary "nodes_summary_${year}.tsv" --way-summary "ways_summary_${year}.tsv" 2> "error_logs_${year}.txt"

# End of script

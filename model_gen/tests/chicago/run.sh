#!/bin/bash

# export OMP_NUM_THREADS=6
# /usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --pop-gis lspop2011/w001001.adf --osm-xml Chicago.osm --adjust-model chicago_adjustments.txt --xfig buildings_424.fig --pop-ring 424 --scale 16384000 > /dev/null

# /usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --pop-gis lspop2011/w001001.adf --osm-xml Chicago.osm --adjust-model chicago_adjustments.txt --xfig buildings_172.fig --pop-ring 172 --scale 16384000 --out-model chicago_model.txt > bld_info.txt

# /usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --pop-gis lspop2011/w001001.adf --osm-xml Chicago.osm --adjust-model chicago_adjustments.txt --xfig communities.fig --scale 1638400 --out-model chicago_model.txt > bld_info.txt

# ---------------------------------------------------------------------
# Added this entry on Dec 15 to test PUMS/PUMA data

# First convert the shapefile for PUMA to WG84 projection so that the 
# latitude-longitude values are consistent
# ogr2ogr -t_srs "WGS84" pums/ipums_puma_2010/ipums_puma_2010_WGS84.shp pums/ipums_puma_2010/ipums_puma_2010.shp

# Now run the model generator to generate model for Chicao using PUMS/PUMA
# /usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --pop-gis lspop2011/w001001.adf --osm-xml Chicago.osm --adjust-model chicago_adjustments.txt --xfig chicago.fig --scale 16384000 --out-model chicago_model.txt --pums-h pums/illinois/psam_h17.csv --pums-p pums/illinois/psam_p17.csv --puma-shp pums/ipums_puma_2010/ipums_puma_2010_WGS84.shp --puma-dbf pums/ipums_puma_2010/ipums_puma_2010.dbf --pums-h-cols SERIALNO SMARTPHONE --puma-ring 3525 > bld_info.txt

/usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --pop-gis lspop2011/w001001.adf --osm-xml Chicago_2023.osm --adjust-model chicago_adjustments.txt --xfig chicago.fig --scale 16384000 --out-model chicago_model.txt --pums-h pums/illinois_2021_5years/psam_h17.csv --pums-p pums/illinois_2021_5years/psam_p17.csv --puma-shp pums/ipums_puma_2010/ipums_puma_2010_WGS84.shp --puma-dbf pums/ipums_puma_2010/ipums_puma_2010_WGS84.dbf --pums-h-cols SERIALNO > bld_info_2022.txt

#------ On Wed Jan 15 2025, got rid of landscan and corrections but without landscan the runs were taking a looooong time --------

# /usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --osm-xml Chicago_2023.osm --xfig chicago.fig --scale 16384000 --out-model chicago_model.txt --pums-h pums/illinois_2021_5years/psam_h17.csv --pums-p pums/illinois_2021_5years/psam_p17.csv --puma-shp pums/ipums_puma_2010/ipums_puma_2010_WGS84.shp --puma-dbf pums/ipums_puma_2010/ipums_puma_2010_WGS84.dbf --pums-h-cols SERIALNO > bld_info_2022.txt

# End of script

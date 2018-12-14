#!/bin/bash

export OMP_NUM_THREADS=6
/usr/bin/time ../../modelgen --shape boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp --dbf boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf --pop-gis lspop2011/w001001.adf --osm-xml Chicago.osm --bld-ignore ignore_buildings.txt > bld_info.txt

# End of script

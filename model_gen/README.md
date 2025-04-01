# CityScape Model Generator

## Compiling

The source code uses GNU autoconf
(https://www.gnu.org/software/autoconf/) to detect the GNU/Linux
distribution settings and generate a Makefile.  Accordingly, the
source code can be built via the following procedure:

* $ cd model_gen
* $ autoreconf -i -v
* $ ./configure
* $ make

## Test data
### General info
1. Location: All test files are located in `tests/[City Name]`
2. File Sources:
    * PUMS data: https://www.census.gov/programs-surveys/acs/microdata/access.html
    * PUMA shapefiles: https://www.census.gov/cgi-bin/geo/shapefiles/index.php
        * PUMA shapefiles are organized by state, so when testing, please make sure to have downloaded the PUMA shapefile for the state of the city in question before generating the model

    > Note: For best compatibility between PUMS data and PUMA shapefiles, make sure the two pieces of data are from the same year (e.g. if the PUMS data is 2022 5 year, make sure to download the corresponding PUMA shapfile for year 2022)

    * OSM files: https://download.bbbike.org/osm/bbbike/
    * Boundary files for different cities:
        1. LA (Los Angeles): https://koordinates.com/layer/98158-los-angeles-city-boundary/
            * Remember to choose `WGS 84 (ESPG:4326)` as the coordinate reference system

### About PUMA Shapefiles
The generator makes use of both `.shp` and `.adf` files from the PUMA shapefile folder to generate the model.

The `.shp` file can be viewed with _QGIS_, and the `.adf` file can be viewed with _Microsoft Excel_.

The `.adf` file contains information for each region (Ring) in the `.shp` file.

For more information regarding the columns contained in the `.adf` file, please [visit this page on Census.gov]( https://www.census.gov/programs-surveys/geography/technical-documentation/complete-technical-documentation/tiger-geo-line.2024.html#list-tab-240499709), select the corresponding year and then go to `Full document` and use your browser's search feature to search the column for which you are looking for information. Likely, you will be directed to an appendix that contains an explanation for every single column in the `.adf` file.


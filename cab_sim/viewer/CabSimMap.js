/**
 *---------------------------------------------------------------------------
 *
 * Copyright (c) PC2Lab Development Team
 * All rights reserved.
 *
 * This file is part of free(dom) software -- you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (GPL)as published by the Free Software Foundation, either
 * version 3 (GPL v3), or (at your option) a later version.
 *
 * The software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Miami University and PC2Lab makes no representations or warranties
 * about the suitability of the software, either express or implied,
 * including but not limited to the implied warranties of
 * merchantability, fitness for a particular purpose, or
 * non-infringement.  Miami University and PC2Lab is not be liable for
 * any damages suffered by licensee as a result of using, result of
 * using, modifying or distributing this software or its derivatives.
 *
 *
 * By using or copying this Software, Licensee agrees to abide by the
 * intellectual property laws, and all other applicable laws of the
 * U.S., and the terms of this license.
 *
 * Authors: Dhananjai M. Rao       raodm@miamiOH.edu
 *
 *---------------------------------------------------------------------------
 */

/**
 * Custom Map to display a snapshot (consisting of a number of taxis)
 * using OpenStreetMap/OpenLayers API.
 *
 * Copyright Miami University 2018
 *
 * NOTE: Prior to using this JavaScript file, you need to include the
 * following supporting libraries in <head/> section of your HTML:
 *
 * <link rel="stylesheet" href="libs/ol.css" type="text/css">
 * <script src="libs/ol.js"
 * <script src="libs/FileSaver.min.js"></script>
 */

/**
 * @constructor
 * @extends {ol.Map}
 *
 * @param {String} targetDiv The target DIV layer into which the map
 * is to be drawn
 *
 * @param {bool} enable_resize Enable veritical resizing of map. This
 * feature requires jquery-ui (both JS and CSS).
 *
 */
const CabSimMap = function(targetDiv, enable_resize) {
    // First initialize the base class -- The settings are all default
    // information from OpenLayers examples.
    ol.Map.call(this, {
        target: targetDiv,  // The DIV defined in HTML
        controls: [
            new ol.control.Attribution({collapsible: true, collapsed: true}),
            new ol.control.Zoom({zoomOutLabel: '\u2013'}),
            new ol.control.ScaleLine({units: 'us'}),
            new ol.control.ScaleLine({units: 'metric',
                                      className: 'scale-line2 ol-scale-line'}),
            new ol.control.OverviewMap({
                className: 'ol-overviewmap my-map-overview',
                collapseLabel: '\xab',
                label: '\xbb'
            }),
            new ol.control.MousePosition(
                {className: 'my-mouse-pos ol-scale-line-inner',
                 coordinateFormat: ol.coordinate.createStringXY(4),
                 projection: 'EPSG:4326'})
        ],
        layers: [
            new ol.layer.Tile({
                source: new ol.source.OSM(),
                zIndex: 0
            }),
        ],
        view: new ol.View({
            // Set center to chicago
            center : ol.proj.fromLonLat([-87.7, 41.88781]),
            zoom   : 12,
            maxZoom: 20,
            minZoom: 3
        })
    });

    // Create and add a new taxi layer
    this.taxiLayer  = new TaxiLayer(0, "Taxis", true);
    this.addLayer(this.taxiLayer);

    // The current time we are working at.
    this.simTime    = 0;
    this.maxSimTime = 0;

    // The index and log file that we are working with (if any)
    this.indexFile  = null;
    this.logFile    = null;

    // Setup the index array -- to be processed as pairs of
    // values. Number (double) at even position is simulation time
    // (double).  Number (long) at odd index position is starting
    // offset of block of data for given time.
    this.logIndex   = new Array();
    
    // Make the map vertically resizable if the user requested this.
    if (enable_resize == true) {
        $('#' + targetDiv).resizable({
            handles: 's',
            ghost: true,
            classes: { "ui-resizable-s" : "ui-icon ui-icon-grip-dotted-horizontal grip-centered" },
            stop: function(event, ui) {
                $(this).css("width", '');
                map.updateSize();
            }
        });
    }
    
    // Add a dummy taxi for testing.
    this.taxiLayer.addTaxi(new Taxi(0, -87.7303, 41.8937, TaxiStatus.FREE));
}

// Setup the inheritance relationship using helper from OL
ol.inherits(CabSimMap, ol.Map);

/**
 * Format mouse position to latitude, longitude value for display on
 * the map.  This method is automatically called by OpenLayers to
 * obtain the formatted information to be displayed.  This information
 * is displayed in the top-left corner of the map.
 * 
 * @protected
 */
CabSimMap.prototype.toStringLatLon = function(coordinate) {
    return ol.coordinate.format(coordinate, '{y}, {x}', 4);
}

/** Helper method to save the current map view as an image. This
 * method has been implemented based on the example from OpenLayers
 * at: http://openlayers.org/en/latest/examples/export-map.html
 * 
 * This method is convenient to make many snapshots at different times
 * and then stich them together to make a video of the simulation
 * progress. However, the browser(s) show images being downloaded and
 * that is a bit interfering at this time.
 *
 * @param {String} fileName The name of the PNG file into which the
 * current snapshot should be saved.
 *
 * @public
 */
CabSimMap.prototype.saveImage = function(fileName) {
    this.once('rendercomplete', function(event) {
        var canvas = event.context.canvas;
        if (navigator.msSaveBlob) {
            navigator.msSaveBlob(canvas.msToBlob(), fileName);
        } else {
            canvas.toBlob(function(blob) {
                saveAs(blob, fileName);
            });
        }
    });
    this.renderSync();
}


CabSimMap.prototype.processLogs = function(indexEntry, lines) {
    console.log(indexEntry);
    console.log(lines);
    
}

/**
 * This method is invoked whenever a block of log entries is to be
 * loaded and processed.  This method only initiates loading the log
 * entries.  It arranges to have the processLogs method in this class
 * to be called.
 *
 * @param {int} indexEntry The entry in the CabSimMap.logIndex array
 * to be processed.  This value should always been an even number.
 *
 * @private
 */
CabSimMap.prototype.loadLogBlock = function(indexEntry) {
    // Ensure that the index entry is even
    if (indexEntry % 2 != 0) {
        console.log("loadLogBlock got an invalid indexEntry");
        return;
    }
    // Start loading a block of log data, if indexEntry is within
    // bounds.
    if (indexEntry < this.logIndex.length - 2) {
        // Create a slice of the log file to be loaded
        const logSlice = this.logFile.slice(this.logIndex[indexEntry + 1],
                                            this.logIndex[indexEntry + 3]);
        // Convenience object to assess in anonymous function(s) further below
        const cabSimMap     = this;    
        // Create a reader to load the chunk of log data        
        var logReader = new FileReader();
        logReader.onload = function(event) {
            cabSimMap.processLogs(indexEntry, event.target.result.split("\n"));
        }
        // Start the log loading.
        logReader.readAsText(logSlice);
    }
}

/**
 * This method is invoked once the log-index has been loaded.  This
 * method is called because the processLogAndIndex method sets up to
 * have this method invoked.  This method converts the log-index into
 * an array of numbers. It then starts loading the first block of log
 * data from the log-file.
 *
 * @param {Array} indexStrArray An array of words read from the
 * log-index. The words are processed in pairs, the first one being
 * simulation time and the second one is the offset into the log-file
 * from where the log entries for the given simulation-time is loaded.
 *
 * @private 
 */
CabSimMap.prototype.processIndex = function(indexStrArray) {
    // The last entry in indexStrArray is typically an empty string
    // and should be removed.
    const lastStr = indexStrArray.pop();
    if (lastStr.length != 0) {
        console.log("Last index entry was not empty as expected.");
    }
    // Now convert the array of string to array of numbers for
    // convenient processing
    this.logIndex = new Array();
    for (var i = 0; (i < indexStrArray.length); i++) {
        this.logIndex.push(+indexStrArray[i]);
    }
    // Now start loading the first entry in the index
    this.loadLogBlock(this.logIndex[0]);
}

/** Method to trigger processing of simulation log and index files.
 * This method is invoked from CabSimViewer.html whenever the user
 * selects 2 files to be processed.  This method assumes that the file
 * chosen are the simulation logs and the associated index files. No
 * special checks are made to verify if the user has choose the files
 * correctly.  This method saves the files in instance variables and
 * triggers the loading of the index data. Once the index is loaded,
 * the processIndex() method will be invoked to process the data
 * further.
 *
 * The file whose name ends with '_index' is assumed to be the
 * log-index and the other file is assumed to be log.
 *
 * \param {Array} fileArray The array of files selected by the user.  This
 * array is validated before it is used.
 *
 * @public
 */
CabSimMap.prototype.processLogAndIndex = function(fileArray) {
    // If the user chooses 2 files then process it.
    if (fileArray.length != 2) {
        alert("Please select simulation logs & index files");
        return;
    }
    // Ensure at least one of the files has a '_index' suffix
    if (!fileArray[0].name.endsWith('_index') &&
        !fileArray[1].name.endsWith('_index')) {
        alert("Neither file has '_index' ending." +
              "Please select simulation logs & index files");
        return;
    }
    
    // First we need to figure out which one is the index based on the name
    if (fileArray[0].name.endsWith("_index")) {
        this.indexFile = fileArray[0];
        this.logFile   = fileArray[1];
    } else {
        this.indexFile = fileArray[1];
        this.logFile   = fileArray[0];
    }

    // Convenience object to assess in anonymous function(s) further below
    const cabSimMap     = this;    
    // First load the whole index file into memory so we know the
    // blocks of text data to be processed.
    const indexFileReader = new FileReader();
    indexFileReader.onload = function(event) {
        cabSimMap.processIndex(event.target.result.split(/\s+/));
    }
    // Have the reader load the index into memory
    indexFileReader.readAsText(this.indexFile);
}

// End of source code

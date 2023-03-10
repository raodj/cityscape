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
 * Customization of OpenLayers-Layer to manage and color taxis. One
 * TaxiLayer is created and used by CabSimMap.
 *
 * NOTE: Prior to using this JavaScript file, you need to include the
 * following supporting libraries in <head/> section of your HTML:
 *
 * <link rel="stylesheet" href="libs/ol.css" type="text/css">
 * <script src="libs/ol.js"
 * <script src="libs/FileSaver.min.js"></script>
 * 
 */

/**
 * @classdef
 *
 * Constructor to create an TaxiLayer.
 *
 * A taxi layer layer is added to a CabViewMap. The taxi layer
 * contains a contains a set of entities added to it. Each entity
 * represents a taxi cab's location and status at a given time.  The
 * entities are drawn as fixed-size circles
 *
 * @param {int} layerID A unique ID associated with this layer. This
 * value is useful to quickly look-up layers, in case multiple layers
 * are used.
 *
 * @param {String} name A human-readable name associated with this
 * layer.
 
 * @param {bool} stroke If this flag is true, then a stroke is added
 * to the entities in this layer.
 *
 * @constructor
 * @extends {ol.layer.Vector}
 */
const TaxiLayer = function(layerID, name, stroke) {
    // First initialize the base class to create a new vector
    // layer. The z-index is set to layerID.  This value is also used
    // to look-up layers, given the layerID.
    ol.layer.Vector.call(this, {
	source: new ol.source.Vector({wrapX: false}),
        zIndex: layerID,  // Also used to find layers.
        opacity: 0.5      // Translucent
    });
    // Save the id and name for future reference
    this.id     = layerID;
    this.name   = name;
    // Setup a sstroke style if requested
    this.stroke = null;
    if (stroke == true) {
        this.stroke = new ol.style.Stroke({
	    color : 'rgba(0, 0, 0)',  // color
	    width : 1    
        });	
    }
    // The list of taxi cabs added to this layer
    this.taxiList = [];
    // The fixed-size circle being used to display each taxi
    this.featureList = [];
    // Track the current wave of color changes. This is used to cancel
    // out any ongoing color changes if multiple color changes are
    // triggered successively, with the latest one given preference.
    this.currColorWave   = 0;
}

// Setup the inheritance relationship using helper from OL
ol.inherits(TaxiLayer, ol.layer.Vector);

/**
 * Convenince method to get a facier icon for this layer based on its
 * name.  The icon is used to create the control associated with this
 * layer.
 *
 * @return {String} An HTML fragment with the icon to be used for this
 * layer.
 *
 * @public
 */
TaxiLayer.prototype.getIcon = function() {
    if (this.name == "Taxis") {
        // Hex excape for a car
        return '&#x1f697;';
    }
    // By default use 1st character as icon
    return this.name[0];
}

/** Add an taxi to this Layer.
 *
 * @param {Taxi} taxi The data associated with the taxi to be added to
 * this layer.
 *
 * @public
 */
TaxiLayer.prototype.addTaxi = function(taxi) {
    // Add taxi to our local list for faster look-ups in future
    this.taxiList.push(taxi);
    // Translate latitude and longitude to OpenLayers coordinates
    const center = ol.proj.transform([taxi.latitude, taxi.longitude],
                                     'EPSG:4326', 'EPSG:3857');
    // Add corresponding feature to our local list for faster look-ups
    const feature = this.getFeature(taxi);
    this.featureList.push(feature);
    // Finally add the entity to the layer.
    this.getSource().addFeature(feature);
}

/** Internal helper method to return a feature for the given taxi.
 *
 * @param {Taxi} taxi The taxi object for which a feature is to be
 * returned.
 *
 * @return {ol.Feature} An open layers feature object containing the
 * features for rendering the taxi cab.
 *
 * @private
 */

TaxiLayer.prototype.getFeature = function(taxi) {
    // Translate latitude and longitude to OpenLayers coordinates
    const center = ol.proj.transform([taxi.latitude, taxi.longitude],
                                     'EPSG:4326', 'EPSG:3857');
    // Create a fixed size circle feature for rendering the taxi cab
    const marker =  new ol.style.Circle({
        radius: 7,
        fill  : new ol.style.Fill({ color: taxi.getColor() }),
        stroke: this.stroke
    });
    // Create a feature in the form of a circle corresponding to this
    // entity.
    const feature = new ol.Feature({
        geometry: new ol.geom.Point(center),
        id      : taxi.id,
    });
    // Set the style for this feature.
    feature.setStyle(new ol.style.Style({ image: marker }));
    // Return the newly created feature
    return feature;
}

/** Get the taxi object for a given index.
 *
 * @param {int} index The zero-based index of the taxi.
 *
 * @return {Taxi} If the index is valid, then this method returns
 * the requested taxi object.  Otherwise it returns null.
 *
 * @public
 */
TaxiLayer.prototype.taxiUpdated = function(index) {
    if ((index < 0) || (index >= this.taxiList.length)) {
        return null;  // invalid index.
    }
    // Return the requested taxi object
    return this.taxiList[index];
}

/** Let the layer know that a taxi object's information was changed
 * and hence it it needs to be redrawn.
 *
 * @param {int} index The zero-based index of the taxi whose
 * information has been updated and its display is to be updated.
 *
 * @return void
 *
 * @public
 */
TaxiLayer.prototype.taxiUpdated = function(index) {
    if ((index < 0) || (index >= this.taxiList.length)) {
        return;  // invalid index.
    }
    // Get the updated feature for this taxi
    const updtFeature = this.getFeature(this.taxiList[index]);
    // Copy the feature information to our existing feature object
    this.featureList[index].setGeometry(uptFeature.getGeometry());
    this.featureList[index].setStyle(uptFeature.getStyle());
}

// End of source code

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
 * A simple Javascript class to encapsulate information associated
 * with a taxi cab.
 */

/** A set of predefined taxi cab status values to improve readability
 * of code.  These values should be consistent with the status codes
 * used in the simulation.
 */
const TaxiStatus = {
    UNKNONW: 0,
    FREE: 1,
    PICKING_UP: 2,
    BUSY: 3
};

/**
 * @classdef
 *
 * Constructor to create a Taxi object.
 *
 * A taxi is typically added to a TaxiLayer.  Default set of
 * information about the taxi is supplied as parameters to the
 * constructor.
 *
 * @param {int} id A unique ID associated with this taxi. This value
 * is useful to quickly look-up taxis for updating their information.
 *
 * @param {double} latitude The current latitude for the taxi.
 *
 * @param {double} longitude The current longitude for the taxi.
 *
 * @param {int} status A status code associated with the taxi.
 *
 * @constructor
 */
const Taxi = function(id, latitude, longitude, status) {
    // Save all the parmaeters for future reference.
    this.id        = id;
    this.latitude  = latitude;
    this.longitude = longitude;
    this.status    = status;
}

/**
 * Convenince method to get the color for this taxi based on its
 * current status.  This method just returns a suitable HTML color
 * code in the format 'rgb(red, green, blue)' values.
 *
 * @return {String} An HTML code code value to be used to draw this
 * taxi.
 *
 * @public
 */
Taxi.prototype.getColor = function() {
    switch (this.status) {
    default:
    case TaxiStatus.FREE: return 'rgb(0, 150, 0)';
    case TaxiStatus.UNKNOWN: return 'rgb(150, 150, 150)';
    }
}

/**
 * Convenince method to update the key information associated with
 * this taxi.  
 *
 * @param {double} latitude The current latitude for the taxi.
 *
 * @param {double} longitude The current longitude for the taxi.
 *
 * @param {int} status A status code associated with the taxi.
 *
 * @public
 */
Taxi.prototype.update = function(latitude, longitude, status) {
    switch (this.status) {
    default:
    case TaxiStatus.UNKNOWN: return 'rgb(150, 150, 150)';
    }
}

// End of source code

#ifndef XFIG_HELPER_CPP
#define XFIG_HELPER_CPP

//---------------------------------------------------------------------------
//
// Copyright (c) PC2Lab Development Team
// All rights reserved.
//
// This file is part of free(dom) software -- you can redistribute it
// and/or modify it under the terms of the GNU General Public
// License (GPL)as published by the Free Software Foundation, either
// version 3 (GPL v3), or (at your option) a later version.
//
// The software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Miami University and PC2Lab makes no representations or warranties
// about the suitability of the software, either express or implied,
// including but not limited to the implied warranties of
// merchantability, fitness for a particular purpose, or
// non-infringement.  Miami University and PC2Lab is not be liable for
// any damages suffered by licensee as a result of using, result of
// using, modifying or distributing this software or its derivatives.
//
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
//
// Authors: Dhananjai M. Rao       raodm@miamiOH.edu
//
//---------------------------------------------------------------------------

#include "XFigHelper.h"
#include "Utilities.h"

XFigHelper::XFigHelper() {
    vertexCount = 0;
}

XFigHelper::~XFigHelper() {
    // Nothing else to be done.
}

bool
XFigHelper::setOutput(const std::string &fileName,
                      const bool genCustomColors,
                      const bool appendFlag) {
    os.open(fileName.c_str(), (appendFlag ? std::ios_base::app :
                               std::ios_base::out));
    if (!os.good()) {
        return false;
    }
    // Dump the XFIG header if needed
    if (!appendFlag) {
        dumpHeader(genCustomColors);
    }
    // Everything went well.
    return true;
}


void
XFigHelper::dumpHeader(const bool genCustomColors) {
    os << "#FIG 3.2  Produced by SEARUMS Tools 0.1\n"
       << "Landscape\n"
       << "Center\n"
       << "Inches\n"
       << "Letter\n"
       << "100.00\n"
       << "Single\n"
       << "-2\n"
       << "1200 2\n"
       << "0 32 #cccccc\n";

    if (genCustomColors) {
        generateColorTable();
    }
}

void
XFigHelper::drawLine(const int x1, const int y1,
                     const int x2, const int y2,
                     const int colorCode, const int level) {
    os  << "2 1 0 1 " << colorCode
        << " 7 " << level << " -1 -1 0.000 0 0 -1 0 0 2\n"
        << "\t  " << x1 << " " << y1
        << " "    << x2  << " " << y2
        << std::endl; 
}

int
XFigHelper::drawText(const std::string& text, const int x, const int y,
                     const int justify, const int fontCode,
                     const int fontSize, const int colorCode, const int level) {
    const int FontHeight = 1200 * fontSize / 72;
    const int BaseLine   = (int) (FontHeight * 0.75);
    
    os << "4 " << justify << " " << colorCode << " " << level << " -1 "
       << fontCode << " "    << fontSize  << " 0.0000 4 0 0 "
       << x << " " << y + BaseLine << " " << text << "\\001\n";
    // Return font height.
    return FontHeight;
}

void
XFigHelper::drawRect(int x, int y, int width, int height, int colorCode,
		     int level, int thickness, int fillStyle) {
    os << "2 2 0 " << thickness << " " << colorCode << " " << colorCode
       << " " << level << " -1 " << fillStyle << " 0.000 0 0 -1 0 0 5\n"
       << "\t" << x           << " " << y
       << " "  << (x + width) << " " << y
       << " "  << (x + width) << " " << (y + height)
       << " "  << x           << " " << (y + height)
       << "\t" << x           << " " << y
       << std::endl;
}

void
XFigHelper::generateColorTable() {
    // Dump 100 custom color codes for further use.
    int colorCode = 32;
    struct Color { int red; int green; int blue; };
    Color lowColor = { 10, 10, 225 }, midColor = { 225, 225, 10 },
          hiColor  = { 225, 10, 10 };
    for (int percent = 0; (percent < 101); percent++) {
        Color clr1 = lowColor, clr2 = midColor;
        int perc = percent;
        if (percent > 50) {
            perc -= 50;
            clr1 = midColor;
            clr2 = hiColor;
        }
        perc *= 2;
        int red   = clr1.red   + (clr2.red   - clr1.red)   * perc / 100.0;
        int green = clr1.green + (clr2.green - clr1.green) * perc / 100.0;
        int blue  = clr1.blue  + (clr2.blue  - clr1.blue)  * perc / 100.0;
        char colorString[16];
        sprintf(colorString, "#%02x%02x%02x", red, green, blue);
        os << "0 " << colorCode << " "
           << colorString << "\n";
        colorCode++;
    }    
}

void
XFigHelper::startPolygon(const int vxCount, const int lineColor,
                         const int fillColor, const int level,
                         const int fillStyle) {
    ASSERT(vertexCount == 0);
    vertexCount = vxCount;
    os << "2 3 0 1 " << lineColor << " " << fillColor
       << " " << level << " -1 " << fillStyle << " 0.000 0 0 -1 0 0 "
       << vertexCount << std::endl;
}

void
XFigHelper::startPolyLine(const int vxCount, const int lineColor,
                          const int level) {
    ASSERT(vertexCount == 0);
    vertexCount = vxCount;
    os << "2 1 0 1 " << lineColor << " 7 " << level
       << " -1 -1 0.000 0 0 -1 0 0 "
       << vertexCount << std::endl;
}

void
XFigHelper::addVertex(const int x, const int y) {
    ASSERT(vertexCount > 0);
    os << "\t " << x << " " << y << std::endl;
    vertexCount--;
}

void
XFigHelper::endPolygon() {
    ASSERT(vertexCount == 0);
}

void
XFigHelper::endPolyLine() {
    ASSERT(vertexCount == 0);
}

void
XFigHelper::addImage(const std::string& fileName, const int x, const int y,
                     const int width, const int height, const int layer) {
    os << "2 5 0 1 0 -1 " << layer
       << " -1 -1 0.000 0 0 -1 0 0 5\n"
       << "\t 0 " << fileName << "\n"
       << "\t" << x           << " " << y
       << " "  << (x + width) << " " << y
       << " "  << (x + width) << " " << (y + height)
       << " "  << x           << " " << (y + height)
       << " "  << x           << " " << y
       << std::endl;
}

void
XFigHelper::drawOval(const int x, const int y,
                     const int xRad, const int yRad,
                     const int lineColor, const int fillColor,
                     const int level, const int fillStyle) {
    os << "1 1 0 1 " << lineColor << " " << fillColor
       << " "    << level << " -1 " << fillStyle << " 0.000 1 0.0000 "
       << x      << " " << y    << " "   // center
       << xRad   << " " << yRad << " "
       << x      << " " << y    << " "   // anchor #1
       << x+xRad << " " << y    << "\n"; // anchor #2
}

void
XFigHelper::addComment(const std::string& comment) {
    os << "# " << comment << std::endl;
}

void
XFigHelper::drawScaleBar(const int x, const int y,
                         const int width, const int height, 
                         std::vector<double> tics, const int layer) {
    const int diffX = width / 100;
    size_t currTic = 0;
    for (int currX = x, i = 0; (i < 101); i++, currX += diffX) {
        drawRect(currX, y, diffX, height, 32 + i, layer, 0);
        if ((currTic < tics.size()) && (tics[currTic] == i)) {
            // Tick at this percentage.
            const int value = tics[currTic + 1];
            int ticX = currX + (i == 100 ? diffX : (i != 0 ? (diffX / 2) : 0));
            drawLine(ticX, y, ticX, y + height + XFIG_SCALE * 3,
                     BLACK, layer);
            drawText(std::to_string(value), currX, y + height + XFIG_SCALE * 4,
                     CENTER_JUSTIFIED, 14, 12, BLACK, layer - 2);
            currTic += 2;
        }
    }
    // Finally draw bounding rectangle to make it look nice
    drawRect(x, y, width + diffX, height, BLACK, layer - 2, 1, NO_FILL);
}

#endif

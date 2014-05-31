#ifndef XFIG_HELPER_CPP
#define XFIG_HELPER_CPP

//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
// 
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
// 
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include "XFigHelper.h"
#include "../Assert.h"

XFigHelper::XFigHelper() {
    vertexCount = 0;
}

XFigHelper::~XFigHelper() {
    // Nothing else to be done.
}

bool
XFigHelper::setOutput(const std::string &fileName,
                      const int width,
                      const int height,
                      const bool genCustomColors,
                      const bool appendFlag) {
    os.open(fileName.c_str(), (appendFlag ? std::ios_base::app :
                               std::ios_base::out));
    if (!os.good()) {
        return false;
    }
    // Dump the XFIG header if needed
    if (!appendFlag) {
        dumpHeader(genCustomColors, width, height);
    }
    // Everything went well.
    return true;
}


void
XFigHelper::dumpHeader(const bool genCustomColors, const int width, const int height) {
    os << "#FIG 3.2  Produced by HAPLOS from http://pc2lab.cec.miamiOH.edu\n"
       << "Landscape\n"
       << "Center\n"
       << "Inches\n"
       << "Letter\n"
       << "100.00\n"
       << "Single\n"
       << "-2\n"
       << width <<" "<<height<<"\n"
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
XFigHelper::addColor(int colorCode, int red, int green, int blue) {
    char colorString[16];
    sprintf(colorString, "#%02x%02x%02x", red, green, blue);
    os << "0 " << colorCode << " " << colorString << "\n";
}

void
XFigHelper::drawRect(int x, int y, int width, int height, int colorCode,
		     int level, int thickness) {
    os << "2 2 0 " << thickness << " " << colorCode << " " << colorCode
       << " " << level << " -1 20 0.000 0 0 -1 0 0 5\n"
       << "\t" << x           << " " << y
       << " "  << (x + width) << " " << y
       << " "  << (x + width) << " " << (y + height)
       << " "  << x           << " " << (y + height)
       << "\t" << x           << " " << y
       << std::endl;
}

void
XFigHelper::generateColorTable() {
    // Dump custom color codes for further use.
    int colorCode = 32;
    for(int red = 0; (red < 256); red += 85) {
        for(int blue = 0; (blue < 256); blue += 85) {
            for(int green = 0; (green < 256); green += 85) {
                char colorString[16];
                sprintf(colorString, "#%02x%02x%02x", red, green, blue);
                os << "0 " << colorCode << " "
                   << colorString << "\n";
                colorCode++;
            }
        }
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

#endif



#ifndef POPULATION_H_
#define POPULATION_H_
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
#include "Person.h"
#include <vector>

class Population {
    /** A class specifically repersenting a population.
     */
    public:
        /** The default constructor for this class.
         
         \param[in] size Size of population to create.
         \param[in] age2024Prob Probablity of someone being age 20-24
         \param[in] age2534Prob Probablity of someone being age 25-34
         \param[in] age3549Prob Probablity of someone being age 35-49
         \param[in] age5064Prob Probablity of someone being age 50-64
         \param[in] age65OlderProb Probablity of someone being age 65 or older
         \param[in] maleProb Probablity of a someone being a male.
         */
    Population(int size, double age2024Prob, double age2534Prob, double age3549Prob, double age5064Prob,double age65OlderProb, double maleProb);
    
        /** Display Statistics about population to Console.
         */
        void displayStatistics();
    
        /** Set the location of a person to coordinate [x, y].
         \param[in] x Row of location.
         \param[in] y Col of location.
         \param[in] person Person to assign location to.
        */
        void setLocationOfPerson(int x, int y, int person);
    
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Population();
    private:
        /** Set gender for population.
         */
        char determineGender();
    
        /** Set age for population.
         */
        int determineAge();
    
        int size;
        double age2024Prob;
        double age2534Prob;
        double age3549Prob;
        double age5064Prob;
        double age65OlderProb;
        double maleProb;

    
        std::vector< Person > population;
};


#endif /* POPULATION_H_ */

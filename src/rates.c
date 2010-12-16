/*
   rates.c - Compute the reaction rates.

   Copyright (c) 2006-2010 Sebastien Maret

   This file is part of Astrochem.

   Astrochem is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   Astrochem is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Astrochem.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "astrochem.h"

#define FRACTION_TIME_GRAIN_70K 3.16e-19
#define GAS_DUST_NUMBER_RATIO 7.57e+11
#define NUMBER_SITE_PER_GRAIN_SURFACE 3.00e+15

double 
rate(double alpha, double beta, double gamm, int reaction_type,
     int reaction_no __attribute__ ((unused)), double av,
     double tgas, double tdust, double chi, double cosmic,
     double grain_size) 
{  
  double k; /* Reaction rate (cm^-3 s^-1) */

  /* Reactions types and rates. The nomencalture is similar to the one
     of the Ohio State University database for astrochemistry, but it
     is extended to include depletion and desorption on/from the grain
     surfaces. */

  /* FixMe: Should we check the values of rates with an exponential? */    
  
  switch (reaction_type)
    {
    case 0:
      /* Gas-grain interaction (excluding depletion and desorption), 
	 Electron-grain recombination. */
      k = alpha * pow (tgas / 300, beta) * GAS_DUST_NUMBER_RATIO;
      break;
      
    case 1:
      /* Cosmic-ray ionization (direct process). Cosmic-ray induced 
	 photoreactions (indirect process)   */
      k = alpha * cosmic;
      break;
      
    case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: 
    case 11: case 12: case 14:
      /* Ion-molecule reactions, charge exchange reactions (2), Negative
	 ion - neutral species reactions (3), Radiative association (4),
	 Associative ejection (5), Neutral + neutral -> ion + electron
	 (6), Neutral-neutral chemical reactions (7), Neutral-neutral
	 radiative association (8), Dissociative recombination (9),
	 Radiative recombination (10), Positive ion - negative ion
	 recombination (11), Electron attachment (12), Others (14) */
      k = alpha * pow (tgas / 300, beta) * exp (-gamm / tgas);
      break;
      
    case 13:
      /* Photo-ionization, Photo-dissociation */
      k = chi * alpha * exp (-gamm * av);
      break;
      
    case 20:
      /* Depletion on the grains */
      {
	double thermal_veloc = pow (8 * CONST_CGSM_BOLTZMANN * tgas
				    / (M_PI * beta * CONST_CGSM_MASS_PROTON),
				    0.5);
	k = M_PI * pow (grain_size, 2) * alpha * thermal_veloc;
	break;
      }
      
    case 21:
      /* Thermal desorption */
      {
	double v0 = pow (2 * NUMBER_SITE_PER_GRAIN_SURFACE * gamm * CONST_CGSM_BOLTZMANN
			 / (M_PI * M_PI * beta * CONST_CGSM_MASS_PROTON),
			 0.5);
	k = v0 * exp (-gamm / tdust );
	k = 0.;
	break;
      }
      
    case 22:
      /* Cosmic ray desorption */
      {
	double v0 = pow (2 * NUMBER_SITE_PER_GRAIN_SURFACE * gamm * CONST_CGSM_BOLTZMANN
			 / (M_PI * M_PI * beta * CONST_CGSM_MASS_PROTON),
			 0.5);
	k = v0 * FRACTION_TIME_GRAIN_70K * exp (-gamm / 70.);
	k = 0.;
	break;
      }
      
    case 23:
      /* Photo-desorption */
      {
	k = chi * exp(-2 * av) * alpha * M_PI * pow (grain_size, 2);
	break;
      }
      
    default:
      fprintf (stderr, "astrochem: %s:%d: %s\n", __FILE__, __LINE__, 
	       "unknown reaction type.\n");
      exit (1);
    }
  return k;  
}

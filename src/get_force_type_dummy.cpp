#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>

void get_force_type_dummy(int type,
		    int    *field) {

  extern param_set_struct param_set;

  /***************************************
    Get meteorological data forcing info
  ***************************************/
  param_set.TYPE[type].SUPPLIED=1;
  param_set.FORCE_INDEX[(*field)] = type;

  (*field)++;
}

///***** Forcing Variable Types *****/
//#define N_FORCING_TYPES 24
//#define AIR_TEMP   0 /* air temperature per time step [C] (ALMA_INPUT: [K]) */
//#define ALBEDO     1 /* surface albedo [fraction] */
//#define CHANNEL_IN 2 /* incoming channel flow [m3] (ALMA_INPUT: [m3/s]) */
//#define CRAINF     3 /* convective rainfall [mm] (ALMA_INPUT: [mm/s]) */
//#define CSNOWF     4 /* convective snowfall [mm] (ALMA_INPUT: [mm/s]) */
//#define DENSITY    5 /* atmospheric density [kg/m3] */
//#define LONGWAVE   6 /* incoming longwave radiation [W/m2] */
//#define LSRAINF    7 /* large-scale rainfall [mm] (ALMA_INPUT: [mm/s]) */
//#define LSSNOWF    8 /* large-scale snowfall [mm] (ALMA_INPUT: [mm/s]) */
//#define PREC       9 /* total precipitation (rain and snow) [mm] (ALMA_INPUT: [mm/s]) */
//#define PRESSURE  10 /* atmospheric pressure [kPa] (ALMA_INPUT: [Pa]) */
//#define QAIR      11 /* specific humidity [kg/kg] */
//#define RAINF     12 /* rainfall (convective and large-scale) [mm] (ALMA_INPUT: [mm/s]) */
//#define REL_HUMID 13 /* relative humidity [fraction] */
//#define SHORTWAVE 14 /* incoming shortwave [W/m2] */
//#define SNOWF     15 /* snowfall (convective and large-scale) [mm] (ALMA_INPUT: [mm/s]) */
//#define TMAX      16 /* maximum daily temperature [C] (ALMA_INPUT: [K]) */
//#define TMIN      17 /* minimum daily temperature [C] (ALMA_INPUT: [K]) */
//#define TSKC      18 /* cloud cover fraction [fraction] */
//#define VP        19 /* vapor pressure [kPa] (ALMA_INPUT: [Pa]) */
//#define WIND      20 /* wind speed [m/s] */
//#define WIND_E    21 /* zonal component of wind speed [m/s] */
//#define WIND_N    22 /* meridional component of wind speed [m/s] */
//#define SKIP      23 /* place holder for unused data columns */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vicNl.h"

void initialize_global() {

  extern option_struct options;
  extern param_set_struct param_set;

  int i, j;

  /** Initialize model option flags **/

  // simulation modes
  options.AboveTreelineVeg      = -1;
  options.AERO_RESIST_CANSNOW   = AR_406_FULL;
  options.BLOWING               = FALSE;
  options.COMPUTE_TREELINE      = FALSE;
  options.CONTINUEONERROR       = TRUE;
  options.CORRPREC              = FALSE;
  options.DIST_PRCP             = FALSE;
  options.EQUAL_AREA            = FALSE;
  options.EXP_TRANS             = FALSE;
  options.FROZEN_SOIL           = FALSE;
  options.FULL_ENERGY           = FALSE;
  options.GRND_FLUX_TYPE        = GF_410;
  options.IMPLICIT              = FALSE;
  options.LAKES                 = FALSE;
  options.LAKE_PROFILE          = FALSE;
  options.LW_CLOUD              = LW_CLOUD_DEARDORFF;
  options.LW_TYPE               = LW_PRATA;
  options.MIN_WIND_SPEED        = 0.1;
  options.MTCLIM_SWE_CORR       = FALSE;
  options.Nlayer                = 3;
  options.Nnode                 = 3;
  options.NOFLUX                = FALSE;
  options.PLAPSE                = TRUE;
  options.PREC_EXPT             = 0.6;
  options.QUICK_FLUX            = TRUE;
  options.QUICK_SOLVE           = FALSE;
  options.ROOT_ZONES            = MISS;
  options.SNOW_ALBEDO           = USACE;
  options.SNOW_BAND             = 1;
  options.SNOW_DENSITY          = DENS_BRAS;
  options.SNOW_STEP             = 1;
  options.SW_PREC_THRESH        = 0;
  options.TFALLBACK             = TRUE;
  options.VP_INTERP             = TRUE;
  options.VP_ITER               = VP_ITER_ALWAYS;
  // input options
  options.ARC_SOIL              = FALSE;
  options.BASEFLOW              = ARNO;
  options.GRID_DECIMAL          = 2;
  options.JULY_TAVG_SUPPLIED    = FALSE;
  options.ORGANIC_FRACT         = FALSE;
  options.VEGPARAM_LAI          = FALSE;
  options.LAI_SRC               = LAI_FROM_VEGLIB;
  // state options
  options.BINARY_STATE_FILE     = FALSE;
  options.INIT_STATE            = FALSE;
  options.SAVE_STATE            = FALSE;
  // output options
  options.ALMA_OUTPUT           = FALSE;
  options.BINARY_OUTPUT         = FALSE;
  options.COMPRESS              = FALSE;
  options.MOISTFRACT            = FALSE;
  options.Noutfiles             = 2;
  options.PRT_HEADER            = FALSE;
  options.PRT_SNOW_BAND         = FALSE;

  /** Initialize forcing file input controls **/

  for(j=0;j<N_FORCING_TYPES;j++) {
    param_set.TYPE[j].SUPPLIED = FALSE;
    param_set.TYPE[j].SIGNED   = 1;
    param_set.TYPE[j].multiplier = 1;
  }
    param_set.FORCE_DT = MISS;
    param_set.N_TYPES = MISS;
    param_set.FORCE_FORMAT = MISS;
    for(j=0;j<N_FORCING_TYPES;j++) param_set.FORCE_INDEX[j] = MISS;

}

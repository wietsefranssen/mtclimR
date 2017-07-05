#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>

static char vcid[] = "$Id$";

global_param_struct get_global_param_R(Rcpp::List list) {
  extern option_struct options;
  extern param_set_struct param_set;
  extern int NF, NR;

  char ErrStr[MAXSTRING];
  int file_num;
  int field = 0;
  global_param_struct global;

  /** Initialize global parameters (that aren't part of the options struct) **/
  global.dt = MISS;
  global.nrecs = MISS;
  global.startyear = MISS;
  global.startmonth = MISS;
  global.startday = MISS;
  global.starthour = MISS;
  global.endyear = MISS;
  global.endmonth = MISS;
  global.endday = MISS;
  global.resolution = MISS;
  global.MAX_SNOW_TEMP = 0.5;
  global.MIN_RAIN_TEMP = -0.5;
  global.measure_h = 2.0;
  global.wind_h = 10.0;
  global.forceyear = MISS;
  global.forcemonth = 1;
  global.forceday = 1;
  global.forcehour = 0;
  global.forceskip = 0;
  file_num = 0;
  global.skipyear = 0;
  global.stateyear = MISS;
  global.statemonth = MISS;
  global.stateday = MISS;
  global.out_dt = MISS;

  int nForcing;
  int forcId;
  nForcing = (int)list["nForcing"];
  Rcpp::NumericVector forcingIds =list["forcingIds"];
  // for(int i=0;i<nForcing;i++) {
  //   // printf("forcIds: %d\n", (int)forcingIds[i]);
  //   printf("forcIds: %d\n", forcId);
  //   forcId = (int)forcingIds[i];
  //   get_force_type_dummy(forcId, &field);
  // }
  // // printf("nforcIds: %d\n", nForcing);

  for(int i=0;i<nForcing;i++) {
    forcId = (int)forcingIds[i];
    // printf("forcIds: %d\n", forcId);
      get_force_type_dummy(forcId, &field);
  }
  // printf("nforcIds: %d\n", nForcing);

  /** Find parameters **/
  // get_force_type_dummy(PREC, &field);
  // get_force_type_dummy(TMIN, &field);
  // get_force_type_dummy(TMAX, &field);
  // get_force_type_dummy(WIND, &field);
  // get_force_type_dummy(SHORTWAVE, &field);
  // get_force_type_dummy(LONGWAVE, &field);

  options.Nlayer = 3;
  options.Nnode = 3; // NODES

  global.dt = (int)list["dt"];
  options.SNOW_STEP = (int)list["SNOW_STEP"];
  global.startyear = (int)list["startyear"];
  global.startmonth = (int)list["startmonth"];
  global.startday = (int)list["startday"];
  global.starthour = 00;
  global.endyear = (int)list["endyear"];
  global.endmonth = (int)list["endmonth"];
  global.endday = (int)list["endday"];
  options.FULL_ENERGY = TRUE;
  options.FROZEN_SOIL = FALSE;
  options.QUICK_FLUX = TRUE;
  options.NOFLUX = FALSE;
  options.DIST_PRCP = FALSE;
  options.PREC_EXPT = 0.6;
  options.CORRPREC = FALSE;
  options.MIN_WIND_SPEED = 0.1;
  global.MIN_RAIN_TEMP = -0.5;
  global.MAX_SNOW_TEMP = 1.0;

  param_set.N_TYPES = 6;
  param_set.FORCE_FORMAT = ASCII;


  /////////////////
  param_set.FORCE_DT = 24;
  global.forceyear = 1960;
  global.forcemonth = 01;
  global.forceday = 01;
  global.forcehour = 00;
  /////////////////

  global.wind_h = 10;
  global.measure_h = 2.0;

  options.ALMA_INPUT = FALSE;
  options.LAI_SRC = LAI_FROM_VEGPARAM;

  /*************************************
   Define output files
   *************************************/
  global.out_dt; //# Output interval (hours); if 0, OUT_STEP = TIME_STEP
  global.skipyear = 0;
  options.COMPRESS = FALSE;
  options.BINARY_OUTPUT = FALSE;
  options.ALMA_OUTPUT = FALSE;
  options.PRT_HEADER = FALSE;
  options.PRT_SNOW_BAND = FALSE;



  /******************************************
   Check for undefined required parameters
   ******************************************/

  // Validate model time step
  if (global.dt == MISS)
    nrerror((char*) "Model time step has not been defined.  Make sure that the global file defines TIME_STEP.");
  else if (global.dt < 1) {
    sprintf(ErrStr, "The specified model time step (%d) < 1 hour.  Make sure that the global file defines a positive number of hours for TIME_STEP.", global.dt);
    nrerror(ErrStr);
  }

  // Validate the output step
  if (global.out_dt == 0 || global.out_dt == MISS) {
    global.out_dt = global.dt;
  } else if (global.out_dt < global.dt || global.out_dt > 24 || (float) global.out_dt / (float) global.dt != (float) (global.out_dt / global.dt)) {
    nrerror((char*) "Invalid output step specified.  Output step must be an integer multiple of the model time step; >= model time step and <= 24");
  }

  // Validate SNOW_STEP and set NR and NF
  if (global.dt < 24 && global.dt != options.SNOW_STEP)
    nrerror((char*) "If the model step is smaller than daily, the snow model should run\nat the same time step as the rest of the model.");
  if (global.dt % options.SNOW_STEP != 0 || options.SNOW_STEP > global.dt)
    nrerror((char*) "SNOW_STEP should be <= TIME_STEP and divide TIME_STEP evenly ");
  NF = global.dt / options.SNOW_STEP;
  if (NF == 1)
    NR = 0;
  else
    NR = NF;

  return global;

}

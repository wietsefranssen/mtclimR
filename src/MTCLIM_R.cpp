#include "Rcpp.h"        // R memory io
#include "Rdefines.h"        // R memory io
#include "Rmath.h"    // R math functions
#include "vicNl.h"

global_param_struct global_param;
option_struct options;
Error_struct Error;
param_set_struct param_set;
int NR; /* array index for atmos struct that indicates the model step avarage or sum */
int NF; /* array index loop counter limit for atmos struct that indicates the SNOW_STEP values */

#include <Rcpp.h>
using namespace Rcpp;
// [[Rcpp::export]]
List mtclimRun(List forcing_dataR, List settings) {

  extern option_struct options;
  extern global_param_struct global_param;

  // printf("forcing: %f\n", forcing_data[0][0]);
  /** Variable Declarations **/
  int cell_cnt;
  dmy_struct *dmy;
  atmos_data_struct *atmos;
  soil_con_struct soil_con;
  out_data_file_struct *out_data_files;
  out_data_struct *out_data;

  /** Read Model Options **/
  initialize_global();

  /** Read Global Control File **/
  global_param = get_global_param_R(settings);

  // global_param.nrecs = nrecs;
  /** Set up output data structures **/
  out_data = create_output_list();
  parse_output_info_R(&out_data_files, out_data, settings);

  /** Make Date Data Structure **/
  dmy = make_dmy(&global_param);

  /** allocate memory for the atmos_data_struct **/
  alloc_atmos(global_param.nrecs, &atmos);

  /************************************
   Run Model for all Active Grid Cells
   ************************************/
  cell_cnt = 0;

  soil_con.time_zone_lng = -30;
  // soil_con.lng = -39.25;
  // soil_con.lat = -8.25;
  // soil_con.elevation = 434;
  soil_con.lng = (double)settings["lon"];
  soil_con.lat = (double)settings["lat"];
  soil_con.elevation = (double)settings["elevation"];
  // printf("elevation: %f %f %f\n", soil_con.elevation, soil_con.lng, soil_con.lat);
  soil_con.slope = 0;
  soil_con.aspect = 0;
  soil_con.ehoriz = 0;
  soil_con.whoriz = 0;
  soil_con.annual_prec = 1;
  soil_con.cell_area = 8.0218315155553431e-314;

  /*******************************
   read in meteorological data
   *******************************/
  // printf("Read meteorological forcing\n");

  double **forcing_data;
  /** Allocate data arrays for input forcing data **/
  forcing_data = (double **)calloc(N_FORCING_TYPES,sizeof(double*));
  // printf("N_FORCING_TYPES: %d %d %d\n", N_FORCING_TYPES, global_param.nrecs, NF);
  for(int i=0;i<N_FORCING_TYPES;i++) {
    if (param_set.TYPE[i].SUPPLIED) {
      forcing_data[i] = (double *)calloc((global_param.nrecs * NF),
                         sizeof(double));
    }
  }
  int     Ndays;
  Ndays = (global_param.nrecs * global_param.dt) / 24;

  // Retreive forcing data from R list
  for(int i=0;i<N_FORCING_TYPES;i++) {
    if (param_set.TYPE[i].SUPPLIED) {
      NumericVector resid = as<NumericVector>(forcing_dataR[i]);
      for(int t=0;t<(Ndays * NF);t++) {
        forcing_data[i][t] = resid[t];
      }
    }
  }

  /**************************************************
   Initialize Meteological Forcing Values That
   Have not Been Specifically Set
   **************************************************/

  // printf("initialize atmos: start\n");
  initialize_atmos(atmos, dmy, forcing_data,
                   &soil_con, out_data_files, out_data);

  /**************************************************
   Output to R
   **************************************************/

  int                 rec, i, j, v;
  int                 dt_sec;
  double              **out_dataAllRecs;
  out_dataAllRecs = (double **)calloc(out_data_files[0].nvars,sizeof(double*));
  for(int i=0;i<out_data_files[0].nvars;i++) {
    out_dataAllRecs[i] = (double *)calloc((global_param.nrecs),
                          sizeof(double));
  }

  dt_sec = global_param.dt*SECPHOUR;

  for ( rec = 0; rec < global_param.nrecs; rec++ ) {
    for ( j = 0; j < NF; j++ ) {

      out_data[OUT_AIR_TEMP].data[0]  = atmos[rec].air_temp[j];
      out_data[OUT_DENSITY].data[0]   = atmos[rec].density[j];
      out_data[OUT_LONGWAVE].data[0]  = atmos[rec].longwave[j];
      out_data[OUT_PREC].data[0]      = atmos[rec].prec[j];
      out_data[OUT_PRESSURE].data[0]  = atmos[rec].pressure[j]/kPa2Pa;
      out_data[OUT_QAIR].data[0]      = EPS * atmos[rec].vp[j]/atmos[rec].pressure[j];
      out_data[OUT_REL_HUMID].data[0] = 100.*atmos[rec].vp[j]/(atmos[rec].vp[j]+atmos[rec].vpd[j]);
      out_data[OUT_SHORTWAVE].data[0] = atmos[rec].shortwave[j];
      out_data[OUT_TSKC].data[0]      = atmos[rec].tskc[j];
      out_data[OUT_VP].data[0]        = atmos[rec].vp[j]/kPa2Pa;
      out_data[OUT_VPD].data[0]       = atmos[rec].vpd[j]/kPa2Pa;
      out_data[OUT_WIND].data[0]      = atmos[rec].wind[j];
      if (out_data[OUT_AIR_TEMP].data[0] >= global_param.MAX_SNOW_TEMP) {
        out_data[OUT_RAINF].data[0] = out_data[OUT_PREC].data[0];
        out_data[OUT_SNOWF].data[0] = 0;
      }
      else if (out_data[OUT_AIR_TEMP].data[0] <= global_param.MIN_RAIN_TEMP) {
        out_data[OUT_RAINF].data[0] = 0;
        out_data[OUT_SNOWF].data[0] = out_data[OUT_PREC].data[0];
      }
      else {
        out_data[OUT_RAINF].data[0] = ((out_data[OUT_AIR_TEMP].data[0]-global_param.MIN_RAIN_TEMP)/(global_param.MAX_SNOW_TEMP-global_param.MIN_RAIN_TEMP))*out_data[OUT_PREC].data[0];
        out_data[OUT_SNOWF].data[0] = out_data[OUT_PREC].data[0]-out_data[OUT_RAINF].data[0];
      }

      for (v=0; v<N_OUTVAR_TYPES; v++) {
        for (i=0; i<out_data[v].nelem; i++) {
          out_data[v].aggdata[i] = out_data[v].data[i];
        }
      }

      if (options.ALMA_OUTPUT) {
        out_data[OUT_PREC].aggdata[0] /= dt_sec;
        out_data[OUT_RAINF].aggdata[0] /= dt_sec;
        out_data[OUT_SNOWF].aggdata[0] /= dt_sec;
        out_data[OUT_AIR_TEMP].aggdata[0] += KELVIN;
        out_data[OUT_PRESSURE].aggdata[0] *= 1000;
        out_data[OUT_VP].aggdata[0] *= 1000;
        out_data[OUT_VPD].aggdata[0] *= 1000;
      }

    }
    ////////////////
    for (int var_idx = 0; var_idx < out_data_files[0].nvars; var_idx++) {
      // Loop over this variable's elements
      for (int elem_idx = 0; elem_idx < out_data[out_data_files[0].varid[var_idx]].nelem; elem_idx++) {
        out_dataAllRecs[var_idx][rec] = out_data[out_data_files[0].varid[var_idx]].aggdata[elem_idx];
      }
    }
  }

  NumericVector outVectorR(global_param.nrecs * out_data_files[0].nvars);
  i = 0;
  for (int var_idx = 0; var_idx < out_data_files[0].nvars; var_idx++) {
    for ( rec = 0; rec < global_param.nrecs; rec++ ) {
      outVectorR[i] = out_dataAllRecs[var_idx][rec];
      i++;
    }
  }

  // free
  for(int i=0;i<out_data_files[0].nvars;i++) {
    if (out_dataAllRecs[i] != NULL) {
      free(out_dataAllRecs[i]);
    }
  }
  free(out_dataAllRecs);
  free_dmy(&dmy);
  free_atmos(global_param.nrecs, &atmos);
  free_out_data(&out_data);
  free_out_data_files(&out_data_files);

  return List::create(Named("out_data") = outVectorR);
}

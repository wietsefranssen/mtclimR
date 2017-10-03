#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"

int  put_data(dist_prcp_struct  *prcp,
	      atmos_data_struct *atmos,
              soil_con_struct   *soil_con,
              out_data_file_struct   *out_data_files,
              out_data_struct   *out_data,
              save_data_struct  *save_data,
	      dmy_struct        *dmy,
              int                rec)
/**********************************************************************
	put_data.c	Dag Lohmann		January 1996

  This routine converts data units, and stores finalized values
  in an array for later output to the output files.

  **********************************************************************/
{
  extern global_param_struct global_param;
  extern option_struct    options;
  double             dp;
  int                skipyear;
  int                     v;
  int                     i;
  int                     dt_sec;
  int                     out_dt_sec;
  int                     out_step_ratio;
  static int              step_count;

  cell_data_struct     ***cell;
  energy_bal_struct     **energy;

  dp = soil_con->dp;
  skipyear = global_param.skipyear;
  dt_sec = global_param.dt*SECPHOUR;
  out_dt_sec = global_param.out_dt*SECPHOUR;
  out_step_ratio = (int)(out_dt_sec/dt_sec);
  if (rec >= 0) step_count++;



  // Initialize output data to zero
  zero_output_list(out_data);

  // Set output versions of input forcings
  out_data[OUT_AIR_TEMP].data[0]  = atmos->air_temp[NR];
  out_data[OUT_DENSITY].data[0]   = atmos->density[NR];
  out_data[OUT_LONGWAVE].data[0]  = atmos->longwave[NR];
  out_data[OUT_PREC].data[0]      = atmos->out_prec; // mm over grid cell
  out_data[OUT_PRESSURE].data[0]  = atmos->pressure[NR]/kPa2Pa;
  out_data[OUT_QAIR].data[0]      = EPS * atmos->vp[NR]/atmos->pressure[NR];
  out_data[OUT_RAINF].data[0]     = atmos->out_rain; // mm over grid cell
  out_data[OUT_REL_HUMID].data[0] = 100.*atmos->vp[NR]/(atmos->vp[NR]+atmos->vpd[NR]);
  out_data[OUT_SHORTWAVE].data[0] = atmos->shortwave[NR];
  out_data[OUT_SNOWF].data[0]     = atmos->out_snow; // mm over grid cell
  out_data[OUT_VP].data[0]        = atmos->vp[NR]/kPa2Pa;
  out_data[OUT_VPD].data[0]       = atmos->vpd[NR]/kPa2Pa;
  out_data[OUT_WIND].data[0]      = atmos->wind[NR];
 
  cell    = prcp->cell;
  energy  = prcp->energy;

  /********************
    Output procedure
    (only execute when we've completed an output interval)
    ********************/
  if (step_count == out_step_ratio) {

    /***********************************************
      Change of units for ALMA-compliant output
    ***********************************************/
    if (options.ALMA_OUTPUT) {
      out_data[OUT_PREC].aggdata[0] /= out_dt_sec;
      out_data[OUT_RAINF].aggdata[0] /= out_dt_sec;
      out_data[OUT_SNOWF].aggdata[0] /= out_dt_sec;
      out_data[OUT_AIR_TEMP].aggdata[0] += KELVIN;
      out_data[OUT_PRESSURE].aggdata[0] *= 1000;
      out_data[OUT_VP].aggdata[0] *= 1000;
      out_data[OUT_VPD].aggdata[0] *= 1000;
    }

    /*************
      Write Data
    *************/
    if(rec >= skipyear) {
      if (options.BINARY_OUTPUT) {
        for (v=0; v<N_OUTVAR_TYPES; v++) {
          for (i=0; i<out_data[v].nelem; i++) {
            out_data[v].aggdata[i] *= out_data[v].mult;
          }
        }
      }
     // write_data(out_data_files, out_data, dmy, global_param.out_dt);
    }

    // Reset the step count
    step_count = 0;

    // Reset the agg data
    for (v=0; v<N_OUTVAR_TYPES; v++) {
      for (i=0; i<out_data[v].nelem; i++) {
        out_data[v].aggdata[i] = 0;
      }
    }

  } // End of output procedure

  return (0);

}


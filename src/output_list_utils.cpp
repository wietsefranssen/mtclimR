#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>

static char vcid[] = "$Id$";

out_data_struct *create_output_list() {
/*************************************************************
  create_output_list()      Ted Bohn     September 08, 2006

  This routine creates the list of output variables.

  Modifications:
  2006-Sep-14 Implemented ALMA-compliant input and output;
              now more variables are tracked.				TJB
  2006-Sep-18 Implemented aggregation of output variables.		TJB
  2006-Oct-10 Shortened the names of variables whose names were
	      too long; fixed typos in other names; added
	      OUT_IN_LONG.						TJB
  2006-Nov-07 Changed default precision from %.1f to %.4f.		TJB
  2006-Nov-07 Added OUT_SOIL_TNODE.					TJB
  2006-Nov-30 Added OUT_DELSURFSTOR.					TJB
  2007-Feb-28 Corrected AGG_TYPE definitions for miscellaneous
	      output variables; re-organized the code to make
	      it easier to debug.					TJB
  2007-Aug-17 Added EXCESS_ICE variables to output list.        	JCA
  2007-Aug-22 Added OUTPUT_WATER_ERROR as output variable.      	JCA
  2008-Sep-09 Added SOIL_TNODE_WL as an output variable, the
	      soil temperature in the wetland fraction of the
	      grid cell.						LCB via TJB
  2009-Jan-16 Added AERO_COND1&2 and AERO_RESIST1&2 to track
	      surface and overstory values; changed AERO_COND
	      and AERO_RESIST to track "scene" values.			TJB
  2009-Feb-22 Added OUT_VPD.						TJB
  2009-May-17 Added OUT_ASAT.						TJB
  2009-Jun-09 Added OUT_PET_*, potential evap computed for
	      various landcover types.					TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.	TJB
  2009-Jul-07 Fixed nelem assignments for some band-specific vars.	TJB
  2009-Sep-19 Changed "*_FLAG" to "*_FBFLAG".				TJB
  2009-Oct-08 Extended T fallback scheme to snow and ice T.		TJB
  2010-Feb-14 Added OUT_LAKE_AREA_FRAC.					TJB
  2010-Mar-31 Added OUT_RUNOFF_IN.					TJB
  2010-Sep-24 Renamed RUNOFF_IN and OUT_RUNOFF_IN to CHANNEL_IN and
	      OUT_LAKE_CHAN_IN, respectively.  Renamed OUT_EVAP_LAKE
	      to OUT_LAKE_EVAP.  Added other lake water balance terms
	      to set of output variables.  Added volumetric versions 
	      of these too.						TJB
  2010-Nov-02 Added OUT_LAKE_RO_IN and OUT_LAKE_RO_IN_V for reporting
	      overland runoff input to lake.  Added OUT_LAKE_RCHRG and
	      OUT_LAKE_RCHRG_V for reporting lake recharge of
	      surrounding wetland.  Added OUT_LAKE_VAPFLX and
	      OUT_LAKE_VAPFLX_V.					TJB
  2010-Nov-21 Added OUT_LAKE_DSTOR, OUT_LAKE_DSTOR_V, OUT_LAKE_DSWE,
	      OUT_LAKE_DSWE_V, OUT_LAKE_SWE, and OUT_LAKE_SWE_V.	TJB
  2010-Dec-01 Added OUT_ZWT.						TJB
  2011-Mar-01 Added OUT_ZWT2, OUT_ZWT3, and OUT_ZWTL.			TJB
  2011-Nov-04 Added OUT_TSKC.						TJB
  2012-Feb-07 Removed OUT_ZWT2 and OUT_ZWTL; renamed OUT_ZWT3 to
	      OUT_ZWT_LUMPED.						TJB
*************************************************************/

  extern option_struct options;
  int v;
  out_data_struct *out_data;

  out_data = (out_data_struct *)calloc(N_OUTVAR_TYPES,sizeof(out_data_struct));

  // Build the list of supported output variables

  // Water Balance Terms - state variables
  strcpy(out_data[OUT_ASAT].varname,"OUT_ASAT");                       /* saturated area fraction */
  strcpy(out_data[OUT_LAKE_AREA_FRAC].varname,"OUT_LAKE_AREA_FRAC");   /* lake surface area as fraction of grid cell area [fraction] */
  strcpy(out_data[OUT_LAKE_DEPTH].varname,"OUT_LAKE_DEPTH");           /* lake depth [m] */
  strcpy(out_data[OUT_LAKE_ICE].varname,"OUT_LAKE_ICE");               /* moisture stored as lake ice [mm] */
  strcpy(out_data[OUT_LAKE_ICE_FRACT].varname,"OUT_LAKE_ICE_FRACT");   /* fractional coverage of lake ice [fraction] */
  strcpy(out_data[OUT_LAKE_ICE_HEIGHT].varname,"OUT_LAKE_ICE_HEIGHT"); /* thickness of lake ice [cm] */
  strcpy(out_data[OUT_LAKE_MOIST].varname,"OUT_LAKE_MOIST");           /* liquid water stored in lake [mm over lake area?] */
  strcpy(out_data[OUT_LAKE_SURF_AREA].varname,"OUT_LAKE_SURF_AREA");   /* lake surface area [m2] */
  strcpy(out_data[OUT_LAKE_SWE].varname,"OUT_LAKE_SWE");               /* liquid water equivalent of snow on top of lake ice [m over lake ice] */
  strcpy(out_data[OUT_LAKE_SWE_V].varname,"OUT_LAKE_SWE_V");           /* volumetric liquid water equivalent of snow on top of lake ice [m3] */
  strcpy(out_data[OUT_LAKE_VOLUME].varname,"OUT_LAKE_VOLUME");         /* lake volume [m3] */
  strcpy(out_data[OUT_ROOTMOIST].varname,"OUT_ROOTMOIST");             /* root zone soil moisture [mm] */
  strcpy(out_data[OUT_SMFROZFRAC].varname,"OUT_SMFROZFRAC");           /* fraction of soil moisture (by mass) that is ice, for each soil layer */
  strcpy(out_data[OUT_SMLIQFRAC].varname,"OUT_SMLIQFRAC");             /* fraction of soil moisture (by mass) that is liquid, for each soil layer */
  strcpy(out_data[OUT_SNOW_CANOPY].varname,"OUT_SNOW_CANOPY");         /* snow interception storage in canopy [mm] */
  strcpy(out_data[OUT_SNOW_COVER].varname,"OUT_SNOW_COVER");           /* fractional area of snow cover [fraction] */
  strcpy(out_data[OUT_SNOW_DEPTH].varname,"OUT_SNOW_DEPTH");           /* depth of snow pack [cm] */
  strcpy(out_data[OUT_SOIL_ICE].varname,"OUT_SOIL_ICE");               /* soil ice content [mm] for each soil layer */
  strcpy(out_data[OUT_SOIL_LIQ].varname,"OUT_SOIL_LIQ");               /* soil liquid moisture content [mm] for each soil layer */
  strcpy(out_data[OUT_SOIL_MOIST].varname,"OUT_SOIL_MOIST");           /* soil total moisture content [mm] for each soil layer */
  strcpy(out_data[OUT_SOIL_WET].varname,"OUT_SOIL_WET");               /* vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
  strcpy(out_data[OUT_SURFSTOR].varname,"OUT_SURFSTOR");               /* storage of liquid water on surface (ponding) [mm] */
  strcpy(out_data[OUT_SURF_FROST_FRAC].varname,"OUT_SURF_FROST_FRAC"); /* fraction of soil surface that is frozen [fraction] */
  strcpy(out_data[OUT_SWE].varname,"OUT_SWE");                         /* snow water equivalent in snow pack [mm] */
  strcpy(out_data[OUT_WDEW].varname,"OUT_WDEW");                       /* total moisture interception storage in canopy [mm] */
  strcpy(out_data[OUT_ZWT].varname,"OUT_ZWT");                         /* water table position [cm] (zwt within lowest unsaturated layer) */
  strcpy(out_data[OUT_ZWT_LUMPED].varname,"OUT_ZWT_LUMPED");           /* lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */

  // Water Balance Terms - fluxes
  strcpy(out_data[OUT_BASEFLOW].varname,"OUT_BASEFLOW");               /* baseflow out of the bottom layer [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_DELINTERCEPT].varname,"OUT_DELINTERCEPT");       /* change in canopy interception storage [mm] */
  strcpy(out_data[OUT_DELSOILMOIST].varname,"OUT_DELSOILMOIST");       /* change in soil water content [mm] */
  strcpy(out_data[OUT_DELSWE].varname,"OUT_DELSWE");                   /* change in snow water equivalent [mm] */
  strcpy(out_data[OUT_DELSURFSTOR].varname,"OUT_DELSURFSTOR");         /* change in surface liquid water storage  [mm] */
  strcpy(out_data[OUT_EVAP].varname,"OUT_EVAP");                       /* total net evaporation [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_EVAP_BARE].varname,"OUT_EVAP_BARE");             /* net evaporation from bare soil [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_EVAP_CANOP].varname,"OUT_EVAP_CANOP");           /* net evaporation from canopy interception [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_INFLOW].varname,"OUT_INFLOW");                   /* moisture that reaches top of soil column [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_PREC].varname,"OUT_PREC");                       /* incoming precipitation [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_RAINF].varname,"OUT_RAINF");                     /* rainfall [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_REFREEZE].varname,"OUT_REFREEZE");               /* refreezing of water in the snow [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_RUNOFF].varname,"OUT_RUNOFF");                   /* surface runoff [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SNOW_MELT].varname,"OUT_SNOW_MELT");             /* snow melt [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SNOWF].varname,"OUT_SNOWF");                     /* snowfall [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_BLOWING].varname,"OUT_SUB_BLOWING");         /* net sublimation of blowing snow [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_CANOP].varname,"OUT_SUB_CANOP");             /* net sublimation from snow stored in canopy [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_SNOW].varname,"OUT_SUB_SNOW");               /* net sublimation from snow pack (surface and blowing) [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_SUB_SURFACE].varname,"OUT_SUB_SURFACE");         /* net sublimation from snow pack surface [mm] (ALMA_OUTPUT: [mm/s]) */
  strcpy(out_data[OUT_TRANSP_VEG].varname,"OUT_TRANSP_VEG");           /* net transpiration from vegetation [mm] (ALMA_OUTPUT: [mm/s]) */

  // Energy Balance Terms - state variables
  strcpy(out_data[OUT_ALBEDO].varname,"OUT_ALBEDO");                   /* albedo [fraction] */
  strcpy(out_data[OUT_BARESOILT].varname,"OUT_BARESOILT");             /* bare soil surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_FDEPTH].varname,"OUT_FDEPTH");                   /* depth of freezing fronts [cm] (ALMA_OUTPUT: [m]) for each freezing front */
  strcpy(out_data[OUT_LAKE_ICE_TEMP].varname,"OUT_LAKE_ICE_TEMP");     /* lake ice temperature [K] */
  strcpy(out_data[OUT_LAKE_SURF_TEMP].varname,"OUT_LAKE_SURF_TEMP");   /* lake surface temperature [K] */
  strcpy(out_data[OUT_RAD_TEMP].varname,"OUT_RAD_TEMP");               /* average radiative surface temperature [K] */
  strcpy(out_data[OUT_SALBEDO].varname,"OUT_SALBEDO");                 /* snow albedo [fraction] */
  strcpy(out_data[OUT_SNOW_PACK_TEMP].varname,"OUT_SNOW_PACK_TEMP");   /* snow pack temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SNOW_SURF_TEMP].varname,"OUT_SNOW_SURF_TEMP");   /* snow surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SNOWT_FBFLAG].varname,"OUT_SNOWT_FBFLAG");       /* snow surface temperature flag */
  strcpy(out_data[OUT_SOIL_TEMP].varname,"OUT_SOIL_TEMP");             /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil layer */
  strcpy(out_data[OUT_SOIL_TNODE].varname,"OUT_SOIL_TNODE");           /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil thermal node */
  strcpy(out_data[OUT_SOIL_TNODE_WL].varname,"OUT_SOIL_TNODE_WL");     /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil thermal node in the wetland */
  strcpy(out_data[OUT_SOILT_FBFLAG].varname,"OUT_SOILT_FBFLAG");       /* soil temperature flag for each soil thermal node */
  strcpy(out_data[OUT_SURF_TEMP].varname,"OUT_SURF_TEMP");             /* average surface temperature [C] (ALMA_OUTPUT: [K]) */
  strcpy(out_data[OUT_SURFT_FBFLAG].varname,"OUT_SURFT_FBFLAG");       /* surface temperature flag */
  strcpy(out_data[OUT_TCAN_FBFLAG].varname,"OUT_TCAN_FBFLAG");         /* Tcanopy flag */
  strcpy(out_data[OUT_TDEPTH].varname,"OUT_TDEPTH");                   /* depth of thawing fronts [cm] (ALMA_OUTPUT: [m]) for each thawing front */
  strcpy(out_data[OUT_TFOL_FBFLAG].varname,"OUT_TFOL_FBFLAG");         /* Tfoliage flag */
  strcpy(out_data[OUT_VEGT].varname,"OUT_VEGT");                       /* average vegetation canopy temperature [C] (ALMA_OUTPUT: [K]) */

  // Miscellaneous Terms
  strcpy(out_data[OUT_AIR_TEMP].varname,"OUT_AIR_TEMP");               /* air temperature [C] */
  strcpy(out_data[OUT_DENSITY].varname,"OUT_DENSITY");                 /* near-surface atmospheric density [kg/m3] */
  strcpy(out_data[OUT_LONGWAVE].varname,"OUT_LONGWAVE");               /* incoming longwave [W/m2] */
  strcpy(out_data[OUT_PRESSURE].varname,"OUT_PRESSURE");               /* near surface atmospheric pressure [kPa] */
  strcpy(out_data[OUT_QAIR].varname,"OUT_QAIR");                       /* specific humidity [kg/kg] */
  strcpy(out_data[OUT_REL_HUMID].varname,"OUT_REL_HUMID");             /* relative humidity [fraction]*/
  strcpy(out_data[OUT_SHORTWAVE].varname,"OUT_SHORTWAVE");             /* incoming shortwave [W/m2] */
  strcpy(out_data[OUT_SURF_COND].varname,"OUT_SURF_COND");             /* surface conductance [m/s] */
  strcpy(out_data[OUT_TSKC].varname,"OUT_TSKC");                       /* cloud cover fraction [fraction] */
  strcpy(out_data[OUT_VP].varname,"OUT_VP");                           /* near surface vapor pressure [kPa] */
  strcpy(out_data[OUT_VPD].varname,"OUT_VPD");                         /* near surface vapor pressure deficit [kPa] */
  strcpy(out_data[OUT_WIND].varname,"OUT_WIND");                       /* near surface wind speed [m/s] */

  // Dynamic Soil Layer Terms - EXCESS_ICE option
#if EXCESS_ICE
  strcpy(out_data[OUT_SOIL_DEPTH].varname,"OUT_SOIL_DEPTH");             /* soil moisture layer depths [m] */
  strcpy(out_data[OUT_SUBSIDENCE].varname,"OUT_SUBSIDENCE");             /* subsidence of soil layer [mm] */
  strcpy(out_data[OUT_POROSITY].varname,"OUT_POROSITY");                 /* porosity [mm/mm] */
  strcpy(out_data[OUT_ZSUM_NODE].varname,"OUT_ZSUM_NODE");               /* depths of thermal nodes [m] */
#endif


  // Set number of elements - default is 1
  for (v=0; v<N_OUTVAR_TYPES; v++) {
    out_data[v].nelem = 1;
  }
  if (options.FROZEN_SOIL) {
    out_data[OUT_FDEPTH].nelem = MAX_FRONTS;
    out_data[OUT_TDEPTH].nelem = MAX_FRONTS;
  }
  out_data[OUT_SMLIQFRAC].nelem = options.Nlayer;
  out_data[OUT_SMFROZFRAC].nelem = options.Nlayer;
  out_data[OUT_SOIL_ICE].nelem = options.Nlayer;
  out_data[OUT_SOIL_LIQ].nelem = options.Nlayer;
  out_data[OUT_SOIL_MOIST].nelem = options.Nlayer;
  out_data[OUT_SOIL_TEMP].nelem = options.Nlayer;
#if EXCESS_ICE
  out_data[OUT_SOIL_DEPTH].nelem = options.Nlayer;
  out_data[OUT_SUBSIDENCE].nelem = options.Nlayer;
  out_data[OUT_POROSITY].nelem = options.Nlayer;
  out_data[OUT_ZSUM_NODE].nelem = options.Nnode;
#endif
  out_data[OUT_SOIL_TNODE].nelem = options.Nnode;
  out_data[OUT_SOIL_TNODE_WL].nelem = options.Nnode;
  out_data[OUT_SOILT_FBFLAG].nelem = options.Nnode;

  // Set aggregation method - default is to average over the interval
  for (v=0; v<N_OUTVAR_TYPES; v++) {
    out_data[v].aggtype = AGG_TYPE_AVG;
  }
  out_data[OUT_ASAT].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_AREA_FRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_DEPTH].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_ICE].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_ICE_FRACT].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_ICE_HEIGHT].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_MOIST].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_SURF_AREA].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_SWE].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_SWE_V].aggtype = AGG_TYPE_END;
  out_data[OUT_LAKE_VOLUME].aggtype = AGG_TYPE_END;
  out_data[OUT_ROOTMOIST].aggtype = AGG_TYPE_END;
  out_data[OUT_SMFROZFRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_SMLIQFRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_CANOPY].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_COVER].aggtype = AGG_TYPE_END;
  out_data[OUT_SNOW_DEPTH].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_ICE].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_LIQ].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_MOIST].aggtype = AGG_TYPE_END;
  out_data[OUT_SOIL_WET].aggtype = AGG_TYPE_END;
  out_data[OUT_SURFSTOR].aggtype = AGG_TYPE_END;
  out_data[OUT_SURF_FROST_FRAC].aggtype = AGG_TYPE_END;
  out_data[OUT_SWE].aggtype = AGG_TYPE_END;
  out_data[OUT_WDEW].aggtype = AGG_TYPE_END;
  out_data[OUT_ZWT].aggtype = AGG_TYPE_END;
  out_data[OUT_ZWT_LUMPED].aggtype = AGG_TYPE_END;
#if EXCESS_ICE
  out_data[OUT_SOIL_DEPTH].aggtype = AGG_TYPE_END;
  out_data[OUT_POROSITY].aggtype = AGG_TYPE_END;
  out_data[OUT_ZSUM_NODE].aggtype = AGG_TYPE_END;
  out_data[OUT_SUBSIDENCE].aggtype = AGG_TYPE_SUM;
#endif
  out_data[OUT_BASEFLOW].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELINTERCEPT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELSOILMOIST].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELSWE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_DELSURFSTOR].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_BARE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_EVAP_CANOP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_INFLOW].aggtype = AGG_TYPE_SUM;
  out_data[OUT_PREC].aggtype = AGG_TYPE_SUM;
  out_data[OUT_RAINF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_REFREEZE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_RUNOFF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOW_MELT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOWF].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_BLOWING].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_CANOP].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_SNOW].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SUB_SURFACE].aggtype = AGG_TYPE_SUM;
  out_data[OUT_TRANSP_VEG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOW_MELT].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SNOWT_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SOILT_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_SURFT_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_TCAN_FBFLAG].aggtype = AGG_TYPE_SUM;
  out_data[OUT_TFOL_FBFLAG].aggtype = AGG_TYPE_SUM;

  // Allocate space for data
  for (v=0; v<N_OUTVAR_TYPES; v++) {
    out_data[v].data = (double *)calloc(out_data[v].nelem, sizeof(double));
    out_data[v].aggdata = (double *)calloc(out_data[v].nelem, sizeof(double));
  }

  // Initialize data values
  init_output_list(out_data, FALSE, (char*) "%.4f", OUT_TYPE_FLOAT, 1);

  return out_data;

}


void init_output_list(out_data_struct *out_data, int write, char *format, int type, float mult) {
/*************************************************************
  init_output_list()      Ted Bohn     September 08, 2006

  This routine initializes the output information for all output variables.

*************************************************************/
  int varid, i;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    out_data[varid].write = write;
    strcpy(out_data[varid].format,format);
    out_data[varid].type = type;
    out_data[varid].mult = mult;
    for(i=0; i<out_data[varid].nelem; i++) {
      out_data[varid].data[i] = 0;
    }
  }

}


int set_output_var(out_data_file_struct *out_data_files,
                    int write,
                    int filenum,
                    out_data_struct *out_data,
                    char *varname,
                    int varnum,
                    char *format,
                    int type,
                    float mult) {
/*************************************************************
  set_output_var()      Ted Bohn     September 08, 2006

  This routine updates the output information for a given output variable.

*************************************************************/
  int varid;
  int found=FALSE;
  int status=0;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    if (strcmp(out_data[varid].varname,varname) == 0) {
      found = TRUE;
      out_data[varid].write = write;
      if (strcmp(format,"*") != 0)
        strcpy(out_data[varid].format,format);
      if (type != 0)
        out_data[varid].type = type;
      if (mult != 0)
        out_data[varid].mult = mult;
      out_data_files->varid[varnum] = varid;
    }
  }
  if (!found) {
    status = -1;
    fprintf(stderr, "Error: set_output_var: \"%s\" was not found in the list of supported output variable names.  Please use the exact name listed in vicNl_def.h.\n", varname);
  }
  return status;

}


void zero_output_list(out_data_struct *out_data) {
/*************************************************************
  zero_output_list()      Ted Bohn     September 08, 2006

  This routine resets the values of all output variables to 0.

*************************************************************/
  int varid, i;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    for(i=0; i<out_data[varid].nelem; i++) {
      out_data[varid].data[i] = 0;
    }
  }

}

void free_out_data_files(out_data_file_struct **out_data_files) {
/*************************************************************
  free_out_data_files()      Ted Bohn     September 08, 2006

  This routine frees the memory in the out_data_files array.

*************************************************************/
  extern option_struct options;
 
    free((*out_data_files)->varid);
  free((*out_data_files));

}

void free_out_data(out_data_struct **out_data) {
/*************************************************************
  free_out_data()      Ted Bohn     April 19, 2007

  This routine frees the memory in the out_data array.

*************************************************************/

  int varid;

  for (varid=0; varid<N_OUTVAR_TYPES; varid++) {
    free((*out_data)[varid].data);
    free((*out_data)[varid].aggdata);
  }
  free((*out_data));

}


#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"

void initialize_atmos(atmos_data_struct        *atmos,
                      dmy_struct               *dmy,
		      double **forcing_data,
		      soil_con_struct          *soil_con,
                      out_data_file_struct     *out_data_files,
                      out_data_struct          *out_data)
{
  extern option_struct       options;
  extern param_set_struct    param_set;
  extern global_param_struct global_param;
  extern int                 NR, NF;

  int     i;
  int     j;
  int     day;
  int     hour;
  int     rec;
  int     idx;
  int    *tmaxhour;
  int    *tminhour;
  double  theta_l;
  double  theta_s;
  double  hour_offset;
  double  phi;
  double  elevation;
  double  slope;
  double  aspect;
  double  ehoriz;
  double  whoriz;
  double  annual_prec;
  double *hourlyrad;
  double *prec;
  double *tmax;
  double *tmin;
  double *tair;
  double *tskc;
  double *daily_vp;
  int     Ndays;
  int     stepspday;
  double  sum, sum2;
  double **local_forcing_data;
  int     type;
  double  delta_t_minus;
  double  delta_t_plus;
  int have_dewpt;
  int have_shortwave;
  int hour_offset_int;
  int tmp_starthour, tmp_endhour;
  int local_startyear, local_startmonth, local_startday;
  int local_starthour;
  int day_in_year, year, month, days_in_month;
  int tmp_nrecs;
  int Ndays_local;
  dmy_struct *dmy_local;
  int month_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int fstepspday;
  double tmp_double;

  theta_l = (double)soil_con->time_zone_lng;
  theta_s = (double)soil_con->lng;
  hour_offset = (theta_l-theta_s)*24/360;
  if (hour_offset < 0)
    hour_offset_int = (int)(hour_offset-0.5);
  else
    hour_offset_int = (int)(hour_offset+0.5);
  hour_offset -= hour_offset_int; // hour_offset is now the distance from the center of local time zone
  phi = soil_con->lat;
  elevation = soil_con->elevation;
  slope = soil_con->slope;
  aspect = soil_con->aspect;
  ehoriz = soil_con->ehoriz;
  whoriz = soil_con->whoriz;
  annual_prec = soil_con->annual_prec;

  /* Check on minimum forcing requirements */
  if ( !param_set.TYPE[PREC].SUPPLIED
    && ( ( !param_set.TYPE[RAINF].SUPPLIED && ( !param_set.TYPE[LSRAINF].SUPPLIED || !param_set.TYPE[CRAINF].SUPPLIED ) )
      || ( ( !param_set.TYPE[SNOWF].SUPPLIED && ( !param_set.TYPE[LSSNOWF].SUPPLIED || !param_set.TYPE[CSNOWF].SUPPLIED ) ) ) ) )
    nrerror((char*) "Input meteorological forcing files must contain some form of precipitation (PREC, or { {RAINF or {LSRAINF and CRAINF}} and {SNOWF or {LSSNOWF and CSNOWF}} }); check input files\n");

  if (   !(   param_set.TYPE[TMAX].SUPPLIED && param_set.FORCE_DT == 24 )
      && !(param_set.TYPE[AIR_TEMP].SUPPLIED && param_set.FORCE_DT < 24) )
    nrerror((char*) "Input meteorological forcing files must contain either: a. Daily TMAX and TMIN (maximum and minimum air temperature) or b. sub-daily AIR_TEMP (air temperature); check input files\n");

//  if ( !param_set.TYPE[WIND].SUPPLIED && !(param_set.TYPE[WIND_N].SUPPLIED && param_set.TYPE[WIND_E].SUPPLIED) )
//    nrerror((char*) "Input meteorological forcing files must contain either WIND (wind speed) or both WIND_N (north component of wind speed) and WIND_E (east component of wind speed); check input files\n");

  /* compute number of simulation days */
  tmp_starthour = 0;
  tmp_endhour = 24 - global_param.dt;
  tmp_nrecs = global_param.nrecs+global_param.starthour-tmp_starthour+tmp_endhour-dmy[global_param.nrecs-1].hour;
  Ndays = (tmp_nrecs * global_param.dt) / 24;

  /* compute number of full model time steps per day */
  stepspday = 24/global_param.dt;

  /* Compute number of days for MTCLIM (in local time); for sub-daily, we must pad start and end with dummy records */
  Ndays_local = Ndays;
  if (hour_offset_int != 0) Ndays_local = Ndays + 1;

  local_starthour = global_param.starthour - hour_offset_int;
  local_startday = global_param.startday;
  local_startmonth = global_param.startmonth;
  local_startyear = global_param.startyear;
  if (local_starthour < 0) {
    local_starthour += 24;
    local_startday--;
    if (local_startday < 1) {
      local_startmonth--;
      if (local_startmonth < 1) {
        local_startmonth = 12;
        local_startyear--;
      }
      local_startday = month_days[local_startmonth-1];
      if (local_startyear % 4 == 0 && local_startmonth == 2) {
        local_startday++;
      }
    }
  }

  /* compute local version of dmy array */
  dmy_local = (dmy_struct *) calloc(Ndays_local*24, sizeof(dmy_struct));
  if (dmy_local == NULL) {
    nrerror((char*) "Memory allocation failure in initialize_atmos()");
  }
  day_in_year = local_startday;
  for (month=1; month <local_startmonth; month++) {
    days_in_month = month_days[month-1];
    if (local_startyear % 4 == 0 && month == 2) {
      days_in_month++;
    }
    day_in_year += days_in_month;
  }
  year = local_startyear;
  month = local_startmonth;
  day = local_startday;
  rec = 0;
  hour = 0;
  while (rec < Ndays_local*24) {
    dmy_local[rec].day_in_year = day_in_year;
    dmy_local[rec].year = year;
    dmy_local[rec].month = month;
    dmy_local[rec].day = day;
    dmy_local[rec].hour = hour;
    rec++;
    hour++;
    if (hour == 24) {
      hour = 0;
      day_in_year++;
      day++;
      days_in_month = month_days[month-1];
      if (year % 4 == 0 && month == 2) {
        days_in_month++;
      }
      if (day > days_in_month) {
        day = 1;
        month++;
        if (month > 12) {
          day_in_year = 1;
          month = 1;
          year++;
        }
      }
    }
  }

  /* mtclim routine memory allocations */
  hourlyrad  = (double *) calloc(Ndays_local*24, sizeof(double));
  prec       = (double *) calloc(Ndays_local*24, sizeof(double));
  tair       = (double *) calloc(Ndays_local*24, sizeof(double));
  tmax       = (double *) calloc(Ndays_local, sizeof(double));
  tmaxhour   = (int *)    calloc(Ndays_local, sizeof(double));
  tmin       = (double *) calloc(Ndays_local, sizeof(double));
  tminhour   = (int *)    calloc(Ndays_local, sizeof(double));
  tskc       = (double *) calloc(Ndays_local*24, sizeof(double));
  daily_vp   = (double *) calloc(Ndays_local, sizeof(double));

  if (hourlyrad == NULL || prec == NULL || tair == NULL || tmax == NULL ||
      tmaxhour == NULL ||  tmin == NULL || tminhour == NULL || tskc == NULL ||
      daily_vp == NULL)
    nrerror((char*) "Memory allocation failure in initialize_atmos()");

  /*************************************************
    Pre-processing
  *************************************************/

  /*************************************************
    Convert units from ALMA to VIC standard, if necessary
  *************************************************/
  if (options.ALMA_INPUT) {
    for (type=0; type<N_FORCING_TYPES; type++) {
      if (param_set.TYPE[type].SUPPLIED) {
        /* Convert moisture flux rates to accumulated moisture flux per time step */
        if (   type == PREC
            || type == RAINF
            || type == CRAINF
            || type == LSRAINF
            || type == SNOWF
            || type == CSNOWF
            || type == LSSNOWF
           ) {
          for (idx=0; idx<(global_param.nrecs*NF); idx++) {
            forcing_data[type][idx] *= param_set.FORCE_DT * 3600;
          }
        }
        /* Convert temperatures from K to C */
        else if (   type == AIR_TEMP
                 || type == TMIN
                 || type == TMAX
                ) {
          for (idx=0; idx<(global_param.nrecs*NF); idx++) {
            forcing_data[type][idx] -= KELVIN;
          }
        }
      }
    }
  }
  else {
    for (type=0; type<N_FORCING_TYPES; type++) {
      if (param_set.TYPE[type].SUPPLIED) {
        /* Convert pressures from kPa to Pa */
        if (   type == PRESSURE
            || type == VP
           ) {
          for (idx=0; idx<(global_param.nrecs*NF); idx++) {
            forcing_data[type][idx] *= kPa2Pa;
          }
        }
      }
    }
  }

  /*************************************************
    If provided, translate rainfall and snowfall
    into total precipitation
    NOTE: this overwrites any PREC data that was supplied
  *************************************************/

  if(param_set.TYPE[RAINF].SUPPLIED && param_set.TYPE[SNOWF].SUPPLIED) {
    /* rainfall and snowfall supplied */
    if (forcing_data[PREC] == NULL) {
      forcing_data[PREC] = (double *)calloc((global_param.nrecs * NF),sizeof(double));
      printf("calloc PREC1\n");

    }
    for (idx=0; idx<(global_param.nrecs*NF); idx++) {
      forcing_data[PREC][idx] = forcing_data[RAINF][idx] + forcing_data[SNOWF][idx];
    }
    param_set.TYPE[PREC].SUPPLIED = param_set.TYPE[RAINF].SUPPLIED;
  }
  else if(param_set.TYPE[CRAINF].SUPPLIED && param_set.TYPE[LSRAINF].SUPPLIED
    && param_set.TYPE[CSNOWF].SUPPLIED && param_set.TYPE[LSSNOWF].SUPPLIED) {
    /* convective and large-scale rainfall and snowfall supplied */
    if (forcing_data[PREC] == NULL) {
      forcing_data[PREC] = (double *)calloc((global_param.nrecs * NF),sizeof(double));
            printf("calloc PREC2\n");

    }
    for (idx=0; idx<(global_param.nrecs*NF); idx++) {
      forcing_data[PREC][idx] = forcing_data[CRAINF][idx] + forcing_data[LSRAINF][idx]
                               + forcing_data[CSNOWF][idx] + forcing_data[LSSNOWF][idx];
    }
    param_set.TYPE[PREC].SUPPLIED = param_set.TYPE[LSRAINF].SUPPLIED;
  }

  /*************************************************
    If provided, translate WIND_E and WIND_N into WIND
    NOTE: this overwrites any WIND data that was supplied
  *************************************************/

  if(param_set.TYPE[WIND_E].SUPPLIED && param_set.TYPE[WIND_N].SUPPLIED) {
    /* specific wind_e and wind_n supplied */
    if (forcing_data[WIND] == NULL) {
      forcing_data[WIND] = (double *)calloc((global_param.nrecs * NF),sizeof(double));
            printf("calloc WIND\n");

    }
    for (idx=0; idx<(global_param.nrecs*NF); idx++) {
      forcing_data[WIND][idx] = sqrt( forcing_data[WIND_E][idx]*forcing_data[WIND_E][idx]
                                    + forcing_data[WIND_N][idx]*forcing_data[WIND_N][idx] );
    }
    param_set.TYPE[WIND].SUPPLIED = param_set.TYPE[WIND_E].SUPPLIED;
  }

  /*************************************************
    Create new forcing arrays referenced to local time
    This will simplify subsequent data processing
  *************************************************/

  local_forcing_data = (double **) calloc(N_FORCING_TYPES, sizeof(double*));
  for (type=0; type<N_FORCING_TYPES; type++) {
    // Allocate enough space for hourly data
    if ( ( local_forcing_data[type] = (double *)calloc(Ndays_local*24, sizeof(double)) ) == NULL ) {
      nrerror((char*) "Memory allocation failure in initialize_atmos()");
    }
    if (param_set.TYPE[type].SUPPLIED) {
      if (param_set.FORCE_DT == 24) {
        // Daily forcings in non-local time will straddle local day boundaries and need to be padded with an extra day at start or end
        for (idx=0; idx<Ndays_local; idx++) {
          i = idx;
          if (hour_offset_int > 0) i--; // W. Hemisphere, in GMT time
          if (i < 0) i = 0; // W. Hemisphere, in GMT time; pad extra day in front
          if (i >= Ndays) i = Ndays-1; // E. Hemisphere, in GMT time; pad extra day at end
          local_forcing_data[type][idx] = forcing_data[type][i];
        }
      }
      else {
        // Local sub-daily forcings will be hourly for coding convenience
        // Sub-daily forcings need to a) start at hour 0, local time and b) draw from the correct element of the supplied forcings (if the supplied forcings are not in local time)
        fstepspday = 24/param_set.FORCE_DT;
        for (idx=0; idx<(Ndays_local*24); idx++) {
          i = (idx - global_param.starthour + hour_offset_int)/param_set.FORCE_DT;
          if (i < 0) i += fstepspday;
          if (i >= (Ndays*fstepspday)) i -= fstepspday;
          if (   type == PREC
              || type == RAINF
              || type == CRAINF
              || type == LSRAINF
              || type == SNOWF
              || type == CSNOWF
              || type == LSSNOWF
             ) {
            /* Amounts per step need to be scaled to new step length */
            local_forcing_data[type][idx] = forcing_data[type][i]/param_set.FORCE_DT;
          }
          else {
            /* All other forcings are assumed constant over hourly substeps */
            local_forcing_data[type][idx] = forcing_data[type][i];
          }
        }
      }
    }
  }

  /*************************************************
    Precipitation
  *************************************************/

  if(param_set.FORCE_DT == 24) {
    /* daily precipitation provided */
    for (rec = 0; rec < global_param.nrecs; rec++) {
      sum = 0;
      for (j = 0; j < NF; j++) {
        hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
        if (global_param.starthour - hour_offset_int < 0) hour += 24;
        idx = (int)((float)hour/24.0);
        atmos[rec].prec[j] = local_forcing_data[PREC][idx] / (float)(NF*stepspday); // divide evenly over the day
        sum += atmos[rec].prec[j];
      }
      if(NF>1) atmos[rec].prec[NR] = sum;
    }
    for (day = 0; day < Ndays_local; day++) {
      prec[day] = local_forcing_data[PREC][day];
    }
  }
  else {
    /* sub-daily precipitation provided */
    for(rec = 0; rec < global_param.nrecs; rec++) {
      sum = 0;
      for(i = 0; i < NF; i++) {
        hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
        if (global_param.starthour - hour_offset_int < 0) hour += 24;
        atmos[rec].prec[i] = 0;
        for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	  atmos[rec].prec[i] += local_forcing_data[PREC][idx];
        }
	sum += atmos[rec].prec[i];
      }
      if(NF>1) atmos[rec].prec[NR] = sum;
    }
    for (day = 0; day < Ndays_local; day++) {
      prec[day] = 0;
      for (hour=0; hour<24; hour++) {
        prec[day] += local_forcing_data[PREC][day*24+hour];
      }
    }
  }

  /*************************************************
    Wind Speed
  *************************************************/

  if (param_set.TYPE[WIND].SUPPLIED) {
    if(param_set.FORCE_DT == 24) {
      /* daily wind provided */
      for (rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for (j = 0; j < NF; j++) {
          hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          idx = (int)((float)hour/24.0);
          atmos[rec].wind[j] = local_forcing_data[WIND][idx]; // assume constant over the day
          sum += atmos[rec].wind[j];
        }
        if(NF>1) atmos[rec].wind[NR] = sum / (float)NF;
	if(global_param.dt == 24) {
	  if(atmos[rec].wind[j] < options.MIN_WIND_SPEED)
	    atmos[rec].wind[j] = options.MIN_WIND_SPEED;
	}
      }
    }
    else {
      /* sub-daily wind provided */
      for(rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for(i = 0; i < NF; i++) {
          hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          atmos[rec].wind[i] = 0;
          for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	    if(local_forcing_data[WIND][idx] < options.MIN_WIND_SPEED)
	      atmos[rec].wind[i] += options.MIN_WIND_SPEED;
	    else
	      atmos[rec].wind[i] += local_forcing_data[WIND][idx];
          }
          atmos[rec].wind[i] /= options.SNOW_STEP;
	  sum += atmos[rec].wind[i];
        }
        if(NF>1) atmos[rec].wind[NR] = sum / (float)NF;
      }
    }
  }
  else {
    /* no wind data provided, use default constant */
    for (rec = 0; rec < global_param.nrecs; rec++) {
      for (i = 0; i < NF; i++) {
	atmos[rec].wind[i] = DEFAULT_WIND_SPEED;
      }
      atmos[rec].wind[NR] = DEFAULT_WIND_SPEED;
    }
  }

  /*************************************************
    Air Temperature, part 1.
  *************************************************/

  /************************************************
    Set maximum daily air temperature if provided
  ************************************************/

  if(param_set.TYPE[TMAX].SUPPLIED) {
    if(param_set.FORCE_DT == 24) {
      /* daily tmax provided */
      for (day = 0; day < Ndays_local; day++) {
	tmax[day] = local_forcing_data[TMAX][day];
      }
    }
    else {
      /* sub-daily tmax provided */
      for (day = 0; day < Ndays_local; day++) {
	tmax[day] = local_forcing_data[TMAX][day*24];
      }
    }
  }

  /************************************************
    Set minimum daily air temperature if provided
  ************************************************/

  if(param_set.TYPE[TMIN].SUPPLIED) {
    if(param_set.FORCE_DT == 24) {
      /* daily tmin provided */
      for (day = 0; day < Ndays_local; day++) {
	tmin[day] = local_forcing_data[TMIN][day];
      }
    }
    else {
      /* sub-daily tmin provided */
      for (day = 0; day < Ndays_local; day++) {
	tmin[day] = local_forcing_data[TMIN][day*24];
      }
    }
  }

  /*************************************************
    Store sub-daily air temperature if provided
  *************************************************/

  if(param_set.TYPE[AIR_TEMP].SUPPLIED) {
    for(rec = 0; rec < global_param.nrecs; rec++) {
      sum = 0;
      for(i = 0; i < NF; i++) {
        hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
        if (global_param.starthour - hour_offset_int < 0) hour += 24;
        atmos[rec].air_temp[i] = 0;
        for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	  atmos[rec].air_temp[i] += local_forcing_data[AIR_TEMP][idx];
        }
        atmos[rec].air_temp[i] /= options.SNOW_STEP;
	sum += atmos[rec].air_temp[i];
      }
      if(NF>1) atmos[rec].air_temp[NR] = sum / (float)NF;
    }
  }

  /******************************************************
    Determine Tmax and Tmin from sub-daily temperatures
  ******************************************************/

  if(!(param_set.TYPE[TMAX].SUPPLIED && param_set.TYPE[TMIN].SUPPLIED)) {
    for (day=0; day<Ndays_local; day++) {
      tmax[day] = tmin[day] = -9999;
      for (hour = 0; hour < 24; hour++) {
        if ( hour >= 9 && ( tmax[day] == -9999 || local_forcing_data[AIR_TEMP][hour] > tmax[day] ) ) tmax[day] = local_forcing_data[AIR_TEMP][hour];
        if ( hour < 12 && ( tmin[day] == -9999 || local_forcing_data[AIR_TEMP][hour] < tmin[day] ) ) tmin[day] = local_forcing_data[AIR_TEMP][hour];
      }
    }
  }


  /*************************************************
    Vapor Pressure, part 1.
  *************************************************/

  if(!param_set.TYPE[VP].SUPPLIED) {

    /*************************************************
      If provided, translate specific humidity and atm. pressure
      into vapor pressure
    *************************************************/

    if(param_set.TYPE[QAIR].SUPPLIED && param_set.TYPE[PRESSURE].SUPPLIED) {
      /* specific humidity and atm. pressure supplied */
      if(param_set.FORCE_DT == 24) {
        for (day=0; day<Ndays_local; day++) {
          if(param_set.FORCE_DT < 24) {
            tmp_double = 0;
            for (hour=0; hour<24; hour++) {
              tmp_double += local_forcing_data[PRESSURE][day*24+hour];
            }
            tmp_double /= 24;
          }
          else {
            tmp_double = local_forcing_data[PRESSURE][day];
          }
          local_forcing_data[VP][day] = local_forcing_data[QAIR][day] * tmp_double / EPS;
          daily_vp[day] = local_forcing_data[VP][day];
        }
      }
      else {
        for (day=0; day<Ndays_local; day++) {
          daily_vp[day] = 0;
          for (hour=0; hour<24; hour++) {
            if(param_set.FORCE_DT == 24) {
              tmp_double = local_forcing_data[PRESSURE][day];
            }
            else {
              tmp_double = local_forcing_data[PRESSURE][day*24+hour];
            }
            local_forcing_data[VP][day*24+hour] = local_forcing_data[QAIR][day*24+hour] * tmp_double / EPS;
            daily_vp[day] += local_forcing_data[VP][day*24+hour];
          }
          daily_vp[day] /= 24;
        }
      }
      param_set.TYPE[VP].SUPPLIED = param_set.TYPE[QAIR].SUPPLIED;
    }

    /*************************************************
      If provided, translate relative humidity and air temperature
      into vapor pressure
    *************************************************/

    else if(param_set.TYPE[REL_HUMID].SUPPLIED && param_set.TYPE[AIR_TEMP].SUPPLIED) {
      /* relative humidity and atm. pressure supplied */
      if(param_set.FORCE_DT == 24) {
        for (day=0; day<Ndays_local; day++) {
          if(param_set.FORCE_DT< 24) {
            tmp_double = 0;
            for (hour=0; hour<24; hour++) {
              tmp_double += svp(local_forcing_data[AIR_TEMP][day*24+hour]);
            }
            tmp_double /= 24;
          }
          else {
            tmp_double = svp(local_forcing_data[AIR_TEMP][day]);
          }
          local_forcing_data[VP][day] = local_forcing_data[REL_HUMID][day] * tmp_double / 100;
          daily_vp[day] = local_forcing_data[VP][day];
        }
      }
      else {
        for (day=0; day<Ndays_local; day++) {
          daily_vp[day] = 0;
          for (hour=0; hour<24; hour++) {
            if(param_set.FORCE_DT == 24) {
              tmp_double = svp(local_forcing_data[AIR_TEMP][day]);
            }
            else {
              tmp_double = svp(local_forcing_data[AIR_TEMP][day*24+hour]);
            }
            local_forcing_data[VP][day*24+hour] = local_forcing_data[REL_HUMID][day*24+hour] * tmp_double / 100;
            daily_vp[day] += local_forcing_data[VP][day*24+hour];
          }
          daily_vp[day] /= 24;
        }
      }
      param_set.TYPE[VP].SUPPLIED = param_set.TYPE[REL_HUMID].SUPPLIED;
    }

  } // end if VP not supplied

  /*************************************************
    If vapor pressure supplied, transfer to appropriate arrays
  *************************************************/

  if(param_set.TYPE[VP].SUPPLIED) {

    have_dewpt = 2; // flag for MTCLIM

    if(param_set.FORCE_DT == 24) {
      /* daily vp provided */
      for (day=0; day<Ndays_local; day++) {
        daily_vp[day] = local_forcing_data[VP][day];
      }
      for (rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for (j = 0; j < NF; j++) {
          hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          idx = (int)((float)hour/24.0);
          atmos[rec].vp[j] = local_forcing_data[VP][idx]; // assume constant over the day
          sum += atmos[rec].vp[j];
        }
        if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
      }
    }
    else {
      /* sub-daily vp provided */
      for (day=0; day<Ndays_local; day++) {
        daily_vp[day] = 0;
        for (hour=0; hour<24; hour++) {
          daily_vp[day] += local_forcing_data[VP][day*24+hour];
        }
        daily_vp[day] /= 24;
      }
      for(rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for(i = 0; i < NF; i++) {
          hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          atmos[rec].vp[i] = 0;
          for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	    atmos[rec].vp[i] += local_forcing_data[VP][idx];
          }
          atmos[rec].vp[i] /= options.SNOW_STEP;
	  sum += atmos[rec].vp[i];
        }
        if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
      }
    }

  }
  else {
    have_dewpt = 0;
  } // end if VP supplied


  /*************************************************
    Shortwave, part 1.
  *************************************************/

  if (param_set.TYPE[SHORTWAVE].SUPPLIED) {
    have_shortwave = 1; // flag for MTCLIM
    for (day=0; day<Ndays_local; day++) {
      for (hour=0; hour<24; hour++) {
        if(param_set.FORCE_DT == 24) {
          hourlyrad[day*24+hour] = local_forcing_data[SHORTWAVE][day];
        }
        else {
          hourlyrad[day*24+hour] = local_forcing_data[SHORTWAVE][day*24+hour];
        }
      }
    }
  }
  else {
    have_shortwave = 0;
  }

  /**************************************************
    Use MTCLIM algorithms to estimate hourly shortwave,
    daily vapor pressure, and cloud radiation attenuation.

    Requires prec, tmax, and tmin.

    If we already have observations of shortwave and/or
    vp, MTCLIM will use them to compute the other variables
    more accurately.
  **************************************************/
  mtclim_wrapper(have_dewpt, have_shortwave, hour_offset, elevation, slope,
                   aspect, ehoriz, whoriz, annual_prec, phi, Ndays_local,
                   dmy_local, prec, tmax, tmin, tskc, daily_vp, hourlyrad);

  /***********************************************************
    Shortwave, part 2.
    Transfer the hourly shortwave from MTCLIM to atmos array.
    This hourly shortwave is one of the following:
    a) exactly equal to the supplied shortwave, if supplied shortwave was hourly
    b) equal to the supplied shortwave when aggregated up to the DT of the supplied shortwave (with hourly variability estimated by MTCLIM)
    c) completely estimated by MTCLIM, if no shortwave was supplied as a forcing
  ***********************************************************/

  // Ignore MTCLIM estimates if sub-daily SW was supplied
  if (param_set.TYPE[SHORTWAVE].SUPPLIED && param_set.FORCE_DT < 24) {
    for (day=0; day<Ndays_local; day++) {
      for (hour=0; hour<24; hour++) {
        hourlyrad[day*24+hour] = local_forcing_data[SHORTWAVE][day*24+hour];
      }
    }
  }
  // Transfer hourlyrad to atmos structure
  for(rec = 0; rec < global_param.nrecs; rec++) {
    sum = 0;
    for(i = 0; i < NF; i++) {
      hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
      if (global_param.starthour - hour_offset_int < 0) hour += 24;
      atmos[rec].shortwave[i] = 0;
      for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	atmos[rec].shortwave[i] += hourlyrad[idx];
      }
      atmos[rec].shortwave[i] /= options.SNOW_STEP;
      sum += atmos[rec].shortwave[i];
    }
    if(NF>1) atmos[rec].shortwave[NR] = sum / (float)NF;
  }

  /**************************************************************************
    Air Temperature, part 2.
  **************************************************************************/

  /**************************************************************************
    Calculate the hours at which the minimum and maximum temperatures occur
    (if sub-daily air_temp will be estimated) and/or at which daily vapor
    pressure will occur (if daily vapor pressure is estimated)
  **************************************************************************/
  set_max_min_hour(hourlyrad, Ndays_local, tmaxhour, tminhour);

  if(!param_set.TYPE[AIR_TEMP].SUPPLIED) {

    /**********************************************************************
      Calculate the subdaily and daily temperature based on tmax and tmin
    **********************************************************************/
    HourlyT(1, Ndays_local, tmaxhour, tmax, tminhour, tmin, tair);
    for(rec = 0; rec < global_param.nrecs; rec++) {
      sum = 0;
      for(i = 0; i < NF; i++) {
        hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
        if (global_param.starthour - hour_offset_int < 0) hour += 24;
        atmos[rec].air_temp[i] = 0;
        for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	  atmos[rec].air_temp[i] += tair[idx];
        }
        atmos[rec].air_temp[i] /= options.SNOW_STEP;
        sum += atmos[rec].air_temp[i];
      }
      if(NF>1) atmos[rec].air_temp[NR] = sum / (float)NF;
    }

  }


  /**************************************************************************
    Atmospheric Pressure and Density
  **************************************************************************/

  /*************************************************
    Store atmospheric density if provided (kg/m^3)
  *************************************************/

  if (param_set.TYPE[DENSITY].SUPPLIED) {
    if(param_set.FORCE_DT == 24) {
      /* daily density provided */
      for (rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for (j = 0; j < NF; j++) {
          hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          idx = (int)((float)hour/24.0);
          atmos[rec].density[j] = local_forcing_data[DENSITY][idx]; // assume constant over the day
          sum += atmos[rec].density[j];
        }
        if(NF>1) atmos[rec].density[NR] = sum / (float)NF;
      }
    }
    else {
      /* sub-daily density provided */
      for(rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for(i = 0; i < NF; i++) {
          hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          atmos[rec].density[i] = 0;
          for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	    atmos[rec].density[i] += local_forcing_data[DENSITY][idx];
          }
          atmos[rec].density[i] /= options.SNOW_STEP;
	  sum += atmos[rec].density[i];
        }
        if(NF>1) atmos[rec].density[NR] = sum / (float)NF;
      }
    }
  }

  /**************************************
    Estimate Atmospheric Pressure (Pa)
  **************************************/

  if(!param_set.TYPE[PRESSURE].SUPPLIED) {
    if(!param_set.TYPE[DENSITY].SUPPLIED) {
      /* Estimate pressure */
      if (options.PLAPSE) {
        /* Assume average virtual temperature in air column
           between ground and sea level = KELVIN+atmos[rec].air_temp[NR] + 0.5*elevation*LAPSE_PM */
        for (rec = 0; rec < global_param.nrecs; rec++) {
          atmos[rec].pressure[NR] = PS_PM*exp(-elevation*G/(Rd*(KELVIN+atmos[rec].air_temp[NR]+0.5*elevation*LAPSE_PM)));
          for (i = 0; i < NF; i++) {
            atmos[rec].pressure[i] = PS_PM*exp(-elevation*G/(Rd*(KELVIN+atmos[rec].air_temp[i]+0.5*elevation*LAPSE_PM)));
          }
        }
      }
      else {
        /* set pressure to constant value */
        for (rec = 0; rec < global_param.nrecs; rec++) {
	  atmos[rec].pressure[NR] = 95500.;
	  for (i = 0; i < NF; i++) {
	    atmos[rec].pressure[i] = atmos[rec].pressure[NR];
	  }
        }
      }
    }
    else {
      /* use observed densities to estimate pressure */
      if (options.PLAPSE) {
        for (rec = 0; rec < global_param.nrecs; rec++) {
          atmos[rec].pressure[NR] = (KELVIN+atmos[rec].air_temp[NR])*atmos[rec].density[NR]*Rd;
          for (i = 0; i < NF; i++) {
            atmos[rec].pressure[i] = (KELVIN+atmos[rec].air_temp[i])*atmos[rec].density[i]*Rd;
          }
        }
      }
      else {
        for (rec = 0; rec < global_param.nrecs; rec++) {
	  atmos[rec].pressure[NR] = (275.0 + atmos[rec].air_temp[NR]) *atmos[rec].density[NR]/0.003486;
	  for (i = 0; i < NF; i++) {
	    atmos[rec].pressure[i] = (275.0 + atmos[rec].air_temp[i]) *atmos[rec].density[i]/0.003486;
	  }
        }
      }
    }
  }
  else {
    /* observed atmospheric pressure supplied */
    if(param_set.FORCE_DT == 24) {
      /* daily pressure provided */
      for (rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for (j = 0; j < NF; j++) {
          hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          idx = (int)((float)hour/24.0);
          atmos[rec].pressure[j] = local_forcing_data[PRESSURE][idx]; // assume constant over the day
          sum += atmos[rec].pressure[j];
        }
        if(NF>1) atmos[rec].pressure[NR] = sum / (float)NF;
      }
    }
    else {
      /* sub-daily pressure provided */
      for(rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for(i = 0; i < NF; i++) {
          hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          atmos[rec].pressure[i] = 0;
          for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	    atmos[rec].pressure[i] += local_forcing_data[PRESSURE][idx];
          }
          atmos[rec].pressure[i] /= options.SNOW_STEP;
	  sum += atmos[rec].pressure[i];
        }
        if(NF>1) atmos[rec].pressure[NR] = sum / (float)NF;
      }
    }
  }

  /********************************************************
    Estimate Atmospheric Density if not provided (kg/m^3)
  ********************************************************/

  if(!param_set.TYPE[DENSITY].SUPPLIED) {
    /* use pressure to estimate density */
    if (options.PLAPSE) {
      for (rec = 0; rec < global_param.nrecs; rec++) {
        atmos[rec].density[NR] = atmos[rec].pressure[NR]/(Rd*(KELVIN+atmos[rec].air_temp[NR]));
        for (i = 0; i < NF; i++) {
          atmos[rec].density[i] = atmos[rec].pressure[i]/(Rd*(KELVIN+atmos[rec].air_temp[i]));
        }
      }
    }
    else {
      for (rec = 0; rec < global_param.nrecs; rec++) {
        atmos[rec].density[NR] = 0.003486*atmos[rec].pressure[NR]/ (275.0 + atmos[rec].air_temp[NR]);
        for (i = 0; i < NF; i++) {
	  atmos[rec].density[i] = 0.003486*atmos[rec].pressure[i]/ (275.0 + atmos[rec].air_temp[i]);
        }
      }
    }
  }

  /**************************************************************************
    Vapor Pressure, part 2.
  **************************************************************************/

  if(!param_set.TYPE[VP].SUPPLIED) {

    if(param_set.TYPE[QAIR].SUPPLIED) {

      /**************************************************************************
        If we arrive here, it means we couldn't use Qair earlier because
        atmospheric pressure wasn't available at that time.  Now it is
        available, so use Qair and pressure to estimate vp.
      **************************************************************************/

      if(param_set.FORCE_DT == 24) {
        /* daily specific humidity provided */
        for (rec = 0; rec < global_param.nrecs; rec++) {
          sum = 0;
          for (j = 0; j < NF; j++) {
            hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
            if (global_param.starthour - hour_offset_int < 0) hour += 24;
            idx = (int)((float)hour/24.0);
            atmos[rec].vp[j] = local_forcing_data[QAIR][idx] * atmos[rec].pressure[j] / EPS;
            sum += atmos[rec].vp[j];
          }
          if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
        }
      }
      else {
        /* sub-daily specific humidity provided */
        for(rec = 0; rec < global_param.nrecs; rec++) {
          sum = 0;
          for(i = 0; i < NF; i++) {
            hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
            if (global_param.starthour - hour_offset_int < 0) hour += 24;
            atmos[rec].vp[i] = 0;
            for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	      atmos[rec].vp[i] += local_forcing_data[QAIR][idx] * atmos[rec].pressure[j] / EPS;
            }
            atmos[rec].vp[i] /= options.SNOW_STEP;
	    sum += atmos[rec].vp[i];
          }
          if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
        }
      }

      param_set.TYPE[VP].SUPPLIED = param_set.TYPE[QAIR].SUPPLIED;

    } // end if QAIR supplied

    else if(param_set.TYPE[REL_HUMID].SUPPLIED) {

      /**************************************************************************
        If we arrive here, it means we couldn't use RH earlier because
        air temperature wasn't available at that time.  Now it is
        available, so use RH and temperature to estimate vp.
      **************************************************************************/

      if(param_set.FORCE_DT == 24) {
        /* daily specific humidity provided */
        for (rec = 0; rec < global_param.nrecs; rec++) {
          sum = 0;
          for (j = 0; j < NF; j++) {
            hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
            if (global_param.starthour - hour_offset_int < 0) hour += 24;
            idx = (int)((float)hour/24.0);
            atmos[rec].vp[j] = local_forcing_data[REL_HUMID][idx] * svp(atmos[rec].air_temp[j]) / 100;
            sum += atmos[rec].vp[j];
          }
          if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
        }
      }
      else {
        /* sub-daily specific humidity provided */
        for(rec = 0; rec < global_param.nrecs; rec++) {
          sum = 0;
          for(i = 0; i < NF; i++) {
            hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
            if (global_param.starthour - hour_offset_int < 0) hour += 24;
            atmos[rec].vp[i] = 0;
            for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	      atmos[rec].vp[i] += local_forcing_data[REL_HUMID][idx] * svp(atmos[rec].air_temp[j]) / 100;
            }
            atmos[rec].vp[i] /= options.SNOW_STEP;
	    sum += atmos[rec].vp[i];
          }
          if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
        }
      }

      param_set.TYPE[VP].SUPPLIED = param_set.TYPE[REL_HUMID].SUPPLIED;

    } // end if REL_HUMID supplied

  } // end if VP not supplied

  if (!param_set.TYPE[VP].SUPPLIED || param_set.FORCE_DT == 24) {

    /**************************************************
      Either no observations of VP, QAIR, or REL_HUMID were supplied,
      in which case we will use MTCLIM's estimates of daily vapor pressure,
      or daily VP was supplied.
      Now, calculate subdaily vapor pressure
    **************************************************/

    if (options.VP_INTERP) {
      /* Linearly interpolate between daily VP values, assuming they occurred at time of tmin */

      for (day = 0; day < Ndays_local; day++) {
        if (day == 0 && Ndays_local == 1) {
          delta_t_minus = 24;
          delta_t_plus = 24;
        }
        else if (day == 0) {
          delta_t_minus = 24;
          delta_t_plus = tminhour[day+1]+24-tminhour[day];
        }
        else if (day == Ndays_local-1) {
          delta_t_minus = tminhour[day]+24-tminhour[day-1];
          delta_t_plus = 24;
        }
        else {
          delta_t_minus = tminhour[day]+24-tminhour[day-1];
          delta_t_plus = tminhour[day+1]+24-tminhour[day];
        }
        for (hour = 0; hour < 24; hour++) {
          if (hour < tminhour[day]) {
            if (day > 0)
              local_forcing_data[VP][day*24+hour] = daily_vp[day-1] + (daily_vp[day]-daily_vp[day-1])*(hour+24-tminhour[day-1])/delta_t_minus;
            else
              local_forcing_data[VP][day*24+hour] = daily_vp[day];
          }
          else {
            if (day < Ndays_local-1)
              local_forcing_data[VP][day*24+hour] = daily_vp[day] + (daily_vp[day+1]-daily_vp[day])*(hour-tminhour[day])/delta_t_plus;
            else
              local_forcing_data[VP][day*24+hour] = daily_vp[day];
          }
        }
      }

    }
    else {
      /* Hold VP constant throughout day */

      for (day = 0; day < Ndays_local; day++) {
        for (hour = 0; hour < 24; hour++) {
          local_forcing_data[VP][day*24+hour] = daily_vp[day];
        }
      }

    }

    /* Transfer sub-daily VP to atmos array */
    for(rec = 0; rec < global_param.nrecs; rec++) {
      sum = 0;
      for(i = 0; i < NF; i++) {
        hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
        if (global_param.starthour - hour_offset_int < 0) hour += 24;
        atmos[rec].vp[i] = 0;
        for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	  atmos[rec].vp[i] += local_forcing_data[VP][idx];
        }
        atmos[rec].vp[i] /= options.SNOW_STEP;
	sum += atmos[rec].vp[i];
      }
      if(NF>1) atmos[rec].vp[NR] = sum / (float)NF;
    }

  } // end computation of sub-daily VP

  /*************************************************
    Vapor Pressure Deficit
  *************************************************/

  for(rec = 0; rec < global_param.nrecs; rec++) {
    sum = 0;
    sum2 = 0;
    for(i = 0; i < NF; i++) {
      atmos[rec].vpd[i] = svp(atmos[rec].air_temp[i]) - atmos[rec].vp[i];
      if (atmos[rec].vpd[i] < 0) {
        atmos[rec].vpd[i] = 0;
        atmos[rec].vp[i] = svp(atmos[rec].air_temp[i]);
      }
      sum += atmos[rec].vpd[i];
      sum2 += atmos[rec].vp[i];
    }
    if (param_set.TYPE[VP].SUPPLIED || options.VP_INTERP) { // ensure that vp[NR] and vpd[NR] are accurate averages of vp[i] and vpd[i]
      if(NF>1) atmos[rec].vpd[NR] = sum / (float)NF;
      if(NF>1) atmos[rec].vp[NR] = sum2 / (float)NF;
    }
    else { // do not recompute vp[NR]; vpd[NR] is computed relative to vp[NR] and air_temp[NR]
      atmos[rec].vpd[NR] = (svp(atmos[rec].air_temp[NR]) - atmos[rec].vp[NR]);
    }
  }

  /*************************************************
    Cloud Transmissivity (from MTCLIM)
  *************************************************/

  for (rec = 0; rec < global_param.nrecs; rec++) {
    sum = 0;
    for (j = 0; j < NF; j++) {
      hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
      if (global_param.starthour - hour_offset_int < 0) hour += 24;
      idx = (int)((float)hour/24.0);
      atmos[rec].tskc[j] = tskc[idx]; // assume constant over the day
      sum += atmos[rec].tskc[j];
    }
    if(NF>1) atmos[rec].tskc[NR] = sum / (float)NF;
  }

  /*************************************************
    Longwave
  *************************************************/

  /****************************************************************************
    calculate the daily and sub-daily longwave.  There is a separate case for
    the full energy and the water balance modes.  For water balance mode we
    need to calculate the net longwave for the daily timestep and the incoming
    longwave for the SNOW_STEPs, for the full energy balance mode we always
    want the incoming longwave.
  ****************************************************************************/

  if ( !param_set.TYPE[LONGWAVE].SUPPLIED ) {
    /** Incoming longwave radiation not supplied **/
    for (rec = 0; rec < global_param.nrecs; rec++) {
      sum = 0;
      for (i = 0; i < NF; i++) {
	calc_longwave(&(atmos[rec].longwave[i]), atmos[rec].tskc[i],
		      atmos[rec].air_temp[i], atmos[rec].vp[i]);
        sum += atmos[rec].longwave[i];
      }
      if(NF>1) atmos[rec].longwave[NR] = sum / (float)NF;
    }
  }
  else {
    if(param_set.FORCE_DT == 24) {
      /* daily incoming longwave radiation provided */
      for (rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for (j = 0; j < NF; j++) {
          hour = rec*global_param.dt + j*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          idx = (int)((float)hour/24.0);
          atmos[rec].longwave[j] = local_forcing_data[LONGWAVE][idx]; // assume constant over the day
          sum += atmos[rec].longwave[j];
        }
        if(NF>1) atmos[rec].longwave[NR] = sum / (float)NF;
      }
    }
    else {
      /* sub-daily incoming longwave radiation provided */
      for(rec = 0; rec < global_param.nrecs; rec++) {
        sum = 0;
        for(i = 0; i < NF; i++) {
          hour = rec*global_param.dt + i*options.SNOW_STEP + global_param.starthour - hour_offset_int;
          if (global_param.starthour - hour_offset_int < 0) hour += 24;
          atmos[rec].longwave[i] = 0;
          for (idx = hour; idx < hour+options.SNOW_STEP; idx++) {
	    atmos[rec].longwave[i] += local_forcing_data[LONGWAVE][idx];
          }
          atmos[rec].longwave[i] /= options.SNOW_STEP;
	  sum += atmos[rec].longwave[i];
        }
        if(NF>1) atmos[rec].longwave[NR] = sum / (float)NF;
      }
    }
  }

  /****************************************************
    Determine if Snow will Fall During Each Time Step
  ****************************************************/

  // Free temporary parameters
  free(hourlyrad);
  free(prec);
  free(tair);
  free(tmax);
  free(tmaxhour);
  free(tmin);
  free(tminhour);
  free(tskc);
  free(daily_vp);

  free(dmy_local);

  for (type=0; type<N_FORCING_TYPES; type++) {
    if (local_forcing_data[i] != NULL) {
      free(local_forcing_data[type]);
    }
  }
  free(local_forcing_data);

  for(i=0;i<N_FORCING_TYPES;i++)
    if (param_set.TYPE[i].SUPPLIED)
      free(forcing_data[i]);
  free(forcing_data);

}

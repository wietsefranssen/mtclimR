#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>

static char vcid[] = "$Id$";

out_data_file_struct *set_output_defaults(out_data_struct *out_data) {
/*************************************************************
  set_output_defaults.c      Ted Bohn     September 08, 2006

  This routine sets the out_data_files and out_data structures to default values.
  These can be overridden by the user in the global control file.

  Modifications:
  2006-Oct-10 Shortened the names of output variables whose names were
              too long.							TJB
  2007-Oct-09 Updated to reflect variables present in traditional 4.1.0
	      output files.  Previously the defaults matched the traditional
	      4.0.6 output files.					TJB
  2008-Apr-11 Added OUT_SUB_BLOWING, OUT_SUB_SURFACE, and OUT_SUB_SNOW to
	      default snow output file for case of options.BLOWING == TRUE.
	      This makes it almost the same as previous versions of 4.1.0,
	      (r3 and earlier) with the exception that previous versions
	      of 4.1.0 multiplied these terms by 100 when saving to the
	      snow file.						TJB
  2010-Sep-24 Renamed RUNOFF_IN and OUT_RUNOFF_IN to CHANNEL_IN and
	      OUT_LAKE_CHAN_IN, respectively.  Renamed OUT_EVAP_LAKE
	      to OUT_LAKE_EVAP.  Added other lake water balance terms
	      to set of output variables.  Added volumetric versions
	      of these too.						TJB

*************************************************************/

  extern option_struct options;
  out_data_file_struct *out_data_files;
  int v, i;
  int filenum;
  int varnum;


  // Output files
  options.Noutfiles = 1;
  out_data_files = (out_data_file_struct *)calloc(options.Noutfiles,sizeof(out_data_file_struct));
  strcpy(out_data_files->prefix,"full_data");
  out_data_files->nvars = 8;
  out_data_files->varid = (int *)calloc(out_data_files->nvars, sizeof(int));

  // Variables in first file
  filenum = 0;
  varnum = 0;
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_PREC", varnum++, (char*) "%.4f", OUT_TYPE_USINT, 40);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_AIR_TEMP", varnum++, (char*) "%.4f", OUT_TYPE_SINT, 100);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_SHORTWAVE", varnum++, (char*) "%.4f", OUT_TYPE_USINT, 50);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_LONGWAVE", varnum++, (char*) "%.4f", OUT_TYPE_USINT, 80);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_DENSITY", varnum++, (char*) "%.4f", OUT_TYPE_USINT, 100);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_PRESSURE", varnum++, (char*) "%.4f", OUT_TYPE_USINT, 100);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_VP", varnum++, (char*) "%.4f", OUT_TYPE_SINT, 100);
  set_output_var(out_data_files, TRUE, filenum, out_data, (char*) "OUT_WIND", varnum++, (char*) "%.4f", OUT_TYPE_USINT, 100);


  return out_data_files;

}

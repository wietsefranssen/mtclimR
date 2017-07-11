#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>

static char vcid[] = "$Id$";

void parse_output_info_dummy(out_data_file_struct **out_data_files,
                             out_data_struct *out_data)
  /**********************************************************************
   parse_output_info	Ted Bohn	            September 10 2006

   This routine reads the VIC model global control file, getting
   information for output variables list (if any).

   Modifications:
   2006-Nov-07 Changed default precision from %.1f to %.4f.	TJB
   2007-Jan-15 Modified to expect "OUT_TYPE_" at beginning of
   output data type strings.				TJB
   2007-Apr-21 Added initialization for format, outfilenum, and
   outvarnum.					TJB
   2008-Feb-15 Added check on number of output files defined vs.
   N_OUTFILES.					TJB
   2009-Feb-09 Sets PRT_SNOW_BAND to FALSE if N_OUTFILES has been
   specified.					TJB
   2009-Mar-15 Added default values for format, typestr, and
   multstr, so that they can be omitted from global
   param file.					TJB
   **********************************************************************/ {
  extern option_struct options;

  int outvarnum;

  outvarnum = 0;

  // WF!!
  options.Noutfiles = 1;
  *out_data_files = (out_data_file_struct *) calloc(options.Noutfiles, sizeof (out_data_file_struct));

  init_output_list(out_data, FALSE, (char*) "%.4f", OUT_TYPE_FLOAT, 1);
  // PRT_SNOW_BAND is ignored if N_OUTFILES has been specified
  options.PRT_SNOW_BAND = FALSE;
  //
  (*out_data_files)[0].nvars = 6;
  // (*out_data_files)[0].nvars = 13;
  (*out_data_files)[0].varid = (int *) calloc((*out_data_files)[0].nvars, sizeof (int));
  outvarnum = 0;

  if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_PREC", 0, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_AIR_TEMP", 1, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_SHORTWAVE", 2, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_LONGWAVE", 3, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_PRESSURE", 4, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_WIND", 5, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }

  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_PREC", 0, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_RAINF", 1, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_SNOWF", 2, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_SWE", 3, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_AIR_TEMP", 4, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_SHORTWAVE", 5, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_LONGWAVE", 6, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_PRESSURE", 7, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_DENSITY", 8, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_WIND", 9, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_QAIR", 10, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_VP", 11, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  // if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) "OUT_REL_HUMID", 12, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
}

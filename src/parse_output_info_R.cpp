#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"
#include <string.h>

void parse_output_info_R(out_data_file_struct **out_data_files,
                             out_data_struct *out_data,
                             Rcpp::List list) {

  extern option_struct options;

  options.Noutfiles = 1;
  *out_data_files = (out_data_file_struct *) calloc(options.Noutfiles, sizeof (out_data_file_struct));

  init_output_list(out_data, FALSE, (char*) "%.4f", OUT_TYPE_FLOAT, 1);

  /** Find parameters **/
  (*out_data_files)[0].nvars = (int)list["nOut"];
  (*out_data_files)[0].varid = (int *) calloc((*out_data_files)[0].nvars, sizeof (int));

  Rcpp::CharacterVector outNames = list["outNames"];
  for(int i=0;i<(int)list["nOut"];i++) {
    if (set_output_var((*out_data_files), TRUE, 0, out_data, (char*) outNames[i], i, (char*) "*", OUT_TYPE_DEFAULT, 0) != 0) { nrerror((char*) "Error in global param file: Invalid output variable specification."); }
  }

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

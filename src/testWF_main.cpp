#include "Rcpp.h"        // R memory io
#include "Rdefines.h"        // R memory io
#include "Rmath.h"    // R math functions
#include "includeWF.h"
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
int mtclimRun(int x, List forcing_dataR) {
  // printf("dddd: %f\n", forcing[0]);
  // printf("dddd: %f\n", forcing[1]);
  // printf("dddd: %f\n", forcing[(24*11688)-1]);
  // Rcpp::List xlist(forcing_dataR);
  // int n = forcing_dataR.size();

  // float iii;
  // NumericVector resid = as<NumericVector>(forcing_dataR[1]);
  // for(int i=0;i<11688;i++) {
  //   iii = resid[1];
  // }
  // float resid2 = as<float>(forcing_dataR[1]);

  // printf("forcing_dataR: %f\n", resid[2]);
  // printf("forcing_dataR: %f\n", iii);
  // printf("forcing_dataR: %f\n", res[0][0]);
  // printf("forcing_dataR: %f\n", forcing_dataR[1]);
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
  global_param = get_global_param_dummy();

  /** Set up output data structures **/
  out_data = create_output_list();
  out_data_files = set_output_defaults(out_data);

  parse_output_info_dummy(&out_data_files, out_data);


  /** Make Date Data Structure **/
  dmy = make_dmy(&global_param);

  /** allocate memory for the atmos_data_struct **/
  alloc_atmos(global_param.nrecs, &atmos);

  /************************************
  Run Model for all Active Grid Cells
  ************************************/
  cell_cnt = 0;

  soil_con.time_zone_lng = -30;
  soil_con.lng = -39.25;
  soil_con.lat = -8.25;
  soil_con.elevation = 434;
  soil_con.slope = 0;
  soil_con.aspect = 0;
  soil_con.ehoriz = 0;
  soil_con.whoriz = 0;
  soil_con.annual_prec = 1;
  soil_con.cell_area = 8.0218315155553431e-314;

  /*******************************
   read in meteorological data
   *******************************/
  double **forcing_data;
  /** Allocate data arrays for input forcing data **/
  forcing_data = (double **)calloc(N_FORCING_TYPES,sizeof(double*));
  printf("N_FORCING_TYPES: %d %d %d\n", N_FORCING_TYPES, global_param.nrecs, NF);
  for(int i=0;i<N_FORCING_TYPES;i++) {
    if (param_set.TYPE[i].SUPPLIED) {
      forcing_data[i] = (double *)calloc((global_param.nrecs * NF),
                         sizeof(double));
    }
  }

  for(int i=0;i<N_FORCING_TYPES;i++) {
    if (param_set.TYPE[i].SUPPLIED) {
      NumericVector resid = as<NumericVector>(forcing_dataR[i]);
      for(int t=0;t<(global_param.nrecs * NF);t++) {
        forcing_data[i][t] = resid[t];
      }
    }
  }
  // forcing_data = read_forcing_data_dummy(global_param, forcing_data);
  // read_atmos_data_dummy(global_param, forcing_data);
  fprintf(stderr, "\nRead meteorological forcing file\n");

  /**************************************************
   Initialize Meteological Forcing Values That
   Have not Been Specifically Set
   **************************************************/


  printf("initialize atmos: start\n");
  initialize_atmos(atmos, dmy, forcing_data,
                   &soil_con, out_data_files, out_data);
  printf("initialize atmos: end\n");

  // takes a numeric input and doubles it
  return 2 * x;
}

extern "C" {
  void testWFR() {
    testWF();
  }
}



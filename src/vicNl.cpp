#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vicNl.h"

/********************************************************************/
/*			GLOBAL VARIABLES                            */
/********************************************************************/
// global_param_struct global_param;
// option_struct options;
// Error_struct Error;
// param_set_struct param_set;
// int NR; /* array index for atmos struct that indicates
// 			 the model step avarage or sum */
// int NF; /* array index loop counter limit for atmos
// 			 struct that indicates the SNOW_STEP values */

/** Main Program **/
int main(int argc, char *argv[]) {

    extern global_param_struct global_param;

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

    forcing_data = read_forcing_data_dummy(global_param, forcing_data);
    fprintf(stderr, "\nRead meteorological forcing file\n");

    /**************************************************
       Initialize Meteological Forcing Values That
       Have not Been Specifically Set
     **************************************************/


    printf("initialize atmos: start\n");
    initialize_atmos(atmos, dmy, forcing_data,
            &soil_con, out_data_files, out_data);
    printf("initialize atmos: end\n");


    /** cleanup **/
    free_atmos(global_param.nrecs, &atmos);
    free_dmy(&dmy);
    free_out_data_files(&out_data_files);
    free_out_data(&out_data);

    return EXIT_SUCCESS;
} /* End Main Program */

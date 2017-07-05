#include <math.h>
#include "vicNl_def.h"
#include "Rcpp.h"        // R memory io
#include "Rdefines.h"        // R memory io

/*** SubRoutine Prototypes ***/

void alloc_atmos(int, atmos_data_struct **);
int   CalcAerodynamic(char, double, double, double, double, double,
	  	       double *, double *, double *, double *, double *);
void   calc_cloud_cover_fraction(atmos_data_struct *, dmy_struct *, int,
				 int, int, double *);
double calc_energy_balance_error(int, double, double, double, double, double);
#if OUTPUT_FORCE_STATS
void   calc_forcing_stats(int, atmos_data_struct *);
#endif // OUTPUT_FORCE_STATS
void   calc_longwave(double *, double, double, double);
void   calc_netlongwave(double *, double, double, double);
double calc_netshort(double, int, double, double *);
double calc_rainonly(double,double,double,double,double);
double calc_rc(double,double,float,double,double,double,double,char);
double CalcSnowPackEnergyBalance(double Tsurf, ...);
double CalcBlowingSnow(double, double, int, double, double, double, double,
                       double, double, double, double, double, float,
                       float, double, int, int, float, double, double, double *);
double calc_atmos_energy_bal(double, double, double, double, double, double,
                             double, double, double, double, double, double,
                             double, double, double, double,
                             double *, double *, double *, double *,
                             double *, double *, double *, double *, char *, int *);
double calc_trans(double, double);
double calc_veg_displacement(double);
double calc_veg_height(double);
double calc_veg_roughness(double);
double calc_water_balance_error(int, double, double, double);
FILE  *check_state_file(char *, dmy_struct *, global_param_struct *, int, int,
                        int *);
filenames_struct cmd_proc(int argc, char *argv[]);
void   collect_eb_terms(energy_bal_struct, snow_data_struct, cell_data_struct,
                        int *, int *, int *, int *, int *, double, double, double,
                        int, int, double, int, int, double *, double *,
#if SPATIAL_FROST
                        double *, double,
#endif
                        out_data_struct *);
void   collect_wb_terms(cell_data_struct, veg_var_struct, snow_data_struct, lake_var_struct,
                        double, double, double, double, int, int, double, int, double *,
#if SPATIAL_FROST
                        double *,
#endif
                        out_data_struct *);
void   compress_files(char string[]);
void   compute_dz(double *, double *, int, double);
void   correct_precip(double *, double, double, double, double);
void   compute_pot_evap(int, dmy_struct *, int, int, double, double , double, double, double, double **, double *);
void   compute_runoff_and_asat(soil_con_struct *, double *, double, double *, double *);
void   compute_treeline(atmos_data_struct *, dmy_struct *, double, double *, char *);
double compute_zwt(soil_con_struct *, int, double);
out_data_struct *create_output_list();

void   display_current_settings(int, filenames_struct *, global_param_struct *);
int    dist_prec(atmos_data_struct *,dist_prcp_struct *,soil_con_struct *, lake_con_struct *,
        out_data_file_struct *,
		 out_data_struct *, save_data_struct *,
		 int, int, char, char, char *, int *);
#if QUICK_FS
int  distribute_node_moisture_properties(double *, double *, double *, double *,
					 double *, double *, double *, double ***,
					 double *, double *, double *, double *, double *,
					 double *, double *, double *, int, int, char);
#else
#if EXCESS_ICE
int  distribute_node_moisture_properties(double *, double *, double *, double *,
					 double *, double *, double *, double *,
					 double *, double *, double *,
					 double *, double *, double *, double *, double *,
					 double *, double *, double *, int, int, char);
#else
int  distribute_node_moisture_properties(double *, double *, double *,
					 double *, double *, double *,
					 double *, double *, double *,
					 double *, double *, double *, double *, double *,
					 double *, double *, double *, int, int, char);
#endif
#endif
void   distribute_soil_property(double *,double,double,
				double **l_param,
				int, int, double *, double *);

double error_calc_atmos_energy_bal(double Tcanopy, ...);
double error_calc_atmos_moist_bal(double , ...);
double error_calc_canopy_energy_bal(double Tsurf, ...);
double error_calc_snow_ground_flux(double Tsurf, ...);
double error_calc_surf_energy_bal(double Tsurf, ...);
double ErrorSnowPackEnergyBalance(double Tsurf, ...);
double error_print_atmos_energy_bal(double, va_list);
double error_print_atmos_moist_bal(double, va_list);
double error_print_canopy_energy_bal(double, va_list);
double error_print_snow_ground_flux(double, va_list);
double ErrorPrintSnowPackEnergyBalance(double, va_list);
double error_print_solve_T_profile(double, va_list);
double error_print_surf_energy_bal(double, va_list);
double error_solve_T_profile(double Tsurf, ...);
double f(double, double, double, double, double, double, double, double,
         double, double, int, double *, double, double, double, double *,
         double *, double *, double *, double *, double *);
void   fda_heat_eqn(double *, double *, int, int, ...);
void   fdjac3(double *, double *, double *, double *, double *,
            void (*vecfunc)(double *, double *, int, int, ...),
            int);
void   find_0_degree_fronts(energy_bal_struct *, double *, double *, int);
void   free_atmos(int nrecs, atmos_data_struct **atmos);
void   free_dist_prcp(dist_prcp_struct *, int);
void   free_dmy(dmy_struct **dmy);
void   free_out_data_files(out_data_file_struct **);
void   free_out_data(out_data_struct **);
double func_aero_resist(double,double,double,double,double);
double func_atmos_energy_bal(double, va_list);
double func_atmos_moist_bal(double, va_list);
double func_canopy_energy_bal(double, va_list);
double func_snow_ground_flux(double, va_list);
double func_surf_energy_bal(double, va_list);

double get_avg_temp(double, double, double *, double *, int);
double get_dist(double, double, double, double);
void   get_force_type(char *, int, int *);
void   get_force_type_dummy(int, int *);
global_param_struct get_global_param(filenames_struct *, FILE *);
global_param_struct get_global_param_dummy();
global_param_struct get_global_param_R(Rcpp::List list);
void   get_next_time_step(int *, int *, int *, int *, int *, int);

double hermint(double, int, double *, double *, double *, double *, double *);
void   hermite(int, double *, double *, double *, double *, double *);
void   HourlyT(int, int, int *, double *, int *, double *, double *);

void   init_output_list(out_data_struct *, int, char *, int, float);
void   initialize_atmos(atmos_data_struct *, dmy_struct *, double **,
			soil_con_struct *, out_data_file_struct *, out_data_struct *);
void   initialize_global();
int    initialize_new_storm(cell_data_struct ***, veg_var_struct ***,
			    int, int, int, double, double);


void   latent_heat_from_snow(double, double, double, double, double,
                             double, double, double *, double *,
                             double *, double *, double *);
double linear_interp(double,double,double,double,double);

cell_data_struct **make_cell_data(int, int);
dist_prcp_struct make_dist_prcp(int);
dmy_struct *make_dmy(global_param_struct *);
energy_bal_struct **make_energy_bal(int);
out_data_struct *make_out_data(int);
snow_data_struct **make_snow_data(int);
veg_var_struct **make_veg_var(int);
void   MassRelease(double *,double *,double *,double *);
#if EXCESS_ICE
double maximum_unfrozen_water(double, double, double, double, double, double);
#else
double maximum_unfrozen_water(double, double, double, double);
#endif
#if QUICK_FS
double maximum_unfrozen_water_quick(double, double, double **);
#endif
double modify_Ksat(double);
void mtclim_wrapper(int, int, double, double, double, double,
                      double, double, double, double,
                      int, dmy_struct *, double *,
                      double *, double *, double *, double *, double *);

double new_snow_density(double);
int    newt_raph(void (*vecfunc)(double *, double *, int, int, ...),
               double *, int);
void   nrerror(char *);

FILE  *open_file(char string[], char type[]);
FILE  *open_state_file(global_param_struct *, filenames_struct, int, int);

void parse_output_info(filenames_struct *, FILE *, out_data_file_struct **, out_data_struct *);
void parse_output_info_dummy(out_data_file_struct **, out_data_struct *);
double penman(double, double, double, double, double, double, double);
double priestley(double, double);
int    put_data(dist_prcp_struct *, atmos_data_struct *,
		soil_con_struct *, out_data_file_struct *,
		out_data_struct *, save_data_struct *,
 	        dmy_struct *, int);

double read_arcinfo_value(char *, double, double);
int    read_arcinfo_info(char *, double **, double **, int **);
void   read_atmos_data(FILE *, global_param_struct, int, int, double **);
double **read_forcing_data_dummy(global_param_struct, double **);
void   read_atmos_data_dummy(global_param_struct, double **);

int    redistribute_during_storm(cell_data_struct ***, veg_var_struct ***,
				 int, int, int, double, double, double,
				 double *);
void set_max_min_hour(double *, int, int *, int *);
void set_node_parameters(double *, double *, double *, double *, double *, double *,
			 double *, double *, double *, double *, double *,
			 double *, double *,
#if QUICK_FS
			 double ***,
#endif
#if EXCESS_ICE
			 double *, double *, double *, double *,
#endif
			 int, int, char);
out_data_file_struct *set_output_defaults(out_data_struct *);
int set_output_var(out_data_file_struct *, int, int, out_data_struct *, char *, int, char *, int, float);
double snow_albedo(double, double, double, double, double, double, int, char);
double snow_density(snow_data_struct *, double, double, double, double, double);
double solve_atmos_energy_bal(double Tcanopy, ...);
double solve_atmos_moist_bal(double , ...);
double solve_canopy_energy_bal(double Tfoliage, ...);
double solve_snow_ground_flux(double Tsurf, ...);
double solve_surf_energy_bal(double Tsurf, ...);
#if QUICK_FS
int    solve_T_profile(double *, double *, char *, int *, double *, double *,double *,
		       double *, double, double *, double *, double *,
		       double *, double *, double *, double *, double, double *, double ***,
		       int, int *, int, int, int, int);
#else
int    solve_T_profile(double *, double *, char *, int *, double *, double *,double *,
		       double *, double, double *, double *, double *,
		       double *, double *, double *, double *, double, double *,
#if EXCESS_ICE
		       double *, double *,
#endif
		       int, int *, int, int, int, int);

#endif
int   solve_T_profile_implicit(double *, double *, double *, double *, double *,
			       double *, double, double *, double *, double *,
#if EXCESS_ICE
			       double *, double *,
#endif
			       double *, double *, double *, double *, double, int, int *,
			       int, int, int, int,
			       double *, double *, double *, double *, double *, double *, double *);
double StabilityCorrection(double, double, double, double, double, double);
double svp(double);
double svp_slope(double);

void tridag(double *,double *,double *,double *,double *,int);
void tridiag(double *, double *, double *, double *, unsigned);

void usage(char *);

void   vicerror(const char *);
double volumetric_heat_capacity(double,double,double,double);

void write_data(out_data_file_struct *, out_data_struct *, dmy_struct *, int);
void write_data_toScreen(out_data_file_struct *, out_data_struct *, dmy_struct *, int);
void write_dist_prcp(dist_prcp_struct *);
#if OUTPUT_FORCE
void write_forcing_file(atmos_data_struct *, int, out_data_file_struct *, out_data_struct *);
void write_forcing_toScreen(atmos_data_struct *, int, out_data_file_struct *, out_data_struct *);
#endif
void write_header(out_data_file_struct *, out_data_struct *, dmy_struct *, global_param_struct);
void write_snow_data(snow_data_struct, int, int);

void write_vegvar(veg_var_struct *, int);

void zero_output_list(out_data_struct *);

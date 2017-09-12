#include "user_def.h"

/***** Model Constants *****/
#define MAXSTRING    2048
#define MINSTRING    20
#define HUGE_RESIST  1.e20	/* largest allowable double number */
#define SPVAL        1.e20	/* largest allowable double number - used to signify missing data */
#define SMALL        1.e-12	/* smallest allowable double number */
#define MISS      -99999.	/* missing value for multipliers in BINARY format */
#define LITTLE 1		/* little-endian flag */
#define BIG 2			/* big-endian flag */

/***** Met file formats *****/
#define ASCII 1
#define BINARY 2

/***** Snow Albedo parametrizations *****/
#define USACE   0
#define SUN1999 1

/***** Snow Density parametrizations *****/
#define DENS_BRAS   0
#define DENS_SNTHRM 1

/***** Baseflow parametrizations *****/
#define ARNO        0
#define NIJSSEN2001 1

/***** Aerodynamic Resistance options *****/
#define AR_406      0
#define AR_406_LS   1
#define AR_406_FULL 2
#define AR_410      3

/***** Ground Flux options *****/
#define GF_406  0
#define GF_410  1

/***** VP algorithm options *****/
#define VP_ITER_NONE     0
#define VP_ITER_ALWAYS   1
#define VP_ITER_ANNUAL   2
#define VP_ITER_CONVERGE 3

/***** Longwave Clear-Sky Algorithm options *****/
#define LW_TVA        0
#define LW_ANDERSON   1
#define LW_BRUTSAERT  2
#define LW_SATTERLUND 3
#define LW_IDSO       4
#define LW_PRATA      5

/***** Longwave Cloud Algorithm options *****/
#define LW_CLOUD_BRAS       0
#define LW_CLOUD_DEARDORFF  1

/***** Potential Evap types *****/
#define N_PET_TYPES 6
#define N_PET_TYPES_NON_NAT 4
#define PET_SATSOIL 0
#define PET_H2OSURF 1
#define PET_SHORT   2
#define PET_TALL    3
#define N_PET_TYPES_NAT 2
#define PET_NATVEG  4
#define PET_VEGNOCR 5

/***** LAI source types *****/
#define LAI_FROM_VEGLIB     0
#define LAI_FROM_VEGPARAM   1

/***** Hard-coded veg class parameters (mainly for pot_evap) *****/
#define BARE_SOIL_ALBEDO 0.2	    /* albedo for bare soil */
#define H2O_SURF_ALBEDO 0.08	    /* albedo for deep water surface */

/***** Time Constants *****/
#define DAYS_PER_YEAR 365.
#define HOURSPERDAY   24        /* number of hours per day */
#define HOURSPERYEAR  24*365    /* number of hours per year */
#define SECPHOUR     3600	/* seconds per hour */
#define SEC_PER_DAY 86400.	/* seconds per day */

/***** Physical Constants *****/
#define RESID_MOIST      0.0        /* define residual moisture content
				       of soil column */
#define MAX_ICE_INIT      0.95        /* define maximum volumetric ice fraction
				       of soil column, for EXCESS_ICE option */
#define ICE_AT_SUBSIDENCE 0.8        /* minimum ice/porosity fraction before
					subsidence occurs, for EXCESS_ICE option */
#define MAX_SUBSIDENCE    1.0        /* maximum depth of subsidence per layer per
					time-step (mm) */
#define ice_density      917.	    /* density of ice (kg/m^3) */
#define T_lapse          6.5        /* temperature lapse rate of US Std
				       Atmos in C/km */
#define von_K        0.40	/* Von Karman constant for evapotranspiration */
#define KELVIN       273.15	/* conversion factor C to K */
#define STEFAN_B     5.6696e-8	/* stefan-boltzmann const in unit W/m^2/K^4 */
#define Lf           3.337e5	/* Latent heat of freezing (J/kg) at 0C */
#define RHO_W        999.842594	/* Density of water (kg/m^3) at 0C */
#define Cp           1013.0	/* Specific heat at constant pressure of moist air
				   (J/deg/K) (H.B.H. p.4.13)*/
#define CH_ICE       2100.0e3	/* Volumetric heat capacity (J/(m3*C)) of ice */
#define CH_WATER     4186.8e3   /* volumetric heat capacity of water */
#define K_SNOW       2.9302e-6  /* conductivity of snow (W/mK) */
#define SOLAR_CONSTANT 1400.0	/* Solar constant in W/m^2 */
#define EPS          0.62196351 /* Ratio of molecular weights: M_water_vapor/M_dry_air */
#define G            9.81       /* gravity */
#define Rd           287        /* Gas constant of dry air (J/degC*kg) */
#define JOULESPCAL   4.1868     /* Joules per calorie */
#define GRAMSPKG     1000.      /* convert grams to kilograms */
#define kPa2Pa 1000.            /* converts kPa to Pa */
#define DtoR 0.017453293	/* degrees to radians */
#ifndef PII
#define PII 3.1415927
#endif

/* define constants for saturated vapor pressure curve (kPa) */
#define A_SVP 0.61078
#define B_SVP 17.269
#define C_SVP 237.3

/* define constants for penman evaporation */
#define CP_PM 1013		/* specific heat of moist air at constant pressure (J/kg/C)
				   (Handbook of Hydrology) */
#define PS_PM 101300		/* sea level air pressure in Pa */
#define LAPSE_PM -0.006		/* environmental lapse rate in C/m */

/***** Physical Constraints *****/
#define MINSOILDEPTH 0.001	/* minimum layer depth with which model can
					work (m) */
#define STORM_THRES  0.001      /* thresehold at which a new storm is
				   decalred */
#define SNOW_DT       5.0	/* Used to bracket snow surface temperatures
				   while computing the snow surface energy
				   balance (C) */
#define SURF_DT       1.0	/* Used to bracket soil surface temperatures
                                   while computing energy balance (C) */
#define SOIL_DT       0.25      /* Used to bracket soil temperatures while
                                   solving the soil thermal flux (C) */
#define CANOPY_DT    1.0	/* Used to bracket canopy air temperatures
                                   while computing energy balance (C) */
#define CANOPY_VP    25.0	/* Used to bracket canopy vapor pressures
                                   while computing moisture balance (Pa) */
#define DEFAULT_WIND_SPEED 3.0  /* Default wind speed [m/s] used when wind is not supplied as a forcing */

/***** Define Boolean Values *****/
#ifndef FALSE
#define FALSE 0
#define TRUE !FALSE
#endif

#ifndef WET
#define WET 0
#define DRY 1
#endif

#ifndef SNOW
#define RAIN 0
#define SNOW 1
#endif

//#define min(a,b) (a < b) ? a : b
//#define max(a,b) (a > b) ? a : b


/***** Forcing Variable Types *****/
#define N_FORCING_TYPES 24
#define AIR_TEMP   0 /* air temperature per time step [C] (ALMA_INPUT: [K]) */
#define ALBEDO     1 /* surface albedo [fraction] */
#define CHANNEL_IN 2 /* incoming channel flow [m3] (ALMA_INPUT: [m3/s]) */
#define CRAINF     3 /* convective rainfall [mm] (ALMA_INPUT: [mm/s]) */
#define CSNOWF     4 /* convective snowfall [mm] (ALMA_INPUT: [mm/s]) */
#define DENSITY    5 /* atmospheric density [kg/m3] */
#define LONGWAVE   6 /* incoming longwave radiation [W/m2] */
#define LSRAINF    7 /* large-scale rainfall [mm] (ALMA_INPUT: [mm/s]) */
#define LSSNOWF    8 /* large-scale snowfall [mm] (ALMA_INPUT: [mm/s]) */
#define PREC       9 /* total precipitation (rain and snow) [mm] (ALMA_INPUT: [mm/s]) */
#define PRESSURE  10 /* atmospheric pressure [kPa] (ALMA_INPUT: [Pa]) */
#define QAIR      11 /* specific humidity [kg/kg] */
#define RAINF     12 /* rainfall (convective and large-scale) [mm] (ALMA_INPUT: [mm/s]) */
#define REL_HUMID 13 /* relative humidity [fraction] */
#define SHORTWAVE 14 /* incoming shortwave [W/m2] */
#define SNOWF     15 /* snowfall (convective and large-scale) [mm] (ALMA_INPUT: [mm/s]) */
#define TMAX      16 /* maximum daily temperature [C] (ALMA_INPUT: [K]) */
#define TMIN      17 /* minimum daily temperature [C] (ALMA_INPUT: [K]) */
#define TSKC      18 /* cloud cover fraction [fraction] */
#define VP        19 /* vapor pressure [kPa] (ALMA_INPUT: [Pa]) */
#define WIND      20 /* wind speed [m/s] */
#define WIND_E    21 /* zonal component of wind speed [m/s] */
#define WIND_N    22 /* meridional component of wind speed [m/s] */
#define SKIP      23 /* place holder for unused data columns */

/***** Output Variable Types *****/
#define N_OUTVAR_TYPES 160
// Water Balance Terms - state variables
#define OUT_ASAT             0  /* Saturated Area Fraction */
#define OUT_LAKE_AREA_FRAC   1  /* lake surface area as fraction of the grid cell area [fraction] */
#define OUT_LAKE_DEPTH       2  /* lake depth (distance between surface and deepest point) [m] */
#define OUT_LAKE_ICE         3  /* moisture stored as lake ice [mm over lake ice area] */
#define OUT_LAKE_ICE_FRACT   4  /* fractional coverage of lake ice [fraction] */
#define OUT_LAKE_ICE_HEIGHT  5  /* thickness of lake ice [cm] */
#define OUT_LAKE_MOIST       6  /* liquid water and ice stored in lake [mm over grid cell] */
#define OUT_LAKE_SURF_AREA   7  /* lake surface area [m2] */
#define OUT_LAKE_SWE         8  /* liquid water equivalent of snow on top of lake ice [m over lake ice area] */
#define OUT_LAKE_SWE_V       9  /* volumetric liquid water equivalent of snow on top of lake ice [m3] */
#define OUT_LAKE_VOLUME     10  /* lake volume [m3] */
#define OUT_ROOTMOIST       11  /* root zone soil moisture  [mm] */
#define OUT_SMFROZFRAC      12  /* fraction of soil moisture (by mass) that is ice, for each soil layer */
#define OUT_SMLIQFRAC       13  /* fraction of soil moisture (by mass) that is liquid, for each soil layer */
#define OUT_SNOW_CANOPY     14  /* snow interception storage in canopy  [mm] */
#define OUT_SNOW_COVER      15  /* fractional area of snow cover [fraction] */
#define OUT_SNOW_DEPTH      16  /* depth of snow pack [cm] */
#define OUT_SOIL_ICE        17  /* soil ice content  [mm] for each soil layer */
#define OUT_SOIL_LIQ        18  /* soil liquid content  [mm] for each soil layer */
#define OUT_SOIL_MOIST      19  /* soil total moisture content  [mm] for each soil layer */
#define OUT_SOIL_WET        20  /* vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
#define OUT_SURFSTOR        21  /* storage of liquid water and ice (not snow) on surface (ponding) [mm] */
#define OUT_SURF_FROST_FRAC 22  /* fraction of soil surface that is frozen [fraction] */
#define OUT_SWE             23  /* snow water equivalent in snow pack (including vegetation-intercepted snow)  [mm] */
#define OUT_WDEW            24  /* total moisture interception storage in canopy [mm] */
#define OUT_ZWT             25  /* water table position [cm] (zwt within lowest unsaturated layer) */
#define OUT_ZWT_LUMPED      26  /* lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */
// Water Balance Terms - fluxes
#define OUT_BASEFLOW        27  /* baseflow out of the bottom layer  [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_DELINTERCEPT    28  /* change in canopy interception storage  [mm] */
#define OUT_DELSOILMOIST    29  /* change in soil water content  [mm] */
#define OUT_DELSURFSTOR     30  /* change in surface liquid water storage  [mm] */
#define OUT_DELSWE          31  /* change in snow water equivalent  [mm] */
#define OUT_EVAP            32  /* total net evaporation [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_EVAP_BARE       33  /* net evaporation from bare soil [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_EVAP_CANOP      34  /* net evaporation from canopy interception [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_INFLOW          35  /* moisture that reaches top of soil column [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_PREC            63  /* incoming precipitation [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_RAINF           64  /* rainfall  [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_REFREEZE        65  /* refreezing of water in the snow  [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_RUNOFF          66  /* surface runoff [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_SNOW_MELT       67  /* snow melt  [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_SNOWF           68  /* snowfall  [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_SUB_BLOWING     69  /* net sublimation of blowing snow [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_SUB_CANOP       70  /* net sublimation from snow stored in canopy [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_SUB_SNOW        71  /* total net sublimation from snow pack (surface and blowing) [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_SUB_SURFACE     72  /* net sublimation from snow pack surface [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_TRANSP_VEG      73  /* net transpiration from vegetation [mm] (ALMA_OUTPUT: [mm/s]) */
#define OUT_WATER_ERROR     74  /* water budget error [mm] */
// Energy Balance Terms - state variables
#define OUT_ALBEDO          75  /* average surface albedo [fraction] */
#define OUT_BARESOILT       76  /* bare soil surface temperature [C] (ALMA_OUTPUT: [K]) */
#define OUT_FDEPTH          77  /* depth of freezing fronts [cm] (ALMA_OUTPUT: [m]) for each freezing front */
#define OUT_LAKE_ICE_TEMP   78  /* temperature of lake ice [C] (ALMA_OUTPUT: [K]) */
#define OUT_LAKE_SURF_TEMP  79  /* lake surface temperature [C] (ALMA_OUTPUT: [K]) */
#define OUT_RAD_TEMP        80  /* average radiative surface temperature [K] */
#define OUT_SALBEDO         81  /* snow pack albedo [fraction] */
#define OUT_SNOW_PACK_TEMP  82  /* snow pack temperature [C] (ALMA_OUTPUT: [K]) */
#define OUT_SNOW_SURF_TEMP  83  /* snow surface temperature [C] (ALMA_OUTPUT: [K]) */
#define OUT_SNOWT_FBFLAG    84  /* snow surface temperature fallback flag */
#define OUT_SOIL_TEMP       85  /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil layer */
#define OUT_SOIL_TNODE      86  /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil thermal node */
#define OUT_SOIL_TNODE_WL   87  /* soil temperature [C] (ALMA_OUTPUT: [K]) for each soil thermal node in the wetland */
#define OUT_SOILT_FBFLAG    88  /* soil temperature flag for each soil thermal node */
#define OUT_SURF_TEMP       89  /* average surface temperature [C] (ALMA_OUTPUT: [K]) */
#define OUT_SURFT_FBFLAG    90  /* surface temperature flag */
#define OUT_TCAN_FBFLAG     91  /* Tcanopy flag */
#define OUT_TDEPTH          92  /* depth of thawing fronts [cm] (ALMA_OUTPUT: [m]) for each thawing front */
#define OUT_TFOL_FBFLAG     93  /* Tfoliage flag */
#define OUT_VEGT            94  /* average vegetation canopy temperature [C] (ALMA_OUTPUT: [K]) */
// Miscellaneous Terms
#define OUT_AIR_TEMP       118  /* air temperature [C] (ALMA_OUTPUT: [K])*/
#define OUT_DENSITY        119  /* near-surface atmospheric density [kg/m3]*/
#define OUT_LONGWAVE       120  /* incoming longwave [W/m2] */
#define OUT_PRESSURE       121  /* near surface atmospheric pressure [kPa] (ALMA_OUTPUT: [Pa])*/
#define OUT_QAIR           122  /* specific humidity [kg/kg] */
#define OUT_REL_HUMID      123  /* relative humidity [fraction]*/
#define OUT_SHORTWAVE      124  /* incoming shortwave [W/m2] */
#define OUT_SURF_COND      125  /* surface conductance [m/s] */
#define OUT_TSKC           126  /* cloud cover fraction [fraction] */
#define OUT_VP             127  /* near surface vapor pressure [kPa] (ALMA_OUTPUT: [Pa]) */
#define OUT_VPD            128  /* near surface vapor pressure deficit [kPa] (ALMA_OUTPUT: [Pa]) */
#define OUT_WIND           129  /* near surface wind speed [m/s] */

/***** Output BINARY format types *****/
#define OUT_TYPE_DEFAULT 0 /* Default data type */
#define OUT_TYPE_CHAR    1 /* char */
#define OUT_TYPE_SINT    2 /* short int */
#define OUT_TYPE_USINT   3 /* unsigned short int */
#define OUT_TYPE_INT     4 /* int */
#define OUT_TYPE_FLOAT   5 /* single-precision floating point */
#define OUT_TYPE_DOUBLE  6 /* double-precision floating point */

/***** Output aggregation method types *****/
#define AGG_TYPE_AVG     0 /* average over agg interval */
#define AGG_TYPE_BEG     1 /* value at beginning of agg interval */
#define AGG_TYPE_END     2 /* value at end of agg interval */
#define AGG_TYPE_MAX     3 /* maximum value over agg interval */
#define AGG_TYPE_MIN     4 /* minimum value over agg interval */
#define AGG_TYPE_SUM     5 /* sum over agg interval */

/***** Codes for displaying version information *****/
#define DISP_VERSION 1
#define DISP_COMPILE_TIME 2
#define DISP_ALL 3


/***** VIC model version *****/
extern char *version;

/* global variables */
extern int NR;			/* array index for atmos struct that indicates
				   the model step avarage or sum */
extern int NF;			/* array index loop counter limit for atmos
				   struct that indicates the SNOW_STEP values */

/***** Data Structures *****/

typedef struct {

  // simulation modes
  int    AboveTreelineVeg; /* Default veg type to use above treeline;
			      Negative number indicates bare soil. */
  char   AERO_RESIST_CANSNOW; /* "AR_406" = multiply aerodynamic resistance
					    by 10 for latent heat but not
					    for sensible heat (as in
					    VIC 4.0.6); do NOT apply stability
					    correction; use surface aero_resist
					    for ET when no snow in canopy.
				 "AR_406_LS" = multiply aerodynamic resistance
					    by 10 for BOTH latent heat AND
					    sensible heat; do NOT apply
					    stability correction;
					    use surface aero_resist
					    for ET when no snow in canopy.
				 "AR_406_FULL" = multiply aerodynamic resistance
					    by 10 for BOTH latent heat AND
					    sensible heat; do NOT apply
					    stability correction;
					    always use canopy aero_resist
					    for ET.
				 "AR_410" = do not multiply aerodynamic
					    resistance by 10 in snow-filled
					    canopy (as in VIC 4.1.0);
					    DO apply stability correction;
					    always use canopy aero_resist
					    for ET. */
  char   BLOWING;        /* TRUE = calculate sublimation from blowing snow */
  char   COMPUTE_TREELINE; /* TRUE = Determine treeline and exclude overstory
			      vegetation from higher elevations */
  char   CONTINUEONERROR;/* TRUE = VIC will continue to run after a cell has an error */
  char   CORRPREC;       /* TRUE = correct precipitation for gage undercatch */
  char   DIST_PRCP;      /* TRUE = Use distributed precipitation model */
  char   EQUAL_AREA;     /* TRUE = RESOLUTION stores grid cell area in km^2;
			    FALSE = RESOLUTION stores grid cell side length in degrees */
  char   EXP_TRANS;      /* TRUE = Uses grid transform for exponential node
			    distribution for soil heat flux calculations*/
  char   FROZEN_SOIL;    /* TRUE = Use frozen soils code */
  char   FULL_ENERGY;    /* TRUE = Use full energy code */
  char   GRND_FLUX_TYPE; /* "GF_406"  = use (flawed) formulas for ground flux, deltaH, and fusion
                                        from VIC 4.0.6 and earlier
                            "GF_410"  = use formulas from VIC 4.1.0 */
  char   IMPLICIT;       /* TRUE = Use implicit solution when computing
			    soil thermal fluxes */
  char   JULY_TAVG_SUPPLIED; /* If TRUE and COMPUTE_TREELINE is also true,
			        then average July air temperature will be read
			        from soil file and used in calculating treeline */
  char   LAKES;          /* TRUE = use lake energy code */
  char   LW_CLOUD;       /* Longwave cloud formulation; "LW_CLOUD_x" = code for LW cloud formulation - see LW_CLOUD codes above */
  char   LW_TYPE;        /* Longwave clear sky algorithm; "LW_x" = code for LW algorithm - see LW codes above */
  float  MIN_WIND_SPEED; /* Minimum wind speed in m/s that can be used by the model. **/
  char   MTCLIM_SWE_CORR;/* TRUE = correct MTCLIM's downward shortwave radiation estimate in presence of snow */
  int    Nlayer;         /* Number of layers in model */
  int    Nnode;          /* Number of soil thermal nodes in the model */
  char   NOFLUX;         /* TRUE = Use no flux lower bondary when computing
			    soil thermal fluxes */
  char   PLAPSE;         /* TRUE = If air pressure not supplied as an
			    input forcing, compute it by lapsing sea-level
			    pressure by grid cell average elevation;
			    FALSE = air pressure set to constant 95.5 kPa */
  float  PREC_EXPT;      /* Exponential that controls the fraction of a
			    grid cell that receives rain during a storm
			    of given intensity */
  int    ROOT_ZONES;     /* Number of root zones used in simulation */
  char   QUICK_FLUX;     /* TRUE = Use Liang et al., 1999 formulation for
			    ground heat flux, if FALSE use explicit finite
			    difference method */
  char   QUICK_SOLVE;    /* TRUE = Use Liang et al., 1999 formulation for
			    iteration, but explicit finite difference
			    method for final step. */
  char   SNOW_ALBEDO;    /* USACE: Use algorithm of US Army Corps of Engineers, 1956; SUN1999: Use algorithm of Sun et al., JGR, 1999 */
  char   SNOW_DENSITY;   /* DENS_BRAS: Use algorithm of Bras, 1990; DENS_SNTHRM: Use algorithm of SNTHRM89 adapted for 1-layer pack */
  int    SNOW_BAND;      /* Number of elevation bands over which to solve the
			    snow model */
  int    SNOW_STEP;      /* Time step in hours to use when solving the
			    snow model */
  float  SW_PREC_THRESH; /* Minimum daily precipitation [mm] that can cause "dimming" of incoming shortwave radiation */
  char   TFALLBACK;      /* TRUE = when any temperature iterations fail to converge,
                                   use temperature from previous time step; the number
                                   of instances when this occurs will be logged and
                                   reported at the end of the cell's simulation
                            FALSE = when iterations fail to converge, report an error
                                    and abort simulation for current grid cell
                            Default = TRUE */
  char   VP_INTERP;      /* How to disaggregate VP from daily to sub-daily;
                            TRUE = linearly interpolate between daily VP values, assuming they occur at the times of Tmin;
                            FALSE = hold VP constant at the daily value */
  char   VP_ITER;        /* VP_ITER_NONE = never iterate with SW
                            VP_ITER_ALWAYS = always iterate with SW
                            VP_ITER_ANNUAL = use annual Epot/PRCP criterion
                            VP_ITER_CONVERGE = always iterate until convergence */
  int    Nlakenode;      /* Number of lake thermal nodes in the model. */

  // input options
  char   ALMA_INPUT;     /* TRUE = input variables are in ALMA-compliant units; FALSE = standard VIC units */
  char   ARC_SOIL;       /* TRUE = use ARC/INFO gridded ASCII files for soil
			    parameters*/
  char   BASEFLOW;       /* ARNO: read Ds, Dm, Ws, c; NIJSSEN2001: read d1, d2, d3, d4 */
  int    GRID_DECIMAL;   /* Number of decimal places in grid file extensions */
  char   VEGPARAM_LAI;   /* TRUE = veg param file contains monthly LAI values */
  char   LAI_SRC;        /* LAI_FROM_VEGLIB = read LAI values from veg library file
                            LAI_FROM_VEGPARAM = read LAI values from the veg param file */
  char   LAKE_PROFILE;   /* TRUE = user-specified lake/area profile */
  char   ORGANIC_FRACT;  /* TRUE = organic matter fraction of each layer is read from the soil parameter file; otherwise set to 0.0. */

  // state options
  char   BINARY_STATE_FILE; /* TRUE = model state file is binary (default) */
  char   INIT_STATE;     /* TRUE = initialize model state from file */
  char   SAVE_STATE;     /* TRUE = save state file */

  // output options
  char   ALMA_OUTPUT;    /* TRUE = output variables are in ALMA-compliant units; FALSE = standard VIC units */
  char   BINARY_OUTPUT;  /* TRUE = output files are in binary, not ASCII */
  char   COMPRESS;       /* TRUE = Compress all output files */
  char   MOISTFRACT;     /* TRUE = output soil moisture as fractional moisture content */
  int    Noutfiles;      /* Number of output files (not including state files) */
  char   PRT_HEADER;     /* TRUE = insert header at beginning of output file; FALSE = no header */
  char   PRT_SNOW_BAND;  /* TRUE = print snow parameters for each snow band. This is only used when default
				   output files are used (for backwards-compatibility); if outfiles and
				   variables are explicitly mentioned in global parameter file, this option
				   is ignored. */

} option_struct;

/*******************************************************
  Stores forcing file input information.
*******************************************************/
typedef struct {
  char    SIGNED;
  int     SUPPLIED;
  double  multiplier;
} force_type_struct;

/******************************************************************
  This structure records the parameters set by the forcing file
  input routines.  Those filled, are used to estimate the paramters
  needed for the model run in initialize_atmos.c.
  ******************************************************************/
typedef struct {
  force_type_struct TYPE[N_FORCING_TYPES];
  int  FORCE_DT;     /* forcing file time step */
  int  FORCE_ENDIAN; /* endian-ness of input file, used for
			   DAILY_BINARY format */
  int  FORCE_FORMAT; /* ASCII or BINARY */
  int  FORCE_INDEX[N_FORCING_TYPES];
  int  N_TYPES;
} param_set_struct;

/*******************************************************
  This structure stores all model run global parameters.
  *******************************************************/
typedef struct {
  double MAX_SNOW_TEMP; /* maximum temperature at which snow can fall (C) */
  double MIN_RAIN_TEMP; /* minimum temperature at which rain can fall (C) */
  double measure_h;  /* height of measurements (m) */
  double wind_h;     /* height of wind measurements (m) */
  float  resolution; /* Model resolution (degrees) */
  int    dt;         /* Time step in hours (24/dt must be an integer) */
  int    out_dt;     /* Output time step in hours (24/out_dt must be an integer) */
  int    endday;     /* Last day of model simulation */
  int    endmonth;   /* Last month of model simulation */
  int    endyear;    /* Last year of model simulation */
  int    forceday;   /* day forcing files starts */
  int    forcehour;  /* hour forcing files starts */
  int    forcemonth; /* month forcing files starts */
  int    forceskip;  /* number of model time steps to skip at the start of
			the forcing file */
  int    forceyear;  /* year forcing files start */
  int    nrecs;      /* Number of time steps simulated */
  int    skipyear;   /* Number of years to skip before writing output data */
  int    startday;   /* Starting day of the simulation */
  int    starthour;  /* Starting hour of the simulation */
  int    startmonth; /* Starting month of the simulation */
  int    startyear;  /* Starting year of the simulation */
  int    stateday;   /* Day of the simulation at which to save model state */
  int    statemonth; /* Month of the simulation at which to save model state */
  int    stateyear;  /* Year of the simulation at which to save model state */
} global_param_struct;

/***********************************************************
  This structure stores the soil parameters for a grid cell.
  ***********************************************************/
typedef struct {
  double   annual_prec;               /* annual average precipitation (mm) */
  double   dp;                        /* soil thermal damping depth (m) */
  float    elevation;                 /* grid cell elevation (m) */
  float    lat;                       /* grid cell central latitude */
  float    lng;                       /* grid cell central longitude */
  double   cell_area;                 /* Area of grid cell (m^2) */
  float    time_zone_lng;             /* central meridian of the time zone */
  int      gridcel;                   /* grid cell number */
  double   slope;
  double   aspect;
  double   ehoriz;
  double   whoriz;
} soil_con_struct;

/***************************************************************************
   This structure stores the atmospheric forcing data for each model time
   step for a single grid cell.  Each array stores the values for the
   SNOW_STEPs during the current model step and the value for the entire model
   step.  The latter is referred to by array[NR].  Looping over the SNOW_STEPs
   is done by for (i = 0; i < NF; i++)
***************************************************************************/
typedef struct {
  double *air_temp;  /* air temperature (C) */
  double *channel_in;/* incoming channel inflow for time step (mm) */
  double *density;   /* atmospheric density (kg/m^3) */
  double *longwave;  /* incoming longwave radiation (W/m^2) (net incoming
                        longwave for water balance model) */
  double out_prec;   /* Total precipitation for time step - accounts
                        for corrected precipitation totals */
  double out_rain;   /* Rainfall for time step (mm) */
  double out_snow;   /* Snowfall for time step (mm) */
  double *prec;      /* average precipitation in grid cell (mm) */
  double *pressure;  /* atmospheric pressure (kPa) */
  double *shortwave; /* incoming shortwave radiation (W/m^2) */
  char   *snowflag;  /* TRUE if there is snowfall in any of the snow
                        bands during the timestep, FALSE otherwise*/
  double *tskc;      /* cloud cover fraction (fraction) */
  double *vp;        /* atmospheric vapor pressure (kPa) */
  double *vpd;       /* atmospheric vapor pressure deficit (kPa) */
  double *wind;      /* wind speed (m/s) */
} atmos_data_struct;

/*************************************************************************
  This structure stores information about the time and date of the current
  time step.
  *************************************************************************/
typedef struct {
  int day;                      /* current day */
  int day_in_year;              /* julian day in year */
  int hour;                     /* beginning of current hour */
  int month;                    /* current month */
  int year;                     /* current year */
} dmy_struct;			/* array of length nrec created */



/******************************************************************
  This structure stores soil variables for the complete soil column
  for each grid cell.
  ******************************************************************/
typedef struct {
  double aero_resist[2];               /* The (stability-corrected) aerodynamic
                                          resistance (s/m) that was actually used
                                          in flux calculations.
					  [0] = surface (bare soil, non-overstory veg, or snow pack)
					  [1] = overstory */
  double asat;                         /* saturated area fraction */
  double baseflow;                     /* baseflow from current cell (mm/TS) */
  double inflow;                       /* moisture that reaches the top of
					  the soil column (mm) */
  double pot_evap[N_PET_TYPES];        /* array of different types of potential evaporation (mm) */
  double runoff;                       /* runoff from current cell (mm/TS) */
  double rootmoist;                    /* total of layer.moist over all layers
                                          in the root zone (mm) */
  double wetness;                      /* average of
                                          (layer.moist - Wpwp)/(porosity*depth - Wpwp)
                                          over all layers (fraction) */
  double zwt;                          /* average water table position [cm] - using lowest unsaturated layer */
  double zwt_lumped;                   /* average water table position [cm] - lumping all layers' moisture together */
} cell_data_struct;

/***********************************************************************
  This structure stores energy balance components, and variables used to
  solve the thermal fluxes through the soil column.
  ***********************************************************************/
typedef struct {
  // State variables
  double  AlbedoLake;            /* albedo of lake surface (fract) */
  double  AlbedoOver;            /* albedo of intercepted snow (fract) */
  double  AlbedoUnder;           /* surface albedo (fraction) */
  double  Cs[2];                 /* heat capacity for top two layers (J/m^3/K) */
  double  Cs_node[MAX_NODES];    /* heat capacity of the soil thermal nodes (J/m^3/K) */
  double  fdepth[MAX_FRONTS];    /* all simulated freezing front depths */
  char    frozen;                /* TRUE = frozen soil present */
  double  ice[MAX_NODES];        /* thermal node ice content */
  double  kappa[2];              /* soil thermal conductivity for top two layers (W/m/K) */
  double  kappa_node[MAX_NODES]; /* thermal conductivity of the soil thermal nodes (W/m/K) */
  double  moist[MAX_NODES];      /* thermal node moisture content */
  int     Nfrost;                /* number of simulated freezing fronts */
  int     Nthaw;                 /* number of simulated thawing fronts */
  double  T[MAX_NODES];          /* thermal node temperatures (C) */
  char    T_fbflag[MAX_NODES];   /* flag indicating if previous step's temperature was used */
  int     T_fbcount[MAX_NODES];  /* running total number of times that previous step's temperature was used */
  int     T1_index;              /* soil node at the bottom of the top layer */
  double  Tcanopy;               /* temperature of the canopy air */
  char    Tcanopy_fbflag;        /* flag indicating if previous step's temperature was used */
  int     Tcanopy_fbcount;       /* running total number of times that previous step's temperature was used */
  double  tdepth[MAX_FRONTS];    /* all simulated thawing front depths */
  double  Tfoliage;              /* temperature of the overstory vegetation */
  char    Tfoliage_fbflag;       /* flag indicating if previous step's temperature was used */
  int     Tfoliage_fbcount;      /* running total number of times that previous step's temperature was used */
  double  Tsurf;                 /* temperature of the understory */
  char    Tsurf_fbflag;          /* flag indicating if previous step's temperature was used */
  int     Tsurf_fbcount;         /* running total number of times that previous step's temperature was used */
  double  unfrozen;              /* frozen layer water content that is unfrozen */
  // Fluxes
  double  advected_sensible;     /* net sensible heat flux advected to snowpack (Wm-2) */
  double  advection;             /* advective flux (Wm-2) */
  double  AtmosError;
  double  AtmosLatent;           /* latent heat exchange with atmosphere */
  double  AtmosLatentSub;        /* latent sub heat exchange with atmosphere */
  double  AtmosSensible;         /* sensible heat exchange with atmosphere */
  double  canopy_advection;      /* advection heat flux from the canopy (W/m^2) */
  double  canopy_latent;         /* latent heat flux from the canopy (W/m^2) */
  double  canopy_latent_sub;     /* latent heat flux of sublimation from the canopy (W/m^2) */
  double  canopy_refreeze;       /* energy used to refreeze/melt canopy intercepted snow (W/m^2) */
  double  canopy_sensible;       /* sensible heat flux from canopy interception (W/m^2) */
  double  deltaCC;               /* change in snow heat storage (Wm-2) */
  double  deltaH;                /* change in soil heat storage (Wm-2) */
  double  error;                 /* energy balance error (W/m^2) */
  double  fusion;                /* energy used to freeze/thaw soil water */
  double  grnd_flux;             /* ground heat flux (Wm-2) */
  double  latent;                /* net latent heat flux (Wm-2) */
  double  latent_sub;            /* net latent heat flux from snow (Wm-2) */
  double  longwave;              /* net longwave flux (Wm-2) */
  double  LongOverIn;            /* incoming longwave to overstory */
  double  LongUnderIn;           /* incoming longwave to understory */
  double  LongUnderOut;          /* outgoing longwave from understory */
  double  melt_energy;           /* energy used to reduce snow cover fraction (Wm-2) */
  double  NetLongAtmos;          /* net longwave radiation to the atmosphere (W/m^2) */
  double  NetLongOver;           /* net longwave radiation from the overstory (W/m^2) */
  double  NetLongUnder;          /* net longwave radiation from the understory (W/m^2) */
  double  NetShortAtmos;         /* net shortwave to the atmosphere */
  double  NetShortGrnd;          /* net shortwave penetrating snowpack */
  double  NetShortOver;          /* net shortwave radiation from the overstory (W/m^2) */
  double  NetShortUnder;         /* net shortwave radiation from the understory (W/m^2) */
  double  out_long_canopy;       /* outgoing longwave to canopy */
  double  out_long_surface;      /* outgoing longwave to surface */
  double  refreeze_energy;       /* energy used to refreeze the snowpack (Wm-2) */
  double  sensible;              /* net sensible heat flux (Wm-2) */
  double  shortwave;             /* net shortwave radiation (Wm-2) */
  double  ShortOverIn;           /* incoming shortwave to overstory */
  double  ShortUnderIn;          /* incoming shortwave to understory */
  double  snow_flux;             /* thermal flux through the snow pack (Wm-2) */
} energy_bal_struct;

/***********************************************************************
  This structure stores vegetation variables for each vegetation type in
  a grid cell.
  ***********************************************************************/
typedef struct {
  double canopyevap;		/* evaporation from canopy (mm/TS) */
  double throughfall;		/* water that reaches the ground through
                                   the canopy (mm/TS) */
  double Wdew;			/* dew trapped on vegetation (mm) */
} veg_var_struct;

/************************************************************************
  This structure stores snow pack variables needed to run the snow model.
  ************************************************************************/
typedef struct {
  // State variables
  double albedo;            /* snow surface albedo (fraction) */
  double canopy_albedo;     /* albedo of the canopy (fract) */
  double coldcontent;       /* cold content of snow pack */
  double coverage;          /* fraction of snow band that is covered with snow */
  double density;           /* snow density (kg/m^3) */
  double depth;             /* snow depth (m) */
  int    last_snow;         /* time steps since last snowfall */
  double max_snow_depth;    /* last maximum snow depth - used to determine coverage
			       fraction during current melt period (m) */
  char   MELTING;           /* flag indicating that snowpack melted
			       previously */
  double pack_temp;         /* depth averaged temperature of the snowpack (C) */
  double pack_water;        /* liquid water content of the snow pack (m) */
  int    snow;              /* TRUE = snow, FALSE = no snow */
  double snow_canopy;       /* amount of snow on canopy (m) */
  double store_coverage;    /* stores coverage fraction covered by new snow (m) */
  int    store_snow;        /* flag indicating whether or not new accumulation
			       is stored on top of an existing distribution */
  double store_swq;         /* stores newly accumulated snow over an
			       established snowpack melt distribution (m) */
  double surf_temp;         /* depth averaged temperature of the snow pack surface layer (C) */
  double surf_temp_fbcount; /* running total number of times that previous step's temperature was used */
  double surf_temp_fbflag;  /* flag indicating if previous step's temperature was used */
  double surf_water;        /* liquid water content of the surface layer (m) */
  double swq;               /* snow water equivalent of the entire pack (m) */
  double snow_distrib_slope;/* current slope of uniform snow distribution (m/fract) */
  double tmp_int_storage;   /* temporary canopy storage, used in snow_canopy */
  // Fluxes
  double blowing_flux;      /* depth of sublimation from blowing snow (m) */
  double canopy_vapor_flux; /* depth of water evaporation, sublimation, or
			       condensation from intercepted snow (m) */
  double mass_error;        /* snow mass balance error */
  double melt;              /* snowpack melt (mm) */
  double Qnet;              /* Residual of energy balance at snowpack surface */
  double surface_flux;      /* depth of sublimation from blowing snow (m) */
  double transport;	    /* flux of snow (potentially) transported from veg type */
  double vapor_flux;        /* depth of water evaporation, sublimation, or
			       condensation from snow pack (m) */
} snow_data_struct;

/******************************************************************
  This structure stores the lake/wetland parameters for a grid cell
  ******************************************************************/
typedef struct {
  // Lake basin dimensions
  int    numnod;                  /* Maximum number of lake nodes for this grid cell */
  double z[MAX_LAKE_NODES+1];     /* Elevation of each lake node (when lake storage is at maximum), relative to lake's deepest point (m) */
  double basin[MAX_LAKE_NODES+1]; /* Area of lake basin at each lake node (when lake storage is at maximum) (m^2) */
  double Cl[MAX_LAKE_NODES+1];    /* Fractional coverage of lake basin at each node (when lake storage is at maximum) (fraction of grid cell area) */
  double b;                       /* Exponent in default lake depth-area profile (y=Ax^b) */
  double maxdepth;                /* Maximum allowable depth of liquid portion of lake (m) */
  double mindepth;                /* Minimum allowable depth of liquid portion of lake (m) */
  double maxvolume;               /* Lake volume when lake depth is at maximum (m^3) */
  double minvolume;               /* Lake volume when lake depth is at minimum (m^3) */
  // Hydrological properties
  float  bpercent;                /* Fraction of wetland baseflow (subsurface runoff) that flows into lake */
  float  rpercent;                /* Fraction of wetland surface runoff that flows into lake */
  double eta_a;                   /* Decline of solar radiation w/ depth (m^-1) */ /* not currently used */
  double wfrac;                   /* Width of lake outlet, expressed as fraction of lake perimeter */
  // Initial conditions
  double depth_in;                /* Initial lake depth (distance from surface to deepest point) (m) */
  int    lake_idx;                /* index number of the lake/wetland veg tile */
} lake_con_struct;

/*****************************************************************
  This structure stores the lake/wetland variables for a grid cell
  *****************************************************************/
typedef struct {
  // Current lake dimensions and liquid water state variables
  int    activenod;               /* Number of nodes whose corresponding layers currently contain water */
  double dz;                      /* Vertical thickness of all horizontal water layers below the surface layer (m) */
  double surfdz;                  /* Vertical thickness of surface (top) water layer (m) */
  double ldepth;                  /* Current depth of liquid water in lake (distance from surface to deepest point) (m) */
  double surface[MAX_LAKE_NODES+1];/* Area of horizontal cross-section of liquid water in lake at each node (m^2) */
  double sarea;                   /* Current surface area of ice+liquid water on lake surface (m^2) */
  double sarea_save;              /* Surface area of ice+liquid water on lake surface (m^2) at beginning of time step */
  double volume;                  /* Current lake water volume, including liquid water equivalent of lake ice (m^3) */
  double volume_save;             /* Lake water volume, including liquid water equivalent of lake ice (m^3) at beginning of time step */
  double temp[MAX_LAKE_NODES];    /* Lake water temperature at each node (C) */
  double tempavg;                 /* Average liquid water temperature of entire lake (C) */
  // Current properties (state variables) specific to lake ice/snow
  double areai;                   /* Area of ice coverage (at beginning of time step) (m^2) */
  double new_ice_area;            /* Area of ice coverage (at end of time step) (m^2) */
  double ice_water_eq;            /* Liquid water equivalent volume of lake ice (m^3) */
  double hice;                    /* Height of lake ice at thickest point (m) */
  double tempi;                   /* Lake ice temperature (C) */
  double swe;                     /* Water equivalence of lake snow cover - end of step (m^3) */
  double swe_save;                /* Water equivalence of lake snow cover - beginning of step (m^3) */
  double surf_temp;               /* Temperature of surface snow layer (C) */
  double pack_temp;               /* Temperature of pack snow layer (C) */
  double coldcontent;             /* cold content of snow pack */
  double surf_water;              /* Water content of surface snow layer (m^3) */
  double pack_water;              /* Water content of pack snow layer (m^3) */
  double SAlbedo;                 /* Albedo of lake snow (fraction) */
  double sdepth;                  /* Depth of snow on top of ice (m^3) */
  // Other current lake properties (derived from state variables and forcings)
  double aero_resist;	          /* Aerodynamic resistance (s/m) after stability correction */
  double density[MAX_LAKE_NODES]; /* Lake water density profile (kg/m^3) */
  // Moisture fluxes
  double baseflow_in;             /* Volume of baseflow into lake from the rest of the grid cell (m3) */
  double baseflow_out;            /* Volume of baseflow out of lake to channel network (m3) */
  double channel_in;              /* Volume of channel inflow into lake (m3) */
  double evapw;                   /* Volume of evaporative flux from lake (and ice/snow) surface (m3) */
  double ice_throughfall;         /* Volume of precipitation reaching lake water surface, i.e. total precip minus accumulation of snow on ice (m3) */
  double prec;                    /* Volume of precipitation falling on lake (and ice/snow) surface (m3) */
  double recharge;                /* Volume of recharge from lake to wetland (m3) */
  double runoff_in;               /* Volume of surface runoff into lake from the rest of the grid cell (m3) */
  double runoff_out;              /* Volume of surface runoff out of lake to channel network (m3) */
  double snowmlt;                 /* Volume of moisture released by melting of lake snow (m3) */
  double vapor_flux;              /* Volume of moisture sublimated from lake snow (m3) */
  // Structures compatible with other land cover types
  // Some of this information is currently redundant with other variables in the lake_var structure
  snow_data_struct  snow;         /* Snow pack on top of lake ice */
  energy_bal_struct energy;       /* Energy fluxes and soil temperatures */
  cell_data_struct  soil;         /* Soil column below lake */
} lake_var_struct;

/*****************************************************************
  This structure stores all variables needed to solve, or save
  solututions for all versions of this model.  Vegetation and soil
  variables are created for both wet and dry fractions of the grid
  cell (for use with the distributed precipitation model).
*****************************************************************/
typedef struct {
  cell_data_struct  **cell[2];    /* Stores soil layer variables (wet and
				     dry) */
  double             *mu;         /* fraction of grid cell that receives
				     precipitation */
  energy_bal_struct **energy;     /* Stores energy balance variables */
  lake_var_struct     lake_var;   /* Stores lake/wetland variables */
  snow_data_struct  **snow;       /* Stores snow variables */
  veg_var_struct    **veg_var[2]; /* Stores vegetation variables (wet and
				     dry) */
} dist_prcp_struct;

/*******************************************************
  This structure stores moisture state information for
  differencing with next time step.
  *******************************************************/
typedef struct {
  double	total_soil_moist; /* total column soil moisture [mm] */
  double	surfstor;         /* surface water storage [mm] */
  double	swe;              /* snow water equivalent [mm] */
  double	wdew;             /* canopy interception [mm] */
} save_data_struct;

/*******************************************************
  This structure stores output information for one variable.
  *******************************************************/
typedef struct {
  char		varname[20]; /* name of variable */
  int		write;       /* FALSE = don't write; TRUE = write */
  char		format[10];  /* format, when written to an ascii file;
		                should match the desired fprintf format specifier, e.g. %.4f */
  int		type;        /* type, when written to a binary file;
		                OUT_TYPE_USINT  = unsigned short int
		                OUT_TYPE_SINT   = short int
		                OUT_TYPE_FLOAT  = single precision floating point
		                OUT_TYPE_DOUBLE = double precision floating point */
  float		mult;        /* multiplier, when written to a binary file */
  int		aggtype;     /* type of aggregation to use;
				AGG_TYPE_AVG    = take average value over agg interval
				AGG_TYPE_BEG    = take value at beginning of agg interval
				AGG_TYPE_END    = take value at end of agg interval
				AGG_TYPE_MAX    = take maximum value over agg interval
				AGG_TYPE_MIN    = take minimum value over agg interval
				AGG_TYPE_SUM    = take sum over agg interval */
  int		nelem;       /* number of data values */
  double	*data;       /* array of data values */
  double	*aggdata;    /* array of aggregated data values */
} out_data_struct;

/*******************************************************
  This structure stores output information for one output file.
  *******************************************************/
typedef struct {
  char		prefix[20];  /* prefix of the file name, e.g. "fluxes" */
  char		filename[MAXSTRING]; /* complete file name */
  FILE		*fh;         /* filehandle */
  int		nvars;       /* number of variables to store in the file */
  int		*varid;      /* id numbers of the variables to store in the file
		                (a variable's id number is its index in the out_data array).
		                The order of the id numbers in the varid array
		                is the order in which the variables will be written. */
} out_data_file_struct;

/********************************************************
  This structure holds all variables needed for the error
  handling routines.
  ********************************************************/
typedef struct {
  atmos_data_struct *atmos;
  double             dt;
  energy_bal_struct *energy;
  int                rec;
  out_data_struct   *out_data;
  out_data_file_struct    *out_data_files;
  snow_data_struct  *snow;
  soil_con_struct    soil_con;
  veg_var_struct    *veg_var;
} Error_struct;


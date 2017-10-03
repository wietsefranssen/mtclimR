# library(devtools)
# install_git("https://github.com/wietsefranssen/mtclimR.git")
rm (list = ls())
library(mtclimR)

## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-1-31",
                         outstep = 6,
                         lonlatbox = c(108.25, 110.25, 35.25, 36.25))
                         # lonlatbox = c(92.25, 110.25, 7.25, 36.25))
# lonlatbox = c(100.75, 102.25, 32.25, 36.25))#,
#lonlatbox = c(-179.75, 179.75, -89.75, 89.75))

settings$system$nCores <- 2
settings$system$maxMem <- 0.0040 # in gb

## INIT INPUT FILES/VARS
settings <- setInputVars(settings,list(
  pr         = list(ncFileName = "./data/pr_Mekong.nc4",        ncName = "pr",        vicIndex = 9,   alma = FALSE),
  tasmin     = list(ncFileName = "./data/tasmin_Mekong.nc4",    ncName = "tasmin",    vicIndex = 17),
  tasmax     = list(ncFileName = "./data/tasmax_Mekong.nc4",    ncName = "tasmax",    vicIndex = 16),
  wind       = list(ncFileName = "./data/wind_Mekong.nc4",      ncName = "wind",      vicIndex = 20)
))
settings$elevation <- list(ncFileName = "./data/elevation_Mekong.nc", ncName = "elevation")

## INIT OUTPUT FILES/VARS
settings$outputVars <- list(
  pr         = list(VICName = "OUT_PREC",       units = "mm"),
  tas        = list(VICName = "OUT_AIR_TEMP",   units = "C"),
  shortwave  = list(VICName = "OUT_SHORTWAVE",  units = "W m-2"),
  longwave   = list(VICName = "OUT_LONGWAVE",   units = "W m-2"),
  pressure   = list(VICName = "OUT_PRESSURE",   units = "kPa"),
  qair       = list(VICName = "OUT_QAIR",       units = "kg kg-1"),
  vp         = list(VICName = "OUT_VP",         units = "kPa"),
  rel_humid  = list(VICName = "OUT_REL_HUMID",  units = "fraction"),
  density    = list(VICName = "OUT_DENSITY",    units = "kg m-3"),
  wind       = list(VICName = "OUT_WIND",       units = "m s-1")
)


main_netcdf(settings)

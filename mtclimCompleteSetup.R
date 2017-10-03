# library(devtools)
# install_git("https://github.com/wietsefranssen/mtclimR.git", branch="mtclimOpenMPParts")
#VALGRIND INFO: http://kevinushey.github.io/blog/2015/04/05/debugging-with-valgrind/
rm (list = ls())
library(mtclimR)

# nCores <- 1
# memMax <- 0.0040 # in gb

## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-1-31",
                         outstep = 6,
                         lonlatbox = c(108.25, 110.25, 35.25, 36.25))
                         # lonlatbox = c(92.25, 110.25, 7.25, 36.25))
# lonlatbox = c(100.75, 102.25, 32.25, 36.25))#,
#lonlatbox = c(-179.75, 179.75, -89.75, 89.75))

## INIT INPUT FILES/VARS
settings <- setInputVars(settings,list(
  pr         = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "prAdjust",        vicIndex = 9,   alma = FALSE),
  tasmin     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasminAdjust",    vicIndex = 17),
  tasmax     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",    vicIndex = 16),
  wind       = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "windAdjust",      vicIndex = 20)
))
settings$elevation <- list(ncFileName = "./data/WFDEI-elevation.nc", ncName = "elevation")

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

## Set outvars in settings
settings$mtclim$nOut <- length(settings$outputVars)
for (i in 1:length(settings$outputVars)) {
  settings$mtclim$outNames[i]<-settings$outputVars[[i]]$VICName
}
rm(i)
main_netcdf()

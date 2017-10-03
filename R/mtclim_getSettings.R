mtclim_getSettings <- function() {
  ## INIT SETTINGS
  settings <- initSettings(startdate = "1950-01-01",
                           enddate = "1950-1-31",
                           outstep = 6,
                           lonlatbox = c(92.25, 110.25, 7.25, 36.25),
                           outfile = "example_output_Mekong.nc")

  ## Some system settings
  settings$system$nCores <- 2
  settings$system$maxMem <- 0.0040 # in Gb

  ncFileNameElevation  <- system.file("extdata", "elevation_Mekong.nc4", package = "mtclimR")
  ncFileNamePr         <- system.file("extdata", "pr_Mekong.nc4", package = "mtclimR")
  ncFileNameTasmin     <- system.file("extdata", "tasmin_Mekong.nc4", package = "mtclimR")
  ncFileNameTasmax     <- system.file("extdata", "tasmax_Mekong.nc4", package = "mtclimR")
  ncFileNameWind       <- system.file("extdata", "wind_Mekong.nc4", package = "mtclimR")

  ## INIT INPUT FILES/VARS
  settings <- setInputVars(settings,list(
    pr         = list(ncFileName = ncFileNamePr,        ncName = "pr",        vicIndex = 9,   alma = FALSE),
    tasmin     = list(ncFileName = ncFileNameTasmin,    ncName = "tasmin",    vicIndex = 17),
    tasmax     = list(ncFileName = ncFileNameTasmax,    ncName = "tasmax",    vicIndex = 16),
    wind       = list(ncFileName = ncFileNameWind,      ncName = "wind",      vicIndex = 20)
  ))
  settings$elevation <- list(ncFileName = ncFileNameElevation, ncName = "elevation")

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

  return(settings)
}

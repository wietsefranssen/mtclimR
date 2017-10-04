# library(devtools)
# install_git("https://github.com/wietsefranssen/mtclimR.git")
library(mtclimR)

## Cleanup
rm(list=ls(all=TRUE))

## Fill struct with settings
settings <- mtclim_getSettings()

settings <- setInputVars(settings,list(
  pr         = list(ncFileName = "./inst/extdata/pr_Mekong.nc4",        ncName = "pr",        vicIndex = 9,   alma = FALSE),
  tasmin     = list(ncFileName = "./inst/extdata/tasmin_Mekong.nc4",    ncName = "tasmin",    vicIndex = 17),
  tasmax     = list(ncFileName = "./inst/extdata/tasmax_Mekong.nc4",    ncName = "tasmax",    vicIndex = 16),
  # rsds       = list(ncFileName = "./inst/extdata/tasmax_Mekong.nc4",    ncName = "tasmax",    vicIndex = 14),
  # rlds       = list(ncFileName = "./inst/extdata/tasmax_Mekong.nc4",    ncName = "tasmax",    vicIndex = 6),
  # pressure   = list(ncFileName = "./inst/extdata/tasmax_Mekong.nc4",    ncName = "tasmax",    vicIndex = 10),
  # vp       = list(ncFileName = "./inst/extdata/tasmax_Mekong.nc4",    ncName = "tasmax",    vicIndex = 19),
  wind       = list(ncFileName = "./inst/extdata/wind_Mekong.nc4",      ncName = "wind",      vicIndex = 20)
))

settings$lonlatbox = c(108.25, 110.25, 35.25, 36.25)
settings$outfile = "out.nc"
## Run the main routine
mtclim_run(settings)

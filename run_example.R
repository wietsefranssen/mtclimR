# library(devtools)
# install_git("https://github.com/wietsefranssen/mtclimR.git")
library(mtclimR)

## Cleanup
rm(list=ls(all=TRUE))

## Fill struct with settings
settings <- mtclim_getSettings()
# lonlatbox = c(108.25, 110.25, 35.25, 36.25),

## Run the main routine
mtclim_run(settings)
#library(devtools)
#install_git("https://github.com/wietsefranssen/mtclimR.git", branch="mtclimMultiCore")

rm (list = ls())
library(WFRTools)
library(doParallel)
library(mtclimR)
# library(pbdNCDF4)
nCores<-2
registerDoParallel(cores=nCores)
start.time.total <- Sys.time()
print(paste("nCores: ", nCores))

## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-1-31",
                         outstep = 6,
#                         lonlatbox = c(100.75, 102.25, 32.25, 36.25))
# lonlatbox = c(92.25, 110.25, 7.25, 36.25))
lonlatbox = c(-179.75, 179.75, -89.75, 89.75))

## INIT INPUT FILES/VARS
settings <- setInputVars(settings,list(
  pr         = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/pr_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "prAdjust",        alma = TRUE, vicIndex = 9),
  tasmin     = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/tasmin_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "tasminAdjust",     vicIndex = 17),
  tasmax     = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/tasmax_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "tasmaxAdjust",     vicIndex = 16),
  wind       = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/wind_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "windAdjust",       vicIndex = 20)
  # shortwave  = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse",        ncName = "tasmaxAdjust", vicIndex = 14),
  # longwave   = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse",        ncName = "tasmaxAdjust",  vicIndex = 6)
))
settings$elevation  <- list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/WFDEI-elevation.nc", ncName = "elevation")

## INIT INPUT FILES/VARS
# settings <- setInputVars(settings,list(
#   pr         = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "prAdjust",        vicIndex = 9,   alma = FALSE),
#   tasmin     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasminAdjust",    vicIndex = 17),
#   tasmax     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",    vicIndex = 16),
#   wind       = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "windAdjust",      vicIndex = 20)
# ))
# settings$elevation <- list(ncFileName = "./data/domain_elev_Mekong.nc", ncName = "elev")

## INIT OUTPUT FILES/VARS
settings$outputVars <- list(
  pr         = list(ncName = "pr"),
  tas     = list(ncName = "tas"),
  shortwave     = list(ncName = "shortwave"),
  longwave     = list(ncName = "longwave"),
  pressure     = list(ncName = "pressure"),
  wind       = list(ncName = "wind")
)

### THE MAIN ROUTINE

## LOAD MASK/ELEVATION
elevation <- ncLoad(file = settings$elevation$ncFileName,
                    varName = settings$elevation$ncName,
                    lonlatbox = settings$lonlatbox)

## makeOutputNetCDF
makeNetcdfOut(settings, elevation)

## DEFINE OUTPUT ARRAY
el <- array(NA, dim = c(length(elevation$xyCoords$x), length(elevation$xyCoords$y), settings$intern$nrec_out))
toNetCDFData <- list(el)[rep(1,length(settings$outputVars))]
print(paste0("output array: ", format(object.size(toNetCDFData), units = "auto")))

## LOAD WHOLE DOMAIN FROM NETCDF
print(paste0("reading..."))
start.time.read <- Sys.time()
forcing_dataRTotal <- readForcingAll(settings, elevation)
print(paste0("forcing array: ", format(object.size(forcing_dataRTotal), units = "auto")))
print(sprintf("read: %6.1f min",as.numeric(Sys.time() - start.time.read, units = "mins")))

start.time.mtclim <- Sys.time()
## CELL LOOP
for (iy in 1:length(elevation$xyCoords$y)) {
  print(paste("running iy: ",iy))
  output<-foreach(ix = 1:length(elevation$xyCoords$x)) %dopar% {
    # for (ix in 1:length(elevation$xyCoords$x)) {
    if (!is.na(elevation$Data[iy,ix])) {
      forcing_dataR <- selectForcingCell(settings, forcing_dataRTotal, ix, iy)

      settings$mtclim$elevation <- elevation$Data[iy,ix]
      settings$mtclim$lon<-elevation$xyCoords$x[ix]
      settings$mtclim$lat<-elevation$xyCoords$y[iy]

      ## RUN MLTCLIM
      output<-mtclimRun(forcing_dataR = forcing_dataR, settings = settings$mtclim)
      output$out_data
    }
  }
  print(paste0("mtclim temp array: ", format(object.size(output), units = "auto")))
  print(sprintf("mtclim: %6.1f min",as.numeric(Sys.time() - start.time.mtclim, units = "mins")))
  print(paste0("adding to output array..."))
  iix <- 1
  print(paste0("llll: " ,length(output)))
  for (ix in 1:length(elevation$xyCoords$x)) {
  # print(paste("ix: ", ix))
    if (!is.na(elevation$Data[iy,ix])) {
      # print(paste("ixXX: ", ix))
      ## ADD TO OUTPUT ARRAY
      for (iVar in 1:length(settings$outputVars)) toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][[iVar]]
      iix <- iix + 1
    }
  }
  # rm(output)
  print(paste0("adding to output array: done"))
  # print(paste(iy,outputt))
}

## ADD OUTPUT TO NETCDF
print(paste0("writing..."))
start.time.write <- Sys.time()
ncid <- nc_open(settings$outfile, write = TRUE)
for (iVar in 1:length(settings$outputVars))
{
  ncvar_put(ncid,
            settings$outputVars[[iVar]]$ncName,
            toNetCDFData[[iVar]],
            start = c(1,
                      1,
                      1),
            count = c(length(elevation$xyCoords$x),
                      length(elevation$xyCoords$y),
                      settings$intern$nrec_out)
  )
}
nc_close(ncid)
print(sprintf("write: %6.1f min",as.numeric(Sys.time() - start.time.write, units = "mins")))
print(sprintf("total: %6.1f min",as.numeric(Sys.time() - start.time.total, units = "mins")))



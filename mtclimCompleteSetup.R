#library(devtools)
#install_git("https://github.com/wietsefranssen/mtclimR.git", branch="newSetup")

rm (list = ls())
library(WFRTools)
start.time1 <- Sys.time()
library(doParallel)
library(mtclimR)
# library(pbdNCDF4)
nCores<-1
registerDoParallel(cores=nCores)
start.time.total <- Sys.time()
print(paste("nCores: ", nCores))

## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-12-31",
                         outstep = 6,
                         lonlatbox = c(100.75, 102.25, 32.25, 36.25))
#lonlatbox = c(92.25, 110.25, 7.25, 36.25))
#lonlatbox = c(-179.75, 179.75, 7.25, 36.25))
#lonlatbox = c(92.25, 110.25, 7.25, 36.25))

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
settings <- setInputVars(settings,list(
  pr         = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "prAdjust",        vicIndex = 9,   alma = FALSE),
  tasmin     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasminAdjust",    vicIndex = 17),
  tasmax     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",    vicIndex = 16),
  wind       = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "windAdjust",      vicIndex = 20)
))
settings$elevation <- list(ncFileName = "./data/domain_elev_Mekong.nc", ncName = "elev")

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
#############
## LOAD MASK/ELEVATION
elevation <- ncLoad(file = settings$elevation$ncFileName,
                    varName = settings$elevation$ncName,
                    lonlatbox = settings$lonlatbox)

## makeOutputNetCDF
makeNetcdfOut(settings, elevation)

## DIVIDE DOMAIN IN PARTS (NOT TO SMALL AND NOT TOO BIG(SPEED vs MEMORY))
#settings$parts<- setSubDomains(settings, elevation, partSize = NULL)
#settings$parts<- setSubDomains(settings, elevation, partSize = 3)


## DEFINE OUTPUT ARRAY
el <- array(NA, dim = c(length(elevation$xyCoords$x), length(elevation$xyCoords$y), settings$intern$nrec_out))
toNetCDFData <- list(el)[rep(1,length(settings$outputVars))]

## LOAD SUBDOMAIN FROM NETCDF
#######
# forcing_dataRTotal <- readForcing(settings, iPart)
forcing_dataRTotal <- readForcingAll(settings, elevation)
#######

## CELL LOOP
for (iy in 1:length(elevation$xyCoords$y)) {
  output<-foreach(ix = 1:length(elevation$xyCoords$x)) %dopar% {
    # for (ix in 1:length(elevation$xyCoords$x)) {
    # ix<-1
    # iy<-1
    print(paste(iy,ix))
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

  for (ix in 1:length(elevation$xyCoords$x)) {
    if (!is.na(elevation$Data[iy,ix])) {
      ## ADD TO OUTPUT ARRAY
      for (iVar in 1:length(settings$outputVars)) toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][[iVar]]
    }
  }
  # print(paste(iy,outputt))
}

## ADD OUTPUT TO NETCDF
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

print(paste("total: ", as.numeric(Sys.time() - start.time.total), units = "mins"))



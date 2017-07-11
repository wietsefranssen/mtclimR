rm (list = ls())
library(WFRTools)
library(doParallel)
# library(pbdNCDF4)
library(ncdf4)
registerDoParallel(cores=2)

start.time1 <- Sys.time()

## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-12-31",
                         outstep = 6,
                         lonlatbox = c(92.25, 110.25, 7.25, 36.25))
# ## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-12-31",
                         outstep = 6,
                         lonlatbox = c(100.75, 102.25, 32.25, 36.25))

settings <- setInputVars(settings,list(
  pr         = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/pr_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "prAdjust",        alma = TRUE, vicIndex = 9),
  tasmin     = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/tasmin_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "tasminAdjust",     vicIndex = 17),
  tasmax     = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/tasmax_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "tasmaxAdjust",     vicIndex = 16),
  wind       = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/wind_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "windAdjust",       vicIndex = 20)
  # shortwave  = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse",        ncName = "tasmaxAdjust", vicIndex = 14),
  # longwave   = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse",        ncName = "tasmaxAdjust",  vicIndex = 6)
))
settings$elevation  <- list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/WFDEI-elevation.nc", ncName = "elevation")

## INIT INPUT FILES
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
# doMtclim(SETTINGS)
{
  ## LOAD MASK/ELEVATION
  elevation <- ncLoad(file = settings$elevation$ncFileName,
                      varName = settings$elevation$ncName,
                      lonlatbox = settings$lonlatbox)

  ## makeOutputNetCDF
  makeNetcdfOut(settings, elevation)

  ## DIVIDE DOMAIN IN PARTS (NOT TO SMALL AND NOT TOO BIG(SPEED vs MEMORY))
  # settings$parts<- setSubDomains(settings, elevation, partSize = NULL)
  settings$parts<- setSubDomains(settings, elevation, partSize = 20)

  ## SUBDOMAIN LOOP / MPI LOOP
  # foreach(iPart = 1:length(settings$parts)) %dopar% {
  for (iPart in 1:length(settings$parts)) {
    # iPart <- 1
  # {
    start.time <- Sys.time()
    print(paste0("doing part: ", iPart, "/", length(settings$parts)))

    ## DEFINE OUTPUT ARRAY
    # el <- array(NA, dim = c(settings$intern$nrec_out,settings$parts[[iPart]]$ny,settings$parts[[iPart]]$nx))
    el <- array(NA, dim = c(settings$parts[[iPart]]$nx,settings$parts[[iPart]]$ny,settings$intern$nrec_out))
    toNetCDFData <- list(el)[rep(1,length(settings$outputVars))]

    ## LOAD SUBDOMAIN FROM NETCDF
    forcing_dataRTotal <- readForcing(settings, iPart)

    ## CELL LOOP
    for (iy in 1:settings$parts[[iPart]]$ny) {
      # print(paste0("iy: ", iy, "/", settings$parts[[iPart]]$ny))
      for (ix in 1:settings$parts[[iPart]]$nx) {
        # print(paste(iy,ix))
        iix <-settings$parts[[iPart]]$sx+ix-1
        iiy <-settings$parts[[iPart]]$sy+iy-1
        if (!is.na(elevation$Data[iiy,iix])) {
          forcing_dataR <- selectForcingCell(settings, forcing_dataRTotal, ix, iy)

          settings$mtclim$elevation <- elevation$Data[iiy,iix]
          settings$mtclim$lon<-elevation$xyCoords$x[iix]
          settings$mtclim$lat<-elevation$xyCoords$y[iiy]

          ## RUN MLTCLIM
          output <- mtclimRun(forcing_dataR = forcing_dataR, settings = settings$mtclim)

          ## ADD TO OUTPUT ARRAY
          for (iVar in 1:length(settings$outputVars)) toNetCDFData[[iVar]][ix,iy,] <- output$out_data[[iVar]]
        }
      }
    }
    end.time <- Sys.time()
    time.taken <- end.time - start.time
    print(time.taken); rm(start.time, end.time, time.taken)

    ## ADD OUTPUT TO NETCDF
    ncid <- nc_open(settings$outfile, write = TRUE)
    for (iVar in 1:length(settings$outputVars))
    {
      ncvar_put(ncid,
                settings$outputVars[[iVar]]$ncName,
                toNetCDFData[[iVar]],
                start = c(settings$parts[[iPart]]$sx,
                          settings$parts[[iPart]]$sy,
                          1),
                count = c(settings$parts[[iPart]]$nx,
                          settings$parts[[iPart]]$ny,
                          settings$intern$nrec_out)
      )
    }
    nc_close(ncid)

  }
}
end.time <- Sys.time()
time.taken <- end.time - start.time1
print(time.taken); rm(start.time1, end.time, time.taken)


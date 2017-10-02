# library(devtools)
# install_git("https://github.com/wietsefranssen/mtclimR.git", branch="mtclimMultiCore")
# install_git("https://github.com/wietsefranssen/mtclimR.git", branch="mtclimOpenMPParts")
#VALGRIND INFO: http://kevinushey.github.io/blog/2015/04/05/debugging-with-valgrind/
rm (list = ls())
library(WFRTools)
library(doParallel)
library(R.utils)
library(mtclimR)
nCores <- 2
memMax <- 0.000040 # in gb

registerDoParallel(cores=nCores)
start.time.total <- Sys.time()
print(paste("nCores: ", nCores))

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
settings$elevation <- list(ncFileName = "./data/domain_elev_Mekong.nc", ncName = "elev")
settings$elevation <- list(ncFileName = "./data/WFDEI-elevation.nc", ncName = "elevation")

## INIT OUTPUT FILES/VARS
settings$outputVars <- list(
  pr         = list(VICName = "OUT_PREC",       units = "mm"),
  tas        = list(VICName = "OUT_AIR_TEMP",   units = "C"),
  shortwave  = list(VICName = "OUT_SHORTWAVE",  units = "W m-2"),
  longwave   = list(VICName = "OUT_LONGWAVE",   units = "W m-2"),
  pressure   = list(VICName = "OUT_PRESSURE",   units = "kPa"),
  #  qair       = list(VICName = "OUT_QAIR",       units = "kg kg-1"),
  vp         = list(VICName = "OUT_VP",         units = "kPa"),
  #  rel_humid  = list(VICName = "OUT_REL_HUMID",  units = "fraction"),
  #  density    = list(VICName = "OUT_DENSITY",    units = "kg m-3"),
  wind       = list(VICName = "OUT_WIND",       units = "m s-1")
)

## Set outvars in settings
settings$mtclim$nOut <- length(settings$outputVars)
for (i in 1:length(settings$outputVars)) {
  settings$mtclim$outNames[i]<-settings$outputVars[[i]]$VICName
}
## LOAD MASK/ELEVATION
elevation <- ncLoad(file = settings$elevation$ncFileName,
                    varName = settings$elevation$ncName,
                    lonlatbox = settings$lonlatbox)

## makeOutputNetCDF
makeNetcdfOut(settings, elevation)

## Subdivide in parts
settings_org <- settings
elevation_org <- elevation

## Calculate minimum number of parts based on the memort in the system:
minNParts <- calcMinNParts(settings, elevation, memMax)

mask<-elevation
parts <- setSubDomains(settings, elevation, nPart = minNParts)
# parts <- setSubDomains(settings, elevation, nPart = 4)

nPart<-length(parts)
# iPart<-1
for (iPart in 1:length(parts)) {

  # ## Change settings for current part
  part <- parts[[iPart]]
  settings$lonlatbox <- c(part$slon, part$elon, part$slat, part$elat)
  elevation <- ncLoad(file = settings$elevation$ncFileName,
                      varName = settings$elevation$ncName,
                      lonlatbox = settings$lonlatbox)

  ## DEFINE OUTPUT ARRAY
  el <- array(NA, dim = c(part$nx, part$ny, settings$intern$nrec_out))
  toNetCDFData <- list(el)[rep(1,length(settings$outputVars))]
  print(paste0("output array: ", format(object.size(toNetCDFData), units = "auto")))
  rm(el)

  ## LOAD WHOLE DOMAIN FROM NETCDF
  print(paste0("reading..."))
  start.time.read <- Sys.time()
  forcing_dataRTotal <- readForcingAll(part, settings, elevation)
  print(paste0("forcing array: ", format(object.size(forcing_dataRTotal), units = "auto")))
  print(sprintf("read: %6.1f min",as.numeric(Sys.time() - start.time.read, units = "mins")))

  # Print part info
  print(sprintf("part:%3.0d/%.0d, nx: %.0d, ny: %.0d",
                iPart, nPart, part$nx, part$ny))

  ## Init progressbar
  pb <- txtProgressBar(min = 0, max = part$ny, initial = 0, char = "=",
                       width = NA, title, label, style = 3, file = "")

  ## CELL LOOP
  for (iy in 1:part$ny) {
    # start.time.mtclim <- Sys.time()
    # print(paste("running iy: ",iy))

    output<-foreach(ix = 1:part$nx) %dopar% {
      # for (ix in 1:length(elevation$xyCoords$x)) {
      if (!is.na(elevation$Data[iy,ix])) {
        settings$mtclim$elevation <- elevation$Data[iy,ix]
        settings$mtclim$lon<-elevation$xyCoords$x[ix]
        settings$mtclim$lat<-elevation$xyCoords$y[iy]

        ## RUN MLTCLIM
        mtclimRun(forcing_dataR = selectForcingCell(settings, forcing_dataRTotal, ix, iy),
                  settings = settings$mtclim)$out_data
      }
    }

    # print(paste0("mtclim temp array: ", format(object.size(output), units = "auto")))
    # print(sprintf("mtclim: %6.1f min",as.numeric(Sys.time() - start.time.mtclim, units = "mins")))

    for (ix in 1:length(elevation$xyCoords$x)) {
      if (!is.na(elevation$Data[iy,ix])) {
        ## ADD TO OUTPUT ARRAY
        for (iVar in 1:length(settings$outputVars)) {
          # #      if (!is.na(output[[ix]][[iVar]][1])) {
          # # toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][[iVar]]
          # # format(output[[1]][((varr*settings$intern$nrec_out)+1):((varr+1)*settings$intern$nrec_out)], scientific=FALSE)
          # varr<-iVar-1
          # toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][((varr*settings$intern$nrec_out)+1):((varr+1)*settings$intern$nrec_out)]
          iStart <- ((iVar-1)*settings$intern$nrec_out)+1
          iEnd <- iVar*settings$intern$nrec_out
          toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][iStart:iEnd]
          #  }
        }
      }
    }
    # rm(output)

    ## refresh progressbar
    setTxtProgressBar(pb, iy)

  }
  # Close ProgressBar
  close(pb)

  ## ADD OUTPUT TO NETCDF
  print(paste0("writing..."))
  start.time.write <- Sys.time()
  ncid <- nc_open(settings$outfile, write = TRUE)
  for (iVar in 1:length(settings$outputVars))
  {
    ncvar_put(ncid,
              names(settings$outputVars)[iVar],
              toNetCDFData[[iVar]],
              start = c(part$sx,
                        part$sy,
                        1),
              count = c(part$nx,
                        part$ny,
                        settings$intern$nrec_out)
    )
  }
  nc_close(ncid)
}
print(sprintf("write: %6.1f min",as.numeric(Sys.time() - start.time.write, units = "mins")))
print(sprintf("total: %6.1f min",as.numeric(Sys.time() - start.time.total, units = "mins")))

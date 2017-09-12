# library(devtools)
# install_git("https://github.com/wietsefranssen/mtclimR.git", branch="mtclimMultiCore")
# install_git("https://github.com/wietsefranssen/mtclimR.git", branch="mtclimOpenMPParts")
rm (list = ls())
library(WFRTools)
library(doParallel)
library(mtclimR)
nCores<-2
memMax<-0.1 # in gb
registerDoParallel(cores=nCores)
start.time.total <- Sys.time()
print(paste("nCores: ", nCores))

## INIT SETTINGS
settings <- initSettings(startdate = "1950-01-01",
                         enddate = "1950-1-31",
                         outstep = 6,
                         lonlatbox = c(108.25, 110.25, 35.25, 36.25))
#lonlatbox = c(92.25, 110.25, 7.25, 36.25))
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

## Calculate memory needed:
length(elevation$Data)
memInput <- length(elevation$Data) * settings$intern$nrec_in * length(settings$inputVars) * 8 /(1024*1024*1024)
memOutput <- length(elevation$Data) * settings$intern$nrec_out * length(settings$outputVars) * 8 /(1024*1024*1024)
memExtra <- length(elevation$Data) * 100 * 8 /(1024*1024*1024)
memTotal <- memInput + memOutput + memExtra
print(sprintf("Total memory: %6.2f Gb (Max mem: %6.2f Gb)", memTotal, memMax))
nPart <- ceiling(memTotal / memMax * 2) #*2 for safety

setSubDomains <- function(settings, mask, nPart = NULL) {
mask<-elevation
# nPart<-4
  nxxx<-length(mask$xyCoords$x)
  nyyy<-length(mask$xyCoords$y)
  nCells<-length(mask$xyCoords$x)*length(mask$xyCoords$y)

  nActive <- length(mask$Data[!is.na(mask$Data)])

  lengthxy<-ceiling(sqrt(nCells/nPart))
  parts<-NULL
  ey<-ex<-0
  sy<-sx<-0
  ny<-nx<-0
  sxPrev<-syPrev<-1
  i<-1
  while(ey < nyyy) {
    while(ex < nxxx) {
      sx[[i]]<-sxPrev
      sy[[i]]<-syPrev
      nx[[i]]<-lengthxy
      ny[[i]]<-lengthxy

      ex<-sxPrev+lengthxy-1
      sxPrev<-sx[[i]]+lengthxy
      i<-i+1
    }
    nx[[i-1]]<-nxxx-sx[[i-1]]+1
    ey<-syPrev+lengthxy-1
    syPrev<-syPrev+lengthxy
    ex<-1
    sxPrev<-1
  }
  nPart<-length(sy)
  for (iPart in 1:nPart) {
    if ((nyyy-(sy[[iPart]])) < lengthxy) {
      ny[[iPart]]<-nyyy- sy[[iPart]]+1
    }
    if ((nxxx-(sx[[iPart]])) < lengthxy) {
      nx[[iPart]]<-nxxx- sx[[iPart]]+1
    }
  }
  part <- list(sx = 1,
               nx = 1,
               sy = NULL,
               ny = NULL,
               slon = min(mask$xyCoords$x),
               elon = max(mask$xyCoords$x),
               slat = NULL,
               elat = NULL)
  parts <- list(part)[rep(1,nPart)]

    ## The devision
  for (iPart in 1:nPart) {
    endx<- sx[iPart]+nx[iPart]-1
    endy<- sy[iPart]+ny[iPart]-1
    # parts[[iPart]]<-xmat[sx[iPart]:endx,sy[iPart]:endy]
  }
  #####

  for (iPart in 1:(nPart)) {
    endx<- sx[iPart]+nx[iPart]-1
    endy<- sy[iPart]+ny[iPart]-1
    parts[[iPart]]$sx <- sx[iPart]
    parts[[iPart]]$sy <- sy[iPart]
    parts[[iPart]]$slon <- mask$xyCoords$x[parts[[iPart]]$sx]
    parts[[iPart]]$elon <- mask$xyCoords$x[endx]
    parts[[iPart]]$slat <- mask$xyCoords$y[parts[[iPart]]$sy]
    parts[[iPart]]$elat <- mask$xyCoords$y[endy]
    parts[[iPart]]$nx <- nx[iPart]
    parts[[iPart]]$ny <- ny[iPart]
  }
  # parts[[nPart]]$elat <- mask$xyCoords$y[length(mask$xyCoords$y)]
  # parts[[nPart]]$nx <- nx[iPart]
  # parts[[nPart]]$ny <- ny[iPart]
  print(paste0("Total cells: ", nCells, ", Active Cells: ", nActive, ", nParts: ", nPart))

  return(parts)
}
parts <- setSubDomains(settings, elevation, nPart = nPart)
nPart<-length(parts)
iPart<-1
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

  ## CELL LOOP
  for (iy in 1:part$ny) {
    start.time.mtclim <- Sys.time()
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
          #      if (!is.na(output[[ix]][[iVar]][1])) {
          toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][[iVar]]
          #  }
        }
      }
    }
    # rm(output)
    print(sprintf("part:%3.0d/%.0d, iy:%3.0d/%.0d, mtclim done in: %6.1f min",
                  iPart, nPart,
                  iy, part$ny,
                  as.numeric(Sys.time() - start.time.mtclim, units = "mins")
                  ))
    # print(paste0("adding to output array: done"))
  }

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

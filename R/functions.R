#library(pbdNCDF4)
library(ncdf4)

initSettings <- function(startdate = NULL, enddate = NULL, outstep = 24, lonlatbox = NULL, outfile = "output.nc") {

  intern <- list (
    nrec_in = NULL,
    nrec_out = NULL
  );
  mtclim <- list (
    outstep = outstep,
    startyear = NULL,
    startmonth = NULL,
    startday = NULL,
    istartdate = NULL,
    endyear = NULL,
    endmonth = NULL,
    endday = NULL,
    ienddate = NULL,
    nForcing = NULL,
    forcingIds = NULL,
    alma_input = FALSE
  )
  settings <- list(
    lonlatbox = lonlatbox,
    startdate = as.Date(startdate),
    enddate = as.Date(enddate),
    outstep = outstep,
    outfile = outfile,
    mtclim = mtclim,
    intern = intern
  )

  settings$intern$nrec_in = as.numeric((settings$enddate - settings$startdate) +1)
  settings$intern$nrec_out = settings$intern$nrec_in * (24/outstep)

  ## Add forcing info to settings
  nForcing = length(settings$inputVars)
  forcingIds = array(NA, dim = c(nForcing))
  for (iFor in 1:nForcing) {
    settings$mtclim$forcingIds[iFor] = settings$inputVars[[iFor]]$vicIndex
  }

  ymdStart<-as.numeric(strsplit(as.character(settings$startdate), "-")[[1]])
  ymdEnd<-as.numeric(strsplit(as.character(settings$enddate), "-")[[1]])

  settings$mtclim$startyear = ymdStart[1]
  settings$mtclim$startmonth = ymdStart[2]
  settings$mtclim$startday = ymdStart[3]
  settings$mtclim$istartdate = 1
  settings$mtclim$endyear = ymdEnd[1]
  settings$mtclim$endmonth = ymdEnd[2]
  settings$mtclim$endday = ymdEnd[3]
  settings$mtclim$ienddate = settings$intern$nrec_in

  return(settings)
}

setInputVars <- function(settings, inputVars) {
  settings$inputVars <- inputVars

  if ( exists("alma", where = settings$inputVars[[1]]))  settings$mtclim$alma_input = settings$inputVars[[1]]$alma

  nForcing <- length(inputVars)
  forcingIds = array(NA, dim = c(nForcing))
  for (iFor in 1:nForcing) {
    forcingIds[iFor] = inputVars[[iFor]]$vicIndex
  }
  settings$mtclim$nForcing = nForcing
  settings$mtclim$forcingIds = forcingIds

  return(settings)
}

makeNetcdfOut <- function(settings, mask) {

  ## CREATE NETCDF
  FillValue <- 1e20

  ## Define dimensions
  dimX <- ncdim_def("lon", "degrees_east", mask$xyCoords$x)
  dimY <- ncdim_def("lat", "degrees_north",mask$xyCoords$y)
  timeString <-format(strptime(settings$startdate, format = "%Y-%m-%d", tz = "GMT"),format="%Y-%m-%d %T")
  timeArray <-c(0:(settings$intern$nrec_out-1)) * (24 / (24/settings$outstep))
  dimT <- ncdim_def("time", paste0("hours since ",timeString), timeArray, unlim = FALSE)

  ################
  data <- ncvar_def(name=names(settings$outputVars[1]), units='', dim=list(dimX,dimY,dimT), missval=FillValue, prec="float")
  dataAllVars <- list(data)[rep(1,length(settings$outputVars))]
  for (iVar in 1:length(settings$outputVars))
  {
    dataAllVars[[iVar]] <- ncvar_def(name=names(settings$outputVars[iVar]), units='', dim=list(dimX,dimY,dimT), missval=FillValue, prec="float")
  }

  ## SAVE AS NC-DATA
  print(paste0("Writing: ", settings$outfile))
  # ncid <- nc_create_par(settings$outfile, dataAllVars,force_v4=TRUE)
  ncid <- nc_create(settings$outfile, dataAllVars, force_v4=TRUE)

  ncatt_put( ncid, "lon", "standard_name", "longitude")
  ncatt_put( ncid, "lon", "long_name",     "Longitude")
  ncatt_put( ncid, "lon", "axis",          "X")
  ncatt_put( ncid, "lat", "standard_name", "latitude")
  ncatt_put( ncid, "lat", "long_name",     "Latitude")
  ncatt_put( ncid, "lat", "axis",          "Y")
  ncatt_put( ncid, "time", "standard_name", "time")
  ncatt_put( ncid, "time", "calendar",     "standard")
  for (iVar in 1:length(settings$outputVars))
  {
    ncatt_put( ncid, names(settings$outputVars[iVar]), "standard_name", names(settings$outputVars[iVar]))
    ncatt_put( ncid, names(settings$outputVars[iVar]), "vic_name", settings$outputVars[[iVar]]$VICName)
    ncatt_put( ncid, names(settings$outputVars[iVar]), "units", settings$outputVars[[iVar]]$units)
  }
  # ## Global Attributes
  ncatt_put( ncid, 0, "NetcdfCreatationDate", as.character(Sys.Date()))

  ## Close Netcdf file
  nc_close(ncid)
}

setSubDomains <- function(settings, mask, nPart = NULL) {
  maxPart <- length(mask$xyCoords$y)
  nCells<-length(mask$xyCoords$x)*length(mask$xyCoords$y)

  if (nPart > maxPart) {
    print(paste0("nPart (", nPart, ") is greater than maxPart (", maxPart, "), nPart is now adapted to maxPart: ", maxPart))
    print(paste0("Be warned that this can case memory issues!"))
    nPart <- maxPart
  }

  nActive <- length(mask$Data[!is.na(mask$Data)])
  print(paste0("Total cells: ", nCells, ", Active Cells: ", nActive, ", nParts: ", nPart))

  partSize <- ceiling(nCells/nPart)
  partSize <- ceiling(partSize/length(mask$xyCoords$x)) * length(mask$xyCoords$x)

  part <- list(sx = 1,
               nx = length(mask$xyCoords$x),
               sy = NULL,
               ny = partSize/length(mask$xyCoords$x),
               slon = min(mask$xyCoords$x),
               elon = max(mask$xyCoords$x),
               slat = NULL,
               elat = NULL)
  parts <- list(part)[rep(1,nPart)]

  for (iPart in 1:(nPart)) {
    parts[[iPart]]$sy <- (iPart -1) * (partSize/length(mask$xyCoords$x)) +1
    parts[[iPart]]$slat <- mask$xyCoords$y[parts[[iPart]]$sy]
    parts[[iPart]]$elat <- mask$xyCoords$y[iPart * (partSize/length(mask$xyCoords$x))]
  }
  parts[[nPart]]$elat <- mask$xyCoords$y[length(mask$xyCoords$y)]
  parts[[nPart]]$ny <- (length(mask$xyCoords$y) - parts[[nPart]]$sy) + 1

  return(parts)
}

setSubDomains_old <- function(settings, mask, partSize = NULL) {
  # mask <- elevation
  # partSize <- 50
  # partBased <- "lon"
  # partSize <-NULL
  nCells<-length(mask$xyCoords$x)*length(mask$xyCoords$y)

  if (is.null(partSize)) {
    print(paste0("Partsize not defined, so using max: ", nCells, " (only 1 part)"))
    partSize <- nCells
  }
  partSizeOld <- partSize
  partSize <- ceiling(partSize/length(mask$xyCoords$x)) * length(mask$xyCoords$x)
  if (partSizeOld != partSize) {
    print(paste0("partsize should be a multiplication of nx(", length(mask$xyCoords$x),"), so changed to: ", partSize))
  }

  nActive <- length(mask$Data[!is.na(mask$Data)])
  nPart<-ceiling(nCells/partSize)
  print(paste0("Total cells: ", nCells, ", Active Cells: ", nActive, ", nParts: ", nPart))

  part <- list(sx = 1,
               nx = length(mask$xyCoords$x),
               sy = NULL,
               ny = partSize/length(mask$xyCoords$x),
               slon = min(mask$xyCoords$x),
               elon = max(mask$xyCoords$x),
               slat = NULL,
               elat = NULL)
  parts <- list(part)[rep(1,nPart)]

  for (iPart in 1:(nPart)) {
    parts[[iPart]]$sy <- (iPart -1) * (partSize/length(mask$xyCoords$x)) +1
    parts[[iPart]]$slat <- mask$xyCoords$y[parts[[iPart]]$sy]
    parts[[iPart]]$elat <- mask$xyCoords$y[iPart * (partSize/length(mask$xyCoords$x))]
  }
  parts[[nPart]]$elat <- mask$xyCoords$y[length(mask$xyCoords$y)]
  parts[[nPart]]$ny <- (length(mask$xyCoords$y) - parts[[nPart]]$sy) + 1

  return(parts)
}
readForcing <- function(settings, iPart) {
  forcing_dataR <- list()
  for (i in 1:length(settings$inputVars)) {
    forcing_dataR[[i]]<- array(0, dim=c(settings$intern$nrec_in,settings$parts[[iPart]]$ny,settings$parts[[iPart]]$nx))
  }
  ## Read data
  sx <- settings$parts[[iPart]]$sx
  sy <- settings$parts[[iPart]]$sy
  nx <- settings$parts[[iPart]]$nx
  ny <- settings$parts[[iPart]]$ny
  for (iVar in 1:length(settings$inputVars)) {
    forcing_dataR[[iVar]][] <- ncLoad(file = settings$inputVars[[iVar]]$ncFileName,
                                      varName = settings$inputVars[[iVar]]$ncName,
                                      lonlatbox = c(settings$parts[[iPart]]$slon,
                                                    settings$parts[[iPart]]$elon,
                                                    settings$parts[[iPart]]$slat,
                                                    settings$parts[[iPart]]$elat),
                                      timesteps = c(1:settings$intern$nrec_in))$Data
  }
  return(forcing_dataR)
}

readForcingAll <- function(part, settings, mask) {
  # mask<-elevation
  forcing_dataR <- list()
  for (i in 1:length(settings$inputVars)) {
    forcing_dataR[[i]]<- array(0, dim=c(settings$intern$nrec_in,  part$ny,  part$nx))
  }
  ## Read data
  for (iVar in 1:length(settings$inputVars)) {
    forcing_dataR[[iVar]][] <- ncLoad(file = settings$inputVars[[iVar]]$ncFileName,
                                      varName = settings$inputVars[[iVar]]$ncName,
                                      # lonlatbox = c(settings$lonlatbox[1],
                                      #               settings$lonlatbox[2],
                                      #               settings$lonlatbox[3],
                                      #               settings$lonlatbox[4]),
                                      lonlatbox = c(part$slon,
                                                    part$elon,
                                                    part$slat,
                                                    part$elat),
                                      timesteps = c(1:settings$intern$nrec_in))$Data
  }
  return(forcing_dataR)
}

selectForcingCell <-function(settings, forcing_dataRTotal, ix, iy) {
  forcing_dataR<-NULL

  for (i in 1:24) {
    forcing_dataR[[i]]<- array(0, dim=c(settings$intern$nrec_in))
  }
  ## Read data
  for (iVar in 1:length(settings$inputVars)) {
    forcing_dataR[[settings$inputVars[[iVar]]$vicIndex+1]][] <- forcing_dataRTotal[[iVar]][,iy,ix]
  }
  return(forcing_dataR)
}




initParams <- function(startdate = NULL, enddate = NULL, outstep = 24, lonlatbox = NULL) {
  intern <- list (
    lonlatboxindex = NULL,
    lons = NULL,
    lats = NULL,
    nx = NULL,
    ny = NULL,
    startx = NULL,
    starty = NULL,
    nrec_in = NULL,
    nrec_out = NULL
  );
  toCpp <- list (
    outstep = outstep,
    startyear = NULL,
    startmonth = NULL,
    startday = NULL,
    istartdate = NULL,
    endyear = NULL,
    endmonth = NULL,
    endday = NULL,
    ienddate = NULL,
    nForcing = NULL,
    forcingIds = NULL,
    alma_input = FALSE
  )
  params <- list(
    lonlatbox = lonlatbox,
    startdate = as.Date(startdate),
    enddate = as.Date(enddate),
    outstep = outstep,
    toCpp = toCpp
  )
  return(params)
}

addNetcdfData2Params <- function(variableInfo, params) {
  startDate <- params$startdate
  endDate <- params$enddate
  lonlatbox <- params$lonlatbox

  ncid <- nc_open(variableInfo[[1]]$ncFileName)
  nt<-ncid$dim$time$len

  lonlatboxindex <- rep(NA,4)
  lonlatboxindex[1] <- match(params$lonlatbox[1],ncid$dim$lon$vals)
  lonlatboxindex[2] <- match(params$lonlatbox[2],ncid$dim$lon$vals)
  lonlatboxindex[3] <- match(params$lonlatbox[3],ncid$dim$lat$vals)
  lonlatboxindex[4] <- match(params$lonlatbox[4],ncid$dim$lat$vals)

  ## getting time indexes and info
  timesteps <- ncvar_get(ncid, "time")
  tunits <- ncatt_get(ncid, "time", "units")
  tustr <- strsplit(tunits$value, " ")
  dUnit <- unlist(tustr)[3]
  allDates <- as.Date(timesteps, origin=dUnit) ## minus 2 because 0001 is officially an invalid year!!!
  iStartDate <- which(allDates == startDate)
  iEndDate <- which(allDates == endDate)
  nrec_in <- iEndDate - iStartDate + 1
  print(paste("start date: ", allDates[iStartDate], " end date: ", allDates[iEndDate]))
  ymdStart<-as.numeric(strsplit(as.character(allDates[iStartDate]), "-")[[1]])
  ymdEnd<-as.numeric(strsplit(as.character(allDates[iEndDate]), "-")[[1]])
  rm(timesteps,tunits,tustr,dUnit,allDates)


  ## Define nx, ny, startx, starty
  nx<-abs(lonlatboxindex[2]-lonlatboxindex[1])+1
  ny<-abs(lonlatboxindex[4]-lonlatboxindex[3])+1
  startx<-min(lonlatboxindex[1:2])
  starty<-min(lonlatboxindex[3:4])

  ## Add to params
  params$intern$lonlatboxindex = lonlatboxindex
  params$intern$lons = ncid$dim$lon$vals[startx:(startx+nx-1)]
  params$intern$lats = ncid$dim$lat$vals[starty:(starty+ny-1)]
  params$intern$nx = nx
  params$intern$ny = ny
  params$intern$startx = startx
  params$intern$starty = starty
  params$toCpp$startyear = ymdStart[1]
  params$toCpp$startmonth = ymdStart[2]
  params$toCpp$startday = ymdStart[3]
  params$toCpp$istartdate = iStartDate
  params$toCpp$endyear = ymdEnd[1]
  params$toCpp$endmonth = ymdEnd[2]
  params$toCpp$endday = ymdEnd[3]
  params$toCpp$ienddate = iEndDate
  params$intern$nrec_in = nrec_in
  if ( exists("alma", where = variableInfo[[1]]))  params$toCpp$alma_input = variableInfo[[1]]$alma

  ## Add forcing info to params
  nForcing = length(variableInfo)
  forcingIds = array(NA, dim = c(nForcing))
  for (iFor in 1:nForcing) {
    forcingIds[iFor] =variableInfo[[iFor]]$vicIndex
  }

  params$toCpp$nForcing = length(variableInfo)
  params$toCpp$forcingIds = forcingIds

  nc_close(ncid)
  return(params)
}

readForcingNetCDF <- function(variableInfo, params) {
  ## Read data
  nx <- params$intern$nx
  ny <- params$intern$ny
  startx <- params$intern$startx
  starty <- params$intern$starty
  forcing_dataR <- list()
  for (i in 1:24) {
    forcing_dataR[[i]]<- array(0, dim=c(nx,ny,params$intern$nrec_in))
  }
  for (iVar in 1:length(variableInfo)) {
    ncid <- nc_open(variableInfo[[iVar]]$ncFileName)
    forcing_dataR[[variableInfo[[iVar]]$vicIndex+1]][] <- ncvar_get(nc = ncid,
                                                                    varid =  variableInfo[[iVar]]$ncName,
                                                                    start = c(startx,
                                                                              starty,
                                                                              params$toCpp$istartdate),
                                                                    count = c(nx,ny,params$intern$nrec_in)
    );
    nc_close(ncid)

    ## Reverse the lat array
    if (params$intern$lonlatboxindex[4] > params$intern$lonlatboxindex[3]) {
      # forcing_dataR[[variableInfo[[iVar]]$vicIndex+1]][] <- apply(forcing_dataR[[variableInfo[[iVar]]$vicIndex+1]][], 3, rev)
    }
  }


  return(forcing_dataR)
}
readElevationNetCDF <- function(elevation, params) {
  ## Read data
  nx <- params$intern$nx
  ny <- params$intern$ny
  startx <- params$intern$startx
  starty <- params$intern$starty
  elevation$data <- array(0, dim=c(nx,ny))
  ncid <- nc_open(elevation$ncFileName)
  elevation$data[] <- ncvar_get(nc = ncid,
                                varid =  elevation$ncName,
                                start = c(startx,
                                          starty),
                                count = c(nx,ny)
  );
  nc_close(ncid)


  return(elevation)
}

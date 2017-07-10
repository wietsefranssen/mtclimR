library(ncdf4)

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

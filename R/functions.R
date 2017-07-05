library(ncdf4)

initParams <- function(startdate = NULL, enddate = NULL, outstep = 24, lonlatbox = NULL) {
  details <- list (
    outstep = outstep,
    lonlatboxindex = NULL,
    startyear = NULL,
    startmonth = NULL,
    startday = NULL,
    istartdate = NULL,
    endyear = NULL,
    endmonth = NULL,
    endday = NULL,
    ienddate = NULL,
    nrec_in = NULL,
    nForcing = NULL,
    forcingIds = NULL
  )
  params <- list(
    lonlatbox = lonlatbox,
    startdate = as.Date(startdate),
    enddate = as.Date(enddate),
    outstep = outstep,
    internal = details
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
  nc_close(ncid)

  ## Add to params
  params$internal$lonlatboxindex = lonlatboxindex
  params$internal$startyear = ymdStart[1]
  params$internal$startmonth = ymdStart[2]
  params$internal$startday = ymdStart[3]
  params$internal$istartdate = iStartDate
  params$internal$endyear = ymdEnd[1]
  params$internal$endmonth = ymdEnd[2]
  params$internal$endday = ymdEnd[3]
  params$internal$ienddate = iEndDate
  params$internal$nrec_in = nrec_in

  ## Add forcing info to params
  nForcing = length(variableInfo)
  forcingIds = array(NA, dim = c(nForcing))
  for (iFor in 1:nForcing) {
    forcingIds[iFor] =variableInfo[[iFor]]$vicIndex
  }

  params$internal$nForcing = length(variableInfo)
  params$internal$forcingIds = forcingIds

  return(params)
}

readForcingNetCDF <- function(variableInfo, params) {
  ## Read data
  forcing_dataR <- list()
  for (i in 1:24) {
    forcing_dataR[[i]]<- array(0, dim=c(params$internal$nrec_in))
  }
  for (iVar in 1:length(variableInfo)) {
    ncid <- nc_open(variableInfo[[iVar]]$ncFileName)
    forcing_dataR[[variableInfo[[iVar]]$vicIndex+1]] <- ncvar_get(nc = ncid,
                                                                  varid =  variableInfo[[iVar]]$ncName,
                                                                  start = c(params$internal$lonlatboxindex[1],
                                                                            params$internal$lonlatboxindex[3],
                                                                            params$internal$istartdate),
                                                                  count = c(1,1,params$internal$nrec_in-1)
                                                                  );
    nc_close(ncid)
  }

  return(forcing_dataR)
}

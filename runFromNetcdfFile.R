rm (list = ls())
library(ncdf4)

variableInfo <- list(
  pr         = list(ncName = "prAdjust",        vicIndex = 9),
  tasmin     = list(ncName = "tasminAdjust",    vicIndex = 17),
  tasmax     = list(ncName = "tasmaxAdjust",    vicIndex = 16),
  wind       = list(ncName = "windAdjust",      vicIndex = 20)
 # shortwave  = list(ncName = "tasmaxAdjust", vicIndex = 14),
 # longwave   = list(ncName = "tasmaxAdjust",  vicIndex = 6)
)

##
startDate <- as.Date("1950-01-01")
endDate <- as.Date("1950-12-31")
OUT_STEP  <- 3
ix <- 4
iy <- 1

## Read NetCDF
ncid <- nc_open("./data/domain_elev_Mekong.nc")
elev <- ncvar_get(nc = ncid,varid =  "elev" )
nc_close(ncid)

ncid <- nc_open("./data/merged_Mekong.nc")
nt<-ncid$dim$time$len

## getting time indexes and info
timesteps <- ncvar_get(ncid, "time")
tunits <- ncatt_get(ncid, "time", "units")
tustr <- strsplit(tunits$value, " ")
dUnit <- unlist(tustr)[3]
allDates <- as.Date(timesteps, origin=dUnit) ## minus 2 because 0001 is officially an invalid year!!!
iStartDate <- which(allDates == startDate)
iEndDate <- which(allDates == endDate)
nDates <- iEndDate - iStartDate + 1
print(paste("start date: ", allDates[iStartDate], " end date: ", allDates[iEndDate]))
ymdStart<-as.numeric(strsplit(as.character(allDates[iStartDate]), "-")[[1]])
ymdEnd<-as.numeric(strsplit(as.character(allDates[iEndDate]), "-")[[1]])
rm(timesteps,tunits,tustr,dUnit,allDates,iStartDate,iEndDate)

## Fill params
nForcing = length(variableInfo)
forcingIds = array(NA, dim = c(nForcing))
for (iFor in 1:nForcing) {
  forcingIds[iFor] =variableInfo[[iFor]]$vicIndex
}
params <- list(
  dt = OUT_STEP,
  SNOW_STEP = OUT_STEP,
  startyear = ymdStart[1],
  startmonth = ymdStart[2],
  startday = ymdStart[3],
  endyear = ymdEnd[1],
  endmonth = ymdEnd[2],
  endday = ymdEnd[3],
  nForcing = length(variableInfo),
  forcingIds = forcingIds
)

forcing_dataR <- list()
for (i in 1:24) {
  forcing_dataR[[i]]<- array(0, dim=c(nDates))
}
for (iVar in 1:length(variableInfo)) {
  forcing_dataR[[variableInfo[[iVar]]$vicIndex+1]] <- ncvar_get(nc = ncid, varid =  variableInfo[[iVar]]$ncName , start = c(ix,iy,1), count = c(1,1,nDates));
}

nc_close(ncid)

## Run!
print("run!")
output<- mtclimRun(forcing_dataR = forcing_dataR, settings = params)
nrecs_out <- length(output$out_data$OUT_PREC)
print("done!")

## Write output to file
file.remove("test")
for (i in 1:nrecs_out) {
  write(sprintf("%4.4f\t%7.4f\t%7.4f\t%7.4f\t%12.4f\t%12.4f\t%12.4f\t%8.4f\t%8.4f\t%7.4f\t%7.4f\t%7.4f\t%7.4f",
                output$out_data[[1]][i],
                output$out_data[[2]][i],
                output$out_data[[3]][i],
                output$out_data[[4]][i],
                output$out_data[[5]][i],
                output$out_data[[6]][i],
                output$out_data[[7]][i],
                output$out_data[[8]][i],
                output$out_data[[9]][i],
                output$out_data[[10]][i],
                output$out_data[[11]][i],
                output$out_data[[12]][i],
                output$out_data[[13]][i]),
        file = "test", append = T)
}


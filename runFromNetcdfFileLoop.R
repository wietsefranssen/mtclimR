rm (list = ls())
library(ncdf4) # DEPENDCY DEFINE!!
library(foreach) # DEPENDCY DEFINE!!
library(doMC)
registerDoMC(2)  #change the 2 to your number of CPU cores

variableInfo <- list(
  pr         = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/pr_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "prAdjust",        alma = TRUE, vicIndex = 9),
  tasmin     = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/tasmin_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "tasminAdjust",     vicIndex = 17),
  tasmax     = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/tasmax_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "tasmaxAdjust",     vicIndex = 16),
  wind       = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/wind_bced_1960_1999_gfdl-esm2m_hist_1950.nc",        ncName = "windAdjust",       vicIndex = 20)
  # shortwave  = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse",        ncName = "tasmaxAdjust", vicIndex = 14),
  # longwave   = list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse",        ncName = "tasmaxAdjust",  vicIndex = 6)
)
elevation  <- list(ncFileName = "/home/wietse/Documents/Projects/VIC_model/MetSim/dataWietse/WFDEI-elevation.nc", ncName = "elevation")

variableInfo <- list(
  pr         = list(ncFileName = "./data/merged_Mekong.nc",      alma = FALSE,  ncName = "prAdjust",        vicIndex = 9),
  tasmin     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasminAdjust",    vicIndex = 17),
  tasmax     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",    vicIndex = 16),
  wind       = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "windAdjust",      vicIndex = 20)
  # shortwave  = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust", vicIndex = 14),
  # longwave   = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",  vicIndex = 6)
)
elevation <- list(ncFileName = "./data/domain_elev_Mekong.nc", ncName = "elev")

start.time <- Sys.time()
## Init params
params <- initParams(startdate = "1950-01-01",
                     enddate = "1950-5-31",
                     outstep = 24,
                     lonlatbox = c(92.25, 110.25, 7.25, 36.25))
params <- initParams(startdate = "1950-01-01",
                     enddate = "1950-12-31",
                     outstep = 24,
                     lonlatbox = c(-179.75, 179.75, -89.75, -30.25))
# params <- initParams(startdate = "1950-01-01",
#                      enddate = "1950-12-31",
#                      outstep = 24,
#                      lonlatbox = c(97.75, 98.75, 7.25, 10.25))
params <- initParams(startdate = "1950-01-01",
                     enddate = "1950-12-31",
                     outstep = 6,
                     lonlatbox = c(92.25, 110.25, 7.25, 36.25))
# #
## Update params (TODO: checks still need to be added)
params <- addNetcdfData2Params(variableInfo, params)

## READ FORCING DATA
forcing_dataRTotal <- readForcingNetCDF(variableInfo, params)
print("Reading forcing done!")
# forcing_dataR[[10]][,,1]

## READ ELEVATION DATA
elevation <- readElevationNetCDF(elevation, params)
print("Reading elevation done!")
# ncid <- nc_open(elevation$ncFileName)
# elevation$data <- ncvar_get(nc = ncid,varid =  elevation$ncName )
# nc_close(ncid)


## Run!
print("run!")

# ny<-10
forcing_dataR<-NULL

nOutVars <- 13

outArrayDefined <- FALSE
# foreach(iy = 1:ny) %dopar% {
for (iy in 1:params$intern$ny) {
  print(paste0("iy: ", iy, "/", params$intern$ny))
  for (ix in 1:params$intern$nx) {
    # print(paste(iy,ix))
    # if (!is.na(forcing_dataRTotal[[1]][ix,iy,1])) {
    if (!is.na(elevation$data[ix,iy])) {
      for (i in 1:length(forcing_dataRTotal)) {
        # print(i)
        forcing_dataR[[i]]<-forcing_dataRTotal[[i]][ix,iy,]
        params$toCpp$elevation<-elevation$data[ix,iy]
        params$toCpp$lon<-params$intern$lons[ix]
        params$toCpp$lat<-params$intern$lats[iy]
      }
      ## MLTCLIM
      output<- mtclimRun(forcing_dataR = forcing_dataR, settings = params$toCpp)

      ## Define output array
      if (!outArrayDefined) {
        params$intern$nrec_out <- length(output$out_data$OUT_PREC)
        el <- array(NA, dim = c(params$intern$nx,params$intern$ny,params$intern$nrec_out))
        toNetCDFData <- list(el)[rep(1,nOutVars)]
        outArrayDefined <- TRUE
      }

      ## Fill output array
      for (iVar in 1:nOutVars)
      {
        toNetCDFData[[iVar]][ix,iy,] <- output$out_data[[iVar]]
        # print(iVar)
      }
    }
  }
}
rm (ix,iy)


## CREATE NETCDF
FillValue <- 1e20

## Define dimensions
dimX <- ncdim_def("lon", "degrees_east", params$intern$lons)
dimY <- ncdim_def("lat", "degrees_north",params$intern$lats)
timeString <-format(strptime(params$startdate, format = "%Y-%m-%d", tz = "GMT"),format="%Y-%m-%d %T")
timeArray <-c(0:(params$intern$nrec_out-1)) * (24 / (24/params$outstep))
# print(timeArray)
dimT <- ncdim_def("time", paste0("hours since ",timeString), timeArray, unlim = FALSE)

iVar<-1
outFile<-"out.nc"
# data <- ncvar_def(name=names(output$out_data)[iVar], units='', dim=list(dimX,dimY,dimT), missval=FillValue, prec="float",chunksizes = c(length(RData$xyCoords$x),length(RData$xyCoords$y),1), compression=4)
################
data <- ncvar_def(name=names(output$out_data)[iVar], units='', dim=list(dimX,dimY,dimT), missval=FillValue, prec="float")
dataAllVars <- list(data)[rep(1,nOutVars)]
for (iVar in 1:nOutVars)
{
  dataAllVars[[iVar]] <- ncvar_def(name=names(output$out_data)[iVar], units='', dim=list(dimX,dimY,dimT), missval=FillValue, prec="float")
}

##########
## SAVE AS NC-DATA
print(paste0("Writing: ", outFile))

# ncid <- nc_create(outFile, list(data,data1))
ncid <- nc_create(outFile, dataAllVars)
for (iVar in 1:nOutVars)
{
  ncvar_put(ncid, dataAllVars[[iVar]], toNetCDFData[[iVar]])
  # print(iVar)
}

ncatt_put( ncid, "lon", "standard_name", "longitude")
ncatt_put( ncid, "lon", "long_name",     "Longitude")
ncatt_put( ncid, "lon", "axis",          "X")
ncatt_put( ncid, "lat", "standard_name", "latitude")
ncatt_put( ncid, "lat", "long_name",     "Latitude")
ncatt_put( ncid, "lat", "axis",          "Y")
ncatt_put( ncid, "time", "standard_name", "time")
ncatt_put( ncid, "time", "calendar",     "standard")
ncatt_put( ncid, names(output$out_data)[iVar], "standard_name", names(output$out_data)[iVar])

## Global Attributes
# ## Get all global attributes from RData and put them in the NetCDF file
# attributeList<-attributes(RData)
# attributeList["names"]<-NULL
# for (iAttribute in 1:length(attributeList)) {
#   ncatt_put( ncid, 0, names(attributeList)[iAttribute], as.character(attributeList[iAttribute[]]))
# }
ncatt_put( ncid, 0, "NetcdfCreatationDate", as.character(Sys.Date()))

## Close Netcdf file
nc_close(ncid)

# ## Write output to file
# file.remove("test")
# for (i in 1:params$intern$nrec_out) {
#   write(sprintf("%4.4f\t%7.4f\t%7.4f\t%7.4f\t%12.4f\t%12.4f\t%12.4f\t%8.4f\t%8.4f\t%7.4f\t%7.4f\t%7.4f\t%7.4f",
#                 output$out_data[[1]][i],
#                 output$out_data[[2]][i],
#                 output$out_data[[3]][i],
#                 output$out_data[[4]][i],
#                 output$out_data[[5]][i],
#                 output$out_data[[6]][i],
#                 output$out_data[[7]][i],
#                 output$out_data[[8]][i],
#                 output$out_data[[9]][i],
#                 output$out_data[[10]][i],
#                 output$out_data[[11]][i],
#                 output$out_data[[12]][i],
#                 output$out_data[[13]][i]),
#         file = "test", append = T)
# }
# rm(i)
#
end.time <- Sys.time()
time.taken <- end.time - start.time
print(time.taken); rm(start.time, end.time, time.taken)

rm (list = ls())
# library(ncdf4) // DEPENDCY DEFINE!!

variableInfo <- list(
  pr         = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "prAdjust",        vicIndex = 9),
  tasmin     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasminAdjust",    vicIndex = 17),
  tasmax     = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",    vicIndex = 16),
  wind       = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "windAdjust",      vicIndex = 20)
  # shortwave  = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust", vicIndex = 14),
  # longwave   = list(ncFileName = "./data/merged_Mekong.nc",        ncName = "tasmaxAdjust",  vicIndex = 6)
)

## Init params
params <- initParams(startdate = "1950-01-01",
                     enddate = "1950-12-31",
                     outstep = 24,
                     lonlatbox = c(93.75, 93.75, 7.25, 7.25))

## Update params (TODO: checks still need to be added)
params <- addNetcdfData2Params(variableInfo, params)

# ## Read NetCDF
# ncid <- nc_open("./data/domain_elev_Mekong.nc")
# elev <- ncvar_get(nc = ncid,varid =  "elev" )
# nc_close(ncid)

forcing_dataR <- readForcingNetCDF(variableInfo, params)

## Run!
print("run!")
output<- mtclimRun(forcing_dataR = forcing_dataR, settings = params$internal)
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
rm(i)


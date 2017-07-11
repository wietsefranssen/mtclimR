#' @keywords internal
.PRINT_INFO<-FALSE

#' Plot a simple map
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
## "keywords internal": hide the fuction from the environment
#' @keywords internal
## "export": export the help and make it available
#' @export
plotje <-function(data, title = NULL) {
  if (is.null(title)) {
    if (!is.na(data$Dates$start[1])) {
      title=paste0(data$Variable$longName, " [",data$Variable$units,"]\n(" ,data$Dates$start[1], ")")
    } else {
      title=paste0(data$Variable$longName, " [",data$Variable$units,"]")
    }
  }

  if (length(dim(data$Data)) > 2) {
    dataTmp<-aperm(data$Data[1,,],c(2,1))
    cat("Plotting first timestep\n")
  } else {
    dataTmp<-aperm(data$Data,c(2,1))
  }
  image.plot(data$xyCoords$x,data$xyCoords$y,dataTmp, asp = 1, main = title, xlab = '', ylab = '')
  world(add = TRUE)
}

#' Make a image of the NetCDF data
#' @details Uses \code{\link{ncLoad}}
#' @return Nothing
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
#' @export
ncPlot <-function(file, varName = NULL, lonlatbox = NULL, timesteps = NULL, z = NULL, convertUnit = NULL) {
  data<-ncLoad(file, varName, lonlatbox, timesteps, z, convertUnit)
  plotje(data)
}

#' Make a image of the NetCDF data
#' @details BLAAT.
#' @return Nothing
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
convertUnit <-function(data) {
  data$Variable$units<-gsub("kg m-2", "mm" ,data$Variable$units)
  data$Variable$units<-gsub("kg/m2", "mm" ,data$Variable$units)
  if (ud.are.convertible(data$Variable$units,"mm day-1")) {
    data$Data[]<-ud.convert(data$Data[],data$Variable$units,"mm day-1")
    data$Variable$units<-"mm day-1"
  }
  if (ud.are.convertible(data$Variable$units,"Celsius")) {
    data$Data[]<-ud.convert(data$Data[],data$Variable$units,"Celsius")
    data$Variable$units<-"Celsius"
  }
  return(data)
}



#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
rDataStructure <-function() {
  rData<-NULL
  rData$Variable$varName<-NA
  rData$Data<-NA
  rData$xyCoords$x<-NA
  rData$xyCoords$y<-NA
  rData$Dates$start<-NA
  rData$Dates$end<-NA
  return(rData)
}
#' Plot a simple map
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
#' @export
r2ggplotDataStructure <-function(data, timestep = 1) {
  lonList = array(1, dim=c(length(data$xyCoords$x)*length(data$xyCoords$y)))
  latList = array(1, dim=c(length(data$xyCoords$x)*length(data$xyCoords$y)))
  i=1
  #for(y in 1:dim(data$xyCoords$y))
  for(x in 1:length(data$xyCoords$x))
  {
    #for(x in 1:dim(data$xyCoords$x))
    for(y in 1:length(data$xyCoords$y))
    {
      lonList[i]=data$xyCoords$x[x];
      latList[i]=data$xyCoords$y[y];
      i=i+1
    }
  }

  ggplotData<-NULL
  ggplotData$lon=lonList
  ggplotData$lat=latList

  if (length(dim(data$Data))==2) {
    ggplotData$data=c(data$Data)
  } else if (length(dim(data$Data))==3) {
    ggplotData$data=c(data$Data[timestep,,])
  } else if (length(dim(data$Data))==4) {
    ggplotData$data=c(data$Data[1,timestep,,])
  }
  #ggplotFinal<-data.frame(ggplotData)
  ggplotFinal<-na.omit(data.frame(ggplotData))

  return(ggplotFinal)
}

#' Plot a simple map
#' @description Loads a NetCDF file as a R-data structure
#' @param file Name of the NetCDF file
#' @param varName Variable name of the NetCDF file
#' @param timesteps "all", Nothing or array of Timesteps
#' @details Loads a NetCDF file as a R-data structure.
#' @details eg: data <- ncLoad("~/DATA/example.nc4", lonlatbox = c(-24.25,37.75,33.25,60.25), varName = "tasmin", timesteps = c(1:3))
#' @details eg: ncPlot("~/DATA/example.nc4", lonlatbox = c(-24.25,37.75,33.25,60.25), varName = "tasmin", timesteps = c(1:3))
#' @return An object of class \code{WF}
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
#' @export
ggplotjeold <-function(rdata,lon = NULL, lat = NULL,title=NULL ) {
  if (is.null(title)) {

    if (!is.na(rdata$Dates$start[1])) {
      #     title=paste0(rdata$Variable$longName, " [",rdata$Variable$units,"]\n(" ,rdata$Dates$start[1], ")")

      title=paste0(rdata$Variable$longName, " [",rdata$Variable$units,"]\n(" ,as.character(format( as.Date(rdata$Dates$start[1]),format="%d %B %Y")), ")")
    } else {
      title=paste0(rdata$Variable$longName, " [",rdata$Variable$units,"]")
    }
  }

  data<-r2ggplotDataStructure(rdata)
  myPalette <- colorRampPalette(rev(brewer.pal(11, "Spectral")), space="Lab")
  #  myPalette <- colorRampPalette(rev(rainbow(100)), space="Lab")

  zp1 <- ggplot(data,
                aes(x = lon, y = lat, fill = data)) +
    ggtitle(title) + # plot title
    geom_tile(aes(x = lon, y = lat, fill = data)) +
    scale_fill_gradientn(colours = myPalette(100)) +
    #   scale_fill_gradientn(colours = myPalette(100), limits=c(-1,1)) +
    #zp1 <- zp1 + scale_x_discrete(expand = c(0, 0))
    #zp1 <- zp1 + scale_x_discrete(limits=c(ggdata$lon[1], ggdata$lon[length(ggdata$lon)]))
    #zp1 <- zp1 + scale_y_discrete(limits=c(ggdata$lat[1], ggdata$lat[length(ggdata$lat)]))
    #zp1 <- zp1 + coord_cartesian(xlim = c(-20, 40),ylim = c(ggdata$lat[1], ggdata$lat[length(ggdata$lat)]))
    #zp1 <- zp1 + ylim(ggdata$lat[1], ggdata$lat[length(ggdata$lat)])
    coord_equal() +
    theme_bw() +
    xlab(expression(Longitude * ' ' * degree * E * ' ')) +
    ylab(expression(Latitude * ' ' * degree * N * ' ')) +
    theme(legend.title=element_blank())

  # xlab(expression(Longitude * ' ' * degree * E * ' ')) +
  #   ylab(expression(Latitude * ' ' * degree * S * ' ')) +

  if (!is.null(lon) && !is.null(lat)) {
    zp1 <- zp1 + scale_shape_identity() + geom_point( colour="black", size = 10,  aes_string(x = lon, y = lat, shape=3))
    zp1 <- zp1 + geom_point( fill="red",colour="black", size = 6,  aes_string(x = lon, y = lat, shape=18))
  }
  print(zp1)
  return(zp1)
}
ggplotje <-function(rdata,lon = NULL, lat = NULL,title=NULL, barLimits=NULL, barDiff=FALSE, backgroudColor=NULL ) {
  if (is.null(title)) {

    if (!is.na(rdata$Dates$start[1])) {
      #     title=paste0(rdata$Variable$longName, " [",rdata$Variable$units,"]\n(" ,rdata$Dates$start[1], ")")

      title=paste0(rdata$Variable$longName, " [",rdata$Variable$units,"]\n(" ,as.character(format( as.Date(rdata$Dates$start[1]),format="%d %B %Y")), ")")
    } else {
      title=paste0(rdata$Variable$longName, " [",rdata$Variable$units,"]")
    }
  }
  if (!is.null(barLimits)) {
    rdata$Data[rdata$Data <= barLimits[1]]<- barLimits[1]
    rdata$Data[rdata$Data > barLimits[2]]<-barLimits[2]
  }

  data<-r2ggplotDataStructure(rdata)
  myPalette <- colorRampPalette(rev(brewer.pal(11, "Spectral")), space="Lab")
  #  myPalette <- colorRampPalette(rev(rainbow(100)), space="Lab")

  zp1 <- ggplot(data,
                aes(x = lon, y = lat, fill = data)) +
    ggtitle(title) + # plot title
    geom_tile(aes(x = lon, y = lat, fill = data)) +
    #zp1 <- zp1 + scale_x_discrete(expand = c(0, 0))
    #zp1 <- zp1 + scale_x_discrete(limits=c(ggdata$lon[1], ggdata$lon[length(ggdata$lon)]))
    #zp1 <- zp1 + scale_y_discrete(limits=c(ggdata$lat[1], ggdata$lat[length(ggdata$lat)]))
    #zp1 <- zp1 + coord_cartesian(xlim = c(-20, 40),ylim = c(ggdata$lat[1], ggdata$lat[length(ggdata$lat)]))
    #zp1 <- zp1 + ylim(ggdata$lat[1], ggdata$lat[length(ggdata$lat)])
    coord_equal() +
    theme_bw() +
    xlab(expression(Longitude * ' ' * degree * E * ' ')) +
    ylab(expression(Latitude * ' ' * degree * N * ' ')) +
    theme(legend.title=element_blank())

  # xlab(expression(Longitude * ' ' * degree * E * ' ')) +
  #   ylab(expression(Latitude * ' ' * degree * S * ' ')) +
  if (!is.null(backgroudColor)) {
    zp1 <- zp1 + theme(panel.background=element_rect(fill=backgroudColor))
  }

  library(scales)
  if (is.null(barLimits)) {
    if (barDiff==TRUE) {
      zp1 <- zp1 + scale_fill_gradient2(low=muted("red"), mid=("yellow"),high=muted("blue"))
    } else {
      zp1 <- zp1 + scale_fill_gradientn(colours = myPalette(100))
    }
  } else {
    if (barDiff==TRUE) {
      zp1 <- zp1 + scale_fill_gradient2(limits=barLimits, low=muted("red"),mid=("ghostwhite"), high=muted("blue"))
    } else {
      zp1 <- zp1 + scale_fill_gradientn(colours = myPalette(100),limits=barLimits)
    }
  }

  if (!is.null(lon) && !is.null(lat)) {
    zp1 <- zp1 + scale_shape_identity() + geom_point( colour="black", size = 10,  aes_string(x = lon, y = lat, shape=3))
    zp1 <- zp1 + geom_point( fill="red",colour="black", size = 6,  aes_string(x = lon, y = lat, shape=18))
  }
  print(zp1)
  return(zp1)
}

countryMap <- function(lon,lat){
  dum = map('worldHires',xlim = c(lon[1],lon[dim(lon)]), ylim = c(lat[1],lat[dim(lat)]), plot = FALSE)
  names(dum) <- c("lon", "lat", "range", "names")
  countries = data.frame(dum[c("lon","lat")])
  #countries$value = mean(d$value)
  countries$value = countries$lon
  #countries$value = NA
  return(countries)
}


ncCheck <-function(ncFile, variable) {
  result<-NULL
  result$xFlip<-FALSE
  result$yFlip<-FALSE
  result$dims<-FALSE
  result$dimids<-FALSE
  result$tArray<-FALSE

  lons  <-ncFile$dim$lon$vals
  lats  <-ncFile$dim$lat$vals
  times <-ncFile$dim$time$vals

  # what is the order of the variable dimensions:
  for (i in 1:length(ncFile$var[[variable]]$dimids)) {
    result$dims[i]<-ncFile$dim[[i]]$name
    result$dimids[i]<-ncFile$dim[[i]]$id
  }

  if (length(times) > 1) { result$tArray<-TRUE }
  if ((lons[2]-lons[1]) < 0) { result$xFlip<-TRUE }
  if ((lats[2]-lats[1]) < 0) { result$yFlip<-TRUE }
  return(result)
}

# @examples
# data <- ncLoad( file = paste0(find.package("WFRTools"),"./examples/data/example.nc4"))
# data <- ncLoad( file = "./examples/data/example.nc4")
# data <- ncLoad( file = "~/Desktop/gg/wfd_pr_1974.nc", varName = "pr")
#find.package("WFRTools")
#' ncLoad
#' @description Loads a NetCDF file as a R-data structure
#' @param file Name of the NetCDF file
#' @param varName Variable name of the NetCDF file
#' @param timesteps "all", Nothing or array of Timesteps
#' @details Loads a NetCDF file as a R-data structure.
#' @details eg: data <- ncLoad("~/DATA/example.nc4", lonlatbox = c(-24.25,37.75,33.25,60.25), varName = "tasmin", timesteps = c(1:3))
#' @details eg: ncPlot("~/DATA/example.nc4", lonlatbox = c(-24.25,37.75,33.25,60.25), varName = "tasmin", timesteps = c(1:3))
#' @return An object of class \code{WF}
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
#' @export
ncLoad <-function(file, varName = NULL, lonlatbox = NULL, timesteps = NULL, z = NULL, convertUnit=NULL, ncid = NULL) {

  ## first build an empty data structure
  data<-rDataStructure()

  ncFile<-ncid
  ## Open the netcdf file
  if (is.null(ncid)) {
    ncFile <- nc_open( file )
  }

  ## if no Variable is given then use the first one in the file
  if (is.null(varName)) {
    varName <- names(ncFile$var)[1]
    if (length(names(ncFile$var))>1){
      cat(paste0("loading the first variable: \"", names(ncFile$var[1]),"\"\n"))
      cat(paste0("Also available:\n"))
      for (i in 2:length(names(ncFile$var))){
        cat(paste0("\t\"", names(ncFile$var[i]),"\"\n"))
      }
    } else {
      cat(paste0("loading the variable: \"", names(ncFile$var[1]),"\"\n"))
    }
  }

  ## Do some sheck on the NetCDF file
  ncCheckResult<-ncCheck(ncFile = ncFile, variable = varName)

  ## SOME CHECKS:
  ## Check time
  if (!is.null(timesteps)) {
    if(is.null(ncFile$dim$time)) {
      stop(paste0("The file doesnot have valid timesteps, so do not give one!\n",
                  "Given:   ", min(timesteps), " till ", max(timesteps), "\n",
                  "Allowed: NONE!"), call. = FALSE)
    }
    if(!timesteps[1] == "all") {
      if(min(timesteps) < 1 || max(timesteps) > ncFile$dim$time$len) {
        stop(paste0("Timestep(s) out of range!\n",
                    "Given:   ", min(timesteps), " till ", max(timesteps), "\n",
                    "Allowed: 1 till ", ncFile$dim$time$len), call. = FALSE)
      }
    }
  }

  ## Check lon and lat
  if (!is.null(lonlatbox)) {
    minLon <-ncFile$dim$lon$vals[1]
    maxLon <-ncFile$dim$lon$vals[ncFile$dim$lon$len]
    if(lonlatbox[1] < minLon || lonlatbox[2] > maxLon) {
      stop(paste0("Longitudes out of range!\n",
                  "Given:   ", lonlatbox[1], " till ", lonlatbox[2], "\n",
                  "Allowed: ", minLon, " till ", maxLon), call. = FALSE)
    }
    minLat <-ncFile$dim$lat$vals[1]
    maxLat <-ncFile$dim$lat$vals[ncFile$dim$lat$len]
    if(lonlatbox[3] < minLat || lonlatbox[4] > maxLat) {
      ##WFF
      # stop(paste0("Latitudes out of range!\n",
      #             "Given:   ", lonlatbox[3], " till ", lonlatbox[4], "\n",
      #             "Allowed: ", minLat, " till ", maxLat), call. = FALSE)
    }
  }
  ## Fill lon and lat indexes
  if (is.null(lonlatbox)) {
    LonIdx <- c(1: ncFile$dim$lon$len)
    LatIdx <- c(1: ncFile$dim$lat$len)
  } else {
    LonIdx <- which( ncFile$dim$lon$vals >= lonlatbox[1] & ncFile$dim$lon$vals <= lonlatbox[2])
    LatIdx <- which( ncFile$dim$lat$vals >= lonlatbox[3] & ncFile$dim$lat$vals <= lonlatbox[4])
  }

  ## Fill time indexes
  if (is.null(timesteps)) {
    timeIdx<-c(1)
  } else if (timesteps[1] == "all") {
    timeIdx<-c(1:ncFile$dim$time$len)
  } else {
    timeIdx<-timesteps
  }

  ## Fill lon and lat
  data$xyCoords$x <- ncFile$dim$lon$vals[LonIdx]
  data$xyCoords$y <- ncFile$dim$lat$vals[LatIdx]
  ## FLIP DATA
  if (ncCheckResult$yFlip) {
    data$xyCoords$y <- rev(data$xyCoords$y)
  }
  if (ncCheckResult$xFlip) {
    data$xyCoords$x <- rev(data$xyCoords$x)
  }


  ## Fill time
  if(!is.null(ncFile$dim$time)) {
    NCtime          <- ncvar_get( ncFile, "time" ) [timeIdx]
    NCtimeAtt       <- ncatt_get( ncFile, "time", "units" )$value
    if (NCtimeAtt == "Years") {
      data$Dates$start<-format(strptime(paste0(NCtime[],"-01-01"), format="%Y-%m-%d", tz = "GMT"), format="%Y-%m-%d %T %Z")
      data$Dates$end<-format(strptime(paste0(NCtime[]+1,"-01-01"), format="%Y-%m-%d", tz = "GMT"), format="%Y-%m-%d %T %Z")
    } else { ## if attributes is "days since..."
      firstTime<-unlist(strsplit(NCtimeAtt, split=' ', fixed=TRUE))[3]
      firstTime<-strptime(firstTime, format = "%Y-%m-%d", tz = "GMT")
      data$Dates$start <- format(firstTime + (86400 * (NCtime+0)), format="%Y-%m-%d %T %Z")
      data$Dates$end   <- format(firstTime + (86400 * (NCtime+1)), format="%Y-%m-%d %T %Z")
    }
  }

  ## Fill data
  if(is.null(z)) {
    if(!is.null(ncFile$dim$time)) {
      data$Data <- ncvar_get( ncFile, varName, start=c(LonIdx[1],LatIdx[1],timeIdx[1]),count=c(length(LonIdx),length(LatIdx),length(timeIdx)))
    } else {
      data$Data <- ncvar_get( ncFile, varName, start=c(LonIdx[1],LatIdx[1]),count=c(length(LonIdx),length(LatIdx)))
    }
  } else {
    data$Data <- ncvar_get( ncFile, varName, start=c(LonIdx[1],LatIdx[1],z,timeIdx[1]),count=c(length(LonIdx),length(LatIdx),1,length(timeIdx)))
  }

  ##WFF
  if (length(dim(data$Data))==2) {
    data$Data<-aperm(data$Data, c(2,1))
    if(ncCheckResult$yFlip) data$Data<-apply(data$Data,2,rev)
    attr(data$Data,"dimensions") <- rev(ncCheckResult$dims[1:2])
  } else if (length(dim(data$Data))==3) {
    data$Data<-aperm(data$Data, c(3,2,1))
    if(ncCheckResult$yFlip) {
      for (iTime in 1:dim(data$Data)[1]) {
        data$Data[iTime,,]<-apply(data$Data[iTime,,],2,rev)
      }
    }
    attr(data$Data,"dimensions") <- rev(ncCheckResult$dims)
  }
  #attr(data$Data,"dimensions") <- rev(ncCheckResult$dims)
  data$Variable$varName  <- varName

  ## Fill attributes
  attTmp<-ncatt_get( ncFile, varName, "long_name" )
  if (attTmp$hasatt == TRUE) {
    data$Variable$longName <- attTmp$value
  } else {
    data$Variable$longName <- varName
  }
  attTmp<-ncatt_get( ncFile, varName, "units" )
  if (attTmp$hasatt == TRUE) {
    data$Variable$units <- attTmp$value
  } else {
    data$Variable$units <- "missing"
  }

  if(!is.null(convertUnit)) {
    data<- convertUnit(data)
  }

  ## Close the file
  if (is.null(ncid)) {
    nc_close(ncFile)
  }
  ## Print some info
  if (.PRINT_INFO == TRUE) {
    cat("Variables:\n")
    print(names(ncFile$var))
    print(ncCheckResult)
  }

  return(data)
}

#' ncLoad
#' @description Loads a NetCDF file as a R-data structure
#' @param file Name of the NetCDF file
#' @param varName Variable name of the NetCDF file
#' @details Loads a NetCDF file as a R-data structure.
#' @return An object of class \code{WF}
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
#' @export
makeMask <-function(data) {

  ## first build an empty data structure
  mask<-rDataStructure()

  ## lon and lats
  mask$xyCoords$x<-data$xyCoords$x
  mask$xyCoords$y<-data$xyCoords$y
  ## Maskout
  if (length(dim(data$Data)) > 2 ) {
    mask$Data<-data$Data[,,1]
  } else {
    mask$Data<-data$Data
  }
  mask$Data[!is.na(mask$Data)] <- 1
  naCells<-sum(is.na( mask$Data ))
  naValid<-sum(!is.na( mask$Data ))
  total<-naCells+naValid
  cat(paste0("mask succesfully made!\n") )
  cat(paste0("valid values: ", naValid, "\n" ) )
  cat(paste0("NA values:    ", naCells, "\n" ) )
  cat(paste0("Total:        ", total, "\n" ) )
  return(mask)
}

#' ncWrite
#' @description Write a R-data structure to a NetCDF file
#' @param file Name of the NetCDF file
#' @param varName Variable name of the NetCDF file
#' @details Loads a NetCDF file as a R-data structure.
#' @details eg: data <- ncLoad("~/DATA/example.nc4", lonlatbox = c(-24.25,37.75,33.25,60.25), varName = "tasmin", timesteps = c(1:3))
#' @details eg: ncPlot("~/DATA/example.nc4", lonlatbox = c(-24.25,37.75,33.25,60.25), varName = "tasmin", timesteps = c(1:3))
#' @return An object of class \code{WF}
#' @author Wietse Franssen \email{wietse.franssen@@wur.nl}
#' @keywords internal
#' @export
ncWrite <-function(outFile = "~/out.nc", rData = data) {
  timeString<-format(strptime(rData$Dates$start, format = "%Y-%m-%d", tz = "GMT"),format="%Y-%m-%d %T")
  #timeArray<-as.double(c(0:(length(rData$Dates$start)-1)))
  timeArray<-as.numeric(difftime(timeString,timeString[1],units="days"))
  dimT <- ncdim_def("time", paste0("days since ",timeString[1]), timeArray, unlim = FALSE)
  dimX <- ncdim_def("lon", "degrees_east", round(rData$xyCoords$x,2))
  dimY <- ncdim_def("lat", "degrees_north",round(rData$xyCoords$y,2))

  FillValue <- NA
  FillValue <- 1e20

  if (!is.null(rData$Variable$units)) {
    datavar <- ncvar_def( rData$Variable$varName, rData$Variable$units, list(dimT,dimY,dimX), FillValue, prec="float")
  } else {
    datavar <- ncvar_def( rData$Variable$varName, " ", list(dimT,dimY,dimX), FillValue, prec="float")
  }
  ncid_out <- nc_create(outFile, datavar)

  ## ADD ATTRIBUTES
  ncatt_put( ncid_out, 0, "institution", "Wageningen University and Research centre (WUR)")
  ncatt_put( ncid_out, 0, "contact", "Wietse Franssen <wietse.franssen@wur.nl>")

  ncatt_put( ncid_out, "lon", "standard_name", "longitude")
  ncatt_put( ncid_out, "lon", "long_name",     "Longitude")
  ncatt_put( ncid_out, "lon", "axis",          "X")
  ncatt_put( ncid_out, "lat", "standard_name", "latitude")
  ncatt_put( ncid_out, "lat", "long_name",     "Latitude")
  ncatt_put( ncid_out, "lat", "axis",          "Y")
  ncatt_put( ncid_out, "time", "standard_name", "time")
  ncatt_put( ncid_out, "time", "calendar",     "standard")
  ncatt_put( ncid_out, rData$Variable$varName, "long_name", rData$Variable$longName)
  ncatt_put( ncid_out, rData$Variable$varName, "_FillValue", FillValue)

  ncvar_put( ncid_out, datavar, rData$Data )

  nc_close(ncid_out)
}

# rm(list=ls())
# library(ncdf4)
# library(fields) # e.g: using the fields library
#
# plotje <-function(plottitle) {
#   image.plot(NClon,NClat,data, asp = 1, main = plottitle, xlab = '', ylab = '')
#   world(add = TRUE)
# }
#
# domainName<-c( "GHA",   "EU")
# lonmin<-    c( 27.75, -24.75)
# lonmax<-    c( 49.25,  39.75)
# latmin<-    c(-12.25,  33.25)
# latmax<-    c( 18.25,  71.75)
#
# iDomain<-1
#
# ## READ WFD NETCDF
# ncFile <- nc_open( "~/Desktop/gg/wfd_pr_1974.nc" )
# LonIdx <- which( ncFile$dim$lon$vals > lonmin[iDomain] | ncFile$dim$lon$vals < lonmax[iDomain])
# LatIdx <- which( ncFile$dim$lat$vals > latmin[iDomain] & ncFile$dim$lat$vals < latmax[iDomain])
# data <- ncvar_get( ncFile, "pr")[ LonIdx, LatIdx, 1]
# landmask<-data
# landmask[!is.na(landmask)] <- 1
# sum( !is.na( landmask ) )
# nc_close(ncFile)
#
# ## READ WFDEI NETCDF
# ncFile <- nc_open( "~/Desktop/gg/wfd_pr_1979.nc" )
# LonIdx <- which( ncFile$dim$lon$vals > lonmin[iDomain] | ncFile$dim$lon$vals < lonmax[iDomain])
# LatIdx <- which( ncFile$dim$lat$vals > latmin[iDomain] & ncFile$dim$lat$vals < latmax[iDomain])
# data <- ncvar_get( ncFile, "pr")[ LonIdx, LatIdx, 1]
# landmask[is.na(data)] <- NA
# sum( !is.na( landmask ) )
# nc_close(ncFile)
#
# ## READ SOIL NETCDF
# ncFile <- nc_open( "~/Desktop/gg/soil_GHA.nc" )
# LonIdx <- which( ncFile$dim$lon$vals > lonmin[iDomain] | ncFile$dim$lon$vals < lonmax[iDomain])
# LatIdx <- which( ncFile$dim$lat$vals > latmin[iDomain] & ncFile$dim$lat$vals < latmax[iDomain])
# data <- ncvar_get( ncFile, "stexture")[ LonIdx, LatIdx]
# landmask<-data
# landmask[!is.na(landmask)] <- 1
#
# landmask[is.na(data)] <- NA
# sum( !is.na( landmask ) )
# nc_close(ncFile)
#
# ## READ SOIL NETCDF
# nameFileNCin<-"~/Desktop/gg/soil_GHA.nc"
# ncid_in=nc_open(nameFileNCin)
# NCdata <- ncvar_get( ncid_in, "stexture")
# nc_close(ncid_in)
#
# ## Plot and add to LandMask
# data<-NCdata[,];plotje(plottitle = "WFDEI")
# landmask[is.na(data)] <- NA
# sum( !is.na( landmask ) )

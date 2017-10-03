main_netcdf<-function(settings = settings) {
  ## Start profiler
  start.time.total <- Sys.time()

  ## Register nr of cores
  print(paste("nCores: ", settings$system$nCores))
  registerDoParallel(cores=settings$system$nCores)

  ## Set outvars in settings
  settings$mtclim$nOut <- length(settings$outputVars)
  for (i in 1:length(settings$outputVars)) {
    settings$mtclim$outNames[i]<-settings$outputVars[[i]]$VICName
  }

  ## LOAD MASK/ELEVATION
  elevation <- ncLoad(file = settings$elevation$ncFileName,
                      varName = settings$elevation$ncName,
                      lonlatbox = settings$lonlatbox)
  mask<-elevation

  profile<-NULL
  ## makeOutputNetCDF
  makeNetcdfOut(settings, elevation)

  ## Calculate minimum number of parts based on the memory in the system
  ## And make pasts list.
  minNParts <- calcMinNParts(settings, elevation)
  parts <- setSubDomains(settings, elevation, nPart = minNParts)
  nPart <- length(parts)

  for (iPart in 1:length(parts)) {
    ## Change settings for current part
    part <- parts[[iPart]]
    settings$lonlatbox <- c(part$slon, part$elon, part$slat, part$elat)
    elevation <- ncLoad(file = settings$elevation$ncFileName,
                        varName = settings$elevation$ncName,
                        lonlatbox = settings$lonlatbox)

    ## Print part info
    cat(sprintf("\n> START Running part:%3.0d/%.0d, nx: %.0d, ny: %.0d\n", iPart, nPart, part$nx, part$ny))

    ## DEFINE OUTPUT ARRAY
    el <- array(NA, dim = c(part$nx, part$ny, settings$intern$nrec_out))
    toNetCDFData <- list(el)[rep(1,length(settings$outputVars))]
    rm(el)

    ## LOAD WHOLE DOMAIN FROM NETCDF
    profile$start.time.read <- Sys.time()
    forcing_dataRTotal <- readForcingAll(part, settings, elevation)
    profile$end.time.read <- Sys.time()

    ## Init progressbar
    pb <- txtProgressBar(min = 0, max = part$ny, initial = 0, char = "=",
                         width = NA, title, label, style = 3, file = "")

    profile$start.time.run <- Sys.time()
    ## CELL LOOP
    for (iy in 1:part$ny) {
      output<-foreach(ix = 1:part$nx) %dopar% {
        if (!is.na(elevation$Data[iy,ix])) {
          settings$mtclim$elevation <- elevation$Data[iy,ix]
          settings$mtclim$lon<-elevation$xyCoords$x[ix]
          settings$mtclim$lat<-elevation$xyCoords$y[iy]

          ## RUN MLTCLIM
          mtclimRun(forcing_dataR = selectForcingCell(settings, forcing_dataRTotal, ix, iy),
                    settings = settings$mtclim)$out_data
        }
      }

      for (ix in 1:length(elevation$xyCoords$x)) {
        if (!is.na(elevation$Data[iy,ix])) {
          ## ADD TO OUTPUT ARRAY
          for (iVar in 1:length(settings$outputVars)) {
            iStart <- ((iVar-1)*settings$intern$nrec_out)+1
            iEnd <- iVar*settings$intern$nrec_out
            toNetCDFData[[iVar]][ix,iy,] <- output[[ix]][iStart:iEnd]
          }
        }
      }
      rm(output)

      ## refresh progressbar
      setTxtProgressBar(pb, iy)

    }
    # Close ProgressBar
    close(pb)

    profile$end.time.run <- Sys.time()

    ## ADD OUTPUT TO NETCDF
    profile$start.time.write <- Sys.time()
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
    profile$end.time.write <- Sys.time()

    ## Print info about part
    cat(sprintf("  Times (read/run/write/total): %.1f/%.1f/%.1f/%.1f minutes",
                as.numeric(profile$end.time.read - profile$start.time.read, units = "mins"),
                as.numeric(profile$end.time.run - profile$start.time.run, units = "mins"),
                as.numeric(profile$end.time.write - profile$start.time.write, units = "mins"),
                as.numeric(profile$end.time.write - profile$start.time.read, units = "mins"),
                format(object.size(forcing_dataRTotal), units = "auto")))
    cat(sprintf("         Sizes (read/write): %s/%s\n",
                format(object.size(forcing_dataRTotal), units = "auto"),
                format(object.size(toNetCDFData), units = "auto")))

  }

  cat(sprintf("\nFinished in %.1f minutes\n",as.numeric(Sys.time() - start.time.total, units = "mins")))
}

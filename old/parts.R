mask <- elevation
partSize <- 20

# partBased <- "lon"
# partSize <-NULL
# mask<-elevation
if (is.null(partSize)) {
  print(paste0("Partsize not defined, so using max: ", length(mask$Data[!is.na(mask$Data)]), " (only 1 part)"))
  partSize <- length(mask$Data[!is.na(mask$Data)])
} else if (partSize < length(mask$xyCoords$y)) {
  print(paste0("Partsize (", partSize, ") should be higher than ny (", length(mask$xyCoords$y), ")"))
  print(paste0("Partsize changed to ny (", length(mask$xyCoords$y), ")"))
  partSize <- length(mask$xyCoords$y)

}

nActive <- length(mask$Data[!is.na(mask$Data)])
print(paste0(nActive, " active cells in mask found"))


## Count nr of parts
counter <- 1
nPart <- 1
for (iy in 1:length(mask$xyCoords$y)) {
  for (ix in 1:length(mask$xyCoords$x)) {
    if (!is.na(mask$Data[iy,ix])) {
      counter <- counter + 1
    }
    if (counter >= partSize) {
      nPart <- nPart + 1
      counter <- 1
      # sy <- iy
      # print(paste(sy))
      break
    }
    # print(paste(sy))
    # print(paste(sx,nx,sy,ny))
  }
}
print(nPart)

part <- list(sx = 1,
             nx = length(mask$xyCoords$x),
             sy = NULL,
             ny = NULL,
             slon = NULL,
             elon = NULL,
             slat = NULL,
             elat = NULL)
parts <- list(part)[rep(1,nPart)]


counter <- 1
iPart <- 1
parts[[1]]$sy <- 1
for (iy in 1:length(mask$xyCoords$y)) {
  for (ix in 1:length(mask$xyCoords$x)) {
    if (!is.na(mask$Data[iy,ix])) {
      counter <- counter + 1
    }
    if (counter >= partSize) {
      iPart <- iPart + 1
      counter <- 1
      parts[[iPart]]$sy <- iy
      sy <- iy
      # print(paste(sy))
      break
    }
    # print(paste(sy))
    # print(paste(sx,nx,sy,ny))
  }
}
print( "!!!")
for (iPart in 1:nPart) {

  print(parts[[iPart]]$sy)
}




# # nPart <- ceiling(nActive / partSize)
# print(paste0(nPart, " parts (partsize: ", partSize, ")"))
#
# part <- list(sx = 1,
#              nx = length(mask$xyCoords$x),
#              sy = NULL,
#              ny = NULL,
#              slon = NULL,
#              elon = NULL,
#              slat = NULL,
#              elat = NULL)
# parts <- list(part)[rep(1,nPart)]
#
# parts[[1]]$sy <- 1
# if(nPart > 1) {
#   counter <- 1
#   iPart <- 1
#   for (iy in 1:length(mask$xyCoords$y)) {
#     for (ix in 1:length(mask$xyCoords$x)) {
#       if (!is.na(mask$Data[iy,ix])) {
#         # print(counter)
#         counter <- counter + 1
#       }
#       if (counter >= partSize) {
#         iPart <- iPart + 1
#         parts[[iPart]]$sy <- iy
#         counter <- 1
#       }
#     }
#   }
#
#   for (iPart in 1:(nPart - 1)) {
#     parts[[iPart]]$ny <- parts[[iPart + 1]]$sy - parts[[iPart]]$sy
#     parts[[iPart]]$slon <- mask$xyCoords$x[parts[[iPart]]$sx]
#     parts[[iPart]]$elon <- mask$xyCoords$x[parts[[iPart]]$sx + parts[[iPart]]$nx -1]
#     parts[[iPart]]$slat <- mask$xyCoords$y[parts[[iPart]]$sy]
#     parts[[iPart]]$elat <- mask$xyCoords$y[parts[[iPart]]$sy + parts[[iPart]]$ny -1]
#   }
# }
# parts[[nPart]]$ny <- length(mask$xyCoords$y) - parts[[nPart]]$sy + 1
# parts[[nPart]]$slon <- mask$xyCoords$x[parts[[nPart]]$sx]
# parts[[nPart]]$elon <- mask$xyCoords$x[parts[[nPart]]$sx + parts[[nPart]]$nx -1]
# parts[[nPart]]$slat <- mask$xyCoords$y[parts[[nPart]]$sy]
# parts[[nPart]]$elat <- mask$xyCoords$y[parts[[nPart]]$sy + parts[[nPart]]$ny -1]
#
# parts[[1]]

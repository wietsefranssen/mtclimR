library(doParallel)
registerDoParallel(cores=2)
foreach(ii=1:3, .combine = rbind) %dopar% {
  hh<-sqrt(ii)
  print(paste("hoi: ", ii, hh))
  write(c(hh,ii), file = paste0("./tt", ii, ".txt"))
}


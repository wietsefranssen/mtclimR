# rm(list = ls(all = TRUE))
# is.loaded("testWFR")
# # .C("testWFR")
#
# inData<-read.delim("./data/forcing_-8.25_-39.25", sep = " ", header = F, skip = 1827)
# nrecs <- 1461
# inData<-inData[1:nrecs,]
#
# ## Generate forcing data
# forcing_dataR <- list()
# for (i in 1:24) {
#   forcing_dataR[[i]]<- array(0, dim=c(nrecs))
# }
#
# forcing_dataR[[10]][]<- inData[,1];
# forcing_dataR[[18]][]<- inData[,2];
# forcing_dataR[[17]][]<- inData[,3];
# forcing_dataR[[21]][]<- inData[,4];
# forcing_dataR[[15]][]<- inData[,5];
# forcing_dataR[[7]][] <- inData[,6];
#
# ## Run!
# output<- mtclimRun(nrecs = nrecs, forcing_dataR = forcing_dataR)
# nrecs_out <- length(output$out_data$OUT_PREC)
#
# ## Write output to file
# file.remove("test")
# for (i in 1:nrecs_out) {
#   write(sprintf("%4.4f\t%7.4f\t%7.4f\t%7.4f\t%8.4f\t%7.4f\t%7.4f\t%8.4f\t%8.4f\t%7.4f\t%7.4f\t%7.4f\t%7.4f",
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

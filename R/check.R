is.loaded("testWFR")
# .C("testWFR")

## Generate forcing data
forcing_dataR <- list()
for (i in 1:24) {
  forcing_dataR[[i]]<- array(0, dim=c(11688))
}
forcing_dataR[[10]][]<- 0.79;
forcing_dataR[[18]][]<- 22.09;
forcing_dataR[[17]][]<- 33.05;
forcing_dataR[[21]][]<- 2.95;
forcing_dataR[[15]][]<- 196.90;
forcing_dataR[[7]][] <- 410.10;

## Run!
# mtclimRun(x = 2, forcing_dataR = forcing_dataR)

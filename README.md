# mtclimR

## Installation

Start R and run the following commands:

    library(devtools)
    install_git("https://github.com/wietsefranssen/mtclimR")

## Examples

Run the following command to test if the package is working properly.
A file called `example_output_Mekong.nc` should be written in the working directory.

    ## Load the library
    library(mtclimR)
    
    ## Fill struct with settings
    settings <- mtclim_getSettings()

    ## Run the main routine
    mtclim_run(settings)

## Usage

TODO

## LICENSE

TODO

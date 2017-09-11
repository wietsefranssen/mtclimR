#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vicNl.h"

static char vcid[] = "$Id$";

void read_atmos_data_dummy(global_param_struct   global_param,
		     double              **forcing_data)
{

  extern param_set_struct param_set;

  int             rec;
  int             Nfields;
  int            *field_index;

  Nfields     = param_set.N_TYPES;
  field_index = param_set.FORCE_INDEX;

  printf("forcing: %f\n", forcing_data[field_index[0]][0]);

    /* read forcing data */
    rec=0;

    while( (rec * param_set.FORCE_DT < global_param.nrecs * global_param.dt ) ) {
//      for(i=0;i<Nfields;i++)
////	fscanf(infile,"%lf", &forcing_data[field_index[i]][rec]);
////      fgets(str, MAXSTRING, infile);
// from line 1828
//        0.790000 22.090000 33.050000 2.950000 196.900000 410.100000
      forcing_data[field_index[0]][rec] = 0.79;
      forcing_data[field_index[1]][rec] = 22.09;
      forcing_data[field_index[2]][rec] = 33.05;
      forcing_data[field_index[3]][rec] = 2.95;
      forcing_data[field_index[4]][rec] = 196.90;
      forcing_data[field_index[5]][rec] = 410.10;

//      forcing_data[field_index[0]][rec] = 3.08;
//      forcing_data[field_index[1]][rec] = 20.72;
//      forcing_data[field_index[2]][rec] = 27.33;
//      forcing_data[field_index[3]][rec] = 1.44;
//      forcing_data[field_index[4]][rec] = 169.40;
//      forcing_data[field_index[5]][rec] = 414.30;
      rec++;
    }
}

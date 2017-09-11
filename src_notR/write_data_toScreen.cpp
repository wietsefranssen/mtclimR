#include <stdio.h>
#include <stdlib.h>
#include "vicNl.h"

static char vcid[] = "$Id$";

void write_data_toScreen(out_data_file_struct *out_data_files,
                         out_data_struct *out_data,
                         dmy_struct      *dmy,
                         int              dt)
{
  extern option_struct options;
  int                 file_idx;
  int                 var_idx;
  int                 elem_idx;
  int                 ptr_idx;
  char               *tmp_cptr;
  short int          *tmp_siptr;
  unsigned short int *tmp_usiptr;
  int                *tmp_iptr;
  float              *tmp_fptr;
  double             *tmp_dptr;


  // Loop over this output file's data variables
  for (var_idx = 0; var_idx < out_data_files[file_idx].nvars; var_idx++) {
    // Loop over this variable's elements
    for (elem_idx = 0; elem_idx < out_data[out_data_files[file_idx].varid[var_idx]].nelem; elem_idx++) {
      if (!(var_idx == 0 && elem_idx == 0)) {
        printf("\t ");
      }
      printf("%f", out_data[out_data_files[file_idx].varid[var_idx]].aggdata[elem_idx]);
    }
  }
  printf("\n");
}


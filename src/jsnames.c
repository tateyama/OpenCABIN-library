/* OpenCABIN Library

   Copyright (C) 2006,2007 KGT Inc.

    This file is part of the OpenCABIN Library developed
    under CnC project sponsored by National Institute of
    Information and Communications Technology (NICT).

    The OpenCABIN Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    The OpenCABIN Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.
*/

//#define STANDALONE

#include <joystick.h>
#include <string.h>

#ifdef STANDALONE
#endif

struct namedata {
  char *name;
  int type, number;
};

static struct namedata NAMEDATA[] = 
  {{"BX", OCL_JS_BUTTON, OCL_JS_BX}, // X
   {"BC", OCL_JS_BUTTON, OCL_JS_BC}, // circle
   {"BB", OCL_JS_BUTTON, OCL_JS_BB}, // box
   {"BT", OCL_JS_BUTTON, OCL_JS_BT}, 	// triangle 
   {"BL1", OCL_JS_BUTTON, OCL_JS_BL1}, 	// L1
   {"BL2", OCL_JS_BUTTON, OCL_JS_BL2}, 	// L2
   {"BR1", OCL_JS_BUTTON, OCL_JS_BR1}, 	// R1
   {"BR2", OCL_JS_BUTTON, OCL_JS_BR2}, 	// R2
   {"BSE", OCL_JS_BUTTON, OCL_JS_BSE}, 		// select
   {"BST", OCL_JS_BUTTON, OCL_JS_BST}, 		// start
   {"BJL", OCL_JS_BUTTON, OCL_JS_BJL}, 		// Joystick Left Button
   {"BJR", OCL_JS_BUTTON, OCL_JS_BJR}, 		// Joystick Left Button
   {"BUP", OCL_JS_BUTTON, OCL_JS_BUP}, 		// up
   {"BRIGHT", OCL_JS_BUTTON, OCL_JS_BRIGHT}, 		// right
   {"BDOWN", OCL_JS_BUTTON, OCL_JS_BDOWN}, 		// down
   {"BLEFT", OCL_JS_BUTTON, OCL_JS_BLEFT}, 		// left
   {"SLX", OCL_JS_STICK, OCL_JS_SLX}, //stick left x (plus right)
   {"SLY", OCL_JS_STICK, OCL_JS_SLY}, //stick left y (plus down)
   {"SRX", OCL_JS_STICK, OCL_JS_SRX}, //stick right x
   {"SRY", OCL_JS_STICK, OCL_JS_SRY}, //stick right y
   {"S3X", OCL_JS_STICK, OCL_JS_S3X}, //stick 3 x
   {"S3Y", OCL_JS_STICK, OCL_JS_S3Y}, //stick 3 y
   {"", 0, 0}};


int
jsnames_str2data(char *str, int *ret_type, int *ret_number) // (without "value")
{
  struct namedata *ndptr = &NAMEDATA[0];
  for (; ndptr->name[0]; ndptr++) {
    if (!strcasecmp(ndptr->name, str)) {
      *ret_type = ndptr->type;
      *ret_number = ndptr->number;
      return 1;
    }
  }
  return 0;
}

char *
jsnames_data2str(int type, int number, int value)
{
  
}


#include <stdio.h>

void
jsnames_print_allnames(FILE *outfp)
{
  struct namedata *ndptr = &NAMEDATA[0];
  for (; ndptr->name[0]; ndptr++) {
    fprintf(outfp, "%s, ", ndptr->name);
  }
  fprintf(outfp, "\n");
}

void
jsnames_print_jsstat(FILE *outfp, struct ocl_jsstat *stat)
{
  struct namedata *ndptr = &NAMEDATA[0];
  for (; ndptr->name[0]; ndptr++) {
    if (ndptr->type == OCL_JS_BUTTON) {
      fprintf(outfp, "Button %6s %d\n", ndptr->name, stat->button[ndptr->number]);
    } else {
      fprintf(outfp, "Stick  %6s %6d\n", ndptr->name, stat->stick[ndptr->number]);
    }
  }
}


#ifdef STANDALONE

int
main(int ac, char **av)
{
  int i, type, number;

  if (ac == 1) {
    fprintf(stderr, "Usage: %s str\n\t", av[0]);
    jsnames_print_allnames(stderr);
    return 1;
  }

  for (i = 1; i < ac; ++i) {
    if (jsnames_str2data(av[i], &type, &number)) {
      printf("%d %d\n", type, number);
    } else {
      printf("no such name \"%s\"\n", av[i]);
    }
  }

  return 0;
}

#endif

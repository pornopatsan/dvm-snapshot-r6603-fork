/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/




typedef struct sblob *PTR_SBLOB;

struct  sblob  { PTR_SYMB  symb;
	             PTR_SBLOB next;
	           };

struct  sblob  syms[100];



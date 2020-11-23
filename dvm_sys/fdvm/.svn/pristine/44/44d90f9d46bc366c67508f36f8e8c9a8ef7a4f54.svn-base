/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/



#define BIFNDE 0
#define DEPNDE 1
#define LLNDE 2
#define SYMNDE 3
#define LISNDE 4
#define BIFLISNDE 5
#define UNUSED -1
#define NUMLIS 100
#define DEPARC  1
#define MAXGRNODE 50

typedef struct lis_node *LIST;

struct lis_node {
	int variant; /* one of BIFNDE, BIFLISNDE, DEPNDE, LLNDE, SYMNDE, LISNDE */
	union list_union {
		PTR_BFND bfnd;
		PTR_BLOB biflis;
		PTR_DEP dep;
		PTR_LLND llnd;
		PTR_SYMB symb;
		LIST lisp;
		} entry;
	LIST next;
	} ;



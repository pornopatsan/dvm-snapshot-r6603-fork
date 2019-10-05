/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


struct ELIST_rec
       {
	  int type; /* 0 for int, 1 for string, 2 for ELIST */
	  char * car;
	  struct ELIST_rec * cdr;
       };

#define TEINT 0
#define TESTRING 1
#define TELIST 2

typedef struct ELIST_rec * ELIST;


/* 
   the following two defines are pretty bad. But have been done so as to
   avoid globals which look like global variables. For these to go away
   libdb.a has to change.
*/
#define currentFile cur_file
#define currentProject cur_proj

extern PTR_FILE currentFile;    /* actually cur_file */
extern PTR_PROJ currentProject; /* actually cur_proj */

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

/* functions that are used within the cbaselib */
ELIST ENew( /* etype */ );
void EFree( /* e */ );
ELIST ECopy( /* e */ );
ELIST ECpCar( /* e */ );
ELIST ECpCdr( /* e */ );
ELIST EAppend( /* e1, e2 */ );
ELIST EString( /* s */ );
ELIST ENumber( /* n */ );
ELIST ECons( /* e1, e2 */ );
int ENumP(/*e*/);
int EStringP(/*e*/);
int EListP(/*e*/);

#define ECar(x) ((x)->car)
#define ECdr(x) ((x)->cdr)
#define ECaar(x) (ECar((ELIST)ECar(x)))
#define ECdar(x) (ECdr((ELIST)ECar(x)))
#define ECadr(x) (ECar(ECdr(x)))
#define ECddr(x) (ECdr(ECdr(x)))

#define ECaaar(x) (ECar((ELIST)ECaar(x)))
#define ECdaar(x) (ECdr((ELIST)ECaar(x)))
#define ECadar(x) (ECar(ECdar(x)))
#define ECaadr(x) (ECar((ELIST)ECadr(x)))
#define ECaddr(x) (ECar(ECddr(x)))
#define ECddar(x) (ECdr(ECdar(x)))
#define ECdadr(x) (ECdr((ELIST)ECadr(x)))
#define ECdddr(x) (ECdr(ECddr(x)))

char *Allocate(/* size */);

PTR_BFND FindCurrBifNode( /* id */ );
PTR_LLND FindLLNode( /* id */ );
PTR_LABEL FindLabNode(/* id */);
PTR_SYMB FindSymbolNode(/* id */);
PTR_TYPE FindTypeNode(/* id */);   
PTR_FILE FindFileObj(/* filename */);
PTR_DEP FindDepNode(/* id */); 
PTR_BFND MakeDeclStmt(/* s */);
int VarId(/* id */);

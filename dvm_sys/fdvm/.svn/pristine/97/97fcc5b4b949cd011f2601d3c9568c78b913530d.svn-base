/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/*******************************************************************/
/* A class for creating a static call tree for C++ and pC++        */
/* functions.  usage:                                              */
/* include "sage++user.h"                                          */
/* include "sage++callgraph.h"                                     */
/* main(){                                                         */
/*   SgProject project("myfile")                                   */
/*   SgCallGraph CG;                                               */
/*   Cg.GenCallTree(&(project->file(0)));                          */
/*   CG.computeClosures();                                         */
/* the object then contains call info for that file.               */
/* see the public functions for data that can be extracted         */
/*******************************************************************/
#define SGMOE_FUN 1
#define SGNORMAL_FUN 0
#define SGMOC_FUN 2
#define SGMAX_HASH 541

class SgCallGraphFunRec;

typedef struct _SgCallSiteList{
  SgStatement *stmt;
  SgExpression *expr;
  struct _SgCallSiteList *next;
}SgCallSiteList;

typedef struct _SgCallGraphFunRecList{
  SgStatement *stmt;
  SgExpression *expr;
  SgCallGraphFunRec *fr;
  struct _SgCallGraphFunRecList *next;
}SgCallGraphFunRecList;

class SgCallGraphFunRec{
  public:
   int type; // either moe, normal or moc.
   SgStatement *body;
   SgCallSiteList *callSites;  // pointer to tail of circular linked list
   SgSymbol *s;
   int Num_Call_Sites;
   SgCallGraphFunRecList *callList; // pointer to tail of circular linked list
   int Num_Call_List;
   int isCollection;  // = 1 if this is a method of a collection
   int calledInPar;   // = 1 if called in a parallel section
   int calledInSeq;   // = 1 if called in sequentail main thread
   SgSymbol *className;  // for member functions.
   int flag;  // used for traversals.

   int id;  // serial number 
   SgCallGraphFunRec *next;  // used for linked list
   SgCallGraphFunRec *next_hash;  // used for hash table collisions
   // used for next* functions
   SgCallSiteList *currentCallSite; 
   SgCallSiteList *currentCallExpr;
   SgCallGraphFunRecList *currentFunCall;
};

class SgCallGraph{

  public:
        SgCallGraph(void) {};                     // constructor
	void GenCallTree(SgFile *);               // initialize and build the call tree
        void printFunctionEntry(SgSymbol *fname); // print info about fname
        int numberOfFunctionsInGraph();           // number of functions in the table.
        int numberOfCallSites(SgSymbol *fname);   // number of call sites for funame
        int numberOfFunsCalledFrom(SgSymbol *fname); // how many call sites in fname

        int isAMethodOfElement(SgSymbol* fname);  // 1 if fname is a method of an element of a coll.
        int isACollectionFunc(SgSymbol* fname);   // 1 if fname is a method of a collection (not MOE)
        int isCalledInSeq(SgSymbol* fname);       // 1 if fname is called in a sequential sect.
        int isCalledInPar(SgSymbol* fname);       // 1 if fname is called in parallel code
	void computeClosures();

	SgSymbol *firstFunction();                // first function in callgraph
	SgSymbol *nextFunction();                 // next function in callgraph
        int functionId(SgSymbol *fname);                  // id of fname
	SgStatement *functionBody(SgSymbol *fname);       // body of fname
        SgStatement *firstCallSiteStmt(SgSymbol *fname);  // stmt of first call of fname
        SgStatement *nextCallSiteStmt(SgSymbol *fname);   // stmt of next call of fname
        SgExpression *firstCallSiteExpr(SgSymbol *fname); // expression of first call
        SgExpression *nextCallSiteExpr(SgSymbol *fname);  // expression of next call
        SgSymbol *firstCalledFunction(SgSymbol *fname);   // first function called in fname
        SgSymbol *nextCalledFunction(SgSymbol *fname);    // next function called in fname
        SgStatement *SgCalledFunctionStmt(SgSymbol *fname); // get statement of current called function
        SgExpression *SgCalledFunctionExpr(SgSymbol *fname); // get expression of current called function

  // obsolete functions: 
        SgSymbol *function(int i);          // i-th function in table (0 = first)
        SgStatement *functionBody(int i);   // i-th function in table (0 = first)
        void printTableEntry(int);          // print the i-th table entry.

        SgStatement *callSiteStmt(SgSymbol *fname, int i);  // stmt of i-th call of fname
        SgExpression *callSiteExpr(SgSymbol *fname, int i); // expression of i-th call
        SgSymbol *calledFunction(SgSymbol *fname, int i);   // i-th function called in fname
  // end obsolete
  protected:
	SgCallGraphFunRec *FunListHead;
	int num_funs_in_table;
	SgCallGraphFunRec *hash_table[SGMAX_HASH];
	SgCallGraphFunRec *locateFunctionInTable(SgSymbol *); 
	SgCallGraphFunRec *lookForFunctionOpForClass(SgSymbol *);
	void updateFunctionTableConnections(SgCallGraphFunRec *, SgStatement *, SgExpression *);
	void findFunctionCalls(SgStatement *, SgExpression *);
        void init();

	void insertInHashTable(SgSymbol *, SgCallGraphFunRec *);
	unsigned long int hashSymbol(SgSymbol *);
   	SgCallGraphFunRec *currentFun;
};

SgType *findTrueType(SgExpression *);
SgType *makeReducedType(SgType *);
	SgSymbol *firstFunction();
	SgSymbol *nextFunction();




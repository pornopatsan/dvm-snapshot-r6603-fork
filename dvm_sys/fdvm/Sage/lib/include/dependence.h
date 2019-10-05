/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/



/* declaration for the dependencies computation and use in the toolbox */

/* on declare de macro d'acces aux dependence de donnee */

#define BIF_DEP_STRUCT1(NODE) ((NODE)->entry.Template.dep_ptr1)
#define BIF_DEP_STRUCT2(NODE) ((NODE)->entry.Template.dep_ptr2)

#define FIRST_DEP_IN_PROJ(X)  ((X)->head_dep)
/* decription d'une dependance */

#define DEP_ID(DEP)          ((DEP)->id)
#define DEP_NEXT(DEP)        ((DEP)->thread)
#define DEP_TYPE(DEP)       ((DEP)->type)
#define DEP_DIRECTION(DEP)   ((DEP)->direct)
#define DEP_SYMB(DEP)        ((DEP)->symbol)
#define DEP_FROM_BIF(DEP)    (((DEP)->from).stmt)
#define DEP_FROM_LL(DEP)     (((DEP)->from).refer)
#define DEP_TO_BIF(DEP)      (((DEP)->to).stmt)
#define DEP_TO_LL(DEP)       (((DEP)->to).refer)
#define DEP_FROM_FWD(DEP)    ((DEP)->from_fwd)
#define DEP_FROM_BACK(DEP)   ((DEP)->from_back)
#define DEP_TO_FWD(DEP)      ((DEP)->to_fwd)
#define DEP_TO_BACK(DEP)     ((DEP)->to_back)


/* la forme normale de dependence de donnee est le vecteur de direction */

/* on rappel temporairement la forme des dep  (sets.h)
struct dep {  data dependencies 
 
    int id;      identification for reading/writing 
    PTR_DEP thread;
 
    char     type;        flow-, output-, or anti-dependence 
    char     direct[MAX_DEP]; direction/distance vector 
 
    PTR_SYMB   symbol;  symbol table entry 
    struct ref from;    tail of dependence 
    struct ref to;     head of dependence 
 
    PTR_DEP  from_fwd, from_back;  list of dependencies going to tail
    PTR_DEP  to_fwd, to_back;      list of dependencies going to head 
 
    } ;

*/



/* pour la gestion memoire */
struct chaining
{
  char *zone;
  struct chaining *list;
};
 
typedef struct chaining *ptchaining;


struct stack_chaining
{
   ptchaining first;
   ptchaining last;
   struct stack_chaining *prev;
   struct stack_chaining *next;
   int level;
};

typedef struct stack_chaining *ptstack_chaining;

/* structure pour les graphes de dependence */
#define MAXSUC 100

struct graph
{
  int id; /* identificateur */
  int linenum;
  int mark;
  int order;
  PTR_BFND stmt;
  PTR_LLND expr;
  PTR_LLND from_expr[MAXSUC];
  PTR_LLND to_expr[MAXSUC];
  PTR_DEP dep_struct[MAXSUC];
  char *dep_vect[MAXSUC];
  char type[MAXSUC];
  struct graph *suc[MAXSUC]; /* next */
  struct graph *pred[MAXSUC]; /* next */
  struct graph *list;  /* chaine les noeuds d'un graphe */
};

typedef struct graph  *PTR_GRAPH;

#define CHAIN_LIST(NODE)           ((NODE)->list)
#define GRAPH_ID(NODE)             ((NODE)->id)
#define GRAPH_ORDER(NODE)          ((NODE)->order)
#define GRAPH_MARK(NODE)           ((NODE)->mark)
#define GRAPH_LINE(NODE)           ((NODE)->linenum)
#define GRAPH_BIF(NODE)            ((NODE)->stmt)
#define GRAPH_LL(NODE)             ((NODE)->expr)
#define GRAPH_DEP(NODE)            (((NODE)->dep_struct))
#define GRAPH_VECT(NODE)           (((NODE)->dep_vect))
#define GRAPH_TYPE(NODE)           ((NODE)->type)
#define GRAPH_SUC(NODE)            (((NODE)->suc))
#define GRAPH_PRED(NODE)           (((NODE)->pred))
#define GRAPH_LL_FROM(NODE)        (((NODE)->from_expr))
#define GRAPH_LL_TO(NODE)          (((NODE)->to_expr))


#define NOT_ORDERED -1

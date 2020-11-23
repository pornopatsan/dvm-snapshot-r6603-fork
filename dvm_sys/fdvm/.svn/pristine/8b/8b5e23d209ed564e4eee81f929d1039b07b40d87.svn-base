/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* file: glob_anal.c */

#include <stdio.h>
#include "db.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
#define MAX_FUNS 500

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

void *malloc();
void bind_call_site_info();

static PTR_FILE current_file;

extern PTR_FILE cur_file;
extern int debug;

typedef struct call_list *PTR_CALLS;
typedef struct function_decl *PTR_FUNCS;

struct call_list {
  char *name;
  int funs_number;              /* set to the index in the funs table */
  /* -1 if the function is unknown      */
  PTR_LLND used, modified;
  PTR_BFND call_site;           /* statement which holds call to this fun */
  PTR_CALLS next;
};


struct function_decl {
  PTR_FILE file;                /* file object where this function was
                                 * defined */
  PTR_SYMB name;                /* point to the symbol table of this functin */
  PTR_BFND fun;                 /* point to the BIF node of this functio */
  int is_done;
  PTR_LLND used, modified;
  PTR_CALLS calls;
} funs[MAX_FUNS];

int num_of_funs = 0;

static int now;
static int val[MAX_FUNS],       /* keep the depth-first numbering */
 ival[MAX_FUNS];                /* keep the inverse calling numbering */


/*
 * visit does the depth-first numbering for nodes
 * for the call graph
 *
 * the array "val" keep the depth-first visiting numbering
 * while the array "ival" is the inverse of "val", i.e. is
 * the reverse calling sequence
 */
static void visit(k)
int k;
{
  PTR_CALLS p;

  ival[now] = k;
  val[k] = now++;
  for (p = funs[k].calls; p; p = p->next)       /* for each adjacent node */
    if (val[p->funs_number] < 0)/* haven't visited yet */
      visit(p->funs_number);
}


/*
 * dfs does the depth-first search of the call graph
 */
static void dfs()
{
  int k;

  now = 0;                      /* keep track of the numbering */
  for (k = 0; k < num_of_funs; k++)     /* initialize to be un-read */
    val[k] = -1;
  for (k = 0; k < num_of_funs; k++)     /* now do the depth-first search */
    if (val[k] < 0)
      visit(k);
}


void reset_llnd(p)
PTR_LLND p;
{
  if (p == NULL)
    return;
  if (p->variant == VAR_REF) {
    p->entry.Template.ll_ptr1 = NULL;
  }
  reset_llnd(p->entry.Template.ll_ptr1);
  reset_llnd(p->entry.Template.ll_ptr2);
}


void reset_scalar_propogation(b)
PTR_BFND b;
{
  PTR_BLOB bl;

  if (b == NULL)
    return;
  if ((b->variant != FUNC_HEDR) && (b->variant != PROC_HEDR)) {
    reset_llnd(b->entry.Template.ll_ptr1);
    reset_llnd(b->entry.Template.ll_ptr2);
    reset_llnd(b->entry.Template.ll_ptr3);
  }
  for (bl = b->entry.Template.bl_ptr1; bl; bl = bl->next)
    reset_scalar_propogation(bl->ref);

  for (bl = b->entry.Template.bl_ptr2; bl; bl = bl->next)
    reset_scalar_propogation(bl->ref);
}


/* make_fun_decl initialized an entry in the funs table for a function at */
/* statement b                                                            */
static void make_fun_decl(f, b)
PTR_FILE f;
PTR_BFND b;
{
  PTR_FUNCS i;
  PTR_LLND make_llnd();

  i = funs + num_of_funs++;
  if (num_of_funs > MAX_FUNS) {
    fprintf(stderr, "Too many functions!\n");
    return;
  }

  /* b's ll_ptr3 points to an expr list whose ll_ptr1 is the pre global */
  /* analysis use set and whose ll_ptr2 will be the post analysis use set */
  if (b->entry.Template.ll_ptr3 == NULL) {      /* summary of use info */
    fprintf(stderr, "bad initial analysis. run vcc or cfp again\n");
    b->entry.Template.ll_ptr3 = make_llnd(cur_file,EXPR_LIST,NULL, NULL, NULL);
  }
  if (b->entry.Template.ll_ptr2 == NULL) {      /* summary of mod info */
    fprintf(stderr, "bad initial analysis. run vcc or cfp again\n");
    b->entry.Template.ll_ptr2 = make_llnd(cur_file,EXPR_LIST, NULL, NULL, NULL);
  }

  i->file = f;
  i->name = b->entry.Template.symbol;
  i->fun = b;
  i->is_done = 0;
  i->used = b->entry.Template.ll_ptr3->entry.Template.ll_ptr1;
  i->modified = b->entry.Template.ll_ptr2->entry.Template.ll_ptr1;
  i->calls = NULL;
}


/* call this function with the project_object   */
/* to build the list of functions.              */
static void make_fun_list(proj)
PTR_PROJ proj;
{
  PTR_FILE f;
  PTR_BLOB b1, b;
  PTR_BFND p;
  PTR_REFL make_name_list();
  PTR_SETS alloc_sets();
  /* Scan through all files in the project */
  for (b1 = proj->file_chain; b1; b1 = b1->next) {
    f = (PTR_FILE) b1->ref;
    for (b = f->global_bfnd->entry.Template.bl_ptr1; b; b = b->next)
      if (b->ref->variant == FUNC_HEDR ||
          b->ref->variant == PROC_HEDR ||
          b->ref->variant == PROG_HEDR) {
        make_fun_decl(f, b->ref);
        p = b->ref;
        if (p->entry.Template.sets == NULL)
          p->entry.Template.sets = alloc_sets();
        p->entry.Template.sets->out_use = NULL;
        p->entry.Template.sets->in_use = NULL;
        p->entry.Template.sets->out_def = NULL;
        p->entry.Template.sets->in_def = NULL;
        p->entry.Template.sets->gen = NULL;
        p->entry.Template.sets->use = NULL;
        /* set in_def  to be a ref list of all */
        /* parameters to this proc.  this is   */
        /* used in the global analysis phase   */
        p->entry.Template.sets->in_def =
          make_name_list(
                         p->entry.Template.symbol->entry.proc_decl.in_list
          );
      }
  }
}


/* find_by_name searches the funs list for the function whose name is */
/* given by the char string s                                         */
static int find_by_name(PTR_FILE f, char *s)
/*PTR_FILE f;*/
/*char *s;*/
{
  int i;

  f = f;                        /* make lint happy, f unused */
  for (i = 0; i < num_of_funs; i++)
    if ( /* funs[i].file == f && */ (!strcmp(s, funs[i].name->ident)))
      return i;
  for (i = 0; i < num_of_funs; i++)
    if (!strcmp(s, funs[i].name->ident))
      return i;
  return (-1);
}

PTR_BFND find_fun_by_name(s)
char *s;
{
  int i;
  i = find_by_name(NULL, s);
  if (i < 0)
    return NULL;
  return funs[i].fun;
}


/* get_fun_number takes a pointer to a symbol table entry and looks */
/* it up in the funs table and returns the index.  like the others  */
/* it returns -1 if nothing is found that matches s.                */
/*static int get_fun_number(f, s)
PTR_FILE f;
PTR_SYMB s;
{
  int i;
  for (i = 0; i < num_of_funs; i++)
    if (funs[i].file == f && funs[i].name == s)
      return i;
  return (-1);
}*/


/* append_to_call_list takes the symbol table pointer of a function */
/* that calls another function whose name is given by a char string */
/* and appends the name of the called function to the calls list of */
/* the funs entry for the calling function.                         */
static void append_to_call_list(calling_fun, called_fun_ident, bf)
int calling_fun;
char *called_fun_ident;
PTR_BFND bf;
{
  int called_fun;
  PTR_CALLS p;
  PTR_BFND b;

  called_fun = find_by_name(funs[calling_fun].file, called_fun_ident);
  if (called_fun == -1) {
    fprintf(stderr, "Called \"%s\" function not in the project\n",
           called_fun_ident);
    return;
  }

  b = funs[calling_fun].fun;
  p = (PTR_CALLS) malloc(sizeof(struct call_list));
#ifdef __SPF
  addToCollection(__LINE__, __FILE__,p, 0);
#endif
  p->name = b->entry.Template.symbol->ident;
  p->funs_number = called_fun;
  p->call_site = bf;
  p->used = NULL;
  p->modified = NULL;
  p->next = funs[calling_fun].calls;
  funs[calling_fun].calls = p;
}


static void func_call_in_llnd(ll, i, bf)
PTR_LLND ll;
int i;
PTR_BFND bf;
{
  if (ll == NULL)
    return;
  if (ll->variant == FUNC_CALL ||
      ll->variant == PROC_CALL ||
      ll->variant == FUNCTION_REF)
    append_to_call_list(i, ll->entry.Template.symbol->ident, bf);

  /* NOTE: the following code is "tag" dependent */
  if (ll->variant >= VAR_LIST && ll->variant < CONST_NAME) {
    func_call_in_llnd(ll->entry.Template.ll_ptr1, i, bf);
    func_call_in_llnd(ll->entry.Template.ll_ptr2, i, bf);
  }
}


static void func_call_in_bfnd(bl, i)
PTR_BLOB bl;
int i;
{
  PTR_BFND bf;
  PTR_BLOB bl1;

  for (bl1 = bl; bl1; bl1 = bl1->next) {
    bf = bl1->ref;
    if (bf->variant == PROC_CALL ||
        bf->variant == FUNC_CALL ||
        bf->variant == PROC_STAT)
      append_to_call_list(i, bf->entry.Template.symbol->ident, bf);
    func_call_in_llnd(bf->entry.Template.ll_ptr1, i, bf);
    func_call_in_llnd(bf->entry.Template.ll_ptr2, i, bf);
    func_call_in_llnd(bf->entry.Template.ll_ptr3, i, bf);

    func_call_in_bfnd(bf->entry.Template.bl_ptr1, i);
    func_call_in_bfnd(bf->entry.Template.bl_ptr2, i);
  }
}

static void rec_list_cgraph(i)
int i;
{
  func_call_in_bfnd(funs[i].fun->entry.Template.bl_ptr1, i);
}


void BuildCallGraph()
{
  int i;
  fprintf(stderr, "\n the call graph is:\n");
  for (i = 0; i < num_of_funs; i++) {
    rec_list_cgraph(i);
  }
}


/*
 * ready_for_analysis returns
 *
 *      0  if not ready
 *      1  if it is ready
 *      2  if analysis is done.
 */
static int ready_for_analysis(i)
int i;
{
  PTR_CALLS calls;

  if (funs[i].is_done == 0) {
    for (calls = funs[i].calls; calls; calls = calls->next)
      if (calls->funs_number > -1 &&
          funs[calls->funs_number].is_done == 0)
        return (0);
    return (1);
  }
  return (2);
}


static PTR_LLND link_ll_chain(list, elist)
PTR_LLND list, elist;
{
  PTR_LLND p;

  p = list;
  while (p != NULL && p->entry.Template.ll_ptr2 != NULL)
    p = p->entry.Template.ll_ptr2;
  if (p != NULL)
    p->entry.Template.ll_ptr2 = elist;
  else
    list = elist;
  return (list);
}


static PTR_LLND link_ll_set_list(b, s)
PTR_LLND s;
PTR_BFND b;
{
  PTR_REFL rl, build_refl(), remove_locals_from_list();
  PTR_LLND link_set_list();

  rl = build_refl(b, s);
  rl = remove_locals_from_list(rl);
  return (link_set_list(rl));
}


static void use_mod(c)
PTR_CALLS c;
{
  PTR_BFND b;
  PTR_LLND used, modified;

  b = c->call_site;
  bind_call_site_info(b, &used, &modified);
  c->used = link_ll_set_list(b, used);
  c->modified = link_ll_set_list(b, modified);
}


static void compute_use_mod()
{
  int modified = 1;
  PTR_CALLS calls;
  PTR_LLND use, mod;
  int i, j;

  while (modified) {
    modified = 0;
    for (j = num_of_funs - 1; j >= 0; j--) {
      i = ival[j];
      if (ready_for_analysis(i) == 1) {
	if (debug) {
	  fprintf(stderr, "_______________________________\n");
          fprintf(stderr, "doing global analysis for %s\n", funs[i].name->ident);
        }
        calls = funs[i].calls;
	current_file = funs[i].file;
        while (calls != NULL) {
          if (calls->funs_number > -1 &&
              funs[calls->funs_number].is_done == 1)
            use_mod(calls);
          calls = calls->next;
        }
        funs[i].is_done = 1;
        /* now link results together */
        use = funs[i].used;
        mod = funs[i].modified;
        calls = funs[i].calls;
        while (calls != NULL) {
          if (calls->funs_number > -1 &&
              funs[calls->funs_number].is_done == 1) {
            use = link_ll_chain(use, calls->used);
            mod = link_ll_chain(mod, calls->modified);
          }
          calls = calls->next;
        }
        use = link_ll_set_list(funs[i].fun, use);
        mod = link_ll_set_list(funs[i].fun, mod);
        funs[i].used = link_ll_set_list(funs[i].fun, use);
        funs[i].modified = link_ll_set_list(funs[i].fun, mod);
        funs[i].fun->entry.Template.ll_ptr3
          ->entry.Template.ll_ptr2 = funs[i].used;
        funs[i].fun->entry.Template.ll_ptr2
          ->entry.Template.ll_ptr2 = funs[i].modified;
        modified = 1;
      }
    }                           /* end for */
  }                             /* end while */

  modified = 0;
  for (i = 0; i < num_of_funs; i++) {
    if (ready_for_analysis(i) == 2) {
      funs[i].fun->entry.Template.ll_ptr3
        ->entry.Template.ll_ptr2 = funs[i].used;
      funs[i].fun->entry.Template.ll_ptr2
        ->entry.Template.ll_ptr2 = funs[i].modified;
    }
    else
      modified = 1;
  }
  if (modified && debug)
    fprintf(stderr, "; cycle in call graph. no global analysis\n");
  current_file = NULL;
}


/****************************************************************
 *                                                              *
 *  GlobalAnal -- does the inter-procedural analysis for the    *
 *                given project                                 *
 *                                                              *
 *  Input:                                                      *
 *        proj - the pointer to the project to be analized      *
 *                                                              *
 *  Output:                                                     *
 *        none                                                  *
 *                                                              *
 ****************************************************************/
void GlobalAnal(proj)
PTR_PROJ proj;
{
  make_fun_list(proj);          /* gather all the functions declared */
  BuildCallGraph();             /* build the call graph */
  dfs();                        /* do the depth-first search */
  compute_use_mod();            /* do the inter-procedural analysis now */
}

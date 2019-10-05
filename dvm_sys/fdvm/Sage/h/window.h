/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/



#define MAX_WINDOW 256
#define MAX_ARRAYREF 256
#define MAX_STEP 10000
#define NO_STEP 10000
struct WINDOW
{
 int dimension;
 int Array_Id[MAX_ARRAYREF];
 int level;
 int level_update;
 char name[64];
 char gain[128];
 int coeff[MAXTILE][MAXTILE];
 int inf[MAXTILE];
 int sup[MAXTILE];
 int nb_ref;
 PTR_SYMB symb;
 PTR_SYMB array_symbol;
 PTR_SYMB pt;
 int lambda[MAXTILE];
 int delta[MAXTILE];
 int size[MAXTILE];
 int cst[MAXTILE];
};

struct WINDOWS 
{
  int nb_windows;
  int nb_loop;
  int tile_order[MAXTILE];
  int tile_sup[MAXTILE];
  int tile_inf[MAXTILE];
  int tile_bounds[MAXTILE];
  struct WINDOW thewindow[MAX_WINDOW];
  PTR_SYMB index[MAXTILE];
};


#define WINDS_NB(NODE)           ((NODE).nb_windows)
#define WINDS_INDEX(NODE)        ((NODE).index)
#define WINDS_NB_LOOP(NODE)      ((NODE).nb_loop)
#define WINDS_TILE_INF(NODE)     ((NODE).tile_inf)
#define WINDS_TILE_SUP(NODE)     ((NODE).tile_sup)
#define WINDS_TILE_ORDER(NODE)   ((NODE).tile_order)
#define WINDS_TILE_BOUNDS(NODE)  ((NODE).tile_bounds)
#define WINDS_WINDOWS(NODE,NUM)  (&((NODE).thewindow[NUM]))

#define WIND_DIM(NODE)           ((NODE)->dimension)
#define WIND_ARRAY(NODE)         ((NODE)->Array_Id)
#define WIND_LEVEL(NODE)         ((NODE)->level)
#define WIND_LEVEL_UPDATE(NODE)  ((NODE)->level_update)
#define WIND_NB_REF(NODE)        ((NODE)->nb_ref)
#define WIND_SYMBOL(NODE)        ((NODE)->symb)
#define WIND_POINTER(NODE)       ((NODE)->pt)
#define WIND_NAME(NODE)          ((NODE)->name)
#define WIND_GAIN(NODE)          ((NODE)->gain)
#define WIND_COEFF(NODE)         ((NODE)->coeff)
#define WIND_INF(NODE)           ((NODE)->inf)
#define WIND_SUP(NODE)           ((NODE)->sup)
#define WIND_LAMBDA(NODE)        ((NODE)->lambda)
#define WIND_DELTA(NODE)         ((NODE)->delta)
#define WIND_SIZE_DIM(NODE)      ((NODE)->size)
#define WIND_DIM_CST(NODE)       ((NODE)->cst)
#define WIND_ARRAY_SYMBOL(NODE)  ((NODE)->array_symbol)

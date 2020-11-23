/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* file: ker_fun.c */

/**********************************************************************/
/*  This file contains the routines called in sets.c that do all cache*/
/*  analysis and estimation routines.                                 */
/**********************************************************************/

#include <stdio.h>
#include "defs.h"
#include "bif.h"
#include "ll.h"
#include "symb.h"
#include "sets.h"

#define PLUS 2
#define ZPLUS 3
#define MINUS 4
#define ZMINUS 5
#define PLUSMINUS 6
#define NODEP -1

#ifdef __SPF
extern void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

extern int show_deps;

void *malloc();
PTR_SETS alloc_sets();
PTR_REFL alloc_ref();
int disp_refl();
PTR_REFL copy_refl();
PTR_REFL union_refl();
int **a_array;
int a_allocd = 0;
int x[20];                      /* a temporary used to compute the vector c */
int c[20];                      /* such that h(c) = dist                    */
int gcd();
int make_induct_list();
int comp_ker();
int find_mults();

int unif_gen(sor, des, vec, troub, source, destin)
int vec[], troub[];
struct ref *sor;
struct ref *des;
struct subscript *source;
struct subscript *destin;
{
  PTR_SYMB sor_ind_l[MAX_NEST_DEPTH], des_ind_l[MAX_NEST_DEPTH];
  struct subscript il_lo[MAX_NEST_DEPTH];
  struct subscript il_hi[MAX_NEST_DEPTH];
  PTR_LLND ll, tl;
  int arr_dim, uniform;
  int v[AR_DIM_MAX];
  int r, i, j, sd, dd, depth;

  /* the a array that is used here is allocated once and used */
  /* again in future calls */

  if (a_allocd == 0) {
      a_allocd = 1;
      a_array = (int **)malloc(MAX_NEST_DEPTH * (sizeof(int *)));
#ifdef __SPF
      addToCollection(__LINE__, __FILE__,a_array, 0);
#endif
      for (i = 0; i < MAX_NEST_DEPTH; i++)
      {
          a_array[i] = (int *)malloc((AR_DIM_MAX + MAX_NEST_DEPTH) * (sizeof(int)));
#ifdef __SPF
          addToCollection(__LINE__, __FILE__,a_array[i], 0);
#endif
      }
  }
  for (i = 0; i < MAX_NEST_DEPTH; i++) {
    sor_ind_l[i] = NULL;
    des_ind_l[i] = NULL;
  }


  dd = make_induct_list(des->stmt, des_ind_l, il_lo, il_hi);
  sd = make_induct_list(sor->stmt, sor_ind_l, il_lo, il_hi);

  depth = (sd < dd) ? sd : dd;

  i = 0;
  while ((i < depth) && (des_ind_l[i] == sor_ind_l[i]))
    i++;
  if (i < depth)
    depth = i;

  arr_dim = 0;
  /* compute the dimension of the array */
  ll = sor->refer;
  if (ll->variant == ARRAY_REF) {
    tl = ll->entry.array_ref.index;
    while (tl != NULL) {
      if ((tl->variant == VAR_LIST) ||
          (tl->variant == EXPR_LIST) ||
          (tl->variant == RANGE_LIST)) {
        tl = tl->entry.list.next;
        arr_dim++;
      }
    }
  }
  uniform = 1;
  for (i = 0; i < arr_dim; i++) {
    if (source[i].decidable != destin[i].decidable)
      uniform = 0;
    v[i] = source[i].offset - destin[i].offset;
    for (j = 0; j < depth; j++)
      if (source[i].coefs[j] != destin[i].coefs[j])
        uniform = 0;
  }
  if (uniform == 1) {
    r = comp_ker(arr_dim, depth, source, a_array, sor_ind_l, v, vec, troub);
  }
  /* else if (show_deps) fprintf(stderr, "not uniform\n"); */
  return (uniform);

}

/* comp_ker is a function that takes the matrix "h" associated with        */
/* a uniformly generated (potential) dependence and a offest vector "dist" */
/* and computes the distance vector "vec" and a trouble vector "troub"     */
/* the matrix is associated with the access function of an array reference */
/* where the array is of dimension "adim" and the depth of nesting is      */
/* depth.  The "a" array is a matrix that is allocated by the caller and   */
/* upon return contains a factorization of "h".  The array is "depth" rows */
/* by dept+adim columns but is viewed as its transpose mathematically.     */
/* It should be allocated as MAX_NEST_DEPTH by AR_DIM_MAX+MAX_NEST_DEPTH   */
/* In other words "a" is first initialized as
             
                  |<- depth ->|
           -------|           |
            ^     |           |
           adim   |    h      |
            v     |           |
           -------|-----------|  where rows in C are columns.
            ^     |           |
           depth  |    I      |
            v     |           |
           --------------------

   A factoriation takes place which converts this to the form where the
h component is now the matrix L and the Identity block I is now a square
matrix B such that
             L = hB

and L is lower triangular and B and L  are integer valued.  

What this means is that
if dist = Lx, for some x then let c be such that c = Bx and we have 
dist = Lx = hBx = hc.  (note x and c are global and returned by side effect.)
and c is the distance vector.

Furturemore, comp_ker returns the dimension of ker(h) and the right hand
dim(ker(h)) columns of B form a basis of the kernel.

*/

  
int comp_ker(adim, depth, sa, a, sor_ind_l, dist, vec, troub)
int adim, depth;
struct subscript *sa;
int **a;
PTR_SYMB sor_ind_l[];
int dist[];
int vec[], troub[];
{
  int i, j, k, piv_row, piv_col, cols_done, m, mval, cur_x;
  int nosolution;
  int p, q, r, s, z;
  int *tmp;

  sor_ind_l = sor_ind_l;        /* make lint happy, sor_ind_l not used */

  /* h components in first adim rows of matrix */
  for (i = 0; i < adim; i++) {
    for (j = 0; j < depth; j++)
      a[j][i] = sa[i].coefs[j];
  }

  /* depth by depth square identity in second block of matrix */
  for (i = adim; i < adim + depth; i++) {
    for (j = 0; j < depth; j++)
      if ((i - adim) == j)
        a[j][i] = 1;
      else
        a[j][i] = 0;
  }
  /* if(show_deps) print_a_arr(adim+depth,depth); */
  /* The following is a factorization of the  array H from the */
  /* function h (stored as the upper part of a ) into a lower  */
  /* triangluar matrix L and a matrix B such that L = HB          */
  /* now do column operations to reduce top to lower triangular */
  /* remember that a is transposed to use pointers for columns */
  /* for each row ... */
  cols_done = 0;
  for (i = 0; i < adim; i++) {
    piv_row = i;
    piv_col = cols_done;
    while ((a[piv_col][piv_row] == 0) && (piv_col < depth))
      piv_col++;
    if (piv_col < depth) {
      m = piv_col;
      mval = a[m][piv_row];
      mval = mval * mval;
      k = 0;
      /* pick min non-zero term on row to right of cols_done */
      for (j = cols_done; j < depth; j++)
        if ((a[j][piv_row] != 0) &&
            ((a[j][piv_row] * a[j][piv_row]) < mval)) {
          m = j;
          mval = a[j][piv_row] * a[j][piv_row];
        }
      /* now move col m to col cols_done */
      tmp = a[m];
      a[m] = a[cols_done];
      a[cols_done] = tmp;
      /* now eliminate rest of row */
      for (j = cols_done + 1; j < depth; j++)
        if (a[j][piv_row] != 0) {
          find_mults(a[cols_done][piv_row],
                     a[j][piv_row], &p, &q, &r, &s);
          for (k = 0; k < adim + depth; k++) {
            z = a[cols_done][k] * p + a[j][k] * q;
            a[j][k] = a[cols_done][k] * r
              + a[j][k] * s;
            a[cols_done][k] = z;
          }
          if (a[cols_done][piv_row] == 0) {
            tmp = a[j];
            a[j] = a[cols_done];
            a[cols_done] = tmp;
          }
        }
      cols_done++;
    }
  }
  /* reduce system by gcd of each column */
  for (j = 0; j < depth; j++) {
    z = gcd(depth + adim, a[j]);
    if (z != 1 && z != 0) {
      for (k = 0; k < adim + depth; k++)
        a[j][k] = a[j][k] / z;
    }
  }

  /* now back solve for x in dist = Lx */
  nosolution = 0;
  cur_x = 0;
  for (j = 0; (j < adim && cur_x < depth); j++) {
    z = 0;
    for (k = 0; k < cur_x; k++)
      z = z + a[k][j] * x[k];
    if (a[cur_x][j] == 0) {
      if (z != dist[j]) {
        nosolution = 1;
      }
      /* this equation is consistent, so skip it */
    }
    else {
      r = (dist[j] - z) / a[cur_x][j];
      if (r * a[cur_x][j] != dist[j] - z) {
        nosolution = 1;
      }
      x[cur_x] = r;
      cur_x++;
    }
  }
  for (j = cur_x; j < depth; j++) x[j] = 0;


  /* the following is a double check on the solution */

  for (j = 0; j < adim; j++) {
    z = 0;
    for (k = 0; k < depth; k++)
      z = z + a[k][j] * x[k];
    if (z != dist[j])
      nosolution = 1;
  }
  /* if there is no solution then there is no dependence! */
  if (nosolution) {
    troub[0] = 1;
    return (depth - cols_done);
  }
  /* because L = HB where B is the lower block of a       */
  /* and dist = Lx  we have dist = HBx, so if c = Bx, dist = Hc    */
  for (j = 0; j < depth; j++) {
    c[j] = 0;
    for (k = 0; k < depth; k++)
      c[j] = c[j] + a[k][j + adim] * x[k];
  }
  /* to compute vec and troub, we start by setting */
  /* vec to c.  (if ker(h) =0) we are done then  */
  for (j = 0; j < depth; j++)
    vec[j + 1] = c[j];
  /* we now modify by the leading terms of the ker basis */
  for (j = cols_done; j < depth; j++) {
    /* find leading non-zero */
    z = -1;
    for (k = 0; k < depth; k++)
      if (z == -1 && a[j][k + adim] != 0)
        z = k;
    if (z > -1) {
      troub[z + 1] = PLUS;
    }
  }
  z = 100;
  for (j = 1; j < depth + 1; j++) {
    if (troub[j] == PLUS || vec[j] > 0)
      z = j;
    if (troub[j] != PLUS && vec[j] < 0 && z == 100) {
      troub[0] = 1;
      /* fprintf(stderr, " reject - wrong direction \n"); */
      return (depth - cols_done);
    }
    if (z < j && troub[j] == PLUS && vec[j] < 0)
      troub[j] = ZPLUS;
  }

  /* print_a_arr(adim+depth,depth); */
  return (depth - cols_done);
}

static int myabs(x)
int x;
{
  if (x < 0)
    return (-x);
  else
    return (x);
}

int eval_h(c, depth, i, val)
int c[];
int depth, i, val;
{
  depth = depth;                /* make lint happy, depth unused */

  return (c[i] * val);
}

int find_mults(a, b, p1, q1, r1, s1)
int a, b;
int *p1;
int *q1;
int *r1;
int *s1;
{
  /* upon return :      a*p+b*q  or a*r+b*s is 0 */
  int p, q, r, s, olda, oldb;

  olda = a;
  oldb = b;
  p = 1;
  q = 0;
  r = 0;
  s = 1;
  while (a * b != 0) {
    if (a == b) {
      r = r - p;
      s = s - q;
      b = 0;
    }
    else if (a == -b) {
      r = r + p;
      s = s + q;
      b = 0;
    }
    else if (myabs(a) < myabs(b)) {
      if (a * b > 0) {          /* same sign */
        r = r - p;
        s = s - q;
        b = b - a;
      }
      else {
        r = r + p;
        s = s + q;
        b = b + a;
      }
    }
    else {
      if (a * b > 0) {
        p = p - r;
        q = q - s;
        a = a - b;
      }
      else {
        p = p + r;
        q = q + s;
        a = a + b;
      }
    }
  }                             /* end while */

  if ((a != (olda * p + oldb * q)) || (b != (olda * r + oldb * s)))
    fprintf(stderr, " reduce failed!\n");
  *p1 = p;
  *q1 = q;
  *r1 = r;
  *s1 = s;
return 1;
}

void print_a_arr(rows, cols)
int rows, cols;
{
  int i, j;
  for (i = 0; i < rows; i++) {
    fprintf(stderr, "    | ");
    for (j = 0; j < cols; j++) {
      fprintf(stderr, " %d ", a_array[j][i]);
      if (j == cols - 1)
        fprintf(stderr, "        |\n");
    }
  }
}








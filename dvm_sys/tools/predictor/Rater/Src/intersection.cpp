/**************************************************************************
*                                                                         *
*  Module      : intersection.c                                           *
*                                                                         *
*  Function:  computing the intersection of regular sections              *
*                                                                         *
*  ONLY for INTERNAL USE                                                  *
*  =========================================                              *
*                                                                         *
*   (l1:u1:s1)  is  section 1                                             *
*   (l2:u2:s2)  is  section 2                                             *
*                                                                         *
*   (l3:u3:s3)  will be the intersection of section 1 and section 2       *
*                                                                         *
*  s_s_intersect (l1, u1, l2, u2, *l3, *u3)                               *
*                                                                         *
*     - special case : s1 = s2 = 1,  note that s3 = 1                     *
*                                                                         *
*  r_s_intersect (l1, u1, s1, l2, u2, *l3, *u3, *s3)                      *
*                                                                         *
*     - special case : s2 = 1   (makes computation easier)                *
*                                                                         *
*  r_r_intersect (l1, u1, s1, l2, u2, s3, *l3, *u3, *s3)                  *
*                                                                         *
*  f : (s_lb:...:s_str) -> (t_lb:...:t_str)                               *
*                                                                         *
*     (l1:u1:s1) is input, (l2:u2:s2) is f((l1:u1:s1))                    *
*                                                                         *
*  map_section (s_lb,s_str,t_lb,t_str, l1, u1, s1, *l2, *u2, *s2)         *
*                                                                         *
*                                                                         *
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <fstream>

using namespace std;

extern ofstream prot; 


        /*********************************************************
        *                                                        *
        *  functions for minimum and maximum                     *
        *                                                        *
        *********************************************************/

static int my_min (long val1, long val2)
{ if (val1 < val2)
     return (val1);
   else 
     return (val2);
} /* my_min */

static int my_max (long val1, long val2)
{ 
   if (val1 > val2)
     return (val1);
   else 
     return (val2);
} /* my_max */

/*********************************************************
*                                                        *
*  correct_upper_bound  of  low:high:step                *
*                                                        *
*     ->  returns high' that high' is value of section   *
*                                                        *
*   101:100:2  -> 100                                    *
*                                                        *
*********************************************************/

static int correct_upper_bound (long low, long high, long step)
{  
#ifdef P_DEBUG
   prot <<"correct upper bound, low = " << low << " high = " << high << ", step = " << step
	    <<" is " << (low + ( (high - low) / step) * step) << endl;
#endif
   if (low > high)
      return (high);
    else
      return (low + ( (high - low) / step) * step);
}

/*********************************************************
*                                                        *
*  raise_lower_bound (low1, low2, step)                  *
*                                                        *
*     low1 <= low2 <= low1+k*step   (k !minimal)         *
*                                                        *
*********************************************************/

static int raise_lower_bound (long low1, long low2, long step)
{ 
   if (low1 < low2)
    return (low1 + ((low2 - low1 - 1) / step + 1) * step);
   else
    return (low1);
}

/*********************************************************
*                                                        *
*  Intersection of  (l1 : u1)  and (l2 : u2)             *
*                                                        *
*********************************************************/

void s_s_intersect (long l1, long u1, long l2, long u2, long * l3, long *u3)
{ 
  *l3 = my_max (l1, l2);
  *u3 = my_min (u1, u2);

# ifdef P_DEBUG
   prot << "s_s_intersect: (" << l1 << ':' << u1 << ") * (" << l2 << ':' << u2
	    << ") = (" << *l3 << ':' << *u3 << ')' << endl;
# endif

} /* s_s_intersect */

/*********************************************************
*                                                        *
*  Intersection of  (l1 : u1 : s1)  and (l2 : u2)        *
*                                                        *
*********************************************************/

void r_s_intersect (long l1, long u1, long s1, long l2, long u2, long * l3, long * u3, long * s3)
{ 
  long high;

//====
  if(s1==0) s1=1;
//=***

  if (s1 == 1)

    {  s_s_intersect (l1, u1, l2, u2, l3, u3);
       *s3 = s1;
    }

  else if (s1 < 0)

    {  r_s_intersect (correct_upper_bound (l1, u1, s1), l1, -s1, 
                            l2, u2, l3, u3, s3);
       *s3 = - *s3;
       high = *u3;
       *u3 = *l3;
       *l3 = high;
    }

  else

    { high = my_min (correct_upper_bound (l1, u1, s1), u2);
      *l3  = raise_lower_bound (l1, l2, s1);
      *u3  = correct_upper_bound (*l3, high, s1);
      *s3  = s1;
    }

# ifdef P_DEBUG
   prot << "r_s_intersect: (" << l1 << ':' << u1 << ':' << s1 << ") * ("
	    << l2 << ':' << u2 << ") = (" << *l3 << ':' << *u3 << ':' << *s3
		<< ')' << endl;
# endif

} /* r_s_intersect */

/**********************************************************
*                                                         *
*  gcd (a, b, x, y, g)                                    *
*                                                         *
*  solves equation:  g = x * a - y * b                    *
*                                                         *
*        with  x > 0,  and y > 0                          *
*                                                         *
*        g is greatest common divisor of a and b          *
*                                                         *
**********************************************************/

static void gcd (long a, long b, long * x, long * y, long * g)
{ 
  long  d, r;

  if (b == 0)
 
    { *x = 1; *y = 0; *g = a; }

  else

    { /* we can divide a by b */

      d = a / b;
      r = a % b;    /* a = d * b + r */

      gcd (b, r, y, x, g);

      /* note : g = y * b - x * r
                  = y * b - x * a + x * d * b 
                  = - x * a - (- x * d - y) * b 
                  = (b - x) * a - (a - x * d - y ) * b
      */
 
      *y = a - *y - *x * d;
      *x = b - *x;

    }
}

/**********************************************************
*                                                         *
*  input : diff , diff >= 0                               *
*          s1 > 0, s2 > 0                                 *
*                                                         *
*  find  k1 >= 0, k2 >= 0 with                            *
*                                                         *
*         k1 * s1 = diff + k2 * s2  = add                 *
*                                                         *
**********************************************************/

static void find_lower_bound (long diff, long s1, long s2, long * found, long * add, long * s3)
{  
   long x, y, g;
   long kgV;

   long k1;

   gcd (s1, s2, &x, &y, &g);

   /* g = x * s1 - y * s2  */

   kgV = s1 * s2 / g;
   *s3 = kgV;

   /* Idea : find k1 with  k1 * s1 = diff (mod s2) 

      hd = diff / g
     
      diff = hd * g = hd * x * s1 - hd * y * s2

      so we know that (hd * x) = diff (mod s2)

   */

   if (diff % g != 0)
     { *found = 0;
       *add   = 0;
     }
   else 

     { *found = 1;

       /* solution 1 : 
 
          k1   = (diff * x / g) % s2;
 
          causes serious error as diff * x can become out of range

          see example prime.hpf with N = 10.000.000 and P = 2 */

       k1   = ( (diff / g % s2)  * (x % s2) ) % s2; 

       *add = k1 * s1;

       /* now make sure that *add + x * kgV is >= diff */

       *add = raise_lower_bound (*add, diff, kgV);
     }
} /* find_lower_bound */

/**********************************************************
*                                                         *
*  intersection (l1:u1:s1, l2:u2:s2)                      *
*                                                         *
**********************************************************/

void r_r_intersect (long l1, long u1, long s1, long l2, long u2, long s2, 
					long * l3, long * u3, long * s3)
{ 
  long high;

  long found, add;

  //====
  if(s1==0) s1=1;
  if(s2==0) s2=1;
	//=***

  if (s2 == 1)
     r_s_intersect (l1, u1, s1, l2, u2, l3, u3, s3);
   else if (s1 == 1)
     r_s_intersect (l2, u2, s2, l1, u1, l3, u3, s3);
   else if (s1 < 0)
     { r_r_intersect (correct_upper_bound (l1, u1, s1), l1, -s1, 
                      l2, u2, s2, l3, u3, s3);
       /* inverse the result range */
       *s3 = - *s3;
       high = *u3;
       *u3 = *l3;
       *l3 = high;
     }
   else if (s2 < 0)
     r_r_intersect (l1, u1, s1, correct_upper_bound (l2, u2, s2), l2, -s2, l3, u3, s3);
   else

     { 
        high = my_min (correct_upper_bound (l1, u1, s1),
                       correct_upper_bound (l2, u2, s2));

        /* find l3 with l3 = l1 + k1 * s1, l3 = l2 + k2 * s2

           or k1 * s1 = l2 - l1 + k2 * s2 */

        if (l1 <= l2)
          { find_lower_bound (l2 - l1, s1, s2, &found, &add, s3);
            *l3 = l1 + add;
          }
         else
          { find_lower_bound (l1 - l2, s2, s1, &found, &add, s3);
            *l3 = l2 + add;
          }

        if (found == 0)
           { *l3 = l1;
             *u3 = l1 - *s3;
           }
          else
             *u3 = correct_upper_bound (*l3, high, *s3);
     }

} /* r_r_intersect */

/**********************************************************
*                                                         *
*  mapping is defined by (s_lb:s_ub:s_str)                *
*                     to (t_lb:t_ub:t_str)                *
*                                                         *
*  map :   s_lb           -> t_lb                         *
*          s_lb +   s_str -> t_lb +   t_str               *
*          s_lb + 2*s_str -> t_lb + 2*t_str               *
*          .....                                          *
*                                                         *
*  find map of (l1:u1:s1), is (l2:u2:s2)                  *
*                                                         *
**********************************************************/

static void map_normal (long x, long y, long base, long str, long * x1, long * y1)
{  /* x = base + x1 * str ,  y = base + x2 * str */

   /* attention: for empty sections y (str > 0) or x (str < 0)
      might be not well defined                                 */

   if (str > 0)
      { if (x > y) y = x - str; }

   if (str < 0)
      { if (x < y) x = y + str; }
   
   *x1 = (x - base) / str;
   *y1 = (y - base) / str;

   if (base + *x1 * str != x)
    { prot << "map normal has serious problems" << endl;
      prot << "x(=" << x << ") != base(=" << base << ") + x1(=" << *x1 
		   << ") * str(=" << str << ')' << endl;
      exit (0);
    }

   if (base + *y1 * str != y)
    { prot << "map normal has serious problems" << endl;
      prot << "y(=" << y << ") != base(=" << base << ") + y1(=" << *y1 
		   << ") * str(=" << str << ')' << endl;
      exit (0);
    }

#ifdef P_DEBUG
   prot << "map normal : " << x << " = " << base << " + (x1=" << *x1 << ") * " << str
	    << ", " << y << " = " << base << " + (y1=" << *y1 << ") * " << str << endl;
#endif

} /* map_normal */


//	s_lb, s_str, t_lb, t_str;    - definition of mapping
//  l1, u1, s1, *l2, *u2, *s2;   - source and target sections

void map_section (long s_lb, long s_str, long t_lb, long t_str, long l1, long u1, long s1,
				  long * l2, long * u2, long * s2)
{  
//   long hl1, hu1;

   if (s_str == t_str)

    { /* e.g. [5::2] -> [10::2],
              [21:41:10] becomes [26:46:10] */

      *l2 = l1 + t_lb - s_lb;
      *u2 = u1 + t_lb - s_lb;
      *s2 = s1;

    }

   else if (s_str == 1)

    { /* e.g. [5:] -> [10::4]
              [20:30:5] becomes [70:110:20] */

      *l2 = t_lb + (l1 - s_lb) * t_str;
      *u2 = t_lb + (u1 - s_lb) * t_str;
      *s2 = s1 * t_str;

    }

   else if (t_str == 1)

    { /* e.g. [10:400:4] -> [5:]  
              [70:110:20] becomes [20:30:5] */

      map_normal (l1, u1, s_lb, s_str, l2, u2);

      *l2 += t_lb;
      *u2 += t_lb;
      *s2 = s1 / s_str;

      /* Problem: [1::2] -> [1::1], map([1:0:2]) = [1:1:1]
 
         so make sure that u1 = s_lb + k * str             */

    }

   else

    { /* there are really different strides */

      map_normal (l1, u1, s_lb, s_str, l2, u2);

      *l2 *= t_str; *l2 += t_lb;
      *u2 *= t_str; *u2 += t_lb;
      *s2 = s1 / s_str * t_str;

    }

# ifdef P_DEBUG
  prot << "map : [" << s_lb << "::" << s_str << "] -> [" << t_lb << "::" << t_str << "], map(["
	   << l1 << ':' << u1 << ':' << s1 << "]) = [" << *l2 << ':' << *u2 << ':' << *s2 << ']' << endl;
# endif 

}

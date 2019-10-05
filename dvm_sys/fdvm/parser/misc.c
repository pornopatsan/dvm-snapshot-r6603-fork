/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * misc.c
 *
 * Misellanious help routines
 */

#include "defs.h"
#include "defines.h"
#include <ctype.h>
#include "db.h"
#include <stdlib.h>
#include <string.h>

extern int blklevel;
extern void warn1();
extern PTR_BFND cur_scope();
PTR_LABEL make_label();
extern PTR_FILE fi;
void free();

extern PTR_CMNT comments;
extern PTR_FNAME cur_thread_file;
extern PTR_FNAME the_file;
extern int yylineno;
extern int mod_offset;
extern PTR_BFND last_bfnd;
extern PTR_BFND head_bfnd, last_bfnd;
extern PTR_LLND head_llnd;
extern PTR_SYMB head_symb;
extern PTR_TYPE head_type;
extern PTR_LABEL head_label;

/*
 * eqn -- checks if first n characters of two strings are the same
 *
 * input:
 *	  n - length to be checked
 *	  a - string1
 *	  b - string2
 *
 * output:
 *	  YES if the first n characters are same.  NO, otherwise.
 */
int 
eqn(n, a, b)
	register int n;
	register unsigned char *a, *b;
{
	while (--n >= 0)
		if ((isupper(*a) ? tolower(*a++) : *a++) != *b++)
			return (NO);
	return (YES);
}

/*
 * StringConcatenation -- concatenate strings
 */
char *StringConcatenation(char *s1, char*s2)
{
   char *res = (char*)malloc(strlen(s1)+strlen(s2)+1);   
   res[0] = '\0';
   strcat(res, s1);
   strcat(res, s2);
   return res;  
}

/*
 * convci - converts an ASCII string to binary
 *
 * input:
 *	  n - length of the string
 *	  s - the string to be converted
 *
 * output:
 *	  the converted long value
 */
long 
convci(n, s)
	register int n;
	register char *s;
{
	register long sum;

	sum = 0;
	while (n-- > 0)
		sum = 10 * sum + (*s++ - '0');
	return (sum);
}


/*
 * convic -- converts a long integer to ASCII string
 *
 * input:
 *	  n - the number to be converted
 *
 * output:
 *	  the converted string
 */
char   *
convic(n)
	long    n;
{
	static char s[20];
	register char *t;

	s[19] = '\0';
	t = s + 19;

	do {
		*--t = '0' + n % 10;
		n /= 10;
	} while (n > 0);

	return (t);
}


/*
 * Get a BIF node
 */
PTR_BFND 
get_bfnd(fid,node_type, symb_ptr, ll1, ll2, ll3)
PTR_FILE fid;
int     node_type;
PTR_SYMB symb_ptr;
PTR_LLND ll1, ll2, ll3;
{
	PTR_BFND new_bfnd, make_bfnd();

	new_bfnd = make_bfnd(fid, node_type, symb_ptr, ll1, ll2, ll3);
        new_bfnd->filename = the_file;
	/*new_bfnd->filename = cur_thread_file;*/ /*podd 18.04.99*/
        new_bfnd->entry.Template.cmnt_ptr = comments;
	new_bfnd->entry.Template.bl_ptr1 = BLNULL;
	new_bfnd->entry.Template.bl_ptr2 = BLNULL;
	new_bfnd->g_line = yylineno;
	new_bfnd->l_line = yylineno - mod_offset;
	last_bfnd = new_bfnd;
	return (new_bfnd);
}


void
release_nodes()
{
	register PTR_BFND p1 = head_bfnd;
	register PTR_LLND p2 = head_llnd;
	register PTR_SYMB p3 = head_symb;
	register PTR_TYPE p4 = head_type;
	register PTR_LABEL p5 =head_label;
	register PTR_BFND t1;
	register PTR_LLND t2;
	register PTR_SYMB t3;
	register PTR_TYPE t4;
	register PTR_LABEL t5;

	while (p1) {
		t1 = p1;
		p1 = p1->thread;
		free ((char *)t1);
	}
	
	while (p2) {
		t2 = p2;
		p2 = p2->thread;
		free ((char *)t2);
	}
	
	while (p3) {
		t3 = p3;
		p3 = p3->thread;
		free ((char *)t3);
	}
	
	while (p4) {
		t4 = p4;
		p4 = p4->thread;
		free ((char *)t4);
	}
	
	while (p5) {
		t5 = p5;
		p5 = p5->next;
		free ((char *)t5);
	}
}


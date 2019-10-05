/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * errors.c
 *
 * Miscellaneous error routines
 */

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "symb.h"
#include "extern.h"


static char buff[100];

void format_num (int num, char num3s[])
{
 if(num>99)
   sprintf(num3s,"%3d",num);
 else if(num>9)
   sprintf(num3s,"0%2d",num);
 else 
  sprintf(num3s,"00%1d",num);
}


/*
 * fatal -- print the fatal error message then exit
 *
 * input:
 *	  s - the message to be printed out
 *        num  - error message number
 */
void 
fatal(s,num)
	char   *s;
        int num;
{char num3s[4];
   format_num(num,num3s);
   (void)fprintf(stderr, "Error %s on line %d of %s: %s\n", num3s,yylineno, infname, s);
	exit(3);
}


/*
 * fatalstr -- sets up the error message according to the given format
 *		then call "fatal" to print it out
 *
 * input:
 *	  t - a string that specifies the output format
 *	  s - a string that contents the error messaged to be formatted
 *        num  - error message number
 */
void 
fatalstr(t, s, num)
	char   *t, *s;
        int num;
{
	(void)sprintf(buff, t, s);
	fatal(buff,num);
}


/*
 * fatali -- formats a fatal error message which contains a number
 *	     then call "fatal" to print it
 *
 * input:
 *	  t - a string that specifies the output format
 *	  d - an integer to be converted according to t
 */
void 
fatali(t, d)
	char   *t;
	int     d;
{
	(void)sprintf(buff, t, d);
	fatal(buff,0);
}

/*
 * err_fatal -- print the fatal error message then exit
 *
 * input:
 *	  s - the message to be printed out
 */
void 
err_fatal(s, num)
	char   *s;
        int num;
{  char num3s[4];
   format_num(num,num3s);
   (void)fprintf(stderr, "Error %s: %s\n", num3s, s);
	exit(3);
}


/*
 * errstr_fatal -- sets up the error message according to the given format
 *		then call "fatal" to print it out
 *
 * input:
 *	  t - a string that specifies the output format
 *	  s - a string that contents the error messaged to be formatted
 */
void 
errstr_fatal(t, s, num)
	char   *t, *s;
        int num;
{
	(void)sprintf(buff, t, s);
	err_fatal(buff, num);
}



/*
 * warn1 -- formats a warning message then call "warn" to print it out
 *
 * input:
 *	  s - string that specifies the conversion format
 *	  t - string that to be converted according to s
 *	  n - warning message number
 */
void 
warn1(s, t, num)
	char   *s, *t;
        int num;
{
   void warn();

   (void)sprintf(buff, s, t);
   warn(buff,num);
}


/*
 * warn -- print the warning message if specified
 *
 * input:
 *	  s - string to be printed
 *	  num - warning message number
 */
void
warn(s, num)
char *s;
int num;
{char num3s[4];
   format_num(num,num3s);
   if (!nowarnflag) {
      ++nwarn;
      (void)fprintf(stderr, "Warning %s on line %d of %s: %s\n", num3s,yylineno, infname, s);
   }
}


/*
 * errstr -- formats the non-fatal error message then call "err" to print it
 *
 * input:
 *	  s - string that specifies the conversion format
 *	  t - string that to be formated according to s
 *	  num - error message number
 */
void 
errstr(s, t, num)
	char   *s, *t;
        int num;
{ 
   void err();
   (void)sprintf(buff, s, t);
   err(buff,num);
}


/*
 * erri -- formats an error number then prints it out
 *
 * input:
 *	  s - string that specifies the output format
 *	  t - number to be formatted
 *	  num - error message number
 */
void
erri(s, t, num)
	char   *s;
	int     t,num;
{
   void err();

   (void)sprintf(buff, s, t);
   err(buff,num);
}


/*
 * err -- prints the error message and does the bookkeeping
 *
 * input:
 *	  s - string to be printed out
 *	  num - error message number
 */
void
err(s, num)
	char   *s;
        int num;
{       char num3s[4];
        format_num(num,num3s);
	if (! errcnt++)
		errline = yylineno;
	(void)fprintf(stderr,"Error %s on line %d of %s: %s\n", num3s,yylineno,infname, s);
/*	exit(3);*/
}
/*
 * err_line -- prints the error message and does the bookkeeping
 *
 * input:
 *	  s - string to be printed out
 *	  num - error message number
 *        ln  - string number
 */
void
err_line(s, num, ln)
	char   *s;
        int num, ln;
{       char num3s[4];
        format_num(num,num3s);
	if (! errcnt++)
		errline = yylineno;
	(void)fprintf(stderr,"Error %s on line %d of %s: %s\n", num3s,ln,infname, s);
}

/*
 * errg -- prints the error message (without line number)
 *
 * input:
 *	  s - string to be printed out
 *	  num - error message number
 */
void
errg(s, num)
	char   *s;
        int num;
{       char num3s[4];
        format_num(num,num3s);
	if (! errcnt++)
		errline = yylineno;
	(void)fprintf(stderr,"Error %s: %s\n", num3s, s);
/*	exit(3);*/
}


/*
 * yyerror -- the error handling routine called by yacc
 *
 * input:
 *	  s - the error message to be printed
 */
void 
yyerror(s)
	char   *s;
    
{
	err(s,14); /* 14 - syntax error */
}


/*
 * dclerr -- prints the error message when find error in declaration part
 *
 * input:
 *	  s - error message string
 *	  v - pointer to the symble table entry
 */
void 
dclerr(s, v)
	char   *s;
	PTR_SYMB v;
{
	char    buf[100];

	if (v) {
		(void)sprintf(buf,"Declaration error for %s: %s",v->ident, s);
		err(buf,0);
	} else
		errstr("Declaration error: %s", s,0);
}


/*
 * execerr -- prints error message for executable part
 *
 * input:
 *	  s - the error message string1
 *	  n - the error message string2
 */
void 
execerr(s, n)
	char   *s, *n;
{
	char    buf1[100], buf2[100];

	/*(void)sprintf(buf1, "Execution error: %s", s);*/
        (void)sprintf(buf1, "%s", s);
	(void)sprintf(buf2, buf1, n);
	err(buf2,0);
}

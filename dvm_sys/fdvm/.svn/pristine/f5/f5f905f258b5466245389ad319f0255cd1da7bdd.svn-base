/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993,1995             */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* Created By Jenq-Kuen Lee  April 14, 1988     */
/* A Sub-program to help yylex() catch all the comments */
/* A small finite automata used to identify the input token corresponding to */
/* Bif node position */

#include <stdio.h>
#include "vparse.h"
#include "vpc.h"
#include "db.h"
#include "vextern.h"
#include "annotate.tab.h"

void reset_semicoln_handler();
void reset();

int lastdecl_id; /* o if no main_type appeared */
int left_paren ;
static int cur_state ;
int cur_counter;

struct {
         PTR_CMNT stack[MAX_NESTED_SIZE];
         int      counter[MAX_NESTED_SIZE];
	 int      node_type[MAX_NESTED_SIZE];
	 int      automata_state[MAX_NESTED_SIZE];
         int      top ;
       }  comment_stack ;


struct {
         PTR_CMNT stack[MAX_NESTED_SIZE + 1 ];
         int      front ;
         int      rear ;
       }  comment_queue;

struct {
         int      line_stack[MAX_NESTED_SIZE + 1 ];
	 PTR_FNAME file_stack[MAX_NESTED_SIZE + 1 ];
         int      front ;
         int      rear ;
	 int      BUGGY[100]; /* This is included because some versions of
                                 gcc seemed to have bugs that overwrite
                                 previous fields without.  */
       }  line_queue;


PTR_FNAME find_file_entry()
{
  /* dummy, should not be use after cleaning */
  return NULL;
}


void put_line_queue(line_offset,name)
int line_offset ;
char *name;
{  PTR_FNAME find_file_entry();

   if (line_queue.rear == MAX_NESTED_SIZE) line_queue.rear = 0;
   else line_queue.rear++;
   if (line_queue.rear == line_queue.front) Message("stack/queue overflow",0);
   line_queue.line_stack[line_queue.rear] = line_offset ;
   line_queue.file_stack[line_queue.rear] = find_file_entry(name);
}


PTR_FNAME
fetch_line_queue(line_ptr )
int *line_ptr;
{
   if (line_queue.front == line_queue.rear)
     { *line_ptr = line_queue.line_stack[line_queue.front] ;
       return(line_queue.file_stack[line_queue.front]);
     }
   if (line_queue.front == MAX_NESTED_SIZE) line_queue.front = 0;
   else line_queue.front++;
   *line_ptr = line_queue.line_stack[line_queue.front] ;
   return(line_queue.file_stack[line_queue.front]);
}


void push_state()
{
  comment_stack.top++;
  comment_stack.stack[  comment_stack.top ] = cur_comment ;
  comment_stack.counter[  comment_stack.top ] = cur_counter ;
  comment_stack.automata_state[  comment_stack.top ] = cur_state ;
}

void pop_state()
{  
  
   cur_comment  =  comment_stack.stack[  comment_stack.top ] ;
   cur_counter =  comment_stack.counter[  comment_stack.top ] ;
   cur_state  =  comment_stack.automata_state[  comment_stack.top ]  ;
   comment_stack.top--;
	
}

void init_stack()
{
  comment_stack.top = 0 ;
  comment_stack.automata_state[  comment_stack.top ] = ZERO;
}



void automata_driver(value)
int value ;
{
 int shift_flag  ;
 int temp_state ;



 for (shift_flag = ON  ; shift_flag==ON ;    )
{  shift_flag = OFF ;

   switch(cur_state) {

   case ZERO :

             switch (value) {
             case IF :
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = IF_STATE;
                   break ;
	     case ELSE :
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = ELSE_EXPECTED_STATE ;
                   break;
             case DO :
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = DO_STATE ;
		   break;
	     case FOR :
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = FOR_STATE ;
		   break;
             case CASE :
             case DEFAULT_TOKEN:
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = CASE_STATE;
                   break;
             case GOTO :
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = GOTO_STATE;
		   break;
             case WHILE :
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = WHILE_STATE;
		   break;
             case SWITCH:
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = SWITCH_STATE;
		   break;
	     case COEXEC :
                   cur_state = COEXEC_STATE ;
                   put_line_queue(line_pos_1,line_pos_fname); 
		   break;
	     case COLOOP:
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = COLOOP_STATE ;
		   break;
	     case RETURN:
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = RETURN_STATE ;
		   break;
             case '}':
                   pop_state();
		   switch (cur_state) {
		   case ELSE_EXPECTED_STATE:
		     put_line_queue(line_pos_1,line_pos_fname); 
		     break;
		   case STATE_4:
		   case BLOCK_STATE:
		     put_line_queue(line_pos_1,line_pos_fname); 
		     reset();
		     reset_semicoln_handler();
		     break;
		   case IF_STATE_4:
		     cur_state= ELSE_EXPECTED_STATE;
		     put_line_queue(line_pos_1,line_pos_fname); 
		     break;
		   case DO_STATE_1:
		     cur_state= DO_STATE_2;
		     reset_semicoln_handler();
		     break;
		   case DO_STATE_2:
		   case STATE_2:
		     break;
		   default:
		     reset();
		     reset_semicoln_handler();
		   }

                   break ;

		 case '{':    
                   temp_state=comment_stack.automata_state[comment_stack.top];
                   if (temp_state == STATE_ARG)
		      comment_stack.automata_state[comment_stack.top]= STATE_4;
                   else {   cur_state = BLOCK_STATE ;
                            put_line_queue(line_pos_1,line_pos_fname); 
                            push_state();
			  }
                   reset();
                   break ;
             case '(':
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = STATE_15;
                   left_paren++;
                   break;
             case IDENTIFIER:
                   put_line_queue(line_pos_1,line_pos_fname); 
                   cur_state = STATE_6 ;
		   break;
             case ';':
		   reset_semicoln_handler();
		   break;
             default : /* other */
                   put_line_queue(line_pos_1,line_pos_fname); 
                   if (class_struct(value)) cur_state = STATE_10 ;
                   else                     cur_state =  STATE_1 ; 
                   break;
		 }
              break;
  case STATE_1 :
          if (value == '(')     {   cur_state =STATE_15 ;
                                    left_paren++;
				  }
          if (class_struct(value))  cur_state =STATE_10 ;
          if (value == IDENTIFIER)  cur_state =STATE_2 ;
          if (value == OPERATOR)    cur_state =STATE_4 ;
	  if (value ==';')  reset_semicoln_handler();
	     break ;

  case STATE_2 :
          if (value == '(')    {    cur_state = STATE_15 ;
				    left_paren++;
				  }
	  if (value ==';')  { 
	    reset();
	    reset_semicoln_handler();
	  }
          break;

  case STATE_4:
          switch (value) {
          case  '(':
                 cur_state =  STATE_15 ;
                 left_paren++;
		 break;
          case '{':         /*  cur_state = STATE_5; */
	         push_state();
                 reset();
		 break;
	  case  '=':
          case  ',':
                 cur_state = STATE_12;
                 break;
	       case  ';':
		 reset_semicoln_handler();
		 break;
          default:
              if (is_declare(value))
	        {  cur_state = STATE_ARG ;
                   push_state();
		   reset();
		 }
	      else      cur_state = STATE_12;
	     }
	   
          break;
   case STATE_6:
          if (value == ':') cur_state = ZERO;
          else {
	    if (value ==';') reset_semicoln_handler();
	    else {  cur_state = STATE_2;
                      shift_flag = ON ;
		    }
	  }
          break;
  case STATE_10 :
          if (value =='{')
	    { cur_state = STATE_2 ;
	      push_state();
	      reset();
	    }
          if ((value == '=' )||(value ==','))  cur_state = STATE_12;
	  if (value == '(' )    {     cur_state = STATE_15;
					 left_paren++;
				       }
	  if (value ==';') reset_semicoln_handler();
	  break ;
  case STATE_12:
	  if (value ==';') reset_semicoln_handler();
           break ;

  case  STATE_15 :
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = STATE_4 ;
           break ;
  case IF_STATE:
           if (value == '(') { left_paren++;
                               cur_state = IF_STATE_2;
			     }
           break;
  case IF_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = IF_STATE_3 ;
           break ;
  case IF_STATE_3:
	     if (value == ';') {
	       put_line_queue(line_pos_1,line_pos_fname); 
	       cur_state= ELSE_EXPECTED_STATE ;
	     }
           if (value =='{') {  cur_state= ELSE_EXPECTED_STATE ;
                               push_state();
			       cur_state = ZERO ; /* counter continuing */
			     }
           if (cur_state == IF_STATE_3) 
	     {  cur_state = IF_STATE_4 ;
                push_state();
                reset();
		shift_flag = ON;
	      }
           break;

  case ELSE_EXPECTED_STATE:
           if (value == ELSE) cur_state = BLOCK_STATE ;
           else  { 
	     reset();
	     reset_semicoln_handler();
	     shift_flag = ON ;
	   }
           break;

  case BLOCK_STATE:
	  if (value ==';') {
	    cur_state =  BLOCK_STATE_WAITSEMI;
	    push_state();
	    reset_semicoln_handler();
	  }
	  if (value == '{') { push_state();
			      reset();
			    }
          if (cur_state == BLOCK_STATE)
	    {  
	      cur_state =  BLOCK_STATE_WAITSEMI;
	      push_state();
	      reset();
	      shift_flag = ON ;
	    }
	  break;
	     
  case WHILE_STATE:
           if (value == '('){ left_paren++;
                              cur_state = WHILE_STATE_2;
			    }
           break;
  case WHILE_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = BLOCK_STATE ;
           break ;

  case FOR_STATE:
           if (value == '(') { left_paren++;
                               cur_state = FOR_STATE_2;
			     }
           break;
  case FOR_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = BLOCK_STATE ;
           break ;

  case COLOOP_STATE:
           if (value == '(') { left_paren++;
			       cur_state = COLOOP_STATE_2;
			     }
           break;
  case COLOOP_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = BLOCK_STATE ;
           break ;

  case COEXEC_STATE:
           if (value == '(') { left_paren++;
			       cur_state = COEXEC_STATE_2;
			     }
           break;
  case COEXEC_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = BLOCK_STATE ;
           break ;

  case SWITCH_STATE:
           if (value == '(') { left_paren++;
			       cur_state = SWITCH_STATE_2;
			     }
           break;
  case SWITCH_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = BLOCK_STATE ;
           break ;

  case CASE_STATE :
           if (value == ':') reset();
           break;
  case DO_STATE : /* Need More, some problem exists */
           if (value == ';') { cur_state = DO_STATE_2 ; }
           if (value == '{') { cur_state = DO_STATE_2 ;
                               push_state();
                               reset();
			     }
           if (cur_state == DO_STATE)
	     { cur_state = DO_STATE_1 ;
	       push_state();
	       reset();
	       shift_flag = ON;
	     }
           break;
   case DO_STATE_2:
           if (value == WHILE) cur_state= DO_STATE_3 ;
           break ;
   case DO_STATE_3:
	   if (value == '(') { cur_state = DO_STATE_4 ; 
                               left_paren++;
			     }
           break;
   case DO_STATE_4:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = DO_STATE_5 ;
           break ;
   case DO_STATE_5:
	  if (value ==';') 
	    { 
	      put_line_queue(line_pos_1,line_pos_fname); 
	      reset();
	      reset_semicoln_handler();
	    }
           break;
   case RETURN_STATE:
	  if (value ==';') reset_semicoln_handler();
	  if (value == '(') { left_paren++;
                                cur_state = RETURN_STATE_2 ;
			     }
           break;
   case RETURN_STATE_2:
           if (value == '(') left_paren++ ;
           if (value == ')')  left_paren--;
           if (left_paren == 0) cur_state = RETURN_STATE_3 ;
           break ;
   case RETURN_STATE_3:
	     if (value ==';') reset_semicoln_handler();
           break;
   case GOTO_STATE:
           if (value == IDENTIFIER) cur_state = GOTO_STATE_2 ;
           break;
   case GOTO_STATE_2:
	     if (value ==';') reset_semicoln_handler();
           break;
   default:
           Message(" comments state un_expected...",0);
           break;
	   }


 }

}

class_struct(value)
register int value ;
{
  switch (value) {
  case ENUM :
  case CLASS:
  case STRUCT :
  case UNION: return(1);
  default : return(0);
  }
}

declare_symb(value)
register int value ;
{
  switch (value) {
  case TYPENAME :
  case TYPESPEC:
  case TYPEMOD:
  case ACCESSWORD:
  case SCSPEC:
  case ENUM :
  case CLASS:
  case STRUCT :
  case UNION: return(1);
  default : return(0);
  }
}


void reset()
{
   cur_state = 0 ;
   cur_counter = 0 ;
   cur_comment     = (PTR_CMNT) NULL ; 

/*   put_line_queue(line_pos_1,line_pos_fname); */
 }

block_like(state)
int state ;
{

   switch( state) {
   case BLOCK_STATE:
   case ZERO:
   case SWITCH_STATE:
   case FOR_STATE :
   case WHILE_STATE :
   case COEXEC_STATE :
   case COLOOP_STATE:
   case STATE_4: /* end of function_body */
            return(1);
   default: return(0);
 }
}

int
is_declare(value)
int value ;
{
  switch (value) {
  case   TYPENAME:
  case   TYPESPEC :
  case   ACCESSWORD:
  case   SCSPEC:
  case   TYPEMOD:
  case   ENUM:
  case   UNION:
  case   CLASS:
  case   STRUCT:  return(1);
    default :     return(0);
  }
}



/* pop state until reach a stable state BLOCK_STATE or ZERO        */
void reset_semicoln_handler()
{
  int sw,state;

  for (sw=1; sw;  )
    {
      if (keep_original(cur_state)) return;
      state = comment_stack.automata_state[comment_stack.top];
      switch (state) {
      case IF_STATE_4:
	pop_state();
	cur_state = ELSE_EXPECTED_STATE ;
	put_line_queue(line_pos_1,line_pos_fname); 
	break;
      case  DO_STATE_1:
	pop_state();
	cur_state = DO_STATE_2 ;
	break;
      case BLOCK_STATE_WAITSEMI:
	put_line_queue(line_pos_1,line_pos_fname); 
	pop_state();
	reset();
	break;
      default :
	reset();
	sw = 0 ;
      }
    }

}


keep_original(state)
int state;
{
  switch (state) {
  case ELSE_EXPECTED_STATE:
  case DO_STATE_2:
  case STATE_2:
       return(1);
 default:
       return(0);
     }
}





/*****************************************************************************/
/* is_at_decl_state() & is_look_ahead_of_identifier()                        */
/* These two routines are used in yylex to identify if a TYPENAME is just    */
/* a IDENTIFIER                                                              */
/*                                                                           */
/*****************************************************************************/
int
is_at_decl_state()
{

  /* to see if it is inside (, )  */
  switch(cur_state) {
  case STATE_15:
  case IF_STATE_2:
  case WHILE_STATE_2:
  case FOR_STATE_2:
  case COLOOP_STATE_2:
  case COEXEC_STATE_2:
  case SWITCH_STATE_2:
  case DO_STATE_4:
    return(0);
  default:
    return(1);
  }
}


int is_look_ahead_of_identifier(c)
char c;
{
  switch (c) {
  case ':' :
  case '(':
  case '[':
  case ',':
  case ';':
  case '=':
    return(1);
  default:
    return(0);
  }

}


void set_up_momentum(value,token)
int value,token;
{

  if (lastdecl_id == 0)
    {
      /* check if main_type appears */
       switch (value) {
       case TYPESPEC:
	 lastdecl_id = 1;
	 break;
       case TYPEMOD:
	 if ((token == (int)RID_LONG)||(token == (int)RID_SHORT)||
	     (token==(int)RID_SIGNED)||(token==(int)RID_UNSIGNED))
         lastdecl_id = 1;
	 break;
       }
     }
  else
    {
      /* case for main_type already appear, then check if 
	 1. this is still a decl.
	 2. reset it to wait for another decl stat.   */
      switch (value) {
      case TYPESPEC:
      case TYPEMOD:
      case SCSPEC:
	break;
      default:
	lastdecl_id = 0;
      }
    }

}


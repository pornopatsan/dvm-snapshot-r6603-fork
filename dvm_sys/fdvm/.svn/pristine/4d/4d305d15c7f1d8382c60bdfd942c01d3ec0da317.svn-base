
/* This is a small prototype for the annotation system, it deliver a 
   set of llnode/bifnode for the annotation system */

%{
#include "macro.h"

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdlib> 
#ifdef _NEEDALLOCAH_
#  include <alloca.h>
#endif
 
#define ON 1
#define OFF 0
#define OTHER 2
#define ID_ONLY  1
#define RANGE_APPEAR 2
#define EXCEPTION_ON 4
#define EXPR_LR      8
#define VECTOR_CONST_APPEAR 16
#define ARRAY_OP_NEED 32
#define TRACEON 0

extern POINTER newNode();

%}

%start annotation
%union { 
         int       token ;
         char      charv ;
         char      *charp;
         PTR_BFND   bfnode ;
         PTR_LLND   ll_node ;
         PTR_SYMB   symbol  ;
         PTR_TYPE   data_type ;
         PTR_HASH   hash_entry ;
         PTR_LABEL  label ;        
         PTR_BLOB   blob_ptr ;
       }

/* Begin  Token for annotation system */
/* The IfDef token */
%token IFDEFA
/* the Apply to token */
%token APPLYTO
%token ALABELT
%token SECTIONT
%token SPECIALAF
%token FROMT
%token TOT
%token TOTLABEL
%token TOFUNCTION
%token DefineANN
/* End Token for annotation system */

/* all identifiers   that are not reserved words
   and are not declared typedefs in the current block */
%token IDENTIFIER
/* all identifiers   that are declared typedefs in the current block.
   In some contexts, they are treated just like IDENTIFIER,
   but they can also serve as typespecs in declarations.  */
%token TYPENAME

/* reserved words that specify storage class.
   yylval contains an IDENTIFIER_NODE which indicates which one.  */
%token SCSPEC

/* reserved words that specify type.
   yylval contains an IDENTIFIER_NODE which indicates which one.  */
%token TYPESPEC

/* reserved words that modify type: "const" or "volatile".
   yylval contains an IDENTIFIER_NODE which indicates which one.  */
%token TYPEMOD

/*character or numeric constants.
   yylval is the node for the constant.  */
%token CONSTANT

/* String constants in raw form.
   yylval is a STRING_CST node.  */
%token STRING

/* "...", used for functions with variable arglists.  */
%token ELLIPSIS

/* the reserved words */
%token SIZEOF ENUM STRUCT UNION IF ELSE WHILE DO FOR SWITCH CASE DEFAULT_TOKEN
%token BREAK CONTINUE RETURN GOTO ASM
%token CLASS PUBLIC FRIEND  ACCESSWORD OVERLOAD
%token OPERATOR COBREAK COLOOP COEXEC LOADEDOPR 

%token MULTIPLEID MULTIPLETYPENAME

/* Define the operator tokens and their precedences.
   The value is an integer because, if used, it is the tree code
   to use in the expression made from the operator.  */

%left  <charv> ','
%right <charv> '='
%right <token> ASSIGN 
%right <charv> '?' ':'
%left <charv> OROR
%left <charv> ANDAND
%left <charv> '|'
%left <charv> '^'
%left <charv> '&'
%left <token> EQCOMPARE
%left <token> ARITHCOMPARE  '>' '<' 
%left <charv> LSHIFT RSHIFT
%left <charv> '+' '-'
%left <token> '*' '/' '%'
%right <token> UNARY PLUSPLUS MINUSMINUS
%left HYPERUNARY
%left <token> DOUBLEMARK 
%left <token> POINTSAT '.'


%type <token> unop
%type <hash_entry> IDENTIFIER TYPENAME  LOADEDOPR
%type <ll_node> CONSTANT STRING  primary 
%type <ll_node> expr_no_commas const_expr_no_commas  
%type <ll_node> expr nonnull_exprlist exprlist const_primary element
%type <ll_node> string
%type <token>   SCSPEC TYPESPEC TYPEMOD 
%type <ll_node> vector_constant triplet  compound_constant vector_list
%type <ll_node> single_v_expr array_expr_a 
%type <ll_node> array_expr_b expr_vector 
%type <ll_node> expr_no_commas_1  
%type <symbol>  identifier    identifiers 
%type <token> ACCESSWORD 
%type <ll_node> IfDefR
%type <ll_node> Alabel
%type <ll_node> ApplyTo
%type <ll_node> LocalDeclare
%type <ll_node> Expression_List
%type <ll_node> declare_local_list
%type <ll_node> onedeclare
%type <ll_node> domain
%type <ll_node> section
%type <ll_node> SECTIONT
%type <hash_entry> SPECIALAF

%{ char      *input_filename;	
   extern    int lastdecl_id;
   PTR_LLND ANNOTATE_NODE = NULL;
   PTR_BFND ANNOTATIONSCOPE = NULL;
   extern PTR_SYMB newSymbol();
   extern PTR_LLND newExpr();
   extern PTR_LLND makeInt();
   static int cur_counter =  0; 
   static int primary_flag=  0;
   PTR_TYPE global_int_annotation = NULL;
   extern PTR_LLND Follow_Llnd();
   static int recursive_yylex = OFF;
   static int exception_flag = 0;
   static PTR_HASH cur_id_entry;
   int line_pos_1 = 0;
   char *line_pos_fname = 0;
   static int old_line = 0;
   static int yylineno=0;
   static int yyerror();
   PTR_CMNT cur_comment = NULL;
   PTR_CMNT new_cur_comment = NULL ;
   PTR_HASH look_up();
   PTR_HASH look_up_type();
   char *STRINGTOPARSE = 0;
   int PTTOSTRINGTOPARSE = 0;
   int LENSTRINGTOPARSE = 0;
   extern PTR_LLND Make_Function_Call();
   static PTR_LLND check_array_id_format();
   static PTR_LLND look_up_section();
   extern PTR_SYMB getSymbolWithName(); /*getSymbolWithName(name, scope)*/
   PTR_SYMB Look_For_Symbol_Ann();
   char AnnExTensionNumber[255]; /* to symbole right for the annotation  */
   static int Recog_My_Token();
   static int look_up_specialfunction();
   static unMYGETC();
   static MYGETC();
   static int map_assgn_op();
%}  

%%

annotation: /* empty */
	| '[' IfDefR   Alabel   ApplyTo   LocalDeclare ';' Expression_List ']'
        { 
	  ANNOTATE_NODE = newExpr(EXPR_LIST,NULL,$2,
	                   newExpr(EXPR_LIST,NULL,$3,
			     newExpr(EXPR_LIST,NULL,$4,
				newExpr(EXPR_LIST,NULL,$5,
				   newExpr(EXPR_LIST,NULL,$7,NULL)))));
	  if (TRACEON)
	    printf("Recognized ANNOTATION\n");
	}
        | '['Expression_List  ']'
        { 
	  ANNOTATE_NODE = newExpr(EXPR_LIST,NULL,NULL,
	                   newExpr(EXPR_LIST,NULL,NULL,
			     newExpr(EXPR_LIST,NULL,NULL,
				newExpr(EXPR_LIST,NULL,NULL,
				   newExpr(EXPR_LIST,NULL,$2,NULL)))));
	   if (TRACEON) printf("Recognized ANNOTATION\n");
	};
        

IfDefR: /* empty */
        {
	  $$ = NULL; 
        }
        | IFDEFA '(' string ')'
        {
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"IfDef", NULL);
	  $$ = Make_Function_Call (ids,NULL,1,$3);
	  if (TRACEON) printf("Recognized IFDEFA \n");
	};

Alabel: /* empty */
        {	
	  $$ = NULL; 
        }
        | ALABELT '(' string ')'
        {
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"Label", NULL);
	  $$ = Make_Function_Call (ids,NULL,1,$3);
	  if (TRACEON) printf("Recognized IFDEFA \n");
	  if (TRACEON) printf("Recognized ALABEL\n");
	};

ApplyTo: /* empty */
        {
	  $$ = NULL; 
        }
        |  APPLYTO '(' section ')'
        {
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"ApplyTo", NULL);
	  $$ = Make_Function_Call (ids,NULL,2,$3, NULL);
	   if (TRACEON) printf("Recognized APPLYTO \n");
	}
        |  APPLYTO '(' section ')' IF expr 
        {
	  PTR_SYMB ids = NULL;
	  /* need a symb there, will be global later */
          ids = Look_For_Symbol_Ann (FUNCTION_NAME,"ApplyTo", NULL);
	  $$ = Make_Function_Call (ids,NULL,2,$3,$6);
	   if (TRACEON) printf("Recognized APPLYTO \n");
	}; 

section : SECTIONT
          { /* SECTIONT return a string_val llnd */
            $$ = $1;
          }
          | TOFUNCTION IDENTIFIER
          {
	    
            $$ =  newExpr(VAR_REF,NULL,$2);
          }
          | FROMT string TOT string
          {  
             $$ = newExpr(EXPR_LIST,NULL,$2,
	                   newExpr(EXPR_LIST,NULL,$4,NULL));
          } 
          | TOT string
          {  
             $$ = newExpr(EXPR_LIST,NULL,NULL,
	                   newExpr(EXPR_LIST,NULL,$2,NULL));
          }
          | TOTLABEL string
          {  
             $$ = $2;
          }
          ;


LocalDeclare: /* empty */
        {
	  if (TRACEON) printf("Recognized LocalDeclare\n");
	  $$ = NULL; 
        }
        | declare_local_list
        {
	  $$ = $1;
	  if (TRACEON) printf("Recognized  declare_local_list\n");
	};
/******************* Annotation Expression Stuff ****************************/ 

Expression_List: /* empty */
        {
	  $$ = NULL; 
	  if (TRACEON) printf("Recognized empty expr\n");
        }
        | SPECIALAF '(' exprlist ')'
        { /* for Key word like parallel loop and so on */
	  PTR_SYMB ids = NULL;
	  ids = Look_For_Symbol_Ann (VARIABLE_NAME, $1,global_int_annotation);
	  $$ = Make_Function_Call (ids,NULL,1,$3);
	  if (TRACEON) printf("Recognized Expression_List SPECIALAF  \n");
	}
        | IDENTIFIER '(' exprlist ')'
        { /* for Key word like parallel loop and so on */
	  PTR_SYMB ids = NULL;
	  ids = Look_For_Symbol_Ann (VARIABLE_NAME, $1,global_int_annotation);
	  $$ = Make_Function_Call (ids,NULL,1,$3);
	  if (TRACEON) printf("Recognized Expression_List SPECIALAF  \n");
	}
        | DefineANN '(' string ',' CONSTANT ')'
        { /* for Key word like parallel loop and so on */
	  PTR_SYMB ids = NULL;
	  ids = Look_For_Symbol_Ann (FUNCTION_NAME, "Define" ,global_int_annotation);
	  $$ = Make_Function_Call (ids,NULL,2,$3,$5);
	  if (TRACEON) printf("Recognized Expression_List Define  \n");
	};   


/******************** LOCAL DECLARATION **********************************/
/* for local declaration */
declare_local_list: 
         {
	  $$ = NULL; 
        }
         | onedeclare
         {
           $$ =  newExpr(EXPR_LIST,NODE_TYPE($1),$1,NULL);
	   if (TRACEON) printf("Recognized onedeclare \n");
	 } 
          | declare_local_list  ',' onedeclare
         {
	   PTR_LLND ll_ptr ;
	   ll_ptr = Follow_Llnd($1,2);               
	   NODE_OPERAND1(ll_ptr) = newExpr(EXPR_LIST,NODE_TYPE($3),$3,NULL);
	   if (TRACEON) printf("Recognized declare_local_list _inlist \n");
	   $$=$1;
	 };

onedeclare:
           TYPESPEC IDENTIFIER domain
          {
	    PTR_SYMB ids = NULL;
	    PTR_LLND expr;
            PTR_HASH p;
	    char temp1[256];
	    
	    /* need a symb there, will be global later */
            p = $2;
	    strcpy(temp1,AnnExTensionNumber);
	    strncat(temp1,p->ident,255);
	    ids = newSymbol (VARIABLE_NAME,temp1,global_int_annotation);
	    expr = newExpr(VAR_REF,global_int_annotation, ids);
	    if ($3)
	      $$ = newExpr(ASSGN_OP,global_int_annotation,expr, $3);
            else
              $$ = expr;
          };
domain: 
       {
	  $$ = NULL; 
        }
       | '=' expr_no_commas
        {
	  $$ = $2;
	};


/********************* PARSER EXPRESSION ************************/
/* Must appear precede expr for resolve precedence problem */
/* A nonempty list of identifiers.  */
identifiers:	
	IDENTIFIER
		{ 
		  /* to modify, must be check before created */
		  $$ = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, $1, NULL); 
		  /* $$ = install_parameter($1,VARIABLE_NAME) ; */
                }
	| identifiers ',' IDENTIFIER
		{ 
		  $$ = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, $3, NULL);
		}
	;

identifier:
	IDENTIFIER
         { $$ = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, $1, NULL);}
	| TYPENAME
         { $$ = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, $1, NULL); }
        ;

unop:   '-'
		{ 
                 $$ = MINUS_OP ;
	       }
	| '!'
		{ 
                 $$ = NOT_OP ;
	       }
	;


expr:	nonnull_exprlist
		{ 
                  $$ = $1 ;
                }
	;

exprlist:
	  /* empty */
		{ 
                  $$ = LLNULL ;
                }
	| nonnull_exprlist
		{ 
                  $$ = $1 ; 
                }
	;

/* modified */
nonnull_exprlist:
          expr_no_commas
		{ 
                  $$ = newExpr(EXPR_LIST,NODE_TYPE($1),$1,NULL);
                }
	| nonnull_exprlist ',' expr_no_commas
		{ PTR_LLND ll_ptr ;
                  ll_ptr = Follow_Llnd($1,2);               
                  NODE_OPERAND1(ll_ptr) = newExpr(EXPR_LIST,NODE_TYPE($3),$3,NULL);

                  $$=$1;
                }
	;

/* modified */
vector_constant : '['  ']'   %prec ','
            {     
                  $$ = newExpr(VECTOR_CONST,NULL,NULL,NULL);
                  primary_flag = VECTOR_CONST_APPEAR ;
		  /* Temporarily setting */
                  NODE_TYPE($$) = global_int_annotation ;
		}
        | '['  vector_list  ']'  %prec ','
             {   
                 $$ = newExpr(VECTOR_CONST,NULL,$2,NULL);
                  primary_flag = VECTOR_CONST_APPEAR ;
		  /* Temporarily setting */
                  NODE_TYPE($$) = global_int_annotation ;
	       }
        ;

vector_list :
        {
           $$ = NULL;
        }
        |  single_v_expr
             { 
               $$ = newExpr(EXPR_LIST,NULL,$1,NULL);
             }
        | vector_list ',' single_v_expr
             {
               PTR_LLND ll_node1 ;
               ll_node1 = Follow_Llnd($1,2);
               NODE_OPERAND1(ll_node1)= newExpr(EXPR_LIST,NULL,$3,NULL);
	       $$=$1;
	     }

        ;

/* modified */
single_v_expr :
          const_expr_no_commas
             { 
               $$ = $1;
             }
        | triplet
             { 
               $$ = $1;
             }
        | compound_constant
             { 
               $$ = $1;
             }
        |   vector_constant
                { 
                  $$ = $1 ;
                }
        ;


 element:
         CONSTANT
            { 
                  $$ = $1 ;
	    }
	| IDENTIFIER
		{ 
		  $$ = newExpr(VAR_REF, NULL,Look_For_Symbol_Ann (VARIABLE_NAME, $1, NULL));
		  exception_flag = ON ;
		}
       ;

 triplet :
          element ':' element ':' element     %prec '.'

            { PTR_LLND p1,p2 ;
              p1 =  newExpr(DDOT,NULL,$1,$3);
              p2 = newExpr(DDOT,NULL,p1,$5);
              $$ = p2 ;
	    }
         | element ':' element               %prec '.'
            { 
              $$= newExpr(DDOT,NULL,$1,$3);
	    }
        ;


compound_constant :
          CONSTANT '#' CONSTANT
           { 
             $$=  newExpr(COPY_NODE,NULL,$1,$3);
	   }

        ;
/* modified */
array_expr_a :        /* empty */
            {
	      $$ = NULL;
	    }
        | expr_no_commas_1 ':' expr_no_commas_1 ':' expr_no_commas_1  %prec ','
            { PTR_LLND p1,p2 ;
	      p1 =  newExpr(DDOT,NULL,$1,$3);
              p2 = newExpr(DDOT,NULL,p1,$5);
              $$ = p2 ;
	    }
        | expr_no_commas_1 ':' expr_no_commas_1                     %prec ','
            {
              $$= newExpr(DDOT,NULL,$1,$3);
	    }
        ;


expr_no_commas_1 :
             {
                $$ = LLNULL ;
             } 
       | expr_no_commas
           { 
                 $$ = $1 ;
           }
       ;
/* modified */
array_expr_b :  expr_no_commas '#' expr_no_commas  
        ;


/* modified */
expr_vector : expr_no_commas   /* original is expr  */
        |  array_expr_a
        ;
 
expr_no_commas:
	primary
           { 
            /* Need Another way to check this one */
            /*  if (primary_flag & EXCEPTION_ON) Message("syntax error 6"); */
             if (exception_flag == ON)  { /* Message("undefined symbol",0); */
                                          exception_flag =OFF;
                                        }
             $$=$1 ;
	   }
	| unop primary %prec UNARY
		{	
		  $$=newExpr($1,NULL,$2);
		}
	| SIZEOF expr_no_commas  %prec UNARY
		{ 
		  $$= newExpr(SIZE_OP,global_int_annotation,$2,LLNULL);
                }
	| expr_no_commas '+' expr_no_commas
		{ 
		  $$=newExpr(ADD_OP,NULL,$1,$3);
                }
	| expr_no_commas '-' expr_no_commas
		{ 
		  $$=newExpr(SUBT_OP,NULL,$1,$3);
                }
	| expr_no_commas '*' expr_no_commas
		{ 
		  $$=newExpr(MULT_OP,NULL,$1,$3);
                }
	| expr_no_commas '/' expr_no_commas
		{ 
		  $$=newExpr(DIV_OP,NULL,$1,$3);
                }
	| expr_no_commas '%' expr_no_commas
		{ 
		  $$=newExpr(MOD_OP,NULL,$1,$3);
                }
	| expr_no_commas ARITHCOMPARE expr_no_commas
		{ int op1 ;
                  op1 = ($2 == ((int) LE_EXPR)) ? LE_OP : GE_OP ;
		  $$=newExpr(op1,NULL,$1,$3);
                }
	| expr_no_commas '<'  expr_no_commas  
                { 
		  $$=newExpr(LT_OP,global_int_annotation,$1,$3);
                }
	| expr_no_commas '>'  expr_no_commas   
                { 
		  $$=newExpr(GT_OP,global_int_annotation,$1,$3);
                }
	| expr_no_commas EQCOMPARE expr_no_commas
		{ int op1 ;
                  op1 = ($2 == ((int) NE_EXPR)) ? NE_OP : EQ_OP  ;
     		  $$=newExpr(op1,global_int_annotation,$1,$3);
                }
	| expr_no_commas '&' expr_no_commas
		{ 
		  $$=newExpr(BITAND_OP,global_int_annotation,$1,$3);
                }
	| expr_no_commas '|' expr_no_commas
		{ 
		  $$=newExpr(BITOR_OP,global_int_annotation,$1,$3);
                }
	| expr_no_commas '^' expr_no_commas
		{ 
		  $$=newExpr(XOR_OP,NULL,$1,$3);
                }
	| expr_no_commas ANDAND expr_no_commas
		{ 
		  $$=newExpr(AND_OP,global_int_annotation,$1,$3);
                }
	| expr_no_commas OROR expr_no_commas
		{ 
		  $$=newExpr(OR_OP,global_int_annotation,$1,$3);
                }
	| expr_no_commas '?' expr_no_commas ':' expr_no_commas  /* expr */
		{ PTR_LLND ll_node1;
		  ll_node1=newExpr(EXPR_IF_BODY,$3,$5);
		  $$=newExpr(EXPR_IF,NULL,$1,ll_node1);
		}
	| expr_no_commas '=' expr_no_commas
		{ 
		  $$=newExpr(ASSGN_OP,NULL,$1,$3);
                }
	| expr_no_commas ASSIGN expr_no_commas
		{ int op1 ;
                  op1 = map_assgn_op($2);
		  $$=newExpr(op1,NULL,$1,$3);
                }

	;
 
const_expr_no_commas:
	const_primary
           { 
             if (exception_flag == ON)  { Message("undefined symbol",0);
                                          exception_flag =OFF;
                                        }
             $$=$1 ;
	   }
	| unop const_expr_no_commas  %prec UNARY
		{ 
		  $$=newExpr($1,NULL,$2);
                }
	| SIZEOF const_expr_no_commas  %prec UNARY
		{ 
		  $$=newExpr(SIZE_OP,NULL,$2);
                }
	| const_expr_no_commas '+' const_expr_no_commas
		{ 
		  $$=newExpr(ADD_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '-' const_expr_no_commas
		{ 
		  $$=newExpr(SUBT_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '*' const_expr_no_commas
		{ 
		  $$=newExpr(MULT_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '/' const_expr_no_commas
		{ 
		  $$=newExpr(DIV_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '%' const_expr_no_commas
		{ 
		  $$=newExpr(MOD_OP,NULL,$1,$3);
                }
	| const_expr_no_commas LSHIFT const_expr_no_commas
		{ 
		  $$=newExpr(LSHIFT_OP,NULL,$1,$3);
                }
	| const_expr_no_commas RSHIFT const_expr_no_commas
		{ 
		  $$=newExpr(RSHIFT_OP,NULL,$1,$3);
                }
	| const_expr_no_commas ARITHCOMPARE const_expr_no_commas
		{ int op1 ;
                  op1 = ($2 == ((int) LE_EXPR)) ? LE_OP : GE_OP ;
		  $$=newExpr(op1,NULL,$1,$3);
                }
	| const_expr_no_commas '<'  const_expr_no_commas  
                { 
		  $$=newExpr(LT_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '>'  const_expr_no_commas   
                { 
		  $$=newExpr(GT_OP,NULL,$1,$3);
                }

	| const_expr_no_commas EQCOMPARE const_expr_no_commas
		{ int op1 ;

                  op1 = ($2 == ((int) NE_EXPR)) ? NE_OP : EQ_OP  ;
		  $$=newExpr(op1,NULL,$1,$3);
                }
	| const_expr_no_commas '&' const_expr_no_commas
		{ 
		  $$=newExpr(BITAND_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '|' const_expr_no_commas
		{ 
		  $$=newExpr(BITOR_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '^' const_expr_no_commas
		{ 
		  $$=newExpr(XOR_OP,NULL,$1,$3);
                }
	| const_expr_no_commas ANDAND const_expr_no_commas
		{ 
		  $$=newExpr(AND_OP,NULL,$1,$3);
                }
	| const_expr_no_commas OROR const_expr_no_commas
		{ 
		  $$=newExpr(OR_OP,NULL,$1,$3);
                }
	| const_expr_no_commas '?' expr ':' const_expr_no_commas
		{ PTR_LLND ll_node1;
		  ll_node1=newExpr(EXPR_IF_BODY,$2,$3);
		  $$=newExpr(EXPR_IF,NULL,$1,ll_node1);
		}
	| const_expr_no_commas '=' const_expr_no_commas
		{ 
		  $$=newExpr(ASSGN_OP,NULL,$1,$3);
                }
	| const_expr_no_commas ASSIGN const_expr_no_commas
		{ int op1 ;
                  op1 = map_assgn_op($2);
		  $$=newExpr(op1,NULL,$1,$3);
                }

	;


/* modified */
primary:
	IDENTIFIER
		{ PTR_SYMB symbptr;
		  symbptr = (PTR_SYMB) Look_For_Symbol_Ann (VARIABLE_NAME, $1,NULL);
		  $$ = newExpr(VAR_REF,global_int_annotation,symbptr);
		  exception_flag = ON ;
		}
	| CONSTANT
                { 
                  $$ = $1 ;
                }
	| string
		{ 
                  $$ = $1 ;    
                }
	| '(' expr ')'
		{ 
                  primary_flag = EXPR_LR ;
                  $$ = $2 ;
                }

	| '(' error ')'
           {
	     $$ = NULL;
	   }
        |   vector_constant       %prec '.'
           {
	     $$ = $1;
	   }
	| primary '('  
                {  PTR_SYMB symb;

                   if (exception_flag == ON)
		    {
                      /* strange behavior for default function */
		      symb = NODE_SYMB($1);
		      SYMB_CODE(symb) = FUNCTION_NAME;
                      exception_flag = OFF ;
                      $<ll_node>$ =  Make_Function_Call (symb,NULL,0,NULL);
                     }
                   else
                      $<ll_node>$ = $1 ;
		 }

          exprlist ')'   %prec '.'
		{ PTR_LLND lnode_ptr ,llp ;
                  int      status;

                  llp = $<ll_node>3 ;
                  status = OFF ;
                  if ((llp->variant == FUNC_CALL) && (!llp->entry.Template.ll_ptr1))
                      { 
                         lnode_ptr = llp;
                         status = FUNC_CALL ;
		       }
                  if ((!status) &&((llp->variant == RECORD_REF)||
				   (llp->variant == POINTST_OP)))
		    {
                       lnode_ptr = llp->entry.Template.ll_ptr2;
		       if ((lnode_ptr)&&(lnode_ptr->variant== FUNCTION_REF))
			 {
                           lnode_ptr->variant = FUNC_CALL;
			 }
                       status = FUNC_CALL ;
		     }
                  if ((!status) &&(llp->variant== FUNCTION_REF))
		    {  llp->variant = FUNC_CALL ;
                       status = FUNC_CALL ;
		       lnode_ptr = llp;
		     }
		  if (!status) {
                       status = FUNCTION_OP;
		       lnode_ptr = llp;
		     }
                  switch (status) {
                  case FUNCTION_OP : $$ =newExpr(FUNCTION_OP,$<ll_node>3,$4);
                                     $$->type = $<ll_node>3->type ;
                                     break;
                  case FUNC_CALL :   lnode_ptr->entry.Template.ll_ptr1=$4;
                                     $$ = $<ll_node>3 ;
                                     break;
	          default :        Message("system error 10",0);
		  }
		}

	| primary '[' expr_vector ']'   %prec '.'
	   { int status ;
             PTR_LLND ll_ptr,lp1;

             ll_ptr = check_array_id_format($1,&status);
             switch (status) {
             case NO : Message("syntax error ",0);
                       break ;
	     case ARRAY_OP_NEED:
                       lp1 = newExpr(EXPR_LIST,NULL,$3,LLNULL);/*mod*/
                       $$ = newExpr(ARRAY_OP,NULL,$1,lp1);
                       break;
             case ID_ONLY :
                       ll_ptr->variant = ARRAY_REF ;
                       ll_ptr->entry.Template.ll_ptr1 = newExpr(EXPR_LIST,NULL,$3,LLNULL);
                       $$ = $1 ;
                       break;
             case RANGE_APPEAR :
	               ll_ptr->entry.Template.ll_ptr2 = newExpr(EXPR_LIST,NULL,$3,LLNULL);
 	               $$ = $1 ; 
                       break;
             }
/*             $$->type = adjust_deref_type($1->type,DEREF_OP);*/
           }
	| primary PLUSPLUS
		{ 
                  $$ = newExpr(PLUSPLUS_OP,NULL,LLNULL,$1);
		  $$->type = $1->type ;
                }
	| primary MINUSMINUS
		{ 
                  $$ = newExpr(MINUSMINUS_OP,NULL,LLNULL,$1);
		  $$->type = $1->type ;
		}
	;




/* modified */
const_primary:

          CONSTANT
                { 
                  $$ = $1 ;    
                }
	| '(' const_expr_no_commas ')'
		{ 
                  primary_flag =EXPR_LR ;
                  $$ = $2 ;
                }

	| '(' error ')'
           {
	     $$ = NULL;
	   }          
	| const_primary PLUSPLUS
		{ 
                  $$ = newExpr(PLUSPLUS_OP,NULL,LLNULL,$1);
                }
	| const_primary MINUSMINUS
		{ 
                  $$ = newExpr(MINUSMINUS_OP,NULL,LLNULL,$1);
		}
	;

/* Produces a STRING_CST with perhaps more STRING_CSTs chained onto it.  */
string:
	  STRING
            { 
              $$ = $1 ;
            }
	| string STRING
	;

%%
int lineno;			/* current line number in file being read */

/* comments structure */
#define MAX_COMMENT_SIZE  1024 
char    comment_buf[MAX_COMMENT_SIZE + 2];  /* OFFSET '2' to avoid boundary */
int     comment_cursor = 0;
int     global_comment_type;


/*************************************************************************
 *                                                                       *
 *                      lexical analyzer                                 * 
 *                                                                       *
 *************************************************************************/

static int maxtoken;		/* Current length of token buffer */
static char *token_buffer;	/* Pointer to token buffer */
static int previous_value ;     /* last token to be remembered */

/* frw[i] is index in rw of the first word whose length is i. */

#define MAXRESERVED 9

/*static char frw[10] =
  { 0, 0, 0, 2, 6, 14, 22, 34, 39, 44 };*/
static char frw[10] =
{ 0, 0, 0, 2, 5, 13, 21, 32, 37, 41 };

static char *rw[] =
  { "if", "do", 
    "int", "for", "asm",
    "case", "char", "auto", "goto", "else", "long", "void", "enum",
    "float", "short", "union", "break", "while", "const",  "IfDef","Label",
    "double", "static", "extern", "struct", "return", "sizeof", "switch", "signed","coexec","coloop","friend",
    "typedef", "default","private","cobreak", "ApplyTo",
    "unsigned", "continue", "register", "volatile","operator"};

static short rtoken[] =
  { IF, DO, 
    TYPESPEC, FOR, ASM,
    CASE, TYPESPEC, SCSPEC, GOTO, ELSE, TYPEMOD, TYPESPEC, ENUM,
    TYPESPEC, TYPEMOD, UNION, BREAK, WHILE, TYPEMOD,  IFDEFA, ALABELT,
    TYPESPEC, SCSPEC, SCSPEC, STRUCT, RETURN, SIZEOF, SWITCH, TYPEMOD,COEXEC,COLOOP,FRIEND,
    SCSPEC, DEFAULT_TOKEN,ACCESSWORD,COBREAK, APPLYTO,
    TYPEMOD, CONTINUE, SCSPEC, TYPEMOD,OPERATOR};

/* This table corresponds to rw and rtoken.
   Its element is an index in ridpointers  */

#define NORID RID_UNUSED

static enum rid rid[] =
  { NORID, NORID, 
    RID_INT, NORID, NORID,
    NORID, RID_CHAR, RID_AUTO, NORID, NORID, RID_LONG, RID_VOID, NORID,
    RID_FLOAT, RID_SHORT, NORID, NORID, NORID, RID_CONST, NORID, NORID,
    RID_DOUBLE, RID_STATIC, RID_EXTERN, NORID, NORID, NORID, NORID, RID_SIGNED,NORID,NORID,NORID,
    RID_TYPEDEF, NORID,RID_PRIVATE,NORID, NORID,
    RID_UNSIGNED, NORID, RID_REGISTER, RID_VOLATILE,NORID};

/* The elements of `ridpointers' are identifier nodes
   for the reserved type names and storage classes.  
tree ridpointers[(int) RID_MAX];
static tree line_identifier;    The identifier node named "line" */


void
init_lex ()
{
  //extern char *malloc();

  /* Start it at 0, because check_newline is called at the very beginning
     and will increment it to 1.  */
  lineno = 0;
  maxtoken = 40;
  lastdecl_id = 0;
  token_buffer = (char *) xmalloc((unsigned)(maxtoken+1));
}

static void
reinit_parse_for_function ()
{
}

/* Put char into comment buffer. When the buffer is full, we make a comment */
/* structure and reset the comment_cursor. */
static int
put_char_buffer(c,sw)
char c ;
int sw;
{
/* no comment here */
return 0;
}

static int
skip_white_space(type)
  int type ;
{
  register int c;


  c = MYGETC();

  for (;;)
    {
      switch (c)
	{
	case '/':
	   return '/';	

	case '\n':
	case ' ':
	case '\t':
	case '\f':
	case '\r':
	case '\b':
	  c = MYGETC();
	  break;

	case '\\':
	  c = MYGETC();
	  if (c == '\n')
	    lineno++;
	  else
	    yyerror("stray '\\' in program");
	  c = MYGETC();
	  break;

	default:
	  return (c);
	}
    }
}

/* Take care of the comments in the tail of the source code */
static int
skip_white_space_2()
{
  register int c;

  c = MYGETC();
  for (;;)
    {
      switch (c)
	{
	case '/':
	  return '/';
	case '\n':
           return(c);

	case ' ':
	case '\t':
	case '\f':
	case '\r':
	case '\b':
	  c = MYGETC();
	  break;

	case '\\':
	  c = MYGETC();
	  if (c == '\n')
	    lineno++;
	  else
	    yyerror("stray '\\' in program");
	  c = MYGETC();
	  break;

	default:
	  return (c);
	}
    }
}



/* make the token buffer longer, preserving the data in it.
p should point to just beyond the last valid character in the old buffer
and the value points to the corresponding place in the new one.  */

static char *
extend_token_buffer(p)
char *p;
{
  register char *newbuf;
  register char *value;
  int newlength = maxtoken * 2 + 10;
  register char *p2, *p1;
  extern char *malloc();

  newbuf = malloc((unsigned)(newlength+1));

  p2 = newbuf;
  p1 = newbuf + newlength + 1;
  while (p1 != p2) *p2++ = 0;

  value = newbuf;
  p2 = token_buffer;
  while (p2 != p)
   *value++ = *p2++;

  token_buffer = newbuf;

  maxtoken = newlength;

  return (value);
}




#define isalnum(char) ((char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || (char >= '0' && char <= '9'))
#define isdigit(char) (char >= '0' && char <= '9')
#define ENDFILE -1  /* token that represents end-of-file */
#define isanop(d) ((d == '+') || (d == '-') || (d == '&') || (d == '|') || (d == '<') || (d == '>') || (d == '*') || (d == '/') || (d == '%') || (d == '^') || (d == '!') || (d == '=') )


int
readescape ()
{
  register int c = MYGETC ();
  register int count, code;

  switch (c)
    {
    case 'x':
      code = 0;
      count = 0;
      while (1)
	{
	  c = MYGETC ();
	  if (!(c >= 'a' && c <= 'f')
	      && !(c >= 'A' && c <= 'F')
	      && !(c >= '0' && c <= '9'))
	    {
	      unMYGETC (c);
	      break;
	    }
	  if (c >= 'a' && c <= 'z')
	    c -= 'a' - 'A';
	  code *= 16;
	  if (c >= 'a' && c <= 'f')
	    code += c - 'a' + 10;
	  if (c >= 'A' && c <= 'F')
	    code += c - 'A' + 10;
	  if (c >= '0' && c <= '9')
	    code += c - '0';
	  count++;
	  if (count == 3)
	    break;
	}
      if (count == 0)
	yyerror ("\\x used with no following hex digits");
      return code;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':
      code = 0;
      count = 0;
      while ((c <= '7') && (c >= '0') && (count++ < 3))
	{
	  code = (code * 8) + (c - '0');
	  c = MYGETC ();
	}
      unMYGETC (c);
      return code;

    case '\\': case '\'': case '"':
      return c;

    case '\n':
      lineno++;
      return -1;

    case 'n':
      return c ;  /*     return TARGET_NEWLINE; */

    case 't':
      return c;  /*      return TARGET_TAB; */

    case 'r':
      return c;/*      return TARGET_CR; */

    case 'f':
      return c;/*       return TARGET_FF;*/

    case 'b':
      return c;/*       return TARGET_BS;*/

    case 'a':
      return c; /*      return TARGET_BELL;*/

    case 'v':
      return c; /*      return TARGET_VT;*/
    }
  return c;
}

 
int
yylex()
{
  register int c;
  register char *p;
  register int value;
  int low /*,high */ ;
  char *str1 ;
/*  double  ddval ; */
/*  int type; */
  int c3;



  if (recursive_yylex == OFF) new_cur_comment = (PTR_CMNT) NULL ;

  /* line_pos_1 = lineno +1 ; */
  c = skip_white_space(FULL);
  /*  yylloc.first_line = lineno;*/

  switch (c)
    {
    case EOF:
      value = ENDFILE; break;

    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':
    case 'F':  case 'G':  case 'H':  case 'I':  case 'J':
    case 'K':  case 'L':  case 'M':  case 'N':  case 'O':
    case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':
    case 'U':  case 'V':  case 'W':  case 'X':  case 'Y':
    case 'Z':
    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
    case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
    case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
    case 'p':  case 'q':  case 'r':  case 's':  case 't':
    case 'u':  case 'v':  case 'w':  case 'x':  case 'y':
    case 'z':
    case '_':

      p = token_buffer;
      while (isalnum(c) || (c == '_') || (c == '~'))
	{
	  if (p >= token_buffer + maxtoken)
	    p = extend_token_buffer(p);
	  *p++ = c;
	  c = MYGETC();
	}

      *p = 0;
      unMYGETC(c);

      value = IDENTIFIER;


      if (p - token_buffer <= MAXRESERVED)
	{
	  register int lim = frw [p - token_buffer + 1];
	  register int i;

	  for (i = frw[p - token_buffer]; i < lim; i++)
	    if (rw[i][0] == token_buffer[0] && !strcmp(rw[i], token_buffer))
	      {
		if (rid[i])
		  yylval.token = (int) rid[i] ;
                  value = (int) rtoken[i];
		break;
	      }
	}

      { int temp;
	if ((temp = Recog_My_Token(token_buffer)) != -1)
	  {
	    yylval.token = temp;
	    value = temp;
	  }
      }

      if (value == IDENTIFIER)
	{ int t_status ;
	  PTR_LLND temp;
	/* temp move it out */

          yylval.hash_entry = look_up_type(token_buffer,&t_status);
          /* if ((t_status)&&(lastdecl_id ==0))   value = TYPENAME;    
	     Wait to fix that */
	  /* temporary fix */
           temp = look_up_section(token_buffer);
          if (temp)
	    {
	      yylval.ll_node = temp;
	      value = SECTIONT;
	    }

	  if (look_up_specialfunction(token_buffer))
	    {
	      value =  SPECIALAF;
	    }
	      
	  
        }

      break;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
    case '.':
      {
	int base = 10;
	int count = 0;
	int largest_digit = 0;
	/* for multi-precision arithmetic,
	   we store only 8 live bits in each short,
	   giving us 64 bits of reliable precision */
	short shorts[8];
	int floatflag = 0;  /* Set 1 if we learn this is a floating constant */

	for (count = 0; count < 8; count++)
	  shorts[count] = 0;

	p = token_buffer;
	*p++ = c;

	if (c == '0')
	  {
	    *p++ = (c = MYGETC());
	    if ((c == 'x') || (c == 'X'))
	      {
		base = 16;
		*p++ = (c = MYGETC());
	      }
	    else
	      {
		base = 8;
	      }
	  }

	while (c == '.'
	       || (isalnum (c) && (c != 'l') && (c != 'L')
		   && (c != 'u') && (c != 'U')
		   && (!floatflag || ((c != 'f') && (c != 'F')))))
	  {
	    if (c == '.')
	      {
		if (base == 16)
		  yyerror ("floating constant may not be in radix 16");
		floatflag = 1;
		base = 10;
		*p++ = c = MYGETC ();
		/* Accept '.' as the start of a floating-point number
		   only when it is followed by a digit.
		   Otherwise, unread the following non-digit
		   and use the '.' as a structural token.  */
		if (p == token_buffer + 2 && !isdigit (c))
		  {
		    if (c == '.')
		      {
			c = MYGETC ();
			if (c == '.')
			  { 
                            value = ELLIPSIS ;
			    goto done ;
                          }
			yyerror ("syntax error");
		      }
		    unMYGETC (c);
		    value = '.';
                    goto done;
		  }
	      }
	    else
	      {
		if (isdigit(c))
		  {
		    c = c - '0';
		  }
		else if (base <= 10)
		  {
		    if ((c&~040) == 'E')
		      {
			if (base == 8)
			  yyerror ("floating constant may not be in radix 8");
			base = 10;
			floatflag = 1;
			break;   /* start of exponent */
		      }
		    yyerror ("nondigits in number and not hexadecimal");
		    c = 0;
		  }
		else if (c >= 'a')
		  {
		    c = c - 'a' + 10;
		  }
		else
		  {
		    c = c - 'A' + 10;
		  }
		if (c >= largest_digit)
		  largest_digit = c;
	    
		for (count = 0; count < 8; count++)
		  {
		    (shorts[count] *= base);
		    if (count)
		      {
			shorts[count] += (shorts[count-1] >> 8);
			shorts[count-1] &= (1<<8)-1;
		      }
		    else shorts[0] += c;
		  }
    
		*p++ = (c = MYGETC());
	      }
	  }

	if (largest_digit >= base)
	  yyerror ("numeric constant contains digits beyond the radix");

	/* Remove terminating char from the token buffer and delimit the string */
	*--p = 0;

	if (floatflag)
	  {
	   /*  enum rid type = DOUBLE_TYPE_CONST ; */

	    /* Read explicit exponent if any, and put it in tokenbuf.  */

	    if ((c == 'e') || (c == 'E'))
	      {
		*p++ = c;
		c = MYGETC();
		if ((c == '+') || (c == '-'))
		  {
		    *p++ = c;
		    c = MYGETC();
		  }
	        while (isdigit(c))
		  {
		    *p++ = c;
		    c = MYGETC();
		  }
	      }

	    *p = 0;

	    while (1)
	      {
/*		if (c == 'f' || c == 'F')
		  type = FLOAT_TYPE_CONST ;
		else if (c == 'l' || c == 'L')
		  type = LONG_DOUBLE_TYPE_CONST ;
		else */

                if((c != 'f') && (c != 'F') && (c != 'l') && (c !='L'))
		  {
		    if (isalnum (c))
		      {
			yyerror ("garbage at end of number");
			while (isalnum (c))
			  c = MYGETC ();
		      }
		    break;
		  }
		c = MYGETC ();
	      }

	    unMYGETC(c);

/*	    ddval = build_real_from_string (token_buffer, 0);  */
            str1= (char *) copys(token_buffer);
            yylval.ll_node = newExpr(FLOAT_VAL,NULL,LLNULL,LLNULL,str1);

	  }
	else
	  {
	    /* enum rid  type; */

	    /* int spec_unsigned = 0; */
	    /* int spec_long = 0;  */

	    while (1)
	      {
/*		if (c == 'u' || c == 'U')
		  {
		    spec_unsigned = 1;
		  }
		else if (c == 'l' || c == 'L')
		  {
		    spec_long = 1;
		  }
		else */

               if((c != 'u') && (c != 'U') && (c != 'l') && (c != 'L'))
		  {
		    if (isalnum (c))
		      {
			yyerror ("garbage at end of number");
			while (isalnum (c))
			  c = MYGETC ();
		      }
		    break;
		  }
		c = MYGETC ();
	      }

	    unMYGETC (c);

	    /* This is simplified by the fact that our constant
	       is always positive.  */

	    low= (shorts[3]<<24) + (shorts[2]<<16) + (shorts[1]<<8) + shorts[0] ;
	  /*  high = (shorts[7]<<24) + (shorts[6]<<16) + (shorts[5]<<8) + shorts[4] ; */
	    

	    /* type = LONG_UNSIGNED_TYPE_CONST ; */
	    yylval.ll_node = makeInt(low);	
	 }

	value = CONSTANT; break;
      }

    case '\'':
      c = MYGETC();
      {

      tryagain:

	if (c == '\\')
	  {
	    c = readescape ();
	    if (c < 0)
	      goto tryagain;
	  }
	else if (c == '\n')
	  {
	      Message ("ANSI C forbids newline in character constant",0);
	    lineno++;
	  }

	c3= c;

	c = MYGETC ();
	if (c != '\'')
	  yyerror("malformatted character constant");
        yylval.ll_node = newExpr(CHAR_VAL,LLNULL,LLNULL,low);
	yylval.ll_node->entry.cval = c3;  
	value = CONSTANT; break;
      }

    case '"':
      {
	c = MYGETC();
	p = token_buffer;

	while (c != '"')
	  {
	    if (c == '\\')
	      {
                /* New Added Three lines */
                if (p == token_buffer + maxtoken)
	          p = extend_token_buffer(p);
  	        *p++ = c;

		c = readescape ();
		if (c < 0)
		  goto skipnewline;
	      }
	    else if (c == '\n')
	      {
		  Message ("ANSI C forbids newline in string constant",0);
		lineno++;
	      }

	    if (p == token_buffer + maxtoken)
	      p = extend_token_buffer(p);
	    *p++ = c;

	  skipnewline:
	    c = MYGETC ();
	  }

	*p++ = 0;

	str1= (char *) copys(token_buffer);
        yylval.ll_node = (PTR_LLND) newNode(STRING_VAL);
	NODE_STRING_POINTER(yylval.ll_node) = str1;
	value = STRING; break;
      }
      
    case '+':
    case '-':
    case '&':
    case '|':
    case '<':
    case '>':
    case '*':
    case '/':
    case '%':
    case '^':
    case '!':
    case '=':
      {
	register int c1;
        if ( previous_value == OPERATOR )
           {
             p = token_buffer;
             while (isanop(c) )
              {
                  if (p >= token_buffer + maxtoken)
	          p = extend_token_buffer(p);
	          *p++ = c;
	          c = MYGETC();
               }
             *p = 0;
             unMYGETC(c);
             value =  LOADEDOPR ;
             yylval.hash_entry = look_up(token_buffer);
             break;
            }
      combine:

	switch (c)
	  {
	  case '+':
	    yylval.token = (int) PLUS_EXPR; break;
	  case '-':
	    yylval.token = (int) MINUS_EXPR; break;
	  case '&':
	    yylval.token = (int) BIT_AND_EXPR; break;
	  case '|':
	    yylval.token = (int) BIT_IOR_EXPR; break;
	  case '*':
	    yylval.token = (int) MULT_EXPR; break;
	  case '/':
	    yylval.token = (int) TRUNC_DIV_EXPR; break;
	  case '%':
	    yylval.token = (int) TRUNC_MOD_EXPR; break;
	  case '^':
	    yylval.token = (int) BIT_XOR_EXPR; break;
	  case LSHIFT:
	    yylval.token = (int) LSHIFT_EXPR; break;
	  case RSHIFT:
	    yylval.token = (int) RSHIFT_EXPR; break;
	  case '<':
	    yylval.token = (int) LT_EXPR; break;
	  case '>':
	    yylval.token = (int) GT_EXPR; break;
	  }	

	c1 = MYGETC();

	if (c1 == '=')
	  {
	    switch (c)
	      {
	      case '<':
		value = ARITHCOMPARE; yylval.token = (int) LE_EXPR; goto done;
	      case '>':
		value = ARITHCOMPARE; yylval.token = (int) GE_EXPR; goto done;
	      case '!':
		value = EQCOMPARE; yylval.token = (int) NE_EXPR; goto done;
	      case '=':
		value = EQCOMPARE; yylval.token = (int) EQ_EXPR; goto done;
	      }	
	    value = ASSIGN; goto done;
	  }
	else if (c == c1)
	  switch (c)
	    {
	    case '+':
	      value = PLUSPLUS; goto done;
	    case '-':
	      value = MINUSMINUS; goto done;
	    case '&':
	      value = ANDAND; goto done;
	    case '|':
	      value = OROR; goto done;
/* testing  */
/*            case ':':
              value = DOUBLEMARK; goto done;  */

	    case '<':
	      c = LSHIFT;
	      goto combine;
	    case '>':
	      c = RSHIFT;
	      goto combine;
	    }
	else if ((c == '-') && (c1 == '>'))
	  { value = POINTSAT; goto done; }
	unMYGETC (c1);


        value = c;
	goto done;
      }

    default:
      value = c;
    }

done:

  if (recursive_yylex == OFF) {
    previous_value = value ;
    line_pos_1 = lineno ;
    c = skip_white_space_2();
    if (c != '\n');
       unMYGETC(c);
    if (value != '}') 
      { c = skip_white_space(NEXT_FULL);
	if (c == '\n') lineno++ ;
	else           unMYGETC(c);
      }
    set_up_momentum(value,yylval.token);
    automata_driver(value);
    cur_counter++;
    old_line = yylineno  ;
    yylineno = line_pos_1;
  }

  if (TRACEON) printf("yylex returned %d\n", value);
  return (value);
}
 

static int yyerror(s)
        char   *s;
{
  /* Message(s,0); empty at the moment, generate false error report?
     to be modified later */
  return 1;  /* PHB needed a return val, 1 seems ok */
}


/*  primary :- primary [ expr_vector ]
 *  <1> check the LHS format
 *  <2> return : NO if incorrect format at LHS
 *               ID_ONLY if LHS only have id format (including multiple id)
 *               RANGE_APPEAR if LHS format owns both id and range_list
 */

static
PTR_LLND check_array_id_format(ll_ptr,state)
int *state;
PTR_LLND ll_ptr ;

{   PTR_LLND temp,temp1;

      temp = ll_ptr;
      switch (NODE_CODE(ll_ptr)) {
      case VAR_REF :
                     *state = ID_ONLY ;
                     return(ll_ptr);
      case ARRAY_REF :
                     temp1 = Follow_Llnd(NODE_OPERAND0(ll_ptr),2);
                     *state = RANGE_APPEAR;
                     return(temp1);
      case  ARRAY_OP:temp1 = Follow_Llnd(NODE_OPERAND1(ll_ptr),2);
                     *state =RANGE_APPEAR ;
                     return(temp1);
        default :    *state = ARRAY_OP_NEED ;
                     return(temp);
      }
  }

static
int
map_assgn_op(value)
int value;
{
  switch (value) {
  case ((int) PLUS_EXPR) :
      return(PLUS_ASSGN_OP);
  case ((int) MINUS_EXPR):
      return(MINUS_ASSGN_OP);
  case ((int) BIT_AND_EXPR):
      return(AND_ASSGN_OP);
  case ((int) BIT_IOR_EXPR):
      return(IOR_ASSGN_OP);
  case ((int) MULT_EXPR):
      return(MULT_ASSGN_OP);
  case ((int) TRUNC_DIV_EXPR):
      return(DIV_ASSGN_OP);
  case ((int) TRUNC_MOD_EXPR):
      return(MOD_ASSGN_OP);
  case ((int) BIT_XOR_EXPR):
      return(XOR_ASSGN_OP);
  case ((int) LSHIFT_EXPR):
      return(LSHIFT_ASSGN_OP);
  case ((int) RSHIFT_EXPR):
      return(RSHIFT_ASSGN_OP);
  }
return 0;
}

PTR_HASH 
look_up_type(st, ip)
     char *st;
     int *ip;
{
  char *pt;
  
  pt =  (char *) xmalloc(strlen(st) +1);
  strcpy(pt,st);
 /* dummy, to be cleaned */
  return (PTR_HASH) pt;
}


PTR_HASH 
look_up(st)
     char *st;
{
  char *pt;
  
  pt =  (char *)  xmalloc(strlen(st) +1);
  strcpy(pt,st);
 /* dummy, to be cleaned */
  return (PTR_HASH)  pt;
}

static
MYGETC()
{
  
  if (LENSTRINGTOPARSE <= PTTOSTRINGTOPARSE)
	return EOF;

  if (STRINGTOPARSE[ PTTOSTRINGTOPARSE] == '\0')
    {
      PTTOSTRINGTOPARSE++;
      return EOF;
    }
  
  PTTOSTRINGTOPARSE++;
  return STRINGTOPARSE[ PTTOSTRINGTOPARSE-1];
}

static
unMYGETC(c)
char c;
{
  if (LENSTRINGTOPARSE <= PTTOSTRINGTOPARSE)
    return EOF;

  if (PTTOSTRINGTOPARSE >0)
    PTTOSTRINGTOPARSE --;
  STRINGTOPARSE[ PTTOSTRINGTOPARSE] = c;
  return c;
}


/* CurrentScope should be the last in the list */
static char *sectionkeyword[] =
  { "NextStmt",
    "NextAnnotation",      
    "EveryWhere",
    "Follow",
/* keep it last*/  "CurrentScope"};


static PTR_LLND
look_up_section(str)
 char *str;
{ int i;
  PTR_LLND pt = NULL;

  for (i = 0; i < RID_MAX; i++)
    {
      if (strcmp(sectionkeyword[i], str) == 0)
	{
	  pt  = (PTR_LLND) newNode(STRING_VAL);
	  NODE_STRING_POINTER(pt) = (char *) xmalloc(strlen(str) +1);
	  strcpy(NODE_STRING_POINTER(pt),str);
	  return pt;
	}
      if (strcmp(sectionkeyword[i],"CurrentScope") == 0)
	return NULL;
    }
  
  return NULL;
}


/* Dummy should be the last in the list */
static char *specialfunction[] =
  { "ListOfAn",
    "Align",
    "Induction",
    "Used",
    "Modified",
    "Alias",
    "Permutation",
    "Assert",
/* keep it last*/  "Dummy"};

static int
look_up_specialfunction(str)
 char *str;
{ int i;

  for (i = 0; i < RID_MAX; i++)
    {
      if (strcmp(specialfunction[i], str) == 0)
	{
	  return TRUE;
	}
      if (strcmp(specialfunction[i],"Dummy") == 0)
	return NULL;
    }
  
  return NULL;
}


static int 
Recog_My_Token(str)
char *str;
{

  if (strcmp("FromAnn",str) == 0)
    return FROMT;

  if (strcmp("ToAnn",str) == 0)
    return TOT;

   if (strcmp("ToLabel",str) == 0)
    return TOTLABEL;

  if (strcmp("ToFunction",str) == 0)
    return TOFUNCTION;

  if (strcmp("Define",str) == 0)
    return DefineANN;

  return -1;
}


PTR_SYMB
Look_For_Symbol_Ann(code,name,type)
     int code;
     char *name;
     PTR_TYPE type;
{
  PTR_SYMB symb;
  char temp1[256];

  strcpy(temp1, AnnExTensionNumber);
  strncat(temp1,name,255);

  if ((symb = getSymbolWithName(temp1, ANNOTATIONSCOPE)))
    return symb;

  if ((symb = getSymbolWithName(name, ANNOTATIONSCOPE)))
    return symb;

  return  newSymbol (code,name,type);
}
 

/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


/* TAG : pC++2dep used Created by Jenq_kuen Lee  Nov 28, 1987 */
/* definitions of Some Key_echo */
/* Define results of standard character escape sequences.  */
#define TARGET_BELL 007
#define TARGET_BS 010
#define TARGET_TAB 011
#define TARGET_NEWLINE 012
#define TARGET_VT 013
#define TARGET_FF 014
#define TARGET_CR 015


#define BITS_PER_UNIT  8
#define pedantic       1

/* Debugging flag */


/* switch used for parser */
#define UP_TO_CLASS     6
#define UP_ONE_LEVEL    5
#define UP_TO_NODECL    4
#define UP_TO_FUNC_HEDR 3
#define OTHER 2
#define ON 1
#define OFF 0

/* switch used for parser */
#define ONE 1
#define TWO 2
#define THREE 3

#define DONOT_CARE     0

#define TYPE_CLEAN 0
#define TYPE_ONE 1
#define TYPE_TWO 2
#define TYPE_THREE 3
#define TYPE_FOUR  4
#define TYPE_FIVE 5

#define BRANCH_OFF 0
#define BRANCH_ON  1

/* flag for declarator rule */
/* information kept in cur_flag */
#define  RULE_PARAM   1
#define  RULE_ID      2
#define  RULE_MULTIPLE_ID 4
#define  RULE_LR      8
#define  RULE_DEREF   16
#define  RULE_ARRAY   32
#define  RULE_ARRAY_E 64
#define  RULE_CLASSINIT  128
#define  RULE_ERROR      256
#define  LAZY_INSTALL    512
#define  CLEAN        0

/* flag for primary_flag */
#define ID_ONLY  1
#define RANGE_APPEAR 2
#define EXCEPTION_ON 4
#define EXPR_LR      8
#define VECTOR_CONST_APPEAR 16
#define ARRAY_OP_NEED 32

/* flag for access_class for parameter_flag  */
#define XDECL       4096

/* automata state for comments.c */
#define ZERO     0
#define STATE_1  1
#define STATE_2  2
#define STATE_3  3
#define STATE_4  4
#define STATE_5  5
#define STATE_6  6
#define STATE_7  7
#define STATE_8  8
#define STATE_9  9
#define STATE_10 10
#define STATE_11 11
#define STATE_12 12
#define STATE_13 13
#define STATE_14 14
#define STATE_15 15
#define STATE_16 16
#define STATE_17 17
#define STATE_18 18
#define STATE_19 19
#define STATE_20 20
#define IF_STATE            30
#define IF_STATE_2          32
#define IF_STATE_3          33
#define IF_STATE_4          34
#define ELSE_EXPECTED_STATE 35
#define BLOCK_STATE         40
#define BLOCK_STATE_2       42
#define WHILE_STATE         50
#define WHILE_STATE_2       52
#define FOR_STATE           55
#define FOR_STATE_2         56
#define CASE_STATE          57
#define COEXEC_STATE        58
#define COEXEC_STATE_2      59
#define COLOOP_STATE        60
#define COLOOP_STATE_2      61
#define DO_STATE            62
#define DO_STATE_1          63 
#define DO_STATE_2          64
#define DO_STATE_3          65
#define DO_STATE_4          66
#define DO_STATE_5          67
#define DO_STATE_6          68
#define RETURN_STATE        70
#define RETURN_STATE_2      71
#define RETURN_STATE_3      72
#define GOTO_STATE          75
#define GOTO_STATE_2        76
#define SWITCH_STATE        80
#define SWITCH_STATE_2      81
#define STATE_ARG           82
#define BLOCK_STATE_WAITSEMI 83
#define TEMPLATE_STATE      84
#define TEMPLATE_STATE_2    85
#define CONSTR_STATE	    86
/* for comments.c */
#define MAX_NESTED_SIZE 800



/* parameter for function body and struct declaration body  */
#define  NOT_SEEN  1    
#define  BEEN_SEEN 0
#define  FUNCTION_BODY_APPEAR 700

/* parameter for find_type_symbol  */
#define  TYPE_ONLY    1     /* TYPE_NAME */
#define  STRUCT_ONLY  2     
#define  VAR_ONLY     4
#define  FIELD_ONLY   8
#define FUNCTION_NAME_ONLY 16
#define MEMBER_FUNC_ONLY 32


/*flag for the error message of lazy_install */
/* No More symbol, Alliant C compiler's symbol table is full */
/* #define NOW 1   */
/* #define DELAY 2 */
/* For symbptr->attr  */
#define ATT_CLUSTER     0
#define ATT_GLOBAL      1
#define PURE            8
#define PRIVATE_FIELD   16
#define PROTECTED_FIELD 32
#define PUBLIC_FIELD    64
#define ELEMENT_FIELD   128
#define COLLECTION_FIELD 256
#define CONSTRUCTOR      512
#define DESTRUCTOR       1024
#define PCPLUSPLUS_DOSUBSET 2048
#define INVALID             4096
#define SUBCOLLECTION       4096*2
/* #define OVOPERATOR	4096*4  (defined in macro.h) (phb) */
#define VIRTUAL_DESTRUCTOR 4096*8 /* added by BW */

/* For find_type_symbol() */
/* for check_field_decl_3 */
#define ALL_FIELDS    1
#define CLASS_ONLY    2
#define COLLECTION_ONLY 3
#define ELEMENT_ONLY    4
#define FUNCTION_ONLY 5

/* for collection nested dimension */
#define MAX_NESTED_DIM 5

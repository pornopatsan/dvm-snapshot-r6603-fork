/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


extern char *Unparse_Annotation();
extern PTR_LLND Parse_Annotation();
extern Is_Annotation();
extern Is_Annotation_Cont();
extern char * Get_Annotation_String();
extern char * Get_to_Next_Annotation_String();
extern Init_Annotation();
extern PTR_LLND Get_Define_Field();
extern char * Get_Define_Label_Field();
extern char * Get_Label_Field();
extern PTR_LLND Get_ApplyTo_Field();
extern PTR_LLND Get_ApplyToIf_Field();
extern PTR_LLND Get_LocalVar_Field();
extern PTR_LLND Get_Annotation_Field();
extern char * Get_Annotation_Field_Label();
extern char * Does_Annotation_Defines();
extern int Set_The_Define_Field();
extern int Get_Annotation_With_Label();
extern Get_Scope_Of_Annotation();
extern Propagate_defined_value();
extern PTR_LLND Does_Annotation_Apply();
extern PTR_LLND Get_Annotation_Field_List_For_Stmt();
extern PTR_LLND Get_Annotation_List_For_Stmt();
extern Get_Number_of_Annotation();
extern PTR_BFND Get_Annotation_Bif();
extern PTR_LLND Get_Annotation_Expr();
extern char * Get_String_of_Annotation();
extern PTR_CMNT Get_Annotation_Comment();
extern int Is_Annotation_Defined();
extern char * Annotation_Defines_string();
extern int Annotation_Defines_string_Value();
extern PTR_LLND Annotation_LLND[];
extern PTR_TYPE  global_int_annotation;


















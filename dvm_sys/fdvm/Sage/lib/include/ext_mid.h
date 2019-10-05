/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/


extern PTR_BFND Make_For_Loop ();
extern PTR_LLND Loop_Set_Borne_Inf ();
extern PTR_LLND Loop_Set_Borne_Sup ();
extern PTR_LLND Loop_Set_Step ();
extern PTR_SYMB Loop_Set_Index ();
extern PTR_LLND Loop_Get_Borne_Inf ();
extern PTR_LLND Loop_Get_Borne_Sup ();
extern PTR_LLND Loop_Get_Step ();
extern PTR_SYMB Loop_Get_Index ();

extern PTR_BFND Get_Scope_For_Declare ();
extern PTR_BFND Get_Scope_For_Label ();

extern PTR_LLND Make_Array_Ref ();
extern PTR_LLND Make_Array_Ref_With_Tab ();
extern PTR_BFND Declare_Array ();

extern PTR_BFND Make_Procedure ();
extern PTR_LLND Make_Function_Call ();
extern PTR_LLND Make_Function_Call_bis ();
extern PTR_BFND Make_Procedure_Call  ();

extern PTR_LLND Make_Linear_Expression ();
extern PTR_LLND Make_Linear_Expression_From_Int ();
extern PTR_LLND Make_Linear_Expression_From_Int_List ();

extern PTR_BFND Make_Assign ();
extern PTR_BFND Make_If_Then_Else ();
extern int Declare_Scalar ();
extern int Perfectly_Nested ();
extern int Is_Good_Loop ();

extern PTR_BFND Extract_Loop_Body ();
extern PTR_BFND Get_Next_Nested_Loop ();
extern PTR_BFND Get_Internal_Loop ();
extern PTR_BFND Get_Previous_Nested_Loop ();

extern PTR_BLOB Get_Label_UD_chain ();

extern int Convert_Loop ();
extern int Loop_Conversion ();

extern PTR_SYMB Generate_Variable_Name ();
extern PTR_SYMB Install_Variable ();

extern int Verif_No_Func ();
extern int Verif_Assign ();
extern int Verif_Assign_If ();

extern int Generate_Alternative_Code ();
extern void Localize_Array_Section ();

extern int Check_Index ();
extern int Check_Right_Assign ();
extern int Check_Left_Assign ();
extern int No_Dependent_Index ();
extern int No_Basic_Induction ();
extern int No_Def_Of_Induction ();

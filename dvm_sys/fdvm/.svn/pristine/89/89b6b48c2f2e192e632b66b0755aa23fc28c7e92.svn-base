
/**************************************************************\
* Fortran DVM                                                  * 
*                                                              *
*              Generating LibDVM Function Calls               *
\**************************************************************/

#include "dvm.h"


/**************************************************************\
*       Run_Time Library initialization and completion        *
\**************************************************************/
void RTLInit ()
{    
//generating assign statement
// dvm000(1) = linit(InitParam)
// (standart initialization : InitParam = 0)
// and inserting it before first executable statemen
    SgFunctionCallExp *fe   = new SgFunctionCallExp(*fdvm[RTLINI]);
    fmask[RTLINI] = 1;
    if(deb_mpi)
      fe->addArg(*ConstRef(2));
    else
      fe->addArg(*ConstRef(0)); 
    doAssignStmt(fe);
    //ndvm--;         // the result of RTLIni isn't used
    return;
}

void RTLExit (SgStatement *st  )

{ 
//generating CALL statement to close all opened files: clfdvm()
//and inserting it before statement 'st'
   LINE_NUMBER_BEFORE(st,st);
   InsertNewStatementBefore(CloseFiles(),st);
//generating call statement
// call dvmh_finish()     
   //if(ACC_program)      /*ACC*/
   InsertNewStatementBefore(RTL_GPU_Finish(),st);
//generating assign statement
// dvm000(i) = lexit(UsersRes)
// UsersRes - result of ending user's program
// !!! temporary :     0 
// and inserting it before statement 'st'
    SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RTLEXI]);
    fmask[RTLEXI] = 1;
    fe->addArg(*ConstRef(0));
    doAssignStmtBefore(fe,st); 
    return;
}
/**************************************************************\
*       Checking Fortran and C data type compatibility        *
\**************************************************************/
void TypeControl()
{  int n ;
  SgCallStmt *call = new SgCallStmt(*fdvm[TPCNTR]);
   /*SgFunctionCallExp *fe =  new SgFunctionCallExp(*fdvm[TPCNTR]);*/
   fmask[TPCNTR] = 2;
   n = (bind == 1 ) ? 6 : 5;
//generating assign statement for arguments of 'tpcntr' function
    doAssignStmt(ConstRef(n)); //Number of types
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Imem,*new SgValueExp(0))));
    TypeMemory(SgTypeInt());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Lmem,*new SgValueExp(0))));
    TypeMemory(SgTypeBool());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Rmem,*new SgValueExp(0))));
    TypeMemory(SgTypeFloat());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Dmem,*new SgValueExp(0))));
    TypeMemory(SgTypeDouble());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Chmem,*new SgValueExp(0))));
    TypeMemory(SgTypeChar());
    if(bind == 1)
       doAssignStmt(GetAddresMem( new SgArrayRefExp(*dvmbuf,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Imem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Lmem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Rmem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Dmem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Chmem,*new SgValueExp(1))));
    if(bind == 1)
       doAssignStmt(GetAddresMem( new SgArrayRefExp(*dvmbuf,*new SgValueExp(2))));
    doAssignStmt(ConstRef(TypeSize(SgTypeInt())));
    doAssignStmt(ConstRef(TypeSize(SgTypeBool())));
    doAssignStmt(ConstRef(TypeSize(SgTypeFloat())));
    doAssignStmt(ConstRef(TypeSize(SgTypeDouble())));
    doAssignStmt(ConstRef(TypeSize(SgTypeChar())));
    if(bind == 1)
       doAssignStmt(ConstRef( DVMTypeLength()));
    doAssignStmt(ConstRef(VarType_RTS(Imem)));
    doAssignStmt(ConstRef(VarType_RTS(Lmem)));
    doAssignStmt(ConstRef(VarType_RTS(Rmem)));
    doAssignStmt(ConstRef(VarType_RTS(Dmem)));
    doAssignStmt(ConstRef(5));
    if(bind == 1)
       doAssignStmt(ConstRef( DVMType()));
//generating assign statement
// and inserting it before first executable statement
// dvm000(i) = tpcntr(Number,FirstAddr[],NextAddr[],Len[],Type[])
    call -> addArg(*DVM000(1));  
    call -> addArg(*DVM000(2)); 
    call -> addArg(*DVM000(2+n)); 
    call -> addArg(*DVM000(2+2*n));  
    call -> addArg(*DVM000(2+3*n)); 
    where->insertStmtBefore(*call,*where->controlParent());
                                   //inserting 'call' statement before 'where'  statement 
    cur_st = call;     
    /*doAssignStmt(fe);*/
    SET_DVM(1);
    return;
}

void TypeControl_New()
{  int n, k ;
                  /*  SgFunctionCallExp *fe =  new SgFunctionCallExp(*fdvm[TPCNTR]);*/ /*18.02.03*/
   SgCallStmt *call = new SgCallStmt(*fdvm[FTCNTR]);
      fmask[FTCNTR] = 2;
   n = (bind == 1 ) ? 6 : 5;
//generating assign statement for arguments of 'ftcntr' function
    doAssignStmt(ConstRef(n)); //Number of types
    if(bind == 1)
      doAssignStmt(GetAddresMem( new SgArrayRefExp(*dvmbuf,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Imem,*new SgValueExp(0))));
    TypeMemory(SgTypeInt());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Lmem,*new SgValueExp(0))));
    TypeMemory(SgTypeBool());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Rmem,*new SgValueExp(0))));
    TypeMemory(SgTypeFloat());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Dmem,*new SgValueExp(0))));
    TypeMemory(SgTypeDouble());
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Chmem,*new SgValueExp(0))));
    TypeMemory(SgTypeChar());
    /*if(bind == 1)
      doAssignStmt(GetAddresMem( new SgArrayRefExp(*dvmbuf,*new SgValueExp(1))));*/
    if(bind == 1)
       doAssignStmt(GetAddresMem( new SgArrayRefExp(*dvmbuf,*new SgValueExp(2))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Imem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Lmem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Rmem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Dmem,*new SgValueExp(1))));
    doAssignStmt(GetAddresMem( new SgArrayRefExp(*Chmem,*new SgValueExp(1))));
    /*if(bind == 1)
      doAssignStmt(GetAddresMem( new SgArrayRefExp(*dvmbuf,*new SgValueExp(2))));*/
    if(bind == 1)
      doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(0)),new SgValueExp(DVMTypeLength())); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(1)),new SgValueExp(TypeSize(SgTypeInt()))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(2)),new SgValueExp(TypeSize(SgTypeBool()))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(3)),new SgValueExp(TypeSize(SgTypeFloat()))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(4)),new SgValueExp(TypeSize(SgTypeDouble()))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(5)),new SgValueExp(TypeSize(SgTypeChar()))); 
//    doAssignStmt(ConstRef(TypeSize(SgTypeInt())));
//    doAssignStmt(ConstRef(TypeSize(SgTypeBool())));
//    doAssignStmt(ConstRef(TypeSize(SgTypeFloat())));
//    doAssignStmt(ConstRef(TypeSize(SgTypeDouble())));
//    doAssignStmt(ConstRef(TypeSize(SgTypeChar())));
    /*if(bind == 1)
        doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(6)),new SgValueExp(DVMTypeLength()));*/ 
//       doAssignStmt(ConstRef( DVMTypeLength()));
    if(bind == 1)
      doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(10)),new SgValueExp(DVMType())); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(11)),new SgValueExp(VarType_RTS(Imem))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(12)),new SgValueExp(VarType_RTS(Lmem))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(13)),new SgValueExp(VarType_RTS(Rmem))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(14)),new SgValueExp(VarType_RTS(Dmem))); 
    doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(15)),new SgValueExp(5)); 

//    doAssignStmt(ConstRef(VarType(Imem)));
//    doAssignStmt(ConstRef(VarType(Lmem)));
//    doAssignStmt(ConstRef(VarType(Rmem)));
//    doAssignStmt(ConstRef(VarType(Dmem)));
//    doAssignStmt(ConstRef(5));
    /* if(bind == 1)
       doAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(16)),new SgValueExp(DVMType())); */

//       doAssignStmt(ConstRef( DVMType()));
//generating assign statement
// and inserting it before first executable statement
// dvm000(i) = tpcntr(Number,FirstAddr[],NextAddr[],Len[],Type[])
    //fe -> addArg(*new SgValueExp(n)); //(*DVM000(1));  
    //fe -> addArg(*DVM000(2)); 
    //fe -> addArg(*DVM000(2+n)); 
    //fe -> addArg(*DVM000(2+2*n));  
    //fe -> addArg(*DVM000(2+3*n));    
    //doAssignStmt(fe);
    k = (bind == 1 ) ? 0 : 1;
    call -> addArg(*new SgValueExp(n)); //(*DVM000(1));  
    call -> addArg(*DVM000(2)); 
    call -> addArg(*DVM000(2+n)); 
    call -> addArg(*new SgArrayRefExp(*Imem,*new SgValueExp(k)));  
    call -> addArg(*new SgArrayRefExp(*Imem,*new SgValueExp(k+10)));
//    call -> addArg(*DVM000(2+2*n));  
//    call -> addArg(*DVM000(2+3*n));
    where->insertStmtBefore(*call,*where->controlParent());
                                   //inserting 'call' statement before 'where'  statement 
    cur_st = call;       
    SET_DVM(1);
    return;
}
/**************************************************************\
*       Requesting processor system                           *
\**************************************************************/
void GetVM ()
{ 
    SgFunctionCallExp *fe =  new SgFunctionCallExp(*fdvm[GETVM]);
    fmask[GETVM] = 1;
//generating assign statement
// and inserting it before first executable statement
// dvm000(3) = getps(AMRef)
    fe -> addArg(*DVM000(2));  // dvm000(2) - AMReference
    doAssignStmt(fe);
    return;
    /*
// generating assign statement
// and inserting it before first executable statement
// dvm000(3) = 0 //PSRef == 0 means current processor system
    doAssignStmt(new SgValueExp(0));
    return;
    */
}

SgExpression * GetProcSys (SgExpression * amref)
{ 
    SgFunctionCallExp *fe =  new SgFunctionCallExp(*fdvm[GETVM]);
    fmask[GETVM] = 1;
//generating function call: getps(AMRef)
    fe -> addArg(*amref);  // AMReference
    return(fe);
}


SgExpression *Reconf(SgExpression *size_array, int rank, int sign)
{
  SgFunctionCallExp *fe;
  //  SgValueExp dPS(3);
  
// generating function call:
//                          psview(PSRef, rank, SizeArray, StaticSign) 
  fe = new SgFunctionCallExp(*fdvm[PSVIEW]);
  fmask[PSVIEW] = 1;
  fe->addArg(*CurrentPS()); //DVM000(3);//dvm000(3) - current processor system reference
  fe -> addArg(*ConstRef(rank));// Rank
  fe -> addArg(*size_array);  // SizeArray
  fe -> addArg(*ConstRef(sign)); // StaticSign
  return(fe);
}

SgExpression *CrtPS(SgExpression *psref, int ii, int il, int sign)
{
  SgFunctionCallExp *fe;
  
// generating function call:
//                          crtps(PSRef,  InitIndexArray[], LastIndexArray[], StaticSign) 
  fe = new SgFunctionCallExp(*fdvm[CRTPS]);
  fmask[CRTPS] = 1;
  fe->addArg(*psref);            // PSRef
  fe -> addArg(*DVM000(ii));     // InitIndexArray
  fe -> addArg(*DVM000(il));     // LastIndexArray
  fe -> addArg(*ConstRef(sign)); // StaticSign
  return(fe);
}
/**************************************************************\
*                         Program blocks                      *
\**************************************************************/
int BeginBlock ()
{   int ib;
    SgExpression *re = new SgFunctionCallExp(*fdvm[BEGBL]);
    fmask[BEGBL] = 1; 
//generating assign statement
// dvm000(1) = BegBl()
// and inserting it before first executable statement
    ib = ndvm;
    doAssignStmt(re);
    return(ib);
}

void BeginBlock_H ()
{ 
//inserting Subroutine Call: dvmh_scope_start() 
    doCallStmt(ScopeStart());
    return;
}

SgStatement *EndBlock_H (SgStatement * st)
{  
    SgStatement *call = ScopeEnd();
    LINE_NUMBER_BEFORE(st,st);
//inserting Subroutine Call: dvmh_scope_end()
//before 'st' statement      
    InsertNewStatementBefore(call,st);
    return(call);
}

void EndBlock (SgStatement * st)
{  
//generating assign statement
// dvm000(i) = EndBl(BlockRef)
// and inserting it before current statement
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ENDBL]);
  fmask[ENDBL] = 1;
  //fe -> addArg(* DVM000(1)); 
   LINE_NUMBER_BEFORE(st,st);
   doAssignStmtBefore(fe,st); 
    return;
}

SgExpression * EndBl(int n)
{ 
//generating Function Call:
//                           EndBl(BlockRef)
 
 SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[ENDBL]);
  fmask[ENDBL] = 1;
  fe->addArg(*DVM000(n));
  return(fe);
}

/**************************************************************\
*            Abstract machine creating and mapping             *
\**************************************************************/
void Get_AM ()
{ 
    SgExpression *re =  new SgFunctionCallExp(*fdvm[GETAM]);
    fmask[GETAM] = 1;
//generating assign statement
// and inserting it before first executable statement
// dvm000(2) = GetAM()
    doAssignStmt(re);
    return;
}

SgExpression *GetAM ()
{ 
    SgExpression *re =  new SgFunctionCallExp(*fdvm[GETAM]);
    fmask[GETAM] = 1;
//generating function call: GetAM()
    return(re);
}

SgExpression *CreateAMView(SgExpression *size_array, int rank, int sign) {
  SgFunctionCallExp *fe;
  SgValueExp dAM(2);
  //SgArrayType *artype;
  SgExpression *arg;
  //algn_attr *atrAT;
  if(sign != 2)
    loc_distr = 1;
  else
    sign = 1;
// generating function call:
//                          CrtAMV(AMRef, rank, SizeArray, StaticSign) 
  fe = new SgFunctionCallExp(*fdvm[CRTAMV]);
  fmask[CRTAMV] = 1;
  arg = CurrentAM(); //new SgArrayRefExp(*dvmbuf, dAM); //dvm000(2) - AMRef
  fe->addArg(*arg);


    arg = ConstRef(rank); // Rank
    fe -> addArg(*arg);
    fe -> addArg(*size_array);  // SizeArray
    fe -> addArg(*ConstRef(sign)); // StaticSign
    return(fe);
}

SgExpression * DistributeAM (SgExpression *amv, SgExpression *psref, int count, int idisars, int iparam) {
// creating function call:
//             DisAM(AMViewRef,PSRef, ParamCount,AxisArray, DistrParamArray)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DISAM]); // DisAM function call
  fmask[DISAM] = 1;
  fe->addArg( amv->copy());
  fe->addArg( * psref);  // PSRef
  fe->addArg( * ConstRef (count));
  fe->addArg( * DVM000(idisars));
  fe->addArg( * DVM000(iparam));
  return(fe);
}

SgStatement *RedistributeAM(SgExpression *ref, SgExpression *psref, int count, int idisars,int sign) {
// creating subroutine call:
//      redis(AMViewRef,PSRef, ParamCount,AxisArray, DistrParamArray, NewSign)
  SgCallStmt *call = new SgCallStmt(*fdvm[RDISAM]);        
  fmask[RDISAM] = 2;
  call->addArg( ref->copy());
  call->addArg( * psref );  // PSRef 
  /*fe->addArg( * ConstRef(0)); */ // current PSRef
  call->addArg( * ConstRef (count));
  call->addArg( * DVM000(idisars));
  call->addArg( * DVM000(idisars+count));
  call->addArg( * ConstRef(sign));
  return(call);
}

SgExpression *GetAMView(SgExpression *headref)
 { SgFunctionCallExp *fe;
// creating function call:
//             getamv(HeaderRef)
 fe = new SgFunctionCallExp(*fdvm[GETAMV]);
 fmask[GETAMV] = 1;
 fe->addArg(* headref);
 return(fe);
}

SgExpression *GetAMR(SgExpression *amvref, SgExpression *index)
 { SgFunctionCallExp *fe;
// creating function call:
//             getamr(AMViewRef,IndexArray)
 fe = new SgFunctionCallExp(*fdvm[GETAMR]);
 fmask[GETAMR] = 1;
 fe->addArg(* amvref);
 fe->addArg(* index);
 return(fe);
}

SgExpression * GenBlock (SgExpression *psref, SgExpression *amv, int iweight, int icount)
 {
// creating function call:
//             genbli(PSRef,AMViewRef, AxisWeightArray, AxisCount)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[GENBLI]); // genbli function call
  fmask[GENBLI] = 1;
  fe->addArg( * psref);  // PSRef
  fe->addArg( amv->copy() );
  fe->addArg( * DVM000(iweight));
  fe->addArg( * ConstRef(icount));
  return(fe);
}

SgExpression * WeightBlock(SgExpression *psref, SgExpression *amv, int iweight, int iwnumb,  int icount)
 {
// creating function call:
//             setelw(PSRef,AMViewRef, LoadWeightArray, WeightNumberArray,Count)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[SETELW]); // setelw() function call
  fmask[SETELW] = 1;
  fe->addArg( * psref);  // PSRef
  fe->addArg( amv->copy() );
  fe->addArg( * DVM000(iweight));
  fe->addArg( * DVM000(iwnumb));
  fe->addArg( * ConstRef(icount));
  return(fe);
}

SgExpression * MultBlock (SgExpression *amv, int iaxisdiv, int n)
 {
// creating function call:
//             blkdiv(AMViewRef, AxisDivArray, AMVAxisCount)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[BLKDIV]); // blkdiv function call
  fmask[BLKDIV] = 1;
 
  fe->addArg( amv->copy() );
  fe->addArg( * DVM000(iaxisdiv));
  fe->addArg( * ConstRef(n));
  return(fe);
}
/**************************************************************\
*            Distributed array creating and mapping           *
\**************************************************************/
SgExpression *CreateDistArray(SgSymbol *das, SgExpression *array_header,                     SgExpression *size_array, int rank, int ileft, int iright,                  int sign, int re_sign) 
{
// creates function call:
//          CrtDA (ArrayHeader,ExtHdrSign,Base,Rank,TypeSize,SizeArray,
//                 StaticSign, ReDistrSign, LeftBSizeArray,RightBSizeArray)  
  SgFunctionCallExp *fe;
  SgExpression *arg;
  SgType *t;
  loc_distr =1;
  if(IS_POINTER(das))
    t = PointerType(das);
  else
    t = (das->type())->baseType();
  if(t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING){
    fe = new SgFunctionCallExp(*fdvm[CRTDA]); // crtda function call
    fmask[CRTDA] = 1;
  } else {
    fe = new SgFunctionCallExp(*fdvm[CRTDA9]); // crtda9 function call
    fmask[CRTDA9] = 1;
  }
  fe->addArg(* array_header);
  fe->addArg(*ConstRef(1)); //ExtHdrSign = 1 for Fortran
  arg = (t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING ) ? new SgArrayRefExp(*baseMemory(SgTypeInt())) : GetAddresMem(new SgArrayRefExp(*baseMemory(t))) ;  //SgArrayRefExp(*baseMemory(t))
  //TypeMemory(t); // marking this type memory use
  fe->addArg(*arg); //Base
  arg = ConstRef(rank);
  fe->addArg(*arg); //Rank
  arg = ConstRef(TypeSize(t));
  //arg = (t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING )? &SgUMinusOp(*ConstRef( TestType_RTS(t))) : ConstRef(TypeSize(t));
  fe->addArg(*arg); //TypeSize
  fe->addArg(size_array->copy()); //Size_array
  fe->addArg(*ConstRef(sign)); //StaticSign
  fe->addArg(*ConstRef(re_sign));  // ReDistrSign 
  fe->addArg(*DVM000(ileft));
  fe->addArg(*DVM000(iright));
  return(fe);
}

SgExpression *AlignArray (SgExpression *array_handle,
                          SgExpression *template_handle,
                          int iaxis, 
                          int icoeff,  
                          int iconst) 
//creating function call:
//  AlgnDA (ArrayHeader, PatternRef, AxisArray, CoeffArray, ConstArray)
{
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[ALGNDA]); // AlgnDA function call
  fmask[ALGNDA] =  1;
  fe->addArg( array_handle->copy());
  fe->addArg( template_handle->copy());
  fe->addArg( *dvm_ref(iaxis));
  fe->addArg( *dvm_ref(icoeff));
  fe->addArg( *dvm_ref(iconst));
  return(fe);
} 

SgStatement *RealignArr (SgExpression *array_header,
                          SgExpression *pattern_ref,
                          int iaxis, 
                          int icoeff,  
                          int iconst,
                          int new_sign ) 
//creating subroutine call:
// realn (ArrayHeader, PatternRef, AxisArray, CoeffArray, ConstArray, NewSign)
{
  SgCallStmt *call = new SgCallStmt(*fdvm[REALGN]);
  fmask[REALGN] = 2;
  call->addArg( array_header->copy());
  call->addArg( pattern_ref->copy());
  call->addArg( *dvm_ref(iaxis));
  call->addArg( *dvm_ref(icoeff));
  call->addArg( *dvm_ref(iconst));
  call->addArg( *ConstRef(new_sign));
  return(call);
}

/**************************************************************\
*            CONSISTENT(replicated) array creating             *
\**************************************************************/
SgExpression *CreateConsistArray(SgSymbol *cas, SgExpression *array_header, SgExpression *size_array, int rank,  int sign, int re_sign) 
{
// creates function call:
//       crtraf or  crtra9 (ArrayHeader,ExtHdrSign,Base,Rank,TypeSize,SizeArray, StaticSign, ReDistrSign, Memory) 
//           
  SgFunctionCallExp *fe;
  SgExpression *arg;
  SgType *t;
  loc_distr =1;
  
  t = (cas->type())->baseType();
    if(t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING){  
    fe = new SgFunctionCallExp(*fdvm[CRTRDA]); // crtraf function call
    fmask[CRTRDA] = 1;
  } else {
    fe = new SgFunctionCallExp(*fdvm[CRTRA9]); // crtra9 function call
    fmask[CRTRA9] = 1;
  }
  fe->addArg(* array_header);
  fe->addArg(*ConstRef(0)); //ExtHdrSign = 0 for consistent array
  //fe->addArg(*ConstRef(1)); //ExtHdrSign = 1 for Fortran
  arg = (t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING) ? new SgArrayRefExp(*cas) : GetAddresMem(new SgArrayRefExp(*baseMemory(t)));//new SgArrayRefExp(*Imem); SgArrayRefExp(*baseMemory(t))
  //TypeMemory(t); // marking this type memory use
  fe->addArg(*arg); //Base
  arg = ConstRef(rank);
  fe->addArg(*arg); //Rank
  arg = (t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING) ? &SgUMinusOp(*ConstRef( TestType_RTS(t))) : ConstRef(TypeSize(t));
  //arg = ConstRef(TypeSize(t));
  fe->addArg(*arg); //TypeSize
  fe->addArg(size_array->copy()); //Size_array
  fe->addArg(*ConstRef(sign)); //StaticSign
  fe->addArg(*ConstRef(re_sign));  // ReDistrSign 
  arg= new SgArrayRefExp(*cas);
  fe->addArg(*GetAddresMem(arg));
  return(fe);
}

SgStatement *CreateDvmArrayHeader(SgSymbol *cas, SgExpression *array_header, SgExpression *size_array, int rank,  int sign, int re_sign) 
{
// creates subroutine call:
//       crtraf or  crtra9 (ArrayHeader,ExtHdrSign,Base,Rank,TypeSize,SizeArray, StaticSign, ReDistrSign, Memory) 
//           
  SgCallStmt *call;
  SgExpression *arg;
  SgType *t;
  int test_type;
  loc_distr =1;
  
  t = (cas->type())->baseType();
  test_type = TestType_RTS(t);
  if(test_type) {  
    call = new SgCallStmt(*fdvm[CRTRDA]); // crtraf function call
    fmask[CRTRDA] = 2;
  } else {
    call = new SgCallStmt(*fdvm[CRTRA9]); // crtra9 function call
    fmask[CRTRA9] = 2;
  }
  call->addArg(* array_header);
  if(!IN_COMPUTE_REGION && !parloop_by_handler)
    call->addArg(*ConstRef(0));     //ExtHdrSign = 0 for consistent array
  else
    call->addArg(*ConstRef(1));     //ExtHdrSign = 1 for dvm array in region
  arg = (test_type) ? (HEADER_OF_REPLICATED(cas) ? new SgArrayRefExp(*baseMemory(t)) : new SgArrayRefExp(*cas)) : GetAddresMem(new SgArrayRefExp(*baseMemory(t)));//new SgArrayRefExp(*Imem); SgArrayRefExp(*baseMemory(t))
  call->addArg(*arg);               //Base
  arg = ConstRef(rank);
  call->addArg(*arg);               //Rank
  arg = (test_type) ? &SgUMinusOp(*ConstRef(test_type)) : ConstRef(TypeSize(t));
  
  call->addArg(*arg);               //TypeSize
  call->addArg(size_array->copy()); //Size_array
  call->addArg(*ConstRef(sign));    //StaticSign
  call->addArg(*ConstRef(re_sign)); // ReDistrSign 
  arg = new SgArrayRefExp(*cas);
  call->addArg(*GetAddresMem(arg)); // Memory
  return(call);
}

/**************************************************************\
*              Parallel Loop Defining                         *
\**************************************************************/
/*
int CreateParLoop(int rank)
{
//generating assign statement:
// dvm000(i) = crtpl( Rank)
// return: i - index in "dvm000" array for LoopRef 
  int il; 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTPLP]);
  fmask[CRTPLP] = 1;
  fe -> addArg( * ConstRef(rank));
  il = ndvm;
  doAssignStmtAfter(fe);
  return(il);
}
*/
SgExpression *CreateParLoop(int rank)
{     
//generating Function Call:
//                         crtpl( Rank)
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTPLP]);
  fmask[CRTPLP] = 1;
  fe -> addArg( * ConstRef(rank));
  return(fe);
}


SgExpression *doLoop(int iloopref)
{
//generating Function Call:
//                           dopl(LoopRef)
 
 SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DOLOOP]);
  fmask[DOLOOP] = 1;
  fe->addArg(*DVM000(iloopref));
  return(fe);
}


SgStatement * BeginParLoop (int iloopref,SgExpression *header, int rank, int iaxis, int nr, int iinp, int iout)
{
//creating subroutine call:
//   mappl(LoopRef, PatternRef, AxisArray[], CoefArray[], ConstArray[],
//          LoopVarAdrArray[], LoopVarTypeArray[], InpInitIndexArray[], InpLastIndexArray[],
//          InpStepArray[], 
//          OutInitIndexArray[], OutLastIndexArray[], OutStepArray[])
  
  SgCallStmt *call= new SgCallStmt(*fdvm[BEGPLP]);
  fmask[BEGPLP] = 2;
  call->addArg(*DVM000(iloopref));
  call->addArg(*header);
  call->addArg(*DVM000(iaxis));
  call->addArg(*DVM000(iaxis+nr));
  call->addArg(*DVM000(iaxis+2*nr));
  call->addArg(*DVM000(iinp));
  call->addArg(*DVM000(iinp+rank));
  call->addArg(*DVM000(iinp+2*rank));
  call->addArg(*DVM000(iinp+3*rank));
  call->addArg(*DVM000(iinp+4*rank));
  call->addArg(*DVM000(iout));
  call->addArg(*DVM000(iout+rank));
  call->addArg(*DVM000(iout+2*rank));
  return(call);
}

SgStatement *EndParLoop(int iloopref)
{
//generating Subroutine Call:
//                           EndPL(LoopRef)

  SgCallStmt *call= new SgCallStmt(*fdvm[ENDPLP]); 
  fmask[ENDPLP] = 2;                               
  call->addArg(*DVM000(iloopref));
  return(call);
}

SgStatement *BoundFirst(int iloopref, SgExpression *gref)
{
//generating Subroutine Call:
//                           exfrst(LoopRef,BoundGroupRef)
 
  SgCallStmt *call= new SgCallStmt(*fdvm[BFIRST]); 
  fmask[BFIRST] = 2;
  call->addArg(*DVM000(iloopref));
  call->addArg(gref->copy());
  return(call);
}

SgStatement *BoundLast(int iloopref, SgExpression *gref)
{
//generating Subroutine Call:
//                           imlast(LoopRef,BoundGroupRef)
 
  SgCallStmt *call= new SgCallStmt(*fdvm[BLAST]); 
  fmask[BLAST] = 2;
  call->addArg(*DVM000(iloopref));
  call->addArg(gref->copy());
  return(call);
}
 
/**************************************************************\
*                        Reduction                             *
\**************************************************************/
SgExpression * CreateReductionGroup()
{
//generating function call:
//                             CrtRG(StaticSign,DelRVSign)
  
  //int ig;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTRG]);
  fmask[CRTRG] = 1;
  fe->addArg(* ConstRef(1)); //StaticSign = 1
  fe->addArg(* ConstRef(1)); //DelRVSign = 1
  //ig = ndvm;
  //doAssignTo_After(gref,fe);
  return(fe);
}

SgExpression *ReductionVar(int num_red, SgExpression *red_array, int ntype, int length, SgExpression *loc_array, int loc_length, int sign)
{
//generating function call:
//                        crtrdf(RedFuncNumb, RedArray, RedArrayType, RedArrayLength,                                         LocArray, LocElmLength, StaticSign)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[REDVARF]);
  fmask[REDVARF] = 1;
    //fe = new SgFunctionCallExp(*fdvm[REDVAR]);
    //fmask[REDVAR] = 1;
  fe->addArg(*ConstRef(num_red));
  fe->addArg(*GetAddresMem(red_array));
    //fe->addArg(red_array->copy()); //!!!It must be: *GetAddresMem(red_array)
  fe->addArg(*ConstRef(ntype));
  fe->addArg(*DVM000(length));
  fe->addArg(loc_array->copy());
  fe->addArg(*DVM000(loc_length));
  fe->addArg(*ConstRef(sign));
  return(fe);
}

SgStatement *InsertRedVar(SgExpression *gref, int irv, int iplp)
{
//creating subroutine call:
//                       insred(RedGroupRef, RedVarRef, PSSpaceRef, RenewSign)
  SgCallStmt *call = new SgCallStmt(*fdvm[INSRV]); 
  fmask[INSRV] = 2;
  call->addArg(gref->copy());
  call->addArg(*dvm_ref(irv));
  if(iplp)
    call->addArg(*dvm_ref(iplp));
  else
    call->addArg(*ConstRef(0));
  call->addArg(*ConstRef(0));
  return(call);
}

SgExpression *LocIndType(int irv, int type)
{
//creating function call:
//                       lindtp(RedVarRef, LocIndType)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[LINDTP]);
  fmask[LINDTP] = 1;
  fe->addArg(*DVM000(irv));
  fe->addArg(*ConstRef(type));
  return(fe);
}

SgStatement *LoopReduction(int ilh, int num_red, SgExpression *red_array, int ntype, SgExpression *length, SgExpression *loc_array, SgExpression *loc_length)
{//creating Subroutine Call:
 //        dvmh_loop_reduction(const DvmType *pCurLoop, const DvmType *pRedType, void *arrayAddr, const DvmType *pVarType, const DvmType *pArrayLength,
 //                             void *locAddr, const DvmType *pLocSize)
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_RED]);  
  fmask[LOOP_RED] = 2;  
  call->addArg(*DVM000(ilh));
  call->addArg(*ConstRef(num_red));
  call->addArg(red_array->copy());     //GetAddresMem(red_array)
  call->addArg(*ConstRef(ntype));
  call->addArg(*DvmType_Ref(length));
  call->addArg(loc_array->copy());
  call->addArg(*DvmType_Ref(loc_length));
  return(call);
}

SgExpression *SaveRedVars(SgExpression *gref)
{
//creating function call:
//                        SaveRV(RedGroupRef)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[SAVERV]);
  fmask[SAVERV] = 1;
  fe->addArg(gref->copy());
  return(fe);
}

SgStatement *StartRed(SgExpression *gref)
{
//creating subroutine call:
//                        strtrd(RedGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[STARTR]);
  fmask[STARTR] = 2;
  call->addArg(gref->copy());
  return(call);
}

SgStatement *WaitRed(SgExpression *gref)
{
//creating subroutine call:
//                        waitrd(RedGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[WAITR]);
  fmask[WAITR] = 2;
  call->addArg(gref->copy());
  return(call);
}

SgExpression *DelRG(SgExpression *gref)
{
//creating function call:
//                        DelRG(RedGroupRef)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DELRG]);
  fmask[DELRG] = 1;
  fe->addArg(gref->copy());
  return(fe);
}

/**************************************************************\
*                   Shadow edge operations                     *
\**************************************************************/
void  CreateBoundGroup(SgExpression *gref)
{
//generating assign statement:
// dvm000(i) = crtshg(StaticSign)
  int st_sign; 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTSHG]);
  fmask[CRTSHG] = 1;
  st_sign = (HPF_program && one_inquiry) ? 1 : 0;
  //StaticSign = 1 if -Honeq option is specified for HPF program,  
  //StaticSign = 0 if other
  fe->addArg(* ConstRef(st_sign)); 
  //ibg = ndvm;
  doAssignTo_After(gref,fe);
  return;
}

SgStatement *InsertArrayBound(SgExpression *gref, SgExpression *head, int ileft, int iright, int corner) 
{
//creating subroutine call:
//                inssh(BounddGroupRef, ArrayHeader[], LeftBSize[], RightBSize[],CornerSign)
  SgCallStmt *call = new SgCallStmt(*fdvm[DATOSHG]);
  fmask[DATOSHG] = 2;
  call->addArg(gref->copy());
  call->addArg(*head);
  call->addArg(*DVM000(ileft));
  call->addArg(*DVM000(iright));
  call->addArg(*ConstRef(corner));
  return(call);
}

SgStatement *InsertArrayBoundDep(SgExpression *gref, SgExpression *head, int ileft, int iright, int max, int ishsign)
{
//creating subroutine call:
//             insshd(BounddGroupRef, ArrayHeader[], LeftBSize[], RightBSize[],MaxShadowCount,ShadowSignArray[])
  SgCallStmt *call = new SgCallStmt(*fdvm[INSSHD]);
  fmask[INSSHD] = 2;
  call->addArg(gref->copy());
  call->addArg(*head);
  call->addArg(*DVM000(ileft));
  call->addArg(*DVM000(iright));
  call->addArg(*ConstRef(max));
  call->addArg(*DVM000(ishsign));
  return(call);
}

SgStatement *InsertArrayBoundSec(SgExpression *gref, SgExpression *head, int ilsec, int irsec, int iilowshs, int illowshs, int iihishs,int ilhishs, int max, int ishsign)
{
//creating subroutine call:
//        incshd(BounddGroupRef, ArrayHeader[], InitDimIndex[], LastDimIndex[],InitLowShdIndex[],
//               LastLowShdIndex[], InitHiShdIndex[], LastHiShdIndex[],LeftBSize[], RightBSize[],MaxShadowCount,ShadowSignArray[])
  SgCallStmt *call = new SgCallStmt(*fdvm[INCSHD]);
  fmask[INCSHD] = 2;
  call->addArg(gref->copy());
  call->addArg(*head);
  call->addArg(*DVM000(ilsec));
  call->addArg(*DVM000(irsec));
  call->addArg(*DVM000(iilowshs));
  call->addArg(*DVM000(illowshs));
  call->addArg(*DVM000(iihishs));
  call->addArg(*DVM000(ilhishs));
  call->addArg(*ConstRef(max));
  call->addArg(*DVM000(ishsign));
  return(call);
}


SgStatement *AddBound( )
{
//creating subroutine call:
//                        addbnd()
  SgCallStmt *call = new SgCallStmt(*fdvm[ADDBND]);
  fmask[ADDBND] = 2;
  return(call);
}

SgStatement *AddBoundShadow(SgExpression *head,int ileft,int iright )
{
//creating subroutine call:
//                        addshd( ArrayHeader[], LeftBSize[], RightBSize[])
  SgCallStmt *call = new SgCallStmt(*fdvm[ADDSHD]);
  fmask[ADDSHD] = 2;
  call->addArg(*head);
  call->addArg(*DVM000(ileft));
  call->addArg(*DVM000(iright));
  return(call);
}

SgStatement *StartBound(SgExpression *gref)
{
//creating subroutine call:
//                        strtsh(BoundGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[STARTSH]);
  fmask[STARTSH] = 2;
  call->addArg(gref->copy());
  return(call);
}

SgStatement *WaitBound(SgExpression *gref)
{
//creating subroutine call:
//                        waitsh(BoundGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[WAITSH]);
  fmask[WAITSH] = 2;
  call->addArg(gref->copy());
  return(call);
}

SgStatement *SendBound(SgExpression *gref)
{
//creating subroutine call:
//                        sendsh(BoundGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[SENDSH]);
  fmask[SENDSH] = 2;
  call->addArg(gref->copy());
  return(call);
}

SgStatement *ReceiveBound(SgExpression *gref)
{
//creating subroutine call:
//                        recvsh(BoundGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[RECVSH]);
  fmask[RECVSH] = 2;
  call->addArg(gref->copy());
  return(call);
}

SgStatement *InitAcross(int acrtype,SgExpression *oldg, SgExpression *newg) 
{
//creating subroutine call:
//               across(AcrossType,OldShadowGroupRef,NewShadowGroupRef,GroupNumber)
  SgCallStmt *call = new SgCallStmt(*fdvm[ACROSS]);
  fmask[ACROSS] = 2;
  call->addArg(*ConstRef(acrtype));
  call->addArg(*oldg);
  call->addArg(*newg);
  call->addArg(*new SgVarRefExp(Pipe));
  return(call);
}


SgExpression *DelBG(SgExpression *gref)
{
//creating function call:
//                        DelShG(BoundGroupRef)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DELSHG]);
  fmask[DELSHG] = 1;
  fe->addArg(gref->copy());
  return(fe);
}

/**************************************************************\
*              Copying  distributed arrays                    *
\**************************************************************/
SgExpression *DA_CopyTo_A(SgExpression *head, SgExpression *toar, int init_ind,                                 int last_ind, int step_ind, int regim)
{
//generating Function Call:
// ArrCpy(ArrayHeader,FromInitIndexArray,FromLastIndexArray,FromStepArray,
//        Array, ToInitIndexArray, ToLastIndexArray, ToStepArray, CopyRegim)
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ARRCPY]);
 fmask[ARRCPY] = 1;
  fe->addArg(head->copy());
  fe->addArg(*DVM000(init_ind));
  fe->addArg(*DVM000(last_ind));
  fe->addArg(*DVM000(step_ind));

  fe->addArg(toar->copy());
  fe->addArg(*DVM000(init_ind)); //is ignored for CopyRegim=2
  fe->addArg(*DVM000(last_ind)); //is ignored for CopyRegim=2
  fe->addArg(*DVM000(step_ind)); //is ignored for CopyRegim=2

  fe->addArg(* ConstRef(regim)); // CopyRegim
  return(fe);
}

SgExpression *A_CopyTo_DA( SgExpression *fromar, SgExpression *head, int init_ind,                                 int last_ind, int step_ind, int regim)
{
//generating Function Call:
// ArrCpy(Array, FromInitIndexArray,FromLastIndexArray,FromStepArray,
//  ArrayHeader, ToInitIndexArray, ToLastIndexArray, ToStepArray, CopyRegim)
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ARRCPY]);
 fmask[ARRCPY] = 1;

  fe->addArg(fromar->copy());
  fe->addArg(*DVM000(init_ind)); //is ignored for CopyRegim=2
  fe->addArg(*DVM000(last_ind)); //is ignored for CopyRegim=2
  fe->addArg(*DVM000(step_ind)); //is ignored for CopyRegim=2

  fe->addArg(head->copy());
  fe->addArg(*DVM000(init_ind));
  fe->addArg(*DVM000(last_ind));
  fe->addArg(*DVM000(step_ind));

  fe->addArg(* ConstRef(regim)); // CopyRegim
  return(fe);
}

SgExpression *ArrayCopy(SgExpression *from_are, int from_init, int from_last, int from_step, SgExpression *to_are, int to_init, int to_last, int to_step, int regim)
{
//generating Function Call:
// ArrCpy(ArrayHeader,FromInitIndexArray,FromLastIndexArray,FromStepArray,
//        Array, ToInitIndexArray, ToLastIndexArray, ToStepArray, CopyRegim)
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ARRCPY]);
 fmask[ARRCPY] = 1;

  fe->addArg(from_are->copy());
  fe->addArg(*DVM000(from_init));
  fe->addArg(*DVM000(from_last));
  fe->addArg(*DVM000(from_step));

  fe->addArg(to_are->copy());
  fe->addArg(*DVM000(to_init)); 
  fe->addArg(*DVM000(to_last)); 
  fe->addArg(*DVM000(to_step)); 

  fe->addArg(* SignConstRef (regim)); // CopyRegim

  return(fe);
}

SgExpression *ReadWriteElement(SgExpression *from, SgExpression *to, int ind) 
{
//generating Function Call:
//                        rwelm(FromArrayHeader, To, IndexArray);
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RWELMF]);
 fmask[RWELMF] = 1;
   //SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RWELM]);
   //fmask[RWELM] = 1;

  fe->addArg(from->copy());
  fe->addArg(*GetAddresMem(to));
    //fe->addArg(to->copy());//!!!it must be: *GetAddresMem(to)
  fe->addArg(*DVM000(ind));
  return(fe);
}   

SgExpression *AsyncArrayCopy(SgExpression *from_are, int from_init, int from_last, int from_step, SgExpression *to_are, int to_init, int to_last, int to_step, int regim, SgExpression *flag)
{
//generating Function Call:
// aarrcp(ArrayHeader,FromInitIndexArray,FromLastIndexArray,FromStepArray,
//        Array, ToInitIndexArray, ToLastIndexArray, ToStepArray, CopyRegim,CopyFlag)
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[AARRCP]);
 fmask[AARRCP] = 1;

  fe->addArg(from_are->copy());
  fe->addArg(*DVM000(from_init));
  fe->addArg(*DVM000(from_last));
  fe->addArg(*DVM000(from_step));

  fe->addArg(to_are->copy());
  fe->addArg(*DVM000(to_init)); 
  fe->addArg(*DVM000(to_last)); 
  fe->addArg(*DVM000(to_step)); 

  fe->addArg(* SignConstRef (regim)); // CopyRegim
  fe->addArg(flag->copy());
  return(fe);
}

SgExpression *WaitCopy(SgExpression *flag)
{
//creating function call:
//                        waitcp(CopyFlag)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[WAITCP]);
  fmask[WAITCP] = 1;
  fe->addArg(flag->copy());
  return(fe);
}

/**************************************************************\
*                     Tasking                                   *
\**************************************************************/
SgStatement *MapAM(SgExpression *am, SgExpression *ps)
{
//generating Subroutine Call:
//                           mapam(AMRef,PSRef)
//creating task (mapping abstract mashine)
 SgCallStmt *call = new SgCallStmt(*fdvm[MAPAM]);
 fmask[MAPAM] = 2;
  
  call->addArg(*am);
  call->addArg(*ps);
  return(call);
}

SgExpression *RunAM(SgExpression *am)
{
//generating Function Call:
//                           runam(AMRef)
//starting task 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RUNAM]);
 fmask[RUNAM] = 1;
  
  fe->addArg(*am);
  return(fe);
}

SgStatement *StopAM()
{
//generating Subroutine Call:
//                           stopam()
//stoping task 
 SgCallStmt *call = new SgCallStmt(*fdvm[STOPAM]);
 fmask[STOPAM] = 2;  
 return(call);
}

SgStatement  *MapTasks(SgExpression *taskCount,SgExpression *procCount,SgExpression *params,SgExpression *low_proc,SgExpression *high_proc,SgExpression *renum)
{
//generating Subroutine Call:
//                           map_tasks(long taskCount,long procCount,double params,long low_proc,long high_proc,long renum) 
  SgCallStmt *call = new SgCallStmt(*fdvm[MAP_TASKS]);  
  fmask[MAP_TASKS] = 2;  
  call -> addArg(*taskCount);
  call -> addArg(*procCount);
  call -> addArg(*params);
  call -> addArg(*low_proc);
  call -> addArg(*high_proc);
  call -> addArg(*renum);
  return(call);
} 
/**************************************************************\
*                      Remote access                           *
\**************************************************************/
/*
SgExpression *LoadBG(SgSymbol *group)
{
//generating Function Call:
//                           loadbg(GroupRef,RenewSign)
//loading buffers of group 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOADBG]); 
 fmask[LOADBG] = 1;
 
  fe->addArg(*GROUP_REF(group,1));
  fe->addArg(*ConstRef(1));
  return(fe);
}

SgExpression *WaitBG(SgSymbol *group)
{
//generating Function Call:
//                           waitbg(GroupRef) 
//waiting of completion of loading buffers of the group
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[WAITBG]);
  fmask[WAITBG] = 1;

  fe->addArg(*GROUP_REF(group,1));
  return(fe);
}
*/

SgExpression *LoadBG(SgExpression *gref)
{
//generating Function Call:
//                           loadbg(GroupRef,RenewSign)
//loading buffers of group 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOADBG]); 
 fmask[LOADBG] = 1;
 
  fe->addArg(*gref);
  fe->addArg(*ConstRef(1));
  return(fe);
}

SgExpression *WaitBG(SgExpression *gref)
{
//generating Function Call:
//                           waitbg(GroupRef) 
//waiting of completion of loading buffers of the group
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[WAITBG]);
  fmask[WAITBG] = 1;

  fe->addArg(*gref);
  return(fe);
}

SgExpression *CreateBG(int st_sign,int del_sign)
{
//generating Function Call:
//                         crtbg(StaticSign,DelBufSign)
//creating group of buffers 
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[CRTBG]);
  fmask[CRTBG] = 1;
  
  fe->addArg(*ConstRef(st_sign));
  fe->addArg(*ConstRef(del_sign));
  return(fe);
}
/*
SgExpression *InsertRemBuf(SgSymbol *group, SgExpression *buf)
{
//generating Function Call:
//                           insrb(GroupRef,BufferHeader[])
//inserting buffer in the group
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[INSRB]);
 fmask[INSRB] = 1;
  
  fe->addArg(*GROUP_REF(group,1));
  fe->addArg(*buf);
  return(fe);
}
*/

SgExpression *InsertRemBuf(SgExpression *gref, SgExpression *buf)
{
//generating Function Call:
//                           insrb(GroupRef,BufferHeader[])
//inserting buffer in the group
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[INSRB]);
 fmask[INSRB] = 1;
  
  fe->addArg(*gref);
  fe->addArg(*buf);
  return(fe);
}

SgStatement *CreateRemBuf(SgExpression *header,SgExpression *buffer,int st_sign,int iplp,int iaxis,int icoeff,int iconst)
{
//generating Subroutine Call:
// crtrbl(ArrayHeader[],BufferHeader[], Base,StaticSign,LoopRef, AxisArray[],CoeffArray[],ConstArray[], )
//creating buffer for remote data
// SgSymbol *sbase;
  SgCallStmt *call = new SgCallStmt(*fdvm[CRTRB]);
  fmask[CRTRB] = 2;
  call->addArg(*header);
  call->addArg(*buffer);
  //sbase = (header->symbol()->type()->baseType()->variant() == T_STRING) ? Chmem : Imem;  /* podd 14.01.12 */
  //fe->addArg(* new SgArrayRefExp(*sbase)); //Base
  call->addArg(* new SgArrayRefExp(*Imem)); //Base
  call->addArg(*ConstRef(st_sign));
  call->addArg(*DVM000(iplp));
  call->addArg(*DVM000(iaxis));
  call->addArg(*DVM000(icoeff));
  call->addArg(*DVM000(iconst));

  return(call);
}
/*
SgExpression *CreateRemBuf(SgExpression *header,SgExpression *buffer,int st_sign,int icoeff,int iconst,int iinit,int ilast,int istep)
{
//generating Function Call:
// crtrbl(ArrayHeader[],BufferHeader[], Base,StaticSign,CoeffArray[],ConstArray[],
//       InitIndexArray[],LastIndexArray[],StepArray[])
//creating buffer for remote data
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTRB]);
 fmask[CRTRB] = 1;
  fe->addArg(*header);
  fe->addArg(*buffer);  
  fe->addArg(* new SgArrayRefExp(*Imem)); //Base
  fe->addArg(*ConstRef(st_sign));
  fe->addArg(*DVM000(icoeff));
  fe->addArg(*DVM000(iconst));
  fe->addArg(*DVM000(iinit));
  fe->addArg(*DVM000(ilast));
  fe->addArg(*DVM000(istep));
  return(fe);
}
*/

SgStatement *CreateRemBufP(SgExpression *header,SgExpression *buffer,int st_sign,SgExpression *psref,int icoord)
{
//generating Subroutine Call:
// crtrbp(ArrayHeader[],BufferHeader[], Base,StaticSign,LoopRef, AxisArray[],CoeffArray[],
//       ConstArray[], )
//creating buffer for remote data
  SgCallStmt *call = new SgCallStmt(*fdvm[CRTRBP]);
// SgSymbol *sbase;
  fmask[CRTRBP] = 2;
  call->addArg(*header);
  call->addArg(*buffer);
  //sbase = (header->symbol()->type()->baseType()->variant() == T_STRING) ? Chmem : Imem; /* podd 14.01.12 */ 
  //fe->addArg(* new SgArrayRefExp(*sbase)); //Base
  call->addArg(* new SgArrayRefExp(*Imem));  //Base
  call->addArg(*ConstRef(st_sign));
  call->addArg(*psref);
  call->addArg(*DVM000(icoord));
  return(call);
}

SgStatement *LoadRemBuf(SgExpression *buf)
{
//generating Subroutine Call:
//                           loadrb(BufferHeader,RenewSign)
//loading buffer
  SgCallStmt *call = new SgCallStmt(*fdvm[LOADRB]);
  fmask[LOADRB] = 2;
  
  call->addArg(*buf);
  call->addArg(*ConstRef(0));
  return(call);
}

SgStatement *WaitRemBuf(SgExpression *buf)
{
//generating Subroutine Call:
//                           waitrb(BufferHeader)
//waiting completion of loading buffer
  SgCallStmt *call = new SgCallStmt(*fdvm[WAITRB]);
  fmask[WAITRB] = 2;
  
  call->addArg(*buf);
  return(call);
}
/*
SgExpression *DelRemBuf(SgExpression *buf)
{
//generating Function Call:
//                           delrb(BufferHeader)
//deleting buffer
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DELRB]);
 fmask[DELRB] = 1;
  
  fe->addArg(*buf);
  return(fe);
}
*/


/**************************************************************\
*  Inquiry about the kind of distributed array element access  *
*                ( for HPF program)                            *                   
\**************************************************************/
SgExpression *RemoteAccessKind(SgExpression *header,SgExpression *buffer,int st_sign,int iplp,int iaxis,int icoeff,int iconst,int ilsh,int ihsh)
{
//generating Function Call:
// rmkind(ArrayHeader[],BufferHeader[], Base,StaticSign,LoopRef, AxisArray[],CoeffArray[],
//        ConstArray[], LowShadowArray[],HiShadowArray[])
//determinating data access kind: 1 - local, 2 - shadow, 3 - remote
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RMKIND]);
 fmask[RMKIND] = 1;
  fe->addArg(*header);
  fe->addArg(*buffer);  
  fe->addArg(* new SgArrayRefExp(*Imem)); //Base
  fe->addArg(*ConstRef(st_sign));
  fe->addArg(*DVM000(iplp));
  fe->addArg(*DVM000(iaxis));
  fe->addArg(*DVM000(icoeff));
  fe->addArg(*DVM000(iconst));
  fe->addArg(*DVM000(ilsh));
  fe->addArg(*DVM000(ihsh));

  return(fe);
}
/**************************************************************\
*                      Indirect access                         *
\**************************************************************/
SgExpression *LoadIG(SgSymbol *group)
{
//generating Function Call:
//                           loadig(GroupRef)
//loading buffers of group 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOADIG]);
 fmask[LOADIG] = 1;
  
  fe->addArg(*GROUP_REF(group,1));
  return(fe);
}

SgExpression *WaitIG(SgSymbol *group)
{
//generating Function Call:
//                           waitig(GroupRef)
//waiting of completion of loading buffers of the group
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[WAITIG]);
 fmask[WAITIG] = 1;
  
  fe->addArg(*GROUP_REF(group,1));
  return(fe);
}

SgExpression *CreateIG(int st_sign,int del_sign)
{
//generating Function Call:
//                           crtig(StaticSign,DelBufSign)
//creating group of buffers 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTIG]);
 fmask[CRTIG] = 1;
  
  fe->addArg(*ConstRef(st_sign));
  fe->addArg(*ConstRef(del_sign));
  return(fe);
}

SgExpression *InsertIndBuf(SgSymbol *group, SgExpression *buf)
{
//generating Function Call:
//                           insib(GroupRef,BufferHeader[])
//inserting buffer in the group
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[INSIB]);
 fmask[INSIB] = 1;
  
  fe->addArg(*GROUP_REF(group,1));
  fe->addArg(*buf);
  return(fe);
}

SgExpression *CreateIndBuf(SgExpression *header,SgExpression *buffer,int st_sign,SgExpression *mehead, int iconst)
{
//generating Function Call:
// crtib(ArrayHeader[],BufferHeader[], Base,StaticSign,MEHeader[],ConstArray[])

//creating buffer for indirect access data
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTIB]);
 fmask[CRTIB] = 1;
  fe->addArg(*header);
  fe->addArg(*buffer);  
  fe->addArg(* new SgArrayRefExp(*Imem)); //Base
  fe->addArg(*ConstRef(st_sign));
  fe->addArg(*mehead);
  fe->addArg(*DVM000(iconst));
  return(fe);
}

SgExpression *LoadIndBuf(SgExpression *buf)
{
//generating Function Call:
//                           loadib(BufferHeader)
//loading buffer
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOADIB]);
 fmask[LOADIB] = 1;
  
  fe->addArg(*buf);
  return(fe);
}

SgExpression *WaitIndBuf(SgExpression *buf)
{
//generating Function Call:
//                           waitib(BufferHeader)
//waiting completion of loading buffer
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[WAITIB]);
 fmask[WAITIB] = 1;
  
  fe->addArg(*buf);
  return(fe);
}
/*
SgExpression *DelIndBuf(SgExpression *buf)
{
//generating Function Call:
//                           delib(BufferHeader)
//deleting buffer
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DELIB]);
 fmask[DELIB] = 1;
  
  fe->addArg(*buf);
  return(fe);
}
*/

/**************************************************************\
*                      Getting array into consistent state     *
\**************************************************************/

SgExpression *StartConsistent(SgExpression *header,int iplp,int iaxis,int icoeff,int iconst,int re_sign)
{
//generating Function Call:
// strtac(ArrayHeader[],LoopRef, AxisArray[],CoeffArray[], ConstArray[], RenewSign )
//      
//start to get array into consistent state
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[STRTAC]);
 fmask[STRTAC] = 1;
  fe->addArg(*header); 
  fe->addArg(*DVM000(iplp));
  fe->addArg(*DVM000(iaxis));
  fe->addArg(*DVM000(icoeff));
  fe->addArg(*DVM000(iconst));
  fe->addArg(*ConstRef(re_sign));

  return(fe);
}

SgExpression *WaitConsistent(SgExpression *header)
{
//generating Function Call:
// waitac(ArrayHeader)
//      
//wait to get array into consistent state
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[WAITAC]);
 fmask[WAITAC] = 1;
  fe->addArg(*header); 

  return(fe);
}

SgExpression *FreeConsistent(SgExpression *header)
{
//generating Function Call:
// rstrda(ArrayHeader)
//      
//free memory of consistent array 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RSTRDA]);
 fmask[RSTRDA] = 1;
  fe->addArg(*header); 

  return(fe);
}

SgExpression *CreateConsGroup(int st_sign,int del_sign)
{
//generating Function Call:
//                         crtcg(StaticSign,DelArraySign)
//creating group of consistent arrays 
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[CRTCG]);
  fmask[CRTCG] = 1;
  
  fe->addArg(*ConstRef(st_sign));
  fe->addArg(*ConstRef(del_sign));
  return(fe);
}


SgExpression *InsertConsGroup(SgExpression *gref,SgExpression *header,int iplp,int iaxis,int icoeff,int iconst,int re_sign)
{
//generating Function Call:
// inscg(GroupRef,ArrayHeader[],LoopRef, AxisArray[],CoeffArray[], ConstArray[],RenewSign )
//      
//insert  array into consistent group
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[INSCG]);
 fmask[INSCG] = 1;
  fe->addArg(*gref);
  fe->addArg(*header); 
  fe->addArg(*DVM000(iplp));
  fe->addArg(*DVM000(iaxis));
  fe->addArg(*DVM000(icoeff));
  fe->addArg(*DVM000(iconst));
  fe->addArg(*ConstRef(re_sign));
  return(fe);
}

SgExpression *ExstractConsGroup(SgExpression *gref, int del_sign)
{
//generating Function Call:
//                         rstcg(GroupRef,DelArraySign)
//extracting all  consistent arrays from group 
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[RSTCG]);
  fmask[RSTCG] = 1;
  
  fe->addArg(*gref);
  fe->addArg(*ConstRef(del_sign));
  return(fe);
}

SgExpression *StartConsGroup(SgExpression *gref)
{
//generating Function Call:
//                           strtcg(GroupRef) 
//starting of getting group of arrays into consistent state
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[STRTCG]);
  fmask[STRTCG] = 1;

  fe->addArg(*gref);
  return(fe);
}

SgExpression *WaitConsGroup(SgExpression *gref)
{
//generating Function Call:
//                           waitcg(GroupRef) 
//waiting completion of getting group of arrays into consistent state
  SgFunctionCallExp *fe  = new SgFunctionCallExp(*fdvm[WAITCG]);
  fmask[WAITCG] = 1;

  fe->addArg(*gref);
  return(fe);
}

/**************************************************************\
*        Getting array into consistent state in Task_Region    *
\**************************************************************/
SgExpression *TaskConsistent(SgExpression *header,SgExpression *amvref, int iaxis, int re_sign)
{
//generating Function Call:
// consda(ArrayHeader,AMViewRef,ArrayAxis,RenewSign)
//      
//start to get array into consistent state in Task_Region
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CONSDA]);
 fmask[CONSDA] = 1;
  fe->addArg(*header); 
  fe->addArg(*amvref); //copy?? 
  fe->addArg(*DVM000(iaxis));
  fe->addArg(*ConstRef(re_sign));
  return(fe);
}

SgExpression *IncludeConsistentTask(SgExpression *gref,SgExpression *header,SgExpression *amvref, int iaxis,int re_sign)
{
//generating Function Call:
// inclcg(GroupRef,ArrayHeader,AMViewRef,ArrayAxis)
//      
//include array into consistent group in Task_Region
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[INCLCG]);
 fmask[INCLCG] = 1;
  fe->addArg(*gref); 
  fe->addArg(*header); 
  fe->addArg(*amvref); //copy?? 
  fe->addArg(*DVM000(iaxis));
  fe->addArg(*ConstRef(re_sign));
  return(fe);
}

/**************************************************************\
*                      Special ACROSS                         *
\**************************************************************/

SgExpression *DVM_Receive(int iplp,SgExpression *mem,int t,int is)
{
//generating Function Call:
//                           dvm_rm(LoopRef,MemAddr,ElmType,ElmNumber)

 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMRM]);
 fmask[DVMRM] = 1;
  fe->addArg(*DVM000(iplp));
  fe->addArg(*mem);
  fe->addArg(*ConstRef(t));
  fe->addArg(*DVM000(is));
  return(fe);
}

SgExpression *DVM_Send(int iplp,SgExpression *mem,int t,int is)
{
//generating Function Call:
//                           dvm_sm(LoopRef,MemAddr,ElmType,ElmNumber)

 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMSM]);
 fmask[DVMSM] = 1;
  fe->addArg(*DVM000(iplp));
  fe->addArg(*mem);
  fe->addArg(*ConstRef(t));
  fe->addArg(*DVM000(is));
  return(fe);
}


/**************************************************************\
*                      Miscellaneous functions                 *
\**************************************************************/
SgExpression *GetRank(int iref)
{
//generating Function Call:
//                           GetRnk(ObjectRef)
// requesting rank of object
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[GETRNK]);
 fmask[GETRNK] = 1;
  fe->addArg(*DVM000(iref));
  return(fe);
}

SgExpression *GetSize(SgExpression *ref,int axis)
{
//generating Function Call:
//                           GetSiz(ObjectRef, Axis)
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[GETSIZ]);
 fmask[GETSIZ] = 1;
  fe->addArg(*ref);
  fe->addArg(* ConstRef (axis));
  return(fe);
}

SgExpression * TestIOProcessor ()
{ 
// creates function call:      TstIOP()
    fmask[TSTIOP] = 1;
    return( new SgFunctionCallExp(*fdvm[TSTIOP]));
}

SgExpression *DeleteObject(SgExpression *objref) 
{
//generating Function Call:
//                        delobj(ObjectRef)
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DELOBJ]);
 fmask[DELOBJ] = 1;

  fe->addArg(objref->copy());
 
  return(fe);
}   

SgExpression *TestElement(SgExpression *head, int ind) 
{
//generating Function Call:
//                        tstelm(ArrayHeader, IndexArray);
 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[TSTELM]);
 fmask[TSTELM] = 1;

  fe->addArg(head->copy());
  fe->addArg(*DVM000(ind));
  return(fe);
}    

SgStatement *SendMemory(int icount, int inda, int indl) 
{
//generating Subroutine Call:
//                     call   srmem (MemoryCount, StartAddrArray, LengthArray);
 send =1;

 SgCallStmt *call = new SgCallStmt(*fdvm[SRMEM]);
 fmask[SRMEM] = 2;

  call->addArg(*ConstRef_F95(icount)); //addArg(*DVM000(icount));
  call->addArg(*DVM000(inda));
  call->addArg(*DVM000(indl));
  return(call);
}      

SgExpression *GetAddres(SgSymbol * var)
{
//generating Function Call:
//                           GetAdr(Var)

 SgFunctionCallExp *fe;
 int ind;
 // ind = GETADR;
  ind = NameIndex(Base_Type(var->type()));
  fe = new SgFunctionCallExp(*fdvm[ind]);
  fmask[ind] = 1;
  fe->addArg(* new SgVarRefExp (* var));
  return(fe);
}

SgExpression *GetAddresMem(SgExpression * em)
{
//generating Function Call:
//                           GetAdr(Var)
 
 SgFunctionCallExp *fe;
 int ind;
 //  ind = GETADR;
  ind = NameIndex(Base_Type(em->type())); 
  fe = new SgFunctionCallExp(*fdvm[ind]);
  fmask[ind] = 1;
  fe->addArg(em->copy());
  return(fe);
}

SgStatement *Addres(SgExpression * em)
{
//generating assign statement:
//                   dvm000(ndvm)= GetAdr(Var)
 
 SgFunctionCallExp *fe;
 int ind;
  ind = NameIndex(Base_Type(em->type())); 
  fe = new SgFunctionCallExp(*fdvm[ind]);
  fmask[ind] = 1;
  fe->addArg(em->copy());
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
}

SgExpression *GetAddresDVM(SgExpression * em)
{
//generating Function Call:
//                           GetAdr(Var)
 
 SgFunctionCallExp *fe;
 int ind;
 //  ind = GETADR;
  ind = NameIndex(SgTypeInt()); //argument type of DVM-Lib functions (headers and others)
  fe = new SgFunctionCallExp(*fdvm[ind]);
  fmask[ind] = 1;
  fe->addArg(em->copy());
  return(fe);
}


SgStatement *CloseFiles() 
{
//generating Subroutine Call:  clfdvm()                          

 SgCallStmt *call = new SgCallStmt(*fdvm[CLFDVM]);
 fmask[CLFDVM] = 2;     
  return(call);
}

SgExpression *AddHeader(SgExpression *head_new,SgExpression *head )
{
//generating Function Call:  addhdr(NewHeadRef, Headref) 

 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ADDHDR]);
 fmask[ADDHDR] =1;
 fe->addArg(*head_new);
 fe->addArg(*head);
 return(fe);
}   
/*
SgExpression *TypeControl(int n, int iadr)
{
//generating Function Call:  tpcntr(Numb,FirstAddr[],NextAddr[],Len[],Type[]) 

 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[TPCNTR]);
 fmask[TPCNTR] =1;
 fe->addArg(*ConstRef(n));
 fe->addArg(*DVM000(iadr));
 fe->addArg(*DVM000(iadr+n));
 fe->addArg(*DVM000(iadr+2*n));
 fe->addArg(*DVM000(iadr+3*n));
 return(fe);
}   
*/

SgExpression *Barrier()
{
//generating Function Call:
//                           bsynch()
//stoping task 
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[BARRIER]);
 fmask[BARRIER] = 1;  
 return(fe);
}
/**************************************************************\
*                    Debugger functions                        *
\**************************************************************/
SgStatement *D_RegistrateArray(int rank, int type, SgExpression *headref,  SgExpression *size_array,SgExpression *arref) 
{
//generating Subroutine Call: drarr(Rank,Type,Addr,Size_array,Operand)                       
  SgCallStmt *call = new SgCallStmt(*fdvm[DRARR]);
  fmask[DRARR] = 2;
  call->addArg(*ConstRef(rank));
  call->addArg(*ConstRef(type));
  call->addArg(*headref);
  call->addArg(*size_array);
  call->addArg(*new SgValueExp(UnparseExpr(arref)));
  return(call);
}   

SgStatement *D_LoadVar(SgExpression *vref, int type, SgExpression *headref, SgExpression *opref) 
{
//generating Subroutine Call: dldv(TypePtr,Addr,Handle,Operand)                       
 
  SgCallStmt *call = new SgCallStmt(*fdvm[DLOADV]);
  fmask[DLOADV] = 2;
  call->addArg(*ConstRef(type));
  call->addArg(*GetAddresMem(vref));
  call->addArg(*headref);
  call->addArg(*new SgValueExp(UnparseExpr(opref)));
  return(call);
/*
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DLOADV]);
 fmask[DLOADV] = 1;
  fe->addArg(*ConstRef(type));
  fe->addArg(*GetAddresMem(vref));
  fe->addArg(*headref);
  fe->addArg(*new SgValueExp(UnparseExpr(opref)));
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
*/
}   

SgStatement *D_LoadVar2(SgExpression *vref, int type, SgExpression *headref, SgExpression *opref) 
{
//generating Subroutine Call: dldv2(TypePtr,Addr,Handle,Operand)                       

 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DLOAD2]);
 fmask[DLOAD2] = 1;
  fe->addArg(*ConstRef(type));
  fe->addArg(*GetAddresMem(vref));
  fe->addArg(*headref);
  fe->addArg(*new SgValueExp(UnparseExpr(opref)));
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
}   

SgStatement *D_StorVar() 
{
//generating Subroutine Call:  dstv()                          

 SgCallStmt *call = new SgCallStmt(*fdvm[DSTORV]);
 fmask[DSTORV] = 2;
  return(call);
/*
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DSTORV]);
 fmask[DSTORV] = 1;
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
*/
}

SgStatement *D_PrStorVar(SgExpression *vref, int type, SgExpression *headref, SgExpression *opref) 
{
//generating Subroutine Call:  dprstv(TypePtr,Addr,Handle,Operand)                          
  SgCallStmt *call = new SgCallStmt(*fdvm[DPRSTV]);
  fmask[DPRSTV] = 2;
  call->addArg(*ConstRef(type));
  call->addArg(*GetAddresMem(vref));
  call->addArg(*headref);
  call->addArg(*new SgValueExp(UnparseExpr(opref)));
  return(call);

/*
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DPRSTV]);
 fmask[DPRSTV] = 1;
  fe->addArg(*ConstRef(type));
  fe->addArg(*GetAddresMem(vref));
  fe->addArg(*headref);
  fe->addArg(*new SgValueExp(UnparseExpr(opref)));
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
*/
}

SgStatement *D_InOutVar(SgExpression *vref, int type, SgExpression *headref) 
{
//generating Subroutine Call: dinout(TypePtr,Addr,Handle)                       
/* 
 SgCallStmt *call = new SgCallStmt(*fdvm[DINOUT]);
         //fmask[DINOUT] = 1;
  call->addArg(*ConstRef(type));
  call->addArg(*GetAddresMem(vref));
  call->addArg(*headref);
  return(call);
*/
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DINOUT]);
 fmask[DINOUT] = 1;
  fe->addArg(*ConstRef(type));
  fe->addArg(*GetAddresMem(vref));
  fe->addArg(*headref); 
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
}   

SgStatement *D_Fname()
{
//generating Subroutine Call:  fname(FileName) 
/*
  SgCallStmt *call = new SgCallStmt(*fdvm[FNAME]);
  call->addArg(*new SgValueExp(fin_name));
  return(call);
*/
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[FNAME]);
 fmask[FNAME] =1;
  fe->addArg(*new SgValueExp(fin_name));
  ndvm++;
  FREE_DVM(1);
  return(new SgAssignStmt(*DVM000(ndvm),*fe));
}   

SgStatement *D_Lnumb(int num_line)
{
//generating Subroutine Call:  lnumb(LineNumber) 
/*
  SgCallStmt *call = new SgCallStmt(*fdvm[LNUMB]);
  call->addArg(*new SgValueExp(num_line));
  return(call);
*/
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LNUMB]);
 fmask[LNUMB] =1;
 fe->addArg(*DVM000(num_line));
  ndvm++;
  FREE_DVM(1);
 return(new SgAssignStmt(*DVM000(ndvm),*fe));
}   

SgStatement *D_FileLine(int num_line, SgStatement *stmt)
{
//generating Subroutine Call:  dvmlf(LineNumber,FileName) 

  //char *fname;
 filename_list *fn;
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMLF]);
 fmask[DVMLF] =1;
 fe->addArg(*DVM000(num_line));
 fn = AddToFileNameList(stmt->fileName());
 //fname= new char[80];
 //sprintf(fname,"%s%s",stmt->fileName()," ");
 //fe->addArg(* new SgValueExp(fname));
  fe->addArg(* new SgVarRefExp(fn->fns));
  ndvm++;
  FREE_DVM(1);
 return(new SgAssignStmt(*DVM000(ndvm),*fe));
}   

SgStatement *D_DummyFileLine(int num_line, const char *fname)
{
//generating Subroutine Call:  dvmlf(LineNumber,FileName) 

 filename_list *fn;
 SgCallStmt *call = new SgCallStmt(*fdvm[DVMLF]);
 fmask[DVMLF] =2;
 call->addArg(*DVM000(num_line));
 fn = AddToFileNameList(fname);
 call->addArg(* new SgVarRefExp(fn->fns));
 ndvm++;
 FREE_DVM(1);
 return(call);
}   

SgStatement *D_FileLineConst(int line, SgStatement *stmt)
{
//generating Subroutine Call:  call dvmlf(LineNumber,FileName) 

 filename_list *fn;
 SgCallStmt *call = new SgCallStmt(*fdvm[DVMLF]);
 fmask[DVMLF] =2;
 call->addArg(*ConstRef_F95(line));
 fn = AddToFileNameList(baseFileName(stmt->fileName()));
 call->addArg(* new SgVarRefExp(fn->fns));
 return(call);
}   


SgStatement *D_Begpl(int num_loop,int rank,int iinit)
{
//generating Subroutine Call:  dbegpl(Rank,No,InitArray,LastArray,StepArray) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DBEGPL]);
  fmask[DBEGPL] = 2;
  call->addArg(*ConstRef(rank));
  call->addArg(*ConstRef_F95(num_loop));//addArg(*DVM000(num_loop));
  call->addArg(*DVM000(iinit));
  call->addArg(*DVM000(iinit+rank));
  call->addArg(*DVM000(iinit+2*rank));
  return(call);
}   

SgStatement *D_Begsl(int num_loop)
{
//generating Subroutine Call:  dbegsl(No) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DBEGSL]);
  fmask[DBEGSL] = 2;
  call->addArg(*ConstRef_F95(num_loop)); //addArg(*DVM000(num_loop));
  return(call);
}   

SgStatement *D_Begtr(int num_treg)
{
//generating Subroutine Call:  dbegtr(No) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DBEGTR]);
  fmask[DBEGTR] = 2;
  call->addArg(*DVM000(num_treg));
  return(call);
}   

SgExpression *doPLmb(int iloopref, int ino)
{ 
//generating Function Call:
//                           doplmb(LoopRef,No)
 
 SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DOPLMB]);
  fmask[DOPLMB] = 1;
  fe->addArg(*DVM000(iloopref));
  fe->addArg(*DVM000(ino));
  return(fe);
}

SgExpression *doPLmbSEQ(int ino, int rank, int iout)
{
//generating Function Call:
//                           doplmbseq(No, Rank, OutInit[], OutLast[], OutStep[])
 
 SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DOPLSEQ]);
  fmask[DOPLSEQ] = 1;
  fe->addArg(*DVM000(ino));
  fe->addArg(* ConstRef(rank));
  fe->addArg(*DVM000(iout));
  fe->addArg(*DVM000(iout+rank));
  fe->addArg(*DVM000(iout+2*rank));
  return(fe);
}


SgExpression *doSL(int num_loop,int iout)
{ 
//generating Function Call:
//                           dosl(No, OutInit, OutLast, OutStep)
 
 SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DOSL]);
  fmask[DOSL] = 1;
  fe->addArg(*ConstRef_F95(num_loop)); //addArg(*DVM000(num_loop));
  fe->addArg(*DVM000(iout));
  fe->addArg(*DVM000(iout+1));
  fe->addArg(*DVM000(iout+2));
  return(fe);
}


SgStatement *D_Skpbl()
{
//generating Subroutine Call:  dskpbl() 
  SgCallStmt *call = new SgCallStmt(*fdvm[DSKPBL]);
  fmask[DSKPBL] = 2;
  return(call);
}   

SgStatement *D_Endl(int num_loop, int begin_line )
{
//generating Subroutine Call:  dendl(No,Line) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DENDL]);
  fmask[DENDL] = 2;
  call->addArg(*ConstRef_F95(num_loop));   //addArg(*DVM000(num_loop));
  call->addArg(*ConstRef_F95(begin_line)); //addArg(*DVM000(begin_line));
  return(call);
}   

SgStatement *D_Iter(SgSymbol *do_var, int type)
{
//generating Subroutine Call:  diter(Index,TypeIndex) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DITER]);
  fmask[DITER] = 2;
  call->addArg(*GetAddres(do_var));
  call->addArg(*ConstRef(type));
  return(call);
}   

SgStatement *D_Iter_I(int ind, int indtp)
{
//generating Subroutine Call:  diter(IndexArray,TypeIndexArray) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DITER]);
  fmask[DITER] = 2;
  call->addArg(*DVM000(ind));
  call->addArg(*DVM000(indtp));
  return(call);
}   

SgStatement *D_Iter_ON(int ind, int type)
{
//generating Subroutine Call:  diter(Index,TypeIndex) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DITER]);
  fmask[DITER] = 2;
  call->addArg(*GetAddresMem(DVM000(ind)));
  call->addArg(*ConstRef(type));
  return(call);
}   

SgStatement *D_RmBuf(SgExpression *source_headref, SgExpression *buf_headref, int rank, int index) 
{
//generating Subroutine Call:  drmbuf(Src,RmtBuff,Rank,Index)                          
 
 SgCallStmt *call = new SgCallStmt(*fdvm[DRMBUF]);
 fmask[DRMBUF] = 2;
  call->addArg(*source_headref );
  call->addArg(*buf_headref);
  call->addArg(* ConstRef(rank));
  call->addArg(* DVM000(index));
  return(call);
}

SgStatement *D_Read(SgExpression *adr) 
{
//generating Subroutine Call:
//                        dread(Addr);

  SgCallStmt *call = new SgCallStmt(*fdvm[DREAD]);
  fmask[DREAD] = 2;
  call->addArg(*adr);
  return(call);
}      

SgStatement *D_ReadA(SgExpression *adr,int indel, int icount) 
{
//generating Subroutine Call:
//                        dreada(StartArrayAddr, ElemLength, ArrayLength);
  SgCallStmt *call = new SgCallStmt(*fdvm[DREADA]);
  fmask[DREADA] = 2;
  call->addArg(*adr);
  call->addArg(*DVM000(indel));
  call->addArg(*DVM000(icount));
  return(call);
}    

SgExpression * D_CreateDebRedGroup()
{
//generating function call:
//                             dcrtrg()
  
  //int ig;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DCRRG]);
  fmask[DCRRG] = 1;
  return(fe);
}

SgStatement *D_InsRedVar(SgExpression *dgref,int num_red, SgExpression *red_array, int ntype, int length, SgExpression *loc_array, int loc_length, int locindtype)
{
//generating subroutine call:
//                 dinsrd(DebRedGroupref, RedFuncNumb, RedArray, RedArrayType, RedArrayLength, LocArray, LocElmLength, LocIndType)
  SgCallStmt *call = new SgCallStmt(*fdvm[DINSRD]);
  fmask[DINSRD] = 2;
 
  call->addArg(dgref->copy());
  call->addArg(*ConstRef(num_red));
  call->addArg(*GetAddresMem(red_array));
  call->addArg(*ConstRef(ntype));
  call->addArg(*DVM000(length));
  call->addArg(loc_array->copy());
  call->addArg(*DVM000(loc_length));
  call->addArg(*ConstRef(locindtype));
  return(call);
}

SgExpression *D_SaveRG(SgExpression *dgref)
{
//creating function call:
//                        dsavrg(DebRedGroupRef)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DSAVRG]);
  fmask[DSAVRG] = 1;
  fe->addArg(dgref->copy());
  return(fe);
}

SgStatement *D_CalcRG(SgExpression *dgref)
{
//creating subroutine call:
//                        dclcrg(DebRedGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[DCLCRG]);
  fmask[DCLCRG] = 2;
  call->addArg(dgref->copy());
  return(call);
}

SgStatement *D_DelRG(SgExpression *dgref)
{
//creating subroutine call:
//                        ddelrg(DebRedGroupRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[DDLRG]);
  fmask[DDLRG] = 2;
  call->addArg(dgref->copy());
  return(call);
}

SgExpression *SummaOfDistrArray(SgExpression *headref, SgExpression *sumvarref)
{
//creating function call:
//                        dacsum(HeaderArrayRef,CheckSum)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[DACSUM]);
  fmask[DACSUM] = 1;
  fe->addArg(*headref);
  fe->addArg(*sumvarref);
  return(fe);
}

SgExpression *SummaOfArray(SgExpression *are, int rank, SgExpression *size, int ntype,SgExpression *sumvarref)
{
//creating function call:
//                        arcsf(addrMem,Rank,SizeArray[],Type,CheckSum)
  SgFunctionCallExp *fe;
  fe = new SgFunctionCallExp(*fdvm[ARCSF]);
  fmask[ARCSF] = 1;
  fe->addArg(*GetAddresMem(are));
  fe->addArg(*ConstRef(rank));
  fe->addArg(*size);
  fe->addArg(*ConstRef(ntype));
  fe->addArg(*sumvarref);
  return(fe);
}
  
SgStatement *D_PutDebugVarAdr(SgSymbol *dbg_var, int flag)
{
//generating Subroutine Call:  dvtr(dbgvar,flag) 
  SgCallStmt *call = new SgCallStmt(*fdvm[DVTR]);
  fmask[DVTR] = 2;
  call->addArg(*new SgVarRefExp(*dbg_var));
  call->addArg(*new SgValueExp(flag));
  return(call);
}   
/**************************************************************\
*              Performance Analyzer functins                   *
\**************************************************************/
SgStatement *St_Binter(int num_fragment, SgExpression *valvar)  //(int num_fragment, int valvar)
{
//generating Subroutine Call:  binter(nfrag, valvar) 
  SgCallStmt *call = new SgCallStmt(*fdvm[BINTER]);
  fmask[BINTER] = 2;
  call->addArg(*ConstRef_F95(num_fragment));                    //(*DVM000(num_fragment));
  call->addArg(*valvar);                                       //(* DVM000(valvar));
  return(call);
}   

SgStatement *St_Einter(int num_fragment,int begin_line)
{
//generating Subroutine Call:  einter(nfrag,nline) 
  SgCallStmt *call = new SgCallStmt(*fdvm[EINTER]);
  fmask[EINTER] = 2;
  call->addArg(*ConstRef_F95(num_fragment));                        //(*DVM000(num_fragment));
  call->addArg(*ConstRef_F95(begin_line));                          // (*DVM000(begin_line));
  return(call);
}   

SgStatement *St_Bsloop(int num_fragment)
{
//generating Subroutine Call:  bsloop(nfrag) 
  SgCallStmt *call = new SgCallStmt(*fdvm[BSLOOP]);
  fmask[BSLOOP] = 2;
  call->addArg(*ConstRef_F95(num_fragment)); //addArg(*DVM000(num_fragment));
  return(call);
}   


SgStatement *St_Bploop(int num_fragment)
{
//generating Subroutine Call:  bploop(nfrag) 
  SgCallStmt *call = new SgCallStmt(*fdvm[BPLOOP]);
  fmask[BPLOOP] = 2;
  call->addArg(*ConstRef_F95(num_fragment)); //addArg(*DVM000(num_fragment));
  return(call);
}   

SgStatement *St_Enloop(int num_fragment,int begin_line)
{
//generating Subroutine Call:  enloop(nfrag,nline) 
  SgCallStmt *call = new SgCallStmt(*fdvm[ENLOOP]);
  fmask[ENLOOP] = 2;
  call->addArg(*ConstRef_F95(num_fragment));//addArg(*DVM000(num_fragment));
  call->addArg(*ConstRef_F95(begin_line));  //addArg(*DVM000(begin_line));
  return(call);
} 

SgStatement *St_Biof()
{
//generating Subroutine Call:  biof() 
  SgCallStmt *call = new SgCallStmt(*fdvm[BIOF]);
  fmask[BIOF] = 2;
  return(call);
} 

SgStatement *St_Eiof()
{
//generating Subroutine Call:  eiof() 
  SgCallStmt *call = new SgCallStmt(*fdvm[EIOF]);
  fmask[EIOF] = 2;
  return(call);
}              



/**************************************************************\
*              FORTRAN 90 functins                             *
\**************************************************************/

SgExpression *SizeFunction(SgSymbol *ar, int i)
{//SgSymbol *symb_SIZE;
 SgFunctionCallExp *fe;
 if(!HEADER(ar)) { 
// generating function call: SIZE(ARRAY, DIM)
   if(!f90[SIZE])     //(!SIZE_function)
     f90[SIZE] = new SgFunctionSymb(FUNCTION_NAME, "size", *SgTypeInt(), *cur_func); 
   fe = new SgFunctionCallExp(*f90[SIZE]);
   fe -> addArg(*new SgArrayRefExp(*ar));//array
   if(i != 0) 
     fe -> addArg(*new SgValueExp(i));  // dimension number 
   return(fe);
  } else
   return(GetSize(HeaderRefInd(ar,1),Rank(ar)-i+1));
}

SgExpression *SizeFunctionWithKind(SgSymbol *ar, int i, int kind)
{//SgSymbol *symb_SIZE;
 SgFunctionCallExp *fe;
 if(!HEADER(ar)) { 
// generating function call: SIZE(ARRAY, DIM)
   if(!f90[SIZE])     //(!SIZE_function)
     f90[SIZE] = new SgFunctionSymb(FUNCTION_NAME, "size", *SgTypeInt(), *cur_func); 
   fe = new SgFunctionCallExp(*f90[SIZE]);
   fe -> addArg(*new SgArrayRefExp(*ar));//array
   if(i != 0) 
     fe -> addArg(*new SgValueExp(i));  // dimension number 
   if(kind != 0) 
     fe -> addArg(*new SgExpression(KIND_OP,new SgValueExp(kind),NULL,NULL));  // kind of type for result 

   return(fe);
  } else
   return(GetSize(HeaderRefInd(ar,1),Rank(ar)-i+1));
}

SgExpression *LBOUNDFunction(SgSymbol *ar, int i)
{//SgSymbol *symb_SIZE;
 SgFunctionCallExp *fe;  
// generating function call: LBOUND(ARRAY, DIM)
  if(!f90[LBOUND])   
    f90[LBOUND] = new SgFunctionSymb(FUNCTION_NAME, "lbound", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[LBOUND]);
  fe -> addArg(*new SgArrayRefExp(*ar));//array
  if(i != 0) 
  fe -> addArg(*new SgValueExp(i));  // dimension number
 
   return(fe);
}

SgExpression *UBOUNDFunction(SgSymbol *ar, int i)
{//SgSymbol *symb_SIZE;
 SgFunctionCallExp *fe;  
// generating function call: UBOUND(ARRAY, DIM)
  if(!f90[UBOUND])   
    f90[UBOUND] = new SgFunctionSymb(FUNCTION_NAME, "ubound", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[UBOUND]);
  fe -> addArg(*new SgArrayRefExp(*ar));//array
  if(i != 0) 
  fe -> addArg(*new SgValueExp(i));  // dimension number
 
   return(fe);
}

SgExpression *LENFunction(SgSymbol *string)
{
 SgFunctionCallExp *fe;  
// generating function call: LEN(STRING)
  if(!f90[LEN])   
    f90[LEN] = new SgFunctionSymb(FUNCTION_NAME, "len", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[LEN]);
  fe -> addArg(*new SgVarRefExp(*string));//string

   return(fe);
}

SgExpression *CHARFunction(int i)
{
 SgFunctionCallExp *fe;  
// generating function call: CHAR(I)
  if(!f90[CHAR])   
    f90[CHAR] = new SgFunctionSymb(FUNCTION_NAME, "char", *SgTypeChar(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[CHAR]);
  fe -> addArg(*new SgValueExp(i));

   return(fe);
}

SgExpression *TypeFunction(SgType *t, SgExpression *e, SgExpression *ke)
{int i = -1;
 SgFunctionCallExp *fe;
 SgExpression *kke;
  
// generating function call: INT(e,KIND(ke)), REAL(e,KIND(ke)),...
  switch(t->variant()) {
      case T_INT:      if(!f90[F_INT])   
                         f90[F_INT] = new SgFunctionSymb(FUNCTION_NAME, "int", *SgTypeInt(), *cur_func); 
                       i = F_INT;
                       break;

      case T_BOOL:     if(!f90[F_LOGICAL])   
                         f90[F_LOGICAL] = new SgFunctionSymb(FUNCTION_NAME, "logical", *SgTypeBool(), *cur_func); 
                       i = F_LOGICAL;
                       break;
      case T_FLOAT:    
      case T_DOUBLE:   if(!f90[F_REAL])   
                         f90[F_REAL] = new SgFunctionSymb(FUNCTION_NAME, "real", *SgTypeFloat(), *cur_func); 
                       i = F_REAL;
                       break;

      case T_COMPLEX:  
      case T_DCOMPLEX: if(!f90[F_CMPLX])   
                         f90[F_CMPLX] = new SgFunctionSymb(FUNCTION_NAME, "cmplx", *SgTypeComplex(current_file), *cur_func); 
                       i = F_CMPLX;
                       break;

      case T_STRING:   
      case T_CHAR:     if(!f90[F_CHAR])   
                         f90[F_CHAR] = new SgFunctionSymb(FUNCTION_NAME, "char", *SgTypeChar(), *cur_func); 
                       i = F_CHAR; 
                       break;


      default:         break;       
  }
  fe = new SgFunctionCallExp(*f90[i]);
  fe -> addArg(e->copy());
  if(ke)
  {  kke = (i==F_CMPLX) ? new SgKeywordArgExp("kind",*ke) : ke;    
     fe -> addArg(*kke);
  }
  return(fe);
}

SgExpression *KINDFunction(SgExpression *arg)
{
 SgFunctionCallExp *fe;  
// generating function call: KIND(arg)
  if(!f90[KIND])   
    f90[KIND] = new SgFunctionSymb(FUNCTION_NAME, "kind", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[KIND]);
  fe -> addArg(*arg);

   return(fe);
}

SgExpression *MaxFunction(SgExpression *arg1,SgExpression *arg2)
{
 SgFunctionCallExp *fe;  
// generating function call: MAX(arg1,arg2)
  if(!f90[MAX_])
      //f90[MAX_] = new SgFunctionSymb(FUNCTION_NAME);   
    f90[MAX_] = new SgFunctionSymb(FUNCTION_NAME, "max", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[MAX_]);
  fe -> addArg(*arg1);
  fe -> addArg(*arg2);

   return(fe);
}

SgExpression *MinFunction(SgExpression *arg1,SgExpression *arg2)
{
 SgFunctionCallExp *fe;  
// generating function call: MIN(arg1,arg2)
  if(!f90[MIN_])
       
    f90[MIN_] = new SgFunctionSymb(FUNCTION_NAME, "min", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[MIN_]);
  fe -> addArg(*arg1);
  fe -> addArg(*arg2);

   return(fe);
}

SgExpression *IandFunction(SgExpression *arg1,SgExpression *arg2)
{
 SgFunctionCallExp *fe;  
// generating function call: IAND(arg1,arg2)
  if(!f90[IAND_])
       
    f90[IAND_] = new SgFunctionSymb(FUNCTION_NAME, "iand", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[IAND_]);
  fe -> addArg(*arg1);
  fe -> addArg(*arg2);

   return(fe);
}

SgExpression *IorFunction(SgExpression *arg1,SgExpression *arg2)
{
 SgFunctionCallExp *fe;  
// generating function call: IOR(arg1,arg2)
  if(!f90[IOR_])
       
    f90[IOR_] = new SgFunctionSymb(FUNCTION_NAME, "ior", *SgTypeInt(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[IOR_]);
  fe -> addArg(*arg1);
  fe -> addArg(*arg2);

   return(fe);
}

SgExpression *AllocatedFunction(SgExpression *arg)
{
 SgFunctionCallExp *fe;  
// generating function call: ALLOCATED(arg)
  if(!f90[ALLOCATED_])
       
    f90[ALLOCATED_] = new SgFunctionSymb(FUNCTION_NAME, "allocated", *SgTypeBool(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[ALLOCATED_]);
  fe -> addArg(*arg);

   return(fe);
}

SgExpression *AssociatedFunction(SgExpression *arg)
{
 SgFunctionCallExp *fe;  
// generating function call: ASSOCIATED(arg)
  if(!f90[ASSOCIATED_])
       
    f90[ASSOCIATED_] = new SgFunctionSymb(FUNCTION_NAME, "associated", *SgTypeBool(), *cur_func); 
  fe = new SgFunctionCallExp(*f90[ASSOCIATED_]);
  fe -> addArg(*arg);

   return(fe);
}

/**************************************************************\
*                      C  functins                             *
\**************************************************************/

SgExpression *mallocFunction(SgExpression *arg, SgStatement *scope)
{
 SgFunctionCallExp *fe;  
// generating function call:
//                             malloc(arg)
        
  SgSymbol *sf = new SgFunctionSymb(FUNCTION_NAME, "malloc", *C_PointerType(C_VoidType()), *scope); 
  fe = new SgFunctionCallExp(*sf);
  fe -> addArg(*arg);

   return(fe);
}

SgExpression *freeFunction(SgExpression *arg, SgStatement *scope)
{
 SgFunctionCallExp *fe;  
// generating function call:
//                             free(arg)
        
  SgSymbol *sf = new SgFunctionSymb(FUNCTION_NAME, "free", *C_VoidType(), *scope); 
  fe = new SgFunctionCallExp(*sf);
  fe -> addArg(*arg);

   return(fe);
}


/**************************************************************\
* ACC                                                          *
*              Generating RTS2 Function Calls                  *
\**************************************************************/

SgStatement *RTL_GPU_Init()
{// generating subroutine call: call dvmh_init(DvmType *flagsRef)
//  flags: 1 - Fortran, 2 - without regions (-noH),
//         4 - sequential program (-s), 8 - OpenMP will be used.

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_INIT]);
  fmask[DVMH_INIT] = 2;
  call -> addArg(*DVM000(ndvm));
  if(!only_debug && ACC_program)
    call -> addComment(OpenMpComment_InitFlags(ndvm));
 
  int flag = 1;
  if(only_debug)
             flag = flag + 4;
  else if(!ACC_program)
             flag = flag + 2;
  doAssignStmtAfter(new SgValueExp(flag));        
  FREE_DVM(1); 
  doCallAfter(call);
  return(call);
}

SgStatement *RTL_GPU_Finish()
{// generating subroutine call: call dvmh_finish()
  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_FINISH]);
  fmask[DVMH_FINISH] = 2;
  return(call);
}

SgStatement *Init_Cuda()
{// generating subroutine call: call init_cuda()
  SgCallStmt *call = new SgCallStmt(*fdvm[INIT_CUDA]);
  fmask[INIT_CUDA] = 2;
  cur_st->insertStmtAfter(*call,*cur_st->controlParent());
  cur_st = call;
  return(call);
}

SgExpression *RegionCreate(int flag)
{ // generating function call: region_create(FlagsRef)
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[REG_CREATE]);
  fmask[REG_CREATE] = 1;
    //fe->addArg(*ConstRef(flag));
  

  if(flag==0) 
    fe->addArg(*ConstRef(flag));
  else
  { SgSymbol *symb;
    symb = region_const[flag]; 
    fe->addArg(*new SgVarRefExp(*symb));    
  }
  return(fe);
}

SgStatement *StartRegion(int irgn)
{ // generating Subroutine call:  region_inner_start(DvmhRegionRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[REG_START]);
  fmask[REG_START] = 2;
  call -> addArg(*DVM000(irgn));
  return(call);
}

SgStatement *RegionForDevices(int irgn,SgExpression *devices)
{ // generating Subroutine call:  region_execute_on_targets(DvmhRegionRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[REG_DEVICES]);
  fmask[REG_DEVICES] = 2;
  call -> addArg(*DVM000(irgn));
  call -> addArg(*devices);
  return(call);
}

/*
SgExpression *RegistrateDataRegion()
{ // generating function call: crt_data_region_gpu()
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DATAREG_GPU]);
  fmask[DATAREG_GPU] = 1;
  return(fe);
}
*/

SgStatement *EndRegion(int irgn)
{ // generating Subroutine call:  region_end(DvmhRegionRef)
  SgCallStmt *call = new SgCallStmt(*fdvm[REG_END]);
  fmask[REG_END] = 2;
  call -> addArg(*DVM000(irgn));
  return(call);
}

/*
SgStatement *UnRegistrateDataRegion(int n)
{ // generating Subroutine call:  end_data_region_gpu(InOutDataRegionGpu)
  SgCallStmt *call = new SgCallStmt(*fdvm[ENDDATAREG_GPU]);
  fmask[ENDDATAREG_GPU] = 2;
  call -> addArg(*GPU000(n));
  return(call);
}
*/
/*
SgStatement *RegistrateDVMArray(SgSymbol *ar,int ireg,int inflag,int outflag)
{  //generating Subroutine Call:  
   //    crtda_gpu(InRegionGpu, InDvmArray[], OutDvmGpuArray[], InDeviceBaseAddr, InCopyinFlag, InCopyoutFlag) 
  SgExpression *gpubase;
  SgCallStmt *call = new SgCallStmt(*fdvm[CRTDA_GPU]);  
  fmask[CRTDA_GPU] = 2;
  
  gpubase = new SgArrayRefExp(*baseGpuMemory(ar->type()->baseType()));
  call -> addArg(*GPU000(ireg));
  call -> addArg(*HeaderRef(ar));
  call -> addArg(*GpuHeaderRef(ar));
  call -> addArg(*gpubase);
  call -> addArg(*ConstRef(inflag));
  call -> addArg(*ConstRef(outflag));
  
  return(call);
}
*/

SgStatement *RegisterScalar(int irgn,SgSymbol *c_intent,SgSymbol *s)
{  //generating Subroutine Call:  
   //    region_register_scalar(DvmhRegionRef, intentRef, addr, sizeRef, varType) 
  int ntype;
  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_SCALAR]);  
  fmask[RGSTR_SCALAR] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  call -> addArg(*new SgVarRefExp(s));
  if(isSgArrayType(s->type()))
     call -> addArg(*TypeFunction(SgTypeInt(),ArrayLength(s,cur_region->region_dir,0), new SgValueExp(DVMTypeLength())));
  else
     call -> addArg(*ConstRef_F95(TypeSize(s->type())));
  ntype = VarType_RTS(s);  // as for reduction variables
  ntype = ntype ? ntype : -1;  // unknown type
  call -> addArg(*ConstRef_F95(ntype) );
  return(call);
}

SgStatement *RegionRegisterScalar(int irgn,SgSymbol *c_intent,SgSymbol *s)
{  //generating Subroutine Call:  
   //    dvmh_region_register_scalar(const DvmType *pCurRegion, const DvmType *pIntent, const void *addr, const DvmType *pTypeSize,const DvmType *pVarNameStr) 
  int ntype;
  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_SCALAR_2]);  
  fmask[RGSTR_SCALAR_2] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  call -> addArg(*new SgVarRefExp(s));
  call -> addArg(*TypeSize_RTS2(s->type()));
  call -> addArg(*DvmhString(new SgValueExp(s->identifier())));
  return(call);
}

SgStatement *RegisterSubArray(int irgn, SgSymbol *c_intent, SgSymbol *ar, int ilow, int ihigh)
{  //generating Subroutine Call:  
   //    region_register_subarray(DvmhRegionRef, intentRef, dvmDesc[], lowIndex[], highIndex[], elemType) 

  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_SUBARRAY]);  
  fmask[RGSTR_SUBARRAY] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  if(HEADER(ar)) //DVM-array
    call -> addArg(*HeaderRef(ar));
  else // replicated array
    call -> addArg(*DVM000(*HEADER_OF_REPLICATED(ar)));
  call -> addArg(*DVM000(ilow));
  call -> addArg(*DVM000(ihigh));
  call -> addArg(*ConstRef_F95( TestType_DVMH(ar->type())));
  return(call);
}

SgStatement *RegionRegisterSubArray(int irgn, SgSymbol *c_intent, SgSymbol *ar, SgExpression *index_list)
{  //generating Subroutine Call:  
   //    dvmh_region_register_subarray(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr,
   //                                  const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */... ) 

  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_SUBARRAY_2]);  
  fmask[RGSTR_SUBARRAY_2] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  if(HEADER(ar)) //DVM-array
    call -> addArg(*HeaderRef(ar));
  else // replicated array
    call -> addArg(*DVM000(*HEADER_OF_REPLICATED(ar)));
  call->addArg(*DvmhString(new SgValueExp(ar->identifier())));
  call -> addArg(*ConstRef_F95(Rank(ar)));
  call -> addArg(*index_list);
  return(call);
}

SgStatement *RegisterArray(int irgn, SgSymbol *c_intent, SgSymbol *ar)
{  //generating Subroutine Call:  
   //    region_register_array(DvmhRegionRef, intentRef, dvmDesc[], elemType) 

  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_ARRAY]);  
  fmask[RGSTR_ARRAY] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  if(HEADER(ar)) //DVM-array or TEMPLATE
    call -> addArg(*HeaderRef(ar));
  else // replicated array
    call -> addArg(*DVM000(*HEADER_OF_REPLICATED(ar)));
  call -> addArg(*ConstRef_F95( TestType_DVMH(ar->type())));
  return(call);
}

SgStatement *RegionRegisterArray(int irgn, SgSymbol *c_intent, SgSymbol *ar)
{  //generating Subroutine Call:  
   //    dvmh_region_register_array(const DvmType *pCurRegion, const DvmType *pIntent, const DvmType dvmDesc[], const DvmType *pVarNameStr) 

  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_ARRAY_2]);  
  fmask[RGSTR_ARRAY_2] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  if(HEADER(ar)) //DVM-array or TEMPLATE
    call -> addArg(*HeaderRef(ar));
  else // replicated array
    call -> addArg(*DVM000(*HEADER_OF_REPLICATED(ar)));
  call -> addArg(*DvmhString(new SgValueExp(ar->identifier())));
  return(call);
}

SgStatement *Dvmh_Line(int line, SgStatement *stmt) 
{ // generating Subroutine call:
  //                   dvmh_line(const DvmType *pLineNumber, const DvmType *pFileNameStr)

 filename_list *fn;
 SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_LINE]);
 fmask[DVMH_LINE] =2;
 call->addArg(*ConstRef_F95(line));
 fn = AddToFileNameList(baseFileName(stmt->fileName()));
 call->addArg(*DvmhString(new SgVarRefExp(fn->fns)));
 return(call);
}   


SgExpression *DvmhString(SgExpression *s) 
{  
  // generating function call:  dvmh_string(const char s[])
  
  fmask[STRING] = 1;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[STRING]);
  fe->addArg(*s);
  return fe; 
}


SgExpression *DvmhStringVariable(SgExpression *v) 
{  
  // generates function call: dvmh_string_variable (char s[])
  
  fmask[STRING_VAR] = 1;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[STRING_VAR]);
  fe->addArg(*v);
  return fe;
  
}

SgExpression *DvmhVariable(SgExpression *v) 
{  
  // generates function call:  dvmh_get_addr(void *pVariable)
  
  fmask[GET_ADDR] = 1;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[GET_ADDR]);
  fe->addArg(*v);
  return fe;
  
}

SgExpression *HasElement(SgExpression *ar_header, int n, SgExpression *index_list)
{
  // generates function call:  
  //               dvmh_has_element(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...);  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_HAS_ELEMENT]);
  fmask[DVMH_HAS_ELEMENT] = 1;
  fe->addArg(*ar_header);
  fe->addArg(*ConstRef_F95(n));
  AddListToList(fe->lhs(),index_list);  
  return fe;

}

SgExpression *CalculateLinear(SgExpression *ar_header, int n, SgExpression *index_list)
{
  // generates function call:  
  //              dvmh_calc_linear(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pGlobalIndex */...);
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CALC_LINEAR]);
  fmask[CALC_LINEAR] = 1;
  fe->addArg(*ar_header);
  fe->addArg(*ConstRef_F95(n));
  AddListToList(fe->lhs(),index_list);  
  return fe;

}

SgStatement *SaveCheckpointFilenames(SgExpression *cpName, std::vector<SgExpression *> filenames) {
  fmask[CP_SAVE_FILENAMES] = 2;
  SgCallStmt *callStmt = new SgCallStmt(*fdvm[CP_SAVE_FILENAMES]);
  callStmt->addArg(*DvmhString(cpName));
  
  SgExpression *filenamesLength = DvmType_Ref(new SgValueExp((int) filenames.size()));
  callStmt->addArg(*filenamesLength);
  
  std::vector<SgExpression *>::iterator it = filenames.begin();
  for (; it != filenames.end(); it++) {
    callStmt->addArg(*DvmhString(*it));
  }
  return callStmt;
}


SgStatement *CheckFilename(SgExpression *cpName, SgExpression *filename) {
  fmask[CP_CHECK_FILENAME] = 2;
  SgCallStmt *callStmt = new SgCallStmt(*fdvm[CP_CHECK_FILENAME]);
  callStmt->addArg(*DvmhString(cpName));
  callStmt->addArg(*DvmhString(filename));
  
  return callStmt;
  
}

SgStatement *CpWait(SgExpression *cpName, SgExpression *statusVar) {
  fmask[CP_WAIT] = 2;
  SgCallStmt *callStmt = new SgCallStmt(*fdvm[CP_WAIT]);
  callStmt->addArg(*DvmhString(cpName));
  callStmt->addArg(*DvmhVariable(statusVar));
  return callStmt;
}

SgStatement *CpSaveAsyncUnit(SgExpression *cpName, SgExpression *file, SgExpression *unit) {
  fmask[CP_SAVE_ASYNC_UNIT] = 2;
  SgCallStmt *callStmt = new SgCallStmt(*fdvm[CP_SAVE_ASYNC_UNIT]);
  callStmt->addArg(*DvmhString(cpName));
  callStmt->addArg(*DvmhString(file));
  callStmt->addArg(*DvmType_Ref(unit));
  return callStmt;
}

SgStatement *GetNextFilename(SgExpression *cpName, SgExpression *lastFile, SgExpression *currentFile) {
  fmask[CP_NEXT_FILENAME] = 2;
  SgCallStmt *callStmt = new SgCallStmt(*fdvm[CP_NEXT_FILENAME]);
  callStmt->addArg(*DvmhString(cpName));
  callStmt->addArg(*DvmhString(lastFile));
  callStmt->addArg(*DvmhStringVariable(currentFile));
  
  return callStmt;
}

/*
SgStatement *RegisterBufferArray(int irgn, SgSymbol *c_intent, SgExpression *bufref, int ilow, int ihigh)
{  //generating Subroutine Call:  
   //    region_register_subarray(DvmhRegionRef, intentRef, dvmDesc[], lowIndex[], highIndex[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[RGSTR_SUBARRAY]);  
  fmask[RGSTR_SUBARRAY] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*new SgVarRefExp(c_intent));
  call -> addArg(*bufref);
  call -> addArg(*DVM000(ilow));
  call -> addArg(*DVM000(ihigh));
  return(call);
}
*/

SgStatement *SetArrayName(int irgn, SgSymbol *ar)
{  //generating Subroutine Call:  
   //   region_set_name_array(DvmhRegionRef *regionRef, long dvmDesc[], const char *name) 

  SgCallStmt *call = new SgCallStmt(*fdvm[SET_NAME_ARRAY]);  
  fmask[SET_NAME_ARRAY] = 2;
  
  call -> addArg(*DVM000(irgn));
 
  if(HEADER(ar)) //DVM-array
    call -> addArg(*HeaderRef(ar));
  else // replicated array
    call -> addArg(*DVM000(*HEADER_OF_REPLICATED(ar)));
  call -> addArg(*new SgValueExp(ar->identifier()));
  return(call);
}

SgStatement *SetVariableName(int irgn, SgSymbol *var)
{  //generating Subroutine Call:  
   //   region_set_name_variable(DvmhRegionRef *regionRef, void *addr, const char *name) 

  SgCallStmt *call = new SgCallStmt(*fdvm[SET_NAME_VAR]);  
  fmask[SET_NAME_VAR] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(* new SgVarRefExp(var));
  call -> addArg(*new SgValueExp(var->identifier()));
  return(call);
}

SgStatement *RegionBeforeLoadrb(SgExpression *bufref)
{  //generating Subroutine Call:  
   //    dvmh_remote_access( dvmDesc[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[BEFORE_LOADRB]);  
  fmask[BEFORE_LOADRB] = 2;
  
  call -> addArg(*bufref);
  return(call);
}

SgStatement *RegionAfterWaitrb(int irgn, SgExpression *bufref)
{  //generating Subroutine Call:  
   //    region_after_waitrb(DvmhRegionRef, dvmDesc[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[REG_WAITRB]);  
  fmask[REG_WAITRB] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*bufref);
  return(call);
}

SgStatement *RegionDestroyRb(int irgn, SgExpression *bufref)
{  //generating Subroutine Call:  
   //    region_destroy_rb(DvmhRegionRef, dvmDesc[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[REG_DESTROY_RB]);  
  fmask[REG_DESTROY_RB] = 2;
  
  call -> addArg(*DVM000(irgn));
  call -> addArg(*bufref);
  return(call);
}

SgStatement *ActualScalar(SgSymbol *s)
{  //generating Subroutine Call:  
   //    dvmh_actual_variable(addr) 
   //  or when RTS2 is used
   //    dvmh_actual_variable2(const void *addr)
  int fNum = INTERFACE_RTS2 ? ACTUAL_SCALAR_2 : ACTUAL_SCALAR;
  SgCallStmt *call = new SgCallStmt(*fdvm[fNum]);  
  fmask[fNum] = 2;
  
  call -> addArg(*new SgVarRefExp(s));
  
  return(call);
}

SgStatement *ActualSubVariable(SgSymbol *s, int ilow, int ihigh)
{  //generating Subroutine Call:  
   //    dvmh_actual_subvariable(addr, lowIndex[], highIndex[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[ACTUAL_SUBVAR]);  
  fmask[ACTUAL_SUBVAR] = 2;
  
  call -> addArg(*new SgVarRefExp(s));
  call -> addArg(*DVM000(ilow));
  call -> addArg(*DVM000(ihigh));
  
  return(call);
}

SgStatement *ActualSubVariable_2(SgSymbol *s, int rank, SgExpression *index_list)
{  //generating Subroutine Call:  
   //    dvmh_actual_subvariable2(const void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...)

  SgCallStmt *call = new SgCallStmt(*fdvm[ACTUAL_SUBVAR_2]);  
  fmask[ACTUAL_SUBVAR_2] = 2;
  
  call -> addArg(*new SgVarRefExp(s));
  call -> addArg(*ConstRef(rank));
  AddListToList(call->expr(0),index_list);    
  return(call);
}


SgStatement *ActualSubArray(SgSymbol *ar, int ilow, int ihigh)
{  //generating Subroutine Call:  
   //    dvmh_actual_subarray(dvmDesc[], lowIndex[], highIndex[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[ACTUAL_SUBARRAY]);  
  fmask[ACTUAL_SUBARRAY] = 2;
  
  call -> addArg(*HeaderRef(ar));
  call -> addArg(*DVM000(ilow));
  call -> addArg(*DVM000(ihigh));
  return(call);
}

SgStatement *ActualSubArray_2(SgSymbol *ar, int rank, SgExpression *index_list)
{  //generating Subroutine Call:  
   //    dvmh_actual_subarray2(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...) 

  SgCallStmt *call = new SgCallStmt(*fdvm[ACTUAL_SUBARRAY_2]);  
  fmask[ACTUAL_SUBARRAY_2] = 2;
  
  call -> addArg(*HeaderRef(ar));
  call -> addArg(*ConstRef(rank));
  AddListToList(call->expr(0),index_list);
  return(call);
}

SgStatement *ActualArray(SgSymbol *ar)
{  //generating Subroutine Call:  
   //    dvmh_actual_array(dvmDesc[]) 
   //  or when RTS2 is used
   //    dvmh_actual_array2(const DvmType dvmDesc[])
  int fNum = INTERFACE_RTS2 ? ACTUAL_ARRAY_2 : ACTUAL_ARRAY;
  SgCallStmt *call = new SgCallStmt(*fdvm[fNum]);  
  fmask[fNum] = 2;
  
  call -> addArg(*HeaderRef(ar));
  return(call);
}

SgStatement *ActualAll()
{  //generating Subroutine Call:  
   //    dvmh_actual_all() 
   //  or when RTS2 is used
   //    dvmh_actual_all2() 
  int fNum = INTERFACE_RTS2 ? ACTUAL_ALL_2 : ACTUAL_ALL;
  SgCallStmt *call = new SgCallStmt(*fdvm[fNum]);  
  fmask[fNum] = 2;
  return(call);
}

SgStatement *GetActualScalar(SgSymbol *s)
{  //generating Subroutine Call:  
   //    dvmh_get_actual_variable(addr) 
   //  or when RTS2 is used
   //    dvmh_get_actual_variable2(void *addr)
  int fNum = INTERFACE_RTS2 ? GET_ACTUAL_SCALAR_2 : GET_ACTUAL_SCALAR;
  SgCallStmt *call = new SgCallStmt(*fdvm[fNum]);  
  fmask[fNum] = 2;
  
  call -> addArg(*new SgVarRefExp(s));
  
  return(call);
}

SgStatement *GetActualSubVariable(SgSymbol *s, int ilow, int ihigh)
{  //generating Subroutine Call:  
   //    dvmh_get_actual_subvariable(addr, lowIndex[], highIndex[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[GET_ACTUAL_SUBVAR]);  
  fmask[GET_ACTUAL_SUBVAR] = 2;
  
  call -> addArg(*new SgVarRefExp(s));
  call -> addArg(*DVM000(ilow));
  call -> addArg(*DVM000(ihigh));
 
  return(call);
}

SgStatement *GetActualSubVariable_2(SgSymbol *s, int rank, SgExpression *index_list)
{  //generating Subroutine Call:  
   //    dvmh_get_actual_subvariable2(void *addr, const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...); 

  SgCallStmt *call = new SgCallStmt(*fdvm[GET_ACTUAL_SUBVAR_2]);  
  fmask[GET_ACTUAL_SUBVAR_2] = 2;
  
  call -> addArg(*new SgVarRefExp(s));
  call -> addArg(*ConstRef(rank));
  AddListToList(call->expr(0),index_list);  
  return(call);
}

SgStatement *GetActualSubArray(SgSymbol *ar, int ilow, int ihigh)
{  //generating Subroutine Call:  
   //    dvmh_get_actual_subarray(dvmDesc[], lowIndex[], highIndex[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[GET_ACTUAL_SUBARRAY]);  
  fmask[GET_ACTUAL_SUBARRAY] = 2;
  
  call -> addArg(*HeaderRef(ar));
  call -> addArg(*DVM000(ilow));
  call -> addArg(*DVM000(ihigh));
  return(call);
}

SgStatement *GetActualSubArray_2(SgSymbol *ar, int rank, SgExpression *index_list)
{  //generating Subroutine Call:  
   //    dvmh_get_actual_subarray2_(const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...)
  SgCallStmt *call = new SgCallStmt(*fdvm[GET_ACTUAL_SUBARR_2]);  
  fmask[GET_ACTUAL_SUBARR_2] = 2;
  
  call -> addArg(*HeaderRef(ar));
  call -> addArg(*ConstRef(rank));
  AddListToList(call->expr(0),index_list);  
  return(call);
}

SgStatement *GetActualArray(SgExpression *objref)
{  //generating Subroutine Call:  
   //    dvmh_get_actual_array(dvmDesc[])
   //  or when RTS2 is used
   //    dvmh_get_actual_array2(const DvmType dvmDesc[]) 
  int fNum = INTERFACE_RTS2 ? GET_ACTUAL_ARR_2 : GET_ACTUAL_ARRAY; 
  SgCallStmt *call = new SgCallStmt(*fdvm[fNum]);  
  fmask[fNum] = 2;
  
  call -> addArg(*objref); //(*HeaderRef(ar));
  return(call);
}

SgStatement *GetActualAll()
{  //generating Subroutine Call:  
   //    dvmh_get_actual_all() 
   //  or when RTS2 is used
   //    dvmh_get_actual_all2()
  int fNum = INTERFACE_RTS2 ? GET_ACTUAL_ALL_2 : GET_ACTUAL_ALL;
  SgCallStmt *call = new SgCallStmt(*fdvm[fNum]);  
  fmask[fNum] = 2;

  return(call);
}

SgStatement *DestroyArray(SgExpression *objref)
{  //generating Subroutine Call:  
   //    dvmh_destroy_array(dvmDesc[]) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DESTROY_ARRAY]);  
  fmask[DESTROY_ARRAY] = 2;
  
  call -> addArg(*objref); //(*HeaderRef(ar));
  return(call);
}

SgStatement *DestroyScalar(SgExpression *objref)
{  //generating Subroutine Call:  
   //    dvmh_destroy_variable(addr) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DESTROY_SCALAR]);  
  fmask[DESTROY_SCALAR] = 2;
  
  call -> addArg(*objref);
  return(call);
}

SgStatement *DeleteObject_H(SgExpression *objref) 
{
//generating Subroutine Call:
//        dvmh_delete_object(ObjectRef)
 
 SgCallStmt *call = new SgCallStmt(*fdvm[DELETE_OBJECT]);
 fmask[DELETE_OBJECT] = 2;

 call->addArg(objref->copy());
 
 return(call);
}   

SgStatement *ForgetHeader(SgExpression *objref) 
{
//generating Subroutine Call:
//        dvmh_forget_header(DvmType dvmDesc[])
 
 SgCallStmt *call = new SgCallStmt(*fdvm[FORGET_HEADER]);
 fmask[FORGET_HEADER] = 2;

 call->addArg(*objref);
 
 return(call);
}   


SgStatement *ScopeStart() 
{
//generating Subroutine Call:
//        dvmh_scope_start()
 
 SgCallStmt *call = new SgCallStmt(*fdvm[SCOPE_START]);
 fmask[SCOPE_START] = 2;
 
 return(call);
}   

SgStatement *ScopeEnd() 
{
//generating Subroutine Call:
//        dvmh_scope_end()
 
 SgCallStmt *call = new SgCallStmt(*fdvm[SCOPE_END]);
 fmask[SCOPE_END] = 2;
 
 return(call);
}
 
SgStatement *ScopeInsert(SgExpression *objref) 
{
//generating Subroutine Call:
//        dvmh_scope_insert(dvmDesc[])
 
 SgCallStmt *call = new SgCallStmt(*fdvm[SCOPE_INSERT]);
 fmask[SCOPE_INSERT] = 2;
 call -> addArg(*objref); 
 return(call);
}   
  

SgStatement *DataEnter(SgExpression *objref, SgExpression *esize)
{  //generating Subroutine Call:  
   //    dvmh_data_enter(addr,size) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DATA_ENTER]);  
  fmask[DATA_ENTER] = 2;
  
  call -> addArg(*objref);
  call -> addArg(*esize);
  return(call);
}

SgStatement *DataExit(SgExpression *objref, int saveFlag)
{  //generating Subroutine Call:  
   //    dvmh_data_exit(addr,saveFlag) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DATA_EXIT]);  
  fmask[DATA_EXIT] = 2;
  
  call -> addArg(*objref);
  call -> addArg(*ConstRef(saveFlag));
  return(call);
}


SgStatement *Redistribute_H(SgExpression *objref, int new_sign)
{  //generating Subroutine Call:  
   //    dvmh_redistribute(dvmDesc[], newValueFlagRef) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_REDISTRIBUTE]);  
  fmask[DVMH_REDISTRIBUTE] = 2;
  
  call -> addArg(*objref); //(*HeaderRef(ar));
  call -> addArg(*ConstRef(new_sign));
  return(call);
}

SgStatement *Realign_H(SgExpression *objref, int new_sign)
{  //generating Subroutine Call:  
   //    dvmh_align(dvmDesc[], newValueFlagRef) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_REALIGN]);  
  fmask[DVMH_REALIGN] = 2;
  
  call -> addArg(*objref); //(*HeaderRef(ar));
  call -> addArg(*ConstRef(new_sign));
  return(call);
}


SgStatement *HandleConsistent(SgExpression *gref)
{
//generating Subroutine Call:
//                           dvmh_handle_consistent(DvmhRegionRef,DvmhConsistGroupRef) 

  SgCallStmt *call  = new SgCallStmt(*fdvm[HANDLE_CONSIST]);
  fmask[HANDLE_CONSIST] = 2;
  call->addArg(cur_region ? *DVM000(cur_region->No) : *ConstRef_F95(0));
  call->addArg(*gref);
  return(call);
}



/*
SgExpression *RegistrateLoop_GPU(int irgn,int iplp,int flag_first,int flag_last)
{ // generating function call: crtpl_gpu(region_ref, dvm_parloop_ref, flag_first, flag_last)
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CRTPL_GPU]);
  fmask[CRTPL_GPU] = 1;
  fe->addArg(*GPU000(irgn));
  fe->addArg(*DVM000(iplp));
  fe->addArg(*ConstRef(flag_first));
  fe->addArg(*ConstRef(flag_last ));
  return(fe);
}
*/
//------------------------- Parallel loop --------------------------------------------------

SgExpression *LoopCreate_H(int irgn,int iplp)
{ // generating function call: loop_create(DvmhRegionRef, dvm_loop_ref(InDvmLoop))
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOOP_CREATE]);
  fmask[LOOP_CREATE] = 1;
  if(irgn)
    fe->addArg(*DVM000(irgn));
  else
    fe->addArg(*ConstRef(0));
  if(iplp)
    fe->addArg(*DVM000(iplp));
  else
    fe->addArg(*ConstRef(0)); 
  return(fe);
}

SgExpression *LoopCreate_H2(int nloop, SgExpression *paramList)
{ // generating function call: dvmh_loop_create(const DvmType *pCurRegion, const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...)
 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOOP_CREATE_2]);
  fmask[LOOP_CREATE_2] = 1;
  fe->addArg(cur_region ? *DVM000(cur_region->No) : *ConstRef_F95(0));
  fe->addArg(*ConstRef(nloop)); 
  AddListToList(fe->lhs(),paramList);  
  return(fe);
}

SgExpression *LoopCreate_H2(SgExpression &paramList)
{ // generating function call: dvmh_loop_create(const DvmType *pCurRegion, const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...)
 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOOP_CREATE_2],paramList);
  fmask[LOOP_CREATE_2] = 1;
  
  return(fe);
}

SgStatement *LoopMap(int ilh, SgExpression *desc, int rank, SgExpression *paramList)
{ // generating subroutine call: dvmh_loop_map(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...);
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_MAP]);
  fmask[LOOP_MAP] = 2;
  call->addArg(*DVM000(ilh));
  call->addArg(*desc);
  call->addArg(*ConstRef(rank));
  AddListToList(call->expr(0),paramList);  
  return(call);
}

SgStatement *LoopMap(SgExpression &paramList)
{ // generating subroutine call: dvmh_loop_map(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pAlignmentHelper */...);
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_MAP],paramList);
  fmask[LOOP_MAP] = 2;
  
  return(call);
}

SgExpression *AlignmentLinear(SgExpression *axis,SgExpression *multiplier,SgExpression *summand)
{ // generating function call: 
  //                 DvmType dvmh_alignment_linear(const DvmType *pAxis, const DvmType *pMultiplier, const DvmType *pSummand) 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ALIGN_LINEAR]);
  fmask[ALIGN_LINEAR] = 1;
 
  fe->addArg(*DvmType_Ref(axis));
  fe->addArg(*DvmType_Ref(multiplier));
  fe->addArg(*DvmType_Ref(summand)); 
  return(fe);
}

SgExpression *Register_Array_H2(SgExpression *ehead)
{ // generating function call: : DvmType dvmh_register_array(DvmType dvmDesc[])
  // DvmDesc -  dvm-array header
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[REGISTER_ARR]);
  fmask[REGISTER_ARR] = 1;
  fe->addArg(*ehead);
  return(fe);
}

SgStatement *LoopStart_H(int il)
{ // generating subroutine call: loop_start(DvmhLoopRef)
  // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_START]);
  fmask[LOOP_START] = 2;
  call->addArg(*DVM000(il));
  return(call);
}

SgStatement *LoopEnd_H(int il)
{ // generating subroutine call: loop_end(DvmhLoopRef)
  // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_END]);
  fmask[LOOP_END] = 2;
  call->addArg(*DVM000(il));
  return(call);
}

SgStatement *LoopPerform_H(int il)
{ // generating subroutine call: loop_perform(DvmhLoopRef)
  // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_PERFORM]);
  fmask[LOOP_PERFORM] = 2;
  call->addArg(*DVM000(il));
  return(call);
}

SgStatement *LoopPerform_H2(int il)
{ // generating subroutine call: dvmh_loop_perform(DvmhLoopRef)
  // DvmhLoopRef - result of dvmh_loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_PERFORM_2]);
  fmask[LOOP_PERFORM_2] = 2;
  call->addArg(*DVM000(il));
  return(call);
}

SgStatement *RegisterHandler_H(int il,SgSymbol *dev_const, SgExpression *flag, SgSymbol *sfun,int bcount,int parcount)
{ // generating subroutine call: loop_register_handler(DvmhLoopRef,deviceTypeRef,flagsRef,FuncRef,basesCount,paramCount,Params...)
  // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[REG_HANDLER]);
  fmask[REG_HANDLER] = 2;
  call->addArg(*DVM000(il));
  call->addArg(* new SgVarRefExp(dev_const));
  call->addArg(* flag);  
  call->addArg(* new SgVarRefExp(sfun));
  call->addArg(* ConstRef(bcount));
  call->addArg(* ConstRef(parcount));
  return(call);
}

SgStatement *RegisterHandler_H2(int il,SgSymbol *dev_const, SgExpression *flag, SgExpression *efun)
{ // generating subroutine call: dvmh_loop_register_handler(const DvmType *pCurLoop, const DvmType *pDeviceType, const DvmType *pHandlerType, const DvmType *pHandlerHelper)

  // DvmhLoopRef - result of dvmh_loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[REG_HANDLER_2]);
  fmask[REG_HANDLER_2] = 2;
  call->addArg(*DVM000(il));
  call->addArg(* new SgVarRefExp(dev_const));
  call->addArg(* flag);  
  call->addArg(* efun);
  return(call);
}

SgExpression *HandlerFunc(SgSymbol *sfun, int paramCount, SgExpression *arg_list)
{ // generating function call:
  //               DvmType dvmh_handler_func(DvmHandlerFunc handlerFunc, const DvmType *pCustomParamCount, /* void *param */...)
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[HANDLER_FUNC]);
  fmask[HANDLER_FUNC] = 1;
  fe->addArg(* new SgVarRefExp(sfun));
  fe->addArg(* ConstRef(paramCount));
  AddListToList(fe->lhs(), arg_list);
  return(fe);
}

/*
SgExpression *Loop_GPU(int il)
{ // generating function call: startpl_gpu(gpu_parloop_ref)
  // gpu_parloop_ref - result of crtpl_gpu()
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[LOOP_GPU]);
  fmask[LOOP_GPU] = 1;
  fe->addArg(*GPU000(il));
  fe->addArg(*new SgVarRefExp(s_blocks));
  fe->addArg(*new SgVarRefExp(s_threads));
  fe->addArg(*new SgArrayRefExp(*baseGpuMemory(IndexType())));
  fe->addArg(*new SgVarRefExp(s_blocks_off));
  return(fe);
}
*/
/*
SgExpression *StartShadow_GPU(int irgn,SgExpression *gref)
{ // generating function call: strtsh_gpu(ComputeRegionRef, BoundGroupRef)
  SgFunctionCallExp *fe= new SgFunctionCallExp(*fdvm[STRTSH_GPU]);
  fmask[STRTSH_GPU] = 1;
  fe->addArg(*GPU000(irgn));
  fe->addArg(gref->copy());
  return(fe);
}
*/

SgExpression *GetActualEdges_H(SgExpression *gref)
{ // generating function call: dvmh_get_actual_edges(ShadowGroupRef)
  SgFunctionCallExp *fe= new SgFunctionCallExp(*fdvm[GET_ACTUAL_EDGES]);
  fmask[GET_ACTUAL_EDGES] = 1;
 
  fe->addArg(gref->copy());
  return(fe);
}

/*
SgStatement *DoneShadow_GPU(int ish)
{// generating subroutine call: donesh_gpu(gpu_ShagowRef) 
 // gpu_ShagowRef - result of strtsh_gpu() 
  SgCallStmt *call = new SgCallStmt(*fdvm[DONESH_GPU]);
  fmask[DONESH_GPU] = 2;
  call->addArg(*GPU000(ish));
  return(call);
}
*/

SgStatement *SetCudaBlock_H(int il, int ib)
{// generating subroutine call: loop_set_cuda_block(DvmhLoopRef,XRef,YRef,ZRef) 
  // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[CUDA_BLOCK]);
  fmask[CUDA_BLOCK] = 2;
  call->addArg(*DVM000(il));
  call->addArg(*DVM000(ib));
  call->addArg(*DVM000(ib+1));
  call->addArg(*DVM000(ib+2));
  return(call);
}

SgStatement *SetCudaBlock_H2(int il, SgExpression *X, SgExpression *Y, SgExpression *Z )
{// generating subroutine call: dvmh_loop_set_cuda_block(DvmhLoopRef,XRef,YRef,ZRef) 
  // DvmhLoopRef - result of dvmh_loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[CUDA_BLOCK_2]);
  fmask[CUDA_BLOCK_2] = 2;
  call->addArg(*DVM000(il));
  call->addArg(*DvmType_Ref(X));
  call->addArg(*DvmType_Ref(Y));
  call->addArg(*DvmType_Ref(Z));
  return(call);
}

SgStatement *Correspondence_H (int il, SgExpression *hedr, SgExpression *axis_list)
{// generating subroutine call: dvmh_loop_array_correspondence(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pLoopAxis */...) 
 // DvmhLoopRef - result of dvmh_loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[CORRESPONDENCE]);
  fmask[CORRESPONDENCE] = 2;
  call->addArg(*DVM000(il));
  call->addArg(*hedr);
  AddListToList(call->expr(0), axis_list);
  return(call);
}

SgStatement *ShadowRenew_H(SgExpression *gref)
{// generating subroutine call: dvmh_shadow_renew(ShadowGroupRef) 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[SHADOW_RENEW]);
  fmask[SHADOW_RENEW] = 2;

  call->addArg(gref->copy());
  return(call);
}

SgStatement *ShadowRenew_H2(SgExpression *head,int corner,int rank,SgExpression *shlist)
{// generating subroutine call: 
 //      dvmh_shadow_renew2(const DvmType dvmDesc[], const DvmType *pCornerFlag, const DvmType *pSpecifiedRank,
 //                        /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);
  
  SgCallStmt *call = new SgCallStmt(*fdvm[SHADOW_RENEW_2]);
  fmask[SHADOW_RENEW_2] = 2;

  call->addArg(*head);
  call->addArg(*ConstRef(corner));
  call->addArg(*ConstRef(rank));
  AddListToList(call->expr(0),shlist);
  return(call);
}


SgStatement *IndirectShadowRenew(SgExpression *head, int axis, SgExpression *shadow_name)
{// generating subroutine call:
 //      dvmh_indirect_shadow_renew_(const DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pShadowNameStr); 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[INDIRECT_SH_RENEW]);
  fmask[INDIRECT_SH_RENEW] = 2;

  call->addArg(*head);
  call->addArg(*ConstRef(axis));
  call->addArg(*DvmhString(shadow_name));    //DvmhString(new SgValueExp(name))
  return(call);
}

SgStatement *LoopShadowCompute_H(int il,SgExpression *headref)
{  //generating subroutine call:  loop_shadow_compute(DvmhLoopRef,dvmDesc[]) 
   // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[SHADOW_COMPUTE]);  
  fmask[SHADOW_COMPUTE] = 2;
  
  call -> addArg(*DVM000(il));
  call -> addArg(*headref);               //(*HeaderRef(ar));
  
  return(call);
}

SgStatement *LoopShadowCompute_Array(int il,SgExpression *headref)
{  //generating subroutine call:  dvmh_loop_shadow_compute_array(const DvmType *pCurLoop, const DvmType dvmDesc[])
   // DvmhLoopRef - result of dvmh_loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[SHADOW_COMPUTE_AR]);  
  fmask[SHADOW_COMPUTE_AR] = 2;
  
  call -> addArg(*DVM000(il));
  call -> addArg(*headref);               
  
  return(call);
}

SgStatement *ShadowCompute(int ilh,SgExpression *head,int rank,SgExpression *shlist)
{// generating subroutine call: 
 //      dvmh_loop_shadow_compute(const DvmType *pCurLoop, const DvmType templDesc[], const DvmType *pSpecifiedRank,
 //                               /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...);
 // DvmhLoopRef - result of dvmh_loop_create()  
  SgCallStmt *call = new SgCallStmt(*fdvm[SHADOW_COMPUTE_2]);
  fmask[SHADOW_COMPUTE_2] = 2;

  call->addArg(*DVM000(ilh));
  call->addArg(*head);
  call->addArg(*ConstRef(rank));
  AddListToList(call->expr(0),shlist);
  return(call);
}

SgStatement *LoopAcross_H(int il,SgExpression *oldGroup,SgExpression *newGroup)
{  //generating subroutine call:  loop_across(DvmhLoopRef *InDvmhLoop, ShadowGroupRef *oldGroup, ShadowGroupRef *newGroup) 
   // DvmhLoopRef - result of loop_create()
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_ACROSS]);  
  fmask[LOOP_ACROSS] = 2;
  
  call -> addArg(*DVM000(il));
  call -> addArg(*oldGroup);
  call -> addArg(*newGroup);               
  
  return(call);
}

SgStatement *LoopAcross_H2(int il, int isOut, SgExpression *headref, int rank, SgExpression *shlist)
{  //generating subroutine call:  
   //            dvmh_loop_across(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pShadowLow, const DvmType *pShadowHigh */...)

  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_ACROSS_2]);  
  fmask[LOOP_ACROSS_2] = 2;
  
  call -> addArg(*DVM000(il));
  call -> addArg(*ConstRef(isOut));
  call -> addArg(*headref);
  call -> addArg(*ConstRef(rank));               
  AddListToList(call->expr(0),shlist);
  return(call);
}

SgExpression *GetStage(SgStatement *first_do,int iplp)
{// generating function call: dvmh_get_next_stage(LineNumber,FileName,LoopRef,DvmhRegionRef)
 // Loopref - result of crtpl()
 SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[GET_STAGE]);
 fmask[GET_STAGE] = 1;
 filename_list *fn = AddToFileNameList(baseFileName(first_do->fileName()));
 fe->addArg(cur_region ? *DVM000(cur_region->No) : *ConstRef_F95(0));
 fe->addArg(*DVM000(iplp));
 fe->addArg(*ConstRef_F95(first_do->lineNumber()));
 fe->addArg(* new SgVarRefExp(fn->fns));
 
 return(fe);
}


/*
SgStatement *EndHostExec_GPU(int il)
{// generating subroutine call: end_host_exec_gpu(gpu_parloop_ref) 
 // gpu_parloop_ref - result of crtpl_gpu() 
  SgCallStmt *call = new SgCallStmt(*fdvm[ENDHOST_GPU]);
  fmask[ENDHOST_GPU] = 2;
  call->addArg(*GPU000(il));
  return(call);
}
*/

SgStatement *CallKernel_GPU(SgSymbol *skernel, SgExpression *blosks_threads)
{// generating Kernel Call:  
 // loop_<file_name>_<loopNo>(InDeviceBaseAddr1,...,InDeviceBaseAddrN,<coeffs_for_arrays>,<uses_vars>, blocks_off) 

 // SgExpression *gpubase;
  
  SgCallStmt *call = new SgCallStmt(*skernel);  

  call->setExpression(1,*blosks_threads);
  //gpubase = new SgArrayRefExp(*baseGpuMemory(ar->type()->baseType()));
  //call -> addArg(*new SgVarRefExp(s_blocks_off));

  call ->setVariant(ACC_CALL_STMT);
  return(call);
}

/*
SgStatement *InsertRed_GPU(int il,int irv,SgExpression *base,SgExpression *loc_base,SgExpression *offset,SgExpression *loc_offset)
{// generating subroutine call: insred_gpu_(gpu_parloop_ref, InRedRefPtr, InDeviceArrayBaseAddr, InDeviceLocBaseAddr, AddrType* ArrayOffsetPtr, AddrType *LocOffsetPtr) 
 // InRedRefPtr - result of crtrdf() 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[INSRED_GPU]);  
  fmask[INSRED_GPU] = 2;
  call -> addArg(*GPU000(il));
  call -> addArg(*DVM000(irv));
  call -> addArg(*base);
  if(loc_base)
    call -> addArg(*loc_base);
  else
    call -> addArg(*ConstRef(0));  
  call -> addArg(*GetAddresMem(offset));
  if(loc_offset)
    call -> addArg(*GetAddresMem(loc_offset));
  else
    call -> addArg(*ConstRef(0));  
  return(call);
}
*/

SgStatement *LoopInsertReduction_H(int ilh, int irv)
{// generating subroutine call: loop_insred(DvmhLoopRef, InRedRefPtr) 
 // InRedRefPtr  - result of crtrdf()
 // DvmhLoopRef - result of loop_create() 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[LOOP_INSRED]);  
  fmask[LOOP_INSRED] = 2;
  call -> addArg(*DVM000(ilh));
  call -> addArg(*DVM000(irv));
  return(call);
}

/*
SgStatement *UpdateDVMArrayOnHost(SgSymbol *s)
{
 // generating subroutine call: dvmh_get_actual_whole_(long InOutDvmArray[]) 
 //InOutDvmArray[] - DVM-array header of array 's' 
  SgCallStmt *call = new SgCallStmt(*fdvm[GET_ACTUAL_WHOLE]);
  fmask[GET_ACTUAL_WHOLE] = 2;
  call->addArg(*HeaderRef(s));
  return(call);
}
*/

//--------- Array Copy ----------------------------------------------------------------

SgExpression *DvmhArraySlice(int rank, SgExpression *slice_list)
{
 // generating function call: 
 //        DvmType dvmh_array_slice_C(DvmType rank, /* DvmType start, DvmType end, DvmType step */...)

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[ARRAY_SLICE]);
  fmask[ARRAY_SLICE] = 1;
  fe->addArg(*ConstRef_F95(rank));
  AddListToList(fe->lhs(), slice_list); //fe->lhs()->setRhs(slice_list); 
  return(fe);
}

SgStatement *DvmhArrayCopy( SgExpression *array_header_right, int rank_right, SgExpression *slice_list_right, SgExpression *array_header_left, int rank_left, SgExpression *slice_list_left )
{
 // generating subroutine call: 
 // dvmh_array_copy (const DvmType srcDvmDesc[], DvmType *pSrcSliceHelper, DvmType dstDvmDesc[], DvmType *pDstSliceHelper)
  
  SgCallStmt *call = new SgCallStmt(*fdvm[COPY_ARRAY]);
  fmask[COPY_ARRAY] = 2;
  call->addArg(*array_header_right);
  call->addArg(*DvmhArraySlice(rank_right, slice_list_right));
  call->addArg(*array_header_left);
  call->addArg(*DvmhArraySlice(rank_left,  slice_list_left));
  return(call);
}

SgStatement *DvmhArrayCopyWhole( SgExpression *array_header_right, SgExpression *array_header_left )
{
 // generating subroutine call: 
 //                      dvmh_array_copy_whole(const DvmType srcDvmDesc[], DvmType dstDvmDesc[])
  
  SgCallStmt *call = new SgCallStmt(*fdvm[COPY_WHOLE]);
  fmask[COPY_WHOLE] = 2;
  call->addArg(*array_header_right);
  call->addArg(*array_header_left);
  return(call);
}

// -------- Distributed array creation ------------------------------------------------

SgStatement *DvmhArrayCreate(SgSymbol *das, SgExpression *array_header, int rank, SgExpression *arglist)
{ 
 // generating subroutine call:
 //    dvmh_array_create(DvmType dvmDesc[], const void *baseAddr, const DvmType *pRank, const DvmType *pTypeSize,
 //                       \* const DvmType *pSpaceLow, const DvmType *pSpaceHigh, const DvmType *pShadowLow, const DvmType *pShadowHigh *\...)
  
  SgCallStmt *call = new SgCallStmt(*fdvm[CREATE_ARRAY]);
  fmask[CREATE_ARRAY] = 2;
  loc_distr =1;

  call->addArg(*array_header);    //(*HeaderRef(das));
  SgType *t = IS_POINTER(das) ? PointerType(das) : (das->type())->baseType();
  SgExpression *base = (t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING ) ? new SgArrayRefExp(*baseMemory(SgTypeInt())) : new SgArrayRefExp(*baseMemory(t));  
  call->addArg(*base); //Base
  call->addArg(*ConstRef(rank)); //Rank
     //int it = TestType_RTS2(t);
     //SgExpression *ts = it >= 0 ? &SgUMinusOp(*ConstRef(it)) : ConstRef_F95(TypeSize(t));
     //call->addArg(*ts); //TypeSize 
                               //(*ConstRef_F95(TypeSize(t)));
  call->addArg(*TypeSize_RTS2(t)); 
  AddListToList(call->expr(0),arglist);  
  return(call);
}

SgStatement *DvmhTemplateCreate(SgSymbol *das, SgExpression *array_header, int rank, SgExpression *arglist)
{ 
 // generating subroutine call:
 //    dvmh_template_create(DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);
  SgCallStmt *call = new SgCallStmt(*fdvm[CREATE_TEMPLATE]);
  fmask[CREATE_TEMPLATE] = 2;
  loc_distr = 1;

  call->addArg(*array_header);    //(*HeaderRef(das));
  call->addArg(*ConstRef(rank)); //Rank
  AddListToList(call->expr(0),arglist);  
  return(call);
}

SgExpression *VarGenHeader(SgExpression *item)
{
  // generates function call:
  //                   dvmh_variable_gen_header(const void *addr, const DvmType *pRank, const DvmType *pTypeSize,
  //                                          \* const DvmType *pSpaceLow, const DvmType *pSpaceHigh \*...)
  
  //	dvmh_variable_gen_header(C, 0_8, int(-rt_FLOAT, 8)) for scalar variables
  //	dvmh_variable_gen_header(B, 2_8, int(-rt_FLOAT, 8), 1_8, 30_8, 1_8, 40_8) for array of size 40*30
  
  fmask[VAR_GEN_HDR] = 1;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[VAR_GEN_HDR]);
  fe->addArg(*item);
  
  int nsubs;
  if (item->symbol() && isSgArrayType(item->symbol()->type()))
    nsubs = isSgArrayType(item->symbol()->type())->dimension();
  else nsubs = 0;
  fe->addArg(*ConstRef_F95(nsubs));
  
  // fe->addArg(*TypeSize_RTS2(item->symbol()->type()));
  
  if (item->symbol()) fe->addArg(*TypeSize_RTS2(item->symbol()->type()));
  else fe->addArg(*TypeSize_RTS2(item->type())); // array expressions don't have symbol
  
  if (nsubs) {
    for (int i = nsubs-1; i >= 0; --i) {
      fe->addArg(*DvmType_Ref(LowerBound(item->symbol(), i)));
      fe->addArg(*DvmType_Ref(UpperBound(item->symbol(), i)));
    }
  }
  
  return fe;
  
}

SgStatement *CreateDvmArrayHeader_2(SgSymbol *ar, SgExpression *array_header,  int rank,  SgExpression *shape_list) 
{
// creates subroutine call:
//       dvmh_variable_fill_header(DvmType dvmDesc[], const void *baseAddr, const void *addr, const DvmType *pRank, const DvmType *pTypeSize,/* const DvmType *pSpaceLow, const DvmType *pSpaceHigh */...);
           
  SgCallStmt *call = new SgCallStmt(*fdvm[VAR_FILL_HDR]);
  fmask[VAR_FILL_HDR] = 2;

  call->addArg(*array_header);
  SgType *t = (isSgArrayType(ar->type())) ? ar->type()->baseType() : ar->type();
  SgExpression *base = (t->variant() != T_DERIVED_TYPE && t->variant() != T_STRING ) ? new SgArrayRefExp(*baseMemory(SgTypeInt())) : new SgArrayRefExp(*baseMemory(t));   
  call->addArg(*base);
  call->addArg(*new SgArrayRefExp(*ar));  
  call->addArg(*ConstRef(rank)); 
  call->addArg(*TypeSize_RTS2(t)); 
  AddListToList(call->expr(0),shape_list);  
  return(call);
} 

SgExpression *DvmhReplicated()
{
  // generates function call:     DvmType dvmh_distribution_replicated()

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_REPLICATED]);
  fmask[DVMH_REPLICATED] = 1;  
  return fe;  

}

SgExpression *DvmhBlock(int axis)
{
  // generates function call:     DvmType dvmh_distribution_block(DvmType pMpsAxis)

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_BLOCK]);
  fmask[DVMH_BLOCK] = 1;
  fe->addArg(*ConstRef(axis));  
  return fe;  

}

SgExpression *DvmhWgtBlock(int axis, SgSymbol *sw, SgExpression *en)
{
  // generates function call:
  //       DvmType dvmh_distribution_wgtblock(DvmType pMpsAxis, const DvmType *pElemType, const void *arrayAddr, const DvmType *pElemCount)

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_WGTBLOCK]);
  fmask[DVMH_WGTBLOCK] = 1;
  SgType *t = (isSgArrayType(sw->type())) ? sw->type()->baseType() : sw->type();
  fe->addArg(*ConstRef(axis));  
  fe->addArg(*ConstRef( TestType_RTS2(t) ));
  fe->addArg(*new SgArrayRefExp(*sw));
  fe->addArg(*en);    //DvmType_Ref(en)
  return fe;  

}


SgExpression *DvmhGenBlock(int axis, SgSymbol *sg)
{
  // generates function call:
  //    DvmType dvmh_distribution_genblock(DvmType pMpsAxis, const DvmType *pElemType, const void *arrayAddr)
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_GENBLOCK]);
  fmask[DVMH_GENBLOCK] = 1;
  SgType *t = (isSgArrayType(sg->type())) ? sg->type()->baseType() : sg->type();
  fe->addArg(*ConstRef(axis));  
  fe->addArg(*ConstRef( TestType_RTS2(t)));
  fe->addArg(*new SgArrayRefExp(*sg));
  return fe;  

}

SgExpression *DvmhMultBlock(int axis, SgExpression *em)
{
  // generates function call:  DvmType dvmh_distribution_multblock(DvmType pMpsAxis, const DvmType *pMultBlock)

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_MULTBLOCK]);
  fmask[DVMH_MULTBLOCK] = 1;
  fe->addArg(*ConstRef(axis));  
  fe->addArg(*em); // *DvmType_Ref(em));
  
  return fe;  

}

#define rt_UNKNOWN (-1)  /*RTS2*/

SgExpression *DvmhIndirect(int axis, SgSymbol *smap)
{
  // generates function call:
  //      DvmType dvmh_distribution_indirect(DvmType pMpsAxis, const DvmType *pElemType, const void *arrayAddr)

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_INDIRECT]);
  fmask[DVMH_INDIRECT] = 1;
  SgType *t = (isSgArrayType(smap->type())) ? smap->type()->baseType() : smap->type();
  fe->addArg(*ConstRef(axis));  
  fe->addArg(HEADER(smap) ? *SignConstRef(rt_UNKNOWN) : *ConstRef( TestType_RTS2(t)));
  fe->addArg(*new SgArrayRefExp(*smap));
  
  return fe;  

}

SgExpression *DvmhDerived(int axis, SgExpression *derived_rhs, SgExpression *counter_func, SgExpression *filler_func)
{ //generating function call:
  //      DvmType dvmh_distribution_derived(DvmType pMpsAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper, const DvmType *pFillingHandlerHelper)  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DVMH_DERIVED]);
  fmask[DVMH_DERIVED] = 1;
  fe->addArg(*ConstRef(axis));    
  fe->addArg(*derived_rhs);
  fe->addArg(*counter_func);
  fe->addArg(*filler_func);
  return fe;
}

SgStatement *DvmhDistribute(SgSymbol *das, int rank, SgExpression *distr_list)
{
 // generating subroutine call:
 //    dvmh_distribute(DvmType dvmDesc[], const DvmType *pRank, 
 //   \* const DvmType *pDistributionHelper *\...);

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_DISTRIBUTE]);
  fmask[DVMH_DISTRIBUTE] = 2;

  call->addArg(*HeaderRef(das));
  call->addArg(*ConstRef_F95(rank));
  AddListToList(call->expr(0),distr_list);
  return(call);
}


SgStatement *DvmhRedistribute(SgSymbol *das, int rank, SgExpression *distr_list)
{
 // generating subroutine call:
 //    dvmh_redistribute2(DvmType dvmDesc[], const DvmType *pRank, 
 //   \* const DvmType *pDistributionHelper *\...);

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_REDISTR_2]);
  fmask[DVMH_REDISTR_2] = 2;

  call->addArg(*HeaderRef(das));
  call->addArg(*ConstRef_F95(rank));
  AddListToList(call->expr(0),distr_list);
  return(call);
}


SgStatement *DvmhAlign(SgSymbol *als, SgSymbol *align_base, int nr, SgExpression *alignment_list)
{
 // generating subroutine call:
 //                   dvmh_align(DvmType dvmDesc[], const DvmType templDesc[], const DvmType *pTemplRank, 
 //                              \* const DvmType *pAlignmentHelper *\...)

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_ALIGN]);
  fmask[DVMH_ALIGN] = 2;

  call->addArg(*HeaderRef(als));
  call->addArg(*HeaderRef(align_base));
  call->addArg(*ConstRef(nr));  //addArg(*ConstRef_F95(Rank(align_base)));
  AddListToList(call->expr(0),alignment_list);
  return(call);
}

SgStatement *DvmhRealign(SgExpression *objref, int new_sign, SgExpression *pattern_ref, int nr, SgExpression *align_list)
{  //generating Subroutine Call:  
   //    dvmh_realign2(dvmDesc[], newValueFlagRef) 

  SgCallStmt *call = new SgCallStmt(*fdvm[DVMH_REALIGN_2]);  
  fmask[DVMH_REALIGN_2] = 2;
  
  call->addArg(*objref); 
  call->addArg(*ConstRef(new_sign));
  call->addArg(*pattern_ref); 
  call->addArg(*ConstRef(nr));
  AddListToList(call->expr(0),align_list);   
  return(call);
}

SgStatement *IndirectLocalize(SgExpression *ref_array, SgExpression *target_array, int iaxis)
{  //generating Subroutine Call:  
   //    dvmh_indirect_localize (const DvmType refDvmDesc[], const DvmType targetDvmDesc[], const DvmType *pTargetAxis) 

  SgCallStmt *call = new SgCallStmt(*fdvm[LOCALIZE]);  
  fmask[LOCALIZE] = 2;
  
  call->addArg(*ref_array); 
  call->addArg(*target_array); 
  call->addArg(*ConstRef_F95(iaxis));   
  return(call);
}

SgStatement *ShadowAdd(SgExpression *templ, int iaxis, SgExpression *derived_rhs, SgExpression *counter_func, SgExpression *filler_func, SgExpression *shadow_name, int nl, SgExpression *array_list)
{  //generating Subroutine Call:
   // dvmh_indirect_shadow_add (DvmType dvmDesc[], const DvmType *pAxis, const DvmType *pDerivedRhsHelper, const DvmType *pCountingHandlerHelper,
   //     const DvmType *pFillingHandlerHelper, const DvmType *pShadowNameStr, const DvmType *pIncludeCount, /* DvmType dvmDesc[] */...);
  
  SgCallStmt *call = new SgCallStmt(*fdvm[SHADOW_ADD]);  
  fmask[SHADOW_ADD] = 2;
  
  call->addArg(*templ);
  call->addArg(*ConstRef_F95(iaxis)); 
  call->addArg(*derived_rhs);
  call->addArg(*counter_func);
  call->addArg(*filler_func);
  call->addArg(*DvmhString(shadow_name)); 
  call->addArg(*ConstRef_F95(nl));
  AddListToList(call->expr(0),array_list);     
  return(call);
}

SgExpression *DvmhExprIgnore()
{
  // generates function call:  dvmh_derived_rhs_expr_ignore()      

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[EXPR_IGNORE]);
  fmask[EXPR_IGNORE] = 1;
  return fe;  
}

SgExpression *DvmhExprConstant(SgExpression *e)
{
  // generates function call:  dvmh_derived_rhs_expr_constant()      

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[EXPR_CONSTANT]);
  fmask[EXPR_CONSTANT] = 1;
  fe->addArg(*DvmType_Ref(e));
  return fe;  
}

SgExpression *DvmhExprScan(SgExpression *edummy)
{
  // generates function call:  dvmh_derived_rhs_expr_scan(const DvmType *pShadowCount, /* const DvmType *pShadowNameStr */...)      

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[EXPR_SCAN]);
  fmask[EXPR_SCAN] = 1;
  SgExpression *el = edummy->lhs();
  SgExpression *eln= NULL;
  int nsh=0;
  for(;el;el=el->rhs(),nsh++)
     eln = AddElementToList(eln,DvmhString(el->lhs()));
  fe->addArg(*ConstRef_F95(nsh));
  fe->lhs()->setRhs(eln);
  return fe;  
}

SgExpression *DvmhDerivedRhs(SgExpression *erhs)
{
  // generates function call: 
  //   dvmh_derived_rhs(const DvmType templDesc[], const DvmType *pTemplRank, /* const DvmType *pDerivedRhsExprHelper */...);      

  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DERIVED_RHS]);
  fmask[DERIVED_RHS] = 1;
  fe->addArg(*HeaderRef(erhs->symbol()));
  SgExpression *el,*e,*eln=NULL;
  int nr=0;
  for(el=erhs->lhs();el;el=el->rhs(),nr++)
  {
     if(isSgKeywordValExp(el->lhs()))           // "*"
        e = DvmhExprIgnore();
     else if(el->lhs()->variant() == DUMMY_REF) // @align-dummy[ + shadow-name ]...
        e = DvmhExprScan(el->lhs());
     else                                       // int_expr
        e = DvmhExprConstant(el->lhs());
     eln = AddElementToList(eln,e);
  }
  fe->addArg(*ConstRef_F95(nr));
  AddListToList(fe->lhs(),eln);  
  return fe;  
}

// -------  Input/Output --------------------------------------------------------------

SgExpression *DvmhConnected(SgExpression *unit, SgExpression *failIfYes) 
{
  // generates function call:
  //               dvmh_ftn_connected(const DvmType *pUnit, const DvmType *pFailIfYes)
  
  fmask[FTN_CONNECTED] = 1;
  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[FTN_CONNECTED]);
  fe->addArg(*unit);
  fe->addArg(*failIfYes);
  
  return fe;  
}

//------ Calls from HOST-procedure(host-handler) for parallel loop --------------------

SgStatement *LoopFillBounds_HH(SgSymbol *loop_s, SgSymbol *sBlow,SgSymbol *sBhigh,SgSymbol *sBstep)
{// generating subroutine call: loop_fill_bounds(DvmhLoopRef, lowIndex[],highIndex[],stepIndex[]) 
 // DvmhLoopRef - result of loop_create() 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[FILL_BOUNDS]);  
  //fmask[FILL_BOUNDS] = 2;
  call -> addArg(*new SgVarRefExp(loop_s));
  call -> addArg(* new SgArrayRefExp(*sBlow, *new SgValueExp(1)));
  call -> addArg(* new SgArrayRefExp(*sBhigh,*new SgValueExp(1)));
  call -> addArg(* new SgArrayRefExp(*sBstep,*new SgValueExp(1)));
  return(call);
}

SgStatement *LoopRedInit_HH(SgSymbol *loop_s, int nred, SgSymbol *sRed,SgSymbol *sLoc)
{// generating subroutine call: loop_red_init(DvmhLoopRef *InDvmhLoop, DvmType *InRedNum, void *arrayPtr, void *locPtr) 
 // DvmhLoopRef - result of loop_create() 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[RED_INIT]);  
  //fmask[RED_INIT] = 2;
  call -> addArg(*new SgVarRefExp(loop_s));
  call -> addArg(*ConstRef_F95(nred)); 
  call -> addArg(* new SgVarRefExp(*sRed));
  if(sLoc)
  {  if(isSgArrayType(sLoc->type())) 
       call -> addArg(*FirstArrayElement(sLoc)); //(* new SgArrayRefExp(*sLoc));
     else
       call -> addArg(*new SgVarRefExp(sLoc));
  }
  else                  
    call -> addArg(*ConstRef_F95(0));  
  return(call);
}

SgStatement *LoopRedPost_HH(SgSymbol *loop_s, int nred, SgSymbol *sRed,SgSymbol *sLoc)
{// generating subroutine call: loop_red_post(DvmhLoopRef *InDvmhLoop, DvmType *InRedNum, void *arrayPtr, void *locPtr) 
 // DvmhLoopRef - result of loop_create() 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[RED_POST]);  
  //fmask[RED_POST] = 2;
  call -> addArg(*new SgVarRefExp(loop_s));
  call -> addArg(*ConstRef_F95(nred)); 
  call -> addArg(* new SgVarRefExp(*sRed));
  if(sLoc)
  {  if(isSgArrayType(sLoc->type())) 
       call -> addArg(*FirstArrayElement(sLoc)); //(* new SgArrayRefExp(*sLoc));
     else
       call -> addArg(*new SgVarRefExp(sLoc));
  }
  else
    call -> addArg(*ConstRef_F95(0));  
  return(call);
}

SgExpression *LoopGetSlotCount_HH(SgSymbol *loop_s)
{// generating function call: loop_get_slot_count(DvmhLoopRef *InDvmhLoop) 
 // DvmhLoopRef - result of loop_create() 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[SLOT_COUNT]);
  //fmask[SLOT_COUNT] = 1;  
  fe -> addArg(*new SgVarRefExp(loop_s));
  return(fe);
}

SgStatement *FillLocalPart_HH(SgSymbol *loop_s, SgSymbol *shead, SgSymbol *spart)
{// generating subroutine call: loop_fill_local_part(DvmhLoopRef *InDvmhLoop, long dvmDesc[], IndexType part[])
 
 // DvmhLoopRef - result of loop_create() 
  
  SgCallStmt *call = new SgCallStmt(*fdvm[FILL_LOCAL_PART]);  
  
  call -> addArg(*new SgVarRefExp(loop_s));
  call -> addArg(* new SgArrayRefExp(*shead, *new SgValueExp(1)));
  call -> addArg(* new SgArrayRefExp(*spart, *new SgValueExp(1)));
  return(call);
}


//------ Calls from handlers for sequence of statements --------------------

SgExpression *HasLocalElement(SgSymbol *s_loop_ref,SgSymbol *ar, SgSymbol *IndAr)
{ // generating function call:
  //                    loop_has_element(DvmhLoopRef *InDvmhLoop, long dvmDesc[], long indexArray[]);
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[HAS_ELEMENT]);
  fmask[HAS_ELEMENT] = 1;
  if(!s_loop_ref)
     s_loop_ref = loop_ref_symb;
  fe->addArg(* new SgVarRefExp(s_loop_ref));
                         //if(HEADER(ar)) //DVM-array
  fe-> addArg(*HeaderRef(ar));

                         //else // replicated array
                         // call -> addArg(*DVM000(*HEADER_OF_REPLICATED(ar)));

  fe->addArg(* new SgArrayRefExp(*IndAr));
  return(fe);

}

SgExpression *HasLocalElement_H2(SgSymbol *s_loop_ref, SgSymbol*ar, int n, SgExpression *index_list)
{ // generating function call:
  //                    dvmh_loop_has_element_(const DvmType *pCurLoop, const DvmType dvmDesc[], const DvmType *pRank, /* const DvmType *pIndex */...);
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[HAS_ELEMENT_2]);
  fmask[HAS_ELEMENT_2] = 1;
  if(!s_loop_ref)
     s_loop_ref = loop_ref_symb;
  fe->addArg(* new SgVarRefExp(s_loop_ref));                         
  fe-> addArg(*HeaderRef(ar));
  fe->addArg(*ConstRef_F95(n));
  AddListToList(fe->lhs(),index_list);  
  
  return(fe);

}

// ------ Calls from Adapter/Cuda-Handler (C Language) --------------------------------------------------------------

SgExpression *GetNaturalBase(SgSymbol *s_cur_dev,SgSymbol *shead)
{ // generating function call: dvmh_get_natural_base (DvmType *deviceRef, DvmType dvmDesc[])
  // or 
  //                           dvmh_get_natural_base_C(DvmType deviceNum, const DvmType dvmDesc[])

  int fNum = INTERFACE_RTS2 ? GET_BASE_C : GET_BASE;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(* new SgVarRefExp(s_cur_dev));
  else
     fe->addArg(SgAddrOp(* new SgVarRefExp(s_cur_dev)));
  fe->addArg(* new SgArrayRefExp(*shead));
  return(fe);
}

SgExpression *GetDeviceAddr(SgSymbol *s_cur_dev,SgSymbol *s_var)
{ // generating function call: dvmh_get_device_addr (DvmType *deviceRef, void *variable)
  // or when RTS2 is used
  //                           dvmh_get_device_addr_C(DvmType deviceNum, const void *addr);

  int fNum = INTERFACE_RTS2 ? GET_DEVICE_ADDR_C : GET_DEVICE_ADDR ;  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(*new SgVarRefExp(s_cur_dev));
  else
     fe->addArg(SgAddrOp(*new SgVarRefExp(s_cur_dev)));
  fe->addArg(*new SgVarRefExp(*s_var));
  return(fe);
}

SgExpression *FillHeader(SgSymbol *s_cur_dev,SgSymbol *sbase,SgSymbol *shead,SgSymbol *sgpuhead)
{ // generating function call: dvmh_fill_header_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[])
  // or when RTS2 is used
  //                   DvmType dvmh_fill_header2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[]);     

  int fNum = INTERFACE_RTS2 ? FILL_HEADER_2 : FILL_HEADER ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  
  fe->addArg(SgAddrOp(*new SgVarRefExp(s_cur_dev)));
  fe->addArg(* new SgVarRefExp(*sbase));
  fe->addArg(* new SgArrayRefExp(*shead));
  fe->addArg(* new SgArrayRefExp(*sgpuhead));
  return(fe);
}

SgExpression *FillHeader_Ex(SgSymbol *s_cur_dev,SgSymbol *sbase,SgSymbol *shead,SgSymbol *sgpuhead,SgSymbol *soutType,SgSymbol *sParams)
{ // generating function call: dvmh_fill_header_ex_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[],DvmType *outTypeOfTransformation, DvmType extendedParams[])
  // or when RTS2 is used    
  //                   DvmType dvmh_fill_header_ex2_(const DvmType *pDeviceNum, const void *baseAddr, const DvmType dvmDesc[], DvmType devHeader[], DvmType extendedParams[])

  int fNum = INTERFACE_RTS2 ? FILL_HEADER_EX_2 : FILL_HEADER_EX ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  SgExpression *e;  
  fe->addArg(SgAddrOp(*new SgVarRefExp(s_cur_dev)));
  fe->addArg(* new SgVarRefExp(*sbase));
  fe->addArg(* new SgArrayRefExp(*shead));
  fe->addArg(* new SgArrayRefExp(*sgpuhead));
  if(!INTERFACE_RTS2)
     fe->addArg(SgAddrOp(*new SgVarRefExp(soutType)));
  fe->addArg(* new SgArrayRefExp(*sParams));
  if(INTERFACE_RTS2)
     e = &SgAssignOp(*new SgVarRefExp(soutType), *fe);

  return(INTERFACE_RTS2 ? e : fe);
}

SgExpression *LoopDoCuda(SgSymbol *s_loop_ref,SgSymbol *s_blocks,SgSymbol *s_threads,SgSymbol *s_stream, SgSymbol *s_blocks_info,SgSymbol *s_const)
{ // generating function call: loop_cuda_do(DvmhLoopRef *InDvmhLoop, dim3 *OutBlocks, void **InOutBlocks, SgExpression *etype)
  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[DO_CUDA]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));

  fe->addArg(SgAddrOp(*new SgVarRefExp(*s_blocks)));//(* new SgExpression(ADDRESS_OP,new SgVarRefExp(*s_blocks),NULL);
  //fe->addArg(* new SgValueExp(0)); //fe->addArg(SgAddrOp(* new SgVarRefExp(*s_threads)));
  //fe->addArg(* new SgValueExp(0)); //fe->addArg(SgAddrOp(* new SgVarRefExp(*s_stream)));
  if(s_blocks_info)
    //fe->addArg(*new SgCastExp(*C_PointerType(C_PointerType(C_VoidType() )), SgAddrOp(* new SgVarRefExp(*s_blocks_info))));
    fe->addArg(SgAddrOp(* new SgVarRefExp(*s_blocks_info)));
  else
    fe->addArg(* new SgValueExp(0));  // for sequence of statements in region
  fe->addArg(* new SgVarRefExp(s_const));
  return(fe);
}

SgFunctionCallExp *CallKernel(SgSymbol *skernel, SgExpression *blosks_threads)
{// generating Kernel Call:  
 // loop_<file_name>_<loopNo>(InDeviceBaseAddr1,dvmhDesc1[]...,InDeviceBaseAddrN,dvmhDescN[],<uses_vars>,<for_red_vars> ,blocks_info,red_count) 
    
  SgExpression *fe = new SgExpression(ACC_CALL_OP);
  fe->setSymbol(*skernel); 
  fe->setRhs(*blosks_threads);
  return((SgFunctionCallExp *)fe);
}

SgExpression *RegisterReduction(SgSymbol *s_loop_ref, SgSymbol *s_var_num, SgSymbol *s_red, SgSymbol *s_loc)
{ // generating function call: loop_cuda_register_red(DvmhLoopRef *InDvmhLoop, DvmType InRedNum, void **ArrayPtr, void **LocPtr)
  // or when RTS2 is used 
  //                      dvmh_loop_cuda_register_red_C(DvmType curLoop, DvmType redIndex, void **arrayAddrPtr, void **locAddrPtr)

  SgExpression *eloc;
  int fNum = INTERFACE_RTS2 ? RED_CUDA_C : RED_CUDA ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(SgDerefOp(*new SgVarRefExp(s_loop_ref)));
  else   
     fe->addArg(* new SgVarRefExp(s_loop_ref));  
  fe->addArg(* new SgVarRefExp(s_var_num));  
  fe->addArg(SgAddrOp(*new SgVarRefExp(*s_red))); 
  if (s_loc)
    eloc = &(SgAddrOp(*new SgVarRefExp(*s_loc)));
  else
    eloc = new SgValueExp(0);
  fe->addArg(*eloc); 
  return( fe);
}


SgExpression *Register_Red(SgSymbol *s_loop_ref, SgSymbol *s_var_num, SgSymbol *s_red_array, SgSymbol *s_loc_array,SgSymbol *s_offset,SgSymbol *s_loc_offset)
{ // generating function call: loop_cuda_register_red_(DvmhLoopRef *InDvmhLoop, DvmType InRedNumRef,void *InDeviceArrayBaseAddr, void *InDeviceLocBaseAddr,CudaOffsetTypeRef *ArrayOffsetPtr, CudaOffsetTypeRef *LocOffsetPtr)


  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[REGISTER_RED]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
 
  fe->addArg(SgAddrOp(* new SgVarRefExp(s_var_num)));  
  fe->addArg(*new SgVarRefExp(*s_red_array));
  if(s_loc_array)
    fe->addArg(*new SgVarRefExp(*s_loc_array)); 
  else
    fe->addArg(*new SgValueExp(0));
  fe->addArg(* new SgVarRefExp(s_offset));
  fe->addArg(* new SgVarRefExp(s_loc_offset));
  return( fe);
}

SgExpression *InitReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num, SgSymbol *s_red,SgSymbol *s_loc)
{ // generating function call: loop_red_init_(DvmhLoopRef *InDvmhLoop, Dvmtype *InRedNum, void *arrayPtr, void *locPtr)
  // or when RTS2 is used  
  //                      dvmh_loop_red_init_(const DvmType *pCurLoop, const DvmType *pRedIndex, void *arrayAddr, void *locAddr)

  SgExpression *eloc;
  int fNum = INTERFACE_RTS2 ? RED_INIT_2 : RED_INIT_C ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));  
  fe->addArg(SgAddrOp(* new SgVarRefExp(s_var_num)));
  fe->addArg(SgAddrOp(* new SgVarRefExp(*s_red)));
  if (s_loc)
    eloc = new SgArrayRefExp(*s_loc); //&(SgAddrOp(*new SgVarRefExp(*s_loc)));
  else
    eloc = new SgValueExp(0);
  fe->addArg(*eloc);
  return(fe);
}

SgExpression *CudaInitReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num,  SgSymbol *s_dev_red,SgSymbol *s_dev_loc) //SgSymbol *s_red,SgSymbol *s_loc,
{ // generating function call: loop_cuda_red_init_ (DvmhLoopRef *InDvmhLoop, Dvmtype InRedNum, void *arrayPtr, void *locPtr, void **devArrayPtr, void **devLocPtr)
  // or when RTS2 is used  
  //                      dvmh_loop_cuda_red_init_C(DvmType curLoop, DvmType redIndex, void **devArrayAddrPtr, void **devLocAddrPtr) 

  SgExpression *eloc;
  int fNum = INTERFACE_RTS2 ? CUDA_RED_INIT_2 : CUDA_RED_INIT ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(SgDerefOp(*new SgVarRefExp(s_loop_ref)));
  else   
     fe->addArg(* new SgVarRefExp(s_loop_ref));
  fe->addArg(* new SgVarRefExp(s_var_num));
       //fe->addArg(* new SgVarRefExp(*s_red));
       //if (s_loc)
       //  eloc = new SgArrayRefExp(*s_loc); //&(SgAddrOp(*new SgVarRefExp(*s_loc)));
       //else
       //  eloc = new SgValueExp(0);
       //fe->addArg(*eloc);
  fe->addArg(SgAddrOp(*new SgVarRefExp(s_dev_red)));
  if (s_dev_loc)
    eloc = new SgArrayRefExp(*s_dev_loc); //&(SgAddrOp(*new SgVarRefExp(*s_dev_loc)));
  else
    eloc = new SgValueExp(0);
  fe->addArg(*eloc);
  return(fe);
}

SgExpression *PrepareReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num, SgSymbol *s_count, SgSymbol *s_fill_flag, int fixedCount, int fillFlag)
{ // generating function call: loop_cuda_red_prepare_(DvmhLoopRef *InDvmhLoop, Dvmtype InRedNumRef, DvmType InCountRef, DvmType InFillFlagRef)
  // or when RTS2 is used  
  //                      dvmh_loop_cuda_red_prepare_C(DvmType curLoop, DvmType redIndex, DvmType count, DvmType fillFlag)                                                     

  int fNum = INTERFACE_RTS2 ? RED_PREPARE_C : RED_PREPARE ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(SgDerefOp(*new SgVarRefExp(s_loop_ref)));
  else   
     fe->addArg(* new SgVarRefExp(s_loop_ref));
  fe->addArg(* new SgVarRefExp(s_var_num));
  if (fixedCount == 0)
    fe->addArg(* new SgVarRefExp(s_count));
  else
    fe->addArg(*new SgValueExp(fixedCount));
  if (fillFlag == -1)
    fe->addArg(* new SgVarRefExp(s_fill_flag));
  else
    fe->addArg(* new SgValueExp(fillFlag));
  return(fe);
}

SgExpression *FinishReduction(SgSymbol *s_loop_ref,  SgSymbol *s_var_num)
{ // generating function call: loop_red_finish_(DvmhLoopRef *InDvmhLoop, DvmType InRedNumRef)
  // or when RTS2 is used  
  //                           dvmh_loop_cuda_red_finish_C(DvmType curLoop, DvmType redIndex)

  int fNum = INTERFACE_RTS2 ? RED_FINISH_C : RED_FINISH ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(SgDerefOp(*new SgVarRefExp(s_loop_ref)));
  else   
     fe->addArg(* new SgVarRefExp(s_loop_ref));
  fe->addArg(* new SgVarRefExp(s_var_num));
  return(fe);
}


SgExpression *LoopSharedNeeded(SgSymbol *s_loop_ref, SgExpression *ecount)
{ // generating function call: loop_cuda_shared_needed_(DvmhLoopRef *InDvmhLoop, DvmType *count)
  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[SHARED_NEEDED]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
  fe->addArg(*ecount);
  return(fe);
}

SgExpression *GetLocalPart(SgSymbol *s_loop_ref, SgSymbol *shead, SgSymbol *s_const)
{ // generating function call:
  //             void * loop_cuda_get_local_part (DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[], DvmType indexType);
  // or when RTS2 is used  
  //         void *dvmh_loop_cuda_get_local_part_C(DvmType curLoop, const DvmType dvmDesc[], DvmType indexType)

  int fNum = INTERFACE_RTS2 ? GET_LOCAL_PART_C : GET_LOCAL_PART ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  if(INTERFACE_RTS2)
     fe->addArg(SgDerefOp(*new SgVarRefExp(s_loop_ref)));
  else   
     fe->addArg(* new SgVarRefExp(s_loop_ref));  
  fe->addArg(* new SgArrayRefExp(*shead));
  fe->addArg(* new SgVarRefExp(s_const));
  return(fe);

}

SgExpression *GetDeviceNum(SgSymbol *s_loop_ref)
{ // generating function call:
  //                       DvmType  loop_get_device_num_ (DvmhLoopRef *InDvmhLoop)
  // or when RTS2 is used
  //                       DvmType dvmh_loop_get_device_num_(const DvmType *pCurLoop)
  
  int  fNum = INTERFACE_RTS2 ? GET_DEVICE_NUM_2 : GET_DEVICE_NUM ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
  
  return(fe);

}

SgExpression *GetOverallStep(SgSymbol *s_loop_ref)
{ // generating function call:
  //                           loop_cuda_get_red_step (DvmhLoopRef *InDvmhLoop)
                                          //DvmType loop_get_overall_blocks_(DvmhLoopRef *InDvmhLoop)
 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[GET_OVERALL_STEP]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
  
  return(fe);

}

SgExpression *FillBounds(SgSymbol *loop_s, SgSymbol *sBlow,SgSymbol *sBhigh,SgSymbol *sBstep)
{// generating function call: 
 //                               loop_fill_bounds_(DvmType *InDvmhLoop, DvmType lowIndex[], DvmType highIndex[], DvmType stepIndex[]) 
 // DvmhLoopRef - result of loop_create()
 // or when RTS2 is used   
 //                          dvmh_loop_fill_bounds_(const DvmType *pCurLoop, DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[]);  

  int fNum = INTERFACE_RTS2 ? FILL_BOUNDS_2 : FILL_BOUNDS_C ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);  
                             
  fe -> addArg(* new SgVarRefExp(loop_s));
  fe -> addArg(* new SgVarRefExp(sBlow));
  fe -> addArg(* new SgVarRefExp(sBhigh));
  if(sBstep)
    fe -> addArg(* new SgVarRefExp(sBstep));
  else
    fe -> addArg(* new SgValueExp(0));
  return(fe);
}

SgExpression *RedPost(SgSymbol *loop_s, SgSymbol *s_var_num, SgSymbol *sRed,SgSymbol *sLoc)
{// generating function call: 
 //                         void  loop_red_post_(DvmhLoopRef *InDvmhLoop, DvmType *InRedNum, void *arrayPtr, void *locPtr) 
 // DvmhLoopRef - result of loop_create() 
 // or when RTS2 is used      
 //                     void dvmh_loop_red_post_(const DvmType *pCurLoop, const DvmType *pRedIndex, const void *arrayAddr, const void *locAddr)   

  int fNum = INTERFACE_RTS2 ? RED_POST_2 : RED_POST_C ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);  
  
  fe -> addArg(* new SgVarRefExp(loop_s));
  fe->addArg(SgAddrOp(* new SgVarRefExp(s_var_num)));
  fe->addArg(SgAddrOp(* new SgVarRefExp(sRed)));
  if(sLoc)
    fe -> addArg(*new SgArrayRefExp(*sLoc));
  else
    fe -> addArg(*new SgValueExp(0));  

  return(fe);
}

SgExpression *CudaReplicate(SgSymbol *Addr, SgSymbol *recordSize, SgSymbol *quantity, SgSymbol *devPtr)
{// generating function call: 
 //                         void dvmh_cuda_replicate_(void *addr, DvmType recordSize, DvmType quantity, void *devPtr) 
 //     
   
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CUDA_REPLICATE]);  
  
  fe->addArg(SgAddrOp(* new SgVarRefExp(Addr)));
  fe->addArg(* new SgVarRefExp(recordSize));
  fe->addArg(* new SgVarRefExp(quantity));
  fe->addArg(* new SgVarRefExp(devPtr));

  return(fe);
}

SgExpression *GetDependencyMask(SgSymbol *s_loop_ref) 
{ // generating function call:
  //                       DvmType loop_get_dependency_mask_(DvmhLoopRef *InDvmhLoop)
  // or when RTS2 is used   
  //                  DvmType dvmh_loop_get_dependency_mask_(const DvmType *pCurLoop)

  int fNum = INTERFACE_RTS2 ? GET_DEP_MASK_2 : GET_DEP_MASK ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
  
  return(fe);

}

SgExpression *CudaTransform(SgSymbol *s_loop_ref, SgSymbol *s_head, SgSymbol *s_BackFlag, SgSymbol *s_headH, SgSymbol *s_addrParam) 
{ // generating function call:
  //                       DvmType loop_cuda_transform_(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[], DvmhLoopRef *backFlagRef, DvmType dvmhDesc[], DvmType addressingParams[])
 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CUDA_TRANSFORM]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
  fe->addArg(* new SgArrayRefExp(*s_head));  
  fe->addArg(SgAddrOp(*new SgVarRefExp(s_BackFlag)));
  fe->addArg(* new SgArrayRefExp(*s_headH));
  fe->addArg(* new SgArrayRefExp(*s_addrParam));  
  return(fe);
}

SgExpression *CudaAutoTransform(SgSymbol *s_loop_ref, SgSymbol *s_head) 
{ // generating function call:
  //                       DvmType loop_cuda_autotransform(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[])
  // or when RTS2 is used  
  //                       DvmType dvmh_loop_autotransform_(const DvmType *pCurLoop, DvmType dvmDesc[])

  int fNum = INTERFACE_RTS2 ? LOOP_AUTOTRANSFORM : CUDA_AUTOTRANSFORM ;
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);
  
  fe->addArg(* new SgVarRefExp(s_loop_ref));
  fe->addArg(* new SgArrayRefExp(*s_head));  
  return(fe);
}

SgExpression *ApplyOffset(SgSymbol *s_head, SgSymbol *s_base, SgSymbol *s_headH) 
{ // generating function call:
  //                       dvmh_apply_offset(DvmType dvmDesc[], void *base, DvmType dvmhDesc[])
 
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[APPLY_OFFSET]);
  
  fe->addArg(* new SgArrayRefExp(*s_head));  
  fe->addArg(* new SgVarRefExp(s_base));
  fe->addArg(* new SgArrayRefExp(*s_headH)); 
  return(fe);

}

SgExpression *GetConfig(SgSymbol *s_loop_ref,SgSymbol *s_shared_perThread,SgSymbol *s_regs_perThread,SgSymbol *s_threads,SgSymbol *s_stream, SgSymbol *s_shared_perBlock)
{ // generating function call: void loop_cuda_get_config_ (DvmhLoopRef *InDvmhLoop, DvmType InSharedPerThread, DvmType InRegsPerThread, dim3 *OutThreads, cudaStream_t *OutStream, DvmType *OutSharedPerBlock)
  // or when RTS2 is used  
  //                           dvmh_loop_cuda_get_config_C(DvmType curLoop, DvmType sharedPerThread, DvmType regsPerThread, void *inOutThreads, void *outStream,DvmType *outSharedPerBlock)

  int fNum = INTERFACE_RTS2 ? GET_CONFIG_C : GET_CONFIG ;  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);

  if(INTERFACE_RTS2)
     fe->addArg(SgDerefOp(*new SgVarRefExp(s_loop_ref)));
  else     
     fe->addArg(* new SgVarRefExp(s_loop_ref));
  if(s_shared_perThread)
    fe->addArg(*new SgVarRefExp(*s_shared_perThread));
  else
    fe->addArg(*new SgValueExp(0));
  if(s_regs_perThread)
    fe->addArg(*new SgVarRefExp(*s_regs_perThread));
  else
    fe->addArg(*new SgValueExp(0));

  fe->addArg(SgAddrOp(* new SgVarRefExp(*s_threads)));
  fe->addArg(SgAddrOp(* new SgVarRefExp(*s_stream)));
  if(s_shared_perBlock)
    fe->addArg(SgAddrOp(* new SgVarRefExp(*s_shared_perBlock)));
  else
    fe->addArg(* new SgValueExp(0));  
  return(fe);
}

SgExpression *ChangeFilledBounds(SgSymbol *s_low,SgSymbol *s_high,SgSymbol *s_idx, SgSymbol *s_n,SgSymbol *s_dep,SgSymbol *s_type,SgSymbol *s_idxs)
{// generating function call: 
 //                         void dvmh_change_filled_bounds(DvmType *low, DvmType *high, DvmType *idx, DvmType n, DvmType dep, DvmType type_of_run, DvmType *idxs); 
 //                              dvmh_change_filled_bounds_C(DvmType boundsLow[], DvmType boundsHigh[], DvmType loopSteps[], DvmType rank, DvmType depMask, DvmType idxPerm[])  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[CHANGE_BOUNDS]);  
  
  fe -> addArg(* new SgVarRefExp(s_low));
  fe -> addArg(* new SgVarRefExp(s_high));
  fe -> addArg(* new SgVarRefExp(s_idx));
  fe -> addArg(* new SgVarRefExp(s_n));
  fe -> addArg(* new SgVarRefExp(s_dep));
  fe -> addArg(* new SgVarRefExp(s_type));
  fe -> addArg(* new SgVarRefExp(s_idxs));
  return(fe);
}

SgExpression *GuessIndexType(SgSymbol *s_loop_ref)
{// generating function call: 
 //                         loop_guess_index_type_(DvmhLoopRef *InDvmhLoop) 
 // or when RTS2 is used  
 //                    dvmh_loop_guess_index_type_(const DvmType *pCurLoop)

  int fNum = INTERFACE_RTS2 ? GUESS_INDEX_TYPE_2 : GUESS_INDEX_TYPE ;  
  SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[fNum]);  
  
  fe -> addArg(* new SgVarRefExp(s_loop_ref));
  return(fe);
}

SgExpression *RtcSetLang(SgSymbol *s_loop_ref, const int lang)
{// generating function call: 
 //                         loop_cuda_rtc_set_lang(DvmType *InDvmhLoop, DvmType lang) 

    SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RTC_SET_LANG]);

    fe->addArg(*new SgVarRefExp(s_loop_ref));
    if (lang == 0)
        fe->addArg(*new SgKeywordValExp("FORTRAN_CUDA"));
    else if (lang == 1)
        fe->addArg(*new SgKeywordValExp("C_CUDA")); 
    else
        fe->addArg(*new SgKeywordValExp("UNKNOWN_CUDA")); 
    return(fe);
}
/**************************************************************\
* Fortran DVM                                                  * 
*                                                              *
*              Translating HPF-program                         *
\**************************************************************/

#include "dvm.h"
int hpf_new_var;
/**************************************************************\
*       Processing distributed array refference                *
\**************************************************************/
/*----------- outside the range of parallel loop -------------*/
int SearchDistArrayRef(SgExpression *e, SgStatement *stmt)
{ int res = 0;
  SgExpression *el,*eleft;
  if(only_local)   // option -Honlyl is specified:
     return (res); // all the operands are local in sequential threads
  //looks  the expression 'e' for distributed array references,
  // adds the attribute REMOTE_VARIABLE to the reference 
  //generates statements for loading the values of distributed array elements into buffers
  if(!e)
    return (res);
   
  if(isSgArrayRefExp(e)) {
    for(el=e->lhs(); el; el=el->rhs())
       res = (SearchDistArrayRef(el->lhs(),stmt)) ? 1 : res;

    if(HEADER( e->symbol()) && e->lhs()) {//is distributed array reference with subscripts
      if(stmt->variant() == ASSIGN_STAT) {
         eleft = isSgArrayRefExp(stmt->expr(0));//left part of assignment statement
         if(eleft && eleft->lhs() && RemAccessRefCompare(eleft, e))
	                        //array reference in right part of assignment statement is 
	                        //the same as one in left part
               return(1);
      }
       BufferDistArrayRef(e,stmt);
                          //add attribute(REMOTE_VARIABLE) to  distributed array reference
       res = 1;
    }      
    return(res);
  }

  res =  SearchDistArrayRef(e->lhs(),stmt);
  res = (SearchDistArrayRef(e->rhs(),stmt)) ? 1 : res;
  return(res);
}

void BufferDistArrayRef(SgExpression *e, SgStatement *stmt)
{//generating statements for loading the value of distributed array element
 // to  buffer scalar variable and inserting ones before statement 'stmt' 
 //adding attribute REMOTE_VARIABLE to distributed array reference 'e'
   int r,n,ibuf;
   SgExpression *el;
   rem_var *remv = new rem_var;
   remv->ncolon = 0;
   remv->index = ibuf = ++rmbuf_size[TypeIndex(e->symbol()->type()->baseType())];
   remv->amv   = -1;
   e->addAttribute(REMOTE_VARIABLE,(void *) remv, sizeof(rem_var));
   r = Rank(e->symbol());
   for(el=e->lhs(),n=0; el; el = el->rhs(),n++) 
       ;
   if(r && n && r != n) {
   Error("Wrong number of subscripts specified for '%s'",e->symbol()->identifier(),175,stmt);
   return;
   }
   if(first_time) {
      SgStatement *st,*stw;
      ReplaceContext(stmt);
      stw = (stmt->variant() == ELSEIF_NODE) ? stmt->controlParent() : stmt;
              //loading buffers for statement ELSEIF is performed before statement IF_THEN
      LINE_NUMBER_STL_BEFORE(st,stmt,stw);
      cur_st = st;
      first_time = 0;
   }
   CopyToBuffer(0, ibuf, e); //loading buffer for distributed array's element
   return;	  
}

/*----------- inside the range of parallel loop --------------*/

SgExpression *IND_ModifiedDistArrayRef(SgExpression *e, SgStatement *st)
// analyzing distributed array reference:
// may this reference be used as IND_target? 
{int i, num, ni, use[MAX_LOOP_NEST], IN_use;
 SgExpression *ei,*el,*es,*ee;
 ni = nIND+nIEX;
 for(i= 0; i<ni; i++)
   use[i] = 0;
 if(!(e->lhs())) return(NULL); //no subscripts 
 ee = &(e->copy());
 for(el=ee->lhs(); el; el=el->rhs())   {
   es = el->lhs();  //subscript expression
   IN_use = 0;
   num = AxisNumOfDoVarInExpr(es, DoVar, ni, &ei, use, &IN_use, st);
   if(num<0) return(NULL);
   if(num>nIEX) {// IND-index is used 
     if(use[num-1] > 1) {
       Error("More one occurance of do-variable '%s' in subscript list", DoVar[num-1]->identifier(),251, st);
       return(NULL);
     }
     if(IN_use) //IND-index and IN-index are used 
       err("More one occurance of a do-variable in subscript expression", 252,st);
           //err("Illegal subscript expression",253,cur_st); 
   } else
     if(IN_use) //IN-index is used
       el->setLhs(new SgExpression(DDOT));       //(new SgKeywordValExp("*"));
 }
 for(i= nIEX; i<ni; i++)
    if(use[i] == 0)
       return(NULL);
 return(ee);
   //return(&(e->copy()));
}

void  IND_UsedDistArrayRef(SgExpression *e, SgStatement *st)
// analyzing the distributed array reference in right part of assignment statement and so on
// including it in the list IND_refs
{int i, num, ni, use[MAX_LOOP_NEST], IN_use, nt;
 SgExpression *ei,*el,*es,*ee, *elbb;
 SgValueExp c0(0),cM1(-1);
 IND_ref_list *ref;
 hpf_new_var=0;
 ni = nIND+nIEX;
 for(i= 0; i<ni; i++)
   use[i] = 0;
 if(!(e->lhs())) return; //no subscripts 
 if(isINDtarget(e)){  // is the same reference as IND_target
                     // ( reference in left part of assignment statement)
   IND_DistArrayRef(e, st, NULL);
   return;
 }
 if((ref=isInINDrefList(e)) != NULL) {// the same reference is in list IND_refs 
   IND_DistArrayRef(e, st, ref);
   return;
 }
 // creating new element of list of distributed array references used in parallel loop
 ref = new IND_ref_list;
 ref->next =  IND_refs;
 IND_refs = ref;
 ee = &(e->copy());
 ref->rmref = ee;
 ref->nc = 0;
 ref->ind = 0;
 nt = 0;
 //looking through the subscript list
 for(el=ee->lhs(); el; el=el->rhs(), nt++)   {
   es = el->lhs();  //subscript expression
   IN_use = 0;
   hpf_new_var=0;
   //determinating kind of subscript expression
   num = AxisNumOfDoVarInExpr(es, DoVar, ni, &ei, use, &IN_use, st);
   if(num>nIEX) {// IND-index is used 
           ref->nc++; 
     if(IN_use) {//IND-index and IN-index are used : f(IN)
       //err("More one occurance of a do-variable in subscript expression", 252,st);
       el->setLhs(new SgExpression(DDOT));  // the subscript is replaced by ':'
           ref->axis[nt] = & cM1.copy();
           ref->coef[nt] = & c0.copy();
           ref->cons[nt] = & c0.copy();  
     } else { 
           ref->axis[nt] = new SgValueExp(num-nIEX);  
       CoeffConst(es, ei, &(ref->coef[nt]), &(ref->cons[nt])); //testing form: a*IND+b
       if(!ref->coef[nt]){ //f(IND)
           //err("Illegal subscript expression", 253, stat);
           el->setLhs(new SgExpression(DDOT));  // the subscript is replaced by ':'
           ref->axis[nt] = & cM1.copy();
           ref->coef[nt] = & c0.copy();
           ref->cons[nt] = & c0.copy();
       }
       else //a*IND+b
         // correcting const with lower bound of array
         if((elbb = LowerBound(ref->rmref->symbol(),nt)) != NULL)
           ref->cons[nt] = &(*(ref->cons[nt])  - (elbb->copy()));
     }  
   } else // IND-index is not used 
     if(IN_use || hpf_new_var) {//IN-index is used: f(IN)  or new variable is used
           el->setLhs(new SgExpression(DDOT));  // the subscript is replaced by ':'
           ref->axis[nt] = & cM1.copy();
           ref->coef[nt] = & c0.copy();
           ref->cons[nt] = & c0.copy(); 
           ref->nc++;   
     }  
     else { // invariant: const,f(IEX)
           ref->axis[nt] = & c0.copy();
           ref->coef[nt] = & c0.copy();
        if((elbb = LowerBound(ref->rmref->symbol(),nt)) != NULL)
           ref->cons[nt] = & (es->copy() - (elbb->copy()));
                                   // correcting const with lower bound of array
        else //error situation
           ref->cons[nt] = & (es->copy()); 
     }
 }
    if(nt < 7)
           ref->axis[nt] = NULL; 

    IND_DistArrayRef(e, st, ref);
    return;
}

int AxisNumOfDoVarInExpr (SgExpression *e, SgSymbol *dovar_ident[], int ni,                             SgExpression **eref, int use[], int *pINuse, SgStatement *st)
{
  SgSymbol *symb;
  SgExpression * e1; 
  int i,i1,i2;
  *eref = NULL;
  if (!e) 
    return(0);
  if(isSgVarRefExp(e))  {
    symb = e->symbol();
    for(i=0; i<ni; i++) {
       if(dovar_ident[i]==NULL)
         continue;
       if(dovar_ident[i]==symb)  { //is IEX- or IND-index
         *eref = e;
         /*
         if (use[i] == 1 && i>= nIEX) 
             Error("More one occurance of do-variable '%s' in subscript list", symb->identifier(),251, st);
	  */
         use[i]++;
         return(i+1);
       } 
    }
    if(isDoVar(symb)) // is IN-index
	   // (symb is not IEX- nor IND-index, but symb is do-variable => symb is IN-index)
       (*pINuse)++;   
    if(isNewVar(symb))
       hpf_new_var=1; 
    return (0);
  }
  i1 = AxisNumOfDoVarInExpr(e->lhs(), dovar_ident, ni, eref, use, pINuse, st);
  e1 = *eref;
  i2 = AxisNumOfDoVarInExpr(e->rhs(), dovar_ident, ni, eref, use, pINuse, st);
  if((i1==-1)||(i2==-1))  return(-1);
  if(i1 && i1>=nIEX && i2 && i2>=nIEX)  {
    err("More one occurance of a do-variable in subscript expression", 252,st);
    return(-1);
  }
  if(i1) *eref = e1;
  return(i1 ? i1 : i2);
}

int isINDtarget(SgExpression *re)
{if(RemAccessRefCompare(IND_target, re))
   return(1);
 else
   return (0);
}

IND_ref_list *isInINDrefList(SgExpression *re)
{IND_ref_list *el;
             //for(el=IND_refs; el; el=el->next)
             //el->rmref->unparsestdout(); //?!!!
 for(el=IND_refs; el; el=el->next)
    if(RemAccessRefCompare(el->rmref, re))
       return(el);
 return (NULL);
}
/*
void IND_DistArrayRef(SgExpression *e, SgStatement *st)
{SgSymbol *ar;
         //replace distributed array reference A(I1,I2,...,In) by
         //                                     n   
         // <memory>( HeaderCopy(n+1) +  I1 + SUMMA(HeaderCopy(n-k+1) * Ik))
         //                                    k=2                    
         // <memory> is I0000M  if A  is of type integer 
         //             R0000M  if A  is of type real 
         //             D0000M  if A  is of type double precision 
         //             C0000M  if A  is of type complex
         //             L0000M  if A  is of type logical 
    ar = e->symbol();
    e->setSymbol(baseMemory(ar->type()->baseType()));   
    if(!e->lhs())
        Error("No subscripts: %s", ar->identifier(),171,st);
    else {  
        (e->lhs())->setLhs(*LinearForm(ar,e->lhs()));
        (e->lhs())->setRhs(NULL); 
         }  
} 
*/

void IND_DistArrayRef(SgExpression *e, SgStatement *st, IND_ref_list *el)
{SgSymbol *ar;
         //replace distributed array reference A(I1,I2,...,In) by
         //                                     n   
         // <memory>( HeaderCopy(n+1) +  I1 + SUMMA(HeaderCopy(n-k+1) * Ik))
         //                                    k=2                    
         // <memory> is I0000M  if A  is of type integer 
         //             R0000M  if A  is of type real 
         //             D0000M  if A  is of type double precision 
         //             C0000M  if A  is of type complex
         //             L0000M  if A  is of type logical 
 ar = e->symbol();
 if(!el) { // local access reference
   e->setSymbol(baseMemory(ar->type()->baseType()));   
   if(!e->lhs())
        Error("No subscripts: %s", ar->identifier(),171,st);
   else {  
        (e->lhs())->setLhs(*LinearForm(ar,e->lhs()));
        (e->lhs())->setRhs(NULL); 
        }  
 } else { 
   int n, num, k;
   SgExpression *esl;
   SgExpression *p = NULL;
   if(el->ind == 0) {//new reference: allocating header copy
     el->ind = nhpf;
     nhpf+=(el->nc)+2;
   }
   hpf_ind = el->ind;
   if(el->nc) { //there are ':' or a*IND+b elements in index list of remote variable
     for(n = 0; n<7 && el->axis[n]; n++)
       ;
     if(n && n != Rank(ar)) {
       Error("Wrong number of subscripts specified for '%s'", ar->identifier(),175,st);
       return;
     }
     //looking through the subscript and index lists 
     for(esl=e->lhs(),k=0; esl && k<n; esl=esl->rhs(),k++){
       num = el->axis[k]->valueInteger();
       if(num == -1) // ':'
          p=esl;
       else if(num > 0){ //do-variable-use: a*IND+b
          esl->setLhs(new SgVarRefExp(IND_var[num-1])); // replace by IND
	  /*
          if(p)  
            esl->setLhs(new SgVarRefExp(IND_var[num-1])); // replace by IND
          else  //first non-invariant index
            if(INTEGER_VALUE(el->coef[k],1) && k == 0) // a == 1
            esl->setLhs(new SgVarRefExp(IND_var[num-1])); // replace by IND
            else
            esl->setLhs(&(*HPF000((el->ind)+(el->nc)-1)*(*new SgVarRefExp(IND_var[num-1]))));
	                                                  // replace by  HeaderCopy(nc)*IND
	   */
          p=esl;
       }
       else   
          //delete corresponding subscript in reference
          if(!p)
            e->setLhs(esl->rhs());
          else
            p->setRhs(esl->rhs());
     }
   } 
   
   e->setSymbol(baseMemory(ar->type()->baseType()));
   num = el->axis[0]->valueInteger();
   if ((num == 0) || ((num > 0) && !INTEGER_VALUE(el->coef[0], 1)) )//first dimension is b or a*IND+b
                                                           // where a != 1
     e->lhs()->setLhs(*HPF000((el->ind)+(el->nc)) * (*e->lhs()->lhs())); 
                         // first non-invariant index I is replaced  by  HeaderCopy(nc)*I
   e->setLhs(*LinearFormB(hpfbuf, (el->ind), el->nc, e->lhs()));
 }
} 
/**************************************************************\
*       Processing independent loop nest                       *
\**************************************************************/
void SkipIndepLoopNest(SgStatement *stmt)
{ 
  SgStatement *st,*stl;
  stl = stmt; 
  // looking through the loop nest 
  for(st=par_do; isSgForStmt(st); st=st->lexNext()){
     stl = st;
     if(st->lexNext()->variant() == HPF_INDEPENDENT_DIR)
       Extract_Stmt(st->lexNext()); //extracting nested INDEPENDENT directive
     else   
       break;  
  }
  cur_st = stl;
}

void LookIndepLoopNest(SgStatement *stmt)
{ int i;
  SgStatement *st,*stl;
  stl = stmt; 
  // looking through the loop nest 
  for(st=stmt->lexNext(),i = 0; isSgForStmt(st); st=st->lexNext(),i++){
     stl = st;
     IND_var[i] = st->symbol(); 
     if(st->lexNext()->variant() == HPF_INDEPENDENT_DIR)
       Extract_Stmt(st->lexNext()); //extracting nested INDEPENDENT directive
     else   
       break;  
  }
  cur_st = stl;
}

int IndependentLoop(SgStatement *stmt)
{
  SgStatement *st, *if_stmt, *stl = NULL;
  SgStatement *first_do;
  SgValueExp c0(0);
  int i, ndo, iout, iinp, ind;
  SgForStmt *stdo;
  SgValueExp c1(1);
  SgExpression *step[MAX_LOOP_LEVEL], 
               *init[MAX_LOOP_LEVEL],
               *last[MAX_LOOP_LEVEL],
               *vpart[MAX_LOOP_LEVEL];

  first_do = stmt -> lexNext();// first DO statement of the loop nest
  IND_var = DoVar+nIEX;
  IND_target = NULL;
  IND_target_R = NULL;
  IND_refs   = NULL;
  redl = NULL;
  irg = 0; idebrg = 0;
  red_list = NULL;
  redgref = NULL;
  //new_red_var_list = NULL;
 
//initialization vpart[]
  for(i=0; i<MAX_LOOP_LEVEL; i++)
     vpart[i] = NULL;
//determinating rank of independent loop 
  for(st=first_do,ndo=0; isSgForStmt(st); st=st->lexNext()) {
    ndo++;
    if(st->lexNext()->variant() == HPF_INDEPENDENT_DIR) {
       if(st->lexNext()->expr(0))
	 stmt->setExpression(0,*ConnectNewList(stmt->expr(0),st->lexNext()->expr(0)));
       //stmt->expr(0)->lhs()->unparsestdout();
       Extract_Stmt(st->lexNext()); //extracting nested INDEPENDENT directive
    } 
    else   
       break;  
  }
  /*    if(st->lexNext()->variant() == HPF_INDEPENDENT_DIR)
       st=st->lexNext();
     else
       break;  
   */

  nIND = ndo;
// generating assign statement:
//   dvm000(i) = lnumb(num); // line number of stmt
  LINE_NUMBER_AFTER(stmt,stmt);
//generating call to 'bploop' function of performance analizer (begin of parallel interval)
  if(perf_analysis && perf_analysis != 2)
  {
    InsertNewStatementAfter(St_Bploop(OpenInterval(stmt)), cur_st, stmt->controlParent()); //inserting after function call 'lnumb'
  }
  ins_st1 = cur_st;

// generating assign statement:
//   dvm000(iplp) = crtpl(Rank);
  iplp = ndvm++; 
  doAssignTo_After(DVM000(iplp), CreateParLoop( ndo)); 

//allocating DebRedGroupRef
  ndvm++;
//allocating RedGroupRef
  ndvm++;
//allocating OutInitIndexArray,OutLastIndexArray,OutStepArray 
  iout = iarg = ndvm; 
  ndvm += 3*ndo;

// looking through the loop nest 
  for(st=first_do,i=0; i<ndo; st=st->lexNext(),i++) {
     stdo = isSgForStmt(st);
     if(!stdo)
       break;  
     stl = st;
     IND_var[i] = stdo->symbol(); 
     step[i]   = stdo->step();
     if(!step[i])
       step[i] = & c1.copy();  // by default: step = 1
     init[i] = isSpecialFormExp(stdo->start(),i,iout+i,vpart,IND_var);
     if( init[i] )
         step[i] = & c1.copy(); 
      else
         init[i]   = stdo->start();  
     last[i]   = stdo->end();  
    
     // setting new loop parameters
     if(vpart[i]) 
       stdo->setStart(*DVM000(iout+i)+ (*vpart[i]));//special form
                                                    //step is not replaced
     else { 
       stdo->setStart(*DVM000(iout+i));  
       //stdo->setStep(*DVM000(iout+i+2*ndo));
     }
     stdo->setEnd(*DVM000(iout+i+ndo));
     SetDoVar(stdo->symbol());
  }

  iinp = ndvm;    
  if(dvm_debug) 
     OpenParLoop_Inter(stl,iinp,iinp+ndo,IND_var,ndo);

  // creating LoopVarAddrArray, LoopVarTypeArray,InpInitIndexArray, InpLastIndexArray
  // and InpStepArray  
  for(i=0; i<ndo; i++) 
     doAssignStmtAfter(GetAddres(IND_var[i]));
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( new SgValueExp(LoopVarType(IND_var[i],stmt)));
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( init[i] );
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( last[i] );
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( step[i] );

 ins_st2 = cur_st; 
 if(dvm_debug) {
      pardo_line = first_do->lineNumber();
      DebugParLoop(cur_st,ndo,iinp+2*ndo);
      /*SET_DVM(iinp+2*ndo); */
  } 
 /* else
    { SET_DVM(iinp); } 
 */

 // generating Logical IF statement:
       // begin_lab  IF (DoPL(LoopRef) .EQ. 0) GO TO end_lab
       // and inserting it before  loop nest
       begin_lab = GetLabel();
       end_lab   = GetLabel();
       if_stmt = new SgLogIfStmt(SgEqOp(*doLoop(iplp) , c0),                                                                *new SgGotoStmt(*end_lab));
       if_stmt -> setLabel(*begin_lab);
       cur_st->insertStmtAfter(*if_stmt);
       (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF
                                                            // (error Sage)
       cur_st = stl; // set cur_st on last DO satement of loop nest
       //cur_st = st->lexPrev();  // set cur_st on last DO satement of loop nest
        // cur_st = stl->lexNext(); 
  return(1); //!!!
}

int IndependentLoop_Debug(SgStatement *stmt)
{ SgStatement *st, *stl = NULL;
  SgStatement *first_do;
  SgValueExp c0(0);
  int i, ndo, iout, iinp, ind;
  SgForStmt *stdo;
  SgValueExp c1(1);
  SgExpression *step[MAX_LOOP_LEVEL], 
               *init[MAX_LOOP_LEVEL],
               *last[MAX_LOOP_LEVEL],
               *vpart[MAX_LOOP_LEVEL];

  first_do = stmt -> lexNext();// first DO statement of the loop nest
  IND_var = DoVar+nIEX;
  IND_target = NULL;
  IND_target_R = NULL;
  IND_refs   = NULL;
  redl = NULL;
  irg = 0; idebrg = 0;
  red_list = NULL;
  redgref = NULL;
  //new_red_var_list = NULL;
 
//determinating rank of independent loop 
  for(st=first_do,ndo=0; isSgForStmt(st); st=st->lexNext()) {
    ndo++;
    if(st->lexNext()->variant() == HPF_INDEPENDENT_DIR) {
       if(st->lexNext()->expr(0))
	 stmt->setExpression(0,*ConnectNewList(stmt->expr(0),st->lexNext()->expr(0)));
       //stmt->expr(0)->lhs()->unparsestdout();
       Extract_Stmt(st->lexNext()); //extracting nested INDEPENDENT directive
    } 
    else   
       break;  
  }
  nIND = ndo;
// generating assign statement:
//   dvm000(i) = lnumb(num); // line number of stmt
  LINE_NUMBER_AFTER(stmt,stmt);
//generating call to 'bploop' function of performance analizer (begin of parallel interval)
  if(perf_analysis && perf_analysis != 2)
  {
    InsertNewStatementAfter(St_Bploop(OpenInterval(stmt)), cur_st, stmt->controlParent()); //inserting after function call 'lnumb'
  }
  ins_st1 = cur_st;

  iplp = 0; 

//allocating DebRedGroupRef
  ndvm++;
//allocating RedGroupRef
  ndvm++;

  iout = iarg = ndvm; 
  //ndvm += 3*ndo;

//initialization vpart[]
  for(i=0; i<MAX_LOOP_LEVEL; i++)
     vpart[i] = NULL;
// looking through the loop nest 
  for(st=first_do,i=0; i<ndo; st=st->lexNext(),i++) {
     stdo = isSgForStmt(st);
     if(!stdo)
       break;  
     stl = st;
     IND_var[i] = stdo->symbol(); 
     step[i]   = stdo->step();
     if(!step[i])
       step[i] = & c1.copy();  // by default: step = 1
     init[i] = isSpecialFormExp(stdo->start(),i,iout+i,vpart,IND_var);
     if( init[i] )
         step[i] = & c1.copy(); 
      else
         init[i]   = stdo->start();  
     last[i]   = stdo->end();  
    
     SetDoVar(stdo->symbol());
  }

  iplp=iinp = ndvm;        
  OpenParLoop_Inter(stl,iinp,iinp+ndo,IND_var,ndo);

  // creating LoopVarAddrArray, LoopVarTypeArray,InpInitIndexArray, InpLastIndexArray
  // and InpStepArray  
  /*  for(i=0; i<ndo; i++) 
     doAssignStmtAfter(GetAddres(IND_var[i]));
  */
  ndvm +=ndo;
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( new SgValueExp(LoopVarType(IND_var[i],stmt)));
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( init[i] );
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( last[i] );
  for(i=0; i<ndo; i++)
     doAssignStmtAfter( step[i] );

  ins_st2 = cur_st; 
  pardo_line = first_do->lineNumber();
  DebugParLoop(cur_st,ndo,iinp+2*ndo); 
  //SET_DVM(iinp+2*nloop);      
  cur_st = stl; // set cur_st on last DO satement of loop nest    
  return(1); 
}

SgExpression *ConnectNewList(SgExpression *el1, SgExpression *el2)
{// el1 , el2 - NEW specifications of INDEPENDENT directives
 SgExpression *el;
 if(!el1)
   return(el2);
 if(!el2) 
   return(el1);
 for(el = el1->lhs(); el->rhs(); el = el->rhs())
       ;
 el->setRhs(el2->lhs());
 //el1->lhs()->unparsestdout();
 return(el1);
}

void IEXLoopAnalyse(SgStatement *func)
{ SgStatement *st;
  int i;
  nIEX = 0;
  IEX_var = DoVar;
  for(i=0; i<MAX_LOOP_NEST; i++)
    DoVar[i] = NULL;
  for(st=par_do->controlParent(); st!=func; st=st->controlParent()) {
    if(st->variant() == FOR_NODE)
       IEXLoopBegin(st);
    else
       continue;   
  }
}

void IEXLoopBegin(SgStatement *st)
{
 DoVar[nIEX] = st->symbol();
 nIEX++;
}

void INDLoopBegin()
{//generating Lib-DVM calls for beginning independent loop
 SgSymbol *spat;
 SgStatement *st;
 int iaxis;
 int nr;//number of aligning rules i.e. length of align-loop-index-list
  
  st = cur_st; //store cur_st(pointer to current statement)
  if(!IND_target)
     IND_target = IND_target_R;
  if(! IND_target) {
    err("No target for independent loop", 254, indep_st);
    return;
  }
  spat = IND_target->symbol();   // target array symbol
    //printf("INN_target");
    //IND_target->unparsestdout();
  /*   for HPF error if IND_target is NULL     
  if(!HEADER(spat)) {   
     Error("'%s' isn't distributed array", spat->identifier(), 72,stmt); 
     return(0);
  }
  */
//creating reduction group
  if(redl) {
      irg = iarg-1; 
      redgref = DVM000(irg);
      cur_st = ins_st1;
      doAssignTo_After(redgref, CreateReductionGroup());
      if(debug_regim){
        idebrg = iarg-2;
        doAssignTo_After(DVM000(idebrg), D_CreateDebRedGroup());
      }     
      ReductionListIND1();    
      //ReductionListIND_Err();    
  }

  cur_st = ins_st2;
// creating AxisArray, CoeffArray and ConstArray 
  iaxis = ndvm; 
  nr = doAlignIterationIND();

// generating assign statement:
//   dvm000(i) =
//       mappl(LoopRef, PatternRef, AxisArray[], CoefArray[], ConstArray[],
//              LoopVarAdrArray[], InpInitIndexArray[], InpLastIndexArray[],
//              InpStepArray[], 
//              OutInitIndexArray[], OutLastIndexArray[], OutStepArray[])
  
  doCallAfter( BeginParLoop (iplp, HeaderRef(spat), nIND, iaxis, nr, iarg+3*nIND, iarg));
 
  if(redgref) 
    ReductionListIND2(redgref);
  
  if(IND_refs)
    RemoteVariableListIND();
   
  cur_st = st; //restore cur_st
}

void INDReductionDebug()
{//generating Lib-DVM calls for debugging independent loop (creating reduction group)
 SgStatement *st;
  
  st = cur_st; //store cur_st(pointer to current statement)

//creating reduction group
  if(redl) {
      irg = iarg-1; 
      redgref = DVM000(irg);
      cur_st = ins_st1;
      doAssignTo_After(redgref, CreateReductionGroup());  
      if(debug_regim){
        idebrg = iarg-2;
        doAssignTo_After( DVM000(idebrg), D_CreateDebRedGroup());
      }     
      ReductionListIND1();
      ReductionListIND2(redgref);   
      //ReductionListIND_Err();                
  }
  cur_st = st; //restore cur_st
}

int doAlignIterationIND()
// creating axis_array, coeff_array and  const_array 
// returns counter of elements in align_iteration_list

{ int i,nt,num, use[MAX_LOOP_LEVEL];
  SgExpression * el,*e,*ei,*elbb;
  SgSymbol *ar;
  SgExpression *axis[MAX_LOOP_LEVEL],
               *coef[MAX_LOOP_LEVEL],
               *cons[MAX_LOOP_LEVEL];
  SgValueExp c1(1),c0(0),cM1(-1);
  
  for (i=0; i<MAX_LOOP_LEVEL; i++)
     use[i] = 0;
  //ni = nIND;
  ar = IND_target->symbol();    // array  
   
  //looking through the align_iteration_list 
  nt = 0;          //counter of elements in align_iteration_list
  for(el=IND_target->lhs(); el; el=el->rhs())   {
     e = el->lhs();  //subscript expression
     if(e->variant()==DDOT) {  // ":"
                            /*if(e->variant()==KEYWORD_VAL) { */ // "*"
       axis[nt] = & cM1.copy();
       coef[nt] = & c0.copy();
       cons[nt] = & c0.copy();   
     }
     else  {  // expression
       num = AxisNumOfDummyInExpr(e, IND_var, nIND, &ei, use, indep_st);
       if (num<=0)   {
         axis[nt] = & c0.copy();
         coef[nt] = & c0.copy();
         if((elbb = LowerBound(ar,nt)) != NULL)
           cons[nt] = & (e->copy() - (elbb->copy()));
                   // correcting const with lower bound of array
         else //error situation
           cons[nt] = & (e->copy()); 
       }
       else {
         axis[nt] = new SgValueExp(num); 
         CoeffConst(e, ei,&coef[nt], &cons[nt]); 
         TestReverse(coef[nt],indep_st);     
         if(!coef[nt]){
           err("Wrong iteration-align-subscript in PARALLEL", 160,indep_st);
           coef[nt] = & c0.copy();
           cons[nt] = & c0.copy();
         }  
         else 
         // correcting const with lower bound of array
           if((elbb = LowerBound(ar,nt)) != NULL)
             cons[nt] = &(*cons[nt]  - (elbb->copy()));
       }       
     }
    
     nt++;
  }

  // setting on arrays
  for(i=nt-1; i>=0; i--)
     doAssignStmtAfter(axis[i]);
  for(i=nt-1; i>=0; i--)
     doAssignStmtAfter(ReplaceFuncCall(coef[i]));
  for(i=nt-1; i>=0; i--)
     doAssignStmtAfter(Calculate(cons[i]));
  return(nt);  
}

void ReductionListIND1()
{ 
  SgExpression  *ev, *evc, *loc_var,*len, *loclen;
  int   irv, num_red, ntype,sign, ilen,locindtype;
  SgSymbol *var; 
  SgValueExp c0(0),c1(1);
  reduction_list *er;
  
  //looking through the reduction list
  for(er = redl; er; er=er->next) {
     loc_var = ConstRef(0);
     loclen = &c0;
     locindtype = 0;
     len =&c1;
     ev = er->red_var;
     evc=&(ev->copy());
     num_red = er->red_op; 
     if( !num_red) 
        err("Wrong reduction operation name", 70, indep_st); 
     var = ev->symbol();
     if(isSgVarRefExp(ev))
             ;
     else if( isSgArrayRefExp(ev)) { 
               if(!ev->lhs()){ //whole array
                 if(Rank(var)>1)
                   Error("Wrong reduction variable '%s'", var->identifier(), 151, indep_st); 
                 len = ArrayDimSize(var,1); // size of vector
                 if(!len || len->variant()==STAR_RANGE){
                   Error("Wrong reduction variable '%s'", var->identifier(), 151, indep_st); 
                   len = &c1;
                 }
                 evc->setLhs(new SgExprListExp(*Exprn(LowerBound(var,0))));
               }  
            }
     else
        err("Wrong reduction variable",151,indep_st); 
     ntype = VarType(var); //RedVarType(var)
     if(!ntype)
        Error("Wrong type of reduction variable '%s'", var->identifier(), 152,indep_st);
     sign = 1;
     ilen = ndvm; // index for RedArrayLength
     doAssignStmtAfter(len);
     doAssignStmtAfter(loclen);
     irv = ndvm; // index for RedVarRef
     if(! only_debug)
       doAssignStmtAfter(ReductionVar(num_red,evc,ntype,ilen, loc_var, ilen+1,sign));
     er->ind = irv;
     if(debug_regim) {
       doCallAfter(D_InsRedVar(DVM000(idebrg),num_red,evc,ntype,ilen, loc_var, ilen+1,locindtype));
     }
  }   
     return;
  }

void ReductionListIND2(SgExpression *gref)
{ reduction_list *er;
//looking through the reduction list
  if(only_debug) return;
  for(er = redl; er; er=er->next) 
     doCallAfter(InsertRedVar(gref,er->ind,(only_debug ? 0 : iplp)));
}

void ReductionListIND_Err()
{ reduction_list *er;
//looking through the reduction list
  for(er = redl; er; er=er->next) 
     Error("Reduction statement inside the range of INDEPENDENT loop, '%s' is reduction variable", er->red_var->symbol()->identifier(), 255, indep_st);
}

void OffDoVarsOfNest(SgStatement *end_stmt)
{ 
  SgStatement *parent;
  SgForStmt *do_st;
  parent = end_stmt->controlParent();
  OffDoVar(parent->symbol());
  if(!end_stmt->label()) // ENDDO is end of DO constuct
    return;
  parent = parent->controlParent();
  while((do_st=isSgForStmt(parent)) && do_st->endOfLoop() 
       && ( LABEL_STMTNO(do_st->endOfLoop()->thelabel)==LABEL_STMTNO(end_stmt->label()->thelabel)))     {
   OffDoVar(parent->symbol());
   parent = parent->controlParent();
 }
 return;
}
/*
void  RemoteVariableListIND()
{ IND_ref_list *el;
  int ibg,ishg,ikind,ibuf,ishw,iaxis,ideb,iq;
  SgSymbol *ar, *b;
  SgExpression *ind_deb[7],*head, *shgref, *bgref;
  int j, n, buf_size, shw_size, rank, static_sign;
  SgValueExp c0(0),cm1(-1);
  SgStatement *if_st,*end_st,*cp, *cp1,*endif_st,*else_st; 
  
  if(!IND_refs) return;
 
  cp = cp1 = cur_st->controlParent();
  if( !one_inquiry){
    ishg = ndvm;  shgref = DVM000(ishg);
    ibg = ndvm+1; bgref  = DVM000(ibg);
    doAssignStmtAfter(ConstRef(0)); // dvm000(ishg) = 0
    doAssignStmtAfter(ConstRef(0)); // dvm000(ibg)  = 0
    static_sign = 0; 
  }
  else {
    iq   = nhpf++;
    InitInquiryVar(iq);
    if_st = doIfThenConstrForIND(HPF000(iq), 0, 1, 0, cur_st, cp);
    cur_st = if_st;
    doAssignTo_After(HPF000(iq), ConstRef(1)); // hpf000(iq) = 1 :inquiry has done
    ishg = nhpf++;  shgref = HPF000(ishg);
    ibg  = nhpf++; bgref  = HPF000(ibg);
    doAssignTo_After(shgref, ConstRef(0)); // hpf000(ishg) = 0
    doAssignTo_After( bgref, ConstRef(0)); // hpf000(ibg)  = 0
    static_sign = 1; 
    cp = if_st;
  }  
  ikind = ndvm++;
  //looking through the IND_reference list
  for(el=IND_refs; el; el=el->next){
     ar = el->rmref->symbol();
     rank = Rank(ar); 
       // looking through the index list of remote variable
       //for(es= el->rmref->lhs(),n=0; es; es= es->rhs(),n++)
       //
     for(n = 0; n<7 && el->axis[n]; n++)
        if( el->axis[n]->valueInteger() == 0)
           ind_deb[n] = &(el->cons[n]->copy());
        else
           ind_deb[n] = &cm1.copy();
     //allocating  buffer header (for remote data)  and arrays of shadow widths
     buf_size = (el->nc) ? 2*(el->nc)+2 : 4; //memory size for buffer
     if( !one_inquiry){
       ibuf = ndvm;
       ndvm+= buf_size;
       b = dvmbuf; //or NULL
     } else {
       ibuf = nhpf;
       nhpf+= buf_size;
       b = hpfbuf;
     }
     ishw = ndvm;
     shw_size = 2*rank;
     //size = (buf_size > shw_size) ? buf_size : shw_size;
     ndvm+= shw_size;
     //generating inquiry for kind of data access
     iaxis = ndvm; 
     for(j=n-1; j>=0; j--)
        doAssignStmtAfter(el->axis[j]);       
     for(j=n-1; j>=0; j--)
        doAssignStmtAfter(ReplaceFuncCall(el->coef[j]));
     for(j=n-1; j>=0; j--)
        doAssignStmtAfter(Calculate(el->cons[j]));

     head = HeaderRef(el->rmref->symbol());
     doAssignTo_After(DVM000(ikind), RemoteAccessKind(head, header_rf(b,ibuf,1),static_sign,iplp,iaxis,iaxis+n,iaxis+2*n,ishw,ishw+rank));
     //SET_DVM(ishw);
     SET_DVM(iaxis); 
     //generating IF(dvm000(ikind).EQ.3) THEN ...ELSE...ENDIF
     if_st = doIfThenConstrForIND(DVM000(ikind), 3, 1, 1, cur_st, cp);         
     end_st = endif_st = if_st->lexNext()->lexNext(); //END IF statement
     else_st  = if_st->lexNext(); // ELSE statement

     //IF(dvm000(ibg).EQ.0) THEN ...ENDIF
     //   hpf000(ibg)
     if_st = doIfThenConstrForIND(bgref, 0, 1, 0, if_st, if_st);
     cur_st = if_st;
     doAssignTo_After(bgref,CreateBG(static_sign,1));//creating group of remote data buffer
     where = else_st;
     doAssignStmt(InsertRemBuf(bgref, header_rf(b,ibuf,1)));//inserting buffer in group
     if(dvm_debug) {
        ideb = ndvm;
        for(j=n-1; j>=0; j--)
           doAssignStmt(ReplaceFuncCall(ind_deb[j]));
        InsertNewStatementBefore(D_RmBuf(head, GetAddresDVM( header_rf(b,ibuf,1)),n,ideb),else_st);
     }
     BufferHeaderCopy(b,ibuf, n, el);
    
     cur_st = else_st; // generating ELSE body
     //generating IF(dvm000(ikind).EQ.2) THEN ...ELSE...ENDIF
     if_st = doIfThenConstrForIND(DVM000(ikind), 2, 1, 0, else_st, else_st);
     end_st = if_st->lexNext(); //END IF statement 
     //IF(dvm000(ishg).EQ.0) THEN ...ENDIF
     //   hpf000(ishg)
     if_st = doIfThenConstrForIND(shgref, 0, 1, 0, if_st, if_st);
     cur_st = if_st;
     CreateBoundGroup(shgref); //creating group of shadow edges
     where  = end_st;
     doAssignStmt(InsertArrayBound(shgref, head, ishw, ishw+rank, 1)); //corner = 1 !!!
                                                                //inserting shadow in group
     //ishsign = ndvm;
     //maxsh = doShadowSignArray(el); see DepList(),doDepLengthArrays()
     //doAssignStmt(InsertArrayBoundDep(shgref, head, ishw, ishw+rank, maxsh, ishsign));
     cur_st = end_st;
     ArrayHeaderCopy(n,el);     
 
     SET_DVM(ishw);
     cur_st = endif_st;
  }
  if(one_inquiry)
     cur_st =  cur_st->lexNext();
  //IF(dvm000(ishg).NE.0) THEN {executing SHADOW group} ENDIF
  //   hpf000(ishg)
  if_st =  doIfThenConstrForIND(shgref, 0, 0, 0, cur_st, cp1);
  end_st = if_st->lexNext(); //END IF statement
  cur_st = if_st;
  doAssignStmtAfter(StartBound(shgref));  // starting exchange of shadow edges
  FREE_DVM(1);
  doAssignStmtAfter(WaitBound (shgref));// waiting completion of shadow edges exchange
  FREE_DVM(1);
  //IF(dvm000(ibg).NE.0) THEN {executing REMOTE group} ENDIF
  //   hpf000(ibg)
  if_st =  doIfThenConstrForIND(bgref, 0, 0, 0, end_st, cp1);
  cur_st = if_st;
  doAssignStmtAfter(LoadBG(bgref)); // starting load of buffer group
  FREE_DVM(1);
  doAssignStmtAfter(WaitBG(bgref));// waiting completion of  buffer group load
  FREE_DVM(1);
  
  if( one_inquiry)
    {SET_HPF(nhpf);}
  else 
    {SET_HPF(1);}
  return;
}
*/

void  RemoteVariableListIND()
{ IND_ref_list *el;
  int ibg,ishg,ikind,ibuf,ishw,iaxis,ideb,iq;
  SgSymbol *ar, *b;
  SgExpression *ind_deb[7],*head, *shgref, *bgref;
  int j, n, buf_size, shw_size, rank, static_sign;
  SgValueExp c0(0),cm1(-1);
  SgStatement *if_st,*end_st,*cp, *cp1,*endif_st,*else_st; 
  
  if(!IND_refs) return;

  cp = cp1 = cur_st->controlParent();
  if( !one_inquiry){
    ishg = ndvm;  shgref = DVM000(ishg);
    ibg = ndvm+1; bgref  = DVM000(ibg);
    doAssignStmtAfter(ConstRef(0)); // dvm000(ishg) = 0
    doAssignStmtAfter(ConstRef(0)); // dvm000(ibg)  = 0
    static_sign = 0; 
  }
  else {
    iq   = nhpf++;
    InitInquiryVar(iq);
    if_st = doIfThenConstrForIND(HPF000(iq), 0, 1, 0, cur_st, cp);
    cur_st = if_st;
    doAssignTo_After(HPF000(iq), ConstRef(1)); // hpf000(iq) = 1 :inquiry has done
    ishg = nhpf++;  shgref = HPF000(ishg);
    ibg  = nhpf++; bgref  = HPF000(ibg);
    doAssignTo_After(shgref, ConstRef(0)); // hpf000(ishg) = 0
    doAssignTo_After( bgref, ConstRef(0)); // hpf000(ibg)  = 0
    static_sign = 1; 
    cp = if_st;
  }  
  ikind = ndvm++;
  //looking through the IND_reference list
  for(el=IND_refs; el; el=el->next){
     ar = el->rmref->symbol();
     rank = Rank(ar); 
         // looking through the index list of remote variable
         //for(es= el->rmref->lhs(),n=0; es; es= es->rhs(),n++)
         
     for(n = 0; n<7 && el->axis[n]; n++)
        if( el->axis[n]->valueInteger() == 0)
           ind_deb[n] = &(el->cons[n]->copy());
        else
           ind_deb[n] = &cm1.copy();
     //allocating  buffer header (for remote data)  and arrays of shadow widths
     buf_size = (el->nc) ? 2*(el->nc)+2 : 4; //memory size for buffer
     if( !one_inquiry){
       ibuf = ndvm;
       ndvm+= buf_size;
       b = dvmbuf; //or NULL
     } else {
       ibuf = nhpf;
       nhpf+= buf_size;
       b = hpfbuf;
     }
     ishw = ndvm;
     shw_size = 2*rank;
     //size = (buf_size > shw_size) ? buf_size : shw_size;
     ndvm+= shw_size;
     //generating inquiry for kind of data access
     iaxis = ndvm; 
     for(j=n-1; j>=0; j--)
        doAssignStmtAfter(el->axis[j]);       
     for(j=n-1; j>=0; j--)
        doAssignStmtAfter(ReplaceFuncCall(el->coef[j]));
     for(j=n-1; j>=0; j--)
        doAssignStmtAfter(Calculate(el->cons[j]));

     head = HeaderRef(el->rmref->symbol());
     doAssignTo_After(DVM000(ikind), RemoteAccessKind(head, header_rf(b,ibuf,1),static_sign,iplp,iaxis,iaxis+n,iaxis+2*n,ishw,ishw+rank));
     //SET_DVM(ishw);
     SET_DVM(iaxis); 
     //generating IF(dvm000(ikind).EQ.4) THEN ...ELSE...ENDIF
     if_st = doIfThenConstrForIND(DVM000(ikind), 4, 1, 1, cur_st, cp);         
     end_st = endif_st = if_st->lexNext()->lexNext(); //END IF statement
     else_st  = if_st->lexNext(); // ELSE statement

     //IF(dvm000(ibg).EQ.0) THEN ...ENDIF
     //   hpf000(ibg)
     if_st = doIfThenConstrForIND(bgref, 0, 1, 0, if_st, if_st);
     cur_st = if_st;
     doAssignTo_After(bgref,CreateBG(static_sign,1));//creating group of remote data buffer
     where = else_st;
     doAssignStmt(InsertRemBuf(bgref, header_rf(b,ibuf,1)));//inserting buffer in group
     if(dvm_debug) {
        ideb = ndvm;
        for(j=n-1; j>=0; j--)
           doAssignStmt(ReplaceFuncCall(ind_deb[j]));
        InsertNewStatementBefore(D_RmBuf(head, GetAddresDVM( header_rf(b,ibuf,1)),n,ideb),else_st);
     }
     BufferHeaderCopy(b,ibuf, n, el);
    
     cur_st = else_st; // generating ELSE body
     ArrayHeaderCopy(n,el);  
     //generating IF(dvm000(ikind).NE.1) THEN ...ELSE...ENDIF
     if_st = doIfThenConstrForIND(DVM000(ikind), 1, 0, 0, else_st, else_st);
     end_st = if_st->lexNext(); //END IF statement 
    //generating IF(dvm000(ikind).EQ.2) THEN {corner = 0} ELSE {corner = 1} ENDIF
     cur_st = doIfThenConstrForIND(DVM000(ikind), 2, 1, 1, if_st, if_st);
     doCallAfter(InsertArrayBound(shgref, head, ishw, ishw+rank, 0)); 
                                           //inserting shadow in group with FullShadowSign=0
     //icorn = ndvm++;
     //doAssignTo_After(DVM000(icorn),new SgValueExp(0)); //corner = 0     
     cur_st = cur_st->lexNext(); // ELSE
     doCallAfter(InsertArrayBound(shgref, head, ishw, ishw+rank, 1)); 
                                           //inserting shadow in groupwith FullShadowSign=1
     //doAssignTo_After(DVM000(icorn),new SgValueExp(1)); //corner = 1
     //IF(dvm000(ishg).EQ.0) THEN ...ENDIF
     //   hpf000(ishg)
     if_st = doIfThenConstrForIND(shgref, 0, 1, 0, if_st, if_st);
     cur_st = if_st;
     CreateBoundGroup(shgref); //creating group of shadow edges
     where  = end_st;
     //doAssignStmt(InsertArrayBound(shgref, head, ishw, ishw+rank, icorn)); 
                                                                //inserting shadow in group
     //ishsign = ndvm;
     //maxsh = doShadowSignArray(el); see DepList(),doDepLengthArrays()
     //doAssignStmt(InsertArrayBoundDep(shgref, head, ishw, ishw+rank, maxsh, ishsign));
        //cur_st = end_st;
        //  ArrayHeaderCopy(n,el);     
 
     SET_DVM(ishw);
     cur_st = endif_st;
  }
  if(one_inquiry)
     cur_st =  cur_st->lexNext();
  //IF(dvm000(ishg).NE.0) THEN {executing SHADOW group} ENDIF
  //   hpf000(ishg)
  if_st =  doIfThenConstrForIND(shgref, 0, 0, 0, cur_st, cp1);
  end_st = if_st->lexNext(); //END IF statement
  cur_st = if_st;
  doCallAfter(StartBound(shgref));  // starting exchange of shadow edges
  doCallAfter(WaitBound (shgref));// waiting completion of shadow edges exchange
  //IF(dvm000(ibg).NE.0) THEN {executing REMOTE group} ENDIF
  //   hpf000(ibg)
  if_st =  doIfThenConstrForIND(bgref, 0, 0, 0, end_st, cp1);
  cur_st = if_st;
  doAssignStmtAfter(LoadBG(bgref)); // starting load of buffer group
  FREE_DVM(1);
  doAssignStmtAfter(WaitBG(bgref));// waiting completion of  buffer group load
  FREE_DVM(1);
  
  if( one_inquiry)
    {SET_HPF(nhpf);}
  else 
    {SET_HPF(1);}
  return;
}
 

void InitInquiryVar(int iq)
{SgStatement *st;
 st = cur_st;//save cur_st
 cur_st = first_hpf_exec;
 doAssignTo_After(HPF000(iq),ConstRef(0));
 cur_st = st; //resave cur_st
}

/**************************************************************\
*                 Creating header copy                         *
*        (calculating coefficients of address expression)      *
\**************************************************************/
void BufferHeaderCopy(SgSymbol *b, int ibuf, int n, IND_ref_list *el)
// n - number of subscripts in array reference
// hpf000(ihpf)        = getai(dvm000(ibuf))- header address
// hpf000(ihpf+i)      = dvm000(ibuf+i) i=1,...,rank-1
// hpf000(ihpf+rank)   = 1
// hpf000(ihpf+rank+1) = f(dvm000(ibuf+1 : ibuf+2*rank+2)) - calculated 

//
//       Copy          BufferHeader(rank=3)
//      _________        _________
//     | adress  |      |         | 1
//     |_________|      |_________|
//     |    *    | <--- |    *    | 2      
//     |_________|      |_________|
//     |    *    | <--- |    *    | 3
//     |_________|      |_________|
//     |    1    |      |         | 4
//     |_________|      |_________|
//     |calculate|      |         | 5
//     |_________|      |_________| 
//                      | .  .  . |
//                      |_________|
//                      
{int k,ind,rank;
 rank = el->nc; // rank of BufferArray
 ind = el->ind;
   doAssignTo(header_rf(hpfbuf,ind,1),GetAddresDVM(header_rf(b,ibuf,1)));
 for(k=2; k<rank+1; k++)
   doAssignTo(header_rf(hpfbuf,ind,k),header_rf(b,ibuf,k));
 if(rank){
   doAssignTo(header_rf(hpfbuf,ind,rank+2), INDBufferHeaderNplus1(el,b,n,ibuf));
   doAssignTo(header_rf(hpfbuf,ind,rank+1), new SgValueExp(1)); 
 } else
   doAssignTo_After(header_rf(hpfbuf,ind,2),header_rf(b,ibuf,2));
/*
 for(k=1; k<rank; k++)
   doAssignTo(HPF000(ind+k),DVM000(ibuf+k));
 if(rank) {
   doAssignTo(HPF000(ind+rank+1), INDBufferHeaderNplus1(el,n,ibuf));
   doAssignTo(HPF000(ind+rank), new SgValueExp(1)); 
 }
 else
   doAssignTo(HPF000(ind+1), DVM000(ibuf+1));
*/
}

SgExpression *INDBufferHeaderNplus1(IND_ref_list *rme, SgSymbol *ar, int ni, int ihead)   
{
//                                      n
// Header(n+1) = Header(n) -  L1*S1 - SUMMA(Header(n-i+1) * Li * Si)
//                                     i=2   
// Si = 1, if i-th remote subscript is ':', else Si = 0 
// Li = lower bound of i-th array dimension if ':',  Li = Header(2*n-i+3) - minimum of
// of lower bound and upper bound of corresponding do-variable,if a*i+b 
  SgArrayType *artype;
  SgExpression *ehead,*e;
 
  SgSymbol *array;
  int i,ind,j,n,k;
  array = rme->rmref->symbol();
  n = rme->nc;
  //ar = NULL;
  if(!(array->attributes() & DIMENSION_BIT)){// for continuing translation
      return (new SgValueExp(0));
  }
   artype = isSgArrayType(array->type());
   if(!artype) // error
     return(new SgValueExp(0)); //  for continuing translation of procedure 

  ind = n+1; 
  ehead =  header_rf(ar,ihead,ind);

  i=0; j=0;
  for(k = 0; k<ni ; k++)//looking  until first ':' or do-variable-use element
    if( rme->axis[k]->valueInteger() != 0)
      {j = 1; break;}
    else
      i++; 
 if(j == 0) //buffer is of one element
   return(ehead);  
 if(rme->axis[k]->valueInteger() == -1) // :
  if(!(e=LowerBound(array,i)))
    return(new SgValueExp(0)); //  for continuing translation of procedure  
  else
    ehead = &(*ehead -  e->copy());
 else //a*i+b
    ehead = &(*ehead -  (*header_rf(ar,ihead,ind+n+1)));
 for(k = k+1, i++; k<ni ; k++) //continue looking through the index list
     if(rme->axis[k]->valueInteger() == -1){
       ind--; 
       e = artype->sizeInDim(i);
       if(e && e->variant() == DDOT && e->lhs())
         ehead = & (*ehead - (*header_rf(ar,ihead,ind) *
                                                  (LowerBound(array,i)->copy())));
       else
         ehead = & (*ehead - (*header_rf(ar,ihead,ind))); // by default Li=1
   }
   else if(rme->axis[k]->valueInteger() > 0){
       ind--; 
       ehead = & (*ehead - (*header_rf(ar,ihead,ind) * (*header_rf(ar,ihead,ind+n+1))));
   }
  return(ehead);
}

void  ArrayHeaderCopy(int n, IND_ref_list *el)
{ int k, i, ind, rank, num;
  SgSymbol *ar;
  SgExpression *e;
  ind = el->ind;
  rank = el->nc;
  ar = el->rmref->symbol(); //array symbol
  doAssignTo_After(HPF000(ind+rank+1),HeaderRefInd(ar,n+2));//HeaderCopy(rank+1)=Header(n+2)
  num = el->axis[0]->valueInteger();
  i = rank;
  if(num == - 1) { // 1-st index is ':'
      doAssignTo_After(HPF000(ind+rank), new SgValueExp(1));//HeaderCopy(rank) = 1
      i--;
  } else {
    if(num > 0) { // 1-st index is a*IND+b
      doAssignTo_After(HPF000(ind+rank), el->coef[0]);      //HeaderCopy(rank) = a
      i--;
    }
    if(el->cons[0]->lhs() && !INTEGER_VALUE(el->cons[0]->lhs(),0)) // b != 0
      doAssignTo_After(HPF000(ind+rank+1), &(*HPF000(ind+rank+1)+(*el->cons[0]->lhs()))); 
                                     //HeaderCopy(rank+1) = HeaderCopy(rank+1) + b 
  }
  for(k=1; k<n; k++){
    num = el->axis[k]->valueInteger();
    if(num == - 1) { // k-th index is ':'
      doAssignTo_After(HPF000(ind+i),HeaderRefInd(ar,n-k+1));//HeaderCopy(i) = Header(k)
      i--;
    } else {
      if(num > 0) { // k-th index is a*IND+b
        e = INTEGER_VALUE(el->coef[k],1) ? HeaderRefInd(ar,n-k+1) : &(*HeaderRefInd(ar,n-k+1)*(*el->coef[k]));                           
        doAssignTo_After(HPF000(ind+i), e);        //HeaderCopy(i) = a * Header(k) 
        i--;
      }
      if(el->cons[k]->lhs() && !INTEGER_VALUE(el->cons[k]->lhs(),0)) // b!= 0
        doAssignTo_After(HPF000(ind+rank+1), &(*HPF000(ind+rank+1)+(*HeaderRefInd(ar,n-k+1)*(*el->cons[k]->lhs()))));    // HeaderCopy(rank+1) = HeaderCopy(rank+1) + b * Header(k)
    }    
  }
  doAssignTo_After(HPF000(ind), GetAddresDVM(HeaderRefInd(ar,1)));
  return;
}
/**************************************************************\
*       Looking for reduction operation                        *
\**************************************************************/

int NodeBefore=ASSIGN_STAT;
int CompareIfReduction(SgExpression *e1, SgExpression *e2)
{ 
  if(!e1||!e2)  return(0); 
  if(e1->variant() != e2->variant())
      return(0);
  if(e1->variant() != VAR_REF && e1->variant() != ARRAY_REF)
      return(0);
  if(e1->symbol() != e2->symbol())
    return(0);
  if(e1->variant() == ARRAY_REF && !ExpCompare(e1->lhs(),e2->lhs()))
    return(0);
  return (1);    
}   

/* Function returns number of reduction operation 			 */
/* expr_ind is used in order to correspond position of reduction variable*/
/* if SgExpression e - if-condition 'rv ol er' expr_ind=0		 */
/* if SgExpression e - if-condition 'er ol rv' expr_ind=1		 */
/* else expr_ind=0							 */
int ReductionFuncNumber(SgExpression *e,int expr_ind)
{
  switch(e->variant())
      {
      case ADD_OP: return (1);
      case MULT_OP: return (2);
      case AND_OP: return (5);
      case OR_OP: return (6);
      case NEQV_OP: return (7);
      case EQV_OP: return (8);
      case XOR_OP: return (0);
      case FUNC_CALL: {
                        char *red_name;
			red_name   = ((e->symbol())->identifier());
			if(!strcmp(red_name, "max"))
			    return(3);
                        if(!strcmp(red_name, "min"))
			    return(4);
                      };break;
      case LT_OP:
      case LTEQL_OP:  if (expr_ind==0) return (3); /*max*/
                      else return (4);/*min*/
      case GT_OP:
      case GTEQL_OP:  if (expr_ind==0) return (4);
                      else return (3);		      
      default:  return (0);
      }
return 0;
}

/* Function checks if pos_red is in newl-list 			 */
int IsInNewList(SgExpression *pos_red, SgExpression *newl)
{
SgExpression *ExprList;
if (!newl) return 0;
if (!pos_red) return 0;
if (pos_red->variant()!=VAR_REF && pos_red->variant()!=ARRAY_REF) return 0;
for (ExprList=newl;ExprList&&(ExprList->variant()==EXPR_LIST);ExprList=ExprList->rhs())
    {
    if ((ExprList->lhs())->variant()==VAR_REF || (ExprList->lhs())->variant()==ARRAY_REF )
	if (ExprList->lhs()->symbol()==pos_red->symbol())
	    return 1;
    }
return 0;
}
/* Function checks if pos_red is already in reduction-list */
int IsInReductionList(SgExpression *pos_red)
{
reduction_list *rlist=redl;
if (!pos_red) return 0;
if(pos_red->variant()!=VAR_REF && pos_red->variant()!=ARRAY_REF) return 0;
for (;rlist;rlist=rlist->next)
    {
    if (rlist->red_var)
	if (rlist->red_var->symbol()==pos_red->symbol())
	    return 1;
    }
return 0;
}

/* Function checks if pos_red is reduction-variable       *
 * pos_red should be variable, shouldn`t be in newl-list, *
 * pos_red shouldn`t be loop-variable and distribute-array*/
int IsReductionVariable(SgExpression *pos_red, SgExpression *newl)
{
if (!pos_red) return 0;

if (pos_red->variant()!=VAR_REF && pos_red->variant()!=ARRAY_REF)
    {
    return 0;
    }
if (IsInNewList(pos_red,newl)) 
    {
    return 0;
    }
if (IS_DISTR_ARRAY(pos_red->symbol())) 
    {
    return 0;
    }
if (isDoVar(pos_red->symbol())) 
    {
    return 0;
    }
return 1;
}

int IsError(SgExpression *pos_red, SgExpression *newl, int variant)
{
if (!pos_red) return 0;
if (IsInNewList(pos_red,newl)) return 0;
if (variant&&IsReductionVariable(pos_red,newl)) return 0;
if (IS_DISTR_ARRAY(pos_red->symbol())) return 0;
return 1;
}

int FindInExpr(SgExpression *red, SgExpression *expr)
{
if(!expr)  return 0;
if (!red) return 0;
if (red->variant()!=VAR_REF && red->variant()!=ARRAY_REF) return 0;

if(red->variant()==VAR_REF && red->variant() == expr->variant())  
  {
  if (red->symbol()== expr->symbol())
      return 1;
  else return 0;
  }

if(red->variant()==ARRAY_REF && red->variant() == expr->variant())
 { 
   if (red->symbol() == expr->symbol())
      return(ExpCompare(red->lhs(),expr->lhs()));
 }
return (FindInExpr(red,expr->lhs())+FindInExpr(red,expr->rhs()));
}


int IsReductionOp(SgStatement *st, SgExpression *newl)
{
reduction_list *rlist;
int variant=0;
SgExpression *ExprList1,*ExprList2,*Reduction;
ExprList1=ExprList2=Reduction=NULL;
if(st || newl) 
    {
    if (st->variant() == ASSIGN_STAT)
	{
	ExprList1=st->expr(0);
	ExprList2=st->expr(1);
	//ExprList =st->expr(1);
	if (ExprList2&&(ExprList2->variant() != FUNC_CALL))
	    {
	    if (ExprList2->lhs())
		{
		/* rv=rv op er */
		if (CompareIfReduction(ExprList1,ExprList2->lhs()))
		    {
		      //  ExprList =ExprList2->rhs();
		    Reduction=ExprList2->lhs();
		    variant=11;
		    }
    		else 
		    {
		    if (ExprList2->rhs())
			{
			/* rv=er op rv */
    			if (CompareIfReduction(ExprList1,ExprList2->rhs()))
			    {
			    Reduction=ExprList2->rhs();
			    //    ExprList =ExprList2->lhs();
			    variant=12;
			    }
			}
		    }
		}
	    }
	else
	    {
	    /* rv=f(rv,er) or rv=f(er,rv) */
	    char *red_name;
	    red_name   = ((ExprList2->symbol())->identifier());
	    if(!strcmp(red_name, "max")||!strcmp(red_name, "min"))
		{
		if (ExprList2->lhs()&&((ExprList2->lhs())->variant()==EXPR_LIST))
		    {
		    /* rv=f(rv,er) */
		    if (CompareIfReduction(ExprList1,ExprList2->lhs()->lhs()))
			{
			variant=21;
			Reduction=(ExprList2->lhs())->lhs();
			// ExprList=(ExprList2->lhs())->rhs();
			}
    		    else 
			{
			/* rv=f(er,rv) */
			if (ExprList2->lhs()->rhs()&&CompareIfReduction(ExprList1,ExprList2->lhs()->rhs()->lhs()))
			    {
			    variant=22;
			    Reduction=ExprList2->lhs()->rhs()->lhs();
			    // ExprList=ExprList2->lhs()->lhs();
			    }
			}
		    }
		}
	    if (!variant)
		{
		if (IsError(ExprList1,newl,variant))
		    err("Illegal statement in the range of parallel loop",94,st);
		return (0);
		}		    		
	    }
	}
      if (IsError(ExprList1,newl,variant))
	{
	/*We need check variant 'if ( rv ol er ) rv = er' or 'if ( er ol rv ) rv = er'*/
	if (NodeBefore!=LOGIF_NODE)
	    err("Illegal statement in the range of parallel loop",94,st); 	
	return (0);
	}
      NodeBefore=ASSIGN_STAT;	      
      if (Reduction&&variant)  
	{
	 if (IsReductionVariable(ExprList1,newl))
	     {
	     if (IsInReductionList(Reduction)||!ReductionFuncNumber(ExprList2,0))
	         {
		 err("Illegal statement in the range of parallel loop",94,st); 	
		 return (0);
		 }
	     rlist= new reduction_list;
	     if (rlist)
	         {
		 if (!redl) rlist->next=NULL;
		 else rlist->next=redl;
	         rlist->red_op=ReductionFuncNumber(ExprList2,0);
                 rlist->red_var=&(Reduction->copy());
                 if(rlist->red_var->variant() == ARRAY_REF)
                   rlist->red_var->setLhs(NULL);
	         redl=rlist;
	         }
	     else return 0;
	     return 1;
	    }
	}
    return 0;	    
    }
 else
  return 0;
}

int IsLIFReductionOp(SgStatement *st, SgExpression *newl)
{
SgStatement *assign;
PTR_BFND abif;
int variant=0;
if(st || newl) 
  {
  reduction_list *rlist;
  /*'if ( rv ol er ) rv = er' or 'if ( er ol rv ) rv = er'*/
  NodeBefore=LOGIF_NODE;
  if (st&&(st->variant()==LOGIF_NODE))
      {
      /* assign = 'rv = er'*/
      abif= BIF_BLOB1(st->thebif) ? BLOB_VALUE(BIF_BLOB1(st->thebif)):(PTR_BFND)NULL;
      assign=new SgStatement(abif);
      if (assign&&(assign->variant()==ASSIGN_STAT))
          {
          if (assign->expr(0)&&(assign->expr(0)->variant()==VAR_REF))
	      if (st->expr(0)&&((st->expr(0)->lhs()->variant()==VAR_REF)||(st->expr(0)->rhs()->variant()==VAR_REF)))
	          {
	          if (st->expr(0)->lhs()->variant()==VAR_REF)
		      {
		      if (st->expr(0)->lhs()->symbol()==assign->expr(0)->symbol())
		         if (!FindInExpr(st->expr(0)->lhs(),st->expr(0)->rhs())&&!FindInExpr(st->expr(0)->lhs(),assign->expr(1)))
			     {
			     /*if ( rv ol er ) rv = er*/
			     variant= 31;
			     /*fprintf(stderr,"variant 31\n");*/
			     }
		      }
		 else if (st->expr(0)->rhs()->symbol()==assign->expr(0)->symbol())
		         if (!FindInExpr(st->expr(0)->rhs(),st->expr(0)->lhs())&&!FindInExpr(st->expr(0)->rhs(),assign->expr(1)))
		             { 
			     /*if ( er ol rv ) rv = er*/
			     variant= 32;
			     /*fprintf(stderr,"variant 32\n");*/
			     }
		 }
         if (IsError(assign->expr(0),newl,variant))
	    {
	    err("Illegal statement in the range of parallel loop",94,st); 	
	    return (0);
	    }
         if (assign->expr(0)&&variant)  
	    {
	     if (IsReductionVariable(assign->expr(0),newl))
	         {
	         if (IsInReductionList(assign->expr(0))||!ReductionFuncNumber(st->expr(0),0))
	             {
		     err("Illegal statement in the range of parallel loop",94,st); 	
		     return (0);
		     }	     
		 rlist= new reduction_list;
	         if (rlist)
	             {
	             if (!redl) rlist->next=NULL;
		     else rlist->next=redl;
		     if (variant==31) rlist->red_op=ReductionFuncNumber(st->expr(0),0);
	             else rlist->red_op=ReductionFuncNumber(st->expr(0),1);
                     rlist->red_var=&(assign->expr(0)->copy());
                     if(rlist->red_var->variant()==ARRAY_REF)
                        rlist->red_var->setLhs(NULL);
	             redl=rlist;
	             }
	         else return 0;
		 return 1;
		}
	    }
      return 0;
          }
      else return 0;
      }
  }
 else
  return 0;
return 0;
}


/**************************************************************\
*                 Miscellaneous functions                      *
\**************************************************************/
int isNewVar(SgSymbol *s)
{SgExpression *enl, *el;
 enl = indep_st->expr(0) ? indep_st->expr(0)->lhs() : indep_st->expr(0);//NEW variable list
 for(el=enl; el; el=el->rhs()) {
   if(s == el->lhs()->symbol()) // is NEW variable
      return(1);
 }
 return(0);
}

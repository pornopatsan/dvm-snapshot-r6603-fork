/*********************************************************************/
/*  Fortran DVM+OpenMP+ACC                                           */
/*                                                                   */
/*                   Parallel Loop Processing                        */
/*********************************************************************/

#include "dvm.h"

SgStatement *parallel_dir;
SgExpression *spec_accr; 
int iacross;
symb_list *newvar_list;

extern int nloopred; //counter of parallel loops with reduction group
extern int nloopcons; //counter of parallel loops with consistent group
extern int opt_base, opt_loop_range; //set on by compiler options (code optimization options)
extern symb_list *redvar_list;

int ParallelLoop(SgStatement *stmt)
{
  SgSymbol     *do_var[MAX_LOOP_LEVEL];
  SgExpression *step[MAX_LOOP_LEVEL], 
               *init[MAX_LOOP_LEVEL],
               *last[MAX_LOOP_LEVEL],
               *vpart[MAX_LOOP_LEVEL];
  SgExpression *dovar;
  SgValueExp c1(1);
  int i=0, nloop=0, ndo=0, iout;
  SgStatement *stl, *st, *first_do;
  SgForStmt *stdo;
  int ub; /*OMP*/
  SgSymbol  *newj = NULL; /*OMP*/
  SgExpression *clause[12] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

  // initialize global variables
  parallel_dir = stmt;
  redgref = NULL;
  red_list = NULL;
  irg=0; idebrg=0;
  iconsg=0; idebcg=0;
  consgref = NULL;
  iacross = 0;
  newvar_list = NULL;

  ub = 0; /*OMP*/
  if (!OMP_program) {/*OMP*/
     first_do = stmt -> lexNext();// first DO statement of the loop nest
  } else {
     first_do = GetLexNextIgnoreOMP(stmt);// first DO statement of the loop nest /*OMP*/
     newj = ChangeParallelDir (stmt);
  }
  
//analysis of clauses
  CheckClauses(stmt,clause);

  int interface = 0; /*ACC*/
//interface selection: 0 - RTS1, 1- RTS1+RTS2(by handler), 2 - RTS2(by handler)
  if(IN_COMPUTE_REGION || parloop_by_handler) 
     interface = 1; 
  if(parloop_by_handler == 2)   
     interface = WhatInterface(stmt);
  
//initialization vpart[]
  for(i=0; i<MAX_LOOP_LEVEL; i++)
     vpart[i] = NULL;
  
//looking through the do_variables list
  if(opt_loop_range) CreateIndexVariables(stmt->expr(2));
  for(dovar=stmt->expr(2); dovar; dovar=dovar->rhs())
     nloop++;

  LINE_NUMBER_AFTER(stmt,stmt); // line number of PARALLEL directive

//generating call to 'bploop' function of performance analizer (begin of parallel interval)
  if(perf_analysis && perf_analysis != 2)
     InsertNewStatementAfter(St_Bploop(OpenInterval(stmt)), cur_st, stmt->controlParent()); //inserting after function call 'lnumb'

  //par_st = cur_st;

//renewing loop-header's variables (used in start-expr, end-expr, step-expr)
  if(IN_COMPUTE_REGION || parloop_by_handler)      /*ACC*/
     ACC_RenewParLoopHeaderVars(first_do,nloop);
 
//allocating LoopRef and OutInitIndexArray,OutLastIndexArray,OutStepArray 
  iplp = ndvm++;
  if(interface != 2)
  {
     iout = ndvm; 
     ndvm += 3*nloop;
  }  
//looking through the loop nest 
  for(st=first_do,stl=NULL,i=0; i<nloop; st=st->lexNext(),i++) {
     stdo = isSgForStmt(st);
     if(!stdo)
       break;     
     else if( stl && !TightlyNestedLoops_Test(stl,st))
       err("Non-tightly-nested loops",339,st);       
        
     stl = st;
       //if(opt_loop_range) {
     ChangeDistArrayRef(stdo->start());
     ChangeDistArrayRef(stdo->end());
     ChangeDistArrayRef(stdo->step());
       // }
     do_var[i] = stdo->symbol(); 
     step[i]   = stdo->step();
     if(!step[i])
       step[i] = & c1.copy();  // by default: step = 1
     init[i] = isSpecialFormExp(stdo->start(),i,iout+i,vpart,do_var);
     if( init[i] )
         step[i] = & c1.copy(); 
      else
         init[i]   = stdo->start();
  
     last[i]   = stdo->end();  
    
     if (OMP_program) {/*OMP*/      
         if (newj != NULL) {/*OMP*/
             if (ub == 0) {/*OMP*/
                 if (isOmpGetNumThreads(last[i])) ub=1;/*OMP*/
                 if (ub == 0) {/*OMP*/
                     isOmpGetNumThreads(init[i]);/*OMP*/
                     ub=2;/*OMP*/
                 }/*OMP*/
             }  /*OMP*/
         } /*OMP*/
     } /*OMP*/    
     // setting new loop parameters
     if(!opt_loop_range) {
        if(vpart[i]) 
           stdo->setStart(*DVM000(iout+i)+ (*vpart[i]));//special form
                                                       //step is not replaced
        else { 
           stdo->setStart(*DVM000(iout+i));         
        }
        stdo->setEnd(*DVM000(iout+i+nloop));
     }
     else
        stdo->setEnd(*DVM000(iout+i+nloop) - *new SgVarRefExp(*INDEX_SYMBOL(do_var[i])));    

     if(dvm_debug) 
        SetDoVar(stdo->symbol());      
  }

  ndo = i; 

// test whether the PARALLEL directive is correct
  if( !TestParallelDirective(stmt, nloop, ndo, first_do) )
     return(0);    // directive is ignored

  if(interface == 2)
     Interface_2(stmt,clause,init,last,step,nloop,ndo,first_do); //,iout,stl,newj,ub);
  else
     Interface_1(stmt,clause,do_var,init,last,step,nloop,ndo,first_do,iplp,iout,stl,newj,ub);

  cur_st = st->lexPrev();  // set cur_st on last DO satement of loop nest
        // cur_st = stl->lexNext(); 

  return(1);
  
}

void CopyHeaderElems(SgStatement *st_after)
{symb_list *sl;
 SgStatement *stat;
 SgExpression *e;
 int i,rank;
 coeffs *c;
 stat=cur_st;
 cur_st= st_after; //par_st;
 for(sl=dvm_ar;sl;sl=sl->next) {
    c = AR_COEFFICIENTS(sl->symb); //((coeffs *) sl->symb-> attributeValue(0,ARRAY_COEF));
    
    rank=Rank(sl->symb);
    for(i=2;i<=rank;i++)
       doAssignTo_After(new SgVarRefExp(*(c->sc[i])), header_ref(sl->symb,i));
    e = opt_base ? (&(*header_ref(sl->symb,rank+2) + * new SgVarRefExp(*(c->sc[1])))) :  header_ref(sl->symb,rank+2);
    doAssignTo_After(new SgVarRefExp(*(c->sc[rank+2])), e);
    //doAssignTo_After(new SgVarRefExp(*(c->sc[rank+2])), header_ref(sl->symb,rank+2));
 }
 cur_st=stat;
 //dvm_ar=NULL;
}

void EndOfParallelLoopNest(SgStatement *stmt, SgStatement *end_stmt, SgStatement *par_do,SgStatement *func)

{ //stmt is last statement of parallel loop or is body of logical IF , which
  // is last statement
       SgStatement *go_stmt;
   
       if(HPF_program) {
                                 //first_hpf_exec = first_dvm_exec;
          INDLoopBegin(); 
          OffDoVarsOfNest(end_stmt); 
       } else if(!IN_COMPUTE_REGION && !parloop_by_handler) {     /*ACC*/   
          CopyHeaderElems(parallel_dir->lexNext());
          dvm_ar=NULL;
       }
               
       // replacing the label of DO statements locating  above parallel loop  in nest,
       // which is ended by stmt(or stmt->controlParent()),
       // by new label and inserting CONTINUE with this label 
       ReplaceDoNestLabel_Above(end_stmt, par_do, GetLabel());

       if(dvm_debug) {
          CloseDoInParLoop(end_stmt); //on debug regim end_stmt==stmt
          end_stmt = cur_st; 
       } else if(perf_analysis == 4 && !IN_COMPUTE_REGION && !parloop_by_handler) { // RTS calls can not be inserted into the handler     
          SeqLoopEndInParLoop(end_stmt,stmt);
          end_stmt = cur_st; 
       }
       if(!IN_COMPUTE_REGION && !parloop_by_handler) {
       // generating GO TO statement:  GO TO begin_lab
       // and inserting it after last statement of parallel loop nest 
          go_stmt = new SgGotoStmt(*begin_lab);     
          go_stmt->addAttribute (OMP_MARK); /*OMP*/
          cur_st->insertStmtAfter(*go_stmt,*par_do->controlParent());
          cur_st = go_stmt; // GO TO statement
          SgStatement *continue_stat = new SgStatement(CONT_STAT); /*OMP*/      
          continue_stat->addAttribute (OMP_MARK); /*OMP*/
          InsertNewStatementAfter( continue_stat,cur_st,cur_st->controlParent()); /*OMP*/     
       }          
       if(dvm_debug) {
       // generating call statement : call dendl(...)
          CloseParLoop(end_stmt->controlParent(),cur_st,end_stmt);
       }
       if(!dvm_debug && stmt->lineNumber())
       {
          LINE_NUMBER_AFTER_WITH_CP(stmt,cur_st,par_do->controlParent());
       }  
       // generating statements for special ACROSS:
       if(iacross == -1){
          SendArray(spec_accr);
          iacross = 0;
       }
       if(IN_COMPUTE_REGION)  /*ACC*/
       // generating call statement to unregister remote_access buffers: 
       // call dvmh_destroy_array(...)  
          ACC_UnregisterDvmBuffers();
       if(parloop_by_handler != 2 || (parloop_by_handler==2 && WhatInterface(parallel_dir) != 2))   
       // generating call statement:
       //  call endpl(LoopRef)
          doCallAfter(EndParLoop(iplp));

       // generating statements for ACROSS:
       if(iacross){
          doCallAfter(SendBound(DVM000(iacross)));
          doCallAfter(WaitBound(DVM000(iacross)));
          doCallAfter(DeleteObject_H (DVM000(iacross)));
       }
       // actualizing of reduction variables
       if(redgref)
          ReductionVarsStart(red_list);
     
       if(irg) {//there is synchronous REDUCTION clause in PARALLEL
          // generating call statement:
          //  call strtrd(RedGroupRef)
          doCallAfter(StartRed(redgref));

          // generating call statement:
          //  call waitrd(RedGroupRef)
          doCallAfter(WaitRed(redgref));

          if(IN_COMPUTE_REGION || parloop_by_handler)       /*ACC*/   
             ACC_ReductionVarsAreActual();
           
	  if(idebrg){
             if(dvm_debug)
               doCallAfter( D_CalcRG(DVM000(idebrg)));
             doCallAfter( D_DelRG (DVM000(idebrg)));
          }
          // generating statement:
          //  call dvmh_delete_object(RedGroupRef)     //dvm000(i) = delobj(RedGroupRef)
          doCallAfter(DeleteObject_H(redgref));
       }
      
       // actualizing of consistent arrays
       if(consgref)
          ConsistentArraysStart(cons_list);

       if(iconsg) {//there is synchronous CONSISTENT clause in PARALLEL
         if(IN_COMPUTE_REGION || parloop_by_handler)    /*ACC*/        
         // generating call statement:
         //  call dvmh_handle_consistent(ConsistGroupRef)
            doCallAfter(HandleConsistent(consgref));    
         // generating assign statement:
         //  dvm000(i) = strtcg(ConsistGroupRef)
         doAssignStmtAfter(StartConsGroup(consgref));

         // generating statement:
         //  dvm000(i) = waitcg(ConsistGroupRef)
         doAssignStmtAfter(WaitConsGroup(consgref));
         
         // generating statement:
         //  call dvmh_delete_object(ConsistGroupRef)   //dvm000(i) = delobj(ConsistGroupRef)
         doCallAfter(DeleteObject_H(consgref));
       }

        // generating call eloop(...) - end of parallel interval
        // (performance analyzer function)
       if(perf_analysis && perf_analysis != 2) {
         InsertNewStatementAfter(St_Enloop(INTERVAL_NUMBER,INTERVAL_LINE),cur_st,cur_st->controlParent());
         CloseInterval();
         if(perf_analysis != 4)
           OverLoopAnalyse(func);
       }
       if(!IN_COMPUTE_REGION && !parloop_by_handler) {
       // setting label of ending parallel loop nest
       if(!go_stmt->lexNext()->label())
         (go_stmt->lexNext())->setLabel(*end_lab);
       else                
         go_stmt->insertStmtAfter(*ContinueWithLabel(end_lab), *go_stmt->controlParent());
       }
       // implementing parallel loop nest in compute region:
       // generating host- and cuda-handlers and cuda kernel for loop body
       if(IN_COMPUTE_REGION || parloop_by_handler)          /*ACC*/ 
       {   ACC_ParallelLoopEnd(par_do);
           if(!IN_COMPUTE_REGION)
             DeleteNonDvmArrays();     
       }
           
       //completing REMOTE_ACCESS 
       if(rma && !rma->rmout) 
          RemoteAccessEnd();
          
       SET_DVM(iplp);

} 



void CheckClauses(SgStatement *stmt, SgExpression *clause[])
{
   SgExpression *el,*e; 
// looking through the specification list
  for(el=stmt->expr(1); el; el=el->rhs()) {
     e = el->lhs();            // specification
     switch (e->variant()) {
           case NEW_SPEC_OP:
	     if(!clause[NEW_]){
                clause[NEW_] = e;
             }  else
                err("Double NEW clause",153,stmt);
                break;
           case REDUCTION_OP:
	     if(!clause[REDUCTION_]){
                clause[REDUCTION_] = e;
             }  else
                err("Double REDUCTION clause",154,stmt);
                break;
                 
           case SHADOW_RENEW_OP:
	     if(!clause[SHADOW_RENEW_] && !clause[SHADOW_START_] && !clause[SHADOW_START_]){
                clause[SHADOW_RENEW_] = e;
             }  else
                err("Double shadow-renew-clause",155,stmt);
                break;

           case SHADOW_START_OP:
	     if(!clause[SHADOW_RENEW_] && !clause[SHADOW_START_] && !clause[SHADOW_START_]){
                clause[SHADOW_START_] = e;
             }  else
                err("Double shadow-renew-clause",155,stmt);
                break;

           case SHADOW_WAIT_OP:
	     if(!clause[SHADOW_RENEW_] && !clause[SHADOW_START_] && !clause[SHADOW_START_]){
                clause[SHADOW_WAIT_] = e;
             }  else
                err("Double shadow-renew-clause",155,stmt);
                break;

           case SHADOW_COMP_OP:
	     if(!clause[SHADOW_COMPUTE_]){
                clause[SHADOW_COMPUTE_] = e;
             }  else
                err("Double SHADOW_COMPUTE clause",155,stmt);
                break;

           case REMOTE_ACCESS_OP:
	     if(!clause[REMOTE_ACCESS_]){
                clause[REMOTE_ACCESS_] = e;
             }  else
                err("Double REMOTE_ACCESS clause",156,stmt);
                break;

           case CONSISTENT_OP:
	     if(!clause[CONSISTENT_]){
                clause[CONSISTENT_] = e;
             }  else
                err("Double CONSISTENT clause",296,stmt);       
                break;
  
           case STAGE_OP:
	     if(!clause[STAGE_]){
                clause[STAGE_] = e;
             }  else
                err("Double STAGE clause",298,stmt);
                break; 

           case ACC_PRIVATE_OP:
	     if(!clause[PRIVATE_]){
                clause[PRIVATE_] = e;
             }  else
                err("Double PRIVATE clause",607,stmt);
                break; 

           case ACC_CUDA_BLOCK_OP:
	     if(!clause[CUDA_BLOCK_]){
                clause[CUDA_BLOCK_] = e;
             }  else
                err("Double CUDA_BLOCK clause",608,stmt);
                break; 

           case ACROSS_OP:
	     if(!clause[ACROSS_]){
                clause[ACROSS_] = e;
             }  else
                err("Double ACROSS clause",157,stmt);       
                break;  
     }
  }      
   
  if(clause[SHADOW_COMPUTE_] && clause[REDUCTION_])
     err("Inconsistent clauses: SHADOW_COMPUTE and REDUCTION",443,stmt);

  if(IN_COMPUTE_REGION && ( clause[SHADOW_START_] || clause[SHADOW_WAIT_] || clause[CONSISTENT_] && clause[CONSISTENT_]->symbol() || clause[REMOTE_ACCESS_] && clause[REMOTE_ACCESS_]->symbol()))
     err("Illegal clause of PARALLEL directive in region (SHADOW_START,SHADOW_WAIT,asynchronous CONSISTENT or asynchronous REMOTE_ACCESS)",445,stmt);       

}

int WhatInterface(SgStatement *stmt)
{
   SgExpression *el,*e;
// undistributed parallel loop
   if(!stmt->expr(0))
      return(2);
// is mapped on template? 
   //if(stmt->expr(0)->symbol()->attributes() & TEMPLATE_BIT)
   //   return (1); 
// looking through the specification list of PARALLEL directive
   for(el=stmt->expr(1); el; el=el->rhs()) {
      e = el->lhs();            // specification
      switch (e->variant()) {
           case ACC_PRIVATE_OP:
           case ACC_CUDA_BLOCK_OP:
           case SHADOW_RENEW_OP:
           case SHADOW_COMP_OP:
           case ACROSS_OP:
                break;
           case REDUCTION_OP:
                if(TestReductionClause(e))
                  break;
                else
                  return(1);
           default:
                return (1);
      }
   }
   return (2);
}

int areIllegalClauses(SgStatement *stmt)
{ SgExpression *el;

  for(el=stmt->expr(1); el; el=el->rhs()) 
    if(el->lhs()->variant() != REDUCTION_OP && el->lhs()->variant() != ACC_PRIVATE_OP && el->lhs()->variant() != ACC_CUDA_BLOCK_OP) // && el->lhs()->variant() != ACROSS_OP)
      return 1;
  return 0;                 
}          

int TestParallelDirective(SgStatement *stmt, int nloop, int ndo, SgStatement *first_do)
{ // stmt - PARALLEL directive; nloop - number of items in the do-variable list of directive;
  // ndo - number of loops (do-statements) in the nest
  SgExpression *dovar;  
  SgStatement *st;
  int flag_err=1; //flag of an error message
   
  if(!nloop)  // not determined yet (AnalyzeRegion())
  {  flag_err = 0;
     // first DO statement of the loop nest   
     first_do = OMP_program ? GetLexNextIgnoreOMP(stmt) : stmt->lexNext();   
     //looking through the do_variable list of directive
     for(dovar=stmt->expr(2); dovar; dovar=dovar->rhs())
        nloop++;
 
     //looking through the loop nest 
     for(st=first_do,ndo=0; ndo<nloop; st=st->lexNext(),ndo++) 
     {     
        if(!isSgForStmt(st))
           break;     
     }
  }

  if(ndo == 0) {
    if(flag_err)
      err("Directive PARALLEL must be followed by DO statement", 97, stmt); 
    return(0);
  }

  if(nloop > ndo) { 
    if(flag_err) 
      err("Length of do-variable list in  PARALLEL directive is greater than the number of nested DO statements", 158,stmt);
    return(0);
  }

  for(st=first_do,dovar=stmt->expr(2); dovar; st=st->lexNext(),dovar=dovar->rhs()) 
  { 
    if(dovar->lhs()->symbol() != st->symbol()) {
      if(flag_err)
        err("Illegal do-variable list in PARALLEL directive",159,stmt);      
      return(0);
    }     
  }

  if(!stmt->expr(0)) //undistributed parallel loop
  {
    if(parloop_by_handler != 2)
    {
      if(flag_err)
        warn("PARALLEL directive is ignored, -Opl2 option should be specified",621,stmt);
      return(0);
    } 
    else
      if(areIllegalClauses(stmt))
      {
        if(flag_err)
          err("Illegal clause",150,stmt );
        return(0);
      }
  }

  if(!only_debug && stmt->expr(0) && !HEADER(stmt->expr(0)->symbol())) {
     if(flag_err)   
       Error("'%s' isn't distributed array", stmt->expr(0)->symbol()->identifier(), 72,stmt); 
     return(0);
  }

  return(1);
}

int doParallelLoopByHandler(int iplp, SgStatement *first, SgExpression *clause[], SgExpression *oldGroup, SgExpression *newGroup,SgExpression *oldGroup2, SgExpression *newGroup2)
{ /*ACC*/
    int ilh = ndvm;    
    LINE_NUMBER_AFTER(first,cur_st);      
    cur_st->addComment(ParallelLoopComment(first->lineNumber()));    
    doAssignStmtAfter(LoopCreate_H(cur_region ? cur_region->No : 0, iplp));       
    if (clause[REDUCTION_])  //there is REDUCTION clause in parallel loop
        InsertReductions_H(clause[REDUCTION_]->lhs(), ilh);

    if (clause[CUDA_BLOCK_])  //there is CUDA_BLOCK clause
    {
        int ib;
        ib = ndvm;
        CudaBlockSize(clause[CUDA_BLOCK_]->lhs());
        InsertNewStatementAfter(SetCudaBlock_H(ilh, ib), cur_st, cur_st->controlParent());
    }
    if (oldGroup) // loop with ACROSS clause
        InsertNewStatementAfter(LoopAcross_H(ilh, oldGroup, newGroup), cur_st, cur_st->controlParent());

    if (oldGroup2) // loop with ACROSS clause
        InsertNewStatementAfter(LoopAcross_H(ilh, oldGroup2, newGroup2), cur_st, cur_st->controlParent());
        
    return(ilh);
}

void Interface_1(SgStatement *stmt,SgExpression *clause[],SgSymbol *do_var[],SgExpression *init[],SgExpression *last[],SgExpression *step[],int nloop,int ndo,SgStatement *first_do,int iplp,int iout,SgStatement *stl,SgSymbol *newj,int ub)
{  
  SgStatement *stc,*if_stmt=NULL,*st2=NULL,*st3=NULL;
  SgStatement *stdeb = NULL,*stat = NULL,*stg = NULL,*stcg = NULL;
  SgValueExp c0(0),c1(1);
  SgExpression *stage=NULL,*dopl=NULL,*dovar,*head;
  SgExpression *oldGroup = NULL, *newGroup=NULL;  /*ACC*/
  SgExpression *oldGroup2 = NULL, *newGroup2=NULL; /*ACC*/  
  SgSymbol  *spat;
  int all_positive_step=-1;
  int iacrg=-1,iinp;
  int iaxis,i, isg = 0;
  int nr; //number of aligning rules i.e. length of align-loop-index-list 
  int ag[3] = {0, 0, 0};
  int        step_mask[MAX_LOOP_LEVEL],
             loop_num[MAX_LOOP_LEVEL];


  stc = cur_st;  // saving
  // generating assign statement:
  //   dvm000(iplp) = crtpl(Rank);
                   //iplp = CreateParLoop( nloop); 
  doAssignTo_After(DVM000(iplp),CreateParLoop(nloop));

  if(dvm_debug && dbg_if_regim>1) {  //copy loop nest
     SgStatement *last_st,*lst;
     last_st= LastStatementOfDoNest(first_do);
     if(last_st != (lst=first_do->lastNodeOfStmt()) || last_st->variant()==LOGIF_NODE) 
     { last_st=ReplaceLabelOfDoStmt(first_do,last_st, GetLabel());
       ReplaceDoNestLabel_Above(last_st,first_do,GetLabel());
     }
     stdeb=first_do->copyPtr();         
  }
  //---------------------------------------------------------------------------
  // processing specifications/clauses

     if(clause[NEW_])
        NewVarList(clause[NEW_]->lhs(),stmt);

     if(clause[REDUCTION_])
     {
        red_list = clause[REDUCTION_]->lhs();
        stat = cur_st; //store current statement    
        cur_st = stc; //insert statements for creating reduction group 
		      //before CrtPL i.e. before creating parallel loop             
        if( clause[REDUCTION_]->symbol()) {
           redgref = new SgVarRefExp(clause[REDUCTION_]->symbol());
           doIfForReduction(redgref,1);
           nloopred++;
           stg = doIfForCreateReduction( clause[REDUCTION_]->symbol(),nloopred,0);
        } else {
           irg = ndvm; 
           redgref = DVM000(irg);
           doAssignStmtAfter(CreateReductionGroup());
           if(debug_regim){
              idebrg = ndvm; 
              doAssignStmtAfter( D_CreateDebRedGroup());
           }
           stg = cur_st;//store current statement
        }   
        cur_st = stat; // restore cur_st 

     }
     if(clause[SHADOW_RENEW_])
     {
        isg = ndvm++;// index for BoundGroupRef
        CreateBoundGroup(DVM000(isg));  
        //looking through the array_with_shadow_list
        ShadowList(clause[SHADOW_RENEW_]->lhs(), stmt, DVM000(isg));
        if(ACC_program)      /*ACC*/
        {// generating call statement ( in and out compute region):
         //  call dvmh_shadow_renew( BoundGroupRef)
              
           doCallAfter(ShadowRenew_H(DVM000(isg)));   //(GPU000(ish_gpu),StartShadow_GPU(cur_region->No,DVM000(isg)));
        }
         // generating assign statement:
         //  dvm000(i) = strtsh(BoundGroupRef)
        doCallAfter(StartBound(DVM000(isg)));
     }

     if(clause[SHADOW_START_])      //sh_start
     { 
        SgExpression *sh_start = new SgVarRefExp(clause[SHADOW_START_]->symbol());
        if(ACC_program)      /*ACC*/
        {// generating call statement ( in and out compute region):
         //  call dvmh_shadow_renew( BoundGroupRef)              
           doCallAfter(ShadowRenew_H(sh_start));   
        }
        // generating assign statement:
        //   dvm000(i) = exfrst(LoopRef,BounGroupRef)
        doCallAfter(BoundFirst(iplp,sh_start));
     }

     if(clause[SHADOW_WAIT_])        //sh_wait
     // generating assign statement:
     //   dvm000(i) = imlast(LoopRef,BounGroupRef)
        doCallAfter(BoundLast(iplp,new SgVarRefExp(clause[SHADOW_WAIT_]->symbol())));

     if(clause[SHADOW_COMPUTE_])
     {
        if( (clause[SHADOW_COMPUTE_]->lhs()))
	   ShadowComp(clause[SHADOW_COMPUTE_]->lhs(),stmt,0);
        else 
           doCallAfter(AddBound());             
     }
     if(clause[REMOTE_ACCESS_])
     {
        //adding new element to remote_access directive/clause list
        AddRemoteAccess(clause[REMOTE_ACCESS_]->lhs(),NULL);        
     }
     if(clause[CONSISTENT_])
     {
        SgExpression *e = clause[CONSISTENT_];
        cons_list = e->lhs();
        stat = cur_st; //store current statement    
        cur_st = stc; //insert statements for creating reduction group 
		     //before CrtPL i.e. before creating parallel loop             
        if(  e->symbol()){
           consgref = new SgVarRefExp(e->symbol());
           doIfForConsistent(consgref);
           nloopcons++;
           stcg = doIfForCreateReduction( e->symbol(),nloopcons,0);
        } else {
           iconsg = ndvm; 
           consgref = DVM000(iconsg);
           doAssignStmtAfter(CreateConsGroup(1,1));
                  //!!!??? if(debug_regim){
                  //  idebcg = ndvm; 
                  //  doAssignStmtAfter( D_CreateDebRedGroup());
                  //}
           stcg = cur_st;//store current statement
        }  
        cur_st = stat; // restore cur_st 
     }

     if(clause[STAGE_])
        stage = ReplaceFuncCall(clause[STAGE_]->lhs());

     if(clause[ACROSS_])
     { 
        int k,not_in;
        SgExpression *ea[2],*in_spec,*out_spec;
        SgExpression *e = clause[ACROSS_];
        SgKeywordValExp *kwe;
                 
        all_positive_step = Analyze_DO_steps(step,step_mask,ndo);
        in_spec = NULL;  out_spec = NULL;
        not_in = 0;
        ea[0] = e->lhs();
        ea[1] = e->rhs();
        for (k=0;k<2;k++){
           if(!ea[k]) continue;
           if(ea[k]->variant() != DDOT) { 
              in_spec  = ea[k]; not_in = 0;/*not_in=1*/
           } else {
              if((kwe=isSgKeywordValExp(ea[k]->lhs())) && (!strcmp(kwe->value(),"in")))
                     in_spec  = ea[k]->rhs();
              else
                     out_spec = ea[k]->rhs();
           } 
        }   
        if(ea[0] && ea[1] && (in_spec == NULL || out_spec == NULL))
           err("Wrong ACROSS clause",256 ,stmt); 
        if(not_in && in_spec && !out_spec) { // old implementation
           stat = cur_st;//store current statement    
           cur_st = stc; //insert statements for creating shadow group 
		         //before CrtPL i.e. before creating parallel loop   
           iacross = ndvm++;// index for ShadowGroupRef
           //looking through the dependent_array_list
           if(DepList(e->lhs(), stmt, DVM000(iacross),ANTIDEP)){
              doCallAfter(StartBound(DVM000(iacross)));
              doCallAfter(WaitBound(DVM000(iacross)));
              doAssignStmtAfter(DeleteObject(DVM000(iacross)));
              SET_DVM(iacross+1); 
           }
           if(DepList(e->lhs(), stmt, DVM000(iacross),FLOWDEP)){
              doCallAfter(ReceiveBound(DVM000(iacross)));
              doCallAfter(WaitBound(DVM000(iacross)));
              SET_DVM(iacross+1);
           } else {
              if (iacross == -1)
                 spec_accr = e->lhs();
              else
                 iacross =  0;
           }
           cur_st = stat; // restore cur_st 
        } else  {// new implementation
           iacrg=ndvm; ndvm+=3;
           if(IN_COMPUTE_REGION || parloop_by_handler)
              ndvm+=3;                                       
           if(all_positive_step) //(PositiveDoStep(step,ndo))
              CreateShadowGroupsForAccross(in_spec,out_spec,stmt,ACC_GroupRef(iacrg),ACC_GroupRef(iacrg+1),ACC_GroupRef(iacrg+2),ag,all_positive_step,loop_num);
           else {
              //ag[1] = -1;
              if(out_spec || in_spec->rhs() || stmt->expr(0)->symbol()  != (in_spec->lhs()->variant() == ARRAY_OP ? in_spec->lhs()->lhs()->symbol() : in_spec->lhs()->symbol()))
                 err("Illegal ACROSS-clause",444,stmt);
                    
              DefineLoopNumberForNegStep(step_mask,DefineLoopNumberForDimension(stmt,loop_num),loop_num);
              CreateShadowGroupsForAccrossNeg(in_spec,stmt,ACC_GroupRef(iacrg),ACC_GroupRef(iacrg+2),ag,all_positive_step,loop_num);
              //k=ag[2]; ag[2] = ag[0]; ag[0] = k;                    
           }                  
        }
     }

//------------------------------------------------------------------------------

  iinp = ndvm;    
  if(dvm_debug) 
     OpenParLoop_Inter(stl,iinp,iinp+nloop,do_var,nloop);
// creating LoopVarAddrArray, LoopVarTypeArray,InpInitIndexArray, InpLastIndexArray
// and InpStepArray  
  for(i=0,dovar=stmt->expr(2); i<nloop; i++,dovar=dovar->rhs()) 
     doAssignStmtAfter(GetAddres(do_var[i]));
  
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( new SgValueExp(LoopVarType(do_var[i],stmt)));
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( init[i] );
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( last[i] );
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( step[i] );

// creating AxisArray, CoeffArray and ConstArray 
  spat = (stmt->expr(0))->symbol();   // target array symbol
  head = HeaderRef(spat);
  iaxis = ndvm; 
  nr = doAlignIteration(stmt,NULL);

  if(isg) {
         // generating assign statement:
         //  dvm000(i) = waitsh(BoundGroupRef)
         doCallAfter(WaitBound(DVM000(isg)));
          }
     
// generating assign statement:
//   dvm000(i) =
//       mappl(LoopRef, PatternRef, AxisArray[], CoefArray[], ConstArray[],
//              LoopVarAdrArray[], InpInitIndexArray[], InpLastIndexArray[],
//              InpStepArray[], 
//              OutInitIndexArray[], OutLastIndexArray[], OutStepArray[])
  
  doCallAfter( BeginParLoop (iplp, head, nloop, iaxis, nr, iinp, iout));

  if(redgref) {
    if(!irg) {
        st2 = doIfForCreateReduction( redgref->symbol(),nloopred,1);
        st3 = cur_st;
        ReductionList(red_list,redgref,stmt,stg,st2,0);
        cur_st = st3;
        InsertNewStatementAfter( new SgAssignStmt(*DVM000(ndvm),*new SgValueExp(0)),cur_st,cur_st->controlParent());
    } else  
        ReductionList(red_list,redgref,stmt,stg,cur_st,0); 
  }

  if(consgref) {
    if(!iconsg) {
        st2 = doIfForCreateReduction( consgref->symbol(),nloopcons,1);
        st3 = cur_st;
        ConsistentArrayList(cons_list,consgref,stmt,stcg,st2);
        cur_st = st3;
        InsertNewStatementAfter( new SgAssignStmt(*DVM000(ndvm),*new SgValueExp(0)),cur_st,cur_st->controlParent());
    } else  
        ConsistentArrayList(cons_list,consgref,stmt,stcg,cur_st);      
  }

  if(clause[REMOTE_ACCESS_])  //rvle
    RemoteVariableList(clause[REMOTE_ACCESS_]->symbol(), clause[REMOTE_ACCESS_]->lhs(), stmt);

  if(iacross == -1)
    ReceiveArray(spec_accr,stmt);

  if(clause[ACROSS_] && !clause[STAGE_])    // there is ACROSS clause and is not STAGE clause
    stage = IN_COMPUTE_REGION ? GetStage(first_do,iplp) : &c0.copy();
  
  if(all_positive_step) {
  if(ag[0]) {
       pipeline=1;
       doAssignTo_After(new SgVarRefExp(Pipe), stage);
                                   
       if(ACC_program && ag[2])      /*ACC*/
       // generating call statement ( in and out compute region):
       //  call dvmh_shadow_renew( BoundGroupRef)              
         doCallAfter(ShadowRenew_H (DVM000(iacrg+2) ));   
       doCallAfter(InitAcross(0,(ag[2] ? DVM000(iacrg+2) : ConstRef(0)),DVM000(iacrg)));
       if(IN_COMPUTE_REGION || parloop_by_handler)
       { oldGroup = ag[2] ? DVM000(iacrg+5) : ConstRef(0); /*ACC*/
         newGroup = DVM000(iacrg+3);                       /*ACC*/
       }
     if(ag[1]) {                                        
       doCallAfter(InitAcross(1,  ConstRef(0),  DVM000(iacrg+1)));
       if(IN_COMPUTE_REGION || parloop_by_handler)
       { oldGroup2 =  ConstRef(0);                         /*ACC*/
         newGroup2 =  DVM000(iacrg+4);                      /*ACC*/
       }
      } 
  } 
  else {
    if(ag[1]){
       pipeline=1; 
       doAssignTo_After(new SgVarRefExp(Pipe), stage);    
         
       if(ACC_program && ag[2])                          /*ACC*/
       // generating call statement ( in and out compute region):
       //  call dvmh_shadow_renew( BoundGroupRef)              
         doCallAfter(ShadowRenew_H (DVM000(iacrg+2) ));   

       doCallAfter(InitAcross(1,(ag[2] ? DVM000(iacrg+2) : ConstRef(0)),DVM000(iacrg+1)));
       if(IN_COMPUTE_REGION || parloop_by_handler)        
       { oldGroup = ag[2] ? DVM000(iacrg+5) : ConstRef(0); /*ACC*/
         newGroup = DVM000(iacrg+4);                       /*ACC*/
       }
    } 
    else if(ag[2]){
       //err("SHADOW_RENEW clause is required",257,stmt);
       if(ACC_program)      /*ACC*/
       // generating call statement ( in and out compute region):
       //  call dvmh_shadow_renew( BoundGroupRef)              
         doCallAfter(ShadowRenew_H (DVM000(iacrg+2) ));   
       doCallAfter(StartBound(DVM000(iacrg+2)));
       doCallAfter(WaitBound (DVM000(iacrg+2)));
       if(IN_COMPUTE_REGION || parloop_by_handler)        
       { oldGroup = DVM000(iacrg+5);                      /*ACC*/
         newGroup = ConstRef(0);                          /*ACC*/
       }

    }
  } 
  } else{ //there is negative loop step
       if(ag[0]  || ag[2]) {
       pipeline=1;
       doAssignTo_After(new SgVarRefExp(Pipe), stage);
                                   
       if(ACC_program && ag[2])      /*ACC*/
       // generating call statement ( in and out compute region):
       //  call dvmh_shadow_renew( BoundGroupRef)              
         doCallAfter(ShadowRenew_H (DVM000(iacrg+2) ));   
       doCallAfter(InitAcross(0,(ag[2] ? DVM000(iacrg+2) : ConstRef(0)),(ag[0] ? DVM000(iacrg) : ConstRef(0))));        
       if(IN_COMPUTE_REGION || parloop_by_handler)      
       { oldGroup = ag[2] ? DVM000(iacrg+5) : ConstRef(0); /*ACC*/
         newGroup = ag[0] ? DVM000(iacrg+3) : ConstRef(0); /*ACC*/
       }
    }
  }
  if(dvm_debug) {
      pardo_line = first_do->lineNumber();
      DebugParLoop(cur_st,nloop,iinp+2*nloop);
  } 
 
  StoreLoopPar(init,nloop,iout,NULL);
  StoreLoopPar(last,nloop,iout+nloop,NULL);

  if(opt_loop_range) ChangeLoopInitPar(first_do,nloop,init,stmt->lexNext());//must be after StoreLoopPar

  if (OMP_program == 1) { /*OMP*/
	  if (clause[ACROSS_]) { /*OMP*/
	      ChangeAccrossOpenMPParam (first_do,newj,ub); /*OMP*/
	  } /*OMP*/
  } /*OMP*/


  if(!IN_COMPUTE_REGION && !parloop_by_handler)
  {
   // generating Logical IF statement:
       // begin_lab  IF (DoPL(LoopRef) .EQ. 0) GO TO end_lab
       // and inserting it before  loop nest
       SgStatement *stn = cur_st;
       SgStatement *continue_stat = new SgStatement(CONT_STAT); /*OMP*/
       continue_stat->addAttribute (OMP_MARK);
       InsertNewStatementAfter(continue_stat,cur_st,cur_st->controlParent()); /*OMP*/
       LINE_NUMBER_AFTER(first_do,cur_st);
       begin_lab = GetLabel();
       stn->lexNext()-> setLabel(*begin_lab); 
       end_lab   = GetLabel();
       if(dvm_debug && dbg_if_regim)
       {
           int ino;
	   ino = ndvm;
           doAssignStmtAfter(new SgValueExp(pardo_No)); 
           dopl = doPLmb(iplp,ino);
       } else
         dopl =  doLoop(iplp);
                     //if_stmt = new SgLogIfStmt(SgEqOp(*dopl , c0), *new SgGotoStmt(*end_lab));
                     //if_stmt -> setLabel(*begin_lab); /*29.06.01*/
                     //          BIF_LABEL(stmt->thebif) = NULL;  
       doAssignStmtAfter(dopl);  // podd 17.05.11 (doLoop(iplp));/*OMP*/
       SgGotoStmt *go=new SgGotoStmt(*end_lab);/*OMP*/
       go->addAttribute (OMP_MARK);/*OMP*/
       if_stmt = new SgLogIfStmt(SgEqOp(*DVM000(ndvm-1), c0), *go);/*OMP*/
       if_stmt->addAttribute (OMP_MARK);/*OMP*/
                     //if_stmt = new SgLogIfStmt(SgEqOp(*dopl , c0), *new SgGotoStmt(*end_lab));
                     //cur_st->insertStmtAfter(*if_stmt);
       InsertNewStatementAfter (if_stmt, cur_st, cur_st->controlParent ());/*OMP*/
       if(opt_loop_range) 
       {
            cur_st=if_stmt->lexNext()->lexNext();
            doAssignIndexVar(stmt->expr(2),iout,init);   
       }          
       (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF
                                                               // (error Sage)
  } 
       	      
  if(IN_COMPUTE_REGION || parloop_by_handler)      /*ACC*/
  {    int ilh = doParallelLoopByHandler(iplp, first_do, clause, oldGroup, newGroup,oldGroup2, newGroup2); 
       ACC_CreateParallelLoop(ilh,first_do,nloop,stmt,clause,1);
  }

  if(dvm_debug && dbg_if_regim>1) 
  {
          SgStatement *ifst = new SgIfStmt(*DebugIfNotCondition(), *stdeb); //*new SgStatement(CONT_STAT));// *stdeb); //, *new SgStatement(CONT_STAT));
           
          (if_stmt->lexNext())->insertStmtAfter(*ifst,*if_stmt->controlParent());

          // generating GO TO statement:  GO TO begin_lab
          // and inserting it after last statement of parallel loop nest copy
                // InsertNewStatementBefore(new SgGotoStmt(*begin_lab),ifst->lastNodeOfStmt());
                //(ifst->lastNodeOfStmt())->insertStmtBefore(*new SgGotoStmt(*begin_lab),*ifst);
                //InsertNewStatementAfter(new SgGotoStmt(*begin_lab),stdeb->lastNodeOfStmt(),ifst);
          (stdeb->lastNodeOfStmt())->insertStmtAfter(*new SgGotoStmt(*begin_lab),*ifst);
          TranslateBlock(stdeb);          
   } 

}

void ChangeLoopInitPar(SgStatement*first_do,int nloop,SgExpression *do_init[],SgStatement *after)
{ SgStatement *stat, *st;
  SgForStmt *stdo;
  SgSymbol *s,*do_var, *s_start;
  SgExpression *init;
  int i;
  stat=cur_st;
  cur_st=after;

  for(st=first_do,i=0; i<nloop; st=st->lexNext(),i++) {
    stdo = isSgForStmt(st);
    if(!stdo) break;  
    do_var = stdo->symbol();     
    init   = stdo->start();
//   for(i=0; i<n; i++) 
    if(isSgVarRefExp(init)) {
      s = init->symbol();
      if(s && isInSymbList(newvar_list,s)){
        s_start = CreateInitLoopVar(do_var,s);
        doAssignTo_After(new SgVarRefExp(s_start),&(init->copy()));
        stdo->setStart(*new SgVarRefExp(s_start));
        do_init[i] = stdo->start(); 
      }
    }
  }
  cur_st=stat;
}

int PositiveDoStep(SgExpression *step[], int i)
{int s;
 SgExpression *es;
 if(step[i]->isInteger())
    s=step[i]->valueInteger();
 else if((es=Calculate(step[i]))->isInteger())
    s= es->valueInteger();
 else
 { err("Non constant step in parallel loop nest with ACROSS clause",613,par_do);
   s =0;
 } 
 if(s >= 0)
    return(1);
 else
    return(0);

}

int Analyze_DO_steps(SgExpression *step[], int step_mask[],int ndo)
{ int s,i;
 s=1;
 for(i=0; i<ndo; i++) {
    step_mask[i] = PositiveDoStep(step, i);
    s = s && step_mask[i];
 }
    return(s);
}

void CreateShadowGroupsForAccross(SgExpression *in_spec,SgExpression *out_spec,SgStatement * stmt,SgExpression *gleft,SgExpression *g,SgExpression *gright,int ag[],int all_positive_step,int loop_num[])
{
  RecurList(in_spec, stmt,gleft, ag,0,all_positive_step,loop_num); 
  RecurList(out_spec,stmt,gleft, ag,0,all_positive_step,loop_num);
  RecurList(in_spec, stmt,gright,ag,2,all_positive_step,loop_num); 
  RecurList(out_spec,stmt,gright,ag,2,all_positive_step,loop_num); 
  if(ag[1] == -1)
     ag[1] = 0;
  else
     RecurList(out_spec,stmt,g,ag,1,all_positive_step,loop_num); 
}

void DefineLoopNumberForNegStep(int step_mask[], int n,int loop_num[])
{int i;
 for(i=0;i<n;i++)
   if(loop_num[i] > 0)
     if(step_mask[loop_num[i]-1] > 0)
       loop_num[i] = 0;

}

void CreateShadowGroupsForAccrossNeg(SgExpression *in_spec, SgStatement * stmt, SgExpression *gleft,SgExpression *gright,int ag[],int all_positive_step,int loop_num[])
{
  RecurList(in_spec, stmt,gleft, ag,0,all_positive_step,loop_num);
 // RecurList(out_spec,stmt,gleft, ag,0);
  RecurList(in_spec, stmt,gright,ag,2,all_positive_step,loop_num);
 // RecurList(out_spec,stmt,gright,ag,2);
  if(ag[1] == -1)
     ag[1] = 0;
 // else
 //    RecurList(out_spec,stmt,g,ag,1); 
}

int RecurList (SgExpression *el, SgStatement *st, SgExpression *gref, int *ag, int gnum,int all_positive_step,int loop_num[])
{ SgValueExp c1(1);
  int rank,ndep;
  int  ileft,idv[6];
  SgExpression *es, *ear, *head, *esec, *esc, *lrec[MAX_DIMS], *rrec[MAX_DIMS], *gref_acc = NULL;
  SgSymbol *ar;
  //int nel = 0;
  
  // looking through the dependent_array_list
  for(es = el; es; es = es->rhs()) {
    if( es->lhs()->variant() == ARRAY_OP){
      ear = es->lhs()->lhs();
      esec= es->lhs()->rhs();
      //corner = 1;
    } else {
      ear = es->lhs(); // dependent_array 
      esec = NULL;
      //corner = 0;
      if(!ear->lhs()){ //whole array
        iacross = -1;
        return(0);
      }
    }
     ar = ear->symbol();
     if(HEADER(ar))
       head = HeaderRef(ar);
     else {
       Error("'%s' isn't distributed array", ar->identifier(), 72,st);
       return(0);
     }
     rank = Rank(ar);
     ileft = ndvm;   
     ndep = doRecurLengthArrays(ear->lhs(), ear->symbol(), st,gnum,all_positive_step,loop_num); 
     if(!ndep) continue;
     if(GROUP_INDEX(gref))
       gref_acc=DVM000(*GROUP_INDEX(gref));
     ag[gnum]++;
     if(ag[gnum] == 1)
     { CreateBoundGroup(gref);        
       if( (IN_COMPUTE_REGION || parloop_by_handler) && GROUP_INDEX(gref) )  /*ACC*/
          CreateBoundGroup(gref_acc);
     }

     if(!esec)
     { doCallAfter(InsertArrayBoundDep(gref, head, ileft, ileft+rank, 1, ileft+2*rank));
       if( (IN_COMPUTE_REGION || parloop_by_handler) && GROUP_INDEX(gref) )  /*ACC*/
          doCallAfter(InsertArrayBoundDep(gref_acc, head, ileft, ileft+rank, 1, ileft+2*rank));
     }
     else {
       if(!Recurrences(ear->lhs(),lrec,rrec,MAX_DIMS))
          err("Recurrence list is not specified", 261, st);
       for(esc=esec; esc; esc=esc->rhs()) {
          doSectionIndex(esc->lhs(), ear->symbol(), st, idv, ileft, lrec, rrec);
          doCallAfter(InsertArrayBoundSec(gref, head, idv[0],idv[1],idv[2], idv[3],idv[4], idv[5], 1, ileft+2*rank));  
          if( (IN_COMPUTE_REGION || parloop_by_handler) && GROUP_INDEX(gref) )  /*ACC*/
            doCallAfter(InsertArrayBoundSec(gref_acc, head, idv[0],idv[1],idv[2], idv[3],idv[4], idv[5], 1, ileft+2*rank));    
       }

     }     
  }
  return(ag[gnum]);
}

int doRecurLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int rtype,int all_positive_step,int loop_num[])
{SgValueExp c0(0),c1(1),cM1(-1),c3(3), c5(5);
 int rank,nw,nnl,flag;
 int i=0;
 nnl = 0;
 SgExpression *wl,*ew, *bound[MAX_DIMS],*null[MAX_DIMS],*shsign[MAX_DIMS],*eneg;
 rank = Rank(ar);
 if(!shl)  //without dependence-list ,
           // by default dependence length is equal to the maximal size of shadow edge
   for(i=rank-1,nnl=1; i>=0; i--) {
        bound[i]  = &cM1;
        null[i]   = &c0;
        shsign[i] = &c3;
   }
 if(!TestMaxDims(shl,ar,st))
     return(0);
 for(wl = shl; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     flag = all_positive_step ? 0 : loop_num[i];
     if(rtype > 0) {
       if(!flag)  
         bound[i] = &(ew->rhs())->copy();//right bound 
       else 
         bound[i] = &(ew->lhs())->copy();//left bound
         
     } 
     else {
       if(!flag) 
         bound[i] = &(ew->lhs())->copy();//left bound
       else
         bound[i] = &(ew->rhs())->copy();//right bound
     }  
     null[i] = &c0;
     if(bound[i]->variant() != INT_VAL) {
         Error("Wrong dependence length of distributed array '%s'",ar->identifier(),179,st);
         shsign[i] = &c1;
     }
     else if(bound[i]->valueInteger() != 0) {           
             nnl++;
             if(!flag)
                 shsign[i] = (rtype > 0) ?  &c5 : &c3;
             else {
                 shsign[i] = (rtype > 0) ?  &c3 : &c5; 
                 eneg = null[i] ;
                 null[i] = bound[i];
                 bound[i] = eneg;
             }
     }    else
               shsign[i] = &c1;
 }
  nw = i; 

  if (rank && (nw != rank) ) {// wrong dependence length list length
    if(rtype == 0)
    Error("Wrong dependence length list of distributed array '%s'", ar->identifier(),180,st); 
    return(0);
  }
  if(!nnl) return(0);
  if(rtype > 0){
    TestShadowWidths(ar, null, bound, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);   
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(shsign[i]);
  }
  else {
    TestShadowWidths(ar, bound, null, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(shsign[i]);
  }
  return(nnl);
}

/* according Language Description (by dependence length)
int doRecurLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int rtype,int all_positive_step,int loop_num[])
{SgValueExp c0(0),c1(1),cM1(-1),c3(3), c5(5);
 int rank,nw,nnl,flag;
 int i=0;
 nnl = 0;
 SgExpression *wl,*ew, *bound[MAX_DIMS],*null[MAX_DIMS],*shsign[MAX_DIMS],*eneg;
 rank = Rank(ar);
 if(!shl)  //without dependence-list ,
           // by default dependence length is equal to the maximal size of shadow edge
   for(i=rank-1,nnl=1; i>=0; i--){
        bound[i]  = &cM1;
        null[i]   = &c0;
        shsign[i] = &c3;
   }
 if(!TestMaxDims(shl,ar,st))
     return(0);
 for(wl = shl; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     flag = all_positive_step ? 0 : loop_num[i];
     if(rtype > 0) {
                      //if(!flag)  
         bound[i] = &(ew->rhs())->copy();//right bound 
                      //else 
                      //  bound[i] = &(ew->lhs())->copy();//left bound
         
     } 
     else {
                     //if(!flag) 
         bound[i] = &(ew->lhs())->copy();//left bound
                     //else
                     //  bound[i] = &(ew->rhs())->copy();//right bound
     }  
     null[i] = &c0;
     if(bound[i]->variant() != INT_VAL) {
         Error("Wrong dependence length of distributed array '%s'",ar->identifier(),179,st);
         shsign[i] = &c1;
     }
     else if(bound[i]->valueInteger() != 0) {           
             nnl++;
             if(!flag)
                 shsign[i] = (rtype > 0) ?  &c5 : &c3;
             else {
                 shsign[i] = (rtype > 0) ?  &c3 : &c5; 
                 eneg = null[i] ;
                 null[i] = bound[i];
                 bound[i] = eneg;
             }
     }    else
               shsign[i] = &c1;
 }
  nw = i; 

  if (rank && (nw != rank) ) {// wrong dependence length list length
    if(rtype == 0)
    Error("Wrong dependence length list of distributed array '%s'", ar->identifier(),180,st); 
    return(0);
  }
  if(!nnl) return(0);
  if(rtype > 0){
    TestShadowWidths(ar, null, bound, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);   
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(shsign[i]);
  }
  else {
    TestShadowWidths(ar, bound, null, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(shsign[i]);
  }
  return(nnl);
}
*/

int Recurrences(SgExpression *shl, SgExpression *lrec[], SgExpression *rrec[],int n)
{SgValueExp c0(0),c1(1);
 int i;
 SgExpression *wl,*ew;
 if(!shl) //without recurrence list
    return(0);
 for(i=n; i;i--){
     rrec[i-1] = &c0.copy();
     lrec[i-1] = &c0.copy();
 }
  for(wl = shl,i=0; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     rrec[i] = &(ew->rhs())->copy();//right bound 
     lrec[i] = &(ew->lhs())->copy();//left bound
} 
 return(i);
}

int DepList (SgExpression *el, SgStatement *st, SgExpression *gref, int dep)
{ SgValueExp c1(1);
  int corner,rank,ndep;
  int  ileft;
  SgExpression *es, *ear, *head;
  SgSymbol *ar;
  int nel = 0;
  // looking through the dependent_array_list
  for(es = el; es; es = es->rhs()) {
    if( es->lhs()->variant() == ARRAY_OP){
      ear = es->lhs()->lhs();
      corner = 1;
    } else {
      ear = es->lhs(); // dependent_array 
      corner = 0;
      if(!ear->lhs()){ //whole array
        iacross = -1;
        return(0);
      }
    }
     ar = ear->symbol();
     if(HEADER(ar))
       head = HeaderRef(ar);
     else {
       Error("'%s' isn't distributed array", ar->identifier(), 72,st);
       return(0);
     }
     rank = Rank(ar);
     ileft = ndvm;  
     ndep = doDepLengthArrays(ear->lhs(), ear->symbol(), st,dep);
     if(!ndep) continue;
     nel++;
     if(nel == 1)
       CreateBoundGroup(gref);
     if(dep == ANTIDEP)
       doCallAfter(InsertArrayBound(gref, head, ileft, ileft+rank, corner));  
     else 
       doCallAfter(InsertArrayBoundDep(gref, head, ileft, ileft+rank,(corner ? rank : 1), ileft+2*rank));       
  }
  return(nel);
}
/*
int doDepLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int dep)
{SgValueExp c0(0);
 int rank,iright,nw,nnl;
 int i=0;
 SgExpression *wl,*ew, *lbound[7], *ubound[7];
 rank = Rank(ar);
 nnl = 0;
 for(wl = shl; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     if(dep == ANTIDEP){
       lbound[i] = &c0;                 //left bound 
       ubound[i] = &(ew->rhs())->copy();//right bound 
       if(ubound[i]->variant() != INT_VAL)
        Error("Wrong dependence length of distributed array '%s'",ar->identifier(),179,st);
       else if(ubound[i]->valueInteger() != 0)
         nnl++;    
     } else {
       lbound[i] = &(ew->lhs())->copy();//left bound
       ubound[i] = &c0;                 //right bound 
       if(lbound[i]->variant() != INT_VAL)
        Error("Wrong dependence length of distributed array '%s'",ar->identifier(),179,st);
       else if(lbound[i]->valueInteger() != 0)
         nnl++; 
     }
  }
  nw = i; 
  TestShadowWidths(ar, lbound, ubound, nw, st);
  if (rank && (nw != rank)) {// wrong shadow width list length
     Error("Length of shadow-edge-list is not equal to  the rank of array '%s'",ar->identifier(),88,st); 
     return(0);
  }
  if(dep == ANTIDEP)
  for(i=rank-1;i>=0; i--)
     doAssignStmtAfter(lbound[i]);
  iright = 0;
  if(nnl)
    iright = ndvm;
    for(i=rank-1;i>=0; i--)
      doAssignStmtAfter(ubound[i]);
  return(iright);

}
*/

int doDepLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int dep)
{SgValueExp c0(0),c1(1),cM1(-1),c3(3);
 int rank,nw,nnl;
 int i=0;
 nnl = 0;
 SgExpression *wl,*ew, *bound[MAX_DIMS],*null[MAX_DIMS],*shsign[MAX_DIMS];
 rank = Rank(ar);
 if(!shl)  //without dependence-list ,
           // by default dependence length is equal to the maximal size of shadow edge
   for(i=rank-1,nnl=1; i>=0; i--){
        bound[i]  = &cM1;
        null[i]   = &c0;
        shsign[i] = &c3;
   }
 if(!TestMaxDims(shl,ar,st))
     return(0);  
 for(wl = shl; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     if(dep == ANTIDEP)
       bound[i] = &(ew->rhs())->copy();//right bound 
     else
       bound[i] = &(ew->lhs())->copy();//left bound
     null[i] = &c0;
     if(bound[i]->variant() != INT_VAL) {
         Error("Wrong dependence length of distributed array '%s'",ar->identifier(),179,st);
         shsign[i] = &c1;
     } 
     else if(bound[i]->valueInteger() != 0) {
             nnl++;
             shsign[i] = &c3;
     }    else
             shsign[i] = &c1;
 }
  nw = i; 

  if (rank && (nw != rank)) {// wrong dependence length list length
    if(dep == ANTIDEP)
    Error("Wrong dependence length list of distributed array '%s'", ar->identifier(),180,st); 
     return(0);
  }
  if(!nnl) return(0);
  if(dep == ANTIDEP){
    TestShadowWidths(ar, null, bound, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);   
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
  }
  else {
    TestShadowWidths(ar, bound, null, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(shsign[i]);
  }
  return(nnl);
}

/*
int doDepLengthArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, int dep, int *maxn)
{SgValueExp c0(0),c1(1),cM1(-1);
 int rank,nw,nnl,nsh;
 int i=0;
 nnl = 0;
 nsh = 0;
 SgExpression *wl,*ew, *bound[7],*null[7],*shsign[7];
 rank = Rank(ar);
 if(!shl)  //without dependence-list ,
           // by default dependence length is equal to the maximal size of shadow edge
   for(i=rank-1,nnl=1; i>=0; i--){
        bound[i]  = &cM1;
        null[i]   = &c0;
        shsign[i] = new SgValueExp(7);
   }
  
 for(wl = shl; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     if(dep == ANTIDEP){
       bound[i] = &(ew->rhs())->copy();//right bound 
       null[i] = &c0;
     }
     else {
       bound[i] = &(ew->lhs())->copy();//left bound
       null[i] =  &(ew->rhs())->copy();//right bound 
     }
     if(bound[i]->variant() != INT_VAL)
         Error("Wrong dependence length of distributed array '%s'",ar->identifier(),179,st);
     else if(bound[i]->valueInteger() != 0) {
             nnl++; nsh++;
             shsign[i] = new SgValueExp(7);
     }    else if(null[i]->valueInteger() != 0){
             shsign[i] = new SgValueExp(5);
             nsh++;
     }    else
             shsign[i] = &c1;
     null[i]   = &c0;
 }
  nw = i; 
  *maxn = nsh;
  if (rank && (nw != rank) && (dep == ANTIDEP)) {// wrong dependence length list length
     Error("Wrong dependence length list of distributed array '%s'", ar->identifier(),180,st); 
     return(0);
  }
  if(!nnl) return(0);
  if(dep == ANTIDEP){
    TestShadowWidths(ar, null, bound, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);   
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
  }
  else {
    TestShadowWidths(ar, bound, null, nw, st);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(bound[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(null[i]);
    for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(shsign[i]);
  }
  return(nnl);
}
*/

SgExpression *doLowHighList(SgExpression *shl, SgSymbol *ar, SgStatement *st)
{
  SgValueExp c1(1);
  int nw, i;
  SgExpression *wl, *ew, *lbound[MAX_DIMS], *hbound[MAX_DIMS];
  int rank = Rank(ar);
  if(!TestMaxDims(shl,ar,st))
     return(NULL);
  for(wl = shl,i=0; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     lbound[i] = &(ew->lhs())->copy(); 
     hbound[i] = &(ew->rhs())->copy();
     
     if(lbound[i]->variant() != INT_VAL || hbound[i]->variant() != INT_VAL) {
        Error("Wrong dependence length of distributed array '%s'",ar->identifier(), 179, st);
        lbound[i] = hbound[i] = &c1;
     }
  }

  nw = i; 

  if (rank && (nw != rank) ) 
     Error("--Wrong dependence length list of distributed array '%s'", ar->identifier(), 180, st); 

  TestShadowWidths(ar, lbound, hbound, nw, st);
  
  SgExpression *shlist = NULL; 
  for(i=0; i<nw; i++)
  { 
     shlist = AddElementToList(shlist, DvmType_Ref(hbound[i]));
     shlist = AddElementToList(shlist, DvmType_Ref(lbound[i]));
  }

  return( shlist );
}

void AcrossList(int ilh, SgExpression *el, SgStatement *st)
{ 
  SgExpression *es, *ear;  
  
  // looking through the dependent_array_list
  for(es = el; es; es = es->rhs()) {
    if( es->lhs()->variant() == ARRAY_OP){
      ear = es->lhs()->lhs();
      err("SECTION  specification is not permitted", 643, st);
    } else {
      ear = es->lhs(); 
      if(!ear->lhs()) { //whole array
        Error("Dependence list is not specified for %s", ear->symbol()->identifier(), 644, st);
        continue;
      }
    }
     SgSymbol *ar = ear->symbol();
     if(!HEADER(ar)) {
       Error("'%s' isn't distributed array", ar->identifier(), 72, st);
       continue;
     }
     
     doCallAfter(LoopAcross_H2(ilh, HeaderRef(ar), Rank(ar), doLowHighList(ear->lhs(), ar, st)));
  }
}

void StoreLoopPar(SgExpression *par[], int n, int ind, SgStatement*stl)
{ SgStatement *stat = NULL;
  SgSymbol*s;
  int i;
 if(!newvar_list) return;
 if(stl) {
   stat=cur_st;
   cur_st=stl;
 }
  for(i=0; i<n; i++) 
    if(isSgVarRefExp(par[i])) {
      s = par[i]->symbol();
      if(s && isInSymbList(newvar_list,s))
        doAssignTo_After(&(par[i]->copy()),DVM000(ind+i));
    }
  if(stl)
    cur_st=stat;
}

void ReductionList  (SgExpression *el,SgExpression *gref, SgStatement *st, SgStatement *stmt1, SgStatement *stmt2, int ilh2)
{ SgStatement *last,*last1;
  SgExpression  *er, *ev, *ered, *loc_var,*len, *loclen, *debgref;
  int   irv, irf, num_red, ia, ntype,sign, num, locindtype;
  int itsk = 0, ilen = 0;
  SgSymbol *var; 
  SgValueExp c0(0),c1(1);

  last = stmt2; last1 = stmt1;
  
  //looking through the reduction list
  for(er = el; er; er=er->rhs()) {
     ered = er->lhs();    //  reduction
     ev = ered->rhs(); // reduction variable reference
     if(!isSgVarRefExp(ev) && !isSgArrayRefExp(ev) && !isSgExprListExp(ev))
     {   err("Wrong reduction variable",151,st);
         continue;
     }
     loc_var = ConstRef(0);
     loclen = &c0;
     locindtype = 0;
     len =&c1;
     num=num_red=RedFuncNumber(ered->lhs()); 
     if( !num_red) 
        err("Wrong reduction operation name", 70,st);
       /* 
        if(num_red == 8)  //EQV
        err("Reduction function EQV is not supported now",st); 
       */
     if(num_red > 8) { // MAXLOC => 9,MINLOC =>10
        num_red -= 6; // MAX => 3,MIN =>4
       // change loc_array       
        ev = ered->rhs()->lhs(); // reduction variable reference 
        if( !ered->rhs()->rhs() || !ered->rhs()->rhs()->rhs() || ered->rhs()->rhs()->rhs()->rhs()){
                                       //the number of operands is not equal to 3
          err("Illegal operand list of MAXLOC/MINLOC",147,st);
          continue;
        }
        loc_var = ered->rhs()->rhs()->lhs();        //location variable reference
        loclen = ered->rhs()->rhs()->rhs()->lhs(); //the number of coordinates
        if(isSgVarRefExp(loc_var))
	    loclen =  TypeLengthExpr(loc_var->type()); //14.03.03 new SgValueExp(TypeSize(loc_var->type())); 
        else if( isSgArrayRefExp(loc_var)) {
            ia = loc_var->symbol()->attributes();
            if((ia & DISTRIBUTE_BIT) ||(ia & ALIGN_BIT) || (ia & INHERIT_BIT))
               Error("'%s' is distributed array", loc_var->symbol()->identifier(), 148,st); 
	 /*
            if(!loc_var->lhs()){ //whole array
              if(Rank(loc_var->symbol())>1)
                Error("Wrong operand of MAXLOC/MINLOC: %s",loc_var->symbol()->identifier(), 149,st);
              loclen = ArrayDimSize(loc_var->symbol(),1); // size of vector in elements
              if(!loclen || loclen->variant()==STAR_RANGE){
                Error("Wrong operand of MAXLOC/MINLOC: %s",loc_var->symbol()->identifier(), st); 
                loclen = &c0;
              }
              else
                loclen = &((*ArrayDimSize(loc_var->symbol(),1)) * (*new SgValueExp(TypeSize(loc_var->symbol()->type()->baseType())))) ; // size of vector in bytes 
	    }
	 */   
            loclen = &(*loclen * (*TypeLengthExpr(loc_var->symbol()->type()->baseType()))) ; // size of vector in bytes       
        	//loclen = &(*loclen * (*new SgValueExp(TypeSize(loc_var->symbol()->type()->baseType())))) ; 14.03.03
	}		   
        else        
            err("Wrong operand of MAXLOC/MINLOC",149,st); 
     }
     var = ev->symbol();
     ia = var->attributes();
     if(isSgVarRefExp(ev))  
           redvar_list= AddNewToSymbList(redvar_list,var); 
     else if( isSgArrayRefExp(ev)) {
           
             //if((ia & DISTRIBUTE_BIT) ||(ia & ALIGN_BIT)|| (ia & INHERIT_BIT))
             //  Error("'%s' is distributed array", var->identifier(), 148,st);
              
           if(!ev->lhs()){ //whole array
              len = ArrayLengthInElems(var,st,1); //size of array 
              ev  = FirstArrayElement(var);  
              if((ia & DISTRIBUTE_BIT) ||(ia & ALIGN_BIT)|| (ia & INHERIT_BIT))
              { if(!only_debug) 
                   ev = HeaderRefInd(var,1); 
              }
           }  
     }
     else
        err("Wrong reduction variable",151,st); 
     ntype = VarType_RTS(var);  //RedVarType
     if(!ntype)
        Error("Wrong type of reduction variable '%s'", var->identifier(), 152,st);

     sign = 1;
     if(stmt1 != stmt2) 
       cur_st = last1;
     if(gref)   // interface of RTS1
     {  ilen = ndvm; // index for RedArrayLength
        doAssignStmtAfter(len);
        doAssignStmtAfter(loclen);
     }
     if(num > 8 && loc_var->symbol()) //MAXLOC,MINLOC
       locindtype =  LocVarType(loc_var->symbol(),st);

     irv = ndvm; // index for RedVarRef
     if(!only_debug) {
       if(IN_COMPUTE_REGION || inparloop && parloop_by_handler)    /*ACC*/
       { 
         if(ilh2)  // interface of RTS2
         {  
            doCallAfter(LoopReduction(ilh2,RedFuncNumber_2(num),ev,ntype,len,loc_var,loclen));
            continue;
         }
         int *index = new int;
         *index = irv;
         // adding the attribute (REDVAR_INDEX) to expression for reduction operation
          ered->addAttribute(REDVAR_INDEX, (void *) index, sizeof(int)); 

         doCallAfter (GetActualScalar(var));
         if(num > 8 && loc_var->symbol())
           doCallAfter (GetActualScalar(loc_var->symbol()));
       }
       doAssignStmtAfter(ReductionVar(num_red,ev,ntype,ilen, loc_var, ilen+1,sign));       
       if(num > 8 && loc_var->symbol()) {//MAXLOC,MINLOC
         doAssignStmtAfter(LocIndType(irv, locindtype)); //LocVarType(loc_var->symbol(),st)));        
       }  
     }
     if(debug_regim && st->variant()!=DVM_TASK_REGION_DIR) {
       debgref = idebrg ? DVM000(idebrg) : DebReductionGroup(gref->symbol());
       doCallAfter(D_InsRedVar(debgref,num_red,ev,ntype,ilen, loc_var, ilen+1,locindtype));
     }
     last1 = cur_st;
     if(stmt1 != stmt2) 
         cur_st = last;
     if(!only_debug){
       if(!itsk && st->variant()==DVM_TASK_REGION_DIR){
         itsk = ndvm;
         doAssignStmtAfter(new SgVarRefExp(TASK_SYMBOL(st->symbol())));
       }
       irf = (st->variant()==DVM_TASK_REGION_DIR) ? itsk : iplp;
       doCallAfter(InsertRedVar(gref,irv,irf));
     }
     last = cur_st;
  }   
  /*  if(! only_debug)  
   *     doAssignStmtAfter(SaveRedVars(gref));
   */
   return;
}     

void ReductionVarsStart  (SgExpression *el)
{ 
  SgExpression  *er, *ev, *ered;
  int    num_red; 

  //looking through the reduction list
  for(er = el; er; er=er->rhs()) {
     ered = er->lhs();    //  reduction
     num_red=RedFuncNumber(ered->lhs()); 
     if(num_red <= 8) { 
        ev = ered->rhs();    // reduction variable reference
        if(isSgVarRefExp(ev)){
             doAssignStmtAfter(GetAddresMem(ev)) ;
             FREE_DVM(1);
        }
        if(isSgArrayRefExp(ev) && !IS_DVM_ARRAY(ev->symbol())) {
          if(!ev->lhs()) {//whole array
             doAssignStmtAfter(GetAddresMem(FirstArrayElement(ev->symbol()))) ;
             FREE_DVM(1);  
	  }
          else {
             doAssignStmtAfter(GetAddresMem(ev)) ;
             FREE_DVM(1);
          }   
        }
     } else  { // MAXLOC => 9,MINLOC =>10
        ev = ered->rhs()->lhs(); // reduction variable reference
        if(isSgVarRefExp(ev)){
             doAssignStmtAfter(GetAddresMem(ev)) ;
             FREE_DVM(1);
        }
        if(isSgArrayRefExp(ev) && !IS_DVM_ARRAY(ev->symbol())) {
          if(!ev->lhs()) {//whole array
             doAssignStmtAfter(GetAddresMem(FirstArrayElement(ev->symbol()))) ;
             FREE_DVM(1);  
	  }
          else {
             doAssignStmtAfter(GetAddresMem(ev)) ;
             FREE_DVM(1);
          }   
        }
     /*
        if( ered->rhs()->rhs()->rhs()){ //there are >1 location variables
          ind = *((int*)(ered)->attributeValue(0,LOC_ARR));
          for ( ind_var_list = ered->rhs()->rhs(),ind_num=0; ind_var_list; ind_var_list=ind_var_list->rhs(), ind_num++)
            doAssignTo_After(DVM000(ind+ind_num),ind_var_list->lhs()) ;
        } else
     */
	if(ered->rhs()->rhs() && isSgVarRefExp( ered->rhs()->rhs()->lhs())){
                                                                       //location variable
            doAssignStmtAfter(GetAddresMem( ered->rhs()->rhs()->lhs())) ;
            FREE_DVM(1);
	}  
	if(ered->rhs()->rhs() && isSgArrayRefExp( ered->rhs()->rhs()->lhs()) && !IS_DVM_ARRAY(ered->rhs()->rhs()->lhs()->symbol())){ //location array
 
          if(!( ered->rhs()->rhs()->lhs())->lhs()) {//whole array
            doAssignStmtAfter(GetAddresMem(FirstArrayElement((ered->rhs()->rhs()->lhs())->symbol()))) ;
            FREE_DVM(1);
	  } else {
            doAssignStmtAfter(GetAddresMem( ered->rhs()->rhs()->lhs())) ;
            FREE_DVM(1);
	  }
	}
	
     }
  }   
  if(redl) {// for HPF_program
    reduction_list *erl;  
    for(erl = redl; erl; erl=erl->next) {
      num_red=erl->red_op;     
      ev = erl->red_var; // reduction variable reference  
      if(isSgVarRefExp(ev)){
          doAssignStmtAfter(GetAddresMem(ev)) ;
          FREE_DVM(1);
      }   
    }  
  }       
}    
/*
void ReductionVarsWait  (SgExpression *el)
{ int ind;
  SgExpression  *er, *ered, *ind_var_list;
  int    num_red, ind_num; 
  //looking through the reduction list
  for(er = el; er; er=er->rhs()) {
     ered = er->lhs();    //  reduction
     num_red=RedFuncNumber(ered->lhs()); 
     if((num_red > 8) && ( ered->rhs()->rhs()->rhs())){ // MAXLOC => 9,MINLOC =>10 and
                                                        //there are >1 location variables
        ind = *((int*)(ered)->attributeValue(0,LOC_ARR));
        for ( ind_var_list = ered->rhs()->rhs(),ind_num=0; ind_var_list; ind_var_list=ind_var_list->rhs(), ind_num++)
          doAssignTo_After(ind_var_list->lhs(),DVM000(ind+ind_num)) ;
       } 
      
  }
     
}    
*/

int LocElemNumber(SgExpression *en)
{
    SgExpression *ec;
    int n;
    n = 0;
    ec = Calculate(en);
    if (ec->isInteger())
        n = ec->valueInteger();
    else
        err("Can not calculate number of elements in location array", 595, parallel_dir);
    return(n);
}

void    InsertReductions_H(SgExpression *red_op_list, int ilh)
{
    SgStatement *last;
    SgExpression  *er, *ev, *ered, *loc_var, *en;
    int   irv, num_red, num;
    SgType *type, *loc_type;

    last = NULL;
    if (!irg && IN_COMPUTE_REGION)
        err("Asynchronous reduction is not implemented yet for GPU", 596, parallel_dir);
    //looking through the reduction_op_list
    for (er = red_op_list; er; er = er->rhs())
    {
        ered = er->lhs();    //  reduction  (variant==ARRAY_OP)
        irv = IND_REDVAR(ered);
        ev = ered->rhs(); // reduction variable reference for reduction operations except MINLOC,MAXLOC 
        num = num_red = RedFuncNumber(ered->lhs());
        if (num > 8)   // MAXLOC => 9,MINLOC =>10
        {
            num_red -= 6; // MAX => 3,MIN =>4      
            ev = ered->rhs()->lhs(); // reduction variable reference        
            loc_var = ered->rhs()->rhs()->lhs();        //location array reference
            if (loc_var->lhs())  // array element reference, it must be array name
                Error("Wrong operand of MAXLOC/MINLOC: %s", loc_var->symbol()->identifier(), 149, parallel_dir);
            en = ered->rhs()->rhs()->rhs()->lhs(); // number of elements in location array
            loc_el_num = LocElemNumber(en);
            loc_type = loc_var->symbol()->type();
        }

        type = ev->symbol()->type();
        if (isSgArrayType(type))
        {
            if (isSgArrayRefExp(ev) && !ev->lhs() && !HEADER(ev->symbol())) // whole one-dimensional array
                ;
            else
                Error("Reduction variable %s is array (array element), not implemented yet", ev->symbol()->identifier(), 597, parallel_dir);
            type = type->baseType();
        }

        //if((nr =TestType(type)) == 5 || nr == 6)  // COMPLEX or DCOMPLEX
        //   Error("Illegal type of reduction variable %s, not implemented yet for GPU",ev->symbol()->identifier(),592,parallel_dir);

        InsertNewStatementAfter(LoopInsertReduction_H(ilh, irv), cur_st, cur_st->controlParent());

    }
}

void NewVarList(SgExpression *nl,SgStatement *stmt)
{SgExpression *el,*e;
  for(el=nl; el;el=el->rhs()){
     e=el->lhs();
     if(e->symbol()){
        newvar_list=AddToSymbList(newvar_list,e->symbol());
        //testing
        if(IS_DUMMY(e->symbol()) || IS_SAVE(e->symbol()) || IN_COMMON(e->symbol()))
	  Error("Illegal variable in new-clause: %s",e->symbol()->identifier(),168,stmt); // variable in NEW clause may not be dummy argument, have the SAVE attribute,occur in a COMMON block        
     }
  } 
}

void ReceiveArray(SgExpression *spec_accr,SgStatement *parst)
{SgExpression *es,*el;
 SgSymbol *ar;
 int is,tp;
 // looking through the array_list
  for(es = spec_accr; es; es = es->rhs()) {
     ar =  es->lhs()->symbol();
     switch(ar->type()->baseType()->variant()) {
      case T_INT:     tp = 1; break;
      case T_FLOAT:   tp = 3; break;
      case T_DOUBLE:  tp = 4; break;
      case T_BOOL:    tp = 1; break;
      case T_COMPLEX: tp = 6; break;
      case T_DCOMPLEX: tp = 8; break;
      default:        tp = 0; break;
     }
     is = ndvm;
     if(tp == 6 || tp == 8){
       doAssignStmtAfter(&(*ArrayLengthInElems(ar,parst,1)*(*new SgValueExp(2))));
       tp = tp/2;
     } else
       doAssignStmtAfter(ArrayLengthInElems(ar,parst,1));
     el = FirstArrayElement(ar);
     if(HEADER(ar))
       DistArrayRef(el,0,parst);
     doAssignStmtAfter(DVM_Receive(iplp,GetAddresMem(el),tp,is));
 
  }
}

void SendArray(SgExpression *spec_accr)
{SgExpression *es,*el;
 SgSymbol *ar;
 int is,tp;
 // looking through the array_list
  for(es = spec_accr; es; es = es->rhs()) {
     ar =  es->lhs()->symbol();
     switch(ar->type()->baseType()->variant()) {
      case T_INT:     tp = 1; break;
      case T_FLOAT:   tp = 3; break;
      case T_DOUBLE:  tp = 4; break;
      case T_BOOL:    tp = 1; break;
      case T_COMPLEX: tp = 6; break;
      case T_DCOMPLEX: tp = 8; break;
      default:        tp = 0; break;
     }
     is = ndvm;
     if(tp == 6 || tp == 8){
         doAssignStmtAfter(&(*ArrayLengthInElems(ar,cur_st,0)*(*new SgValueExp(2))));
         tp = tp/2;
     } else
         doAssignStmtAfter(ArrayLengthInElems(ar,cur_st,0));
     el = FirstArrayElement(ar);
     if(HEADER(ar))
       DistArrayRef(el,0,cur_st);
     doAssignStmtAfter(DVM_Send(iplp,GetAddresMem(el),tp,is));
 
  }
}

void  CudaBlockSize(SgExpression *cuda_block_list)
{
    SgExpression *el;
    el = cuda_block_list;
    if (!el)  return;
    doAssignStmtAfter(el->lhs());
    el = el->rhs();
    if (el)
        doAssignStmtAfter(el->lhs());
    else
    {
        doAssignStmtAfter(new SgValueExp(1));  //by default sizeY = 1
        doAssignStmtAfter(new SgValueExp(1));  //by default sizeZ = 1
        return;
    }
    el = el->rhs();
    if (el)
        doAssignStmtAfter(el->lhs());
    else
        doAssignStmtAfter(new SgValueExp(1));  //by default sizeZ = 1
}

void  CudaBlockSize(SgExpression *cuda_block_list,SgExpression *esize[])
{
    SgExpression *el;
    el = cuda_block_list;
    esize[0] = el->lhs();
    el = el->rhs();
    if (el)
        esize[1] = el->lhs();
    else
    {
        esize[1] = new SgValueExp(1);  //by default sizeY = 1
        esize[2] = new SgValueExp(1);  //by default sizeZ = 1
        return;
    }
    el = el->rhs();
    if (el)
        esize[2] = el->lhs();
    else
        esize[2] = new SgValueExp(1);  //by default sizeZ = 1
}

//***********************************************************************************************
//  Interface of RTS2
//***********************************************************************************************
int TestReductionClause(SgExpression *e)
{         
   if( e->symbol()) // asynchronous reduction
      return 0;
   SgExpression *er, *ev;
   for(er = e->lhs(); er; er=er->rhs()) 
   {
     ev = er->lhs()->rhs();    // reduction variable reference
     if(isSgArrayRefExp(ev) && HEADER(ev->symbol()) ) 
        return 0;
     if(isSgExprListExp(ev) && HEADER(ev->lhs()->symbol()) )   //MAXLOC,MINLOC
        return 0;
   }
   return 1;     
}

int CreateParallelLoopByHandler_H2(SgExpression *init[], SgExpression *last[], SgExpression *step[], int nloop)
{ SgExpression *e=NULL,*el,*arglist=NULL;
 // generate call dvmh_loop_create(const DvmType *pCurRegion, const DvmType *pRank, /* const DvmType *pStart, const DvmType *pEnd, const DvmType *pStep */...)
   for(int i=nloop-1; i>=0; i--)
   {   
      e =  len_DvmType ? TypeFunction(SgTypeInt(),step[i],new SgValueExp(len_DvmType) ) : step[i];
      (el = new SgExprListExp(*e))->setRhs(arglist);           
      arglist = el; 
      e =  len_DvmType ? TypeFunction(SgTypeInt(),last[i],new SgValueExp(len_DvmType) ) : last[i];
      (el = new SgExprListExp(*e))->setRhs(arglist);           
      arglist = el; 
      e =  len_DvmType ? TypeFunction(SgTypeInt(),init[i],new SgValueExp(len_DvmType) ) : init[i];
      (el = new SgExprListExp(*e))->setRhs(arglist);           
      arglist = el; 
   }
   int ilh = ndvm;
   doAssignStmtAfter(LoopCreate_H2(nloop,arglist));
   return(ilh);
}

void MappingParallelLoop(SgStatement *stmt, int ilh )
{
   SgExpression *axis[MAX_LOOP_LEVEL],
                *coef[MAX_LOOP_LEVEL],
                *cons[MAX_LOOP_LEVEL];
   SgExpression *arglist=NULL, *el, *e;

   if(!stmt->expr(0))   // undistributed parallel loop
      return;
   int nt = Alignment(stmt,NULL,axis,coef,cons,2); // 2 - interface of RTS2
   for(int i=0; i<nt; i++)
   {         
      e = AlignmentLinear(axis[i],ReplaceFuncCall(coef[i]),cons[i]);   //Calculate(cons[i])    
      (el = new SgExprListExp(*e))->setRhs(arglist);
      arglist = el;
   }
   SgExpression *desc = HeaderRef(stmt->expr(0)->symbol()); //Register_Array_H2(HeaderRef(stmt->expr(0)->symbol())); //!!! temporary
   doCallAfter(LoopMap(ilh,desc,nt,arglist));
}

void Interface_2(SgStatement *stmt,SgExpression *clause[],SgExpression *init[],SgExpression *last[],SgExpression *step[],int nloop,int ndo,SgStatement *first_do) //int iout,SgStatement *stl,SgSymbol *newj,int ub))
{ 
  if (clause[SHADOW_RENEW_])  //there is SHADOW_RENEW clause
     ShadowList(clause[SHADOW_RENEW_]->lhs(), stmt, NULL);
                                       
  // create loop
  int ilh = CreateParallelLoopByHandler_H2(init, last, step, nloop);
  MappingParallelLoop(stmt, ilh);
  //--------------------------------------------------------------------------- 
  // processing specifications/clauses
  // 
  if (clause[CUDA_BLOCK_])  //there is CUDA_BLOCK clause
  {
     SgExpression *eSize[3];
     CudaBlockSize(clause[CUDA_BLOCK_]->lhs(), eSize);
     doCallAfter(SetCudaBlock_H2(ilh, eSize[0], eSize[1], eSize[2]));
  }
  if(clause[SHADOW_COMPUTE_]) //there is SHADOW_COMPUTE clause
  {
     if( (clause[SHADOW_COMPUTE_]->lhs()))
	ShadowComp(clause[SHADOW_COMPUTE_]->lhs(),stmt,ilh);
     else 
        doCallAfter(ShadowCompute(ilh,HeaderRef(stmt->expr(0)->symbol()),0,NULL));          
        //doCallAfter(ShadowCompute(ilh,Register_Array_H2(HeaderRef(stmt->expr(0)->symbol())),0,NULL));                 
  }
  if(clause[REDUCTION_])  //there is REDUCTION clause
  {
     red_list = clause[REDUCTION_]->lhs();
     ReductionList(red_list,NULL,stmt,cur_st,cur_st,ilh);
  }
  if(clause[ACROSS_])  //there is ACROSS clause
  {
        SgExpression *in_spec=NULL;
        SgExpression *e = clause[ACROSS_];
        SgKeywordValExp *kwe;        
        if(e->rhs())
           err("Wrong ACROSS clause",256 ,stmt); 
        else if((e->lhs()->variant() == DDOT) && (kwe=isSgKeywordValExp(e->lhs()->lhs())))
        {
           if(!strcmp(kwe->value(),"in"))
              in_spec = e->lhs()->rhs();
           else
              err("Wrong ACROSS clause",256 ,stmt); 
        } 
        else
           in_spec = e->lhs();            
        if(in_spec)
           AcrossList(ilh,in_spec,stmt);
  }

  //---------------------------------------------------------------------------
  LINE_NUMBER_AFTER(first_do,cur_st);      
  cur_st->addComment(ParallelLoopComment(first_do->lineNumber()));    

  ACC_CreateParallelLoop(ilh,first_do,nloop,stmt,clause,2);  //oldGroup,newGroup,oldGroup2,newGroup2
}
//************************************************************************************************

int ParallelLoop_Debug(SgStatement *stmt)
{
  SgStatement *st,*stl = NULL,*stg, *st3;
  SgStatement *first_do, *stdeb = NULL;
  SgValueExp c0(0);
  int i,nloop,ndo, iinp,iout,ind, mred;
   
  SgForStmt *stdo;
  SgValueExp c1(1);
  
  SgExpression *step[MAX_LOOP_LEVEL], 
               *init[MAX_LOOP_LEVEL],
               *last[MAX_LOOP_LEVEL],
               *vpart[MAX_LOOP_LEVEL];
  SgSymbol     *do_var[MAX_LOOP_LEVEL];
  
  SgExpression *vl, *dovar, *e, *el;

  if (!OMP_program) {/*OMP*/
	 first_do = stmt -> lexNext();// first DO statement of the loop nest
  } else {
      first_do = GetLexNextIgnoreOMP(stmt);// first DO statement of the loop nest /*OMP*/
  }
  newvar_list = NULL;
  redgref = NULL; red_list = NULL; irg = 0; idebrg = 0; mred =0;
  LINE_NUMBER_AFTER(stmt,stmt);
 //generating call to 'bploop' function of performance analizer (begin of parallel interval)
  if(perf_analysis && perf_analysis != 2) 
    InsertNewStatementAfter(St_Bploop(OpenInterval(stmt)), cur_st, stmt->controlParent()); //inserting after function call 'lnumb'
  
  iplp = 0;
  ndo = i = nloop = 0;
  // looking through the do_variables list
  vl = stmt->expr(2); // do_variables list
  for(dovar=vl; dovar; dovar=dovar->rhs())
        nloop++;

  // looking through the specification list
  for(el=stmt->expr(1); el; el=el->rhs()) {
     e = el->lhs();            // specification
     switch (e->variant()) {       
           case REDUCTION_OP:  
                if(mred !=0) break;
                mred = 1;
                red_list = e->lhs();      
                if(  e->symbol()){
		  redgref = new SgVarRefExp(e->symbol());
                  doIfForReduction(redgref,1);
                  nloopred++;
                  stg = doIfForCreateReduction( e->symbol(),nloopred,1);
                  //cur_st->setControlParent(stmt->controlParent()); //to insert correctly next statements
                  st3 = cur_st;
                  cur_st = stg;
                 //looking through the reduction list
                  ReductionList(red_list,redgref, stmt, cur_st, cur_st, 0); 
                  cur_st = st3;
                  InsertNewStatementAfter( new SgAssignStmt(*DVM000(ndvm),*new SgValueExp(0)),cur_st,cur_st->controlParent()); 
               
                } else {
                  irg = ndvm; 
                  redgref = DVM000(irg);
                  doAssignStmtAfter(CreateReductionGroup());
                  idebrg = ndvm; 
                  doAssignStmtAfter( D_CreateDebRedGroup());
                  //looking through the reduction list
                  ReductionList(red_list,redgref, stmt, cur_st, cur_st, 0); 
                }
                break; 
        
           case CONSISTENT_OP:                                         
           case NEW_SPEC_OP:       
           case SHADOW_RENEW_OP: 
           case SHADOW_COMP_OP:        
           case SHADOW_START_OP:
           case SHADOW_WAIT_OP:
           case REMOTE_ACCESS_OP:  
           case INDIRECT_ACCESS_OP:
           case STAGE_OP:
           case ACROSS_OP:          
                break;  
     }
  }       

  iout = ndvm; 
  //initialization vpart[]
  for(i=0; i<MAX_LOOP_LEVEL; i++)
     vpart[i] = NULL;
  i = 0; 
  // looking through the loop nest 
  for(st=first_do; i<nloop; st=st->lexNext(),i++) {
     stdo = isSgForStmt(st);
     if(!stdo)
       break; 
     stl = st; 
     step[i]   = stdo->step();
     if(!step[i])
       step[i] = & c1.copy();  // by default: step = 1
     init[i]=isSpecialFormExp(&stdo->start()->copy(),i,iout+i,vpart,do_var);
     if(init[i])
         step[i] = & c1.copy(); 
       else
         init[i]   = stdo->start();
        
  
     last[i]   = stdo->end();  
 
     if(dbg_if_regim) {// setting new loop parameters
       if(vpart[i]) 
         stdo->setStart(*DVM000(iout+i)+ (*vpart[i]));//special form
                                                    //step is not replaced
       else  
         stdo->setStart(*DVM000(iout+i));  
       
       stdo->setEnd(*DVM000(iout+i+nloop));
     }

     do_var[i] = stdo->symbol(); 
     SetDoVar(stdo->symbol());  
             
  }
  ndo = i;  

  // test whether the directive is correct
  if( !TestParallelDirective(stmt, nloop, ndo, first_do))
     return(0);    // directive is ignored
  
  if(dbg_if_regim>1) {  //copy loop nest
    SgStatement *last_st,*lst;
    last_st= LastStatementOfDoNest(first_do);
    if(last_st != (lst=first_do->lastNodeOfStmt()) || last_st->variant()==LOGIF_NODE) 
     { last_st=ReplaceLabelOfDoStmt(first_do,last_st, GetLabel());
       ReplaceDoNestLabel_Above(last_st,first_do,GetLabel());
     }
    stdeb=first_do->copyPtr();         
  }


  for(i=0; i<nloop; i++)
     doAssignStmtAfter( init[i] );
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( last[i] );
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( step[i] );
  
  iplp = iout;
  iinp = ndvm;     
  OpenParLoop_Inter(stl,iinp,iinp+nloop,do_var,nloop);
  // creating LoopVarTypeArray
  ndvm += nloop;
  for(i=0; i<nloop; i++)
     doAssignStmtAfter( new SgValueExp(LoopVarType(do_var[i],stmt)));
     
  pardo_line = first_do->lineNumber();
  DebugParLoop(cur_st,nloop,iout); //DebugParLoop(cur_st,nloop,iinp+2*nloop); 


  if(dbg_if_regim){ // generating Logical IF statement:
       // begin_lab  IF (doplmbseq(...) .EQ. 0) GO TO end_lab
       // and inserting it before  loop nest
       int ino;
       SgExpression *dopl;
       SgStatement *stn, *if_stmt;
       stn = cur_st;
       LINE_NUMBER_AFTER(first_do,cur_st);
       begin_lab = GetLabel();
       stn->lexNext()-> setLabel(*begin_lab); 
       end_lab   = GetLabel();

       ino = ndvm;
       doAssignStmtAfter(new SgValueExp(pardo_No)); 
       dopl = doPLmbSEQ(ino, nloop, iout);

       if_stmt = new SgLogIfStmt(SgEqOp(*dopl , c0), *new SgGotoStmt(*end_lab));
       cur_st->insertStmtAfter(*if_stmt);
       (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF
                                                            // (error Sage)
  

       if(dbg_if_regim>1) {
           SgStatement *ifst;
           ifst = new SgIfStmt(*DebugIfNotCondition(), *stdeb);
           
           (if_stmt->lexNext())->insertStmtAfter(*ifst,*if_stmt->controlParent());

          // generating GO TO statement:  GO TO begin_lab
          // and inserting it after last statement of parallel loop nest copy
          (stdeb->lastNodeOfStmt())->insertStmtAfter(*new SgGotoStmt(*begin_lab),*ifst);
          TranslateBlock(stdeb);          
       }
  }

  cur_st = stl->lexNext();
      //cur_st = st->lexPrev();  // set cur_st on last DO satement of loop nest
  return(1);  
}

int Reduction_Debug(SgStatement *stmt)
{
  int  mred;
  SgExpression  *e, *el;
  SgStatement *stg,*st3;
  redgref = NULL; irg = 0; idebrg = 0; mred =0;
  LINE_NUMBER_BEFORE(stmt,stmt);
  cur_st = stmt->lexPrev();
  // looking through the specification list
  for(el=stmt->expr(1); el; el=el->rhs()) {
     e = el->lhs();            // specification
     if (e->variant() == REDUCTION_OP) {       
                if(mred !=0) break;
                mred = 1;
                red_list = e->lhs();         
                if(  e->symbol()){
		  redgref = new SgVarRefExp(e->symbol());
                  doIfForReduction(redgref,1);
                  nloopred++;
                  stg = doIfForCreateReduction( e->symbol(),nloopred,1);
                  st3 = cur_st;
                  cur_st = stg;
                 //looking through the reduction list
                  ReductionList(red_list,redgref, stmt, cur_st, cur_st, 0); 
                  cur_st = st3;
                } else {
                  irg = ndvm; 
                  redgref = DVM000(irg);
                  doAssignStmtAfter(CreateReductionGroup());
                  idebrg = ndvm; 
                  doAssignStmtAfter( D_CreateDebRedGroup());
                  //looking through the reduction list
                  ReductionList(red_list,redgref, stmt, cur_st, cur_st, 0); 
                }                                                      
        
     }
  }    
 return(0);   
}

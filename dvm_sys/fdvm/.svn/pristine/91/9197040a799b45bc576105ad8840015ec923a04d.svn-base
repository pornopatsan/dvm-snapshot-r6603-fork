/**************************************************************\
* Fortran DVM                                                  * 
*                                                              *
*   Creating and Inserting New Statement in the Program        *
*           Restructuring Program                              *
\**************************************************************/

#include "dvm.h"

void doAssignStmt (SgExpression *re) {
  SgExpression *le;
  SgValueExp * index;
  SgStatement *ass;
// creating assign statement with right part "re" and inserting it
// before first executable statement (after last generated statement)
  index = new SgValueExp (ndvm++);
  le = new SgArrayRefExp(*dvmbuf,*index);
  ass = new SgAssignStmt (*le,*re);
// for debug
//    ass->unparsestdout();
//
  where->insertStmtBefore(*ass,*where->controlParent());
                                   //inserting 'ass' statement before 'where'  statement 
  cur_st = ass;                                   
  }

SgExpression * LeftPart_AssignStmt (SgExpression *re) {
// creating assign statement with right part "re" and inserting it
// before first executable statement (after last generated statement);
// returns left part of this statement
  SgExpression *le;
  SgValueExp * index;
  SgStatement *ass;
  index = new SgValueExp (ndvm++);
  le = new SgArrayRefExp(*dvmbuf,*index);
  ass = new SgAssignStmt (*le,*re);
// for debug
//    ass->unparsestdout();
//
  where->insertStmtBefore(*ass,*where->controlParent());
                                    //inserting 'ass' statement before 'where'  statement 
  cur_st = ass;  
  return(le);
  }


void doAssignTo (SgExpression *le, SgExpression *re) {
  SgStatement *ass;
// creating assign statement with right part "re" and
// left part "le" and inserting it
// before first executable statement (after last generated statement)
  ass = new SgAssignStmt (*le,*re);
// for debug
//    ass->unparsestdout();
//
  where->insertStmtBefore(*ass,*where->controlParent());
                                  //inserting 'ass' statement before 'where'  statement 
  cur_st = ass;
  }

void doAssignTo_After (SgExpression *le, SgExpression *re) {
  SgStatement *ass;
// creating assign statement with right part "re" and
// left part "le" and inserting it
// after last generated statement
  ass = new SgAssignStmt (*le,*re);

  cur_st->insertStmtAfter(*ass);//inserting after 
                                //current statement 
  cur_st = ass;
  }

void doAssignStmtAfter (SgExpression *re) {
  SgExpression *le;
  SgValueExp * index;
  SgStatement *ass;
// creating assign statement with right part "re" and inserting it
// after current statement (after last generated statement)
  index = new SgValueExp (ndvm++);
  le = new SgArrayRefExp(*dvmbuf,*index);
  ass = new SgAssignStmt (*le,*re);
// for debug
//    ass->unparsestdout();
//
  cur_st->insertStmtAfter(*ass);//inserting after current statement
  cur_st = ass;                                   
 
  }
void doAssignStmtBefore (SgExpression *re, SgStatement *current) {
  SgExpression *le;
  SgValueExp * index;
  SgStatement *ass,*st;
// creating assign statement with right part "re" and inserting it
// before current statement 
  index = new SgValueExp (ndvm++);
  le = new SgArrayRefExp(*dvmbuf,*index);
  ass = new SgAssignStmt (*le,*re);
// for debug
//    ass->unparsestdout();
//
  st = current->controlParent(); 
  if(st->variant() == LOGIF_NODE)  { // Logical IF
     // change by construction IF () THEN <current> ENDIF and
     // then insert assign  statement before current statement
     st->setVariant(IF_NODE);
     current->insertStmtAfter(* new SgStatement(CONTROL_END));
         //printVariantName( (current->lexNext())->variant());
     st-> insertStmtAfter(*ass);
     return;   
   }

  if (current-> hasLabel() && current->variant() != FORMAT_STAT && current->variant() != DATA_DECL && current->variant()  != ENTRY_STAT)    { //current statement has label
    //insert assign statement before current and set on it the label of current
     SgLabel *lab;
     lab = current->label(); 
     BIF_LABEL(current->thebif) = NULL;
     current->insertStmtBefore(*ass,*current->controlParent());//inserting before current statement
     ass-> setLabel(*lab);
     return;
   }  
  current->insertStmtBefore(*ass,*current->controlParent());//inserting before current statement 
  }

void doCallAfter(SgStatement *call)
{
  cur_st->insertStmtAfter(*call);//inserting call statement after current statement
  cur_st = call;                                   
}

void doCallStmt(SgStatement *call)
{
  where->insertStmtBefore(*call,*where->controlParent());//inserting call statement before 'where' statement
  cur_st = call;                                   
}


void Extract_Stmt(SgStatement *st)
{ char *st1_comment,*st2_comment, *pt;
  if(!st) return; 
// save comment (add to next statement)
  st1_comment = st->comments();
  if(st1_comment && st->lexNext())
  { st2_comment = st->lexNext()->comments();
    if(!st2_comment)
      st->lexNext()->addComment(st1_comment); 
      
    
    else
    { 
            //st->addComment(st2_comment);
            //st->lexNext()->setComments(st->comments());
      pt = (char *) malloc(strlen(st1_comment) + strlen(st2_comment) +1);
      sprintf(pt,"%s%s",st1_comment,st2_comment);
      CMNT_STRING(BIF_CMNT(st->lexNext()->thebif)) = pt;
    }
  }   
  
// extract
  st-> extractStmt();

}

void InsertNewStatementAfter (SgStatement *stat, SgStatement *current, SgStatement *cp)
{SgStatement *st;
 st = current;
 if(current->variant() == LOGIF_NODE)   // Logical IF
    st = current->lexNext();
 if(cp->variant() == LOGIF_NODE) 
   LogIf_to_IfThen(cp);
 st->insertStmtAfter(*stat,*cp);
 cur_st = stat;
}

void InsertNewStatementBefore (SgStatement *stat, SgStatement *current) {
  //SgExpression *le;
  //SgValueExp * index;
  SgStatement *st;

  st = current->controlParent(); 
  if(st->variant() == LOGIF_NODE)  { // Logical IF
     // change by construction IF () THEN <current> ENDIF and
     // then insert statement before current statement
     st->setVariant(IF_NODE);
     SgStatement *control = new SgStatement(CONTROL_END);/*OMP*/
     if (current->numberOfAttributes(OMP_MARK) > 0) {/*OMP*/
        control->addAttribute (OMP_MARK);/*OMP*/
     }/*OMP*/
     current->insertStmtAfter(*control);
     st-> insertStmtAfter(*stat);
     return;   
   }

  if (current-> hasLabel() && current->variant() != FORMAT_STAT && current->variant() != DATA_DECL && current->variant()  != ENTRY_STAT)    { //current statement has label
    //insert statement before current and set on it the label of current
     SgLabel *lab;
     lab = current->label(); 
     BIF_LABEL(current->thebif) = NULL;
     current->insertStmtBefore(*stat,*current->controlParent());//inserting before current statement
     stat-> setLabel(*lab);
     return;
   }  
  current->insertStmtBefore(*stat,*current->controlParent());//inserting before current statement 
  }

void ReplaceByIfStmt(SgStatement *stmt)
{ SgStatement *if_stmt, *cp;
  SgLabel *lab = NULL;
  char * cmnt=NULL;

  ChangeDistArrayRef(stmt->expr(0)); /*24.06.14 podd*/
  ChangeDistArrayRef(stmt->expr(1)); /*24.06.14 podd*/

  // testing: is control parent  Logical IF statement
  if_stmt = stmt->controlParent(); 
  if((if_stmt->variant() == LOGIF_NODE))  {
      if_stmt->setExpression(0,               
                 (*(if_stmt->expr(0))) && SgNeqOp(*TestIOProcessor(),                                                         *new SgValueExp(0) ));
                 // adding condition: TstIO()
      return;
  }

  if (stmt-> hasLabel()) {    // PRINT statement has label
    // set on new if-statement  the label of current statement 
      lab = stmt->label(); 
      BIF_LABEL(stmt->thebif) = NULL;   
  }
  cmnt=stmt-> comments();
  if (cmnt)    // PRINT has preceeding comments 
      BIF_CMNT(stmt->thebif) = NULL;

  cur_st =  stmt->lexNext();
  //cur_st =  stmt->lexPrev();
  cp = stmt->controlParent();
  stmt->extractStmt(); 
  if_stmt =  new SgLogIfStmt(SgNeqOp(*TestIOProcessor(),                                                         *new SgValueExp(0) ),*stmt);
  cur_st->insertStmtBefore(*if_stmt, *cp);
  cur_st = if_stmt->lexNext(); // PRINT statement
  if (cur_st->numberOfAttributes(OMP_MARK) > 0) {/*OMP*/
    DelAttributeFromStmt (OMP_MARK, cur_st);/*OMP*/
    //if_stmt->addAttribute (OMP_MARK);/*OMP*/
  }/*OMP*/
  (cur_st->lexNext())-> extractStmt(); //extract ENDIF (error Sage
  if(lab)
      if_stmt -> setLabel(*lab);
  if(cmnt)
      if_stmt -> setComments(cmnt);
  return;
}

SgStatement *ReplaceStmt_By_IfThenConstr(SgStatement *stmt,SgExpression *econd)
{ SgStatement *ifst, *cp, *curst;
  SgLabel *lab = NULL;
//  replace  <statement>
// by construction:  IF  ( <condition> ) THEN
//                         <statement> 
//                   ENDIF

  if (stmt-> hasLabel()) {    // statement has label
    // set on new if-statement  the label of current statement 
      lab = stmt->label(); 
      BIF_LABEL(stmt->thebif) = NULL;   
  }
 
  curst =  stmt->lexNext();
  
  cp = stmt->controlParent();
  stmt->extractStmt(); 

 ifst = new SgIfStmt( *econd, *stmt);
 curst->insertStmtBefore(*ifst, *cp);
 
 if (curst->numberOfAttributes(OMP_MARK) > 0) {/*OMP*/
    ifst->addAttribute (OMP_MARK);/*OMP*/
    ifst->lexNext()->lexNext()->addAttribute (OMP_MARK);/*OMP*/
  }/*OMP*/
  if(lab)
      ifst -> setLabel(*lab);
 
 return(ifst->lexNext()->lexNext());// ENDIF
}

SgStatement *CreateIfThenConstr(SgExpression *cond, SgStatement *st)
{SgStatement *ifst;

// creating
//          IF ( cond ) THEN
//            <statement-st-or-CONTINUE>
//          ENDIF 
 st = st ? st : new SgStatement(CONT_STAT);
 ifst = new SgIfStmt( *cond, *st);
 return(ifst);
}

void ReplaceAssignByIf(SgStatement *stmt)
{ SgStatement *if_stmt, *cp;
  SgLabel *lab = NULL;
  char * cmnt=NULL;
  SgSymbol *ar = NULL;
  SgExpression *el = NULL,*ei[MAX_DIMS];
  SgExpression *condition=NULL, *index_list=NULL;  
  int iind,i,j,k;
  if(isSgArrayRefExp(stmt->expr(0))) { 
     ar = stmt->expr(0)->symbol();
     el = stmt->expr(0)->lhs(); //index list
  }
  if(stmt->expr(0)->variant() == ARRAY_OP){
     ar = stmt->expr(0)->lhs()->symbol();
     el = stmt->expr(0)->lhs()->lhs(); //index list
  }
  if (!el || !TestMaxDims(el,ar,stmt)) //error situation: no subscripts or the number of subscripts > MAX_DIMS
     return; 
  
  if (stmt-> hasLabel()) {    // assign statement has label
    // set on new if-statement  the label of current statement 
     lab = stmt->label(); 
     BIF_LABEL(stmt->thebif) = NULL;   
  }
  cmnt=stmt-> comments();
  if (cmnt)    // statement has preceeding comments 
     BIF_CMNT(stmt->thebif) = NULL;
    
  for(i=0;el;el=el->rhs(),i++) 
  {  ei[i] = &(el->lhs()->copy());       
     ChangeDistArrayRef(ei[i]);
     if(!IN_COMPUTE_REGION && !INTERFACE_RTS2)  
        ei[i] = &(*ei[i]- *Exprn(LowerBound(ar,i)));
  } 
  iind = ndvm;

  where = stmt;

  if(for_kernel)  /*ACC*/
     cur_st = stmt->lexPrev(); /*ACC*/
  else if(INTERFACE_RTS2)
  { 
     cur_st = stmt->lexPrev();     
     for(j=i; j; j--) 
        index_list= AddListToList(index_list,new SgExprListExp(*DvmType_Ref(ei[j-1])));
  }
  else 
  {
//    if(IN_COMPUTE_REGION )                      /*ACC*/
//        doAssignTo(VECTOR_REF(indexArraySymbol(ar),1),ei[i-1]);  /*ACC*/
//     else
//        doAssignStmt(ei[i-1]);          
//     cur_st->addAttribute (OMP_CRITICAL); /*OMP*/
//     if(lab)
//        cur_st -> setLabel(*lab);
     
     for(j=i,k=1; j; j--) 
     {  if(IN_COMPUTE_REGION)   /*ACC*/
           doAssignTo(VECTOR_REF(indexArraySymbol(ar),k++),ei[j-1]);/*ACC*/ 
        else 
           doAssignStmtAfter(ei[j-1]);
        if(lab && k==1)
           cur_st -> setLabel(*lab);
        cur_st->addAttribute (OMP_CRITICAL); /*OMP*/     
     }
      
  } 
  cp = stmt->controlParent(); /*ACC*/ 
  stmt->extractStmt();
  if(IN_COMPUTE_REGION && !for_kernel)       /*ACC*/
     condition =  & SgNeqOp(INTERFACE_RTS2 ? *HasLocalElement_H2(NULL,ar,i,index_list) : *HasLocalElement(NULL,ar,indexArraySymbol(ar)), *new SgValueExp(0) );
  else if(for_kernel)                       /*ACC*/
     condition = LocalityConditionInKernel(ar,ei);    /*ACC*/
  else
     condition = & SgNeqOp(INTERFACE_RTS2 ? *HasElement(HeaderRef(ar),i,index_list) : *TestElement(HeaderRef(ar),iind), *new SgValueExp(0) );  
  if_stmt =  new SgLogIfStmt(*condition,*stmt);
  stmt->addAttribute (OMP_CRITICAL); /*OMP*/
  if_stmt->addAttribute (OMP_CRITICAL); /*OMP*/
  if((for_kernel || INTERFACE_RTS2) && lab)             /*ACC*/
     if_stmt -> setLabel(*lab);

  cur_st->insertStmtAfter(*if_stmt,*cp);
  cur_st = if_stmt->lexNext(); // assign statement
  (cur_st->lexNext())-> extractStmt(); //extract ENDIF (error Sage

  if(cmnt)
     if_stmt -> setComments(cmnt);
  
  SET_DVM(iind);
  return;
}
 
void ReplaceDoNestLabel(SgStatement *last_st, SgLabel *new_lab)
//replaces the label of DO statement nest, which is ended by last_st,
// by new_lab
//         DO 1 I1 = 1,N1                              DO 99999 I1 = 1,N1
//         DO 1 I2 = 1,N2                              DO 99999 I2 = 1,N2
//          .  .   .                                      .  .   .       
//         DO 1 IK = 1,NK                              DO 99999 IK = 1,NK
//           . . .                                      .  .   . 
// 1       statement                              1     statement   
//                                            99999    CONTINUE
{SgStatement *parent,*st;
 SgLabel *lab;
 
 parent = last_st->controlParent();
 lab = last_st->label();
 //change 04.03.08
 //while((do_st=isSgForStmt(parent)) != NULL && do_st->endOfLoop()) {
 while((parent->variant()==FOR_NODE || parent->variant()==WHILE_NODE) && BIF_LABEL_USE(parent->thebif)) {
    if(LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(BIF_LABEL_USE(parent->thebif))){
      if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(parent->thebif) = new_lab->thelabel;
      parent = parent->controlParent();
    }
    else
      break;
 }

 //inserts CONTINUE statement with new_lab as label  
 st = new SgStatement(CONT_STAT);
 st->setLabel(*new_lab);
 // for debug regim
 LABEL_BODY(new_lab->thelabel) = st->thebif;
 BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
 if(last_st->variant() != LOGIF_NODE)
   //last_st->insertStmtAfter(*st);
    last_st->insertStmtAfter(*st,*last_st->controlParent());
 else
     (last_st->lexNext())->insertStmtAfter(*st,*last_st->controlParent());
    //   st->setControlParent(*last_st->controlParent());
 //printVariantName(last_st->controlParent()->variant());

 /*
//renew global variable 'end_loop_lab' (for parallel loop)
 if(end_loop_lab)
    if(LABEL_STMTNO(end_loop_lab->thelabel) ==                                                                       LABEL_STMTNO(lab->thelabel))  
      end_loop_lab = new_lab;
  */
}

SgLabel * LabelOfDoStmt(SgStatement *stmt)
{ if(BIF_LABEL_USE(stmt->thebif))
    return (LabelMapping(BIF_LABEL_USE(stmt->thebif)));
  else
    return(NULL);
}

void ReplaceDoNestLabel_Above(SgStatement *last_st, SgStatement *from_st,SgLabel *new_lab)
//replaces the label of DO statements locating  above  'from_st'  in nest,
// which is ended by 'last_st', by 'new_lab' 
//         DO 1 I1 = 1,N1                              DO 99999 I1 = 1,N1
//         DO 1 I2 = 1,N2                              DO 99999 I2 = 1,N2
//          .  .   .                                      .  .   .       
//         DO 1 IK = 1,NK                              DO 99999 IK = 1,NK
// CDVM$   PARALLEL (J1,...,JL) ON A(...) ==>  CDVM$   PARALLEL (J1,...,JL) ON A(...)
//         DO 1 J1 = 1,N1                              DO 1 J1 = 1,N1
//         DO 1 J2 = 1,N2                              DO 1 J2 = 1,N2
//          .  .   .                                       .  .   .          
//         DO 1 JL = 1,NL                              DO 1 JL = 1,NL
//           . . .                                      .  .   . 
// 1       CONTINUE                            1       CONTINUE
//                                            99999    CONTINUE
{SgStatement *parent,*st,*par;
 SgLabel *lab;
 int is_above;
 par = parent = from_st->controlParent();
 lab = LabelOfDoStmt(from_st); //((SgForStmt *)from_st)->endOfLoop();
 if(!lab) //DO statement 'from_st' has no label
   return;
 is_above = 0;

 while((parent->variant()==FOR_NODE || parent->variant()==WHILE_NODE) && BIF_LABEL_USE(parent->thebif)) {
    if(LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(BIF_LABEL_USE(parent->thebif))){
      if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(parent->thebif) = new_lab->thelabel;
      is_above = 1;
      parent = parent->controlParent();
    }
    else
      break;
 }
/*
 while((do_st=isSgForStmt(parent)) != NULL  && do_st->endOfLoop()) {
    if(LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(do_st->endOfLoop()->thelabel)){
      if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(do_st->thebif) = new_lab->thelabel;
      is_above = 1;
      parent = parent->controlParent();
    }
    else
      break;
 }
 */

 //inserts CONTINUE statement with new_lab as label
 if(is_above) { 
   st = new SgStatement(CONT_STAT);
   st->setLabel(*new_lab);
    //for debug regim
   LABEL_BODY(new_lab->thelabel) = st->thebif;
   BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
   if(last_st->variant() != LOGIF_NODE)
     last_st->insertStmtAfter(*st,*par);
   else
     (last_st->lexNext())->insertStmtAfter(*st,*par);
 }
}

void ReplaceParDoNestLabel(SgStatement *last_st, SgStatement *from_st,SgLabel *new_lab)
//replaces the label of DO statements locating  above  'from_st'  in nest,
// which is ended by 'last_st', by 'new_lab' 
// CDVM$   PARALLEL (I1,...,IL) ON A(...) ==>  CDVM$   PARALLEL (I1,...,IL) ON A(...)
//         DO 1 I1 = 1,N1                              DO 99999 I1 = 1,N1
//         DO 1 I2 = 1,N2                              DO 99999 I2 = 1,N2
//          .  .   .                                      .  .   .       
//         DO 1 IK = 1,NK                              DO 99999 IK = 1,NK
//           . . .                                      .  .   . 
// 1       CONTINUE                           99999       CONTINUE
//                                            
{SgStatement *parent,*st,*par;
 SgLabel *lab;
 int is_above;
 par = parent = from_st->controlParent();
 lab = LabelOfDoStmt(parent); //((SgForStmt *)parent)->endOfLoop();
 if(!lab) //DO statement  has no label
   return;
 is_above = 0;

while((parent->variant()==FOR_NODE || parent->variant()==WHILE_NODE) && BIF_LABEL_USE(parent->thebif)) {
    if(LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(BIF_LABEL_USE(parent->thebif))){
      if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(parent->thebif) = new_lab->thelabel;
      is_above = 1;
      parent = parent->controlParent();
    }
    else
      break;
 }

/*
 while((do_st=isSgForStmt(parent)) != NULL && do_st->endOfLoop()) {
 if(LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(do_st->endOfLoop()->thelabel)){
     if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(do_st->thebif) = new_lab->thelabel;
      is_above = 1;
      parent = parent->controlParent();
    }
    else
      break;
 }
*/

 //inserts CONTINUE statement with new_lab as label
 if(is_above) { 
   st = new SgStatement(CONT_STAT);
   st->setLabel(*new_lab);
    //for debug regim
   LABEL_BODY(new_lab->thelabel) = st->thebif;
   BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
   if(last_st->variant() != LOGIF_NODE)
     last_st->insertStmtAfter(*st,*par);
   else
     (last_st->lexNext())->insertStmtAfter(*st,*par);
 }
}

SgStatement *ReplaceDoLabel(SgStatement *last_st, SgLabel *new_lab)
//replaces the label of DO statement, which is ended by last_st,
// by new_lab
//         DO 1 I = 1,N                              DO 99999 I = 1,N
//           . . .                                      .  .   . 
// 1       statement                              1     statement   
//                                            99999    CONTINUE

{SgStatement *parent, *st;
 SgLabel *lab;
 parent = last_st->controlParent();
 if((parent->variant()==FOR_NODE || parent->variant()==WHILE_NODE) && (lab=LabelOfDoStmt(parent))){
               //if((do_st=isSgForStmt(parent)) != NULL && (lab=do_st->endOfLoop())){
      if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(parent->thebif) = new_lab->thelabel;
 }
 else
      return(NULL);

 //inserts CONTINUE statement with new_lab as label  
 st = new SgStatement(CONT_STAT);
 st->setLabel(*new_lab);
 //for debug regim
 LABEL_BODY(new_lab->thelabel) = st->thebif;
 BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
 if(last_st->variant() != LOGIF_NODE)
    last_st->insertStmtAfter(*st,*parent);
 else
    (last_st->lexNext())->insertStmtAfter(*st,*parent);
 return(st);
}

SgStatement *ReplaceLabelOfDoStmt(SgStatement *first,SgStatement *last_st, SgLabel *new_lab)
//replaces the label of first DO statement of DO nest, which is ended by last_st,
// by new_lab
//         DO 1 I = 1,N                              DO 99999 I = 1,N
//         DO 1 J = 1,N                              DO 1 J = 1,N
//           . . .                                      .  .   . 
// 1       statement                              1     statement   
//                                             99999    CONTINUE

{SgStatement *parent, *st;
 SgLabel *lab;
 parent = last_st->controlParent();
 if((first->variant()==FOR_NODE || first->variant()==WHILE_NODE) && (lab=LabelOfDoStmt(first))){
                       //if((do_st=isSgForStmt(first)) != NULL && (lab=do_st->endOfLoop())){
      if(!new_lab)
        new_lab = GetLabel();
      BIF_LABEL_USE(first->thebif) = new_lab->thelabel;
 }
 else
      return(NULL);

 //inserts CONTINUE statement with new_lab as label  
 st = new SgStatement(CONT_STAT);
 st->setLabel(*new_lab);
 //for debug regim
 LABEL_BODY(new_lab->thelabel) = st->thebif;
 BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
 if(last_st->variant() != LOGIF_NODE)
    last_st->insertStmtAfter(*st,*first);
 else
    (last_st->lexNext())->insertStmtAfter(*st,*first);
 return(st);
}

SgStatement *ReplaceBy_DO_ENDDO(SgStatement *first,SgStatement *last_st)
//replaces  first DO statement of DO nest with label, which is ended by last_st,
// by DO-ENDDO construct
//         DO 1 I = 1,N                              DO   I = 1,N
//         DO 1 J = 1,N                              DO 1 J = 1,N
//           . . .                                      .  .   . 
// 1       statement                              1     statement   
//                                                   ENDDO

{SgStatement *parent, *st;
 SgLabel *lab;
 parent = last_st->controlParent();
 if((first->variant()==FOR_NODE || first->variant()==WHILE_NODE) && (lab=LabelOfDoStmt(first))){
      BIF_LABEL_USE(first->thebif) = NULL;
 }
 else
      return(NULL);

 //inserts ENDDO statement  
 st = new SgControlEndStmt(); //new SgStatement(CONTROL_END);

 //for debug regim
 BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
 if(last_st->variant() != LOGIF_NODE)
    last_st->insertStmtAfter(*st,*first);
 else
    (last_st->lexNext())->insertStmtAfter(*st,*first);
 return(st);
}

void ReplaceContext(SgStatement *stmt)
{
 if(isDoEndStmt_f90(stmt))
   ReplaceDoNestLabel(stmt, GetLabel());
 else if(isSgLogIfStmt(stmt->controlParent())) {
   if(isDoEndStmt_f90(stmt->controlParent()))
      ReplaceDoNestLabel(stmt->controlParent(),GetLabel());
   LogIf_to_IfThen(stmt->controlParent());
 } 
} 

void LogIf_to_IfThen(SgStatement *stmt)
{
//replace Logical IF statement: IF ( <condition> ) <statement>
// by construction:  IF  ( <condition> ) THEN
//                         <statement> 
//                   ENDIF
 SgControlEndStmt *control = new SgControlEndStmt();
 stmt->setVariant(IF_NODE);
(stmt->lexNext())->insertStmtAfter(* control,*stmt);
 if (stmt->numberOfAttributes(OMP_MARK) > 0) {/*OMP*/
    control->addAttribute (OMP_MARK);/*OMP*/
 }/*OMP*/ 
}


SgStatement *doIfThenConstr(SgSymbol *ar)
{SgStatement *ifst;
 SgExpression *ea;
// creating
//          IF ( ar(1) .EQ. 0) THEN
//          ENDIF 
 ea = new SgArrayRefExp(*ar, *new SgValueExp(1));  ///IS_TEMPLATE(ar) && !INTERFACE_RTS2 ? new SgArrayRefExp(*ar) : new SgArrayRefExp(*ar, *new SgValueExp(1));
 ifst = new SgIfStmt( SgEqOp(*ea, *new SgValueExp(0)), *new SgStatement(CONT_STAT));
 where->insertStmtBefore(*ifst,*where->controlParent());
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 return(ifst);
}

SgStatement *doIfThenConstrWithArElem(SgSymbol *ar, int ind)
{SgStatement *ifst;
// creating
//          IF ( ar(ind) .EQ. 0) THEN
//             ar(ind) = 1;
//          ENDIF 
 ifst = new SgIfStmt( SgEqOp(*ARRAY_ELEMENT(ar,ind), *new SgValueExp(0)), *new SgAssignStmt(*ARRAY_ELEMENT(ar,ind), *new SgValueExp(1)));
 where->insertStmtBefore(*ifst,*where->controlParent());
// ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 return(ifst);
}

SgStatement *doIfForFileVariables(SgSymbol *s)
{SgStatement *ifst;
// creating
//          IF ( s .EQ. 0) THEN
//          ENDIF 
 ifst = new SgIfStmt( SgEqOp(*new SgVarRefExp(*s), *new SgValueExp(0)), *new SgStatement(CONT_STAT));
 cur_st->insertStmtAfter(*ifst,*cur_st->controlParent());
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 return(ifst);
}

SgStatement *doIfThenConstrForRedis(SgExpression *headref, SgStatement *stmt, int index)
{SgStatement *ifst;
 SgExpression *e;
// creating
//          IF ( headref .EQ. 0) THEN  /*08.05.17*/  //IF ( getamv(HeaderRef) .EQ. 0) THEN

//          ELSE

//          ENDIF 

 e = headref; /*08.05.17*/ //e = (index>1) ? headref : GetAMView( headref); //TEMPLATE or not
 ifst = new SgIfStmt( SgEqOp(*e, *new SgValueExp(0)), *new SgStatement(CONT_STAT),*new SgStatement(CONT_STAT));
 stmt->insertStmtBefore(*ifst,*stmt->controlParent());      //10.12.12   after=>before
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 ifst->lexNext()->lexNext()->extractStmt(); // extracting second CONTINUE statement
 return(ifst);
}

SgStatement *doIfThenConstrForRealign(int iamv, SgStatement *stmt, int cond)
{SgStatement *ifst;
 SgExpression *econd;
// creating
//          IF ( dvm000(iamv) .EQ. 0) THEN            or   .NE.

//          ENDIF 
 econd = cond ?  &SgEqOp(*DVM000(iamv), *new SgValueExp(0)) : &SgNeqOp(*DVM000(iamv), *new SgValueExp(0));
 ifst = new SgIfStmt( *econd, *new SgStatement(CONT_STAT));
 stmt->insertStmtAfter(*ifst,*stmt->controlParent());
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 return(ifst);
}

SgStatement *doIfThenConstrForRealign(SgExpression *headref, SgStatement *stmt, int cond)
{SgStatement *ifst;
 SgExpression *econd;
// creating
//          IF ( headref .EQ. 0) THEN            or   .NE.

//          ENDIF 

 econd = cond ?  &SgEqOp(*headref, *new SgValueExp(0)) : &SgNeqOp(*headref, *new SgValueExp(0));
 ifst = new SgIfStmt( *econd, *new SgStatement(CONT_STAT));
 stmt->insertStmtAfter(*ifst,*stmt->controlParent());
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 return(ifst);
}

SgStatement *doIfThenConstrForPrefetch(SgStatement *stmt)
{SgStatement *ifst;
// creating
//          IF ( GROUP(1) .EQ. 0) THEN
//               GROUP(2) = 0
//          ELSE
//               GROUP(2) = 1
//          ENDIF 

 ifst = new SgIfStmt( SgEqOp(*GROUP_REF(stmt->symbol(),1), *new SgValueExp(0)), *new SgAssignStmt(*GROUP_REF(stmt->symbol(),2),*new SgValueExp(0)),*new SgAssignStmt(*GROUP_REF(stmt->symbol(),2),*new SgValueExp(1)));
 stmt->insertStmtAfter(*ifst,*stmt->controlParent());
 //cur_st = ifst->lexNext()->lexNext()->lexNext()->lexNext();//END IF
 return(ifst);
}

SgStatement *doIfThenConstrForRemAcc(SgSymbol *group, SgStatement *stmt)
{SgStatement *ifst, *st;
// creating
//          IF ( GROUP(2) .EQ. 0) THEN
//              
//          ELSE
//               IF  ( GROUP(3) .EQ. 1) THEN
//                 GROUP(3) = 0
//               ENDIF
//          ENDIF 
//          CONTINUE

 ifst = new SgIfStmt( SgEqOp(*GROUP_REF(group,2), *new SgValueExp(0)), *new SgStatement(CONT_STAT),*new SgIfStmt( SgEqOp(*GROUP_REF(group,3), *new SgValueExp(1)),*new SgAssignStmt(*GROUP_REF(group,3),*new SgValueExp(0))));
 st=new SgStatement(CONT_STAT);                     //generating and
 stmt->insertStmtAfter(*st,*stmt->controlParent()); //inserting CONTINUE statement
 stmt->insertStmtAfter(*ifst,*stmt->controlParent());
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
   //cur_st = ifst->lexNext()->lexNext();//internal IF THEN
   //doAssignStmtAfter(WaitBG(group));
   //FREE_DVM(1);
 //cur_st = cur_st->lexNext()->lexNext()->lexNext();//END IF

 cur_st = st;
 return(ifst);
}

void  doIfForReduction(SgExpression *redgref, int deb)
{SgStatement *if_stmt;
// creating
//          IF ( GROUP .EQ. 0) THEN
//               GROUP = crtrdf(...)
//          ENDIF 
 if_stmt =  new SgIfStmt(SgEqOp(*redgref, *new SgValueExp(0) ),*new SgAssignStmt(*redgref,*CreateReductionGroup()));
  cur_st->insertStmtAfter(*if_stmt, *cur_st->controlParent());
  cur_st = if_stmt->lexNext();
  if(debug_regim && deb){
     doAssignTo_After( DebReductionGroup( redgref->symbol()), D_CreateDebRedGroup());
  }

  cur_st = cur_st->lexNext(); //END IF
}

SgStatement *doIfForCreateReduction(SgSymbol *gs, int i, int flag)
{SgStatement *if_stmt, *st;
 SgSymbol *rgv, *go;
 SgExpression *rgvref;
// creating
//          IF ( <red-group-var>(i) .EQ. 0) THEN
//            [ <red-group-var>(i) = 1 ]   //   if flag == 1
//          ENDIF 
//          CONTINUE
 go = ORIGINAL_SYMBOL(gs);
 rgv = * ((SgSymbol **) go -> attributeValue(0,RED_GROUP_VAR));
 rgvref = new SgArrayRefExp(*rgv,*new SgValueExp(i));
 st = flag ? new SgAssignStmt(*rgvref,*new SgValueExp(1)) : new SgStatement(CONT_STAT);
 if_stmt =  new SgIfStmt(SgEqOp(*rgvref, *new SgValueExp(0) ), *st);
  cur_st->insertStmtAfter(*if_stmt);
  //cur_st = if_stmt->lexNext()->lexNext(); //END IF
  st=new SgStatement(CONT_STAT);
  if_stmt->lexNext()->lexNext()->insertStmtAfter(*st);
  cur_st = st;
  if(!flag) 
    if_stmt->lexNext()->extractStmt(); // extracting CONTINUE statement
  
  return(if_stmt);
}


void  doIfForConsistent(SgExpression *gref)
{SgStatement *if_stmt;
// creating
//          IF ( GROUP .EQ. 0) THEN
//               GROUP = crtcg(...)
//          ENDIF 
  if_stmt =  new SgIfStmt(SgEqOp(*gref,*new SgValueExp(0) ),*new SgAssignStmt(*gref,*CreateConsGroup(1,1))); 
  cur_st->insertStmtAfter(*if_stmt, *cur_st->controlParent());
  cur_st = if_stmt->lexNext();
  //if(debug_regim){
     //doAssignTo_After( DebReductionGroup( gref->symbol()), D_CreateDebRedGroup());
  //}

  cur_st = cur_st->lexNext(); //END IF
}

void doLogIfForHeap(SgSymbol *heap, int size)
{SgStatement *if_stmt,*stop;
 stop = new SgStatement(STOP_STAT);
 stop ->setExpression(0,*new SgValueExp("Error 166: HEAP limit is exceeded"));
 if_stmt = new SgLogIfStmt(*ARRAY_ELEMENT(heap,1) > *new SgValueExp(size+1),*stop);
 cur_st->insertStmtAfter(*if_stmt);
 (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF 
}

void doLogIfForIOstat(SgSymbol *s, SgExpression *espec, SgStatement *stmt)
{
  SgExpression *cond;
  SgKeywordValExp *kwe = isSgKeywordValExp(espec->lhs());  
  if (!strcmp(kwe->value(),"err")) 
     cond = &operator > (*new SgVarRefExp(s), *new SgValueExp(0));
  else
     cond = &operator < (*new SgVarRefExp(s), *new SgValueExp(0));

  SgStatement *goto_stmt = new SgGotoStmt(*((SgLabelRefExp *) espec->rhs())->label());
  SgStatement *if_stmt =  new SgLogIfStmt(*cond,*goto_stmt);  
  stmt->insertStmtAfter(*if_stmt, *stmt->controlParent());
  (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF 
  BIF_LINE(if_stmt->thebif)   = stmt->lineNumber();
  BIF_LINE(goto_stmt->thebif) = stmt->lineNumber();
   
}

void doIfForDelete(SgSymbol *sg, SgStatement *stmt)
{SgStatement *if_stmt,*delst;
     //delst = new SgAssignStmt(*DVM000(ndvm++),*DeleteObject(new SgVarRefExp(*sg)));
     //FREE_DVM(1);
 delst = DeleteObject_H(new SgVarRefExp(*sg)); 
 if_stmt = new SgLogIfStmt(SgNeqOp(*new SgVarRefExp(sg), *new SgValueExp(0)),*delst);
 InsertNewStatementBefore(if_stmt,stmt);
 (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF 
}

void doLogIfForAllocated(SgExpression *objref, SgStatement *stmt)
{SgStatement *if_stmt,*call;
 call = DataExit(objref,0); 
 if_stmt = new SgLogIfStmt(*AllocatedFunction(objref),*call);
 InsertNewStatementBefore(if_stmt,stmt);
 (if_stmt->lexNext()->lexNext()) -> extractStmt(); //extract ENDIF 
}

int isDoEndStmt(SgStatement *stmt)
{
 SgLabel *lab, *do_lab;
 SgForStmt *parent;
 if(!(lab=stmt->label()) && stmt->variant() != CONTROL_END) //the statement has no label and
   return(0);                                               //is not ENDDO 
 parent = isSgForStmt(stmt->controlParent());
 if(!parent)  //parent isn't DO statement
   return(0);
 do_lab = parent->endOfLoop(); // label of loop end or NULL
 if(do_lab) //  DO statement with label
   if(lab && LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(do_lab->thelabel))
                           // the statement label is the label of loop end  
     return(1);
   else
     return(0);
 else   //  DO statement without label
   if(stmt->variant() == CONTROL_END)
     return(1);
   else
     return(0);
}

int isDoEndStmt_f90(SgStatement *stmt)
{// loop header may be
 //  DO <label> i=N1,N2,N3  or  DO <label> WHILE ( <condition> )

 SgLabel *lab; // *do_lab;
 SgStatement *parent;
 if(!(lab=stmt->label()) && stmt->variant() != CONTROL_END) //the statement has no label and
   return(0);                                               //is not ENDDO 
 parent = stmt->controlParent();
 if(parent->variant()!=FOR_NODE && parent->variant()!=WHILE_NODE)
   return(0);

 if(BIF_LABEL_USE(parent->thebif)) //  DO statement with label
   if(lab && LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(BIF_LABEL_USE(parent->thebif)))
                           // the statement label is the label of loop end  
     return(1);
   else
     return(0);
 else   //  DO statement without label
   if(stmt->variant() == CONTROL_END)
     return(1);
   else
     return(0);
}

SgStatement * lastStmtOfDo(SgStatement *stdo)
{ SgStatement *st;
// second version  (change 04.03.08) 
    st = stdo;
RE: st = st->lastNodeOfStmt();
    if((st->variant() == FOR_NODE) || (st->variant() == WHILE_NODE))
          goto RE;

    else if(st->variant() == LOGIF_NODE)
          return(st->lexNext());

    else
          return(st);
  

/*
  SgLabel *lab;
  SgForStmt *loop;
  SgLabel *dolab; 
  if ((loop = isSgForStmt(stdo)) != NULL)
    dolab = loop->endOfLoop();
  else
    return(NULL);
  if(do_lab) { //DO statement with label
    for(st=stdo; st; st = st->lexNext()) 
      if((lab=st->label()) != NULL) //the statement has label 
          if(LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(dolab->thelabel))
                           // the statement label is the label of loop end 
             if(st->variant() == LOGIF_NODE)
                   return(st->lexNext());
             else
                   return(st);

  } else  //DO statement without label
      for(st=stdo; st; st = st->lexNext()) 
         if(st->variant() == CONTROL_END && st->controlParent() == stdo) 
            return(st);

  return(NULL); //error situation
*/

}

SgStatement * lastStmtOfIf(SgStatement *stif)
{ SgStatement *st;
// look for endif
    st = stif;
RE: st = st->lastNodeOfStmt();
    if((st->variant() == ELSEIF_NODE) )
          goto RE;
    else
          return(st);
}

SgStatement * lastStmtOf(SgStatement *st)
{ SgStatement *last;
     
      if(st->variant() == LOGIF_NODE)
         last = st->lexNext();
      else if((st->variant() == FOR_NODE) || (st->variant() == WHILE_NODE))
         last = lastStmtOfDo(st);
      else if(st->variant() == IF_NODE || st->variant() == ELSEIF_NODE)
         last = lastStmtOfIf(st); 
      else
         last = st->lastNodeOfStmt();
           
      return(last); 
}

SgStatement *lastStmtOfFile(SgFile *f)
{SgStatement *stmt, *last=NULL;

 for(stmt=f->firstStatement(); stmt; stmt=stmt->lexNext())
    last = stmt;
 return(last);
}

int isParallelLoopEndStmt(SgStatement *stmt,SgStatement *first_do)
{
    SgLabel *do_lab;
    return( ((do_lab = ((SgForStmt *)first_do)->endOfLoop()) && 
            stmt->label() && 
            (LABEL_STMTNO(stmt->label()->thelabel) == LABEL_STMTNO(do_lab->thelabel)) )
                || 
            (!do_lab && 
            (stmt->variant() == CONTROL_END) && 
            (stmt->controlParent() == first_do))
          );
}

int TightlyNestedLoops_Test(SgStatement *prev_do,SgStatement *dost)
{ SgStatement *end_prev,*end_do; //*next
  SgLabel *dolab,*prevlab;
  end_do = lastStmtOfDo(dost);
  end_prev = lastStmtOfDo(prev_do);
  prevlab = ((SgForStmt *) prev_do)->endOfLoop();
  dolab = ((SgForStmt *) dost)->endOfLoop();
  if(prevlab)
  {  if(dolab && dolab==prevlab)
         return 1;
     else 
     { if(NextExecStat(end_do)!=end_prev )
         return 0;
       else if(end_prev->variant()!=CONT_STAT && end_prev->variant()!=CONTROL_END)
         return 0;
       else
         return 1;
     }
  }
   return(NextExecStat(end_do)==end_prev);
}

SgStatement *NextExecStat(SgStatement *st)
{  SgStatement *next;    
  next = st->lexNext();
  while( (next && (next->variant() == FORMAT_STAT)) || (next->variant() == DATA_DECL) )
      next=next->lexNext();
  return(next);
}

SgStatement *ContinueWithLabel(SgLabel *lab)
{SgStatement *st;
 st = new SgStatement(CONT_STAT);
 st->setLabel(*lab);
 return(st);
}

SgStatement *PrintStat(SgExpression *item)
{//generates the statement: PRINT *, <item>
 SgStatement *print;
 SgExpression *e1,*e2;
 print = new SgStatement(PRINT_STAT);
 //IO item list
 print->setExpression(0,*new SgExprListExp(*item));
 //control list: *
 e1 = new SgKeywordValExp("fmt");
 e2 = new SgKeywordValExp("*");
 print->setExpression(1,*new SgExpression(SPEC_PAIR, e1, e2, NULL));
                                    
 return(print);
}

SgStatement *doIfThenConstrForIND(SgExpression *e, int cnst, int cond, int has_else, SgStatement *stmt, SgStatement *cp) 
{SgStatement *ifst;
 SgExpression *econd;
// creating
//          IF (  e .EQ. cnst)  THEN                cond = 1
//                  .NE.                            cond = 0
//        [ ELSE ]                                  has_else = 1

//          ENDIF 

 econd = cond ? &SgEqOp(*e, *new SgValueExp(cnst)) : &SgNeqOp(*e, *new SgValueExp(cnst));
 if(has_else) //with ELSE clause
   ifst = new SgIfStmt(*econd,*new SgStatement(CONT_STAT),*new SgStatement(CONT_STAT));
 else
   ifst = new SgIfStmt(*econd, *new SgStatement(CONT_STAT));
 stmt->insertStmtAfter(*ifst,*cp);
 ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
 if(has_else)
  ifst->lexNext()->lexNext()->extractStmt(); // extracting second CONTINUE statement
 return(ifst);
}


SgStatement *ReplaceOnByIf(SgStatement *stmt,SgStatement *end_stmt)
{ // stmt - ON directive, end_stmt - END ON directive
  SgStatement *if_stmt, *end_if, *cp;
  SgLabel *lab = NULL;
  char * cmnt=NULL;
  SgSymbol *ar = NULL;
  SgExpression *el = NULL,*ei[MAX_DIMS];
  SgExpression *condition=NULL, *index_list=NULL;  
  int iind,i,j,k;
  if(isSgArrayRefExp(stmt->expr(0))) { 
     ar = stmt->expr(0)->symbol();
     el = stmt->expr(0)->lhs(); //index list
  }
  if(stmt->expr(0)->variant() == ARRAY_OP){
     ar = stmt->expr(0)->lhs()->symbol();
     el = stmt->expr(0)->lhs()->lhs(); //index list
  }
  if (!el || !TestMaxDims(el,ar,stmt)) //error situation: no subscripts or the number of subscripts > MAX_DIMS
    return NULL; 
  
  cmnt=stmt-> comments();
  if (cmnt)    // statement has preceeding comments 
      BIF_CMNT(stmt->thebif) = NULL;
  
  
  for(i=0;el;el=el->rhs(),i++) 
  { ei[i] = &(el->lhs()->copy());       
    ChangeDistArrayRef(ei[i]);
    if(!IN_COMPUTE_REGION && !INTERFACE_RTS2)  
       ei[i] = &(*ei[i]- *Exprn(LowerBound(ar,i)));
  } 
   iind = ndvm;

   where = stmt;
   if(for_kernel)  /*ACC*/
      cur_st = stmt->lexPrev(); /*ACC*/
   else if(INTERFACE_RTS2)
   {
      cur_st = stmt->lexPrev(); 
      for(j=i; j; j--) 
         index_list= AddListToList(index_list,new SgExprListExp(*DvmType_Ref(ei[j-1])));        
   }
   else
   {
     for(j=i, k=1; j; j--) 
     {  if(IN_COMPUTE_REGION )   /*ACC*/
           doAssignTo(VECTOR_REF(indexArraySymbol(ar),k++),ei[j-1]);/*ACC*/ 
        else 
           doAssignStmtAfter(ei[j-1]);
        cur_st->addAttribute (OMP_CRITICAL); /*OMP*/     
     }      
   } 

  cp = stmt->controlParent(); /*ACC*/ 

  if(IN_COMPUTE_REGION && !for_kernel)       /*ACC*/
    condition =  & SgNeqOp(INTERFACE_RTS2 ? *HasLocalElement_H2(NULL,ar,i,index_list) : *HasLocalElement(NULL,ar,indexArraySymbol(ar)), *new SgValueExp(0) );
  else if(for_kernel)                       /*ACC*/
    condition = LocalityConditionInKernel(ar,ei);    /*ACC*/
  else
    condition = & SgNeqOp(INTERFACE_RTS2 ? *HasElement(HeaderRef(ar),i,index_list) : *TestElement(HeaderRef(ar),iind), *new SgValueExp(0) );
  if_stmt =  new SgIfStmt(*condition,*new SgStatement(CONT_STAT));
  if_stmt->addAttribute (OMP_CRITICAL); /*OMP*/

  cur_st->insertStmtAfter(*if_stmt,*cp); 
               //(cur_st->lexNext())-> extractStmt(); //extract CONTINUE (CONT_STAT)
               //cur_st = if_stmt->lexNext(); //ENDIF 
  end_if = if_stmt->lexNext()->lexNext();
  if(cmnt)
      if_stmt -> setComments(cmnt);
  
  TransferBlockIntoIfConstr(if_stmt,stmt,end_stmt);   
  SET_DVM(iind);
  cur_st = end_if; //if_stmt->lexNext(); //ENDIF 
  return(if_stmt);
}

void TransferStmtAfter(SgStatement *stmt, SgStatement *where)
{
 stmt->extractStmt();
 where->insertStmtAfter(*stmt);
}

void TransferBlockIntoIfConstr(SgStatement *ifst, SgStatement *stmt1, SgStatement *stmt2)
{SgStatement *st, *where;
  if(!stmt1 || !stmt2)
    return;
  st = stmt1->lexNext();
  where = ifst->lexNext(); 
  while(st != stmt2){
    st->extractStmt();
    where->insertStmtBefore(*st,*ifst);
    st=stmt1->lexNext();
  }
  where->extractStmt();
}

void TransferStatementGroup(SgStatement *first_st, SgStatement *last_st, SgStatement *st_end)
{
    SgStatement *st, *next;
   
    for (st = first_st; IN_STATEMENT_GROUP(st); st = next)
    {
            next = lastStmtOf(st)->lexNext();
            st->extractStmt();
            st_end->insertStmtBefore(*st, *st_end->controlParent());                          
    }
}

void TransferStatementBlock(SgStatement *first_st, SgStatement *out_st, SgStatement *st_end)
{
    SgStatement *st, *next;
   
    for (st = first_st; st!=out_st; st = next)
    {
            next = lastStmtOf(st)->lexNext();
            st->extractStmt();
            st_end->insertStmtBefore(*st, *st_end->controlParent());                          
    }
}


void ReplaceComputedGoTo(stmt_list *gol)
{//GO TO (lab1,lab2,..,labk), <int_expr>
// is replaced by
//       [  iv = int_expr     ]
//          IF ( iv.EQ.1) THEN
//            GO TO lab1 
//          ENDIF 
//          IF ( iv.EQ.2) THEN
//            GO TO lab2 
//          ENDIF 
//           . . .
//          IF ( iv.EQ.k) THEN
//            GO TO labk 
//          ENDIF
  stmt_list *gl, *gotol; 
  SgStatement *ass, *ifst, *stmt;
  SgLabel *lab_st, *labgo;
  SgGotoStmt *gost;
  SgExpression  *cond, *el;
  SgSymbol *sv;
  int lnum,i;

  stmt = gol->st;
  lnum = stmt->lineNumber();
  lab_st = stmt->label();
  if(isSgVarRefExp(stmt->expr(1)))
  {  sv = stmt->expr(1)->symbol();
     ass = NULL;
  }
  else
  {  sv = DebugGoToSymbol(stmt->expr(1)->type());
     ass = new SgAssignStmt (*new SgVarRefExp(sv),*stmt->expr(1));
     stmt->insertStmtBefore(*ass,*stmt->controlParent());//inserting before stmt
     if(lab_st)
       ass-> setLabel(*lab_st);
     BIF_LINE(ass->thebif) = lnum;
  }
  gotol = gol->next;
  for(el=stmt->expr(0),i=1; el; el=el->rhs(),i++)
  {
    labgo = ((SgLabelRefExp *) (el->lhs()))->label();
    gost = new SgGotoStmt(*labgo);
    BIF_LINE(gost->thebif) = lnum;
    gl = new stmt_list;
    gl->st = gost;
    gl->next = gotol;
    gotol = gl;
    cond = &SgEqOp(*new SgVarRefExp(sv), *new SgValueExp(i));
    ifst = new SgIfStmt( *cond, *gost);
    stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt 

    if(i==1 && lab_st && !ass ) 
      ifst-> setLabel(*lab_st);
  }
  stmt->extractStmt();
  gol->next = gotol;
}

void ReplaceArithIF(stmt_list *gol)
{//IF (expr) lab1,lab2,lab3
// is replaced by
//       [  iv = expr        ]
//          IF ( v.LT.0) THEN
//            GO TO lab1 
//          ENDIF 
//          IF ( v.EQ.0) THEN
//            GO TO lab2 
//          ENDIF 
//          //IF ( v.GT.0) THEN
//            GO TO lab3
//          //ENDIF
  stmt_list *gl, *gotol;  
  SgStatement *ass, *ifst, *stmt;
  SgLabel *lab_st, *labgo;
  SgGotoStmt *gost;
  SgExpression *cond;
  SgSymbol *sv;
  int lnum;

  stmt = gol->st;
  lnum = stmt->lineNumber();
  lab_st = stmt->label(); 
  if(isSgVarRefExp(stmt->expr(0)))
  {  sv = stmt->expr(0)->symbol();
     ass = NULL;
  }
  else
  {  sv = DebugGoToSymbol(stmt->expr(0)->type());
     ass = new SgAssignStmt (*new SgVarRefExp(sv),*stmt->expr(0));
     stmt->insertStmtBefore(*ass,*stmt->controlParent());//inserting before stmt
     if(lab_st)
       ass-> setLabel(*lab_st);
  }

  gotol = gol->next;
  labgo = ((SgLabelRefExp *) (stmt->expr(1)->lhs()))->label();
  gost = new SgGotoStmt(*labgo);
  BIF_LINE(gost->thebif) = lnum;
  gl = new stmt_list;
  gl->st = gost;
  gl->next = gotol;
  gotol = gl;
  cond = &operator < (*new SgVarRefExp(sv), *new SgValueExp(0));
  ifst = new SgIfStmt( *cond, *gost);
  stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt
  if(lab_st && !ass) 
     ifst-> setLabel(*lab_st);

  labgo = ((SgLabelRefExp *) (stmt->expr(1)->rhs()->lhs()))->label();
  gost = new SgGotoStmt(*labgo);
  BIF_LINE(gost->thebif) = lnum;
  gl = new stmt_list;
  gl->st = gost;
  gl->next = gotol;
  gotol = gl;
  cond = &SgEqOp(*new SgVarRefExp(sv), *new SgValueExp(0));
  ifst = new SgIfStmt(*cond, *gost);
  stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt

  labgo = ((SgLabelRefExp *) (stmt->expr(1)->rhs()->rhs()->lhs()) )->label();
  gost = new SgGotoStmt(*labgo);
  BIF_LINE(gost->thebif) = lnum;
  gl = new stmt_list;
  gl->st = gost;
  gl->next = gotol;
  gotol = gl;
  //cond = &operator > (*new SgVarRefExp(sv), *new SgValueExp(0));
  //ifst = new SgIfStmt( *cond, *gost);
  //stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt
  stmt->insertStmtBefore(*gost,*stmt->controlParent());//inserting before stmt
  stmt->extractStmt();
  gol->next = gotol;
}

SgStatement *IncludeLine(char *str)
{ SgExpression *estr;
  SgStatement  *incl_line;
  
  incl_line = new SgStatement(INCLUDE_LINE);
  estr = new SgExpression(STMT_STR);
  NODE_STR(estr->thellnd)= str;   //  for example: "\"dvm.h\"","<malloc.h>" 
  incl_line->setExpression(0,*estr);
  return(incl_line);
}

SgStatement *PreprocessorDirective(char *str)
{ SgExpression *estr;
  SgStatement  *pre_dir;
  
  pre_dir = new SgStatement(PREPROCESSOR_DIR);
  estr = new SgExpression(STMT_STR);
  NODE_STR(estr->thellnd)= str;   //  for example: "#ifdef" 
  pre_dir->setExpression(0,*estr);
  return(pre_dir);
}

SgStatement *PreprocessorDirective(const char *str_in)
{
    SgExpression *estr;
    SgStatement  *pre_dir;
    char *str = new char[strlen(str_in) + 1];
    strcpy(str, str_in);
    
    pre_dir = new SgStatement(PREPROCESSOR_DIR);
    estr = new SgExpression(STMT_STR);
    NODE_STR(estr->thellnd) = str;   //  for example: "#ifdef" 
    pre_dir->setExpression(0, *estr);
    return(pre_dir);
}

SgStatement *ifdef_dir(char *str)
{ char *dir = new char  [8 + strlen(str)];
  dir[0] = '\0';
  strcat(dir,"#ifdef ");
  strcat(dir,str); 
  
  return(PreprocessorDirective(dir));
}

SgStatement *ifndef_dir(char *str)
{ char *dir = new char  [9 + strlen(str)];
  dir[0] = '\0';
  strcat(dir,"#ifndef ");
  strcat(dir,str); 

  return(PreprocessorDirective(dir));
}

SgStatement *endif_dir()
{  return(PreprocessorDirective("#endif"));}

SgStatement *else_dir()
{  return(PreprocessorDirective("#else"));}


int isExecutableDVMHdirective(SgStatement *stmt)
{
    switch(stmt->variant()) {
      
       case DVM_PARALLEL_ON_DIR:
       case DVM_ASYNCHRONOUS_DIR:
       case DVM_ENDASYNCHRONOUS_DIR:
       case DVM_REDUCTION_START_DIR:
       case DVM_REDUCTION_WAIT_DIR: 
       case DVM_SHADOW_GROUP_DIR:
       case DVM_SHADOW_START_DIR:      
       case DVM_SHADOW_WAIT_DIR:
       case DVM_REMOTE_ACCESS_DIR:     
       case DVM_NEW_VALUE_DIR:  
       case DVM_REALIGN_DIR:
       case DVM_REDISTRIBUTE_DIR: 
       case DVM_ASYNCWAIT_DIR:
       case DVM_F90_DIR:
       case DVM_CONSISTENT_START_DIR: 
       case DVM_CONSISTENT_WAIT_DIR:

       case DVM_INTERVAL_DIR:
       case DVM_ENDINTERVAL_DIR:
       case DVM_OWN_DIR: 
       case DVM_DEBUG_DIR:
       case DVM_ENDDEBUG_DIR:
       case DVM_TRACEON_DIR:
       case DVM_TRACEOFF_DIR:
       case DVM_BARRIER_DIR:
       case DVM_CHECK_DIR:

       case DVM_TASK_REGION_DIR:	          
       case DVM_END_TASK_REGION_DIR:
       case DVM_ON_DIR: 
       case DVM_END_ON_DIR:                
       case DVM_MAP_DIR:     
       case DVM_RESET_DIR:
       case DVM_PREFETCH_DIR:  
       case DVM_PARALLEL_TASK_DIR: 
 
       case ACC_REGION_DIR: 
       case ACC_END_REGION_DIR:   
       case ACC_GET_ACTUAL_DIR:
       case ACC_ACTUAL_DIR:
       case ACC_CHECKSECTION_DIR:
       case ACC_END_CHECKSECTION_DIR:
            return 1;
       default:
            return 0;
    }

}

void DeleteSaveAttribute(SgStatement *stmt)
{ SgExpression *el = stmt->expr(2);
  if(!el) return;
  if( el->lhs()->variant()==SAVE_OP )
  { 
    if(!el->rhs()) 
      BIF_LL3(stmt->thebif) = NULL;
    else
      BIF_LL3(stmt->thebif) = el->rhs()->thellnd;
    return;  
  }
  SgExpression *el1 = el; 
  for(el=el->rhs(); el; el=el->rhs())
  {  
     if(el->lhs()->variant()==SAVE_OP)
     {
        el1->setRhs(el->rhs());
        return;
     }
  }
}
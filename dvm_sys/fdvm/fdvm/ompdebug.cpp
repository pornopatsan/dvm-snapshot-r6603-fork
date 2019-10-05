#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#undef IN_DVM_
#include "dvm.h"
#define  Max(a,b) ((a)>(b)?(a):(b))

#define MaxContextBufferLength 4000

struct ref_list {
    SgExpression *ref;
    ref_list *next;
} *ListOfRefs = NULL;

int isIOStmt (SgStatement *st) {
    switch(st->variant ()){
        case WRITE_STAT:
        case PRINT_STAT:
        case READ_STAT:
        case OPEN_STAT:
        case CLOSE_STAT:
        case ENDFILE_STAT:
        case BACKSPACE_STAT:
        case INQUIRE_STAT:
        case REWIND_STAT:
        return 1;
    }
    return 0;
}

void IntoArrayRefList (SgExpression *exp) {
    if (ListOfRefs == NULL) {
        ListOfRefs = new ref_list;
        ListOfRefs->ref = exp;
        ListOfRefs->next = NULL;
    } else {
        ref_list *tmp = new ref_list;
        tmp->ref = exp;
        tmp->next = ListOfRefs;
        ListOfRefs = tmp;
    }
}

int InArrayRefList (SgExpression *exp) {
    if (ListOfRefs == NULL) {
        return 0;
    } else {
        for (ref_list *tmp = ListOfRefs; tmp; tmp = tmp->next) {
            if (ExpCompare(tmp->ref, exp)) return 1;
        }
    }
    return 0;
}

void ClearArrayRefList () {
    if (ListOfRefs == NULL) {
        return;
    }
    for (ref_list *tmp=ListOfRefs; ListOfRefs != NULL; ) {    
        tmp = ListOfRefs;
        ListOfRefs = ListOfRefs->next;
        tmp->ref = NULL;
        tmp->next = NULL;
        delete tmp;
    }
    ListOfRefs = NULL;    
}


void DBGSearchVarsInFunction (SgStatement *func);
void RegisterSymbol (SgSymbol *sym);
void RegistrateVariable (SgSymbol *sym);
void RegisterArray(SgSymbol *sym);
void RegisterAllocatableArrays(SgStatement *stat);
void UnregisterAllocatableArrays(SgStatement *stat);
void RegisterVar(SgSymbol *sym);
int GenerateCallGetHandle (char * strContextString);
void InstrumentOmpParallelDir (SgStatement *st,char * strContextString);
void InstrumentOmpDoDir (SgStatement *st,char * strContextString);
void InstrumentSerialDoLoop(SgStatement *st, char *strStaticContext);
void InstrumentAssignStat(SgStatement *st, char *strStaticContext);
void InstrumentIfStat (SgStatement *st, char *strStaticContext);
void InstrumentProcStat(SgStatement *st, char *strStaticContext);
void InstrumentFuncCall (SgStatement *st, SgExpression *exp);
void InstrumentFunctionBegin(SgStatement *st, char *strStaticContext, SgStatement *func);
void InstrumentFunctionEnd(SgStatement *st, SgStatement *func);
void InstrumentGotoStmt(SgStatement *st);
void InstrumentExitFromLoops (SgStatement *st);
void InstrumentOmpSingleDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpCriticalDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpOrderelDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpMasterDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpBarrierDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpFlushDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpThreadPrivateDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpThreadPrivateDir (SgStatement *st, SgStatement *before, char *strStaticContext);
void InstrumentOmpSectionsDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpSectionDir (SgStatement *st, char *strStaticContext);
void InstrumentOmpWorkshareDir (SgStatement *st, char *strStaticContext);
void InstrumentExitStmt (SgStatement *stat);
SgStatement *GetLastStatementOfLoop (SgStatement *forst);
void InstrumentReadVar (SgStatement *st, SgExpression *exp, SgArrayRefExp *var);
void InstrumentReadArray (SgStatement *st, SgExpression *exp, SgArrayRefExp *var);
void InstrumentIntervalDir (SgStatement *bst, SgStatement *st, char *strStaticContext);
void InstrumentIOStmt (SgStatement *st, char *strStaticContext);
void MarkFormalParameters (SgStatement *st);
void DeclareExternalProcedures (SgStatement *debug);
void UpdateIncludeVarsFile(SgStatement *st, const char *input_file);
void UpdateIncludeInitFile(SgStatement *st, const char *input_file);
SgExpression *GetOmpAddresMem (SgExpression *exp);
void FindExternalProcedures (SgStatement *debug);
void GenerateNowaitPlusBarrier (SgStatement *st);
void GenerateFileAndLine (SgStatement *st, char *strStaticContext);
SgStatement *GetFirstExecutableStatement (SgStatement *func);
SgStatement *GetFirstExecutableNotDebugStatement (SgStatement *func);

int nArrStaticHandleCount = 0; //StaticContextStringsCount
int nArrHandleCount = 0; //Dynamic
int nMaxArrHandleCount = 0;
SgVarRefExp *varThreadID = NULL;
SgSymbol *symStatMP = NULL;
SgSymbol *symDynMP = NULL;
SgStatement *stLastDebug = NULL;
SgValueExp *C4,*C3,*C2,*C1,*C0, *M1;
SgVarRefExp *atomic_varref = NULL;

SgSymbol *sym_dbg_init=NULL;
SgSymbol *sym_dbg_finalize=NULL;
SgSymbol *symDbgInitHandles=NULL;
SgSymbol *sym_dbg_get_handle=NULL;
SgSymbol *sym_dbg_regarr=NULL;
SgSymbol *sym_dbg_unregarr=NULL;
SgSymbol *sym_dbg_regvar=NULL;
SgSymbol *sym_dbg_before_parallel=NULL;
SgSymbol *sym_dbg_after_parallel=NULL;
SgSymbol *sym_dbg_parallel_event=NULL;
SgSymbol *sym_dbg_parallel_event_end=NULL;

SgSymbol *sym_dbg_before_omp_loop=NULL;
SgSymbol *sym_dbg_after_omp_loop=NULL;
SgSymbol *sym_dbg_omp_loop_event=NULL;

SgSymbol *sym_dbg_before_loop=NULL;
SgSymbol *sym_dbg_after_loop=NULL;
SgSymbol *sym_dbg_loop_event=NULL;

SgSymbol *sym_dbg_write_var_begin=NULL;
SgSymbol *sym_dbg_write_arr_begin=NULL;
SgSymbol *sym_dbg_write_var_end=NULL;
SgSymbol *sym_dbg_write_arr_end=NULL;
SgSymbol *sym_dbg_read_var=NULL;
SgSymbol *sym_dbg_read_arr=NULL;

SgSymbol *sym_dbg_regcommon=NULL;
SgSymbol *sym_dbg_regpararr=NULL;
SgSymbol *sym_dbg_regparvar=NULL;
SgSymbol *sym_dbg_get_addr=NULL;

SgSymbol *sym_dbg_before_sections=NULL;
SgSymbol *sym_dbg_after_sections=NULL;
SgSymbol *sym_dbg_section_event=NULL;
SgSymbol *sym_dbg_section_event_end=NULL;
SgSymbol *sym_dbg_before_single=NULL;
SgSymbol *sym_dbg_single_event=NULL;
SgSymbol *sym_dbg_single_event_end=NULL;
SgSymbol *sym_dbg_after_single=NULL;
SgSymbol *sym_dbg_before_workshare=NULL;
SgSymbol *sym_dbg_after_workshare=NULL;
SgSymbol *sym_dbg_master_begin=NULL;
SgSymbol *sym_dbg_master_end=NULL;
SgSymbol *sym_dbg_before_critical=NULL;
SgSymbol *sym_dbg_critical_event=NULL;
SgSymbol *sym_dbg_critical_event_end=NULL;
SgSymbol *sym_dbg_after_critical=NULL;
SgSymbol *sym_dbg_before_barrier=NULL;
SgSymbol *sym_dbg_after_barrier=NULL;
SgSymbol *sym_dbg_before_flush=NULL;
SgSymbol *sym_dbg_flush_event=NULL;
SgSymbol *sym_dbg_before_ordered=NULL;
SgSymbol *sym_dbg_ordered_event=NULL;
SgSymbol *sym_dbg_after_ordered=NULL;
SgSymbol *sym_dbg_threadprivate=NULL;
SgSymbol *sym_dbg_before_funcall=NULL;
SgSymbol *sym_dbg_funcparvar=NULL;
SgSymbol *sym_dbg_funcpararr=NULL;
SgSymbol *sym_dbg_after_funcall=NULL;
SgSymbol *sym_dbg_funcbegin=NULL;
SgSymbol *sym_dbg_funcend=NULL;
SgSymbol *sym_dbg_if_loop_event=NULL;
SgSymbol *sym_dbg_omp_if_loop_event=NULL;
SgFunctionSymb *FuncLeftBound = NULL;
SgFunctionSymb *FuncRightBound = NULL;
SgSymbol *sym_dbg_interval_begin=NULL;
SgSymbol *sym_dbg_interval_end=NULL;
SgSymbol *sym_dbg_before_io=NULL;
SgSymbol *sym_dbg_after_io=NULL;

int isMainProgram = 0;
void ConvertLoopWithLabelToEnddoLoop (SgStatement *stat) {
	SgForStmt *forst = isSgForStmt (stat);       
	if (forst != NULL) {
		if (forst->isEnddoLoop()) return;
		if (!forst->convertLoop()) {    
			SgStatement *last_st,*lst;
			last_st= LastStatementOfDoNest(forst);
			if(last_st != (lst=forst->lastNodeOfStmt()) || last_st->variant()==LOGIF_NODE) {
				last_st=ReplaceLabelOfDoStmt(forst,last_st, GetLabel());
				ReplaceDoNestLabel_Above(last_st,forst,GetLabel());
				forst->convertLoop();
			}
		}
	}
}

void ComputedGoTo_to_IfGoto (SgStatement *stmt)
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
  SgStatement *ass, *ifst;
  SgLabel *lab_st, *labgo;
  SgGotoStmt *gost;
  SgExpression  *cond, *el;
  SgSymbol *sv;
  int lnum,i;
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
  for(el=stmt->expr(0),i=1; el; el=el->rhs(),i++)
  {
    labgo = ((SgLabelRefExp *) (el->lhs()))->label();
    gost = new SgGotoStmt(*labgo);
    BIF_LINE(gost->thebif) = lnum;
    cond = &SgEqOp(*new SgVarRefExp(sv), *new SgValueExp(i));
    ifst = new SgIfStmt( *cond, *gost);
    stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt 

    if(i==1 && lab_st && !ass ) 
      ifst-> setLabel(*lab_st);
  }
  Extract_Stmt(stmt);
}

void ArithIF_to_IfGoto(SgStatement *stmt)
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
  SgStatement *ass, *ifst;
  SgLabel *lab_st, *labgo;
  SgGotoStmt *gost;
  SgExpression *cond;
  SgSymbol *sv;
  int lnum;

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
  labgo = ((SgLabelRefExp *) (stmt->expr(1)->lhs()))->label();
  gost = new SgGotoStmt(*labgo);
  BIF_LINE(gost->thebif) = lnum;
  cond = &operator < (*new SgVarRefExp(sv), *new SgValueExp(0));
  ifst = new SgIfStmt( *cond, *gost);
  stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt
  if(lab_st && !ass) 
     ifst-> setLabel(*lab_st);

  labgo = ((SgLabelRefExp *) (stmt->expr(1)->rhs()->lhs()))->label();
  gost = new SgGotoStmt(*labgo);
  BIF_LINE(gost->thebif) = lnum;
  cond = &SgEqOp(*new SgVarRefExp(sv), *new SgValueExp(0));
  ifst = new SgIfStmt(*cond, *gost);
  stmt->insertStmtBefore(*ifst,*stmt->controlParent());//inserting before stmt
  labgo = ((SgLabelRefExp *) (stmt->expr(1)->rhs()->rhs()->lhs()) )->label();
  gost = new SgGotoStmt(*labgo);
  BIF_LINE(gost->thebif) = lnum;
  stmt->insertStmtBefore(*gost,*stmt->controlParent());//inserting before stmt
  Extract_Stmt(stmt);
}


void SearchVarAndArrayInExpression(SgStatement *st, SgExpression *exp);
void RegisterCommonBlock (SgStatement *st, SgStatement *func) {    
    char *strStaticContext = new char [MaxContextBufferLength];
    SgExpression *exp = st->expr(0);
    for (SgExpression *ex=exp; ex; ex=ex->rhs()) {
        SgExpression *e=ex->lhs ();
        if (e != NULL) {
            SgSymbol *sym=ex->symbol();
            if (strcmp (sym->identifier(),"dbg_stat")&&
                strcmp (sym->identifier(),"dbg_dyn")&&
                strcmp (sym->identifier(),"dbg_thread")) {
			SgCallStmt *fe;
			SgStatement *stFirst = GetFirstExecutableNotDebugStatement(func);
			if (stFirst == NULL) continue;
			if (sym_dbg_regcommon == NULL) sym_dbg_regcommon = new SgSymbol (PROCEDURE_NAME, "dbg_regcommon");
			fe = new SgCallStmt(*sym_dbg_regcommon);
			SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
			*arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
			fe->addArg(**arrStaticRef);
			fe->addArg(*varThreadID);
			fe->addAttribute(DEBUG_STAT);
			stFirst->insertStmtBefore(*fe, *stFirst->controlParent());
			sprintf (strStaticContext, "*type=common_name*file=%s*line1=%d*name1=%s*name2=%s",st->fileName(),st->lineNumber(),sym->identifier(),UnparseExpr (e));
			GenerateCallGetHandle (strStaticContext);
            }
        }
    }
    delete strStaticContext;
}
void MarkSymbolsInDecl (SgStatement *st) {
	for (SgExpression *ex=st->expr(2); ex; ex=ex->rhs()) {
		if (ex != NULL) {
			SgExprListExp *list = isSgExprListExp (ex);
			if (list !=NULL){        
				for (int i=0; i<list->length (); i++) {
					SgExpression *exp = list->elem(i);
					if (exp->variant()== SAVE_OP){
						for (SgExpression *expr=st->expr(0); expr; expr=expr->rhs()) {
							SgExprListExp *varlist = isSgExprListExp (expr);
							if (varlist !=NULL){        
								for (int j=0; j<varlist->length (); j++) {
									SgExpression *varexp = varlist->elem(j);
									switch (varexp->variant ()){
										case ARRAY_REF: 
										case VAR_REF: varexp->symbol()->addAttribute(SAVE_VAR);
										break;
									}
									
								}
							}
						}
						break;
					}
				}
			}
		}
	}
}

void MarkSymbolsInCommon (SgStatement *st) {
    for (SgExpression *ex=st->expr(0); ex; ex=ex->rhs()) {
        SgExpression *e=ex->lhs ();
        if (e != NULL) {
            SgExprListExp *list = isSgExprListExp (e);
            if (list !=NULL){        
                for (int i=0; i<list->length (); i++) {
                    SgExpression *exp = list->elem(i);
                    switch (exp->variant ()){
                        case ARRAY_REF: 
                        case VAR_REF: exp->symbol()->addAttribute(COMMON_VAR);
                        break;
                    }
                }
            }
        }
    }
}

void MarkFormalParameters (SgStatement *st) {
    SgFunctionSymb *func = isSgFunctionSymb (st->symbol ());        
    if (func != NULL) {
        for (int i=0; i<func->numberOfParameters(); i++) {
            SgSymbol *sym=func->parameter(i);
            int *pos = new int;
            *pos = i+1;
            switch (sym->variant ()){
                case VARIABLE_NAME: sym->addAttribute(FORMAL_PARAM,(void*) pos, sizeof(int));                     
                    break;
            }
        }            
    }
}
void MarkSymbolsInSave (SgStatement *st) {
    SgExprListExp *list = isSgExprListExp (st->expr(0));
    if (list !=NULL){        
        for (int i=0; i<list->length (); i++) {
            SgExpression *exp = list->elem(i);
            switch (exp->variant ()){
                case ARRAY_REF: 
                case VAR_REF: exp->symbol()->addAttribute(SAVE_VAR); 
                    break;
            }
        }            
    }
}

int GenerateCallGetHandle (char * strContextString) {
    if (stLastDebug != NULL) {
        if (sym_dbg_get_handle == NULL) {
            sym_dbg_get_handle = new SgSymbol(PROCEDURE_NAME, "dbg_get_handle");
        }
        SgCallStmt *fe = new SgCallStmt(*sym_dbg_get_handle);
        SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
        int nLen = strlen (strContextString);
        char *strString = new char [MaxContextBufferLength];
        sprintf (strString,"%d%s**", (nLen+2), strContextString);
        fe->addArg(*arrStaticRef);
        fe->addArg(*new SgValueExp(strString));
        fe->addAttribute(COMMON_VAR);
        stLastDebug->insertStmtBefore(*fe, *stLastDebug->controlParent());                
        return ++nArrStaticHandleCount;
    }
    return -1;
}

int GenerateCallGetHandle (char * strContextString, int nArrStaticHandleCount) {
    if (stLastDebug != NULL) {
        if (sym_dbg_get_handle == NULL) {
            sym_dbg_get_handle = new SgSymbol(PROCEDURE_NAME, "dbg_get_handle");
        }
        SgCallStmt *fe = new SgCallStmt(*sym_dbg_get_handle);
        SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
        int nLen = strlen (strContextString);
        char *strString = new char [MaxContextBufferLength];
        sprintf (strString,"%d%s**", (nLen+2), strContextString);
        fe->addArg(*arrStaticRef);
        fe->addArg(*new SgValueExp(strString));
        fe->addAttribute(COMMON_VAR);
        stLastDebug->insertStmtBefore(*fe, *stLastDebug->controlParent());                
        return nArrStaticHandleCount+1;
    }
    return -1;
}


SgStatement *doOmpAssignStmt(SgExpression *re, SgStatement *before) {
    SgExpression *le;
    SgValueExp * index;
    SgStatement *assign;
    // creating assign statement with right part "re" and inserting it
    // before first executable statement (after last generated statement)
    index = new SgValueExp (nArrHandleCount++);
    le = new SgArrayRefExp(*symDynMP,*index);
    assign = new SgAssignStmt (*le,*re);
    assign->addAttribute(DEBUG_STAT);
    before->insertStmtBefore(*assign,*before->controlParent());
    nMaxArrHandleCount = Max (nMaxArrHandleCount,nArrHandleCount);
    return assign;
}

SgStatement * doOmpAssignTo(SgExpression *le, SgExpression *re, SgStatement *before) {
  SgStatement *assign = new SgAssignStmt (*le,*re);
  assign->addAttribute(DEBUG_STAT);
  before->insertStmtBefore(*assign,*before->controlParent());
  return assign;
}

char *ReplaceInExpr(char *val) { // Delete spaces from expression and replace "*" by "\*"
    int count=0;
    char *res = NULL;
    int vallen = strlen(val);
    for (int i=0; i< vallen; i++) {
        if (val[i]=='*') count++;
        if (val[i]==' ') count--;
    }
    if (count==0) return val;
    res = new char [vallen + count + 1];
    memset(res, 0, vallen + count);
    for (int i=0,j=0; i< vallen; i++,j++) {
        if (val[i]!='*') {
            if (val[i] ==' ') {
                j--;
                continue;
            }
            res[j]=val[i];
        } else {
            res[j++]='\\';
            res[j]=val[i];
        }
    }
    res[vallen + count]='\0';
    return res;
}
void ConvertElseIFToElse_IF(SgStatement *stat) {
	stat->setVariant(IF_NODE);
	addControlEndToStmt(stat->controlParent()->thebif);
}

char *GenerateContextStringForExpressionList (SgExpression *e){
    char *result = NULL;
    int maxlen=0;
    SgExprListExp *exp = isSgExprListExp (e);
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *elem = exp->elem (i);
            if (elem->variant () == VAR_REF) {
                maxlen += strlen(elem->symbol()->identifier ()) + 1;
            } else if (elem->variant () == ARRAY_REF) {
                maxlen += strlen(UnparseExpr (elem)) + 1;
            } else if (elem->variant () == OMP_THREADPRIVATE) {
                maxlen += strlen(elem->lhs ()->symbol()->identifier ()) + 3;
            } else {
                fprintf (stderr, "Error: Incorrect member in EXPR_LIST");
                exit (-1);
            }
        }
        result = new char [maxlen];
        memset(result, 0, maxlen);
        for (int i=0; i<exp->length(); i++) {
            SgExpression *elem = exp->elem (i);
            if (strlen (result)!=0) {
                strcat(result,",");
            } 
            if (elem->variant () == VAR_REF) {
                strcat(result,elem->symbol()->identifier ());
            } else if (elem->variant () == ARRAY_REF) {
                strcat(result,UnparseExpr (elem));
            } else if (elem->variant () == OMP_THREADPRIVATE) {
                strcat(result,"/");
                strcat(result,elem->lhs ()->symbol()->identifier ());
                strcat(result,"/");
            } else {
                fprintf (stderr, "Error: Incorrect member in EXPR_LIST");
                exit (-1);
            }
        }
    }
    if (result == NULL) {
        result = new char[1];
        result[0] = '\0';
    }
        
    return result;
}

void GenerateFileAndLine (SgStatement *st, char *strStaticContext) {
    sprintf(strStaticContext,"%s*file=%s*line1=%d",strStaticContext,st->fileName(),st->lineNumber());
}

SgStatement *GetLastDeclarationStatement (SgStatement *func){
    SgStatement *st = func->lastDeclaration ();
    for (;st && st->lexNext ();st=st->lexNext ()) {
        int variant=st->lexNext()->variant ();
        if (isADeclBif (variant)) continue;
        else switch (variant) {
            case COMM_STAT:
            case SAVE_DECL:
            case DATA_DECL:
            case STMTFN_STAT:
            case ENTRY_STAT:
            case INTERFACE_STMT:
            case INTERFACE_ASSIGNMENT:
            case INTERFACE_OPERATOR:
            case USE_STMT:
            case STRUCT_DECL:
            case FORMAT_STAT:
            case HPF_TEMPLATE_STAT:
            case HPF_PROCESSORS_STAT:
            case DVM_DYNAMIC_DIR:
            case DVM_SHADOW_DIR:
            case DVM_TASK_DIR: 
            case DVM_CONSISTENT_DIR: 
            case DVM_INDIRECT_GROUP_DIR:
            case DVM_REMOTE_GROUP_DIR:
            case DVM_CONSISTENT_GROUP_DIR:
            case DVM_REDUCTION_GROUP_DIR:
            case DVM_INHERIT_DIR: 
            case DVM_ALIGN_DIR:
            case DVM_DISTRIBUTE_DIR:
            case DVM_POINTER_DIR:
            case DVM_HEAP_DIR:
            case DVM_ASYNCID_DIR:
            case DVM_VAR_DECL: continue;
            default: {            
                return st;
            }
        }
    }
    return st;
}

SgStatement *GetFirstExecutableStatement (SgStatement *func){
    SgStatement *st = func->lastDeclaration ()->lexNext ();
    for (;st;st=st->lexNext ()) {
        int variant=st->variant ();
        if (isADeclBif (variant)) continue;
        else switch (variant) {
            case COMM_STAT:
            case SAVE_DECL:
            case DATA_DECL:
            case STMTFN_STAT:
            case ENTRY_STAT:
            case INTERFACE_STMT:
            case INTERFACE_ASSIGNMENT:
            case INTERFACE_OPERATOR:
            case USE_STMT:
            case STRUCT_DECL:
            case FORMAT_STAT:
            case HPF_TEMPLATE_STAT:
            case HPF_PROCESSORS_STAT:
            case DVM_DYNAMIC_DIR:
            case DVM_SHADOW_DIR:
            case DVM_TASK_DIR: 
            case DVM_CONSISTENT_DIR: 
            case DVM_INDIRECT_GROUP_DIR:
            case DVM_REMOTE_GROUP_DIR:
            case DVM_CONSISTENT_GROUP_DIR:
            case DVM_REDUCTION_GROUP_DIR:
            case DVM_INHERIT_DIR: 
            case DVM_ALIGN_DIR:
            case DVM_DISTRIBUTE_DIR:
            case DVM_POINTER_DIR:
            case DVM_HEAP_DIR:
            case DVM_ASYNCID_DIR:
            case DVM_VAR_DECL: continue;
            default: {
                return st;
            }
        }
    }
    return st;
}

SgStatement *GetFirstExecutableNotDebugStatement (SgStatement *func) {
    SgStatement *st = func->lastDeclaration ()->lexNext ();
    for (;st;st=st->lexNext ()) {
        int variant=st->variant ();
        if (isADeclBif (variant)) continue;
        else switch (variant) {
            case COMM_STAT:
            case SAVE_DECL:
            case DATA_DECL:
            case STMTFN_STAT:
            case ENTRY_STAT:
            case INTERFACE_STMT:
            case INTERFACE_ASSIGNMENT:
            case INTERFACE_OPERATOR:
            case USE_STMT:
            case STRUCT_DECL:
            case FORMAT_STAT:
            case HPF_TEMPLATE_STAT:
            case HPF_PROCESSORS_STAT:
            case DVM_DYNAMIC_DIR:
            case DVM_SHADOW_DIR:
            case DVM_TASK_DIR: 
            case DVM_CONSISTENT_DIR: 
            case DVM_INDIRECT_GROUP_DIR:
            case DVM_REMOTE_GROUP_DIR:
            case DVM_CONSISTENT_GROUP_DIR:
            case DVM_REDUCTION_GROUP_DIR:
            case DVM_INHERIT_DIR: 
            case DVM_ALIGN_DIR:
            case DVM_DISTRIBUTE_DIR:
            case DVM_POINTER_DIR:
            case DVM_HEAP_DIR:
            case DVM_ASYNCID_DIR:
            case DVM_VAR_DECL: continue;
            default: {
        	if (st->getAttribute(0,DEBUG_STAT)!=NULL) continue;
                return st;
            }
        }
    }
    return st;
}


void GenerateContextStringForClauses (SgExpression *elem, char *strStaticContext) {
    switch (elem->variant ()) {
        case OMP_PRIVATE: {
            strcat(strStaticContext,"*private=");
            strcat(strStaticContext,GenerateContextStringForExpressionList (elem->lhs ()));
            break;
        }
        case OMP_FIRSTPRIVATE: {
            strcat(strStaticContext,"*firstprivate=");
            strcat(strStaticContext,GenerateContextStringForExpressionList (elem->lhs ()));
            break;
        }
        case OMP_LASTPRIVATE: {
            strcat(strStaticContext,"*lastprivate=");
            strcat(strStaticContext,GenerateContextStringForExpressionList (elem->lhs ()));
            break;
        }
        case OMP_COPYIN: {
            strcat(strStaticContext,"*copyin=");
            strcat(strStaticContext,GenerateContextStringForExpressionList (elem->lhs ()));
            break;
        }
        case OMP_SHARED: {
            strcat(strStaticContext,"*shared=");
            strcat(strStaticContext,GenerateContextStringForExpressionList (elem->lhs ()));
            break;
        }
        case OMP_DEFAULT: {                               
            SgValueExp *val = isSgValueExp (elem->lhs ());
            if (val != NULL) {
                strcat(strStaticContext,"*default=");
                strcat(strStaticContext,NODE_STR(val->thellnd));
            }
            break;
        }
        case OMP_REDUCTION: {                                
            SgExprListExp *ex = isSgExprListExp (elem->lhs ());
            if (ex != NULL) {
                if (ex->elem(0)->variant() == DDOT) {
                    strcat(strStaticContext,"*redop=");
                    strcat(strStaticContext,NODE_STR(ex->elem(0)->lhs()->thellnd));
                    SgExprListExp *e = isSgExprListExp (ex->elem(0)->rhs());
                    if (e != NULL) {
                        strcat(strStaticContext,"*reduction=");
                        strcat(strStaticContext,GenerateContextStringForExpressionList (e));
                    }
                }
            }
            break;
        }
        case OMP_IF: {
            char *ifexpr = UnparseExpr (elem->lhs ());
            if (ifexpr != NULL) {
                strcat(strStaticContext,"*if=");
                strcat(strStaticContext,ReplaceInExpr(ifexpr));
            }
            break;
        }
        case OMP_NUM_THREADS: {
            char *numthreads = UnparseExpr (elem->lhs ());
            if (numthreads != NULL) {
                strcat(strStaticContext,"*num_threads=");
                strcat(strStaticContext,ReplaceInExpr(numthreads));
            }
            break;
        }
        case OMP_SCHEDULE: {
            char *schedule = NULL;
            if (elem->rhs () != NULL ) schedule = UnparseExpr (elem->rhs ());
            SgValueExp *val = isSgValueExp (elem->lhs ());
            if (val != NULL) {
                strcat(strStaticContext,"*schedule=");
                strcat(strStaticContext,NODE_STR(val->thellnd));
            }
            if (schedule != NULL) {
                strcat(strStaticContext,"*chunk_size=");
                strcat(strStaticContext,ReplaceInExpr(schedule));
            }
            break;
        }
        case OMP_ORDERED: {
            strcat(strStaticContext,"*ordered=1");
            break;
        }
        case OMP_NOWAIT: {
            strcat(strStaticContext,"*nowait=1");
            break;
        }
        case OMP_COPYPRIVATE: {
            strcat(strStaticContext,"*copyprivate=");
            strcat(strStaticContext,GenerateContextStringForExpressionList (elem->lhs ()));
            break;
        }
    }
}
	
void TempVarOmpDebug(SgStatement * func) {
    
    SET_DVM(1);
    SgValueExp C16(16);
    SgArrayType *typearray;
    SgStatement *stFirstExecutableFunc = GetFirstExecutableStatement(func);    
    typearray = new SgArrayType(*SgTypeInt());
    typearray = new SgArrayType(*SgTypeFloat());
    typearray-> addRange(*C2);
    Rmem = new SgVariableSymb("r0000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*Rmem->makeVarDeclStmt ());
    typearray = new SgArrayType(*SgTypeDouble());
    typearray-> addRange(*C2);
    Dmem = new SgVariableSymb("d0000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*Dmem->makeVarDeclStmt ());
    typearray = new SgArrayType(*SgTypeInt());
    typearray-> addRange(C16);
    Imem = new SgVariableSymb("i0000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*Imem->makeVarDeclStmt ());
    typearray = new SgArrayType(*SgTypeBool());
    typearray-> addRange(*C2);
    Lmem = new SgVariableSymb("l0000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*Lmem->makeVarDeclStmt ());
    typearray = new SgArrayType(* SgTypeComplex(current_file));
    typearray-> addRange(*C2);
    Cmem = new SgVariableSymb("c0000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*Cmem->makeVarDeclStmt ());
    typearray = new SgArrayType(* SgTypeDoubleComplex(current_file));
    typearray-> addRange(*C2);
    DCmem = new SgVariableSymb("dc000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*DCmem->makeVarDeclStmt ());
    typearray = new SgArrayType(*SgTypeChar());
    typearray-> addRange(*C2);
    Chmem = new SgVariableSymb("ch000m", *typearray, *func);
    stFirstExecutableFunc->insertStmtBefore (*Chmem->makeVarDeclStmt ());
    return;
}

void TypeControlOmpDebug(SgStatement *func, SgStatement *before) {
	int n, k ;    
	SgCallStmt *call = new SgCallStmt(*new SgFunctionSymb(FUNCTION_NAME, "dbg_type_control", *SgTypeInt(), *func));
	TempVarOmpDebug(func);
	nArrHandleCount = 1;
	n = (bind == 1 ) ? 6 : 5;
	//generating assign statement
	// and inserting it before first executable statement
	k = (bind == 1 ) ? 1 : 2;
	call -> addArg(*new SgValueExp(n)); 
	call -> addArg(*new SgArrayRefExp(*symDynMP,*new SgValueExp(1))); 
	call -> addArg(*new SgArrayRefExp(*symDynMP,*new SgValueExp(n+1))); 
	call -> addArg(*new SgArrayRefExp(*Imem,*new SgValueExp(k)));
	call -> addArg(*new SgArrayRefExp(*Imem,*new SgValueExp(k+10)));
	if (sym_dbg_init == NULL) sym_dbg_init = new SgSymbol(PROCEDURE_NAME, "dbg_init");
    SgCallStmt *init = new SgCallStmt(*sym_dbg_init);
    init->addArg(*varThreadID);
    init->addAttribute(DEBUG_STAT);
    before->insertStmtBefore(*init,*before->controlParent());
    if (sym_dbg_finalize == NULL) sym_dbg_finalize = new SgSymbol(PROCEDURE_NAME, "dbg_finalize");
    SgCallStmt *finalize = new SgCallStmt(*sym_dbg_finalize);
    finalize->addAttribute(DEBUG_STAT);
    func->lastNodeOfStmt ()->insertStmtBefore(*finalize,*func);
    symDbgInitHandles = new SgSymbol(PROCEDURE_NAME, "dbg_init_handles");
    init = new SgCallStmt(*symDbgInitHandles);
    init->addAttribute(DEBUG_STAT);
    before->insertStmtBefore(*init,*before->controlParent());
    call->addAttribute(DEBUG_STAT);
    before->insertStmtBefore(*call,*before->controlParent());
	if(bind == 1)
		doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*symDynMP,*C1)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Imem,*C1)),call);	
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Lmem,*C1)),call);	
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Rmem,*C1)),call);	
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Dmem,*C1)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Chmem,*C1)),call);
	if(bind == 1)
		doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*symDynMP,*C2)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Imem,*C2)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Lmem,*C2)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Rmem,*C2)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Dmem,*C2)),call);
	doOmpAssignStmt(GetOmpAddresMem( new SgArrayRefExp(*Chmem,*C2)),call);
	if(bind == 1)
		doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(1)),new SgValueExp(DVMTypeLength()),call); 
	doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(2)),new SgValueExp(TypeSize(SgTypeInt())),call); 
	doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(3)),new SgValueExp(TypeSize(SgTypeBool())),call); 
	doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(4)),new SgValueExp(TypeSize(SgTypeFloat())),call); 
	doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(5)),new SgValueExp(TypeSize(SgTypeDouble())),call); 
	doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(6)),new SgValueExp(TypeSize(SgTypeChar())),call); 
	if(bind == 1)
		doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(11)),new SgValueExp(DVMType()),call); 
    doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(12)),new SgValueExp(VarType_RTS(Imem)),call); 
    doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(13)),new SgValueExp(VarType_RTS(Lmem)),call); 
    doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(14)),new SgValueExp(VarType_RTS(Rmem)),call); 
    doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(15)),new SgValueExp(VarType_RTS(Dmem)),call); 
    doOmpAssignTo(new SgArrayRefExp(*Imem,*new SgValueExp(16)),new SgValueExp(5),call); 
	return;
}

void InstrumentFunctionForOpenMPDebug(SgStatement *func, SgStatement *debug) {
    SgStatement *stat;
    SgStatement *stLastFunc = func->lastNodeOfStmt ();
    SgStatement *stLastSpecFunc = GetLastDeclarationStatement(func);
    SgStatement *stFirstExecutableFunc = GetFirstExecutableStatement(func);
    if (func->variant () == PROG_HEDR) {
        isMainProgram = 1;
        char *data_str = new char[20];
        sprintf(data_str,"include 'dbg_vars.h'");
        SgStatement *st = new SgStatement(DATA_DECL);// creates DATA statement
        SgExpression *es = new SgExpression(STMT_STR);
        NODE_STR(es->thellnd) = data_str;
        st -> setExpression(0,*es); 
        st->addAttribute(DEBUG_STAT);
        stLastSpecFunc -> insertStmtAfter(*st);        
        stLastSpecFunc = st;
        TypeControlOmpDebug (func, stFirstExecutableFunc);
    } else {
     	char *data_str = new char[20];
    	sprintf(data_str,"include 'dbg_vars.h'"); 
    	SgStatement *st = new SgStatement(DATA_DECL);// creates DATA statement
    	SgExpression *es = new SgExpression(STMT_STR);
    	NODE_STR(es->thellnd) = data_str;
    	st -> setExpression(0,*es);
        st->addAttribute(DEBUG_STAT);    
    	stLastSpecFunc -> insertStmtAfter(*st);
        stLastSpecFunc = st;
    }
    char *strStaticContext = new char [MaxContextBufferLength];
    for (stat=func; stat && stat != stLastFunc; stat=stat->lexNext ()) {
        ClearArrayRefList ();
        if (func->variant () != PROG_HEDR) {
            if (stat == stLastSpecFunc) {
                memset(strStaticContext, 0, MaxContextBufferLength);
                strcat(strStaticContext,"*type=function");         
                InstrumentFunctionBegin (stat, strStaticContext, func);
                GenerateCallGetHandle (strStaticContext);
            }
        }
        if (stat->getAttribute(0,DEBUG_STAT)!=NULL) continue;
		if ((stat->variant () == FORALL_STAT) ||
			(stat->variant () == OMP_WORKSHARE_DIR)) {
			stat=stat->lastNodeOfStmt ();
			continue;
		}
        memset(strStaticContext, 0, MaxContextBufferLength);
        if (stat->hasLabel ()&& (stat->variant() != FORMAT_STAT)&& (stat->variant() != CONT_STAT)) {
            SgStatement *tmp = new SgStatement (CONT_STAT);
            tmp->setLabel (*stat->label ());
            stat->insertStmtBefore(*tmp, *stat->controlParent());
            BIF_LABEL(stat->thebif)=NULL;
        }
        /*if (stat->variant () == ARITHIF_NODE) {		
            ArithIF_to_IfGoto(stat);		
            continue;
        }
        if (stat->variant () == COMGOTO_NODE) {		
            ComputedGoTo_to_IfGoto(stat);		
            continue;
        }*/
        if (stat->variant () == COMM_STAT) {
            if (omp_debug>=D3){
                RegisterCommonBlock (stat, func);
            }
            continue;
        }
        if (stat->variant () == OMP_PARALLEL_DIR) {
            if (omp_debug>=D2){
                strcat(strStaticContext,"*type=parallel");
                GenerateFileAndLine (stat, strStaticContext);
                InstrumentOmpParallelDir (stat, strStaticContext);
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_DO_DIR) {
            if (omp_debug>=D2){
                strcat(strStaticContext,"*type=omploop");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpDoDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);            
            }
            continue;
        }
        if (stat->variant () == DVM_INTERVAL_DIR) {
            if (omp_debug==DPERF){
                OpenInterval(stat);
            }
            continue;
        }
        if (stat->variant () == DVM_ENDINTERVAL_DIR) {
            if (omp_debug==DPERF){
                if(!St_frag){
                     err("Unmatched directive",182,stat);
                     break;
                }
                if(St_frag && St_frag->begin_st &&  (St_frag->begin_st->controlParent() != stat->controlParent()))
                err("Misplaced directive",103,stat); //interval must be a block
                strcat(strStaticContext,"*type=interval");            
                GenerateFileAndLine (St_frag->begin_st, strStaticContext);            
				InstrumentIntervalDir (St_frag->begin_st, stat, strStaticContext);
                GenerateCallGetHandle (strStaticContext);
                CloseInterval();
            }
            continue;
        }
        if (stat->variant () == FOR_NODE) {
            if (omp_debug>=D2 && omp_debug!=DPERF){            
                strcat(strStaticContext,"*type=seqloop");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentSerialDoLoop (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
		if (stat->variant()== IF_NODE) {
			if (omp_debug>=D3) {            
				strcat(strStaticContext,"*type=file_name");            
				GenerateFileAndLine (stat, strStaticContext);            
				InstrumentIfStat (stat, strStaticContext);            
				GenerateCallGetHandle (strStaticContext);
			}
			continue;
		}
		if (stat->variant()==ALLOCATE_STMT) {
			RegisterAllocatableArrays (stat);
			continue;
		}
		if (stat->variant()==DEALLOCATE_STMT) {
			UnregisterAllocatableArrays (stat);
			continue;
		}
		//NULLIFY_STMT
		if (stat->variant () == ASSIGN_STAT) {
			//printf ("%d\n",stat->expr(0)->variant());
			//if (stat->expr(0)->lhs()&&stat->expr(0)->lhs()->lhs())
			//	printf ("-%d\n",stat->expr(0)->lhs()->lhs()->variant());
			if (omp_debug>=D3) {            
				strcat(strStaticContext,"*type=file_name");            
				GenerateFileAndLine (stat, strStaticContext);            
				InstrumentAssignStat (stat, strStaticContext);                            
			}
			continue;
		}
        if (stat->variant () == PROC_STAT) {                        
            if (omp_debug>=D2){          
                strcat(strStaticContext,"*type=func_call");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentProcStat (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_SINGLE_DIR) {
            if (omp_debug>=D2){
                strcat(strStaticContext,"*type=single");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpSingleDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_CRITICAL_DIR) {                     
            if (omp_debug>=D2){            
                strcat(strStaticContext,"*type=critical");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpCriticalDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_ORDERED_DIR) {                        
            if (omp_debug>=D2){
                strcat(strStaticContext,"*type=ordered");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpOrderelDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_MASTER_DIR) {
            if (omp_debug>=D2){            
                strcat(strStaticContext,"*type=master");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpMasterDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if ((stat->variant () == OMP_BARRIER_DIR) || (stat->variant () == DVM_BARRIER_DIR)){                        
            if (omp_debug>=D2){            
                strcat(strStaticContext,"*type=barrier");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpBarrierDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);            
            }
            continue;
        }
        if (stat->variant () == OMP_FLUSH_DIR){                        
            if (omp_debug>=D2){            
                strcat(strStaticContext,"*type=flush");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpFlushDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);            
            }
            continue;
        }
        if (stat->variant () == OMP_THREADPRIVATE_DIR){                        
            if (omp_debug>=D2){            
                strcat(strStaticContext,"*type=threadprivate");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpThreadPrivateDir(stat, stFirstExecutableFunc, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_SECTIONS_DIR){                        
            if (omp_debug>=D2){
                strcat(strStaticContext,"*type=sections");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpSectionsDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }
        if (stat->variant () == OMP_SECTION_DIR){
            if (omp_debug>=D2){                        
                strcat(strStaticContext,"*type=sect_ev");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpSectionDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);            
            }
            continue;
        }
        if (stat->variant () == OMP_WORKSHARE_DIR){
            if (omp_debug>=D2){                                                
                strcat(strStaticContext,"*type=workshare");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentOmpWorkshareDir (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);
            }
            continue;
        }       
        if ((stat->variant () == EXIT_STMT) ||
            (stat->variant () == STOP_STAT)) {
            if (omp_debug>=D2){                                                
                InstrumentExitFromLoops (stat);            
                InstrumentExitStmt (stat);
            }
            continue;
        }
        if (stat->variant () == RETURN_STAT) {
            if (omp_debug>=D2){                                                            
                InstrumentExitFromLoops (stat);            
                InstrumentFunctionEnd (stat, func);
            }
            continue;
        }
        if (stat->variant () == GOTO_NODE) {                        
            if (omp_debug>=D2){                                                                            
                InstrumentGotoStmt (stat);
            }
            continue;
        }
        if (isIOStmt (stat)){
            if (omp_debug==DPERF){            
                strcat(strStaticContext,"*type=io");            
                GenerateFileAndLine (stat, strStaticContext);            
                InstrumentIOStmt (stat, strStaticContext);            
                GenerateCallGetHandle (strStaticContext);            
            }
            continue;
        }

    }
    if ((stat->variant () == CONTROL_END) && ((stat->controlParent ()->variant () == FUNC_HEDR) || (stat->controlParent ()->variant () == PROC_HEDR))) {
        if (omp_debug>=D2){
            InstrumentFunctionEnd (stat, func);
        }
    }
    delete strStaticContext;
}

void FindOrDeclareOmpDebugVariables (SgStatement *debug) {
    SgStatement *stat;
    SgSymbol *symThreadID=NULL;
    stLastDebug = debug->lastNodeOfStmt ();
    SgStatement *stLastSpecDebug = GetLastDeclarationStatement(debug);
    for (stat=debug; stat && (stat != stLastSpecDebug->lexNext ()); stat=stat->lexNext ()) {
        if (stat->variant () == EXTERN_STAT) {
                FindExternalProcedures (stat);
                continue;
        }
    	SgVarListDeclStmt *vardecl = isSgVarListDeclStmt (stat);
     	if (vardecl != NULL) {
	    for (int i=0; i< vardecl->numberOfSymbols(); i++) {
    	        SgSymbol *sym = vardecl->symbol(i);
                if (!strcmp (sym->identifier(),"ithreadid")) {
                    symThreadID = sym;
                    continue;
                }
                if (!strcmp (sym->identifier(),"dbg_get_addr")) {
                    sym_dbg_get_addr = sym;
                    continue;
                }
                if (!strcmp (sym->identifier(),"istat_mp")) {
                    symStatMP = sym;
                    SgArrayType *ArrStaticHandle = isSgArrayType (sym->type());
                    if (ArrStaticHandle != NULL) {
                        if (ArrStaticHandle->dimension() == 1) {
                            if (ArrStaticHandle->sizeInDim(0)->isInteger ()) {
                                nArrStaticHandleCount=ArrStaticHandle->sizeInDim(0)->valueInteger ();
                            }
                        }
                    }
                    continue;
			    }
			    if (!strcmp (sym->identifier(),"idyn_mp")) {
                    symDynMP = sym;
				    SgArrayType *ArrHandle = isSgArrayType (sym->type());
				    if (ArrHandle != NULL) {
    					if (ArrHandle->dimension() == 1) {
	    					if (ArrHandle->sizeInDim(0)->isInteger ()) {
		    					nArrHandleCount=ArrHandle->sizeInDim(0)->valueInteger ();
						    }
					    }
				    }
			    }
            }
        } else {
            SgVarDeclStmt *vardec = isSgVarDeclStmt (stat);
            if (vardec != NULL) {
                for (int i=0; i< vardec->numberOfSymbols(); i++) {
                    SgSymbol *sym = vardec->symbol(i);
                    if (!strcmp (sym->identifier(),"ithreadid")) {
                        symThreadID = sym;
                        continue;
                    }
                    if (!strcmp (sym->identifier(),"dbg_get_addr")) {
                        sym_dbg_get_addr = sym;
                        continue;
                    }
                }
            }
        }
    }
    if (nArrStaticHandleCount == 0) {
	    (void)fprintf (stderr, "Error: Array istat_mp in file \"dbg_vars.h\" not found\n");
	    exit(1);
    }
    if (nArrHandleCount == 0) {
    	(void)fprintf (stderr, "Error: Array idyn_mp in file \"dbg_vars.h\" not found\n");
	    exit(1);
    }
    nMaxArrHandleCount = nArrHandleCount;
    if (symThreadID == NULL) {
        SgExprListExp *list = NULL;
        symThreadID = new SgSymbol(VARIABLE_NAME, "ithreadid");
        varThreadID = new SgVarRefExp(symThreadID);
        sym_dbg_get_addr = new SgSymbol(VARIABLE_NAME, "dbg_get_addr");
        list = new SgExprListExp (*varThreadID);
        SgType *type = NULL;
        if (len_DvmType) {
	        SgExpression *le = new SgExpression(LEN_OP);
	        le->setLhs(new SgValueExp(8));
	        type = new SgType(T_INT, le, SgTypeInt());
        } else {
        	type = new SgType(T_INT);
	    }
        if (symStatMP!=NULL) list->append (*new SgVarRefExp(symStatMP));
        if (symDynMP!=NULL) list->append (*new SgVarRefExp(symDynMP));
        if (sym_dbg_get_addr!=NULL) list->append (*new SgVarRefExp(sym_dbg_get_addr));
        SgVarDeclStmt *vdecl = new SgVarDeclStmt (*list,*type);
        vdecl->addAttribute(DEBUG_STAT);
        stLastSpecDebug->insertStmtAfter(*vdecl);
    } else {
        varThreadID = new SgVarRefExp(symThreadID);
    }
}
int ompdbgvar=0;
void Arg_FunctionCallSearch(SgExpression *e, SgStatement *st, SgExpression *parent, int left);
SgExpression *GenerateTemporaryVariable (SgType *type, SgStatement *stat) {	
	char *strString = new char [12];
	sprintf (strString,"dbgomp%d", ompdbgvar++);
	SgStatement *scope = stat->getScopeForDeclare();
	SgSymbol *sym = new SgSymbol(VARIABLE_NAME, strString, type, scope);
	if (type->variant()==T_FLOAT) sym->setType (new SgType (T_DOUBLE));
	SgExpression *expr = new SgVarRefExp (*sym);
	SgStatement *stLastSpecDebug = GetLastDeclarationStatement(scope);
	SgStatement *thrprivate = new SgStatement (OMP_THREADPRIVATE_DIR);
	thrprivate->setExpression(0, *new SgExprListExp (*expr));
	thrprivate->setlineNumber(stat->lineNumber());
	stLastSpecDebug->insertStmtAfter(*thrprivate,*stLastSpecDebug->controlParent());
	SgStatement *vardecl = sym->makeVarDeclStmt ();
	sym->addAttribute(SAVE_VAR);
	vardecl->setlineNumber(stat->lineNumber());
	SgExprListExp *exprlist = isSgExprListExp(vardecl->expr(2));
	if (exprlist != NULL) exprlist->append(*new SgAttributeExp(SAVE_OP));
	else {
		exprlist = new SgExprListExp (*new SgAttributeExp(SAVE_OP));
		vardecl->setExpression(2,*exprlist);
	}
	stLastSpecDebug->insertStmtAfter(*vardecl);
	return expr;
}

void FunctionCallSearch(SgExpression *e, SgStatement *st,SgExpression *parent, int left) 
{
  SgExpression *el;
  if(!e)return;
  if(isSgFunctionCallExp(e)) {
	  for(el=e->lhs(); el; el=el->rhs())
		  Arg_FunctionCallSearch(el->lhs(),st,el,1);
	  if (parent) {
		  if (e->symbol()->type()){
			SgExpression *var=GenerateTemporaryVariable (e->symbol()->type(), st);
			SgAssignStmt *as=new SgAssignStmt (*var,*e);
			as->setlineNumber (st->lineNumber());
			st->insertStmtBefore(*as,*st->controlParent());
			if (left){
				parent->setLhs (*var);
			} else {
				parent->setRhs (*var);
			}
		  }
	  }
	  return;
  }
  if ((e->variant ()!= ASSGN_OP) && (e->variant ()!= POINTST_OP))
	FunctionCallSearch(e->lhs(),st,e,1);
  FunctionCallSearch(e->rhs(),st,e,0);
  return;
}

void Arg_FunctionCallSearch(SgExpression *e, SgStatement *st, SgExpression *parent, int left) 
{ 
	if (!e->rhs ()) {
		FunctionCallSearch(e,st,parent,left);
	} else {
		if (parent) {
			if (e->type()) {
				SgExpression *var=GenerateTemporaryVariable (e->type(), st);
				SgAssignStmt *as=new SgAssignStmt (*var,*e);
				as->setlineNumber (st->lineNumber());
				st->insertStmtBefore(*as,*st->controlParent());
				if (left){
					parent->setLhs (*var);
				} else {
					parent->setRhs (*var);
				}
				FunctionCallSearch(as->expr(0),as,NULL,1);   // left part
				FunctionCallSearch(as->expr(1),as,NULL,0);   // right part
			}
		}
	}
	return;
}

void InstrumentForOpenMPDebug(SgFile *f) {
    SgStatement *stat, *func=NULL;
    SgStatement *debug=NULL;
    stat = f->firstStatement(); // file header 
    C4=new SgValueExp(4);
    C3=new SgValueExp(3);
    C2=new SgValueExp(2);
    C1=new SgValueExp(1);
    C0=new SgValueExp(0);
    M1=new SgValueExp(-1);
    nfrag = 0 ; //counter of intervals for performance analizer 
    St_frag = NULL;
    for(stat=stat->lexNext(); stat; stat=stat->lastNodeOfStmt()->lexNext ()) {
    	// PROGRAM, SUBROUTINE, FUNCTION header
    	if (stat->variant () != PROC_HEDR) continue;
        if(!strcmp(stat->symbol()->identifier(),"dbg_init_handles")) { 
    		debug = func = stat;
		    break;
	    }
    }
    if (func == NULL) {
        (void)fprintf (stderr, "Error: Subroutine DBG_Init_Handles in file \"dbg_init.h\" not found\n");
        exit(1);
    }
    FindOrDeclareOmpDebugVariables (func);
    stat = f->firstStatement(); // file header
    for(stat=stat->lexNext(); stat; stat=stat->lexNext ()) {
        if (!strcmp(stat->fileName(),"dbg_init.h")) {
            stat=stat->lastNodeOfStmt();
            continue;
        }
        if (stat->variant () == COMM_STAT) {
            MarkSymbolsInCommon(stat);
            continue;
        }
        if (stat->variant () == SAVE_DECL) {
            MarkSymbolsInSave(stat);
            continue;
        }
		if (stat->variant () == VAR_DECL) {
			MarkSymbolsInDecl(stat);
			continue;
		}
		if(stat->variant () == DATA_DECL) {
            continue;
        }
        if ((stat->variant () == PROC_HEDR) ||
            (stat->variant () == FUNC_HEDR)) {
                MarkFormalParameters (stat);
                continue;
        }
        if (stat->variant () == FOR_NODE) {
		ConvertLoopWithLabelToEnddoLoop (stat);
		continue;
        }
		if (stat->variant()== ELSEIF_NODE) {
			ConvertElseIFToElse_IF(stat);
		}
		if (stat->variant () == LOGIF_NODE) {
			LogIf_to_IfThen(stat);		
		}
        if (stat->variant () == OMP_ATOMIC_DIR) {
            SgStatement *assign = stat->lexNext ();
            if (atomic_varref == NULL) {
                atomic_varref = new SgVarRefExp(*new SgSymbol (VARIABLE_NAME, "dbg_atomic"));
            }
            stat->setExpression (0, *atomic_varref);
            stat->setVariant (OMP_CRITICAL_DIR);
            SgStatement *endst = new SgStatement (OMP_END_CRITICAL_DIR);
            endst->setlineNumber (stat->lineNumber ());
            endst->setExpression (0, *atomic_varref);
            assign->insertStmtAfter (*endst, *stat);
			SgStatement *tmp = &assign->copy ();
			tmp->setlineNumber (assign->lineNumber ());
            assign->insertStmtAfter (*tmp, *stat);
            assign->extractStmt ();
            continue;
        }
        if (stat->variant () == OMP_PARALLEL_DO_DIR) {
            stat->setVariant (OMP_PARALLEL_DIR);
            SgExprListExp *list = NULL;
            SgExprListExp *parallel_clause = NULL;
            SgExprListExp *do_clause = NULL;
            if (stat->expr(0) != NULL) {
                list = isSgExprListExp (stat->expr(0));
                for (int i=0; i<list->length (); i++) {
                    SgExpression *exp = list->elem (i);
                    switch (exp->variant ()) {
                        case OMP_SCHEDULE:
                        case OMP_ORDERED:
                        case OMP_LASTPRIVATE: {
                            if (do_clause != NULL) {
                                do_clause->append (*exp);
                            } else {
                                do_clause = new SgExprListExp (*exp);
                            }
                            break;
                        }
                        default: {
                            if (parallel_clause != NULL) {
                                parallel_clause->append (*exp);
                            } else {
                                parallel_clause = new SgExprListExp (*exp);
                            }
                            break;
                        }
                    }
                }
            }
            if (parallel_clause != NULL) stat->setExpression (0, *parallel_clause);
            else BIF_LL1(stat->thebif)=NULL;
	    ConvertLoopWithLabelToEnddoLoop (stat->lexNext ());
            SgForStmt *forst= isSgForStmt (stat->lexNext ());
            if (forst) {
                SgStatement *last = GetLastStatementOfLoop (forst)->lexNext ();
                if (last->variant () == OMP_END_PARALLEL_DO_DIR) {
                    SgStatement * tmp = last;
                    last=last->lexNext ();
                    tmp->extractStmt ();
                } 
                SgStatement *dodir = new SgStatement (OMP_DO_DIR);
                if (do_clause != NULL) dodir->setExpression (0, *do_clause);
                dodir->setlineNumber (stat->lineNumber ());
                SgStatement *enddodir = new SgStatement (OMP_END_DO_DIR);
                SgStatement *endparalleldir = new SgStatement (OMP_END_PARALLEL_DIR);
                enddodir->setlineNumber (last->lineNumber ());
                endparalleldir->setlineNumber (last->lineNumber ());
                forst->insertStmtBefore (*dodir, *stat);
                if (forst->controlParent () != NULL) {
                    PTR_BLOB bl1,bl2,blob=NULL;
                    for (bl1 = bl2 = BIF_BLOB1(forst->controlParent()->thebif); (blob == NULL) && bl1; bl1 = BLOB_NEXT (bl1)) {
                        if (BLOB_VALUE (bl1) == forst->thebif) {
                            BLOB_NEXT (bl2) = BLOB_NEXT (bl1);
                            blob=bl1;                            
                        }
                        bl2 = bl1;
                    }
                    for (bl1 = bl2 = BIF_BLOB2(forst->controlParent()->thebif); (blob == NULL) && bl1; bl1 = BLOB_NEXT (bl1)) {
                        if (BLOB_VALUE (bl1) == forst->thebif) {
                            BLOB_NEXT (bl2) = BLOB_NEXT (bl1);
                            blob=bl1;
                        }
                        bl2 = bl1;
                    }
                }                
                appendBfndToList1(forst->thebif, stat->thebif);
                last->insertStmtBefore (*enddodir, *stat);
                last->insertStmtBefore (*endparalleldir, *stat);                
            }
            continue;
        }
        if (stat->variant () == OMP_PARALLEL_SECTIONS_DIR) {
            stat->setVariant (OMP_SECTIONS_DIR);
            SgExprListExp *list = NULL;
            SgExprListExp *parallel_clause = NULL;
            SgExprListExp *section_clause = NULL;            
            if (stat->expr(0) != NULL) {
                list = isSgExprListExp (stat->expr(0));
                for (int i=0; i<list->length (); i++) {
                    SgExpression *exp = list->elem (i);
                    switch (exp->variant ()) {
                        case OMP_LASTPRIVATE: {
                            if (section_clause != NULL) {
                                section_clause->append (*exp);
                            } else {
                                section_clause = new SgExprListExp (*exp);
                            }
                            break;
                        }
                        default: {
                            if (parallel_clause != NULL) {
                                parallel_clause->append (*exp);
                            } else {
                                parallel_clause = new SgExprListExp (*exp);
                            }
                            break;
                        }
                    }
                }
            }
            SgStatement *last = stat->lastNodeOfStmt ();
			last->setVariant (OMP_END_SECTIONS_DIR);
            if (section_clause != NULL) stat->setExpression (0, *section_clause);
            else BIF_LL1(stat->thebif)=NULL;
			SgStatement *parallel = new SgStatement (OMP_PARALLEL_DIR);
            if (parallel_clause != NULL) parallel->setExpression (0, *parallel_clause);
            parallel->setlineNumber (stat->lineNumber ());
            SgStatement *endparallel = new SgStatement (OMP_END_PARALLEL_DIR);
            endparallel->setlineNumber (last->lineNumber ());			
			stat->insertStmtBefore (*parallel, *stat->controlParent());
			last->insertStmtAfter (*endparallel, *stat->controlParent());
			if (stat->controlParent () != NULL) {
				PTR_BLOB bl1,bl2,blob=NULL;
				for (bl1 = bl2 = BIF_BLOB1(stat->controlParent()->thebif); (blob == NULL) && bl1; bl1 = BLOB_NEXT (bl1)) {
					if (BLOB_VALUE (bl1) == stat->thebif) {
						BLOB_NEXT (bl2) = BLOB_NEXT (bl1);
						blob=bl1;                            
					}
					bl2 = bl1;
				}
			}                
			if (stat->controlParent () != NULL) {
				PTR_BLOB bl1,bl2,blob=NULL;
				for (bl1 = bl2 = BIF_BLOB1(stat->controlParent()->thebif); (blob == NULL) && bl1; bl1 = BLOB_NEXT (bl1)) {
					if (BLOB_VALUE (bl1) == endparallel->thebif) {
						BLOB_NEXT (bl2) = BLOB_NEXT (bl1);
						blob=bl1;                            
					}
					bl2 = bl1;
				}
			}                
			appendBfndToList1(stat->thebif, parallel->thebif);
			appendBfndToList1(endparallel->thebif, parallel->thebif);
			continue;
        }
        if (stat->variant () == OMP_PARALLEL_WORKSHARE_DIR) {
            stat->setVariant (OMP_PARALLEL_DIR);
            SgExprListExp *list = NULL;
            SgExprListExp *parallel_clause = NULL;
            SgExprListExp *workshare_clause = NULL;            
            if (stat->expr(0) != NULL) {
                list = isSgExprListExp (stat->expr(0));
                for (int i=0; i<list->length (); i++) {
                    SgExpression *exp = list->elem (i);
                    switch (exp->variant ()) {
                        case OMP_SCHEDULE:
                        case OMP_ORDERED:
                        case OMP_LASTPRIVATE: {
                            if (workshare_clause != NULL) {
                                workshare_clause->append (*exp);
                            } else {
                                workshare_clause = new SgExprListExp (*exp);
                            }
                            break;
                        }
                        default: {
                            if (parallel_clause != NULL) {
                                parallel_clause->append (*exp);
                            } else {
                                parallel_clause = new SgExprListExp (*exp);
                            }
                            break;
                        }
                    }
                }
            }
            SgStatement *last = stat->lastNodeOfStmt ();
            if (parallel_clause != NULL) stat->setExpression (0, *parallel_clause);
			else BIF_LL1(stat->thebif)=NULL;
			SgStatement *workshare = new SgStatement (OMP_WORKSHARE_DIR);
            if (workshare_clause != NULL) workshare->setExpression (0, *workshare_clause);
            workshare->setlineNumber (stat->lineNumber ());
            SgStatement *endworkshare = new SgStatement (OMP_END_WORKSHARE_DIR);
            endworkshare->setlineNumber (last->lineNumber ());
            last->setVariant (OMP_END_PARALLEL_DIR);
            stat->insertStmtAfter (*workshare, *stat);
            last->insertStmtBefore (*endworkshare, *stat);
            continue;
        }
		if (omp_debug>=D5) {
			switch (stat->variant()) {
			   case ENTRY_STAT:
				   // !!!!!!!
				   break;
			   case SWITCH_NODE:           // SELECT CASE ...
			   case ARITHIF_NODE:          // Arithmetical IF
			   case IF_NODE:               // IF... THEN
			   case WHILE_NODE:            // DO WHILE (...) 
			   case CASE_NODE:             // CASE ...
			   case ELSEIF_NODE:           // ELSE IF...
			   case LOGIF_NODE:            // Logical IF
				   FunctionCallSearch(stat->expr(0),stat,NULL,1);
				   break; 
			   case COMGOTO_NODE:          // Computed GO TO
			   case OPEN_STAT:
			   case CLOSE_STAT:
			   case INQUIRE_STAT:
			   case BACKSPACE_STAT:
			   case ENDFILE_STAT:
			   case REWIND_STAT:
				   FunctionCallSearch(stat->expr(1),stat,NULL,0);
				   break;
			   case PROC_STAT:  {           // CALL
				   SgExpression *el;
				   // looking through the arguments list
				    for(el=stat->expr(0); el; el=el->rhs())            
					   Arg_FunctionCallSearch(el->lhs(),stat,el,1);   // argument
					}
					break;
			   case ASSIGN_STAT:             // Assign statement
				   FunctionCallSearch(stat->expr(0),stat,NULL,1);   // left part
				   FunctionCallSearch(stat->expr(1),stat,NULL,0);   // right part
				   break;
			   case WRITE_STAT:
			   case READ_STAT:
			   case PRINT_STAT:
			   case FOR_NODE:  
				   FunctionCallSearch(stat->expr(0),stat,NULL,1);   // left part
				   FunctionCallSearch(stat->expr(1),stat,NULL,0);   // right part
				   break;
			}
		}
    }    
    if (omp_debug>=D3){    
        for (SgSymbol *sym=f->firstSymbol(); sym; sym=sym->next ()) {        
            RegisterSymbol (sym);    
        }
    }    
    stat = f->firstStatement(); // file header
    for(stat=stat->lexNext(); stat; stat=stat->lastNodeOfStmt()->lexNext ()) {
    	if(strcmp(stat->symbol()->identifier(),"dbg_init_handles")) { 
        	InstrumentFunctionForOpenMPDebug (stat, func);
	    }		
    }    
    if (symStatMP != NULL) {
        SgArrayType *type = isSgArrayType (symStatMP->type());
        if (type != NULL) {
            if (TYPE_RANGES(type->thetype) != NULL) {
                if (NODE_OPERAND0(TYPE_RANGES(type->thetype)) != NULL) {
                    if (NODE_OPERAND0(TYPE_RANGES(type->thetype))->variant == INT_VAL) {
                        NODE_INT_CST_LOW (NODE_OPERAND0(TYPE_RANGES(type->thetype))) = nArrStaticHandleCount;
                    }
                }
            }
        }        
    }
    if (symDynMP != NULL) {
        SgArrayType *type = isSgArrayType (symDynMP->type());
        if (type != NULL) {
            if (TYPE_RANGES(type->thetype) != NULL) {
                if (NODE_OPERAND0(TYPE_RANGES(type->thetype)) != NULL) {
                    if (NODE_OPERAND0(TYPE_RANGES(type->thetype))->variant == INT_VAL) {
                        NODE_INT_CST_LOW (NODE_OPERAND0(TYPE_RANGES(type->thetype))) = nMaxArrHandleCount;
                    }
                }
            }
        }        
    }
    if (debug != NULL) {        
        DeclareExternalProcedures (GetLastDeclarationStatement(debug));
        UpdateIncludeVarsFile(debug, "dbg_vars.h");
        UpdateIncludeInitFile(debug, "dbg_init.h");
    }
}

void RegisterSymbol(SgSymbol *sym) {
    if (sym->variant ()== VARIABLE_NAME) {
			RegistrateVariable (sym);
    }
}

void DBGSearchVarsInExpression (SgExpression *exp) {
    if (exp == NULL) return;
    if (exp->symbol() != NULL) {
        RegisterSymbol(exp->symbol ());
    }
    DBGSearchVarsInExpression (exp->lhs());
    DBGSearchVarsInExpression (exp->rhs());
}

void DBGSearchVarsInFunction (SgStatement *func) {
    return;
    SgStatement *st;
    for (st=func; st; st=st->lexNext ()) {
        if (st->hasSymbol ()) {
            RegisterSymbol (st->symbol ());
        } else {
            for (int i=0; i<3; i++) {
                DBGSearchVarsInExpression (st->expr(i));
            }
        }
    }
}

void RegistrateVariable (SgSymbol *sym) {
    if (sym->type()->variant () == T_ARRAY) {
			RegisterArray(sym);
    } else {
			RegisterVar(sym);
    }
}

void RegisterVar (SgSymbol *sym) {
    SgStatement *stFirst = NULL;
    SgCallStmt *fe;
	if (!strcmp (sym->identifier(),"dbg_get_addr")) return;
    if (!strcmp (sym->identifier(),"ithreadid")) return;
    if (!strcmp (sym->identifier(),"dbg000")) return; 
    if (!strcmp (sym->identifier(),"mem000")) return; 
    if (!strcmp (sym->identifier(),"heap00")) return; 
    if (!strcmp (sym->identifier(),"dbg_atomic")) return;
    if (sym->scope () != NULL) {
        stFirst = GetFirstExecutableNotDebugStatement(sym->scope ());
    }
    if (stFirst == NULL) return;
    SgStatement *stDeclared = sym->declaredInStmt ();
    if (stDeclared == NULL) stDeclared = stFirst;
    char *strStaticContext = new char [MaxContextBufferLength];
    memset(strStaticContext, 0, MaxContextBufferLength);    
    strcat(strStaticContext,"*type=var_name");
    GenerateFileAndLine (stDeclared, strStaticContext);// To DO ISINDATA ISINCOMMON ISINSAVE
    sprintf (strStaticContext,"%s*name1=%s*vtype=%d*isindata=0*isincommon=%d*isinsave=%d",strStaticContext,sym->identifier(),VarType(sym),((sym->getAttribute(0,COMMON_VAR)==NULL)?0:1),((sym->getAttribute(0,SAVE_VAR)==NULL) ? 0:1));    
    int *pos = new int;
    pos = ((int *)sym->attributeValue(0,FORMAL_PARAM));
    if (pos != NULL) {
        if (sym_dbg_regparvar == NULL) sym_dbg_regparvar = new SgSymbol (PROCEDURE_NAME, "dbg_regparvar");
        fe = new SgCallStmt(*sym_dbg_regparvar);
    } else {
        if (sym_dbg_regvar == NULL) sym_dbg_regvar = new SgSymbol (PROCEDURE_NAME, "dbg_regvar");
        fe = new SgCallStmt(*sym_dbg_regvar);
    }
    SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
    *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*new SgVarRefExp(sym));
    if (pos != NULL) {
        fe->addArg(*new SgValueExp (*pos));
    }
    fe->addAttribute(DEBUG_STAT);
    stFirst->insertStmtBefore(*fe, *stFirst->controlParent());
    sym->addAttribute (STATIC_CONTEXT, (void *)arrStaticRef, sizeof(SgArrayRefExp *));
    GenerateCallGetHandle (strStaticContext);
}

SgExpression *GetLeftBoundFunction(SgSymbol *ar, int i) {
	SgFunctionCallExp *fe;  
	// generating function call: LBOUND(ARRAY, DIM)
	if(!FuncLeftBound)   
		FuncLeftBound = new SgFunctionSymb(FUNCTION_NAME, "lbound", *SgTypeInt(), *ar->scope()); 
	fe = new SgFunctionCallExp(*FuncLeftBound);
	fe -> addArg(*new SgArrayRefExp(*ar));//array
	if(i != 0) fe -> addArg(*new SgValueExp(i));  // dimension number
	return(fe);
}

SgExpression *GetRightBoundFunction(SgSymbol *ar, int i) {
	SgFunctionCallExp *fe;  
	// generating function call: UBOUND(ARRAY, DIM)
	if(!FuncRightBound) FuncRightBound = new SgFunctionSymb(FUNCTION_NAME, "ubound", *SgTypeInt(), *ar->scope()); 
	fe = new SgFunctionCallExp(*FuncRightBound);
	fe -> addArg(*new SgArrayRefExp(*ar));//array
	if(i != 0) fe -> addArg(*new SgValueExp(i));  // dimension number
	return(fe);
}

void RegisterArray (SgSymbol *sym) {    
	SgStatement *stFirst = NULL;
	SgCallStmt *fe = NULL;
	if (IS_ALLOCATABLE_POINTER (sym)) return;
	if (!strcmp (sym->identifier(),"istat_mp")) return;
	if (!strcmp (sym->identifier(),"idyn_mp")) return;
	if (sym->scope () != NULL) {
		stFirst = GetFirstExecutableNotDebugStatement(sym->scope ());
	}
	if (stFirst == NULL) return;
	SgExpression **arrFirstElement = new (SgExpression *);
	*arrFirstElement = FirstArrayElement(sym);
	SgArrayType *arType= isSgArrayType(sym->type());
	SgExpression *arrLowerSize = NULL;
	SgExpression *arrUpperSize = NULL;
	SgStatement *stDeclared = sym->declaredInStmt ();
	if (stDeclared == NULL) stDeclared = stFirst;
	char *strStaticContext = new char [MaxContextBufferLength];
	memset(strStaticContext, 0, MaxContextBufferLength);    
	strcat(strStaticContext,"*type=arr_name");
	GenerateFileAndLine (stDeclared, strStaticContext);// To DO ISINDATA ISINCOMMON ISINSAVE
	sprintf (strStaticContext,"%s*name1=%s*vtype=%d*rank=%d*isindata=0*isincommon=%d*isinsave=%d",strStaticContext,sym->identifier(),VarType(sym),arType->dimension(),((sym->getAttribute(0,COMMON_VAR)==NULL) ? 0:1),((sym->getAttribute(0,SAVE_VAR)==NULL) ? 0:1));
	nArrHandleCount=1;
	if (arType != NULL) {
		for (int i=0; i<arType->dimension(); i++) {
			SgExpression *exp = arType->sizeInDim(i);
			SgSubscriptExp *sbe = isSgSubscriptExp(exp);
			if (sbe != NULL) {
				if ((sbe->ubound() == NULL)||(sbe->ubound()->variant() == STAR_RANGE)) {
					sprintf (strStaticContext,"%s*isassumed=1",strStaticContext);
					if (sbe->lbound() != NULL) {
						arrUpperSize = sbe->lbound();
						arrLowerSize = sbe->lbound();
					} else {
						Error("Assumed-size array: %s",sym->identifier(), 162, stFirst);
					}
				} else {
					if(sbe->lbound() != NULL) {
						arrLowerSize = sbe->lbound();
					} else {
						arrLowerSize = C1;
					}
					if(sbe->ubound() != NULL) {
						arrUpperSize = sbe->ubound();
					}
				}		
			} else {
				if(exp->variant() != STAR_RANGE) {// dim=ubound = *
					arrLowerSize = C1;
					arrUpperSize = exp;
				} else {
					sprintf (strStaticContext,"%s*isassumed=1",strStaticContext);
					arrUpperSize = C1;
					arrLowerSize = C1;                        
				}
			}
			doOmpAssignStmt(arrLowerSize, stFirst);
			doOmpAssignStmt(arrUpperSize, stFirst);
		}
		int *pos = new int;
		pos = ((int *)sym->attributeValue(0,FORMAL_PARAM));
		if (pos != NULL) {
			if (sym_dbg_regpararr == NULL) sym_dbg_regpararr = new SgSymbol (PROCEDURE_NAME, "dbg_regpararr");
			fe = new SgCallStmt(*sym_dbg_regpararr);
		} else {
			if (sym_dbg_regarr == NULL) sym_dbg_regarr = new SgSymbol (PROCEDURE_NAME, "dbg_regarr");
			fe = new SgCallStmt(*sym_dbg_regarr);
		}
		SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
		*arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
		SgArrayRefExp *arrDynamicRef = new SgArrayRefExp(*symDynMP,*C1);
		fe->addArg(**arrStaticRef);
		fe->addArg(*varThreadID);
		fe->addArg(*arrDynamicRef);
		fe->addArg(**arrFirstElement);
		if (pos != NULL) {
			fe->addArg(*new SgValueExp (*pos));
		}
		fe->addAttribute(DEBUG_STAT);
		stFirst->insertStmtBefore(*fe, *stFirst->controlParent());
		sym->addAttribute (STATIC_CONTEXT, (void *)arrStaticRef, sizeof(SgArrayRefExp *));
		sym->addAttribute (FIRST_ELEM, (void *)arrFirstElement, sizeof(SgExpression *));
		GenerateCallGetHandle (strStaticContext);
	}    
}

void RegisterAllocatableArrays (SgStatement *stat) {    
	SgCallStmt *fe = NULL;
	SgExprListExp *list = isSgExprListExp(stat->expr(0));
	SgStatement *next=stat->lexNext();
	for (int i=0; i<list->length (); i++) {
		if (list->elem(i)->variant()==ARRAY_REF) {
			SgSymbol *sym = list->elem(i)->symbol();
			SgExprListExp *arrlist = isSgExprListExp(list->elem(i)->lhs ());
			SgArrayRefExp *leftbound = new SgArrayRefExp (*sym);
			SgArrayRefExp *rightbound = new SgArrayRefExp (*sym);
			nArrHandleCount=1;
			if (arrlist) {
				for (int j=0;j<arrlist->length();j++) {
					if (arrlist->elem(j)->variant()==DDOT) {
						leftbound->addSubscript(*arrlist->elem(j)->lhs());
						rightbound->addSubscript(*arrlist->elem(j)->rhs());
						doOmpAssignStmt(arrlist->elem(j)->lhs(), next);
						doOmpAssignStmt(arrlist->elem(j)->rhs(), next);
					} else {
						leftbound->addSubscript(*C1);
						rightbound->addSubscript(*arrlist->elem(j));
						doOmpAssignStmt(C1, next);
						doOmpAssignStmt(arrlist->elem(j), next);
					}
				}
			}
			SgExpression **arrFirstElement = new (SgExpression *);
			*arrFirstElement = leftbound;
			SgArrayType *arType= isSgArrayType(sym->type());
			//SgStatement *stDeclared = sym->declaredInStmt ();
			//if (stDeclared == NULL) stDeclared = stat;
			char *strStaticContext = new char [MaxContextBufferLength];
			memset(strStaticContext, 0, MaxContextBufferLength);    
			strcat(strStaticContext,"*type=arr_name");
			GenerateFileAndLine (stat, strStaticContext);// To DO ISINDATA ISINCOMMON ISINSAVE
			sprintf (strStaticContext,"%s*name1=%s*vtype=%d*rank=%d*isindata=0*isincommon=%d*isinsave=%d",strStaticContext,sym->identifier(),VarType(sym),arType->dimension(),((sym->getAttribute(0,COMMON_VAR)==NULL) ? 0:1),((sym->getAttribute(0,SAVE_VAR)==NULL) ? 0:1));
			if (sym_dbg_regarr == NULL) sym_dbg_regarr = new SgSymbol (PROCEDURE_NAME, "dbg_regarr");
			fe = new SgCallStmt(*sym_dbg_regarr);
			SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
			*arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
			SgArrayRefExp *arrDynamicRef = new SgArrayRefExp(*symDynMP,*C1);
			fe->addArg(**arrStaticRef);
			fe->addArg(*varThreadID);
			fe->addArg(*arrDynamicRef);
			fe->addArg(**arrFirstElement);
			fe->addAttribute(DEBUG_STAT);
			next->insertStmtBefore(*fe, *next->controlParent());
			for (int j=0; j<sym->numberOfAttributes();j++) {
				if ((sym->attributeType(j)==STATIC_CONTEXT) ||
					(sym->attributeType(j)==FIRST_ELEM))
					sym->deleteAttribute(j);
			}
			sym->addAttribute (STATIC_CONTEXT, (void *)arrStaticRef, sizeof(SgArrayRefExp *));
			sym->addAttribute (FIRST_ELEM, (void *)arrFirstElement, sizeof(SgExpression *));
			GenerateCallGetHandle (strStaticContext);
		}
	}    
}

void UnregisterAllocatableArrays (SgStatement *stat) {    
	SgCallStmt *fe = NULL;
	SgExprListExp *list = isSgExprListExp(stat->expr(0));
	for (int i=0; i<list->length (); i++) {
		if (list->elem(i)->variant()==ARRAY_REF) {
			SgSymbol *sym = list->elem(i)->symbol();
			SgExpression **arrFirstElement = NULL;
			arrFirstElement = new (SgExpression *);
			arrFirstElement = (SgExpression **) sym->attributeValue(0,FIRST_ELEM);
			SgArrayType *arType= isSgArrayType(sym->type());
			char *strStaticContext = new char [MaxContextBufferLength];
			memset(strStaticContext, 0, MaxContextBufferLength);    
			strcat(strStaticContext,"*type=arr_name");
			GenerateFileAndLine (stat, strStaticContext);// To DO ISINDATA ISINCOMMON ISINSAVE
			sprintf (strStaticContext,"%s*name1=%s*vtype=%d*rank=%d*isindata=0*isincommon=%d*isinsave=%d",strStaticContext,sym->identifier(),VarType(sym),arType->dimension(),((sym->getAttribute(0,COMMON_VAR)==NULL) ? 0:1),((sym->getAttribute(0,SAVE_VAR)==NULL) ? 0:1));
			if (sym_dbg_unregarr == NULL) sym_dbg_unregarr = new SgSymbol (PROCEDURE_NAME, "dbg_unregarr");
			fe = new SgCallStmt(*sym_dbg_unregarr);
			SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
			StatContext = (SgArrayRefExp **)sym->attributeValue(0,STATIC_CONTEXT);
			if (StatContext != NULL) {
				fe->addArg(**StatContext);
			}
			fe->addArg(*varThreadID);
			if (arrFirstElement != NULL) fe->addArg(**arrFirstElement);
			fe->addAttribute(DEBUG_STAT);
			stat->insertStmtBefore(*fe, *stat->controlParent());
			for (int j=0; j<sym->numberOfAttributes();j++) {
				if ((sym->attributeType(j)==STATIC_CONTEXT) ||
					(sym->attributeType(j)==FIRST_ELEM))
					sym->deleteAttribute(j);
			}
			GenerateCallGetHandle (strStaticContext);
		}
	}    
}

void InstrumentOmpParallelDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    SgCallStmt *fperf = NULL;
    if (sym_dbg_before_parallel == NULL) sym_dbg_before_parallel = new SgSymbol (PROCEDURE_NAME, "dbg_before_parallel");
    if (sym_dbg_after_parallel == NULL) sym_dbg_after_parallel = new SgSymbol (PROCEDURE_NAME, "dbg_after_parallel");
    if (sym_dbg_parallel_event == NULL) sym_dbg_parallel_event = new SgSymbol (PROCEDURE_NAME, "dbg_parallel_event");
    if (omp_debug == DPERF) {    
        if (sym_dbg_interval_begin == NULL) sym_dbg_interval_begin = new SgSymbol (PROCEDURE_NAME, "dbg_interval_begin");
        if (sym_dbg_interval_end == NULL) sym_dbg_interval_end = new SgSymbol (PROCEDURE_NAME, "dbg_interval_end");
        if (sym_dbg_parallel_event_end == NULL) sym_dbg_parallel_event_end = new SgSymbol (PROCEDURE_NAME, "dbg_parallel_event_end");
    }
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_parallel);
    SgExprListExp *exp = isSgExprListExp (st->expr(0));
    nArrHandleCount = 1;
    int nNumThreads = 0;
    int nIfExpr = 0;
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
            if (ex->variant () == OMP_NUM_THREADS){
                nNumThreads = nArrHandleCount;
                doOmpAssignStmt (ex->lhs(),st);
                continue;
            }
            if (ex->variant () == OMP_IF) {
                nIfExpr = nArrHandleCount;
                doOmpAssignStmt (ex->lhs(),st);
            }
        }
        SgExpression *expStatMPPrivate = new SgExpression (OMP_SHARED);
        expStatMPPrivate->setLhs (*new SgExprListExp (*new SgVarRefExp(symStatMP)));
        exp->append (*expStatMPPrivate);
    }
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
	if (omp_debug == DPERF) {
		fperf = new SgCallStmt(*sym_dbg_interval_begin);
		fperf->addArg(*arrStaticRef);
		fperf->addArg(*varThreadID);
		fperf->addArg(*new SgValueExp (nArrStaticHandleCount));
		fperf->addAttribute(DEBUG_STAT);
	}
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    if (nNumThreads == 0) {
        fe->addArg(*M1);
    } else {
        fe->addArg(*new SgArrayRefExp(*symDynMP,((nNumThreads==1)? *C1:*C2 )));
    }
    if (nIfExpr == 0) {
        fe->addArg(*M1);
    } else {
        fe->addArg(*new SgArrayRefExp(*symDynMP,((nIfExpr==1)? *C1:*C2 )));
    }
    fe->addAttribute(DEBUG_STAT);
    if (fperf != NULL) stat->insertStmtBefore(*fperf, *stat->controlParent());
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_parallel_event);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    stat=st->lastNodeOfStmt ();
    if (omp_debug==DPERF) {
        fe = new SgCallStmt(*sym_dbg_parallel_event_end);
        fe->addArg(*arrStaticRef);
        fe->addArg(*varThreadID);
        fe->addAttribute(DEBUG_STAT);
        stat->insertStmtBefore(*fe, *stat->controlParent());
        fperf = new SgCallStmt(*sym_dbg_interval_end);
        fperf->addArg(*arrStaticRef);
        fperf->addArg(*varThreadID);
        fperf->addArg(*new SgValueExp (nArrStaticHandleCount));
        fperf->addAttribute(DEBUG_STAT);
    }
    fe = new SgCallStmt(*sym_dbg_after_parallel);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    if (fperf != NULL) stat->insertStmtBefore(*fperf, *stat->controlParent());
}

void InstrumentOmpDoDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    SgForStmt *ForStat = isSgForStmt (st->lexNext ());
    if (ForStat == NULL) {
        (void)fprintf (stderr, "Error: Incorrect OpenMP loop in %s line %d\n", st->fileName(), st->lineNumber ());
        exit (-1);
    }
    if (ForStat->hasLabel ()) {
        SgStatement *tmp = new SgStatement (CONT_STAT);
        tmp->setLabel (*ForStat->label ());
        st->insertStmtBefore(*tmp, *st->controlParent());
        BIF_LABEL(ForStat->thebif)=NULL;
    }
    if (sym_dbg_before_omp_loop == NULL) sym_dbg_before_omp_loop = new SgSymbol (PROCEDURE_NAME, "dbg_before_omp_loop");
    if (sym_dbg_after_omp_loop == NULL) sym_dbg_after_omp_loop = new SgSymbol (PROCEDURE_NAME, "dbg_after_omp_loop");
    if (sym_dbg_omp_loop_event == NULL) sym_dbg_omp_loop_event = new SgSymbol (PROCEDURE_NAME, "dbg_omp_loop_event");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_omp_loop);
    SgExprListExp *exp = isSgExprListExp (st->expr(0));
    nArrHandleCount = 1;
    int nChunk = 0;
    doOmpAssignStmt(ForStat->start(),st);
    doOmpAssignStmt(ForStat->end(),st);
    if (ForStat->step() != NULL) {
        doOmpAssignStmt(ForStat->step(),st);
    } else {
        doOmpAssignStmt(C1,st);
    }
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
            if (ex->variant () == OMP_SCHEDULE) {
                if (ex->rhs () != NULL) {
                    doOmpAssignStmt (ex->rhs(),st);
                    nChunk = 1;
                }
            }
        }
    }
    SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
    *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp ((omp_debug != DPERF) ? nArrStaticHandleCount : (nArrStaticHandleCount+1)));
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*new SgArrayRefExp(*symDynMP,*C1));
    fe->addArg(*new SgArrayRefExp(*symDynMP,*C2));
    fe->addArg(*new SgArrayRefExp(*symDynMP,*C3));
    if (nChunk == 0) {
        fe->addArg(*M1);
    } else {
        fe->addArg(*new SgArrayRefExp(*symDynMP,*C4));
    }
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_omp_loop_event);
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*new SgVarRefExp (*ForStat->symbol ()));
    SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
    StatContext = (SgArrayRefExp **)ForStat->symbol ()->attributeValue(0,STATIC_CONTEXT);
    if (StatContext != NULL) {
        fe->addArg(**StatContext);
    }
    stat=ForStat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    if (omp_debug!=DPERF){
        stat->insertStmtBefore(*fe, *stat->controlParent());
    }
    fe = new SgCallStmt(*sym_dbg_after_omp_loop);
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    stat=GetLastStatementOfLoop (ForStat);
    stat = stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    if (stat->variant () == OMP_END_DO_DIR) {
        stat->lexNext ()->insertStmtBefore(*fe, *stat->controlParent());
        exp = isSgExprListExp (stat->expr(0));
        if (exp != NULL) {
            for (int i=0; i<exp->length (); i++) {
                GenerateContextStringForClauses (exp->elem (i), strStaticContext);
            }
        }
        if (omp_debug == DPERF) {        
            GenerateNowaitPlusBarrier (stat);
        }        
    } else {
        stat->insertStmtBefore(*fe, *stat->controlParent());
        if (omp_debug == DPERF) {
            SgStatement *enddodir = new SgStatement (OMP_END_DO_DIR);
            enddodir->setlineNumber (stat->lineNumber());
            enddodir->addAttribute(DEBUG_STAT);
            fe->insertStmtBefore(*enddodir,*stat->controlParent());
            GenerateNowaitPlusBarrier (enddodir);            
        }
    }
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
    ForStat->addAttribute (STATIC_CONTEXT, (void *)arrStaticRef, sizeof(SgArrayRefExp *));
}

void InstrumentSerialDoLoop (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    SgForStmt *ForStat = isSgForStmt(st);
    if (ForStat->hasLabel ()) {
        SgStatement *tmp = new SgStatement (CONT_STAT);
        tmp->setLabel (*ForStat->label ());
        st->insertStmtBefore(*tmp, *st->controlParent());
        BIF_LABEL(ForStat->thebif)=NULL;
    }
    if (sym_dbg_before_loop == NULL) sym_dbg_before_loop = new SgSymbol (PROCEDURE_NAME, "dbg_before_loop");
    if (sym_dbg_after_loop == NULL) sym_dbg_after_loop = new SgSymbol (PROCEDURE_NAME, "dbg_after_loop");
    if (sym_dbg_loop_event == NULL) sym_dbg_loop_event = new SgSymbol (PROCEDURE_NAME, "dbg_loop_event");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_loop);
    isSgExprListExp (st->expr(0));
    nArrHandleCount = 1;
    doOmpAssignStmt(ForStat->start(),st);
    doOmpAssignStmt(ForStat->end(),st);
    if (ForStat->step() != NULL) {
        doOmpAssignStmt(ForStat->step(),st);
    } else {
        doOmpAssignStmt(C1,st);
    }    
    SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
    *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*new SgArrayRefExp(*symDynMP,*C1));
    fe->addArg(*new SgArrayRefExp(*symDynMP,*C2));
    fe->addArg(*new SgArrayRefExp(*symDynMP,*C3));
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_loop_event);
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*new SgVarRefExp (*ForStat->symbol ()));
    SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
    StatContext = (SgArrayRefExp **)ForStat->symbol ()->attributeValue(0,STATIC_CONTEXT);
    if (StatContext != NULL) {
        fe->addArg(**StatContext);
    }
    stat=ForStat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_after_loop);
    fe->addArg(**arrStaticRef);
    fe->addArg(*varThreadID);
    stat=GetLastStatementOfLoop (ForStat);
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
    stat = stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    ForStat->addAttribute (STATIC_CONTEXT, (void *)arrStaticRef, sizeof(SgArrayRefExp *));
}

void InstrumentOmpSingleDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_before_single == NULL) sym_dbg_before_single = new SgSymbol (PROCEDURE_NAME, "dbg_before_single");
    if (sym_dbg_after_single == NULL) sym_dbg_after_single = new SgSymbol (PROCEDURE_NAME, "dbg_after_single");
    if (sym_dbg_single_event == NULL) sym_dbg_single_event = new SgSymbol (PROCEDURE_NAME, "dbg_single_event");
    if (omp_debug == DPERF) {
        if (sym_dbg_single_event_end == NULL) sym_dbg_single_event_end = new SgSymbol (PROCEDURE_NAME, "dbg_single_event_end");
    }
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_single);
    SgExprListExp *exp = isSgExprListExp (st->expr(0));
    nArrHandleCount = 1;
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
        }
    }
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp ((omp_debug != DPERF) ? nArrStaticHandleCount : (nArrStaticHandleCount+1)));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_single_event);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());        
    stat=st->lastNodeOfStmt ();        
    if (omp_debug == DPERF) {            
        fe = new SgCallStmt(*sym_dbg_single_event_end);
        fe->addArg(*arrStaticRef);    
        fe->addArg(*varThreadID);
        stat->insertStmtBefore(*fe, *stat->controlParent());
        fe->addAttribute(DEBUG_STAT);
    }
    fe = new SgCallStmt(*sym_dbg_after_single);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    exp = isSgExprListExp (stat->expr(0));    
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
        }
    }
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());        
    stat=stat->lexNext ();
    if (omp_debug == DPERF) {        
        GenerateNowaitPlusBarrier (stat->lexPrev());
    }           
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
}

SgStatement *GetLastStatementOfLoop (SgStatement *forst) {
    SgStatement *st, *res=NULL;
    int lbl=-1;
    if (forst->thebif->entry.for_node.doend !=NULL)
        lbl=forst->thebif->entry.for_node.doend->stateno;
    if (forst != NULL){
        res = forst->lastNodeOfStmt ();
    }    
    if (res->variant () == CONTROL_END) {        
        return res;
    }
    for (st=res;st; st=st->lexNext()) {
        if (st->variant() == CONT_STAT) {
            if (lbl != 0) {                
                if (st->hasLabel()) {
                    if (st->label()->thelabel->stateno == lbl) {
                        return st;
                    }
                }
            }
        }
        if (st->variant() == CONTROL_END) {
            if (st->controlParent() == forst) {                
                return st;
            }
        }
    } 
    return res;
}

void InstrumentOmpCriticalDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_before_critical == NULL) sym_dbg_before_critical = new SgSymbol (PROCEDURE_NAME, "dbg_before_critical");
    if (sym_dbg_after_critical == NULL) sym_dbg_after_critical = new SgSymbol (PROCEDURE_NAME, "dbg_after_critical");
    if (sym_dbg_critical_event == NULL) sym_dbg_critical_event = new SgSymbol (PROCEDURE_NAME, "dbg_critical_event");
    if (omp_debug == DPERF) {
        if (sym_dbg_critical_event_end == NULL) sym_dbg_critical_event_end = new SgSymbol (PROCEDURE_NAME, "dbg_critical_event_end");
    }
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_critical);    
    nArrHandleCount = 1;
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_critical_event);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    stat=st->lastNodeOfStmt ();
    if (omp_debug==DPERF) {
        fe = new SgCallStmt(*sym_dbg_critical_event_end);    
        fe->addArg(*arrStaticRef);    
        fe->addArg(*varThreadID);
        fe->addAttribute(DEBUG_STAT);
        stat->insertStmtBefore(*fe, *stat->controlParent());
    }
    fe = new SgCallStmt(*sym_dbg_after_critical);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    if (st->expr(0)!= NULL) {
        sprintf(strStaticContext,"%s*name1=%s*line2=%d",strStaticContext,UnparseExpr (st->expr(0)),stat->lineNumber());
    } else {
        sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
    }
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
}

void InstrumentOmpOrderelDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_before_ordered == NULL) sym_dbg_before_ordered = new SgSymbol (PROCEDURE_NAME, "dbg_before_ordered");
    if (sym_dbg_after_ordered == NULL) sym_dbg_after_ordered = new SgSymbol (PROCEDURE_NAME, "dbg_after_ordered");
    if (sym_dbg_ordered_event == NULL) sym_dbg_ordered_event = new SgSymbol (PROCEDURE_NAME, "dbg_ordered_event");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_ordered);    
    nArrHandleCount = 1;
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_ordered_event);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    fe = new SgCallStmt(*sym_dbg_after_ordered);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=st->lastNodeOfStmt ();
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
}

void InstrumentOmpMasterDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st->lexNext ();
    if (sym_dbg_master_begin == NULL) sym_dbg_master_begin = new SgSymbol (PROCEDURE_NAME, "dbg_master_begin");
    if (sym_dbg_master_end == NULL) sym_dbg_master_end = new SgSymbol (PROCEDURE_NAME, "dbg_master_end");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_master_begin);    
    nArrHandleCount = 1;
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *st);
    fe = new SgCallStmt(*sym_dbg_master_end);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);    
    stat=st->lastNodeOfStmt ();
    stat->insertStmtBefore(*fe, *st);
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
}

void InstrumentOmpBarrierDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st->lexNext ();
    if (sym_dbg_before_barrier == NULL) sym_dbg_before_barrier = new SgSymbol (PROCEDURE_NAME, "dbg_before_barrier");
    if (sym_dbg_after_barrier == NULL) sym_dbg_after_barrier = new SgSymbol (PROCEDURE_NAME, "dbg_after_barrier");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_barrier);    
    nArrHandleCount = 1;
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    st->insertStmtBefore(*fe, *st->controlParent());
    fe = new SgCallStmt(*sym_dbg_after_barrier);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *st->controlParent());
}

void InstrumentOmpFlushDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_flush_event == NULL) sym_dbg_flush_event = new SgSymbol (PROCEDURE_NAME, "dbg_flush_event");
    if (omp_debug == DPERF){
        if (sym_dbg_before_flush == NULL) sym_dbg_before_flush = new SgSymbol (PROCEDURE_NAME, "dbg_before_flush");
    }
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    SgCallStmt *fe = NULL;
    if (omp_debug == DPERF){    
        fe = new SgCallStmt(*sym_dbg_before_flush);
        fe->addArg(*arrStaticRef);    
        fe->addArg(*varThreadID);    
        fe->addAttribute(DEBUG_STAT);    
        stat->insertStmtBefore(*fe, *st->controlParent());
    }
    fe = new SgCallStmt(*sym_dbg_flush_event);    
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat = st->lexNext ();
    if (st->expr(0)!= NULL) {
        sprintf(strStaticContext,"%s*name1=%s",strStaticContext,UnparseExpr (st->expr(0)));
    }
    stat->insertStmtBefore(*fe, *st->controlParent());    
}

void InstrumentIOStmt (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_before_io == NULL) sym_dbg_before_io = new SgSymbol (PROCEDURE_NAME, "dbg_before_io");
    if (sym_dbg_after_io == NULL) sym_dbg_after_io = new SgSymbol (PROCEDURE_NAME, "dbg_after_io");
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    SgCallStmt *fe = NULL;
    fe = new SgCallStmt(*sym_dbg_before_io);
    fe->addArg(*arrStaticRef);    
    fe->addArg(*varThreadID);    
    fe->addAttribute(DEBUG_STAT);    
    stat->insertStmtBefore(*fe, *st->controlParent());
    fe = new SgCallStmt(*sym_dbg_after_io);    
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat = st->lexNext ();
    stat->insertStmtBefore(*fe, *st->controlParent());    
}

void InstrumentIntervalDir (SgStatement *bst, SgStatement *st, char *strStaticContext){
    SgStatement *stat = bst;
    if (sym_dbg_interval_begin == NULL) sym_dbg_interval_begin = new SgSymbol (PROCEDURE_NAME, "dbg_interval_begin");
    if (sym_dbg_interval_end == NULL) sym_dbg_interval_end = new SgSymbol (PROCEDURE_NAME, "dbg_interval_end");
	SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    SgCallStmt *fe = NULL;
    fe = new SgCallStmt(*sym_dbg_interval_begin);
    fe->addArg(*arrStaticRef);    
    fe->addArg(*varThreadID);    
    fe->addArg(*new SgValueExp (INTERVAL_NUMBER));
    fe->addAttribute(DEBUG_STAT);    
    stat->insertStmtBefore(*fe, *bst->controlParent());
    stat = st;
	sprintf(strStaticContext,"%s*line2=%d",strStaticContext,st->lineNumber());
    fe = new SgCallStmt(*sym_dbg_interval_end);
    fe->addArg(*arrStaticRef);    
    fe->addArg(*varThreadID);    
    fe->addArg(*new SgValueExp (INTERVAL_NUMBER));
    fe->addAttribute(DEBUG_STAT);    
    stat->insertStmtBefore(*fe, *st->controlParent());
}

void InstrumentOmpThreadPrivateDir (SgStatement *st, SgStatement *before, char *strStaticContext) {    
    if (sym_dbg_threadprivate == NULL) sym_dbg_threadprivate = new SgSymbol (PROCEDURE_NAME, "dbg_threadprivate");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_threadprivate);    
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    if (st->expr(0)!= NULL) {
        sprintf(strStaticContext,"%s*name1=%s",strStaticContext,UnparseExpr (st->expr(0)));
    }
    before->insertStmtBefore(*fe, *before->controlParent());    
}

void InstrumentOmpSectionsDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_before_sections == NULL) sym_dbg_before_sections = new SgSymbol (PROCEDURE_NAME, "dbg_before_sections");
    if (sym_dbg_after_sections == NULL) sym_dbg_after_sections = new SgSymbol (PROCEDURE_NAME, "dbg_after_sections");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_sections);
    SgExprListExp *exp = isSgExprListExp (st->expr(0));
    nArrHandleCount = 1;
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
        }
    }
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp ((omp_debug != DPERF) ? nArrStaticHandleCount : (nArrStaticHandleCount+1)));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());    
    fe = new SgCallStmt(*sym_dbg_after_sections);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=st->lastNodeOfStmt ();
    /*exp = isSgExprListExp (stat->expr(0));    
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
        }
    }
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());*/
    stat=stat->lexNext ();
    if (omp_debug == DPERF) {        
        GenerateNowaitPlusBarrier (stat->lexPrev());
    }           
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
}

void InstrumentOmpSectionDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st->lexNext ();
    if (sym_dbg_section_event == NULL) sym_dbg_section_event = new SgSymbol (PROCEDURE_NAME, "dbg_section_event");
    if (omp_debug == DPERF) {    
        if (sym_dbg_section_event_end == NULL) sym_dbg_section_event_end = new SgSymbol (PROCEDURE_NAME, "dbg_section_event_end");
    }
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_section_event);
    nArrHandleCount = 1;
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());    
    stat=st->lastNodeOfStmt ();        
    if (omp_debug == DPERF) {            
        fe = new SgCallStmt(*sym_dbg_section_event_end);
        fe->addArg(*arrStaticRef); 
        fe->addArg(*varThreadID);    
        fe->addAttribute(DEBUG_STAT);    
        stat->insertStmtBefore(*fe, *stat->controlParent());        
    }
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());    
}
void InstrumentExitStmt (SgStatement *stat) {
    if (sym_dbg_finalize == NULL) sym_dbg_finalize = new SgSymbol(PROCEDURE_NAME, "dbg_finalize");
    SgCallStmt *finalize = new SgCallStmt(*sym_dbg_finalize);
    finalize->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore (*finalize, *stat->controlParent());
}

void InstrumentOmpWorkshareDir (SgStatement *st, char *strStaticContext){
    SgStatement *stat = st;
    if (sym_dbg_before_workshare == NULL) sym_dbg_before_workshare = new SgSymbol (PROCEDURE_NAME, "dbg_before_workshare");
    if (sym_dbg_after_workshare == NULL) sym_dbg_after_workshare = new SgSymbol (PROCEDURE_NAME, "dbg_after_workshare");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_workshare);    
    nArrHandleCount = 1;
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp ((omp_debug != DPERF) ? nArrStaticHandleCount : (nArrStaticHandleCount+1)));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());    
    fe = new SgCallStmt(*sym_dbg_after_workshare);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=st->lastNodeOfStmt ();
    SgExprListExp *exp = isSgExprListExp (stat->expr(0));    
    if (exp != NULL) {
        for (int i=0; i<exp->length (); i++) {
            SgExpression *ex= exp->elem (i);
            GenerateContextStringForClauses (ex, strStaticContext);
        }
    }
    sprintf(strStaticContext,"%s*line2=%d",strStaticContext,stat->lineNumber());
    stat=stat->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    if (omp_debug == DPERF) {        
        GenerateNowaitPlusBarrier (stat->lexPrev());
    }           
    stat->insertStmtBefore(*fe, *stat->controlParent());
}

void SearchVarAndArrayInExpression(SgStatement *st, SgExpression *exp, SgArrayRefExp *var) {
	if (exp == NULL) return;
	switch (exp->variant()) {
	case INT_VAL:
	case LABEL_REF:
	case FLOAT_VAL:
	case DOUBLE_VAL:
	case STMT_STR:
	case STRING_VAL:
	case COMPLEX_VAL:
	case KEYWORD_VAL:
	case KEYWORD_ARG:
	case BOOL_VAL:
	case CHAR_VAL:
	case CONST_REF:
	case ENUM_REF:
	case TYPE_REF:
	case INTERFACE_REF:
    case DEFAULT:
	case DEF_CHOICE	:
	case SEQ:
	case SPEC_PAIR:
	case ACCESS:
	case IOACCESS:
    case OVERLOADED_CALL:
    case ORDERED_OP:
	case EXTEND_OP:
	case PARAMETER_OP:
	case PUBLIC_OP:
	case PRIVATE_OP:
	case ALLOCATABLE_OP:
	case EXTERNAL_OP:
	case OPTIONAL_OP:
	case IN_OP:
	case OUT_OP:
	case INOUT_OP:
	case INTRINSIC_OP:
	case POINTER_OP:
	case SAVE_OP:
	case TARGET_OP:
    case STAR_RANGE:
	case VARIABLE_NAME:
		break;
	case VAR_REF:
        InstrumentReadVar (st, exp, var);
		break;
	case ARRAY_REF:
        if (exp->symbol ()->type()->variant () == T_ARRAY) {
            InstrumentReadArray (st, exp, var); 
        } else {
            InstrumentReadVar (st, exp, var); /*Äëÿ character**/
        }
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case ARRAY_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	case RECORD_REF:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	case STRUCTURE_CONSTRUCTOR:
	case CONSTRUCTOR_REF:
	case ACCESS_REF:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case CONS:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	case PROC_CALL:
	case FUNC_CALL:
		InstrumentFuncCall(st,exp);
		//SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case EXPR_LIST:
	case EQUI_LIST:
	case COMM_LIST:
	case NAMELIST_LIST:
	case VAR_LIST:
	case RANGE_LIST:
	case CONTROL_LIST:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	case DDOT:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	case EQ_OP:
	case LT_OP:
	case GT_OP:
	case NOTEQL_OP:
	case LTEQL_OP:
	case GTEQL_OP:
	case ADD_OP:
	case SUBT_OP:
	case OR_OP:
	case MULT_OP:
	case DIV_OP:
	case MOD_OP:
	case AND_OP:
	case EXP_OP:
	case EQV_OP:
	case NEQV_OP:
	case XOR_OP:
	case CONCAT_OP: {
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	}
	case MINUS_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case UNARY_ADD_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case NOT_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case PAREN_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case ASSGN_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case IMPL_TYPE:
		if (exp->lhs () != NULL)
		{
		     SearchVarAndArrayInExpression(st,exp->lhs (),var);
		}
		break;
	case MAXPARALLEL_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case DIMENSION_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case LEN_OP:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case TYPE_OP:
		break;
	case ONLY_NODE:
		if (exp->lhs ()) SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case DEREF_OP:
	        SearchVarAndArrayInExpression(st,exp->lhs (),var);
		break;
	case RENAME_NODE:
		SearchVarAndArrayInExpression(st,exp->lhs (),var);
		SearchVarAndArrayInExpression(st,exp->rhs (),var);
		break;
	default:
		fprintf(stderr,"SearchVarAndArrayInExpression -- bad llnd ptr %d!\n",exp->variant());
		break;
	}
}

void InstrumentAssignStat (SgStatement *st, char *strStaticContext) {
    SgExpression *exp = st->expr (0);
    SgStatement *stat=st;
    if ((exp->variant () != ARRAY_REF)&&(exp->variant () != VAR_REF)) return;
    SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
    StatContext = (SgArrayRefExp **)exp->symbol ()->attributeValue(0,STATIC_CONTEXT);
    if (StatContext == NULL) return;
    if (sym_dbg_write_var_begin == NULL) sym_dbg_write_var_begin = new SgSymbol (PROCEDURE_NAME, "dbg_write_var_begin");
    if (sym_dbg_write_arr_begin == NULL) sym_dbg_write_arr_begin = new SgSymbol (PROCEDURE_NAME, "dbg_write_arr_begin");
    if (sym_dbg_write_arr_end == NULL) sym_dbg_write_arr_end = new SgSymbol (PROCEDURE_NAME, "dbg_write_arr_end");
    if (sym_dbg_write_var_end == NULL) sym_dbg_write_var_end = new SgSymbol (PROCEDURE_NAME, "dbg_write_var_end");
    if (sym_dbg_read_arr == NULL) sym_dbg_read_arr = new SgSymbol (PROCEDURE_NAME, "dbg_read_arr");
    if (sym_dbg_read_var == NULL) sym_dbg_read_var = new SgSymbol (PROCEDURE_NAME, "dbg_read_var");
    int isArray = (exp->variant () == ARRAY_REF) ? (exp->symbol ()->type()->variant () == T_ARRAY) : 0;    
    SgCallStmt *fe = new SgCallStmt((isArray ? *sym_dbg_write_arr_begin : *sym_dbg_write_var_begin));
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*exp);
    fe->addArg(**StatContext);
    SgExpression **arrFirstElement = NULL;
    if (isArray) {
        arrFirstElement = new (SgExpression *);
        arrFirstElement = (SgExpression **)exp->symbol ()->attributeValue(0,FIRST_ELEM);
        if (arrFirstElement != NULL) fe->addArg(**arrFirstElement);
    }
    fe->addAttribute(DEBUG_STAT);
    st->insertStmtBefore(*fe, *st->controlParent());
    fe = new SgCallStmt((isArray ? *sym_dbg_write_arr_end : *sym_dbg_write_var_end));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addArg(*exp);
    fe->addArg(**StatContext);
    if (isArray) {
        if (arrFirstElement != NULL) fe->addArg(**arrFirstElement);
    }
    stat=st->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
	GenerateCallGetHandle (strStaticContext);
	if (st->expr(0)->lhs ()) {
        SearchVarAndArrayInExpression (st, st->expr(0)->lhs(),arrStaticRef);
    }
    if (st->expr(1)) {
        SearchVarAndArrayInExpression (st, st->expr(1),arrStaticRef);
    }
}

void InstrumentIfStat (SgStatement *st, char *strStaticContext) {
	SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
	if (sym_dbg_read_arr == NULL) sym_dbg_read_arr = new SgSymbol (PROCEDURE_NAME, "dbg_read_arr");
	if (sym_dbg_read_var == NULL) sym_dbg_read_var = new SgSymbol (PROCEDURE_NAME, "dbg_read_var");
	SearchVarAndArrayInExpression (st, st->expr(0),arrStaticRef);
}

void InstrumentProcStat (SgStatement *st, char *strStaticContext) {
    //SgExpression *exp = st->expr (0);
    SgStatement *stat=st;
    SgCallStmt *f = isSgCallStmt (st);
    if (f == NULL) return;
    if (sym_dbg_before_funcall == NULL) sym_dbg_before_funcall = new SgSymbol (PROCEDURE_NAME, "dbg_before_funcall");
    if (sym_dbg_after_funcall == NULL) sym_dbg_after_funcall = new SgSymbol (PROCEDURE_NAME, "dbg_after_funcall");
    if (sym_dbg_funcparvar == NULL) sym_dbg_funcparvar = new SgSymbol (PROCEDURE_NAME, "dbg_funcparvar");
    if (sym_dbg_funcpararr == NULL) sym_dbg_funcpararr = new SgSymbol (PROCEDURE_NAME, "dbg_funcpararr");    
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_funcall);
    sprintf(strStaticContext,"%s*name1=%s*rank=%d",strStaticContext,stat->symbol ()->identifier (),f->numberOfArgs());
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    st->insertStmtBefore(*fe, *st->controlParent());    
    fe = new SgCallStmt(*sym_dbg_after_funcall);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=st->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    stat = fe;	
    for (int i=0; i<f->numberOfArgs(); i++) {
        SgExpression *par = f->arg(i);
        if ((par->variant () != ARRAY_REF)&&(par->variant () != VAR_REF)) continue;
        SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
        StatContext = (SgArrayRefExp **)par->symbol ()->attributeValue(0,STATIC_CONTEXT);
        if (StatContext == NULL) continue;
        int isArray = (par->variant () == ARRAY_REF) ? (par->symbol ()->type()->variant () == T_ARRAY) : 0;
        fe = new SgCallStmt((isArray ? *sym_dbg_funcpararr : *sym_dbg_funcparvar));
        fe->addArg(*arrStaticRef);
        fe->addArg(*varThreadID);
        fe->addArg(*new SgValueExp(i+1));
        fe->addArg(*par);
        fe->addArg(**StatContext);
        if (isArray) {
            SgExpression **arrFirstElement = new (SgExpression *);
            arrFirstElement = (SgExpression **)par->symbol ()->attributeValue(0,FIRST_ELEM);
            if (arrFirstElement != NULL) fe->addArg(**arrFirstElement);
        }
        fe->addArg(*C1);
        fe->addAttribute(DEBUG_STAT);
        st->insertStmtBefore(*fe, *st->controlParent());
        SgStatement *after = fe->copyPtr ();
        after->addAttribute(DEBUG_STAT);
        stat->insertStmtBefore(*after, *stat->controlParent());
    }
}

void InstrumentFuncCall (SgStatement *st, SgExpression *exp) {
    SgStatement *stat=st;
    SgFunctionCallExp *f = isSgFunctionCallExp (exp);
	if (omp_debug<D2) return;
    if (f == NULL) return;
	char *strStaticContext = new char [MaxContextBufferLength];
	memset(strStaticContext, 0, MaxContextBufferLength);
	strcat(strStaticContext,"*type=func_call");                            
	GenerateFileAndLine (stat, strStaticContext);	
	sprintf(strStaticContext,"%s*name1=%s*rank=%d",strStaticContext,f->funName()->identifier (),f->numberOfArgs());
	GenerateCallGetHandle (strStaticContext);
	if (sym_dbg_before_funcall == NULL) sym_dbg_before_funcall = new SgSymbol (PROCEDURE_NAME, "dbg_before_funcall");
    if (sym_dbg_after_funcall == NULL) sym_dbg_after_funcall = new SgSymbol (PROCEDURE_NAME, "dbg_after_funcall");
    if (sym_dbg_funcparvar == NULL) sym_dbg_funcparvar = new SgSymbol (PROCEDURE_NAME, "dbg_funcparvar");
    if (sym_dbg_funcpararr == NULL) sym_dbg_funcpararr = new SgSymbol (PROCEDURE_NAME, "dbg_funcpararr");    
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_before_funcall);
    SgArrayRefExp *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount-1));
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    st->insertStmtBefore(*fe, *st->controlParent());    
    fe = new SgCallStmt(*sym_dbg_after_funcall);
    fe->addArg(*arrStaticRef);
    fe->addArg(*varThreadID);
    stat=st->lexNext ();
    fe->addAttribute(DEBUG_STAT);
    stat->insertStmtBefore(*fe, *stat->controlParent());
    stat = fe;
    for (int i=0; i<f->numberOfArgs(); i++) {
        SgExpression *par = f->arg(i);
        if ((par->variant () != ARRAY_REF)&&(par->variant () != VAR_REF)) continue;
        SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
        StatContext = (SgArrayRefExp **)par->symbol ()->attributeValue(0,STATIC_CONTEXT);
        if (StatContext == NULL) continue;
        int isArray = (par->variant () == ARRAY_REF) ? (par->symbol ()->type()->variant () == T_ARRAY) : 0;
        fe = new SgCallStmt((isArray ? *sym_dbg_funcpararr : *sym_dbg_funcparvar));
        fe->addArg(*arrStaticRef);
        fe->addArg(*varThreadID);
        fe->addArg(*new SgValueExp(i+1));
        fe->addArg(*par);
        fe->addArg(**StatContext);
        if (isArray) {
            SgExpression **arrFirstElement = new (SgExpression *);
            arrFirstElement = (SgExpression **)par->symbol ()->attributeValue(0,FIRST_ELEM);
            if (arrFirstElement != NULL) fe->addArg(**arrFirstElement);
        }
        fe->addArg(*C1);
        fe->addAttribute(DEBUG_STAT);
        st->insertStmtBefore(*fe, *st->controlParent());
        SgStatement *after = fe->copyPtr ();
        after->addAttribute(DEBUG_STAT);
        stat->insertStmtBefore(*after, *stat->controlParent());
    }
}


void InstrumentFunctionBegin (SgStatement *st, char *strStaticContext, SgStatement *func) {
    //SgExpression *exp = st->expr (0);
    SgStatement *stat=st->lexNext ();
    if (sym_dbg_funcbegin == NULL) sym_dbg_funcbegin = new SgSymbol (PROCEDURE_NAME, "dbg_funcbegin");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_funcbegin);
    if ((func->variant () == PROC_HEDR) || (func->variant () == FUNC_HEDR)) {
        SgFunctionSymb *funcsym = isSgFunctionSymb (func->symbol ());        
        if (funcsym == NULL) return;
        if (func->variant () == FUNC_HEDR)
            sprintf(strStaticContext,"%s*file=%s*line1=%d*line2=%d*name1=%s*vtype=%d*rank=%d",strStaticContext,func->fileName (),func->lineNumber(),func->lastNodeOfStmt()->lineNumber(),func->symbol ()->identifier (),VarType(funcsym),funcsym->numberOfParameters());
        else
            sprintf(strStaticContext,"%s*file=%s*line1=%d*line2=%d*name1=%s*rank=%d",strStaticContext,func->fileName (),func->lineNumber(),func->lastNodeOfStmt()->lineNumber(),func->symbol ()->identifier (),funcsym->numberOfParameters());
        SgArrayRefExp **arrStaticRef = new (SgArrayRefExp *);
        *arrStaticRef = new SgArrayRefExp(*symStatMP,*new SgValueExp (nArrStaticHandleCount));
        func->symbol()->addAttribute (STATIC_CONTEXT, (void *)arrStaticRef, sizeof(SgArrayRefExp *));
        fe->addArg(**arrStaticRef);
        fe->addArg(*varThreadID);
        fe->addAttribute(DEBUG_STAT);
        stat->insertStmtBefore(*fe, *stat->controlParent());
    }
}

void InstrumentFunctionEnd (SgStatement *st, SgStatement *func) {
    if (sym_dbg_funcend == NULL) sym_dbg_funcend = new SgSymbol (PROCEDURE_NAME, "dbg_funcend");
    SgCallStmt *fe = new SgCallStmt(*sym_dbg_funcend);
    SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
    StatContext = (SgArrayRefExp **)func->symbol ()->attributeValue(0,STATIC_CONTEXT);
    if (StatContext == NULL) return;
    fe->addArg(**StatContext);
    fe->addArg(*varThreadID);
    fe->addAttribute(DEBUG_STAT);
    st->insertStmtBefore(*fe, *st->controlParent());
}


void InstrumentReadVar (SgStatement *st, SgExpression *exp, SgArrayRefExp *var) {
    if (InArrayRefList (exp)) return;
    SgArrayRefExp **StatContext = new (SgArrayRefExp *);
    StatContext = ((SgArrayRefExp **)exp->symbol ()->attributeValue(0,STATIC_CONTEXT));
    if (*StatContext != NULL) {
        SgCallStmt *fe = new SgCallStmt(*sym_dbg_read_var);
        fe->addArg(*var);
        fe->addArg(*varThreadID);
        fe->addArg(*exp);
        fe->addArg(**StatContext);
        fe->addAttribute(DEBUG_STAT);
        st->insertStmtBefore(*fe, *st->controlParent());
        IntoArrayRefList (exp);
    }
}

void InstrumentReadArray (SgStatement *st, SgExpression *exp, SgArrayRefExp *var) {
    if (InArrayRefList (exp)) return;
    SgArrayRefExp **StatContext = new (SgArrayRefExp *);
    StatContext = (SgArrayRefExp **)exp->symbol ()->attributeValue(0,STATIC_CONTEXT);
    if (*StatContext != NULL) {        
        SgExpression **arrFirstElement = new (SgExpression *);
        arrFirstElement = (SgExpression **)exp->symbol ()->attributeValue(0,FIRST_ELEM);
        if ((arrFirstElement != NULL) && (*arrFirstElement != NULL)) {
            SgCallStmt *fe = new SgCallStmt(*sym_dbg_read_arr);
            fe->addArg(*var);
            fe->addArg(*varThreadID);
            fe->addArg(*exp);
            fe->addArg(**StatContext);            
            fe->addArg(**arrFirstElement);
            fe->addAttribute(DEBUG_STAT);
            st->insertStmtBefore(*fe, *st->controlParent());
            IntoArrayRefList (exp);
        }
    }
}

void FindExternalProcedures (SgStatement *stat) {
        if (stat->variant () == EXTERN_STAT) {
            SgExprListExp *list = isSgExprListExp(stat->expr(0));  
            for (int i=0; i< list->length ();i++) {
                SgSymbol *sym=list->elem (i)->symbol ();
                char *str=sym->identifier ();
                if (!strcmp (str,"dbg_finalize")) {
                    sym_dbg_finalize = sym;
                    sym_dbg_finalize->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_init")) {
                    sym_dbg_init = sym;
                    sym_dbg_init->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_get_handle")) {
                    sym_dbg_get_handle = sym;
                    sym_dbg_get_handle->addAttribute (DECLARED_FUNC);
                    continue;		
                }
				if (!strcmp (str,"dbg_regarr")) {
					sym_dbg_regarr = sym;
					sym_dbg_regarr->addAttribute (DECLARED_FUNC);
					continue;		
				}
				if (!strcmp (str,"dbg_unregarr")) {
					sym_dbg_unregarr = sym;
					sym_dbg_unregarr->addAttribute (DECLARED_FUNC);
					continue;		
				}
                if (!strcmp (str,"dbg_regvar")) {
                    sym_dbg_regvar = sym;
                    sym_dbg_regvar->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_parallel")) {
                    sym_dbg_before_parallel = sym;
                    sym_dbg_before_parallel->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_parallel")) {
                    sym_dbg_after_parallel = sym;
                    sym_dbg_after_parallel->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_parallel_event")) {
                    sym_dbg_parallel_event = sym;
                    sym_dbg_parallel_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_parallel_event_end")) {
                    sym_dbg_parallel_event_end = sym;
                    sym_dbg_parallel_event_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_omp_loop")) {
                    sym_dbg_before_omp_loop = sym;
                    sym_dbg_before_omp_loop->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_omp_loop")) {
                    sym_dbg_after_omp_loop = sym;
                    sym_dbg_after_omp_loop->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_omp_loop_event")) {
                    sym_dbg_omp_loop_event = sym;
                    sym_dbg_omp_loop_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_loop")) {
                    sym_dbg_before_loop = sym;
                    sym_dbg_before_loop->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_loop")) {
                    sym_dbg_after_loop = sym;
                    sym_dbg_after_loop->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_loop_event")) {
                    sym_dbg_loop_event = sym;
                    sym_dbg_loop_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_write_var_begin")) {
                    sym_dbg_write_var_begin = sym;
                    sym_dbg_write_var_begin->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_write_arr_begin")) {
                    sym_dbg_write_arr_begin = sym;
                    sym_dbg_write_arr_begin->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_write_var_end")) {
                    sym_dbg_write_var_end = sym;
                    sym_dbg_write_var_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_write_arr_end")) {
                    sym_dbg_write_arr_end = sym;
                    sym_dbg_write_arr_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_read_arr")) {
                    sym_dbg_read_arr = sym;
                    sym_dbg_read_arr->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_read_var")) {
                    sym_dbg_read_var = sym;
                    sym_dbg_read_var->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_regpararr")) {
                    sym_dbg_regpararr = sym;
                    sym_dbg_regpararr->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_regparvar")) {
                    sym_dbg_regparvar = sym;
                    sym_dbg_regparvar->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_regcommon")) {
                    sym_dbg_regcommon = sym;
                    sym_dbg_regcommon->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_sections")) {
                    sym_dbg_before_sections = sym;
                    sym_dbg_before_sections->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_sections")) {
                    sym_dbg_after_sections = sym;
                    sym_dbg_after_sections->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_section_event")) {
                    sym_dbg_section_event = sym;
                    sym_dbg_section_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_section_event_end")) {
                    sym_dbg_section_event_end = sym;
                    sym_dbg_section_event_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_single")) {
                    sym_dbg_before_single = sym;
                    sym_dbg_before_single->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_single_event")) {
                    sym_dbg_single_event = sym;
                    sym_dbg_single_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_single_event_end")) {
                    sym_dbg_single_event_end = sym;
                    sym_dbg_single_event_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_single")) {
                    sym_dbg_after_single = sym;
                    sym_dbg_after_single->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_workshare")) {
                    sym_dbg_before_workshare = sym;
                    sym_dbg_before_workshare->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_workshare")) {
                    sym_dbg_after_workshare = sym;
                    sym_dbg_after_workshare->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_master_begin")) {
                    sym_dbg_master_begin = sym;
                    sym_dbg_master_begin->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_master_end")) {
                    sym_dbg_master_end = sym;
                    sym_dbg_master_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_critical")) {
                    sym_dbg_before_critical = sym;
                    sym_dbg_before_critical->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_critical_event")) {
                    sym_dbg_critical_event = sym;
                    sym_dbg_critical_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }                                
                if (!strcmp (str,"dbg_critical_event_end")) {
                    sym_dbg_critical_event_end = sym;
                    sym_dbg_critical_event_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }

                if (!strcmp (str,"dbg_after_critical")) {
                    sym_dbg_after_critical = sym;
                    sym_dbg_after_critical->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_barrier")) {
                    sym_dbg_before_barrier = sym;
                    sym_dbg_before_barrier->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_barrier")) {
                    sym_dbg_after_barrier = sym;
                    sym_dbg_after_barrier->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_flush_event")) {
                    sym_dbg_flush_event = sym;
                    sym_dbg_flush_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_flush")) {
                    sym_dbg_before_flush = sym;
                    sym_dbg_before_flush->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_ordered")) {
                    sym_dbg_before_ordered = sym;
                    sym_dbg_before_ordered->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_ordered_event")) {
                    sym_dbg_ordered_event = sym;
                    sym_dbg_ordered_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_ordered")) {
                    sym_dbg_after_ordered = sym;
                    sym_dbg_after_ordered->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_threadprivate")) {
                    sym_dbg_threadprivate = sym;
                    sym_dbg_threadprivate->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_funcall")) {
                    sym_dbg_before_funcall = sym;
                    sym_dbg_before_funcall->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_funcparvar")) {
                    sym_dbg_funcparvar = sym;
                    sym_dbg_funcparvar->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_funcpararr")) {
                    sym_dbg_funcpararr = sym;
                    sym_dbg_funcpararr->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_funcall")) {
                    sym_dbg_after_funcall = sym;
                    sym_dbg_after_funcall->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_funcbegin")) {
                    sym_dbg_funcbegin = sym;
                    sym_dbg_funcbegin->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_funcend")) {
                    sym_dbg_funcend = sym;
                    sym_dbg_funcend->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_if_loop_event")) {
                    sym_dbg_if_loop_event = sym;
                    sym_dbg_if_loop_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_omp_if_loop_event")) {
                    sym_dbg_omp_if_loop_event = sym;
                    sym_dbg_omp_if_loop_event->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_interval_begin")) {
                    sym_dbg_interval_begin = sym;
                    sym_dbg_interval_begin->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_interval_end")) {
                    sym_dbg_interval_end = sym;
                    sym_dbg_interval_end->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_before_io")) {
                    sym_dbg_before_io = sym;
                    sym_dbg_before_io->addAttribute (DECLARED_FUNC);
                    continue;		
                }
                if (!strcmp (str,"dbg_after_io")) {
                    sym_dbg_after_io = sym;
                    sym_dbg_after_io->addAttribute (DECLARED_FUNC);
                    continue;		
                }
            }
        }
}

void DeclareExternalProcedures (SgStatement *debug) {
    SgStatement *decl = new SgStatement(EXTERN_STAT);
    //SgExprListExp *list = new SgExprListExp(*new SgVarRefExp(*sym_dbg_init));
    SgExprListExp *list = new SgExprListExp();
    if ((sym_dbg_init != NULL) && (sym_dbg_init->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_init));
    if ((sym_dbg_finalize != NULL) && (sym_dbg_finalize->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_finalize));
    if ((sym_dbg_get_handle != NULL) && (sym_dbg_get_handle->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_get_handle));
	if ((sym_dbg_regarr != NULL) && (sym_dbg_regarr->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_regarr));
	if ((sym_dbg_unregarr != NULL) && (sym_dbg_unregarr->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_unregarr));
    if ((sym_dbg_regvar != NULL) && (sym_dbg_regvar->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_regvar));
    if ((sym_dbg_before_parallel != NULL) && (sym_dbg_before_parallel->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_parallel));
    if ((sym_dbg_after_parallel != NULL) && (sym_dbg_after_parallel->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_parallel));
    if ((sym_dbg_parallel_event != NULL) && (sym_dbg_parallel_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_parallel_event));
    if ((sym_dbg_parallel_event_end != NULL) && (sym_dbg_parallel_event_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_parallel_event_end));
    if ((sym_dbg_before_omp_loop != NULL) && (sym_dbg_before_omp_loop->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_omp_loop));
    if ((sym_dbg_after_omp_loop != NULL) && (sym_dbg_after_omp_loop->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_omp_loop));
    if ((sym_dbg_omp_loop_event != NULL) && (sym_dbg_omp_loop_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_omp_loop_event));
    if ((sym_dbg_before_loop != NULL) && (sym_dbg_before_loop->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_loop));
    if ((sym_dbg_after_loop != NULL) && (sym_dbg_after_loop->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_loop));
    if ((sym_dbg_loop_event != NULL) && (sym_dbg_loop_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_loop_event));
    if ((sym_dbg_write_var_begin != NULL) && (sym_dbg_write_var_begin->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_write_var_begin));
    if ((sym_dbg_write_arr_begin != NULL) && (sym_dbg_write_arr_begin->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_write_arr_begin));
    if ((sym_dbg_write_var_end != NULL) && (sym_dbg_write_var_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_write_var_end));
    if ((sym_dbg_write_arr_end != NULL) && (sym_dbg_write_arr_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_write_arr_end));
    if ((sym_dbg_read_var != NULL) && (sym_dbg_read_var->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_read_var));
    if ((sym_dbg_read_arr != NULL) && (sym_dbg_read_arr->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_read_arr));
    if ((sym_dbg_regpararr != NULL) && (sym_dbg_regpararr->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_regpararr));
    if ((sym_dbg_regparvar != NULL) && (sym_dbg_regparvar->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_regparvar));
    if ((sym_dbg_regcommon != NULL) && (sym_dbg_regcommon->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_regcommon));
    if ((sym_dbg_before_sections != NULL) && (sym_dbg_before_sections->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_sections));
    if ((sym_dbg_after_sections != NULL) && (sym_dbg_after_sections->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_sections));
    if ((sym_dbg_section_event != NULL) && (sym_dbg_section_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_section_event));
    if ((sym_dbg_section_event_end != NULL) && (sym_dbg_section_event_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_section_event_end));
    if ((sym_dbg_before_single != NULL) && (sym_dbg_before_single->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_single));
    if ((sym_dbg_single_event != NULL) && (sym_dbg_single_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_single_event));
    if ((sym_dbg_single_event_end != NULL) && (sym_dbg_single_event_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_single_event_end));
    if ((sym_dbg_after_single != NULL) && (sym_dbg_after_single->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_single));
    if ((sym_dbg_before_workshare != NULL) && (sym_dbg_before_workshare->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_workshare));
    if ((sym_dbg_after_workshare != NULL) && (sym_dbg_after_workshare->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_workshare));
    if ((sym_dbg_master_begin != NULL) && (sym_dbg_master_begin->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_master_begin));
    if ((sym_dbg_master_end != NULL) && (sym_dbg_master_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_master_end));
    if ((sym_dbg_before_critical != NULL) && (sym_dbg_before_critical->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_critical));
    if ((sym_dbg_critical_event != NULL) && (sym_dbg_critical_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_critical_event));
    if ((sym_dbg_critical_event_end != NULL) && (sym_dbg_critical_event_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_critical_event_end));
    if ((sym_dbg_after_critical != NULL) && (sym_dbg_after_critical->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_critical));
    if ((sym_dbg_before_barrier != NULL) && (sym_dbg_before_barrier->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_barrier));
    if ((sym_dbg_after_barrier != NULL) && (sym_dbg_after_barrier->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_barrier));
    if ((sym_dbg_flush_event != NULL) && (sym_dbg_flush_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_flush_event));
    if ((sym_dbg_before_flush != NULL) && (sym_dbg_before_flush->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_flush));
    if ((sym_dbg_before_ordered != NULL) && (sym_dbg_before_ordered->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_ordered));
    if ((sym_dbg_ordered_event != NULL) && (sym_dbg_ordered_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_ordered_event));
    if ((sym_dbg_after_ordered != NULL) && (sym_dbg_after_ordered->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_ordered));
    if ((sym_dbg_threadprivate != NULL) && (sym_dbg_threadprivate->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_threadprivate));
    if ((sym_dbg_before_funcall != NULL) && (sym_dbg_before_funcall->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_funcall));
    if ((sym_dbg_after_funcall != NULL) && (sym_dbg_after_funcall->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_funcall));
    if ((sym_dbg_funcparvar != NULL) && (sym_dbg_funcparvar->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_funcparvar));
    if ((sym_dbg_funcpararr != NULL) && (sym_dbg_funcpararr->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_funcpararr));
    if ((sym_dbg_funcbegin != NULL) && (sym_dbg_funcbegin->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_funcbegin));
    if ((sym_dbg_funcend != NULL) && (sym_dbg_funcend->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_funcend));
    if ((sym_dbg_if_loop_event != NULL) && (sym_dbg_if_loop_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_if_loop_event));
    if ((sym_dbg_omp_if_loop_event != NULL) && (sym_dbg_omp_if_loop_event->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_omp_if_loop_event));
    if ((sym_dbg_before_io != NULL) && (sym_dbg_before_io->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_before_io));
    if ((sym_dbg_after_io != NULL) && (sym_dbg_after_io->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_after_io));
    if ((sym_dbg_interval_begin != NULL) && (sym_dbg_interval_begin->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_interval_begin));
    if ((sym_dbg_interval_end != NULL) && (sym_dbg_interval_end->getAttribute(0,DECLARED_FUNC) == NULL)) list->append(*new SgVarRefExp(*sym_dbg_interval_end));

    if (list->length ()>1) {
        decl -> setExpression(0,*list->rhs());
	    debug-> insertStmtBefore(*decl, *debug->controlParent());
    }
}

void UpdateIncludeVarsFile(SgStatement *st, const char *input_file) {
    freopen (input_file,"w",stdout);
    SgStatement *last = st->lastNodeOfStmt ();
    for (SgStatement *stat=st->lexNext (); stat && (stat != last); stat=stat->lexNext()) {
        if (stat->variant () != PROC_STAT) {
            stat->unparsestdout ();
        }
    }
    fclose (stdout);
}

void UpdateIncludeInitFile(SgStatement *st, const char *input_file) {    
    freopen (input_file,"w",stdout);
    SgStatement *last = st->lastNodeOfStmt ();
    SgStatement *prev = st;    
    for (SgStatement *stat=st->lexNext (); stat && (stat != last); stat=prev->lexNext()) {
        if (stat->variant () != PROC_STAT) {
            prev->setLexNext (*stat->lexNext());
            stat->extractStmt ();
        } else prev = stat;
    }
    char *data_str = new char[20];
    sprintf(data_str,"include 'dbg_vars.h'"); 
    SgStatement *decl = new SgStatement(DATA_DECL);// creates DATA statement
    SgExpression *es = new SgExpression(STMT_STR);
    NODE_STR(es->thellnd) = data_str;
    decl -> setExpression(0,*es);
    st->insertStmtAfter (*decl);    
    st->unparsestdout ();
    if (isMainProgram == 1) {
        char *data_str = new char[20];        
        sprintf(data_str,"include 'dbg_init.h'"); 
        SgStatement *decl = new SgStatement(DATA_DECL);
        SgExpression *es = new SgExpression(STMT_STR);
        NODE_STR(es->thellnd) = data_str;
        decl -> setExpression(0,*es);
        last->insertStmtAfter (*decl);
        data_str = new char[20];
	    sprintf(data_str,"data ithreadid /-1/"); 
	    decl = new SgStatement(DATA_DECL);
	    es = new SgExpression(STMT_STR);
	    NODE_STR(es->thellnd) = data_str;
	    decl -> setExpression(0,*es);
        SgExpression *common = new SgExpression (COMM_LIST);
        SgSymbol *dbg_thread=new SgSymbol (VARIABLE_NAME,"dbg_thread");
        common->setSymbol (*dbg_thread);
        SgVarRefExp *ithreadid = new SgVarRefExp (*new SgSymbol (VARIABLE_NAME,"ithreadid"));
        common->setLhs (*ithreadid);
        SgStatement *common_stat= new SgStatement(COMM_STAT);
        common_stat->setExpression (0, *common);
        SgStatement *thread = new SgStatement (OMP_THREADPRIVATE_DIR);
        SgExpression *th = new SgExpression (OMP_THREADPRIVATE);
        th->setLhs (*new SgExprListExp (*new SgVarRefExp (*dbg_thread)));
        thread->setExpression (0, *th);
        SgStatement *BlockData = new SgStatement(BLOCK_DATA);
        BlockData->setSymbol (*new SgSymbol (VARIABLE_NAME,"dbgthread"));
        last->insertStmtAfter(*BlockData);
        last->insertStmtAfter(*new SgStatement(CONTROL_END), *BlockData);
        last->insertStmtAfter(*decl, *BlockData);
        last->insertStmtAfter(*thread, *BlockData);
        last->insertStmtAfter(*common_stat, *BlockData);
        
    }
    st->extractStmtBody ();
    st->extractStmt ();
    fclose (stdout);
}
SgExpression *GetOmpAddresMem (SgExpression *exp) {
	SgFunctionCallExp *fe;
	if (sym_dbg_get_addr == NULL) {
		sym_dbg_get_addr = new SgSymbol(PROCEDURE_NAME, "dbg_get_addr");
	}
	fe = new SgFunctionCallExp(*sym_dbg_get_addr);
	fe->addArg(exp->copy());
	return(fe);
}
SgStatement * FindOuterLoop(SgStatement *st) {
    SgStatement *tmp=NULL;
    SgStatement *res=NULL;
    for (tmp=st; tmp && (tmp->variant () != GLOBAL); tmp = tmp->controlParent ()) {
        if (isSgForStmt (tmp)) {
            res = tmp;
        }
    }
    return res;
}

int FindLabelInLoop(SgStatement *st, SgLabel *lbl) {
    SgStatement *tmp=NULL;
    SgStatement *last=GetLastStatementOfLoop (st);
    int res=0;
    if (isSgForStmt(st)) {
		if (last->hasLabel ()) 
			if (LABEL_STMTNO(last->label()->thelabel) == LABEL_STMTNO (lbl->thelabel)) return 1;
        for (tmp=st; tmp && (tmp != last); tmp = tmp->lexNext ()) {
            if (tmp->hasLabel ())
                if (LABEL_STMTNO(tmp->label()->thelabel) == LABEL_STMTNO (lbl->thelabel)) return 1;
        }
    }
    return res;
}

void InstrumentGotoStmt (SgStatement *st) {
    SgGotoStmt *gotost = isSgGotoStmt (st);
    if (!gotost) return;
    SgLabel *lbl = gotost->branchLabel();
    if (!lbl) return;
    SgStatement *tmp=NULL;
    for (tmp=st; tmp && (tmp->variant () != GLOBAL); tmp = tmp->controlParent ()) {
        if (isSgForStmt (tmp)) {
            int inparloop = tmp->lexPrev () && (tmp->lexPrev ()->variant () == OMP_DO_DIR);
            if (!FindLabelInLoop(tmp, lbl)) {
                SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
                StatContext = (SgArrayRefExp **)tmp->attributeValue(0,STATIC_CONTEXT);
                if (StatContext != NULL) {
                    SgCallStmt *fe = NULL;
                    if (inparloop) {                         
                            if (sym_dbg_after_omp_loop == NULL) sym_dbg_after_omp_loop = new SgSymbol (PROCEDURE_NAME, "dbg_after_omp_loop");
                            fe = new SgCallStmt(*sym_dbg_after_omp_loop);
                    } else {
                            if (sym_dbg_after_loop == NULL) sym_dbg_after_loop = new SgSymbol (PROCEDURE_NAME, "dbg_after_loop");
                            fe = new SgCallStmt(*sym_dbg_after_loop);
                    }
                    fe->addArg(**StatContext);
                    fe->addArg(*varThreadID);    
                    fe->addAttribute(DEBUG_STAT);
                    st->insertStmtBefore(*fe, *st->controlParent());
                }
            }
        }
    }
}

void InstrumentExitFromLoops (SgStatement *st) {
    SgStatement *tmp=NULL;
    for (tmp=st; tmp && (tmp->variant () != GLOBAL); tmp = tmp->controlParent ()) {
        if (isSgForStmt (tmp)) {
            int inparloop = tmp->lexPrev () && (tmp->lexPrev ()->variant () == OMP_DO_DIR);
            SgArrayRefExp **StatContext =  new (SgArrayRefExp *);
            StatContext = (SgArrayRefExp **)tmp->attributeValue(0,STATIC_CONTEXT);
            if (StatContext != NULL) {
                SgCallStmt *fe = NULL;
                if (inparloop) {                         
                    if (sym_dbg_after_omp_loop == NULL) sym_dbg_after_omp_loop = new SgSymbol (PROCEDURE_NAME, "dbg_after_omp_loop");
                    fe = new SgCallStmt(*sym_dbg_after_omp_loop);
                } else {
                    if (sym_dbg_after_loop == NULL) sym_dbg_after_loop = new SgSymbol (PROCEDURE_NAME, "dbg_after_loop");
                    fe = new SgCallStmt(*sym_dbg_after_loop);
                }
                fe->addArg(**StatContext);
                fe->addArg(*varThreadID);    
                fe->addAttribute(DEBUG_STAT);
                st->insertStmtBefore(*fe, *st->controlParent());
            }
        }
    }
}
void GenerateNowaitPlusBarrier (SgStatement *st) {
    char *strStaticContext = new char [MaxContextBufferLength];
    int wasnowaitclause = 0;
    if ((st->variant () == OMP_END_DO_DIR) ||
     (st->variant () == OMP_END_SINGLE_DIR)||    
     (st->variant () == OMP_END_SECTIONS_DIR)||    
     (st->variant () == OMP_END_WORKSHARE_DIR)){
        SgExprListExp *exp = isSgExprListExp (st->expr(0));
        if (exp != NULL) {
            for (int i=0; i<exp->length (); i++) {
                if (exp->elem (i)->variant()== OMP_NOWAIT) {
                       wasnowaitclause = 1; 
                       break;
                }
            }
            if (wasnowaitclause) {
                return;
            }
            exp->append (*new SgExpression (OMP_NOWAIT));
        } else {
            st->setExpression (0, *new SgExprListExp(*new SgExpression(OMP_NOWAIT)));
        }
    }
    SgStatement *next = st->lexNext ();
    SgStatement *stat = new SgStatement (OMP_BARRIER_DIR);
    stat->addAttribute(DEBUG_STAT);
    stat->setlineNumber (st->lineNumber ());
    next->insertStmtBefore(*stat, *next->controlParent());
    memset(strStaticContext, 0, MaxContextBufferLength);
    strcat(strStaticContext,"*type=barrier");            
    GenerateFileAndLine (stat, strStaticContext);            
    InstrumentOmpBarrierDir (stat, strStaticContext);            
    GenerateCallGetHandle (strStaticContext);
}
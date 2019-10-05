#include "dvm.h"
void AddSharedClauseForDVMVariables (SgStatement *first, SgStatement *last);

int IsPositiveDoStep(SgExpression *step) {
    int s;
    if (step == NULL) return (1);
    if(step->isInteger())
        s=step->valueInteger();
    else 
        s = 0;
    if(s >= 0)
        return(1);
    else
        return(0);
}


int isOmpDir (SgStatement * st) {
	if ((BIF_CODE(st->thebif)>800) && (BIF_CODE(st->thebif)<847)) {
		return 1;
	}
	return 0;
}
inline int isDvmDir (SgStatement * st) {
	switch (BIF_CODE(st->thebif)) {
		case DVM_INTERVAL_DIR:
		case DVM_ENDINTERVAL_DIR:
		case DVM_DEBUG_DIR:
		case DVM_ENDDEBUG_DIR:
		case DVM_TRACEON_DIR:
		case DVM_TRACEOFF_DIR:
		case DVM_PARALLEL_ON_DIR:
		case DVM_SHADOW_START_DIR:
		case DVM_SHADOW_GROUP_DIR:
		case DVM_SHADOW_WAIT_DIR:
		case DVM_REDUCTION_START_DIR:
		case DVM_REDUCTION_GROUP_DIR:
		case DVM_REDUCTION_WAIT_DIR:
		case DVM_DYNAMIC_DIR:
		case DVM_ALIGN_DIR:
		case DVM_REALIGN_DIR:
		case DVM_REALIGN_NEW_DIR:
		case DVM_REMOTE_ACCESS_DIR:
		case HPF_INDEPENDENT_DIR:
		case DVM_SHADOW_DIR:
		case DVM_NEW_VALUE_DIR:
		case DVM_VAR_DECL:
		case DVM_POINTER_DIR:
		case HPF_TEMPLATE_STAT:
		case HPF_ALIGN_STAT:
		case HPF_PROCESSORS_STAT:
		case DVM_REDISTRIBUTE_DIR:
		case DVM_TASK_REGION_DIR:
		case DVM_END_TASK_REGION_DIR:
		case DVM_ON_DIR:
		case DVM_END_ON_DIR:
		case DVM_TASK_DIR:
		case DVM_MAP_DIR:
		case DVM_PARALLEL_TASK_DIR:
		case DVM_INHERIT_DIR:
		case DVM_INDIRECT_GROUP_DIR:
		case DVM_INDIRECT_ACCESS_DIR:
		case DVM_REMOTE_GROUP_DIR:
		case DVM_RESET_DIR:
		case DVM_PREFETCH_DIR:
		case DVM_OWN_DIR:
		case DVM_HEAP_DIR:
		case DVM_ASYNCID_DIR:
		case DVM_ASYNCHRONOUS_DIR:
		case DVM_ENDASYNCHRONOUS_DIR:
		case DVM_ASYNCWAIT_DIR:
		case DVM_F90_DIR:
		case DVM_BARRIER_DIR:
		case DVM_CONSISTENT_GROUP_DIR:
		case DVM_CONSISTENT_START_DIR:
		case DVM_CONSISTENT_WAIT_DIR:
		case DVM_CONSISTENT_DIR:
		case DVM_CHECK_DIR: return 1; break;
	}
	return 0;
}

int HideOmpStmt (SgStatement * st) {
	int res=0;
	SgStatement *prev = st->lexPrev ();
	SgStatement *next =st->lexNext ();
	while (prev && (isDvmDir(prev) || isOmpDir(prev))) prev = prev -> lexPrev ();
	while (next && (isDvmDir(next) || isOmpDir(next))) next = next -> lexNext ();
	if (prev && next) {
		int length=st->numberOfAttributes();
		int i=0;
		SgAttribute *sa=NULL;
		res=1;
		switch (st->variant ()) {
			case OMP_END_PARALLEL_DO_DIR:
			case OMP_END_DO_DIR: {
				for (i=0; i<length; i++) {
					sa=st->getAttribute(i);
					prev->addAttribute(sa->getAttributeType(),sa->getAttributeData(),sa->getAttributeSize());
				}
				for (i=length; i>0; i--) {
					st->deleteAttribute(i);
				} 
				prev->addAttribute(OMP_STMT_AFTER, (void*) st->copyPtr (), sizeof(SgStatement *));
				break;
			}
			default: {
				for (i=0; i<length; i++) {
					sa=st->getAttribute(i);
					next->addAttribute(sa->getAttributeType(),sa->getAttributeData(),sa->getAttributeSize());
				}
				for (i=length; i>0; i--) {
					st->deleteAttribute(i);
				} 				
				next->addAttribute(OMP_STMT_BEFORE, (void*) st->copyPtr (), sizeof(SgStatement *));
				break;
			}
		}        
	}
	return res;
}

void AddAttributeOmp (SgStatement *stmt) {
	SgStatement *last;
	if (!stmt) return;
	last = stmt->lastNodeOfStmt ()->lexNext ();
	for (SgStatement *st=stmt;st && (st != last); st=st->lexNext ()) {
		st->addAttribute (OMP_MARK);
	}
}

void DelAttributeFromStmt (int type, SgStatement *st) {
int length=st->numberOfAttributes();
for (int i=0; i<length; i++) {
	SgAttribute *sa=NULL;
	sa = st->getAttribute(i);
	if (sa->getAttributeType() == type) {
		st->deleteAttribute(i);
		break;
	}
}
}

int AddOmpStmt (SgStatement * st) {
	int res = 0;
	int length=st->numberOfAttributes(OMP_STMT_BEFORE);
	int i=0;
	SgStatement *stmt = NULL;
	SgStatement *last = st->lastNodeOfStmt ();
	for (i=0;i<length; i++) {
	
		SgAttribute *sa=st->getAttribute(i,OMP_STMT_BEFORE);
		stmt = ((SgStatement *)sa->getAttributeData());
		AddAttributeOmp (stmt);
                if ((st->variant () == FOR_NODE) && (stmt->variant () == ASSIGN_STAT)) {
                   SgExpression *expr = stmt->expr (1);
                   if (expr->variant () == FUNC_CALL) {
                      if (!strcmp(expr->symbol()->identifier(),"min")) {
                         SgExprListExp *exp = isSgExprListExp(expr->lhs ());
                         if (exp) {
                              exp = isSgExprListExp(exp->rhs ());
                              if (exp) {                             
                                 SgForStmt *forst = isSgForStmt (st);
                                 if (forst) {
					 //TO DO
					  if ((forst->step () != NULL)&&(forst->step ()->isInteger ())) {
				            if (forst->step ()->valueInteger ()>0)
						 exp->setValue (*forst->end () - *forst->start());
					    else
						 exp->setValue (*forst->start () - *forst->end());
					 } else if (forst->step () == NULL) {
    						 exp->setValue (*forst->end () - *forst->start());	 
					 } else {
    					    SgFunctionCallExp *func = new SgFunctionCallExp(*new SgVariableSymb("abs"));
					    func->addArg(*forst->end () - *forst->start());
					    exp->setValue (*func);										 
					 }
                                 }
                              }
                         }                          
                      }
                   }                 
                }
		st->insertStmtBefore (*stmt);
	}
	length=st->numberOfAttributes(OMP_STMT_AFTER);
	for (i=length; i>0; i--) {
		SgAttribute *sa=st->getAttribute(i-1,OMP_STMT_AFTER);
		stmt = ((SgStatement *)sa->getAttributeData());
		AddAttributeOmp (stmt);
		last->insertStmtAfter (*stmt);
		res++;
	}
	return res;
}

SgStatement * GetLexNextIgnoreOMP(SgStatement *st) { 
	SgStatement *ret=st->lexNext ();
    if (ret && isOmpDir (ret)) {
		return GetLexNextIgnoreOMP (ret);
	}
	return ret;
}

int isOmpGetNumThreads(SgExpression *e)
{
  int replace = 0;
  if (e == NULL) return 0;
  if ((e->variant()==FUNC_CALL) && !strcmp(e->symbol()->identifier(),"omp_get_num_threads")) {
       NODE_CODE(e->thellnd)=INT_VAL;
       NODE_TYPE(e->thellnd) =  GetAtomicType(T_INT);
       NODE_INT_CST_LOW (e->thellnd) = 1;
       replace = 1;
  }
  if((e->variant()==ADD_OP) || (e->variant()==SUBT_OP)){
        replace = isOmpGetNumThreads (e->rhs());
	if (!replace) replace = isOmpGetNumThreads (e->lhs());
   }
  return replace;
}

SgExpression * FindSubExpression (SgExpression *expr1,SgExpression *expr2) {
   SgExpression * res= NULL;
   if ((expr1 == NULL) || (expr2 == NULL)) return res;
   if ((expr1->variant () == expr2->variant ()) &&
       (expr1->lhs () != NULL) &&
       (expr2->lhs () != NULL) &&
       (expr1->rhs () != NULL) &&
       (expr2->rhs () != NULL) &&
       isSgVarRefExp(expr1->lhs ()) &&
       isSgVarRefExp(expr1->rhs ()) &&
       isSgVarRefExp(expr2->lhs ()) &&
       isSgVarRefExp(expr2->rhs ())) {
          SgSymbol *expr1_sym1=expr1->lhs ()->symbol ();
          SgSymbol *expr1_sym2=expr1->rhs ()->symbol ();
          SgSymbol *expr2_sym1=expr2->lhs ()->symbol ();
          SgSymbol *expr2_sym2=expr2->rhs ()->symbol ();
          if (!strcmp (expr1_sym1->identifier(),expr2_sym1->identifier()) && !strcmp (expr1_sym2->identifier(),expr2_sym2->identifier())) return expr1;
   }
   res = FindSubExpression(expr1->lhs (), expr2);
   if (res == NULL) return FindSubExpression(expr1->rhs (), expr2);
   return res;
}

SgSymbol *ChangeParallelDir (SgStatement *stmt) {
   SgExprListExp *exp=isSgExprListExp (stmt->expr(1));
   int i=0;
   if (exp == NULL) return NULL;   
   for (SgExpression *expr=exp->elem(i); i<exp->length(); i++) {
       if (expr->variant () == ACROSS_OP) {
           SgStatement *st;
           SgStatement *loop=GetLexNextIgnoreOMP (stmt);
           for(st=loop; st && (st != loop->lastNodeOfStmt ()); st=st->lexNext ()) {
               if (st->variant () == ASSIGN_STAT) {
                   if (st->lexNext ()->variant () == FOR_NODE) {
                       SgStatement *forst = st->lexNext ();
                       int length=forst->numberOfAttributes(OMP_STMT_BEFORE);
                       int find=0;
                       for (int i=0; i<length; i++) {
                           SgAttribute *sa=forst->getAttribute(i,OMP_STMT_BEFORE);
                           if (((SgStatement *)sa->getAttributeData())->variant () == OMP_DO_DIR) {
                                find=1; break;
                           }
                       }
                       if (find == 0) return NULL;
                       SgSymbol *j=st->expr(0)->symbol();
                       SgSymbol *newj=st->expr(1)->lhs()->symbol();
                       SgExpression *newj_iam=st->expr(1);
                       SgExpression *res = FindSubExpression (stmt->expr(0),newj_iam);
                       if (res != NULL) {                           
                           NODE_CODE(res->thellnd) = VAR_REF;
                           res->setSymbol (*j);
                           delete res->lhs();
                           delete res->rhs();
                           res->setLhs (NULL);
                           res->setRhs (NULL);                           
                       }
                       stmt->replaceSymbBySymb(*newj,*j);
                       loop->setSymbol (*j);
                       if (HideOmpStmt (st)) st->extractStmt ();
                       return newj;
                   }	           
               }
               if (isSgForStmt (st)) loop = st;
           }
        }        
   }
   return NULL; 
}

void ChangeAccrossOpenMPParam (SgStatement *stmt, SgSymbol *newj, int ub) {
   SgStatement *st=stmt;
   SgStatement *loop=NULL;
   SgValueExp c1(1);
   if (ub == 0) return;
   int find=0;
   for(; st && st->lexNext () && (st != stmt->lastNodeOfStmt ()); st=st->lexNext ()) {
       if (st->variant ()== FOR_NODE) loop = st;
       SgStatement * forst=st->lexNext ();
       int length=forst->numberOfAttributes(OMP_STMT_BEFORE);
       find=0;
       for (int i=0; i<length; i++) {
           SgAttribute *sa=forst->getAttribute(i,OMP_STMT_BEFORE);
           if (((SgStatement *)sa->getAttributeData())->variant () == OMP_DO_DIR) {
               find=1; break;
           }               
       }       
       if (find == 1) break;
   }
   if ((find==1) && loop && (newj != NULL))  {
       SgForStmt *accr_do = isSgForStmt(loop);
       for (;st && (st->lexNext() != NULL) && (st != loop->lastNodeOfStmt ()); st=st->lexNext ())
       if ((st->lexNext()!= NULL) && (st->lexNext()->lexNext() != NULL)) {
            SgExpression *expr = new SgVarRefExp (loop->symbol ());
            SgStatement *stIfStmt = st->lexNext()->lexNext();
            if (IsPositiveDoStep(accr_do->step())) {
                *expr = expr->copy() < accr_do->start()->copy() || expr->copy() > accr_do->end()->copy ();
            } else {
                *expr = expr->copy() < accr_do->end()->copy() || expr->copy() > accr_do->start()->copy ();
            }                 
            if (stIfStmt->lexNext()->variant () == CYCLE_STMT) {
                SgIfStmt *ifst = isSgIfStmt (stIfStmt);
                if (ifst != NULL) {
                    ifst->setExpression (0, *expr);
                } else {
                    SgLogIfStmt *logifst = isSgLogIfStmt (stIfStmt);
                    if (logifst != NULL) {
                        logifst->setExpression (0, *expr);
                    }
                } 
            }
       }
       if (ub == 1) {
           SgExpression *ind = accr_do->end ();
           *ind = *ind + *new SgFunctionCallExp(*new SgVariableSymb("OMP_GET_NUM_THREADS")) - c1.copy ();
           accr_do->setEnd(*ind);  
       } else if (ub == 2) {
           SgExpression *ind = accr_do->start ();
           *ind = *ind + *new SgFunctionCallExp(*new SgVariableSymb("OMP_GET_NUM_THREADS")) - c1.copy ();
           accr_do->setStart(*ind);  
       }
       loop->setSymbol (*newj);
   }
}

void ChangeParallelLoopHideOpenmp(SgStatement *stmt)
{
  int nloop=0;
  SgStatement *prev=NULL;
  SgStatement *st;
  stmt_list *stmt_to_delete = NULL;
  for(SgExpression *dovar=stmt->expr(2); dovar; dovar=dovar->rhs()) nloop++;
  SgStatement *next=stmt->lexNext ();
  SgStatement *forst, *last;
  prev=stmt->lexPrev ();
  if ((next->variant () == OMP_PARALLEL_DO_DIR) ||
       (next->variant () == OMP_DO_DIR)) {
       forst = next->lexNext ();
       if (forst->variant () == FOR_NODE) {
            forst->addAttribute(OMP_STMT_BEFORE, (void*) next->copyPtr (), sizeof(SgStatement *));
            stmt_to_delete = addToStmtList(stmt_to_delete, next);
            last=forst->lastNodeOfStmt ()->lexNext ();
            if ((last->variant () == OMP_END_PARALLEL_DO_DIR) ||
               (last->variant () == OMP_END_DO_DIR)) {
                   forst->addAttribute(OMP_STMT_AFTER, (void*) last->copyPtr (), sizeof(SgStatement *));
                   stmt_to_delete = addToStmtList(stmt_to_delete, last);
	    }
        }
   } else {
        if ((prev->variant () == OMP_PARALLEL_DO_DIR) ||
            (prev->variant () == OMP_DO_DIR)) {
               forst = next;
               if (forst->variant () == FOR_NODE) {
                   forst->addAttribute(OMP_STMT_BEFORE, (void*) prev->copyPtr (), sizeof(SgStatement *));
                   stmt_to_delete = addToStmtList(stmt_to_delete, prev);
               }
               last=forst->lastNodeOfStmt ()->lexNext ();
               if ((last->variant () == OMP_END_PARALLEL_DO_DIR) ||
                 (last->variant () == OMP_END_DO_DIR)) {
                  forst->addAttribute(OMP_STMT_AFTER, (void*) last->copyPtr (), sizeof(SgStatement *));
                  stmt_to_delete = addToStmtList(stmt_to_delete, last);
	       }      
        } else {
          if (next->variant () == FOR_NODE) {
             for(st=next, prev=st; st && (nloop>0); st=st->lexNext ()) {
                if (st->variant () == FOR_NODE) {
	           if ((prev != st) && (prev->lexNext () != st)) {
                      for(SgStatement *s=prev->lexNext (); s && (s!= st); s=s->lexNext ()) {
                         st->addAttribute(OMP_STMT_BEFORE, (void*) s->copyPtr (), sizeof(SgStatement *));
                         stmt_to_delete = addToStmtList(stmt_to_delete, s);
                         s=s->lastNodeOfStmt ();
                      }
                      SgStatement *last=prev->lastNodeOfStmt();
                      for(SgStatement *s=st->lastNodeOfStmt()->lexNext (); s && (s!= last); s=s->lexNext ()) {
                         st->addAttribute(OMP_STMT_AFTER, (void*) s->copyPtr (), sizeof(SgStatement *));
                         stmt_to_delete = addToStmtList(stmt_to_delete, s);
                         s=s->lastNodeOfStmt ();
                      }
                   }
                   prev = st;
                   nloop--;
                }
             }
          }
      }
  }
  for(;stmt_to_delete; stmt_to_delete= stmt_to_delete->next) Extract_Stmt(stmt_to_delete->st);// extracting  OpenMP Directives
}

void MarkAndReplaceOriginalStmt (SgStatement *func) {
  SgStatement *stmt = NULL;
  SgStatement *first = func->lexNext();
  SgStatement *last = func->lastNodeOfStmt();
  SgStatement *next = NULL;
  int res=0;
  for (stmt = first; stmt && (stmt != last);stmt=stmt->lexNext ()) {
      if (stmt->hasLabel ()&& (stmt->variant() != FORMAT_STAT)&& (stmt->variant() != CONT_STAT)) {
            SgStatement *tmp = new SgStatement (CONT_STAT);
            tmp->setLabel (*stmt->label ());
            tmp->setlineNumber (stmt->lineNumber());
            tmp->addAttribute(OMP_MARK);
            stmt->insertStmtBefore(*tmp, *stmt->controlParent());
            BIF_LABEL(stmt->thebif)=NULL;
      }
      stmt->addAttribute(OMP_MARK);
      if (stmt->variant () == DVM_PARALLEL_ON_DIR) ChangeParallelLoopHideOpenmp(stmt);
      continue;      
      switch (stmt->variant ()) {
          case OMP_PARALLEL_DO_DIR:
          case OMP_DO_DIR:
          case OMP_END_PARALLEL_DO_DIR:
          case OMP_END_DO_DIR: res=HideOmpStmt (stmt); break;
          case LOGIF_NODE: LogIf_to_IfThen(stmt); break;
      }
      if (res == 0) {
          stmt = stmt->lexNext();
      } else {
          res = 0;
          next = stmt->lexNext();
          stmt->extractStmt ();
          stmt = next;
      }
  }
}
stmt_list * PushToStmtList(stmt_list *pstmt, SgStatement *stat) {
   stmt_list *stl; 
   if (!pstmt) {
              pstmt = new stmt_list;
              pstmt->st = stat;
              pstmt->next =  NULL;
   } else {
	      stl = new stmt_list;
              stl->st = stat;
              stl->next = pstmt;
              pstmt = stl;
   }
   return (pstmt);
}

int ValFromStmtList(stmt_list *pstmt) {
	if (pstmt) {
		return pstmt->st->variant ();     
	}
	return 0;
}

stmt_list * PopFromStmtList(stmt_list *pstmt) {
    if (pstmt) {
        stmt_list *tmp = pstmt;
        pstmt = pstmt->next;
        tmp->next = NULL;
        delete tmp;        
        return (pstmt);
    }
    return NULL;
}

int isFromOneThread (int variant) {
    switch (variant) {
        case OMP_ONETHREAD_DIR:
		case OMP_DO_DIR:
		case OMP_SECTIONS_DIR:
		case OMP_SINGLE_DIR:
		case OMP_WORKSHARE_DIR:
		case OMP_PARALLEL_DO_DIR:
		case OMP_PARALLEL_SECTIONS_DIR:
		case OMP_PARALLEL_WORKSHARE_DIR:
		case OMP_MASTER_DIR:
		case OMP_CRITICAL_DIR:
        case PROG_HEDR:
		case OMP_ORDERED_DIR: {
            return 1; break;
        }
        case PROC_HEDR:
        case FUNC_HEDR:
        case OMP_PARALLEL_DIR: {
            return 0; break;
        }
        default: {
            return -1;
            break;
        }
    }
    return -1;
}

SgStatement * InsertBeginSynchroStat (SgStatement *current) { /*OMP*/
	if (isADeclBif(current->variant ())) return NULL;   
	return current;
}

int InsertEndSynchroStat (SgStatement *current) { /*OMP*/
	if (isADeclBif(current->variant ())) return 0;
	if (current->variant () != CONTROL_END) {
       	    current->insertStmtAfter(*new SgStatement (OMP_BARRIER_DIR),*current->controlParent()); /*OMP*/
            //current->insertStmtAfter(*new SgStatement (OMP_END_MASTER_DIR),*current->controlParent()); /*OMP*/
        } else {
	        current->lexNext ()->insertStmtBefore(*new SgStatement (OMP_BARRIER_DIR),*current->lexNext ()->controlParent()); /*OMP*/
       		//current->lexNext ()->insertStmtBefore(*new SgStatement (OMP_END_MASTER_DIR),*current->lexNext ()->controlParent()); /*OMP*/
	}
	return 1;
}

void InsertSynchroBlock (SgStatement *begin, SgStatement *end) {
        SgStatement *last=end->lexPrev ();
	SgStatement *barrier = new SgStatement (OMP_BARRIER_DIR);
	SgStatement *master = new SgStatement (OMP_MASTER_DIR);
	barrier->addAttribute (OMP_MARK);
	master->addAttribute (OMP_MARK);
        if (begin->lexPrev ()->variant () != OMP_BARRIER_DIR) begin->insertStmtBefore(*barrier,*begin->controlParent());
	begin->insertStmtBefore(*master,*begin->controlParent());
	barrier = new SgStatement (OMP_BARRIER_DIR);
	master = new SgStatement (OMP_END_MASTER_DIR);
	barrier->addAttribute (OMP_MARK);
	master->addAttribute (OMP_MARK);
	if (end->lexNext () != NULL) {
	    if (end->lexNext ()->variant () != OMP_BARRIER_DIR) last->insertStmtAfter(*barrier,*last->controlParent());
	} else {
	    last->insertStmtAfter(*barrier,*last->controlParent());
	}
	last->insertStmtAfter(*master,*last->controlParent());
}

SgStatement * InsertCriticalBlock (SgStatement *begin, SgStatement *end) {
	SgStatement *critical = new SgStatement (OMP_CRITICAL_DIR);
	critical->setExpression (0,*new SgVarRefExp(new SgSymbol (VARIABLE_NAME,"dvmcritical")));
	critical->addAttribute (OMP_MARK);
	begin->insertStmtBefore(*critical,*begin->controlParent());
	critical = new SgStatement (OMP_END_CRITICAL_DIR);
	critical->setExpression (0,*new SgVarRefExp(new SgSymbol (VARIABLE_NAME,"dvmcritical")));
	critical->addAttribute (OMP_MARK);
	end->insertStmtBefore(*critical,*end->controlParent());
	return critical;
}

void MarkParameters (SgStatement *st) {
	SgExprListExp *list=isSgExprListExp(st->expr(0));
	if (list!= NULL) {
		for (int i=0;i<list->length (); i++) {
			SgExpression *exp=list->elem (i);
			if (exp->variant ()== CONST_REF) {
				exp->symbol ()->addAttribute (OMP_MARK);
			}
		}
	}
}

void AddOpenMPSynchro (SgStatement *func) {
  SgStatement *stmt = NULL;
  SgStatement *first = func->lexNext();
  SgStatement *last = func->lastNodeOfStmt();
  stmt_list *omp_list = NULL;
  omp_list = PushToStmtList (omp_list, func);
  int FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
  SgStatement * SynchroBlockBegin = NULL;
  for (stmt = first; stmt && (stmt != last); stmt = stmt->lexNext()) {
      AddOmpStmt (stmt);
  }
  for(stmt = first; stmt && (stmt != last); stmt = stmt->lexNext()) {
      if (stmt->variant () == OMP_ONETHREAD_DIR) {
            FromOneThread = 1;
            omp_list = PushToStmtList (omp_list, stmt);
            continue;
      }
	  if (stmt->variant () == PARAM_DECL) {
		  MarkParameters (stmt);
		  continue;
	  }
      if (isADeclBif(stmt->variant ())) continue;
      if (isOmpDir (stmt) || stmt->variant () == CONTROL_END || stmt->variant () == CONT_STAT) {
          switch (stmt->variant ()) {
                case OMP_END_PARALLEL_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_PARALLEL_DIR) {
						AddSharedClauseForDVMVariables (omp_list->st, stmt);
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP PARALLEL directive for this $OMP END PARALLEL directive %s", "", 701, stmt);
                    }
                    break;
                }
                case OMP_END_DO_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_DO_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP DO directive for this $OMP END DO directive %s", "", 702, stmt);
                    }
                    break;
                }
				case OMP_END_SECTIONS_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_SECTIONS_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP SECTIONS directive for this $OMP END SECTIONS directive %s", "", 703, stmt);
                    }
                    break;
                }
                case OMP_END_SINGLE_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_SINGLE_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP SINGLE directive for this $OMP END SINGLE directive %s", "", 704, stmt);
                    }
                    break;
                }
                case OMP_END_WORKSHARE_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_WORKSHARE_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP WORKSHARE directive for this $OMP END WORKSHARE directive %s", "", 705, stmt);
                    }
                    break;
                }
                case OMP_END_PARALLEL_DO_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_PARALLEL_DO_DIR) {
						AddSharedClauseForDVMVariables (omp_list->st, stmt);
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP PARALLEL DO directive for this $OMP END PARALLEL DO directive %s", "", 706, stmt);
                    }
                    break;
                }
                case OMP_END_PARALLEL_SECTIONS_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_PARALLEL_SECTIONS_DIR) {
						AddSharedClauseForDVMVariables (omp_list->st, stmt);
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP PARALLEL SECTIONS directive for this $OMP END PARALLEL SECTIONS directive %s", "", 707, stmt);
                    }
                    break;
                }
                 case OMP_END_PARALLEL_WORKSHARE_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_PARALLEL_WORKSHARE_DIR) {
						AddSharedClauseForDVMVariables (omp_list->st, stmt);
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP PARALLEL WORKSHARE directive for this $OMP END PARALLEL WORKSHARE directive %s", "", 708, stmt);
                    }
                    break;
                }
                case OMP_END_MASTER_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_MASTER_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP MASTER directive for this $OMP END MASTER directive %s", "", 709, stmt);
                    }
                    break;
                }
                case OMP_END_CRITICAL_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_CRITICAL_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP CRITICAL directive for this $OMP END CRITICAL directive %s", "", 710, stmt);
                    }
                    break;
                }
                case OMP_END_ORDERED_DIR: {
                    if (ValFromStmtList (omp_list) == OMP_ORDERED_DIR) {
                        omp_list = PopFromStmtList (omp_list);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                    } else {
                        Error("Can`t find $OMP ORDERED directive for this $OMP END ORDERED directive %s", "", 711, stmt);
                    }
                    break;
                }
                case OMP_PARALLEL_DIR:
                case OMP_DO_DIR: 
                case OMP_SECTIONS_DIR:
                case OMP_SINGLE_DIR:
                case OMP_WORKSHARE_DIR:
                case OMP_PARALLEL_DO_DIR:
                case OMP_PARALLEL_SECTIONS_DIR:
                case OMP_PARALLEL_WORKSHARE_DIR:
                case OMP_MASTER_DIR:
                case OMP_CRITICAL_DIR:
                case OMP_ORDERED_DIR: {
                        omp_list = PushToStmtList (omp_list, stmt);
                        FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                        break;
                }
                case CONT_STAT:
                case CONTROL_END: {                    
                    SgStatement *next =stmt->lexNext ();
                    if (next && (next->variant () == OMP_END_PARALLEL_DO_DIR || next->variant () == OMP_END_DO_DIR)) break;
                    SgStatement *cp =stmt->controlParent ();
                    if (cp && cp->variant () == FOR_NODE) {
                        SgStatement *prev = cp->lexPrev ();
                        if (prev) {
                            if (prev->variant () == OMP_DO_DIR) {
                                if (ValFromStmtList (omp_list) == OMP_DO_DIR) {
                                    omp_list = PopFromStmtList (omp_list);
                                    FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                                }
                                break;
                            }
                            if (prev->variant () == OMP_PARALLEL_DO_DIR) {
                                if (ValFromStmtList (omp_list) == OMP_PARALLEL_DO_DIR) {
 						            AddSharedClauseForDVMVariables (omp_list->st, stmt);
                                    omp_list = PopFromStmtList (omp_list);
                                    FromOneThread = isFromOneThread (ValFromStmtList (omp_list));
                                }
                                break;
                            }
                        }
                    }
                }
          }
      }
      if (stmt->numberOfAttributes(OMP_CRITICAL) != 0) {
        SgStatement *tmp=stmt;
        for (; tmp; tmp = tmp->lexNext ()) {
            if (tmp->numberOfAttributes(OMP_CRITICAL) == 0) break;
        }
        if (SynchroBlockBegin == NULL) stmt = InsertCriticalBlock (stmt, tmp);
        else stmt = tmp->lexPrev ();
        continue;
      }
      if ((stmt->numberOfAttributes(OMP_MARK) == 0) || (stmt->numberOfAttributes(OMP_CRITICAL) != 0)) {
         if ((SynchroBlockBegin != NULL) || (FromOneThread == 1)) continue;
         else {
            SynchroBlockBegin = stmt;
         }
      } else {
         if (SynchroBlockBegin != NULL) {
            InsertSynchroBlock (SynchroBlockBegin, stmt);
            SynchroBlockBegin = NULL;
         }
      }
  }
  if (SynchroBlockBegin != NULL) InsertSynchroBlock (SynchroBlockBegin, last);
}

SgExprListExp * FindDVMVariableRefsInExpr (SgExpression *expr, SgExprListExp *list)
{
  if (expr==NULL)
    return list;
  if (expr->variant() == VAR_REF)
    {
		SgSymbol *sym = expr->symbol ();
		if (sym->numberOfAttributes(OMP_MARK) == 0) {
			if (list != NULL) {
				if (!list->IsSymbolInExpression (*sym)) list->append (*expr);
			} else {					
				list = new SgExprListExp (*expr);
			}
		}
    }
  if (expr->variant() == ARRAY_REF)
    {
		SgSymbol *sym = expr->symbol ();
		if (sym->numberOfAttributes(OMP_MARK) == 0) {
			if (list != NULL) {			
				if (!list->IsSymbolInExpression (*sym)) list->append (*new SgArrayRefExp(*sym));
			} else {
				list = new SgExprListExp (*new SgArrayRefExp(*sym));
			}
		}
    }
  list = FindDVMVariableRefsInExpr(expr->lhs (),list);
  list = FindDVMVariableRefsInExpr(expr->rhs (),list);
  return list;
}

SgExprListExp * FindDVMVariableRefsInStmt (SgStatement *stmt, SgExprListExp *list)
{
  if (stmt==NULL)
    return list;
  list = FindDVMVariableRefsInExpr(stmt->expr (0),list);
  list = FindDVMVariableRefsInExpr(stmt->expr (1),list);
  list = FindDVMVariableRefsInExpr(stmt->expr (2),list);
  return list;
}

SgExprListExp * FindDVMVariableRefsInStmts (SgStatement *first, SgStatement *last)
{
  SgExprListExp *list = NULL;
  for (SgStatement * stmt=first; stmt && (stmt != last); stmt=stmt->lexNext ()) {
	  list = FindDVMVariableRefsInStmt (stmt, list);
  }
  return list;
}

void AddSharedClauseForDVMVariables (SgStatement *first, SgStatement *last)
{
  SgExprListExp *list = FindDVMVariableRefsInStmts (first->lexNext (), last);
  if (list!=NULL) {
	  switch (first->variant ()) {
         case OMP_PARALLEL_DIR:
         case OMP_PARALLEL_DO_DIR:
         case OMP_PARALLEL_SECTIONS_DIR:
         case OMP_PARALLEL_WORKSHARE_DIR:
			 if (first->expr (0)) {
				 SgExprListExp *ll = isSgExprListExp (first->expr (0));
				 if (ll) ll->append (* new SgExpression (OMP_SHARED, list,NULL,NULL,NULL));
			 } else {
                 first->setExpression (0, *new SgExprListExp (* new SgExpression (OMP_SHARED, list,NULL,NULL,NULL)));
			 }
	  }
  }
}


void TranslateFileOpenMPDVM(SgFile *f)
{
  SgStatement *func,*stat;
  //int i,numfun;
  SgStatement *end_of_unit; // last node (END or CONTAINS statement ) of program unit
  

// grab the first statement in the file.
  stat = f->firstStatement(); // file header 
  //numfun = f->numberOfFunctions(); //  number of functions
// function is program unit accept BLOCKDATA and MODULE (F90),i.e. 
// PROGRAM, SUBROUTINE, FUNCTION
  if(debug_fragment || perf_fragment) // is debugging or performance analizing regime specified ?
    BeginDebugFragment(0,NULL);// begin the fragment with number 0 (involving whole file(program) 
  //for(i = 0; i < numfun; i++) { 
  //   func = f -> functions(i);

  for (SgSymbol *sym=f->firstSymbol(); sym; sym=sym->next ()) {
	  sym->addAttribute (OMP_MARK);
  }
  for(stat=stat->lexNext(); stat; stat=end_of_unit->lexNext()) {
    if(stat->variant() == CONTROL_END) {  //end of procedure or module with CONTAINS statement  
      end_of_unit = stat;  
      continue;
    }

    if( stat->variant() == BLOCK_DATA){//BLOCK_DATA header 
      TransBlockData(stat,end_of_unit); //changing variant VAR_DECL with VAR_DECL_90
      continue;
    }
    // PROGRAM, SUBROUTINE, FUNCTION header
    func = stat; 
    cur_func = func;
    
        //scanning the Symbols Table of the function 
        //     ScanSymbTable(func->symbol(), (f->functions(i+1))->symbol());

     
    // translating the function
     if(only_debug)
        InsertDebugStat(func, end_of_unit);
     else {
        MarkAndReplaceOriginalStmt (func);
        TransFunc (func, end_of_unit);
        AddOpenMPSynchro (func);
     }
  }
}

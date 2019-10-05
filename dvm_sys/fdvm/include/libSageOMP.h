void InstrumentForOpenMPDebug (SgFile *f);
//OMP PRIVATE CLAUSE

class SgOMP_PRIVATE: public SgExpression{
  // OMP PRIVATE (expr1, expr2, ....)
  // variant == OMP_PRIVATE
public:
  inline SgOMP_PRIVATE(PTR_LLND ll);
  inline SgOMP_PRIVATE(SgExpression &paramList);
  inline ~SgOMP_PRIVATE();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_PRIVATE--inlines

inline SgOMP_PRIVATE::SgOMP_PRIVATE(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_PRIVATE::SgOMP_PRIVATE(SgExpression &paramList):SgExpression(OMP_PRIVATE)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_PRIVATE::~SgOMP_PRIVATE()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_PRIVATE::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_PRIVATE::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_PRIVATE::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_PRIVATE::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }

//OMP SHARED CLAUSE

class SgOMP_SHARED: public SgExpression{
  // OMP SHARED (expr1, expr2, ....)
  // variant == OMP_SHARED
public:
  inline SgOMP_SHARED(PTR_LLND ll);
  inline SgOMP_SHARED(SgExpression &paramList);
  inline ~SgOMP_SHARED();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_SHARED--inlines

inline SgOMP_SHARED::SgOMP_SHARED(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_SHARED::SgOMP_SHARED(SgExpression &paramList):SgExpression(OMP_SHARED)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_SHARED::~SgOMP_SHARED()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_SHARED::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_SHARED::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_SHARED::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_SHARED::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }

//OMP FIRSTPRIVATE CLAUSE

class SgOMP_FIRSTPRIVATE: public SgExpression{
  // OMP FIRSTPRIVATE (expr1, expr2, ....)
  // variant == OMP_FIRSTPRIVATE
public:
  inline SgOMP_FIRSTPRIVATE(PTR_LLND ll);
  inline SgOMP_FIRSTPRIVATE(SgExpression &paramList);
  inline ~SgOMP_FIRSTPRIVATE();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_FIRSTPRIVATE--inlines

inline SgOMP_FIRSTPRIVATE::SgOMP_FIRSTPRIVATE(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_FIRSTPRIVATE::SgOMP_FIRSTPRIVATE(SgExpression &paramList):SgExpression(OMP_FIRSTPRIVATE)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_FIRSTPRIVATE::~SgOMP_FIRSTPRIVATE()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_FIRSTPRIVATE::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_FIRSTPRIVATE::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_FIRSTPRIVATE::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_FIRSTPRIVATE::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }

//OMP LASTPRIVATE CLAUSE

class SgOMP_LASTPRIVATE: public SgExpression{
  // OMP LASTPRIVATE (expr1, expr2, ....)
  // variant == OMP_LASTPRIVATE
public:
  inline SgOMP_LASTPRIVATE(PTR_LLND ll);
  inline SgOMP_LASTPRIVATE(SgExpression &paramList);
  inline ~SgOMP_LASTPRIVATE();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_LASTPRIVATE--inlines

inline SgOMP_LASTPRIVATE::SgOMP_LASTPRIVATE(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_LASTPRIVATE::SgOMP_LASTPRIVATE(SgExpression &paramList):SgExpression(OMP_LASTPRIVATE)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_LASTPRIVATE::~SgOMP_LASTPRIVATE()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_LASTPRIVATE::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_LASTPRIVATE::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_LASTPRIVATE::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_LASTPRIVATE::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }

//OMP THREADPRIVATE CLAUSE

class SgOMP_THREADPRIVATE: public SgExpression{
  // OMP THREADPRIVATE (expr1, expr2, ....)
  // variant == OMP_THREADPRIVATE
public:
  inline SgOMP_THREADPRIVATE(PTR_LLND ll);
  inline SgOMP_THREADPRIVATE(SgExpression &paramList);
  inline ~SgOMP_THREADPRIVATE();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_THREADPRIVATE--inlines

inline SgOMP_THREADPRIVATE::SgOMP_THREADPRIVATE(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_THREADPRIVATE::SgOMP_THREADPRIVATE(SgExpression &paramList):SgExpression(OMP_THREADPRIVATE)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_THREADPRIVATE::~SgOMP_THREADPRIVATE()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_THREADPRIVATE::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_THREADPRIVATE::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_THREADPRIVATE::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_THREADPRIVATE::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }


//OMP COPYIN CLAUSE

class SgOMP_COPYIN: public SgExpression{
  // OMP COPYIN (expr1, expr2, ....)
  // variant == OMP_COPYIN
public:
  inline SgOMP_COPYIN(PTR_LLND ll);
  inline SgOMP_COPYIN(SgExpression &paramList);
  inline ~SgOMP_COPYIN();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_COPYIN--inlines

inline SgOMP_COPYIN::SgOMP_COPYIN(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_COPYIN::SgOMP_COPYIN(SgExpression &paramList):SgExpression(OMP_COPYIN)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_COPYIN::~SgOMP_COPYIN()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_COPYIN::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_COPYIN::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_COPYIN::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_COPYIN::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }

//OMP COPYPRIVATE CLAUSE

class SgOMP_COPYPRIVATE: public SgExpression{
  // OMP COPYPRIVATE (expr1, expr2, ....)
  // variant == OMP_COPYPRIVATE
public:
  inline SgOMP_COPYPRIVATE(PTR_LLND ll);
  inline SgOMP_COPYPRIVATE(SgExpression &paramList);
  inline ~SgOMP_COPYPRIVATE();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_COPYPRIVATE--inlines

inline SgOMP_COPYPRIVATE::SgOMP_COPYPRIVATE(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_COPYPRIVATE::SgOMP_COPYPRIVATE(SgExpression &paramList):SgExpression(OMP_COPYPRIVATE)
{
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgOMP_COPYPRIVATE::~SgOMP_COPYPRIVATE()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_COPYPRIVATE::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_COPYPRIVATE::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_COPYPRIVATE::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_COPYPRIVATE::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }


//OMP DEFAULT CLAUSE
class SgOMP_DEFAULT: public SgExpression{
  // OMP DEFAULT ( PRIVATE | SHARED | NONE )
  // variant == OMP_DEFAULT
public:
  inline SgOMP_DEFAULT(PTR_LLND ll);
  inline SgOMP_DEFAULT(char *raspis);
  inline ~SgOMP_DEFAULT();
};

// SgOMP_DEFAULT--inlines

inline SgOMP_DEFAULT::SgOMP_DEFAULT(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_DEFAULT::SgOMP_DEFAULT(char *raspis):SgExpression(OMP_DEFAULT)
{
  SgKeywordValExp	*x=new SgKeywordValExp(raspis);
  NODE_OPERAND0(thellnd) = x->thellnd;
}

inline SgOMP_DEFAULT::~SgOMP_DEFAULT()
{ RemoveFromTableLlnd((void *) this); }


//OMP ORDERED CLAUSE
class SgOMP_ORDERED: public SgExpression{
  // OMP ORDERED ( PRIVATE | SHARED | NONE )
  // variant == OMP_ORDERED
public:
  inline SgOMP_ORDERED(PTR_LLND ll);
  inline SgOMP_ORDERED(char *name);
  inline ~SgOMP_ORDERED();
};

// SgOMP_ORDERED--inlines

inline SgOMP_ORDERED::SgOMP_ORDERED(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_ORDERED::SgOMP_ORDERED(char *name):SgExpression(OMP_ORDERED)
{
  SgKeywordValExp *x=new SgKeywordValExp("ORDERED");
  NODE_OPERAND0(thellnd) = x->thellnd;
}

inline SgOMP_ORDERED::~SgOMP_ORDERED()
{ RemoveFromTableLlnd((void *) this); }


//OMP SCHEDULE CLAUSE
class SgOMP_SCHEDULE: public SgExpression{
  // OMP SCHEDULE ( type , chunk )
  // variant == OMP_SCHEDULE
public:
  inline SgOMP_SCHEDULE(PTR_LLND ll);
  inline SgOMP_SCHEDULE(char *name);
  inline SgOMP_SCHEDULE(char *name, SgExpression &chunk);
  inline SgOMP_SCHEDULE(const char *name);
  inline SgOMP_SCHEDULE(const char *name, SgExpression &chunk);
  inline ~SgOMP_SCHEDULE();
};

// SgOMP_SCHEDULE--inlines

inline SgOMP_SCHEDULE::SgOMP_SCHEDULE(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_SCHEDULE::SgOMP_SCHEDULE(char *name):SgExpression(OMP_SCHEDULE)
{
  SgKeywordValExp *x=new SgKeywordValExp(name);
  NODE_OPERAND0(thellnd) = x->thellnd;
}

inline SgOMP_SCHEDULE::SgOMP_SCHEDULE(const char *name) :SgExpression(OMP_SCHEDULE)
{
    SgKeywordValExp *x = new SgKeywordValExp(name);
    NODE_OPERAND0(thellnd) = x->thellnd;
}

inline SgOMP_SCHEDULE::SgOMP_SCHEDULE(char *name,SgExpression &chunk):SgExpression(OMP_SCHEDULE)
{
  SgKeywordValExp *x=new SgKeywordValExp(name);
  NODE_OPERAND0(thellnd) = x->thellnd;
  NODE_OPERAND1(thellnd) = chunk.thellnd;
}

inline SgOMP_SCHEDULE::SgOMP_SCHEDULE(const char *name, SgExpression &chunk) :SgExpression(OMP_SCHEDULE)
{
    SgKeywordValExp *x = new SgKeywordValExp(name);
    NODE_OPERAND0(thellnd) = x->thellnd;
    NODE_OPERAND1(thellnd) = chunk.thellnd;
}

inline SgOMP_SCHEDULE::~SgOMP_SCHEDULE()
{ RemoveFromTableLlnd((void *) this); }


//OMP REDUCTION CLAUSE

class SgOMP_REDUCTION: public SgExpression{
  // OMP REDUCTION ({operator|intrinsic_procedure_name} : expr1, expr2, ....)
  // variant == OMP_REDUCTION
public:
  inline SgOMP_REDUCTION(PTR_LLND ll);
  inline SgOMP_REDUCTION(SgExpression &redop,SgExpression &redList);
  inline SgOMP_REDUCTION(char *redname);
  inline SgOMP_REDUCTION(char *redname,SgExpression &redList);
  inline SgOMP_REDUCTION(const char *redname);
  inline SgOMP_REDUCTION(const char *redname, SgExpression &redList);
  inline ~SgOMP_REDUCTION();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgOMP_REDUCTION--inlines

inline SgOMP_REDUCTION::SgOMP_REDUCTION(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_REDUCTION::SgOMP_REDUCTION(SgExpression &redOp,SgExpression &redList):SgExpression(OMP_REDUCTION)
{
  NODE_OPERAND0(thellnd) = redList.thellnd;
  NODE_OPERAND1(thellnd) = redOp.thellnd;

}
inline SgOMP_REDUCTION::SgOMP_REDUCTION(char *name):SgExpression(OMP_REDUCTION)
{
  SgKeywordValExp *x=new SgKeywordValExp(name);
  NODE_OPERAND1(thellnd) = x->thellnd;
}
inline SgOMP_REDUCTION::SgOMP_REDUCTION(char *name,SgExpression &redList):SgExpression(OMP_REDUCTION)
{
  SgKeywordValExp *x=new SgKeywordValExp(name);
  NODE_OPERAND0(thellnd) = redList.thellnd;
  NODE_OPERAND1(thellnd) = x->thellnd;
}
inline SgOMP_REDUCTION::SgOMP_REDUCTION(const char *name) :SgExpression(OMP_REDUCTION)
{
    SgKeywordValExp *x = new SgKeywordValExp(name);
    NODE_OPERAND1(thellnd) = x->thellnd;
}
inline SgOMP_REDUCTION::SgOMP_REDUCTION(const char *name, SgExpression &redList) : SgExpression(OMP_REDUCTION)
{
    SgKeywordValExp *x = new SgKeywordValExp(name);
    NODE_OPERAND0(thellnd) = redList.thellnd;
    NODE_OPERAND1(thellnd) = x->thellnd;
}

inline SgOMP_REDUCTION::~SgOMP_REDUCTION()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgOMP_REDUCTION::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgOMP_REDUCTION::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgOMP_REDUCTION::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgOMP_REDUCTION::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }

//OMP IF CLAUSE
class SgOMP_IF: public SgExpression{
  // OMP IF ( scalar_logical_expression )
  // variant == OMP_IF
public:
  inline SgOMP_IF(PTR_LLND ll);
  inline SgOMP_IF(SgExpression &cond);
  inline ~SgOMP_IF();
};

// SgOMP_IF--inlines

inline SgOMP_IF::SgOMP_IF(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_IF::SgOMP_IF(SgExpression &cond):SgExpression(OMP_IF)
{
  SgKeywordValExp	*x=new SgKeywordValExp("IF");
  NODE_OPERAND0(thellnd) = x->thellnd;
  NODE_OPERAND1(thellnd) =cond.thellnd;
}

inline SgOMP_IF::~SgOMP_IF()
{ RemoveFromTableLlnd((void *) this); }

//OMP NUM_THREADS CLAUSE
class SgOMP_NUM_THREADS: public SgExpression{
  // OMP NUM_THREADS ( scalar_integer_expression )
  // variant == OMP_NUM_THREADS
public:
  inline SgOMP_NUM_THREADS(PTR_LLND ll);
  inline SgOMP_NUM_THREADS(SgExpression &cond);
  inline ~SgOMP_NUM_THREADS();
};

// SgOMP_NUM_THREADS--inlines

inline SgOMP_NUM_THREADS::SgOMP_NUM_THREADS(PTR_LLND ll):SgExpression(ll)
{}

inline SgOMP_NUM_THREADS::SgOMP_NUM_THREADS(SgExpression &cond):SgExpression(OMP_NUM_THREADS)
{
  SgKeywordValExp	*x=new SgKeywordValExp("NUM_THREADS");
  NODE_OPERAND0(thellnd) = x->thellnd;
  NODE_OPERAND1(thellnd) =cond.thellnd;
}

inline SgOMP_NUM_THREADS::~SgOMP_NUM_THREADS()
{ RemoveFromTableLlnd((void *) this); }


// !!!! OPENMP Statement OPENMP !!!!

class SgOMPStmt: public SgStatement{
  // the Openmp Fortran  statement
  // variant == OPENMP_STAT
public:
  inline SgOMPStmt(int variant);
  inline SgOMPStmt(int variant, SgExpression &e1);
  inline SgOMPStmt(int variant, SgExpression &e1,
                     SgExpression &e2);
  inline ~SgOMPStmt();
  inline void addClause0(SgExpression &clause);
  inline void addClause1(SgExpression &clause);
  inline void addClause2(SgExpression &clause);
 };

// SgOMPStmt--inlines
inline SgOMPStmt::SgOMPStmt(int variant):SgStatement(variant)
{ }

inline SgOMPStmt::SgOMPStmt(int variant, SgExpression &e1):SgStatement(variant)
{ BIF_LL1(thebif) = e1.thellnd; }


inline SgOMPStmt::SgOMPStmt(int variant, SgExpression &e1, SgExpression &e2)
                   :SgStatement(variant)
{
  BIF_LL1(thebif) = e1.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e2.thellnd);
}

inline SgOMPStmt::~SgOMPStmt()
{ RemoveFromTableBfnd((void *) this); }


inline void SgOMPStmt::addClause0(SgExpression &clause)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), clause.thellnd); }

inline void SgOMPStmt::addClause1(SgExpression &clause)
{ BIF_LL2(thebif) = addToExprList(BIF_LL2(thebif), clause.thellnd); }

inline void SgOMPStmt::addClause2(SgExpression &clause)
{ BIF_LL3(thebif) = addToExprList(BIF_LL3(thebif), clause.thellnd); }




class SgRecordStmt: public SgClassStmt{
  // type type_name
  //    fields
  // end type
  // variant == STRUCT_DECL
public:
  // consider like a class.
  inline SgRecordStmt();
  inline SgRecordStmt(SgSymbol &type,SgStatement &body);
  inline ~SgRecordStmt();
  
};

// SgRecordStmt--inlines

inline SgRecordStmt::SgRecordStmt():SgClassStmt(STRUCT_DECL)
{}

inline SgRecordStmt::SgRecordStmt(SgSymbol &type,SgStatement &body):SgClassStmt(type)
{
  if (CurrentProject->Fortranlanguage())
    {
	  BIF_SYMB(thebif) = type.thesymb;
	  BIF_CODE(thebif) = RECORD_DECL;
	  insertBfndListIn(body.thebif,thebif,thebif);
	  addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}

inline SgRecordStmt::~SgRecordStmt()
{ RemoveFromTableBfnd((void *) this); }


class SgFunctionCallStmt: public SgStatement{
  // function_call(expr1, expr2, ....)
  // or
  // ALLOCATE(expr1, expr2, ....)
  // variant == FUNC_CALL

public:
  inline SgFunctionCallStmt(SgExpression &paramList);
  inline SgFunctionCallStmt(SgSymbol &fun, SgExpression &paramList);
  inline ~SgFunctionCallStmt();
  inline SgSymbol *funName();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

// SgFunctionCallStmt--inlines


inline SgFunctionCallStmt::SgFunctionCallStmt(SgSymbol &fun, SgExpression &paramList):SgStatement(FUNC_STAT)
{
  BIF_SYMB(thebif)= fun.thesymb;
  BIF_CODE(thebif)= FUNC_STAT;
  BIF_LL1(thebif) = paramList.thellnd;
}

inline SgFunctionCallStmt::SgFunctionCallStmt(SgExpression &paramList):SgStatement(FUNC_STAT)
{
  SgSymbol fun(FUNCTION_NAME,"allocate");
  BIF_SYMB(thebif)= fun.thesymb;
  BIF_CODE(thebif)= FUNC_STAT;
  BIF_LL1(thebif) = paramList.thellnd;

}
inline SgFunctionCallStmt::~SgFunctionCallStmt()
{ RemoveFromTableLlnd((void *) this); }

inline SgSymbol *SgFunctionCallStmt::funName()
{ return SymbMapping(BIF_SYMB(thebif)); }

inline SgExpression * SgFunctionCallStmt::args()
{ return LlndMapping(BIF_LL1(thebif)); }

inline  int SgFunctionCallStmt::numberOfArgs()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgExpression * SgFunctionCallStmt::arg(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif),i));  }

inline void SgFunctionCallStmt::addArg(SgExpression &arg)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd); }


class SgF90PointerAssignStmt: public SgExecutableStatement{
  // Fortran assignment Statment
  // variant == POINTER_ASSIGN_STAT
public:
  inline SgF90PointerAssignStmt(int variant);
  inline SgF90PointerAssignStmt(SgExpression &lhs, SgExpression &rhs);
  inline SgExpression *lhs();  // the left hand side
  inline SgExpression *rhs();  // the right hand side
  inline void replaceLhs(SgExpression &e); // replace lhs with e
  inline void replaceRhs(SgExpression &e); // replace rhs with e
};

// SgF90PointerAssignStmt--inlines

inline SgF90PointerAssignStmt::SgF90PointerAssignStmt(int variant):SgExecutableStatement(variant)
{}
inline SgF90PointerAssignStmt::SgF90PointerAssignStmt(SgExpression &lhs, SgExpression &rhs):SgExecutableStatement(POINTER_ASSIGN_STAT)
{
  BIF_LL1(thebif) = lhs.thellnd;
  BIF_LL2(thebif) = rhs.thellnd;
}

inline SgExpression * SgF90PointerAssignStmt::lhs()
{ return LlndMapping(BIF_LL1(thebif)); }

// the right hand side
inline SgExpression * SgF90PointerAssignStmt::rhs()
{ return LlndMapping(BIF_LL2(thebif)); }

// replace lhs with e
inline void SgF90PointerAssignStmt::replaceLhs(SgExpression &e)
{ BIF_LL1(thebif) = e.thellnd; }

// replace rhs with e
inline void SgF90PointerAssignStmt::replaceRhs(SgExpression &e)
{ BIF_LL2(thebif) = e.thellnd; }
///////////////////////////NEW LIST STRUCTURES ////////////////////////

extern SgStatement *cur_func;  // current function 
extern SgFile *current_file;    //current file

struct pointer_list {
       pointer_list *next;
	   int dim;
       SgSymbol *symb;
};
struct type_list {
       type_list *next;
	   SgType *type;
	   SgType *base;
};
///////////////////////////OPENMP FUNCTIONS INTERFACE////////////////////////
void TranslateFileDVMOpenMP(SgFile *f);
SgSymbol* GenerateNewVariableName(const char *name);
SgSymbol* GenerateTypeName(int dim);
void GenerateOMPRoutinesName(void);
SgExpression * TranslateOpenMPReduction(SgExpression *red) ;
SgExpression * TranslateOpenMPPrivate(SgExpression *pri,SgExpression *on) ;
SgExpression * AddOpenMPPrivate(SgExpression *private_clause,SgExprListExp *explist) ;
SgSymbol * DeclarePointerTypeRecord(SgStatement *stmt,int numdim,SgType *type);
void DeclarePointerType(SgStatement *stmt,SgSymbol *sym,int numdim,SgType *type);
pointer_list  *AddToPointerList ( pointer_list *ls, SgSymbol *s,int dim);
int FindInPointerList ( pointer_list *ls, SgSymbol *s);
pointer_list* FindPointerInPointerList ( pointer_list *ls, SgSymbol *s);
type_list  *AddToTypeList ( type_list *ls, SgType *t, SgType *base);
type_list* FindTypeInTypeList ( type_list *ls, SgType *t, int dim);
void TransFuncOpenMP(SgStatement *func) ;
//char* filterDVM(char *s);
///////////////////////////OPENMP FUNCTIONS DEFINITION////////////////////////
void TranslateFileDVMOpenMP(SgFile *f)
{
  SgStatement *func;
  SgExpression *var_ref;
  int i,numfun;
  numfun = f->numberOfFunctions(); //  number of functions
// function is program unit accept BLOCKDATA and MODULE (F90),i.e. 
// PROGRAM, SUBROUTINE, FUNCTION
  for(var_ref=f->firstExpression();var_ref;var_ref=var_ref->nextInExprTable())
  {
	if ((var_ref->variant()==ARRAY_REF)&&((var_ref->symbol()->thesymb->attr & HEAP_BIT)||!strcmp("heap",var_ref->symbol()->identifier())))
		{
			SgExpression *arr_ref=NULL;		
			arr_ref=var_ref->lhs();
			if (arr_ref&&isSgExprListExp(arr_ref)&&(isSgArrayRefExp(arr_ref->lhs())||isSgFunctionCallExp(arr_ref->lhs())||isSgVarRefExp(arr_ref->lhs())))
				{
				SgSymbol *s;
				SgExpression *lhs,*tmp;
				tmp=arr_ref->lhs();	
				if (tmp&&tmp->symbol())
					{
					if(tmp->lhs())
						{
						lhs=new SgExpression(ARRAY_REF,tmp->lhs()->copyPtr(),NULL,tmp->symbol());
						//lhs=tmp->lhs()->copyPtr();
						//var_ref->setSymbol(*tmp->symbol());
						s=new SgSymbol(FIELD_NAME,"PTR");
						var_ref->setLhs(*lhs);
						NODE_CODE(var_ref->thellnd)=RECORD_REF;
						//var_ref->setVariant(RECORD_REF);
						var_ref->setRhs(*(new SgVarRefExp(*s)));
						}
					else 
						{
						var_ref->setSymbol(*tmp->symbol());
						var_ref->setLhs(NULL);
						}
					}
				else printf("error in string statement - line  \n");
				}
		}
  }
  for(i = 0; i < numfun; i++) { 
     func = f -> functions(i);
     cur_func = func;
     TransFuncOpenMP(func);
  }
}

SgSymbol* GenerateNewVariableName(const char *name)
{
	int i=0,ok=1;
	char str[80];
	SgSymbol *tmp;
	while (ok)
	{
		if (i) sprintf(str,"%s%i",name,i);
		else sprintf(str,"%s",name);
		for (tmp = current_file->firstSymbol(); tmp ; tmp = tmp->next())
		{
			if (!strcmp(tmp->identifier(),str)) 
			{
				i++;
				break;
			}
		}
	if (!tmp) ok=0;
	}
	return (new SgSymbol(VARIABLE_NAME,str));
}

void GenerateOMPRoutinesName(void)
{
    int i = 0;// ok = 1;
	const char *str[]={"omp_get_num_threads","omp_get_thread_num"};
	char name[80];
	SgSymbol *tmp;
	for(i=0;i<2;i++) 
	{
		sprintf(name,"%s",str[i]);
		for (tmp = current_file->firstSymbol(); tmp ; tmp = tmp->next())
		{
			if ((tmp->scope()==cur_func)&&(!strcmp(tmp->identifier(),name))) 
			{
			    break;
			}
		}
		if (!tmp)
		{
		tmp=new SgSymbol(VARIABLE_NAME,str[i]);
		tmp->declareTheSymbol(*cur_func);
		}
	}
}

SgSymbol* GenerateTypeName(int dim)
{
	SgSymbol *tmp;
	int i=0,ok=1;
	char str[80];
	while (ok)
	{
		if (i) sprintf(str,"ptr_type%i%i",dim,i);
		else sprintf(str,"ptr_type%i",dim);
		for (tmp = current_file->firstSymbol(); tmp ; tmp = tmp->next())
		{
			if (!strcmp(tmp->identifier(),str)) 
			{
				i++;
				break;
			}
		}
	if (!tmp) ok=0;
	}
	return (new SgSymbol(VARIABLE_NAME,str));
}



SgExpression * TranslateOpenMPPrivate(SgExpression *private_clause,SgExpression *on_clause) 
{
    SgExprListExp *explist = NULL, *prilist = NULL;
    int i, length = 0;
	if (private_clause!=NULL&&private_clause->lhs()!=NULL) 
		{	prilist=new SgExprListExp(private_clause->lhs()->thellnd);
			if (on_clause->lhs()!=NULL) explist=new SgExprListExp(on_clause->thellnd);
			length=explist->length();
			for (i=0;i<length;i++)
				{
					prilist->append(*explist->elem(i));
				}	
		return (new SgOMP_PRIVATE(*prilist)); 
		}
	else return (new SgOMP_PRIVATE(*on_clause)); 
}

SgExpression * AddOpenMPPrivate(SgExpression *private_clause,SgExprListExp *explist) 
{
	SgExprListExp *prilist;
    int i, length = 0;
	if (private_clause!=NULL&&private_clause->lhs()!=NULL) 
		{	prilist=new SgExprListExp(private_clause->lhs()->thellnd);
			if (explist!=NULL) 	length=explist->length();
			for (i=0;i<length;i++)
				{
					prilist->append(*explist->elem(i));
				}	
		return (new SgOMP_PRIVATE(*prilist)); 
		}
	else return (new SgOMP_PRIVATE(*explist)); 
}

SgExpression * TranslateOpenMPReduction(SgExpression *reduction_clause) 
{
	SgExprListExp *explist = NULL, *OpenMPReductions;
	SgExpression *clause;
	SgExprListExp *red_max,*red_min,*red_sum,*red_product;
	SgExprListExp *red_and,*red_eqv,*red_neqv;
	SgExprListExp *red_or;
	int i,length;
	red_max=red_min=red_sum=red_product=red_or=red_and=red_eqv=red_neqv=NULL;
	OpenMPReductions=NULL;
	if (reduction_clause->lhs()!=NULL) explist=new SgExprListExp(reduction_clause->lhs()->thellnd);
	length=explist->length();
	for (i=0;i<length;i++)
		{
			clause=explist->elem(i);
			switch (clause->variant())
				{
				case ARRAY_OP:
					{
						if ((clause->lhs()!=NULL)&&(clause->rhs()!=NULL))
						{
							if (clause->lhs()->variant()==KEYWORD_VAL)
								{
								char *reduction_name=NODE_STRING_POINTER(clause->lhs()->thellnd);
								if (!strcmp(reduction_name,"max"))
									{
									 if (red_max!=NULL) red_max->append(*clause->rhs());
									 else red_max=new SgExprListExp(*clause->rhs());
								 	 continue;
									}
								if (!strcmp(reduction_name,"min"))
									{
									 if (red_min!=NULL) red_min->append(*clause->rhs());  
									 else red_min=new SgExprListExp(*clause->rhs());
									 continue;
									}
								if (!strcmp(reduction_name,"sum"))
									{
									 if (red_sum!=NULL) red_sum->append(*clause->rhs());  
									 else red_sum=new SgExprListExp(*clause->rhs());
									 continue;
									}
								if (!strcmp(reduction_name,"product"))
									{
									 if (red_product!=NULL) red_product->append(*clause->rhs());  
									 else red_product=new SgExprListExp(*clause->rhs());
									 continue;
									}

								if (!strcmp(reduction_name,"or"))
									{
									 if (red_or!=NULL) red_or->append(*clause->rhs());  
									 else red_or=new SgExprListExp(*clause->rhs());
			 						 continue;
									}
								if (!strcmp(reduction_name,"and"))
									{
									 if (red_and!=NULL) red_and->append(*clause->rhs());  
 								     else red_and=new SgExprListExp(*clause->rhs());
									 continue;
									}
								if (!strcmp(reduction_name,"eqv"))
									{
									 if (red_eqv!=NULL) red_eqv->append(*clause->rhs());  
 								     else red_eqv=new SgExprListExp(*clause->rhs());
									 continue;
									}
								if (!strcmp(reduction_name,"neqv"))
									{
									 if (red_neqv!=NULL) red_neqv->append(*clause->rhs());  
 								     else red_neqv=new SgExprListExp(*clause->rhs());
									 continue;
									}
								if (!strcmp(reduction_name,"maxloc"))
									{
									return NULL;
									}
								if (!strcmp(reduction_name,"minloc"))
									{
									return NULL;
									}
							}
									
						}
						break;
					}

				}
		}
	//OpenMPReductions=new SgExprListExp();
	if (red_max!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION("max",*red_max)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION("max",*red_max)));
		}
	if (red_min!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION("min",*red_min)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION("min",*red_min)));
		}
	if (red_sum!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION("+",*red_sum)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION("+",*red_sum)));
		}
	if (red_product!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION("*",*red_product)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION("*",*red_product)));
		}
	if (red_eqv!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION(".eq.",*red_eqv)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION(".eq.",*red_eqv)));
		}
	if (red_neqv!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION(".neqv.",*red_neqv)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION(".neqv.",*red_neqv)));
		}
	if (red_or!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION(".or.",*red_or)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION(".or.",*red_or)));
		}
	if (red_and!=NULL) 
		{
		if (!OpenMPReductions) OpenMPReductions=new SgExprListExp(*(new SgOMP_REDUCTION(".and.",*red_and)));
		else OpenMPReductions->append(*(new SgOMP_REDUCTION(".and.",*red_and)));
		}
	return OpenMPReductions;
}

SgExpression *GenerateArrayRefDimList(int length,SgSymbol *arr_name,SgExpression *sub)
{
	int i,subpos,j;
	SgExprListExp *expr,*ret;
	expr=ret=NULL;
	if ((sub!=NULL)&&(expr=isSgExprListExp(sub)))
		{
		for(i=0;i<expr->length();i++)
			{
			if (isSgValueExp(expr->elem(i))) break;
			}
		subpos=i;
		for(j=1;j<=length;j++)
			{
			SgExprListExp *tmp=NULL;
			for(i=0;i<expr->length();i++)
				{
				if (i==subpos) 
					{
					if(tmp) tmp->append(*(new SgValueExp(j)));
					else tmp=new SgExprListExp(*(new SgValueExp(j)));
					}
				else
					{
					if(tmp) tmp->append(*expr->elem(i));
					else tmp=new SgExprListExp(*expr->elem(i));
					}
				}
			if (ret) ret->append(* (new SgArrayRefExp(*arr_name,*tmp)));
			else ret=new SgExprListExp(* (new SgArrayRefExp(*arr_name,*tmp)));
			}

		}
	else 
		{
		for(j=1;j<=length;j++)
			{
			//if(tmp) tmp->append(*(new SgValueExp(j)));
			//else tmp=new SgExprListExp(*(new SgValueExp(j)));
			//if (ret) ret->append(SgArrayRefExp(*arr_name,*tmp));
			//else ret=new SgExprListExp(SgArrayRefExp(*arr_name,*tmp));
			if (ret != NULL)
                ret->append(* (new SgArrayRefExp(*arr_name,*(new SgExprListExp(*(new SgValueExp(j)))))));
			else 
                ret=new SgExprListExp(* (new SgArrayRefExp(*arr_name,*(new SgExprListExp(*(new SgValueExp(j)))))));
			
			}
		}
return ret;
}

SgStatement *NextStmtAfterLoopStmt(SgStatement *st)
{
	SgForStmt *for_loop=NULL;
	if ( (for_loop=isSgForStmt(st)) != 0)
		{
		if (for_loop->isEnddoLoop())
			{
			return st->lastNodeOfStmt();
			}
		else
			{
			SgLabel *label=NULL;
			SgStatement *ret=NULL;
			if ( (label=for_loop->endOfLoop()) != 0)
				{
				ret=st;
				while (ret)
					{
					ret=ret->lexNext();
					if (ret->label()&&(ret->label()->id()==label->id()))
						return ret;
					} 
					return NULL;
				}
			else return NULL;
			}
		}
	else return NULL;
}
void TransFuncOpenMP(SgStatement *func) {
  SgStatement *stmt,*last, *first;
  pointer_list *pointer=NULL;
  symb_list *heaps=NULL;
  stmt_list *stmt_to_delete = NULL;
  first = func->lexNext();
  last = func->lastNodeOfStmt();
  if(!(last->variant() == CONTROL_END))
     printf(" END Statement is absent\n");
  if ((func->variant()==FUNC_HEDR)&&func->symbol()&&(!strcmp(func->symbol()->identifier(),"allocate")))
	{
	  //SgSymbol *s;
	  SgSymbol old=func->symbol()->copyLevel1();
	  //s=GenerateNewVariableName("user_allocate");  	    
	  //if (func->IsSymbolReferenced(*func->symbol()))
	  func->replaceSymbBySymb(*func->symbol(),*GenerateNewVariableName("user_allocate"));
	  SYMB_FUNC_PARAM(func->symbol()->thesymb)=SYMB_FUNC_PARAM(old.thesymb);
	}
  for (stmt = first; stmt && (stmt != last); stmt = stmt->lexNext())
  {
	if (isSgForStmt(stmt)) convertToEnddoLoop(stmt->thebif); 
  }
  for (stmt = first; stmt && (stmt != last); stmt = stmt->lexNext())
  {
    //printf("stmt line :%i\n",stmt->lineNumber());
    if (isSgExecutableStatement(stmt))
	// is Fortran specification statement
	// isSgExecutableStatement: 
	//               FALSE  -  for specification statement of Fortan 90
	//               TRUE   -  for executable statement of Fortan 90 and
	//                         all directives of F-DVM 
      {
		if(stmt->variant()==DVM_HEAP_DIR)
			{
				SgExprListExp *heap;
				if (isSgExprListExp(stmt->expr(0)))
					{
					int i;
					heap=isSgExprListExp(stmt->expr(0));
					for(i=0;i<heap->length();i++)
						heaps=AddToSymbList(heaps,heap->elem(i)->symbol());
					}
				else printf("error in string statement - line  %i\n",stmt->lineNumber());
			}
		if(stmt->variant()==DVM_POINTER_DIR) 
			{
				SgExprListExp *vars,*dim;
				SgTypeExp *type;
				int i;
				if (isSgExprListExp(stmt->expr(0))&&isSgExprListExp(stmt->expr(1))&&isSgTypeExp(stmt->expr(2)))
					{
					vars=isSgExprListExp(stmt->expr(0));
					dim=isSgExprListExp(stmt->expr(1));
					type=isSgTypeExp(stmt->expr(2));
					for(i=0;i<vars->length();i++)
						{
						if (vars->elem(i)->symbol())
							{
							SgSymbol *s;
							s=new SgSymbol(VARIABLE_NAME,vars->elem(i)->symbol()->identifier());
							s->setType(*type->type());
							pointer=AddToPointerList(pointer,s,dim->length());
							}
						}
					}
			}
		if(stmt->variant()==ASSIGN_STAT) 
			{
			SgExpression *left;
			SgFunctionCallExp *right;
			left=NULL;
			right=NULL;
			left=stmt->expr(0);
			SgSymbol ptr(VARIABLE_NAME,"PTR");
			ptr.setType(* (new SgType(T_ARRAY)));
			if (left&&stmt->expr(1)&&(right=isSgFunctionCallExp(stmt->expr(1))))
				{
				if (!strcmp(right->funName()->identifier(),"allocate"))
					{
					SgExprListExp *expr_list=NULL;
					SgArrayRefExp *array_ref=NULL;
					if (right->args()&&right->arg(0)) 
						{
						expr_list=new SgExprListExp(*right->arg(0));
						}
					stmt_to_delete = addToStmtList(stmt_to_delete, stmt);
					if (isSgArrayRefExp(expr_list->elem(0))||isSgFunctionCallExp(expr_list->elem(0))||isSgVarRefExp(expr_list->elem(0)))
						{
						expr_list->elem(0)->symbol()->setType(* (new SgType(T_ARRAY)));
						if (expr_list->elem(0)->lhs())
							array_ref=new SgArrayRefExp(*expr_list->elem(0)->symbol(),*expr_list->elem(0)->lhs());
						else
							{
							array_ref=new SgArrayRefExp(*expr_list->elem(0)->symbol());
							}
						if (left&&left->symbol())
							{
							if (left->lhs())
								{
								array_ref=new SgArrayRefExp(ptr,*GenerateArrayRefDimList(FindInPointerList(pointer,left->symbol()),array_ref->symbol(),array_ref->lhs()));				
								stmt->insertStmtAfter(*(new SgFunctionCallStmt(*(new SgExpression(RECORD_REF,left,array_ref,NULL,NULL)))));	
								}
							else
								{
								stmt->insertStmtAfter(*(new SgFunctionCallStmt(*(new SgExpression(ARRAY_REF,GenerateArrayRefDimList(FindInPointerList(pointer,left->symbol()),array_ref->symbol(),array_ref->lhs()),NULL,left->symbol())))));	
								}
							}
						}
					else printf("error in string statement - line  %i\n",stmt->lineNumber());
					}
				}
			else
				if (left&&stmt->expr(1))
					{
					switch (left->variant())
						{
							case ARRAY_REF:
							case VAR_REF:	
								{
									if (left->symbol()->thesymb->attr & DVM_POINTER_BIT)
										{
										//SgRecordRefExp *rec_ref=NULL;
										SgExpression *lhs,*rhs;
										lhs=left;
										rhs=stmt->expr(1);
										if ((left->variant()==ARRAY_REF)&&left->lhs())
											{
											lhs=new SgExpression(RECORD_REF,left,new SgVarRefExp(* (new SgSymbol(VARIABLE_NAME,"PTR"))),NULL);
											}
										if ((rhs->variant()==ARRAY_REF)&&rhs->lhs())
											{
											rhs=new SgExpression(RECORD_REF,stmt->expr(1),new SgVarRefExp(* (new SgSymbol(VARIABLE_NAME,"PTR"))),NULL);
											}
										stmt->insertStmtBefore(* (new SgF90PointerAssignStmt(*lhs,*rhs)));
										stmt_to_delete = addToStmtList(stmt_to_delete, stmt);
										}
									break;
								}
						}
					}
		
			}
	    if(stmt->variant()==DVM_PARALLEL_ON_DIR) 
			{
			SgOMPStmt  *parallel=NULL;
			SgStatement *last_node_of_loop=NULL;
			SgExprListExp *explist=NULL;
			SgExpression *new_clause=NULL;
			SgExpression *reduction_clause=NULL;
            SgExpression *shadow_clause=NULL;
            SgExpression *remote_access_clause=NULL;
			SgExpression *indirect_access_clause=NULL;
			SgExpression *stage_clause=NULL;
			SgExpression *across_clause=NULL;
			SgExpression *on_clause=NULL;
			SgForStmt *for_stmt=NULL;
			int ignore=0;
			if (stmt->expr(2)!=NULL) on_clause=stmt->expr(2);
			if (stmt->lexNext()!=NULL) for_stmt=isSgForStmt(stmt->lexNext());
			if (stmt->expr(1)!=NULL) 
				{
				int i,length;
				explist= new SgExprListExp(stmt->expr(1)->thellnd);
				length=explist->length();
				for (i=0;i<length;i++)
				{
					switch (explist->elem(i)->variant())
					{
						case NEW_SPEC_OP:
						{
							new_clause=explist->elem(i)->copyPtr();
							//new_clause=TranslateOpenMPPrivate(new_clause,on_clause);
							break;
						}
						case REDUCTION_OP:
						{
							reduction_clause=explist->elem(i);
							if (reduction_clause!=NULL) ignore=1;
							reduction_clause=TranslateOpenMPReduction(reduction_clause); 
							if (reduction_clause!=NULL) ignore=0;
							break;
						}
						case SHADOW_RENEW_OP:
						case SHADOW_START_OP:
						case SHADOW_WAIT_OP:
						case SHADOW_COMP_OP:
						{
							shadow_clause=explist->elem(i);
							break;
						}
						case REMOTE_ACCESS_OP:
						{
							remote_access_clause=explist->elem(i);
							break;
						}
						case INDIRECT_ACCESS_OP:
						{
							indirect_access_clause=explist->elem(i);
							break;
						}
						case STAGE_OP:
						{
							stage_clause=explist->elem(i);
							break;
						}
						case ACROSS_OP:
						{
							across_clause=explist->elem(i);
							break;
						}
					}
				}
				if (ignore) continue;
				for (i=0;i<length;i++)
				{
					switch (explist->elem(i)->variant())
					{
					/*28.05.2002
					case ACROSS_OP: 
							SgSymbol *tmp,*jj;
							SgVarRefExp *iam,*numt,*jjref;
							SgFunctionCallExp *ogtn,*ognt;
							SgAssignStmt *numt_ognt,*iam_ogtn;
							SgExpression *dovar,*start,*end,*loop;
							SgStatement *next_loop_stmt,*tmp_stmt;
							SgOMPStmt *tmp_stmt1;
							SgForStmt *next_for_stmt;
							SgIfStmt *if_stmt;
							SgValueExp c1(1);
							SgExprListExp *explist;
							SgOMP_SCHEDULE *omp_schedule=new SgOMP_SCHEDULE("STATIC");
							tmp=GenerateNewVariableName("iam");
							tmp->declareTheSymbol(*cur_func);
							iam=new SgVarRefExp(*tmp);
							tmp=GenerateNewVariableName("numt");
							tmp->declareTheSymbol(*cur_func);
							numt=new SgVarRefExp(*tmp);
							tmp=new SgSymbol(VARIABLE_NAME,"omp_get_thread_num");
							ogtn=new SgFunctionCallExp(*tmp);	
							tmp=new SgSymbol(VARIABLE_NAME,"omp_get_num_threads");
							ognt=new SgFunctionCallExp(*tmp);
							numt_ognt=new  SgAssignStmt(*numt,*ognt);
							iam_ogtn=new  SgAssignStmt(*iam,*ogtn);
							stmt->insertStmtAfter(*numt_ognt);
							stmt->insertStmtAfter(*iam_ogtn);
							start=for_stmt->start();
							end=for_stmt->end();
							loop=for_stmt->end();
							for_stmt->setEnd(*loop+*numt-c1);
							tmp=for_stmt->symbol();
							dovar=new SgVarRefExp(*tmp);
							jj=GenerateNewVariableName(tmp->identifier());
							jj->declareTheSymbol(*cur_func);
							for_stmt->setSymbol(*jj);
							next_loop_stmt=for_stmt->getNextLoop();
							jjref=new SgVarRefExp(*jj);
							tmp_stmt=new SgAssignStmt(*dovar,*jjref-*iam);
							next_loop_stmt->insertStmtBefore(*tmp_stmt);
							tmp_stmt1=new SgOMPStmt(OMP_DO_DIR);
							tmp_stmt1->addClause1(*omp_schedule);
							tmp_stmt->insertStmtAfter(* tmp_stmt1);
							next_for_stmt=isSgForStmt(next_loop_stmt);
							next_loop_stmt=next_loop_stmt->lexNext();
							if_stmt=new SgIfStmt(*dovar>=*start&&*dovar<=*end,*next_loop_stmt->copyBlockPtr());
							next_for_stmt->set_body(*if_stmt);
							tmp_stmt=new SgStatement(CONTROL_END);	
							tmp_stmt->setControlParent(*next_for_stmt->controlParent());
							next_loop_stmt->insertStmtBefore(* tmp_stmt);
							tmp_stmt->insertStmtAfter(* new SgOMPStmt(OMP_END_PARALLEL_DIR));
							tmp_stmt->insertStmtAfter(* new SgOMPStmt(OMP_END_DO_DIR));
							explist=new SgExprListExp(*jjref);
							explist->append(*numt);
							explist->append(*iam);
							new_clause=AddOpenMPPrivate(new_clause,explist);
							ignore=1;*/
						case ACROSS_OP: //28.05.2002 
							SgSymbol *tmp,*jj;
							SgVarRefExp *iam,*numt,*jjref;
							SgFunctionCallExp *ogtn,*ognt;
							SgAssignStmt *numt_ognt,*iam_ogtn;
							SgExpression *dovar,*start,*end,*loop;
							SgStatement *tmp_stmt;
							SgOMPStmt *ompstmt;
							SgIfStmt *if_stmt;
							SgValueExp c1(1),c0(0);
							SgExprListExp *explist;
							SgOMP_SCHEDULE *omp_schedule=new SgOMP_SCHEDULE("STATIC");
							last_node_of_loop=NextStmtAfterLoopStmt(stmt->lexNext());
							GenerateOMPRoutinesName();
							tmp=GenerateNewVariableName("iam");
							tmp->declareTheSymbol(*cur_func);
							iam=new SgVarRefExp(*tmp);
							tmp=GenerateNewVariableName("numt");
							tmp->declareTheSymbol(*cur_func);
							numt=new SgVarRefExp(*tmp);
							tmp=new SgSymbol(VARIABLE_NAME,"omp_get_thread_num");
							ogtn=new SgFunctionCallExp(*tmp);	
							tmp=new SgSymbol(VARIABLE_NAME,"omp_get_num_threads");
							ognt=new SgFunctionCallExp(*tmp);
							numt_ognt=new  SgAssignStmt(*numt,*ognt);
							numt_ognt->setlineNumber(-1);
							iam_ogtn=new  SgAssignStmt(*iam,*ogtn);
							iam_ogtn->setlineNumber(-1);
							stmt->insertStmtBefore(*(new SgAssignStmt(*iam,c0)));
							stmt->insertStmtBefore(*(new SgAssignStmt(*numt,c1)));
							stmt->insertStmtAfter(*numt_ognt);
							stmt->insertStmtAfter(*iam_ogtn);
							start=for_stmt->start();
							end=for_stmt->end();
							loop=for_stmt->end();
							for_stmt->setEnd(*loop+*numt-c1);
							tmp=for_stmt->symbol();
							dovar=new SgVarRefExp(*tmp);
							jj=GenerateNewVariableName(tmp->identifier());
							jj->declareTheSymbol(*cur_func);
							for_stmt->setSymbol(*jj);
							tmp_stmt=for_stmt->getNextLoop();
							jjref=new SgVarRefExp(*jj);
							tmp_stmt->insertStmtBefore(*(new SgAssignStmt(*dovar,*jjref-*iam)));
							//tmp_stmt->lexPrev()->setlineNumber(-1);
							ompstmt=new SgOMPStmt(OMP_DO_DIR);
							ompstmt->addClause1(*omp_schedule);
							tmp_stmt->insertStmtBefore(* ompstmt);
							if_stmt=new SgIfStmt(*dovar<*start||*dovar>*end,*(new SgStatement(CYCLE_STMT)));
							if_stmt->setlineNumber(-1);
							if_stmt->trueBody(1)->setlineNumber(-1);
							if_stmt->trueBody(2)->setlineNumber(-1);
							tmp_stmt->insertStmtAfter(*if_stmt);
							if_stmt->setControlParent(*tmp_stmt->controlParent());
                            if (last_node_of_loop != 0)
                            {
                                last_node_of_loop->insertStmtAfter(*new SgOMPStmt(OMP_END_PARALLEL_DIR));
                                last_node_of_loop->insertStmtBefore(*new SgOMPStmt(OMP_END_DO_DIR));
                            }
							explist=new SgExprListExp(*jjref);
							explist->append(*numt);
							explist->append(*iam);
							new_clause=AddOpenMPPrivate(new_clause,explist);
							ignore=1;
					}
				}
				}
				if (!ignore)
					{
					if ( (last_node_of_loop = NextStmtAfterLoopStmt(stmt->lexNext())) != 0)
						{
						parallel=new SgOMPStmt(OMP_END_PARALLEL_DIR);
						last_node_of_loop->insertStmtAfter(*parallel);
						parallel=new SgOMPStmt(OMP_END_DO_DIR);
						last_node_of_loop->insertStmtAfter(*parallel);
						parallel=new SgOMPStmt(OMP_DO_DIR);
						parallel->addClause1(*(new SgOMP_SCHEDULE("STATIC")));
						stmt ->insertStmtAfter(*parallel);
						}
					else printf("error in string statement - line  %i\n",stmt->lineNumber());
					}
				parallel=new SgOMPStmt(OMP_PARALLEL_DIR);
				if (reduction_clause!=NULL) 
                parallel->addClause1(*reduction_clause); 
	//				if (new_clause!=NULL) 
				parallel->addClause1(*TranslateOpenMPPrivate(new_clause,on_clause)); 
				stmt->insertStmtAfter(*parallel);
			}
	    if(stmt->variant()==DVM_TASK_REGION_DIR) 
			{
			SgOMPStmt  *parallel=NULL;
			SgStatement *task=NULL;
			SgExpression *reduction_clause=NULL;
            SgExpression *on_clause=NULL;
			SgExpression *private_clause=NULL;
			SgForStmt *for_stmt;
			int ignore=0,task_loop=0;
			if (stmt->expr(2)!=NULL) on_clause=stmt->expr(2);
			if (stmt->expr(0)!=NULL) 
				{
					switch (stmt->expr(0)->variant())
					{
						case REDUCTION_OP:
						{
							reduction_clause=stmt->expr(0);
							if (reduction_clause!=NULL) ignore=1;
							reduction_clause=TranslateOpenMPReduction(reduction_clause); 
							if (reduction_clause!=NULL) ignore=0;
							break;
						}
				}
				}
			for (task = stmt; task && (task != last); task = task->lexNext())
				{
					if (isSgExecutableStatement(task))
					  {

						if(task->variant()==DVM_PARALLEL_TASK_DIR)
							{
							//////////////////////////////////////////////////////////////								
								SgExprListExp *explist=NULL;
								SgExpression *newclause=NULL;
								SgExpression *redclause=NULL;
								SgExpression *onclause=NULL;
								if (task->expr(2)!=NULL) onclause=task->expr(2);
								if (task->lexNext()!=NULL) for_stmt=(SgForStmt *)(task->lexNext());
								if (task->expr(1)!=NULL) 
									{
										int i,length;
										explist= new SgExprListExp(task->expr(1)->thellnd);
										length=explist->length();
										for (i=0;i<length;i++)
											{
												switch (explist->elem(i)->variant())
													{
														case NEW_SPEC_OP:
														{
															newclause=explist->elem(i)->copyPtr();
															break;
														}
														case REDUCTION_OP:
														{
															redclause=explist->elem(i);
															if (redclause!=NULL) task_loop=1;
															redclause=TranslateOpenMPReduction(redclause); 
															if (redclause!=NULL) task_loop=0;
															break;
														}
													}
											}
									}
							///////////////////////////////////////////////////////////////
									if (!ignore&&!task_loop)
										{	
											SgStatement *last_node_of_loop=NULL;
											if ( (last_node_of_loop = NextStmtAfterLoopStmt(task->lexNext())) != 0)
												{
												parallel=new SgOMPStmt(OMP_END_PARALLEL_DIR);
												last_node_of_loop->insertStmtAfter(*parallel);
												parallel=new SgOMPStmt(OMP_END_DO_DIR);
												last_node_of_loop->insertStmtAfter(*parallel);
												}
											else printf("error in string statement - line  %i\n",task->lineNumber());
											parallel=new SgOMPStmt(OMP_DO_DIR);
											parallel->addClause1(*(new SgOMP_SCHEDULE("STATIC")));
											task ->insertStmtAfter(*parallel);
											parallel=new SgOMPStmt(OMP_PARALLEL_DIR);
											if (redclause!=NULL) 
												parallel->addClause1(*redclause); 
											if (reduction_clause!=NULL) 
												parallel->addClause1(*reduction_clause); 
											parallel->addClause1(*TranslateOpenMPPrivate(newclause,onclause)); 
											task->insertStmtAfter(*parallel);
										}
							ignore=1;	
							task_loop=1;
							////////////////////////////////////////////////////////////////
							}

						if(task->variant()==DVM_END_TASK_REGION_DIR) 
							{
							if (!ignore)
								{
									SgOMPStmt  *tregion=NULL;
									tregion=new SgOMPStmt(OMP_END_PARALLEL_DIR);
									task ->insertStmtAfter(*tregion);
									if (!task_loop)
										{
										tregion=new SgOMPStmt(OMP_END_SECTIONS_DIR);
										task ->insertStmtAfter(*tregion);	
										}
									break;
								}
							}
						if(task->variant()==DVM_ON_DIR) 
						{
							if (!ignore)
							{
								SgOMPStmt  *tregion=NULL;
								tregion=new SgOMPStmt(OMP_SECTION_DIR);
								task ->insertStmtAfter(*tregion);
								if ((task->expr(1))!=NULL&&(task->expr(1)->lhs()!=NULL)&&(task->expr(1)->lhs()->variant()==EXPR_LIST)) 
									{				
									private_clause=TranslateOpenMPPrivate(private_clause,task->expr(1)->lhs()->copyPtr());
									}
							}
						}
					}
				}
			if (!ignore)
				{
					if (!task_loop)
						{
						parallel=new SgOMPStmt(OMP_SECTIONS_DIR);
						stmt ->insertStmtAfter(*parallel);
						}
					parallel=new SgOMPStmt(OMP_PARALLEL_DIR);
					if (reduction_clause!=NULL) 
					parallel->addClause1(*reduction_clause); 
					if (private_clause!=NULL) 
					parallel->addClause1(*private_clause); 
					stmt->insertStmtAfter(*parallel);
				}
			}
	    if(stmt->variant()==DVM_ASYNCHRONOUS_DIR) 
			{
			stmt_list *pstmt = NULL;
			SgOMPStmt  *parallel=NULL;
			SgStatement *section=NULL;
			SgStatement *prev=NULL;
			for (section = stmt->lexNext(); section && (section != last); section = section->lexNext())
				{
					prev=section;	
					if (isSgExecutableStatement(section))
					  {
						if(section->variant()==DVM_F90_DIR) 
							{
								SgAssignStmt *sect_assign;
								if ((section->expr(0)!=NULL)&&(section->expr(1)!=NULL))
									{
									sect_assign=new SgAssignStmt(*section->expr(0),*section->expr(1));
									section->insertStmtAfter(*sect_assign);
									}
								continue;
							}
						if(section->variant()==DVM_ENDASYNCHRONOUS_DIR) 
							{
								parallel=new SgOMPStmt(OMP_END_PARALLEL_DIR);
								section ->insertStmtAfter(*parallel);
								parallel=new SgOMPStmt(OMP_END_WORKSHARE_DIR);
								section ->insertStmtAfter(*parallel);
								break;
							}
						if(section->variant()==FOR_NODE) 
							{		
								pstmt = addToStmtList(pstmt, section);
							}
					}
				}
			for(;pstmt; pstmt= pstmt->next) Extract_Stmt(pstmt->st);
			parallel=new SgOMPStmt(OMP_WORKSHARE_DIR);
			stmt ->insertStmtAfter(*parallel);
			parallel=new SgOMPStmt(OMP_PARALLEL_DIR);
			stmt ->insertStmtAfter(*parallel);
			}
		}
	}
for(;stmt_to_delete; stmt_to_delete= stmt_to_delete->next) Extract_Stmt(stmt_to_delete->st);

if (pointer)
	{
	type_list *tlist=NULL;
	for (stmt = first; stmt && (stmt != last); stmt = stmt->lexNext())
		{
			SgExprListExp *vars,*type,*dims,*new_vars;
			vars=type=dims=new_vars=NULL;
			pointer_list *tmp;
			//printf ("Variant %i = %i\n",stmt->variant(),stmt->lineNumber());
			if (stmt->variant()==VAR_DECL)
				{
					if(stmt->expr(0)&&(vars=isSgExprListExp(stmt->expr(0))));
					if(stmt->expr(1)&&(type=isSgExprListExp(stmt->expr(1))));
					if(stmt->expr(2)&&(dims=isSgExprListExp(stmt->expr(2))));
					for(int i=0;i<vars->length();i++)
						{
						if(vars->elem(i)->symbol()&&(tmp=FindPointerInPointerList(pointer,vars->elem(i)->symbol()))) 
							{
							type_list *tname;
							SgSymbol *s;
							SgTypeExp *type_op;
							SgType *type_name;
							if (!vars->elem(i)->lhs())
								{
								//printf("Arr REF vars->elem(i)->symbol %s\n",vars->elem(i)->symbol()->identifier());
								DeclarePointerType(stmt,vars->elem(i)->symbol(),tmp->dim,tmp->symb->type());
								continue;
								}

							if (tlist&&(tname=FindTypeInTypeList(tlist,tmp->symb->type(),tmp->dim)))
								{
								type_name=tname->type;
								}
							else
								{
								s=DeclarePointerTypeRecord(stmt,tmp->dim,tmp->symb->type());
								type_name=new SgDerivedType(*s);
								TYPE_SYMB(type_name->thetype)=s->thesymb;
								TYPE_LENGTH(type_name->thetype)=tmp->dim;
								tlist=AddToTypeList(tlist,type_name,tmp->symb->type());
								}
							type_op=new SgTypeExp(*type_name);
							stmt->insertStmtBefore(* (new SgStatement(VAR_DECL,NULL,NULL,vars->elem(i),type_op,NULL)));
							}
						else 
							{
								if (!strcmp(vars->elem(i)->symbol()->identifier(),"heap")||!strcmp(vars->elem(i)->symbol()->identifier(),"allocate")||(vars->elem(i)->symbol()->thesymb->attr & HEAP_BIT)) continue		;
								if (!new_vars)
									new_vars=new SgExprListExp(*vars->elem(i));
								else new_vars->append(*vars->elem(i));

							}

						}
					if (new_vars)
						{
						stmt->setExpression (0,*new_vars);
						}
					else 
						{
						stmt_to_delete = addToStmtList(stmt_to_delete, stmt);
						}
				}
			if (stmt->variant()==COMM_STAT)
				{
					SgExprListExp *vars,*new_vars;
					vars=new_vars=NULL;
					if(stmt->expr(0)&&(vars=isSgExprListExp(stmt->expr(0)->lhs())))
					{
					for(int i=0;i<vars->length();i++)
						{
						if(vars->elem(i)->symbol()&&(isInSymbList(heaps,vars->elem(i)->symbol()))) 
							continue;
						if (!new_vars)
								new_vars=new SgExprListExp(*vars->elem(i));
						else new_vars->append(*vars->elem(i));
						}
					if (new_vars)
						{
						stmt->expr(0)->setLhs(*new_vars);
						}
					else 
						{
						stmt_to_delete = addToStmtList(stmt_to_delete, stmt);
						}
					}
				}
		}	

	for(;stmt_to_delete; stmt_to_delete= stmt_to_delete->next) Extract_Stmt(stmt_to_delete->st);		
	}
}		



SgSymbol* DeclarePointerTypeRecord(SgStatement *stmt,int numdim,SgType *type)
{
	int i;
	SgSymbol *sym;
	SgExpression *symref;
	SgRecordStmt *record;
	SgExprListExp *expr_list=NULL;
	SgExprListExp *dim=NULL;
	SgExpression *tmp=NULL;
	sym=new SgSymbol(VARIABLE_NAME,"PTR");
	sym->setType(*(new SgArrayType(*type)));
	symref=new SgArrayRefExp(*sym);
	tmp=new SgExpression(POINTER_OP);
	expr_list=new SgExprListExp(*tmp);
	tmp=new SgExpression(DIMENSION_OP);
	dim=new SgExprListExp(*(new SgKeywordValExp(":")));
	for(i=numdim-1;i>0;i--)
		dim->append(*(new SgKeywordValExp(":")));
	tmp->setLhs(*dim);
	expr_list->append(*tmp);
	sym=GenerateTypeName(numdim);
	record=new SgRecordStmt(*sym,*(new SgVarDeclStmt(*symref,*expr_list,*type)));
	//cur_func->lastDeclaration()
	stmt->insertStmtBefore(*record);
	return sym;
}

void DeclarePointerType(SgStatement *stmt,SgSymbol *sym,int numdim,SgType *type)
{
	int i;
	SgExpression *symref;
	SgExprListExp *expr_list=NULL;
	SgExprListExp *dim=NULL;
	SgExpression *tmp=NULL;
	sym->setType(*(new SgArrayType(*type)));
	symref=new SgArrayRefExp(*sym);
	tmp=new SgExpression(POINTER_OP);
	expr_list=new SgExprListExp(*tmp);
	tmp=new SgExpression(DIMENSION_OP);
	dim=new SgExprListExp(*(new SgKeywordValExp(":")));
	for(i=numdim-1;i>0;i--)
		dim->append(*(new SgKeywordValExp(":")));
	tmp->setLhs(*dim);
	expr_list->append(*tmp);
	//printf("Insert\n");
	stmt->insertStmtBefore(* (new SgVarDeclStmt(*symref,*expr_list,*type)));
}

////////////////////////LIST FUNCTIONS DEFINITION/////////////////
pointer_list  *AddToPointerList ( pointer_list *ls, SgSymbol *s,int dim)
{
pointer_list *l;
//adding the symbol 's' to pointer_list 'ls'
  if(!ls) {	
     ls = new pointer_list;
     ls->symb = s;
     ls->dim=dim;
     ls->next = NULL;
  } else {
     l = new pointer_list;
     l->symb = s;
     l->dim=dim;
     l->next = ls;
     ls = l;
  }
  return(ls);
}

int FindInPointerList ( pointer_list *ls, SgSymbol *s)
{
pointer_list *l;
for(l=ls; l; l=l->next)
   if(!strcmp(l->symb->identifier(),s->identifier()))
      return l->dim;
return 0;

}

pointer_list* FindPointerInPointerList ( pointer_list *ls, SgSymbol *s)
{
pointer_list *l;
for(l=ls; l; l=l->next)
   if(!strcmp(l->symb->identifier(),s->identifier()))
      return l;
return NULL;

}

type_list  *AddToTypeList ( type_list *ls, SgType *type_name,SgType *base_type)
{
type_list *l;
//adding the symbol 's' to type_list 'ls'
  if(!ls) {	
	 ls = new type_list;
     ls->type = type_name;
	 ls->base = base_type;
     ls->next = NULL;
  } else {
     l = new type_list;
	 l->type = type_name;
	 l->base = base_type;
     l->next = ls;
     ls = l;
  }
  return(ls);
}

type_list* FindTypeInTypeList ( type_list *ls, SgType *t, int dim)
{
type_list *l;
for(l=ls; l; l=l->next)
	{
		if(( TYPE_LENGTH(l->type->thetype)==dim)&&t->equivalentToType(l->base))
			  return l;
	}
return NULL;

}

/*
char* filterDVM(char *s)
{
  char c;
  int i = 1;
  char temp[1024];
  int temp_i = 0;
  int buf_i = 0;
  int commentline = 0;
  char *resul, *init;
  int OMP,DVM;
  OMP=DVM=0;
  
  if (!s) return NULL;
  if (strlen(s)==0) return s;
  make_a_malloc_stack();
  resul = (char *) mymalloc(2*strlen(s));
  memset(resul, 0, 2*strlen(s));
  init = resul;
  c = s[0];

  if ((c != ' ')
      &&(c != '\n')
      && (c != '0')
      && (c != '1') 
      && (c != '2') 
      && (c != '3') 
      && (c != '4') 
      && (c != '5') 
      && (c != '6') 
      && (c != '7') 
      && (c != '8') 
      && (c != '9'))
		commentline = 1;
  else
    commentline = 0;
  if(commentline)
	{	if (s[1]=='$') 
			OMP=1;
		else	
			if (s[1]=='D') 
				DVM=1;		
			else OMP=DVM=0;
	}
  temp_i = 0;
  i = 0;
  buf_i =0;
  while (c!='\0')
    {
      c = s[i];
      temp[ buf_i] = c;
      if (c == '\n')
        {
          temp[ buf_i+1] = '\0';
          sprintf(resul,"%s",temp);
          resul = resul + strlen(temp);
          temp_i = -1;
          buf_i = -1;
          if ((s[i+1] != ' ')
              &&(s[i+1] != '\n')
              && (s[i+1] != '0')
              && (s[i+1] != '1') 
              && (s[i+1] != '2') 
              && (s[i+1] != '3') 
              && (s[i+1] != '4') 
              && (s[i+1] != '5') 
              && (s[i+1] != '6') 
              && (s[i+1] != '7') 
              && (s[i+1] != '8') 
              && (s[i+1] != '9'))
				commentline = 1;
          else
            commentline = 0;
		  if(commentline)
			{	if (s[i+1]=='$') 
					OMP=1;
				else	
					if (s[i+1]=='D') 
						DVM=1;		
					else OMP=DVM=0;
			}
        } else
          {
            if ((temp_i == 71) && !commentline)
              { 
                temp[ buf_i+1]  = '\0';
                sprintf(resul,"%s\n",temp);
                resul = resul + strlen(temp)+1;
                sprintf(resul,"     +");
                resul = resul + strlen("     +");
                commentline = 0;
                memset(temp, 0, 1024);
                temp_i = strlen("     +")-1;
                buf_i = -1;
              }
	    if ((temp_i == 71)&&commentline&&(OMP||DVM))
              { 
		int count=0;
		for(;s[i]!='$';i--,count++)
            	    {
		    if (strncmp(&(s[i]),"ONTO", strlen("ONTO"))== 0)
			break;
		    if (strncmp(&(s[i]),"BEGIN", strlen("BEGIN"))== 0)
			break;
		    if (strncmp(&(s[i]),"WITH", strlen("WITH"))== 0)
			break;
		    if (strncmp(&(s[i]),"NEW", strlen("NEW"))== 0)
			break;
		    if (strncmp(&(s[i]),"REDUCTION", strlen("REDUCTION"))== 0)
			break;
			if (strncmp(&(s[i]),"REMOTE", strlen("REMOTE"))== 0)
			break;    
		    if (strncmp(&(s[i]),"TEMPLATE", strlen("TEMPLATE"))== 0)
			break;
		    if (strncmp(&(s[i]),"SHADOW", strlen("SHADOW"))== 0)
			break;
		    if (strncmp(&(s[i]),"INHERIT", strlen("INHERIT"))== 0)
			break;
		    if (strncmp(&(s[i]),"DYNAMIC", strlen("DYNAMIC"))== 0)
			break;
		    if (strncmp(&(s[i]),"DIMENSION", strlen("DIMENSION"))== 0)
			break;
			if (strncmp(&(s[i]),"PRIVATE", strlen("PRIVATE"))== 0)
			break;
		    if (strncmp(&(s[i]),"SCHEDULE", strlen("SCHEDULE"))== 0)
			break;    
			if (strncmp(&(s[i]),"ACROSS", strlen("ACROSS"))== 0)
			break;    
			if (strncmp(&(s[i]),"DO", strlen("DO"))== 0)
			break;
			if (strncmp(&(s[i]),"PROCESSORS", strlen("PROCESSORS"))== 0)
			break;
		    if (strncmp(&(s[i]),"DISTRIBUTE", strlen("DISTRIBUTE"))== 0)
			break;
		    if (strncmp(&(s[i]),"ALIGN", strlen("ALIGN"))== 0)
			break;
		    }
		i--;count++;      
		if (count<36)
		    temp[ buf_i+1-count]  = '\0';
		else 
		    {
		    i+=count;
		    temp[ buf_i+1]  = '\0';
		    }
                sprintf(resul,"%s\n",temp);
                resul = resul + strlen(temp)+1;
				memset(temp, 0, 1024);
				if (OMP)
					{
					sprintf(resul,"!$OMP*");
					resul = resul + strlen("!$OMP*");
					temp_i = strlen("!$OMP*")-1;
					}
				if (DVM)
					{
					sprintf(resul,"CDVM$*");
					resul = resul + strlen("CDVM$*");
					temp_i = strlen("CDVM$*")-1;
					}
                buf_i = -1;
              }
          }
      i++;
      temp_i++;
      buf_i++;
    }

  return init;  
}
*/

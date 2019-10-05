/*********************************************************************/
/*                   Inline Expansion    2006                        */
/*********************************************************************/


/*********************************************************************/
/*                         Inliner                                   */
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "inline.h"

#ifdef __SPF
extern "C" void printLowLevelWarnings(const char *fileName, const int line, const wchar_t *messageR, const char *messageE, const int group) { }
extern "C" void addToCollection(const int line, const char *file, void *pointer, int type) { }
extern "C" void removeFromCollection(void *pointer) { }

#include <map>
#include <string>

std::map<PTR_BFND, std::pair<std::string, int>> sgStats;
std::map<PTR_LLND, std::pair<std::string, int>> sgExprs;
void addToGlobalBufferAndPrint(const std::string &toPrint) { }
#endif

int current_file_id;     //number of current file 

void Inliner(graph_node *gtop)
{
    SgStatement *header, *stmt, *last, *newst;
    int i;

    header = gtop->st_header;
    top_header = header;
    if (with_cmnt)
        top_header->addComment("!*****AFTER INLINE EXPANSION******\n");
    top_node = gtop;
    vcounter = 0;
    max_lab = getLastLabelId();
    num_lab = 0;
    for (i = 0; i < 10; i++)
        do_var[i] = NULL;
    top_temp_vars = NULL;

    if (deb_reg)
        printf("\nINLINER  %s [%d]\n", gtop->symb->identifier(), gtop->symb->id());

    //Find all entry points
    EntryPointList(gtop->file);

    //Substitute all integer symbolic constants in "top level" routine
    IntegerConstantSubstitution(header);

    //Clean "top level" routine (precalculation of function call and actual parameter expressions)
    RoutineCleaning(header);
    SetScopeToLabels(header);

    // for debugging
    if (deb_reg > 1)
        PrintSymbolTable(gtop->file);

    // Perform the inline expansion
    // for each call site to be expanded (as encountered at "top level")
    last = header->lastNodeOfStmt();
    top_last = last;
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
        if (isSgExecutableStatement(stmt) && stmt->variant() != FORMAT_STAT) {
            top_first_executable = stmt; break;
        }
    top_last_declaration = top_first_executable->lexPrev();

    newst = new SgStatement(CONT_STAT);
#if __SPF
    insertBfndListIn(newst->thebif, top_last_declaration->thebif, NULL);
#else
    top_last_declaration->insertStmtAfter(*newst);
#endif
    top_first_executable = newst;

    MakeDeclarationForTempVarsInTop();  //finish cleaning

    for (stmt = top_first_executable; stmt && (stmt != last); )
    {
        switch (stmt->variant())
        {
        case ASSIGN_STAT:
            if (stmt->expr(1)->variant() == FUNC_CALL)
                stmt = InlineExpansion(gtop, stmt, stmt->expr(1)->symbol(), stmt->expr(1)->lhs()); //stmt = first inserted statement or next statement
            else
                stmt = stmt->lexNext();
            continue;
        case PROC_STAT:
            stmt = InlineExpansion(gtop, stmt, stmt->symbol(), stmt->expr(0));  //stmt = first inserted statement or next statement
            continue;
        default:
            stmt = stmt->lexNext();
            continue;
        }
    }
    // Make delarations for temporary variables created by translation algorithm (TranslateSubprogramReferences()) 
    MakeDeclarationForTempVarsInTop();

    // Transform declaration part of top level routine
    // DATA and statement functions -> after all specification statements (standard F77)
    TransformForFortran77();

    newst->extractStmt();

    // Extract routines for all the graph nodes except top node
    if (deb_reg && gtop && gtop->to_called)
        printf("\n   T a b l e   o f   I n l i n e   E x p a n s i o n s  i n  %s\n\n", gtop->symb->identifier());

    ExtractSubprogramsOfCallGraph(gtop);

    //
    if (deb_reg > 2)
        PrintSymbolTable(gtop->file);
    return;
}

void EntryPointList(SgFile *file)
//find entry point in the inline flow DAG 
{
    SgStatement *first_st, *stmt;
    first_st = file->firstStatement();
    for (stmt = first_st; stmt; stmt = stmt->lexNext())
        if (stmt->variant() == ENTRY_STAT)
            entryst_list = addToStmtList(entryst_list, stmt);
}

void IntegerConstantSubstitution(SgStatement *header)
//Substitute all integer symbolic constants in  routine
{
    SgStatement *last, *stmt;
    SgExpression *e;
    SgExprListExp *el;
    SgConstantSymb  *sc;
    // PTR_LLND ranges;
    int i;
    last = header->lastNodeOfStmt();
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
    {  // PARAMETER statement
        if (stmt->variant() == PARAM_DECL)

        {
            for (el = isSgExprListExp(stmt->expr(0)); el; el = el->next())
            {
                e = el->lhs(); sc = isSgConstantSymb(e->symbol());
                SYMB_VAL(sc->thesymb) = ReplaceIntegerParameter(&(sc->constantValue()->copy()))->thellnd;
            }
            //printf("PARAM_DECL\n");
            continue;
        }
        if (stmt->variant() == VAR_DECL)
            ReplaceIntegerParameter_InType(stmt->expr(1)->type());

        // any other statement
        for (i = 0; i < 3; i++)
            if (stmt->expr(i))
                stmt->setExpression(i, *ReplaceIntegerParameter(stmt->expr(i)));

    }
    ReplaceIntegerParameterInTypeOfVars(header, last);
}

void ReplaceIntegerParameterInTypeOfVars(SgStatement *header, SgStatement *last)
{
    SgSymbol *s, *sl;
    // PTR_LLND ranges;
    sl = last->lexNext() ? last->lexNext()->symbol() : NULL;

    //if(sl) printf("%s  %s\n",header->symbol()->identifier(),sl->identifier());
    for (s = header->symbol(); s != sl && s != NULL; s = s->next())
        if (s->scope() == header)   //local variable
            ReplaceIntegerParameter_InType(s->type());
    return;
}
void ReplaceIntegerParameter_InType(SgType *t)
{
    PTR_LLND ranges;
    SgExpression *ne;
    if (!t) return;
    if ((ranges = TYPE_RANGES(t->thetype)) != 0)
    {
        ne = ReplaceIntegerParameter(LlndMapping(ranges));
        // if(isSgArrayType(t))     //ranges->variant() == EXPR_LIST
          //  Calculate_List(ne); 
    }
    if ((ranges = TYPE_KIND_LEN(t->thetype)) != 0)
        ne = ReplaceIntegerParameter(LlndMapping(ranges));

}


void MakeDeclarationForTempVarsInTop()
{
    symb_list *sl;
    for (sl = top_temp_vars; sl; sl = sl->next)
        MakeDeclarationStmtInTop(sl->symb);
    top_temp_vars = NULL;
}

void TransformForFortran77()
{
    SgStatement *stmt, *st1;
    for (stmt = top_header; stmt != top_last_declaration; )
    {
        if (stmt->variant() == DATA_DECL || stmt->variant() == STMTFN_STAT)
        {
            st1 = stmt;
            stmt = stmt->lexNext();
            st1->extractStmt();
            top_first_executable->insertStmtBefore(*st1, *top_header);
        }
        else
            stmt = stmt->lexNext();
    }
}

void ExtractSubprogramsOfCallGraph(graph_node *gtop)
{
    edge *el;
    // graph_node *nd;

    for (el = gtop->to_called; el; el = el->next)
    {
        if (el->to->st_header)
        {
            el->to->st_header->extractStmt();
            el->to->st_header = NULL;
            if (deb_reg)
                printf("  %s: %d\n", el->to->symb->identifier(), el->to->count);
            ExtractSubprogramsOfCallGraph(el->to);
        }
    }
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//                 R O U T I N E   C L E A N I N G
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

void RoutineCleaning(SgStatement *header)
{
    SgStatement *last, *stmt;
    //SgExpression *e;
    //SgExprListExp *el;
    //SgConstantSymb  *sc;
    SgSymbol *s;
    //int i;
    cur_func = header;
    last = header->lastNodeOfStmt();
    //scanning local symbols,
    // if symbol used as a variable and is an intrinsic function name,
    // rename the symbol to not conflict with any intrinsic function names 
    for (s = header->symbol(); s; s = s->next())
        if (s->scope() == header && isSgVariableSymb(s) && isIntrinsicFunctionName(s->identifier()))
            SYMB_IDENT(s->thesymb) = ChangeIntrinsicFunctionName(s->identifier());
    // cleaning each executable statement 
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
    {
        if (isSgExecutableStatement(stmt)) //is not Fortran specification statement        
            StatementCleaning(stmt);
    }
}


void StatementCleaning(SgStatement *stmt)
{
    SgAssignStmt *asst;
    SgSymbol *sf;
    if ((asst = isSgAssignStmt(stmt)) != 0)
        //if(stmt->variant() == ASSIGN_STAT)
    {
        if ((asst->rhs()->variant() == FUNC_CALL) &&
            (isSgVarRefExp(asst->lhs())
                ||
                (isSgArrayRefExp(asst->lhs()) && !isSgArrayType(asst->lhs()->type()))))
        {
            ReplaceContext(stmt);
            SearchFunction(asst->lhs(), stmt);
            SearchFunction(asst->rhs()->lhs(), stmt); // actual parameter expression list
            PrecalculateActualParameters(asst->rhs()->symbol(), asst->rhs()->lhs(), stmt);
            return;
        }

    }
    if ((sf = SearchFunction(stmt->expr(0), stmt)) != 0)   stmt->setExpression(0, *new SgVarRefExp(sf));
    if ((sf = SearchFunction(stmt->expr(1), stmt)) != 0)   stmt->setExpression(1, *new SgVarRefExp(sf));
    if ((sf = SearchFunction(stmt->expr(2), stmt)) != 0)   stmt->setExpression(2, *new SgVarRefExp(sf));

    if (stmt->variant() == PROC_STAT)
    {
        ReplaceContext(stmt);
        PrecalculateActualParameters(stmt->symbol(), stmt->expr(0), stmt);
    }
}

SgSymbol *SearchFunction(SgExpression *e, SgStatement *stmt)
{
    SgSymbol *sf;
    if (!e)
        return(NULL);
    if (e->variant() == FUNC_CALL)
    {
        return(PrecalculateFtoVar(e, stmt));
    }

    if ((sf = SearchFunction(e->lhs(), stmt)) != 0)  e->setLhs(new SgVarRefExp(sf));
    if ((sf = SearchFunction(e->rhs(), stmt)) != 0)  e->setRhs(new SgVarRefExp(sf));
    return (NULL);
}

SgSymbol *PrecalculateFtoVar(SgExpression *e, SgStatement *stmt)
{
    SgStatement *newst;
    SgSymbol *sf;
    SgType *t;
    t = TypeOfResult(e);
    if (!t)
        err("Wrong type", 2, stmt);
    sf = GetTempVarForF(e->symbol(), t);
    newst = new SgAssignStmt(*new SgVarRefExp(sf), *e);
    InsertNewStatementBefore(newst, stmt);
    StatementCleaning(newst);
    return(sf);
}

void PrecalculateActualParameters(SgSymbol *s, SgExpression *e, SgStatement *stmt)
{// Precalculate actual parameter expressions
 //e - actual parameter list
    int i;
    SgExpression *el;
    SgSymbol *sp;
    if (!e) return;
    if (is_NoExpansionFunction(s)) return; // expansion may not be made
    i = 1;
    for (el = e; el; el = el->rhs(), i++)
        switch (ParameterType(el->lhs(), stmt))
        {
        case 1:  break; //actual parameter can be accessed by reference
       //case 2:  PrecalculateSubscripts(el->lhs(),stmt); break;
        default: sp = GetTempVarForArg(i, s, el->lhs()->type());
            PrecalculateExpression(sp, el->lhs(), stmt); //to support access by reference 
            el->setLhs(new SgVarRefExp(sp)); //replace actual parameter expression by 'sp' reference
            break;
        }
}

void PrecalculateExpression(SgSymbol *sp, SgExpression *e, SgStatement *stmt)
{
    SgStatement *newst;
    newst = new SgAssignStmt(*new SgVarRefExp(sp), *e);
    InsertNewStatementBefore(newst, stmt);
}


int ParameterType(SgExpression *e, SgStatement *stmt)
{
    if (isSgVarRefExp(e) ||                                      // scalar variable
        (isSgArrayRefExp(e) && !e->lhs()) ||                     // array variable whithout subscript or string variable
        e->variant() == CONST_REF ||                              // symbol (named) constant
        (isSgValueExp(e) && e->type()->variant() != T_STRING) || // literal constant
        (isSgArrayRefExp(e) && TestSubscripts(e->lhs(), stmt)) || // array reference whose subscripts are constant or scalar
        (e->variant() == ARRAY_OP && isSgVarRefExp(e->lhs()) &&
            TestRange(e->rhs(), stmt)) ||// substring reference whose subscripts are constant or scalar
            (e->variant() == ARRAY_OP && isSgArrayRefExp(e->lhs())
                && TestSubscripts(e->lhs()->lhs(), stmt)
                && TestRange(e->rhs(), stmt)))    // substring reference whose subscripts are constant or scalar                                                    
        return(1); // actual parameter can be accessed by reference

    //  else if(isSgArrayRefExp(e))
    //    return(2);
    //  else if(e->variant()==ARRAY_OP)
    //    return(3);

    else
        return(0); // precalculation expression is needed to support access by reference   
}

int TestSubscripts(SgExpression *e, SgStatement *stmt)
{
    SgExpression *el, *ei;
    //SgSymbol *sp;
    for (el = e; el; el = el->rhs()) {
        ei = el->lhs(); // a subscript
        if (isSgVarRefExp(ei) || (ei->variant() == CONST_REF) || isSgValueExp(ei)) // constant or scalar
            continue;
        else
            //return(0);
        {//sp=GetTempVarForSubscr(ei->type());
         //PrecalculateExpression(sp,ei,stmt); //to support access by reference 
         //el->setLhs(new SgVarRefExp(sp)); //replace subscript expression by 'sp' reference
            continue;
        }
    }
    return(1);
}

int TestRange(SgExpression *e, SgStatement *stmt)
{
    SgExpression *ei;
    SgSymbol *sp;

    int ret;
    ret = 0;
    //e->unparsestdout();  (e->lhs())->unparsestdout(); //(e->rhs())->unparsestdout();
    //printf("  testrange %d  %d\n", e->variant(), (e->lhs())->variant());

    ei = e->lhs();

    if (!ei || isSgVarRefExp(ei) || (ei->variant() == CONST_REF) || isSgValueExp(ei))
        ret = 1;
    else
    {
        sp = GetTempVarForSubscr(ei->type());
        PrecalculateExpression(sp, ei, stmt); //to support access by reference 
        e->setLhs(new SgVarRefExp(sp)); //replace subrange expression by 'sp' reference
    }

    ei = e->rhs();
    if (!ei || isSgVarRefExp(ei) || (ei->variant() == CONST_REF) || isSgValueExp(ei))
        return(1);
    else
        //return(0);
    {
        sp = GetTempVarForSubscr(ei->type());
        PrecalculateExpression(sp, ei, stmt); //to support access by reference 
        e->setRhs(new SgVarRefExp(sp)); //replace subscript expression by 'sp' reference
        return(1);
    }

    return 1;
}

void LabelList(SgStatement *header)
{
    SgStatement *last, *stmt;

    last = header->lastNodeOfStmt();
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
    {
        if (stmt->hasLabel())
            proc_labels = addToLabelList(proc_labels, stmt->label());
    }
}

void SetScopeToLabels(SgStatement *header)
{
    SgStatement *last, *stmt;

    last = header->lastNodeOfStmt();
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
    {
        if (stmt->hasLabel())
            LABEL_SCOPE(stmt->label()->thelabel) = header->thebif;
    }
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//            I N L I N E    E X P A N S I O N
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

SgStatement *InlineExpansion(graph_node *gtop, SgStatement *stmt, SgSymbol *sf, SgExpression *args)
// return next processed statement in top level routine:
// first of inline expansion statements (inserted in top level routine)
//  or
// next statement following stmt in top level routine ( stmt->lexNext()), if it is not inlined call
{
    graph_node *gnode;
    SgStatement *header_tmplt, *global_st, *header_work, *calling_stmt, *expanded_stmt;
    SgSymbol *scopy;
    SgLabel *lab;
    /*
      if(!(pnode = ATTR_NODE(sf)))
      { printf("Error: NO ATTRIBUTE \n");
        return (stmt->lexNext());
      } else
        gnode = *pnode;
      if(!isInlinedCall(gtop,gnode))
         return(stmt->lexNext());
    */
    //gnode = getAttrNodeForSymbol(sf);
    if (deb_reg > 1)
        printf("INLINE EXPANSION %s \n", sf->identifier());
    if (!ATTR_NODE(sf))   // call without inline expansion (dummy argument, statement function) 15.03.07
        return(stmt->lexNext());
    gnode = getNodeForSymbol(gtop, sf->identifier());
    if (!gnode)
        return(stmt->lexNext());
    if (deb_reg > 1)
        printf("node %d for symbol %s\n", gnode->id, sf->identifier());
    //if(!isInlinedCallSite(stmt))  // if there is assertion (special comment) in program for call site 
      // return(stmt->lexNext());

    (gnode->count)++;
    // 1. if gnode is not template object 
    //    create a template inline object by performing site-independent transformations
    if (!gnode->tmplt)
        header_tmplt = CreateTemplate(gnode);

    // 2. clone the "template" inline object to create work inline object:  
    //    copying subprogram, inserting after global statement of file (in beginning of file)
    global_st = gtop->file->firstStatement();
    top_global = global_st;
    scopy = &((gnode->symb)->copySubprogram(*(global_st)));
    header_work = scopy->body(); //global_st->lexNext();


// 3. perform site_specific transformations
    if (stmt->variant() == ASSIGN_STAT)
        RemapFunctionResultVar(stmt->expr(0), scopy);
    ConformActualAndFormalParameters(scopy, args, stmt);

    // 4. transform all references to subprogram variables to "top level" form
    expanded_stmt = TranslateSubprogramReferences(header_work);

    // debugging
    if (deb_reg > 1)
        (gtop->file)->unparsestdout();
    if (deb_reg > 2)
    {
        printf("---------------------\n");
        expanded_stmt->unparsestdout();
        printf("---------------------\n");
        printf("\n");
    }
    // 5. replace the calling statement in the "top level" routine by transformed statements
    calling_stmt = stmt;
    /*  if(sf->variant() == FUNCTION_NAME)   //calling_stmt->variant()==ASSIGN_STAT
      {
           newst = new SgAssignStmt(*stmt->expr(0),*new SgVarRefExp(sf) );
           InsertNewStatementAfter(newst,stmt,stmt->controlParent());
      }
     */
    if (with_cmnt)
    {
        char *buf;
        buf = stmt->lexNext()->comments();
        BIF_CMNT(stmt->lexNext()->thebif) = NULL;
        Add_Comment(gnode, stmt->lexNext(), 1);
        stmt->lexNext()->addComment(buf);
    }
    InsertBlockAfter(stmt, expanded_stmt, header_work);

    if (with_cmnt)
    {
        expanded_stmt->addComment(stmt->comments());
        Add_Comment(gnode, expanded_stmt, 0);
    }
    lab = (stmt->hasLabel()) ? stmt->label() : NULL;
    if (lab)
    {
        if (expanded_stmt->hasLabel())
            InsertNewStatementBefore(new SgStatement(CONT_STAT), stmt);
        else
            BIF_LABEL(expanded_stmt->thebif) = lab->thelabel;
    }
    calling_stmt->extractStmt();

    // temporary !!!!
    //   return(stmt->lexNext());

    return(expanded_stmt);
}

void Add_Comment(graph_node *g, SgStatement *stmt, int flag)
{
    char *buf;
    buf = new char[80];
    if (!flag)
        sprintf(buf, "!*********INLINE EXPANSION %s[%d]*********\n", g->symb->identifier(), g->count);
    else
        sprintf(buf, "!*********END OF EXPANSION %s[%d]*********\n", g->symb->identifier(), g->count);
    stmt->addComment(buf);
}


graph_node *getNodeForSymbol(graph_node *gtop, char *name)
{
    edge *el;
    graph_node *nd;
    for (el = gtop->to_called; el; el = el->next)
    {
        if (!strcmp(el->to->symb->identifier(), name))
            return(el->to);
        else if ((nd = getNodeForSymbol(el->to, name)) != 0)
            return(nd);
    }
    return NULL;
}

graph_node *getAttrNodeForSymbol(SgSymbol *sf)
{
    graph_node *gnode, **pnode;
    if (!(pnode = ATTR_NODE(sf)))
    {
        printf("Warning: NO ATTRIBUTE FOR  %s\n", sf->identifier());
        gnode = NULL;
    }
    else
        gnode = *pnode;
    return(gnode);
}

int isInlinedCall(graph_node *gtop, graph_node *gnode)
{
    edge  *edgl;

    // testing incoming edge list of called routine graph-node: gnode 
    for (edgl = gnode->from_calling; edgl; edgl = edgl->next)
        if (edgl->from == gtop) //there is incoming edge: : gtop->[gnode] 
            return(1);
    return(0);
}

SgStatement * CreateTemplate(graph_node *gnode)
{ // Create a template inline object by performing site-independent transformations
    gnode->tmplt = 1;
    // routine cleaning
    RoutineCleaning(gnode->st_header);
    SetScopeToLabels(gnode->st_header);
    // site-independent transformation
    SiteIndependentTransformation(gnode);
    if (deb_reg > 1)
        printf("template for %s\n", gnode->st_header->symbol()->identifier());
    return(gnode->st_header);
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//    S I T E   I N D E P E N D E N T   T R A N S F O R M A T I O N S
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

void SiteIndependentTransformation(graph_node *gnode) //(SgStatement *header)

{// Perform site-independent transformation

    SgStatement *last, *first_executable, *last_declaration, *stmt, *return_st, *prev;
    SgStatement *header;
    SgLabel *lab_return;
    int has_return;
    stmt_list *DATA_list = NULL;
    header = gnode->st_header;
    last = header->lastNodeOfStmt();
    first_executable = NULL;
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
        if (isSgExecutableStatement(stmt) && stmt->variant() != FORMAT_STAT) {
            first_executable = stmt; break;
        }
    //last_declaration = first_executable->lexPrev();

   //----------------------------
   //Move all entry points to the top of the subprogram
    for (stmt = first_executable; stmt && (stmt != last); stmt = stmt->lexNext())
        if (stmt->variant() == ENTRY_STAT)
            MoveToTopOfRoutine(stmt, first_executable);

    //stmt_list *entryl;
    //for(entryl=entryst_list; entryl; entryl=entryl->next)
    //   if(entryl->st->controlParent() == header)
    //      MoveToTop(entryl->st, first_executable);
    //   else
    //      continue;

//----------------------------
//Move all return points to the bottom of the subprogram
    prev = last->lexPrev();
    return_st = NULL;
    lab_return = NULL;
    has_return = 0;
    if (prev->variant() == RETURN_STAT && prev->controlParent()->variant() != LOGIF_NODE)
    {
        return_st = prev;
        if (return_st->hasLabel())
            lab_return = return_st->label();
    }
    if (!lab_return)
    {
        lab_return = NewLabel();
        SetScopeOfLabel(lab_return, header);
    }

    for (stmt = first_executable; stmt && (stmt != return_st) && (stmt != last); stmt = stmt->lexNext())
        if (stmt->variant() == RETURN_STAT)
        {
            stmt = ReplaceByGoToBottomOfRoutine(stmt, lab_return);
            has_return = 1;
        }
    if (has_return)
    {
        if (!return_st)
        {
            stmt = new SgStatement(CONT_STAT);
            InsertNewStatementBefore(stmt, last);
            stmt->setLabel(*lab_return);
        }
        else
        {
            return_st->setLabel(*lab_return);
            ReplaceReturnByContinue(return_st);
        }
    }
    else if (return_st)
        ReplaceReturnByContinue(return_st);

    //----------------------------
    //Substitute all integer symbolic constants in subprogram
    IntegerConstantSubstitution(header);

    //----------------------------
    //Move all FORMAT statements into the top level routine
    format_labels = NULL;
    for (stmt = header; stmt && (stmt != last); )
        if (stmt->variant() == FORMAT_STAT)
            //MoveFormatToTopOfRoutine(stmt, last_declaration);
            stmt = MoveFormatIntoTopLevel(stmt, gnode->clone);
        else if (stmt->variant() == DATA_DECL)
        {
            DATA_list = addToStmtList(DATA_list, stmt);
            stmt = stmt->lexNext();
            //!!!!
            Error("DATA statement in procedure %s. Sorry, not implemented yet", header->symbol()->identifier(), 1, stmt);
        }
        else
            stmt = stmt->lexNext();
    ReplaceFormatLabelsInStmts(header);
    //----------------------------
    //Precalculate all of the subprogram's adjustable array bounds
    last_declaration = first_executable->lexPrev();

    AdjustableArrayBounds(header, last_declaration);
    first_executable = last_declaration->lexNext();
    //---------------------------- 
    //Replace each reference to whole formal array in I/O statements
    //by implied DO-loop 
    ReplaceWholeArrayRefInIOStmts(header);
    //---------------------------- 
    //Remap all local subprogram variables by creating new unconflicting top level variables
    top_symb_list = CreateListOfLocalVariables(top_header);
    sub_symb_list = CreateListOfLocalVariables(header);
    //PrintTopSymbList();

    //PrintSymbList(sub_symb_list, header);


    RemapConstants(header, first_executable);
    RemapLocalVariables(header);

    //---------------------------- 
    //Remap COMMON bloks
    CreateTopCommonBlockList();
    RemapCommonBlocks(header);
    //----------------------------
    //Remap EQUIVALENCE blocks 
    //---------------------------- 
    //Move all DATA statements into top level routine
      //DATA_list has been created: list of DATA statements
      // internal form of DATA statement must be changed in parser and unparser
      //if(DATA_list)   // temporary !!!
        //printf("There are DATA statements in procedure. Sorry, not implemented yet \n" );

}

void MoveToTopOfRoutine(SgStatement *entrystmt, SgStatement *first_executable)
{//Move entry point to the top of the subprogram
 // generate GO TO statement (will be removed after expansion)
    SgStatement *go_to;
    SgLabel *entry_lab;

    if (!entrystmt->lexNext()->hasLabel())
    {
        entry_lab = NewLabel();
        SetScopeOfLabel(entry_lab, entrystmt->controlParent());
        entrystmt->lexNext()->setLabel(*entry_lab);
    }
    else
        entry_lab = entrystmt->lexNext()->label();
    go_to = new SgGotoStmt(*entry_lab);
    entrystmt->extractStmt();
    InsertNewStatementBefore(entrystmt, first_executable);
    InsertNewStatementAfter(go_to, entrystmt, entrystmt->controlParent());
}

//-------------------------------------------------------------------------------------------
SgStatement *ReplaceByGoToBottomOfRoutine(SgStatement *retstmt, SgLabel *lab_return)
{//Replace return point by goto to the bottom of the subprogram
 // generate GO TO statement 
    SgStatement *go_to;
    go_to = new SgGotoStmt(*lab_return);
    InsertNewStatementBefore(go_to, retstmt);
    retstmt->extractStmt();
    return(go_to);
}

void ReplaceReturnByContinue(SgStatement *return_st)
{
    InsertNewStatementBefore(new SgStatement(CONT_STAT), return_st);
    return_st->extractStmt();
}

//-------------------------------------------------------------------------------------------
void MoveFormatToTopOfRoutine(SgStatement *format_stmt, SgStatement *last_declaration)
{//Move FORMAT statements  to the top of the subprogram
    SgLabel *format_lab;
    // SgLabel *label_insection[200];

    if (format_stmt->hasLabel())
    {
        format_lab = format_stmt->label();
        if (!TestFormatLabel(format_stmt->label()))
        {
            format_lab = NewLabel();
            format_stmt->setLabel(*format_lab);
        }
        format_stmt->extractStmt();
        InsertNewStatementAfter(format_stmt, last_declaration, last_declaration->controlParent());
        last_declaration = format_stmt;
    }
}

SgStatement *MoveFormatIntoTopLevel(SgStatement *format_stmt, int clone)
{
    SgStatement *next;
    SgLabel *format_lab;
    next = format_stmt->lexNext();
    format_lab = format_stmt->label();
    if (!clone && isLabelOfTop(format_stmt->label()))
    {
        if (deb_reg > 2)
            printf("new label: %d -> ", (int)LABEL_STMTNO(format_lab->thelabel));
        format_labels = addToLabelList(format_labels, format_lab);
        format_lab = NewLabel();
        format_stmt->setLabel(*format_lab);
        format_labels->newlab = format_lab;
        if (deb_reg > 2)
            printf(" %d\n", (int)LABEL_STMTNO(format_lab->thelabel));
    }

    format_stmt->extractStmt();
    InsertNewStatementAfter(format_stmt, top_last_declaration, top_header);
    SetScopeOfLabel(format_lab, top_header);
    //top_last_declaration = format_stmt;

    return(next);
}

label_list  *addToLabelList(label_list *lablist, SgLabel *lab)
{
    // adding the label to the beginning of label list 

    label_list * nl;
    if (!lablist) {
        lablist = new label_list;
        lablist->lab = lab;
        lablist->next = NULL;
    }
    else {
        nl = new label_list;
        nl->lab = lab;
        nl->next = lablist;
        lablist = nl;
    }
    return (lablist);
}

int isInLabelList(SgLabel *lab, label_list *lablist)
{
    label_list *ll;
    for (ll = lablist; ll; ll = ll->next)
        if (LABEL_STMTNO(ll->lab->thelabel) == LABEL_STMTNO(lab->thelabel))
            return(1);
    return(0);
}

int isLabelOfTop(SgLabel *lab)
{
    return(isLabelWithScope(LABEL_STMTNO(lab->thelabel), top_header) != NULL);
}

void ReplaceFormatLabelsInStmts(SgStatement *header)
{
    SgStatement *stmt, *last;
    if (!format_labels)
        return;
    if (deb_reg > 2)
        printf("replace format labels in %s\n", header->symbol()->identifier());
    last = header->lastNodeOfStmt();
    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
    {
        switch (stmt->variant())
        {
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
        { SgKeywordValExp *kwe;
        SgExpression *e, *ee, *el, *fmt;
        fmt = NULL;
        e = stmt->expr(1); // IO control list
        if (e->variant() == SPEC_PAIR)
        {
            if (stmt->variant() == PRINT_STAT)
                fmt = e;
            else
            {
                kwe = isSgKeywordValExp(e->lhs());
                if (!kwe)
                    break;
                if (!strcmp(kwe->value(), "fmt"))
                    fmt = e;
                else
                    break;;
            }
        }
        else if (e->variant() == EXPR_LIST)
        {
            for (el = e; el; el = el->rhs())
            {
                ee = el->lhs();
                if (ee->variant() != SPEC_PAIR)
                    break; // IO_control list error
                kwe = isSgKeywordValExp(ee->lhs());
                if (!kwe)
                    break;
                if (!strcmp(kwe->value(), "fmt"))
                {
                    fmt = ee;
                    break;
                }
            }
        }
        else
            break;

        // analis  fmt
        { SgLabel *lab, *newlab;
        lab = NULL;
        if (deb_reg > 2)
            printf("fmt variant %d\n", fmt->rhs()->variant());
        if (fmt && fmt->rhs()->variant() == LABEL_REF)
        {
            lab = ((SgLabelRefExp *)(fmt->rhs()))->label();
            if (deb_reg > 2)
                printf("label [%d] \n", lab->id());
        }
        else if (fmt && fmt->rhs()->variant() == INT_VAL)  //!!!parser error
        {
            if (deb_reg > 2)
                printf("variant fmt = %d  %d\n", fmt->rhs()->variant(), ((SgValueExp *)(fmt->rhs()))->intValue());
            lab = isLabelWithScope(((SgValueExp *)(fmt->rhs()))->intValue(), header);
            if (lab)
                fmt->setRhs(new SgLabelRefExp(*lab));
        }
        if (!lab)  break;
        //printf("label [%d] %d\ n",lab->id(),LABEL_STMTNO(lab->thelabel));
      // replace label in fmt->lhs() 
        if ((newlab = isInFormatMap(lab)) != NULL)
            NODE_LABEL(fmt->rhs()->thellnd) = newlab->thelabel;
        }
        }
        break;
        default:
            break;
        }
    }
    return;
}

SgLabel *isInFormatMap(SgLabel *lab)
{
    label_list *ll;
    for (ll = format_labels; ll; ll = ll->next)
    {
        if (ll->lab == lab)
            return(ll->newlab);
    }
    return(NULL);
}

//-------------------------------------------------------------------------------------------
void AdjustableArrayBounds(SgStatement *header, SgStatement *after)
{
    int npar, i, j, rank;
    SgExpression *bound;
    SgSymbol *param;

    cur_func = header;
    npar = ((SgProgHedrStmt *)header)->numberOfParameters();
    for (i = 0; i < npar; i++)
    {
        param = ((SgProgHedrStmt *)header)->parameter(i);
        if (isSgArrayType(param->type()))  // is array
        {
            rank = Rank(param);
            for (j = 0; j < rank; j++)
            {
                if (isAdustableBound(bound = LowerBound(param, j)))
                    PrecalculateArrayBound(param, bound, after, header);

                if (isAdustableBound(bound = UpperBound(param, j)))
                    PrecalculateArrayBound(param, bound, after, header);
            } //end for j
        }
    } // end for i
}

int isAdustableBound(SgExpression *bound)
{
    if (!bound)
        return 0;
    if (bound->variant() == INT_VAL)
        return 0;
    return(SearchVarRef(bound));
}

int SearchVarRef(SgExpression *e)
{
    if (!e)
        return 0;
    if (isSgVarRefExp(e) && e->symbol()->variant() == VARIABLE_NAME)
        return 1;
    if (SearchVarRef(e->lhs()) || SearchVarRef(e->rhs()))
        return 1;
    else
        return 0;
}
void PrecalculateArrayBound(SgSymbol *ar, SgExpression *bound, SgStatement *after, SgStatement *header)

{
    SgStatement *newst;
    SgSymbol *sb;
    SgExpression **pbe = new (SgExpression *);

    sb = GetTempVarForBound(ar);
    newst = new SgAssignStmt(*new SgVarRefExp(sb), bound->copy());
    InsertNewStatementAfter(newst, after, header);
    *pbe = new SgVarRefExp(sb);
    bound->addAttribute(PRE_BOUND, (void *)pbe, sizeof(SgExpression *));

    return;
}

//-------------------------------------------------------------------------------------------
void ReplaceWholeArrayRefInIOStmts(SgStatement *header)
{
    SgStatement *stmt, *last;
    SgExpression *iol, *e;

    cur_func = header;

    last = header->lastNodeOfStmt();

    for (stmt = header; stmt && (stmt != last); stmt = stmt->lexNext())
    {
        switch (stmt->variant())
        {
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
            iol = stmt->expr(0); //input-output list
            for (; iol; iol = iol->rhs())
            {
                e = iol->lhs();  // list item
                if (isSgArrayRefExp(e) && isSgArrayType(e->symbol()->type()) && !e->lhs() && isDummyArgument(e->symbol())) //whole formal array ref
                    iol->setLhs(ImplicitLoop(e->symbol()));
            }
            break;
        default:
            break;
        }
    } //end for
}


SgExpression *ImplicitLoop(SgSymbol *ar)
{
    SgExpression *ei[10];
    SgArrayRefExp *eref;
    int rank, i;

    rank = Rank(ar);
    for (i = 0; i < rank; i++)
        if (!do_var[i])
        {
            do_var[i] = GetImplicitDoVar(i);
            MakeDeclarationStmtInTop(do_var[i]);
        }
    //ei[0] = new SgIOAccessExp(*do_var[0], *LowerLoopBound(ar,0), *UpperLoopBound(ar,0));
    ei[0] = new SgExpression(IOACCESS);
    ei[0]->setSymbol(do_var[0]);
    ei[0]->setRhs(new SgExpression(SEQ, new SgExpression(DDOT, LowerLoopBound(ar, 0), UpperLoopBound(ar, 0), NULL), NULL, NULL));
    eref = new SgArrayRefExp(*ar);
    for (i = 0; i < rank; i++)
        eref->addSubscript(*new SgVarRefExp(do_var[i]));
    ei[0]->setLhs(new SgExprListExp(*eref));

    for (i = 1; i < rank; i++)
    {  //ei[i] = new SgIOAccessExp(*si[i], LowerBound(ar,i)->copy(), UpperBound(ar,i)->copy());
        ei[i] = new SgExpression(IOACCESS);
        ei[i]->setSymbol(do_var[i]);
        ei[i]->setRhs(new SgExpression(SEQ, new SgExpression(DDOT, LowerLoopBound(ar, i), UpperLoopBound(ar, i), NULL), NULL, NULL));
        ei[i]->setLhs(new SgExprListExp(*ei[i - 1]));
    }
    return(ei[rank - 1]);
}

SgExpression * LowerLoopBound(SgSymbol *ar, int i)
{
    SgExpression *e;
    e = LowerBound(ar, i);
    if (PREBOUND(e))
        e = *PREBOUND(e);
    return(&(e->copy()));
}

SgExpression * UpperLoopBound(SgSymbol *ar, int i)
{
    SgExpression *e;
    e = UpperBound(ar, i);
    if (PREBOUND(e))
        e = *PREBOUND(e);
    return(&(e->copy()));
}


//-------------------------------------------------------------------------------------------
void RemapConstants(SgStatement *header, SgStatement *first_exec)
{
    SgStatement *stmt;
    common_list = common_list_l = NULL;
    equiv_list = equiv_list_l = NULL;
    for (stmt = header; stmt && (stmt != first_exec); stmt = stmt->lexNext())
    {
        switch (stmt->variant())
        {
        case PARAM_DECL:
        {SgExpression *el;
        for (el = stmt->expr(0); el; el = el->rhs())
        {
            RemapLocalObject(el->lhs()->symbol());
        }
        continue;
        }
        case COMM_STAT:
            CommonBlockList(stmt);
            continue;
        case EQUI_STAT:
            EquivBlockList(stmt);
            continue;

        default:
            continue;
        }
    }
}

void RemapLocalVariables(SgStatement *header)
{
    SgSymbol  *s;
    for (s = sub_symb_list; s; s = NextSymbol(s))
    {  //printf("*****%s\n",s->identifier());
        if (s->variant() == CONST_NAME)
            continue;
        if (IN_COMMON(s))
            continue;

        RemapLocalObject(s);
    }
}

/*
void RemapLocalVariables(SgStatement *header)
{ SgSymbol *symb_list, *s, *ts, *snew;
  int is_in_top;
  top_symb_list = CreateListOfLocalVariables(top_header);
      symb_list = CreateListOfLocalVariables(header);
  for(s=symb_list; s; s=NextSymbol(s) )
  {  //printf("*****%s\n",s->identifier());
     RemapLocalObject(s);
       if(isDummyArgument(s))
        continue;
     if(s->variant() == CONST_NAME && s->type()->variant() == T_INT)
        continue;
     is_in_top = 0;
     for(ts=top_symb_list; ts; ts=NextSymbol(ts) )
     {
        if(!strcmp(s->identifier(),ts->identifier()))
          {is_in_top = 1; break;}
     }
     if(is_in_top)
     {
        if((s->variant()==CONST_NAME) && (ts->variant()==CONST_NAME) && CompareConstants(s,ts)) // is the same constant
        {  s->thesymb->entry.Template.declared_name = ts->thesymb;    // symbol map
             continue;
        }
        else
        { snew = GetNewTopSymbol(s);  //create new symbol of top_header scope
          s->thesymb->entry.Template.declared_name = snew->thesymb;    // symbol map
        }
     }
     else
     {  snew = s;
        SYMB_SCOPE(snew->thesymb) = top_header->thebif; //move symbol into top level routine
     }
     if(snew->variant() == CONST_NAME)
       MakeDeclarationStmtsForConstant(snew);
     else
       MakeDeclarationStmtInTop(snew);

  }

}
*/

void RemapLocalObject(SgSymbol *s)
{
    int is_in_top, md;
    SgSymbol *ts, *snew;

    if (isDummyArgument(s))
        return;
    if (s->variant() == CONST_NAME && s->type()->variant() == T_INT)
        return;
    if (s->variant() == CONST_NAME)
        TranslateExpression(((SgConstantSymb *)s)->constantValue(), &md);

    is_in_top = 0;
    for (ts = top_symb_list; ts; ts = NextSymbol(ts))
    {
        if (!strcmp(s->identifier(), ts->identifier()))
        {
            is_in_top = 1; break;
        }
    }
    if (is_in_top)
    {
        if ((s->variant() == CONST_NAME) && (ts->variant() == CONST_NAME) && CompareConstants(s, ts)) // is the same constant
        {
            s->thesymb->entry.Template.declared_name = ts->thesymb;    // symbol map
            return;
        }
        else
        {
            snew = GetNewTopSymbol(s);  //create new symbol of top_header scope
            s->thesymb->entry.Template.declared_name = snew->thesymb;    // symbol map
        }
    }
    else
    {
        snew = s;
        SYMB_SCOPE(snew->thesymb) = top_header->thebif; //move symbol into top level routine
    }
    if (snew->variant() == CONST_NAME)
        MakeDeclarationStmtsForConstant(snew);
    else
        MakeDeclarationStmtInTop(snew);

}

void RemapCommonObject(SgSymbol *s, SgSymbol *tops)
{
    s->thesymb->entry.Template.declared_name = tops->thesymb;    // symbol map
}

SgSymbol *CreateListOfLocalVariables(SgStatement *header)
{
    SgSymbol *s, *first, *symb_list;
    //first = header->symbol(); 
    first = (header == top_header) ? top_node->file->firstSymbol() : header->symbol();
    symb_list = NULL;
    for (s = first; s; s = s->next())
        if (SYMB_SCOPE(s->thesymb) == header->thebif) //if( s->scope() == header )
        {
            SYMB_LIST(s->thesymb) = symb_list ? symb_list->thesymb : NULL;    //s->thesymb->id_list
            symb_list = s;
        }

    return symb_list;
}

SgSymbol *NextSymbol(SgSymbol *s)
{
    return(SymbMapping(SYMB_LIST(s->thesymb)));
}

void MakeDeclarationStmtInTop(SgSymbol *s)
{
    SgStatement *st;
    st = s->makeVarDeclStmt();
#if __SPF
    insertBfndListIn(st->thebif, top_last_declaration->thebif, NULL);
#else
    top_last_declaration->insertStmtAfter(*st);
#endif
    top_last_declaration = st;
    if (IS_ALLOCATABLE(s)) {
        SgDeclarationStatement *allocatableStmt = new SgDeclarationStatement(ALLOCATABLE_STMT);
        SgVarRefExp *expr = new SgVarRefExp(s);
        SgExprListExp *list = new SgExprListExp(*expr);
        allocatableStmt->setExpression(0, *list);
#if __SPF        
        BIF_CP(allocatableStmt->thebif) = top_last_declaration->controlParent()->thebif;
#else
        allocatableStmt->setControlParent(top_last_declaration->controlParent());
#endif

#if __SPF
        insertBfndListIn(allocatableStmt->thebif, top_last_declaration->thebif, NULL);
#else
        top_last_declaration->insertStmtAfter(*allocatableStmt);
#endif
        top_last_declaration = allocatableStmt;
    }
}
void MakeDeclarationStmtsForConstant(SgSymbol *s)
{
    SgStatement *st;
    SgExpression *eel;
    st = new SgStatement(PARAM_DECL);
    eel = new SgExprListExp(*new SgRefExp(CONST_REF, *((SgConstantSymb *)s)));
    eel->setRhs(NULL);
    st->setExpression(0, *eel);
#if __SPF
    insertBfndListIn(st->thebif, top_last_declaration->thebif, NULL);
#else
    top_last_declaration->insertStmtAfter(*st);
#endif
    //top_header -> insertStmtAfter(*st);
    st = s->makeVarDeclStmt();
    //top_header -> insertStmtAfter(*st);
#if __SPF
    insertBfndListIn(st->thebif, top_last_declaration->thebif, NULL);
#else
    top_last_declaration->insertStmtAfter(*st);
#endif
    top_last_declaration = st->lexNext();
}
// SgConstantSymb * sc =  isSgConstantSymb(e->symbol());
//  return(ReplaceIntegerParameter(&(sc->constantValue()->copy())));

int CompareConstants(SgSymbol *rs, SgSymbol *ts)
{
    PTR_LLND cers, cets;
    int ic;
    cers = SYMB_VAL(rs->thesymb);
    cets = SYMB_VAL(ts->thesymb);
    if (cers->variant != cets->variant)
        return(0);

    /*
     if(cers->variant==FLOAT_VAL || cers->variant==DOUBLE_VAL || cers->variant==STRING_VAL)
     {   if(!strcmp(NODE_STR(cers),NODE_STR(cets)) )
           return(1);
         else
           return(0);
     }
     if(cers->variant==COMPLEX_VAL) {
         int icm;
         icm = CompareConstants(NODE_TEMPLATE_LL1(cers)) && CompareConstants(cers->rhs());
         return(icm);
     }
     if(cers->variant==BOOL_VAL)
         if(NODE_BV(cers) == NODE_BV(cets))
           return(1);
         else
           return(0);
     return(0);
    */

    ic = 0;
    switch (cers->variant)
    {
    case (FLOAT_VAL):
    case (DOUBLE_VAL):
    case (STRING_VAL):
        if (!strcmp(NODE_STR(cers), NODE_STR(cets)))
            ic = 1;
        break;
    case (BOOL_VAL):
        if (NODE_BV(cers) == NODE_BV(cets))
            ic = 1;;
        break;
    case (COMPLEX_VAL):
        ic = CompareValues(NODE_TEMPLATE_LL1(cers), NODE_TEMPLATE_LL1(cets)) && CompareValues(NODE_TEMPLATE_LL2(cers), NODE_TEMPLATE_LL2(cets));
        break;
    default:
        break;
    }
    return (ic);
}

int CompareValues(PTR_LLND pe1, PTR_LLND pe2)
{
    if (pe1->variant != pe2->variant)
        return(0);
    if ((pe1->variant != FLOAT_VAL) && (pe1->variant != DOUBLE_VAL))
        return(0);
    if (!strcmp(NODE_STR(pe1), NODE_STR(pe2)))
        return(1);
    return(0);
}

void CommonBlockList(SgStatement *stmt)
{
    SgExpression *ec, *el;
    SgSymbol *sc;
    for (ec = stmt->expr(0); ec; ec = ec->rhs()) // looking through COMM_LIST
    { //if(isInCommonList(common_list->block->symbol(),common_list)
        common_list_l = AddToBlockList(common_list_l, ec);
        if (!common_list) common_list = common_list_l;
        for (el = ec->lhs(); el; el = el->rhs())
        {
            sc = el->lhs()->symbol();
            //if(sc && ((sc->attributes() & ALIGN_BIT) || (sc->attributes() & DISTRIBUTE_BIT)) )
              // el->lhs()->setLhs(NULL);  
            if (sc)
                SYMB_ATTR(sc->thesymb) = SYMB_ATTR(sc->thesymb) | COMMON_BIT;
        }
    }
}

void TopCommonBlockList(SgStatement *stmt)
{
    SgExpression *ec, *el;
    SgSymbol *sc;
    for (ec = stmt->expr(0); ec; ec = ec->rhs()) // looking through COMM_LIST
    {
        top_common_list_l = AddToBlockList(top_common_list_l, ec);
        if (!top_common_list) top_common_list = top_common_list_l;
        for (el = ec->lhs(); el; el = el->rhs())
        {
            sc = el->lhs()->symbol();
            //if(sc && ((sc->attributes() & ALIGN_BIT) || (sc->attributes() & DISTRIBUTE_BIT)) )
              // el->lhs()->setLhs(NULL);  
            if (sc)
                SYMB_ATTR(sc->thesymb) = SYMB_ATTR(sc->thesymb) | COMMON_BIT;
        }
    }
}

void CreateTopCommonBlockList()
{
    SgStatement *stmt;
    top_common_list = top_common_list_l = NULL;
    top_equiv_list = top_equiv_list_l = NULL;
    for (stmt = top_header; stmt && (stmt != top_first_executable); stmt = stmt->lexNext())
    {
        switch (stmt->variant())
        {
        case COMM_STAT:
            TopCommonBlockList(stmt);
            continue;
        case EQUI_STAT:
            //TopEquivBlockList(stmt);
            continue;

        default:
            continue;
        }
    }
}


block_list *AddToBlockList(block_list *blist_last, SgExpression *eb)
{
    block_list * bl;
    bl = new block_list;
    bl->block = eb;
    bl->next = NULL;
    if (!blist_last) {
        blist_last = bl;
    }
    else {
        blist_last->next = bl;
        blist_last = bl;
    }
    return(blist_last);
}

void EquivBlockList(SgStatement *stmt)
{
    SgExpression *ec;
    // SgSymbol *sc; 
    for (ec = stmt->expr(0); ec; ec = ec->rhs()) // looking through LIST
    {
        equiv_list_l = AddToBlockList(equiv_list_l, ec);
        if (!equiv_list) equiv_list = equiv_list_l;
    }
}

void RemapCommonBlocks(SgStatement *header)
{
    block_list *bl, *topbl;
    SgStatement *com;
    SgExpression *tl, *rl;
    SgSymbol *tops = NULL;
    //int md[1];
   // for each subprogram COMMON block 
    for (bl = common_list; bl; bl = bl->next)
        if (!(topbl = isConflictingCommon(bl->block->symbol())))  //unconflicting common
        {  //bl->block->lhs()->unparsestdout();
            RemapCommonList(bl->block->lhs());
            EditExpressionList(bl->block->lhs());
            TranslateExpressionList(bl->block->lhs());
            //bl->block->lhs()->unparsestdout();
            com = DeclaringCommonBlock(bl->block); //creating new COMMON statement and inserting one in top routine
#if __SPF
            insertBfndListIn(com->thebif, top_last_declaration->thebif, NULL);
#else
            top_last_declaration->insertStmtAfter(*com);
#endif
            top_last_declaration = com;
        }
        else
        {
            tl = topbl->block->lhs();
            rl = bl->block->lhs();
            while (tl && rl)
            {
                if (!areOfSameType(tl->lhs()->symbol(), rl->lhs()->symbol()))
                {
                    Error("COMMON block in procedure  %s  with unconformable reference. Sorry, not implemented yet", header->symbol()->identifier(), 1, header);  //tops = generate an equivalenced top level variable
                    printf("%s    %s\n", tl->lhs()->symbol()->identifier(), rl->lhs()->symbol()->identifier());
                }
                else
                    tops = tl->lhs()->symbol();
                RemapCommonObject(rl->lhs()->symbol(), tops); //!!! remake after realizing CalculateTopLevelRef()
                CalculateTopLevelRef(tops, tl->lhs(), rl->lhs());
                MakeRefsConformable(tl->lhs(), rl->lhs());
                tl = tl->rhs();
                rl = rl->rhs();
            }
        }
}
void RemapCommonList(SgExpression *el)
{
    SgExpression *coml;
    coml = el;
    while (coml)
    {
        RemapLocalObject(coml->lhs()->symbol());
        coml = coml->rhs();
    }
}

int areOfSameType(SgSymbol *st, SgSymbol *sr)
{
    int res;
    SgType *tt, *rt;
    tt = BaseType(st->type());
    rt = BaseType(sr->type());
    res = tt->variant() == rt->variant() && TypeSize(tt) && TypeSize(tt) == TypeSize(rt);
    return(res);
}

int IntrinsicTypeSize(SgType *t)
{
    switch (t->variant()) {
    case T_INT:
    case T_BOOL:     return (4);
    case T_FLOAT:    return (4);
    case T_COMPLEX:  return (8);
    case T_DOUBLE:   return (8);

    case T_DCOMPLEX: return(16);

    case T_STRING:
    case T_CHAR:
        return(1);
    default:
        return(0);
    }
}

int TypeSize(SgType *t)
{
    //SgExpression *le;
    int len;
    if (!TYPE_RANGES(t->thetype) && !TYPE_KIND_LEN(t->thetype))       return (IntrinsicTypeSize(t));

    if ((len = TypeLength(t)))    return(len);

    //le = TypeLengthExpr(t);
    //if(le->isInteger()){
    //  len = le->valueInteger();
    //  len = len < 0 ? 0 : len; //according to standard F90
    //} else
    //  len = -1; //may be error situation

    return(0);
}

int TypeLength(SgType *t)
{
    SgExpression *le;
    SgValueExp *ve;
    //if(t->variant() == T_STRING)   return (0);
    if (TYPE_RANGES(t->thetype)) {
        le = t->length();
        if ((ve = isSgValueExp(le)))
            return (ve->intValue());
        else
            return (0);
    }
    if (TYPE_KIND_LEN(t->thetype)) {        /*22.04.14*/
        le = t->selector()->lhs();
        if ((ve = isSgValueExp(le)))
            if (t->variant() == T_COMPLEX || t->variant() == T_DCOMPLEX)
                return (2 * ve->intValue());
            else
                return (ve->intValue());
        else
            return (0);
    }

    return(0);
}

SgType *BaseType(SgType *type)
{
    return (isSgArrayType(type) ? type->baseType() : type);
}

int isUnconflictingCommon(SgSymbol *s)
{
    block_list *bl;
    for (bl = top_common_list; bl; bl = bl->next)
        if (bl->block->symbol() == s)
            return(0);
    return(1);
}

block_list *isConflictingCommon(SgSymbol *s)
{
    block_list *bl;
    //printSymb(s);
    //printf("  variant %d\n",s->variant());
    for (bl = top_common_list; bl; bl = bl->next) {
        //if(bl && bl->block ) printSymb(bl->block->symbol());
        if (bl->block->symbol() == s)
            return(bl);
    }
    //printf("NO\n");
    return(NULL);
}

block_list *isInCommonList(SgSymbol *s, block_list *blc)
{
    block_list *bl;
    for (bl = blc; bl; bl = bl->next)
        if (bl->block->symbol() == s)
            return(bl);
    return(NULL);
}


SgStatement *DeclaringCommonBlock(SgExpression *bl)
{
    SgStatement *com;
    //SgExpression *eeq;
        // eeq = new SgExpression (COMM_LIST);
        // eeq -> setSymbol(*bl->symbol());
        // eeq -> setLhs(*bl->lhs());
        // com = new SgStatement(COMM_STAT);
        // com->setExpression(0,*eeq);
    com = new SgStatement(COMM_STAT);
    com->setExpression(0, *bl);

    return(com);
}
//-------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//    S I T E - S P E C I F I C    T R A N S F O R M A T I O N S
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

void RemapFunctionResultVar(SgExpression *topref, SgSymbol *sf)
{
    SgSymbol *topvar;
    topvar = topref->symbol();
    sf->thesymb->entry.Template.declared_name = topvar->thesymb;    // symbol map
    if (isSgArrayRefExp(topref) && topref->lhs())
        sf->addAttribute(ARRAY_MAP_1, (void *)topref, 0);
}

void ConformActualAndFormalParameters(SgSymbol *scopy, SgExpression *args, SgStatement *parentSt)
{
    PTR_SYMB  dummy;
    SgSymbol *darg;
    SgExpression *fact, *farglist;
    //int cnf_type;
    int adj;
    adj = 0;
    farglist = args;
    dummy = scopy->thesymb->entry.proc_decl.in_list;
    /*
      if(!dummy) return;
        printf("dummy of %s: %s\n",scopy->identifier(),dummy->ident);
      next = dummy->entry.var_decl.next_in ;
      while(next)
      {   //if(!next) return;
        printf("dummy of %s: %s\n",scopy->identifier(),next->ident);
        next = next->entry.var_decl.next_in ;
      }
   */


   // alternative return, dummy is *, represented by symbol with kind DEFAULT and name "*" !!!!????

    while (dummy && farglist)
    {  // printf("dummy of %s: %s\n",scopy->identifier(),dummy->ident);
        fact = farglist->lhs();
        darg = SymbMapping(dummy);
        if (isAdjustableArray(darg))
        {
            adj = 1;
            darg->addAttribute(ADJUSTABLE_, (void *)fact, 0);
        }
        else
            ConformReferences(darg, fact, parentSt);
        dummy = dummy->entry.var_decl.next_in;
        farglist = farglist->rhs();
    }
    dummy = scopy->thesymb->entry.proc_decl.in_list;
    while (adj && dummy)
    {
        darg = SymbMapping(dummy);
        if ((fact = ADJUSTABLE(darg)))
        {
            TranslateArrayTypeExpressions(darg);
            ConformReferences(darg, fact, parentSt);
        }
        dummy = dummy->entry.var_decl.next_in;
    }

}

void ConformReferences(SgSymbol *darg, SgExpression *fact, SgStatement *parentSt)
{
    int cnf_type;

    cnf_type = TestConformability(darg, fact, parentSt);
    if (!cnf_type)
    {
        Error("Non conformable %s. Case not implemented yet", darg->identifier(), 1, parentSt); // not realized
        //fact->unparsestdout(); printf("\n"); darg->scope()->unparsestdout();
        if (deb_reg)
            printf("Non conformable. Case not implemented yet\n");
    }

    switch (cnf_type)
    {
    case _IDENTICAL_:
        darg->thesymb->entry.Template.declared_name = fact->symbol()->thesymb;
        break;

    case SCALAR_ARRAYREF:
        darg->thesymb->entry.Template.declared_name = fact->symbol()->thesymb;
        darg->addAttribute(ARRAY_MAP_1, (void *)fact, 0);
        break;

    case _SUBARRAY_:
        darg->thesymb->entry.Template.declared_name = fact->symbol()->thesymb;
        darg->addAttribute(ARRAY_MAP_1, (void *)(fact->lhs()), 0);
        break;
    case _CONSTANT_:
        darg->addAttribute(CONSTANT_MAP, (void *)fact, 0);
        break;
    case VECTOR_ARRAYREF:
        darg->thesymb->entry.Template.declared_name = fact->symbol()->thesymb;
        //if(fact->lhs()->lhs())
        darg->addAttribute(ARRAY_MAP_2, (void *)(fact->lhs()), 0);
        break;
    case _ARRAY_:
        break;
    }
}

int isAdjustableArray(SgSymbol *param)
{
    int rank, j;
    if (!isSgArrayType(param->type()))
        return(0);
    rank = Rank(param);
    for (j = 0; j < rank; j++)
    {
        if (isAdustableBound(LowerBound(param, j)))
            return(1);;

        if (isAdustableBound(UpperBound(param, j)))
            return(1);;
    }
    return(0);
}

SgSymbol *FirstDummy(SgSymbol *sf)
{
    return(SymbMapping(sf->thesymb->entry.proc_decl.in_list));
}


SgSymbol *NextDummy(SgSymbol *s)
{    
    return(SymbMapping(s->thesymb->entry.var_decl.next_in));
}

int TestConformability(SgSymbol *darg, SgExpression *fact, SgStatement *parentSt)
{
    SgArrayType *ftp;

    if (isFormalProcedure(darg))
        return(_IDENTICAL_);

    if (!SameType(darg, fact))
        return(NON_CONFORMABLE);

    if (isSgValueExp(fact))
        return(_CONSTANT_);

    if (isScalar(darg))
    {  //printf("scalar %s(%d): %s\n", darg->identifier(),darg->variant(),fact->symbol()->identifier());
        if (isSgArrayRefExp(fact) && fact->lhs() && !isSgArrayType(fact->type()))
            return(SCALAR_ARRAYREF);
        else
            return(_IDENTICAL_);
    }

    if (isArray(darg))
    {  //printf("array %s(%d): %s\n", darg->identifier(),darg->variant(),fact->symbol()->identifier()); 
        if ((ftp = isSgArrayType(fact->symbol()->type())) && fact->lhs() && TestShapes(ftp, (SgArrayType *)(darg->type())) && TestBounds(fact, ftp, (SgArrayType *)(darg->type())))
            return(_SUBARRAY_);
        if ((ftp = isSgArrayType(fact->symbol()->type())) && fact->lhs() && TestVector(fact, ftp, (SgArrayType *)(darg->type())))
            return(VECTOR_ARRAYREF);

        if ((ftp = isSgArrayType(fact->symbol()->type())) && !fact->lhs() && SameShapes(ftp, (SgArrayType *)(darg->type())))
            return(_IDENTICAL_);

    }
    Error("TestConformability(%s,...). Case not implemented yet", darg->identifier(), 1, parentSt);
    if (deb_reg)
        printf("TestConformability(). Case not implemented yet\n");
    return(NON_CONFORMABLE);
}

int SameType(SgSymbol *darg, SgExpression *fact)
{
    SgType *dtype, *fact_type, *fstype;
    SgSymbol *fsymb;
    dtype = darg->type();
    if (isSgArrayType(dtype))
        dtype = dtype->baseType();
    fact_type = fact->type();
    fsymb = fact->symbol();

    // if(isSgVarRefExp(fact) && !isSgArrayType(fact->symbol()->type()) && 
    //    Same(dtype,fact->symbol()->type())
    //   return(1);

     //if(isScalar(darg) && !isSgArrayType(fact->type()))
    {   if (isSgVarRefExp(fact) || fact->variant() == CONST_REF)
        return(Same(fsymb->type(), dtype));
    if (isSgArrayRefExp(fact) && isSgArrayType(fsymb->type()))
        return(Same(fsymb->type()->baseType(), dtype));
    if (isSgValueExp(fact))
        return(Same(fact->type(), dtype));
    if (isSgArrayRefExp(fact) && fsymb->type()->variant() == T_STRING)
        return(Same(fsymb->type(), dtype));
    if (fact->variant() == ARRAY_OP)
    {
        if (isSgArrayType(fstype = fact->lhs()->symbol()->type()))
            fstype = fstype->baseType();
        return(Same(fstype, dtype));
    }
    }
    ////!!!!!!!
    return(0);
}

int Same(SgType *ft, SgType *dt)
{
    //TYPE_RANGES((T)->thetype)

    if (!ft || !dt)
        return(1);
    if ((dt->variant() == T_STRING) != 0)
    {
        if (ft->variant() == dt->variant())
            return(1);
        else
            return(0);
    }

    if (ft->variant() == dt->variant() && TypeSize(ft) && TypeSize(ft) == TypeSize(dt))
        return(1);

    if (ft->variant() == T_DOUBLE && dt->variant() == T_FLOAT && TypeSize(ft) == TypeSize(dt))
        return(1);
    if (dt->variant() == T_DOUBLE && ft->variant() == T_FLOAT && TypeSize(ft) == TypeSize(dt))
        return(1);

    if (ft->variant() == T_DCOMPLEX && dt->variant() == T_COMPLEX && TypeSize(ft) == TypeSize(dt))
        return(1);
    if (dt->variant() == T_DCOMPLEX && ft->variant() == T_COMPLEX && TypeSize(ft) == TypeSize(dt))
        return(1);
    return(0);

    //return(1); // temporary!!!!
}

int isScalar(SgSymbol *symb)
{
    if ((symb->variant() == VARIABLE_NAME) && !isSgArrayType(symb->type()))
        return(1);
    else
        return(0);
}

int isArray(SgSymbol *symb)
{
    if ((symb->variant() == VARIABLE_NAME) && isSgArrayType(symb->type()))
        return(1);
    else
        return(0);
}

int isFormalProcedure(SgSymbol *symb)
{
    switch (symb->variant())
    {
    case PROCEDURE_NAME:
    case FUNCTION_NAME:
    case ROUTINE_NAME:
        return(1);
    default:
        return(0);
    }
}

/*
int TestShapes(SgArrayType *ftp, SgArrayType *dtp)
{SgExpression *fe, *de;

 if(dtp && dtp->dimension() == 1 && ftp->dimension() > 1 && IdenticalValues((fe=ftp->sizeInDim(0)),(de=dtp->sizeInDim(0))) && IdenticalValues(LowerBoundOfDim(fe),LowerBoundOfDim(de)) )
    return(1);
 else
    return(0);
}
*/

int TestShapes(SgArrayType *ftp, SgArrayType *dtp)
{
    SgExpression *fe, *de;
    int rank, i;
    if (!dtp || !ftp) return(0);
    rank = dtp->dimension();
    if (rank > ftp->dimension())
        return(0);

    for (i = 0; i < rank; i++)
    {
        fe = ftp->sizeInDim(i);
        de = dtp->sizeInDim(i);
        if (!SameDims(fe, de))
            return(0);
    }
    return(1);
}

int TestBounds(SgExpression *fact, SgArrayType *ftp, SgArrayType *dtp)
{
    SgExpression *fe, *fl;
    int rank, i;
    if (!dtp || !ftp) return(0);
    rank = dtp->dimension();
    fl = fact->lhs();
    for (i = 0; i < rank; i++, fl = fl->rhs())
    {
        fe = ftp->sizeInDim(i);
        if (!isSgSubscriptExp(fe) && fl->lhs()->isInteger() && fl->lhs()->valueInteger() == 1)
            continue;
        if (IdenticalValues(fl->lhs(), LowerBoundOfDim(fe)))
            continue;
        else
            return(0);
    }
    return(1);
}

int TestVector(SgExpression *fact, SgArrayType *ftp, SgArrayType *dtp)
{//SgExpression *fe, *de, *e1;
    int rank;
    if (!dtp || !ftp) return(0);
    rank = dtp->dimension();
    if (rank > 1) return(0);
    //fl = fact->lhs();
    //de=dtp->sizeInDim(0);
    //fe=ftp->sizeInDim(0);
   /* e1=&(*(fl->lhs()) - (LowerBoundOfDim(de)->copy()));
    fl->setLhs(e1);
    if(e1->isInteger() && e1->valueInteger()==0)
      fl->setLhs(NULL);
   */
    return(1);
}


int SameDims(SgExpression *fe, SgExpression *de)
{
    if (isSgSubscriptExp(fe) || isSgSubscriptExp(de))
    {
        if (!IdenticalValues(LowerBoundOfDim(fe), LowerBoundOfDim(de)))
            return(0);
    }
    if (!IdenticalValues(UpperBoundOfDim(fe), UpperBoundOfDim(de)))
        return(0);

    return(1);
}


int SameShapes(SgArrayType *ftp, SgArrayType *dtp)
{
    SgExpression *fe, *de;
    int rank, i;
    if (!dtp || !ftp) return(0);
    rank = dtp->dimension();
    if (rank != ftp->dimension())
        return(0);

    for (i = 0; i < rank; i++)
    {
        fe = ftp->sizeInDim(i);
        de = dtp->sizeInDim(i);
        if (isSgSubscriptExp(fe) || isSgSubscriptExp(de))
        {
            if (!IdenticalValues(LowerBoundOfDim(fe), LowerBoundOfDim(de)))
                return(0);
        }
        if (i < rank - 1 && !IdenticalValues(UpperBoundOfDim(fe), UpperBoundOfDim(de)))
            return(0);
    }
    return(1);
}

SgExpression *LowerBoundOfDim(SgExpression *e)
// lower bound of dimension e
{
    SgSubscriptExp *sbe;

    if (!e)
        return(NULL);

    if ((sbe = isSgSubscriptExp(e)) != NULL) {
        if (sbe->lbound())
            return(sbe->lbound());
        else
            return(new SgValueExp(1));
    }
    else
        return(new SgValueExp(1));  // by default lower bound = 1      
}

SgExpression *UpperBoundOfDim(SgExpression *e)
// upper bound of dimension e
{
    SgSubscriptExp *sbe;

    if (!e)
        return(NULL);
    if ((sbe = isSgSubscriptExp(e)) != NULL) {
        if (sbe->ubound())
            return(sbe->ubound());
    }
    return(e);

}


SgExpression *FirstIndexChange(SgExpression *e, SgExpression *index)
{ //SgExpression *e0;
  //e0 = e->lhs();
    if (!index)
        return(e);
    e->setLhs(index->copy());
    return(e);
}

SgExpression *IndexChange(SgExpression *e, SgExpression *index, SgExpression *lbe)
{
    SgExpression *e0;
    int iv;
    if (!index)
        return(e);
    //e->setLhs(index->copy()+*(e->lhs())-lbe->copy());

    e0 = &(*(e->lhs()) - lbe->copy());

    if (e0->isInteger())
    {
        if ((iv = e0->valueInteger()) == 0)
            e->setLhs(index->copy());
        else
            e->setLhs(index->copy() + *new SgValueExp(iv));
    }
    else
        e->setLhs(index->copy() + *e0);
    return(e);
}

SgExpression *FirstIndexesChange(SgExpression *mape, SgExpression *re)
{
    SgExpression *el, *mel;
    for (el = re, mel = mape; el; el = el->rhs(), mel = mel->rhs())
        mel->setLhs(el->lhs());
    return(mape);
}



int IdenticalValues(SgExpression *e1, SgExpression *e2)
{
    //return(ExpCompare(Calculate(e1), Calculate(e2)));
    if (!e1 || !e2)
        return(0);
    if (e1->isInteger() && e2->isInteger())
    {
        if (e1->valueInteger() == e2->valueInteger())
            return(1);
        else
            return(0);
    }
    else
        return(0);
}

void TranslateArrayTypeExpressions(SgSymbol *darg)
{
    SgArrayType *arrtype;
    SgExpression *el;
    int rank, md;
    arrtype = isSgArrayType(darg->type());
    rank = arrtype->dimension();
    el = arrtype->getDimList();
    TranslateExpression(el, &md);

}

SgStatement *TranslateSubprogramReferences(SgStatement *header)
{
    SgStatement *stmt, *last, *first_executable = NULL, *last_decl;
    SgSymbol *s_top;
    int mdfd[3];
    last = header->lastNodeOfStmt();
    cur_func = top_header;
    for (stmt = header->lexNext(); stmt && (stmt != last); stmt = stmt->lexNext())
        if (isSgExecutableStatement(stmt) && stmt->variant() != FORMAT_STAT) {
            first_executable = stmt;  break;
        }
    last_decl = stmt->lexPrev();
    for (stmt = first_executable; stmt && (stmt != last); stmt = stmt->lexNext())
    {
        mdfd[0] = mdfd[1] = mdfd[2] = 0; //modified=0;
        switch (stmt->variant())
        {
            /* case OPEN_STAT:
               case CLOSE_STAT:
               case INQUIRE_STAT:
               case BACKSPACE_STAT:
               case ENDFILE_STAT:
               case REWIND_STAT:
                    break;
            */
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
            //mdfd[0]=mdfd[1]=0; //modified=0;
            if (stmt->expr(1))
                stmt->setExpression(1, *TranslateExpression(stmt->expr(1), &mdfd[1]));
            if (stmt->expr(0))
                stmt->setExpression(0, *TranslateExpression(stmt->expr(0), &mdfd[0]));
            if (mdfd[0] || mdfd[1])
                StatementCleaning(stmt);
            continue;

        case FOR_NODE:
        case PROC_STAT:
            if ((s_top = SymbolMap(stmt->symbol())) != 0)
            {
                stmt->setSymbol(*s_top);
                if (stmt->variant() == PROC_STAT)
                    mdfd[0] = 1;
            }

        default:
            //mdfd[0]=mdfd[1]=mdfd[2]=0; //modified=0;
            if (stmt->expr(0))
                stmt->setExpression(0, *TranslateExpression(stmt->expr(0), &mdfd[0]));
            if (stmt->expr(1))
                stmt->setExpression(1, *TranslateExpression(stmt->expr(1), &mdfd[1]));
            if (stmt->expr(2))
                stmt->setExpression(2, *TranslateExpression(stmt->expr(2), &mdfd[2]));
            if (mdfd[0] || mdfd[1] || mdfd[2])
                StatementCleaning(stmt);
            continue;
        }

    }
    return(last_decl->lexNext());
}

SgExpression *TranslateExpression(SgExpression *e, int *md)
{
    SgExpression *el, *aref, *cref;
    SgSymbol *s_top, *s;
    if (!e)
        return(e);

    if (isSgArrayRefExp(e))
    {
        for (el = e->lhs(); el; el = el->rhs())
            el->setLhs(TranslateExpression(el->lhs(), md));
        s = e->symbol();
        /* if((s_top=SymbolMap(s)))
          if(!(aref=ArrayMap(s)))
            e->setSymbol(s_top);
          else if(aref->variant() == EXPR_LIST)
          { e->setSymbol(s_top);
            e->setLhs(FirstIndexesChange(&(aref->copy()),e->lhs()));
            *md = 1;
          }
        */
        if ((s_top = SymbolMap(s)))
            e->setSymbol(s_top);
        if ((aref = ArrayMap(s)) && (aref->variant() == EXPR_LIST))
        {
            e->setLhs(FirstIndexesChange(&(aref->copy()), e->lhs()));
            *md = 1;
        }
        if ((aref = ARRAYMAP2(s)))
        {
            e->setLhs(IndexChange(&(aref->copy()), e->lhs(), LowerBound(s, 0)));
            *md = 1;
        }
        return(e);
    }
    //if(e->variant()==ARRAY_OP)
    // ;
    if (isSgVarRefExp(e))
    {
        s = e->symbol();
        //if((s_top=SymbolMap(s)) && !ArrayMap(s))
         // e->setSymbol(s_top);
        if ((s_top = SymbolMap(s)) != 0)
        {
            if (!(aref = ArrayMap(s)))
                e->setSymbol(s_top);
            else      //if(aref->variant() == ARRAY_REF)
            {
                NODE_CODE(e->thellnd) = ARRAY_REF;   //e->setVariant(ARRAY_REF);
                e->setSymbol(s_top);
                e->setLhs(aref->lhs()->copy());
            }
        }

        if ((cref = CONSTANTMAP(s)))
        {
            return(&(cref->copy()));
        }

        return(e);
    }

    if (e->variant() == CONST_REF)
    {
        s = e->symbol();
        if ((s_top = SymbolMap(s)))
            e->setSymbol(s_top);
        return(e);
    }


    if (isSgFunctionCallExp(e))
    {
        s = e->symbol();
        if ((s_top = SymbolMap(s)))
        {
            e->setSymbol(s_top);
            *md = 1;
        }
    }

    e->setLhs(TranslateExpression(e->lhs(), md));
    e->setRhs(TranslateExpression(e->rhs(), md));
    return(e);
}


/*
void TranslateExpression(SgExpression *e, int *md)
{  SgExpression *el, *aref;
   SgSymbol *s_top, *s;
 if(!e)
  return;
 if(isSgArrayRefExp(e))
 {
   for(el=e->lhs();el;el=el->rhs())
      TranslateExpression(el->lhs(),md);
   s= e->symbol();
   if((s_top=SymbolMap(s)))
    if(!(aref=ArrayMap(s)))
      e->setSymbol(s_top);
    else if(aref->variant() == EXPR_LIST)
    { e->setSymbol(s_top);
      e->setLhs(FirstIndexChange(&(aref->copy()),e->lhs()->lhs()));
      *md = 1;
    }
   return;
 }
 //if(e->variant()==ARRAY_OP)
 // ;
 if(isSgVarRefExp(e))
 { s= e->symbol();
   //if((s_top=SymbolMap(s)) && !ArrayMap(s))
    // e->setSymbol(s_top);
   if((s_top=SymbolMap(s)) )
     if(!(aref=ArrayMap(s)))
       e->setSymbol(s_top);
     else      //if(aref->variant() == ARRAY_REF)
     { NODE_CODE(e->thellnd) = ARRAY_REF;   //e->setVariant(ARRAY_REF);
       e->setSymbol(s_top);
       e->setLhs(aref->lhs()->copy());
     }
   return;
 }
 TranslateExpression(e->lhs(),md);
 TranslateExpression(e->rhs(),md);
}
*/

void TranslateExpression_1(SgExpression *e)
{
    SgExpression *el;
    SgSymbol *s_top, *s;
    if (!e)
        return;
    if (isSgArrayRefExp(e))
    {
        for (el = e->lhs(); el; el = el->rhs())
            TranslateExpression_1(el->lhs());
        s = e->symbol();
        if ((s_top = SymbolMap(s)) && !ArrayMap(s))
            e->setSymbol(s_top);
        return;
    }
    //if(e->variant()==ARRAY_OP)
    // ;
    if (isSgVarRefExp(e))
    {
        s = e->symbol();
        if ((s_top = SymbolMap(s)) && !ArrayMap(s))
            e->setSymbol(s_top);
        return;
    }
    TranslateExpression_1(e->lhs());
    TranslateExpression_1(e->rhs());
}

void EditExpressionList(SgExpression *e)
{
    SgExpression *el;
    for (el = e; el; el = el->rhs())
        el->lhs()->setLhs(NULL);
}


void TranslateExpressionList(SgExpression *e)
{
    SgExpression *el;
    for (el = e; el; el = el->rhs())
        TranslateExpression_1(el->lhs());
}

SgSymbol *SymbolMap(SgSymbol *s)
{
    return(SymbMapping(s->thesymb->entry.Template.declared_name));
}

SgExpression *ArrayMap(SgSymbol *s)
{
    SgExpression *aref;
    if ((aref = ARRAYMAP(s)))
        return(aref);
    else
        return(NULL);
}

SgExpression *ArrayMap2(SgSymbol *s)
{
    SgExpression *aref;
    if ((aref = ARRAYMAP2(s)))
        return(aref);
    else
        return(NULL);
}

void InsertBlockAfter(SgStatement *after, SgStatement *first, SgStatement *header)
{
    SgStatement *prevst, *last;
    last = header->lastNodeOfStmt();
    if ((prevst = last->lexPrev()) && prevst->variant() == CONT_STAT && !(prevst->hasLabel()))
        prevst->extractStmt();
    header->extractStmt();
#if __SPF
    insertBfndListIn(first->thebif, after->thebif, NULL);
#else
    after->insertStmtAfter(*first);
#endif
    last->extractStmt();  //extract  END

}
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//      S T A T E M E N T S  (inserting, creating and so all)
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

void InsertNewStatementBefore(SgStatement *stat, SgStatement *current) {
    //SgExpression *le;
    //SgValueExp * index;
    SgStatement *st;

    st = current->controlParent();
    if (st->variant() == LOGIF_NODE) { // Logical IF
       // change by construction IF () THEN <current> ENDIF and
       // then insert statement before current statement
        st->setVariant(IF_NODE);
#if __SPF
        insertBfndListIn((new SgStatement(CONTROL_END))->thebif, current->thebif, NULL);
#else
        current->insertStmtAfter(*new SgStatement(CONTROL_END));
#endif

#if __SPF
        insertBfndListIn(stat->thebif, st->thebif, NULL);
#else
        st->insertStmtAfter(*stat);
#endif
        return;
    }

    if (current->hasLabel() && current->variant() != FORMAT_STAT && current->variant() != DATA_DECL && current->variant() != ENTRY_STAT) { //current statement has label
      //insert statement before current and set on it the label of current
        SgLabel *lab;
        lab = current->label();
        BIF_LABEL(current->thebif) = NULL;
        current->insertStmtBefore(*stat, *current->controlParent());//inserting before current statement
        stat->setLabel(*lab);
        return;
    }
    current->insertStmtBefore(*stat, *current->controlParent());//inserting before current statement 
}

void InsertNewStatementAfter(SgStatement *stat, SgStatement *current, SgStatement *cp)
{
    SgStatement *st;
    st = current;
    if (current->variant() == LOGIF_NODE)   // Logical IF
        st = current->lexNext();
    if (cp->variant() == LOGIF_NODE)
        LogIf_to_IfThen(cp);
    st->insertStmtAfter(*stat, *cp);
    // cur_st = stat;
}

void LogIf_to_IfThen(SgStatement *stmt)
{
    //replace Logical IF statement: IF ( <condition> ) <statement>
    // by construction:  IF  ( <condition> ) THEN
    //                         <statement> 
    //                   ENDIF
    stmt->setVariant(IF_NODE);
    (stmt->lexNext())->insertStmtAfter(*new SgControlEndStmt(), *stmt);
}

void ReplaceContext(SgStatement *stmt)
{
    if (isDoEndStmt(stmt))
        ReplaceDoNestLabel(stmt, NewLabel());
    else if (isSgLogIfStmt(stmt->controlParent())) {
        if (isDoEndStmt(stmt->controlParent()))
            ReplaceDoNestLabel(stmt->controlParent(), NewLabel());
        LogIf_to_IfThen(stmt->controlParent());
    }
}

int isDoEndStmt(SgStatement *stmt)
{
    SgLabel *lab, *do_lab;
    SgForStmt *parent;
    if (!(lab = stmt->label()) && stmt->variant() != CONTROL_END) //the statement has no label and
        return(0);                                               //is not ENDDO 
    parent = isSgForStmt(stmt->controlParent());
    if (!parent)  //parent isn't DO statement
        return(0);
    do_lab = parent->endOfLoop(); // label of loop end or NULL
    if (do_lab) //  DO statement with label
        if (lab && LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(do_lab->thelabel))
            // the statement label is the label of loop end  
            return(1);
        else
            return(0);
    else   //  DO statement without label
        if (stmt->variant() == CONTROL_END)
            return(1);
        else
            return(0);
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
{
    SgStatement *parent, *st;
    SgLabel *lab;
    SgForStmt *do_st;
    parent = last_st->controlParent();
    lab = last_st->label();
    while ((do_st = isSgForStmt(parent)) != NULL && do_st->endOfLoop()) {
        if (LABEL_STMTNO(lab->thelabel) == LABEL_STMTNO(do_st->endOfLoop()->thelabel)) {
            if (!new_lab)
                new_lab = NewLabel();
            BIF_LABEL_USE(do_st->thebif) = new_lab->thelabel;
            parent = parent->controlParent();
        }
        else
            break;
    }
    //inserts CONTINUE statement with new_lab as label  
    st = new SgStatement(CONT_STAT);
    st->setLabel(*new_lab);
    SetScopeOfLabel(new_lab, cur_func);
    // for debug regim
    LABEL_BODY(new_lab->thelabel) = st->thebif;
    //BIF_LINE(st->thebif) = (last_st->lineNumber()) ? last_st->lineNumber() : LineNumberOfStmtWithLabel(lab);
    if (last_st->variant() != LOGIF_NODE)
        last_st->insertStmtAfter(*st, *last_st->controlParent());
    else
        (last_st->lexNext())->insertStmtAfter(*st, *last_st->controlParent());
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//   T E M P O R A R Y   V A R I B L E S
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

SgSymbol *GetTempVarForF(SgSymbol *sf, SgType *t)
{
    char *name;
    SgSymbol *sn;
    name = new char[80];
    sprintf(name, "%s_%d_%d", sf->identifier(), sf->id(), vcounter++);
    sn = new SgVariableSymb(name, *t, *cur_func);
    if (isInSymbolTable(sn))
        sn = GetTempVarForF(sf, t);
    if (cur_func == top_header)
        top_temp_vars = AddToSymbList(top_temp_vars, sn);
    return(sn);
}

SgType * TypeOfResult(SgExpression *e)
{
    int indf;
    SgSymbol *sf;
    sf = e->symbol();
    indf = is_IntrinsicFunction(sf);
    if (deb_reg > 2)
        printf("indf: %d\n", indf);
    if (indf > 0)
        return(TypeF(indf, e));
    else
        return(sf->type());
}

SgType *TypeF(int indf, SgExpression *e)
{
    graph_node *gnode;
    //SgFile *f;
    gnode = getAttrNodeForSymbol(e->symbol());
    current_file = gnode->file;

    switch (intrinsic_type[indf])
    {
    case 1:    return(SgTypeInt());
    case 2:    return(SgTypeBool());
    case 3:    return(SgTypeFloat());
    case 4:    return(SgTypeDouble());
    case 5:    return(SgTypeComplex(current_file));
    case 6:    return(SgTypeDoubleComplex(current_file));
    case 7:    return(SgTypeChar());
    case (-1): //return(e->lhs()->lhs()->type()); //type of first argument
        return(TypeOfArgument(e->lhs()->lhs()));
    default:
        return(NULL);
    }
}

SgType *TypeOfArgument(SgExpression *e)
//set_expr_type() in types.c
{
    SgType *t;
    //int indf;
    //SgSymbol *sf;
    t = e ? e->type() : NULL;
    switch (e->variant()) {
    case (FUNC_CALL):
    {
        /*  sf = e->symbol();
          indf=is_IntrinsicFunction(sf);
          if(indf>0 )
          { t=TypeF(indf,e);
            if(!t)
              t=sf->type();
          }
          else
            t=sf->type();
        */
        t = TypeOfResult(e);
        break;
    }
    /*    case (VAR_REF):
             if(e->symbol())
               t=e->symbol()->type();
             else
               t=NULL;
        case (ARRAY_REF):

        case (AND_OP):
        case (OR_OP):
        case (EQ_OP):
        case (LT_OP):
        case (GT_OP):
        case (NOTEQL_OP):
        case (LTEQL_OP):
        case (EQV_OP):
        case (NEQV_OP):
        case (GTEQL_OP):
    */
    case (DIV_OP):
    case (ADD_OP):
    case (SUBT_OP):
    case (MULT_OP):
    case (EXP_OP):
    {PTR_LLND expr, len;
    PTR_TYPE l_operand, r_operand;
    int l_type, r_type, ilen = 0;
    expr = e->thellnd;
    l_operand = expr->entry.binary_op.l_operand->type;
    r_operand = expr->entry.binary_op.r_operand->type;
    if (!l_operand || !r_operand)
        break;
    else {
        if (l_operand->variant == T_ARRAY)
            l_type = l_operand->entry.ar_decl.base_type->variant;
        else
            l_type = l_operand->variant;
        if (r_operand->variant == T_ARRAY)
            r_type = r_operand->entry.ar_decl.base_type->variant;
        else
            r_type = r_operand->variant;
        if (l_operand->entry.Template.ranges)
        {
            len = (l_operand->entry.Template.ranges)->entry.Template.ll_ptr1;
            if (len && len->variant == INT_VAL)
                ilen = len->entry.ival;
            if (l_type == T_FLOAT && ilen == 8)
                l_type = T_DOUBLE;
            if (l_type == T_COMPLEX && ilen == 16)
                l_type = T_DCOMPLEX;
        }
        if (r_operand->entry.Template.ranges)
        {
            len = (r_operand->entry.Template.ranges)->entry.Template.ll_ptr1;
            if (len && len->variant == INT_VAL)
                ilen = len->entry.ival;
            if (r_type == T_FLOAT && ilen == 8)
                r_type = T_DOUBLE;
            if (r_type == T_COMPLEX && ilen == 16)
                r_type = T_DCOMPLEX;
        }

        if (l_type == T_DCOMPLEX || r_type == T_DCOMPLEX)
            t = SgTypeDoubleComplex(current_file);
        else if (l_type == T_COMPLEX || r_type == T_COMPLEX)
            t = SgTypeComplex(current_file);
        else if (l_type == T_DOUBLE || r_type == T_DOUBLE)
            t = SgTypeDouble();
        else if (l_type == T_FLOAT || r_type == T_FLOAT)
            t = SgTypeFloat();
        else if (l_type == T_INT && r_type == T_INT)
            t = SgTypeInt();

        else t = NULL;
        /*
      if (l_operand->variant == T_ARRAY)
      {
           expr->type = copy_type_node(expr->entry.binary_op.l_operand->type);
           expr->type->entry.ar_decl.base_type =  temp;
      }
                  else if (r_operand->variant == T_ARRAY)
          {
              expr->type = copy_type_node(expr->entry.binary_op.r_operand->type);
              expr->type->entry.ar_decl.base_type =  temp;
          }
      else  expr->type =  temp;
                 */
    }
    break;
    }
    case (NOT_OP):
    case (UNARY_ADD_OP):
    case (MINUS_OP):
    case (CONCAT_OP):
        //expr->type = expr->entry.unary_op.operand->type;
        t = e->lhs()->type();
        break;
    default:
        //err("Expression variant not known",322);
        break;
    }
    e->setType(t);
    return(t);

}




SgType * SgTypeComplex(SgFile *f)
{
    SgType *t;
    for (t = f->firstType(); t; t = t->next())
        if (t->variant() == T_COMPLEX)
            return(t);

    return(new SgType(T_COMPLEX));
}

SgType * SgTypeDoubleComplex(SgFile *f)
{
    SgType *t;
    for (t = f->firstType(); t; t = t->next())
        if (t->variant() == T_DCOMPLEX)
            return(t);

    return(new SgType(T_DCOMPLEX));
}

int is_IntrinsicFunction(SgSymbol *sf)
{
    graph_node *gnode;
    //printf("is intrinsic ?\n");
    gnode = getAttrNodeForSymbol(sf);
    //printf("gnode:%d\n",gnode);
    if (!gnode) return (-1);
    if (isNoBodyNode(gnode))
        return(IntrinsicInd(sf));
    else
        return(-1);
}

int is_NoExpansionFunction(SgSymbol *sf)
{
    graph_node *gnode;
    //printf("is no body ?\n");
    gnode = getAttrNodeForSymbol(sf);
    //printf("gnode:%d\n",gnode);
    if (isDummyArgument(sf)) return(0);
    if (!gnode) return (1);
    return(isNoBodyNode(gnode));
}

int IntrinsicInd(SgSymbol *sf)
{
    int i;
    if (deb_reg > 2)
        printf("is intrinsic %s\n", sf->identifier());
    for (i = 0; i < MAX_INTRINSIC_NUM; i++)
    {
        if (!intrinsic_name[i])
            break;
        //printf("%d   %s = %s\n", i, intrinsic_name[i], sf->identifier());
        if (!strcmp(sf->identifier(), intrinsic_name[i]))
            return(i);
    }
    return(-1);
}


SgSymbol *GetTempVarForArg(int i, SgSymbol *sf, SgType *t)
{
    char *name;
    SgSymbol *sn;
    name = new char[80];
    sprintf(name, "%s_%d_arg%d_%d", sf->identifier(), sf->id(), i, vcounter++);
    sn = new SgVariableSymb(name, *t, *cur_func);
    if (isInSymbolTable(sn))
        sn = GetTempVarForArg(i, sf, t);
    if (cur_func == top_header)
        top_temp_vars = AddToSymbList(top_temp_vars, sn);

    return(sn);
}

SgSymbol *GetTempVarForSubscr(SgType *t)
{
    char *name;
    SgSymbol *sn;
    name = new char[80];
    sprintf(name, "sbscr_arg_%d", vcounter++);
    sn = new SgVariableSymb(name, *t, *cur_func);
    if (isInSymbolTable(sn))
        sn = GetTempVarForSubscr(t);
    if (cur_func == top_header)
        top_temp_vars = AddToSymbList(top_temp_vars, sn);

    return(sn);
}


SgSymbol *GetTempVarForBound(SgSymbol *sa)
{
    char *name;
    SgSymbol *sn;
    name = new char[80];
    sprintf(name, "%s_%d_%d", sa->identifier(), sa->id(), vcounter++);
    sn = new SgVariableSymb(name, *SgTypeInt(), *(sa->scope()));
    if (isInSymbolTable(sn))
        sn = GetTempVarForBound(sa);
    return(sn);
}

SgSymbol *GetImplicitDoVar(int j)
{
    char *name;
    SgSymbol *sn;
    name = new char[80];
    sprintf(name, "i0%d", j + 1);
    name = NewName(name);

    //if(sn = isTopName(name)
     // if(sn->type == SgTypeInt())
       //  return(sn);
     // else
        // return(GetImplicitDoVar
    //else

    sn = new SgVariableSymb(name, *SgTypeInt(), *top_header);
    return(sn);
}

int isInSymbolTable(SgSymbol *sym)
{
    SgSymbol *s;
    for (s = cur_func->symbol(); s; s = s->next())
        if (sym != s && !strcmp(sym->identifier(), s->identifier()))
            return(1);
    return(0);
}

char *NewName(char *name)
{
    if (isTopName(name))
    {
        sprintf(name, "%s_", name);
        name = NewName(name);
    }
    return(name);
}

SgSymbol *isTopName(char *name)
{
    SgSymbol *s;
    for (s = top_header->symbol(); s; s = s->next())
        if (s->scope() == top_header && !strcmp(name, s->identifier()))
            return(s);
    return(NULL);
}

SgSymbol *isTopNameOfType(char *name, SgType *type)
{
    SgSymbol
        *s;
    for (s = top_header->symbol(); s; s = s->next())
        if (s->scope() == top_header && !strcmp(name, s->identifier()) && type == s->type())
            return(s);
    return(NULL);
}

SgSymbol *GetNewTopSymbol(SgSymbol *s)
{
    char *name;
    SgSymbol *sn;
    name = new char[80];

    sprintf(name, "%s__%d", s->identifier(), vcounter++);
    sn = new SgSymbol(s->variant(), name, *s->type(), *top_header);
    if (sn->variant() == CONST_NAME)
        SYMB_VAL(sn->thesymb) = SYMB_VAL(s->thesymb);

    if (isInTopSymbList(sn))
        sn = GetNewTopSymbol(s);

    return(sn);

}

int isInTopSymbList(SgSymbol *sym)
{
    SgSymbol *s;
    for (s = top_symb_list; s; s = NextSymbol(s))
        if (sym != s && !strcmp(sym->identifier(), s->identifier()))
            return(1);
    return(0);
}

void PrintTopSymbList()
{
    SgSymbol *s;
    printf("\nSymbol List of Top:\n");
    for (s = top_symb_list; s; s = NextSymbol(s))
        printf("    %s", s->identifier());
    return;
}

void PrintSymbList(SgSymbol *slist, SgStatement *header)
{
    SgSymbol *s;
    printf("\nSymbol List of  %s:\n", header->symbol()->identifier());
    for (s = slist; s; s = NextSymbol(s))
        printf("    %s", s->identifier());
    return;
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//   N O T   R E A L I S E D  ! ! !
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

int isIntrinsicFunctionName(char *name)
{
    return(0);
}

char *ChangeIntrinsicFunctionName(char *name)
{
    return(name);
}

int isInlinedCallSite(SgStatement *stmt)
{ // !!!!! temporary
    return(1);
}
int TestFormatLabel(SgLabel *lab)
{
    return 0;
}

void MakeRefsConformable(SgExpression *tref, SgExpression *ref)
{
    return;
}

void CalculateTopLevelRef(SgSymbol *tops, SgExpression *tref, SgExpression *ref)
{
    return;
}
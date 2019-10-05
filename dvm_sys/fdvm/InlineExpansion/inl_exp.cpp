/*********************************************************************/
/*                   Inline Expansion    2006                        */
/*********************************************************************/


/*********************************************************************/
/*                   Inliner Driver                                  */
/*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <sstream>
//#define IN_DVM_
//#include "dvm.h" 
//#undef IN_DVM_

#define IN_M_ 
#include "inline.h" 
#undef IN_M_

// Inliner version
#define VERSION_NUMBER "4"

using std::string;
using std::map;
using std::set;
using std::vector;

const char *name_loop_var[8] = { "idvm00","idvm01","idvm02","idvm03", "idvm04","idvm05","idvm06","idvm07" };
const char *name_bufIO[6] = { "i000io","r000io", "d000io","c000io","l000io","dc00io" };
SgSymbol *rmbuf[6];
const char *name_rmbuf[6] = { "i000bf","r000bf", "d000bf","c000bf","l000bf","dc00bf" };
SgSymbol *dvmcommon;
SgSymbol *heapcommon;
SgSymbol *redcommon;
SgSymbol *dbgcommon;
int lineno;  // number of line in file
SgStatement *first_exec; // first executable statement in procedure
int nproc, ndis, nblock, ndim, nblock_all;
int iblock, isg, iacross;
int saveall; //= 1 if there is SAVE without name-list in current function(procedure)  
int mem_use[6] = { 0,0,0,0,0,0 };
int buf_use[6] = { 0,0,0,0,0,0 };
base_list *mem_use_structure;
int lab;  // current label  
int v_print = 0; //set to 1 by -v flag
int warn_all = 0; //set to 1 by -w flag
int own_exe;
symb_list *new_red_var_list;
SgSymbol *file_var_s;
int nloopred; //counter of parallel loops with reduction group
int nloopcons; //counter of parallel loops with consistent group
stmt_list *wait_list; // list of REDUCTION_WAIT directives
int task_ps = 0;
SgStatement *end_of_unit; // last node (END statement) of program unit
SgStatement *has_contains; //node for CONTAINS statement
int dvm_const_ref;

extern "C" int out_free_form;
//
//-----------------------------------------------------------------------
// FOR DEBUGGING
//#include "dump_info.C"
//-----------------------------------------------------------------------

set<string> needToInline;
#ifdef __SPF
void removeIncludeStatsAndUnparse(SgFile *file, const char *fileName, const char *fout);
#endif

int main(int argc, char *argv[]) 
{
    FILE *fout;
    char *fout_name = (char *)"out.f";
    //char *fout_name = NULL;
    int level, hpf, openmp, isz;
    // initialisation
    initialize();
    
#ifdef __SPF
    if (argc == 1)
    {
        printf("Usage:\n");
        printf("Parse project with 'Parser' command first.\n");
        printf("Specify functions to inline by parameter:\n");
        printf("   -toInlined N name1 name2 name3... nameN, \n");
        printf("where N - number of functions to inline, nameI - name of each function.\n");
        printf("NOTE: count of nameI and N must be equal.\n");
        return 0;
    }
#endif
    openmp = hpf = 0;
    argv++;
    while ((argc > 1) && (*argv)[0] == '-') 
    {
        if ((*argv)[1] == 'o' && ((*argv)[2] == '\0')) 
        {
            fout_name = argv[1];
            argv++;
            argc--;
        }
        else if (!strcmp(argv[0], "-dc"))
            with_cmnt = 1;
        else if ((*argv)[1] == 'd')
        {
            switch ((*argv)[2])
            {
                /*case '0':  level = 0; break;*/
            case '1':  level = 1; break;
            case '2':  level = 2; break;
            case '3':  level = 3; break;
            case '4':  level = 4; break;
                /* case '5':  level = -1; many_files=1; break;*/
            default:  level = -1;
            }
            if (level > 0)
                deb_reg = level;
        }
        else if (!strcmp(argv[0], "-p")) {
            only_debug = 0; hpf = 0;
        }
        else if (!strcmp(argv[0], "-s")) {
            only_debug = 1; hpf = 0;
        }
        else if (!strcmp(argv[0], "-v"))
            v_print = 1;
        else if (!strcmp(argv[0], "-w"))
            warn_all = 1;
        else if (!strcmp(argv[0], "-bind0"))
            bind = 0;
        else if (!strcmp(argv[0], "-bind1")) {
            bind = 1; len_long = 8;
        }
        else if (!strcmp(argv[0], "-hpf") || !strcmp(argv[0], "-hpf1") || !strcmp(argv[0], "-hpf2"))
            hpf = 1;
        else if (!strcmp(argv[0], "-mp"))
            openmp = 1;
        else if (!strcmp(argv[0], "-ffo"))
            out_free_form = 1;
        else if (!strncmp(argv[0], "-bufio", 6)) 
        {
            if ((*argv)[6] != '\0' && (isz = is_integer_value(*argv + 6)))
                IOBufSize = isz;
        }
        else if (!strcmp(argv[0], "-ver"))
        {
            (void)fprintf(stderr, "inliner version is \"%s\"\n", VERSION_NUMBER);
            exit(0);
        }
#ifdef __SPF
        else if (!strcmp(argv[0], "-toInlined"))
        {
            argc--;
            argv++;
            int count = 0;
            int err = sscanf(argv[0], "%d", &count);
            //TODO: check err
            argc--;
            argv++;
            for (int z = 0; z < count; ++z)
            {
                needToInline.insert(argv[0]);
                if (z != count - 1)
                {
                    argc--;
                    argv++;
                }
            }

            if (needToInline.size() > 0)
            {
                printf("need to inline:\n");
                for (auto it = needToInline.begin(); it != needToInline.end(); ++it)
                    printf("%s\n", (*it).c_str());
            }
        }
#endif
        argc--;
        argv++;
    }

    SgProject project((char *)"dvm.proj");
    SgFile *file;
    int i;
    //printf("Number Of Files: %d\n",project.numberOfFiles());

    for (i = 0; i < project.numberOfFiles(); i++) 
    {
        SgFile *f;
        f = &(project.file(i));
        if (deb_reg)
            printf("              FILE[%d]: %s\n", i, project.fileName(i));
    }
    
    file = &(project.file(0));
    fin_name = new char[80];
    sprintf(fin_name, "%s%s", project.fileName(0), " ");
    //fin_name = strcat(project.fileName(0)," "); 
                                            // for call of function 'tpoint' 
                                            //added one symbol to input-file name
    initVariantNames();
    initIntrinsicNames();
    //InitDVM(file);

    current_file = file;   // global variable (used in SgTypeComplex)
    max_lab = getLastLabelId();
    //if(dbg_if_regim) GetLabel(); //set maxlabval=90000
    /*
       printf("Labels:\n");
       printf("first:%d  max: %d \n",firstLabel(file)->thelabel->stateno, getLastLabelId());
       for(int num=1; num<=getLastLabelId(); num++)
       if(isLabel(num))
       printf("%d is label\n",num);
       else
       printf("%d isn't label\n",num);
    */
    if (v_print)
        (void)fprintf(stderr, "<<<<< Inline Expansion   >>>>>\n");
    
    //build CallGraph of all files
    for (int i = 0; i < project.numberOfFiles(); i++)
    {
        SgFile *currF = &(project.file(i));
        // Building a directed acyclic call multigrahp (call DAMG) 
        // which represents calls between routines of the program
        // which are to be (or not to be) expanded

        for (int k = 0; k < currF->numberOfFunctions(); ++k)
        {
            SgStatement *func = currF->functions(k);
            cur_func = func;
            cur_symb = func->symbol();
            CallGraph(func);
        }
    }
    InlinerDriver(file);

    /*
    { SgSymbol *s, *scop;

      s= file->functions(0)->symbol();
      //file =&(project.file(1));
      //scop= &(s->copyAcrossFiles(*(file->firstStatement())));
      scop= &(s->copySubprogram(*(file->firstStatement())));
      printf(" \n****** BODY COPY FUNCTION(0) %s ********\n", scop->identifier());
      scop->body()->unparsestdout();
      printf(" \n****** AFTER COPY FUNCTION(0) ********\n");
      file->unparsestdout();
     }
    */

    if (v_print)
        (void)fprintf(stderr, "<<<<< End Inline Expansion >>>>>\n");

    /* DEBUG */
    /*       classifyStatements(file);
           printf("**************************************************\n");
           printf("**** Expression Table ****************************\n");
           printf("**************************************************\n");
           classifyExpressions(file);
           printf("**************************************************\n");
           printf("**** Symbol  Table *******************************\n");
           printf("**************************************************\n");
           classifySymbols(file);
           printf("**************************************************\n");
     */
     /*  end DEBUG */


    if (errcnt) {
        (void)fprintf(stderr, "%d error(s)\n", errcnt);
        //!!! exit(1);
        return 1;
    }
    //file->saveDepFile("dvm.dep");
    // DVMFileUnparse(file);
    //  file->saveDepFile("f.dep");
    if (!fout_name) { //outfile is not specified, output result to stdout
        file->unparsestdout();
        return 0;
    }
#ifdef __SPF
    string outFile;
    //printf("out file is %s\n", fout_name);
    if (string("out.f") == fout_name)
    {
        outFile = file->filept->filename;
        auto itS = outFile.end();
        itS--;
        size_t pos = outFile.size() - 1;
        while (itS[0] != '.' && itS != outFile.begin())
        {
            itS--;
            pos--;
        }

        FILE *check = NULL;
        string insert = "_inl";
        do
        {
            string copy(outFile);
            copy.insert(pos, insert);
            if (check)
                fclose(check);
            check = fopen(copy.c_str(), "r");
            if (check)
                insert += "_";
        } while (check);

        outFile.insert(pos, insert);
    }
    else
        outFile = fout_name;
    printf("out file is %s\n", outFile.c_str());
    removeIncludeStatsAndUnparse(file, file->filept->filename, outFile.c_str());
#else
    //writing result of converting into file
    if ((fout = fopen(fout_name, "w")) == NULL) {
        (void)fprintf(stderr, "Can't open file %s for write\n", fout_name);
        // exit (1);
        return 1;
    }

    if (v_print)
        (void)fprintf(stderr, "<<<<<  Unparsing   %s  >>>>>\n", fout_name);

    file->unparse(fout);

    if ((fclose(fout)) < 0) 
    {
        fprintf(stderr, "Could not close %s\n", fout_name);
        return 1;
    }

    if (v_print)
        fprintf(stderr, "\n*****  Done  *****\n");
#endif
    return 0;
}

void initialize()
{
    node_list = NULL;
    do_dummy = 0; do_stmtfn = 0;
    gcount = 0;
    deb_reg = 0;
    with_cmnt = 0;
}

void initVariantNames() 
{
    for (int i = 0; i < MAXTAGS; i++) 
        tag[i] = NULL;
    /*!!!*/
#include "tag.h"
}

void initIntrinsicNames() 
{
    for (int i = 0; i < MAX_INTRINSIC_NUM; i++) 
    {
        intrinsic_type[i] = 0;
        intrinsic_name[i] = NULL;
    }
#include "intrinsic.h" 
}



/***********************************************************************/

void InlinerDriver(SgFile *f)
{
  // function is program unit accept BLOCKDATA and MODULE (F90),i.e. 
  // PROGRAM, SUBROUTINE, FUNCTION
    //if(debug_fragment || perf_fragment) // is debugging or performance analizing regime specified ?
     // BeginDebugFragment(0,NULL);// begin the fragment with number 0 (involving whole file(program) 
    
    if (deb_reg > 1)
        PrintWholeGraph();

    //Removing nodes representing "dead" subprogram
    RemovingDeadSubprograms();

    //Removing nodes representing "nobody" subprogram
    NoBodySubprograms();

    if (deb_reg > 1)
    {
        PrintWholeGraph();
        PrintWholeGraph_kind_2();
    }

    //Building a list of header nodes to represent "top level" routines  
    BuildingHeaderNodeList();

    // for debug
    //PrintSymbolTable(f);

  // Looking through the list of header nodes,
  // splitting header node n which has "inlined" edges representing inlined calls to n
    {
        graph_node *gnode, *gnode_new;
        graph_node_list *ln;
        edge *edg;
        global_st = f->firstStatement();
        if (deb_reg > 1)
            printf("\nLooking header node list ....\n");
        for (ln = header_node_list; ln; ln = ln->next)
        {
            gnode = ln->node;
            if (deb_reg > 1)
                printf("\nlooking NODE[%d]  %s\n", gnode->id, gnode->symb->identifier());

            // looking through the incoming edges list of gnode
            for (edg = gnode->from_calling; edg; edg = edg->next)
            {
                if (edg->inlined) //gnode has "inlined" incoming edge
                {
                    //split gnode, creating node gnode_new
                    gnode_new = SplittingNode(gnode);
                    //reset all edges representing inlined calls to gnode to point to gnode_new
                    ReseatEdges(gnode, gnode_new);
                    break;
                }
            }
        }
    }

    // Removing all edges representing uninlined calls 
    RemovingUninlinedEdges();

    // for debug
    if (deb_reg > 1)
    {
        PrintWholeGraph();
        PrintWholeGraph_kind_2();
        PrintSymbolTable(f);
        PrintTypeTable(f);
    }

    // Parttion the call graph into inline flow graphs
    Partition();
    if (deb_reg)
    {
        PrintWholeGraph();
        PrintWholeGraph_kind_2();
    }

    // For each non-trivial inline flow graph 
    //     call the inliner to create the corresconding "top level" routine    
    for (graph_node_list *ln = header_node_list; ln; ln = ln->next)
    {
        if (ln->node->to_called)
            Inliner(ln->node);
    }
    //(f->functions(0)->symbol())->copyAcrossFiles(*(f->firstStatement()));
    //printf(" \n****** AFTER COPY FUNCTION(0) ********\n");
    if (deb_reg > 1)
        f->unparsestdout();
    return;

    /*
      has_contains = NULL;
      //all_replicated=1;
      for(stat=stat->lexNext(); stat; stat=end_of_unit->lexNext()) {
        //end of external procedure with CONTAINS statement
        if(has_contains && stat->variant() == CONTROL_END && has_contains->controlParent() == stat->controlParent()){
          end_of_unit = stat; has_contains = NULL;
          continue;
        }
        if( stat->variant() == BLOCK_DATA){//BLOCK_DATA header
          end_of_unit = stat->lastNodeOfStmt();
          //TransModule(stat); //changing variant VAR_DECL with VAR_DECL_90
          continue;
        }
        // PROGRAM, SUBROUTINE, FUNCTION header
        func = stat;
        cur_func = func;

            //scanning the Symbols Table of the function
            //     ScanSymbTable(func->symbol(), (f->functions(i+1))->symbol());

       // all_replicated= has_contains ? 0 : 1;
    // translating the function
    //     if(only_debug)
    //        InsertDebugStat(func);
    //     else
    //        TransFunc(func);

      }

    */
}


void CallGraph(SgStatement *func)
{
    // Build a directed acyclic call multigrahp (call DAMG) 
    // which represents calls between routines of the program
    // which are to be (or not to be) expanded

    SgStatement *stmt, *last, *data_stf, *first, *last_spec, *stam;
    //SgExpression *e;
    //SgStatement *task_region_parent, *on_parent, *mod_proc, *begbl;
    //SgStatement *copy_proc = NULL;
    SgLabel *lab_exec;

    //int i;
    //stmt_list *pstmt = NULL;
    //initialization              
    data_stf = NULL;

    DECL(func->symbol()) = 1;
    if (func->variant() == PROG_HEDR)
        PROGRAM_HEADER(func->symbol()) = func->thebif;

    //creating graph node for header of function (procedure, program)
    cur_node = CreateGraphNode(func->symbol(), func);

    first = func->lexNext();
    //printf("\n%s  header_id= %d \n", func->symbol()->identifier(), func->symbol()->id());
      //!!!debug
      //if(fsymb)
      //printf("\n%s   %s \n", header(func->variant()),fsymb->identifier()); 
      //else {
      //printf("Function name error  \n");
      //return;
      //}
   //get the last node of the program unit(function) 
    last = func->lastNodeOfStmt();
    end_of_unit = last;
    if (!(last->variant() == CONTROL_END))
        printf(" END Statement is absent\n");

    //**********************************************************************
    //           Specification Directives Processing 
    //**********************************************************************
    // follow the statements of the function in lexical order
    // until first executable statement
    for (stmt = first; stmt && (stmt != last); stmt = stmt->lexNext()) {

        if (!isSgExecutableStatement(stmt)) //is Fortran specification statement
    // isSgExecutableStatement: 
    //               FALSE  -  for specification statement of Fortan 90
    //               TRUE   -  for executable statement of Fortan 90 and
    //                         all directives of F-DVM 
        {
            //!!!debug
                //  printVariantName(stmt->variant()); //for debug
                //  printf("\n");


            if ((stmt->variant() == DATA_DECL) || (stmt->variant() == STMTFN_STAT)) {
                /* if(stmt->variant() == STMTFN_STAT && stmt->expr(0) && stmt->expr(0)->symbol() && ((!strcmp(stmt->expr(0)->symbol()->identifier(),"number_of_processors")) || (!strcmp(stmt->expr(0)->symbol()->identifier(),"processors_rank")) || (!strcmp(stmt->expr(0)->symbol()->identifier(),"processors_size")))){
                        stmt=stmt->lexPrev();
                        stmt->lexNext()->extractStmt();
                                      //deleting the statement-function declaration named
                                  //  NUMBER_OF_PROCESSORS or PROCESSORS_RANK or PROCESSORS_SIZE
                        continue;
                     }
                   */
                if (!data_stf)
                    data_stf = stmt; //first statement in data-or-function statement part 
                continue;
            }
            if (stmt->variant() == ENTRY_STAT) {
                //err("ENTRY statement is not permitted in FDVM", stmt);  
                //warn("ENTRY among specification statements", 81,stmt);  
                continue;
            }

            continue;
        }

        if ((stmt->variant() == FORMAT_STAT))
            continue;


        // processing the DVM Specification Directives

         /*   //including the DVM specification directive to list of these directives
              pstmt = addToStmtList(pstmt, stmt);

            switch(stmt->variant()) {

               case(HPF_TEMPLATE_STAT):
               case(HPF_PROCESSORS_STAT):
               continue;
           }
        */
        // all declaration statements are processed,
        // current statement is executable (F77/DVM)

        break;
    }

    //**********************************************************************
    //              LibDVM References Generation
    //           for distributed and aligned arrays
    //**********************************************************************


    first_exec = stmt; // first executable statement

    lab_exec = first_exec->label(); // store the label of first ececutable statement 
    last_spec = first_exec->lexPrev();//may be extracted after
    where = first_exec; //before first executable statement will be inserted new statements
    stam = NULL;


    //**********************************************************************
    //           Executable Directives Processing 
    //**********************************************************************

    //initialization
    // . . .
    //follow the executable statements in lexical order until last statement
    // of the function

    for (stmt = first_exec; stmt && (stmt != last); stmt = stmt->lexNext()) {  //for(stmt=first_exec;stmt ; stmt=stmt->lexNext()) 
        cur_st = stmt;

        switch (stmt->variant()) {

        case ENTRY_STAT:
            // !!!!!!!
            break;

        case CONTROL_END:
        case STOP_STAT:
        case PAUSE_NODE:
        case GOTO_NODE:             // GO TO             
            break;

        case SWITCH_NODE:           // SELECT CASE ...
        case ARITHIF_NODE:          // Arithmetical IF
        case IF_NODE:               // IF... THEN
        case WHILE_NODE:            // DO WHILE (...) 
        case CASE_NODE:             // CASE ...
        case ELSEIF_NODE:           // ELSE IF...
        case LOGIF_NODE:            // Logical IF
            FunctionCallSearch(stmt->expr(0));
            break;

        case COMGOTO_NODE:          // Computed GO TO
        case OPEN_STAT:
        case CLOSE_STAT:
        case INQUIRE_STAT:
        case BACKSPACE_STAT:
        case ENDFILE_STAT:
        case REWIND_STAT:
            FunctionCallSearch(stmt->expr(1));
            break;

        case PROC_STAT: {           // CALL
            SgExpression *el;
#ifdef __SPF
            if (needToInline.find(stmt->symbol()->identifier()) != needToInline.end())
                Call_Site(stmt->symbol(), 1);
            else
                Call_Site(stmt->symbol(), 0);
#else
            Call_Site(stmt->symbol(), 1);
#endif
            // looking through the arguments list
            for (el = stmt->expr(0); el; el = el->rhs())
                Arg_FunctionCallSearch(el->lhs());   // argument
        }
                        break;

        case ASSIGN_STAT:             // Assign statement
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
        case FOR_NODE:
            FunctionCallSearch(stmt->expr(0));   // left part
            FunctionCallSearch(stmt->expr(1));   // right part
            break;

        default:
            break;
        }

    } // end of processing executable statement/directive 

   //END_:
       // for debugging
    if (deb_reg > 1)
        PrintGraphNode(cur_node);
    return;
}





void Replace(SgStatement *stfun) {
    SgSymbol *fname, *name;
    fname = stfun->symbol();
    SYMB_IDENT(fname->thesymb) = (char*)"DEBUG";
    name = stfun->lexNext()->expr(0)->lhs()->symbol();
    SYMB_IDENT(name->thesymb) = (char*)"dvdvdv";
}

/*
void TransFunc(SgStatement *func) {
  SgStatement *stmt,*last,*rmout, *data_stf, *first, *first_dvm_exec, *last_spec, *stam;
  SgStatement *st_newv = NULL;// for NEW_VALUE directives
  SgExpression *e;
  SgStatement *task_region_parent, *on_parent, *mod_proc, *begbl;
  SgStatement *copy_proc = NULL;
  SgLabel *lab_exec;

  int i;
  int begin_block;
  distribute_list *distr =  NULL;
  distribute_list *dsl,*distr_last;
  align *pal = NULL;
  align *node, *root;
  stmt_list *pstmt = NULL;
  int inherit_is = 0;
  int contains[2];
           CallGraph(func);
return;
   if(func->variant() != PROG_HEDR){
       stmt=func->copyPtr();
       Replace(stmt);
       func->insertStmtBefore(*stmt,*(func->controlParent()));
   }
   return;
}
*/



void FunctionCallSearch(SgExpression *e)
{
    SgExpression *el;
    if (!e)
        return;

    /*  if(isSgArrayRefExp(e)) {
        for(el=e->lhs(); el; el=el->rhs())
           FunctionCallSearch(el->lhs());

        return;
      }
    */

    if (isSgFunctionCallExp(e)) 
    {
#ifdef __SPF
        if (needToInline.find(e->symbol()->identifier()) != needToInline.end())
            Call_Site(e->symbol(), 1);
        else
            Call_Site(e->symbol(), 0);
#else
        Call_Site(e->symbol(), 1);
#endif
        for (el = e->lhs(); el; el = el->rhs())
            Arg_FunctionCallSearch(el->lhs());
        return;
    }
    FunctionCallSearch(e->lhs());
    FunctionCallSearch(e->rhs());
    return;
}

void Arg_FunctionCallSearch(SgExpression *e)
{
    FunctionCallSearch(e);
    return;
}

void FunctionCallSearch_Left(SgExpression *e)
{
    FunctionCallSearch(e);
}


void Call_Site(SgSymbol *s, int inlined)
{
    graph_node * gnode;
    //printf("\n%s  id= %d \n", s->identifier(), s->id());
    if (!do_dummy  && isDummyArgument(s)) 
        return;
    if (!do_stmtfn && isStatementFunction(s)) 
        return;
    // if(isIntrinsicFunction(s)) return; 
 //printf("\nLINE %d", cur_st->lineNumber());
    gnode = CreateGraphNode(s, NULL);
    CreateOutcomingEdge(gnode, inlined); // for node 'cur_node' edge: [cur_node]-> gnode
    CreateIncomingEdge(gnode, inlined); // for node 'gnode'    edge:  cur_node ->[gnode]
}

graph_node *CreateGraphNode(SgSymbol *s, SgStatement *header_st)
{
    graph_node * gnode;
    graph_node **pnode = new (graph_node *);
    gnode = NodeForSymbInGraph(s, header_st);
    if (!gnode)
        gnode = NewGraphNode(s, header_st);

    *pnode = gnode;
    if (!ATTR_NODE(s)) 
    {
        s->addAttribute(GRAPH_NODE, (void*)pnode, sizeof(graph_node *));
        if (deb_reg > 1)
            printf("attribute NODE[%d] for %s[%d]\n", GRAPHNODE(s)->id, s->identifier(), s->id());
    }
    return gnode;
}

graph_node *NodeForSymbInGraph(SgSymbol *s, SgStatement *stheader)
{
    graph_node *ndl;
    for (ndl = node_list; ndl; ndl = ndl->next) 
    {
#ifdef __SPF
        //TODO: improve this!
        if (std::string(s->identifier()) == ndl->symb->identifier())
        {
            if (ndl->st_header == NULL)
            {
                ndl->st_header = stheader;
                ndl->symb = s;
            }
            return ndl;
        }
#else
        if (s == ndl->symb) 
            return ndl;
        if ((ndl->st_header == NULL) && !strcmp(ndl->symb->identifier(), s->identifier()) && (ndl->symb->scope() == s->scope()))
        {
            if (stheader)
            {
                ndl->st_header = stheader;
                ndl->symb = s;
            }
            return ndl;
        }
#endif
        /*  else   //if(s->thesymb->decl == NULL)
        {   Err_g("Call graph error '%s' ", s->identifier(), 1);
        (void) fprintf( stderr,"%s %d    %d  in line %d\n",s->identifier(),s->id(),ndl->symb->id(),cur_st->lineNumber());
        }
        */
    }
    return NULL;
}

graph_node *NewGraphNode(SgSymbol *s, SgStatement *header_st)
{
    graph_node * gnode;

    gnode = new graph_node;
    gnode->id = ++gcount;
    gnode->next = node_list;
    node_list = gnode;
    gnode->file = current_file;
    gnode->st_header = header_st;
    gnode->symb = s;
    gnode->to_called = NULL;
    gnode->from_calling = NULL;
    gnode->split = 0;
    gnode->tmplt = 0;
    gnode->clone = 0;
    gnode->count = 0;
    return(gnode);
}

edge *CreateOutcomingEdge(graph_node *gnode, int inlined)
{
    edge *out_edge, *edgl;
    //SgSymbol *sunit;
     //sunit = cur_func->symbol();

     // testing outcoming edge list of current (calling) routine graph-node: cur_node 
    for (edgl = cur_node->to_called; edgl; edgl = edgl->next)
        if ((edgl->to->symb == gnode->symb) && (edgl->inlined == inlined)) //there is outcoming edge: [cur_node]->gnode 
            return(edgl);
    // creating new edge: [cur_node]->gnode 
    out_edge = NewEdge(NULL, gnode, inlined);   //NULL -> cur_node
    out_edge->next = cur_node->to_called;
    cur_node->to_called = out_edge;
    return(out_edge);
}

edge *CreateIncomingEdge(graph_node *gnode, int inlined)
{
    edge *in_edge, *edgl;
    //SgSymbol *sunit;
    //sunit = cur_func->symbol();

    // testing incoming edge list of called routine graph-node: gnode 
    for (edgl = gnode->from_calling; edgl; edgl = edgl->next)
        if ((edgl->from->symb == cur_node->symb) && (edgl->inlined == inlined)) //there is incoming edge: : cur_node->[gnode] 
            return(edgl);
    // creating new edge: cur_node->[gnode]
    in_edge = NewEdge(cur_node, NULL, inlined);   //NULL -> gnode
    in_edge->next = gnode->from_calling;
    gnode->from_calling = in_edge;
    return(in_edge);
}

edge *NewEdge(graph_node *from, graph_node *to, int inlined)
{
    edge *nedg;
    nedg = new edge;
    nedg->from = from;
    nedg->to = to;
    nedg->inlined = inlined;
    return(nedg);
}
/**********************************************************************/

/*    Testing Functions                                               */

/**********************************************************************/

int isDummyArgument(SgSymbol *s)
{
    if (s->thesymb->entry.var_decl.local == IO)  // is dummy argument
        return(1);
    else
        return(0);
}

int isHeaderStmtSymbol(SgSymbol *s)
{
    return(DECL(s) == 1 && (s->variant() == FUNCTION_NAME || s->variant() == PROCEDURE_NAME || s->variant() == PROGRAM_NAME));
}

int isStatementFunction(SgSymbol *s)
{
    if (s->scope() == cur_func && s->variant() == FUNCTION_NAME) 
        return 1; //is statement function symbol
    else 
        return 0;
}

int isHeaderNode(graph_node *gnode)
{
    //header node represent a "top level" routine:
    //main program, or any subprogram which was called 
    //without inline expansion somewhere in the original program 
    edge * edgl;
#ifdef __SPF
    if (needToInline.find(gnode->symb->identifier()) == needToInline.end())
#else
    if (gnode->symb->variant() == PROGRAM_NAME)
#endif
        return 1;

    for (edgl = gnode->from_calling; edgl; edgl = edgl->next)
        if (!edgl->inlined) 
            return 1;
    return 0;
}

int isDeadNode(graph_node *gnode)
{
    // dead node represent a "dead" routine:
    // a subprogram which was not called
#ifdef __SPF
    if (gnode->from_calling || needToInline.find(gnode->symb->identifier()) == needToInline.end())
#else
    if (gnode->from_calling || gnode->symb->variant() == PROGRAM_NAME)
#endif
        return 0;
    else
        return 1;
}

int isNoBodyNode(graph_node *gnode)
{
    // nobody node represent a "nobody" routine: intrinsic or absent

    if (gnode->st_header)
        return(0);
    else
        return(1);
}

/**********************************************************************/
stmt_list* addToStmtList(stmt_list *pstmt, SgStatement *stat)
{
    // adding the statement to the beginning of statement list 
    // pstmt-> stat -> stmt-> ... -> stmt     
    stmt_list * stl;
    if (!pstmt) 
    {
        pstmt = new stmt_list;
        pstmt->st = stat;
        pstmt->next = NULL;
    }
    else 
    {
        stl = new stmt_list;
        stl->st = stat;
        stl->next = pstmt;
        pstmt = stl;
    }
    return pstmt;
}

stmt_list* delFromStmtList(stmt_list *pstmt)
{
    // deletinging last statement from the statement list 
    // pstmt-> stat -> stmt-> ... -> stmt
    pstmt = pstmt->next;
    return (pstmt);
}


graph_node_list* addToNodeList(graph_node_list *pnode, graph_node *gnode)
{
    // adding the node to the beginning of node list 
    // pnode-> gnode -> gnode-> ... -> gnode     
    graph_node_list * ndl;
    if (!pnode) {
        pnode = new graph_node_list;
        pnode->node = gnode;
        pnode->next = NULL;
    }
    else {
        ndl = new graph_node_list;
        ndl->node = gnode;
        ndl->next = pnode;
        pnode = ndl;
    }
    return (pnode);
}

graph_node_list* delFromNodeList(graph_node_list *pnode, graph_node *gnode)
{
    // deleting the node from the node list 

    graph_node_list *ndl, *l;
    if (!pnode) 
        return NULL;

    if (pnode->node == gnode) 
        return pnode->next;
    l = pnode;
    for (ndl = pnode->next; ndl; ndl = ndl->next)
    {
        if (ndl->node == gnode)
        {
            l->next = ndl->next;
            return pnode;
        }
        else
            l = ndl;
    }
    return pnode;
}

graph_node_list  *isInNodeList(graph_node_list *pnode, graph_node *gnode)
{
    // testing: is there node in the node list 

    graph_node_list * ndl;
    if (!pnode) return (NULL);
    for (ndl = pnode; ndl; ndl = ndl->next)
    {
        if (ndl->node == gnode)
            return(ndl);
    }
    return (NULL);
}


void PrintGraphNode(graph_node *gnode)
{
    edge * edgl;
    printf("%s(%d)  ->   ", gnode->symb->identifier(), gnode->symb->id());    
    for (edgl = gnode->to_called; edgl; edgl = edgl->next)
        printf("  %s(%d)", edgl->to->symb->identifier(), edgl->to->symb->id());
    printf("\n");
}

void PrintGraphNodeWithAllEdges(graph_node *gnode)
{
    edge * edgl;
    printf("\n");
    for (edgl = gnode->from_calling; edgl; edgl = edgl->next)
        printf("   %s(%d)", edgl->from->symb->identifier(), edgl->from->symb->id());
    if (!gnode->from_calling)
        printf("          ");
    printf("   ->%s(%d)->   ", gnode->symb->identifier(), gnode->symb->id());
    for (edgl = gnode->to_called; edgl; edgl = edgl->next)
        printf("   %s(%d)", edgl->to->symb->identifier(), edgl->to->symb->id());
}

void PrintWholeGraph()
{
    graph_node *ndl;
    printf("\n%s\n", "C a l l  G r a p h");
    for (ndl = node_list; ndl; ndl = ndl->next)
        PrintGraphNode(ndl);
    printf("\n");
    fflush(NULL);
}

void PrintWholeGraph_kind_2()
{
    graph_node *ndl;
    printf("\nC a l l  G r a p h  2\n");
    for (ndl = node_list; ndl; ndl = ndl->next)
        PrintGraphNodeWithAllEdges(ndl);
    printf("\n");
    fflush(NULL);
}


void BuildingHeaderNodeList()
{
    //Build a list of header nodes to represent "top level" routines

    graph_node *ndl;
    if (deb_reg)
        printf("\nH e a d e r   N o d e   L i s t\n");
    for (ndl = node_list; ndl; ndl = ndl->next) {
        if (isHeaderNode(ndl))
        {
            header_node_list = addToNodeList(header_node_list, ndl);
            if (deb_reg)
                printf("%s\n", ndl->symb->identifier());
        }
    }
}

void RemovingDeadSubprograms()
{
    //Prune the call graph by removing nodes representing "dead" subprogram

    graph_node *ndl, *lnode;
    int dead;
    edge *edgl;

    do
    {
        lnode = NULL; dead = 0;
        for (ndl = node_list; ndl; ndl = ndl->next) {
            if (isDeadNode(ndl)) //removing node ndl
            {
                if (deb_reg)
                    printf("\n%s(%d)  dead  ", ndl->symb->identifier(), ndl->symb->id());
                dead = 1;
                //removing dead node from node_list
                if (lnode)
                    lnode->next = ndl->next;
                else
                    node_list = ndl->next;
                //removing  edges that are incomig to any node from dead node 
                for (edgl = ndl->to_called; edgl; edgl = edgl->next)
                    DeleteIncomingEdgeFrom(edgl->to, ndl);
                //removing the code of subpogram (extracting statements)
                //?????????
                //includind dead node in dead_node_list
                dead_node_list = addToNodeList(dead_node_list, ndl);
            }
            else
                lnode = ndl;
        }
    } while (dead == 1);

    if (dead_node_list && deb_reg) {
        graph_node_list *dl;
        printf("\n%s\n", "D e a d   N o d e   L i s t");
        for (dl = dead_node_list; dl; dl = dl->next)
            printf("\n%s\n", dl->node->symb->identifier());
    }
}


void NoBodySubprograms()
{
    //looking through the call graph for nodes representing "no body" subprogram: intrinsic or absent

    graph_node *ndl, *lnode;
    int empty;
    edge *edgl;

    do
    {
        lnode = NULL; empty = 0;
        for (ndl = node_list; ndl; ndl = ndl->next) {
            if (isNoBodyNode(ndl)) //removing node ndl
            {
                empty = 1;

                //removing empty node from node_list
                if (lnode)
                    lnode->next = ndl->next;
                else
                    node_list = ndl->next;
                //removing  edges that are incoming to empty node from any node 
                for (edgl = ndl->from_calling; edgl; edgl = edgl->next)
                    DeleteOutcomingEdgeTo(edgl->from, ndl);
                //includind empty node in nobody_node_list
                nobody_node_list = addToNodeList(nobody_node_list, ndl);

            }
            else
                lnode = ndl;
        }
    } while (empty == 1);

    if (nobody_node_list && deb_reg) {
        graph_node_list *dl;
        printf("\n\nN o  B o d y   N o d e   L i s t\n");
        for (dl = nobody_node_list; dl; dl = dl->next)
            printf("%s\n", dl->node->symb->identifier());
    }
    //deleting nobody nodes
    //?????????? there are references to node from attribute(GRAPH_NODE) of symbols
}

void DeleteIncomingEdgeFrom(graph_node *gnode, graph_node *from)
{
    // deleting edge that is incoming to node 'gnode' from node 'from' 
    edge *edgl, *ledge;
    ledge = NULL;
    for (edgl = gnode->from_calling; edgl; edgl = edgl->next) {
        if (edgl->from == from) {
            if (deb_reg > 1)
                printf("\n%s(%d)-%s(%d) edge dead  ", from->symb->identifier(), from->symb->id(), gnode->symb->identifier(), gnode->symb->id());

            if (ledge)
                ledge->next = edgl->next;
            else
                gnode->from_calling = edgl->next;
        }
        else
            ledge = edgl;
    }
}

void DeleteOutcomingEdgeTo(graph_node *gnode, graph_node *gto)
{
    // deleting edge that is outcoming from node 'gnode' to node 'gto' 
    edge *edgl, *ledge;
    ledge = NULL;
    for (edgl = gnode->to_called; edgl; edgl = edgl->next) {
        if (edgl->to == gto) {
            if (deb_reg > 1)
                printf("\n%s(%d)-%s(%d) edge empty  ", gnode->symb->identifier(), gnode->symb->id(), gto->symb->identifier(), gto->symb->id());

            if (ledge)
                ledge->next = edgl->next;
            else
                gnode->to_called = edgl->next;
        }
        else
            ledge = edgl;
    }
}


void ScanSymbolTable(SgFile *f)
{
    SgSymbol *s;
    for (s = f->firstSymbol(); s; s = s->next())
        if (isHeaderStmtSymbol(s))
            printSymb(s);
}

void ScanTypeTable(SgFile *f)
{
    SgType *t;
    for (t = f->firstType(); t; t = t->next())
    {   // printf("TYPE[%d] : ", t->id());
        printType(t);
    }
}

void ReseatEdges(graph_node *gnode, graph_node *newnode)
{//reseat all edges representing inlined calls to gnode to point to newnode 
    edge *edgl, *tol, *ledge, *curedg;
    graph_node *from;
    ledge = NULL;
    // for(edgl=gnode->from_calling; edgl; edgl=edgl->next)
// looking through the incoming edge list of gnode
    edgl = gnode->from_calling;
    while (edgl)
    {
        if (edgl->inlined)
        {
            from = edgl->from;
            // reseating outcoming edge to 'gnode' to point to 'newnode'
            for (tol = from->to_called; tol; tol = tol->next)
                if (tol->to == gnode && tol->inlined)
                {
                    tol->to = newnode; break;
                }
            // removing "inlined" incoming edge of gnode
            if (ledge)
                ledge->next = edgl->next;
            else
                gnode->from_calling = edgl->next;

            curedg = edgl;    // set curedg to point at removed edge  
            edgl = edgl->next;  // to next node of list

            // adding removed edge  to 'newnode' 
            curedg->next = newnode->from_calling;
            newnode->from_calling = curedg;

        }
        else
        {
            ledge = edgl;
            edgl = edgl->next;
        }
    } //end while  
}

graph_node *SplittingNode(graph_node *gnode)
{
    if (!gnode->split)
    { // . . . !!! new COMMON block and BLOCK DATA
        gnode->split = 1;
    }
    if (deb_reg)
        printf("\nSplitting NODE[%d]  %s\n", gnode->id, gnode->symb->identifier());

    return (CloneNode(gnode));
}

graph_node *CloneNode(graph_node *gnode)
{// Clone gnode to create a new node gnew
    graph_node *gnew;
    SgSymbol *scopy;
    graph_node **pnode = new (graph_node *);
    // copying subprogram, inserting after END statement of last subroutine of current file
    scopy = &((gnode->symb)->copySubprogram(*(global_st)));    // copyAcrossFiles(*(cur_st)));
 // for debug
    //printf(" \n****** BODY COPY FUNCTION(0) %s [%d] ********\n", scopy->identifier(), scopy->id());
    //scopy->body()->unparsestdout(); 

 // creating new graph node 
    gnew = NewGraphNode(scopy, scopy->body());
    gnew->clone = 1;
    // copying edges
          //CopyIncomingEdges (gnode,gnew);
    CopyOutcomingEdges(gnode, gnew);
    // adding the attribute GRAPH_NODE to new symbol: scopy
    *pnode = gnew;
    scopy->addAttribute(GRAPH_NODE, (void*)pnode, sizeof(graph_node *));
    if (deb_reg > 1)
        printf("\n attribute NODE[%d] for %s[%d]  CLONE of NODE[%d]\n", GRAPHNODE(scopy)->id, scopy->identifier(), scopy->id(), gnode->id);

    return(gnew);
}

void CopyOutcomingEdges(graph_node *gnode, graph_node *gnew)
{
    edge *out_edge, *in_edge, *edgl;
    graph_node *s;
    // looking through the outcoming edge list of gnode
    for (edgl = gnode->to_called; edgl; edgl = edgl->next)
    {
        s = edgl->to; // successor of gnode
  // creating new edge of gnew (copy of edgl)
        out_edge = NewEdge(NULL, edgl->to, edgl->inlined);
        out_edge->next = gnew->to_called;
        gnew->to_called = out_edge;
        // creating new edge of  s (successor  of gnode)
        in_edge = NewEdge(gnew, NULL, edgl->inlined);
        in_edge->next = s->from_calling;
        s->from_calling = in_edge;
    }
    return;
}

void CopyIncomingEdges(graph_node *gnode, graph_node *gnew)
{
    edge *in_edge, *out_edge, *edgl;
    graph_node *p;
    // looking through the incoming edge list of gnode
    for (edgl = gnode->from_calling; edgl; edgl = edgl->next)
    {
        p = edgl->from; // predecessor of gnode
     // creating new edge of gnew (copy of edgl) 
        in_edge = NewEdge(edgl->from, NULL, edgl->inlined);
        in_edge->next = gnew->from_calling;
        gnew->from_calling = in_edge;
        // creating new edge of p (predecessor of gnode)
        out_edge = NewEdge(NULL, gnew, edgl->inlined);
        out_edge->next = p->to_called;
        p->to_called = out_edge;

    }
    return;
}

void RemovingUninlinedEdges()
{
    // Removing all edges representing uninlined calls
    graph_node *ndl;
    edge  *edgl, *ledge;
    for (ndl = node_list; ndl; ndl = ndl->next)
    {
        ledge = NULL;
        // looking through the incoming edge list 
        for (edgl = ndl->from_calling; edgl; edgl = edgl->next)
        {
            if (!edgl->inlined)
            {//removing uninlined edge
                if (ledge)
                    ledge->next = edgl->next;
                else
                    ndl->from_calling = edgl->next;
            }
            else
                ledge = edgl;
        }
        ledge = NULL;
        // looking through the outcoming edge list 
        for (edgl = ndl->to_called; edgl; edgl = edgl->next)
        {
            if (!edgl->inlined)
            {//removing uninlined edge
                if (ledge)
                    ledge->next = edgl->next;
                else
                    ndl->to_called = edgl->next;
            }
            else
                ledge = edgl;
        }
    }
}


/************************ P A R T I T I O N ************************************/
void Partition()
{
    graph_node_list *ndl, *replication, *interval, *Ilist;
    graph_node *hnode, *n, *s, *nnew;
    edge *edg;
    for (ndl = header_node_list; ndl; ndl = ndl->next)
    {
        hnode = ndl->node;
        replication = NULL; interval = NULL;
        interval = addToNodeList(interval, hnode);
        hnode->Inext = NULL; DAG_list = hnode;

        while (replication || unvisited_in(interval))
        {//-------------------------------------------------------
            do
                for (Ilist = interval; Ilist; Ilist = Ilist->next)
                {
                    n = Ilist->node;
                    if (n->visited == 1) continue;
                    n->visited = 1;
                    for (edg = n->to_called; edg; edg = edg->next)
                    {
                        s = edg->to;
                        if (inInterval(s, interval)) continue;
                        if (allPredecessorInInterval(s, interval))
                        {
                            interval = addToNodeList(interval, s);
                            s->Inext = DAG_list; DAG_list = s;
                            MoveEdgesPointTo(s);
                            replication = delFromNodeList(replication, s);
                        }
                        else
                        {
                            if (!isInNodeList(replication, s))
                                replication = addToNodeList(replication, s);
                        }
                    }
                }
            while (unvisited_in(interval));
            //--------------------------------------------------------
            for (Ilist = replication; Ilist; Ilist = Ilist->next)
            {
                n = Ilist->node;
                replication = delFromNodeList(replication, n);
                nnew = SplittingNode(n);
                interval = addToNodeList(interval, n);
                n->Inext = DAG_list; DAG_list = n;
                ReseatEdgesOutsideToNew(n, nnew, interval);
                MoveEdgesPointTo(n);
            }
        }
    }
    return;
}

int unvisited_in(graph_node_list *interval)
{
    graph_node_list *Ilist;
    for (Ilist = interval; Ilist; Ilist = Ilist->next)
        if (Ilist->node->visited == 0) return(1);
    return(0);
}

int inInterval(graph_node *gnode, graph_node_list *interval)
{
    graph_node_list *Ilist;
    for (Ilist = interval; Ilist; Ilist = Ilist->next)
        if (Ilist->node == gnode) return(1);
    return(0);
}

int allPredecessorInInterval(graph_node *gnode, graph_node_list *interval)
{
    edge *edg;
    for (edg = gnode->from_calling; edg; edg = edg->next)
        if (!inInterval(edg->from, interval)) return(0);
    return(1);
}

void MoveEdgesPointTo(graph_node *gnode)
{
    edge *edg, *el;
    for (edg = gnode->from_calling; edg; edg = edg->next)
    {
        edg->inlined = 2;
        for (el = edg->from->to_called; el; el = el->next)
            if (el->to == gnode)
            {
                el->inlined = 2; break;
            }
    }
}

void ReseatEdgesOutsideToNew(graph_node *gnode, graph_node *gnew, graph_node_list *interval)
{//reseat all edges from nodes outside interval to 'gnode' to point to 'gnew' 
    edge *edgl, *tol, *ledge, *curedg;
    ledge = NULL;
    //looking through the incoming edge list of 'gnode' 
    edgl = gnode->from_calling;
    while (edgl)
        //for(edgl=gnode->from_calling; edgl; edgl=edgl->next)
    {
        if (inInterval(edgl->from, interval)) { ledge = edgl; edgl = edgl->next; continue; }
        // reseating  outcoming edge to 'gnode' to point to 'gnew'
        for (tol = edgl->from->to_called; tol; tol = tol->next)
            if (tol->to == gnode)
            {
                tol->to = gnew; break;
            }
        // removing  incoming edge of 'gnode'
        if (ledge)
            ledge->next = edgl->next;
        else
            gnode->from_calling = edgl->next;

        curedg = edgl;    // set curedg to point at removed edge  
        edgl = edgl->next;  // to next node of list

        // adding removed edge to 'gnew' 
        curedg->next = gnew->from_calling;
        gnew->from_calling = curedg;
    }
}

#ifdef __SPF
static void splitString(const string &strIn, const char delim, vector<string> &result)
{
    std::stringstream ss;
    ss.str(strIn);

    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);
}

void removeIncludeStatsAndUnparse(SgFile *file, const char *fileName, const char *fout)
{
    fflush(NULL);
    int funcNum = file->numberOfFunctions();
    FILE *currFile = fopen(fileName, "r");
    if (currFile == NULL)
    {        
        printf("ERROR: Can't open file %s for read\n", fileName);
        //addToGlobalBufferAndPrint(buf);
        //throw(-1);
    }

    // name -> unparse comment
    map<string, string> includeFiles;

    // TODO: extend buff size in dynamic
    char buf[8192];
    while (!feof(currFile))
    {
        char *read = fgets(buf, 8192, currFile);
        if (read)
        {
            string line(read);
            size_t posF = line.find("include");
            if (posF != string::npos)
            {
                posF += sizeof("include") - 1;
                int tok = 0;
                size_t st = -1, en;
                for (size_t k = posF; k < line.size(); ++k)
                {
                    if (line[k] == '\'' && tok == 1)
                        break;
                    else if (line[k] == '\'')
                        tok++;
                    else if (tok == 1 && st == -1)
                        st = k;
                    else
                        en = k;
                }
                string inclName(line.begin() + st, line.begin() + en + 1);

                auto toInsert = includeFiles.find(inclName);
                if (toInsert == includeFiles.end())
                    includeFiles.insert(toInsert, make_pair(inclName, line));
                //printf("insert %s -> %s\n", inclName.c_str(), line.c_str());
            }
        }
    }

    vector<string> needDel;

    vector<SgStatement*> removeFunctions;
    for (int i = 0; i < funcNum; ++i)
    {
        SgStatement *st = file->functions(i);
        if (string(st->fileName()) != fileName)
        {
            removeFunctions.push_back(st);
            continue;
        }
        SgStatement *lastNode = st->lastNodeOfStmt();

        set<string> toInsert;
        SgStatement *first = NULL;
        bool start = false;

        while (st != lastNode)
        {
            if (st == NULL)
            {
                printf("Internal error\n");
                break;
            }

            if (strcmp(st->fileName(), fileName))
            {
                toInsert.insert(st->fileName());
                start = true;
            }
            else if (start && first == NULL)
                first = st;
            st = st->lexNext();
        }

        for (auto it = toInsert.begin(); it != toInsert.end(); ++it)
        {
            auto foundIt = includeFiles.find(*it);
            if (foundIt != includeFiles.end())
            {
                if (first)
                {
                    if (first->comments() == NULL)
                        first->addComment(foundIt->second.c_str());
                    else
                    {
                        const char *comments = first->comments();
                        if (strstr(comments, foundIt->second.c_str()) == NULL)
                            first->addComment(foundIt->second.c_str());
                    }
                }
                else //TODO
                    printf("Internal error\n");
            }
        }

        // remove code from 'include' only from file, not from Sage structures
        start = file->functions(i);
        st = file->functions(i);
        lastNode = st->lastNodeOfStmt();

        while (st != lastNode)
        {
            if (st == NULL)
            {
                printf("Internal error\n");
                break;
            }

            if (strcmp(st->fileName(), fileName))
                splitString(st->unparse(), '\n', needDel);
            st = st->lexNext();
        }
    }

    for (int i = 0; i < removeFunctions.size(); ++i)
        removeFunctions[i]->extractStmt();

    FILE *fOut = fopen(fout, "w");
    if (fOut == NULL)
        printf("Internal error\n");
    file->unparse(fOut);
    fclose(fOut);

    if (needDel.size() > 0)
    {
        fOut = fopen(fout, "r");

        string currFile = "";
        int idxDel = 0;
        while (!feof(fOut))
        {
            fgets(buf, 8192, fOut);
            const int len = strlen(buf);
            if (len > 0)
                buf[len - 1] = '\0';

            if (needDel.size() > idxDel)
            {
                if (needDel[idxDel] == buf)
                    idxDel++;
                else
                {
                    currFile += buf;
                    currFile += "\n";
                }
            }
            else
            {
                currFile += buf;
                currFile += "\n";
            }
        }
        fclose(fOut);

        fOut = fopen(fout, "w");
        fwrite(currFile.c_str(), sizeof(char), currFile.length(), fOut);
        fclose(fOut);
    }
}
#endif
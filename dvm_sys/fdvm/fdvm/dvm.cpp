 
/*********************************************************************/
/*                   Fortran DVM  V.5    2011   (DVM+OpenMP+ACC)     */
/*********************************************************************/ 

#include <stdio.h>
#include <string.h>

#define IN_DVM_
#include "dvm.h"
#undef IN_DVM_

#include "libSageOMP.h"

const char *name_loop_var[MAX_DIMS+1] = {"idvm00","idvm01","idvm02","idvm03", "idvm04","idvm05","idvm06","idvm07","idvm08","idvm09","idvm10","idvm11","idvm12","idvm13","idvm14","idvm15"};  
const char *name_bufIO[Ntp] = {"i000io","r000io", "d000io","c000io","l000io","dc00io","ch00io","i100io","i200io","i800io","l100io","l200io","l800io"};
SgSymbol *rmbuf[Ntp];
const char *name_rmbuf[Ntp] = {"i000bf","r000bf", "d000bf","c000bf","l000bf","dc00bf","ch00bf","i100bf","i200bf","i800bf","l100bf","l200bf","l800bf"};
SgSymbol *dvmcommon, *dvmcommon_ch;
SgSymbol *heapcommon;
SgSymbol *redcommon;
SgSymbol *dbgcommon;
int lineno;  // number of line in file
SgStatement *first_exec; // first executable statement in procedure
int nproc,ndis,nblock,ndim, nblock_all;
SgVariableSymb *mem_symb[Ntp]; 
int mem_use[Ntp];

int lab;  // current label  
//SgExpression * size_array, *array_handle, *align_template;
//SgExpression * axis_array, *coeff_array, *const_array;
//SgExpression *rml; //remote-variable list of REMOTE_ACCESS directive

int inasynchr; //set to 1 in the range of ASYNCHRONOUS
symb_list *dsym;  //distributed array symbol list
group_name_list *grname;  //shadow/reduction group name list
int v_print = 0; //set to 1 by -v flag
int warn_all = 0; //set to 1 by -w flag
int own_exe;
symb_list *redvar_list;
int pointer_in_tree; //set to 1 if there is a POINTER in alignment tree
                     //used by GenDistArray and GenAlignArray 
symb_list *proc_symb;//processor array symbol list
symb_list *task_symb;//task array symbol list
symb_list * consistent_symb;// consistent array symbol list
symb_list *async_symb;// ASYNCID symbol list
symb_list *loc_templ_symb;// local TEMPLATE symbol list
symb_list *index_symb;// INDEX_DELTA variable list (code optimization)
int in_task_region;//set to 1 in the range of TASK_REGION
int task_ind; //current task index is storing in dvm000(task_ind)
int in_task; //set to 1 in the range of ON directive
SgSymbol *task_array;// current task array symbol pointer 
SgLabel *task_lab; 
SgStatement *task_do;
SgStatement * task_region_st;
fragment_list *cur_fragment = NULL; //current fragment number (used in debuging directives)
SgExpression *heap_ar_decl;
int   is_heap_ref;
int heap_size; //calculated size of array HEAP(volume of memory for all pointer headers)
stmt_list * pref_st; //list of PREFETCH directive in procedure
int maxbuf = 5; //maximal number of remote group  buffers for given array
int gen_block, mult_block;
SgExpression *async_id;
SgExpression *struct_component;
SgSymbol *file_var_s;
int nloopred; //counter of parallel loops with reduction group
int nloopcons; //counter of parallel loops with consistent group
stmt_list *wait_list; // list of REDUCTION_WAIT directives
int task_ps = 0;
int opt_base, opt_loop_range; //set on by compiler options (code optimization options)
SgExpression *sum_dvm = NULL;
int dvm_const_ref;
int unparse_functions;
int privateall = 0;

extern SgStatement *parallel_dir;
extern int iacross;

extern "C" int out_free_form;
extern "C" int out_upper_case;
extern "C" int out_line_unlimit;
extern "C" PTR_SYMB last_file_symbol;

Options options;

//
//-----------------------------------------------------------------------
// FOR DEBUGGING
//#include "dump_info.C"
//-----------------------------------------------------------------------


int main(int argc, char *argv[]){
    FILE *fout = NULL;
    FILE *fout_cuf = NULL, *fout_C_cu = NULL, *fout_info = NULL; /*ACC*/
    const char *fout_name = "out.DVMH.f";
    char *fout_name_cuf;     /*ACC*/
    char *fout_name_C_cu;    /*ACC*/
    char *fout_name_info_C;  /*ACC*/


    const char *proj_name = "dvm.proj";
    char *source_name;
    int level, hpf, openmp, isz, dvm_type_size;

    // initialisation
    initialize();

    openmp = hpf = 0; dvm_type_size = 0;

    argv++;
    while ((argc > 1) && (*argv)[0] == '-')
    {
        if ((*argv)[1] == 'o' && ((*argv)[2] == '\0')) {
            fout_name = argv[1];
            argv++;
            argc--;
        }
        else if ((*argv)[1] == 'a' && ((*argv)[2] == '\0')) {
            proj_name = argv[1];
            argv++;
            argc--;
        }
        else if (!strcmp(argv[0], "-dc"))
            check_regim = 1;
        else if (!strcmp(argv[0], "-dbif1"))
            dbg_if_regim = 1;
        else if (!strcmp(argv[0], "-dbif2"))
            dbg_if_regim = 2;
        else if (!strcmp(argv[0], "-speedL0")) /* for dedugging ACROSS-scheme */
            options.setOn(SPEED_TEST_L0);          /*ACC*/
        else if (!strcmp(argv[0], "-speedL1")) /* for dedugging ACROSS-scheme */
            options.setOn(SPEED_TEST_L1);          /*ACC*/
        else if (!strcmp(argv[0], "-dmpi"))
            deb_mpi = 1;
        else if (!strcmp(argv[0], "-dnoind"))
            d_no_index = 1;
        else if (!strcmp(argv[0], "-dperf")) {
            debug_regim = 1;
            omp_debug = DPERF;
        }
        else if (!strcmp(argv[0], "-dvmLoopAnalysisEC"))   /*ACC*/
        {
            options.setOn(LOOP_ANALYSIS);
            options.setOn(OPT_EXP_COMP);
        }
        else if (!strcmp(argv[0], "-dvmIrregAnalysis"))   /*ACC*/
        {
            options.setOn(LOOP_ANALYSIS);
            options.setOn(OPT_EXP_COMP);
            options.setOn(GPU_IRR_ACC);
        }
        else if (!strcmp(argv[0], "-dvmLoopAnalysis"))   /*ACC*/
            options.setOn(LOOP_ANALYSIS);
        else if (!strcmp(argv[0], "-dvmPrivateAnalysis"))   /*ACC*/
            options.setOn(PRIVATE_ANALYSIS);
        else if ((*argv)[1] == 'd') {
            switch ((*argv)[2]) {
            case '0':  level = 0; break;
            case '1':  level = 1; omp_debug = D1; /*OMP*/ break;
            case '2':  level = 2; omp_debug = D2; /*OMP*/ break;
            case '3':  level = 3; omp_debug = D3; /*OMP*/ break;
            case '4':  level = 4; omp_debug = D4; /*OMP*/ break;
            case '5':  level = 5; omp_debug = D5; /*OMP*/ break;
                /* case '5':  level = -1; many_files=1; break;*/
            default:  level = -1;
            }
            if (level > 0)
                debug_regim = 1;
            if ((*argv)[3] == '\0')
                AddToFragmentList(0, 0, level, -1);
            else if ((*argv)[3] == ':')
                FragmentList(*argv + 4, level, -1);
        }
        else if ((*argv)[1] == 'e') {
            switch ((*argv)[2]) {
            case '0':  level = 0; break;
            case '1':  level = 1; break;
            case '2':  level = 2; break;
            case '3':  level = 3; break;
            case '4':  level = 4; break;
			case 'm':  omp_perf = 1; break;
            default:  level = -1;
            }
            if ((*argv)[3] == '\0')
                AddToFragmentList(0, 0, -1, level);
            else if ((*argv)[3] == ':')
                FragmentList(*argv + 4, -1, level);
        }
        else if (!strcmp(argv[0], "-spf"))
        {  
            (void)fprintf(stderr, "Illegal option -spf \n");
            return 1;
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
        else if (!strcmp(argv[0], "-bind1"))
            bind = 1;
        else if (!strcmp(argv[0], "-t8"))
            dvm_type_size = 8;
        else if (!strcmp(argv[0], "-t4"))
            dvm_type_size = 4;
        else if (!strcmp(argv[0], "-r8"))
            default_real_size = 8;
        else if (!strcmp(argv[0], "-i8"))
            default_integer_size = 8;
        else if (!strcmp(argv[0], "-hpf") || !strcmp(argv[0], "-hpf1") || !strcmp(argv[0], "-hpf2"))
            hpf = 1;
        else if (!strcmp(argv[0], "-mp")) {
            OMP_program = 1; /*OMP*/
            openmp = 1;
        }
        //else if (!strcmp(argv[0],"-ta")) 
        //  ACC_program = 1;      
        else if (!strcmp(argv[0], "-noH"))
            ACC_program = 0;
        else if (!strcmp(argv[0], "-noCudaType"))  /*ACC*/
            undefined_Tcuda = 1;
        else if (!strcmp(argv[0], "-noCuda"))
            options.setOn(NO_CUDA);                /*ACC*/
        else if (!strcmp(argv[0], "-noPureFunc"))
            options.setOn(NO_PURE_FUNC);           /*ACC*/
        else if (!strcmp(argv[0], "-C_Cuda"))      /*ACC*/
            options.setOn(C_CUDA);
        else if (!strcmp(argv[0], "-FTN_Cuda") || !strcmp(argv[0], "-F_Cuda"))    /*ACC*/
            options.setOff(C_CUDA);
        else if (!strcmp(argv[0], "-no_blocks_info") || !strcmp(argv[0], "-noBI"))
            options.setOn(NO_BL_INFO);             /*ACC*/
        else if (!strcmp(argv[0], "-cacheIdx"))
            options.setOff(NO_BL_INFO);             /*ACC*/
        else if (!strcmp(argv[0], "-Ohost"))       /*ACC*/
            options.setOn(O_HOST);
        else if (!strcmp(argv[0], "-Opl2"))         /*ACC*/
        {  
            parloop_by_handler = 2;
            options.setOn(O_HOST);
           // options.setOn(NO_CUDA);
        }
        else if (!strcmp(argv[0], "-Opl"))         /*ACC*/
            parloop_by_handler = 1; 
       else if (!strcmp(argv[0], "-oneThread"))   /*ACC*/
            options.setOn(ONE_THREAD);
        else if (!strcmp(argv[0], "-noTfm"))       /*ACC*/
            options.setOff(AUTO_TFM);
        else if (!strcmp(argv[0], "-autoTfm"))     /*ACC*/
            options.setOn(AUTO_TFM);
        else if (!strcmp(argv[0], "-gpuO0"))       /*ACC*/
            options.setOn(GPU_O0);
        else if (!strcmp(argv[0], "-gpuO1"))       /*ACC*/
            options.setOn(GPU_O1);
        else if (!strcmp(argv[0], "-rtc"))         /*ACC*/
            options.setOn(RTC);  //for NVRTC compilation and execution
        else if (!strcmp(argv[0], "-ffo"))
            out_free_form = 1;
        else if (!strcmp(argv[0], "-upcase"))  
            out_upper_case = 1;
        else if (!strcmp(argv[0], "-noLimitLine"))  
            out_line_unlimit = 1; 
        else if (!strcmp(argv[0], "-noRemote"))  
            options.setOn(NO_REMOTE); 
        else if (!strcmp(argv[0], "-lgstd"))
        {
            (void)fprintf(stderr, "Illegal option -lgstd \n");
            return 1;
        }
        else if (!strcmp(argv[0], "-byFunUnparse"))
            unparse_functions = 1;
        else if (!strncmp(argv[0], "-bufio", 6)) {
            if ((*argv)[6] != '\0' && (isz = is_integer_value(*argv + 6)))
                IOBufSize = isz;
        }
        else if (!strncmp(argv[0], "-bufUnparser", 12)) {
            if ((*argv)[12] != '\0' && (isz = is_integer_value(*argv + 12)))
                UnparserBufSize = isz * 1024 * 1024;
        }
        else if (!strcmp(argv[0], "-ioRTS"))
            options.setOn(IO_RTS);
        else if (!strcmp(argv[0], "-read_all"))
            options.setOn(READ_ALL);
        else if (!strcmp(argv[0], "-Obase"))
            opt_base = 1;
        else if (!strcmp(argv[0], "-Oloop_range"))
            opt_loop_range = 1;
        else if ((*argv)[1] == 'H') {
            if ((*argv)[2] == 's' && (*argv)[3] == 'h' && (*argv)[4] == 'w') {
                if ((*argv)[5] != '\0' && (all_sh_width = is_integer_value(*argv + 5)))
                    ;
            }
            else if (!strcmp(*argv + 2, "nora"))
                no_rma = 1;
            else if (!strcmp(*argv + 2, "oneq"))
                one_inquiry = 1;
            else if (!strcmp(*argv + 2, "onlyl"))
                only_local = 1;
        }
        else if (!strncmp(argv[0], "-collapse", 9))
            if ((*argv)[9] != '\0' && (collapse_loop_count = is_integer_value(*argv + 9)));
        argc--;
        argv++;
    }
    // Check options combinations
    options.checkCombinations();

    if (isHPFprogram(source_name = *argv)) {
        HPF_program = 1;
        hpf = 0;
    }
    if (hpf) 
        return 0;  

    //if(ACC_program)
    {    
        fout_name_cuf = ChangeFtoCuf(fout_name);         /*ACC*/
        fout_name_C_cu = ChangeFto_C_Cu(fout_name);      /*ACC*/
        fout_name_info_C = ChangeFto_info_C(fout_name);  /*ACC*/
    }

    if (fout_name && source_name && !strcmp(source_name, fout_name))
    {
        (void)fprintf(stderr, "Output file has the same name as source file\n");
        return 1;
    }

    // definition of DvmType size: len_DvmType 
    // len_DvmType==0, if DvmType-size == default_integer_size == 4 
    if (bind == 1)
        len_DvmType = 8;  //sizeof(long) == 8
    if (dvm_type_size)
        len_DvmType = dvm_type_size;
    if (len_DvmType == 0 && default_integer_size == 8)
        len_DvmType = 4;

    if (ACC_program && debug_regim && !only_debug)
    {
        (void)fprintf(stderr, "Warning: -noH option is set to debug mode\n");
        ACC_program = 0;
    }
    if (parloop_by_handler>0 && debug_regim)
    {
        (void)fprintf(stderr, "Warning: -Opl/Opl2 option is ignored in debug mode\n");
        parloop_by_handler = 0;
    }

    if (openmp && ACC_program)
    {
        (void)fprintf(stderr, "Warning: -noH option is set to -mp mode\n");
        ACC_program = 0;
    }
    if (parloop_by_handler == 2 && !options.isOn(O_HOST))
       (void)fprintf(stderr, "Warning: -Ohost option is set to -Opl2 mode\n");
    
    if (v_print)
        (void)fprintf(stderr, "<<<<<  Translating  >>>>>\n");


    {
        SgProject project(proj_name);
        SgFile *file;
        addNumberOfFileToAttribute(&project);

        //----------------------------
        ProjectStructure(project);
        Private_Vars_Project_Analyzer();
        //----------------------------
        
        file = &(project.file(0));
        fin_name = new char[80];
        sprintf(fin_name, "%s%s", project.fileName(0), " ");

        //fin_name = strcat(project.fileName(0)," "); 
        // for call of function 'tpoint' 
        //added one symbol to input-file name
        initVariantNames();           //for project
        initIntrinsicFunctionNames(); //for project
        initSupportedVars(); // for project, acc_f2c.cpp
        initF2C_FunctionCalls(); // for project, acc_f2c.cpp

        //set the last symbol of file
        last_file_symbol = CUR_FILE_CUR_SYMB(); //LastSymbolOfFile(file)->thesymb;    //for low_level.c
        initLibNames();   //for every file
        InitDVM(file);    //for every file
        current_file = file;   // global variable (used in SgTypeComplex)
        max_lab = getLastLabelId();

        if (dbg_if_regim) 
            GetLabel(); //set maxlabval=90000
        /*
           printf("Labels:\n");
           printf("first:%d  max: %d \n",firstLabel(file)->thelabel->stateno, getLastLabelId());
           for(int num=1; num<=getLastLabelId(); num++)
           if(isLabel(num))
           printf("%d is label\n",num);
           else
           printf("%d isn't label\n",num);
           */

        if (openmp) { /*OMP*/
            if (debug_regim > 0) /*OMP*/
                InstrumentForOpenMPDebug(file); /*OMP*/            
            else /*OMP*/
                TranslateFileOpenMPDVM(file); /*OMP*/            
        }
        else 
            TranslateFileDVM(file);
        /* DEBUG */
        /* {FILE *fout; fout = fopen("out.out","w"); file->unparse(fout);} */
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

        // file->unparsestdout();

        if (errcnt) {
            (void)fprintf(stderr, "%d error(s)\n", errcnt);
            //!!! exit(1);
            return 1;
        }
        //file->saveDepFile("dvm.dep");
        //DVMFileUnparse(file);
        //file->saveDepFile("f.dep");
        
        if (!fout_name) { //outfile is not specified, output result to stdout
            file->unparsestdout();
            return 0;
        }

        //writing result of converting into file
        if ((fout = fopen(fout_name, "w")) == NULL) {
            (void)fprintf(stderr, "Can't open file %s for write\n", fout_name);
            return 1;
        }

        if (GeneratedForCuda())  /*ACC*/
        {
            if ((fout_C_cu = fopen(fout_name_C_cu, "w")) == NULL) {
                (void)fprintf(stderr, "Can't open file %s for write\n", fout_name_C_cu);
                return 1;
            }

            if (!options.isOn(C_CUDA))
            {
                if ((fout_cuf = fopen(fout_name_cuf, "w")) == NULL) {
                    (void)fprintf(stderr, "Can't open file %s for write\n", fout_name_cuf);
                    return 1;
                }
            }

            if ((fout_info = fopen(fout_name_info_C, "w")) == NULL) {
                (void)fprintf(stderr, "Can't open file %s for write\n", fout_name_info_C);
                return 1;
            }            
        }


        if (v_print)
            (void)fprintf(stderr, "<<<<<  Unparsing   %s  >>>>>\n", fout_name);
        if (mod_gpu) /*ACC*/
            UnparseTo_CufAndCu_Files(file, fout_cuf, fout_C_cu, fout_info);

        if (unparse_functions)
            UnparseFunctionsOfFile(file, fout);
        else if (UnparserBufSize)
            //UnparseProgram_ThroughAllocBuf(fout,file->filept,UnparserBufSize);
            file->unparseS(fout, UnparserBufSize);
        else
            file->unparse(fout);

        if ((fclose(fout)) < 0) {
            (void)fprintf(stderr, "Could not close %s\n", fout_name);
            return 1;
        }

        if (GeneratedForCuda())  /*ACC*/
        {
            if ((fclose(fout_C_cu)) < 0) {
                (void)fprintf(stderr, "Could not close %s\n", fout_name_C_cu);
                return 1;
            }

            if (!options.isOn(C_CUDA))
            {
                if ((fclose(fout_cuf)) < 0) {
                    (void)fprintf(stderr, "Could not close %s\n", fout_name_cuf);
                    return 1;
                }
            }

            if ((fclose(fout_info)) < 0) {
                (void)fprintf(stderr, "Could not close %s\n", fout_name_info_C);
                return 1;
            }
        }

        if (v_print)
            (void)fprintf(stderr, "\n*****  Done  *****\n");
        return 0;
    }
}

void initialize()
{
    int i;
    Dloop_No = 0;
    nfrag = 0; //counter of intervals for performance analizer 
    St_frag = 0;
    St_loop_first = 0;
    St_loop_last = 0;
    close_loop_interval = 0;
    len_int = 0;
    len_DvmType = 0;
    if (sizeof(long) == 8)   //default rule for bind, set by options -bind0,-bind1
        bind = 1;
    else
        bind = 0;
    perf_analysis = 0; //set to 1 by -e1
	omp_perf = 0; //set to 1 by -emp
    dvm_debug = 0;      //set to 1 by -d1  or -d2 or -d3 or -d4 flag
    only_debug = 0;     //set to 1 by -s flag
    level_debug = 0;    //set to 1 by -d1, to 2 by -d2, ...
    debug_fragment = NULL;
    perf_fragment = NULL;
    debug_regim = 0;
    dbg_if_regim = 0;
    check_regim = 0; //set by option -dc
    deb_mpi = 0;    //set by option -dmpi
    d_no_index = 0;   //set by option -dnoind
    IOBufSize = SIZE_IO_BUF;
    HPF_program = 0;
    many_files = 1; /*29.06.01*/
    iacross = 0;     //for HPF_program
    irg = 0;         //for HPF_program
    redgref = NULL;  //for HPF_program
    idebrg = 0;      //for HPF_program
    iconsg = 0;
    consgref = NULL;
    idebcg = 0;
    all_sh_width = no_rma = one_inquiry = only_local = 0;
    opt_base = 0;
    opt_loop_range = 0;
    in_interface = 0;
    out_free_form = 0;
    out_upper_case = 0;
    out_line_unlimit = 0;
    default_integer_size = 4;
    default_real_size = 4;
    unparse_functions = 0; //set to 1 by option -byFunUnparse
    for (i = 0; i < Ndev; i++)      /*ACC*/
        device_flag[i] = 0;     // set by option and by TARGETS clause  of REGION directive
    ACC_program = 1;            /*ACC*/
    region_debug = 0;           /*ACC*/
    region_compare = 0;         /*ACC*/
    undefined_Tcuda = 0;        /*ACC*/
    options.setOn(C_CUDA);      /*ACC*/
    options.setOn(NO_BL_INFO);  /*ACC*/
    parloop_by_handler = 0;     /*ACC*/
    collapse_loop_count = 0;    /*ACC*/
    cuda_functions = 0;         /*ACC*/
}

SgSymbol *LastSymbolOfFile(SgFile *f)
{ SgSymbol *s;
  s = f->firstSymbol();
  while(s->next())
    s = s->next();

  return s;
}

int isHPFprogram(char *filename)
{
   int    i;
  
   if (!filename)
     return (0);
   
   for (i = strlen(filename)-1 ; i >= 0 ; i --)
   {
        if ( filename[i] == '.' ) 
             break;
   }

   //if (i>=0 && !strcmp(&(filename[i+1]),"hpf"))
     if(i>=0 && (filename[i+1] == 'h' || filename[i+1] =='H') && (filename[i+2] == 'p' || filename[i+2] =='P') && (filename[i+3] == 'f' || filename[i+3] =='F'))
     return(1);  
 else
     return(0);
}

void initVariantNames(){
   for(int i = 0; i < MAXTAGS; i++) tag[i] = NULL;
/*!!!*/
#include "tag.h"
}

void initLibNames(){
   for(int i = 0; i < MAX_LIBFUN_NUM; i++) {
       fdvm[i] = NULL;
       name_dvm[i] =  NULL;
   }
#include "libdvm.h" 
}

void initMask(){
  for(int i = 0; i < MAX_LIBFUN_NUM; i++) {
       fmask[i] = 0;
  }
}

void InitDVM( SgFile *f) {
  SgStatement *fst;
  int i;
  fst = f->firstStatement();    //fst -> File header
  // Initialize COMMON names
  dvmcommon = new SgSymbol(VARIABLE_NAME,"mem000",*fst);//DEFAULT variant is right for COMMON
                                                  //but Sage don't want to create such symbol
  dvmcommon_ch = new SgSymbol(VARIABLE_NAME,"mch000",*fst);
  heapcommon = new SgSymbol(VARIABLE_NAME,"heap00",*fst);
  dbgcommon = new SgSymbol(VARIABLE_NAME,"dbg000",*fst);

// Initialize the functions symbols (for  LibDVM functions)
  for (i=0; name_dvm[i] && i<MAX_LIBFUN_NUM; i++) {
     fdvm[i] = new SgFunctionSymb(FUNCTION_NAME, name_dvm[i], *SgTypeInt(), *fst);
     // printf("name_dvm[%d] = %s\n", i , name_dvm[i]);
  }

  return;
}

void initF90Names() {
   for(int i = 0; i < MAX_FUN_F90; i++) 
      f90[i] = NULL; 
}

SgType * SgTypeComplex(SgFile *f)
{
  SgType *t;
  for(t=f->firstType(); t; t=t->next())
     if(t->variant()==T_COMPLEX)
       return(t);
 
  return(new SgType(T_COMPLEX));
}

SgType * SgTypeDoubleComplex(SgFile *f)
{
  SgType *t;
  for(t=f->firstType(); t; t=t->next())
     if(t->variant()==T_DCOMPLEX)
       return(t);
 
  return(new SgType(T_DCOMPLEX));
}

int MemoryUse()
{
  int i;
  for(i=0; i<Ntp; i++)
     if(mem_use[i] != 0 )
        return(1);
  return(0);
}
 
void TempVarDVM(SgStatement * func ) {

  int i;   
  SgValueExp N(100),M1(1),M0(0), MB(64);
  SgExpression  *MS;
  //SgSubscriptExp M00(M0,M0); 
  // SgExpression *M00(DDOT,&M0,&M0,NULL);
  SgExpression  *M00 =  new SgExpression(DDOT,&M0.copy(),&MB.copy(),NULL); 
  SgExpression *le = NULL ;
  SgArrayType *typearray;

  typearray = new SgArrayType(*SgTypeInt());  
  dvmbuf = new SgVariableSymb("dvm000", *typearray, *func);
  typearray = new SgArrayType(*SgTypeInt());
  hpfbuf = new SgVariableSymb("hpf000", *typearray, *func); 

  Iconst[0] = new SgConstantSymb("dvm0c0", *func, *new SgValueExp(0));
  Iconst[1] = new SgConstantSymb("dvm0c1", *func, *new SgValueExp(1));
  Iconst[2] = new SgConstantSymb("dvm0c2", *func, *new SgValueExp(2));
  Iconst[3] = new SgConstantSymb("dvm0c3", *func, *new SgValueExp(3));
  Iconst[4] = new SgConstantSymb("dvm0c4", *func, *new SgValueExp(4));
  Iconst[5] = new SgConstantSymb("dvm0c5", *func, *new SgValueExp(5));
  Iconst[6] = new SgConstantSymb("dvm0c6", *func, *new SgValueExp(6));
  Iconst[7] = new SgConstantSymb("dvm0c7", *func, *new SgValueExp(7));
  Iconst[8] = new SgConstantSymb("dvm0c8", *func, *new SgValueExp(8));
  Iconst[9] = new SgConstantSymb("dvm0c9", *func, *new SgValueExp(9));

  if(debug_regim)
    dbg_var =  new SgVariableSymb("dbgvar00", *SgTypeInt(), *func);

  if(only_debug)
     return;
 
  typearray = new SgArrayType(*SgTypeFloat());
  typearray-> addRange(*M00);
  Rmem = mem_symb[Real] =   new SgVariableSymb("r0000m", *typearray, *func);
  //Rmem-> declareTheSymbol(*func);
  typearray = new SgArrayType(*SgTypeDouble());
  typearray-> addRange(*M00);
  Dmem = mem_symb[Double] =  new SgVariableSymb("d0000m", *typearray, *func);
  //Dmem-> declareTheSymbol(*func);
  typearray = new SgArrayType(*SgTypeInt());
  typearray-> addRange(*M00);
  Imem = mem_symb[Integer] = new SgVariableSymb("i0000m", *typearray, *func);
  //Imem-> declareTheSymbol(*func);
  typearray = new SgArrayType(*SgTypeBool());
  typearray-> addRange(*M00);
  Lmem = mem_symb[Logical] = new SgVariableSymb("l0000m", *typearray, *func);
  //Lmem-> declareTheSymbol(*func);
//!!!!!!!
  typearray = new SgArrayType(* SgTypeComplex(current_file));
  typearray-> addRange(*M00);
  Cmem = mem_symb[Complex] = new SgVariableSymb("c0000m", *typearray, *func);
  typearray = new SgArrayType(* SgTypeDoubleComplex(current_file));
  typearray-> addRange(*M00);
  DCmem = mem_symb[DComplex] =  new SgVariableSymb("dc000m", *typearray, *func);
  typearray = new SgArrayType(*SgTypeChar());
  typearray-> addRange(*M00);
  Chmem = mem_symb[Character] = new SgVariableSymb("ch000m", *typearray, *func);
//---------
  le= new SgExpression(LEN_OP);
  le->setLhs(new SgValueExp(1));
  SgType *tint1 = new SgType(T_INT, le, NULL);
  le= new SgExpression(LEN_OP);
  le->setLhs(new SgValueExp(2));
  SgType *tint2 = new SgType(T_INT, le, NULL);
  le= new SgExpression(LEN_OP);
  le->setLhs(new SgValueExp(8));
  SgType *tint8 = new SgType(T_INT, le, NULL);
//----------
  typearray = new SgArrayType(*tint1);
  typearray-> addRange(*M00);
  mem_symb[Integer_1] = new SgVariableSymb("i000m1", *typearray, *func); 
  typearray = new SgArrayType(*tint2);
  typearray-> addRange(*M00);
  mem_symb[Integer_2] = new SgVariableSymb("i000m2", *typearray, *func); 
  typearray = new SgArrayType(*tint8);
  typearray-> addRange(*M00);
  mem_symb[Integer_8] = new SgVariableSymb("i000m8", *typearray, *func); 
//---------
  le= new SgExpression(LEN_OP);
  le->setLhs(new SgValueExp(1));
  SgType *tlog1 = new SgType(T_BOOL, le, NULL);
  le= new SgExpression(LEN_OP);
  le->setLhs(new SgValueExp(2));
  SgType *tlog2 = new SgType(T_BOOL, le, NULL);
  le= new SgExpression(LEN_OP);
  le->setLhs(new SgValueExp(8));
  SgType *tlog8 = new SgType(T_BOOL, le, NULL);
//----------
  typearray = new SgArrayType(*tlog1);
  typearray-> addRange(*M00);
  mem_symb[Logical_1] = new SgVariableSymb("l000m1", *typearray, *func); 
  typearray = new SgArrayType(*tlog2);
  typearray-> addRange(*M00);
  mem_symb[Logical_2] = new SgVariableSymb("l000m2", *typearray, *func); 
  typearray = new SgArrayType(*tlog8);
  typearray-> addRange(*M00);
  mem_symb[Logical_8] = new SgVariableSymb("l000m8", *typearray, *func); 
  
  for(i=0; i<8; i++)
    loop_var[i] = new SgVariableSymb(name_loop_var[i], *SgTypeInt(), *func);

  MS = new SgValueExp(IOBufSize);
  typearray = new SgArrayType(*SgTypeInt());
  typearray-> addRange(*MS);
  bufIO[Integer] =   new SgVariableSymb(name_bufIO[Integer], *typearray, *func);
  typearray = new SgArrayType(*SgTypeFloat());
  typearray-> addRange(*MS);
  bufIO[Real] =      new SgVariableSymb(name_bufIO[Real], *typearray, *func);
  typearray = new SgArrayType(*SgTypeDouble());
  typearray-> addRange(*MS);
  bufIO[Double] =    new SgVariableSymb(name_bufIO[Double], *typearray, *func);
  typearray = new SgArrayType(* SgTypeComplex(current_file));
  typearray-> addRange(*MS);
  bufIO[Complex] =   new SgVariableSymb(name_bufIO[Complex], *typearray, *func);
  typearray = new SgArrayType(*SgTypeBool());
  typearray-> addRange(*MS);
  bufIO[Logical] =   new SgVariableSymb(name_bufIO[Logical], *typearray, *func);
  typearray = new SgArrayType(* SgTypeDoubleComplex(current_file));
  typearray-> addRange(*MS);
  bufIO[DComplex] =  new SgVariableSymb(name_bufIO[DComplex], *typearray, *func);
  typearray = new SgArrayType(* new SgType(T_STRING));
  typearray-> addRange(*MS);
  bufIO[Character] = new SgVariableSymb(name_bufIO[Character], *typearray, *func);
  typearray = new SgArrayType(*tint1);
  typearray-> addRange(*MS);
  bufIO[Integer_1] = new SgVariableSymb(name_bufIO[Integer_1], *typearray, *func); 
  typearray = new SgArrayType(*tint2);
  typearray-> addRange(*MS);
  bufIO[Integer_2] = new SgVariableSymb(name_bufIO[Integer_2], *typearray, *func); 
  typearray = new SgArrayType(*tint8);
  typearray-> addRange(*MS);
  bufIO[Integer_8] = new SgVariableSymb(name_bufIO[Integer_8], *typearray, *func); 
  typearray = new SgArrayType(*tlog1);
  typearray-> addRange(*MS);
  bufIO[Logical_1] = new SgVariableSymb(name_bufIO[Logical_1], *typearray, *func); 
  typearray = new SgArrayType(*tlog2);
  typearray-> addRange(*MS);
  bufIO[Logical_2] = new SgVariableSymb(name_bufIO[Logical_2], *typearray, *func); 
  typearray = new SgArrayType(*tlog8);
  typearray-> addRange(*MS);
  bufIO[Logical_8] = new SgVariableSymb(name_bufIO[Logical_8], *typearray, *func); 

  typearray = new SgArrayType(*SgTypeInt());
  rmbuf[Integer] =   new SgVariableSymb(name_rmbuf[Integer], *typearray, *func);
  typearray = new SgArrayType(*SgTypeFloat());
  rmbuf[Real] =      new SgVariableSymb(name_rmbuf[Real], *typearray, *func);
  typearray = new SgArrayType(*SgTypeDouble());
  rmbuf[Double] =    new SgVariableSymb(name_rmbuf[Double], *typearray, *func);
  typearray = new SgArrayType(* SgTypeComplex(current_file));
  rmbuf[Complex] =   new SgVariableSymb(name_rmbuf[Complex], *typearray, *func);
  typearray = new SgArrayType(*SgTypeBool());
  rmbuf[Logical] =   new SgVariableSymb(name_rmbuf[Logical], *typearray, *func);
  typearray = new SgArrayType(* SgTypeDoubleComplex(current_file));
  rmbuf[DComplex] =  new SgVariableSymb(name_rmbuf[DComplex], *typearray, *func);
  typearray = new SgArrayType(* new SgType(T_STRING));
  rmbuf[Character] = new SgVariableSymb(name_rmbuf[Character], *typearray, *func);
  typearray = new SgArrayType(*tint1);
  rmbuf[Integer_1] = new SgVariableSymb(name_rmbuf[Integer_1], *typearray, *func); 
  typearray = new SgArrayType(*tint2);
  rmbuf[Integer_2] = new SgVariableSymb(name_rmbuf[Integer_2], *typearray, *func); 
  typearray = new SgArrayType(*tint8);
  rmbuf[Integer_8] = new SgVariableSymb(name_rmbuf[Integer_8], *typearray, *func); 
  typearray = new SgArrayType(*tlog1);
  rmbuf[Logical_1] = new SgVariableSymb(name_rmbuf[Logical_1], *typearray, *func); 
  typearray = new SgArrayType(*tlog2);
  rmbuf[Logical_2] = new SgVariableSymb(name_rmbuf[Logical_2], *typearray, *func); 
  typearray = new SgArrayType(*tlog8);
  rmbuf[Logical_8] = new SgVariableSymb(name_rmbuf[Logical_8], *typearray, *func); 

  typearray = new SgArrayType(*SgTypeInt());
  heapdvm = new SgVariableSymb("heap00", *typearray, *func);

  Pipe = new SgVariableSymb("pipe00", *SgTypeDouble(), *func);
  
     return;
}

char* FileNameVar(int i)
{ char *name;
 name = new char[80];
 sprintf(name,"%s%d","filenm00",i); 
 return(name);
}

char* RedGroupVarName(SgSymbol *gr)
{ char *name;
 name = new char[80];
 sprintf(name,"%s%s",gr->identifier(),"00"); 
 return(name);
}

char* ModuleProcName(SgSymbol *smod)
{ char *name;
 name = new char[80];
 sprintf(name,"dvm_%s",smod->identifier()); 
 return(name);
}

SgSymbol* BaseSymbol(SgSymbol *ar)
{ char *name;
 SgSymbol *sbs, *base;
 SgArrayType *typearray;
 SgValueExp M0(0), MB(64);
 SgExpression  *M00 =  new SgExpression(DDOT,&M0.copy(),&MB.copy(),NULL); 
 name = new char[80];
 base = baseMemory(ar->type()->baseType());
 //strncpy(name,base->identifier(),5);
 //strcat (name,ar->identifier());
 sprintf(name,"%.4s_%s",base->identifier(),ar->identifier());
 typearray = new SgArrayType(*ar->type()->baseType());
 typearray-> addRange(*M00);
 sbs = new SgVariableSymb(name, *typearray, *cur_func); 
 return(sbs);
}

SgSymbol* IndexSymbol(SgSymbol *si)
{ char *name;
 SgSymbol *sn;
 name = new char[80];
 sprintf(name,"%s__d",si->identifier()); 
 sn = new SgVariableSymb(name, *si->type(), *cur_func); 
 return(sn);
}

SgSymbol* InitLoopSymbol(SgSymbol *si,SgType *t)
{ char *name;
 SgSymbol *sn;
 name = new char[80];
 sprintf(name,"%s__init",si->identifier()); 
 sn = new SgVariableSymb(name, *t, *cur_func); 
 return(sn);
}

SgSymbol* DerivedTypeBaseSymbol(SgSymbol *stype,SgType *t)
{
 char *name;
 SgSymbol *sn;
 SgArrayType *typearray;
 SgValueExp M0(0), MB(64);
 SgExpression  *M00 =  new SgExpression(DDOT,&M0.copy(),&MB.copy(),NULL); 
 name = new char[80];
 sprintf(name,"%s0000m",stype->identifier()); 
 typearray = new SgArrayType(*t);
 typearray-> addRange(*M00);
 sn = new SgVariableSymb(name, *typearray, *cur_func);
 return(sn);
}

SgSymbol* CommonSymbol(SgSymbol *stype)
{ char *name;
 name = new char[80];
 sprintf(name,"mem000%s",stype->identifier()); 
 return(new SgSymbol(VARIABLE_NAME,name,*cur_func->controlParent()));
}

SgSymbol *CheckSummaSymbol()
{ 
 return(new SgVariableSymb("check_sum00",*SgTypeDouble(),*cur_func));
}

SgSymbol *DebugGoToSymbol(SgType *t)
{char *name;
 SgSymbol *sn;
 name = new char[80];
 sprintf(name,"dbv_goto00%d",++nifvar);
 sn = new SgVariableSymb(name,*t,*cur_func);
 if_goto = AddToSymbList(if_goto, sn); 
 return(sn);
}


SgSymbol *TaskAMVSymbol(SgSymbol *s)
{ char *name;
 name = (char *) malloc((unsigned)(strlen(s->identifier())+5)); 
 sprintf(name,"%s_amv",s->identifier()); 
 return(new SgSymbol(VARIABLE_NAME,name,*cur_func));
}

SgSymbol *TaskIndSymbol(SgSymbol *s)
{ char *name;
 name = (char *) malloc((unsigned)(strlen(s->identifier())+3)); 
 sprintf(name,"i_%s",s->identifier()); 
 return(new SgVariableSymb(name,*SgTypeInt(),*cur_func));
}

SgSymbol *TaskRenumArraySymbol(SgSymbol *s)
{ char *name;
 name = (char *) malloc((unsigned)(strlen(s->identifier())+7)); 
 sprintf(name,"renum_%s",s->identifier()); 
 return(new SgVariableSymb(name,*(s->type()),*cur_func));
}

SgSymbol *TaskLPsArraySymbol(SgSymbol *s)
{ char *name;
 name = (char *) malloc((unsigned)(strlen(s->identifier())+5)); 
 sprintf(name,"lps_%s",s->identifier()); 
 return(new SgVariableSymb(name,*(s->type()),*cur_func));
}

SgSymbol *TaskHPsArraySymbol(SgSymbol *s)
{ char *name;
 name = (char *) malloc((unsigned)(strlen(s->identifier())+5)); 
 sprintf(name,"hps_%s",s->identifier()); 
 return(new SgVariableSymb(name,*(s->type()),*cur_func));
}

SgSymbol * CreateRegistrationArraySymbol()
{
 SgSymbol *sn;
 SgArrayType *typearray;
 char *ident = cur_func->symbol()->identifier(); //Module identifier
 char *name = new char[10+strlen(ident)];
 sprintf(name,"deb_%s_dvm",ident); 
 typearray = new SgArrayType(*SgTypeInt());
 sn = new SgVariableSymb(name, *typearray, *cur_func);
 return(sn);
}

void CreateCoeffs(coeffs* scoef,SgSymbol *ar)
{int i,r,i0;
 char *name;
 r=Rank(ar);
 i0 = opt_base ? 1 : 2;
 if(opt_loop_range) i0=0;
 for(i=i0;i<=r+2;i++){
   name = new char[strlen(ar->identifier()) + 6];
   sprintf(name,"%s%s%d", ar->identifier(),"000",i); 
   scoef->sc[i] =  new SgVariableSymb(name, *SgTypeInt(), *cur_func); 
   //printf("%s",(scoef->sc[i])->identifier());
 }
  scoef->use = 0;
 if(IN_MODULE && !IS_TEMPLATE(ar))
     scoef->use = 1;
}

SgSymbol *CreateConsistentHeaderSymb(SgSymbol *ar)
{
 char *name;
 name = new char[80];
 SgArrayType *typearray;
 //SgValueExp M1(1);
 name = new char[80];
 sprintf(name,"%s%s",ar->identifier(),"000");
 typearray = new SgArrayType(*SgTypeInt());
 //typearray-> addRange(M1);
 return( new SgVariableSymb(name, *typearray, *cur_func));   
}

SgSymbol *IOstatSymbol()
{
  if(!IOstat)
    IOstat = new SgSymbol(VARIABLE_NAME, "iostat_dvm", *SgTypeInt(), *cur_func); 
  return (IOstat);
}

SgStatement  *doPublicStmtForDvmModuleProcedure(SgSymbol *smod)
{
  mod_attr *attrm;
  SgStatement *st = NULL;
  
  if((attrm=DVM_PROC_IN_MODULE(smod)) && attrm->symb){
    st = new SgStatement(PUBLIC_STMT);
    st->setExpression(0, *new SgExprListExp(*new SgVarRefExp(*attrm->symb)));
  }
  return (st);
}

void DeclareVarDVM(SgStatement *lstat, SgStatement *lstat2)
{
//lstat is not equal lstat2 only for MODULE:
//lstat2 is header of generated module procedure dvm_<module_name>
//some generated specification statements are inserted in specification part
//of module and other are inserted in module procedure

 SgArrayType *typearray;
 SgStatement *equiv, *st,*st1,*com, *st_next;
 SgExpression *em[Ntp], *eeq, *ed;
 SgValueExp c1(1),c0(0);
 SgExprListExp *el, *eel;
 int i=0;
 int j;
 SgType *tlen = NULL;
 if(len_DvmType) {
      SgExpression *le;
      le = new SgExpression(LEN_OP);
      le->setLhs(new SgValueExp(len_DvmType));
      tlen = new SgType(T_INT, le, SgTypeInt());

 } 

 st_next = lstat->lexNext();

 if(in_interface) goto HEADERS_; //only array header declaration is created in interface body of interface block

 // create DATA statement for SAVE groups: DATA gref(1)/0/ gred/0/...
 if(grname && !IN_MODULE) { //group name list is not empty
   group_name_list *sl;
   char *data_str= new char[4000];
   int i =0; 
   sprintf(data_str,"data "); 
   for(sl=grname; sl; sl=sl->next)                                           
     if(IS_SAVE(sl->symb)) {
       i++;
       if (sl->symb->variant() == REF_GROUP_NAME){
         strcat(data_str,sl->symb->identifier());
         strcat(data_str,"(1)/0/ "); 
       } else {
         strcat(data_str,sl->symb->identifier());
         strcat(data_str,"/0/ ");  
       }
     }         
   if(i) {
   st = new SgStatement(DATA_DECL);// creates DATA statement
   SgExpression  *es = new SgExpression(STMT_STR);
   NODE_STR(es->thellnd) = data_str;    //e->thellnd->entry.string_val = data_str;
   st -> setExpression(0,*es); 
   lstat -> insertStmtAfter(*st);  
   }
 } 


 // inserting in main program SAVE statement (without list): for OpenMP translation 
 if(IN_MAIN_PROGRAM && !saveall)
    lstat -> insertStmtAfter(*new SgStatement(SAVE_DECL)); 

 if (!only_debug) {
  // declare  array bases for DVM-arrays
  if(opt_base && !HPF_program  && dsym) {
   symb_list *sl;
   coeffs *c;
   for(sl=dsym; sl; sl=sl->next) {
     if(IS_TEMPLATE(sl->symb))
         continue;
     c = ((coeffs *) sl->symb-> attributeValue(0,ARRAY_COEF));
     if(!c->use) 
       continue;
     st =   (*ARRAY_BASE_SYMBOL(sl->symb))->makeVarDeclStmt(); 
     lstat -> insertStmtAfter(*st);
   }
  }
 
 // create DATA statement for SAVE array headers: DATA a(1)/0/ b(1)/0/...
  if(dsym && !IN_MODULE) { //distributed objects list is not empty
   symb_list *sl;
   char *data_str= new char[4000];
   int i =0;                                                                       
   sprintf(data_str,"data ");
   for(sl=dsym; sl; sl=sl->next) {
     if(IS_SAVE(sl->symb)) {
        i++;
        /*   if (i==5) {
          strcat(data_str, "\n     +     ");
           i=1;
        }
        */
       strcat(data_str,sl->symb->identifier());
       strcat(data_str,"(1)/0/ ");
	// sprintf(data_str, "%s%s(1)/0/",data_str,sl->symb->identifier());  
     }         
   }
   // strcat(data_str,"\n");
   if(i) {
   st = new SgStatement(DATA_DECL);// creates DATA statement
   SgExpression  *es = new SgExpression(STMT_STR);
     // e = new SgValueExp(data_str);
     // NODE_STR(es->thellnd) = NODE_STR(e->thellnd);
   NODE_STR(es->thellnd) = data_str;    //e->thellnd->entry.string_val = data_str;
   st -> setExpression(0,*es); 
   lstat -> insertStmtAfter(*st);  
   }
   }
 
 // declaring DVM do-variables
  for(j=0; j<nio;  j++) {
          // loop_var[j] -> declareTheSymbol(*func);
     st = loop_var[j] ->makeVarDeclStmt();

     lstat2 -> insertStmtAfter(*st);
  }
 
 // declaring DVM memory variables
  st1 = lstat2->lexNext();
  
  if(MemoryUse())  
                          //if (mem_use[Integer] || mem_use[Real] || mem_use[Double] || mem_use[Complex] || mem_use[Logical] ||  mem_use[DComplex] || mem_use[Character])
     mem_use[Integer] =  mem_use[Double] = 1; //DVM-COMMON-blocks must have the same length 
  else
      if(IN_MAIN_PROGRAM)                                 
         mem_use[Integer] =  mem_use[Double] = 1;   //in MAIN-program DVM-COMMON must be always
 
  for(j=0,i=0; j<Ntp; j++)
     if(mem_use[j] != 0)
     {
        st = mem_symb[j]->makeVarDeclStmt();
        lstat2 -> insertStmtAfter(*st);
        em[j] = new SgArrayRefExp(*mem_symb[j]);
        i++;
     }   
 
  if(i>1) {
    // generating EQUIVALENCE statement
    // EQUIVALENCE (Imem(0), Rmem(0),...,Lmem(0)) 
 
    j=0;
    while (!mem_use[j])
       j++;
    el = new SgExprListExp(*em[j]);
    for(j=j+1; j<Ntp; j++){
       if(mem_use[j]) {
            //el->append(*em[j]);
         eel = new SgExprListExp(*em[j]);
         eel->setRhs(*el);
         el = eel;
       }
    }
    eeq = new SgExpression (EQUI_LIST);
    eeq -> setLhs(*el);
    equiv = new SgStatement(EQUI_STAT);
    equiv->setExpression(0,*eeq);
    st1->insertStmtBefore(*equiv);
  }

 // declaring DVM memory variable of type CHARACTER in MAIN-program
 // in MAIN-program DVM-COMMON must be always declared character array ch000m(0:1)
   if(IN_MAIN_PROGRAM && !mem_use[Character]) { 
     st = Chmem ->makeVarDeclStmt();
     lstat -> insertStmtAfter(*st);
  } 


 // declaring COMMON block for DVM memory variables
  if(i) {
     el = new SgExprListExp(* new SgArrayRefExp(*Imem));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*dvmcommon);
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     st1->insertStmtBefore(*com);
  }      
/*  if(mem_use[Character]) {
     el = new SgExprListExp(* new SgArrayRefExp(*Chmem));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*dvmcommon_ch);
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     st1->insertStmtBefore(*com);     
  }
*/
 // declaring DVM memory variable of derived type
  if(mem_use_structure){
    base_list *el;
    SgExpression *e;
    for(el=mem_use_structure;el;el=el->next) {     
      st = el->base_symbol ->makeVarDeclStmt();
      lstat2 -> insertStmtAfter(*st);
            
 // declaring COMMON block for DVM memory variables of derived type
  
     e = new SgExprListExp(* new SgArrayRefExp(*el->base_symbol));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*CommonSymbol(el->type_symbol));
     eeq -> setLhs(*e);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     st1->insertStmtBefore(*com);
    }
  }      


 // declaring buffer variables for remote access
  for(i=0; i<Ntp; i++) 
    if(rmbuf_size[i]) {
      typearray = isSgArrayType(rmbuf[i]->type());
      typearray-> addRange(* new SgValueExp(rmbuf_size[i]));
      //rmbuf[i]-> declareTheSymbol(*func);
      st = rmbuf[i] ->makeVarDeclStmt();
      lstat -> insertStmtAfter(*st);
    }

 // declaring DVM buffer variables for Input/Output
  st1 = lstat->lexNext();
  i=0;
  for (j=0; j<Ntp; j++)
     if(buf_use[j]){
       //bufIO[j]-> declareTheSymbol(*func);
       st = bufIO[j] ->makeVarDeclStmt();
       lstat -> insertStmtAfter(*st);
       em[j] = new SgArrayRefExp(*bufIO[j]);
       i++;
     }
  
  if(i && !buf_use[0]) { //declare integer I/O buffer always
     buf_use[0] = 1;
     st = bufIO[0] ->makeVarDeclStmt();
     lstat -> insertStmtAfter(*st);
     em[0] = new SgArrayRefExp(*bufIO[0]);
     i++;  
  } 
   
  if(i>1) {
    // generating EQUIVALENCE statement
    // EQUIVALENCE (i000io(1), r000io(1),...,l000io(1))
    //              bufIO[0]   bufIO[1]      bufIO[4]
    j=0;
    while (!buf_use[j])
       j++;
    el = new SgExprListExp(*em[j]);
    for(j=j+1; j<Ntp; j++){
       if(buf_use[j]) {
         eel = new SgExprListExp(*em[j]);
         eel->setRhs(*el);
         el = eel;
         // el->append(*em[j]);
       }
    }
    eeq = new SgExpression (EQUI_LIST);
    eeq -> setLhs(*el);
    equiv = new SgStatement(EQUI_STAT);
    equiv->setExpression(0,*eeq);
    st1->insertStmtBefore(*equiv);
  }

// declaring buffer HEAP for headers of dynamic arrays
  if(heap_ar_decl && heap_size){
    typearray = isSgArrayType(heapdvm->type());
    typearray-> addRange(* new SgValueExp(heap_size));
    st = heapdvm ->makeVarDeclStmt();
      //st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
      //heap_ar_decl->setLhs(new SgExprListExp(new SgValueExp(heap_size)));
      //(heap_ar_decl->lhs())->setRhs(NULL); 
      //st -> setExpression(0,*new SgExprListExp(*heap_ar_decl)); 
    if(len_DvmType)
     st->expr(1)->setType(tlen); 
    lstat -> insertStmtAfter(*st);
// declaring COMMON block for headers of dynamic arrays
     el = new SgExprListExp(* new SgArrayRefExp(*heapdvm));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*heapcommon);
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     lstat->insertStmtAfter(*com);            
  }
// declaring dvm-procedure for module as public
   if(IN_MODULE && privateall && (st=doPublicStmtForDvmModuleProcedure(cur_func->symbol())))
     lstat->insertStmtAfter(*st);

} //endif !only_debug

// declaring variable for new IOSTAT specifier of Input/Output statement (if END=,ERR=,EOR= are replaced with IOSTAT=)
  if(IOstat) 
  {
     st = IOstat ->makeVarDeclStmt();
     lstat -> insertStmtAfter(*st);    
  }

// declare  mask for registration (only in module)
   if(debug_regim && count_reg ) {
     typearray = isSgArrayType(registration_array->type());
     typearray-> addRange(* new SgValueExp(count_reg));  
     st = registration_array ->makeVarDeclStmt();
     eeq = DVMVarInitialization(st->expr(0)->lhs());
     st->expr(0)->setLhs(eeq);
     if(len_DvmType)
     st->expr(1)->setType(tlen); 
     st->setVariant(VAR_DECL_90);
     lstat -> insertStmtAfter(*st);       
   }

// generate PARAMETER statement

 if(dvm_const_ref == 1) { 
   st= new SgStatement(PARAM_DECL);   
   el = NULL;
   for(j=0; j<10; j++) {
      eel =   new SgExprListExp(* new SgRefExp(CONST_REF, *Iconst[j]));
      eel->setRhs(el);
      el = eel;     
   }   
   st->setExpression(0,*el);
   lstat2 -> insertStmtAfter(*st);

// declare constants as INTEGER        
   st = fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
  
  for(j=0; j<10; j++) {
      eel =   new SgExprListExp(* new SgVarRefExp(Iconst[j]));
      eel->setRhs(el);
      el = eel;     
   }
  st -> setExpression(0,*el); 
  if(len_DvmType)
     st->expr(1)->setType(tlen); 
  lstat2 -> insertStmtAfter(*st);
 }

// declare  group names as INTEGER
   if(grname) {
   group_name_list *sl;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=grname; sl; sl=sl->next) {
      if (sl->symb->variant() == REF_GROUP_NAME)
        eeq =  new SgArrayRefExp(*(sl->symb),*new SgValueExp(3));
      else 
        eeq =  new SgVarRefExp(*(sl->symb));
      if(IN_MODULE)
        eeq = DVMVarInitialization(eeq);
      eel =   new SgExprListExp(* eeq);
      eel->setRhs(el);
      el = eel;
   }
   st -> setExpression(0,*el);
   if(len_DvmType)
     st->expr(1)->setType(tlen);  
   if(IN_MODULE)
     st->setVariant(VAR_DECL_90);
   lstat -> insertStmtAfter(*st);

 
// declare common blocks for remote references groups
   for(sl=grname; sl; sl=sl->next) 
     if (sl->symb->variant() == REF_GROUP_NAME) {
     el = new SgExprListExp(* new SgArrayRefExp(*(sl->symb)));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*(sl->symb));
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     st->insertStmtAfter(*com);
   }

// declare variables  for reduction groups and consistent groups
  st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=grname; sl; sl=sl->next) {
     if (sl->symb->variant() == REDUCTION_GROUP_NAME || sl->symb->variant() == CONSISTENT_GROUP_NAME) {
        SgSymbol *rgv;
        int nl;
        nl = sl->symb->variant() == REDUCTION_GROUP_NAME ? nloopred : nloopcons;
        rgv = * ((SgSymbol **) (sl->symb)-> attributeValue(0,RED_GROUP_VAR)); 
        ed = new SgExpression(DDOT,new SgValueExp(0),new SgValueExp(nl),NULL);
        eeq =  new SgArrayRefExp(*rgv,*ed);
        if(IN_MODULE)
          eeq = DVMVarInitialization(eeq);
        //eeq =  new SgArrayRefExp(*rgv,*new SgValueExp(nloopred));
        eel =   new SgExprListExp(* eeq);
        eel->setRhs(el);
        el = eel;
     }
   }
   if(el) {
    st -> setExpression(0,*el);
    if(len_DvmType)
      st->expr(1)->setType(tlen); 
    if(IN_MODULE)
      st->setVariant(VAR_DECL_90);
    lstat -> insertStmtAfter(*st);
   } 
}
// declare common block for reduction variables
   if(redvar_list && !only_debug) {
     symb_list *sl;
     char * ncom = new char[100];
     char * f_name;
     el = NULL;
     for(sl=redvar_list; sl; sl=sl->next) 
     if (CURRENT_SCOPE(sl->symb) && !IS_ARRAY(sl->symb) && !IN_COMMON(sl->symb) && !IN_DATA(sl->symb) && !IS_DUMMY(sl->symb) && !IS_SAVE(sl->symb) && !IN_EQUIVALENCE(sl->symb) && strcmp(sl->symb->identifier(),cur_func->symbol()->identifier())) {
     eel = new SgExprListExp(* new SgVarRefExp(*(sl->symb)));
     eel->setRhs(el);
     el = eel;
     }
     if (el){
     f_name = cur_func->symbol()->identifier();
     if(f_name[0]=='_') //main program unit without name: sage-name == _MAIN
       f_name=f_name+1;
     sprintf(ncom,"%s%s", f_name,"dvm");
     st = cur_func->symbol()->scope();
     redcommon = new SgSymbol(VARIABLE_NAME,ncom,*st);
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*redcommon);
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     lstat->insertStmtAfter(*com);  
     }     
   }

// declare processor array names as INTEGER
   if(proc_symb) {
   symb_list *sl;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=proc_symb; sl; sl=sl->next) {
      eel =   new SgExprListExp(* new SgVarRefExp(*(sl->symb)));
      eel->setRhs(el);
      el = eel;
   }
   st -> setExpression(0,*el);
   if(len_DvmType)
     st->expr(1)->setType(tlen);  
   lstat -> insertStmtAfter(*st);
   }

// declare index variables (optimization code)
   if(index_symb) {
   symb_list *sl;
   for(sl=index_symb; sl; sl=sl->next) {
     st =   sl->symb->makeVarDeclStmt(); 
     lstat -> insertStmtAfter(*st);
   }
   }

// declare  task arrays as INTEGER
   if(task_symb){
   symb_list *sl;
   SgArrayType *artype;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=task_symb; sl; sl=sl->next) {
      artype = isSgArrayType(sl->symb->type());
      eel = new SgExprListExp(* new SgArrayRefExp(*(sl->symb),*new SgValueExp(2),*artype->sizeInDim(0)));
      eel->setRhs(el);
      el = eel;
      eel = new SgExprListExp(*new SgVarRefExp(TASK_SYMBOL(sl->symb))); // symbol for TASK AMview
      eel->setRhs(el);
      el = eel;
   }
   st -> setExpression(0,*el);
   if(len_DvmType)
     st->expr(1)->setType(tlen);  
   lstat -> insertStmtAfter(*st);
             //SgSymbol *s= TASK_IND_VAR(task_symb->symb);
   st = fdvm[0]->makeVarDeclStmt();
   el = NULL;
   for(sl=task_symb; sl; sl=sl->next) {
      artype = isSgArrayType(sl->symb->type());
      eel = new SgExprListExp(* new SgArrayRefExp(*TASK_RENUM_ARRAY(sl->symb),*artype->sizeInDim(0)));
      eel->setRhs(el);
      el = eel;
      if(TASK_AUTO(sl->symb))
      {
      eel = new SgExprListExp(* new SgArrayRefExp(*TASK_HPS_ARRAY(sl->symb),*artype->sizeInDim(0)));
      eel->setRhs(el);
      el = eel;
      eel = new SgExprListExp(* new SgArrayRefExp(*TASK_LPS_ARRAY(sl->symb),*artype->sizeInDim(0)));
      eel->setRhs(el);
      el = eel;
      }
      //eel = new SgExprListExp(*new SgVarRefExp(TASK_IND_VAR(sl->symb))); // symbol for TASK index variable
      //eel->setRhs(el);
      //el = eel;
   }
   st -> setExpression(0,*el);
   if(len_DvmType)
     st->expr(1)->setType(tlen);  
   lstat -> insertStmtAfter(*st);

   }

// declare  ASYNCID as INTEGER
   if(async_symb){
   symb_list *sl;
   SgArrayType *artype;
   //SgArrayRefExp *ae;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=async_symb; sl; sl=sl->next) {
     //eel = new SgExprListExp(* new SgArrayRefExp(*(sl->symb),*new SgValueExp(ASYNCID_NUMB)));
       //eeq = new SgArrayRefExp(*(sl->symb),*new SgValueExp(ASYNCID_NUMB));
      eeq = new SgArrayRefExp(*(sl->symb));
      artype = isSgArrayType(sl->symb->type());
      if(artype) 
	 eeq->setLhs(artype->getDimList());  //add dimensions of array
      else
	 eeq->setLhs(new SgValueExp(ASYNCID_NUMB));
      if(IN_MODULE)
        eeq = DVMVarInitialization(eeq);
      eel = new SgExprListExp(*eeq);
      eel->setRhs(el);
      el = eel;
   }
   st -> setExpression(0,*el);
   if(len_DvmType)
     st->expr(1)->setType(tlen);  
   if(IN_MODULE)
     st->setVariant(VAR_DECL_90);
   lstat -> insertStmtAfter(*st);
   

// declare common blocks for ASYNCID variables 
   for(sl=async_symb; sl; sl=sl->next) {
    if(IN_COMMON(sl->symb)) {
     el = new SgExprListExp(* new SgArrayRefExp(*(sl->symb)));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*(sl->symb));
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     st->insertStmtAfter(*com);
    }
   }
  }

// declare  scalar variables for copying array header elements used for referencing array  
   if(!HPF_program && dsym ) {
   symb_list *sl;
   coeffs * c;
   int i,rank,i0;
   SgExpression *eepub, *lpub=NULL;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=dsym; sl; sl=sl->next) {
     c = ((coeffs *) sl->symb-> attributeValue(0,ARRAY_COEF));
     if(IS_TEMPLATE(sl->symb) || !c->use) 
       continue;
     int flag_public = IN_MODULE && privateall && sl->symb->attributes() & PUBLIC_BIT ? 1 : 0;
     rank=Rank(sl->symb);
     i0 = opt_base ? 1 : 2;
     if(opt_loop_range) i0=0;
     for(i=i0;i<=rank;i++){
       eel = new SgExprListExp(* new SgVarRefExp(*(c->sc[i])));
       eepub = flag_public ? &eel->copy() : NULL;
       eel->setRhs(el);
       el = eel;
       if(flag_public)
       {         
         eepub->setRhs(lpub);
         lpub = eepub;
       }
     } 
     eel = new SgExprListExp(* new SgVarRefExp(*(c->sc[rank+2])));
     eepub = flag_public ? &eel->copy() : NULL;
     eel->setRhs(el);
     el = eel;
     if(flag_public)
     {
       eepub->setRhs(lpub);
       lpub = eepub;
     }

   }
   if(el){
     st -> setExpression(0,*el);
     if(len_DvmType)
       st->expr(1)->setType(tlen);  
     lstat -> insertStmtAfter(*st);
   }
   if(lpub){
     st = new SgStatement(PUBLIC_STMT);
     st->setExpression(0,*lpub);
     lstat -> insertStmtAfter(*st);
   }
   }


// declare  Pipeline variable for ACROSS implementation
   if(pipeline){
     st = Pipe->makeVarDeclStmt();
     lstat -> insertStmtAfter(*st);
   }

// declare  Debug variable for -dbif regim
   if(dbg_if_regim && dbg_var && !IN_MODULE) {
      st = dbg_var->makeVarDeclStmt();
     lstat -> insertStmtAfter(*st);
  
// declaring COMMON block for Debug variable
  
     el = new SgExprListExp(* new SgVarRefExp(*dbg_var));
     eeq = new SgExpression (COMM_LIST);
     eeq -> setSymbol(*dbgcommon);
     eeq -> setLhs(*el);
     com = new SgStatement(COMM_STAT);
     com->setExpression(0,*eeq);
     lstat->insertStmtAfter(*com);
  }      


// declare  CheckSumma variable for -dc regim
   if(check_sum){
     st = check_sum->makeVarDeclStmt();
     lstat -> insertStmtAfter(*st);
   }

// declare  FileNameVariables
   if(fnlist){
   filename_list *sl;
   for(sl=fnlist; sl; sl=sl->next) {
     st =sl->fns->makeVarDeclStmt();//character variables
     
     st->expr(0)->setLhs(FileNameInitialization(st->expr(0)->lhs(),sl->name));     
     st->setVariant(VAR_DECL_90);
     
     lstat2 -> insertStmtAfter(*st);
   }
   }

// declare  CONSISTENT array headers as INTEGER
   if(consistent_symb) {
   symb_list *sl;
   SgExpression *ea;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed

   el = NULL;
   for(sl=consistent_symb; sl; sl=sl->next) {
  
     /* if(IN_COMMON(sl->symb) && cur_func->variant() != PROG_HEDR)
	continue;*/ /*25.03.03*/
      ea = new SgArrayRefExp(*(CONSISTENT_HEADER(sl->symb)),*new SgValueExp(HSIZE(Rank(sl->symb))));
      ea->setType(*SgTypeInt());       
      eel =   new SgExprListExp(*ea);
      eel->setRhs(el);
      el = eel;
   }
   if(el) {
   st -> setExpression(0,*el); 
   if(len_DvmType)
     st->expr(1)->setType(tlen); 
   lstat -> insertStmtAfter(*st);
   }
   }

// declare variables for saving conditional expression for Arithmetic IF and Computed GO TO
// for regim of debugging and performance analysing
   if(if_goto) {
   symb_list *sl;
   for(sl=if_goto; sl; sl=sl->next)
     {st = (sl->symb)->makeVarDeclStmt();
      lstat -> insertStmtAfter(*st);
     }
   }
 
 HEADERS_:     //begin generating for interface block 

// declare  array headers as INTEGER
   if(dsym) {
   symb_list *sl;
   SgExpression *ea,*ehs;
   st =fdvm[0]->makeVarDeclStmt();// creates INTEGER name, then name is removed
   el = NULL;
   for(sl=dsym; sl; sl=sl->next) {
        if(IS_BY_USE(sl->symb)) continue;
         //if(!isSgArrayType(sl->symb->type())) //for POINTER       
         // sl->symb ->setType(* new SgArrayType(*SgTypeInt()));
      ///if(IS_TEMPLATE(sl->symb) && !RTS2_OBJECT(sl->symb)) { 
      ///     ea = new SgVarRefExp(*(sl->symb));
	
      ///} else {
        ehs = IS_POINTER_F90(sl->symb) ? new SgExpression(DDOT) : new SgValueExp(HEADER_SIZE(sl->symb));
        ea = new SgArrayRefExp(*(sl->symb),*ehs);
        if(IS_POINTER(sl->symb) && (sl->symb->attributes() & DIMENSION_BIT)) { //array of POINTER
          SgArrayType *artype;
          artype = isSgArrayType(sl->symb->type());
          if(artype) 
            (ea->lhs())->setRhs(artype->getDimList());  //add dimensions of array
        }  
      ///}
            //TYPE_BASE(sl->symb->type()->thetype) = SgTypeInt()->thetype;
      ea->setType(*SgTypeInt()); 
      if(IN_MODULE && !IS_POINTER_F90(sl->symb))
        ea = DVMVarInitialization(ea);
      eel =   new SgExprListExp(*ea);
      eel->setRhs(el);
      el = eel;
   }
   if(el) {
     st -> setExpression(0,*el); 
     if(len_DvmType)
       st->expr(1)->setType(tlen); 
     if(IN_MODULE)
       st->setVariant(VAR_DECL_90);
     lstat -> insertStmtAfter(*st);   
   }
 
  }

//declare Common-blocks for TEMPLATE with attribute COMMON
  {   
   symb_list *sl;
   for(sl=dsym; sl; sl=sl->next) {
     if(IS_TEMPLATE(sl->symb) && IN_COMMON(sl->symb)) {     
       el = new SgExprListExp(* new SgVarRefExp(*(sl->symb)));
       eeq = new SgExpression (COMM_LIST);
       eeq -> setSymbol(*(sl->symb));
       eeq -> setLhs(*el);
       com = new SgStatement(COMM_STAT);
       com->setExpression(0,*eeq);
       st->insertStmtAfter(*com);
     }     
   }
  }
// end of declaration generating for interface block
   if(in_interface) return; 

// declare array hpf000(N), N = maxhpf
  if(HPF_program && maxhpf != 0) {    
  typearray = isSgArrayType(hpfbuf->type());
  typearray-> addRange(* new SgValueExp(maxhpf));
  st = hpfbuf ->makeVarDeclStmt();
  if(len_DvmType)
    st->expr(1)->setType(tlen); 
  lstat2 -> insertStmtAfter(*st);
 }

// declare array dvm000(N), N = maxdvm
 if(!(maxdvm <= 3 && fmask[RTLINI] == 0 && fmask[BEGBL] == 0 && fmask[FNAME] == 0 && fmask[GETVM] == 0 && fmask[GETAM] == 0 && fmask[DVMLF] == 0)) {      
  typearray = isSgArrayType(dvmbuf->type());
  typearray-> addRange(* new SgValueExp(maxdvm));
  //dvmbuf-> declareTheSymbol(*func);
  st = dvmbuf ->makeVarDeclStmt();
  if(len_DvmType)
    st->expr(1)->setType(tlen); 
  lstat2 -> insertStmtAfter(*st);
 }

// declare LibDVM functions as INTEGER
  i=0; 
  while ( (i<MAX_LIBFUN_NUM) && (fmask[i] != 1) ) //looking for first element of fmask[] equal to 1
      i++; 
   if(i == MAX_LIBFUN_NUM) goto EXTERN_; 
   st = fdvm[i]->makeVarDeclStmt();
   el = isSgExprListExp(st->expr(0));
                       //   el = new SgExprListExp(* new SgVarRefExp(fdvm[0]));
   for(j=i+1; fdvm[j] && j<MAX_LIBFUN_NUM ; j++) {
     if(fmask[j] == 1) {
      eel =   new SgExprListExp(* new SgVarRefExp(fdvm[j]));
      eel->setRhs(*el);
      el = eel;
      //el->append (* em[0]);
     }
   }
   st -> setExpression(0,*el); 
   if(len_DvmType)
     st->expr(1)->setType(tlen); 

   lstat2 -> insertStmtAfter(*st);

// declare LibDVM subroutines as EXTERNAL
EXTERN_:
  i=0; 
  while ( (i<MAX_LIBFUN_NUM) && (fmask[i] != 2) ) //looking for first element of fmask[] equal to 2
      i++; 
   if(i == MAX_LIBFUN_NUM)   goto GPU_; 
   st = new SgStatement(EXTERN_STAT);
   el = new SgExprListExp(* new SgVarRefExp(fdvm[i]));
   for(j=i+1; fdvm[j] && j<MAX_LIBFUN_NUM ; j++) {
     if(fmask[j] == 2) {
      eel =   new SgExprListExp(* new SgVarRefExp(fdvm[j]));
      eel->setRhs(*el);
      el = eel;
     }
   }
   st -> setExpression(0,*el); 

   lstat2 -> insertStmtAfter(*st);

GPU_:  
// declare GPU objects
   if(!IN_MODULE)   
     DeclareVarGPU(lstat,tlen);  /*ACC*/
// add comment
   if(lstat->lexNext() != st_next)
    (lstat->lexNext())->setComments("! DVMH declarations \n");
}

void TranslateFileDVM(SgFile *f)
{
   SgStatement *func,*stat,*end_of_source_file;
   SgStatement *end_of_unit;  // last node (END or CONTAINS statement) of program unit

                
   InitializeACC();

// grab the first statement in the file.
   stat = f->firstStatement(); // file header
//last statement of file 
   end_of_source_file = FILE_LAST_STATEMENT(stat) ? *FILE_LAST_STATEMENT(stat) : lastStmtOfFile(f); 
// add empty-statement to insert generated procedures at the end of file (after that)  
   end_of_source_file->insertStmtAfter( *new SgStatement(COMMENT_STAT),*stat);
   end_of_source_file = end_of_source_file->lexNext();
   if(ACC_program || parloop_by_handler)
     end_of_source_file->addComment("!-----------------------------------------------------------------------\n"); 
  
                     //numfun = f->numberOfFunctions(); //  number of functions
// function is program unit accept BLOCKDATA and MODULE (F90),i.e. 
// PROGRAM, SUBROUTINE, FUNCTION
   if(debug_fragment || perf_fragment) // is debugging or performance analizing regime specified ?
     BeginDebugFragment(0,NULL);// begin the fragment with number 0 (involving whole file(program) 
                    //for(i = 0; i < numfun; i++) { 
                    //   func = f -> functions(i);
  
   for(stat=stat->lexNext(); stat!=end_of_source_file; stat=end_of_unit->lexNext()) 
   {
     if(stat->variant() == CONTROL_END) {  //end of procedure or module with CONTAINS statement  
       end_of_unit = stat; 
       continue;
     }

     if( stat->variant() == BLOCK_DATA){//BLOCK_DATA header 
       TransBlockData(stat, end_of_unit); //replacing variant VAR_DECL with VAR_DECL_90 for declaration statement with initialisation
       continue;
     }
     // PROGRAM, SUBROUTINE, FUNCTION header
     func = stat; 
     cur_func = stat;
    
        //scanning the Symbols Table of the function 
        //     ScanSymbTable(func->symbol(), (f->functions(i+1))->symbol());

    
     // translating the program unit (procedure, module)
     if(only_debug)
        InsertDebugStat(func, end_of_unit);
     else
        TransFunc(func, end_of_unit);
        
   }
  
   if(ACC_program)
   { InsertCalledProcedureCopies(); 
     AddExternStmtToBlock_C();
     GenerateEndIfDir();
     GenerateDeclarationDir(); 
     GenerateStmtsForInfoFile();    
   }
}


void TransFunc(SgStatement *func,SgStatement* &end_of_unit) {
  SgStatement *stmt,*last,*rmout, *data_stf, *first, *first_dvm_exec, *last_spec, *stam, *last_dvm_entry, *lentry = NULL;
  SgStatement *st_newv = NULL;// for NEW_VALUE directives
  SgExpression *e;
  SgStatement *task_region_parent = NULL, *on_stmt = NULL, *mod_proc, *begbl = NULL;
  SgStatement *copy_proc = NULL;
  SgStatement *has_contains = NULL;
  SgLabel *lab_exec;

  int i;
  int begin_block;
  distribute_list *distr =  NULL;
  distribute_list *dsl,*distr_last = NULL;
  align *pal = NULL;
  align *node, *root = NULL;
  stmt_list *pstmt = NULL;
  int inherit_is = 0;  
  int contains[2];
  int in_on = 0;
  char io_modes_str[4] = "\0";

  //initialization
  dsym = NULL;
  grname = NULL;
  saveall = 0;
  maxdvm = 0;
  maxhpf = 0;
  count_reg = 0;
  initMask();
  data_stf = NULL;
  loc_distr = 0;
  begin_block = 0;
  goto_list = NULL;
  proc_symb = NULL;
  task_symb = NULL;
  consistent_symb = NULL;
  async_symb = NULL;
  check_sum = NULL;
  loc_templ_symb=NULL;
  index_symb = NULL;
  nio = 0;
  task_do = NULL;
  for (i=0; i<Ntp; i++)
  {  mem_use[i] = 0;
     mem_symb[i] = NULL;
  }
  mem_use_structure = NULL;
  heap_ar_decl = NULL;
  is_heap_ref = 0;
  //heap_size = 1;
  heap_size = 0;
  pref_st = NULL;
  pipeline = 0;  
  registration = NULL;
  filename_num = 0;
  fnlist = NULL;
  nloopred = 0;
  nloopcons = 0;
  wait_list = NULL;
  SIZE_function = NULL;
  dvm_const_ref = 0;
  in_interface = 0;
  mod_proc = NULL;
  if_goto = NULL;
  nifvar = 0;
  entry_list = NULL;
  dbif_cond = 0;
  dbif_not_cond = 0;
  last_dvm_entry = NULL;
  allocated_list = NULL;
  privateall = 0;
  //if(ACC_program)
  InitializeInFuncACC();
  all_replicated = isInternalOrModuleProcedure(func) ? 0 : 1;
  //Private_Vars_Function_Analyzer(func);
  TempVarDVM(func);
  initF90Names();
  first = func->lexNext();
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
  if(!(last->variant() == CONTROL_END))
     printf(" END Statement is absent\n");
/*
  fsymb = func->symbol();
  if((func->variant() == PROG_HEDR) && !strcmp(fsymb->identifier(),"_MAIN")){ 
     progsymb = new SgFunctionSymb(PROGRAM_NAME, "MAIN", *SgTypeInt(), *current_file->firstStatement() );
     func->setSymbol(*progsymb);
  }
*/

//**********************************************************************
//           Specification Directives Processing 
//**********************************************************************
// follow the statements of the function in lexical order
// until first executable statement
  for (stmt = first; stmt && (stmt != last); stmt = stmt->lexNext()) {
               //printf("statement %d %s\n",stmt->lineNumber(),stmt->fileName());
    if (!isSgExecutableStatement(stmt)) //is Fortran specification statement
// isSgExecutableStatement: 
//               FALSE  -  for specification statement of Fortan 90
//               TRUE   -  for executable statement of Fortan 90 and
//                         all directives of F-DVM 
      {
	 //!!!debug
         //  printVariantName(stmt->variant()); //for debug
         //  printf("\n");

        //discovering distributed arrays in COMMON-blocks
        if(stmt->variant()==COMM_STAT) {
          DeleteShapeSpecDAr(stmt);

          if( !DeleteHeapFromList(stmt) ) { //common list is empty
             stmt=stmt->lexPrev();
             stmt->lexNext()->extractStmt(); //deleting the statement
          } 
          continue; 
	}  
	// analizing SAVE statement
	if(stmt->variant()==SAVE_DECL) { 
           if (!stmt->expr(0))  //SAVE without name-list
             saveall = 1;
           else if(IN_MAIN_PROGRAM)
             pstmt = addToStmtList(pstmt, stmt);   //for extracting and replacing by SAVE without list
           continue;
        }
        // deleting SAVE-attribute from Type Declaration Statement (for replacing by SAVE without list)
        if(IN_MAIN_PROGRAM && isSgVarDeclStmt(stmt))          
             DeleteSaveAttribute(stmt);

	if(IN_MODULE && stmt->variant() == PRIVATE_STMT && !stmt->expr(0))
             privateall = 1; 

        // deleting distributed arrays from variable list of declaration
        // statement and testing are there any group names
        if( isSgVarDeclStmt(stmt) || isSgVarListDeclStmt(stmt)) {
           
           if( !DeleteDArFromList(stmt) ) { //variable list is empty
             stmt=stmt->lexPrev();
             stmt->lexNext()->extractStmt(); //deleting the statement
           }
           continue;
        }
 
        if((stmt->variant() == DATA_DECL) || (stmt->variant() == STMTFN_STAT)) {
	  if(stmt->variant() == STMTFN_STAT && stmt->expr(0) && stmt->expr(0)->symbol() && ((!strcmp(stmt->expr(0)->symbol()->identifier(),"number_of_processors")) || (!strcmp(stmt->expr(0)->symbol()->identifier(),"processors_rank")) || (!strcmp(stmt->expr(0)->symbol()->identifier(),"processors_size")))){
             stmt=stmt->lexPrev();
             stmt->lexNext()->extractStmt(); 
                           //deleting the statement-function declaration named 
	                   //  NUMBER_OF_PROCESSORS or PROCESSORS_RANK or PROCESSORS_SIZE 
             continue;
          } 
          if(stmt->variant()==STMTFN_STAT)
            DECL(stmt->expr(0)->symbol()) = 2;     //flag of statement function name

          if(!data_stf)
            data_stf = stmt; //first statement in data-or-function statement part 
          continue; 
        }   
        if (stmt->variant() == ENTRY_STAT) {
	     //err("ENTRY statement is not permitted in FDVM", stmt);  
          warn("ENTRY among specification statements", 81,stmt);  
          continue;
        }
        if(stmt->variant() == INTERFACE_STMT || stmt->variant() == INTERFACE_ASSIGNMENT || stmt->variant() == INTERFACE_OPERATOR){
          stmt = InterfaceBlock(stmt);  //stmt->lastNodeOfStmt();
          continue;
	}

        if( stmt->variant() == USE_STMT) {
          all_replicated=0; 
          if(stmt->lexPrev() != func && stmt->lexPrev()->variant()!=USE_STMT) 
            err("Misplaced USE statement", 639, stmt); 
          UpdateUseListWithDvmArrays(stmt);
          continue;
        }

	if(stmt->variant() == STRUCT_DECL){
          StructureProcessing(stmt);
          stmt=stmt->lastNodeOfStmt();
          continue;
        }

        continue;             
      }

    if ((stmt->variant() == FORMAT_STAT))        // || (stmt->variant() == DATA_DECL))
       {// printf("  ");
	 // printVariantName(stmt->variant()); //for debug
         //printf("\n");
         continue;
       }
    

// processing the DVM Specification Directives

    //including the DVM specification directive to list of these directives
      pstmt = addToStmtList(pstmt, stmt); 
    
    switch(stmt->variant()) {
       case(ACC_ROUTINE_DIR):
           ACC_ROUTINE_Directive(stmt); 
           continue;
       case(HPF_TEMPLATE_STAT):
           if(IN_MODULE && stmt->expr(1))
              err("Illegal directive in module",632,stmt);
           TemplateDeclarationTest(stmt);
           continue;
       case(HPF_PROCESSORS_STAT):
         //!!!for debug
	 // printf("CDVM$    ");
         //  printVariantName(stmt->variant()); 
         //  printf("\n");
         //
           continue;
     case(DVM_DYNAMIC_DIR):
          {SgExpression *el;
           SgSymbol *ar;
           for(el = stmt->expr(0); el; el=el->rhs()){ // array name list
               ar = el->lhs()->symbol();  //array name
               //if(!(ar->attributes() & ALIGN_BIT) && !(ar->attributes() & DISTRIBUTE_BIT) && !(ar->attributes() & INHERIT_BIT))
		 // SYMB_ATTR(ar->thesymb)= SYMB_ATTR(ar->thesymb) | POSTPONE_BIT;
	   }
           all_replicated = 0;
          }
           continue;
       case(DVM_SHADOW_DIR):
           {SgExpression *el;
            SgExpression **she = new (SgExpression *);
            SgSymbol *ar;
            int nw=0;
         // calculate lengh of shadow_list
            for(el = stmt->expr(1); el; el=el->rhs())
               nw++;
           *she = stmt->expr(1);
           for(el = stmt->expr(0); el; el=el->rhs()){ // array name list
               ar = el->lhs()->symbol();  //array name
               ar->addAttribute(SHADOW_WIDTH, (void *) she,                                                                  sizeof(SgExpression *));
	       /*   if(nw<Rank(ar)) 
                 Warning("Length of shadow-spec-list is smaller than the rank of array '%s'", ar->identifier(), stmt);   
		*/ 
               if (nw!=Rank(ar)) // wrong shadow width list
                Error("Length of shadow-edge-list is not equal to the rank of array '%s'", ar->identifier(), 88, stmt);
	   }
           }   
//!!!for debug
           //printf("CDVM$    ");
           //printVariantName(stmt->variant()); 
           // printf("\n");
//
           continue;

       case(DVM_TASK_DIR): 
           {SgExpression * sl; 
	    for(sl=stmt->expr(0); sl; sl = sl->rhs()) 
              task_symb=AddToSymbList(task_symb, sl->lhs()->symbol()); 
           }
            continue;

      case(DVM_CONSISTENT_DIR): 
           {SgExpression * sl; 
	    for(sl=stmt->expr(0); sl; sl = sl->rhs()) {
              SgSymbol **header = new (SgSymbol *); 
              consistent_symb=AddToSymbList(consistent_symb, sl->lhs()->symbol());
              *header= CreateConsistentHeaderSymb(sl->lhs()->symbol());
              // adding the attribute (CONSISTENT_ARRAY_HEADER) to distributed array symbol
              sl->lhs()->symbol()->addAttribute(CONSISTENT_ARRAY_HEADER, (void*) header, sizeof(SgSymbol *));
            } 
           }
            continue;

       case(DVM_INDIRECT_GROUP_DIR):
       case(DVM_REMOTE_GROUP_DIR):
           {SgExpression * sl; 
            if(options.isOn(NO_REMOTE))
               continue;    
	    for(sl=stmt->expr(0); sl; sl = sl->rhs()){
               SgArrayType *artype;
               artype = new SgArrayType(*SgTypeInt());  
               artype->addRange(*new SgValueExp(3));
               sl->lhs()->symbol()->setType(artype);
               AddToGroupNameList(sl->lhs()->symbol()); 
           }
           }
            continue;

       case DVM_CONSISTENT_GROUP_DIR:
       case DVM_REDUCTION_GROUP_DIR:
	   {SgExpression * sl; 
	    for(sl=stmt->expr(0); sl; sl = sl->rhs())
               AddToGroupNameList(sl->lhs()->symbol()); 
           }
            continue;
      
       case(DVM_INHERIT_DIR): 
          {SgExpression * sl; 
            inherit_is = 1; all_replicated = 0;
	    for(sl=stmt->expr(0); sl; sl = sl->rhs()){
              if(IS_DUMMY(sl->lhs()->symbol()))
                ArrayHeader(sl->lhs()->symbol(),1);
              else 
                Error("Inconsistent declaration of identifier '%s'",sl->lhs()->symbol()->identifier(),16,stmt);
            }
          }
            continue;

 ALIGN:
       case(DVM_ALIGN_DIR): // adding the alignees and the align_base to
                            // the Align_Tree_List
         { SgSymbol *base, *alignee;
           SgExpression *eal;
           algn_attr *attr_base, *attr_alignee;
           //dvm = 1;
           attr_base = attr_alignee = NULL;
           if(stmt->expr(2)){
             base = (stmt->expr(2)->variant()==ARRAY_OP) ? (stmt->expr(2))->rhs()->symbol()                                                          :  (stmt->expr(2))->symbol();
                                                   // align_base symbol
             attr_base = (algn_attr *) base->attributeValue(0,ALIGN_TREE);
           }
           else 
             base = NULL;
           for(eal=stmt->expr(0); eal; eal=eal->rhs()) {
                                              //scanning the alignees list 
                                              // (eal - SgExprListExp)
              alignee = (eal->lhs())->symbol();
              if(alignee == base)
              {  Error("'%s' is aligned with itself", alignee->identifier(), 266,stmt);
                 continue;
              } 
              if(stmt->expr(1) && IN_MODULE && IS_ALLOCATABLE_POINTER(alignee))
                Error("Inconsistent declaration of identifier '%s'", alignee->identifier(), 16,stmt);
              attr_alignee=(algn_attr *) alignee->attributeValue(0,ALIGN_TREE);
              if(stmt->expr(2) && (stmt->expr(2)->variant()==ARRAY_OP) && !IS_DUMMY(alignee))
	        Error("Inconsistent declaration of identifier '%s'", alignee->identifier(), 16,stmt);
              if(!stmt->expr(1) && ! stmt->expr(2)) {
                SYMB_ATTR(alignee->thesymb)= SYMB_ATTR(alignee->thesymb) | POSTPONE_BIT;
                if(!attr_alignee){
                // creating new node for the alignee
                 node = new align; 
                 node->symb = alignee;
                 node->next = pal;
                 node->alignees = NULL;
                 node->align_stmt = stmt;
                 pal = node;
                // adding the attribute (ALIGN_TREE) to the alignee symbol
                 attr_alignee = new algn_attr;
                 attr_alignee->type = NODE;
                 attr_alignee->ref  = node;     
                 alignee->addAttribute(ALIGN_TREE, (void *) attr_alignee,                                                            sizeof(algn_attr));
               } else 
                 if(attr_alignee->type == NODE) {
                   Err_g("Duplicate aligning of the array '%s'",alignee->identifier(),82);
                   continue;
                 }
                node= attr_alignee->ref;
                node->align_stmt = stmt; 
                continue;
                 
	      }  
              if (!pal || (!attr_base && !attr_alignee))  {
                 // creating new tree with root for align_base
                 node = new align; // creating new node for the alignee
                 node->symb = alignee;
                 node->next = NULL;
                 node->alignees = NULL;
                 node->align_stmt = stmt;
                 root = new align; // creating new node for the base (root)
                 root->symb = base;
                 root->next = pal;
                 root->alignees = node;
                 root->align_stmt = NULL;
                 pal = root; // pal points to this tree
      
                 // adding the attribute (ALIGN_TREE) to the base symbol
                 attr_base = new algn_attr;
                 attr_base->type = ROOT;
                 attr_base->ref  = root;     
                 base->addAttribute(ALIGN_TREE, (void *) attr_base,                                                                  sizeof(algn_attr));
//for debug
               //printf("Attribute ALIGN_TREE of %s : type = %d\n",         base->identifier(), ((algn_attr*) base->attributeValue(0,ALIGN_TREE))->type);                 
                 // adding the attribute (ALIGN_TREE) to the alignee symbol
                 attr_alignee = new algn_attr;
                 attr_alignee->type = NODE;
                 attr_alignee->ref  = node;     
                 alignee->addAttribute(ALIGN_TREE, (void *) attr_alignee,                                                            sizeof(algn_attr));
//for debug
               //printf("Attribute ALIGN_TREE of %s : type = %d\n", alignee->identifier(), ((algn_attr*) alignee->attributeValue(0,ALIGN_TREE))->type);
              } 
              else if (!attr_alignee && attr_base) {
                 // creating new node for the alignee and
                 // adding it to  alignees_list of the node for align_base 
                 root = ((algn_attr*) base->attributeValue(0,ALIGN_TREE))->ref;
                 node = new align; // creating new node for the alignee
                 node->symb = alignee;
                 node->next = root->alignees;
                 node->alignees = NULL;
                 node->align_stmt = stmt;
                 root->alignees = node;  // adding it to  alignees_list of
                                         // the node for align_base
                 // adding the attribute (ALIGN_TREE) to the alignee symbol
                 attr_alignee = new algn_attr;
                 attr_alignee->type = NODE;
                 attr_alignee->ref  = node;     
                 alignee->addAttribute(ALIGN_TREE, (void *) attr_alignee,                                                            sizeof(algn_attr));
//for debug
               //printf("Attribute ALIGN_TREE of %s : type = %d\n", alignee->identifier(), ((algn_attr*) alignee->attributeValue(0,ALIGN_TREE))->type);
              } 
              else if (attr_alignee && !attr_base) {

                 if(attr_alignee->type == NODE) {
                   Err_g("Duplicate aligning of the array '%s'",                                                               alignee->identifier(),82);
                   continue;
                 }
                 // creating new node for align_base,   
                 // adding a tree for the alignee to alignees_list of it

                 node=((algn_attr*) alignee->attributeValue(0,ALIGN_TREE))->ref;
                 // deleting tree for the alignee from Align_Tree_List
                 if (pal == node)
                    pal = node->next;
                 else
                    for(root=pal ; root->next != node; root=root->next)
                 ; 
                 root->next = node->next;
       
                 root = new align; // creating new node for the base (root)
                 root->symb = base;
                 root->next = pal;
                 root->alignees = node;
                 root->align_stmt = NULL;                 
                 node->align_stmt = stmt; // setting the field 'align_stmt' 
                                          // of the node for alignee
                 node->next = NULL; // setting off 'next' field of the node
                                    //for alignee
                 pal = root; // pal points to new tree
                // adding the attribute (ALIGN_TREE) to the base symbol
                 attr_base = new algn_attr;
                 attr_base->type = ROOT;
                 attr_base->ref  = root;     
                 base->addAttribute(ALIGN_TREE, (void *) attr_base,                                                                  sizeof(algn_attr));
//for debug
               //printf("Attribute ALIGN_TREE of %s : type = %d\n", base->identifier(), ((algn_attr*) base->attributeValue(0,ALIGN_TREE))->type);       
                 // changing field 'type'of the attribute (ALIGN_TREE)
                 // of the alignee symbol
                    attr_alignee->type = NODE;
//for debug
               //printf("Attribute ALIGN_TREE of %s : type = %d\n", alignee->identifier(), ((algn_attr*) alignee->attributeValue(0,ALIGN_TREE))->type);
        
             }
             else if (attr_alignee && attr_base) {

	       if(attr_alignee->type == NODE) {
                   Err_g("Duplicate aligning of the array '%s'",                                                                 alignee->identifier(),82);
                   continue;
               }
                //testing: is a node for align_base the node of alignee tree  
                // ...
                 // adding a tree for the alignee to alignees_list
                 // of the node for align_base
                 node=((algn_attr*) alignee->attributeValue(0,ALIGN_TREE))->ref;                 
                 // deleting tree for the alignee from Align_Tree_List
                 if (pal == node)
                    pal = node->next;
                 else
                    for(root=pal ; root->next != node; root=root->next)
                 ; 
                 root->next = node->next;
                    
                 root = ((algn_attr*) base->attributeValue(0,ALIGN_TREE))->ref;
                 node->align_stmt = stmt;
                 node->next = root->alignees;
                 root->alignees = node;

                 // changing field 'type'of the attribute (ALIGN_TREE)
                 // of the alignee symbol
                    attr_alignee->type = NODE;
//for debug
               //printf("Attribute ALIGN_TREE of %s : type = %d\n", alignee->identifier(), ((algn_attr*) alignee->attributeValue(0,ALIGN_TREE))->type);
             }                

           }             
        }
//!!!for debug
           //printf("CDVM$    ");
           //printVariantName(stmt->variant()); 
           //printf("\n");
//
           continue;

 DISTR:
       case(DVM_DISTRIBUTE_DIR): // adding the statement to the Distribute
                                 // directive list          
           //dvm = 1;
           if (!distr) {
              distr = new distribute_list;
              distr->stdis = stmt;
              distr->next =  NULL;
              distr_last = distr;
           } else {
              dsl = new distribute_list;
              dsl->stdis = stmt;
              dsl->next = NULL;
              distr_last->next = dsl;
              distr_last = dsl;
           }
//!!!for debug
           //printf("CDVM$    ");
           //printVariantName(stmt->variant()); 
           //printf("\n");
//
           DistributeArrayList(stmt); //adding the attribute DISTRIBUTE_ to distribute-array symbol
           continue;
       case(DVM_POINTER_DIR):
           {SgExpression *el;
            SgStatement **pst = new (SgStatement *);
            SgSymbol *sym;
            int *index;
            *pst = stmt;
            for(el = stmt->expr(0); el; el=el->rhs()){ //  name list
               sym = el->lhs()->symbol();  // name
               sym->addAttribute(POINTER_, (void *) pst,                                                                  sizeof(SgStatement *));  
               if((sym->type()->variant() != T_INT) && (sym->type()->variant() != T_ARRAY))
                 Error("POINTER '%s' is not integer variable",sym->identifier(),83,stmt); 
               if( (sym->type()->variant() == T_ARRAY) && (sym->type()->baseType()->variant() != T_INT))
                 Error("POINTER '%s' is not integer variable",sym->identifier(),83,stmt); 
               //if(IS_DUMMY(sym) || IN_COMMON(sym))
               if(IS_DUMMY(sym))
                  Error("Inconsistent declaration of identifier '%s' ",sym->identifier(),16,stmt); 
               if(IS_SAVE(sym))
                  Error("POINTER may not have SAVE attribute: %s",sym->identifier(),84,stmt); 
           /*
               if(!IS_DVM_ARRAY(sym)) 
                  Error("POINTER '%s' is not distributed object",sym->identifier(), 85,stmt);
	   */
               if(!IS_DVM_ARRAY(sym)) 
                    // AddDistSymbList(sym);
                 ArrayHeader(sym,0);
                 index = new int;
                 *index = heap_size+1;
                 // adding the attribute (HEAP_INDEX) to POINTER symbol
                 sym->addAttribute(HEAP_INDEX, (void *) index, sizeof(int));  
                 heap_size = heap_size + HEADER_SIZE(sym)*NumberOfElements(sym,stmt,1);                
	   }
           }   
//!!!for debug
           //printf("CDVM$    ");
           //printVariantName(stmt->variant()); 
           // printf("\n");
//
           continue;
 
       case (DVM_HEAP_DIR):
           heap_ar_decl = new SgArrayRefExp(*heapdvm);
           continue;

       case (DVM_ASYNCID_DIR):
           {SgExpression * sl; 
            SgArrayType *artype;
	    for(sl=stmt->expr(0); sl; sl = sl->rhs()) {              
               artype = new SgArrayType(*SgTypeInt());  
               artype->addRange(*new SgValueExp(ASYNCID_NUMB));
               if(sl->lhs()->lhs()) //array specification
                   artype->addRange(*(sl->lhs()->lhs()));
               sl->lhs()->symbol()->setType(artype);
                async_symb=AddToSymbList(async_symb, sl->lhs()->symbol());
                if(stmt->expr(1)) // ASYNCID,COMMON:: name-list
                  SYMB_ATTR(sl->lhs()->symbol()->thesymb)= SYMB_ATTR(sl->lhs()->symbol()->thesymb) | COMMON_BIT;
	    } 
           }
           continue;

       case (DVM_VAR_DECL):
          { SgExpression *el,*eol,*eda;
            SgSymbol *symb;
            int i, nattrs[8]; 
            for(i=0; i<8; i++)
               nattrs[i] = 0;
            eda = NULL;
            //testing obgect list	    
	    isListOfArrays(stmt->expr(0),stmt);

            for(el = stmt->expr(2); el; el=el->rhs()) // attribute list
	      switch(el->lhs()->variant()) {
	          case (ALIGN_OP):
                      nattrs[0]++;
                      eda = el->lhs();
                      break;
                  case (DISTRIBUTE_OP): 
                      nattrs[1]++;
                      eda = el->lhs();
                      break; 
                  case (TEMPLATE_OP): 
                      nattrs[2]++;
                      TemplateDeclarationTest(stmt);
                      break;  
                  case (PROCESSORS_OP): 
                      nattrs[3]++;
                      break;
                  case (DIMENSION_OP): 
                      nattrs[4]++;
                      for(eol=stmt->expr(0); eol; eol=eol->rhs()) { //testing object list
                        symb=eol->lhs()->symbol();
                        if(!( (symb->attributes() & TEMPLATE_BIT) ||  (symb->attributes() & PROCESSORS_BIT)))
                          Error("Object '%s' has neither TEMPLATE nor PROCESSORS attribute",symb->identifier(), 86,stmt);                 
                      }
                      //testing shape specification (el->lhs()->lhs()) : each expression                           is specification expression 
                   if((el->lhs()->lhs()) && (! TestShapeSpec(el->lhs()->lhs())))
                        err("Illegal shape specification in DIMENSION attribute",87,stmt);  
                      break; 
                  case (DYNAMIC_OP): 
                      nattrs[5]++;
                      break;       
                  case (SHADOW_OP): 
                      {SgExpression *eln;
                       SgExpression **she = new (SgExpression *);
                       SgSymbol *ar;
                       int nw=0;

                       nattrs[6]++;

                       // calculate lengh of shadow_list
                       for(eln = el->lhs()->lhs() ; eln; eln=eln->rhs())
                           nw++;
                       *she = el->lhs()->lhs(); //shadow specification
                       for(eln = stmt->expr(0); eln; eln=eln->rhs()){ // array name list
                          ar = eln->lhs()->symbol();  //array name
                          ar->addAttribute(SHADOW_WIDTH, (void *) she,                                                                  sizeof(SgExpression *)); 
			  /* if(nw<Rank(ar)) 
                            Warning("Length of shadow-spec-list is smaller than the rank of array '%s'", ar->identifier(), stmt);   
			   */
                          if (nw!=Rank(ar)) // wrong shadow width list
                            Error("Length of shadow-edge-list is not equal to the rank of array '%s'", ar->identifier(), 88,stmt);
                       }     
                       break;   
                      }
                  case (COMMON_OP): 
                      nattrs[7]++;
                      break;                           
	      }
              for(i=0; i<8; i++)
               if( nattrs[i]>1)
                 Error("%s attribute appears more than once in the combined-directive", AttrName(i), 89, stmt);
	      if(eda)
                if(eda->variant() == ALIGN_OP){
                      stmt->setVariant(DVM_ALIGN_DIR);
                      if(! eda->lhs())
                         BIF_LL2(stmt->thebif)= NULL;
                      else
                         BIF_LL2(stmt->thebif)= eda->lhs()->thellnd;
                      if(! eda->rhs())
                         BIF_LL3(stmt->thebif)= NULL;
                      else
                         BIF_LL3(stmt->thebif)= eda->rhs()->thellnd;
                      //stmt->setExpression(1,*eda->lhs());
                      //stmt->setExpression(2,*eda->rhs());
                      goto ALIGN; 
                }
                else {
                      stmt->setVariant(DVM_DISTRIBUTE_DIR);
                      if(! eda->lhs())
                         BIF_LL2(stmt->thebif)=NULL;
                      else
                         BIF_LL2(stmt->thebif)= eda->lhs()->thellnd;
                      if(! eda->rhs())
                         BIF_LL3(stmt->thebif)= NULL;
                      else
                         BIF_LL3(stmt->thebif)= eda->rhs()->thellnd;
                      //stmt->setExpression(1,*eda->lhs());
                      //stmt->setExpression(2,*eda->rhs());
                      if( eda->symbol())
                         stmt->setSymbol(*eda->symbol());
                      goto DISTR; 
                }
          }
           continue; 
 
    }


// all declaration statements are processed,
// current statement is executable (F77/DVM)

    break;
  }

  if(pstmt && (stmt != last))
    pstmt = pstmt->next; //deleting first executable statement from 
                         // DVM Specification Directive List  

//**********************************************************************
//              LibDVM References Generation
//           for distributed and aligned arrays
//**********************************************************************

  //TempVarDVM(func);
  first_exec = stmt; // first executable statement

// testing procedure (-dbif2 regim)
  if(debug_regim && dbg_if_regim>1 && ((func->variant() == PROC_HEDR) || (func->variant() == FUNC_HEDR)) && !pstmt && !isInternalOrModuleProcedure(func) && !lookForDVMdirectivesInBlock(first_exec,func->lastNodeOfStmt(),contains) && !contains[0] && !contains[1])
     copy_proc = CreateCopyOfExecPartOfProcedure();  

  lab_exec = first_exec->label(); // store the label of first ececutable statement 
  BIF_LABEL(first_exec->thebif) = NULL;
  last_spec = first_exec->lexPrev();//may be extracted after
  where = first_exec; //before first executable statement will be inserted new statements
  stam = NULL;
  if(grname)
     CreateRedGroupVars();

  ndvm = 1; // ndvm is number of first free element of array "dvm000"
  nhpf = 1; // nhpf is number of first free element of array "hpf000"

//generating "dummy" assign statement (always it is deleted)
// dvm000(1) = fname(file_name)
//function 'fname' tells the name of source file to DVM run-time system
  InsertNewStatementBefore(D_Fname(),first_exec);
  first_dvm_exec = last_spec->lexNext(); //first DVM function call

  if(IN_MODULE){
     if(TestDVMDirectivesInModule(pstmt) || TestUseStmts() || debug_regim) {
       mod_proc = CreateModuleProcedure(cur_func,first_exec,has_contains);
       where = mod_proc->lexNext();
       end_of_unit = where;
     } else {
       first_dvm_exec = last_spec->lexNext();
       goto EXEC_PART_;
     }
  }

  if(HPF_program) 
     first_hpf_exec = first_dvm_exec;

  if(func->variant() == PROG_HEDR)  { // MAIN-program
//generating a call statement:
// call dvmlf(line_number_of_first_executable_statement,source-file-name)
      LINE_NUMBER_BEFORE(first_exec,first_exec);
//generating function call  ftcntr(...)
//function 'ftcntr' checks Fortran and C data type compatibility
      TypeControl_New();  
//generating the function call which initializes the control structures of DVM run-time system,
//   it's inserted in MAIN program) 
// dvm000(1) = <flag>
// call dvmh_init(dvm000(1))
       RTL_GPU_Init();
       if(!task_symb)  // !!! added the condition temporarily 
       {
         BeginBlock_H();
         begin_block = 1;
         begbl = cur_st;
       }
       if(dbg_if_regim)
         InitDebugVar();              
  }

  else if(func->variant() == MODULE_STMT)  // Module
    ndvm++; 
  else
// generating assign statement
// dvm000(1) = BegBl()
// ( function BegBl defines the begin of object localisation block) 
    if(distr || task_symb || TestDVMDirectivesInProcedure(pstmt)) { 
       BeginBlock_H();
       begin_block = 1;
       begbl = cur_st;
    }  
    else
       ndvm++; 

//generating  assign statement
// dvm000(2) = GetAM()
//(function GetAM  creates initial abstract machine)
//and  assign statement
// dvm000(3) = GetPS(AMRef)
//(function GetPS returns virtual machine reference, on what abstract
// machine is mapped)
  stam = NULL;

  ndvm = 4; // 3 first elements are reserved 

//generating call (module procedure) and/or assign statements for USE statements
  GenForUseStmts(func,where);

//Creating (reconfiguring) processor systems
  ReconfPS(pstmt);

//Creating task arrays
   if(task_symb){
     symb_list *tl; 
     for(tl=task_symb; tl; tl=tl->next) ///looking through the task symbol list
       CreateTaskArray(tl->symb);   
   }
//Initializing  groups 
   if(grname && !IN_MODULE) 
       InitGroups();
   
//Initializing HEAP counter
   if(heap_size != 0 ) //there are declared POINTER variables 
     if( !heap_ar_decl )
      Err_g("Missing %s declaration", "HEAP", 91);
   // else
      //generating assign statement: HEAP(1) = 2
   // InitHeap(heap_ar_decl->symbol());
//Initializing ASYNCID counter
   if(!IN_MODULE)
     //if(IN_MAIN_PROGRAM)   // (27.01.05)
       InitAsyncid();
//Creating CONSISTENT arrays
  /*  if(consistent_symb){
       symb_list *cl; 
       for(cl=consistent_symb; cl; cl=cl->next) ///looking through the consistent array symbol list
         CreateConsistentArray(cl->symb);   
   }*/
//Looking through the Distibute Directive List
   for(dsl=distr; dsl; dsl=dsl->next) {  
     SgExpression *target,*ps = NULL;
     int idis; // DisRuleArray index 
     SgSymbol *das;
     int no_rules;
     no_rules = 1;
     for(e=dsl->stdis->expr(0); e; e=e->rhs()){//are there in dist-name-list array-name 
                                               //that is not a dummy, a pointer, and
                                               //a COMMON-block element in procedure                         
        das = (e->lhs())->symbol();
        if( !IS_DUMMY(das) && !IS_POINTER(das) && !(IN_COMMON(das) &&  (das->scope()->variant() != PROG_HEDR)) && !IS_ALLOCATABLE_POINTER(das)){
           no_rules = 0; ps = NULL;
           break;
        }
     } 
     
     SgExpression *distr_rule_list = doDisRules(dsl->stdis,no_rules,idis);
     nproc = 0;
     target = hasOntoClause(dsl->stdis);
     if( target )  { //is there ONTO_clause 
       nproc = RankOfSection(target);
       if(dsl->stdis->expr(1) && nblock && nproc && (nblock > nproc))
          Error("The number of BLOCK/GENBLOCK elements of dist-format-list is greater than the rank of PROCESSORS '%s'  ", target->symbol()->identifier(),90,dsl->stdis);
     } 
     /*  if(dsl->stdis->expr(1) && nblock && (nblock != nblock_all))
          err("The number of BLOCK elements of dist-format-list must be the same in all  DISTRIBUTE  and REDISTRIBUTE directives", dsl->stdis);*/
	 
     if(!no_rules)
       ps = PSReference(dsl->stdis);

//looking through the dist_name_list
     for(e=dsl->stdis->expr(0); e; e=e->rhs()) {
        das = (e->lhs())->symbol(); // distribute array symbol
        	/*  if(dsl->stdis->expr(2) && !IS_DUMMY(das))
	            Error("'%s' is not a dummy argument", das->identifier(),dsl->stdis);
                 */
        int is_global_template_in_procedure =  IS_TEMPLATE(das) && IN_COMMON(das) && !IN_MAIN_PROGRAM;
        if(!dsl->stdis->expr(1) && !is_global_template_in_procedure)
	  SYMB_ATTR(das->thesymb)= SYMB_ATTR(das->thesymb) | POSTPONE_BIT;
                /*if(IS_POINTER(das) && (das->attributes() & DIMENSION_BIT))
	             Error("Distributee '%s' with POINTER attribute is not a scalar variable", das->identifier(),dsl->stdis);
                 */
                
        // creating LibDVM function calls for distributed array and its Align Tree

        //GenDistArray(das,idis,dis_rules,ps,dsl->stdis);    
        GenDistArray(das,idis,distr_rule_list,ps,dsl->stdis);      
     }

   }
  
  //Looking through the Align Tree List
   for(root=pal; root; root=root->next) {
     if(!( root->symb->attributes() & DISTRIBUTE_BIT) && !( root->symb->attributes() & ALIGN_BIT) && !( root->symb->attributes() & INHERIT_BIT) && !( root->symb->attributes() & POSTPONE_BIT))
        Err_g("Alignment tree root '%s' is not distributed", root->symb->identifier(),92);
     if(( root->symb->attributes() & POSTPONE_BIT) && !( root->symb->attributes() & DISTRIBUTE_BIT) && CURRENT_SCOPE(root->symb) ) {
        GenAlignArray(root,NULL,0,NULL,0);
        AlignTree(root);
     }
     if( (root->symb->attributes() & INHERIT_BIT) || !CURRENT_SCOPE(root->symb) ) 
        AlignTree(root);
     
   }

  if(debug_regim && registration)  {       // registrating arrays for debugger
    LINE_NUMBER_BEFORE(func,where);   //(first_exec,where);
    ArrayRegistration();
  }
// testing procedure
//  if(dvm_debug && dbg_if_regim>1 && ((func->variant() == PROC_HEDR) || (func->variant() == FUNC_HEDR)) && !pstmt)// && !hasParallelDir(first_exec,func))
//    copy_proc=1;
  for(;pstmt; pstmt= pstmt->next)
     Extract_Stmt(pstmt->st);// extracting  DVM Specification Directives

  if(!loc_distr && !task_symb && !proc_symb && !IN_MAIN_PROGRAM) {
                                       //there are no local distributed arrays 
                                       //no task array , no asinc and no processor array
    if(begin_block){
      begbl->extractStmt(); //extract dvmh_scope_start /*begbl()*/ call
      begin_block = 0;
      fmask[SCOPE_START] = 0;  //fmask[BEGBL] = 0; 
    } 
    if(!loc_templ_symb && stam) {   
      stam->lexNext()->extractStmt();   //extract getps() call 
      stam->extractStmt();              //extract getam() call
      fmask[GETAM] = 0; fmask[GETVM] = 0; 
    }  
  }

  if(begin_block && !IN_MAIN_PROGRAM) {    
      LINE_NUMBER_BEFORE(first_exec,begbl);
  }
 
  if(lab_exec)
      first_exec-> setLabel(*lab_exec);  //restore label of first executable statement

  last_dvm_entry = first_exec->lexPrev();

  if(copy_proc)  
     InsertCopyOfExecPartOfProcedure(copy_proc);  

//**********************************************************************
//           Executable Directives Processing 
//**********************************************************************

EXEC_PART_:
  for (i=0; i<Ntp; i++)
    buf_use[i] = rmbuf_size[i]= 0; 
  IOstat = NULL;
  inparloop = 0; 
  inasynchr = 0;
  own_exe = 0;
  redvar_list = NULL;
  rma =NULL;
  in_task_region = 0;
  task_ind = 0;
  in_task=0;
  task_lab = NULL;
  dvm_ar= NULL;

  if(IN_MODULE) { 
    if(!mod_proc && first_exec->variant() == CONTAINS_STMT) 
         end_of_unit = has_contains = first_exec;
           //else if(mod_proc) 
           //   mod_proc = MayBeDeleteModuleProc(mod_proc,end_of_unit); 
    goto END_;
  }

//follow the executable statements in lexical order until last statement
// of the function
  for(stmt=first_exec; stmt ; stmt=stmt->lexNext()) {
    cur_st = stmt;     //printf("executable statement %d %s\n",stmt->lineNumber(),stmt->fileName());
 
    while(rma && rma->rmout == stmt)//current statement is out of scope REMOTE_ACCESS directive
       RemoteAccessEnd();

    if(isACCdirective(stmt))     /*ACC*/
    { pstmt = addToStmtList(pstmt, stmt);    
      stmt = ACC_Directive(stmt);            
      continue;
    }  

    if(IN_COMPUTE_REGION && IN_STATEMENT_GROUP(stmt)) /*ACC*/
    {  stmt = ACC_CreateStatementGroup(stmt); 
       continue;
    }
    switch(stmt->variant()) {
       case CONTROL_END:
            if(stmt == last) {
              EndOfProgramUnit(stmt, func, begin_block);
              goto END_;            
            }
            if(inasynchr) //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
            break;

       case CONTAINS_STMT:
            has_contains = end_of_unit = stmt;
            EndOfProgramUnit(stmt, func, begin_block);
            goto END_;
            break;
       case RETURN_STAT:
            EndOfProgramUnit(stmt, func, begin_block);
            if(dvm_debug || perf_analysis ) 
            { // RETURN statement is added to list for debugging (exit the loop)       
              goto_list = addToStmtList(goto_list, stmt);
              if(begin_block)
                AddDebugGotoAttribute(stmt,stmt->lexPrev()->lexPrev()); //to insert statements for debugging before call endbl() inserted before RETURN
            } 
            if(stmt->lexNext() == last) 
                goto END_;  
            if(stmt->lexNext()->variant() == CONTAINS_STMT){ 
                has_contains = end_of_unit = stmt->lexNext();                
                goto END_;  
            }
            break;
       case STOP_STAT:
            if(begin_block && func->variant() != PROG_HEDR)
               EndBlock_H(stmt); 
            if(stmt->expr(0)){
               SgStatement *print_st;
               InsertNewStatementBefore(print_st=PrintStat(stmt->expr(0)),stmt);
               ReplaceByIfStmt(print_st);
            } 
            RTLExit(stmt);
            if(stmt->lexNext() == last)
               goto END_;
            break;
       case PAUSE_NODE: 
            err("PAUSE statement is not permitted in FDVM", 93,stmt); 
            break; 
       case EXIT_STMT:
           if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
            //if(dvm_debug || perf_analysis ) 
              // EXIT statement is added to list for debugging (exit the loop)       
              //goto_list = addToStmtList(goto_list, stmt);
            break;
       case ENTRY_STAT: 
	     if(distr) {
               warn("ENTRY of program unit distributed arrays are in",169,stmt); 
	      // err("ENTRY statement is not permitted in FDVM", stmt);
             } 
             GoRoundEntry(stmt);
              //BeginBlockForEntry(stmt);
             entry_list=addToStmtList(entry_list,stmt);
             
            break;  

       case SWITCH_NODE:           // SELECT CASE ...
       case ARITHIF_NODE:          // Arithmetical IF
       case IF_NODE:               // IF... THEN
       case WHILE_NODE:            // DO WHILE (...) 
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
            if(HPF_program && !inparloop){
              first_time = 1;
              SearchDistArrayRef(stmt->expr(0),stmt);
              cur_st = stmt;
            }
	    if(dvm_debug)
              DebugExpression(stmt->expr(0),stmt); 
            else          
              ChangeDistArrayRef(stmt->expr(0));

            if((dvm_debug || perf_analysis) && stmt->variant()==ARITHIF_NODE ) 
              goto_list = addToStmtList(goto_list, stmt);          

            break;
       
       case CASE_NODE:             // CASE ...
       case ELSEIF_NODE:           // ELSE IF...
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
            if(HPF_program && !inparloop){
              first_time = 1;
              SearchDistArrayRef(stmt->expr(0),stmt);
              cur_st = stmt;
            }
            ChangeDistArrayRef(stmt->expr(0));
            break; 

       case LOGIF_NODE:            // Logical IF 
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
            if( !stmt->lineNumber()) {//inserted statement
              stmt = stmt->lexNext();
              break; 
            } 
            if(HPF_program) {
              if(!inparloop){ //outside the range of parallel loop
                ReplaceContext(stmt);
                first_time = 1;
                SearchDistArrayRef(stmt->expr(0),stmt); //look for distributed array elements
                cur_st = stmt;
              } else         //inside the range of parallel loop
                IsLIFReductionOp(stmt, indep_st->expr(0) ? indep_st->expr(0)->lhs() : indep_st->expr(0));                                           //look for reduction operator
            }
	    if(dvm_debug) {
              ReplaceContext(stmt);
              DebugExpression(stmt->expr(0),stmt);	
            } else {
              ChangeDistArrayRef(stmt->expr(0));
              if(perf_analysis && IsGoToStatement(stmt->lexNext()))
                ReplaceContext(stmt);
            }
            continue; // to next statement


       case FORALL_STAT:          // FORALL statement
            {SgSymbol *do_var; 
	    SgExpression *el,*ei,*etriplet,*ec;
            el=stmt->expr(0); //list of loop indexes
            for(el= stmt->expr(0); el; el=el->rhs()){
               ei=el->lhs(); //expression: i=l:u:s
               etriplet= ei->lhs();//l:u:s
               do_var=ei->symbol();//do-variable
               //printf("%s=",do_var->identifier());
               
               //etriplet->unparsestdout();
               //printf("  ");
             }
            ec=stmt->expr(1); // conditional expression
            //ec->unparsestdout();
         
            }
            stmt=stmt->lexNext();//  statement that is a part of FORALL statement         
            break;
            // continue; 
       case GOTO_NODE:          // GO TO
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
            if((dvm_debug || perf_analysis) && stmt->lineNumber() ) 
              goto_list = addToStmtList(goto_list, stmt);          
            break;

       case COMGOTO_NODE:          // Computed GO TO
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
            if(HPF_program && !inparloop){
               ReplaceContext(stmt);
               first_time = 1;
               SearchDistArrayRef(stmt->expr(1),stmt);
               cur_st = stmt;
            }
            if(dvm_debug) {
              ReplaceContext(stmt);
              DebugExpression(stmt->expr(1),stmt);
            } else 
            {  ChangeDistArrayRef(stmt->expr(1));
               if (perf_analysis ) 
                 ReplaceContext(stmt); 
            }
            if(dvm_debug || perf_analysis ) 
              goto_list = addToStmtList(goto_list, stmt);          
            break;

       case ASSIGN_STAT:             // Assign statement  
	  { SgSymbol *s;
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
              if(ArrayAssignment(stmt)) { //Fortran 90
                AsynchronousCopy(stmt);                 
              } 
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              stmt=cur_st;
              break;
            }
            if( !stmt->lineNumber()) //inserted debug statement
                break;   

           if((s=stmt->expr(0)->symbol()) && IS_POINTER(s)){ // left part variable is POINTER
             if(isSgFunctionCallExp(stmt->expr(1)) && !strcmp(stmt->expr(1)->symbol()->identifier(),"allocate")){
               if(inparloop)
                 err("Illegal statement in the range of parallel loop", 94, stmt);
               AllocateArray(stmt,distr); 
               if(stmt != cur_st){//stmt == cur_st in error situation
                 Extract_Stmt(stmt);
                 stmt=cur_st;
               } 

             } else if( (isSgVarRefExp(stmt->expr(1)) || isSgArrayRefExp(stmt->expr(1))) && stmt->expr(1)->symbol() && IS_POINTER(stmt->expr(1)->symbol())) {
               AssignPointer(stmt);
               if(stmt != cur_st){
                 Extract_Stmt(stmt);
                 stmt=cur_st;
               } 

             } else 
               err("Only a value of ALLOCATE function or other POINTER may be assigned to a POINTER",95,stmt);
      
             break;
	   } 
            if(HPF_program){
              if(!inparloop){ //outside the range of parallel loop
                ReplaceContext(stmt);
                first_time = 1;
                SearchDistArrayRef(stmt->expr(1),stmt); //look for distributed array elements
                cur_st = stmt;
              } else //inside the range of parallel loop
                IsReductionOp(stmt,indep_st->expr(0) ? indep_st->expr(0)->lhs() : indep_st->expr(0));                                               //look for reduction operator
            } 
                     /*  if(own_exe) { // "owner executes" rule
                           ReplaceContext(stmt);
                           ReplaceAssignByIf(stmt);
                         } else */
            if(!inparloop && isDistObject(stmt->expr(0))){
              if( !isSgArrayType(stmt->expr(0)->type())){ //array element
                if(all_replicated == 0){ // not all arrays in procedure are replicated
                   ReplaceContext(stmt);
                   
                   
                   if(!in_on) {
                     LINE_NUMBER_BEFORE(stmt,stmt);
                     ReplaceAssignByIf(stmt);
                   }
                   //own_exe = 1;
                   if(warn_all)
                     warn("Owner-computes rule", 139, stmt);
                    //warn("Assignment of distributed array element outside the range of parallel loop: owner executes", stmt);  
                } 
                own_exe = 1;
              }
              else { //array section
                if(DistrArrayAssign(stmt)) { 
                  pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
                  stmt=cur_st;
                  break;
                }
              }
     	    }

            if(!inparloop && AssignDistrArray(stmt)) {
                  pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
                  stmt=cur_st;
                  break;
                 }                

	    // if(inparloop && !TestLeftPart(new_red_var_list, stmt->expr(0)))
	    //  Error("Illegal assignment in the range of parallel loop",stmt); 

                       
            if(dvm_debug) { 
              SgStatement *where_st, *stmt1, *stparent;
              where_st=stmt->lexNext();  
              ReplaceContext(stmt);
              DebugAssignStatement(stmt);

              if(own_exe && !in_on) { //declaring omitted block
                 where_st = where_st->lexPrev();
                 stmt1 = dbg_if_regim ?  CreateIfThenConstr(DebugIfCondition(),D_Skpbl())  : D_Skpbl();
                 stparent = (all_replicated == 0) ? stmt->controlParent()->controlParent() : stmt->controlParent();
                 InsertNewStatementAfter(stmt1,where_st,stparent);
              } 
              stmt = cur_st;   
            } else { 
              ChangeDistArrayRef_Left(stmt->expr(0));   // left part
              ChangeDistArrayRef(stmt->expr(1));   // right part
            }
            own_exe =0;
          }
            break;

       case PROC_STAT:             // CALL            
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	       pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
               break;
            }
            if( !stmt->lineNumber()) //inserted debug statement
               break; 
            if(HPF_program && !inparloop){
               ReplaceContext(stmt);
               first_time = 1;
               SearchDistArrayRef(stmt->expr(0),stmt);
               cur_st = stmt;
            }
            if(dvm_debug){ 
               ReplaceContext(stmt);
               DebugExpression(NULL,stmt);
            } else {
               // looking through the arguments list
               SgExpression * el;
               for(el=stmt->expr(0); el; el=el->rhs())            
                  ChangeArg_DistArrayRef(el);   // argument
            }            
            break;
       case ALLOCATE_STMT: 
            ALLOCATEf90_arrays(stmt,distr);
            if(!stmt->expr(0)){
               cur_st=stmt->lexPrev();
               Extract_Stmt(stmt);
               stmt=cur_st;  
            } else 
            {  cur_st = stmt;
               if(debug_regim) 
                  AllocatableArrayRegistration(stmt);
               EnterDataRegionForAllocated(stmt); /*ACC*/
               stmt=cur_st;
            }            
            break;
       case DEALLOCATE_STMT:
            DEALLOCATEf90_arrays(stmt);
            if(!stmt->expr(0)){
               Extract_Stmt(stmt);
               stmt=cur_st;  
            }
            break;
       case DVM_PARALLEL_ON_DIR:
	    if(inparloop){
              err("Nested PARALLEL directives are not permitted", 96, stmt); 
              break;
            }                     
                          //!!!acc printf("parallel on %d region %d\n",stmt->lineNumber(), cur_region);

            par_do = stmt->lexNext();// first DO statement of parallel loop

            while(isOmpDir (par_do))      // || isACCdirective(par_do) 
            { cur_st = par_do;
              par_do=par_do->lexNext();               
            }  
            if(!isSgForStmt(par_do)) {
              err("PARALLEL directive must be followed by DO statement",97,stmt); //directive is ignored
              break;
            } 
            inparloop = 1;
            if(!ParallelLoop(stmt))// error in PARALLEL directive
                 inparloop = 0;   
            
            pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements       
                                 //Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;
                   // setting stmt on last DO statement of parallel loop nest
            break;

     case HPF_INDEPENDENT_DIR:
            if(inparloop){
              //illegal nested INDEPENDENT directive is ignored
              pstmt = addToStmtList(pstmt, stmt); //including the HPF directive to list
              break;
            }  
            indep_st = stmt; // INDEPENDENT directive 
            par_do = stmt->lexNext();// first DO statement of parallel loop 
            if(!isSgForStmt(par_do)) {
              err("INDEPENDENT directive must be followed by DO statement",97,stmt); 
                                                                     //directive is ignored
              break;
            } 
            inparloop = 1;
            IEXLoopAnalyse(func);
            if(!IndependentLoop(stmt))// error in INDEPENDENT directive
                 inparloop = 0;   
	                     
	         
            //including the HPF directive to list
            pstmt = addToStmtList(pstmt, stmt); 
            stmt = cur_st; // setting stmt on last DO statement of parallel loop nest
            break;  

     case DVM_SHADOW_GROUP_DIR:
           {
            SgSymbol *s;
            SgExpression *gref;
            //int *index = new int;
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            s = stmt->symbol();
            AddToGroupNameList (s);
            gref = new SgVarRefExp(s);
            CreateBoundGroup(gref);  
            //s -> addAttribute(SHADOW_GROUP_IND, (void *) index, sizeof(int)); 
            ShadowList(stmt->expr(0), stmt, gref);
           }
            Extract_Stmt(stmt); // extracting DVM-directive
            stmt = cur_st;//setting stmt on last inserted statement 
            break;

       case DVM_SHADOW_START_DIR:
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM
            if(ACC_program)      /*ACC*/
              // generating call statement ( in and out compute region):
              //  call dvmh_shadow_renew( BoundGroupRef)              
              doCallAfter(ShadowRenew_H(new SgVarRefExp(stmt->symbol()) ));  
           
            doCallAfter(StartBound(new SgVarRefExp(stmt->symbol())));        
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;//setting stmt on  inserted statement 
            break;

       case DVM_SHADOW_WAIT_DIR:
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            doCallAfter(WaitBound(new SgVarRefExp(stmt->symbol()))); 
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;//setting stmt on  inserted statement 
            break;
 
       case DVM_REDUCTION_START_DIR:
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            doCallAfter(StartRed(new SgVarRefExp(stmt->symbol())));        
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;//setting stmt on  inserted statement 
            break;

       case DVM_REDUCTION_WAIT_DIR:
	   {SgExpression *rg = new SgVarRefExp(stmt->symbol());
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            doCallAfter(WaitRed(rg)); 
            if(dvm_debug)             
              doCallAfter( D_CalcRG(DebReductionGroup( rg->symbol())));
            
            doCallAfter(DeleteObject_H(rg)); 
            doAssignTo_After(rg, new SgValueExp(0));
            if(debug_regim)
              doCallAfter( D_DelRG(DebReductionGroup( rg->symbol())));              
	   }
              //Extract_Stmt(stmt); // extracting DVM-directive
            wait_list = addToStmtList(wait_list, stmt); 
            pstmt = addToStmtList(pstmt, stmt);            
            stmt = cur_st;//setting stmt on last inserted statement                   
            break;


       case DVM_CONSISTENT_START_DIR:
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            doAssignStmtAfter(StartConsGroup(new SgVarRefExp(stmt->symbol())));        
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;//setting stmt on  inserted statement 
            break;

       case DVM_CONSISTENT_WAIT_DIR:
	   {SgExpression *rg = new SgVarRefExp(stmt->symbol());
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            doAssignStmtAfter(WaitConsGroup(rg)); 
            //if(dvm_debug)             
              //doAssignStmtAfter( D_CalcRG(DebReductionGroup( rg->symbol())));
            if(cur_st->controlParent()->variant() != PROG_HEDR){
              doCallAfter(DeleteObject_H(rg)); 
              doAssignTo_After(rg, new SgValueExp(0));
            }  
            //if(debug_regim)
              //doAssignStmtAfter( D_DelRG(DebReductionGroup( rg->symbol())));              
	   }
            wait_list = addToStmtList(wait_list, stmt); 
            pstmt = addToStmtList(pstmt, stmt);            
            stmt = cur_st;//setting stmt on last inserted statement                   
            break;

       case DVM_REMOTE_ACCESS_DIR:
	    if(inparloop) {
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            }
            ReplaceContext(stmt->lexNext());
            switch(stmt->lexNext()->variant()) {
	            case LOGIF_NODE:
                        rmout = stmt->lexNext()->lexNext()->lexNext(); 
                        break;
	            case SWITCH_NODE:
                        rmout = stmt->lexNext()->lastNodeOfStmt()->lexNext();
                        break;
	            case IF_NODE:
                        rmout = lastStmtOfIf(stmt->lexNext())->lexNext();
                        break;
	            case CASE_NODE:
                    case ELSEIF_NODE:          
                        err("Misplaced REMOTE_ACCESS directive", 99,stmt);
                        rmout = stmt->lexNext()->lexNext();
                        break;
                    case FOR_NODE:
                        rmout = lastStmtOfDo(stmt->lexNext())->lexNext();
                        break;
                    case WHILE_NODE:
                        rmout = lastStmtOfDo(stmt->lexNext())->lexNext();
                        break;
		    case DVM_PARALLEL_ON_DIR:
                        rmout = lastStmtOfDo(stmt->lexNext()->lexNext())->lexNext();
                        break;
	            default:
                        rmout = stmt->lexNext()->lexNext();
                        break;
              }
            //adding new element to remote_access directive/clause list
            AddRemoteAccess(stmt->expr(0),rmout); 
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing 
	    // looking through the remote variable list
	                    // RemoteVariableList1(stmt->symbol(),stmt->expr(0),stmt);
            RemoteVariableList(stmt->symbol(),stmt->expr(0),stmt);
            
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;        
            break;
 
      case DVM_NEW_VALUE_DIR:
            if((stmt->lexNext()->variant()==DVM_REDISTRIBUTE_DIR) ||                           (stmt->lexNext()->variant()==DVM_REALIGN_DIR))
               st_newv = stmt;
            else
               err("NEW_VALUE directive must be followed by REDISTRIBUTE or REALIGN directive", 146,stmt);
            break; 
 
       case DVM_REALIGN_DIR:           
	    if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt);
              st_newv = 0;  
              break;
            } else {
            int iaxis; // AxisArray index 
            int nr,new_sign,ia; 
            SgSymbol *als,*tgs;

            where = stmt;  //for inserting before current directive                   
            iaxis = ndvm;
            ia = 0;
            //sta = NULL;
	    // new_val = isSgExprListExp(stmt->expr(2)) ? (stmt->expr(2)->rhs()->lhs()) :                                                               (SgExpression *) NULL;
 
            tgs = isSgExprListExp(stmt->expr(2)) ? (stmt->expr(2))->lhs()->symbol() :                                                            (stmt->expr(2))->symbol();
            if(!HEADER(tgs))    
               Error("'%s' isn't distributed array", tgs->identifier(), 72,stmt);
           
            new_sign = 0;
            if(st_newv)
                  new_sign = 1; // NEW_VALUE without variable list
            //looking through the alignee_list
            for(e=stmt->expr(0); e; e=e->rhs()) {
               als = (e->lhs())->symbol(); // realigned array symbol
                           //nr = doAlignRule(als, stmt, ia);
               SgExpression *align_rule_list = doAlignRules(als, stmt, ia, nr);
	       /* 
                *if(sta) // is not first list element
                * for(i=0;i<2*nr;i++) 
                *     Extract_Stmt(sta->lexNext());//extracting axis and coeff 
                *                                  //assignment statements
		*/
          
	     /*   
              * if(new_val) 
              *   if(!new_val->lhs()) // NEW_VALUE without variable list
              *       new_sign = 1;
              *   else
              *     for(env=new_val->lhs(); env; env=env->rhs()) {
              *       symb=env->lhs()->symbol();
              *       if(symb==als) {
              *          new_sign = 1;
              *          break;
              *       }   
              *   }
	      */  
              LINE_NUMBER_AFTER(stmt,cur_st);// doAssignStmt in doAlignRule resets cur_st
	      //all inserted statements for REALIGN directive appear before it         
              RealignArray(als,tgs,iaxis,nr,align_rule_list,new_sign,stmt);
                     // doAssignStmt(RealignArr(DistObjectRef(als),DistObjectRef(stmt->expr(2)->symbol()),iaxis,iaxis+nr,iaxis+2*nr,new_sign));
              
              ia = iaxis;
              
            }
            SET_DVM(iaxis);
            
            }

            Extract_Stmt(stmt); // extracting REALIGN directive 
            if(st_newv)          
              Extract_Stmt(st_newv); //extracting preceeding NEW_VALUE directive    
            stmt = cur_st;//setting stmt on last inserted statement        
            st_newv = 0;              
            break;
	    
       case DVM_REDISTRIBUTE_DIR:
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);    
            else {
            int idis; // DisRuleArray index 
            int new_sign,isave; 
            SgSymbol *das;
            SgExpression *target,*ps;
	       // new_val = hasNewValueClause(stmt);
            nproc = 0; 
            isave = ndvm;
            where = stmt; //for inserting before current directive 
            LINE_NUMBER_BEFORE(stmt,stmt);
            SgExpression *distr_rule_list = doDisRules(stmt,0,idis);  
            target = hasOntoClause(stmt);
            if ( target ) { //is there ONTO_clause   
              nproc=RankOfSection(target); // rank of Processors
              if(nblock && nproc && nblock > nproc)
                Error("The number of BLOCK/GENBLOCK elements of dist-format-list is greater than the rank of PROCESSORS '%s'", target->symbol()->identifier(),90,stmt);
            }
            ps = PSReference(stmt);
              //LINE_NUMBER_AFTER(stmt,cur_st);// doAssignStmt in doDisRuleArrays resets cur_st
	      //all inserted statements for REDISTRIBUTE directive appear before it   
            new_sign = 0;
            if(st_newv)
                  new_sign = 1; // NEW_VALUE without variable list    
            //looking through the dist_name_list
            for(e=stmt->expr(0); e; e=e->rhs()) {
               das = (e->lhs())->symbol(); // distribute array symbol
               // for debug
               //printf("%s\n ", das->identifier());
               //
               //new_sign = 0;
               //if(new_val) 
               //  if(!new_val->lhs()) // NEW_VALUE without variable list
               //      new_sign = 1;
               //  else
               //    for(env=new_val->lhs(); env; env=env->rhs()) {
               //      symb=env->lhs()->symbol();
               //      if(symb==das) {
               //         new_sign = 1;
               //         break;
               //      }   
               //  }
               // if(Rank(das)!=ndis)
               //   Error("Length of dist-format-list is not equal the rank of %s  ", das->identifier(),stmt);
         
               // creating LibDVM function calls for redistributing array
               
               RedistributeArray(das,idis,distr_rule_list,ps,new_sign,e->lhs(),stmt);
            
            }
            
            SET_DVM(isave);
            Extract_Stmt(stmt); // extracting REDISTRIBUTE directive 
            if(st_newv)          
              Extract_Stmt(st_newv); //extracting preceeding NEW_VALUE directive    
            stmt = cur_st;//setting stmt on last inserted statement
               
            }        
            st_newv = 0;  
	    break;

       case DVM_LOCALIZE_DIR:
          { 
            int iaxis;
            int rank=Rank(stmt->expr(1)->symbol());
            SgExpression *ei;    
            if(!INTERFACE_RTS2)
            {
               warn("LOCALIZE directive is ignored, -Opl2 option should be specified",621,stmt);
               pstmt = addToStmtList(pstmt, stmt);
               break;
            }
            LINE_NUMBER_AFTER(stmt,stmt);
            for(ei=stmt->expr(1)->lhs(),iaxis=rank; ei; ei=ei->rhs(),iaxis--)
               if(ei->lhs()->variant() == DDOT)
                  break; 
            doCallAfter(IndirectLocalize(stmt->expr(0),HeaderRef(stmt->expr(1)->symbol()),iaxis));
            Extract_Stmt(stmt);
            stmt = cur_st;
            break;
            }
           
       case DVM_SHADOW_ADD_DIR:
            if(!INTERFACE_RTS2)
            {
               warn("SHADOW_ADD directive is ignored, -Opl2 option should be specified",621,stmt);
               pstmt = addToStmtList(pstmt, stmt);
               break;
            }
            LINE_NUMBER_AFTER(stmt,stmt);
            Shadow_Add_Directive(stmt);
            Extract_Stmt(stmt);
            stmt = cur_st;
            break;
            
//Debugging Directive
      case DVM_INTERVAL_DIR:
	  if (perf_analysis > 1){
            //generating call to 'binter' function of performance analizer
	    // (begin of user interval)
            
            LINE_NUMBER_AFTER(stmt,stmt);
            InsertNewStatementAfter(St_Binter(OpenInterval(stmt),Value_F95(stmt->expr(0))), cur_st,cur_st->controlParent());       
          }
          pstmt = addToStmtList(pstmt, stmt);  //including the DVM  directive to list
          stmt = cur_st;
          break;

      case DVM_ENDINTERVAL_DIR:
          if (perf_analysis > 1){
            //generating call to 'einter' function of performance analizer
	    // (end of user interval)
            
            if(!St_frag){
              err("Unmatched directive",182,stmt);
              break;
            }
            if(St_frag && St_frag->begin_st &&  (St_frag->begin_st->controlParent() != stmt->controlParent()))
                err("Misplaced directive",103,stmt); //interval must be a block
	    LINE_NUMBER_AFTER(stmt,stmt);
            InsertNewStatementAfter(St_Einter(INTERVAL_NUMBER,INTERVAL_LINE), cur_st, stmt->controlParent());
            CloseInterval();
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;
          }
          else
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
          break;

      case DVM_EXIT_INTERVAL_DIR:
          if (perf_analysis > 1){
            //generating calls to 'einter' function of performance analizer
	    // (exit from user intervals)
            
            if(!St_frag){
              err("Misplaced directive",103,stmt);
              break;
            }
            ExitInterval(stmt);
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;
          }
          else
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
          break;

       case DVM_MAP_DIR:
	 {  int ind;
            SgExpression *ps,*am,*index;
            SgSymbol *s_tsk;
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            }
            LINE_NUMBER_BEFORE(stmt,stmt);
            where = stmt; //for inserting before current directive 
            ind = ndvm;
            s_tsk = stmt->expr(0)->symbol();
            if(!stmt->expr(2))  // MAP ... ONTO ...
            { index = Calculate(stmt->expr(0)->lhs()->lhs());
              if(!isSgValueExp(index) && !isSgVarRefExp(index))
              {  doAssignStmt(index);
                 index = DVM000(ind);
              }
              PSReference(stmt);
              ps =  new SgArrayRefExp(*s_tsk,*new SgValueExp(1),*index);
              cur_st->setExpression(0,*ps);
              am = new SgArrayRefExp(*s_tsk,*new SgValueExp(2),*index);
              doCallStmt(MapAM(am,ps));
              SET_DVM(ind);
            } else            //  MAP ... BY ...
            { SgExpression *section, *ev_tsk, *e_count;
              SgSymbol *s_ind;
              int ips,i_size, i_lps, ic;
              SgStatement *dost;
              s_tsk->addAttribute(TSK_AUTO, (void*) 1, 0);
              section = stmt->expr(0)->lhs();
              i_size = ndvm;
              doAssignStmt(GetSize(ParentPS(),0));
              // pr = psview(PSRef, rank, SizeArray, StaticSign)
              ips = ndvm;              
              doAssignStmt(Reconf(DVM000(i_size), 1, 0));
              s_ind = loop_var[0]; //TASK_IND_VAR(s_tsk);  
              ev_tsk = new SgVarRefExp(s_ind);
              ic = ndvm;
              e_count = CountOfTasks(stmt);
              doAssignStmt(e_count);           
              TestParamType(stmt);               
              doCallStmt(MapTasks(DVM000(ic),DVM000(i_size),new SgVarRefExp(stmt->expr(2)->symbol()),new SgVarRefExp(TASK_LPS_ARRAY(s_tsk)),new SgVarRefExp(TASK_HPS_ARRAY(s_tsk)),new SgVarRefExp(TASK_RENUM_ARRAY(s_tsk)))); 
              ps =  new SgArrayRefExp(*s_tsk,*new SgValueExp(1),*ev_tsk);
              am =  new SgArrayRefExp(*s_tsk,*new SgValueExp(2),*ev_tsk);
              dost = new SgForStmt(*s_ind,*new SgValueExp(1),*e_count,*MapAM(am,ps));
              where->insertStmtBefore(*dost);   
              cur_st = dost;
              i_lps = ndvm;
              doAssignStmtAfter( &(*new SgArrayRefExp(*TASK_LPS_ARRAY(s_tsk),*ev_tsk) - *new SgValueExp(1)) );
              doAssignStmtAfter( &(*new SgArrayRefExp(*TASK_HPS_ARRAY(s_tsk),*ev_tsk) - *new SgValueExp(1)) );
              doAssignTo_After(ps, CrtPS(DVM000(ips), i_lps, i_lps+1, 0) );
              cur_st = dost->lastNodeOfStmt(); 
              SET_DVM(i_size);  
            }
            Extract_Stmt(stmt); // extracting DVM-directive 
            stmt = cur_st;  
	 }
            break; 

       case DVM_TASK_REGION_DIR:
	    if(in_task_region++) {
              err("Nested TASK_REGION  are not permitted", 100,stmt);
              break;
            }  
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            }    
            if((stmt->lexNext()->variant() != DVM_ON_DIR) && (stmt->lexNext()->variant() != DVM_END_TASK_REGION_DIR) &&  (stmt->lexNext()->variant() != DVM_PARALLEL_TASK_DIR))
              err("Statement is outside of on-block",101,stmt->lexNext()); 
            LINE_NUMBER_AFTER(stmt,stmt);
            //if(stmt->expr(0))
	     Reduction_Task_Region(stmt);
            //if(stmt->expr(1))
	     Consistent_Task_Region(stmt);
            task_region_st = stmt;
            task_region_parent = stmt->controlParent(); //to test nesting blocks
            task_lab = (SgLabel *) NULL;
            task_ind = ndvm++; 
            if(dvm_debug)
                DebugTaskRegion(stmt);                 
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);
            stmt = cur_st;      
            break;

       case DVM_END_TASK_REGION_DIR:
	    if(!in_task_region--) {
              err("No matching TASK_REGION", 102,stmt); 
              break;
            }             
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            } 
            if(stmt->controlParent() != task_region_parent) //test of nesting blocks
              err("Misplaced directive",103,stmt);
            LINE_NUMBER_AFTER(stmt,stmt);
            if(dvm_debug)
                CloseTaskRegion(task_region_st,stmt); 
            EndReduction_Task_Region(stmt);
            EndConsistent_Task_Region(stmt);
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
            stmt = cur_st; 
            break;
   
       case DVM_ON_DIR: 
            if(in_task++) {
              err("Nested ON-blocks are not permitted", 104,stmt); 
              break;
            }
    
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop",98, stmt); 
              break;
            } 

            if(!isSgArrayRefExp(stmt->expr(0)) || !stmt->expr(0)->symbol()) {
              err("Syntax error",14, stmt); 
              break;
            } 

            on_stmt = stmt;                                                            
            if(HEADER(stmt->expr(0)->symbol())) // ON <dvm-array-element> construct
            { 
              LINE_NUMBER_BEFORE(stmt,stmt);
              in_on++;  
              break;
            }
            // ON <task-array-element> construct
            if(!in_task_region)
              err("ON directive is outside of the task region", 105,stmt); 
            if( stmt->expr(0)->symbol()->attributes() & TASK_BIT)
            {
	      LINE_NUMBER_AFTER(stmt,stmt); 
              task_lab = GetLabel();    
              StartTask(stmt);
              pstmt = addToStmtList(pstmt, stmt); 
              stmt = cur_st;
            } 
            else 
              Error("'%s' is not task array", stmt->expr(0)->symbol()->identifier(),77,stmt);
            break;

       case DVM_END_ON_DIR:
            if(!in_task) {
              err("No matching ON directive", 106,stmt); 
              break;
            } else
              in_task--;
            if(in_task)  //nested ON constructs
               break;  
            
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            }  
            if(on_stmt && stmt->controlParent() != on_stmt->controlParent())
              err("Misplaced directive",103,stmt);
            if(in_on) // end of ON <dvm-array-element> construct
            {
               ReplaceOnByIf(on_stmt,stmt);
               Extract_Stmt(on_stmt); // extracting DVM-directive (ON) 
               in_on--; 

               if(dvm_debug)
               {
                  SgStatement *std = dbg_if_regim ?  CreateIfThenConstr(DebugIfCondition(),D_Skpbl())  : D_Skpbl();
                  InsertNewStatementAfter(std,stmt,stmt->controlParent());
                  cur_st = lastStmtOf(std);     
               }          
               Extract_Stmt(stmt);  // extracting DVM-directive (END_ON)
               stmt = cur_st;
               break;
            }
            //end of ON <task-array-element> construct
            if((stmt->lexNext()->variant() != DVM_ON_DIR) && (stmt->lexNext()->variant() != DVM_END_TASK_REGION_DIR))
              err("Statement is outside of on-block",101,stmt->lexNext());  
            LINE_NUMBER_AFTER(stmt,stmt);
            doCallAfter(StopAM());
            InsertNewStatementAfter(new SgStatement(CONT_STAT),cur_st,stmt->controlParent());
            if(task_lab)
              cur_st->setLabel(*task_lab);
            FREE_DVM(1);   
            Extract_Stmt(stmt);// extracting DVM-directive (END_ON)   
            stmt = cur_st;
            break;
  
       case DVM_RESET_DIR:
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            } 
            if(options.isOn(NO_REMOTE)) {
              pstmt = addToStmtList(pstmt, stmt);
              break;
            }
            LINE_NUMBER_AFTER(stmt,stmt);    
            doCallAfter(DeleteObject_H(GROUP_REF(stmt->symbol(),1)));               
            doAssignTo_After(GROUP_REF(stmt->symbol(),1),new SgValueExp(0));
            Extract_Stmt(stmt);// extracting DVM-directive   
            stmt = cur_st;
            break;

       case DVM_PREFETCH_DIR: 
            if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            } 
            if(options.isOn(NO_REMOTE)) {
              pstmt = addToStmtList(pstmt, stmt);
              break;
            }
            {SgStatement *if_st,*endif_st;
            pref_st = addToStmtList(pref_st, stmt);//add to list of PREFETCH directive
            if_st = doIfThenConstrForPrefetch(stmt);
            cur_st = if_st->lexNext()->lexNext();//ELSE IF
            endif_st = cur_st->lexNext()->lexNext(); //END IF
            doAssignStmtAfter((stmt->symbol()->attributes() & INDIRECT_BIT) ?  LoadIG(stmt->symbol()) : LoadBG(GROUP_REF(stmt->symbol(),1)));            
            doAssignTo_After(GROUP_REF(stmt->symbol(),3),new SgValueExp(1));
            cur_st = if_st;//IF THEN
            doAssignTo_After(GROUP_REF(stmt->symbol(),1),(stmt->symbol()->attributes() & INDIRECT_BIT) ? CreateIG(0,1) :  CreateBG(0,1));
            LINE_NUMBER_AFTER(stmt,stmt); 
            Extract_Stmt(stmt);// extracting DVM-directive   
            stmt = endif_st;
            }
            break;

	    /* case DVM_INDIRECT_ACCESS_DIR:*/ 
/*
     case DVM_OWN_DIR: 
	   if(inparloop){
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              break;
            } 
            if(stmt->lexNext()->variant() == ASSIGN_STAT) 
               own_exe = 1;
            else
               err("OWN directive must precede an assignment statement",stmt);
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt); 
	  
            break;
   */   
       case DVM_PARALLEL_TASK_DIR: 
	  {  //SgForStmt *stdo;
            SgExpression *el;
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            if(!in_task_region)
              err("Parallel-task-loop directive is outside of the task region", 107,stmt); 
            if(in_task++) {
              err("Nested ON-blocks are not permitted", 104,stmt); 
              break;
            } 
               //stdo = isSgForStmt(stmt->lexNext());
            if(! isSgForStmt(stmt->lexNext())){
              err(" PARALLEL directive must be followed by DO statement",97,stmt);
                                                                   //directive is ignored  
              break;
            }    
            for(el=stmt->expr(1); el; el=el->rhs()) {
               if(el->lhs()->variant() != NEW_SPEC_OP)
                 err("Illegal clause",150,stmt); 
               break;
            }          
            task_do = stmt->lexNext();       
            LINE_NUMBER_AFTER(stmt,stmt);
            cur_st = task_do;
            task_lab = GetLabel();//stdo->endOfLoop()
            // task_do_ind = <renum_array>(loop_var_ind)
            doAssignTo_After(new SgVarRefExp(task_do->symbol()),new SgArrayRefExp(*TASK_RENUM_ARRAY(stmt->expr(0)->symbol()),*new SgVarRefExp(loop_var[0])));     
            task_do->setSymbol(*loop_var[0]);              
            StartTask(stmt);   
            pstmt = addToStmtList(pstmt, stmt); 
            //Extract_Stmt(stmt);// extracting DVM-directive 
            //stmt = cur_st;
	 }
            break;

       case DVM_ASYNCWAIT_DIR:
	    {
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt);  
            LINE_NUMBER_AFTER(stmt,stmt); //for tracing set on global variable of LibDVM  
            AsyncCopyWait(stmt->expr(0));
	    }
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;//setting stmt on last inserted statement                     
            break;

       case DVM_ASYNCHRONOUS_DIR:
            inasynchr++;
            async_id = stmt->expr(0);
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop",98, stmt);
            pstmt = addToStmtList(pstmt, stmt);  
            break;

       case DVM_ENDASYNCHRONOUS_DIR:
            inasynchr--;
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop",98, stmt);      
            pstmt = addToStmtList(pstmt, stmt); 
            break;

       case DVM_F90_DIR:
	    if(inparloop) {
              err("The directive is inside the range of PARALLEL loop",98, stmt); 
              break;
            }  
            if(!inasynchr)
               err("Misplaced directive",103,stmt);
            AsynchronousCopy(stmt);
            pstmt = addToStmtList(pstmt, stmt); 
            stmt=cur_st; 
            break;

       case DVM_TEMPLATE_CREATE_DIR:
            LINE_NUMBER_BEFORE(stmt,stmt);
            Template_Create(stmt);
            pstmt = addToStmtList(pstmt, stmt);             
            stmt = cur_st;
            break;

       case DVM_TEMPLATE_DELETE_DIR:
            LINE_NUMBER_BEFORE(stmt,stmt);
            Template_Delete(stmt);
            pstmt = addToStmtList(pstmt, stmt);
            stmt = cur_st;
            break;

       case DVM_TRACEON_DIR:
            InsertNewStatementAfter(new SgCallStmt(*fdvm[TRON]),stmt,stmt->controlParent());
            LINE_NUMBER_AFTER(stmt,stmt); 
            Extract_Stmt(stmt);// extracting DVM-directive             
            stmt = cur_st;
            break;

       case DVM_TRACEOFF_DIR:  
            InsertNewStatementAfter(new SgCallStmt(*fdvm[TROFF]),stmt,stmt->controlParent()); 
            LINE_NUMBER_AFTER(stmt,stmt); 
            Extract_Stmt(stmt);// extracting DVM-directive 
            stmt = cur_st;
            break;

       case DVM_BARRIER_DIR:
            doAssignStmtAfter(Barrier()); 
            FREE_DVM(1);
            LINE_NUMBER_AFTER(stmt,stmt);
            Extract_Stmt(stmt);// extracting DVM-directive             
            stmt = cur_st;
            break;

       case DVM_CHECK_DIR:
	    if(check_regim) {
              cur_st = Check(stmt);  
              Extract_Stmt(stmt); // extracting DVM-directive            
              stmt = cur_st;
            } else
              pstmt = addToStmtList(pstmt, stmt);     
            break;

       case DVM_DEBUG_DIR:
         { int num;
	 /*
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
	 */ 
            if((stmt->expr(0)->variant() != INT_VAL) || (num=stmt->expr(0)->valueInteger())<= 0)
              err("Illegal fragment number",181,stmt);  
            else  if(debug_fragment || perf_fragment)
              BeginDebugFragment(num,stmt);
            
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
	 }
            break;
       case DVM_ENDDEBUG_DIR: 
	 { int num;
         /*
            if(inparloop)
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
	 */ 	  
            if((stmt->expr(0)->variant() != INT_VAL) || (num=stmt->expr(0)->valueInteger())<= 0)
              err("Illegal fragment number",181,stmt);   
            else if((debug_fragment || perf_fragment) && ((cur_fragment && cur_fragment->No != num) || !cur_fragment))
              err("Unmatched directive",182,stmt);
            else {
             if(cur_fragment && cur_fragment->begin_st && (stmt->controlParent() != cur_fragment->begin_st->controlParent()))
	       err("Misplaced directive",103,stmt); //fragment must be a block
             EndDebugFragment(num);
	    }
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt); 
	 } 
            break;

       case DVM_IO_MODE_DIR:
            IoModeDirective(stmt,io_modes_str,WITH_ERR_MSG);
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt); 
	    break;
       case OPEN_STAT:
            Open_Statement(stmt,io_modes_str,WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case CLOSE_STAT:
            Close_Statement(stmt,WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case INQUIRE_STAT:
            Inquiry_Statement(stmt,WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case BACKSPACE_STAT:
       case ENDFILE_STAT:
       case REWIND_STAT:
            FilePosition_Statement(stmt,WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case WRITE_STAT:
       case READ_STAT:
	    ReadWrite_Statement(stmt, WITH_ERR_MSG);
	    stmt = cur_st;
            break;
       case PRINT_STAT:
            Any_IO_Statement(stmt);
            ReadWritePrint_Statement(stmt,WITH_ERR_MSG);
            stmt = cur_st;
            break;

       case DVM_CP_CREATE_DIR:     /*Check Point*/
            CP_Create_Statement(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case DVM_CP_SAVE_DIR:
            CP_Save_Statement(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case DVM_CP_LOAD_DIR:
            CP_Load_Statement(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;                 
      case DVM_CP_WAIT_DIR:
            CP_Wait(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;                /*Check Point*/
           
       case FOR_NODE:
            if(inasynchr){ //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
              break;
            }
             if(HPF_program)
                SetDoVar(stmt->symbol());
	     if(perf_analysis == 4 && !IN_COMPUTE_REGION)
                SeqLoopBegin(stmt);
             if(dvm_debug) 
       	        DebugLoop(stmt);
             else
             { 
                ChangeDistArrayRef(stmt->expr(0));
                ChangeDistArrayRef(stmt->expr(1));
             }
       default:
            if(inasynchr) //inside the range  of ASYNCHRONOUS construct
	      pstmt = addToStmtList(pstmt, stmt); // add to list of extracted statements
                         
            break;      
    }
    
   // analyzing of loop end statement
    {
      SgStatement *end_stmt;
      end_stmt = isSgLogIfStmt(stmt->controlParent()) ? stmt->controlParent() : stmt; 
      if(inparloop && isParallelLoopEndStmt(end_stmt,par_do))
                           
      { //stmt is last statement of parallel loop or is body of logical IF , which
        // is last statement 
        EndOfParallelLoopNest(stmt,end_stmt,par_do,func); 
        inparloop = 0;  // end of parallel loop nest        
        stmt = cur_st;
        //SET_DVM(iplp);
        continue;
      } // end of processing last statement of parallel loop
                                                            //printf("!!! end parallel loop %d\n",end_stmt->lineNumber());  
      if(HPF_program && isDoEndStmt(end_stmt))
        OffDoVarsOfNest(end_stmt); 
  
      if(task_do &&  isDoEndStmt(end_stmt) && end_stmt->controlParent() == task_do){
        SgStatement *st;
        st=ReplaceDoLabel(end_stmt,task_lab);
        if(st) {
          BIF_LABEL(st->thebif) = NULL;
          stmt = st;
          InsertNewStatementBefore (StopAM(),st);
          st->setLabel(*task_lab);

        } else {//ENDDO
          InsertNewStatementBefore (StopAM(),stmt);
        }
        in_task--;   
      }
  
      if(dvm_debug){
        if( isDoEndStmt_f90(stmt)) {
           //on debug regim logical IF may not be end of loop
          CloseLoop(stmt);
          stmt = cur_st;
        }  
      }
      else if(perf_analysis && close_loop_interval)
        if(isDoEndStmt_f90(end_stmt)){
          SeqLoopEnd(end_stmt,stmt);
          stmt = cur_st; 
        }
  
    } // end of processing last statement of loop nest

  } // end of processing executable statement/directive 
  
END_: // end of program unit     
 //checking: is in program unit any enclosed DVM-construct? 
   if(in_task_region)
      err("Missing ENDTASK_REGION directive",108,stmt); 
   if(in_task)
      err("Missing ENDON directive",109,stmt); 
//checking: is in program unit any enclosed ACC-construct? /*ACC*/
   if(cur_region)   /*ACC*/
   {  if( cur_region->is_data)
         err("Missing END DATA REGION directive",602,stmt);
      else
         err("Missing END REGION directive",603,stmt);
   }

// for declaring dvm000(N) is used maximal value of ndvm
  SET_DVM(ndvm);
  cur_st =  first_dvm_exec;
  if(last_dvm_entry)
    lentry = last_dvm_entry->lexNext(); // lentry - statement following first_dvm_exec or last generated dvm-initialization statement(before first_exec)
                                        // before first_exec may be new statements generated for first_exec 

  if(!IN_MODULE) {
    if(has_contains)
      MarkCoeffsAsUsed();
    InitBaseCoeffs();
    InitRemoteGroups();
    InitShadowGroups();
    InitRedGroupVariables();
    WaitDirList();
    EnterDataRegionForLocalVariables(begin_block ? begbl : cur_st); /*ACC*/
    DoStmtsForENTRY(first_dvm_exec,lentry); // copy the previously generated statements for each ENTRY
                                            // except for statements generated for the first executable statement if it is DVM-directive
    UnregisterVariables(begin_block); // close data region before exit from the procedure
 
    fmask[FNAME] = 0;
    stmt = data_stf ? data_stf->lexPrev() : first_dvm_exec->lexPrev();
    DeclareVarDVM(stmt,stmt); 

  } else {
    if(mod_proc){
      cur_st = end_of_unit->lexPrev();
      InitBaseCoeffs();
      MayBeDeleteModuleProc(mod_proc,end_of_unit); 
    }
    fmask[FNAME] = 0;
    nloopred = nloopcons = MAX_RED_VAR_SIZE;
    stmt= mod_proc ? has_contains->lexPrev() : first_dvm_exec->lexPrev();
    DeclareVarDVM(stmt, (mod_proc ? mod_proc : stmt)); 
  }
                      
 Extract_Stmt(first_dvm_exec);  //extract fname() call
 for(;pstmt; pstmt= pstmt->next)
     Extract_Stmt(pstmt->st);// extracting  DVM Directives and 
                             //statements (inside the range of ASYNCHRONOUS construct) 
  return; 
}


int DeleteDArFromList(SgStatement *stmt)
{ SgExpression *el,*preve,*pl,*opl,*dvm_list, *dvml;
  SgSymbol * s;
  int ia,is_assign; 

	 if(stmt->variant() == SAVE_DECL || stmt->variant() == INTENT_STMT || stmt->variant() == OPTIONAL_STMT || stmt->variant() == PRIVATE_STMT || stmt->variant() == PUBLIC_STMT)
	    return(1); 
          
          pl =  stmt->expr(0); 
          preve = 0;
          is_assign = 0;  
          dvm_list = NULL;
          for(el=stmt->expr(0); el; el=el->rhs()) {
              if(el->lhs()->variant() == ASSGN_OP || el->lhs()->variant() == POINTST_OP) is_assign = 1;//with initial value
              s = el->lhs()->symbol();
              if(s) {
                if(debug_regim && !in_interface && IS_ARRAY(s) )
                    registration = AddNewToSymbList( registration, s); 
                if(!strcmp(s->identifier(),"heap") && el->lhs()->lhs())
		    //  heap_ar_decl = el->lhs();
		    //heap_ar_decl->setSymbol(*heapdvm);
                  heap_ar_decl = new SgArrayRefExp(*heapdvm);
		    // heap_ar_decl = el->lhs()->lhs(); 
                ia = s->attributes();
                if(IS_GROUP_NAME(s))
                  Error("Inconsistent declaration of identifier: %s",s->identifier(),16,stmt);
                                
                if(((ia & DISTRIBUTE_BIT) || (ia & ALIGN_BIT) || (ia & INHERIT_BIT)) &&  !(ia & DVM_POINTER_BIT)  || (ia & HEAP_BIT) || !strcmp(s->identifier(),"heap") ){
                   el->lhs()->setLhs(NULL);
		   if(stmt->variant() == POINTER_STMT || stmt->variant() == TARGET_STMT || stmt->variant() == STATIC_STMT)
                      continue;
                   dvml = new SgExprListExp(el->lhs()->copy());     
		   dvml->setRhs(dvm_list);
                   dvm_list = dvml;

                   if(preve)
                         preve->setRhs( el->rhs());
                       // else
                       //  NODE_OPERAND1(preve->thellnd) = NULL;
                       //  preve->setRhs(LLNULL);
                   else
                   pl = el->rhs();
                }
                else
                   preve = el;
              }
             else
                preve = el;
          }
          if(stmt->variant() == VAR_DECL && dvm_list) {	   
            for( opl = stmt->expr(2); opl; opl=opl->rhs()) //looking through the option list and generating new statements
              NewSpecificationStatement(opl->lhs(),dvm_list,stmt);
          }
          if(is_assign && stmt->variant() == VAR_DECL && !stmt->expr(2))
            stmt->setVariant(VAR_DECL_90);
           
          if(pl) { 
            stmt->setExpression(0, *pl);
            return (1);
          }
          else // variable list is empty
            return (0);
         
}        


int DeleteHeapFromList(SgStatement *stmt)
{ SgExpression *el,*ec,*preve,*pl, *prcl, *cl;
  SgSymbol * s;
  int ia;
  // stmt is COMMON statement
    prcl = NULL;
    cl = stmt->expr(0); 
    for(ec=stmt->expr(0); ec; ec=ec->rhs()) {// looking through COMM_LIST
       pl = ec->lhs();
       preve = NULL;  
          for(el=ec->lhs(); el; el=el->rhs()) {  
              s = el->lhs()->symbol();
              if(s) {
                ia = s->attributes();   
                if( (ia & HEAP_BIT) || !strcmp(s->identifier(),"heap") ){
                   if(preve)
                     preve->setRhs( el->rhs());                      
                   else
                     pl = el->rhs();
                }
                else
                   preve = el;
              }
              else
                preve = el;
          } //end of loop el
          if(pl) {
            ec->setLhs(pl);
            prcl = ec;
          }
          else {// common variable list is empty
            if(prcl)
               prcl->setRhs(ec->rhs());
            else
               cl = ec->rhs();
          }
    }   
    if(cl) {
      stmt->setExpression(0, *cl);
      return(1); 
    }
    else // COMM_LIST is empty
      return(0);
}        

void NewSpecificationStatement(SgExpression *op, SgExpression *dvm_list, SgStatement *stmt)
{SgStatement *st;
	      switch(op->variant()){
                case PUBLIC_OP:
		  st = new SgStatement(PUBLIC_STMT);
                  break;
	        case PRIVATE_OP:
		  st = new SgStatement(PRIVATE_STMT);
                  break;
                case IN_OP:
	        case OUT_OP:
	        case INOUT_OP:
		  st = new SgStatement(INTENT_STMT);
                  st->setExpression(1, op->copy());
                  break;
                case SAVE_OP:
		  st = new SgStatement(SAVE_DECL);
                  break;
                case OPTIONAL_OP:
		  st = new SgStatement(OPTIONAL_STMT);
                  break;
                case POINTER_OP:
		  st = new SgStatement(POINTER_STMT);
                  break;
                case TARGET_OP:
		  st = new SgStatement(TARGET_STMT);
                  break;
                case STATIC_OP:
		  st = new SgStatement(STATIC_STMT);
                  break;
	      default: st = NULL;
              }                  
              if(st){
                st->setExpression(0,*dvm_list);
                stmt->insertStmtBefore(*st, *stmt->controlParent());
              }
} 
             
int DeferredShape(SgExpression *eShape)
{
   SgExpression *el;
   SgSubscriptExp *sbe;
   for(el=eShape; el; el=el->rhs())
   { 
      if ((sbe=isSgSubscriptExp(el->lhs())) != NULL && !sbe->ubound() && !sbe->lbound()) 
            continue;
      else
            return 0;
    }              
    return 1;
}

void TemplateDeclarationTest(SgStatement *stmt)
{
   SgExpression *eol;
   SgSymbol *symb;
   for(eol=stmt->expr(0); eol; eol=eol->rhs()) { //testing object list
      symb=eol->lhs()->symbol();
      if(IS_DUMMY(symb))
         Error("Template may not be a dummy argument: %s",symb->identifier(), 80,stmt); 
      if(DeferredShape(eol->lhs()->lhs()))
         symb->addAttribute(DEFERRED_SHAPE,(void*)1,0);                      
      if(IN_COMMON(symb) && IN_MODULE)
      {
         SYMB_ATTR(symb->thesymb) =  SYMB_ATTR(symb->thesymb) & (~COMMON_BIT);
         Warning("COMMON attribute is ignored: %s",symb->identifier(), 641,stmt);
      }  
   }
}

void  CreateArray_RTS2(SgSymbol *das, int indh, SgStatement *stdis) 
{
 int rank = Rank(das);
 SgExpression *shape_list  = DEFERRED_SHAPE_TEMPLATE(das) ? NULL : doDvmShapeList(das,stdis); 
 if(IS_TEMPLATE(das))
 {
      // adding to the Template_array Symbol the attribute (ARRAY_HEADER) 
      // with integer value "indh"  //"iamv" 
      ArrayHeader(das,indh);  // or 2
      SgExpression *array_header = HeaderRef(das);
      das->addAttribute(RTS2_CREATED, (void*) 1, 0);
      if(!DEFERRED_SHAPE_TEMPLATE(das)) 
         doCallStmt(DvmhTemplateCreate(das,array_header,rank,shape_list));
 }      
  else
 {
      // create dvm-array
      ArrayHeader(das,indh); 
      SgExpression *array_header = HeaderRef(das);
      SgExpression *shadow_list = DeclaredShadowWidths(das);
      doCallStmt(DvmhArrayCreate(das,array_header,rank,ListUnion(shape_list,shadow_list)));
      if(!HAS_SAVE_ATTR(das) && !IN_MODULE)
         doCallStmt(ScopeInsert(array_header));        
  }
}

void GenDistArray (SgSymbol *das, int idisars, SgExpression *distr_rule_list, SgExpression *ps, SgStatement *stdis) {

  int iamv,rank,iaxis,ileft,iright,ifst,indh;
  SgExpression *am_view = NULL, *array_header, *size_array;
                
  int ia,sign,re_sign,postponed_root;
  SgStatement *savest;

  savest = where; 
  ifst = ndvm;
  pointer_in_tree = 0;
  postponed_root = 0;
  indh = 1;

  if(IS_POINTER(das)) { //is POINTER 
     ArrayHeader(das,0);
     loc_distr = 1;  // POINTER is local object   
     goto TREE_;
  }
  if(IS_ALLOCATABLE(das)) { // ALLOCATABLE
     ArrayHeader(das,-2);
     loc_distr = 1;  // ALLOCATABLE is local object   
     goto TREE_;
  }

  if(IS_DUMMY(das)) { //is dummy argument
     ArrayHeader(das,1);
     //ReplaceArrayBounds(das);
     goto TREE_;
  }
  if(IS_POINTER_F90(das)) { // POINTER F90
     ArrayHeader(das,-2);
     if(!IS_DUMMY(das))
	 loc_distr = 1;
     goto TREE_;
  }
  if(IN_COMMON(das)) //  COMMON-block element or TEMPLATE_COMMON
    if(das->scope()->variant() != PROG_HEDR) { // is not in MAIN-program
                 //if(stdis->controlParent()->variant() != PROG_HEDR)
      
      if(IS_TEMPLATE(das))
      { 
        if(idisars == -1) { //interface of RTS2
          das->addAttribute(RTS2_CREATED, (void*) 1, 0);
         // ArrayHeader(das,1);
        } //else
        ArrayHeader(das,2);
      } else
        ArrayHeader(das,1);  
      goto TREE_;
    } 
  //if(DEFERRED_SHAPE_TEMPLATE(das)

  if((das->attributes() & SAVE_BIT) || (saveall && (!IN_COMMON(das)))
 || ORIGINAL_SYMBOL(das)->scope()->variant() == MODULE_STMT) {
    SgStatement *if_st;   
    if_st = doIfThenConstr(das);
    //first_exec = if_st->lexNext();  // reffer to ENDIF statement 
    where = if_st->lexNext();  // reffer to ENDIF statement   
  }       

  LINE_NUMBER_BEFORE(stdis,where); // for tracing set the global variable of LibDVM to
                                        // line number of statement(stdis) 
  ia = das->attributes();
      //if(ia & DYNAMIC_BIT && IS_SAVE(das)) 
      //    Error ("Saved object may not have the DYNAMIC attribute: %s", das->identifier(), 111,stdis);
 
  rank = Rank(das);
  if(ndis && rank && rank != ndis)
      Error ("Rank of  array %s  is not equal to the length of the dist_format_list", das->identifier(), 110,stdis);

  if((ia & SAVE_BIT) || saveall || IN_MODULE)
     sign = 1;
  else
     sign = 0; 
  if(ia & TEMPLATE_BIT) { //!!! must be changed
     if(ia & ALIGN_BASE_BIT)
       sign = 1; 
     else { //template  is not used in ALIGN or REALIGN directive
            //(is used  only in parallel directive)
       sign = 2;
       loc_templ_symb=AddToSymbList(loc_templ_symb,das);
     }
  }
  if(ia & POSTPONE_BIT)
    indh = -1;
  
  if(idisars == -1) { //interface of RTS2
    CreateArray_RTS2(das,indh,stdis);
    // distribute dvm-array 
    if(!(ia & POSTPONE_BIT))  //distr_rule_list!=NULL
       doCallStmt(DvmhDistribute(das,rank,distr_rule_list));
    where = savest;
    goto TREE_;
  }
                     // interface of RTS1
  if(DEFERRED_SHAPE_TEMPLATE(das))
  { 
    iamv = ndvm; ifst = iamv+1;
    ArrayHeader(das,iamv);
    doAssignStmt(new SgValueExp(0));
    doAssignTo(HeaderRef(das),DVM000(iamv)); // t = AMViewRef
    where = savest;
    goto TREE_;
  }      

// dvm000(i) = crtamv(AMRef, rank, SizeArray, StaticSign)
// crtamv() creates current Abstract_Machine view 
  size_array = doSizeArray(das,stdis); 
  if(!rank)  //distributee is not array
    size_array = new SgValueExp(0); // for continuing translation of procedure 

  iamv = ndvm; ifst = iamv+1; 
  if(ia & POSTPONE_BIT){
    //indh = -1;
    if(ia & TEMPLATE_BIT)
      //dvm000(i) = 0; (AMViewRef = 0) 
      doAssignStmt(new SgValueExp(0));
    else 
      ifst = ndvm; 
  } else {
    am_view = LeftPart_AssignStmt(CreateAMView(size_array, rank, sign));
    if(mult_block)
      doAssignStmt(MultBlock(am_view, mult_block, ndis));
    //dvm000(i) = genbli(PSRef, AMViewRef, AxisWeightArray, AxisCount) 
    // genbli sets on the weights of elements of processor system 
    if(gen_block == 1) 
      doAssignStmt(GenBlock(ps,am_view, idisars+2*nblock,nblock));
    if(gen_block == 2) 
      doAssignStmt(WeightBlock(ps,am_view, idisars+2*nblock, idisars+3*nblock,nblock));
    //dvm000(i) = DisAM(AMViewRef, PSRef, ParamCount, AxisArray,DistrParamArray) 
    // DisAM distributes resourses of parent (current) AM between children     
    doAssignStmt(DistributeAM(am_view, ps, nblock, idisars, idisars+nblock));
    if(mult_block)
      doAssignStmt(MultBlock(am_view, mult_block, 0));
  }  
 
//if distributed object isn't template then
// 1) create distribute array (CrtDa)
// 2) align distribute array with AM view:
//    align (i1,...,ik) with AM(i1,...,ik):: dist_array


  if(! (ia & TEMPLATE_BIT)) {
      // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
      //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray)
      // function CrtDA creates system structures, dosn't allocate array
 
      ArrayHeader(das,indh); 
      array_header = HeaderRef(das);
      //creating LeftBSizeArray and RightBSizeArray
      ileft = ndvm;
      iright = BoundSizeArrays(das);
      if(ia & DYNAMIC_BIT) 
          re_sign = 3;
      else
          re_sign = 0;  

      StoreLowerBoundsPlus(das,NULL);
         
      doAssignStmt(CreateDistArray(das,array_header,size_array,rank,ileft,iright,sign,re_sign));
       
      //ndvm--; // CrtDa result is exit code, test and free

      if(!(ia & POSTPONE_BIT)) {

      // dvm000(i) = AlgnDA (ArrayHandle,AMViewHandle,
      //                               Axis Array,Coeff Array),Const Array)
      //function AlgnDA alignes the array according to aligning template
      //actually AlgnDA distributes aligned array elements between virtual
      //processors 
      iaxis = ndvm;
      doAlignRule_1(rank);
            //     doAlignRule_1(axis_array,coeff_array,const_array);
      doAssignStmt(AlignArray(array_header, am_view, iaxis, iaxis+rank, iaxis+2*rank));
       
           // AlgnDA result is exit code, isn't used */
           // axis_array, coeff_array and const_array arn't used more
      }
       SET_DVM(ileft);
    
       //doAssignTo(header_ref(das,rank+2),HeaderNplus1(das));
                                                   // calculating HEADER(rank+1) 
  }
  else

     // adding to the Template_array Symbol the attribute (ARRAY_HEADER) 
     // with integer value "iamv"
    { 
      ArrayHeader(das,iamv);
      doAssignTo(HeaderRef(das),DVM000(iamv)); // t = AMViewRef
      if(IN_COMMON(das))
        StoreLowerBoundsPlus(das,NULL);
    }      
  where = savest; //first_exec;
 
TREE_: 
// Looking through the Align Tree of distributed array
   if(das->numberOfAttributes(ALIGN_TREE)) {//there are any align statements
   algn_attr * attr;
   align * root;
  
   postponed_root = (das->attributes() & POSTPONE_BIT);
   attr = (algn_attr *) das->attributeValue(0,ALIGN_TREE);
   root = attr->ref; // reference to root of align tree
              // test: attr->type == ROOT ????
              //   for(node=root->alignees; node; node=node->next) 
   AlignTree(root);
   }
   if(!pointer_in_tree && !postponed_root) // there are not any allocatable aligned arrays in alignment_tree
     {SET_DVM(ifst);}
//end GenDistArray
}

/*
void  RedistributeArray_RTS2(das,headref,*distr_rule_list,stdis)
{
  if(ia & POSTPONE_BIT) {
    SgStatement *if_st,*end_if;  
    SgExpression *size_array;  
    int iaxis;
    int iamv = INDEX(das);
    if_st = doIfThenConstrForRedis(headref,stdis,iamv);
    where = end_if = if_st->lexNext()->lexNext();  // reffer to ENDIF statement

    int ia = das->attributes();
    int rank = Rank(das);
 
    // distribute dvm-array 
    if(distr_rule_list!=NULL) 
       doCallStmt(DvmhDistribute(das,rank,distr_rule_list));
  }
  else {
  

  }
}
*/

void RedistributeArray(SgSymbol *das, int idisars, SgExpression *distr_rule_list, SgExpression *ps, int sign, SgExpression *dasref, SgStatement *stdis) 
{ int rank,ia;
  SgExpression *headref, *stre;
  rank = Rank(das);
  headref = IS_POINTER(das) ?  PointerHeaderRef(dasref,1) : HeaderRef(das);
  if(isSgRecordRefExp(dasref))
  { stre = & (dasref->copy());
    stre-> setLhs(headref);
    headref = stre;
  }
  if(rank && rank != ndis)
      Error ("Rank of array '%s' isn't equal to the length of the dist_format_list",das->identifier(), 110,stdis);

  ia=das->attributes();
  if(!(ia & DYNAMIC_BIT) && !(ia & POSTPONE_BIT))
      Error (" '%s' hasn't the DYNAMIC attribute",das->identifier(), 113,stdis);
  if(!(ia & DISTRIBUTE_BIT) && !(ia & INHERIT_BIT))
      Error (" '%s' does not appear in DISTRIBUTE/INHERIT directive ",das->identifier(), 114,stdis);
  if(ia & ALIGN_BIT)
      Error ("A distributee may not have the ALIGN attribute: %s",das->identifier(), 54, stdis);
  if(!HEADER(das)) {   
     Error("'%s' isn't distributed array", das->identifier(), 72,stdis); 
     return;
  } 

  if(idisars==-1)  // indirect distribution => interface of RTS2
  {
    //RedistributeArray_RTS2(das,headref,distr_rule_list,stdis);
    doCallStmt(DvmhRedistribute(das,rank,distr_rule_list));
    doAssignTo(HeaderRefInd(das,HEADER_SIZE(das)),new SgValueExp(1));   // Header(HEADER_SIZE) = 1 => the array has been distributed already 
    return;
  } 

  if(ia & POSTPONE_BIT){
    SgStatement *if_st,*end_if;  
    SgExpression * size_array, *am_view, *amvref, *headref_flag;  
    int i1,st_sign,iaxis,iamv;
    iamv = INDEX(das);
    if(ia & TEMPLATE_BIT) //TEMPLATE   ( iamv>1 )
      headref_flag = headref;
    else
      headref_flag = IS_POINTER(das) ? PointerHeaderRef(dasref,HEADER_SIZE(das)) : HeaderRefInd(das,HEADER_SIZE(das));
    if_st = doIfThenConstrForRedis(headref_flag,stdis,iamv); /*08.05.17*/
    where = end_if = if_st->lexNext()->lexNext();  // reffer to ENDIF statement
    i1 = ndvm;
    if(ACC_program)                   /*ACC*/
       where->insertStmtBefore(*Redistribute_H(headref,sign),*where->controlParent());
    amvref =   (ia & TEMPLATE_BIT) ? headref : GetAMView( headref); 
    //inserting after ELSE 
    if(mult_block)
      doAssignStmt(MultBlock(amvref, mult_block, ndis));
    //dvm000(i) = genbli(PSRef, AMViewRef, AxisWeightArray, AxisCount) 
    // genbli sets on the weights of processor system elements
    if(gen_block == 1) 
      doAssignStmt(GenBlock(ps,amvref, idisars+2*nblock,nblock)); 
    if(gen_block == 2) 
      doAssignStmt(WeightBlock(ps,amvref,idisars+2*nblock,idisars+3*nblock,nblock));  
    doCallStmt(RedistributeAM(headref, ps, nblock,idisars,sign));
    if(mult_block)
      doAssignStmt(MultBlock(amvref, mult_block, 0));
    where = if_st->lexNext();  // reffer to ELSE statement 
    //inserting after IF (...) THEN 
    if (DEFERRED_SHAPE_TEMPLATE(das))
      am_view = DVM000(INDEX(das));
    else
    {  
      if(ia & TEMPLATE_BIT)
        size_array = doSizeArray(das,stdis);
      else
        size_array = doSizeArrayQuery( IS_POINTER(das) ? headref : HeaderRefInd(das,1),rank); 
      if(!rank)  //distributee is not array
        size_array = new SgValueExp(0); // for continuing translation of procedure 
 
      // dvm000(i) = crtamv(AMRef, rank, SizeArray, StaticSign)
      //crtamv creates current Abstract_Machine view 

      if((ia & SAVE_BIT) || saveall || IN_COMMON(das) || das->scope() != cur_func || IS_BY_USE(das) )
        st_sign = 1;
      else
        st_sign = 0; 
      if(iamv <= 1) // is not TEMPLATE
        iamv = ndvm++;
      am_view = DVM000(iamv);
      doAssignTo(am_view,CreateAMView(size_array, rank, st_sign));
    }

    if(mult_block)
      doAssignStmt(MultBlock(am_view, mult_block, ndis));
    //dvm000(i) = genbli(PSRef, AMViewRef, AxisWeightArray, AxisCount) 
    // genbli sets on the weights of elements of processor system 
    if(gen_block == 1) 
      doAssignStmt(GenBlock(ps,am_view, idisars+2*nblock,nblock));
    if(gen_block == 2) 
      doAssignStmt(WeightBlock(ps,am_view, idisars+2*nblock, idisars+3*nblock,nblock));  
    //dvm000(i) = DisAM(AMViewRef, PSRef, ParamCount, AxisArray,DistrParamArray) 
    // DisAM distributes resourses of parent (current) AM between children   
    doAssignStmt(DistributeAM(am_view,ps,nblock,idisars,idisars+nblock)); 
    if(mult_block)
      doAssignStmt(MultBlock(am_view, mult_block, 0));  
    if (!(ia & TEMPLATE_BIT)) {
      // dvm000(i) = AlgnDA (ArrayHandle,AMViewHandle,
      //                               Axis Array,Coeff Array,Const Array)
      //function AlgnDA alignes the array according to aligning template
      //actually AlgnDA distributes aligned array elements between virtual
      //processors 
      iaxis = ndvm;
      doAlignRule_1(rank);
      doAssignStmt(AlignArray( headref, am_view, iaxis, iaxis+rank, iaxis+2*rank));
      doAssignTo(headref_flag, new SgValueExp(1));  // Header(HEADER_SIZE) == 1 => the array has been distributed already 
    } else
      doAssignTo(headref,am_view); // t = AMViewRef
    //  Looking through the Align Tree of distributed array
    if(das->numberOfAttributes(ALIGN_TREE) && !IS_ALLOCATABLE_POINTER(das)) {//there are any align statements
      algn_attr * attr;
      align * root;
      attr = (algn_attr *) das->attributeValue(0,ALIGN_TREE);
      root = attr->ref; // reference to the root of align tree    
      AlignTreeAlloc(root,stdis);
    }
    SET_DVM(i1);
    cur_st = end_if;  //  => where   10.12.12 ;
    where = stdis;    //10.12.12
  }
  else {
    SgExpression *amvref;  
    
    if(ACC_program)                   /*ACC*/
       where->insertStmtBefore(*Redistribute_H(headref,sign),*where->controlParent());

    amvref =   (ia & TEMPLATE_BIT) ? headref : GetAMView( headref); 
    if(mult_block)
      doAssignStmt(MultBlock(amvref, mult_block, ndis));
    if(gen_block == 1) 
      // genbli sets on the weights of processor system elements
      doAssignStmt(GenBlock(ps,amvref, idisars+2*nblock,nblock));  
    if(gen_block == 2) 
      doAssignStmt(WeightBlock(ps,amvref,idisars+2*nblock,idisars+3*nblock,nblock)); 
    doCallStmt(RedistributeAM(headref,ps,nblock,idisars,sign));
    //doAssignTo_After(header_ref(das,rank+2),HeaderNplus1(das));
                                                   // calculating HEADER(rank+1) 
    if(mult_block)
      doAssignStmt(MultBlock(amvref, mult_block, 0)); 
  }
}

void AlignTree( align *root) {
    align *node; 
    int nr,iaxis,ia;
    SgStatement *stalgn;
    int pointer_is;
    stalgn = NULL;
    pointer_is = 0;
    iaxis = 0;
    for(node=root->alignees; node; node=node->next) {
       if (stalgn != node->align_stmt) {
	 if(IN_COMMON(node->symb) && (node->symb->scope()->variant() != PROG_HEDR) || !CURRENT_SCOPE(node->symb))
	 { stalgn = NULL; ia = -1;}
         else { 
         stalgn = node->align_stmt;
         iaxis = ndvm; ia = 0;
         }
       }
       else if(!INDEX(root->symb) || pointer_is || (INDEX(root->symb)==-1))
       { iaxis = ndvm; ia = 0;}
       else
         ia = iaxis;
       if(IS_ALLOCATABLE(node->symb) || (IS_ALLOCATABLE(root->symb) && CURRENT_SCOPE(root->symb)))
         ia = -2;  //doAlignRule is empty: align rules are not generated
       if(IS_POINTER_F90(node->symb) || (IS_POINTER_F90(root->symb) && !IS_DUMMY(root->symb) && CURRENT_SCOPE(root->symb)))
	  ia = -2; //doAlignRule is empty: align rules are not generated
       SgExpression *align_rule_list = doAlignRules(node->symb,node->align_stmt,ia,nr);// creating axis_array,
                                                                                       // coeff_array and  const_array
       GenAlignArray(node,root, nr, align_rule_list, iaxis);
       pointer_is =  IS_POINTER(node->symb) || IS_ALLOCATABLE_POINTER(node->symb);
       AlignTree(node);
    } 
}


void    GenAlignArray(align *node, align *root, int nr, SgExpression *align_rule_list, int iaxis) {

// 1) creates Distribute Array for "node"
// 2) alignes Distribute Array with Distribute Array for "root" or with Template 

// To array symbol added attribute ARRAY_HEADER (by function ArrayHeader):
//    0, for DVM-pointer
//   -1, for array with postponed allignment and for array allined with one or DVM-pointer 
//   -2, for ALLOCATABLE array
//    1, for other arrays

  int rank,ileft,iright,isize;
  int sign,re_sign,ia,indh;
  SgSymbol *als;
  SgExpression *array_header,*size_array;
  SgStatement *savest;
  //st = first_exec;   // store first_exec
  savest = where;
  als = node->symb;
  ia = als->attributes();
 
  // for debug
  //printf("%s\n", als->identifier());
  //
    
  if(IS_POINTER(als)) { //alignee  is POINTER   
   
     int *index = new int [2];
     *index = iaxis;
     *(index+1) = nr;
     als-> addAttribute(ALIGN_RULE, (void*) index, 2*sizeof(int)); 
 
     ArrayHeader(als,0);
     loc_distr = 1; //POINTER is local object
     pointer_in_tree = 1;
     return;
  }
  if(IS_ALLOCATABLE(als)) { //alignee  is ALLOCATABLE array   
   
   //  int *index = new int [2];
   //  *index = 0; //iaxis;
   //  *(index+1) = nr;
   //  als-> addAttribute(ALIGN_RULE, (void*) index, 2*sizeof(int)); 
 
     ArrayHeader(als,-2);
     loc_distr = 1; //ALLOCATABLE array is local object
     pointer_in_tree = 1;
     return;
  }
  if(IS_POINTER_F90(als)) { // POINTER F90
   if(IS_DUMMY(als))
     ArrayHeader(als,1);
   else{
     ArrayHeader(als,-2);
     pointer_in_tree = 1;
	 loc_distr = 1;
   }
   return;
  }

  if(root){    
    indh = INDEX(root->symb);
    if(CURRENT_SCOPE(root->symb) && ((indh == 0) || (indh == -1)  || ((indh > 1) && (root->symb->attributes() & POSTPONE_BIT)))) {
                        //align-target is allocatable array: it is aligned directly
                        // or indirectly with POINTER 
                        //or
                        //align-target is "postponed" array:it is aligned directly
                        // or indirectly with array having  POSTPONE_BIT attribute 
                        // or
                        // align-target is TEMPLATE with POSTPONE_BIT
     int *index = new int [2];
     *index = iaxis;
     *(index+1) = nr;
     als-> addAttribute(ALIGN_RULE, (void*) index, 2*sizeof(int)); 
 
     ArrayHeader(als,-1);
     indh = -1; 
    } else 
     ArrayHeader(als,1);

    if(root && IS_ALLOCATABLE(root->symb) && CURRENT_SCOPE(root->symb)) {
      Error("Array '%s' may not be alligned with ALLOCATABLE array",als->identifier(),401,node->align_stmt); 
      return;  
    }
 
  } else {
     ArrayHeader(als,-1); // with POSTPONE_BIT
     indh = 1;
  }
  

  if(IS_TEMPLATE(als)){
    Error("Template '%s' appears as an alignee",als->identifier(),116,node->align_stmt);
    return;
  }
  if(IS_DUMMY(als)) { //alignee is dummy argument
    if(!root) return;
    if(!IS_DUMMY(root->symb)){ // align-target is local array
       if(!IN_COMMON(root->symb) && CURRENT_SCOPE(root->symb))
        Error("Dummy argument '%s' is aligned with a local array", als->identifier(),117, node->align_stmt);
    }
    else
       if(warn_all)
         warn("Associated actual arguments must be aligned",177,node->align_stmt);
    return;
  }

  if(IN_COMMON(als)){ //  COMMON-block element
    if(root && !IN_COMMON(root->symb) && (root->symb->scope()->variant() != PROG_HEDR)) {
                         //align-target is not in COMMON and its scope is not MAIN-program
      Error("Aligned array '%s' is in COMMON  but align-target is not", als->identifier(),  118,node->align_stmt);
      return;
    }
    if(als->scope()->variant() != PROG_HEDR)  // is not in MAIN-program
      return;
  }
  if(indh <= 0 && root && CURRENT_SCOPE(root->symb)) //align-target is allocatable  or "postponed" array /podd 31.05.08/
    return;

  if(IS_SAVE(als)) {  // has SAVE attribute
    if(root && !IS_TEMPLATE(root->symb) && !IN_COMMON(root->symb) && !HAS_SAVE_ATTR(root->symb) && CURRENT_SCOPE(root->symb) ) {
      Error("Aligned array '%s' has SAVE attribute but align-target has not", als->identifier(),119,node->align_stmt);
      return;
    }
  } 
  if(IS_SAVE(als) || ORIGINAL_SYMBOL(als)->scope()->variant() == MODULE_STMT) {
    SgStatement *ifst;   
    ifst = doIfThenConstr(als);
    //first_exec = ifst->lexNext();  // reffer to ENDIF statement  
    where = ifst->lexNext();  // reffer to ENDIF statement   
  }      
  LINE_NUMBER_BEFORE(node->align_stmt,where); 
                                        // for tracing set the global variable of LibDVM to
                                        // line number of ALIGN directive 

  array_header = HeaderRef(als);
  rank = Rank(als);

  if(INTERFACE_RTS2) { //interface of RTS2

    doCallStmt(DvmhArrayCreate(als,array_header,rank,ListUnion(doDvmShapeList(als,node->align_stmt),DeclaredShadowWidths(als))));
    if(!HAS_SAVE_ATTR(als) && !IN_MODULE) 
      doCallStmt(ScopeInsert(array_header));
    if(!(ia & POSTPONE_BIT) && align_rule_list)
      doCallStmt(DvmhAlign(als,root->symb,nr,align_rule_list));
    where = savest;
    return;
  }
                       // interface of RTS1
  isize = ndvm;
  size_array = doSizeArray(als, node->align_stmt );
  ileft = ndvm;
  iright= BoundSizeArrays(als);
  if((ia & SAVE_BIT) || saveall || IN_MODULE)
     sign = 1;
  else
     sign = 0;  

  if(ia & DYNAMIC_BIT){
  /*
    if( IS_SAVE(als))
       Error ("Saved object may not have the DYNAMIC attribute: %s", als->identifier(), 111,node->align_stmt);
    
    if(IN_COMMON(als))
       Error ("Object in COMMON  may not have the DYNAMIC attribute: %s", als->identifier(), 112,node->align_stmt);
  */
     re_sign = 2;
  }
   else if(ia & POSTPONE_BIT) 
     re_sign = 2;
   else
     re_sign = 0;  
  // aligned array may not be redisributed 

  StoreLowerBoundsPlus(als,NULL);
  // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
  //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray) 
  // function CrtDA creates system structures, dosn't allocate array    
  doAssignStmt(CreateDistArray(als, array_header, size_array,rank,ileft,iright,sign,re_sign));  
 /* ndvm--; // CrtDa result is exit code, test and free  */

  if(!(ia & POSTPONE_BIT)) { 
  // dvm000(i) = AlgnDA (ArrayHeader,PatternRef,
  //                               Axis Array,Coeff Array,Const Array)
  doAssignStmt(AlignArray(array_header,HeaderRef(root->symb),
                              iaxis, iaxis+nr,iaxis+2*nr));
  //doAssignTo(header_ref(als,rank+2),HeaderNplus1(als));//calculating HEADER(rank+1)
  }
  SET_DVM(isize);  
  //first_exec = st; //restore first_exec
  where = savest; //first_exec;   
}

void RealignArray(SgSymbol *als, SgSymbol *tgs, int iaxis, int nr, SgExpression *align_rule_list, int new_sign, SgStatement *stal) 
{ int ia,iamv;
  SgStatement *if_st;
  SgExpression *header_flag = HeaderRefInd(als,HEADER_SIZE(als));

  ia=als->attributes();
  if(!(ia & DYNAMIC_BIT) &&  !(ia & POSTPONE_BIT))
      Error (" '%s' hasn't the DYNAMIC attribute",als->identifier(), 113,stal);
  if(!(ia & ALIGN_BIT)  && !(ia & INHERIT_BIT))
      Error (" '%s' does not appear in ALIGN or INHERIT directive ",als->identifier(),120, stal);
  if(ia & DISTRIBUTE_BIT)
      Error ("An alignee may not have the DISTRIBUTE attribute: %s",als->identifier(), 57,  stal);
  if(!HEADER(als)) {   
      Error("%s isn't distributed array", als->identifier(), 72,stal); 
      return;
  } 
  if(!HEADER(tgs))    
    return;
  if(INTERFACE_RTS2)
  {
    doCallAfter(DvmhRealign(HeaderRef(als),new_sign,HeaderRef(tgs),nr,align_rule_list));
    return; 
  }
  iamv = ndvm;
  if(ACC_program )              /*ACC*/
  { if( !(ia & POSTPONE_BIT) )
      doCallAfter(Realign_H(HeaderRef(als),new_sign));
    else {      
      if_st = doIfThenConstrForRealign(header_flag,cur_st,0); 
      cur_st = if_st;
      doCallAfter(Realign_H(HeaderRef(als),new_sign));
      cur_st = if_st->lexNext()->lexNext(); //ENDIF statement 
    }
  }
  doCallAfter(RealignArr(HeaderRef(als),HeaderRef(tgs),iaxis,iaxis+nr,iaxis+2*nr,new_sign));
  

  if(ia & POSTPONE_BIT) {
      if_st = doIfThenConstrForRealign(header_flag,cur_st,1);
      where = if_st->lexNext();  // reffer to ENDIF statement
      algn_attr *attr = (algn_attr *) als->attributeValue(0,ALIGN_TREE);
      align  *root = attr->ref; // reference to the root of align tree    
      if( !(ia & ALLOCATABLE_BIT) && !(ia & POINTER_BIT) && root->alignees)
        //  Looking through the Align Tree of array      
        AlignTreeAlloc(root,stal);
      doAssignTo(header_flag, new SgValueExp(1));
      SET_DVM(iamv);
      cur_st = where;// ENDIF statement
      where = stal;    //11.12.12
  }
}

void ALLOCATEf90_arrays(SgStatement *stmt, distribute_list *distr)
{SgExpression *alce,*al, *new_list, *apr;
 SgSymbol *ar;
 int dvm_flag = 0;
 where = stmt;
 ReplaceContext(stmt); 
 //LINE_NUMBER_BEFORE(stmt,stmt);  /*26.10.17*/
 if(stmt->hasLabel())              /*26.10.17*/
   InsertNewStatementBefore(new SgStatement(CONT_STAT),stmt);  // lab  CONTINUE
 SgStatement *prev = stmt->lexPrev();
 new_list = stmt->expr(0); apr = NULL;
 for(al=stmt->expr(0); al; al=al->rhs()) {
   alce = al->lhs(); //allocation
   
   if(isSgRecordRefExp(alce)) 
   {  struct_component = alce;
      alce = RightMostField(alce);
   }  else
      struct_component = NULL;
   ar = alce->symbol();
   //ar = (isSgRecordRefExp(alce)) ? RightMostField(alce)->symbol() : alce->symbol();
   if(!IS_ALLOCATABLE_POINTER(ar)) {
      Error("An allocate/deallocate object must have the ALLOCATABLE or POINTER attribute: %s",ar->identifier(),287,stmt);
      continue;
   }
   if(only_debug)
      return;
   if(ar->attributes() & DISTRIBUTE_BIT) {
    //determine corresponding DISTRIBUTE statement
     SgStatement *dist_st = (DISTRIBUTE_DIRECTIVE(ar)) ? *(DISTRIBUTE_DIRECTIVE(ar)) : NULL;
     if(ar->attributes() & POINTER_BIT) 
       AllocatePointerHeader(ar,stmt);
     if(struct_component)
       ALLOCATEStructureComponent(ar,struct_component,alce,stmt);  
    //allocate distributed array
     if(dist_st)
       ALLOCATEf90DistArray(ar,alce,dist_st,stmt);
    //delete from list of ALLOCATE statement 
     if(apr)
        apr->setRhs(al->rhs());
     else
        new_list = al->rhs();
     dvm_flag = 1;
   }
       
   else if(ar->attributes() & ALIGN_BIT) {
     if(ar->attributes() & POINTER_BIT) 
       AllocatePointerHeader(ar,stmt);
    //allocate aligned array  
     if(struct_component)
       ALLOCATEStructureComponent(ar,struct_component,alce,stmt);  
     else
       AllocateAlignArray(ar,alce,stmt);
    //delete from list of ALLOCATE statement 
     if(apr)
        apr->setRhs(al->rhs());
     else
        new_list = al->rhs(); 
     dvm_flag = 1;   
   }
   else
     apr = al;    
 }
  //replace allocation-list of ALLOCATE statement  by new_list
  //stmt->setExression(0,new_list);
  if(new_list)
    BIF_LL1(stmt->thebif)= new_list->thellnd;
  else
    BIF_LL1(stmt->thebif)= NULL;

  if(dvm_flag)
    LINE_NUMBER_AFTER_WITH_CP(stmt,prev,stmt->controlParent());  /*26.10.17*/
  return;
}

void AllocatePointerHeader(SgSymbol *ar,SgStatement *stmt)
{SgStatement *alst;
 SgExpression *headerRef, *structRef;
 alst = new SgStatement(ALLOCATE_STMT);
 headerRef = new SgArrayRefExp(*ar,*new SgValueExp(HEADER_SIZE(ar)));
 if(ar->variant() == FIELD_NAME)
 {    structRef = &(struct_component->copy());
	  structRef->setRhs(headerRef);
	  headerRef = structRef;
 }
 alst->setExpression(0, *new SgExprListExp(*headerRef));
    //alst->setExpression(0, *new SgExprListExp(*new SgArrayRefExp(*ar,*new SgValueExp(HEADER_SIZE(ar)))));
 InsertNewStatementBefore(alst,stmt); 
}

void DEALLOCATEf90_arrays(SgStatement *stmt)
{SgExpression *al, *new_list, *apr;
 SgSymbol *ar;
 SgStatement *prev;
 int dvm_flag = 0;
 
 ReplaceContext(stmt);  
 //LINE_NUMBER_BEFORE(stmt,stmt);  /*26.10.17*/
 if(stmt->hasLabel())              /*26.10.17*/
   InsertNewStatementBefore(new SgStatement(CONT_STAT),stmt);  // lab  CONTINUE
 cur_st = prev = stmt->lexPrev();
 new_list = stmt->expr(0); apr = NULL;
 for(al=stmt->expr(0); al; al=al->rhs()) {
   ar = (isSgRecordRefExp(al->lhs())) ? RightMostField(al->lhs())->symbol() : al->lhs()->symbol();
   if(!IS_ALLOCATABLE_POINTER(ar)) {
      Error("An allocate/deallocate object must have the ALLOCATABLE or POINTER attribute: %s",ar->identifier(),287,stmt);
      continue;
   }
   if(ar->variant()==FIELD_NAME && IS_DVM_ARRAY(ar))
   { SgExpression *structRef, *headerRef;
       headerRef = new SgArrayRefExp(*ar,*new SgValueExp(1));
       structRef = &(al->lhs()->copy());
       structRef->setRhs(headerRef);
       headerRef = structRef; 
       InsertNewStatementAfter(DeleteObject_H(headerRef),cur_st,stmt->controlParent()); /*26.10.17*/
       dvm_flag = 1;
           //doCallAfter(DeleteObject_H(headerRef));
           //if(ACC_program)    /*ACC*/
               //InsertNewStatementAfter(DestroyArray(headerRef),cur_st,stmt->controlParent());
         
       apr = al;
       continue;
   }
   if(HEADER(ar)) {
       InsertNewStatementAfter(DeleteObject_H(HeaderRefInd(ar,1)),cur_st,stmt->controlParent()); /*26.10.17*/  
       dvm_flag = 1;
           //if(ACC_program)      /*ACC*/
               //InsertNewStatementAfter(DestroyArray(HeaderRefInd(ar,1)),cur_st,stmt->controlParent());
           //FREE_DVM(1);
           //doCallAfter(DeleteObject_H(HeaderRefInd(ar,1)));

     if(IS_POINTER_F90(ar)){
      apr = al;
      continue;
     }
     if(apr)
        apr->setRhs(al->rhs());
     else
        new_list = al->rhs();

   } else
   { apr = al;
     InsertNewStatementAfter(DataExit(&al->lhs()->copy(),0),cur_st,stmt->controlParent()); /*26.10.17*/
          //if(ACC_program)    /*ACC*/
              // InsertNewStatementAfter(DestroyScalar(&al->lhs()->copy()),cur_st,stmt->controlParent());
         //doCallAfter(DataExit(&al->lhs()->copy(),0)); /*ACC*/
   }
 }
  //replace deallocation-list of DEALLOCATE statement  by new_list
  if(new_list)
    BIF_LL1(stmt->thebif)= new_list->thellnd;
  else
    BIF_LL1(stmt->thebif)= NULL;

  if(dvm_flag)
    LINE_NUMBER_AFTER_WITH_CP(stmt,prev,stmt->controlParent());  /*26.10.17*/
  return; 
}


void AllocateArray(SgStatement *stmt, distribute_list *distr)
{ SgExpression *desc;
  SgSymbol *p;
  if(!stmt->expr(1)->lhs()) {// empty argument list of allocate function call
      err("Wrong argument list of ALLOCATE function call", 262, stmt);
      return;
  }
  desc = stmt->expr(1)->lhs()->lhs(); //descriptor array reference
  if(!isSgArrayRefExp(desc) || !desc->symbol() || (desc->symbol()->type()->baseType()->variant() != T_INT) || IS_POINTER(desc->symbol()) || IS_DVM_ARRAY(desc->symbol()))
  {
    err("Descriptor array error", 122, stmt);
    return;
  }
  if(desc->lhs())
     ChangeDistArrayRef(desc);

  where = stmt;
  p = stmt->expr(0)->symbol(); // pointer in left part
  /*if (p->attributes() & DIMENSION_BIT)
     Error("POINTER in left part has DIMENSION attribute: %s",p->identifier(),stmt);*/
  if(p->attributes() & DISTRIBUTE_BIT) {
    //determine corresponding DISTRIBUTE statement
     SgStatement *dist_st;
     SgExpression *el;
     distribute_list *dsl;
     dist_st = NULL;
     for(dsl=distr; dsl && !dist_st; dsl=dsl->next)
     for(el=dsl->stdis->expr(0); el; el=el->rhs())
       if(el->lhs()->symbol() == p) {
         dist_st = dsl->stdis;
         break;
       }
    //allocate distributed array
     ReplaceContext(stmt);   
     AllocateDistArray(p,desc,dist_st,stmt);
     return;
  }

  if(p->attributes() & ALIGN_BIT) {
    //allocate aligned array
    ReplaceContext(stmt);   
    AllocateAlignArray(p,desc,stmt);
    return;
  }

  Error("POINTER '%s' is not distributed object",p->identifier(), 85,stmt);
  return;
}

void   AllocateDistArray(SgSymbol *p, SgExpression *desc, SgStatement *stdis, SgStatement *stmt) {

  int iamv,rank,iaxis,ileft,iright,ifst;
  SgExpression  *array_header, *size_array, *ps, *arglist, *lbound;  
  //SgSymbol *sheap;          
  int ia,sign,re_sign;
  int idisars;
 
  ifst = ndvm;
   // if(IS_DUMMY(p) || IN_COMMON(p)) { //is dummy argument or  COMMON-block element
   //    return;
   //}
  LINE_NUMBER_BEFORE(stmt,stmt); // for tracing set the global variable of LibDVM to
                                 // line number of statement(stmt)
  SgExpression *distr_rule_list = doDisRules(stdis,0,idisars);
    //idisars = doDisRuleArrays(stdis,0,NULL);
  if(idisars == -1)
      Error ("INDIRECT/DERIVED format is not permitted for pointer %s", p->identifier(), 626,stdis);
  rank = PointerRank(p);
  if(ndis && rank && rank != ndis)
      Error ("Rank of pointer %s  is not equal to  the length of the dist_format_list", p->identifier(), 123,stdis);
 
 // dvm000(i) = CrtAMV(AMRef, rank, SizeArray, StaticSign)
 //CrtAMV creates current Abstract_Machine view 
  ia = p->attributes();
  size_array = ReverseDim(desc,rank); 
  if((ia & SAVE_BIT) || saveall || (ia & COMMON_BIT))
     sign = 1;
  else
     sign = 0;  
  iamv = ndvm;  /* ifst = iamv+1; */
  if(!(ia & POSTPONE_BIT)){
    doAssignStmt(CreateAMView(size_array, rank, sign));

    ps = PSReference(stdis);
    if(mult_block)
      doAssignStmt(MultBlock(DVM000(iamv), mult_block, ndis));
    //dvm000(i) = genbli(PSRef, AMViewRef, AxisWeightArray, AxisCount) 
    // genbli sets on the weights of elements of processor system 
    if(gen_block == 1) 
     doAssignStmt(GenBlock(ps,DVM000(iamv), idisars+2*nblock,nblock));
    if(gen_block == 2) 
     doAssignStmt(WeightBlock(ps,DVM000(iamv),idisars+2*nblock, idisars+3*nblock,nblock));
    //dvm000(i) = DisAM(AMViewRef, PSRef, ParamCount, AxisArray,DistrParamArray) 
    // DisAM distributes resourses of parent (current) AM between children     
    doAssignStmt(DistributeAM(DVM000(iamv),ps,nblock,idisars,idisars+nblock));  
    if(mult_block)
     doAssignStmt(MultBlock(DVM000(iamv), mult_block, 0));     
  }
   
 // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
 //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray)
 // function CrtDA creates system structures, doesn't allocate array

      //sheap = heap_ar_decl ? heap_ar_decl->symbol() : p;//heap_ar_decl == NULL is user error
      //doAssignTo(stmt->expr(0), ARRAY_ELEMENT(sheap,1)); 
                                                  // P = HEAP(1) or P(I) = HEAP(1)
      if(!stmt->expr(0)->lhs()) // case P
        doAssignTo(stmt->expr(0), new SgValueExp(POINTER_INDEX(p))); 
                                                  // P = <heap-index> or P(I) = <heap-index>
      else {                   // case P(I,...)
        doAssignTo(stmt->expr(0), HeapIndex(stmt)); 
      }  
      array_header =  PointerHeaderRef(stmt->expr(0),1);
      //doAssignTo( ARRAY_ELEMENT(sheap, 1), &(* ARRAY_ELEMENT(sheap, 1) + *new SgValueExp(HEADER_SIZE(p))));                            
                       //HEAP(1) = HEAP(1) + <header_size>
      //doLogIfForHeap(sheap, heap_size);

      //creating LeftBSizeArray and RightBSizeArray
      ileft = ndvm;
      iright = BoundSizeArrays(p);
      if(ia & DYNAMIC_BIT)
        re_sign = 3;
      else
        re_sign = 0;  
      arglist= stmt->expr(1)->lhs();
      lbound=0;
      if(arglist->rhs() && arglist->rhs()->rhs() && arglist->rhs()->rhs()->rhs() ) {//there are 3-nd and 4-nd argument of ALLOCATE function call
        SgExpression *heap;
        lbound =  arglist->rhs()->rhs()->lhs();  //lower bound array reference ??
        heap   =  arglist->rhs()->lhs();   //heap array reference ??
        if(heap  && isSgArrayRefExp(heap) && !heap->lhs() && lbound  && isSgArrayRefExp(lbound)) 
             ;
        else
          lbound = 0;
      }
      if(!lbound)
        StoreLowerBoundsPlus(p,stmt->expr(0));
      else
        StoreLowerBoundsPlusFromAllocate(p,stmt->expr(0),lbound);
      doAssignStmt(CreateDistArray(p,array_header,size_array,rank,ileft,iright,sign,re_sign));
      if(debug_regim && TestType(PointerType(p))) {
        SgExpression *heap;
	if(stmt->expr(1)->lhs()->rhs()) {//there is 2-nd argument of ALLOCATE function call
           heap =  stmt->expr(1)->lhs()->rhs()->lhs(); //heap array reference
           if(heap  && isSgArrayRefExp(heap) && !heap->lhs())
             InsertNewStatementBefore(D_RegistrateArray(rank, TestType(PointerType(p)), GetAddresDVM(array_header),size_array,stmt->expr(0) ) ,stmt);
	}
      }
      if(ia & POSTPONE_BIT)
	{ SET_DVM(ifst); return;}
      // dvm000(i) = AlgnDA (ArrayHandle,AMViewHandle,
      //                               Axis Array,Coeff Array),Const Array)
      //function AlgnDA alignes the array according to aligning template
      //actually AlgnDA distributes aligned array elements between virtual
      //processors 
      iaxis = ndvm;
      doAlignRule_1(rank);
      //             doAlignRule_1(axis_array,coeff_array,const_array);
      doAssignStmt(AlignArray(array_header, DVM000(iamv), iaxis, iaxis+rank,                                                                 iaxis+2*rank));
       // axis_array, coeff_array and const_array arn't used more
       SET_DVM(ileft);

       // doAssignTo(header_ref(p,rank+2),HeaderNplus1(p));
                                                   // calculating HEADER(rank+1) 
    
   
// Looking through the Align Tree of distributed array
   //algn_attr * attr;
   //align * root;
   if(p->numberOfAttributes(ALIGN_TREE)) {//there are any align statements
   algn_attr * attr;
   align * root;
   attr = (algn_attr *) p->attributeValue(0,ALIGN_TREE);
   root = attr->ref; // reference to root of align tree
    
   AlignTreeAlloc(root,stmt);
   }

   SET_DVM(ifst);
}

void   ALLOCATEf90DistArray(SgSymbol *p, SgExpression *desc, SgStatement *stdis, SgStatement *stmt) {

  int iamv,rank,iaxis,ileft,iright,ifst;
  SgExpression  *array_header, *size_array, *ps;            
  int ia,sign,re_sign;
  int idisars;
  SgType *type;
/*  
 if(p->variant() == FIELD_NAME)
 { SgExpression  *structRef ;
   structRef = &(struct_component->copy());
   array_header = new SgArrayRefExp(*p,*new SgValueExp(HEADER_SIZE(p)));
   structRef->setRhs(array_header); 
   array_header = structRef;
   
 } else
 */
  if(!HEADER(p)) return; 
  ifst = ndvm;

       //idisars = doDisRuleArrays(stdis,0,NULL);
  SgExpression *distr_rule_list = doDisRules(stdis,0,idisars);
  rank = Rank(p);
  if(ndis && rank && rank != ndis)
      Error ("Rank of array %s is not equal to  the length of the dist_format_list", p->identifier(), 110,stdis);
  type = p->type();
  size_array = doSizeAllocArray(p,desc,stmt,(idisars==-1 ? RTS2 : RTS1)); 
  array_header =  HeaderRef(p);
  ia = p->attributes();

  if(idisars == -1) //interface of RTS2
  {
    SgExpression *shadow_list = DeclaredShadowWidths(p);
    doCallStmt(DvmhArrayCreate(p,array_header,rank,ListUnion(size_array,shadow_list)));
    //doCallStmt(ScopeInsert(array_header));            
    if(!(ia & POSTPONE_BIT))  //distr_rule_list!=NULL
      doCallStmt(DvmhDistribute(p,rank,distr_rule_list)); // distribute dvm-array 
    SET_DVM(ifst);
    return;   
  } 

 // dvm000(i) = crtamv(AMRef, rank, SizeArray, StaticSign)
 // crtamv function creates current Abstract_Machine view  
  if((ia & SAVE_BIT) || saveall || (ia & COMMON_BIT) || p->scope()!=cur_func || IS_BY_USE(p))
    sign = 1;
  else
    sign = 0;  
  iamv = ndvm; 
  if(!(ia & POSTPONE_BIT)){
    doAssignStmt(CreateAMView(size_array, rank, sign));
    ps = PSReference(stdis);
    if(mult_block)
      doAssignStmt(MultBlock(DVM000(iamv), mult_block, ndis));  
    //dvm000(i) = genbli(PSRef, AMViewRef, AxisWeightArray, AxisCount) 
    // genbli sets on the weights of elements of processor system 
    if(gen_block == 1) 
      doAssignStmt(GenBlock(ps,DVM000(iamv), idisars+2*nblock,nblock));
    if(gen_block == 2) 
      doAssignStmt(WeightBlock(ps,DVM000(iamv),idisars+2*nblock, idisars+3*nblock,nblock));
    //dvm000(i) = DisAM(AMViewRef, PSRef, ParamCount, AxisArray,DistrParamArray) 
    // DisAM distributes resourses of parent (current) AM between children     
    doAssignStmt(DistributeAM(DVM000(iamv),ps,nblock,idisars,idisars+nblock));
    if(mult_block)
      doAssignStmt(MultBlock(DVM000(iamv), mult_block, 0));       
  }
   
  // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
  //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray)
  // function CrtDA creates system structures, doesn't allocate array

  //creating LeftBSizeArray and RightBSizeArray
  ileft = ndvm;
  iright = BoundSizeArrays(p);
  if(ia & DYNAMIC_BIT)
      re_sign = 3;
  else
      re_sign = 0;
  
  StoreLowerBoundsPlusOfAllocatable(p,desc);

  doAssignStmt(CreateDistArray(p,array_header,size_array,rank,ileft,iright,sign,re_sign));
  if(debug_regim && TestType(type)) 
      InsertNewStatementBefore(D_RegistrateArray(rank, TestType(type), GetAddresDVM(HeaderRefInd(p,1)),size_array,new SgVarRefExp(p)) ,stmt);

  if(ia & POSTPONE_BIT)
    { SET_DVM(ifst); return;}

  // dvm000(i) = AlgnDA (ArrayHandle,AMViewHandle,
  //                               Axis Array,Coeff Array),Const Array)
  //function AlgnDA alignes the array according to aligning template
  //actually AlgnDA distributes aligned array elements between virtual processors
 
  iaxis = ndvm;
  doAlignRule_1(rank);
  doAssignStmt(AlignArray(array_header, DVM000(iamv), iaxis, iaxis+rank, iaxis+2*rank));    

  SET_DVM(ifst);
}

void   ALLOCATEStructureComponent(SgSymbol *p, SgExpression *struct_e, SgExpression *desc, SgStatement *stmt) {

  int rank,ileft,iright,ifst;
  SgExpression  *array_header, *size_array;            
  int ia,sign,re_sign;
  SgType *type;
  SgExpression  *structRef, *struct_ , *struct_comp;
 // p->variant() == FIELD_NAME
  
  structRef = &(struct_e->copy());
  array_header = new SgArrayRefExp(*p, *new SgValueExp(1)); //*new SgValueExp(HEADER_SIZE(p)));
  structRef->setRhs(array_header); 
  array_header = structRef;
  ifst = ndvm;
  rank = Rank(p);
  type = p->type();
  size_array = doSizeAllocArray(p,desc,stmt,(INTERFACE_RTS2 ? RTS2:RTS1)); 
  if( INTERFACE_RTS2 ) // interface of RTS2
  {
      doCallStmt(DvmhArrayCreate(p,array_header,rank,ListUnion(size_array,DeclaredShadowWidths(p)))); 
      //doCallStmt(ScopeInsert(array_header));           
      return;    
  }
                       //interface of RTS1
  SgSymbol *s_struct = LeftMostField(struct_e)->symbol();
  ia = s_struct->attributes();
  if((ia & SAVE_BIT) || saveall || (ia & COMMON_BIT) || s_struct->scope()!=cur_func || IS_BY_USE(s_struct))
     sign = 1;
  else
     sign = 0;  
   
  // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
  //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray)
  // function CrtDA creates system structures, doesn't allocate array

  //creating LeftBSizeArray and RightBSizeArray
  ileft = ndvm;
  iright = BoundSizeArrays(p);
  if(p->attributes() & DYNAMIC_BIT)
      re_sign = 3;
  else
      re_sign = 0;
  
  struct_ = &(struct_e->copy());
  struct_ ->setRhs(NULL);    
  StoreLowerBoundsPlusOfAllocatableComponent(p,desc,struct_);

  doAssignStmt(CreateDistArray(p,array_header,size_array,rank,ileft,iright,sign,re_sign));
  struct_comp = &(struct_->copy());
  struct_comp->setRhs(new SgArrayRefExp(*p));
  if(debug_regim && TestType(type)) 
      InsertNewStatementBefore(D_RegistrateArray(rank, TestType(type), GetAddresDVM(header_ref_in_structure(p,1,struct_)),size_array,struct_comp) ,stmt);

  SET_DVM(ifst);
  return;
}


void AlignTreeAlloc( align *root,SgStatement *stmt) {
    align *node; 
    int nr,iaxis=-1,ia,*ix;
    SgStatement *stalgn;
    SgExpression *align_rule_list=NULL;
    stalgn = NULL;
 
    for(node=root->alignees; node; node=node->next) {
       if(IS_POINTER(node->symb)) //node is pointer must not be allocated
         continue;
       ix = ALIGN_RULE_INDEX(node->symb);
       if(ix)
         {iaxis = *ix; nr = *(++ix);}
       else {
         if (stalgn != node->align_stmt) {
           stalgn = node->align_stmt;
           iaxis = ndvm; ia = 0;
         }
         else
           ia = iaxis;
         align_rule_list = doAlignRules(node->symb,node->align_stmt,ia,nr);// creating axis_array,
       }                                          // coeff_array and  const_array
    
       AlignAllocArray(node,root, nr, iaxis, NULL, stmt);
       AlignTreeAlloc(node,stmt);
    } 
}
align *CopyAlignTreeNode(SgSymbol *ar)
{
     algn_attr * attr;
     align  *node, *node_copy;
     SgStatement *algn_st;
   
     attr = (algn_attr *)  ORIGINAL_SYMBOL(ar)->attributeValue(0,ALIGN_TREE);
     node = attr->ref; // reference to root of align tree
     node_copy = new align;
     node_copy->symb = ar;
     node_copy->align_stmt = node->align_stmt; 
     //algn_st = node->align_stmt;
     return(node_copy);
}

void   AllocateAlignArray(SgSymbol *p, SgExpression *desc, SgStatement *stmt) {
 int nr=0,iaxis=0,*ix=NULL,ifst=0;
 SgStatement *algn_st;
 SgSymbol *base, *pb;
 SgExpression *align_rule_list;
 align *node,*root=NULL, *node_copy;
 ifst = ndvm; 
 pb = ORIGINAL_SYMBOL(p);
 if(!pb->attributeValue(0,ALIGN_TREE))
    return;
 node = ((algn_attr *) pb->attributeValue(0,ALIGN_TREE))->ref;
 algn_st = node->align_stmt;
 node_copy = IS_BY_USE(p) ? CopyAlignTreeNode(p) : node;
 if(algn_st->expr(2)){
   base = (algn_st->expr(2)->variant()==ARRAY_OP) ? (algn_st->expr(2))->rhs()->symbol() : (algn_st->expr(2))->symbol();// align_base symbol
   root = ((algn_attr *) base->attributeValue(0,ALIGN_TREE))->ref;
 }
 if(IS_ALLOCATABLE_POINTER(p)){
   AlignAllocArray(node_copy,root,0,0,desc,stmt);
   return;
 } 
/* 
 if(!algn_st->expr(2)){ //postponed aligning
   root = NULL;
   if(IS_ALLOCATABLE_POINTER(p)){
     AlignAllocArray(node,root,0,0,desc,stmt);
     return;
   } 
 }
 else {
 base = (algn_st->expr(2)->variant()==ARRAY_OP) ? (algn_st->expr(2))->rhs()->symbol() : (algn_st->expr(2))->symbol();// align_base symbol
 root = ((algn_attr *) base->attributeValue(0,ALIGN_TREE))->ref;
  
 if(IS_ALLOCATABLE_POINTER(p)){
   AlignAllocArray(node,root,0,0,desc,stmt);
   return;
 } 
*/
 if(root) {
   LINE_NUMBER_BEFORE(stmt,stmt); // for tracing set the global variable of LibDVM to
                                // line number of statement(stmt)
   ix = ALIGN_RULE_INDEX(p);
   if(ix)
      {iaxis = *ix; nr = *(++ix);}
   else {
     iaxis = ndvm;
     align_rule_list = doAlignRules(p,algn_st,0,nr);
   }
 }
 //sheap = heap_ar_decl ? heap_ar_decl->symbol() : p;//heap_ar_decl == NULL is user error
 //doAssignTo(stmt->expr(0), ARRAY_ELEMENT(sheap,1)); 
                                    // P = HEAP(1) or P(I) = HEAP(1)
 if(!stmt->expr(0)->lhs())     // case P
        doAssignTo(stmt->expr(0), new SgValueExp(POINTER_INDEX(p))); 
                                                  // P = <heap-index> or P(I) = <heap-index>
      else {                   // case P(I,...)
        doAssignTo(stmt->expr(0), HeapIndex(stmt)); 
      }  
 //doAssignTo( ARRAY_ELEMENT(sheap, 1), &(* ARRAY_ELEMENT(sheap, 1) + *new SgValueExp(HEADER_SIZE(p))));                       
              //HEAP(1) = HEAP(1) + <header_size>
 //doLogIfForHeap(sheap, heap_size);  //IF(HEAP(1) > heap_size) STOP 'HEAP limit is exceeded'

 AlignAllocArray(node,root,nr,iaxis,desc,stmt);
 AlignTreeAlloc(node,stmt);
 SET_DVM(ifst);
}

void    AlignAllocArray(align *node, align *root, int nr, int iaxis,SgExpression *desc, SgStatement *stmt) {

// 1) creates Distributed Array for "node"
// 2) alignes Distributed Array with Distributed Array for "root" or with
//    Template 
  
  int rank,ileft,iright,isize;
  int sign,re_sign,ia;
  SgSymbol *als;
  SgExpression *array_header,*size_array,*pref, *arglist, *lbound;
  SgExpression *align_rule_list;
  SgType *type;
 
  als = node->symb;
  ia = als->attributes();
 
  if(!HEADER(ORIGINAL_SYMBOL(als))){
    Error("Array '%s' may not be allocated", als->identifier(),124,node->align_stmt);
    return;
  }
  if(IS_TEMPLATE(als) || IS_DUMMY(als) || (IN_COMMON(als) && !IS_POINTER(als) && !IS_ALLOCATABLE_POINTER(als)))
    return;    

  if(IS_SAVE(als)) {  // has SAVE attribute    
    if(root && !IS_TEMPLATE(root->symb) && !IN_COMMON(root->symb) && !HAS_SAVE_ATTR(root->symb) && CURRENT_SCOPE(root->symb) ) {
      Error("Aligned array '%s' has SAVE attribute but align-target has not", als->identifier(),119,node->align_stmt);
      return;
    }
    SgStatement *ifst;   
    ifst = doIfThenConstr(als);
    where = ifst->lexNext();  // reffer to ENDIF statement   
  }      
  LINE_NUMBER_BEFORE(stmt,where); 
  rank = Rank(als);

  if(INTERFACE_RTS2) { //interface of RTS2
    size_array = NULL;
    array_header = HeaderRef(als);
    if(IS_ALLOCATABLE_POINTER(als))  
      size_array = doSizeAllocArray(als, desc, stmt, RTS2);
    else if(!IS_POINTER(als))
      size_array = doDvmShapeList(als,node->align_stmt);  
    doCallStmt(DvmhArrayCreate(als,array_header,rank,ListUnion(size_array,DeclaredShadowWidths(als))));
    //doCallStmt(ScopeInsert(array_header));
    align_rule_list = root ? doAlignRules(node->symb,node->align_stmt,0,nr) : NULL;
    if( root && align_rule_list)     //!(ia & POSTPONE_BIT) 
      doCallStmt(DvmhAlign(als,root->symb,nr,align_rule_list));
    if(IS_SAVE(als))
      where = where->lexNext();
    return;
  }
                      //interface of RTS1
  isize = ndvm;
  if(IS_POINTER(als)){
    size_array = ReverseDim(desc,rank);
    pref = where->expr(0);
    array_header =  PointerHeaderRef(pref,1);
    type = PointerType(als);
  } else if(IS_ALLOCATABLE_POINTER(als)) {
    size_array = doSizeAllocArray(als, desc, stmt, RTS1);
    pref = NULL;
    array_header = HeaderRef(als);    
    type = als->type();
  } else {
    size_array = doSizeArray(als, node->align_stmt );
    pref = NULL;
    array_header = HeaderRef(als);
    type = als->type();
  }
 
  ileft = ndvm;
  iright= BoundSizeArrays(als);
  if((ia & SAVE_BIT) || saveall || (ia & COMMON_BIT) || als->scope()!=cur_func || IS_BY_USE(als))
     sign = 1;
  else
     sign = 0;  

  if(ia & DYNAMIC_BIT)
     re_sign = 2;
   else
     re_sign = 0;  
  //re_sign = 0; aligned array may not be redisributed 
  if(IS_ALLOCATABLE_POINTER(als)) {
      StoreLowerBoundsPlusOfAllocatable(als,desc);
      iaxis = ndvm;
      if(root)  //!(ia & POSTPONE_BIT)       
        align_rule_list = doAlignRules(node->symb,node->align_stmt,0,nr); //nr = doAlignRule(als,node->align_stmt,0);
  }
  else {
      arglist= stmt->expr(1)->lhs();
      lbound=0;
      if(arglist->rhs() && arglist->rhs()->rhs() && arglist->rhs()->rhs()->rhs() ) {//there are 3-nd and 4-nd argument of ALLOCATE function call
        SgExpression *heap;
        lbound =  arglist->rhs()->rhs()->lhs();  //lower bound array reference ??
        heap   =  arglist->rhs()->lhs();   //heap array reference ??
        if(heap  && isSgArrayRefExp(heap) && !heap->lhs() && lbound  && isSgArrayRefExp(lbound)) 
             ;
        else
          lbound = 0;
      }
      if(!lbound)
        StoreLowerBoundsPlus(als,pref);
      else
        StoreLowerBoundsPlusFromAllocate(als,pref,lbound);
}

  // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
  //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray) 
  // function CrtDA creates system structures, dosn't allocate array    
  doAssignStmt(CreateDistArray(als, array_header, size_array,rank,ileft,iright,sign,re_sign)); 
  if( debug_regim  && TestType(type)) {
     if(IS_POINTER(als) ){ 
        SgExpression *heap;
	if(stmt->expr(1)->lhs()->rhs()) {//there is 2-nd argument of ALLOCATE function call
           heap =  stmt->expr(1)->lhs()->rhs()->lhs(); //heap array reference
           if(heap  && isSgArrayRefExp(heap) && !heap->lhs())
             InsertNewStatementBefore(D_RegistrateArray(rank, TestType(PointerType(als)), GetAddresDVM(array_header),size_array,stmt->expr(0) ) ,stmt);
        }
     } else if(IS_ALLOCATABLE_POINTER(als))
             InsertNewStatementBefore(D_RegistrateArray(rank, TestType(type), GetAddresDVM(HeaderRefInd(als,1)),size_array,new SgVarRefExp(als)),stmt);
       else
             InsertNewStatementBefore(D_RegistrateArray(rank, TestType(type), GetAddresDVM(HeaderRefInd(als,1)),size_array,new SgVarRefExp(als)),where);
  }
  if(root) // non postponed aligning ((ia & POSTPONE_BIT)==0)

    // dvm000(i) = AlgnDA (ArrayHeader,PatternRef,
    //                               Axis Array,Coeff Array,Const Array)
    doAssignStmt(AlignArray(array_header,HeaderRef(root->symb),
                              iaxis, iaxis+nr,iaxis+2*nr));

    //doAssignTo(header_ref(als,rank+2),HeaderNplus1(als));//calculating HEADER(rank+1)
  SET_DVM(isize);
  if(IS_SAVE(als))
    where = where->lexNext();  
}

void    PostponedAlignArray(align *node, align *root, int nr, int iaxis) {

// 1) creates Distributed Array for "node"
// 2) alignes Distributed Array with Distributed Array for "root" 
  
  int rank,ileft,iright,isize;
  int sign,re_sign,ia;
  SgSymbol *als;
  SgExpression *array_header,*size_array;
 
  als = node->symb;
  ia = als->attributes();
 
  if(!HEADER(als)){
    Error("Array '%s' may not be aligned", als->identifier(),125,node->align_stmt);
    return;
  }
  if(IS_TEMPLATE(als) || IS_DUMMY(als) || IN_COMMON(als))
    return;    

  if(IS_SAVE(als)) {  // has SAVE attribute
    if(root && !IS_TEMPLATE(root->symb) && !IN_COMMON(root->symb) && !HAS_SAVE_ATTR(root->symb) && CURRENT_SCOPE(root->symb) ) {
      Error("Aligned array '%s' has SAVE attribute but align-target has not", als->identifier(),119,node->align_stmt);
      return;
    }
    SgStatement *ifst;   
    ifst = doIfThenConstr(als);
    where = ifst->lexNext();  // reffer to ENDIF statement   
  }      
  LINE_NUMBER_BEFORE(node->align_stmt,where); 
                                        // for tracing set the global variable of LibDVM to
                                        // line number of ALIGN directive 
  array_header = HeaderRef(als);
  isize = ndvm;
  size_array = doSizeArray(als, node->align_stmt );
  rank = Rank(als);
  ileft = ndvm;
  iright= BoundSizeArrays(als);
  if((ia & SAVE_BIT) || saveall)
     sign = 1;
  else
     sign = 0;  

  if(ia & DYNAMIC_BIT)
     re_sign = 2;
   else
     re_sign = 0;  

  StoreLowerBoundsPlus(als,NULL);

  // dvm000(i) = CrtDA (ArrayHeader,Base,Rank,TypeSize,SizeArray,
  //                StaticSign,ReDistrSign, LeftBSizeArray,RightBSizeArray) 
  // function CrtDA creates system structures, dosn't allocate array    
  doAssignStmt(CreateDistArray(als, array_header, size_array,rank,ileft,iright,sign,re_sign)); 

  // dvm000(i) = AlgnDA (ArrayHeader,PatternRef,
  //                               Axis Array,Coeff Array,Const Array)
  doAssignStmt(AlignArray(array_header,HeaderRef(root->symb),
                              iaxis, iaxis+nr,iaxis+2*nr));
  SET_DVM(isize);
  if(IS_SAVE(als))
    where = where->lexNext();  
}

void Template_Create(SgStatement *stmt)
{
   SgExpression *el;
   int isave = ndvm;
   for(el = stmt->expr(0); el; el=el->rhs())
   {
      if(isSgArrayRefExp(el->lhs()))
      {  
         SgSymbol *s = el->lhs()->symbol();
         int rank = Rank(s);
         if(!HEADER(s))
         {
            Error("'%s' has not DISTRIBUTE attribute ", s->identifier(), 637,stmt); 
            continue;
         } 
         if(!(s->attributes() & POSTPONE_BIT))
         {
            Error("Template '%s' has no postponed distribution", s->identifier(), 638,stmt); 
            continue;
         } 
         if(!DEFERRED_SHAPE_TEMPLATE(s))
         {
            Error("Template '%s' has no deferred shape", s->identifier(), 640,stmt); 
            continue;
         }         
         where = stmt;
         SgExpression *size_array = doSizeAllocArray(s, el->lhs(), stmt, (INTERFACE_RTS2 ? RTS2 : RTS1));
         cur_st = stmt;
         if(INTERFACE_RTS2)
         {
            doCallAfter(DvmhTemplateCreate(s,HeaderRef(s),rank,size_array));
            //doCallAfter(ScopeInsert(HeaderRef(s)));
         }
         else
         {
            doAssignTo_After(DVM000(INDEX(s)),CreateAMView(size_array, rank, 1));
            where = cur_st; 
            StoreLowerBoundsPlusOfAllocatable(s,el->lhs());
         }
      }
      else
      {
         err("Illegal element of list",636,stmt);
         continue;
      }
   }
   SET_DVM(isave);
}

void Template_Delete(SgStatement *stmt)
{
   SgExpression *el;
   for(el = stmt->expr(0); el; el=el->rhs())
   {
      if(isSgArrayRefExp(el->lhs()))
      {  
         SgSymbol *s = el->lhs()->symbol();
         if(!HEADER(s))
         {
            Error("'%s' has not DISTRIBUTE attribute ", s->identifier(), 637,stmt); 
            continue;
         } 
         if(!DEFERRED_SHAPE_TEMPLATE(s))
         {
            Error("Template '%s' has no deferred shape", s->identifier(), 640,stmt); 
            continue;
         }  
 
         doCallAfter(DeleteObject_H(HeaderRef(s)));
      }
      else
      {
         err("Illegal element of list",636,stmt);
         continue;
      }
   }
}   

SgExpression * dvm_array_ref () {
// creates array reference: dvm000(i) , i - index of first free element
   SgValueExp * index = new SgValueExp(ndvm);
   return( new SgArrayRefExp(*dvmbuf, *index));
}

SgExpression * dvm_ref (int n) {
// creates array reference: dvm000(n) 
   SgValueExp * index = new SgValueExp(n);
   return( new SgArrayRefExp(*dvmbuf, *index));
}


void Align_Tree(align *root) {
  align *p;
  if (!root) 
     return;

// looking through alignees of the root
  for(p=root->alignees; p; p=p->next)
  {
   //printf(" %s is aligned with %s (statement at line %d)\n", p->symb->identifier(), root->symb->identifier(), p->align_stmt->lineNumber());
     Align_Tree(p);
   }
  return;
}

stmt_list  *addToStmtList(stmt_list *pstmt, SgStatement *stat)
{
// adding the statement to the beginning of statement list 
// pstmt-> stat -> stmt-> ... -> stmt     
    stmt_list * stl; 
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

stmt_list  *delFromStmtList(stmt_list *pstmt)
{
// deletinging last statement from the statement list 
// pstmt-> stat -> stmt-> ... -> stmt
    pstmt = pstmt->next;     
    return (pstmt);
}

void RenamingDvmArraysByUse(SgStatement *stmt)
{
   SgSymbol *ar;
  SgExpression *e = stmt->expr(0), *el;
  
  if(e && e->variant()==ONLY_NODE)
    e = e->lhs();
  for(el=e; el; el=el->rhs())
  {
    ar = el->lhs()->lhs()->symbol(); 
    if(!IS_DVM_ARRAY(ar)) continue;
    // if(el->lhs()->rhs())
    if(strcmp(ar->identifier(),ORIGINAL_SYMBOL(ar)->identifier())) //case of renaming in a use statement
    {  //printf("%s  %s  SCOPE: %s\n", ar->identifier(),ORIGINAL_SYMBOL(ar)->identifier(),ar->scope()->symbol()->identifier());
      //adding the distributed array symbol 'ar' to symb_list 'dsym'
      if(!(ar->attributes() & DVM_POINTER_BIT))
        AddDistSymbList(ar);
      // creating variables used for optimisation array references in parallel loop
      coeffs *scoef  = new coeffs;
      CreateCoeffs(scoef,ar);
      // adding the attribute (ARRAY_COEF) to distributed array symbol
      ar->addAttribute(ARRAY_COEF, (void*) scoef, sizeof(coeffs));
    }
  }
}

void ArrayHeader (SgSymbol *ar,int ind)
{
// creating  header of distributed array: HEADER(0:N+1),
// N - rank of array
  // Rank+1 elements for DVM system
  // and 1 element for F_DVM

  int *index = new int;
  int * count = new int;
  coeffs *scoef  = new coeffs;
  SgSymbol **base = new (SgSymbol *);
  SgType *btype;
  
  if(IS_BY_USE(ar)) 
     return;
  
  if(HEADER(ar)) {
     Err_g("Illegal aligning of '%s'", ar->identifier(),126);
     return;
  }
  btype = Base_Type(ar->type());

 /*  
  if(btype->variant() == T_STRING)
     Err_g("Illegal type of '%s'", ar->identifier(),141);
 */ /* podd 13.01.12 */

  if( ar->attributes() & DATA_BIT )
    Err_g("Distributed object may not be initialized (in DATA statement): %s", ar->identifier(), 265);
  if(!(ar->attributes() & DIMENSION_BIT) && !(ar->attributes() &  DVM_POINTER_BIT))
      Err_g("Distributed object '%s' is not array", ar->identifier(),127); 
  if(ar->attributes() & DVM_POINTER_BIT)
    //TypeMemory(PointerType(ar)); // marking  type memory use 
    TypeMemory(SgTypeInt()); // marking  type memory use  
  else if(!(ar->attributes() & TEMPLATE_BIT) )  //ind == 1 
  {  
     TypeMemory(btype); // marking  type memory use
     if(TypeIndex(btype) == -1 &&  btype->variant()!=T_DERIVED_TYPE)
                  //if(TypeSize(btype) != TypeSize(baseMemory(btype)->type()->baseType()))
        Err_g("Illegal type of '%s'", ar->identifier(),141);
  } 
//adding the distributed array symbol 'ar' to symb_list 'dsym'
 if(!(ar->attributes() & DVM_POINTER_BIT))
   AddDistSymbList(ar);


 *index = ind;
// adding the attribute (ARRAY_HEADER) to distributed array symbol
  ar->addAttribute(ARRAY_HEADER, (void*) index, sizeof(int));
 *count = 0;
// adding the attribute (BUFFER_COUNT) to distributed array symbol
// counter of remote group buffers
  ar->addAttribute(BUFFER_COUNT, (void*) count, sizeof(int));
// creating variables used for optimisation array references in parallel loop
  CreateCoeffs(scoef,ar);
// adding the attribute (ARRAY_COEF) to distributed array symbol
  ar->addAttribute(ARRAY_COEF, (void*) scoef, sizeof(coeffs));
//creating base variable 
  if(opt_base) {
 *base= BaseSymbol(ar);
// adding the attribute (ARRAY_BASE) to distributed array symbol
  ar->addAttribute(ARRAY_BASE, (void*) base, sizeof(SgSymbol *));
  }
}

int Rank (SgSymbol *s)
{
  SgArrayType *artype;
  if(IS_POINTER(s))
    return(PointerRank(s));
  artype=isSgArrayType(s->type());
  if(artype)
    return (artype->dimension());
  else
    return (0);
}

SgExpression *doSizeArrayQuery(SgExpression *headref,int rank) 
{int ind,i;
  ind = ndvm;
  for(i=1; i<=rank ; i++) 
    doAssignStmt(GetSize(headref,i));
  return(DVM000(ind));
}

SgExpression *doDvmShapeList(SgSymbol *ar, SgStatement *st)   /* RTS2 */
{
  SgExpression *l_bound, *u_bound, *pe, *result=NULL;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;
  int i;
  artype = isSgArrayType(ar->type());
  if((! artype) || (!(ar->attributes() & DIMENSION_BIT)))  {//isn't array
    ndim = 0;
    return (NULL); 
  }
  ndim = artype->dimension();
  for(i=0; i<ndim ; i++) {   
    pe = artype->sizeInDim(i);
    if ((sbe=isSgSubscriptExp(pe)) != NULL) {

      if(!sbe->ubound()) {
         Error("Illegal array shape: %s",ar->identifier(), 162,st);
         u_bound = &(c1.copy());
      }    
      else if(sbe->ubound()->variant() == STAR_RANGE) {// ubound = *
	 Error("Assumed-size array: %s",ar->identifier(), 162,st);
         u_bound = &(c1.copy());
      } 
      else 
         u_bound =  &((sbe->ubound())->copy());
      if(sbe->lbound())
         l_bound = &((sbe->lbound())->copy());     
      else  if(sbe->ubound())
         l_bound = &(c1.copy());  
      else {
         Error("Illegal array shape: %s",ar->identifier(), 162,st);
         l_bound = &(c1.copy());  
      }
    }
    else {
         if(pe->variant() == STAR_RANGE) // dim=ubound = *
	    Error("Assumed-size array: %s",ar->identifier(),162,st); 
         u_bound = &(pe->copy());
         l_bound = &(c1.copy());    
    }
    //reversing dimensions for LibDVM
    result = AddElementToList(result, DvmType_Ref(Calculate(u_bound)));
    result = AddElementToList(result, DvmType_Ref(Calculate(l_bound)));
  }
  return(result);
}

SgExpression *doShapeList(SgSymbol *ar, SgStatement *st)   /* RTS2 */
{
  SgExpression *l_bound, *u_bound, *pe, *result=NULL;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;
  int i;
  artype = isSgArrayType(ar->type());
  if((! artype) || (!(ar->attributes() & DIMENSION_BIT)))  {//isn't array
    ndim = 0;
    return (NULL); 
  }
  ndim = artype->dimension();
  for(i=0; i<ndim ; i++) {   

    pe = artype->sizeInDim(i);
    if(IS_BY_USE(ar)) {
         u_bound = UBOUNDFunction(ar,i+1);
         l_bound = LBOUNDFunction(ar,i+1);  
    }
    else if ((sbe=isSgSubscriptExp(pe)) != NULL) {
      if(sbe->ubound() && (sbe->ubound()->variant() == INT_VAL || sbe->ubound()->variant() == CONST_REF) && (!sbe->lbound() || sbe->lbound() && (sbe->lbound()->variant() == INT_VAL || sbe->lbound()->variant() == CONST_REF))) {
         u_bound =  &((sbe->ubound())->copy());
         if(sbe->lbound())
           l_bound = &((sbe->lbound())->copy());     
         else  
           l_bound = &(c1.copy());     
      }
      else {
         u_bound = UBOUNDFunction(ar,i+1);
         l_bound = LBOUNDFunction(ar,i+1);  
      }
    }
    else
    {
       if(pe->variant() == INT_VAL || pe->variant() == CONST_REF) {
         u_bound = &(pe->copy());
         l_bound = &(c1.copy());    
       }
       else {
         u_bound = UBOUNDFunction(ar,i+1);
         l_bound = LBOUNDFunction(ar,i+1);  
       }  
    }
    //reversing dimensions for LibDVM
    result = AddElementToList(result, DvmType_Ref(u_bound));
    result = AddElementToList(result, DvmType_Ref(l_bound));

  }  
  return(result);
}


SgExpression * doSizeFunctionArray(SgSymbol *ar, SgStatement *st)
{
  SgExpression *esize, *pe, *result;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;
  int i,n;

//allocating SizeArray and setting on it
  result = dvm_array_ref();     // SizeArray reference
  artype = isSgArrayType(ar->type());
  if((! artype) || (!(ar->attributes() & DIMENSION_BIT)))  {//isn't array
    ndim = 0;
    return (result);
  }
  ndim =  n = artype->dimension();
  for(i=n-1; i>=0 ; i--) {   //reversing dimensions for LibDVM
    pe = artype->sizeInDim(i);
    if ((sbe=isSgSubscriptExp(pe)) != NULL) {

      if(!sbe->ubound() || sbe->ubound()->variant() == STAR_RANGE)           
          esize = SizeFunction(ar,i+1);
      else 
          if(sbe->lbound())
             esize = &(((sbe->ubound())->copy()) - ((sbe->lbound())->copy()) + c1);
          else
             esize = &((sbe->ubound())->copy());      
    }
    else 
       esize = &(pe->copy());
    

// dvm000(N+j) = size_in_dimension_(n-j)
    esize = Calculate( esize);
    if(esize->variant()!=INT_VAL)
         esize = SizeFunction(ar,i+1);
    doAssignStmt(esize);
   }
   return (result);
}


SgExpression * doSizeArray(SgSymbol *ar, SgStatement *st)
{
  SgExpression *esize, *pe, *result;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;
  int i,n;

//allocating SizeArray and setting on it
  result = dvm_array_ref();     // SizeArray reference
  artype = isSgArrayType(ar->type());
  if((! artype) || (!(ar->attributes() & DIMENSION_BIT)))  {//isn't array
    ndim = 0;
    //Error (" Distributed object %s isn't declared as array\n", ar->identifier(),st);
    return (result);
  }
  ndim =  n = artype->dimension();
  for(i=n-1; i>=0 ; i--) {   //reversing dimensions for LibDVM
    pe = artype->sizeInDim(i);
    if ((sbe=isSgSubscriptExp(pe)) != NULL) {

      if(!sbe->ubound()) {
          Error("Illegal array shape: %s",ar->identifier(), 162,st);
          esize = &(c1.copy()); //SizeFunction(ar,i+1);
      }
      else if(sbe->ubound()->variant() == STAR_RANGE) {// ubound = *
	  Error("Assumed-size array: %s",ar->identifier(), 162,st);
          esize = &(sbe->ubound()->copy());
      } 
      else 
          if(sbe->lbound())
             esize = &(((sbe->ubound())->copy()) - ((sbe->lbound())->copy()) + c1);
          else
             esize = &((sbe->ubound())->copy());      
    }
    else {
       if(pe->variant() == STAR_RANGE) // dim=ubound = *
	  Error("Assumed-size array: %s",ar->identifier(),162,st); 
       esize = &(pe->copy());
    }

// dvm000(N+j) = size_in_dimension_(n-j)
    doAssignStmt(Calculate( esize));
   }
   return (result);
}

SgExpression * doSizeArrayD(SgSymbol *ar, SgStatement *st)
{
  SgExpression *esize, *pe, *result;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;
  int i,n;
  if(st)
    ;
//allocating SizeArray and setting on it
  result = dvm_array_ref();     // SizeArray reference
  artype = isSgArrayType(ar->type());
  if((! artype) || (!(ar->attributes() & DIMENSION_BIT)))  {//isn't array
    ndim = 0;
    //Error (" Distributed object %s isn't declared as array\n", ar->identifier(),st);
    return (result);
  }
  ndim =  n = artype->dimension();
  for(i=0; i<n; i++) { //direct order of dimensions
    pe = artype->sizeInDim(i);
    if ((sbe=isSgSubscriptExp(pe)) != NULL)
      esize = &(((sbe->ubound())->copy()) - ((sbe->lbound())->copy()) + c1);
    else
// !!! test : ubound = *
      esize = &(pe->copy());
// dvm000(N+j) = size_in_dimension(j)
    doAssignStmt(Calculate( esize));
  }
   return (result);
}

SgExpression * doSizeAllocArray(SgSymbol *ar, SgExpression *desc, SgStatement *st, int RTS_flag)
{
  SgExpression *pe, *result, *size[MAX_DIMS], *el;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;
  int i,n;

//allocating SizeArray and setting on it
  result = RTS_flag == 1 ? dvm_array_ref() : NULL;     // SizeArray reference/Shape list
  artype = isSgArrayType(ar->type());
  if((! artype) || (!(ar->attributes() & DIMENSION_BIT)))  {//isn't array
    ndim = 0;
    return (result);
  }
  ndim =  artype->dimension();
  if(!desc->lhs())
    Error("No allocaton specifications for %s",ar->identifier(),293,st);
  if(!TestMaxDims(desc->lhs(), ar, st))
    return(result);
  for(el=desc->lhs(),n=0; el; el=el->rhs(),n++){
   pe = el->lhs();
   if((sbe=isSgSubscriptExp(pe)) != NULL)
   { 
     if(RTS_flag == RTS1)
       size[n] = &(((sbe->ubound())->copy()) - ((sbe->lbound())->copy()) + c1);
     else //RTS2
     {
       result = AddElementToList(result, DvmType_Ref(Calculate(sbe->ubound())));
       result = AddElementToList(result, DvmType_Ref(Calculate(sbe->lbound())));     
     }  
   }
   else
     if(RTS_flag == RTS1)
       size[n] = &(pe->copy());
     else //RTS2
     {
       result = AddElementToList(result, DvmType_Ref(Calculate(pe)));
       result = AddElementToList(result, DvmType_Ref(Calculate(&c1)));     
     }  

  }
  if(ndim != n)
    Error("Rank of array '%s' is not equal the length of allocation-specification-list",ar->identifier(),292,st);
  if(RTS_flag == RTS1)
  {
     for(i=n-1; i>=0 ; i--)    //reversing dimensions for LibDVM
        doAssignStmt(Calculate( size[i]));
  }
  return (result);
}


SgExpression * ArrayDimSize(SgSymbol *ar, int i)
{
// i= 1,...,Rank
  SgExpression *esize,*pe;
  SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgArrayType *artype;

   if(IS_POINTER(ar))
     return(UpperBound(ar,i-1)); // lower bound = 1

   if(!(ar->attributes() & DIMENSION_BIT)){// Error isn't array
      ndim = 0;
      return (NULL);
  }
   artype = isSgArrayType(ar->type());
   /*
          if(! artype)   { // Error: isn't array
            ndim = 0;
            return (NULL);
          }
  */
    pe = artype->sizeInDim(i-1);
    if ((sbe=isSgSubscriptExp(pe)) != NULL){
      if(!sbe->ubound())
          esize = SizeFunction(ar,i);
      else if(sbe->ubound()->variant() == STAR_RANGE) {// ubound = *
	//Error("Assumed-size array: %s",ar->identifier(),cur_st);
          esize = &(sbe->ubound()->copy());
      } 
      else 
        if(sbe->lbound())
          esize = &(((sbe->ubound())->copy()) - ((sbe->lbound())->copy()) + c1);
        else
          esize = &((sbe->ubound())->copy());      
    }
    else
      //if(pe->variant() == STAR_RANGE) // dim=ubound = *
	// Error("Assumed-size array: %s",ar->identifier(),cur_st); 
         esize = &(pe->copy());

   return (esize);
}


SgSymbol * baseMemory(SgType *t) 
{
  TypeMemory(t);   //14.03.03
  if(t->variant() == T_DERIVED_TYPE)
     return  baseMemoryOfDerivedType(t) ;
  int Tind = TypeIndex(t);  //21.04.15
  if(Tind != -1)
     return  mem_symb[Tind] ;
  else
  {  //Err_g ("There is not dvm-base for array %s", " ", 616); 
     return  mem_symb[Integer] ;
  }   
   
}

SgSymbol *baseMemoryOfDerivedType(SgType *t)
{SgSymbol *stype;
 base_list *el;
 stype = t->symbol();
 for(el=mem_use_structure; el; el = el->next)
   if(el->type_symbol == stype)  return(el->base_symbol);
 Error("Can not define base memory symbol for %s",stype->identifier(),333,cur_st);
 return(Imem);//error
}

void TypeMemory(SgType *t) 
{
  if(t->variant() == T_DERIVED_TYPE)
     DerivedTypeMemory(t);
  int tInd = TypeIndex(t);
  
  if(tInd != -1) 
     mem_use[tInd] = 1;
                      
}

void DerivedTypeMemory(SgType *t)
{SgSymbol *stype;
 base_list *el;

 stype = t->symbol();
 for(el=mem_use_structure; el; el = el->next)
 {  if(el->type_symbol == stype) 
    {  if(!el->base_symbol)
          el->base_symbol = DerivedTypeBaseSymbol(stype,t);
       return;
    }
 }
 el = new base_list;
 el->type_symbol = stype;
 el->base_symbol = DerivedTypeBaseSymbol(stype,t);
 el->gpu_symbol = NULL;
 el->next=mem_use_structure;
 mem_use_structure = el; 
}


int IntrinsicTypeSize(SgType *t)
{
    switch(t->variant()) {
      case T_INT:
      case T_BOOL:     return (len_int ?  len_int  : default_integer_size);
      case T_FLOAT:    return (len_int ?  len_int  : default_real_size);
      case T_COMPLEX:  return (len_int ? 2*len_int : 2*default_real_size);
      case T_DOUBLE:   return (len_int ? 2*len_int : 8);

      case T_DCOMPLEX: return(16);

      case T_STRING:                     
      case T_CHAR:
                       return(1);
      default:
                       return(0);
    }
}

SgExpression * TypeLengthExpr(SgType *t)
{
  SgExpression *len;
  SgExpression *selector;
  if(t->variant() == T_DERIVED_TYPE) return(new SgValueExp(StructureSize(t->symbol())));
  len = TYPE_RANGES(t->thetype) ? t->length() : NULL;
  selector = TYPE_KIND_LEN(t->thetype) ? t->selector() : NULL;
     // printf("\nTypeSize");
     // printf("\nranges:"); if(len) len->unparsestdout();
     // printf("\nkind_len:");  if(selector) selector->unparsestdout();
  if(!len && !selector) //the number of bytes is not specified in type declaration statement
    return (new SgValueExp(IntrinsicTypeSize(t)));
  else if(len && !selector)   //INTEGER*2,REAL*8,CHARACTER*(N+1)
    return(Calculate(len));
  else
    return(Calculate(LengthOfKindExpr(t, selector, len))); //specified kind or/and len
}

SgExpression *LengthOfKindExpr(SgType *t, SgExpression *se, SgExpression *le)
{
  switch(t->variant()) {
      case T_INT:
      case T_FLOAT:
      case T_BOOL:
      case T_DOUBLE:
             return(se->lhs());
      case T_COMPLEX:
      case T_DCOMPLEX:
             return(&(*new SgValueExp(2) * (*(se->lhs()))));
      case T_CHAR:
      case T_STRING:
	{   SgExpression *length, *kind;	    
	    if(se->rhs() && se->rhs()->variant() == LENGTH_OP ) {
              length = se->rhs()->lhs();
              kind   = se->lhs()->lhs(); 
            }
            else if(se->rhs() && se->rhs()->variant() != LENGTH_OP){
              length = se->lhs()->lhs();
              kind   = se->rhs()->lhs(); 
            } 
            else {
              length = se->lhs();
              kind = NULL;
            }
            length = le ? le : length;
            if(kind)
               return(&(*length * (*kind)));
              //return(Calculate(length)->valueInteger() * Calculate(kind)->valueInteger());
            else
               return(length);
              //return(Calculate(length)->valueInteger());

	    /*length = se->rhs() ? (se->rhs()->variant() == LENGTH_OP ? se->rhs()->lhs() : se->lhs()->lhs()) : se->lhs();
	    length = le ? le : length;
            if(se->rhs()) // specified KIND and LEN
              return((se->lhs()->lhs()->valueInteger()) * (se->rhs()->lhs()->valueInteger()) ); //kind*len
            else
	    return(se->lhs()->valueInteger()); */ 
        }
 
      default:   
              return(NULL);
  }
}

int TypeSize(SgType *t) 
{
  SgExpression *le;
  int len;
  if(IS_INTRINSIC_TYPE(t))            return (IntrinsicTypeSize(t));
  if(t->variant() == T_DERIVED_TYPE)  return (StructureSize(t->symbol()));
  if((len = NumericTypeLength(t)))    return(len);
  le = TypeLengthExpr(t);
  if(le->isInteger()){
    len = le->valueInteger();
    len = len < 0 ? 0 : len; //according to standard F90
  } else
    len = -1; //may be error situation
  return(len);
}

SgExpression *StringLengthExpr(SgType *t, SgSymbol *s)
{ SgExpression *le;
 le = TypeLengthExpr(t);
 if (isSgKeywordValExp(le))
    le = LENFunction(s);
 if (le->lhs() && isSgKeywordValExp(le->lhs()))
    le->setLhs(LENFunction(s));
 return(le);
}

int NumericTypeLength(SgType *t)
{ SgExpression *le;
  SgValueExp *ve;
 if(t->variant() == T_STRING)   return (0);
 if(TYPE_RANGES(t->thetype)){
   le = t->length();
   if((ve =isSgValueExp(le)))
      return (ve->intValue());
   else
      return (0);
 }
 if(TYPE_KIND_LEN(t->thetype) ) {
   le = t->selector()->lhs();
   if((ve=isSgValueExp(le)))
     if(t->variant() == T_COMPLEX || t->variant() == T_DCOMPLEX)
       return (2*ve->intValue());
     else
       return (ve->intValue());
   else
      return (0);
 } 
   return(0);
}

int StructureSize(SgSymbol *s)
{ //SgClassSymb *sc;
  //SgFieldSymb *sf;
  SgSymbol *sf;
  //SgType *type;
  // SgExpression *le; 
  int n;
  int size;
  size = 0;
    //n = ((SgClassSymb *) s)->numberOfFields();
    //for(i=0;i<n;i++) {
    //for(sf=((SgClassType *)(s->type()))->fieldSymb(1);sf;sf=((SgFieldSymb *)sf)->nextField()){
  for(sf=FirstTypeField(s->type());sf;sf=((SgFieldSymb *)sf)->nextField()){

    //sf = sc->field(i); 
   if(IS_POINTER_F90(sf))
   { size = size + DVMTypeLength();
     continue;
   }    
   if(isSgArrayType(sf->type())) { 
     //le= ArrayLength(sf,cur_st,1);
     //if (le->isInteger())
     //  size = size + le->valueInteger();
     n= NumberOfElements(sf,cur_st,2);//ArrayLength(sf,cur_st,1);
     if (n != 0)
       size = size + n*TypeSize(sf->type()->baseType());
     else 
       Error("Can't calulate structure size: %s", s->identifier(),294,cur_st);    
   }
   else  
     size = size + TypeSize(sf->type()); 
  }

  return(size);
}

SgSymbol *FirstTypeField(SgType *t)
{return(SymbMapping(TYPE_COLL_FIRST_FIELD(t->thetype)));}



int DVMTypeLength()
{return( len_DvmType ? len_DvmType : TypeSize(SgTypeInt()));}


int CharLength(SgType *t)
{
 if(!TYPE_RANGES(t->thetype))
    return(1); // CHARACTER (without len, default len=1)
 
 return(ReplaceParameter( &(t->length()->copy()) )->valueInteger() );
 //return(ReplaceParameter( (new SgExpression(TYPE_RANGES(t->thetype)))->lhs() )->valueInteger() );
}


int TypeIndex(SgType *t) 
{
  if(!t) return -1;
  int Tsize = TypeSize(t); 
  switch(t->variant()) {
     case T_INT:     if(Tsize==4)
                        return (Integer);                      
                     else if (Tsize==1)
                        return (Integer_1);
                     else if (Tsize==2)
                        return (Integer_2);
                     else if (Tsize==8)
                        return (Integer_8);
                     else
                        break;
     case T_FLOAT:   if(Tsize == 4)
                        return (Real); 
                     else if(Tsize == 8)	               
                        return (Double);
                     else
                        break;
     case T_DOUBLE:     return (Double);
     case T_COMPLEX: if(Tsize == 8)
                        return (Complex); 
                     else if(Tsize == 16)		         
                        return (DComplex);
                     else
                        break;
     case T_DCOMPLEX:   return (DComplex);
     case T_BOOL:    if(Tsize==4)
                        return (Logical); 
                     else if(Tsize==1)
                        return (Logical_1);
                     else if (Tsize==2)
                        return (Logical_2);
                     else if (Tsize==8)
                        return (Logical_8);
                     else
                        break;
     case T_STRING:  if(Tsize==1)
                        return (Character); /*13.01.12*/
                     else
                        break;  
     default:           break;
  }
  
  return (-1);
}

int CompareTypes(SgType *t1,SgType *t2)
{
  if(!t1 || !t2) return(1);
  if(TypeIndex(t1) >= 0 )
     if( TypeIndex(t1)==TypeIndex(t2) )
        return(1);
     else
        return(0);
  if(t1->variant() == T_DERIVED_TYPE )
     if(t2->variant() == T_DERIVED_TYPE && !strcmp(t1->symbol()->identifier(), t2->symbol()->identifier()))
        return(1);
     else
        return(0);
  if(TypeIndex(t1)==-1 && TypeIndex(t2)==-1)
     return(1);
  else
     return(0);
  return(0);
}

int BoundSizeArrays (SgSymbol *das)
// returns dvm-index of RightBSizeArray
{
 int iright;
 int i,nw,rank,width;
 SgExpression *wl,*ew, *lbound[MAX_DIMS], *ubound[MAX_DIMS], *she;

 rank = Rank(das);
 if(SHADOW_(das)) { // there is SHADOW directive, i.e. shadow  widths are
                    // specified
   iright = 0;
   she = *SHADOW_(das);
   if(!TestMaxDims(she,das,0)) return(0);
   for(wl = she,i=0; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     if(ew->variant() == DDOT){
       lbound[i] = &(ew->lhs())->copy();//left bound 
       ubound[i] = &(ew->rhs())->copy();//right bound 
     } else {
       lbound[i] = &(ew->copy());//left bound == right bound
       ubound[i] = &(ew->copy());
     }
   }
   nw = i;
 
   if(nw<rank) 
     for(; i<rank; i++) {      
       lbound[i] = new SgValueExp(1); // by default, bound width = 1
       ubound[i] = new SgValueExp(1);
     }
   
  if (nw != rank) // wrong shadow width list
     return(0);
  
 } else {//  shadow  widths are not  specified in program
     if(HPF_program && all_sh_width) // shadow width is specified by option -Hshw 
        width = all_sh_width;        // for all arrays of HPF program
     else
        width = 1; //by default shadow width = 1
     for(i=0; i<rank; i++) {      
        lbound[i] = new SgValueExp(width); 
     }
     iright=ndvm;
 }
 for(i=rank-1;i>=0; i--)
     doAssignStmt(lbound[i]);
 if(!iright) { // shadow widths are specified in program
  iright = ndvm;
  for(i=rank-1;i>=0; i--)
     doAssignStmt(ubound[i]);
 }
  return(iright);
}

void TestWeightArray(SgExpression *efm, SgStatement *st)
{
  SgArrayType *artype;
  if(VarType_RTS(efm->symbol())!=4) //DOUBLE PRECISION
    Error("Illegal type of '%s'",efm->symbol()->identifier(),141,st);

  artype = isSgArrayType(efm->symbol()->type());
  if(! artype || !artype->getDimList())   //isn't array
  {
    Error ("'%s' isn't array", efm->symbol()->identifier(),66,st);
    return;
  }
  
  if(artype->dimension() != 1)
  {
    Error ("Illegal rank of '%s'", efm->symbol()->identifier(),76,st);
    return;
  } 
  SgExpression *arsize = Calculate(artype->sizeInDim(0));
  if(arsize->variant() == INT_VAL)
  {
    SgExpression *nblock = Calculate(efm->lhs());
    if(nblock->variant() == INT_VAL)
    {
      if(((SgValueExp *)arsize)->intValue() < ((SgValueExp *)nblock)->intValue())
      { 
        Error("Illegal array size of '%s'",efm->symbol()->identifier(),340,st);
        return;
      }      
    }
  } 
}

SgExpression *AddElementToList(SgExpression *list, SgExpression *e)
{
  SgExpression *el = new SgExprListExp(*e);
     el->setRhs(list);
     return (el);
}

SgExpression *ListUnion(SgExpression *list1, SgExpression *list2)
{
  SgExpression *el1=list1, *el2=list2,*result=list1;
  for( ; el1 && el2; el1=list1,el2=list2)
  {
     list1=list1->rhs()->rhs();
     list2=list2->rhs()->rhs();     
     el2->rhs()->setRhs(list1);
     el1->rhs()->setRhs(el2);
  }
     return (result);
}

int isInterfaceRTS2(SgStatement *stdis)
{
  SgExpression *e, *efm;
  for(e=stdis->expr(1); e; e = e->rhs()) {
     efm = e->lhs(); //dist_format expression
     
     if(efm->variant() == INDIRECT_OP)
     {
        if(stdis->expr(2))
        {
           err("ONTO/NEW_VALUE clause is not supported",625,stdis);
           return(0);
        }
        if(parloop_by_handler == 2)       
           return(1);
        else
        {
           err("Indirect/Derived distribution, -Opl2 option should be specified",624,stdis);
           return(0);
        } 
     }
  }
  return(parloop_by_handler==2 ? 1 : 0);
}

SgExpression *doDisRules(SgStatement *stdis, int aster, int &idis) {

  SgExpression **dis_rules,*distr_list[1]; // DisRule's list
   
  dis_rules = isInterfaceRTS2(stdis) ? distr_list : NULL;    
  idis = doDisRuleArrays(stdis, aster, dis_rules); 
  return (idis==-1 ? *dis_rules : NULL);
}

int doDisRuleArrays (SgStatement *stdis, int aster, SgExpression **distr_list ) {

  SgExpression *e, *efm, *ed, *nblk[MAX_DIMS], *dist_format, *multiple[MAX_DIMS], *numb[MAX_DIMS];
  SgSymbol *genbl[MAX_DIMS];
  int iaxis, i, axis[MAX_DIMS], param[MAX_DIMS], tp, mps_axis;
  SgValueExp M1(1);
//looking through the dist_format_list and
// creating AxisArray and DistrParamArray
  ndis = 0;
  nblock = 0;
  gen_block = 0;
  mult_block = 0;
  mps_axis = 0;
  iaxis = ndvm;
  if(distr_list)
     *distr_list = NULL;
  dist_format = stdis->expr(1);
  if(!dist_format){ //dist_format list is absent
    all_replicated=0;
    return(distr_list ? -1 : iaxis);
  }
  for(i=0; i<MAX_DIMS; i++) 
     numb[i] = NULL;
  for(e=dist_format; e; e = e->rhs()) {
     efm = e->lhs(); //dist_format expression
     if(ndis==MAX_DIMS)
     {
        err("Too many dimensions",43,stdis); 
        break;
     }
     ndis++;
     if(efm->variant() == BLOCK_OP) {
        nblock++;
        mps_axis++;
        if(!( efm->symbol() ) ) // case: BLOCK or MULT_BLOCK                               
        {
           if( !efm->rhs() ) // case: BLOCK 
           {
              if(distr_list)
                 *distr_list = AddElementToList(*distr_list,DvmhBlock(mps_axis)); 
                              
	      multiple[ndis-1] = &M1;
           }
           else {            // case: MULT_BLOCK (k)
              if(distr_list)
                 *distr_list = AddElementToList(*distr_list,DvmhMultBlock(mps_axis, DVM000(iaxis+ndis-1))); 
              multiple[ndis-1] = numb[ndis-1] = efm->rhs();
              mult_block = 1;              
          }  
          axis[ndis-1]  = ndis;
          param[ndis-1] = 0;
          genbl[ndis-1] = NULL;
        }
        else if (!efm->lhs())   // case: GEN_BLOCK  
	{ if( gen_block == 2 ) // there is WGT_BLOCK in format-list
            err("GEN_BLOCK and WGT_BLOCK in format-list",129,stdis);
          else 
            gen_block = 1;
          if(distr_list)
            *distr_list = AddElementToList(*distr_list,DvmhGenBlock(mps_axis, efm->symbol())); 
          multiple[ndis-1] = &M1;
          axis[ndis-1]  = ndis;
          param[ndis-1] = 0;
          genbl[ndis-1] = efm->symbol();
          tp = VarType_RTS(efm->symbol());
          if((bind == 0 && tp != 2 && tp != 1) || (bind == 1 && tp != 1)) //INTEGER
            Error("Illegal type of '%s'",efm->symbol()->identifier(),141,stdis);
          SgArrayType *artype=isSgArrayType(efm->symbol()->type()); 
          if( !artype || !artype->getDimList() )
            Error("'%s' isn't array",efm->symbol()->identifier(),66,stdis);
        } 
        else                    // case: WGT_BLOCK 
        { if( gen_block == 1 ) // there is GEN_BLOCK in format-list
            err("GEN_BLOCK and WGT_BLOCK in format-list",129,stdis);
          else 
            gen_block = 2;
          if(distr_list)
            *distr_list = AddElementToList(*distr_list,DvmhWgtBlock(mps_axis, efm->symbol(),DVM000(iaxis+ndis-1))); 
          multiple[ndis-1] = &M1;
          axis[ndis-1]  = ndis;
          param[ndis-1] = 0;
          genbl[ndis-1] = efm->symbol();
          nblk[ndis-1]  = numb[ndis-1] = efm->lhs();

          TestWeightArray(efm,stdis);
        }               
       /* else if ((efm->lhs())->variant() == SPEC_PAIR)
        *                       //there is one operand (variant==SPEC_PAIR)
        *                       // case: BLOCK(SHADOW=...)
        *{
        *  efm = (efm->lhs())->rhs();
        *
        *} else          //there is one operand (variant==CONS)
        *                 // case: BLOCK(LOW_SHADOW=...,HIGH_SHADOW=...)
        *     {  }
        */
     } else if(efm->variant() == INDIRECT_OP)
     {
        mps_axis++;
        if(distr_list)
        {
           if(efm->symbol())  // case INDIRECT(map)
              *distr_list = AddElementToList(*distr_list,DvmhIndirect(mps_axis, efm->symbol()));
           else               // case  DERIVED(...)
           {
              SgExpression *eFunc[2];
              SgExpression *edrv = efm->lhs(); // efm->lhs()->variant()  == DERIVED_OP
              DerivedSpecification(edrv, stdis, eFunc);
              *distr_list = AddElementToList(*distr_list,DvmhDerived(mps_axis, DvmhDerivedRhs(edrv->rhs()),eFunc[0],eFunc[1]));
           }
        }
     } else        // variant ==KEYWORD_VAL  ("*")
       {  axis[ndis-1] = 0;  
          multiple[ndis-1] = &M1; 
          if(distr_list)
            *distr_list = AddElementToList(*distr_list,DvmhReplicated()); 
       }
  }

  if( gen_block == 1 && mult_block) // there are GEN_BLOCK and MULT_BLOCK in format-list
    err("GEN_BLOCK and MULT_BLOCK in format-list",129,stdis);

  if(!nblock_all && dist_format)
    nblock_all = nblock;

  if(nblock)
    all_replicated=0;

  if(aster)  // dummy arguments  inherit distribution
     return(distr_list ? -1 : iaxis);

  if(distr_list)
  { 
    for(i=0; i<ndis; i++) {
     if(numb[i]) 
        doAssignTo(DVM000(iaxis+i),numb[i]);
    }
    return(-1);
  } 
  
  for(i=0; i<ndis; i++) {
     if(axis[i]) // axis[i] != 0
        doAssignStmt(new SgValueExp(ndis - axis[i] + 1));
  }
  for(i=0; i<ndis; i++) {
     if(axis[i]) // axis[i] != 0
        doAssignStmt(new SgValueExp(param[i]));
  }
  if(gen_block == 1 || gen_block == 2)
    for(i=0; i<ndis; i++) {
      if(axis[i])  // axis[i] != 0
        doAssignStmt(genbl[i] ? GetAddresMem(new SgArrayRefExp(*genbl[i], *Exprn(LowerBound(genbl[i],0)))) : ConstRef(0));      
    }
  if(gen_block == 2)
    for(i=0; i<ndis; i++) {
      if(axis[i])  // axis[i] != 0
        doAssignStmt(genbl[i] ? nblk[i] : ConstRef(0));      
    }
  if(mult_block)
  { mult_block = ndvm;
    for(i=ndis-1; i>=0; i--) 
        doAssignStmt(&(multiple[i]->copy()));   
  }

  if(!nblock) //replication ("*") in all dimensions
        doAssignStmt(new SgValueExp(0));

  return (iaxis);
}

void doAlignRule_1 (int rank)
// (SgExpression **p_axis,
//                    SgExpression **p_coeff, SgExpression **p_const) 
{ int i;
  SgValueExp *num;
  SgValueExp c1(1),c0(0);
 // creating axis_array 
//  axis_array = dvm_array_ref();  // dvm000(ndvm)
  for(i=1; i<=rank; i++) {
     num = new SgValueExp (i);
     doAssignStmt(num);            // AxisArray(i)=i 
  }
 // creating coeff_array 
 // coeff_array =  dvm_array_ref();  // dvm000(ndvm)      
  for(i=1; i<=rank; i++) 
     doAssignStmt(&c1.copy());     // CoeffArray(i)=1 
  // creating const_array 
  //const_array =  dvm_array_ref();  // dvm000(ndvm)      
  for(i=1; i<=rank; i++) 
     doAssignStmt(&c0.copy());     // ConstArray(i)=0 
}  

int doAlignRule (SgSymbol *alignee, SgStatement *algn_st, int iaxis)
// creating axis_array, coeff_array and  const_array 
// returns length of align_source_list (dimension_identifier_list)
// (SgExpression **p_axis,
//                    SgExpression **p_coeff, SgExpression **p_const) 
{ int i,j,rank,ni,nt,ia,num, use[MAX_DIMS];
    //algn_attr *attr;
    //SgStatement *algn_st;
  SgExpression * el,*e,*ei,*elbi,*elbb;
  SgSymbol *dim_ident[MAX_DIMS],*align_base;
  SgExpression *axis[MAX_DIMS], *coef[MAX_DIMS], *cons[MAX_DIMS], *et;
  SgValueExp c1(1),c0(0),cM1(-1);
  int num_dim[MAX_DIMS], ncolon, ntriplet;
  for(i=0;i<MAX_DIMS;i++)
     num_dim[i]=0;

  rank = Rank(alignee);    // rank of aligned array
     //algn_st = node->align_stmt; // align statement
 
  if(iaxis == -2) return(rank);//for ALLOCATABLE array in specification part 
                               //can't generate align rules because there is not declared array shape

  ni = 0; //counter of elements in align_source_list(dimension_identifier_list)
  ncolon = 0; //counter of elements ':'in align_source_list
  if(!algn_st->expr(1))  //align_source_list is absent
    for(;ni<rank;ni++,ncolon++) {
       num_dim[ncolon] = ni;
       dim_ident[ni] = NULL;   
       use[ni] = 0;  
    }
  //looking through the align_source_list (dimension_identifier_list)
  for(el=algn_st->expr(1); el; el=el->rhs())   {
     if(ni==MAX_DIMS) {
        err("Illegal align-source-list",633,algn_st); 
        break;
     }
     if(isSgVarRefExp(el->lhs())) {  // dimension identifier
       if(el->lhs()->symbol()->attributes() & PARAMETER_BIT)
         Error("The align-dummy %s isn't a scalar integer variable",el->lhs()->symbol()->identifier(), 62,algn_st);
       dim_ident[ni] = (el->lhs())->symbol();
     } 
     else if (el->lhs()->variant() == DDOT) {   // ':'
             num_dim[ncolon++] = ni;
             dim_ident[ni] = NULL;
     }
     else                           // "*"
             dim_ident[ni] = NULL;         
     use[ni] = 0;

     ni++;
  }
  if(rank && rank != ni)
    Error ("Rank of aligned array %s isn't equal to the length of align-source-list", alignee->identifier(),128,algn_st);

  ia = alignee->attributes();
  if(ia & DISTRIBUTE_BIT) 
    Error ("An alignee may not have the DISTRIBUTE attribute: %s", alignee->identifier(),57,algn_st);

  et =(algn_st->expr(2)->variant()==ARRAY_OP) ? (algn_st->expr(2))->rhs() : algn_st->expr(2);
  align_base = et->symbol();

  nt = 0;//counter of elements in align_subscript_list
  ntriplet = 0; //counter of triplets in align_subscript_list
  if(! et->lhs())  //align_subscript_list is absent
    for( ; nt<Rank(align_base); nt++,ntriplet++) {
       axis[nt] = new SgValueExp(ni-num_dim[ntriplet]);
       coef[nt] = new SgValueExp(1);
       cons[nt] =  &(*Exprn(LowerBound(align_base,nt)) -
                    (*Exprn( LowerBound(alignee,num_dim[ntriplet])))); 
  } 
 //looking through the align_subscript_list 
  for(el=et->lhs(); el; el=el->rhs())   {
     if(nt==MAX_DIMS) {
        err("Illegal align-subscript-list",634,algn_st); 
        break;
     }
     e = el->lhs();  //subscript expression
     if(e->variant()==KEYWORD_VAL) {  // "*"
       axis[nt] = & cM1.copy();
       coef[nt] = & c0.copy();
       cons[nt] = & c0.copy();   
     }
     else if (e->variant()==DDOT) { // triplet
       axis[nt] = new SgValueExp(ni-num_dim[ntriplet]);
       coef[nt] = (e->lhs() && e->lhs()->variant()==DDOT) ? & e->rhs()->copy() :
                                                            new SgValueExp(1);
       //elbi = Exprn( LowerBound(alignee,num_dim[ntriplet]));
       //if (e->lhs() && e->lhs()->variant()==DDOT)
       //  elbi = &(coef[nt]->copy()* (*elbi));
       //else
       //   elbi = NULL;
       elbb = Exprn(LowerBound(align_base,nt)); 
       if  (e->lhs()) 
          if(e->lhs()->variant()!=DDOT)
             cons[nt] = &(e->lhs()->copy() - (*elbb));
          else if (e->lhs()->lhs())
             cons[nt] = &(e->lhs()->lhs()->copy() - (*elbb));
          else
             cons[nt] = & c0.copy();
       else
             cons[nt] = & c0.copy();
       //cons[nt] = &(*elbb - *elbi);  
     
       ntriplet++;  
     }
     else  {  // expression
       num = AxisNumOfDummyInExpr(e, dim_ident, ni, &ei, use, algn_st);
       //ei->unparsestdout();
       //printf("\nnum = %d\n", num);
       if (num<=0)   {
       axis[nt]  = & c0.copy();
       coef[nt] = & c0.copy();
       elbb = LowerBound(align_base,nt);
       if(elbb)
         cons[nt] = & (e->copy() - (elbb->copy()));
                  // correcting const with lower bound of align-base array 
       else // error situation : rank of align-base less than list length
         cons[nt] = & (e->copy()); 
       }
       else {
       axis[nt] = new SgValueExp(ni-num+1); // reversing numbering
       CoeffConst(e, ei,&coef[nt], &cons[nt]);  
       if(!iaxis) TestReverse(coef[nt],algn_st); 
       if(!coef[nt]) {
         if(!iaxis) err("Wrong align-subscript expression", 130,algn_st);
         coef[nt] = & c0.copy();
         cons[nt] = & c0.copy();   
       }
       else {
       // correcting const with lower bound of alignee and align-base arrays 
        elbb = LowerBound(align_base,nt);
        elbi = LowerBound(alignee,num-1);
        if(elbb  && elbi)   
          cons[nt] = &(*cons[nt] + (*coef[nt] * (elbi->copy())) - (elbb->copy()));  
        }        
       }
     }
    
     nt++;
  }
  ia = align_base->attributes();
  if(!iaxis) {
    if(!(ia & DIMENSION_BIT) && !IS_POINTER(align_base)) 
      Error ("Align-target %s isn't declared as array",align_base->identifier(),61,algn_st);
    else
    if(Rank(align_base) != nt) 
      Error ("Rank of align-target %s isn't equal to the length of align_subscript-list", align_base->identifier(),132,algn_st);
    if(ntriplet != ncolon)
       err ("The number of colons in align-source-list isn't equal to the number of subscript-triplets",131,algn_st);
    // setting on arrays with reversing
    for(i=nt-1; i>=0; i--)
      doAssignStmt(axis[i]);
    for(i=nt-1; i>=0; i--)
      doAssignStmt(ReplaceFuncCall(coef[i]));
    for(i=nt-1; i>=0; i--)
      doAssignStmt(Calculate(cons[i]));
  }
  else  if(iaxis == -1)
    return(nt);
  else { 
    j = iaxis + 2*nt;
    for(i=nt-1; i>=0; i--)
      doAssignTo(DVM000(j++),Calculate(cons[i]));
  } 
  
  return(nt);  
}


int doAlignRuleArrays (SgSymbol *alignee, SgStatement *algn_st, int iaxis, SgExpression *axis[], SgExpression *coef[],SgExpression *cons[], int interface )
// creating axis_array, coeff_array and  const_array 
// returns length of align_source_list (dimension_identifier_list)
// (SgExpression **p_axis,
//                    SgExpression **p_coeff, SgExpression **p_const) 
{ int i,j,rank,ni,nt,ia,num, use[MAX_DIMS];
    //algn_attr *attr;
    //SgStatement *algn_st;
  SgExpression * el,*e,*ei,*elbi,*elbb;
  SgSymbol *dim_ident[MAX_DIMS],*align_base;
  SgExpression *et;
  SgValueExp c1(1),c0(0),cM1(-1);
  int num_dim[MAX_DIMS], ncolon, ntriplet;
  for(i=0;i<MAX_DIMS;i++)
     num_dim[i]=0;

  rank = Rank(alignee);    // rank of aligned array
 
  if(iaxis == -2) return(rank);//for ALLOCATABLE array in specification part 
                               //can't generate align rules because there is not declared array shape

  ni = 0; //counter of elements in align_source_list(dimension_identifier_list)
  ncolon = 0; //counter of elements ':'in align_source_list
  if(!algn_st->expr(1))  //align_source_list is absent
    for(;ni<rank;ni++,ncolon++) {
       num_dim[ncolon] = ni;
       dim_ident[ni] = NULL;   
       use[ni] = 0;  
    }
  //looking through the align_source_list (dimension_identifier_list)
  for(el=algn_st->expr(1); el; el=el->rhs())   {
     if(ni==MAX_DIMS) {
        err("Illegal align-source-list",633,algn_st); 
        break;
     }
     if(isSgVarRefExp(el->lhs())) {  // dimension identifier
       if(el->lhs()->symbol()->attributes() & PARAMETER_BIT)
         Error("The align-dummy %s isn't a scalar integer variable",el->lhs()->symbol()->identifier(), 62,algn_st);
       dim_ident[ni] = (el->lhs())->symbol();
     } 
     else if (el->lhs()->variant() == DDOT) {   // ':'
             num_dim[ncolon++] = ni;
             dim_ident[ni] = NULL;
     }
     else                           // "*"
             dim_ident[ni] = NULL;         
     use[ni] = 0;

     ni++;
  }
  if(rank && rank != ni)
    Error ("Rank of aligned array %s isn't equal to the length of align-source-list", alignee->identifier(),128,algn_st);

  ia = alignee->attributes();
  if(ia & DISTRIBUTE_BIT) 
    Error ("An alignee may not have the DISTRIBUTE attribute: %s", alignee->identifier(),57,algn_st);

  et =(algn_st->expr(2)->variant()==ARRAY_OP) ? (algn_st->expr(2))->rhs() : algn_st->expr(2);
  align_base = et->symbol();

  nt = 0;//counter of elements in align_subscript_list
  ntriplet = 0; //counter of triplets in align_subscript_list
  if(! et->lhs())  //align_source_list is absent
    for( ; nt<Rank(align_base); nt++,ntriplet++) {
       axis[nt] = new SgValueExp(ni-num_dim[ntriplet]);
       coef[nt] = new SgValueExp(1);
       cons[nt] = interface == RTS2 ? new SgValueExp(0) : &(*Exprn(LowerBound(align_base,nt)) -
                    (*Exprn( LowerBound(alignee,num_dim[ntriplet])))); 
  } 
 //looking through the align_subscript_list 
  for(el=et->lhs(); el; el=el->rhs())   {
     if(nt==MAX_DIMS) {
        err("Illegal align-subscript-list",634,algn_st); 
        break;
     }
     e = el->lhs();  //subscript expression
     if(e->variant()==KEYWORD_VAL) {  // "*"
       axis[nt] = & cM1.copy();
       coef[nt] = & c0.copy();
       cons[nt] = & c0.copy();   
     }
     else if (e->variant()==DDOT) { // triplet
       axis[nt] = new SgValueExp(ni-num_dim[ntriplet]);
       coef[nt] = (e->lhs() && e->lhs()->variant()==DDOT) ? & e->rhs()->copy() :
                                                            new SgValueExp(1);
       elbb = Exprn(LowerBound(align_base,nt)); 
       if  (e->lhs()) 
          if(e->lhs()->variant()!=DDOT)
             cons[nt] = interface == RTS2 ? &(e->lhs()->copy()) : &(e->lhs()->copy() - (*elbb));
          else if (e->lhs()->lhs())
             cons[nt] = interface == RTS2 ? &(e->lhs()->lhs()->copy()) : &(e->lhs()->lhs()->copy() - (*elbb));
          else
             cons[nt] = & c0.copy();
       else
             cons[nt] = & c0.copy();  
     
       ntriplet++;  
     }
     else  {  // expression
       num = AxisNumOfDummyInExpr(e, dim_ident, ni, &ei, use, algn_st);
            //ei->unparsestdout();
            //printf("\nnum = %d\n", num);
       if (num<=0)   {
          axis[nt] = & c0.copy();
          coef[nt] = & c0.copy();
          cons[nt] = & (e->copy()); 
          if(interface != RTS2 && (elbb = LowerBound(align_base,nt)) )
             cons[nt] = & (*cons[nt] - (elbb->copy()));
                  // correcting const with lower bound of align-base array 
                  // elbb==NULL is error situation : rank of align-base less than list length
            
       }
       else {
          axis[nt] = new SgValueExp(ni-num+1); // reversing numbering
          CoeffConst(e, ei,&coef[nt], &cons[nt]);  
          if(!iaxis) TestReverse(coef[nt],algn_st); 
          if(!coef[nt]) {
             if(!iaxis) err("Wrong align-subscript expression", 130,algn_st);
             coef[nt] = & c0.copy();
             cons[nt] = & c0.copy();   
          }
          else {
          // correcting const with lower bound of alignee and align-base arrays 
           elbb = LowerBound(align_base,nt);
           elbi = LowerBound(alignee,num-1);
           if(interface != RTS2 && elbb  && elbi)   
              cons[nt] = &(*cons[nt] + (*coef[nt] * (elbi->copy())) - (elbb->copy()));  
          }        
       }
     }
    
     nt++;
  }
  ia = align_base->attributes();
  if(!iaxis) {
    if(!(ia & DIMENSION_BIT) && !IS_POINTER(align_base)) 
      Error ("Align-target %s isn't declared as array",align_base->identifier(),61,algn_st);
    else
    if(Rank(align_base) != nt) 
      Error ("Rank of align-target %s isn't equal to the length of align_subscript-list", align_base->identifier(),132,algn_st);
    if(ntriplet != ncolon)
       err ("The number of colons in align-source-list isn't equal to the number of subscript-triplets",131,algn_st);
  }
    return (nt);
}

int TestExprArray(SgExpression *e[], int n)
{
  int i;
  for(i=0; i<n; i++)
     if(isSgValueExp(e[i]) || isSgVarRefExp(e[i]) || e[i]->variant()==CONST_REF)
        continue;
     else
        return (0);
  return (1);
}

SgExpression *doAlignRules (SgSymbol *alignee, SgStatement *algn_st, int iaxis, int &nt)
{
  SgExpression *axis[MAX_DIMS],
               *coef[MAX_DIMS],
               *cons[MAX_DIMS];
  SgExpression *el, *e, *alignment_list = NULL;
  int i,j; 
  nt = doAlignRuleArrays (alignee, algn_st, iaxis, axis, coef, cons, INTERFACE_RTS2 ? RTS2 : RTS1);
  if(iaxis == -1 || iaxis == -2)
     return(NULL);
  if(INTERFACE_RTS2) {
     int flag_coef = TestExprArray(coef,nt);
     int flag_cons = TestExprArray(cons,nt);
     int j1 = ndvm, j2;
     if(!iaxis) {     
        if(!flag_coef)
           for(i=nt-1; i>=0; i--)
              doAssignStmt(ReplaceFuncCall(coef[i]));
        j2 = ndvm;
        if(!flag_cons)
           for(i=nt-1; i>=0; i--)
              doAssignStmt(Calculate(cons[i]));
     } else { 
        j1=iaxis; 
        j2=flag_coef ? iaxis : iaxis+nt;
     }
     for(int i=0; i<nt; i++)
     {          
        e = AlignmentLinear(axis[i],flag_coef ? coef[i] : DVM000(j1++),flag_cons ? cons[i] : DVM000(j2++));        
                  //e = AlignmentLinear(axis[i],ReplaceFuncCall(coef[i]),cons[i]);   //Calculate(cons[i])    
        (el = new SgExprListExp(*e))->setRhs(alignment_list);
        alignment_list = el;        
     }
     return (alignment_list);
  }
  if(!iaxis) {
    // setting on arrays with reversing
    for(i=nt-1; i>=0; i--)
      doAssignStmt(axis[i]);
    for(i=nt-1; i>=0; i--)
      doAssignStmt(ReplaceFuncCall(coef[i]));
    for(i=nt-1; i>=0; i--)
      doAssignStmt(Calculate(cons[i]));
  }
  else {
    j = iaxis + 2*nt;
    for(i=nt-1; i>=0; i--)
      doAssignTo(DVM000(j++),Calculate(cons[i]));
  } 
  
  return(NULL);  

}

SgExpression * Exprn(SgExpression *e)
{return((!e) ? new SgValueExp(0) : & e->copy());}

int AxisNumOfDummyInExpr (SgExpression *e, SgSymbol *dim_ident[], int ni,                             SgExpression **eref, int use[], SgStatement *st)
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
       if(dim_ident[i]==NULL)
         continue;
       if(dim_ident[i]==symb)  {
         *eref = e;
         if (use[i] == 1)
           if(st && st->variant() == DVM_PARALLEL_ON_DIR)
             Error("More one occurance of do-variable '%s'  in iteration-align-subscript-list", symb->identifier(),133, st);
           else if(st)
             Error("More one occurance of align_dummy '%s' in align-subscript-list", symb->identifier(), 134,st);
           use[i]++;
         return(i+1);
       }
    }
    return (0);
  }
  i1 = AxisNumOfDummyInExpr(e->lhs(), dim_ident, ni, eref, use, st);
  e1 = *eref;
  i2 = AxisNumOfDummyInExpr(e->rhs(), dim_ident, ni, eref, use, st);
  if((i1==-1)||(i2==-1))  return(-1);
  if(i1 && i2)  {
    if(st && st->variant() == DVM_PARALLEL_ON_DIR)
      err("More one occurance of a do-variable in do-variable-use expression", 135,st);
    else if (st)  
      err("More one occurance of an align_dummy in align-subscript expression", 136,st);
    return(-1);
  }
  if(i1) *eref = e1;
  return(i1 ? i1 : i2);
}

void CoeffConst(SgExpression *e, SgExpression *ei,                                          SgExpression **pcoef, SgExpression **pcons)
//  ei == I;  e == a * I + b 
// result: *pcoef = a, *pcons = b
{
  SgValueExp c1(1), c0(0), cM1(-1);
  switch(e->variant()) {
       case VAR_REF:                // I                            
                       *pcoef = & c1.copy();
                       *pcons = & c0.copy();
                       break;
       case UNARY_ADD_OP:                // +I  
                       if(e->lhs()==ei) {                           
                       *pcoef = & c1.copy();
                       *pcons = & c0.copy();
                       }
                       else
                       *pcoef = NULL;
                       break;
       case MINUS_OP:                // -I  
                       if(e->lhs()==ei) {                            
                       *pcoef = & cM1.copy();
                       *pcons = & c0.copy();
		       }
                       else
                       *pcoef = NULL;
                       break;
                       
       case MULT_OP:                // a * I
                       if (e->lhs()==ei) 
                       *pcoef = &(e->rhs())->copy(); 
                       else if (e->rhs()==ei)
                       *pcoef = &(e->lhs())->copy() ;
                       else 
                       *pcoef = NULL;
                       *pcons = & c0.copy();
                       break;
       case DIV_OP :               // I / a
                       if(e->rhs()==ei) 
                         *pcoef = NULL;  // Error
                       else {
                         *pcoef = & (c1.copy() / (e->rhs())->copy());
                         *pcons = & c0.copy();
                       }
                       break;
       case ADD_OP :               
                       if(e->lhs()==ei) {                  // I + b
                         *pcoef = & c1.copy();
                         *pcons = & (e->rhs())->copy();     

                       } else  if(e->rhs()==ei) {          // b + I
                         *pcoef = & c1.copy();
                         *pcons = & (e->lhs())->copy();                                            
                       } else if (((e->lhs())->lhs()==ei)){ // I * a + b
                         if(e->lhs()->variant() == MULT_OP){
                         *pcons = & (e->rhs())->copy();
                         *pcoef = & ((e->lhs())->rhs())->copy();
                         }
                         else if(e->lhs()->variant() == MINUS_OP){
                         *pcons = & (e->rhs())->copy();
                         *pcoef = & cM1.copy();
                         }                        
                         else
                         *pcoef = NULL;
                       
                       } else if (((e->lhs())->rhs()==ei)){ // a * I + b
                         if(e->lhs()->variant() == MULT_OP){
                         *pcons = & (e->rhs())->copy();
                         *pcoef = & ((e->lhs())->lhs())->copy();
                         }
                         else
                         *pcoef = NULL;
                       
                       } else if (((e->rhs())->lhs()==ei)){ // b + I * a
                         if(e->rhs()->variant() == MULT_OP){ 
                         *pcons = & (e->lhs())->copy();
                         *pcoef = & ((e->rhs())->rhs())->copy();
                         } 
                         else if(e->rhs()->variant() == MINUS_OP){
                         *pcons = & (e->lhs())->copy();
                         *pcoef = & cM1.copy();
                         }                        
                         else
                         *pcoef = NULL;                    

                       } else if (((e->rhs())->rhs()==ei)){ // b + a * I
                         if(e->rhs()->variant() == MULT_OP){  
                         *pcons = & (e->lhs())->copy();
                         *pcoef = & ((e->rhs())->lhs())->copy();
                         } 
                       }
                         else
                         *pcoef = NULL;
                       break;
      case SUBT_OP :               
                       if(e->lhs()==ei) {                  // I - b
                         *pcoef = & c1.copy();
                         *pcons = & SgUMinusOp((e->rhs())->copy());     

                       } else  if(e->rhs()==ei) {          // b - I
                         *pcoef = & cM1.copy();
                         *pcons = & (e->lhs())->copy();                                            
                       } else if (((e->lhs())->lhs()==ei)){ // I * a - b
                         if(e->lhs()->variant() == MULT_OP){
                         *pcons = & SgUMinusOp((e->rhs())->copy());
                         *pcoef = & ((e->lhs())->rhs())->copy();
                         }
                         else if(e->lhs()->variant() == MINUS_OP){
                         *pcons = & SgUMinusOp((e->rhs())->copy());
                         *pcoef = & cM1.copy();
                         }                       
                         else
                         *pcoef = NULL;

                       } else if (((e->lhs())->rhs()==ei)){ // a * I - b
                         if(e->lhs()->variant() == MULT_OP){
                         *pcons = & SgUMinusOp((e->rhs())->copy());
                         *pcoef = & ((e->lhs())->lhs())->copy();
                         }
                         else
                         *pcoef = NULL;

                       } else if (((e->rhs())->lhs()==ei)){ // b - I * a
                         if(e->rhs()->variant() == MULT_OP){  
                         *pcons = & (e->lhs())->copy();
                         *pcoef = & SgUMinusOp(((e->rhs())->rhs())->copy());
                         }
                         else
                         *pcoef = NULL; 

                       } else if (((e->rhs())->rhs()==ei)){ // b - a * I 
                         if(e->rhs()->variant() == MULT_OP){  
                         *pcons = & (e->lhs())->copy();
                         *pcoef = & SgUMinusOp(((e->rhs())->lhs())->copy());
                         }
                       } 
                         else
                         *pcoef = NULL;
                       break;
       default:
                       *pcoef = NULL;
                       break; 
                          
  }
}
//-----------------------------------------------------------------------
void ChangeDistArrayRef(SgExpression *e)
{ 
  SgExpression *el;
 
  if(!e)
    return;
  if( e->variant() != BOOL_VAL && e->variant() != INT_VAL && e->symbol() &&  IS_GROUP_NAME(e->symbol()))
      Error("Illegal group name use: '%s'",e->symbol()->identifier(),137,cur_st);
  
  if(opt_loop_range && inparloop && isSgVarRefExp(e) && INDEX_SYMBOL(e->symbol())){
    ChangeIndexRefBySum(e);  
    return;
  }
  if(isSgArrayRefExp(e)) {
    if(opt_loop_range && inparloop && (sum_dvm=TestDVMArrayRef(e)))
      ;
    else
    for(el=e->lhs(); el; el=el->rhs())
       ChangeDistArrayRef(el->lhs());
 /*   
    if(HEADER( e -> symbol()) && !isPrivateInRegion(e -> symbol())   //is distributed array reference not private in loop of region
       || IN_COMPUTE_REGION && HEADER_OF_REPLICATED(e -> symbol()) )   //or is array reference in compute region                                               
       DistArrayRef(e,0,cur_st); //replace distributed array reference      
 */
 /*  
    if (   IN_COMPUTE_REGION && is_acc_array(e->symbol())  
       || !IN_COMPUTE_REGION && HEADER(e->symbol()) )
       DistArrayRef(e,0,cur_st); //replace dvm-array reference 
 */
    if  ( HEADER( e -> symbol())  
       || (IN_COMPUTE_REGION || inparloop && parloop_by_handler) && DUMMY_FOR_ARRAY(e -> symbol()) && isIn_acc_array_list(*DUMMY_FOR_ARRAY(e -> symbol()))  )
       DistArrayRef(e,0,cur_st); //replace dvm-array reference if required  
    return;
  }
  if(isSgFunctionCallExp(e)) {
                   //if(!e->lhs()) 
                   //argument list is absent
    ReplaceFuncCall(e);
    for(el=e->lhs(); el; el=el->rhs())
       ChangeArg_DistArrayRef(el);
    return;
  }    
  ChangeDistArrayRef(e->lhs());
  ChangeDistArrayRef(e->rhs());
  return;
}

void ChangeDistArrayRef_Left(SgExpression *e)
{ 
  SgExpression *el;
 
  if(!e)
    return;
  
  if( e->symbol() &&  IS_GROUP_NAME(e->symbol()))
      Error("Illegal group name use: '%s'",e->symbol()->identifier(),137,cur_st);
   
  if(isSgArrayRefExp(e)) {
    if(opt_loop_range && inparloop && (sum_dvm=TestDVMArrayRef(e)))
      ;
    else
      for(el=e->lhs(); el; el=el->rhs())
        ChangeDistArrayRef(el->lhs());
/*
    if(HEADER( e -> symbol()) && !isPrivateInRegion(e -> symbol()) //is distributed array reference not private in loop of region
      || IN_COMPUTE_REGION && HEADER_OF_REPLICATED(e -> symbol()))  //or is array reference in compute region          
 
      DistArrayRef(e,1,cur_st);//replace distributed array reference (1 -modified variable) 
*/
/*    
    if (   IN_COMPUTE_REGION && is_acc_array(e->symbol())  
       || !IN_COMPUTE_REGION && HEADER(e->symbol()) )
       DistArrayRef(e,0,cur_st); //replace dvm-array reference 
*/
    if  ( HEADER( e -> symbol())  
       || (IN_COMPUTE_REGION || inparloop && parloop_by_handler) && DUMMY_FOR_ARRAY(e -> symbol()) && isIn_acc_array_list(*DUMMY_FOR_ARRAY(e -> symbol()))  )
       DistArrayRef(e,1,cur_st); //replace dvm-array reference if required  
           
    return;
  }

 //if(e->variant()==ARRAY_OP){ //substring
      ChangeDistArrayRef_Left(e->lhs());
      ChangeDistArrayRef(e->rhs());
 // }            

  return;
}

void ChangeArg_DistArrayRef(SgExpression *ele)
{//ele is SgExprListExp
  SgExpression  *el, *e;
  e = ele->lhs();
  if(!e)
    return;
 if(isSgKeywordArgExp(e))
   e = e->rhs();

 if(isSgArrayRefExp(e)) {

    if(!e->lhs()){ //argument is  whole array (array name)
                 // no changes are required  because  array header name is       
                 // the same as array name  
      if(IS_POINTER(e->symbol()))
        Error("Illegal POINTER reference: '%s'",e->symbol()->identifier(),138,cur_st);
      if((inparloop && parloop_by_handler || IN_COMPUTE_REGION) ) 
        if(DUMMY_FOR_ARRAY(e->symbol()) && isIn_acc_array_list(*DUMMY_FOR_ARRAY(e ->symbol())) )   
        {  e->setLhs(FirstArrayElementSubscriptsForHandler(e->symbol()));
                                        //changed by first array element reference
           if(!for_host)
             DistArrayRef(e,0,cur_st); 
        }
        if(HEADER(e->symbol()) && for_host)
           e->setSymbol(*HeaderSymbolForHandler(e->symbol())); 
      return;
    }
      el=e->lhs()->lhs();  //first subscript of argument
      //testing: is first subscript of ArrayRef a POINTER 
      if((isSgVarRefExp(el) || isSgArrayRefExp(el)) && IS_POINTER(el->symbol())){
        ChangeDistArrayRef(el->lhs()); 
	                 // ele->setLhs(PointerHeaderRef(el,1));
                        //replace  ArrayRef by PointerRef: A(P)=>P(1) or A(P(I)) => P(1,I) 
        if(!strcmp(e->symbol()->identifier(),"heap") || (e->symbol()->attributes() & HEAP_BIT))
          is_heap_ref = 1;
        else
          Error("Illegal POINTER reference: '%s'", el->symbol()->identifier(),138,cur_st);
        if(e->lhs()->rhs())  //there are other subscripts
          Error("Illegal POINTER reference: '%s'", el->symbol()->identifier(),138,cur_st);
        if(HEADER(e->symbol()))
          Error("Illegal POINTER reference: '%s'", el->symbol()->identifier(),138,cur_st);

        e->setSymbol(*heapdvm); //replace ArrayRef: A(P)=>HEAP00(P) or A(P(I))=>HEAP00(P(I)) 
        return;
      } 
  }

  ChangeDistArrayRef(e); 
   
  return;
  }

SgExpression *ToInt(SgExpression *e)
{ if(!e) return(e);
  return( e->type() && e->type()->variant()==T_INT) ? e : TypeFunction(SgTypeInt(),e,NULL);
}

SgExpression *LinearForm (SgSymbol *ar,  SgExpression *el)
{
  int j,n;
  SgExpression *elin,*e;
// el - subscript list (I1,I2,...In), n - rank of array (ar)
// ind - index of array header in dvm000
// generating                         
// [Header(n) +]
//                               n   
//         Header(n+1) +  I1 + SUMMA(Header(n-k+1) * Ik)
//                              k=2
//or for Cuda kernel
//           n   
//         SUMMA(Header(n-k+1) * Ik)
//          k=1

// Header(0:n+1) - distributed array descriptor

    n = Rank(ar);
    if(!el)     // there aren't any subscripts
      return( coef_ref(ar,n+1) ); //Header(n) 
                                               
    if(for_kernel)           /*ACC*/           
      elin = NULL;                             
    else if(opt_loop_range && inparloop && sum_dvm)
              //   elin = sum_dvm;
      elin = coef_ref(ar,0);
    else       
      elin = coef_ref(ar,n+2);                                    //   Header(n+1)   
    e = ToInt(el->lhs());
    if (for_kernel && options.isOn(AUTO_TFM))   /*ACC*/
      e = &(*coef_ref(ar,n+1) * (*e));                            //  + Header(n)*I1  for loop Cuda-kernel
                                                                  // or
    elin = elin ? &(*elin + *e) : e;                              //  + I1  
    j =  n ;
    for(e=el->rhs(); e && j; e=e->rhs(),j--) {
      if(j>=2) //there is coef_ref(ar,j)
        elin = &(*elin + (*coef_ref(ar,j) * (*ToInt(e->lhs())))); //  + Header(n-k+1)*Ik
    }
     
    if(ACROSS_MOD_IN_KERNEL && (e=analyzeArrayIndxs(ar,el)))   /*ACC*/
      elin = &(*elin + *e); 
 
    if(n && j != 1)
        Error("Wrong number of subscripts specified for '%s'", ar->identifier(),175,cur_st);
    return(elin);
}

SgExpression *LinearFormB (SgSymbol *ar, int ihead, int n,  SgExpression *el)
{
  int j;
  SgExpression *elin,*e;
// el - subscript list (I1,I2,...In), n - rank of array (ar)
// generating                         
// [Header(n) +]
//                               n   
//         Header(n+1) +  I1 + SUMMA(Header(n-k+1) * Ik)
//                              k=2
// Header(0:n+1) - distributed array descriptor
    if(n == 0)
      return( header_rf(ar,ihead,2) ); //Header(1) 
    if(!el)      // there aren't any subscripts     
      return( header_rf(ar,ihead,n+1) ); //Header(n)
 
    elin = header_rf(ar,ihead,n+2);                                    //    Header(n+1)
    e = ToInt(el->lhs());
    elin = &(*elin + *e);                                              //  + I1
    j =  n ;
    for(e=el->rhs(); e && j; e=e->rhs(),j--)
      elin = &(*elin + (*header_rf(ar,ihead,j) * (*ToInt(e->lhs()))));//+ Header(n-k+1)*Ik
  
    return(elin);
}
/*
SgExpression *LinearFormB (SgSymbol *ar, int ihead, int n,  SgExpression *el)
{
  int j;
  SgExpression *elin,*e;
// el - subscript list (I1,I2,...In), n - rank of array (ar)
// generating                         
// [Header(n) +]
//                               n   
//         Header(n+1) +  I1 + SUMMA(Header(n-k+1) * Ik)
//                              k=2
// Header(0:n+1) - distributed array descriptor

    if(n == 0)
      return( header_rf(ar,ihead,2) ); //Header(1) 
    if(!el)      // there aren't any subscripts     
      return( header_rf(ar,ihead,n+1) ); //Header(n)
    if(IN_COMPUTE_REGION)         //ACC
      elin = for_kernel ? NULL : coef_ref(ar,n+2); //ACC
    else                                                           //    Header(n+1)
      elin = header_rf(ar,ihead,n+2);             
    e = el->lhs();
    elin = elin ? &(*elin + *e) : e;                               //  + I1
    j =  n ;
    for(e=el->rhs(); e && j; e=e->rhs(),j--)
      if(IN_COMPUTE_REGION)     //ACC
         elin = &(*elin + (*coef_ref(ar,j) * (*e->lhs())));        
      else                                                          //+ Header(n-k+1)*Ik
         elin = &(*elin + (*header_rf(ar,ihead,j) * (*e->lhs()))); 
  
    return(elin);
}
*/

SgExpression *LinearFormB_for_ComputeRegion (SgSymbol *ar, int n,  SgExpression *el)
{ /*ACC*/
  int j;
  SgExpression *elin,*e;

// el - subscript list (I1,I2,...In), n - rank of remote access buffer (ar)
// generating                         
// [Header(n) +]
//                               n   
//         Header(n+1) +  I1 + SUMMA(Header(n-k+1) * Ik)
//                              k=2
// Header(0:n+1) - distributed array descriptor
//
// for CUDA-kernel
//                               n   
//                             SUMMA(Header(n-k+1) * Ik)
//                              k=1

    if(n == 0)
    { if(for_kernel )   /*ACC*/
        return( new SgValueExp(0) );                               // 0     
      else
        return( coef_ref(ar,2) );                                  // Header(1) - offset
    }

    if(!el)      // there aren't any subscripts     
      return( coef_ref(ar,n+1) ); //Header(n)
   
    elin = for_kernel ? NULL : coef_ref(ar,n+2);                   //    Header(n+1)
    e = ToInt(el->lhs());
    if (for_kernel && options.isOn(AUTO_TFM))     /*ACC*/
      e = &(*coef_ref(ar,n+1) * (*e));                             //    Header(n)*I1  for loop Cuda-kernel
                                                                   // or
    elin = elin ? &(*elin + *e) : e;                               //  [+] I1
    j =  n ;
    for(e=el->rhs(); e && j; e=e->rhs(),j--)
        elin = &(*elin + (*coef_ref(ar,j) * (*ToInt(e->lhs()))));  // +  Header(n-k+1)*Ik

    if(ACROSS_MOD_IN_KERNEL && (e=analyzeArrayIndxs(ar,el)))    /*ACC*/
      elin = &(*elin + *e);  
  
    return(elin);
}


SgExpression * head_ref (SgSymbol *ar, int n) {
// creates array header reference 
       SgValueExp *index = new SgValueExp(n);
       if(ar->thesymb->entry.var_decl.local == IO)  // is dummy argument
          return( new SgArrayRefExp(*ar, *new SgValueExp(1)));
       else 
          return( new SgArrayRefExp(*dvmbuf, *index));
}

SgExpression * header_ref (SgSymbol *ar, int n) {
// creates array header reference: Header(n-1) 
// Header(0:n+1) - distributed array descriptor
      // int ind; 
       return( new SgArrayRefExp(*ar, *new SgValueExp(n)));
    /*
       if(!HEADER(ar))
           return(NULL);
       ind = INDEX(ar);
       if(ind==1) //is not template
          return( new SgArrayRefExp(*ar, *new SgValueExp(n)));
       else
          return( new SgArrayRefExp(*dvmbuf, *new SgValueExp(ind+n-1)));

    */
}

SgExpression * header_ref_in_structure (SgSymbol *ar, int n, SgExpression *struct_) {
// creates array header reference: Header(n-1) 
// Header(0:n+1) - distributed array descriptor
  SgExpression *estr;
       estr = &(struct_->copy());
       estr->setRhs(new SgArrayRefExp(*ar, *new SgValueExp(n)));
       return(estr);
       //return( new SgArrayRefExp(*ar, *new SgValueExp(n)));
}

coeffs *DvmArrayCoefficients(SgSymbol *ar)
{
     if(!ar->attributeValue(0,ARRAY_COEF))  //BY USE
     {
        coeffs *c_new = new coeffs;
        CreateCoeffs(c_new,ar);
        ar->addAttribute(ARRAY_COEF, (void*) c_new, sizeof(coeffs));
     }
     return (coeffs *) ar->attributeValue(0,ARRAY_COEF); 
}

SgExpression * coef_ref (SgSymbol *ar, int n) {
// creates cofficient for dvm-array addressing
//array header reference Header(n)  or its copy reference
// Header(0:n+1) - distributed array descriptor
  if(inparloop && !HPF_program || for_kernel) { /*ACC*/
     coeffs * scoef;
     scoef = AR_COEFFICIENTS(ar); //(coeffs *) ar->attributeValue(0,ARRAY_COEF);
     dvm_ar=AddNewToSymbList(dvm_ar,ar);
     scoef->use = 1;
     return (new SgVarRefExp(*(scoef->sc[n]))); //!!!must be 2<= n <=Rank(ar)+2
     
  } else    
    return( new SgArrayRefExp(*ar, *new SgValueExp(n)));
}

SgExpression * header_rf (SgSymbol *ar, int ihead, int n) {
// creates array header reference: Header(n-1) 
// Header(0:r+1) - distributed array descriptor
       //int ind; 
       if(!ar)
          return( new SgArrayRefExp(*dvmbuf, *new SgValueExp(ihead+n-1)));
       else //(may be hpfbuf in HPF_program)
          return( new SgArrayRefExp(*ar, *new SgValueExp(ihead+n-1)));
 
       //if(!HEADER(ar))
         // return(NULL);
       //ind = INDEX(ar);
       //if(ind==1) //is not template
         // return( new SgArrayRefExp(*ar, *new SgValueExp(n)));
       //else
         // return( new SgArrayRefExp(*dvmbuf, *new SgValueExp(ind+n-1)));
}

SgExpression * acc_header_rf (SgSymbol *ar, int ihead, int n) {
// creates array header reference: Header(n-1) 
// Header(0:r+1) - distributed array descriptor
        
       if(!ar)
          return( new SgArrayRefExp(*dvmbuf, *new SgValueExp(ihead+n-1)));
       else //(may be hpfbuf in HPF_program)
          return( new SgArrayRefExp(*ar, *new SgValueExp(ihead+n-1)));
 
}


SgExpression * HeaderRef (SgSymbol *ar) {
// creates array header reference 
       int ind; 
       if(!HEADER(ar))
           return(NULL);
       ind = INDEX(ar);
       if (ind == 0)   // is pointer
          return(PointerHeaderRef(new SgVarRefExp(ar),1));
       else ///if(ind<=1 || INTERFACE_RTS2) //is not template or interface of RTS2
         return( new SgArrayRefExp(*ar, *new SgValueExp(1)) ); /*10.03.03*/
          /*return( new SgArrayRefExp(*ar)); */
       ///else            //is template in RTS1
       ///  return( new SgVarRefExp(*ar) );
	 //return( new SgArrayRefExp(*dvmbuf, *new SgValueExp(ind)));
}

SgExpression *HeaderRefInd(SgSymbol *ar, int n) {
       int ind;
       if(!HEADER(ar))
         return (NULL);
       ind = INDEX(ar);
       if (ind == 0)   // is pointer
         return(PointerHeaderRef(new SgVarRefExp(ar),n));
       else if(ind<=1) //is not template
         return(new SgArrayRefExp(*ar, *new SgValueExp(n)));
       else            //is template
         return(new SgArrayRefExp(*dvmbuf, *new SgValueExp(ind+n-1)));
}

/*
SgExpression * DistObjectRef (SgSymbol *ar) {
//!!! temporary
// creates distributed object  reference
 int ind;
 ind = INDEX(ar);
 return(head_ref(ar,ind));
}
*/

SgExpression *HeaderNplus1(SgSymbol * ar)   
{
//                                   n
// Header(n+1) = Header(n) -  L1 - SUMMA(Header(n-i+1) * Li)
//                                  i=2   
  SgArrayType *artype;
  SgExpression *ehead,*e;
  SgSubscriptExp *sbe;
  int i,n,ind;

  if(IS_POINTER(ar)){
    // Li=1, i=1,n
    ind = n = PointerRank(ar);
    ehead =  &(*header_ref(ar,ind+1) - (*new SgValueExp(1)));
    for(; ind>=2; ind--)
       ehead = & (*ehead - (*header_ref(ar,ind)));  
    return(ehead);
  }
     
  artype = isSgArrayType(ar->type());
  if(!artype) // error
    return(new SgValueExp(0)); //  for continuing translation of procedure 
  n=artype->dimension();
  if(!n) // error
    return(new SgValueExp(0)); //  for continuing translation of procedure 
  ind = n;
  ehead = &(*header_ref(ar,ind+1) -  LowerBound(ar,0)->copy());
  for(i=2; i<=n; i++,ind--) {
     e = artype->sizeInDim(i-1);
     if((sbe=isSgSubscriptExp(e)) != NULL)
       ehead = & (*ehead - (*header_ref(ar,ind) *
                                                (sbe->lbound()->copy())));
     else
        ehead = & (*ehead - (*header_ref(ar,ind))); // by default Li=1
  }
      //ehead =  & SgUMinusOp(*ehead);
  return(ehead);
}
/*
SgExpression *BufferHeaderNplus1(SgExpression * rme, int n, int ihead)   
{
//                                   n
// Header(n+1) = Header(n) -  L1 - SUMMA(Header(n-i+1) * Li)
//                                  i=2   
  SgArrayType *artype;
  SgExpression *ehead,*e,*el;
  // SgSubscriptExp *sbe;
  SgSymbol *ar;
  int i,ind;
   ar = rme->symbol();
  if(!(ar->attributes() & DIMENSION_BIT)){// for continuing translation
      return (new SgValueExp(0));
  }
   artype = isSgArrayType(ar->type());
   if(!artype) // error
     return(new SgValueExp(0)); //  for continuing translation of procedure 

  ind = n;
  i=0;
  for (el=rme->lhs(); el; el=el->rhs()) //looking through the index list until first ':'element
    if(el->lhs()->variant() == DDOT)
      break;
    else
      i++; 
 if(!(e=LowerBound(ar,i)))
    return(new SgValueExp(0)); //  for continuing translation of procedure  
 else
  ehead = &(* DVM000(ihead+ind) -  e->copy());
  
 for (el=el->rhs(),i++; el; el=el->rhs(),i++) //continue looking through the index list
   if(el->lhs()->variant() == DDOT) {
     ind--;
     e = artype->sizeInDim(i);
     if(e && e->variant() == DDOT && e->lhs())
       ehead = & (*ehead - (*DVM000(ihead+ind) *
                                                (e->lhs()->copy())));
     else
        ehead = & (*ehead - (*DVM000(ihead+ind))); // by default Li=1
   }

  return(ehead);
}
*/

SgExpression *BufferHeaderNplus1(SgExpression * rme, int n, int ihead,SgSymbol *ar)   
{
//                                      n
// Header(n+1) = Header(n) -  L1*S1 - SUMMA(Header(n-i+1) * Li * Si)
//                                     i=2   
// Si = 1, if i-th remote subscript is ':', else Si = 0 
// Li = lower bound of i-th array dimension if ':',  Li = Header(2*n-i+3) - minimum of
// of lower bound and upper bound of corresponding do-variable,if a*i+b 
  SgArrayType *artype;
  SgExpression *ehead,*e,*el;
 
  SgSymbol *array;
  int i,ind,j;
  array = rme->symbol();
  if(!(array->attributes() & DIMENSION_BIT)){// for continuing translation
      return (new SgValueExp(0));
  }
   artype = isSgArrayType(array->type());
   if(!artype) // error
     return(new SgValueExp(0)); //  for continuing translation of procedure 

  ind = n+1; 
  ehead =  header_rf(ar,ihead,ind);

  if(!rme->lhs()) {  // buffer is equal to whole array
    ehead = &(*ehead -  *Exprn(LowerBound(array,0)));
    for(i=1,ind=n;ind>1;ind--,i++){
       e = artype->sizeInDim(i);
       if(e && e->variant() == DDOT && e->lhs())
         ehead = & (*ehead - (*header_rf(ar,ihead,ind) *
                                                  (LowerBound(array,i)->copy())));
       else
         ehead = & (*ehead - (*header_rf(ar,ihead,ind))); // by default Li=1
    }
    return(ehead);
  }

  i=0; j=0;
  for (el=rme->lhs(); el; el=el->rhs()) //looking through the index list until first ':' or do-variable-use element
    if((el->lhs()->variant() == DDOT) || IS_DO_VARIABLE_USE(el->lhs()))
      {j = 1; break;}
    else
      i++; 
 if(j == 0) //buffer is of one element
   return(ehead);  
 if( el->lhs()->variant() == DDOT)// :
  if(!(e=LowerBound(array,i)))
    return(new SgValueExp(0)); //  for continuing translation of procedure  
  else
    ehead = &(*ehead -  e->copy());
 else //a*i+b
    ehead = &(*ehead -  (*header_rf(ar,ihead,ind+n+1)));
 for (el=el->rhs(),i++; el; el=el->rhs(),i++) //continue looking through the index list
   if(el->lhs()->variant() == DDOT)  {
       ind--; 
       e = artype->sizeInDim(i);
       if(e && e->variant() == DDOT && e->lhs())
         ehead = & (*ehead - (*header_rf(ar,ihead,ind) *
                                                  (LowerBound(array,i)->copy())));
       else
         ehead = & (*ehead - (*header_rf(ar,ihead,ind))); // by default Li=1
   }
   else if( IS_DO_VARIABLE_USE(el->lhs())){
       ind--; 
       ehead = & (*ehead - (*header_rf(ar,ihead,ind) * (*header_rf(ar,ihead,ind+n+1))));
   }
  return(ehead);
}



SgExpression *BufferHeader4(SgExpression * rme, int ihead) 
{//temporary
 if(rme)
   return(DVM000(ihead+2));
 else
   return(NULL);
}

SgExpression *LowerBound(SgSymbol *ar, int i)
// lower bound of i-nd dimension of array ar (i= 0,...,Rank(ar)-1)
{
  SgArrayType *artype;
  SgExpression *e;
  SgSubscriptExp *sbe;
  if(IS_POINTER(ar))
    return(new SgValueExp(1));
  artype = isSgArrayType(ar->type());
  if(!artype)
    return(NULL);
  e = artype->sizeInDim(i);
  if(!e) 
    return(NULL);
  if((sbe=isSgSubscriptExp(e)) != NULL) {
    if(sbe->lbound())
      return(IS_BY_USE(ar) ? Calculate(sbe->lbound()) : sbe->lbound());
    else if(IS_ALLOCATABLE_POINTER(ar) || IS_TEMPLATE(ar)) {       
      if(HEADER(ar))
        return(header_ref(ar,Rank(ar)+3+i));
      else
        return(LBOUNDFunction(ar,i+1));
    }
    else
      return(new SgValueExp(1)); 
  }
  else
    return(new SgValueExp(1));  // by default lower bound = 1      
}         

SgExpression *UpperBound(SgSymbol *ar, int i)
// upper bound of i-nd dimension of array ar (i= 0,...,Rank(ar)-1)
{
  SgArrayType *artype;
  SgExpression *e;
  SgSubscriptExp *sbe;
  int ri;   //06.11.09
  ri = Rank(ar) - i;
  if(IS_POINTER(ar))
    return(GetSize(HeaderRefInd(ar,1), ri)); //i+1));  6.11.09
  artype = isSgArrayType(ar->type());
  if(!artype)
    return(NULL);
  e = artype->sizeInDim(i);
  if(!e) 
    return(NULL);
  if((sbe=isSgSubscriptExp(e)) != NULL){
    if(sbe->ubound())
      return(IS_BY_USE(ar) ? Calculate(sbe->ubound()) : sbe->ubound());
    else if(HEADER(ar))
              //return(&(*GetSize(HeaderRefInd(ar,1),i+1)-*HeaderRefInd(ar,Rank(ar)+3+i)+*new SgValueExp(1))); 06.11.09
      return(&(*GetSize(HeaderRefInd(ar,1),ri)+*HeaderRefInd(ar,Rank(ar)+3+i)-*new SgValueExp(1)));
    else
      return(UBOUNDFunction(ar,i+1));
  }
  else
    return(e);  
// !!!! test case "*"
}     

void ShadowList (SgExpression *el, SgStatement *st, SgExpression *gref)
{
  int corner;
  int  ileft,iright;
  //int ibsize = 0;
  SgExpression *es, *ear, *head, *shlist[1]; 
  SgSymbol *ar;
  // looking through the array_with_shadow_list
  for(es = el; es; es = es->rhs()) {
     ear = es->lhs(); // array_with_shadow (variant:ARRAY_REF or ARRAY_OP)
     if(ear->variant() == ARRAY_OP) {
        corner = 1;
        ear = ear->lhs();   
     }
     else
        corner = 0;
     ar = ear->symbol();
     if(HEADER(ar))
       head = HeaderRef(ar);
     else {
       Error("'%s' isn't distributed array", ar->identifier(),72, st);
       return;
     }
     if(gref)  //interface of RTS1
     {
        if(ear->lhs()){
           ileft = ndvm;  
           iright = doShadSizeArrays(ear->lhs(), ear->symbol(), st, NULL);
        } else
            ileft=iright= doShadSizeArrayM1(ar,NULL);
 
        doCallAfter(InsertArrayBound(gref, head, ileft, iright, corner));
         
     } else  //interface of RTS2
     {
        if(ear->lhs()) 
        { 
           doShadSizeArrays(ear->lhs(), ear->symbol(), st, shlist);
           if(*shlist)
             doCallAfter(ShadowRenew_H2(head,corner,Rank(ar),*shlist));   
           //doCallAfter(ShadowRenew_H2(Register_Array_H2(head),corner,Rank(ar),*shlist));   
        }
        else
           doCallAfter(ShadowRenew_H2(head,corner,0,NULL));  
           //doCallAfter(ShadowRenew_H2(Register_Array_H2(head),corner,0,NULL));          
     }
  }
}

int doShadSizeArrayM1(SgSymbol *ar, SgExpression **shlist)
{
 int n,i;
 int ileft;
 n = Rank(ar);
 if(!shlist)
 { 
    ileft = ndvm;
    for(i=0; i<n; i++)
       doAssignStmtAfter(new SgValueExp(-1));  
    return(ileft);
 }
 *shlist = NULL;
 for(i=0; i<2*n; i++)
    *shlist = AddListToList(*shlist,new SgExprListExp(*ConstRef_F95(-1)));
// *shlist = AddListToList(*shlist,&(*shlist)->copy());
 return (0); 
}
  
int doShadSizeArrays(SgExpression *shl, SgSymbol *ar, SgStatement *st, SgExpression **shlist) 
{
 int rank,nw;
 int i=0,iright=0,j=0;
 SgExpression *wl,*ew,*lbound[MAX_DIMS], *ubound[MAX_DIMS];
 rank = Rank(ar);
 if(!TestMaxDims(shl,ar,st)) 
     return (0);
 for(wl = shl; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     if(ew->variant() == SHADOW_NAMES_OP) {
       lbound[i] = new SgValueExp(0); 
       ubound[i] = new SgValueExp(0);
       j++;
       if(!shlist)  //interface of RTS1
         Error("Illegal shadow width specification of array '%s'", ar->identifier(), 56, st); 
       else        //interface of RTS2 
         ShadowNames(ar,rank-i,ew->lhs());               
     }
     else if(ew->variant() == DDOT) {
       lbound[i] = &(ew->lhs())->copy();//left bound 
       ubound[i] = &(ew->rhs())->copy();//right bound 
     } else {
       lbound[i] = &(ew->copy());//left bound == right bound
       ubound[i] = &(ew->copy());
     }
 }
  nw = i; 
  TestShadowWidths(ar, lbound, ubound, nw, st);
  if (nw != rank) {// wrong shadow width list length
     Error("Length of shadow-edge-list is not equal to the rank of array '%s'", ar->identifier(), 88, st); 
     return(0);
  }
  if(shlist && j==i)  //interface of RTS2 
  {
    *shlist = NULL;
    return(0);
  }
  if(!shlist) //interface of RTS1 
  {
     for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(lbound[i]);
     iright = ndvm;
     for(i=rank-1;i>=0; i--)
        doAssignStmtAfter(ubound[i]);      
  } else     //interface of RTS2
  {
     *shlist = NULL; 
     for(i=rank-1;i>=0; i--)
     { 
        *shlist = AddListToList(*shlist,new SgExprListExp(*DvmType_Ref(lbound[i])) );
        *shlist = AddListToList(*shlist,new SgExprListExp(*DvmType_Ref(ubound[i])) );
     }
  }
  return(iright);
}

void ShadowNames(SgSymbol *ar, int axis, SgExpression *shadow_name_list)
{
 SgExpression *nml;
 SgExpression *head=HeaderRef(ar);
 if(!head) return;
 for(nml = shadow_name_list; nml; nml = nml->rhs())   
   doCallAfter(IndirectShadowRenew(head,axis,nml->lhs()));
}

void TestShadowWidths(SgSymbol *ar, SgExpression * lbound[], SgExpression * ubound[], int nw, SgStatement *st)
  //compare shadow widths with that specified for array 'ar' in SHADOW directive
  // or SHADOW attribute of combined directive
{SgExpression *lw[MAX_DIMS], *uw[MAX_DIMS],**pe,*wl,*ew;
 int i,n;
 pe=SHADOW_(ar);
 if(pe){ //distributed array has SHADOW attribute
 //looking through the shadow width list of SHADOW directive/attribute 
   if(!TestMaxDims(*pe,ar,0)) return;
   for(wl = *pe, i=0; wl; wl = wl->rhs(),i++) {
     ew = wl->lhs();
     if(ew->variant() == DDOT){
       lw[i] = ew->lhs();//left bound 
       uw[i] = ew->rhs();//right bound 
     }
     else {
       lw[i] = ew;//left bound == right bound
       uw[i] = ew;
     }
   }
   n = i;
   for(i=0; i<nw && i<n; i++){
     if(lbound[i]->isInteger() && lw[i]->isInteger() && lbound[i]->valueInteger() > lw[i]->valueInteger() )
       Error("Low shadow width  of  '%s' is greater than the corresponding one specified in SHADOW directive", ar->identifier(), 142,st); 
     if(ubound[i]->isInteger() && uw[i]->isInteger() && ubound[i]->valueInteger() > uw[i]->valueInteger() )
       Error("High shadow width  of  '%s' is greater than the corresponding one specified in SHADOW directive", ar->identifier(), 143,st); 
   }
 }
 else  {//by default shadow width = 1
   if(!IS_DUMMY(ar) && HEADER(ar))     
     for(i=0; i<nw; i++){
       if(lbound[i]->isInteger() && lbound[i]->valueInteger() > 1 )
         Error("Low shadow width  of  '%s' is greater than 1", ar->identifier(), 144,st); 
       if(ubound[i]->isInteger() && ubound[i]->valueInteger() > 1 )
         Error("High shadow width  of  '%s' is greater than 1", ar->identifier(), 145,st);
    }
 }
} 

SgExpression *DeclaredShadowWidths(SgSymbol *ar)
{
  SgExpression **pe,*wl,*ew, *shlist=NULL;
  int i;
  pe=SHADOW_(ar);
  if(pe) //distributed array has SHADOW attribute
  {
     //looking through the shadow width list of SHADOW directive/attribute 
     for(wl = *pe, i=0; wl; wl = wl->rhs(),i++) {
        ew = wl->lhs();
        if(ew->variant() == DDOT){
          shlist = AddElementToList(shlist, DvmType_Ref(ew->rhs()));
          shlist = AddElementToList(shlist, DvmType_Ref(ew->lhs()));
        }
        else {
          shlist = AddElementToList(shlist, DvmType_Ref(ew));
          shlist = AddElementToList(shlist, DvmType_Ref(ew));
        }
     }
  }
  else  //by default shadow width = 1
  {
     int rank = Rank(ar);
     for (i=0; i<rank; i++) { 
          shlist = AddElementToList(shlist, ConstRef_F95(1));
          shlist = AddElementToList(shlist, ConstRef_F95(1));
     }
  }
  return(shlist);
} 


void ShadowComp (SgExpression *ear, SgStatement *st, int ilh)
{
  int  ileft,iright;
  SgExpression *head,*shlist[1];
  SgSymbol *ar;
    
     // array_with_shadow (variant:ARRAY_REF)
     ar = ear->symbol();
     if(HEADER(ar))
        head = HeaderRef(ar);
     else {
        Error("'%s' isn't distributed array", ar->identifier(),72, st);
        return;
     }
     if(st->expr(0)->symbol() != ar){ 
        Error("Illegal array in SHADOW_COMPUTE clause: %s", ar->identifier(),264, st);
     }
     if(!ilh)  //interface of RTS1
     {
        if(ear->lhs()){
           ileft = ndvm;  
           iright = doShadSizeArrays(ear->lhs(), ar, st, NULL);
        } else
           ileft=iright= doShadSizeArrayM1(ar, NULL);
        doCallAfter(AddBoundShadow(head, ileft, iright));
  
     } else    //interface of RTS2
        if(ear->lhs()){
           doShadSizeArrays(ear->lhs(), ar, st, shlist);     
           doCallAfter(ShadowCompute(ilh,head,Rank(ar),*shlist));
           //doCallAfter(ShadowCompute(ilh,Register_Array_H2(head),Rank(ar),*shlist));
        } else
           doCallAfter(ShadowCompute(ilh,head,0,NULL));
           //doCallAfter(ShadowCompute(ilh,Register_Array_H2(head),0,NULL));
}

symb_list *DerivedRhsAnalysis(SgExpression *derived_op,SgStatement *stmt, int &nd)
{
  SgExpression *el;
  symb_list *dummy_list = NULL;
  SgSymbol *s_dummy = NULL;
  nd = 0;
  // looking through the rhs of derived_op ( WITH target_spec )
  for(el=derived_op->rhs()->lhs();el;el=el->rhs())
  {
     if(el->lhs()->variant() == DUMMY_REF) // @align-dummy[ + shadow-name ]...
     {
        s_dummy = el->lhs()->symbol();
        dummy_list = AddNewToSymbList(dummy_list,s_dummy);
        nd++;
     }
  }
/*
  if(!s_dummy)  //???
    err("Illegal DERIVED/SHADOW_ADD specification", 629, stmt);
*/
  //reversing dummy_list
  symb_list *sl = NULL;
  for( ; dummy_list; dummy_list=dummy_list->next)
     sl= AddNewToSymbList(sl,dummy_list->symb);
  return (sl); //(dummy_list);
}

int is_derived_dummy(SgSymbol *s, symb_list *dummy_list)
{
  symb_list *sl;
  for(sl=dummy_list; sl; sl=sl->next)
     if(s == sl->symb)  return 1;
  return 0;
}

symb_list *DerivedElementAnalysis(SgExpression *e, symb_list *dummy_list, symb_list *arg_list, SgStatement *stmt)
{
  if(!e)
     return (arg_list);
  if(isSgValueExp(e))
     return (arg_list);

  if(isSgVarRefExp(e) && !is_derived_dummy(e->symbol(),dummy_list) || e->variant() == CONST_REF)
  {
     arg_list = AddNewToSymbList(arg_list,e->symbol());
     return (arg_list); 
  }

  if(isSgArrayRefExp(e) )  //!!! look trough the tree
  {
     if(HEADER(e->symbol()))
        arg_list = AddNewToSymbList(arg_list,e->symbol());
     else
        Error("Illegal use of array '%s' in DERIVED/SHADOW_ADD, not implemented yet",e->symbol()->identifier(), 629, stmt);      
     arg_list = DerivedElementAnalysis(e->lhs(), dummy_list, arg_list, stmt);
     return (arg_list);
  }
  
  arg_list = DerivedElementAnalysis(e->lhs(), dummy_list, arg_list, stmt);   
  arg_list = DerivedElementAnalysis(e->rhs(), dummy_list, arg_list, stmt);
  return (arg_list);   
}

symb_list *DerivedLhsAnalysis(SgExpression *derived_op, symb_list *dummy_list, SgStatement *stmt)
{
  SgExpression *el,*e;
  symb_list *arg_list = NULL, *sl;
  SgExpression *elhs = derived_op->lhs(); //derived_elem_list
  // looking through the lhs of derived_op (derived_elem_list)
  
  for(el=elhs; el; el=el->rhs())
  {
     e = el->lhs();  // derived_elem
     arg_list = DerivedElementAnalysis(e, dummy_list, arg_list, stmt);
  }
  return (arg_list);   
}

SgExpression *FillerActualArgumentList(symb_list *paramList, int &nArg)
{
  SgExpression *arg_expr_list = NULL;
  symb_list *sl;
  nArg = 0;
  for (sl = paramList; sl; sl=sl->next)
  { 
     if(isSgArrayType(sl->symb->type()))
     {  
        if(!HEADER(sl->symb)) 
          continue; 
        arg_expr_list = AddListToList(arg_expr_list,new SgExprListExp(*new SgArrayRefExp(*sl->symb)));
        arg_expr_list = AddListToList(arg_expr_list,ElementOfAddrArgumentList(sl->symb));
        nArg+=2;
     }
     else
     {
        arg_expr_list = AddListToList(arg_expr_list,new SgExprListExp(*new SgVarRefExp(*sl->symb)));
        nArg++;
     }
  }
  return arg_expr_list;
}

void DerivedSpecification(SgExpression *edrv, SgStatement *stmt, SgExpression *eFunc[])
{
  int narg = 0, nd = 0;
  symb_list *dummy_list   = DerivedRhsAnalysis(edrv,stmt,nd);
  symb_list *paramList    = DerivedLhsAnalysis(edrv,dummy_list,stmt); 
  SgSymbol *sf_counter    = IndirectFunctionSymbol(stmt,"counter");
  SgSymbol *sf_filler     = IndirectFunctionSymbol(stmt,"filler");
  SgStatement *st_counter = CreateIndirectDistributionProcedure(sf_counter, paramList, dummy_list, edrv->lhs(), 0);
  SgStatement *st_filler  = CreateIndirectDistributionProcedure(sf_filler,  paramList, dummy_list, edrv->lhs(), 1);
  st_counter->addComment(Indirect_ProcedureComment(stmt->lineNumber())); 
  SgExpression *argument_list = FillerActualArgumentList(paramList,narg);  
  eFunc[0] = HandlerFunc (sf_counter, narg, argument_list);  // counter function
  eFunc[1] = HandlerFunc (sf_filler,  narg, argument_list ? &argument_list->copy() : NULL); // filler function
  return;
}

void Shadow_Add_Directive(SgStatement *stmt)
{
  int n,iaxis;
  SgExpression *el,*edrv;
  for (el=stmt->expr(2),n=0; el; el=el->rhs(),n++)
     ; //el->setLhs(HeaderRef(el->lhs()->symbol()));HederRef() for each element of el->lhs()
  int rank = Rank(stmt->expr(0)->symbol());
  for (el=stmt->expr(0)->lhs(),iaxis=rank; el; el=el->rhs(),iaxis--)
     if(el->lhs()->variant()==DERIVED_OP) 
     { 
        edrv = el->lhs();    
        break;
     }
  SgExpression *eFunc[2];
  DerivedSpecification(edrv, stmt, eFunc);
  doCallAfter(ShadowAdd(HeaderRef(stmt->expr(0)->symbol()),iaxis,DvmhDerivedRhs(edrv->rhs()),eFunc[0],eFunc[1],stmt->expr(1),n,stmt->expr(2)));
  return;
} 

int doAlignIteration(SgStatement *stat, SgExpression *aref)
{
  SgExpression *axis[MAX_LOOP_LEVEL],
               *coef[MAX_LOOP_LEVEL],
               *cons[MAX_LOOP_LEVEL];
  int i;
  int nt = Alignment(stat,aref,axis,coef,cons,0);
  // setting on arrays
  for(i=nt-1; i>=0; i--)
     doAssignStmtAfter(axis[i]);
  for(i=nt-1; i>=0; i--)
     doAssignStmtAfter(ReplaceFuncCall(coef[i]));
  for(i=nt-1; i>=0; i--)
     doAssignStmtAfter(Calculate(cons[i]));
  return(nt);   
}

int Alignment(SgStatement *stat, SgExpression *aref, SgExpression *axis[], SgExpression *coef[], SgExpression *cons[],int interface)
// creating axis_array, coeff_array and  const_array 
// returns the number of elements in align_iteration_list

{ int i,ni,nt,num, use[MAX_LOOP_LEVEL];
  SgExpression * el,*e,*ei,*elbb, *es;
  SgSymbol *l_var[MAX_LOOP_LEVEL], *ar;
  SgValueExp c1(1),c0(0),cM1(-1);
  
 
  ni = 0; //counter of elements in loop_control_variable_list
  //looking through the loop_control_variable_list 
  for(el=stat->expr(2); el; el=el->rhs())   {
       l_var[ni] = (el->lhs())->symbol(); 
       use[ni] = 0;
       ni++;
  }
  es = aref ? aref : stat->expr(0);
  ar = es->symbol();    // array  
   
  //looking through the align_iteration_list 
  nt = 0;          //counter of elements in align_iteration_list
  for(el=es->lhs(); el; el=el->rhs())   {
     e = el->lhs();  //subscript expression
     if(e->variant()==KEYWORD_VAL || e->variant()==DDOT) {  // "*" or ":"
       axis[nt] = & cM1.copy();
       coef[nt] = & c0.copy();
       cons[nt] = & c0.copy();   
     }
     
     else  {  // expression
       num = AxisNumOfDummyInExpr(e, l_var, ni, &ei, use, stat);
       //printf("\nnum = %d\n", num);
       if (num<=0)   {
         axis[nt] = & c0.copy();
         coef[nt] = & c0.copy();
         cons[nt] = & (e->copy()); 
         if((elbb = LowerBound(ar,nt)) != NULL && interface != 2)
           cons[nt] = & (*cons[nt] - (elbb->copy()));
                        // correcting const with lower bound of array, if interface != 2
       }                
       else {
         axis[nt] = new SgValueExp(num); 
         CoeffConst(e, ei, &coef[nt], &cons[nt]); 
         if(interface != 2)
           TestReverse(coef[nt],stat);     
         if(!coef[nt]){
           err("Wrong iteration-align-subscript in PARALLEL", 160,stat);
           coef[nt] = & c0.copy();
           cons[nt] = & c0.copy();
         }  
         else 
         // correcting const with lower bound of array, if interface != 2
           if((elbb = LowerBound(ar,nt)) != NULL  && interface != 2 )
             cons[nt] = &(*cons[nt]  - (elbb->copy()));
       }       
     }
       
     nt++;
  }
  
  if(Rank(ar) &&  Rank(ar) != nt) 
    Error("Rank of array '%s' isn't equal to the length of iteration-align-subscript-list", ar->identifier(), 161,stat);

  return(nt);  
}

int DefineLoopNumberForDimension(SgStatement * stat,int loop_num[])
{ int ni,nt,num, use[MAX_LOOP_LEVEL];
  SgExpression * el,*e,*ei;
  SgSymbol *l_var[MAX_LOOP_LEVEL], *ar;
   
  ni = 0; //counter of elements in loop_control_variable_list
  //looking through the loop_control_variable_list 
  for(el=stat->expr(2); el; el=el->rhs())   {
       l_var[ni] = (el->lhs())->symbol(); 
       use[ni] = 0;
       ni++;
  }
  ar = stat->expr(0)->symbol();    // array  
   
  //looking through the align_iteration_list 
  nt = 0;          //counter of elements in align_iteration_list
  for(el=(stat->expr(0))->lhs(); el; el=el->rhs())   {
     e = el->lhs();  //subscript expression
     if(e->variant()==KEYWORD_VAL) {  // "*"
       loop_num[nt] = 0; // -1;

     }
     
     else  {  // expression
       num = AxisNumOfDummyInExpr(e, l_var, ni, &ei, use, stat);
       //printf("\nnum = %d\n", num);
       if (num<=0)   
         loop_num[nt] = 0;
       else 
         loop_num[nt] = num;      
     }
       
     nt++;
  }
  
 
  return(nt);  
}

int RedFuncNumber(SgExpression *kwe)
{
  char *red_name;
  //PTR_LLND thellnd;
  red_name   = ((SgKeywordValExp *) kwe)->value();
//  red_name  = NODE_STRING_POINTER(kwe->thellnd);
  if(!strcmp(red_name, "sum"))
    return(1);
  if(!strcmp(red_name, "product"))
    return(2);
  if(!strcmp(red_name, "max"))
    return(3);
  if(!strcmp(red_name, "min"))
    return(4);
  if(!strcmp(red_name, "and"))
    return(5);
  if(!strcmp(red_name, "or"))
    return(6);
  if(!strcmp(red_name, "neqv"))
    return(7);
  if(!strcmp(red_name, "eqv"))
    return(8);
  if(!strcmp(red_name, "maxloc"))
    return(9);
  if(!strcmp(red_name, "minloc"))
    return(10);

  return(0);
}

int RedFuncNumber_2(int num)
{  //MAXLOC: 9=>11, MINLOC: 10=>12
  return(num>8 ? num+2 : num);
}

int VarType_RTS(SgSymbol *var)
{int t;
 t=TestType(var->type());
 if(t==7) //LOGICAL
   t=(bind==0) ? 2 : 1;  //there is not LOGICAL type in RTS
 return(t);  
}

int VarType(SgSymbol *var)
{ if(IS_POINTER_F90(var) )
     return(0);
  else
     return (TestType(var->type())); 
}

int TestType_DVMH(SgType *type)
{ 
  if(!type)
    return(-1);
  
  SgArrayType *artype = isSgArrayType(type);
  if(artype)
    type = artype->baseType();
  switch(type->variant()) 
  {
        case T_BOOL:     
        case T_INT:        return(1);
                        
  
        case T_FLOAT:    
        case T_DOUBLE:     return(3);
                        

        case T_COMPLEX:  
        case T_DCOMPLEX:   return(5);
                       

        default:           return(-1);
  }

}

int TestType_RTS(SgType *type)
{ int t;
 t=TestType(type);
 if(t==7) //LOGICAL
   t=(bind==0) ? 2 : 1;  //there is not LOGICAL type in RTS
  return (t);
}

int TestType(SgType *type)
{ int len;
  SgArrayType *artype;

  if(!type)
    return(0);
  
  artype=isSgArrayType(type);
  if(artype)
    type = artype->baseType();
  len = TypeSize(type); /*16.04.04*/
      //len = IS_INTRINSIC_TYPE(type) ? 0 : TypeSize(type);
      //len = (TYPE_RANGES(type->thetype)) ? type->length()->valueInteger() : 0; 14.03.03
  if(bind == 0)
  switch(type->variant()) {
        case T_BOOL:    if     (len == 4) return(7); /*14.11.06 type LOGICAL was introduced in debuger*/
                        else              return(0); 

        case T_INT:     if     (len == 4) return(1); /*3.11.06  2 => 1 */
                        else              return(0);
  
        case T_FLOAT:   if     (len == 8) return(4);
                        else if(len == 4) return(3);
                        else              return(0);
 
        case T_DOUBLE:  if     (len == 8) return(4);
                        else              return(0); 

        case T_COMPLEX: if     (len ==16) return(6);
                        else if(len == 8) return(5);
                        else              return(0);
 
        case T_DCOMPLEX:if     (len ==16) return(6);
                        else   return(0);

        default:        return(0);
  }
  if(bind == 1)
  switch(type->variant()) {
        case T_BOOL:    if     (len == 8) return(2);
                        else if(len == 4) return(7); /*14.11.06 type LOGICAL was introduced in debuger*/ 
                        else              return(0); 
        case T_INT:     if     (len == 8) return(2);
                        else if(len == 4) return(1);
                        else              return(0);  
        case T_FLOAT:   if     (len == 8) return(4);
                        else if(len == 4) return(3);
                        else              return(0); 
        case T_DOUBLE:  if     (len == 8) return(4);
                        else              return(0); 

        case T_COMPLEX: if     (len ==16) return(6);
                        else if(len == 8) return(5);
                        else              return(0); 
        case T_DCOMPLEX:if     (len ==16) return(6);
                        else   return(0);
        default:        return(0);
  }
  return(0);
}

/*RTS2*/
#define rt_UNKNOWN (-1)
#define rt_CHAR 0
#define rt_INT 1
#define rt_LONG 2
#define rt_FLOAT 3
#define rt_DOUBLE 4
#define rt_FLOAT_COMPLEX 5
#define rt_DOUBLE_COMPLEX 6
#define rt_LOGICAL 7
#define rt_LLONG 8
#define rt_UCHAR 9
#define rt_UINT 10
#define rt_ULONG 11
#define rt_ULLONG 12
#define rt_SHORT 13
#define rt_USHORT 14

int TestType_RTS2(SgType *type)
{ int len;
  SgArrayType *artype;

  if(!type)
    return(rt_UNKNOWN);
  
  artype=isSgArrayType(type);
  if(artype)
    type = artype->baseType();
  len = TypeSize(type); 
  if(bind == 0)
  switch(type->variant()) {
        case T_BOOL:    if     (len == 4) return(rt_LOGICAL);
                        else if(len == 2) return(rt_USHORT);  
                        else              return(rt_UNKNOWN); 

        case T_INT:     if     (len == 4) return(rt_INT); 
                        else if(len == 2) return(rt_SHORT);
                        else              return(rt_UNKNOWN);
  
        case T_FLOAT:   if     (len == 8) return(rt_DOUBLE);
                        else if(len == 4) return(rt_FLOAT);
                        else              return(rt_UNKNOWN);
 
        case T_DOUBLE:  if     (len == 8) return(rt_DOUBLE);
                        else              return(rt_UNKNOWN); 

        case T_COMPLEX: if     (len ==16) return(rt_DOUBLE_COMPLEX);
                        else if(len == 8) return(rt_FLOAT_COMPLEX);
                        else              return(rt_UNKNOWN);
 
        case T_DCOMPLEX:if     (len ==16) return(rt_DOUBLE_COMPLEX);
                        else   return(rt_UNKNOWN);
        case T_STRING:
        case T_CHAR:    if     (len == 1) return(rt_CHAR);
                        else   return(rt_UNKNOWN);

        default:        return(rt_UNKNOWN);
  }
  if(bind == 1)
  switch(type->variant()) {

        case T_BOOL:    if     (len == 8) return(rt_ULONG);
                        else if(len == 4) return(rt_LOGICAL);
                        else if(len == 2) return(rt_USHORT); 
                        else              return(rt_UNKNOWN); 
        case T_INT:     if     (len == 8) return(rt_LONG);
                        else if(len == 4) return(rt_INT);
                        else if(len == 2) return(rt_SHORT);
                        else              return(rt_UNKNOWN);  
        case T_FLOAT:   if     (len == 8) return(rt_DOUBLE);
                        else if(len == 4) return(rt_FLOAT);
                        else              return(rt_UNKNOWN); 
        case T_DOUBLE:  if     (len == 8) return(rt_DOUBLE);
                        else              return(rt_UNKNOWN); 

        case T_COMPLEX: if     (len ==16) return(rt_DOUBLE_COMPLEX);
                        else if(len == 8) return(rt_FLOAT_COMPLEX);
                        else              return(rt_UNKNOWN); 
        case T_DCOMPLEX:if     (len ==16) return(rt_DOUBLE_COMPLEX);
                        else   return(rt_UNKNOWN);
        case T_STRING:
        case T_CHAR:    if     (len == 1) return(rt_CHAR);
                        else   return(rt_UNKNOWN);

        default:        return(rt_UNKNOWN);
  }
  return(rt_UNKNOWN);
}

SgExpression *TypeSize_RTS2(SgType *type)
{
  SgArrayType *artype=isSgArrayType(type);
  if(artype)
    type = artype->baseType();
  int it = TestType_RTS2(type);
  SgExpression *ts = it >= 0 ? &SgUMinusOp(*ConstRef(it)) : ConstRef_F95(TypeSize(type));
  return(ts);
}

int DVMType()
{return(2);}

int NameIndex(SgType *type)
{int len;
   len = TypeSize(type);   //IS_INTRINSIC_TYPE(type) ? 0 : TypeSize(type);                          
   switch ( type->variant()) {
      case T_INT:      return (GETAI);
      case T_FLOAT:    return((len == 8) ? GETAD : GETAF); 
      case T_BOOL:     return (GETAL);
      case T_DOUBLE:   return (GETAD);
      case T_COMPLEX:  return (GETAC);
      case T_DCOMPLEX: return (GETAC);
      case T_STRING:   return (GETACH);
      case T_CHAR:     return (GETACH);
      default:         return (GETAI);
      }
}

SgType *Base_Type(SgType *type)
{ return ( isSgArrayType(type) ? type->baseType() : type);}
 
void doLoopStmt(SgStatement *st)
{
 SgStatement *dost, *contst;
 SgValueExp c1(1);
 SgLabel *loop_lab; 
 SgSymbol *sio;
 int i;
//!!!
 nio = 3;
//!!!
 sio = st->expr(0)->lhs()->symbol();
 buf_use[TypeIndex(sio->type()->baseType())] = 1;
// SgSymbol * dovar = new SgVariableSymb("IDVM01",*SgTypeInt(), *func);
 loop_lab = GetLabel();
 contst = new SgStatement(CONT_STAT);
 dost= new SgForStmt(*loop_var[0], c1.copy(), c1.copy(), c1.copy(), *contst);
 BIF_LABEL_USE(dost->thebif) = loop_lab->thelabel;
 (dost->lexNext())->setLabel(*loop_lab); 
 for(i=1; i<3; i++){
   dost= new SgForStmt(*loop_var[i], c1.copy(), c1.copy(), c1.copy(), 
                                               *dost);
   BIF_LABEL_USE(dost->thebif) = loop_lab->thelabel;
 }

 st->insertStmtAfter(*dost);
 for(i=0; i<3; i++)
  contst->lexNext()->extractStmt(); 
    //dost->lexNext()->lexNext()->lexNext()->extractStmt();
    //dost->lexNext()->lexNext()->lexNext()->extractStmt();

     // generating the construction IF () THEN <   > ELSE <   > ENDIF 
     // and then insert it before CONTINUE statement
 /*  SgStatement *if_stmt =new SgIfStmt(*(current->controlParent())->expr(0)                                , *current);
      contst -> insertStmtBefore(*if_stmt);
 */
 cur_st = contst;
} 

SgExpression *ReplaceParameter(SgExpression *e)
{
  if(!e)
    return(e);
  if(e->variant() == CONST_REF) {
     SgConstantSymb * sc =  isSgConstantSymb(e->symbol());
     if(!sc->constantValue())
     {  Err_g("An initialization expression is missing: %s",sc->identifier(),267);
        return(e);
     }
     return(ReplaceParameter(&(sc->constantValue()->copy())));
  }
  e->setLhs(ReplaceParameter(e->lhs()));
  e->setRhs(ReplaceParameter(e->rhs()));
  return(e);
}

SgExpression *ReplaceFuncCall(SgExpression *e)
{
  if(!e)
    return(e);
  if(isSgFunctionCallExp(e) && e->symbol()) {//function call
     if( !e->lhs()  && (!strcmp(e->symbol()->identifier(),"number_of_processors") || !strcmp(e->symbol()->identifier(),"actual_num_procs") || !strcmp(e->symbol()->identifier(),"number_of_nodes"))) {             //NUMBER_OF_PROCESSORS() or                                                         // ACTUAL_NUM_PROCS() or NUMBER_OF_NODES()
    SgExprListExp *el1,*el2;
    if(!strcmp(e->symbol()->identifier(),"number_of_processors"))
      el1 = new SgExprListExp(*ParentPS());
    else
      el1 = new SgExprListExp(*CurrentPS());
    el2 = new SgExprListExp(*ConstRef(0));
    e->setSymbol(fdvm[GETSIZ]);
    fmask[GETSIZ] = 1;
    el1->setRhs(el2);
    e->setLhs(el1);
    return(e);
    }

   if( !e->lhs() && (!strcmp(e->symbol()->identifier(),"processors_rank"))) {
                                                                //PROCESSORS_RANK()
    SgExprListExp *el1;
    el1 = new SgExprListExp(*ParentPS());
    e->setSymbol(fdvm[GETRNK]);
    fmask[GETRNK] = 1;
    e->setLhs(el1);
    return(e);
    }

   if(!strcmp(e->symbol()->identifier(),"processors_size")) {
                                                               //PROCESSORS_SIZE()
    SgExprListExp *el1;
    el1 = new SgExprListExp(*ParentPS());
    e->setSymbol(fdvm[GETSIZ]);
    fmask[GETSIZ] = 1;
    el1->setRhs(*(e->lhs())+(*ConstRef(0)));  //el1->setRhs(e->lhs());
    e->setLhs(el1);
    return(e);
   }
  }
  e->setLhs(ReplaceFuncCall(e->lhs()));
  e->setRhs(ReplaceFuncCall(e->rhs()));
  return(e);
}

SgExpression *Calculate(SgExpression *e)
{ SgExpression *er;
   er  = ReplaceParameter( &(e->copy())); 
   if(er->isInteger())
      return( new SgValueExp(er->valueInteger()));
    else
      return(ReplaceFuncCall(e));
}

int ExpCompare(SgExpression *e1, SgExpression *e2)
{//compares two expressions
// returns 1 if they are textually identical
  if(!e1 && !e2) // both expressions are null
      return(1);
  if(!e1 || !e2) // one of them is null
      return(0);
  if(e1->variant() != e2->variant()) // variants are not equal
      return(0);
  switch (e1->variant()) {
      case INT_VAL: 
          return(NODE_IV(e1->thellnd) == NODE_IV(e2->thellnd));
      case BOOL_VAL:
          return(NODE_BOOL_CST(e1->thellnd) == NODE_BOOL_CST(e2->thellnd)); 
      case FLOAT_VAL: 
      case DOUBLE_VAL:  
      case CHAR_VAL:
      case STRING_VAL: 
	  return(!strcmp(NODE_STR(e1->thellnd),NODE_STR(e2->thellnd)));
      case COMPLEX_VAL:
          return(ExpCompare(e1->lhs(),e2->lhs()) && ExpCompare (e1->rhs(),e2->rhs()));   
      case CONST_REF:
      case VAR_REF:
          return(e1->symbol() == e2->symbol());
      case ARRAY_REF:
      case FUNC_CALL:
          if(e1->symbol() == e2->symbol())
            return(ExpCompare(e1->lhs(),e2->lhs())); // compares subscript/argument lists
          else
            return(0);
      case EXPR_LIST:
	 {SgExpression *el1,*el2;
          for(el1=e1,el2=e2; el1&&el2; el1=el1->rhs(),el2=el2->rhs())
	     if(!ExpCompare(el1->lhs(),el2->lhs()))  // the corresponding elements of lists are not identical
               return(0);
          if(el1 || el2) //one list is shorter than other
             return(0);
          else
             return(1);
	 } 
      case MINUS_OP:  //unary operations
      case NOT_OP:
	  return(ExpCompare(e1->lhs(),e2->lhs())); // compares operands    
      default:
          return(ExpCompare(e1->lhs(),e2->lhs()) && ExpCompare (e1->rhs(),e2->rhs())); 
  }
}

int RemAccessRefCompare(SgExpression *e1, SgExpression *e2)
{ // returns 1 if e2 ArrayRef in current statement is identical the e1 ArrayREf in precedent REMOTE_ACCESS statement  
  SgExpression *el1, *el2; 
  if(!e1) // for error situation in REMOTE_ACCESS
      return(0);
 
  if(e1->variant() != e2->variant()) // variants are not equal ( for error situation in REMOTE_ACCESS)
      return(0);

  if(e1->symbol() != e2->symbol()) //different array references 
    return(0);
  
  if(!e1->lhs()) // whole array in REMOTE_ACCESS
    return(1);

  for(el1=e1->lhs(),el2=e2->lhs(); el1&&el2; el1=el1->rhs(),el2=el2->rhs()) //compares subscript lists
     if(el1->lhs()->variant() == DDOT) // is ':' element
        ;
     else
       if(!ExpCompare(el1->lhs(),el2->lhs())) // corresponding subscript expressions are not identical
            return(0);
  if(el1 || el2) //one list is shorter than other
       return(0);
  else
       return(1); 
}   

SgExpression * isRemAccessRef(SgExpression *e)
  //returns remote-variable  with which array reference 'e' consides  or NULL        
{SgExpression *el;
 rem_acc *r;
 if(HPF_program && !inparloop){
   //rem_var *rv = (rem_var *) e->attributeValue(0,REMOTE_VARIABLE) ;
    if( e->attributeValue(0,REMOTE_VARIABLE))
      return(e);
    else
      return(NULL);
 }
//looking through the remote-access directive/clause list
 for(r=rma; r; r=r->next)
//looking through the remote-variable list
   for(el=r->rml; el; el=el->rhs()) 
      if(el->lhs()->attributeValue(0,REMOTE_VARIABLE) && RemAccessRefCompare(el->lhs(), e))
        return(el->lhs());
 return(NULL);
}

void ChangeRemAccRef(SgExpression *e, SgExpression *rve)
//changes remote-access reference by special buffer reference (multiplicated array i.e.DISTRIBUTE(*,*,...,*))
// remote-variable attribute saves information about this buffer array
{rem_var *rv = (rem_var *) rve->attributeValue(0,REMOTE_VARIABLE) ;
 SgExpression *p = NULL;
 SgExpression *el1, *el2,**dov;
 SgSymbol *ar;

ar = e->symbol();
if(rv->ncolon) { //there are ':'elements in index list of remote variable
  //looking through the subscript and index lists 
  for(el1=rve->lhs(),el2=e->lhs(); el1 && el2; el1=el1->rhs(),el2=el2->rhs())
     if(el1->lhs()->variant() == DDOT) // ':'
          p=el2;
     else if((dov=IS_DO_VARIABLE_USE(el1->lhs()))){ //do-variable-use
          el2->setLhs(*dov);
          p=el2;
     }
     else   
        //delete corresponding subscript in remote_access reference
        if(!p)
          e->setLhs(el2->rhs());
        else
          p->setRhs(el2->rhs());

  if(for_kernel || for_host)
  {
     if(rv->buffer) 
        e->setSymbol(rv->buffer);                                    /*ACC*/
  }
  else 
     e->setSymbol(baseMemory(ar->type()->baseType()));
  if(for_host)                                                    /*ACC*/
     return;    // is not linearized
  
  if(IN_COMPUTE_REGION || inparloop && parloop_by_handler)
  {
      if(rv->buffer)
         (e->lhs())->setLhs(*LinearFormB_for_ComputeRegion (rv->buffer, rv->ncolon, e->lhs()));  /*ACC*/  
  }
  else 
      (e->lhs())->setLhs(*LinearFormB(((rv->amv == 1) ? ar : (SgSymbol *) NULL), rv->index, rv->ncolon, e->lhs()));
  (e->lhs())->setRhs(NULL);
}
else {
  if(rv->amv == -1) 
  {
    int tInt = TypeIndex(e->symbol()->type()->baseType());
    if(tInt != -1)
       e->setSymbol(rmbuf[tInt]);
    e->setLhs(new SgExprListExp(*new SgValueExp(rv->index)));
  }
  else {
    if(for_kernel || for_host) 
    {
      if(rv->buffer)  
         e->setSymbol(rv->buffer);                                   /*ACC*/
    }
    else 
      e->setSymbol(baseMemory(ar->type()->baseType()));
    if(for_host)
    {                                                             /*ACC*/
      e->setLhs (*new SgExprListExp(*new SgValueExp(0)));       
      return;
    }
    if(IN_COMPUTE_REGION || inparloop && parloop_by_handler)
    { 
       if(rv->buffer)
          (e->lhs())->setLhs(*LinearFormB_for_ComputeRegion (rv->buffer, rv->ncolon, NULL));  /*ACC*/
    }
    else 
       (e->lhs())->setLhs(*LinearFormB(((rv->amv == 1) ? ar : (SgSymbol *) NULL), rv->index, rv->ncolon, NULL));    
    (e->lhs())->setRhs(NULL);
  }
}
return;
}

int CreateBufferArray (int rank, SgExpression *rme, int *amview, SgStatement *stmt)
{int ihead,isize,i,j,iamv,ileft,idis;
 SgExpression *es,*esz[MAX_DIMS], *elb[MAX_DIMS];
 ihead = ndvm; // allocating array header for buffer array
 ndvm+=2*rank+2;
 iamv = *amview =  ndvm++; 
 for(es=rme->lhs(),i=0,j=0; es; es=es->rhs(),i++) //looking through the index list 
     if(es->lhs()->variant() == DDOT) {
        //determination of dimension size
        esz[j] = ArrayDimSize(rme->symbol(),i+1);
        if(esz[j] && esz[j]->variant()==STAR_RANGE)
          Error("Assumed-size array: %s",rme->symbol()->identifier(),162,stmt);
        if(!esz[j]) //esz[j] == NULL (error situation)
	  esz[j] = new SgValueExp(1);  //for continuing traslation
        else
          esz[j] = Calculate(esz[j]);
        elb[j] =  header_ref(rme->symbol(),Rank(rme->symbol())+i+3);
                        // Exprn(LowerBound(rme->symbol(),i));                                
        j++;
     }
 isize = ndvm; 
 for(j=rank; j; j--) //creating Size Array
   doAssignStmtAfter(esz[j-1]); 
 
 /*generating function call:CrtAMV(AMRef,Rank,SizeArray,StaticSign)*/        
 doAssignTo_After(DVM000(iamv),CreateAMView(DVM000(isize),rank,0)); //creating the representation of abstact machine
 
 idis = ndvm; 
 for(j=rank; j; j--) //creating DisRule Array for DISTRIBUTE(*,*,...,*)
   doAssignStmtAfter(new SgValueExp(0));
 /*generating function call:DisAM(AMViewRef,PSRef,ParamCount, AxisArray, DistrParamArray)*/        
 doAssignStmtAfter(DistributeAM(DVM000(iamv),CurrentPS(),rank,idis,idis));//distributing
 
 
  ileft = ndvm;
  for(j=rank; j; j--) //creating LeftShSizeArray == RightShSizeArray  = {0,..,0} 
    doAssignStmtAfter(new SgValueExp(0));

  for(j=0; j<rank; j++) //storing lower bounds
    doAssignTo_After(DVM000(ihead+rank+2+j),elb[j]); 

  /*generating call:CrtDA(ArrayHeader,Base,Rank,TypeSize,SizeArray,StaticSign,ReDistrSign,LeftShSizeArr,RightShSizeAr)*/  
   doAssignStmtAfter(CreateDistArray(rme->symbol(),DVM000(ihead),DVM000(isize),rank,ileft,ileft,0,0));
                                               //creating distributed array ("replicated")  
                

    ndvm = isize;
   for(j=1; j<=rank; j++) //creating AxisArray = {1,2,..,rank} 
       doAssignStmtAfter(new SgValueExp(j));

    ndvm = idis;
    for(j=rank; j; j--) //creating CoeffArray  = {1,1,...,1}
       doAssignStmtAfter(new SgValueExp(1)); 
   
     //ConstArray = {0,0,...,0}  

   /*generating call:AlnDa(ArrayHeader,AMViewRef,AxisArray,CoefArray,ConstArray)*/ 
   doAssignStmtAfter(AlignArray(DVM000(ihead),DVM000(iamv),isize,idis,ileft));//aligning
   

   //doAssignTo_After(DVM000(ihead+rank+1),BufferHeaderNplus1(rme,rank,ihead));
                                                   // calculating HEADER(rank+1) 
 SET_DVM(isize);
 return(ihead);
}

void CopyToBuffer(int rank,  int ibuf, SgExpression *rme)
{  int itype,iindex,i,j,from_init,to_init;
  SgExpression *es,*ei[MAX_DIMS],*el[MAX_DIMS],*head;
  SgValueExp MM1(-1); 

  if(!rank) { // copying one element of distributed array to buffer 
  itype = TypeIndex(rme->symbol()->type()->baseType());
  if(itype == -1) 
    itype = 0;
  SgExpression *are = new SgArrayRefExp(*rmbuf[itype],*new SgValueExp(ibuf));//buffer reference

  for(es=rme->lhs(),i=0; es; es=es->rhs(),i++){ //looking through the index list    
    ei[i] =  &( es->lhs()->copy() - *Exprn( LowerBound(rme->symbol(),i)));
  }
  iindex = ndvm;
  for(j=i; j; j--)
      doAssignStmtAfter(ei[j-1]); 
  
  if((head=HeaderRef(rme->symbol())) != NULL) // NULL if array is not distributed (error)
    doAssignStmtAfter(ReadWriteElement(head,are,iindex));

  if(dvm_debug)
    InsertNewStatementAfter(D_RmBuf(head,GetAddresMem(are),0,iindex),cur_st,cur_st->controlParent());
  
  SET_DVM(iindex);
  return;
  }
  //copying section of distributed array to buffer array
 
 for(es=rme->lhs(),i=0; es; es=es->rhs(),i++)  {//looking through the index list    
    if(es->lhs()->variant() != DDOT)    
      ei[i] =  &( es->lhs()->copy() - * Exprn(LowerBound(rme->symbol(),i))); //init index   
    else
       ei[i] =& MM1.copy(); // -1
    el[i] = & ei[i]->copy(); //last index
 }
 from_init = ndvm;
 for(j=i; j; j--)
      doAssignStmtAfter(ei[j-1]); 
 for(j=i; j; j--)
      doAssignStmtAfter(el[j-1]); 
 to_init = ndvm;
 for(j=rank; j; j-- ) 
      doAssignStmtAfter(& MM1.copy()); 

 if((head=HeaderRef(rme->symbol())) != NULL) // NULL if array is not distributed (error)
    doAssignStmtAfter(ArrayCopy(head, from_init, from_init+i, from_init, DVM000(ibuf), to_init, to_init, to_init, 0));
 if(dvm_debug)
    InsertNewStatementAfter(D_RmBuf(head,GetAddresMem(DVM000(ibuf)),i,from_init),cur_st,cur_st->controlParent());

 SET_DVM(from_init);
 return;
}

void RemoteAccessDirective(SgStatement *stmt)
{SgStatement *rmout;
 	    if(inparloop) {
              err("The directive is inside the range of PARALLEL loop", 98,stmt); 
              return;
            }
            ReplaceContext(stmt->lexNext());
            switch(stmt->lexNext()->variant()) {
	            case LOGIF_NODE:
                        rmout = stmt->lexNext()->lexNext()->lexNext(); 
                        break;
	            case SWITCH_NODE:
                        rmout = stmt->lexNext()->lastNodeOfStmt()->lexNext();
                        break;
	            case IF_NODE:
                        rmout = lastStmtOfIf(stmt->lexNext())->lexNext();
                        break;
	            case CASE_NODE:
                    case ELSEIF_NODE:          
                        err("Misplaced REMOTE_ACCESS directive", 99,stmt);
                        rmout = stmt->lexNext()->lexNext();
                        break;
                    case FOR_NODE:
                    case WHILE_NODE:
                        rmout = lastStmtOfDo(stmt->lexNext())->lexNext();
                        break;
		    case DVM_PARALLEL_ON_DIR:
                        rmout = lastStmtOfDo(stmt->lexNext()->lexNext())->lexNext();
                        break;
	            default:
                        rmout = stmt->lexNext()->lexNext();
                        break;
              }
            // adding new element to remote_access directive/clause list
            AddRemoteAccess(stmt->expr(0),rmout); 

            LINE_NUMBER_AFTER(stmt,stmt); //for tracing
 
	    // looking through the remote variable list
	                   
            RemoteVariableList(stmt->symbol(),stmt->expr(0),stmt);            
}

void RemoteVariableList1(SgSymbol *group,SgExpression *rml, SgStatement *stmt)
{ SgStatement *if_st,*end_st = NULL;
  SgExpression *el, *es;
  int nc; //counter of ':' elements of remote-index-list
  int n;  //counter of  elements of remote-index-list
  int rank;  //rank of remote variable
  int ibuf = 0; 
  int iamv =-1;
  if(group){
     if_st =  doIfThenConstrForRemAcc(group,cur_st);
     end_st = cur_st; //END IF
     cur_st = if_st;
  }
  for(el=rml; el; el= el->rhs()) {  
       if(!HEADER(el->lhs()->symbol()))  //if non-distributed array occurs
               Error("'%s' is not distributed array",el->lhs()->symbol()->identifier(),72,stmt);
	n = 0;           
        nc = 0;
        // looking through the index list of remote variable
        for(es=el->lhs()->lhs(); es; es= es->rhs(),n++)  
            if(es->lhs()->variant() == DDOT)
               nc++;
        if((rank=Rank(el->lhs()->symbol())) && rank != n)
             Error("Length of remote-index-list is not equal to the rank of remote variable",el->lhs()->symbol()->identifier(),165,stmt);
        else
	  if (nc) {
                 ibuf = CreateBufferArray(nc,el->lhs(),&iamv, stmt);//creating replicated array
                 //copying to Buffer Array
                 CopyToBuffer(nc, ibuf, el->lhs());
	          }
          else    {
                 ibuf = ++rma->rmbuf_use[TypeIndex(el->lhs()->symbol()->type()->baseType())];
                 //copying to buffer
                 CopyToBuffer(nc, ibuf, el->lhs());
                  }  
        //adding attribute REMOTE_VARIABLE 
        rem_var *remv = new rem_var;
        remv->ncolon = nc;

        remv->index = ibuf;
        remv->amv   = iamv;
	(el->lhs())->addAttribute(REMOTE_VARIABLE,(void *) remv, sizeof(rem_var));  
  }
 if(group)
   //  cur_st = if_st->lastNodeOfStmt();
       cur_st = end_st;
}

void RemoteVariableList(SgSymbol *group, SgExpression *rml, SgStatement *stmt)
{ SgStatement *if_st,*end_st = NULL; 
  SgExpression *el, *es,*coef[MAX_DIMS],*cons[MAX_DIMS],*axis[MAX_DIMS], *do_var;  
  SgExpression  *ind_deb[MAX_DIMS];
  int nc; //counter of ':' or do-var-use elements of remote-index-list
  int n;  //counter of  elements of remote-index-list
  int rank;  //rank of remote variable
  int num,use[MAX_DIMS];   
  int i,j,st_sign,iaxis,ideb=-1;
  SgSymbol *dim_ident[MAX_DIMS],*ar;
  int ibuf = 0; 
  int iamv =0;
  int err_subscript = 0;
  SgValueExp c0(0),cm1(-1),c1(1);
  st_sign = 0;

  if(options.isOn(NO_REMOTE))
     return;    
  if(IN_COMPUTE_REGION && group)
     err("Asynchronous REMOTE_ACCESS clause in compute region",574,stmt);

  if(group){
     if_st =  doIfThenConstrForRemAcc(group,cur_st);
     end_st = cur_st; //END IF
     cur_st = if_st;
     st_sign = 1;
  }
  if(stmt->variant() == DVM_PARALLEL_ON_DIR)
    for(el=stmt->expr(2),i=0; el; el= el->rhs(),i++){ //do-variable list
      //use[i] = 0;
      dim_ident[i] = el->lhs()->symbol();
    }
  else
     i = 0;
      
  for(el=rml; el; el= el->rhs()) {  
        if(!HEADER(el->lhs()->symbol())) { //if non-distributed array occurs
            Error("'%s' isn't distributed array",el->lhs()->symbol()->identifier(),72,stmt);
            doAssignStmtAfter(&c0); 
            continue;
        }
	n = 0;           
        nc = 0;
        err_subscript = 0;
        for(j=0; j<i;j++)
          use[j] = 0; 
        if(!TestMaxDims(el->lhs()->lhs(),el->lhs()->symbol(),stmt)) continue;  
        // looking through the index list of remote variable
        for(es=el->lhs()->lhs(); es; es= es->rhs(),n++)  
	  if(es->lhs()->variant() == DDOT){
             axis[n]    = &cm1.copy();
             coef[n]    = &c0.copy(); 
             cons[n]    = &c0.copy(); 
             ind_deb[n] = &cm1.copy();
             //init[n]  = &c0.copy(); 
             //last[n]  = &c0.copy();
             //step[n]  = &c0.copy();
             //dim[nc]    = es->lhs();        /*ACC*/
             //dim_num[nc]= n;                /*ACC*/
             nc++;
          }
          else if ((stmt->variant() == DVM_PARALLEL_ON_DIR) && (do_var=isDoVarUse(es->lhs(),use,dim_ident,i,&num,stmt))) {
             CoeffConst(es->lhs(), do_var, &coef[n], &cons[n]);
             axis[n] = new SgValueExp(num); 
             TestReverse(coef[n],stmt);
             //dim[nc]    = es->lhs();        /*ACC*/
             //dim_num[nc]= n;                /*ACC*/
             nc++; 
             if(!coef[n]) {
                 err("Wrong regular subscript expression", 164,stmt);
                 err_subscript++; 
                 coef[n]  = &c0.copy();
                 cons[n]  = &c0.copy(); 
                 ind_deb[n] = &c0.copy();
                 //init[n]  = &c0.copy(); 
                 //last[n]  = &c0.copy();
                 //step[n]  = &c0.copy();
             } else {
            // correcting const with lower bound of corresponding array dimension
             cons[n]  = &(*cons[n] - *Exprn( LowerBound(el->lhs()->symbol(),n))); 
             ind_deb[n] = &cm1.copy();
                //init[n]  = &(init_do[num-1]->copy());
                //last[n]  = &(last_do[num-1]->copy());   
                //step[n]  = &(step_do[num-1]->copy());
             //adding attribute DO_VARIABLE_USE to regular subscript expression 
             SgExpression **dov = new (SgExpression *);
             *dov = do_var;
	     (es->lhs())->addAttribute(DO_VARIABLE_USE,(void *) dov, sizeof(SgExpression *));  
             }
    
          } else {
             axis[n]  = &c0.copy();
             coef[n]  = &c0.copy();
             cons[n]  = &(es->lhs()->copy() - *Exprn( LowerBound(el->lhs()->symbol(),n))) ;
             ind_deb[n] = &(cons[n]->copy());
             //init[n]  = &c0.copy(); 
             //last[n]  = &c0.copy();
             //step[n]  = &c0.copy(); 
          }
        rank=Rank(el->lhs()->symbol());
        if(n && rank && rank != n) {
             Error("Length of remote-subscript-list is not equal to the rank of remote variable",el->lhs()->symbol()->identifier(),165,stmt);
             continue;
        }
        if(err_subscript) continue;   //there is illegal subscript
        if(!n) {//remote-subscript-list is absent (whole array is remote data)
          for (; n<=rank-1; n++) {
             axis[n]  = &cm1.copy();
             coef[n]  = &c0.copy(); 
             cons[n]  = &c0.copy();
             ind_deb[n] = &cm1.copy(); 
	      //init[n]  = &c0.copy(); 
	      //last[n]  = &c0.copy();
              //step[n]  = &c0.copy();
             //dim[n]    = new SgExpression(DDOT); /*ACC*/
             //dim_num[n]= n;                      /*ACC*/ 
          } 
          nc = rank;
        }
          // allocating array header for buffer array
	  if(group){
             int nbuf;
             nbuf = BUFFER_INDEX(el->lhs()->symbol());
             if(nbuf == maxbuf)
               err("Buffer limit exceeded",183,stmt);
             ibuf = 2*(nbuf+1)*(rank+1) + 2;
             BUFFER_COUNT_PLUS_1(el->lhs()->symbol())
	     // buffer_head = HeaderRefInd(el->lhs()->symbol(),ibuf);
             ar = el->lhs()->symbol(); 
          } else {
             ibuf = ndvm; 
             if(nc)
               ndvm+=2*nc+2;
             else
               ndvm+=4;
             //buffer_head = DVM000(ibuf);
             ar = NULL;
          }
	  // creating buffer for remote elements of array 
          iaxis = ndvm;
          if (stmt->variant() == DVM_PARALLEL_ON_DIR) {
            for(j=n-1; j>=0; j--)
              doAssignStmtAfter(axis[j]);       
            for(j=n-1; j>=0; j--)
              doAssignStmtAfter(ReplaceFuncCall(coef[j]));
            for(j=n-1; j>=0; j--)
              doAssignStmtAfter(Calculate(cons[j]));
	        /*
                     for(j=n-1; j>=0; j--)
                        doAssignStmtAfter(ReplaceFuncCall(init[j]));
                     for(j=n-1; j>=0; j--)
                        doAssignStmtAfter(ReplaceFuncCall(last[j]));
                     for(j=n-1; j>=0; j--)
                        doAssignStmtAfter(ReplaceFuncCall(step[j]));
	         */
            doCallAfter(CreateRemBuf( HeaderRef(el->lhs()->symbol()), header_rf(ar,ibuf,1), st_sign,iplp,iaxis,iaxis+n,iaxis+2*n));
          } else {
            ideb = ndvm;
            for(j=n-1; j>=0; j--)
              doAssignStmtAfter(Calculate(ind_deb[j]));
            doCallAfter(CreateRemBufP( HeaderRef(el->lhs()->symbol()), header_rf(ar,ibuf,1), st_sign,ConstRef(0),ideb));
          }
                           //if(nc)
                           //  doAssignTo_After(header_rf(ar,ibuf,nc+2),BufferHeaderNplus1(el->lhs(),nc,ibuf,ar));
                                                   // calculating HEADER(nc+1) 
                           //if(IN_COMPUTE_REGION)    /*ACC*/
                           //   ACC_StoreLowerBoundsOfDvmBuffer(el->lhs()->symbol(), dim, dim_num, nc, ibuf, stmt);

          if(ACC_program)    /*ACC*/                     
            ACC_Before_Loadrb(header_rf(ar,ibuf,1));

          // loading the buffer
          doCallAfter(LoadRemBuf( header_rf(ar,ibuf,1)));        
          // waiting completion of loading the buffer
          doCallAfter(WaitRemBuf( header_rf(ar,ibuf,1)));

          if(IN_COMPUTE_REGION)  /*ACC*/
            ACC_Region_After_Waitrb(header_rf(ar,ibuf,1));
          if(group)
          //inserting buffer in group
            doAssignStmtAfter(InsertRemBuf(GROUP_REF(group,1), header_rf(ar,ibuf,1)));
          if(dvm_debug) {
            if (stmt->variant() == DVM_PARALLEL_ON_DIR) {
              ideb = ndvm;
              for(j=n-1; j>=0; j--)
                doAssignStmtAfter(ReplaceFuncCall(ind_deb[j]));
            }
            InsertNewStatementAfter(D_RmBuf( HeaderRef(el->lhs()->symbol()),GetAddresDVM( header_rf(ar,ibuf,1)),n,ideb),cur_st,cur_st->controlParent());
          }
	  SET_DVM(iaxis);
          //adding attribute REMOTE_VARIABLE 
          rem_var *remv = new rem_var;
          remv->ncolon = nc;
          remv->index = ibuf;
          remv->amv   = group ? 1 : iamv;
          remv->buffer = NULL;               /*ACC*/

	  (el->lhs())->addAttribute(REMOTE_VARIABLE,(void *) remv, sizeof(rem_var));
	  
  }
  if(group) {
       cur_st = cur_st->lexNext()->lexNext();//IF THEN after ELSE
       doAssignStmtAfter(WaitBG(GROUP_REF(group,1)));
       FREE_DVM(1);
	 //cur_st = if_st->lastNodeOfStmt();
       cur_st = end_st;
  }
}

void IndirectList(SgSymbol *group, SgExpression *rml, SgStatement *stmt)
{ SgStatement *if_st,*end_st = NULL; 
  SgExpression *el, *es,*cons[MAX_DIMS];
  SgSymbol *mehead;
  int nc; //counter of indirect access dimensions
  int n;  //counter of  elements of indirect-subscript-list
  int rank;  //rank of remote variable
  int j,st_sign,icons;
  SgSymbol *dim_ident;
  int ibuf = 0; 
  int iamv =0;
  SgValueExp c0(0),cm1(-1),c1(1);
  st_sign = 0;
  if(group){
     if_st =  doIfThenConstrForRemAcc(group,cur_st);
     end_st = cur_st; //END IF
     cur_st = if_st;
     st_sign = 1;
  }
  dim_ident = stmt->expr(2)->lhs()->symbol();  //do-variable 
  for(el=rml; el; el= el->rhs()) {  
        if(!HEADER(el->lhs()->symbol()))  //if non-distributed array occurs
             Error("'%s' isn't distributed array",el->lhs()->symbol()->identifier(),72,stmt);
	n = 0;           
        nc = 0;
        // looking through the index list of remote variable
        for(es=el->lhs()->lhs(); es; es= es->rhs(),n++)  
          if ((mehead = isIndirectSubscript(es->lhs(),dim_ident,stmt))) {
             nc++; 
             cons[n]  =   & SgUMinusOp(*Exprn( LowerBound(el->lhs()->symbol(),n))); 
             //adding attribute INDIRECT_SUBSCRIPT to irregular subscript expression 
             SgSymbol **me = new (SgSymbol *);
             *me = mehead;
	     (es->lhs())->addAttribute(INDIRECT_SUBSCRIPT,(void *) me, sizeof(SgSymbol *));      
          } else 
             cons[n]  = &(es->lhs()->copy() - *Exprn( LowerBound(el->lhs()->symbol(),n))) ;
          
        if((rank=Rank(el->lhs()->symbol())) && rank != n) {
             Error("Length of indirect-subscript-list is not equal to the rank of remote variable",el->lhs()->symbol()->identifier(),302,stmt);
             continue;
        }
          
          // allocating array header for buffer array
          ibuf = ndvm; 
          ndvm+=+4;
          if(!mehead || (nc > 1)){
            // err("Illegal indirect reference",stmt);
             return;
          }
	  // creating buffer for indirect access elements of array  
          icons = ndvm;       
          for(j=n-1; j>=0; j--)
             doAssignStmtAfter(Calculate(cons[j]));
          doAssignStmtAfter(CreateIndBuf( HeaderRef(el->lhs()->symbol()), DVM000(ibuf), st_sign,HeaderRef(mehead),icons));
          doAssignTo_After(DVM000(ibuf+3),BufferHeader4(el->lhs(),ibuf));
                                                   // calculating HEADER(nc+1) 
          // loading the buffer
          doAssignStmtAfter(LoadIndBuf(DVM000(ibuf)));
          if(group)
          //inserting buffer in group
            doAssignStmtAfter(InsertIndBuf(group,DVM000(ibuf)));
          // waiting completion of loading the buffer
          doAssignStmtAfter(WaitIndBuf(DVM000(ibuf)));
          if(dvm_debug)
            InsertNewStatementAfter(D_RmBuf( HeaderRef(el->lhs()->symbol()),GetAddresMem(DVM000(ibuf)),n,icons),cur_st,cur_st->controlParent());
	  SET_DVM(icons);
        //adding attribute REMOTE_VARIABLE 
        rem_var *remv = new rem_var;
        remv->ncolon = nc;

        remv->index = ibuf;
        remv->amv   = iamv;
	(el->lhs())->addAttribute(REMOTE_VARIABLE,(void *) remv, sizeof(rem_var));
	  
  }
 if(group) {
       cur_st = cur_st->lexNext()->lexNext();//IF THEN after ELSE
       doAssignStmtAfter(WaitIG(group));
       FREE_DVM(1);
	 //cur_st = if_st->lastNodeOfStmt();
       cur_st = end_st;
 }
}



void DeleteBuffers(SgExpression *rml)
{ SgExpression *el;
  rem_var *remv;
  SgStatement *current = cur_st;//store value of cur_st
  SgLabel *lab;
  //cur_st = cur_st->lexPrev();
  for(el=rml; el; el= el->rhs()) {  //looking through the remote variable list
     remv = (rem_var *) (el->lhs())->attributeValue(0,REMOTE_VARIABLE); 
   /*   if(remv->ncolon) {
      doAssignStmtBefore(DeleteObject(DVM000(remv->index)),current);//delete distributed array
      doAssignStmtBefore(DeleteObject(DVM000(remv->amv)),current);//delete abstract machine view  
      FREE_DVM(2); 
      }
   */
     if(remv && remv->amv == 0){ //buffer is not included in named group
       current->insertStmtBefore(*DeleteObject_H(header_rf((SgSymbol *) NULL,remv->index,1)),*current->controlParent());
     }
  }
  cur_st = current; //restore cur_st
}

void RemoteAccessEnd()
{int i;
 for (i=0; i<Ntp; i++) // calculating number of used scalar buffers of different type
    rmbuf_size[i] =(rmbuf_size[i] < rma->rmbuf_use[i]) ? rma->rmbuf_use[i] : rmbuf_size[i];                                                                         //maximum         
 if(rma->rmout)  // REMOTE_ACCESS directive (not clause)   
   DeleteBuffers(rma->rml); //deleting array buffers
 DelRemoteAccess(); //deletes element from remote_access directive/clause list
                    //and concurently frees scalar buffers
      
}   

void AddRemoteAccess(SgExpression *rml, SgStatement *rmout)
{int i;
 rem_acc *elem = new rem_acc;
 elem->rml = rml;
 elem->rmout = rmout;
 if(!rma) {// first element
   elem->next = NULL;
   for(i=0; i<Ntp; i++)
      elem->rmbuf_use[i] = 0;
 }
 else {
   elem->next = rma;
   for(i=0; i<Ntp; i++)
      elem->rmbuf_use[i] = rma->rmbuf_use[i];
 }
 rma = elem;
}

void DelRemoteAccess()
{
 if(rma)
   rma = rma->next;
}

SgExpression *isSpecialFormExp(SgExpression *e,int i,int ind,SgExpression *vpart[],SgSymbol *do_var[])
{
  if(e->variant()==ADD_OP){
   if(isInvariantPart(e->lhs()) && isDependentPart(e->rhs(),do_var)) {
     vpart[i] = RenewSpecExp(e->rhs(),e->lhs()->valueInteger(),ind);
     return(e->lhs());
   }
  if(isInvariantPart(e->rhs()) && isDependentPart(e->lhs(),do_var)) {
     vpart[i] = RenewSpecExp(e->lhs(),e->rhs()->valueInteger(),ind);
     return(e->rhs());
   }
  }
  if(isDependentPart(e,do_var)){
    vpart[i] = RenewSpecExp(e,0,ind);
    return(new SgValueExp(0));
  }
 return(NULL); 
}

int isInvariantPart(SgExpression *e)
  { return(e->isInteger());}

int isDependentPart(SgExpression *e,SgSymbol *do_var[])
{//!!! temporaly
 if(do_var[0])
   ;
 if(isSgFunctionCallExp(e)){
   if(!strcmp(e->symbol()->identifier(),"mod") && (e->lhs()->lhs()->variant()==ADD_OP))
      return(1);
 }
 return(0);
}

SgExpression *RenewSpecExp(SgExpression *e, int cnst, int ind)
{ if(cnst % 2)
 ( e->lhs())->setLhs(*DVM000(ind) + (*new SgValueExp(cnst % 2)) + (*e->lhs()->lhs()));
  else
 ( e->lhs())->setLhs(*DVM000(ind) + (*e->lhs()->lhs()));
 return(e);
}

int isDistObject(SgExpression *e)
{
  if(!e)
    return(0);
  if(isSgArrayRefExp(e)) 
     if(HEADER(e->symbol()))
       return(1);
  if(e->variant() == ARRAY_OP)
     return(isDistObject(e->lhs()));
  return(0);
} 

int isListOfArrays(SgExpression *e, SgStatement *st)
{SgExpression *el;
 int test = 0;
 for(el=e; el; el = el->rhs()) {
   if(!(el->lhs()->symbol()->attributes() & DIMENSION_BIT) && !IS_POINTER(el->lhs()->symbol())) {
      Error("'%s' is not array",el->lhs()->symbol()->identifier(), 66,st);   
      test = 1;
   }

   if( el->lhs()->lhs() && !((el->lhs()->symbol()->attributes() & TEMPLATE_BIT) || (el->lhs()->symbol()->attributes() & PROCESSORS_BIT)))
      Error("Shape specification is not permitted: %s", el->lhs()->symbol()->identifier(), 263, st);
 }
 return(test);  
}

char * AttrName(int i)
{ switch (i) {
        case 0: return("ALIGN");
        case 1: return("DISTRIBUTE");
        case 2: return("TEMPLATE");
        case 3: return("PROCESSORS");
        case 4: return("DIMENSION");
        case 5: return("DYNAMIC");
        case 6: return("SHADOW");
        case 7: return("COMMON");
        default: return("NONE");
  }
}

int TestShapeSpec(SgExpression *e)
{//temporary
 return(isSgValueExp(e)? 1 : 1);
}

void AddToGroupNameList (SgSymbol *s)
{group_name_list *gs;
//adding the  symbol 's' to group_name_list
  if(!grname) {
     grname = new group_name_list;
     grname->symb = s;
     grname->next = NULL;
  } else {
     for(gs=grname; gs; gs=gs->next)
        if(gs->symb == s)
          return;
     gs = new group_name_list;
     gs->symb = s;
     gs->next = grname;
     grname = gs;
  }
}

symb_list  *AddToSymbList ( symb_list *ls, SgSymbol *s)
{symb_list *l;
//adding the symbol 's' to symb_list 'ls'
  if(!ls) {
     ls = new symb_list;
     ls->symb = s;
     ls->next = NULL;
  } else {
       /*
          for(l=ls; l; l=l->next)
            if(l->symb == s)
              return;
       */
     l = new symb_list;
     l->symb = s;
     l->next = ls;
     ls = l;
  }
  return(ls);
}

symb_list  *AddNewToSymbList ( symb_list *ls, SgSymbol *s)
{symb_list *l;
//adding the symbol 's' to symb_list 'ls'
  if(!ls) {
     ls = new symb_list;
     ls->symb = s;
     ls->next = NULL;
  } else {
     for(l=ls; l; l=l->next)
        if(l->symb == s)
           return(ls);       
     l = new symb_list;
     l->symb = s;
     l->next = ls;
     ls = l;
  }
  return(ls);
}

symb_list  *AddNewToSymbListEnd ( symb_list *ls, SgSymbol *s)
{symb_list *l, *lprev;
//adding the symbol 's' to symb_list 'ls'
  if(!ls) {
     ls = new symb_list;
     ls->symb = s;
     ls->next = NULL;
  } else {
     for(l=ls; l; lprev=l, l=l->next)
        if(l->symb == s)
           return(ls);        
     l = new symb_list;
     l->symb = s;
     l->next = NULL;
     lprev->next = l;
  }
  return(ls);
}

symb_list  *MergeSymbList(symb_list *ls1, symb_list *ls2)
{
  symb_list *l =ls1;
  if(!ls1)
     return (ls2);
  while(l->next)
     l = l->next;
  l->next = ls2;
  return ls1;
}

symb_list *CopySymbList(symb_list *ls)
{
  symb_list *l=NULL, *el, *cp=NULL;
  while(ls)
  {
    el = new symb_list;
    el->symb = ls->symb;
    el->next = NULL;
    if(l)
      l->next  = el;
    else
      cp = el;
    l = el;
    ls = ls->next;
  }
  return cp;
}

void DeleteSymbList(symb_list *ls)
{symb_list *l;

  while(ls)
  { l = ls;
    ls =ls->next;
    delete l;
  }
}

filename_list  *AddToFileNameList ( char *s)
{filename_list *ls;
 SgType *tch;
 SgExpression *le;
 int length;
//adding the name 's' to filename_list 'ls'
  if(!fnlist) {
     ls = new filename_list;
     ls->name = s;
     ls->next = NULL;
     le = new SgExpression(LEN_OP);
     length = strlen(s)+1;
     le->setLhs(new SgValueExp(length));
     tch = new SgType(T_STRING,le,SgTypeChar());
     ls->fns  =  new SgVariableSymb(FileNameVar(++filename_num), *tch, *cur_func);
     fnlist   = ls; 
  } else {
     for(ls=fnlist; ls; ls=ls->next)
        if(ls->name == s)
           return(ls);       
     ls = new filename_list;
     ls->name = s;
     ls->next = fnlist;
     le = new SgExpression(LEN_OP);
     length = strlen(s)+1;
     le->setLhs(new SgValueExp(length));
     tch = new SgType(T_STRING,le,SgTypeChar());
     ls->fns  =  new SgVariableSymb(FileNameVar(++filename_num), *tch, *cur_func);
     fnlist   = ls; 
  }
  return(ls);
}

filename_list  *AddToFileNameList(const char *s_in)
{
    char *s = new char[strlen(s_in) + 1];
    strcpy(s, s_in);

    filename_list *ls;
    SgType *tch;
    SgExpression *le;
    int length;
    //adding the name 's' to filename_list 'ls'
    if (!fnlist) {
        ls = new filename_list;
        ls->name = s;
        ls->next = NULL;
        le = new SgExpression(LEN_OP);
        length = strlen(s) + 1;
        le->setLhs(new SgValueExp(length));
        tch = new SgType(T_STRING, le, SgTypeChar());
        ls->fns = new SgVariableSymb(FileNameVar(++filename_num), *tch, *cur_func);
        fnlist = ls;
    }
    else {
        for (ls = fnlist; ls; ls = ls->next)
        if (ls->name == s)
            return(ls);
        ls = new filename_list;
        ls->name = s;
        ls->next = fnlist;
        le = new SgExpression(LEN_OP);
        length = strlen(s) + 1;
        le->setLhs(new SgValueExp(length));
        tch = new SgType(T_STRING, le, SgTypeChar());
        ls->fns = new SgVariableSymb(FileNameVar(++filename_num), *tch, *cur_func);
        fnlist = ls;
    }
    return(ls);
}

void InsertDebugStat(SgStatement *func, SgStatement* &end_of_unit) 
{
   SgStatement *stmt,*last, *data_stf, *first,*first_dvm_exec,*last_spec,*last_dvm_entry, *lentry = NULL;
   SgStatement *mod_proc;
   SgStatement *copy_proc = NULL;
   SgStatement *has_contains = NULL;
   SgLabel *lab_exec;
   stmt_list *pstmt = NULL;
   int contains[2];
   int in_on=0;

  //initialization
  dsym = NULL;
  grname = NULL;
  saveall = 0;
  maxdvm = 0;
  maxhpf = 0;
  count_reg = 0;
  initMask();
  data_stf = NULL;
  inparloop = 0;
  inasynchr = 0;
  redvar_list = NULL;
  goto_list = NULL;
  proc_symb = NULL; 
  task_symb = NULL; 
  consistent_symb = NULL; 
  async_symb=NULL;
  check_sum = NULL;
  loc_templ_symb=NULL; 
  index_symb = NULL;
  in_task_region = 0;
  task_ind = 0;
  in_task = 0;
  task_lab = NULL;
  pref_st = NULL;
  pipeline = 0;
  registration = NULL;
  filename_num = 0;
  fnlist = NULL;
  nloopred = 0;
  nloopcons = 0;
  wait_list = NULL;
  SIZE_function = NULL;  
  dvm_const_ref = 0;
  in_interface = 0;
  mod_proc = NULL;
  if_goto = NULL;
  nifvar = 0;
  entry_list = NULL;
  dbif_cond = 0;
  dbif_not_cond = 0;
  last_dvm_entry = NULL;
  all_replicated = 0; 
  IOstat = NULL;

  TempVarDVM(func);
  initF90Names();

  first = func->lexNext();
 //get the last node of the program unit(function) 
  last = func->lastNodeOfStmt();
  end_of_unit = last;
  if(!(last->variant() == CONTROL_END))
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
//               TRUE   -  for executable statement of Fortan 90 
    {
	                  //!!!debug
                          //  printVariantName(stmt->variant()); 
                          //  printf("\n");
                          //  printf("%s  %d\n",stmt->lineNumber(),
      // analizing SAVE statement
      if(stmt->variant()==SAVE_DECL) { 
           if (!stmt->expr(0))  //SAVE without name-list
             saveall = 1;
           else if(IN_MAIN_PROGRAM)
            pstmt = addToStmtList(pstmt, stmt);   //for extracting and replacing by SAVE without list
           continue;
      }
      // deleting SAVE-attribute from Type Declaration Statement (for replacing by SAVE without list)
      if(IN_MAIN_PROGRAM && isSgVarDeclStmt(stmt))          
          DeleteSaveAttribute(stmt);

      if(debug_regim) {
        if(stmt->variant()==COMM_STAT) {
          SgExpression *ec, *el;
          SgSymbol *sc; 
	  for(ec=stmt->expr(0); ec; ec=ec->rhs()) // looking through COMM_LIST
	    for(el=ec->lhs(); el; el=el->rhs()) {  
              sc = el->lhs()->symbol();
              if(sc){
                 SYMB_ATTR(sc->thesymb)= SYMB_ATTR(sc->thesymb) | COMMON_BIT;
                 if(IS_ARRAY(sc))
                    registration = AddNewToSymbList( registration, sc); 
              }        
	    }  
          continue; 
	}   

        // registrating  arrays from variable list of declaration statement
        if( isSgVarDeclStmt(stmt) || isSgVarListDeclStmt(stmt)) {
           RegistrationList(stmt);
           continue;
        }
      }


      if(isSgVarDeclStmt(stmt)) VarDeclaration(stmt);// for analizing object list and changing variant of declaration statement by VAR_DECL_90
      if((stmt->variant() == DATA_DECL) || (stmt->variant() == STMTFN_STAT)) {
          if(stmt->variant()==STMTFN_STAT)
            DECL(stmt->expr(0)->symbol()) = 2;     //flag of statement function name
 
          if(!data_stf)
            data_stf = stmt; //first statement in data-or-function statement part 
	  continue;
      }

      if(stmt->variant() == INTERFACE_STMT || stmt->variant() == INTERFACE_ASSIGNMENT || stmt->variant() == INTERFACE_OPERATOR) {
	  stmt = InterfaceBlock(stmt); //stmt= stmt->lastNodeOfStmt();  
          continue;
      }

        if( stmt->variant() == USE_STMT) { 
          if(stmt->lexPrev() != func && stmt->lexPrev()->variant()!=USE_STMT) 
            err("Misplaced USE statement", 639, stmt);      
          continue;     
        }
	if(stmt->variant() == STRUCT_DECL){
          StructureProcessing(stmt);
          stmt=stmt->lastNodeOfStmt();
          continue;
        }
  
        continue;
      }
    if ((stmt->variant() == FORMAT_STAT))        
       {
         continue;
       }  
 

// processing the DVM Specification Directives

    switch(stmt->variant()) {
      case DVM_REDUCTION_GROUP_DIR:
	     //if (dvm_debug)
          if (debug_regim)
	   {SgExpression * sl; 
            for(sl=stmt->expr(0); sl; sl = sl->rhs())
               AddToGroupNameList(sl->lhs()->symbol()); 
           } 
           //including the DVM specification directive to list
           pstmt = addToStmtList(pstmt, stmt); 
           continue;

       case(DVM_INDIRECT_GROUP_DIR):
       case(DVM_REMOTE_GROUP_DIR):
           if (debug_regim && !options.isOn(NO_REMOTE))
           {SgExpression * sl; 
	    for(sl=stmt->expr(0); sl; sl = sl->rhs()){
               SgArrayType *artype;
               artype = new SgArrayType(*SgTypeInt());  
               artype->addRange(*new SgValueExp(3));
               sl->lhs()->symbol()->setType(artype);
               AddToGroupNameList(sl->lhs()->symbol()); 
           }
           }
           //including the DVM specification directive to list
           pstmt = addToStmtList(pstmt, stmt); 
            continue;       
       case(DVM_POINTER_DIR):
           if(debug_regim)
           {SgExpression *el;
            SgStatement **pst = new (SgStatement *);
            SgSymbol *sym;
            *pst = stmt;
            for(el = stmt->expr(0); el; el=el->rhs()){ //  name list
               sym = el->lhs()->symbol();  // name
               sym->addAttribute(POINTER_, (void *) pst,                                                                  sizeof(SgStatement *)); 
            }
	   }
           //including the DVM specification directive to list
           pstmt = addToStmtList(pstmt, stmt); 
           continue;
       case(ACC_ROUTINE_DIR):    
       case(HPF_PROCESSORS_STAT):
       case(HPF_TEMPLATE_STAT):
       case(DVM_DYNAMIC_DIR):
       case(DVM_SHADOW_DIR):
       case(DVM_ALIGN_DIR):
       case(DVM_DISTRIBUTE_DIR):
       case(DVM_VAR_DECL):
       case(DVM_TASK_DIR): 
       case(DVM_INHERIT_DIR): 
       case(DVM_HEAP_DIR):
       case(DVM_ASYNCID_DIR): 
       case(DVM_CONSISTENT_DIR):
       case(DVM_CONSISTENT_GROUP_DIR):
           //including the DVM specification directive to list
           pstmt = addToStmtList(pstmt, stmt); 
           continue;
    }     
// all declaration statements are processed,
// current statement is executable (F77/DVM)
    break;
  }   

  //TempVarDVM(func);

  for(;pstmt; pstmt= pstmt->next)
  Extract_Stmt(pstmt->st);// extracting  DVM Specification Directives

  first_exec = stmt; // first executable statement
 
  // testing procedure (-dbif2 regim)
  if(debug_regim && dbg_if_regim>1 && ((func->variant() == PROC_HEDR) || (func->variant() == FUNC_HEDR)) && !pstmt && !isInternalOrModuleProcedure(func) && !lookForDVMdirectivesInBlock(first_exec,func->lastNodeOfStmt(),contains) && !contains[0] && !contains[1])
     copy_proc = CreateCopyOfExecPartOfProcedure();  

  lab_exec = first_exec->label(); // store the label of first ececutable statement 
  BIF_LABEL(first_exec->thebif) = NULL;
  last_spec = stmt->lexPrev();
  where = first_exec;
  ndvm = 1; // ndvm is number of first free element of array "dvm000"
  nhpf = 1; // nhpf is number of first free element of array "hpf000"

//generating assign statement
// dvm000(1) = fname(file_name)
//function 'fname' tells the name of source file to DVM run-time system
  InsertNewStatementBefore(D_Fname(),first_exec);

 first_dvm_exec = last_spec->lexNext(); //first DVM function call
 if(IN_MODULE){
     if(debug_regim ) {
       mod_proc = CreateModuleProcedure(cur_func,first_exec,has_contains);
       where = mod_proc->lexNext();
       end_of_unit = where;
     } else {
       first_dvm_exec = last_spec->lexNext();
       goto EXEC_PART_;
     }
  }
 
  if(func->variant() == PROG_HEDR)  { // MAIN-program
//generating a call  statement
// call dvmlf(line_number_of_first_executable_statement,source-file-name)
      LINE_NUMBER_STL_BEFORE(cur_st,first_exec,first_exec);
//generating the function call which initializes the control structures of DVM run-time system,
//   it's inserted in MAIN program) 
// dvm000(1) = <flag>
// call dvmh_init(dvm000(1))
       RTL_GPU_Init();
       if(dbg_if_regim)
         InitDebugVar();       
  }
 
 ndvm = 4;
        // first_dvm_exec = last_spec->lexNext(); //first DVM function call
 nio = 0;
//generating call (module procedure) and/or assign statements for USE statements
 GenForUseStmts(func,where);

 if(debug_regim && grname) {
    if(!IN_MODULE)
      InitGroups();
    CreateRedGroupVars();
 }
 if(debug_regim && registration) {
    LINE_NUMBER_BEFORE(cur_func,where); //(first_exec,first_exec);
    ArrayRegistration(); // before array registration number of cur_func line
                         // must be put to debugger
 }
 if(lab_exec)
      first_exec-> setLabel(*lab_exec);  //restore label of first executable statement
  
 last_dvm_entry = first_exec->lexPrev();

 if(copy_proc)  
     InsertCopyOfExecPartOfProcedure(copy_proc);  

 EXEC_PART_:

  if(IN_MODULE) { 
    if(!mod_proc && first_exec->variant() == CONTAINS_STMT) 
         end_of_unit = has_contains = first_exec;
    goto END_;
  }

//follow the executable statements in lexical order until last statement
// of the function
  for(stmt=first_exec; stmt ; stmt=stmt->lexNext()) {
    cur_st = stmt;
    if(isACCdirective(stmt))
    { pstmt = addToStmtList(pstmt, stmt);
      continue;
    }
    switch(stmt->variant()) {
       case CONTROL_END:
            if(stmt == last) {
              if(func->variant() == PROG_HEDR)  // for MAIN program
                RTLExit(stmt);
              goto END_;            
            }
            break;
       case CONTAINS_STMT:
            if(func->variant() == PROG_HEDR)  // for MAIN program
                RTLExit(stmt);
            has_contains = end_of_unit = stmt;
            goto END_;            
            break;
       case RETURN_STAT:
            if(dvm_debug || perf_analysis ) 
              goto_list = addToStmtList(goto_list, stmt); 
         
            if(stmt->lexNext() == last) 
                goto END_;  
            break;
       case STOP_STAT:
            if(stmt->expr(0)){
               SgStatement *print_st;
               InsertNewStatementBefore(print_st=PrintStat(stmt->expr(0)),stmt);
               ReplaceByIfStmt(print_st);
            } 
            RTLExit(stmt);
            if(stmt->lexNext() == last)
               goto END_;
            break;
       /*
       case PAUSE_NODE: 
            err("PAUSE statement is not permitted in FDVM", 93,stmt); 
            break;
       case ENTRY_STAT: 
            if(debug)
              err("ENTRY statement is not permitted in FDVM", stmt); 
            break;
       */
       case EXIT_STMT:
            //if(dvm_debug || perf_analysis ) 
              // EXIT statement is added to list for debugging (exit the loop)       
             // goto_list = addToStmtList(goto_list, stmt);
            break;

       case ENTRY_STAT: 
             GoRoundEntry(stmt);
              //BeginBlockForEntry(stmt);
             entry_list=addToStmtList(entry_list,stmt);             
            break;  

       case SWITCH_NODE:           // SELECT CASE ... 
       case ARITHIF_NODE:          // Arithmetical IF
       case IF_NODE:               // IF... THEN
       case WHILE_NODE:            // DO WHILE (...)
	    /*case ELSEIF_NODE:           // ELSE IF...*/ 
	    if(dvm_debug)
              DebugExpression(stmt->expr(0),stmt);
            if((dvm_debug || perf_analysis) && stmt->variant()==ARITHIF_NODE ) 
              goto_list = addToStmtList(goto_list, stmt);            
            break;

       case LOGIF_NODE:            // Logical IF 
            if( !stmt->lineNumber()) {//inserted statement
               stmt = stmt->lexNext();
               break; 
            } 
            if(dvm_debug){
              if(HPF_program && inparloop)
                IsLIFReductionOp(stmt, indep_st->expr(0) ? indep_st->expr(0)->lhs() : indep_st->expr(0));                                           //look for reduction operator
              ReplaceContext(stmt);
              DebugExpression(stmt->expr(0),stmt);
	    }
            else if(perf_analysis && IsGoToStatement(stmt->lexNext()))
              ReplaceContext(stmt);

            continue; // to next statement
       case FORALL_STAT:          // FORALL statement
            stmt=stmt->lexNext();//  statement that is a part of FORALL statement         
            break;

       case GOTO_NODE:          // GO TO
            if((dvm_debug || perf_analysis) && stmt->lineNumber() ) 
              goto_list = addToStmtList(goto_list, stmt);          
            break;
       case COMGOTO_NODE:          // Computed GO TO
            if(dvm_debug){
              ReplaceContext(stmt);
              DebugExpression(stmt->expr(1),stmt);
             } else if(perf_analysis)
              ReplaceContext(stmt);
            if( dvm_debug || perf_analysis ) 
              goto_list = addToStmtList(goto_list, stmt);          
            break;

       case ASSIGN_STAT:             // Assign statement 
	   {SgSymbol *s;
            if(!stmt->lineNumber())  //inserted debug statement
              break;
            s=stmt->expr(0)->symbol();
            if(s && IS_POINTER(s)){ // left part variable is POINTER
             if(isSgFunctionCallExp(stmt->expr(1)) && !strcmp(stmt->expr(1)->symbol()->identifier(),"allocate")){
               if(inparloop)
                 err("Illegal statement in the range of parallel loop",94,stmt);
               if(debug_regim)
		   //alloc_st = addToStmtList(alloc_st, stmt);
                 AllocArrayRegistration(stmt);
              
             } else if( (isSgVarRefExp(stmt->expr(1)) || isSgArrayRefExp(stmt->expr(1))) && stmt->expr(1)->symbol() && IS_POINTER(stmt->expr(1)->symbol())) {
                ;
             } else 
               err("Only a value of ALLOCATE function or other POINTER may be assigned to a POINTER",95,stmt);

             break;
	   }

           if(s && !inparloop && IS_DVM_ARRAY(s) && DistrArrayAssign(stmt))
              break;
           if(s && !inparloop && AssignDistrArray(stmt))
              break;
    
            if(dvm_debug){ 
              SgStatement *stcur, *after_st = NULL, *stmt1; 
              if(HPF_program && inparloop)
                IsReductionOp(stmt,indep_st->expr(0) ? indep_st->expr(0)->lhs() : indep_st->expr(0));                                               //look for reduction operator
              ReplaceContext(stmt);
              DebugAssignStatement(stmt);

              if(own_exe) //"owner executes" rule
                InsertNewStatementAfter(D_Skpbl(),cur_st,cur_st->controlParent()); 
	      else if(!inparloop && !in_on && stmt->expr(0)->symbol() && IS_DVM_ARRAY(stmt->expr(0)->symbol()))
                InsertNewStatementAfter(D_Skpbl(),cur_st,cur_st->controlParent());  
              own_exe = 0;
              stmt = cur_st; 
            }
	 }

            break;

       case PROC_STAT:             // CALL
            if(!stmt->lineNumber())  //inserted debug statement
              break;
            if(dvm_debug){
              ReplaceContext(stmt);
              DebugExpression(NULL,stmt);
            }    
            break;

       case ALLOCATE_STMT:
            if(debug_regim) {
               AllocatableArrayRegistration(stmt);
               stmt=cur_st;
            }
            break;
 
       case DEALLOCATE_STMT:
            break; 
       case FOR_NODE:
	    if (perf_analysis == 4)
              SeqLoopBegin(stmt);
            if(dvm_debug) 
	      DebugLoop(stmt);          
            break;

        case DVM_PARALLEL_ON_DIR:
             if(debug_regim && !dvm_debug)
               Reduction_Debug(stmt);
             par_do = stmt->lexNext(); // first DO statement of parallel loop 
             while( isOmpDir (par_do))  //|| isACCdirective(par_do) 
             { cur_st = par_do;
               par_do=par_do->lexNext();               
             }  

             if(!isSgForStmt(par_do) && (dvm_debug || perf_analysis && perf_analysis != 2)) { 
                                        //directive is ignored
               err("PARALLEL directive must be followed by DO statement",97,stmt);                                                                     
               break; 
             }  

	     if(dvm_debug){ //debugging mode
               if(inparloop){
                 err("Nested PARALLEL directives are not permitted", 96,stmt);
                 break;
               }
                     
               inparloop = 1;
               if(!ParallelLoop_Debug(stmt)) // error in PARALLEL directive
                 inparloop = 0;                  
	     
               Extract_Stmt(stmt); // extracting DVM-directive           
               stmt = cur_st;
                  // setting stmt on last DO statement of parallel loop nest
	     }

	     else if(perf_analysis && perf_analysis != 2) {
               inparloop = 1;
               
               //generating call to 'bploop' function of performance analizer
	       // (begin of parallel interval)
               LINE_NUMBER_AFTER(stmt,stmt);
               InsertNewStatementAfter(St_Bploop(OpenInterval(stmt)), cur_st,stmt->controlParent());
             
               if(perf_analysis == 4)
                 SkipParLoopNest(stmt); 
               Extract_Stmt(stmt); // extracting DVM-directive           
               stmt = cur_st;
             } 
             else // dvm_debug == 0 && perf_analysis == 0 or 2, i.e. standard mode
               //including the DVM  directive to list
               pstmt = addToStmtList(pstmt, stmt);   
             break;

       case HPF_INDEPENDENT_DIR:
             if(dvm_debug){ //debugging mode
               if(inparloop){
                //illegal nested INDEPENDENT directive is ignored
                pstmt = addToStmtList(pstmt, stmt); //including the HPF directive to list
                break;
               }                     
               par_do = stmt->lexNext();// first DO statement of parallel loop 
               indep_st = stmt; 
               if(!isSgForStmt(par_do)) {
                  err("INDEPENDENT directive must be followed by DO statement",97,stmt);
                                                                  //directive is ignored 
                  break; 
               }  
               inparloop = 1;
               IEXLoopAnalyse(func);
               if(!IndependentLoop_Debug(stmt)) // error in INDEPENDENT directive
                   inparloop = 0;                  	               
	     }

	     else if(perf_analysis && perf_analysis != 2) {
               inparloop = 1;
               par_do = stmt->lexNext();// first DO statement of parallel loop
               indep_st = stmt; 
               //generating call to 'bploop' function of performance analizer
	       // (begin of parallel interval)
               LINE_NUMBER_AFTER(stmt,stmt);
               InsertNewStatementAfter(St_Bploop(OpenInterval(stmt)), cur_st,stmt->controlParent());            
               SkipIndepLoopNest(stmt);         
             } 
             else {// dvm_debug == 0 && perf_analysis == 0 or 2, i.e. standard mode 
               par_do = stmt->lexNext();// first DO statement of parallel loop 
	       SkipIndepLoopNest(stmt); //  to extract nested INDEPENDENT directives
             }
             //including the HPF  directive to list
             pstmt = addToStmtList(pstmt, stmt); 
             stmt = cur_st; // setting stmt on last DO statement of parallel loop nest
             break;

       case DVM_REDUCTION_WAIT_DIR:
	    if(debug_regim) {
	      
	      SgExpression *rg = new SgVarRefExp(stmt->symbol());
              LINE_NUMBER_AFTER(stmt,stmt);
              doCallAfter(DeleteObject_H(rg)); 
              doAssignTo_After(rg, new SgValueExp(0)); 	   
                  //Extract_Stmt(stmt); // extracting DVM-directive  
              doCallAfter( D_DelRG(DebReductionGroup( rg->symbol())));               
            }
            wait_list = addToStmtList(wait_list, stmt); 
            pstmt = addToStmtList(pstmt, stmt); 
            stmt = cur_st;//setting stmt on last inserted statement       
            break;
       case DVM_ASYNCHRONOUS_DIR:
	    dvm_debug=0;
            pstmt = addToStmtList(pstmt, stmt); 
            break;
       case DVM_ENDASYNCHRONOUS_DIR:
	    dvm_debug=(cur_fragment && cur_fragment->dlevel)? 1 : 0;
	    pstmt = addToStmtList(pstmt, stmt); 
            break;
       case DVM_REDUCTION_START_DIR: 
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
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt); 
            break;

//Debugging Directive
      case DVM_INTERVAL_DIR:
	  if (perf_analysis > 1){
            //generating call to 'binter' function of performance analizer
	    // (begin of user interval)
            
            LINE_NUMBER_AFTER(stmt,stmt);
            InsertNewStatementAfter(St_Binter(OpenInterval(stmt),Value_F95(stmt->expr(0))), cur_st,cur_st->controlParent()); 
          }
          pstmt = addToStmtList(pstmt, stmt);  //including the DVM  directive to list
          stmt = cur_st; 
          break;

      case DVM_ENDINTERVAL_DIR:
          if (perf_analysis > 1){
            //generating call to 'einter' function of performance analizer
	    // (end of user interval)
            
            if(!St_frag){
              err("Unmatched directive",182,stmt);
              break;
            }
            if(St_frag && St_frag->begin_st &&  (St_frag->begin_st->controlParent() != stmt->controlParent()))
                err("Misplaced directive",103,stmt); //interval must be a block
	    LINE_NUMBER_AFTER(stmt,stmt);
            InsertNewStatementAfter(St_Einter(INTERVAL_NUMBER,INTERVAL_LINE), cur_st, stmt->controlParent());
            CloseInterval();
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;
          }
          else
            pstmt = addToStmtList(pstmt, stmt); //including the DVM  directive to list 
          break;

      case DVM_EXIT_INTERVAL_DIR:
          if (perf_analysis > 1){
            //generating calls to 'einter' function of performance analizer
	    // (exit from user intervals)
            
            if(!St_frag){
              err("Misplaced directive",103,stmt);
              break;
            }
            ExitInterval(stmt);
            Extract_Stmt(stmt); // extracting DVM-directive           
            stmt = cur_st;
          }
          else
            pstmt = addToStmtList(pstmt, stmt);  //including the DVM  directive to list
            break;

       case DVM_OWN_DIR: 
            if(dvm_debug && stmt->lexNext()->variant() == ASSIGN_STAT) 
               own_exe = 1;
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
            break;
       case DVM_DEBUG_DIR:
         { int num;
            if((stmt->expr(0)->variant() != INT_VAL) || (num=stmt->expr(0)->valueInteger())<= 0)
              err("Illegal fragment number",181,stmt);  
            else  if(debug_fragment || perf_fragment)
              BeginDebugFragment(num,stmt);
            
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
	 }
            break;

       case DVM_ENDDEBUG_DIR: 
	 { int num; 
            if((stmt->expr(0)->variant() != INT_VAL) || (num=stmt->expr(0)->valueInteger())<= 0)
              err("Illegal fragment number",181,stmt);   
            else if((cur_fragment && cur_fragment->No != num) || !cur_fragment && (debug_fragment || perf_fragment))
              err("Unmatched directive",182,stmt);
            else {
             if(cur_fragment && cur_fragment->begin_st && (stmt->controlParent() != cur_fragment->begin_st->controlParent()))
                                                               //test of nesting blocks
               err("Misplaced directive",103,stmt); 
             EndDebugFragment(num);
	    }
 
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt); 
	 } 
            break;

       case DVM_TRACEON_DIR:
            InsertNewStatementAfter(new SgCallStmt(*fdvm[TRON]),stmt,stmt->controlParent()); 
            Extract_Stmt(stmt);// extracting DVM-directive 
            stmt = cur_st;
            break;

       case DVM_TRACEOFF_DIR:  
            InsertNewStatementAfter(new SgCallStmt(*fdvm[TROFF]),stmt,stmt->controlParent());  
            Extract_Stmt(stmt);// extracting DVM-directive 
            stmt = cur_st;
            break;

       case DVM_BARRIER_DIR:
            doAssignStmtAfter(Barrier()); 
            FREE_DVM(1);
            LINE_NUMBER_AFTER(stmt,stmt);
            Extract_Stmt(stmt);// extracting DVM-directive             
            stmt = cur_st;
            break;

       case DVM_CHECK_DIR:
	    if(check_regim) {
              cur_st = Check(stmt);  
              Extract_Stmt(stmt); // extracting DVM-directive            
              stmt = cur_st;
            } else
              pstmt = addToStmtList(pstmt, stmt);     
            break;

      case DVM_TASK_REGION_DIR:
          task_region_st = stmt;
          in_task_region++; 
	  if(dvm_debug){	
            //task_region_st = stmt;
            //task_region_parent = stmt->controlParent(); //to test nesting blocks
            //task_lab = (SgLabel *) NULL;
            task_ind = ndvm++; 
            DebugTaskRegion(stmt);
	  }                
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);
            stmt = cur_st;   
            break;
          
      case DVM_END_TASK_REGION_DIR:
          if(dvm_debug)
                CloseTaskRegion(task_region_st,stmt); 
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
            stmt = cur_st; 
            in_task_region--;
            break;     
      case DVM_ON_DIR: 
          if(dvm_debug) {
            if( stmt->expr(0)->symbol() && IS_DVM_ARRAY(stmt->expr(0)->symbol()))
              in_on++;
	    else if(in_task_region) {
              LINE_NUMBER_AFTER(stmt,stmt);
              doAssignTo_After(DVM000(task_ind),ReplaceFuncCall(stmt->expr(0)->lhs()->lhs()));
              InsertNewStatementAfter(D_Iter_ON(task_ind,TypeDVM()),cur_st,stmt->controlParent());
	    }
          }
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
            stmt = cur_st; 
            break;  
       case DVM_END_ON_DIR:
           pstmt = addToStmtList(pstmt, stmt);
           if(dvm_debug && in_on) { 
             SgStatement *std = dbg_if_regim ?  CreateIfThenConstr(DebugIfCondition(),D_Skpbl())  : D_Skpbl();             
             InsertNewStatementAfter(std,stmt,stmt->controlParent());
             stmt =lastStmtOf(std);  
             in_on--;
           }
            break;
                 
	 /* case DVM_INDIRECT_ACCESS_DIR: */
       case DVM_MAP_DIR:    
       case DVM_RESET_DIR:
       case DVM_PREFETCH_DIR:  
       case DVM_PARALLEL_TASK_DIR:
       case DVM_LOCALIZE_DIR:
       case DVM_SHADOW_ADD_DIR: 
       case DVM_IO_MODE_DIR:
       case DVM_TEMPLATE_CREATE_DIR:
       case DVM_TEMPLATE_DELETE_DIR:    
            //including the DVM  directive to list
            pstmt = addToStmtList(pstmt, stmt);  
            break;
//Input/Output statements
       case OPEN_STAT:
       case CLOSE_STAT:
       case INQUIRE_STAT:
       case BACKSPACE_STAT:
       case ENDFILE_STAT:
       case REWIND_STAT:
       case WRITE_STAT:
       case READ_STAT:
       case PRINT_STAT:
            if(perf_analysis)  
              stmt = Any_IO_Statement(stmt); 
            break;
       case DVM_CP_CREATE_DIR:  /*Chek Point*/
            CP_Create_Statement(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case DVM_CP_SAVE_DIR:
            CP_Save_Statement(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;
       case DVM_CP_LOAD_DIR:
            CP_Load_Statement(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;
      case DVM_CP_WAIT_DIR:
            CP_Wait(stmt, WITH_ERR_MSG);
            stmt = cur_st;
            break;            /*Chek Point*/

       default:
            break;     
    }

  { SgStatement  *end_stmt; 
  end_stmt = isSgLogIfStmt(stmt->controlParent()) ? stmt->controlParent() : stmt;

  if(inparloop && isParallelLoopEndStmt(end_stmt,par_do))  { // is last statement of parallel loop
       SgStatement *go_stmt = NULL;  
       inparloop = 0;  // closing parallel loop nest
       //replacing the label of DO statements locating  above parallel loop  in nest,
       // which is ended by stmt,
       // by new label and inserting CONTINUE with this label 
       ReplaceDoNestLabel_Above(end_stmt, par_do, GetLabel());
       if(debug_regim && HPF_program)
         INDReductionDebug();
       if(dvm_debug) { 
         CloseDoInParLoop(end_stmt); //on debug regim end_stmt==stmt
         end_stmt = cur_st;
         if(dbg_if_regim) {
           // generating GO TO statement:  GO TO begin_lab
           // and inserting it after last statement of parallel loop nest 
           go_stmt = new SgGotoStmt(*begin_lab);
           cur_st->insertStmtAfter(*go_stmt,*par_do->controlParent());
           cur_st = go_stmt; // GO TO statement
         }
	 // generating call statement : call dendl(...)
         CloseParLoop(end_stmt->controlParent(),cur_st,end_stmt);
         if(dbg_if_regim) 
           //setting label of ending parallel loop nest
           (go_stmt->lexNext())->setLabel(*end_lab);
         if(irg) {
           // generating statement:
           //  call dvmh_delete_object(RedGroupRef)     // dvm000(i) = delobj(RedGroupRef)
           doCallAfter(DeleteObject_H(redgref));
           if(idebrg)
              doCallAfter( D_DelRG(DVM000(idebrg)));   
         } 
       } else  if(perf_analysis == 4)
         SeqLoopEndInParLoop(end_stmt,stmt);

       if(perf_analysis && perf_analysis != 2) {
         // generating call eloop(...) - end of parallel interval
         //(performance analyzer function)
         InsertNewStatementAfter(St_Enloop(INTERVAL_NUMBER,INTERVAL_LINE),cur_st,cur_st->controlParent());
         CloseInterval();
         if(perf_analysis != 4)
           OverLoopAnalyse(func);
      }

      stmt = cur_st;
      if(dvm_debug)
        {SET_DVM(iplp);}
      continue;
  }    
  
  if(isDoEndStmt_f90(end_stmt)) {
    if(dvm_debug)
      CloseLoop(stmt); // on debug regim stmt=end_stmt 
    else if (perf_analysis && close_loop_interval)
      SeqLoopEnd(end_stmt,stmt);
    stmt = cur_st; 
   }
  }
 }  
  
END_:      

 // for declaring dvm000(N) is used maximal value of ndvm
  SET_DVM(ndvm); 
  cur_st =  first_dvm_exec;
  if(last_dvm_entry)
    lentry = last_dvm_entry->lexNext();
  if(!IN_MODULE) { 
    InitRemoteGroups();
    //InitFileNameVariables();
    if(debug_regim) {
      InitRedGroupVariables();
      WaitDirList();
    }
    DoStmtsForENTRY(first_dvm_exec,lentry);
    fmask[FNAME] = 0;
    stmt = data_stf ? data_stf->lexPrev() : first_dvm_exec->lexPrev();
    DeclareVarDVM(stmt,stmt);
  } else {
    if(mod_proc)
      MayBeDeleteModuleProc(mod_proc,end_of_unit);
    fmask[FNAME] = 0;     
    nloopred = nloopcons = MAX_RED_VAR_SIZE;
    stmt= mod_proc ? has_contains->lexPrev() : first_dvm_exec->lexPrev();
    DeclareVarDVM(stmt, (mod_proc ? mod_proc : stmt));  
  }
  first_dvm_exec->extractStmt();   //extract fname() call 
  for(;pstmt; pstmt= pstmt->next)
     Extract_Stmt(pstmt->st);// extracting  DVM+ACC  Directives  

  return; 
}

void VarDVM(SgStatement * func )
 { SgArrayType *typearray;
 typearray =new SgArrayType(*SgTypeInt()); //typearray-> addRange(N);
 dvmbuf = new SgVariableSymb("dvm000", *typearray, *func);
 }

void  RegistrateArg(SgExpression *ele)
{ 
  SgExpression *el, *e;
  e = ele->lhs(); //argument
 if(!e)
    return;
 
 if(isSgArrayRefExp(e)) {
   if(!(e->lhs())) // argument is whole array (array name)
       return;
   el=e->lhs()->lhs();  //first subscript of argument
   //testing: is first subscript of ArrayRef a POINTER 
   if((isSgVarRefExp(el) || isSgArrayRefExp(el)) && IS_POINTER(el->symbol())){
      if(!strcmp(e->symbol()->identifier(),"heap") || (e->symbol()->attributes() & HEAP_BIT))
         heap_point = HeapList(heap_point,e->symbol(),el->symbol());
   }      
 }   
 return;
}

SgExpression *CalcLinearForm(SgSymbol *ar, SgExpression *el)
{
  int i;
  SgExpression *ei, *index_list=NULL;
  for(i=0; el; el=el->rhs(),i++) 
  { 
     ei = &(el->lhs()->copy()); 
     ei = new SgExprListExp(*DvmType_Ref(ei)); 
     ei->setRhs(index_list);
     index_list = ei;
  } 
  return (CalculateLinear(HeaderRef(ar),i,index_list));

}

void DistArrayRef(SgExpression *e, int modified, SgStatement *st)
{ SgSymbol *ar;
  SgExpression *rme;
  int *h;
         //replace distributed array reference A(I1,I2,...,In) by
         //                                 n   
         // <memory>( Header(n+1) +  I1 + SUMMA(Header(n-k+1) * Ik))
         //                                k=2                    
         // <memory> is I0000M  if A  is of type integer 
         //             R0000M  if A  is of type real 
         //             D0000M  if A  is of type double precision 
         //             C0000M  if A  is of type complex
         //             L0000M  if A  is of type logical 

         // modified == 1 for variable in left part of assign statement
   
  hpf_ind = 0;
  ar = e -> symbol();  
  if(IS_POINTER(ar)){
     Error("Illegal POINTER reference: '%s'",ar->identifier(),138,st); 
     return;
  }                                       
  h=HEADER(ar);
  if(h && isSgArrayType(e->type()))
  {  Error("Illegal distributed array reference: %s",ar->identifier(),335,st); 
     return;
  }

  if(h) { //distributed array reference
    if(*h > 1)
         Error("Illegal template reference: '%s'",ar->identifier(),167,st);
    if(HPF_program && inparloop && modified && !IND_target)
       IND_target = IND_ModifiedDistArrayRef(e,st);
    if(HPF_program && inparloop && !modified ) {
       if(!IND_target_R)
           IND_target_R = IND_ModifiedDistArrayRef(e,st);
       IND_UsedDistArrayRef(e,st);
       return;
    }
    if(!modified && (rma || HPF_program) && (rme=isRemAccessRef(e))) 
                                                       // is remote variable reference
         ChangeRemAccRef(e,rme);
      
    else {   
         /*	if(!inparloop && !own_exe) 
          Error("Distributed array element reference outside the range of parallel loop: '%s'",ar->identifier(),cur_st);  */

      if(isPrivateInRegion(ar))  //private array in loop of region
        return;  // array reference is not changed !!!
      if(for_host)       //if(IN_COMPUTE_REGION && inparloop && !for_kernel && options.isOn(O_HOST) )
        return;  // array reference is not changed !!!
      if(for_kernel)   /*ACC*/
        ;
      else if(opt_base && inparloop && !HPF_program)
        e->setSymbol( *ARRAY_BASE_SYMBOL(ar)); 
      else
        e->setSymbol(baseMemory(ar->type()->baseType()));  
      if(!e->lhs())
        Error("No subscripts: %s", ar->identifier(),171,st);
      else {  
        (e->lhs())->setLhs( (INTERFACE_RTS2 && !inparloop) ? *CalcLinearForm(ar,e->lhs()) : *LinearForm(ar,e->lhs()));
        (e->lhs())->setRhs(NULL); 
           } 
    } 
  /*ACC*/
  } else   { // replicated array in region   
      if(for_host)       
        return;  // array reference is not changed !!!    
      if(!for_kernel)   /*ACC*/
        e->setSymbol(baseMemory(ar->type()->baseType()));  
      if(!e->lhs())
        Error("No subscripts: %s", ar->identifier(),171,st);
      else    
      { if(DUMMY_FOR_ARRAY(ar) && *DUMMY_FOR_ARRAY(ar)!=NULL)  // for case of syntax error in PARALLEL directive 
        { (e->lhs())->setLhs(*LinearForm(*DUMMY_FOR_ARRAY(ar),e->lhs()));
          (e->lhs())->setRhs(NULL);
        } 
      } 
    
  }

} 


void GoRoundEntry(SgStatement *stmt)
{SgLabel *lab;
if((stmt->lexPrev()->variant() == RETURN_STAT) || (stmt->lexPrev()->variant() == STOP_STAT) ||(stmt->lexPrev()->variant() == GOTO_NODE)) // going round is 
   return;

if(!(lab=stmt->lexNext()->label())) {//next statement has not label 
    lab = GetLabel();
    (stmt->lexNext())->setLabel(*lab); 
}
stmt->insertStmtBefore(* new SgGotoStmt(*lab));
return;
}
void BeginBlockForEntry(SgStatement *stmt)
{if(stmt)
    return;
 return;
}
int TestLeftPart(symb_list *new_red_var_list, SgExpression *le)
{symb_list *ls;
  if(!le)
    return(0);
  if(isDistObject(le))
    return(1);
  if(le->variant() == ARRAY_OP)
    return(TestLeftPart(new_red_var_list,le->lhs()));
  if(le->symbol()){
    for(ls= new_red_var_list; ls; ls=ls->next)
       if( le->symbol() == ls->symb)
         return(1);
    return(0); 
  }   
  else
   return(0);
}
int isInSymbList(symb_list *ls,SgSymbol *s)
{symb_list *l;
 for(l=ls; l; l=l->next)
    if(s == l->symb)
       return(1);
 return(0); 
}

void TestReverse(SgExpression *e,SgStatement *st)
{
 if(e && e->isInteger() && (e->valueInteger() < 0))
     err("Reverse is not supported",163,st);
 return;
} 

void LineNumber(SgStatement *st)
{st->insertStmtAfter(*D_Lnumb(st->lineNumber()),*st->controlParent());}


int PointerRank(SgSymbol *p)
{int rank ;
 SgExpression *el;
 rank = 0;
 for(el= (*POINTER_DIR(p))->expr(1); el; el=el->rhs())
    rank++;
 return (rank);
}

SgType * PointerType(SgSymbol *p)
{return( (*POINTER_DIR(p))->expr(2)->type());}

void AssignPointer(SgStatement *ass)
{int r;
 SgSymbol *pl, *pr;
 //SgExpression *head_new, *head;
 //ifst=ndvm;
 pl = ass->expr(0)->symbol();
 pr = ass->expr(1)->symbol();
 /* if(IS_DVM_ARRAY(pl))
    Error("POINTER '%s' in left part of assign statement has DISTRIBUTE or ALIGN attribute",pl->identifier(), 172,ass);*//*28.12.99*/
 /* if(!IS_DVM_ARRAY(pr))
   Error("POINTER '%s' in right part of assign statement has not DISTRIBUTE or ALIGN attribute",pr->identifier(), ass);*/
 r = PointerRank(pl);
 if(PointerRank(pr) != r)
   err("Pointers are of different rank", 173,ass);
 if(PointerType(pr) != PointerType(pl))
   err("Pointers are of different type", 174,ass);
 TestArrayRef(ass->expr(0),ass);
 TestArrayRef(ass->expr(1),ass);

 /*LINE_NUMBER_AFTER(ass,ass);*/
 /*
    head_new = (ass->expr(0)->lhs()) ? AddFirstSubscript(ass->expr(0),new SgValueExp(1)) : HeaderRefInd(pl,1); 
     head     = (ass->expr(1)->lhs()) ? AddFirstSubscript(ass->expr(1),new SgValueExp(1)) : HeaderRefInd(pr,1); 
    doAssignStmtAfter(AddHeader(head_new,head));
 */
 /* 
 doAssignStmtAfter(AddHeader(PointerHeaderRef(ass->expr(0),1),PointerHeaderRef(ass->expr(1),1)));
 CopyHeader(ass->expr(0),ass->expr(1),r);
 SET_DVM(ifst);
 */
 return;
}

void AddFirstSubscript(SgExpression *ea, SgExpression *ei)
{SgExpression *el,*efirst;
 if(!ei || !ea)
   return;
 el = ea->lhs();
 efirst = new SgExprListExp(*ei);
 efirst -> setRhs(el);
 ea -> setLhs(efirst);
}
/*
SgExpression * PointerHeaderRef(SgExpression *pe, int ind)
  // P => P(ind)
  // P(i,j,...) => P(ind,i,j,...)
{SgSymbol *p;
 if(!(p=pe->symbol()))
    return (pe);
 if(p->attributes() & DIMENSION_BIT){ // POINTER p declared as array
    SgExpression *ef,*cpe;
    if(!pe->lhs())
     return (pe);
    cpe = & (pe->copy());
    ef = new SgExprListExp(* new SgValueExp(ind));
    ef->setRhs(cpe->lhs());
    cpe->setLhs(ef);
    return(cpe);
 }
 else
    return(HeaderRefInd(p,ind));
}
*/

SgExpression * PointerHeaderRef(SgExpression *pe, int ind)
  // P => HEAP(P+ind-1)
  // P(i,j,...) => HEAP(P(i,j,...)+ind-1)
{ SgExpression *ef,*cpe;
 if(!(pe->symbol()))
    return (pe); 
 if(!heap_ar_decl)
   return(pe);  //error: HEAP isn't declared
 cpe = new SgArrayRefExp(*heap_ar_decl->symbol());
 ef = (ind == 1) ? new SgExprListExp(pe->copy()) : new SgExprListExp(pe->copy()+(*new SgValueExp(ind-1)));
 cpe->setLhs(ef);
 return(cpe);
} 


void CopyHeader(SgExpression *ple, SgExpression *pre, int rank)
{    //int i;
     // for(i=0; i<rank; i++)
     // doAssignTo_After(PointerHeaderRef(ple,rank+2+i), PointerHeaderRef(pre,rank+2+i));
 doAssignTo_After(PointerHeaderRef(ple,rank+2), PointerHeaderRef(pre,rank+2));
     //for(i=0; i<rank; i++)
     // doAssignTo_After(PointerHeaderRef(ple,rank+2+i), new SgValueExp(1));
}

int TestArrayRef(SgExpression *e, SgStatement *stmt)
{SgSymbol *s;
 if(!(s=e->symbol()))
    return (0);
 if((s->attributes() & DIMENSION_BIT) && !e->lhs()) { // s  declared as array
    Error("No subscripts: %s", s->identifier(),171,stmt);
    return(0);
 } 
 return(1);
}

void AddDistSymbList(SgSymbol *s)
{  symb_list *ds;
  if(!dsym) {
     dsym = new symb_list;
     dsym->symb = s;
     dsym->next = NULL;
  } else {
     ds = new symb_list;
     ds->symb = s;
     ds->next = dsym;
     dsym = ds;
  }
}

void StoreLowerBoundsPlus(SgSymbol *ar,SgExpression *arref)
// generating assign statements to
//store lower bounds of array in Header(rank+3:2*rank+2)
//and to initialize counter of remote access buffers:  HEADER(2*rank+3) = 2*rank+4
//and to set the flag to 0: array is not distributed yet
{int i,rank;
 SgExpression *le;
 rank = Rank(ar);
 for(i=0;i<rank;i++) {
   le = IS_POINTER(ar) ? new SgValueExp(1) : Exprn( LowerBound(ar,i));
   doAssignTo(!arref ? header_ref(ar,rank+3+i) : PointerHeaderRef(arref,rank+3+i), le) ; 
 }
 if(!IS_TEMPLATE(ar)) {
   doAssignTo(!arref ? header_ref(ar,HSIZE(rank)+1) : PointerHeaderRef(arref,HSIZE(rank)+1), new SgValueExp(HSIZE(rank)+2));
                          // initializing HEADER(2*rank+3) - counter of remote access buffers
   if(ar->attributes() & POSTPONE_BIT)
     doAssignTo(!arref ? header_ref(ar,HEADER_SIZE(ar)) : PointerHeaderRef(arref,HEADER_SIZE(ar)), new SgValueExp(0));  
                          // HEADER(HEADER_SIZE) = 0 => the array is not distributed yet 
 } 
}

void StoreLowerBoundsPlusFromAllocate(SgSymbol *ar,SgExpression *arref,SgExpression *lbound)
// generating assign statements to
//store lower bounds of array in Header(rank+3:2*rank+2)
//and to initialize counter of remote access buffers:  HEADER(2*rank+3) = 2*rank+4
//and to set the flag to 0: array is not distributed yet
{int i,rank;
 SgExpression *le;
 rank = Rank(ar);
 for(i=0;i<rank;i++) {
   le = &(lbound->copy());
   if(lbound->lhs())
       le->lhs()->setLhs(Calculate(&(lbound->lhs()->lhs()->copy()+ *new SgValueExp(i))));
    else
       le->setLhs(new SgExprListExp(*new SgValueExp(i+1)));
   
   doAssignTo(!arref ? header_ref(ar,rank+3+i) : PointerHeaderRef(arref,rank+3+i), le) ; 
  }
 if(!IS_TEMPLATE(ar)) {
   doAssignTo(!arref ? header_ref(ar,HSIZE(rank)+1) : PointerHeaderRef(arref,HSIZE(rank)+1),     new SgValueExp(HSIZE(rank)+2));
                          // initializing HEADER(2*rank+3) - counter of remote access buffers
   if(ar->attributes() & POSTPONE_BIT)
     doAssignTo(!arref ? header_ref(ar,HEADER_SIZE(ar)) : PointerHeaderRef(arref,HEADER_SIZE(ar)), new SgValueExp(0));  
                          // HEADER(HEADER_SIZE) = 0 => the array is not distributed yet  
 }
}


void StoreLowerBoundsPlusOfAllocatable(SgSymbol *ar,SgExpression *desc)
// generating assign statements to
//store lower bounds of ALLOCATABLE array in Header(rank+3:2*rank+2)
//and to initialize counter of remote access buffers:  HEADER(2*rank+3) = 2*rank+4
//and to set the flag to 0: array is not distributed yet
{int i,rank;
 SgExpression *le,*el;
 rank = Rank(ar);
 for(i=0,el=desc->lhs();el;i++,el=el->rhs()) {
   le = (el->lhs()->variant() == DDOT) ? &el->lhs()->lhs()->copy() : new SgValueExp(1)  ;
   doAssignTo(header_ref(ar,rank+3+i), le) ; 
  }
 if(!IS_TEMPLATE(ar)) {
   doAssignTo(header_ref(ar,HSIZE(rank)+1),  new SgValueExp(HSIZE(rank)+2));
                          // initializing HEADER(2*rank+3) - counter of remote access buffers
   if(ar->attributes() & POSTPONE_BIT)
     doAssignTo(header_ref(ar,HEADER_SIZE(ar)), new SgValueExp(0));  
                          // HEADER(HEADER_SIZE) = 0 => the array is not distributed yet 
 } 
}


void StoreLowerBoundsPlusOfAllocatableComponent(SgSymbol *ar,SgExpression *desc, SgExpression *struct_)
// generating assign statements to
//store lower bounds of ALLOCATABLE array in Header(rank+3:2*rank+2)
//and to initialize counter of remote access buffers:  HEADER(2*rank+3) = 2*rank+4
//and to set the flag to 0: array is not distributed yet
{int i,rank;
 SgExpression *le,*el;
 rank = Rank(ar);
 for(i=0,el=desc->lhs();el;i++,el=el->rhs()) {
   le = (el->lhs()->variant() == DDOT) ? &el->lhs()->lhs()->copy() : new SgValueExp(1)  ;
   doAssignTo(header_ref_in_structure(ar,rank+3+i,struct_), le) ; 
  }
 doAssignTo(header_ref_in_structure(ar,HSIZE(rank)+1,struct_),  new SgValueExp(HSIZE(rank)+2));
                          // initializing HEADER(2*rank+3) - counter of remote access buffers
 if(ar->attributes() & POSTPONE_BIT)
   doAssignTo(header_ref_in_structure(ar,HEADER_SIZE(ar),struct_), new SgValueExp(0));  
                          // HEADER(HEADER_SIZE) = 0 => the array is not distributed yet  

}

void ReplaceLowerBound(SgSymbol *ar, int i)
//replace i-th lower bound of array 'ar' with Header(rank+3+i) reference in Symbol Table
// Li : Ui =>  Header(rank+3+i) : Ui
//i=0,...,rank-1
{SgExpression *e;
 SgArrayType *artype;
 artype = isSgArrayType(ar->type());
 if(artype) {
   e = artype->sizeInDim(i);
   if(e->lhs() && e->rhs()) // Li : Ui
     if(!(ReplaceParameter(&e->lhs()->copy())->isInteger()))
       e->setLhs(header_ref(ar,Rank(ar)+3+i));
 }
}

void ReplaceArrayBounds(SgSymbol *ar) 
{int i,rank;
 rank = Rank(ar);
 if( IS_DUMMY(ar))
   for(i=0; i<rank; i++)
       ReplaceLowerBound(ar,i); 
}

void StoreOneBounds(SgSymbol *ar)
// generating assign statements:
// Header(2*rank+3 +i) = 1, i=0,...,rank-1
{int i,rank;
 rank = Rank(ar);
 for(i=0;i<rank;i++)
   doAssignTo(header_ref(ar,rank+3+i), new SgValueExp(1)); 
}
 

SgExpression *ConstRef(int ic)
{
 dvm_const_ref = 1; 
 if(ic>9){
   if(ic == 16)
      return(&(*new SgVarRefExp(Iconst[8])+(*new SgVarRefExp(Iconst[8]))));
   else if(ic-9 < 10)
      return(&(*new SgVarRefExp(Iconst[ic-9])+(*new SgVarRefExp(Iconst[9]))));
   else
      return(&(*new SgVarRefExp(Iconst[9])+(*new SgValueExp(ic-9))));
     // err("Compiler bug. Integer constant > 9", 0,cur_st);
   return(new SgValueExp(ic)); 
 }
 return(new SgVarRefExp(Iconst[ic]));
}

SgExpression *SignConstRef(int ic)
{SgExpression *res;
 res = (ic < 0) ? &SgUMinusOp(*ConstRef(-ic)) : ConstRef(ic); 
 return(res);
} 

void TestParamType(SgStatement *stmt)
{SgType *t;
 t = stmt->expr(2)->symbol()->type();
 if(isSgArrayType(t) && (t->baseType()->variant() == T_FLOAT && TypeSize(t->baseType())==8 || t->baseType()->variant() == T_DOUBLE) && Rank(stmt->expr(2)->symbol())==2)
   return ;
 Error("Illegal type of parameter array '%s'",stmt->expr(2)->symbol()->identifier(),615,stmt);
} 

SgExpression *CountOfTasks(SgStatement *st)
{SgExpression *e;
  e = st->expr(0)->lhs()->lhs();  
  if(e->variant()==DDOT && !e->lhs() && !e->rhs()) //whole task's array
      return(ReplaceFuncCall(ArrayDimSize(st->expr(0)->symbol(),1)));
  else //section of task's array
  {   err("Section/element of task array. Not implemented yet.",614,st);
      return(new SgValueExp(0));
  } 
}

void ReconfPS( stmt_list *pstmt)
{ int rank;
  SgSymbol *pr;
  SgExpression *size_array, *le;
  stmt_list *lst;
  //looking through the DVM specification directive (pstmt) 
  for(lst=pstmt; lst; lst=lst->next)
    if(lst->st->variant() == HPF_PROCESSORS_STAT)       
      for (le=lst->st->expr(0); le; le = le->rhs()) { //looking through the processor list
         pr= le->lhs()->symbol();
         proc_symb = AddToSymbList(proc_symb, pr);
         LINE_NUMBER_BEFORE(lst->st,where);
                                   // for tracing set the global variable of LibDVM to
                                  // line number of directive PROCESSORS
         rank = Rank(pr);
         if(!rank) { // is not array  P => P(1)
           size_array = dvm_array_ref();
           doAssignStmt(new SgValueExp(1));
           rank = 1;
         } else   
           size_array = doSizeArrayD(pr,lst->st); 

         // pr = reconf(PSRef, rank, SizeArray, StaticSign)
         // reconf() creates processor system
         doAssignTo(new SgVarRefExp(pr),Reconf(size_array, rank, 0));
      }
}

SgExpression *CurrentPS ()
{SgExpression *ps;
  if(in_task_region)
     ps = new SgArrayRefExp(*task_array, *new SgValueExp(1),*DVM000(task_ind)); 
  /* else if(fmask[GETAM] == 0) // not GETVM but GETAM !!
     ps = GetProcSys(ConstRef(0));  //ConstRef(0); constant = 0
  else
     ps =  DVM000(3); 
   */
  else 
    ps = ConstRef(0); 
  return(ps);
 
}

SgExpression *CurrentAM ()
{SgExpression *am;
  am = ConstRef(0); //DVM000(2); //ConstRef(0); //GetAM(); 
  return(am);
}

SgExpression *ParentPS ()
{ return( GetProcSys(&SgUMinusOp(*ConstRef(1))));} 

SgExpression *PSReference(SgStatement *st)
{SgExpression *target,*es,*le[MAX_DIMS],*re[MAX_DIMS];
 SgValueExp c1(1);
 int ile,ips,rank,j,i;

 target =  (st->variant() == DVM_MAP_DIR) ? st->expr(1) : st->expr(2);
 if(!target)
   return( CurrentPS());
 /*
 if(st->variant() == DVM_REDISTRIBUTE_DIR){
    target = target->lhs(); 
    if(target->variant() == NEW_VALUE_OP)
       return( CurrentPS());
 }
 */
 if(target->symbol()->attributes()  & PROCESSORS_BIT){
    if(!target->lhs())
       return(target);
      // return( new SgVarRefExp(target->symbol()));
    
    for(es=target->lhs(),j=0; es; es=es->rhs(),j++){ //looking through the subscript list
       if(j==MAX_DIMS) {
            Error("Too many dimensions specified for %s", target->symbol()->identifier(),43,st);
            break;
       }  
       if(es->lhs()->variant() == DDOT) {
         //determination of dimension bounds
         if(!es->lhs()->lhs() && !es->lhs()->rhs()){ 
            le[j] = new SgValueExp(0);
            re[j] = &(*Exprn(UpperBound(target->symbol(),j)) - *Exprn(LowerBound(target->symbol(),j)));
         } else if(!es->lhs()->lhs()  && es->lhs()->rhs()) {
            le[j] = new SgValueExp(0);
            re[j] = &(*es->lhs()->rhs() - *Exprn(LowerBound(target->symbol(),j)));
         } else if(es->lhs()->lhs()  && !es->lhs()->rhs()) {
            le[j] = &(*es->lhs()->lhs() - *Exprn(LowerBound(target->symbol(),j)));
            re[j] = &(*Exprn(UpperBound(target->symbol(),j)) - *Exprn(LowerBound(target->symbol(),j)));
         } else if(es->lhs()->lhs()  && es->lhs()->rhs()) {
            le[j] = &(*es->lhs()->lhs() - *Exprn(LowerBound(target->symbol(),j)));
            re[j] = &(*es->lhs()->rhs() - *Exprn(LowerBound(target->symbol(),j)));
         }
       } else {
            le[j] = &(*es->lhs() - *Exprn(LowerBound(target->symbol(),j)));
            re[j] = &le[j]->copy();
       }
    }
    rank = Rank(target->symbol());
    if(rank && rank != j)
      Error("Wrong  number of subscripts specified for %s", target->symbol()->identifier(),140,st);   

    ile = ndvm; 
    for(i=0; i<j; i++) //creating Size Array
      doAssignStmt(Calculate(le[i])); 
    for(i=0; i<j; i++) //creating Size Array
      doAssignStmt(Calculate(re[i])); 
    ips = ndvm;
    doAssignStmt(CrtPS(new SgArrayRefExp(*target->symbol()), ile, ile+j, 0));
    return (DVM000(ips));
 } 

 if(target->symbol()->attributes()  & TASK_BIT)     
   return(TaskPS(target,st));
 return( CurrentPS());
}

SgExpression *TaskPS(SgExpression *target,SgStatement *st)
{
  if(!target->lhs() || target->lhs()->rhs()) //there are no subscript or >1
      Error("Wrong  number of subscripts specified for  %s", target->symbol()->identifier(),140,st);  
  return( new SgArrayRefExp(*target->symbol(), *new SgValueExp(1),*target->lhs()->lhs()));
}

SgExpression *hasNewValueClause(SgStatement *stdis)
{SgExpression *e;
  e = stdis->expr(2); 
  if(!e) // NEW_VALUE clause is absent
     return (e);
  e = e->lhs();
  if(e->variant() == NEW_VALUE_OP)
     return(e);
  else if(e->rhs())
     return(e->rhs()->lhs());
  return(NULL);
}

SgExpression *hasOntoClause(SgStatement *stdis)
{SgExpression *target;
 SgSymbol *tsymb;
  target = stdis->expr(2); 
  if(!target) //ONTO clause is absent
     return (target);
  if(isSgExprListExp(target)){
     target = target->lhs();
     if(target->variant() == NEW_VALUE_OP)
     return(NULL);
  }
  tsymb = target->symbol();
  if(!(tsymb->attributes() & DIMENSION_BIT))
       Error("'%s' isn't array",tsymb->identifier(),66,stdis);
  if(stdis->variant() == DVM_DISTRIBUTE_DIR){
    if(!(tsymb->attributes() & PROCESSORS_BIT))
       Error("'%s' hasn't PROCESSORS attribute",tsymb->identifier(),176,stdis);
  }  else  // REDISTRIBUTE directive 
       if(!(tsymb->attributes() & PROCESSORS_BIT) && !(tsymb->attributes() & TASK_BIT))
          Error("'%s' hasn't PROCESSORS/TASK attribute",tsymb->identifier(),176,stdis); 
  return(target);     
}

int RankOfSection(SgExpression *are)
{int rank;
// SgExpression *el;
//int ndim;
 if(!are)
   return(0);
 if(are->symbol()->attributes() & TASK_BIT)
   return(0);
 rank = Rank(are->symbol());
 if(!are->lhs()) 
    return(rank ? rank : 1 );

 return (rank);
 /*for(el=are->lhs(),ndim=0; el; el = el->rhs(), ndim++)
     ; 
 return(ndim <= rank ? ndim : rank); 
 */
}

void   CreateTaskArray(SgSymbol *ts)
{int isize,iamv;
 SgExpression *le,*re, *e;
 SgArrayType *artype;
 SgSymbol **tsk_amv = new (SgSymbol *);
 SgSymbol **tsk_ind = new (SgSymbol *);
 SgSymbol **tsk_renum_array = new (SgSymbol *);
 SgSymbol **tsk_lps = new (SgSymbol *);
 SgSymbol **tsk_hps = new (SgSymbol *);

 isize = ndvm++;
 SgStatement *dost,*as;
 nio = (nio < 1 ) ? 1: nio;
 artype = isSgArrayType(ts->type());
 doAssignTo(DVM000(isize),ReplaceFuncCall(&artype->sizeInDim(0)->copy()));
 iamv = ndvm; 
 task_ps=iamv;
                    //doAssignStmt(CreateAMView(DVM000(isize), 1, 0));
 *tsk_amv = TaskAMVSymbol(ts);
 doAssignTo(new SgVarRefExp(*tsk_amv),CreateAMView(DVM000(isize), 1, 0));
                    //loop_lab = GetLabel();
 le = new SgArrayRefExp(*ts,*new SgValueExp(2),*new SgVarRefExp(loop_var[0]));
 *tsk_renum_array = TaskRenumArraySymbol(ts);
 e = &(*new SgArrayRefExp(**tsk_renum_array,*new SgVarRefExp(loop_var[0])) - *new SgValueExp(1));
 re = GetAMR(new SgVarRefExp(*tsk_amv),e);
 as = new SgAssignStmt(*le,*re);
 dost= new SgForStmt(loop_var[0], new SgValueExp(1), DVM000(isize), new SgValueExp(1), as);
                    //BIF_LABEL_USE(dost->thebif) = loop_lab->thelabel;
                    //as->setLabel(*loop_lab); 
 where->insertStmtBefore(*dost,*where->controlParent());
                    //as->lexNext()->extractStmt();
                    //le = DVM000(iamv+1);
                    //re = &(*new SgVarRefExp(loop_var[0]) - *new SgValueExp(1)); //dvm000(...)=i-1
 /* initializing renumeration array */
 le = new SgArrayRefExp(**tsk_renum_array,*new SgVarRefExp(loop_var[0]));
 re = new SgVarRefExp(loop_var[0]);
 as->insertStmtBefore(*new SgAssignStmt(*le,*re));
 //SET_DVM(isize); 
              // index = new int;
             // *index = task_ps;
            // adding the attribute (TASK_INDEX) to TASK symbol
           //   ts->addAttribute(TASK_INDEX, (void *) index, sizeof(int));
 // adding the attribute (TSK_SYMBOL) to TASK symbol  
   ts->addAttribute(TSK_SYMBOL, (void*) tsk_amv, sizeof(SgSymbol *));
 *tsk_ind = TaskIndSymbol(ts);
 // adding the attribute (TSK_IND_VAR) to TASK symbol  
   ts->addAttribute(TSK_IND_VAR, (void*) tsk_ind, sizeof(SgSymbol *));

 // adding the attribute (TSK_RENUM_ARRAY) to TASK symbol  
   ts->addAttribute(TSK_RENUM_ARRAY, (void*) tsk_renum_array, sizeof(SgSymbol *));
 *tsk_lps = TaskLPsArraySymbol(ts);
 // adding the attribute (TSK_LPS_ARRAY) to TASK symbol  
   ts->addAttribute(TSK_LPS_ARRAY, (void*) tsk_lps, sizeof(SgSymbol *));
 *tsk_hps = TaskHPsArraySymbol(ts);
 // adding the attribute (TSK_HPS_ARRAY) to TASK symbol  
   ts->addAttribute(TSK_HPS_ARRAY, (void*) tsk_hps, sizeof(SgSymbol *));         
 return;
}

int LoopVarType(SgSymbol *var,SgStatement *st)
{ int len;
  SgType *type;
 
  type = var->type();
  if(!type)
    return(0);
    len =  TypeSize(type);    /*16.04.04 */
     /*len = IS_INTRINSIC_TYPE(type) ? 0 : TypeSize(type);*/
    //len = (TYPE_RANGES(type->thetype)) ? type->length()->valueInteger() : 0; 14.03.03
  if(bind == 0)
    switch(type->variant()) {
        case T_INT:    return((len == 2) ? 2 : 0); // (long = int)
        default:    
                     { Error("Illegal type of do-variable '%s'",var->identifier(),178,st);
                       return(0);
                     }
    }
  if(bind == 1)
    switch(type->variant()) {
        case T_INT:    if     (len == 8) return(0);
                       else if(len == 2) return(2);
                       else              return(1);
    
        default:     { Error("Illegal type of do-variable '%s'",var->identifier(),178,st);
                       return(0);
                     }
    }
  return(0);
}

int LocVarType(SgSymbol *var,SgStatement *st)
{ int len;
  SgType *type;
  if(!var)
    return(0);
  type = var->type();
  if(!type)
    return(0);
  if (isSgArrayType(type))
    type = type->baseType();
  len =  TypeSize(type);    /*16.04.04 */
  if(bind == 0)
    switch(type->variant()) {
        case T_INT:  if(len == 4)      return(0);  // (long = int)
                     else if(len == 2) return(2);
                     else if(len == 1) return(3);
                     else
                     { err("Wrong operand of MAXLOC/MINLOC",149,st);
                       return(0);
                     }
                         
        default:    
                     { err("Wrong operand of MAXLOC/MINLOC",149,st);
                       return(0);
                     }
    }
  if(bind == 1)
    switch(type->variant()) {
        case T_INT:    if     (len == 8) return(0);
                       else if(len == 4) return(1);
                       else if(len == 2) return(2);
                       else if(len == 1) return(3);
                       else             
                       { err("Wrong operand of MAXLOC/MINLOC",149,st);
                         return(0);
                       }
        default:     { err("Wrong operand of MAXLOC/MINLOC",149,st);
                       return(0);
                     }
    }
  return(0);
}


int TypeDVM()
{return(0);}

void StartTask(SgStatement *stmt)
{SgStatement *if_stmt, *st;
 SgExpression *ei;
 ei = stmt->expr(0)->lhs()->lhs();
 doAssignTo_After(DVM000(task_ind),ReplaceFuncCall(ei));
 if(!isSgVarRefExp(ei) && !isSgValueExp(ei))
    ei = DVM000(task_ind); 
 st =  (stmt->variant()==DVM_ON_DIR) ? new SgGotoStmt(*task_lab) : new SgStatement(CYCLE_STMT);
 if_stmt =  new SgLogIfStmt(SgEqOp(*RunAM(new SgArrayRefExp(*(stmt->expr(0)->symbol()),
*new SgValueExp(2),*ei)),*new SgValueExp(0) ),*st);
  cur_st->insertStmtAfter(*if_stmt);
  cur_st = if_stmt->lexNext(); // CYCLE statement or GOTO statement
  (cur_st->lexNext())-> extractStmt(); //extract ENDIF
  if(dvm_debug)
     if( stmt->variant()==DVM_ON_DIR) 
       InsertNewStatementAfter(D_Iter_ON(task_ind,TypeDVM()),cur_st,stmt->controlParent());
 
 return;
}

void InitGroups()
{  group_name_list *sl;
   for(sl=grname; sl; sl=sl->next)
     if(!IS_SAVE(sl->symb))  
     /* if (sl->symb->variant() == REF_GROUP_NAME){
        doAssignTo(new SgArrayRefExp(*sl->symb,*new SgValueExp(1)),new SgValueExp(0));
        doAssignTo(new SgArrayRefExp(*sl->symb,*new SgValueExp(2)),new SgValueExp(0));
        doAssignTo(new SgArrayRefExp(*sl->symb,*new SgValueExp(3)),new SgValueExp(0));
      } else  */
        if (sl->symb->variant() == REDUCTION_GROUP_NAME || sl->symb->variant() == CONSISTENT_GROUP_NAME) 
          doAssignTo(new SgVarRefExp(*sl->symb),new SgValueExp(0));
        
}  
void CreateRedGroupVars()
{  group_name_list *sl;
   SgSymbol *rgs;

   for(sl=grname; sl; sl=sl->next)
        //if(!IS_SAVE(sl->symb))  ??? 
     if (sl->symb->variant() == REDUCTION_GROUP_NAME || sl->symb->variant() == CONSISTENT_GROUP_NAME) {
         SgSymbol **ss = new (SgSymbol *);
         rgs = new SgVariableSymb(RedGroupVarName(sl->symb), *new SgArrayType(*SgTypeInt()), *cur_func); 
         *ss = rgs;
         (sl->symb)->addAttribute( RED_GROUP_VAR, (void *) ss, sizeof(SgSymbol *));
     }    
}  

void InitShadowGroups()
{  group_name_list *sl;
   for(sl=grname; sl; sl=sl->next)
     if(!IS_SAVE(sl->symb))  
        if (sl->symb->variant() == SHADOW_GROUP_NAME)
          doAssignTo_After(new SgVarRefExp(*sl->symb),new SgValueExp(0));        
}  


void InitRemoteGroups()
{stmt_list *stl;
for(stl=pref_st; stl; stl=stl->next) {
doAssignTo_After(new SgArrayRefExp(*stl->st->symbol(),*new SgValueExp(1)),new SgValueExp(0));
doAssignTo_After(new SgArrayRefExp(*stl->st->symbol(),*new SgValueExp(2)),new SgValueExp(0));
doAssignTo_After(new SgArrayRefExp(*stl->st->symbol(),*new SgValueExp(3)),new SgValueExp(0));
}  
}


void InitRedGroupVariables()
{group_name_list *gl;
 int i,nl;
 SgSymbol *rgv;
 for(gl=grname; gl; gl=gl->next) 
  if (gl->symb->variant() == REDUCTION_GROUP_NAME  || gl->symb->variant() == CONSISTENT_GROUP_NAME) {
    rgv = * ((SgSymbol **) (gl->symb)-> attributeValue(0,RED_GROUP_VAR)); 
    nl = gl->symb->variant() == REDUCTION_GROUP_NAME ? nloopred : nloopcons;
    for(i=nl; i; i--)
      doAssignTo_After(new SgArrayRefExp(*rgv,*new SgValueExp(i)),new SgValueExp(0));
  }
}

void WaitDirList()
{stmt_list *stl;
 SgStatement *stat;
 SgSymbol *rgv, *rg;
 int i,nl;
 stat = cur_st;
 for(stl=wait_list; stl; stl=stl->next) {
  cur_st = stl->st;
  rg = ORIGINAL_SYMBOL(stl->st->symbol());
  rgv = * ((SgSymbol **) rg -> attributeValue(0,RED_GROUP_VAR)); 
  nl =(cur_st ->variant() == DVM_CONSISTENT_WAIT_DIR) ? ((cur_st->controlParent()->variant() == PROG_HEDR) ? 0 : nloopcons) : nloopred;
  for(i=nl; i; i--)
    doAssignTo_After(new SgArrayRefExp(*rgv,*new SgValueExp(i)),new SgValueExp(0));
}
 cur_st = stat;
}

void InitDebugVar()
{SgStatement *stcall;
 int flag;
if(!dbg_var) return;
flag = (only_debug) ? 0 : 1;
doAssignTo_After(new SgVarRefExp(*dbg_var),new SgValueExp(dbg_if_regim));
 cur_st->insertStmtAfter(*(stcall=D_PutDebugVarAdr(dbg_var,flag)));
 cur_st = stcall;
}

void InitFileNameVariables()
{  filename_list *sl;
   SgExpression *lenexp,*e;
   int length;
   SgFunctionSymb *fs = new SgFunctionSymb(FUNCTION_NAME, "char", *SgTypeChar(), *cur_func->controlParent());
   SgFunctionCallExp *fcall =  new SgFunctionCallExp(*fs);
   fcall->addArg(* new SgValueExp(0));
   if(filename_num>1 && cur_func->variant() != PROG_HEDR) {
     file_var_s = new SgVariableSymb(FileNameVar(0), *SgTypeInt(), *cur_func);
     cur_st = doIfForFileVariables(file_var_s);
   }
   for(sl=fnlist; sl; sl=sl->next){
      length = strlen(sl->name)+1;
      lenexp = new SgValueExp(length);
      e =  new SgExpression(ARRAY_OP);
      e->setLhs(new SgVarRefExp(*sl->fns));
      e->setRhs(new SgExpression(DDOT,lenexp,lenexp,(SgSymbol *)NULL));
      doAssignTo_After( e, fcall);  
   }  
   if(filename_num>1 && cur_func->variant() != PROG_HEDR){
     doAssignTo_After( new SgVarRefExp(*file_var_s), new SgValueExp(1));  
     cur_st = cur_st->lexNext();  
   }  
}  


void InitHeap(SgSymbol *heap)
//generating assign statement: HEAP(1) = 2
{  doAssignTo(ARRAY_ELEMENT(heap,1), new SgValueExp(2)); }

void InitAsyncid()
{symb_list *sl;
 for(sl=async_symb; sl; sl=sl->next)
  //generating assign statement: ASINCID(1) = 1 
   if((IN_COMMON(sl->symb) && IN_MAIN_PROGRAM) || !IN_COMMON(sl->symb))
     doAssignTo(ARRAY_ELEMENT(sl->symb,1), new SgValueExp(1));
 }

SgExpression * isDoVarUse (SgExpression *e, int use[], SgSymbol *ident[], int ni, int *num, SgStatement *st)
{
 SgExpression *ei;
 *num =  AxisNumOfDummyInExpr(e, ident, ni, &ei, use, st);
 if (*num<=0) 
   return(NULL);
 return(ei);
}

SgSymbol* isIndirectSubscript (SgExpression *e, SgSymbol *ident, SgStatement *st)
{//temporary
 if(e && ident && st)
   return(NULL);
 return(NULL);
}


/*
void InsertRedVarsInGroup(SgExpression *redgref,int irv,int nred)
{int i;
 for(i=irv+nred-1; i>=irv; i--)
   doAssignStmtAfter(InsertRedVar(redgref,i,iplp));
}
*/

/*
void BeginDebugFragment(int num,SgStatement *stmt)
{fragment_list *curfr;
 fragment_list_in *fr;

// searhing frament
  fr=debug_fragment;
//looking through the fragment list of command line
  while(fr && (fr->N1 >  num || fr->N2 < num) ) 
          fr=fr->next;
  if (fr){ //fragment with number 'num' is  found (N1 <= num <= N2)
        if(fr->dlevel){
               dvm_debug = 1;
               level_debug = fr->dlevel;
        }                
        if(fr->elevel)
               perf_analysis = fr->elevel;
        curfr = new fragment_list;
        curfr->No = num;
        if(fr->dlevel) 
            curfr->dlevel = fr->dlevel;
        else
            curfr->dlevel = cur_fragment ? cur_fragment->dlevel : 0;
        if(fr->elevel) 
            curfr->elevel = fr->elevel;
        else
            curfr->elevel = cur_fragment ? cur_fragment->elevel : 0;
        curfr->next = cur_fragment; 
        cur_fragment = curfr;   
  } else {//fragment with number 'num' is not found
        curfr = new fragment_list;
        curfr->No = num;
        curfr->dlevel = cur_fragment ? cur_fragment->dlevel : 0;
        curfr->elevel = cur_fragment ? cur_fragment->elevel : 0;
        curfr->next = cur_fragment; 
        cur_fragment = curfr;  
  } 
   return;
}

void BeginDebugFragment(int num, SgStatement *stmt)
{fragment_list *curfr;
 fragment_list_in *fr;
 int max_dlevel,max_elevel,is_max;
//determing maximal level
  if(stmt)
    is_max = MaxLevels(stmt,&max_dlevel,&max_elevel); 
  else 
    is_max =0;
  
// searhing fragment 
  fr=debug_fragment;
//looking through the fragment list of command line
  while(fr && (fr->N1 >  num || fr->N2 < num) ) 
          fr=fr->next;
  if (fr){ //fragment with number 'num' is  found (N1 <= num <= N2)
        if(fr->dlevel){
            if(fr->dlevel == -1){
               dvm_debug = 0;
               level_debug = 0;
	    } else {        
               dvm_debug = 1;
               level_debug = MinLevel(fr->dlevel,max_dlevel,is_max);
            }
        }                
        if(fr->elevel)
            if(fr->elevel == -1)  
               perf_analysis = 0;
            else
               perf_analysis = MinLevel(fr->elevel,max_elevel,is_max);
        curfr = new fragment_list;
        curfr->No = num;
        curfr->dlevel = level_debug;
        curfr->elevel = perf_analysis;
        curfr->next = cur_fragment; 
        cur_fragment = curfr;   
  } else {//fragment with number 'num' is not found
        curfr = new fragment_list;
        curfr->No = num;
        curfr->dlevel = cur_fragment ? MinLevel(cur_fragment->dlevel,max_dlevel,is_max) : 0;
        curfr->elevel = cur_fragment ? MinLevel(cur_fragment->elevel,max_elevel,is_max) : 0;
        curfr->next = cur_fragment; 
        cur_fragment = curfr; 
        perf_analysis =  curfr->elevel;
	level_debug =  curfr->dlevel;
	dvm_debug = level_debug ? 1 : 0; 
  } 
   return;
}
*/

void BeginDebugFragment(int num, SgStatement *stmt)
{
    fragment_list *curfr;
    fragment_list_in *fr;
    int max_dlevel, max_elevel, is_max, d_current, e_current, spec_dlevel, spec_elevel;
    //determing maximal level of debugging and performance analyzing
    if (stmt)
        is_max = MaxLevels(stmt, &max_dlevel, &max_elevel);
    else
    {
        is_max = 0;
        max_dlevel = max_elevel = 4;
    }

    // level specified for surrounding fragment
    d_current = cur_fragment ? cur_fragment->dlevel_spec : 0;
    e_current = cur_fragment ? cur_fragment->elevel_spec : 0;

    // searhing fragment in 2 lists
    fr = debug_fragment;
    //looking through the fragment list specified for debugging (-d) in  command line
    while (fr && (fr->N1 > num || fr->N2 < num))
        fr = fr->next;
    if (fr) //fragment with number 'num' is  found (N1 <= num <= N2)
        spec_dlevel = fr->level;
    else
        spec_dlevel = d_current;

    fr = perf_fragment;
    //looking through the fragment list specified for performance analyze (-e) in  command line
    while (fr && (fr->N1 > num || fr->N2 < num))
        fr = fr->next;
    if (fr) //fragment with number 'num' is  found (N1 <= num <= N2)
        spec_elevel = fr->level;
    else
        spec_elevel = e_current;
    level_debug = MinLevel(spec_dlevel, max_dlevel, is_max);
    dvm_debug = level_debug ? 1 : 0;
    perf_analysis = MinLevel(spec_elevel, max_elevel, is_max);
    curfr = new fragment_list;
    curfr->No = num;
    curfr->begin_st = stmt;
    curfr->dlevel = level_debug;
    curfr->elevel = perf_analysis;
    curfr->dlevel_spec = spec_dlevel;
    curfr->elevel_spec = spec_elevel;
    curfr->next = cur_fragment;
    cur_fragment = curfr; 
}

int MinLevel(int level, int max, int is_max)
{
    if (is_max)
        return((level > max) ? max : level);
    else
        return(level);
}

int MaxLevels(SgStatement *stmt,int *max_dlevel,int *max_elevel)
{ SgExpression *el,*ee;
  SgKeywordValExp *kwe;
  int n,is_max;
  *max_dlevel = 4;
  *max_elevel = 4;
  is_max =0;
  for(el=stmt->expr(1); el; el = el->rhs()) {
       ee = el->lhs();
       kwe = isSgKeywordValExp(ee->lhs());   
       if (!strcmp(kwe->value(),"d")) {
            if((ee->rhs()->variant() != INT_VAL) || (n=ee->rhs()->valueInteger()) < 0)
              err("Illegal debug parameter",303,stmt);
            else
	      {*max_dlevel = n; is_max = 1;}
       }
       else if (!strcmp(kwe->value(),"e")) {
            if((ee->rhs()->variant() != INT_VAL)  || (n=ee->rhs()->valueInteger()) < 0)
              err("Illegal debug parameter",303,stmt);
            else
	      {*max_elevel = n; is_max = 1;}
       }
  }
  return(is_max);
}

void EndDebugFragment(int num)
{ if(!cur_fragment || cur_fragment->No != num) return;
  cur_fragment =  cur_fragment->next; 
  level_debug = cur_fragment->dlevel;
  dvm_debug = level_debug ? 1 : 0; 
  perf_analysis = cur_fragment->elevel;
}

SgExpression *PointerArrElem(SgSymbol *p,SgStatement *stdis)
{
 SgExpression *el;
 for (el = stdis->expr(0); el; el = el->rhs())
  if(el->lhs()->symbol() == p)
    return(el->lhs());
 return(NULL);
}

SgExpression *ReverseDim(SgExpression *desc,int rank)
{int i,ind;
SgExpression *e,*de;
 ind = ndvm;
 e = desc->lhs();   
 for(i= rank-1; i>=0; i--){
    de = &(desc->copy());
    if(e) 
       de->lhs()->setLhs(Calculate(&(e->lhs()->copy()+ *new SgValueExp(i))));
    else
       de->setLhs(new SgExprListExp(*new SgValueExp(i+1)));
    doAssignStmt(de);
 }
return(DVM000(ind));
}
/*
SgExpression *DoSubscriptList(SgExpression *are,int ind)
{return(new SgExprListExp(*new SgValueExp(ind)));}
 */

void EndReduction_Task_Region(SgStatement *stmt)
{
     if(!stmt) return;     
   // actualizing of reduction variables
       if(redgrefts)
         ReductionVarsStart(task_red_list);

        if(irgts) {
         // generating call statement:
         //  call strtrd(RedGroupRef)
         doCallAfter(StartRed(redgrefts));

         // generating call statement:
         //  call waitrd(RedGroupRef)
         doCallAfter(WaitRed(redgrefts));
         /*ReductionVarsWait(red_list);*/
	       //if(idebrg){
               // if(dvm_debug)
               //     doAssignStmtAfter( D_CalcRG(DVM000(idebrg)));
               // doAssignStmtAfter( D_DelRG (DVM000(idebrg)));
               // }
         // generating assign statement:
         //  dvm000(i) = delobj(RedGroupRef)
         doCallAfter(DeleteObject_H(redgrefts));
       }
}


void Reduction_Task_Region(SgStatement *stmt)
{SgExpression  *e;
 SgStatement  *st2, *st3;

   irgts=0;
   redgrefts=NULL;
   e=stmt->expr(0);
   if(!e) return;
   task_red_list = e->lhs();
   if(  e->symbol()){
      redgrefts = new SgVarRefExp(e->symbol());
      doIfForReduction(redgrefts,0);
      nloopred++;
      //stcg = doIfForCreateReduction( e->symbol(),nloopcons,0);
      st2 = doIfForCreateReduction( redgrefts->symbol(),nloopred,1);      
      st3 = cur_st;
      cur_st = st2;
      ReductionList(task_red_list,redgrefts,stmt,st2,st2,0);
      cur_st = st3;      
      InsertNewStatementAfter( new SgAssignStmt(*DVM000(ndvm),*new SgValueExp(0)),cur_st,cur_st->controlParent()); 

    } else {
      irgts = ndvm; 
      redgrefts = DVM000(irgts);
      doAssignStmtAfter(CreateReductionGroup());
                  //!!!??? if(debug_regim){
                  //  idebcg = ndvm; 
                  //  doAssignStmtAfter( D_CreateDebRedGroup());
                  //}
      
      ReductionList(task_red_list,redgrefts,stmt,cur_st,cur_st,0); 
    }  
}


int NumberOfElements(SgSymbol *sym, SgStatement *stmt, int err)
{int i,rank,nm;
 SgExpression *esize,*numb,*pe;
 SgArrayType *artype;
 SgValueExp c1(1);
 SgSubscriptExp *sbe;
 artype=isSgArrayType(sym->type());
 if(artype)
   rank = artype->dimension();//array
 else
    return(1);  //scalar variable
 numb = &c1;
 for(i=1; i<=rank; i++) { //array
    //calculating size of i-th dimension
    pe = artype->sizeInDim(i-1);
    if ((sbe=isSgSubscriptExp(pe)) != NULL){ // [lbound] : [ubound]

      if(err && !sbe->ubound()){ // [lbound] :
         Error("Assumed-shape or deffered-shape array: %s",sym->identifier(), 295, stmt);
         esize = &(pe->copy()); 
      }
      else if(err && sbe->ubound()->variant() == STAR_RANGE) // ubound = *
	 Error("Assumed-size array: %s",sym->identifier(), 162, stmt);
      
      esize = &(((sbe->ubound())->copy()) - (sbe->lbound() ? (sbe->lbound())->copy() : c1 ) + c1); 
      
    } else { // ubound
      if(err && pe->variant() == STAR_RANGE) // dim=ubound = *
	 Error("Assumed-size array: %s",sym->identifier(), 162, stmt); 
      esize = &(pe->copy());
    }
    if(esize)
      numb = &(*numb * (*esize));    
 }
 numb = ReplaceParameter(numb); 
 if (numb->isInteger()) // calculating length if it is possible
    nm = numb->valueInteger(); 
 else 
   { Error("Can't calculate array length: %s",sym->identifier(),194,stmt); 
     nm = 1;
     if(err == 2) nm=0;
   }
 return(nm);
 }


SgExpression * HeapIndex(SgStatement *st)
{SgSymbol *s;
 SgExpression *e;
 SgArrayType *artype;
 int rank;
 s = st->expr(0)->symbol();
 artype=isSgArrayType(s->type());
 if(!artype)
   return(new SgValueExp(POINTER_INDEX(s))); 
 
 rank = artype->dimension();
 
 if(rank == 1) {
   e =&(*new SgValueExp(POINTER_INDEX(s)) + (*st->expr(0)->lhs()->lhs() - *LowerBoundOfDimension(artype,0))* ( *new SgValueExp(HEADER_SIZE(s))));
   return(e);
 }
 return(new SgValueExp(POINTER_INDEX(s)));
}
 
SgExpression * LowerBoundOfDimension(SgArrayType *artype, int i)
{ SgExpression *e,*eb;
  SgSubscriptExp *sbe;
  e = artype->sizeInDim(i);
  if(!e) // pointer declaration error 
    return(new SgValueExp(1)); 
  if((sbe=isSgSubscriptExp(e)) != NULL)
    eb = & (sbe->lbound()->copy());
  else
    eb = new SgValueExp(1);  // by default lower bound = 1  
  return(eb);
}



SgExpression *AsyncArrayElement(SgExpression *asc, SgExpression *ei)
{SgArrayRefExp *e;
    e = new SgArrayRefExp(*ORIGINAL_SYMBOL(asc->symbol()),*ei);
    if(asc->lhs())
      e->addSubscript(asc->lhs()->copy());
    return(e);
}

void AsyncCopyWait(SgExpression * asc)
{SgForStmt *dost;
 SgStatement *as,*st;
 SgExpression *eas;
 SgLabel *loop_lab;
 int i;
 st = cur_st;
 
 //doAssignTo_After(ARRAY_ELEMENT(asc,1),new SgValueExp(1));
 doAssignTo_After(AsyncArrayElement(asc,new SgValueExp(1)),new SgValueExp(1));
 nio = (nio <1) ? 1 : nio;  
 //eas = new SgArrayRefExp(*asc,*new SgVarRefExp(*loop_var[0]));
 eas = AsyncArrayElement(asc, new SgVarRefExp(*loop_var[0]));
 i = ndvm++;
 loop_lab = GetLabel();
 as = new SgAssignStmt(*DVM000(i),*WaitCopy(eas));
 //dost= new SgForStmt(loop_var[0], new SgValueExp(2), ARRAY_ELEMENT(asc,1), new SgValueExp(1), as);
 dost= new SgForStmt(loop_var[0], new SgValueExp(2), AsyncArrayElement(asc,new SgValueExp(1)), new SgValueExp(1), as);
 BIF_LABEL_USE(dost->thebif) = loop_lab->thelabel;
 as->setLabel(*loop_lab); 
 InsertNewStatementAfter(dost, st, st->controlParent());
 as->lexNext()->extractStmt();
 cur_st = as;
 
 SET_DVM(i);
}

int DistrArrayAssign(SgStatement *stmt)
{SgExpression *le,*re,*headl,*headr;
 int to_init,rl,from_init,rr,dvm_ind,left_whole,right_whole;
 SgSymbol *ar;
 SgType *typel,*typer;
  
 re = stmt->expr(1);
 le = stmt->expr(0);
 if(!isSgArrayRefExp(le)) 
     return(0);
 if(!isSgArrayType(le->type()))
     return(0);
 if(isSgArrayType(re->type()))
   if(!isSgArrayRefExp(re))
     return(0);
   else
 // assignment statement of kind: <dvm_array_section> = <array_section>
   { 
     if(only_debug)
       return(1);
     left_whole  = !le->lhs();
     right_whole = !re->lhs();
     ChangeDistArrayRef(le->lhs());   //replacing dvm-array references in subscript list
     ChangeDistArrayRef(re->lhs());
     LINE_NUMBER_BEFORE(stmt,stmt);
     cur_st = stmt;
     dvm_ind = 0;
     ar = le->symbol();
     rl = Rank(ar);
     typel = ar->type()->baseType();
     headl = HeaderRef(ar);
     
     SgExpression *left_section_list = ArraySection(le,ar,rl,stmt,to_init);
     ar = re->symbol();
     typer = ar->type()->baseType();
     if(!CompareTypes(typel,typer))
        err("Different types of left and right side",620,stmt);
     rr = Rank(ar);
     headr = HeaderRef(ar);
     if(!headr)
     {         //Warning("'%s' isn't distributed array", ar->identifier(), 72,stmt);
      /*
        if(re->lhs())  // section
        { dvm_ind = HeaderForNonDvmArray(ar,stmt);
          headr = DVM000(dvm_ind);
        } else         // whole array
          headr = FirstElementOfSection(re);
      */ 
        dvm_ind = HeaderForNonDvmArray(ar,stmt);
        headr = DVM000(dvm_ind); 
     }
     SgExpression *right_section_list  = ArraySection(re,ar,rr,stmt,from_init);  
     if(INTERFACE_RTS2)
     {  
        if(left_whole && right_whole)  // whole-array = whole-array
           doCallAfter(DvmhArrayCopyWhole(headr,headl));
        else
           doCallAfter(DvmhArrayCopy(headr,rr,right_section_list,headl,rl,left_section_list));
     }
     else
        doAssignStmtAfter(ArrayCopy(headr, from_init, from_init+rr, from_init+2*rr, headl, to_init, to_init+rl, to_init+2*rl, 0));
     if(dvm_ind)
        doCallAfter(DeleteObject_H(DVM000(dvm_ind)));
     SET_DVM(to_init);    
     return(1);
   }
  
 // assignment statement of kind: <dvm_array_section> = <scalar_expression>
 if(only_debug)
     return(1);
 if(INTERFACE_RTS2)
     err("Illegal array copy statement in -Opl2 regim", 642, stmt);

 ChangeDistArrayRef(stmt->expr(0)->lhs());   //replacing dvm-array references in subscript list
 ChangeDistArrayRef(stmt->expr(1));

 LINE_NUMBER_BEFORE(stmt,stmt);
 cur_st = stmt;
 ar = le->symbol();
 rl = Rank(ar);
 headl = HeaderRef(ar);
 typel = ar->type()->baseType();
 headr = TypeFunction(typel,re,KINDFunction(new SgArrayRefExp(*baseMemory(ar->type()->baseType()))));
 SgExpression *left_section_list  = ArraySection(le,ar,rl,stmt,to_init);  
 doAssignStmtAfter(ArrayCopy(headr, to_init, to_init, to_init, headl, to_init, to_init+rl, to_init+2*rl, -1));
 SET_DVM(to_init);
 return(1);
}

int AssignDistrArray(SgStatement *stmt)
{SgExpression *le,*re,*headl,*headr;
 int to_init,rl,from_init,rr,dvm_ind,left_whole,right_whole;
 SgSymbol *ar;
 SgType *typel,*typer; 
    re = stmt->expr(1);
    le = stmt->expr(0);
    if(!isSgArrayRefExp(le) || !isSgArrayType(le->type())) 
      return(0);
    if(!isSgArrayRefExp(re) || !isSgArrayType(re->type()) || !IS_DVM_ARRAY(re->symbol()))
      return(0);

 // assignment statement of kind: <array_section> = <dvm_array_section>
    if(only_debug)
       return(1);
     left_whole  = !le->lhs();
     right_whole = !re->lhs();

     ChangeDistArrayRef(stmt->expr(0)->lhs());   //replacing dvm-array references in subscript list
     ChangeDistArrayRef(stmt->expr(1)->lhs());
     
     LINE_NUMBER_BEFORE(stmt,stmt); //LINE_NUMBER_AFTER(stmt,stmt);
     cur_st = stmt;
     ar = le->symbol();
     typel = ar->type()->baseType();
                      //Warning("'%s' isn't distributed array", ar->identifier(), 72,stmt);
     rl = Rank(ar);
   /*
     if(le->lhs())  // section
     { dvm_ind = HeaderForNonDvmArray(ar,stmt);
       headl = DVM000(dvm_ind);
     } else         // whole array
     { dvm_ind = 0; 
       headl = FirstElementOfSection(le); 
     }
   */
     dvm_ind = HeaderForNonDvmArray(ar,stmt);
     headl = DVM000(dvm_ind);
     SgExpression *left_section_list  = ArraySection(le,ar,rl,stmt,to_init);
     ar = re->symbol();
     typer = ar->type()->baseType();
     rr = Rank(ar);
     headr = HeaderRef(ar);
     if(!headr)  // if there is error of dvm-array specification, header is not created
        return(0);
     if(!CompareTypes(typel,typer))
        err("Different types of left and right side",620,stmt);

     SgExpression *right_section_list  = ArraySection(re,ar,rr,stmt,from_init);
     if(INTERFACE_RTS2)
     {  
        if(left_whole && right_whole)  // whole-array = whole-array
           doCallAfter(DvmhArrayCopyWhole(headr,headl));
        else
           doCallAfter(DvmhArrayCopy(headr,rr,right_section_list,headl,rl,left_section_list));
     }
     else
        doAssignStmtAfter(ArrayCopy(headr, from_init, from_init+rr, from_init+2*rr, headl, to_init, to_init+rl, to_init+2*rl, 0));   

     if(dvm_ind)
        doCallAfter(DeleteObject_H(DVM000(dvm_ind)));
        
     SET_DVM(dvm_ind ? dvm_ind : to_init) ;   //SET_DVM(to_init);
     return(1);
}

SgExpression *ArraySection(SgExpression *are, SgSymbol *ar, int rank, SgStatement *stmt, int &init)
{
 SgExpression *el,*einit[MAX_DIMS],*elast[MAX_DIMS],*estep[MAX_DIMS];
 SgExpression *section_list = NULL;
 int i,j;
 init = ndvm;
 if(!are->lhs()) { //MakeSection(are); // A => A(:,:, ...,:)
   if(INTERFACE_RTS2)
      MakeSection(are);  // A => A(:,:, ...,:)
   else {
      for(j=rank; j; j--)
         doAssignStmtAfter(Calculate(new SgValueExp(-1)));
      ndvm += 2*rank;
      return (section_list);//return(init);
   }
 }
 if(!TestMaxDims(are->lhs(),ar,stmt)) return(0);
 for(el=are->lhs(),i=0; el; el=el->rhs(),i++)    
    Triplet(el->lhs(),ar,i, einit,elast,estep); 
 if(i != rank){
    Error("Wrong number of subscripts specified for '%s'",ar->identifier(),140 ,stmt);
    //return (0);
 }
 if(INTERFACE_RTS2) 
    for(j=0; j<i; j++) //reversing dimensions for LibDVM
    {
       section_list = AddElementToList(section_list, DvmType_Ref(estep[j]));
       section_list = AddElementToList(section_list, DvmType_Ref(elast[j]));
       section_list = AddElementToList(section_list, DvmType_Ref(einit[j]));
    }
 else {
    for(j=i; j; j--)
       doAssignStmtAfter(Calculate(einit[j-1])); 
    for(j=i; j; j--)
       doAssignStmtAfter(Calculate(elast[j-1])); 
    for(j=i; j; j--)
       doAssignStmtAfter(estep[j-1]);
 }  
 return (section_list); //return(init);
}

void AsynchronousCopy(SgStatement *stmt)
{SgExpression *le,*re,*el,*einit[MAX_DIMS],*elast[MAX_DIMS],*estep[MAX_DIMS],*headl,*headr,*flag,*ec;
 int j,i,from_init,to_init,rl,rr;
 SgSymbol *ar,*ar1;
 SgType *typel,*typer;
 if(!async_id)
     return;
 LINE_NUMBER_AFTER(stmt,stmt); 
 ec =  AsyncArrayElement(async_id, new SgValueExp(1));
 //flag = new SgArrayRefExp(*async_id, *ec);
 flag = AsyncArrayElement(async_id, ec);
 doAssignTo_After(ec, &(*ec + (*new SgValueExp(1))));

 re = stmt->expr(1);
 if(!isSgArrayRefExp(re)) {
     err("Syntax error",14,stmt);
     return;
 }
 
 ar = re->symbol();
 typer = ar->type()->baseType();
 ar1=ar;
 rr = Rank(ar);
 headr = HeaderRef(ar);
 if(!TestMaxDims(re->lhs(),ar,stmt)) return;
 if(!re->lhs()) MakeSection(re); // A => A(:,:, ...,:)
 for(el=re->lhs(),i=0; el; el=el->rhs(),i++)    
    Triplet(el->lhs(),ar,i, einit,elast,estep);
 if(i != rr){
    Error("Wrong number of subscripts specified for '%s'",ar->identifier(),140 ,stmt);
    return;
 }
 from_init = ndvm;
 for(j=i; j; j--)
      doAssignStmtAfter(Calculate(einit[j-1])); 
 for(j=i; j; j--)
      doAssignStmtAfter(Calculate(elast[j-1])); 
 for(j=i; j; j--)
      doAssignStmtAfter(estep[j-1]); 

 le = stmt->expr(0);
 if(!isSgArrayRefExp(le)) {
     err("Syntax error",14,stmt);
     return;
 }
 ar = le->symbol();
 rl = Rank(ar);
 typel = ar->type()->baseType();
 if(!CompareTypes(typel,typer))
    err("Different types of left and right side",620,stmt);
 headl = HeaderRef(ar);
 if(!TestMaxDims(le->lhs(),ar,stmt)) return;
 if(!le->lhs()) MakeSection(le); // A => A(:,:, ...,:)
 for(el=le->lhs(),i=0; el; el=el->rhs(),i++)    
    Triplet(el->lhs(),ar,i, einit,elast,estep);
 if(i != rl){
    Error("Wrong number of subscripts specified for '%s'",ar->identifier(),140 ,stmt);
    return;
 }
 to_init = ndvm;
 for(j=i; j; j--)
      doAssignStmtAfter(Calculate(einit[j-1])); 
 for(j=i; j; j--)
      doAssignStmtAfter(Calculate(elast[j-1])); 
 for(j=i; j; j--)
      doAssignStmtAfter(estep[j-1]); 

 if(!headr && !headl) {
     err("Both arrays  are not distributed", 297,stmt); 
     return;
 } else if(!headr) {
     Warning("'%s' isn't distributed array", ar1->identifier(), 72,stmt);
     headr = FirstElementOfSection(re); 
 } else if(!headl) {
     Warning("'%s' isn't distributed array", ar->identifier(), 72,stmt);
     headl = FirstElementOfSection(le);
 }

 doAssignStmtAfter(AsyncArrayCopy(headr, from_init, from_init+rr, from_init+2*rr, headl, to_init, to_init+rl, to_init+2*rl, 0, flag));

 SET_DVM(from_init);
}

void Triplet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[],SgExpression *elast[],SgExpression *estep[])
{SgValueExp c1(1),c0(0);

  if(e->variant() != DDOT) { //is not triplet
      einit[i] =  INTERFACE_RTS2 ? e : &(*e-*Exprn(LowerBound(ar,i)));
      elast[i] =  einit[i];
      estep[i] =  &c1.copy();
      return;
  }
  // is triplet

  if(e->lhs() && e->lhs()->variant() == DDOT) { // there is step
      estep[i] = e->rhs();
      e  = e->lhs();    
  } else
      estep[i] =  &c1.copy();
  if (!e->lhs()) 
      einit[i] =  INTERFACE_RTS2 ? ConstRef_F95(-2147483648) : &c0.copy();
  else
      einit[i] =  INTERFACE_RTS2 ? e->lhs() : &(*(e->lhs())-*Exprn(LowerBound(ar,i)));
  if (!e->rhs())
      elast[i] =  INTERFACE_RTS2 ? ConstRef_F95(-2147483648) : &(*Exprn(UpperBound(ar,i))-*Exprn(LowerBound(ar,i)));
  else
      elast[i] =  INTERFACE_RTS2 ? e->rhs() : &(*(e->rhs())-*Exprn(LowerBound(ar,i)));   
  
 return;
}

void LowerBoundInTriplet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[])
{
   SgValueExp c1(1),c0(0);
   if(e->variant() != DDOT) { //is not triplet
      einit[i] = &(e->copy());
      return;
   }
   // is triplet
   if(e->lhs() && e->lhs()->variant() == DDOT)  // there is step
      e = e->lhs(); 
   e = e->lhs();   
   if (!e) 
    einit[i] = Exprn(LowerBound(ar,i)); //new SgValueExp(1);
   else
    einit[i] = &(e->copy());
   return;
}
                                              

void UpperBoundInTriplet(SgExpression *e,SgSymbol *ar,int i, SgExpression *einit[])
{
   //SgValueExp c1(1),c0(0);
   if(e->variant() != DDOT) { //is not triplet
      einit[i] = &(e->copy());
      return;
   }
   // is triplet
   if(e->lhs() && e->lhs()->variant() == DDOT)  // there is step
      e = e->lhs(); 
   e = e->rhs();   
   if (!e) 
    einit[i] = Exprn(UpperBound(ar,i)); 
   else
    einit[i] = &(e->copy());
   return;
}
                                              
 
int doSectionIndex(SgExpression *esec, SgSymbol *ar, SgStatement *st, int idv[], int ileft, SgExpression *lrec[], SgExpression *rrec[])
{int i, j, rank, isec, ilow, ihi;
 SgExpression *el,*einit[MAX_DIMS],*elast[MAX_DIMS],*estep[MAX_DIMS];
 SgValueExp cM1(-1);
 rank = Rank(ar);
 isec = ndvm;
 for(j=rank; j; j--)
   doAssignStmtAfter(&cM1); 
 if(! esec->lhs()) { //no array section
   idv[0] = isec;
   idv[1] = idv[0];
 } else {
   if(!TestMaxDims(esec->lhs(),ar,st)) return (0);
   for(el=esec->lhs(),i=0; el; el=el->rhs(),i++) //looking through the section index list  
      Triplet(el->lhs(),ar,i, einit,elast,estep);
   if(i != rank){
      Error("Wrong number of subscripts specified for '%s'",ar->identifier(),140 ,st);
      return(0);
   }
 
   for(j=i; j; j--)
      doAssignStmtAfter(Calculate(einit[j-1])); 
   for(j=i; j; j--)
      doAssignStmtAfter(Calculate(elast[j-1])); 

   idv[0] = isec+rank;
   idv[1] = isec+2*rank;
 }
 if(!esec->rhs()){
    idv[2] = isec;
    idv[3] = ileft; 
    idv[4] = isec;
    idv[5] = ileft+rank;
    return(1); 
 }
 ilow=ndvm;
 if(!esec->rhs()->lhs()) {//no low shadow section
   idv[2] = isec;
   idv[3] = ileft; 
 } else {
   if(!TestMaxDims(esec->rhs()->lhs(),ar,st)) return (0); 
   for(el=esec->rhs()->lhs(),i=0; el; el=el->rhs(),i++)//looking through the section index list  
      ShadowSectionTriplet(el->lhs(), i, einit,elast,estep,lrec,rrec,0);
   if(i != rank){
      Error("Wrong number of subscripts specified for '%s'",ar->identifier(),140 ,st);
      return(0);
   }
 
   for(j=i; j; j--)
      doAssignStmtAfter(Calculate(einit[j-1])); 
   for(j=i; j; j--)
      doAssignStmtAfter(Calculate(elast[j-1])); 

   idv[2] = ilow;
   idv[3] = ilow+rank;
 }
 ihi=ndvm;
 if(!esec->rhs()->rhs()) {//no high shadow section
   idv[4] = isec;
   idv[5] = ileft+rank; 
 } else { 
   if(!TestMaxDims(esec->rhs()->rhs(),ar,st)) return (0);
   for(el=esec->rhs()->rhs(),i=0; el; el=el->rhs(),i++)//looking through the section index list  
      ShadowSectionTriplet(el->lhs(), i, einit,elast,estep,lrec,rrec,1);
   if(i != rank){
      Error("Wrong number of subscripts specified for '%s'",ar->identifier(),140 ,st);
      return(0);
   }
 
   for(j=i; j; j--)
      doAssignStmtAfter(Calculate(einit[j-1])); 
   for(j=i; j; j--)
      doAssignStmtAfter(Calculate(elast[j-1])); 

   idv[4] = ihi;
   idv[5] = ihi+rank;
 } 
 return(1);
}

void ShadowSectionTriplet(SgExpression *e, int i, SgExpression *einit[], SgExpression *elast[], SgExpression *estep[],  SgExpression *lrec[], SgExpression *rrec[], int flag)
{SgValueExp c1(1),c0(0),cM1(-1);

  if(e->variant() != DDOT) { //is not triplet
      einit[i] = &(*e-c1.copy());
      elast[i] =  einit[i];
      estep[i] = &c1.copy();
      return; 
  } 
  // is triplet

  if(e->lhs() && e->lhs()->variant() == DDOT) { // there is step
      estep[i] = e->rhs();
      e  = e->lhs();    
  } else
      estep[i] = &c1.copy();

  if(!e->lhs() && !e->rhs()) {
      einit[i] =  &cM1.copy();
      elast[i] = (flag == 0 )? lrec[i] : rrec[i];
      return;
  }
  if(!e->lhs())
      einit[i] =  &c0.copy();
  else
      einit[i] =  &(*(e->lhs())- c1.copy());
  if (!e->rhs())
      elast[i] =   &(((flag == 0 )? *lrec[i] : *rrec[i]) -  c1.copy());
  else
      elast[i] =  &(*(e->rhs()) - c1.copy());
  
 return;
}

void DeleteShadowGroups(SgStatement *stmt)
{  group_name_list *sl;
   //int i;
   //i=0;
   for(sl=grname; sl; sl=sl->next)
                      //if(!IS_SAVE(sl->symb))    /*podd 18.09.07*/  
       if (sl->symb->variant() == SHADOW_GROUP_NAME){
          //if(i == 0)
	    //{ LINE_NUMBER_BEFORE(stmt,stmt);}
          //i++;
          doIfForDelete(sl->symb,stmt);  
       }             
}  

void DeleteLocTemplate(SgStatement *stmt)
{symb_list *sl;
 SgExpression *e;
 //if(loc_templ_symb)
   //{ LINE_NUMBER_BEFORE(stmt,stmt);}
 for(sl=loc_templ_symb; sl; sl=sl->next){
    e =  HeaderRef(sl->symb);
    if(e)
      InsertNewStatementBefore(DeleteObject_H(e),stmt);
 }         
}

void RegistrationList(SgStatement *stmt)
{ SgExpression *el;
  SgSymbol * s;
  int is_assign;
    is_assign =0;           
    for(el=stmt->expr(0); el; el=el->rhs()) {
        if(el->lhs()->variant() == ASSGN_OP || el->lhs()->variant() == POINTST_OP) is_assign = 1;//with initial value
        s = el->lhs()->symbol();
        if(debug_regim && s && IS_ARRAY(s)) 
             registration = AddNewToSymbList( registration, s);        
    }
    if(is_assign && stmt->variant() == VAR_DECL && !stmt->expr(2))
       stmt->setVariant(VAR_DECL_90);
    return;         
}        

SgExpression *DebReductionGroup(SgSymbol *gs)
{
    SgSymbol *rgv;
    SgExpression *rgvref;
    rgv = * ((SgSymbol **) (ORIGINAL_SYMBOL(gs)) -> attributeValue(0,RED_GROUP_VAR));
    rgvref = new SgArrayRefExp(*rgv,*new SgValueExp(0)); 
    return(rgvref);
}

void EndOfProgramUnit(SgStatement *stmt, SgStatement *func, int begin_block)
{
              if(func->variant() == PROG_HEDR) {  // for MAIN program
                if(begin_block)
                  EndBlock_H(stmt);           
                RTLExit(stmt);
              }              
              else if (func->variant() == PROC_HEDR || func->variant() == FUNC_HEDR) {
                SgStatement *stat = stmt;                                
                if(begin_block)
                  stat = EndBlock_H(stmt);                
                else
                  DeleteShadowGroups(stmt);
                if(loc_templ_symb) 
                  DeleteLocTemplate(stmt);
                acc_return_list = addToStmtList(acc_return_list,stat); //save the point to insert RTSH-calls:dvmh_data_exit                
              }
}
void InitBaseCoeffs()
{
   if(opt_base && !HPF_program && dsym) {
     symb_list *sl;
     coeffs * c;
     SgExpression *e,*el;
     SgType *t;
     for(sl=dsym; sl; sl=sl->next) {
     c = AR_COEFFICIENTS(sl->symb); //((coeffs *) sl->symb-> attributeValue(0,ARRAY_COEF));
     if(!c->use) 
       continue;
     e = new SgVarRefExp(*(c->sc[1]));
     t = sl->symb->type()->baseType();
     el = &((*GetAddresMem( new SgArrayRefExp(*baseMemory(t),*new SgValueExp(0))) - *GetAddresMem( new SgArrayRefExp(**ARRAY_BASE_SYMBOL(sl->symb),*new SgValueExp(0)))) / *new SgValueExp(TypeSize(t)));
     
     doAssignTo_After(e, el); 
     //     rank=Rank(sl->symb);
     //for(i=1;i<=rank;i++){
     //  eel = new SgExprListExp(* new SgVarRefExp(*(c->sc[1])));
     }
   }
}

void CreateIndexVariables(SgExpression *dol)
{SgExpression *dovar;
// looking through the do_variables list
 for(dovar=dol; dovar; dovar=dovar->rhs())
   if(!(INDEX_SYMBOL(dovar->lhs()->symbol()))){
        SgSymbol **s = new (SgSymbol *);
        //creating new variable
        *s = IndexSymbol(dovar->lhs()->symbol());
       // adding the attribute (INDEX_DELTA) to do-variable symbol
        (dovar->lhs()->symbol())->addAttribute(INDEX_DELTA, (void*) s, sizeof(SgSymbol *)); 
        index_symb = AddToSymbList(index_symb,*s);
   }    
}

void  doAssignIndexVar(SgExpression *dol,int iout, SgExpression *init[])
{SgExpression *dovar;
 int i;
// looking through the do_variables list
 for(dovar=dol,i=0; dovar; dovar=dovar->rhs(),i++){
   if(INDEX_SYMBOL(dovar->lhs()->symbol())) 
    doAssignTo_After(new SgVarRefExp(*INDEX_SYMBOL(dovar->lhs()->symbol())),&(*DVM000(iout+i) - init[i]->copy())); 
}
}

SgExpression *TestDVMArrayRef(SgExpression *e)
{SgExpression *dovar, *vl, *ei, *el, *coeff, *cons, *eop;
 SgSymbol *dim_ident[MAX_DIMS];
 int i,j,k,n,num,use[MAX_DIMS],is;
 sum_dvm = NULL;
 is = isInSymbList(dvm_ar,e->symbol());
    
 if(!HEADER(e->symbol())) return(NULL);
 n = Rank(e->symbol());
 sum_dvm = coef_ref(e->symbol(),n+2);
 vl = parallel_dir->expr(2); // do_variables list of PARALLEL directive
 for(dovar=vl,i=0; dovar; dovar=dovar->rhs(),i++){
   dim_ident[i] = dovar->lhs()->symbol();
   //fprintf(stderr,"%s\n",dovar->lhs()->symbol()->identifier());
   use[i] = 0;
 }
 //fprintf(stderr,"%d\n",i);
 for(el=e->lhs(),k=n+1;el;el=el->rhs(),k--){
   //fprintf(stderr,"%d\n",k);
   for(j=0;j<i;j++)
     use[j] = 0;
   num=AxisNumOfDummyInExpr(el->lhs(),dim_ident,i,&ei,use,NULL);
   //fprintf(stderr,"num%d\n",num);
   if(num<0){
    Warning("Maybe incorrect subscript of DVM-array reference: %s",e->symbol()->identifier(),332,cur_st);
    return(NULL);
   }
   if(num == 0) continue;
   CoeffConst(el->lhs(),ei,&coeff,&cons);
   if(!coeff){
      Warning("Maybe incorrect subscript of DVM-array reference: %s",e->symbol()->identifier(),332,cur_st);
      return(NULL);
   }
   eop = new SgVarRefExp(*INDEX_SYMBOL(dim_ident[num-1]));
  
   if(k!=(n+1)){
     eop = &((*coef_ref(e->symbol(),k))* (*eop));
     //  fprintf(stderr,"%d\n",k);
   }
   if(coeff->isInteger() && coeff->valueInteger() == 1)
     {;} 
   else
     eop = &((coeff->copy()) *(*eop));  
   sum_dvm = &(*sum_dvm + (*eop) );

 }
 //do_var=isDoVarUse(es->lhs(),use,dim_ident,i,&num,par_st)
 //*num =  AxisNumOfDummyInExpr(e, ident, ni, &ei, use, cur_st);
 //if (*num<=0) 
 //  return(NULL);
 //return(ei);
 //sum_dvm->unparsestdout();
 //eop->unparsestdout();
 //fprintf(stderr,"%s%d\n",e->symbol()->identifier(),k);

   if(!is) ChangeArrayCoeff(e->symbol());
 return(sum_dvm);
}


void ChangeIndexRefBySum(SgExpression *ve)
{
 SgSymbol *is,*s;
 is = *INDEX_SYMBOL(ve->symbol());
 s = ve->symbol();
 NODE_CODE(ve->thellnd) = ADD_OP;
 //ve->setVariant(ADD_OP);
 ve->setLhs(*new SgVarRefExp(*s));
 //ve->setLhs(ve->copy());
 //ve->setLhs(*new SgValueExp(1));
 ve->setRhs(*new SgVarRefExp(is));
 ve->setSymbol((SgSymbol*) NULL);
 //NODE_SYMB(ve->thellnd) = NULL;  
}

void ChangeArrayCoeff(SgSymbol *ar)
{

 InsertNewStatementBefore(new SgAssignStmt(*coef_ref(ar,0),*sum_dvm),first_do_par);

}


SgSymbol *CreateInitLoopVar(SgSymbol *dovar, SgSymbol *init)
{   
   if(INIT_LOOP_VAR(dovar)) 
      return( *INIT_LOOP_VAR(dovar));
   else {
        SgSymbol **s = new (SgSymbol *);
        //creating new variable
        *s = InitLoopSymbol(dovar,init->type());
       // adding the attribute (INIT_LOOP) to do-variable symbol
        dovar->addAttribute(INIT_LOOP, (void*) s, sizeof(SgSymbol *)); 
        index_symb = AddToSymbList(index_symb,*s);
        return(*s);
   } 
}


void ConsistentArrayList  (SgExpression *el,SgExpression *gref, SgStatement *st, SgStatement *stmt1, SgStatement *stmt2)
{ SgStatement *last,*last1;
  SgExpression  *er, *ev, *header = NULL,*size_array;
  int  nr, ia=-1, sign, re_sign,renew_sign,iaxis,rank;
  SgSymbol *var; 
//  SgValueExp c0(0),c1(1);
  last = stmt2; last1 = stmt1;
  //looking through the consistent array list
  for(er = el; er; er=er->rhs()) {
     ev = er->lhs(); // consistent array reference
     var = ev->symbol();

   /*  if(st->variant() == DVM_CONSISTENT_GROUP_DIR){
       red_group_var_list=AddToSymbList(red_group_var_list,var);
       if(loc_var->symbol())
         red_group_var_list =AddToSymbList(red_group_var_list,loc_var->symbol());                  
     }    
     else{
        new_red_var_list=AddToSymbList(new_red_var_list,var);
       if(loc_var->symbol())
         new_red_var_list =AddToSymbList(new_red_var_list,loc_var->symbol()); 
     }
    */
  
     if(var) 
       ia = var->attributes();
    
     if( isSgArrayRefExp(ev)) {
           
         if((ia & DISTRIBUTE_BIT) ||(ia & ALIGN_BIT)|| (ia & INHERIT_BIT))      //06.12.12
         {   Error("Illegal object '%s' in CONSISTENT clause ", var->identifier(), 399,st);
                                      //  Error("'%s' is distributed array", var->identifier(), 148,st);
             continue;
         }                              
         else if(!(ia & CONSISTENT_BIT) )                                      // 06.12.12    && !(ia & DISTRIBUTE_BIT) && !(ia & ALIGN_BIT) && !(ia & INHERIT_BIT)){
         {   Error("Illegal object '%s' in CONSISTENT clause ", var->identifier(), 399,st);             
             continue;
         }
         
     } else {
        err("Illegal object in CONSISTENT clause ", 399,st);
                    //err("Wrong consistent array",151,st); //??? error number 
        continue;
     }

     if(stmt1 != stmt2) 
       cur_st = last1;

     if(!only_debug) {
        header = new SgArrayRefExp(*(CONSISTENT_HEADER(var)),*new SgValueExp(1));   //HeaderRef(var);
        rank = Rank(var);
        if(IN_COMPUTE_REGION || inparloop && parloop_by_handler)    /*ACC*/
        { int i;
          for(i=0;i<rank;i++) 
             doAssignTo_After(header_ref(header->symbol(),rank+3+i) , Exprn( LowerBound(var,i))) ;     
        }
        size_array = DVM000(ndvm);

        sign = 1;
        re_sign = 0;  // aligned array may not be redisributed 

       // call crtraf (ArrayHeader,ExtHdrSign,Base,Rank,TypeSize,SizeArray, StaticSign, ReDistrSign, Memory)  

       doCallAfter(CreateDvmArrayHeader(var, header, size_array, rank, sign, re_sign)); 
       where = cur_st;
       doSizeFunctionArray(var,st); 
       cur_st = where;
     }

     //if(debug_regim) {
     //  debgref = idebrg ? DVM000(idebrg) : DebReductionGroup(gref->symbol());
     //  doAssignStmtAfter(D_InsRedVar(debgref,num_red,ev,ntype,ilen, loc_var, ilen+1,locindtype));
     //}

     last1 = cur_st;

     if(stmt1 != stmt2) 
         cur_st = last;
     renew_sign = 0; //????
     if(!only_debug){
       iaxis = ndvm;
       //insert  array into consistent group
       if(st->variant() == DVM_TASK_REGION_DIR){
          doAxisTask(st,ev);
            //doAssignStmtAfter(IncludeConsistentTask(gref,header,DVM000(PS_INDEX(st->symbol())),iaxis,re_sign));  
          doAssignStmtAfter(IncludeConsistentTask(gref,header,new SgVarRefExp(TASK_SYMBOL(st->symbol())),iaxis,re_sign));  

       }
       else {//DVM_PARALLEL_ON_DIR
         nr = doAlignIteration(st, ev);
         doAssignStmtAfter(InsertConsGroup(gref,header,iplp,iaxis, iaxis+nr, iaxis+2*nr,re_sign));     
       }
     }
     last = cur_st;
  }   
  
   return;
}     

void ConsistentArraysStart  (SgExpression *el)
{ 
  SgExpression  *er, *ev; 

  //looking through the consistent array list
  for(er = el; er; er=er->rhs()) {
     ev = er->lhs();    //  consistent array reference
               
        if(isSgArrayRefExp(ev) && !IS_DVM_ARRAY(ev->symbol())) {        
             doAssignStmtAfter(GetAddresMem(FirstArrayElement(ev->symbol()))) ;
             FREE_DVM(1);  
        }
  }   
}    

void Consistent_Task_Region(SgStatement *stmt)
{SgExpression  *e;
 SgStatement  *st2, *st3;

   iconsgts=0;
   consgrefts=NULL;
   e=stmt->expr(1);
   if(!e) return;
   task_cons_list = e->lhs();
   if(  e->symbol()){
      consgrefts = new SgVarRefExp(e->symbol());
      doIfForConsistent(consgrefts);
      nloopcons++;
      //stcg = doIfForCreateReduction( e->symbol(),nloopcons,0);
      st2 = doIfForCreateReduction( consgrefts->symbol(),nloopcons,1);
      //stcg = st2;
      st3 = cur_st;
      cur_st = st2;
      ConsistentArrayList(task_cons_list,consgrefts,stmt,st2,st2);
      cur_st = st3;
      InsertNewStatementAfter( new SgAssignStmt(*DVM000(ndvm),*new SgValueExp(0)),cur_st,cur_st->controlParent()); 

    } else {
      iconsgts = ndvm; 
      consgrefts = DVM000(iconsgts);
      doAssignStmtAfter(CreateConsGroup(1,1));
                  //!!!??? if(debug_regim){
                  //  idebcg = ndvm; 
                  //  doAssignStmtAfter( D_CreateDebRedGroup());
                  //}
      //stcg = cur_st;//store current statement
      ConsistentArrayList(task_cons_list,consgrefts,stmt,cur_st,cur_st); 
    }  
}

void EndConsistent_Task_Region(SgStatement *stmt)
{
       if(!stmt) return;     
       //LINE_NUMBER_AFTER(stmt,stmt);
   // actualizing of consistent arrays
       if(consgrefts)
         ConsistentArraysStart(task_cons_list);

       if(!iconsgts) return;

       //there is synchronous CONSISTENT clause in TASK_REGION
         // generating assign statement:
         //  dvm000(i) = strtcg(ConsistGroupRef)
         doAssignStmtAfter(StartConsGroup(consgrefts));

         // generating assign statement:
         //  dvm000(i) = waitcg(ConsistGroupRef)
         doAssignStmtAfter(WaitConsGroup(consgrefts));
         
    	   //if(idebcg){
             //if(dvm_debug)
             //  doAssignStmtAfter( D_CalcRG(DVM000(idebrg)));
             //doAssignStmtAfter( D_DelRG (DVM000(idebrg)));
           //}

         // generating statement:
         //  call dvmh_delete_object(ConsistGroupRef)     //dvm000(i) = delobj(ConsistGroupRef)
         doCallAfter(DeleteObject_H(consgrefts));
}

void doAxisTask(SgStatement *st, SgExpression *eref)
{int i,iaxis=-1;
 SgExpression *el;
 SgSymbol *ar;
 ar = eref->symbol();
 for(el=eref->lhs(),i=0; el; el=el->rhs(),i++)
    if(el->lhs()->variant() !=DDOT)
       iaxis = i;
 if(i != Rank(ar))
   Error("Rank of array '%s' isn't equal to the length of subscript list", ar->identifier(), 161,st);
 doAssignStmtAfter(new SgValueExp(i-iaxis));
 return;
}


void TransBlockData(SgStatement *hedr,SgStatement* &end_of_unit)
{SgStatement* stmt;
 end_of_unit = hedr->lastNodeOfStmt();
 for (stmt = hedr; stmt && (stmt != end_of_unit); stmt = stmt->lexNext()) 
    if(isSgVarDeclStmt(stmt)) VarDeclaration(stmt);
        // analizing object list and replacing variant of declaration statement with initialisation by VAR_DECL_90 
}

void VarDeclaration(SgStatement *stmt)
{ SgExpression *el;
  int is_assign;
    is_assign =0;           
    for(el=stmt->expr(0); el; el=el->rhs()) {
        if(el->lhs()->variant() == ASSGN_OP || el->lhs()->variant() == POINTST_OP) is_assign = 1;//with initial value
    }
    if(is_assign && stmt->variant() == VAR_DECL && !stmt->expr(2))
       stmt->setVariant(VAR_DECL_90);
    return;         
}        

SgExpression *LeftMostField(SgExpression *e)
{SgExpression *ef;
 ef = e;
 while(ef->variant() == RECORD_REF)
   ef = ef->lhs();
 return(ef);
}

SgExpression *RightMostField(SgExpression *e)
{return(e->rhs());}

SgStatement *InterfaceBlock(SgStatement *hedr)
{ SgStatement *stmt;
 in_interface++;
 for(stmt=hedr->lexNext(); stmt->variant()!=CONTROL_END; stmt=stmt->lexNext())
 {
   if(stmt->variant() == FUNC_HEDR || stmt->variant() == PROC_HEDR) //may be module procedure statement
     stmt = InterfaceBody(stmt);
   else if(stmt->variant() != MODULE_PROC_STMT)
     err("Misplaced directive/statement", 103, stmt);     
 }
 //if(stmt->controlParent() != hedr)
 //  Error("Illegal END statement");

 in_interface--;
 return(stmt);
}

SgStatement *InterfaceBody(SgStatement *hedr)
{ 
 SgStatement *stmt, *last, *dvm_pred;
 symb_list *distsym;
 SgSymbol *s = hedr->symbol();
 distsym = NULL;
 dvm_pred = NULL;
 
 if (hedr->expr(2))
 {
    if (hedr->expr(2)->variant() == PURE_OP)
       SYMB_ATTR(s->thesymb) = SYMB_ATTR(s->thesymb) | PURE_BIT;
   
    else if (hedr->expr(2)->variant() == ELEMENTAL_OP)
       SYMB_ATTR(s->thesymb) = SYMB_ATTR(s->thesymb) | ELEMENTAL_BIT;
 }
 last = hedr->lastNodeOfStmt();
 
 for(stmt=hedr->lexNext(); stmt; stmt=stmt->lexNext()) {
    if(dvm_pred)
       Extract_Stmt(dvm_pred); // deleting preceding DVM-directive
    if(stmt == last) break;   //end of interface body
    dvm_pred = NULL;

    if (!isSgExecutableStatement(stmt)) {//is Fortran specification statement
	
      if(only_debug){
          if(isSgVarDeclStmt(stmt)) VarDeclaration(stmt);// for analizing object list and replacing variant of statement
          continue;
      } 
        //discovering distributed arrays in COMMON-blocks
        if(stmt->variant()==COMM_STAT) {
	
           DeleteShapeSpecDAr(stmt);
           if( !DeleteHeapFromList(stmt) ) { //common list is empty
             stmt=stmt->lexPrev();
             stmt->lexNext()->extractStmt(); //deleting the statement
           } 
          continue; 
	}  
    
        // deleting distributed arrays from variable list of declaration
        // statement and testing are there any group names
        if( isSgVarDeclStmt(stmt) || isSgVarListDeclStmt(stmt)) {
           
           if( !DeleteDArFromList(stmt) ) { //variable list is empty
             stmt=stmt->lexPrev();
             stmt->lexNext()->extractStmt(); //deleting the statement
           }
           continue;
	} 
 
        if(stmt->variant() == STMTFN_STAT) {
	   if(stmt->expr(0) && stmt->expr(0)->symbol() && ((!strcmp(stmt->expr(0)->symbol()->identifier(),"number_of_processors")) || (!strcmp(stmt->expr(0)->symbol()->identifier(),"processors_rank")) || (!strcmp(stmt->expr(0)->symbol()->identifier(),"processors_size")))){
             stmt=stmt->lexPrev();
             stmt->lexNext()->extractStmt(); 
                           //deleting the statement-function declaration named 
	                   //  NUMBER_OF_PROCESSORS or PROCESSORS_RANK or PROCESSORS_SIZE 
           }
           continue; 
        }
   
        if (stmt->variant() == ENTRY_STAT) {  
          warn("ENTRY among specification statements", 81,stmt);  
          continue;
        }

        if(stmt->variant() == INTERFACE_STMT || stmt->variant() == INTERFACE_ASSIGNMENT || stmt->variant() == INTERFACE_OPERATOR){
          stmt=InterfaceBlock(stmt); 
          continue;
	}

	if(stmt->variant() == STRUCT_DECL){
          stmt=stmt->lastNodeOfStmt();
          continue;
        }

        if( stmt->variant() == USE_STMT || stmt->variant() == DATA_DECL)       
          continue;

        continue;             
     } // end of if(!isSgExecutable...
     
    if ((stmt->variant() == FORMAT_STAT))     
         continue;
           
// processing the DVM Specification Directives

    switch(stmt->variant()) {

      case (DVM_VAR_DECL):
          { SgExpression *el;
	    int eda;
	    eda = 0;
            for(el = stmt->expr(2); el; el=el->rhs()) // looking through the attribute list
	      switch(el->lhs()->variant()) {
	          case (ALIGN_OP):
                  case (DISTRIBUTE_OP): 
		     eda = 1; 
		     break;
	          default:
                     break;
	      }
	    if(eda == 0){
              dvm_pred = stmt; 
              continue;
            }
	  }
       case (DVM_INHERIT_DIR): 
       case (DVM_ALIGN_DIR): 
       case (DVM_DISTRIBUTE_DIR):
         { 
           SgExpression *sl; 
           for(sl=stmt->expr(0); sl; sl=sl->rhs())  //scanning the alignees list  
              if(!IS_POINTER(sl->lhs()->symbol()))        
                distsym = AddNewToSymbList(distsym,sl->lhs()->symbol());
         }
           dvm_pred = stmt; 
	   continue;
       case (ACC_ROUTINE_DIR):
           ACC_ROUTINE_Directive(stmt);
           dvm_pred = stmt;  
           continue;

       case (HPF_TEMPLATE_STAT):
       case (HPF_PROCESSORS_STAT):
       case (DVM_DYNAMIC_DIR):
       case (DVM_SHADOW_DIR):  
       case (DVM_TASK_DIR): 
       case (DVM_CONSISTENT_DIR): 
       case (DVM_INDIRECT_GROUP_DIR):
       case (DVM_REMOTE_GROUP_DIR):
       case (DVM_CONSISTENT_GROUP_DIR):
       case (DVM_REDUCTION_GROUP_DIR):
       case (DVM_POINTER_DIR): 
       case (DVM_HEAP_DIR):
       case (DVM_ASYNCID_DIR):
	   dvm_pred = stmt;  
       default:
	   continue; 
    }

    break;
 } //end of loop

 if(!only_debug)
    DeclareVarDVMForInterface(stmt->lexPrev(),distsym);
 return(stmt);
}

void DeleteShapeSpecDAr(SgStatement *stmt)
{
          SgExpression *ec, *el;
          SgSymbol *sc; 
	  for(ec=stmt->expr(0); ec; ec=ec->rhs()) // looking through COMM_LIST
	    for(el=ec->lhs(); el; el=el->rhs()) {  
              sc = el->lhs()->symbol();
              if(sc && ((sc->attributes() & ALIGN_BIT) || (sc->attributes() & DISTRIBUTE_BIT)) )
                 el->lhs()->setLhs(NULL);  
              if(sc && !in_interface) {
                 SYMB_ATTR(sc->thesymb)= SYMB_ATTR(sc->thesymb) | COMMON_BIT;
                 if(debug_regim &&  IS_ARRAY(sc) )
                    registration = AddNewToSymbList( registration, sc); 
              
                 if( !strcmp(sc->identifier(),"heap"))
                    heap_ar_decl = new SgArrayRefExp(*heapdvm); 
              }      
              if(sc && (sc->attributes() & TEMPLATE_BIT))
                 Error("Template '%s' is in COMMON",sc->identifier(),79,stmt);                         
           }  
}

void DeclareVarDVMForInterface(SgStatement *lstat, symb_list *distsymb)
{symb_list *save;
 if(!distsymb) return;
 save = dsym; //save global variable 'dsym' - list of distributed arrays for procedure
 dsym = distsymb;
 DeclareVarDVM(lstat,lstat);
 dsym = save; //resave global variable 'dsym'
}

SgExpression  *DVMVarInitialization(SgExpression *es)
{SgExpression *einit, *er;
 switch(es->symbol()->variant()) { //initialization expression
   case ASYNC_ID:   einit = new SgValueExp(1);  //new SgExpExpression(CONSTRUCTOR_REF); //SgConstExp
                    break;
   default:         einit = new SgValueExp(0);
                    break;
 } 
 er = new SgExpression(ASSGN_OP,es,einit,NULL); 
 return(er);
}

SgExpression  *FileNameInitialization(SgExpression *es,char *name)
{SgExpression *einit, *er;
 einit = new SgExpression(CONCAT_OP,new SgValueExp(name),CHARFunction(0),NULL); 
 er = new SgExpression(ASSGN_OP,es,einit,NULL); 
 return(er);
}

SgStatement *CreateModuleProcedure(SgStatement *mod_hedr, SgStatement *lst, SgStatement* &has_contains)
 { mod_attr *attrmod;
   SgStatement *last;
   SgStatement *st_end ;
   SgStatement *st;
   SgSymbol *smod; 

     attrmod = new mod_attr;
     attrmod->symb = NULL;
     mod_hedr->symbol()->addAttribute(MODULE_STR, (void *) attrmod, sizeof(mod_attr));

     //  if(mod_hedr->lexNext()->variant() != USE_STMT && !dsym && !task_symb && !proc_symb)
     //       return(NULL);

     smod = new SgSymbol(PROCEDURE_NAME, ModuleProcName(mod_hedr->symbol()), *mod_hedr);           
     attrmod->symb = smod;
     st = new SgStatement(PROC_HEDR);
     st->setSymbol(*smod);
     st_end = new SgStatement(CONTROL_END);
     
     if(lst->variant() != CONTAINS_STMT) {
        last = new SgStatement(CONTAINS_STMT);
        lst-> insertStmtBefore(*last);
     } else
        last = lst;
     has_contains = last;
     //last = (lst->variant() == CONTAINS_STMT) ? lst->lexNext() : lst;
     last->insertStmtAfter(*st);
     st->insertStmtAfter(*st_end);
     return(st);
  }
   
void GenForUseStmts(SgStatement *hedr,SgStatement *where_st)
{SgStatement *stmt;
  for(stmt=hedr->lexNext();stmt->variant() == USE_STMT;stmt=stmt->lexNext()){
     GenCallForUSE(stmt,where_st);
 /*  
   if(!(stmt->expr(0)))
       GenCallForUSE(stmt,where_st);
     else if(stmt->expr(0)->variant() == ONLY_NODE)
       GenForUseList(stmt->expr(0)->lhs(),stmt,where_st);
     else {
       GenForUseList(stmt->expr(0),stmt,where_st);
       GenCallForUSE(stmt,where_st);
     }
 */
  }
  
}

void GenForUseList(SgExpression *ul,SgStatement *stmt, SgStatement *where_st)
{SgExpression *el, *e;
 
 for(el=ul; el; el=el->rhs()){
   e = el->lhs();
   if(e->variant() == RENAME_NODE){
      e = e->lhs();    //new symbol reference
   }
   if(!only_debug && IS_DVM_ARRAY(e->symbol()))
     GenDVMArray(e->symbol(),stmt,where_st); 
   if(debug_regim && IS_ARRAY(e->symbol()))
     Registrate_Ar(e->symbol());
 }
}

void GenDVMArray(SgSymbol *ar, SgStatement *stmt, SgStatement *where_st)
{SgStatement *savest;
//SgExpression *dce;
// SgArrayType *artype;
 savest = where;
 where = where_st;
 //generating
 
 /*
 dce = new SgArrayRefExp(*ar);
 artype = isSgArrayType(ar->type());
 dce->setLhs(artype->getDimList()->copy());

  if(ar->attributes() & POINTER_BIT) 
     AllocatePointerHeader(ar,where_st);
 */
 if( IS_POINTER(ar) || (IN_COMMON(ar) &&  (ar->scope()->variant() != PROG_HEDR)) || IS_ALLOCATABLE_POINTER(ar))
	   return;
 if(ar->attributes() & DISTRIBUTE_BIT) {
    //determine corresponding DISTRIBUTE statement
     SgStatement *dist_st = *(DISTRIBUTE_DIRECTIVE(ar));
    //create distributed array
     int idis;
     SgExpression *distr_rule_list = doDisRules(dist_st,0,idis);
     SgExpression *ps = PSReference(dist_st); 
     GenDistArray(ar,idis,distr_rule_list,ps,dist_st);
   }
       
   else if(ar->attributes() & ALIGN_BIT) {
    //create aligned array  
     int nr,iaxis; 
     algn_attr * attr;
     align * root, *node,*node_copy, *root_copy = NULL;
     SgStatement *algn_st;
     SgSymbol *base;
     attr = (algn_attr *)  ORIGINAL_SYMBOL(ar)->attributeValue(0,ALIGN_TREE);
     node = attr->ref; // reference to root of align tree
     node_copy = new align;
     node_copy->symb = ar;
     node_copy->align_stmt = node->align_stmt; 
     algn_st = node->align_stmt;
     if(!algn_st->expr(2)) //postponed aligning
       root = NULL;
     else {
       base = (algn_st->expr(2)->variant()==ARRAY_OP) ? (algn_st->expr(2))->rhs()->symbol() :                                                  (algn_st->expr(2))->symbol();// align_base symbol
       root = ((algn_attr *) ORIGINAL_SYMBOL(base)->attributeValue(0,ALIGN_TREE))->ref;
       root_copy = new align;
       root_copy->symb = Rename(base,stmt);
       root_copy->align_stmt = root->align_stmt;
     }
     iaxis = ndvm;
     SgExpression *align_rule_list = doAlignRules(ar,node->align_stmt,0,nr);// creating axis_array, coeff_array and  const_array
     GenAlignArray(node_copy,root_copy, nr, align_rule_list, iaxis);
     /* AllocateAlignArray(ar,dce,stmt);*/
   }
 loc_distr = 0;
 pointer_in_tree = 0;
 where = savest;
}

SgSymbol *Rename(SgSymbol *ar, SgStatement *stmt)
{SgExpression *el, *e, *eold;

 for(el=stmt->expr(0);el;el=el->rhs()){
    e = el->lhs(); eold = NULL;
   if(e->variant() == RENAME_NODE){
      e = e->lhs();    //new symbol reference
      eold = el->lhs()->rhs(); //old symbol reference
   }
//   if(eold && ORIGINAL_SYMBOL(eold->symbol()) == ORIGINAL_SYMBOL(ar))
   if(eold && !strcmp(eold->symbol()->identifier(),ar->identifier()))
     return(e->symbol());     
 }
 return(ar);
}

void AddAttributeToLastElement(SgExpression *use_list)
{
  SgExpression *el = use_list;
  while(el && el->rhs())
    el = el->rhs();
  el->addAttribute(END_OF_USE_LIST, (void*) 1, 0); 
}

void UpdateUseListWithDvmArrays(SgStatement *use_stmt)
{
  SgExpression *el, *coeff_list=NULL;
  SgExpression *use_list = use_stmt->expr(0);
  SgSymbol *s,*sloc;
  int i,r,i0;
  i0 = opt_base ? 1 : 2;
  if(opt_loop_range) i0=0;
  
  if(use_list && use_list->variant()==ONLY_NODE)
    use_list = use_list->lhs();
  if(use_list)
    AddAttributeToLastElement(use_list);
  for(el=use_list; el; el=el->rhs())
  { 
    // el->lhs()->variant() is RENAME_NODE
    sloc = el->lhs()->lhs()->symbol(); // local symbol
    if(!IS_DVM_ARRAY(sloc)) continue;
    r = Rank(sloc);
    if(el->lhs()->rhs())      // use symbol reference in renaming_op: local_symbol=>use_symbol
    {  
       s = el->lhs()->rhs()->symbol();    //use symbol
       if(strcmp(sloc->identifier(),s->identifier()))  // different names
       {
       // creating variables used for optimisation array references in parallel loop (linearization coefficients)
         coeffs *c_new  = new coeffs;
         CreateCoeffs(c_new,sloc);
       // adding the attribute (ARRAY_COEF) to distributed array symbol
         sloc->addAttribute(ARRAY_COEF, (void*) c_new, sizeof(coeffs));
       // add  renaming_op for all coefficients (2:rank+2) to use_list: coeff_of_sloc=>coeff_of_s         
         coeffs *c_use  =  AR_COEFFICIENTS(s);
         for(i=i0;i<=r+2;i++)
           if(i != r+1)
           {
             SgExpression *rename = new SgExpression(RENAME_NODE, new SgVarRefExp(c_new->sc[i]), new SgVarRefExp(c_use->sc[i]), NULL);
             coeff_list = AddListToList(coeff_list,new SgExprListExp(*rename));
           }         
       }
    } else
    {
       // add cofficients of use_symbol to use_list
       s = el->lhs()->symbol(); //use symbol
       coeffs *c_use  =  AR_COEFFICIENTS(s);
       for(i=i0;i<=r+2;i++)
         if(i != r+1) 
           coeff_list = AddListToList(coeff_list,new SgExprListExp(*new SgVarRefExp(c_use->sc[i])));
    }
  }
    if(coeff_list)
       AddListToList(use_list,coeff_list);
}
       
void updateUseStatementWithOnly(SgStatement *st_use, SgSymbol *s_func)
{ // add name of s_func to only-list of USE statement
  SgExpression *clause = st_use->expr(0);
  if(clause && clause->variant() == ONLY_NODE)
  {
     SgExpression *el = new SgExprListExp(*new SgVarRefExp(s_func));
     if(clause->lhs())  // only-list is not empty
       AddListToList(clause->lhs(), el);
     else
       clause->setLhs(el);
  }
}

void GenCallForUSE(SgStatement *hedr,SgStatement *where_st)
{SgSymbol *smod;
 SgStatement *call;
 mod_attr *attrm;
  smod = hedr->symbol();
  if((attrm=DVM_PROC_IN_MODULE(smod)) && attrm->symb){
       call = new SgCallStmt(*attrm->symb);
       where_st->insertStmtBefore(*call);
       updateUseStatementWithOnly(hedr,attrm->symb); // add dvm-module-procedure name to only-list
  }
}

SgStatement *MayBeDeleteModuleProc(SgStatement *mod_proc,SgStatement *end_mod)
{ mod_attr *attrm;
                   //mod_proc->unparsestdout(); 
                    //printf("-----%d  %d\n",end_mod->lexPrev()->variant(),end_mod->variant()); end_mod->unparsestdout(); 
 if(!isSgExecutableStatement(end_mod->lexPrev()) || mod_proc->lexNext()==end_mod ) {// there are not executable statements in module procedure
   attrm=DVM_PROC_IN_MODULE(cur_func->symbol()) ;
   attrm->symb=NULL; // deleting module procedure reference in attribute
            //deleting module procedure
           //for(stmt=mod_proc->lexNext(),prev=mod_proc; stmt!=end_mod->lexNext(); stmt=stmt->lexNext())
           //{  prev->extractStmt(); prev = stmt; }
           //end_mod->extractStmt();
           //return(NULL);
 }  
 return(mod_proc);
}

int TestDVMDirectivesInModule(stmt_list *pstmt)
{stmt_list *stmt;
 int flag;
 flag = 0;
  for(stmt=pstmt; stmt; stmt=stmt->next) {
     switch(stmt->st->variant()) {
        //case HPF_TEMPLATE_STAT:
        case DVM_ALIGN_DIR:
        case DVM_DISTRIBUTE_DIR:
        case HPF_PROCESSORS_STAT:
        case DVM_VAR_DECL:
        case DVM_TASK_DIR:
             flag = 1;
             break;
        default:
             break;
     }
  }  
 return(flag);
}

int TestDVMDirectivesInProcedure(stmt_list *pstmt)
{stmt_list *stmt;
 for(stmt=pstmt; stmt; stmt=stmt->next) {
    if(stmt->st->variant() != DVM_INHERIT_DIR)
       return( 1 );
 }
 return ( 0 );
}

int TestUseStmts()
{SgStatement *stmt;
 mod_attr *attrm;
 int flag;
  flag =0;
  //looking through the USE statements
  for(stmt=cur_func->lexNext();stmt->variant() == USE_STMT;stmt=stmt->lexNext()){
    if((attrm=DVM_PROC_IN_MODULE(stmt->symbol())) && attrm->symb) //module has DVM-module-procedure
          flag =1;
  }
  return(flag);
}

int ArrayAssignment(SgStatement *stmt)
{
  if(isSgArrayRefExp(stmt->expr(0)) || isSgArrayType(stmt->expr(0)->type()))
    return(1);
  else
    return(0);
}

int DVMArrayAssignment(SgStatement *stmt)
{
  if(HEADER(stmt->expr(0)->symbol()) && isSgArrayType(stmt->expr(0)->type()))
    return(1);
  else
    return(0);
}

void MakeSection(SgExpression *are)
{int n;
 SgArrayRefExp *ae;   
 if(!(ae=isSgArrayRefExp(are))) return;
 for(n = Rank(are->symbol()); n; n--)
   ae->addSubscript(*new SgExpression(DDOT));  
}

void  DistributeArrayList(SgStatement *stdis)
{SgExpression *el;
 SgSymbol *das;
 SgStatement **dst = new (SgStatement *);

    *dst = stdis;
    for(el=stdis->expr(0); el; el=el->rhs()){                   
        das = el->lhs()->symbol();
        das->addAttribute(DISTRIBUTE_, (void *) dst, sizeof(SgStatement *)); 
    }
}

SgExpression *DebugIfCondition()
{ if(!dbif_cond)
    dbif_cond=&SgEqOp(*new SgVarRefExp(*dbg_var), *new SgValueExp(1));
  return(dbif_cond);
}
/*
SgExpression *DebugIfCondition()
{return(&SgEqOp(*new SgVarRefExp(*dbg_var), *new SgValueExp(1)));}
*/

SgExpression *DebugIfNotCondition()
{ if(!dbif_not_cond)
    dbif_not_cond=&SgEqOp(*new SgVarRefExp(*dbg_var), *new SgValueExp(0));
  return(dbif_not_cond);
}
/*
SgExpression *DebugIfNotCondition()
{return(&SgEqOp(*new SgVarRefExp(*dbg_var), *new SgValueExp(0)));}
*/

SgStatement *LastStatementOfDoNest(SgStatement *first_do)
{SgStatement *last;
    last=first_do->lastNodeOfStmt();
    if(last->variant() == FOR_NODE || last->variant() == WHILE_NODE )
       last=LastStatementOfDoNest(last);
     
 return(last);
}

void TranslateBlock (SgStatement *stat)
{  
   TranslateFromTo(stat,lastStmtOf(stat),0); //0 - without error messages 
}

/*
void TranslateBlock (SgStatement *stat)
SgStatement *stmt, *last, *next;
// last is the statement following last statement of block  
    
 last = lastStmtOf(stat);  //podd 03.06.14  stat->lastNodeOfStmt();
                           //if (last->variant() == LOGIF_NODE)   
                           //  last =last->lexNext();  
 //last =last->lexNext();    
*/

void TranslateFromTo(SgStatement *first, SgStatement *last, int error_msg)
//TranslateBlock (SgStatement *stat)
{SgStatement *stmt, *out, *next;
 SgLabel *lab_on;
 SgStatement *in_on = NULL;
 char io_modes_str[4] = "\0";
 out =last->lexNext();      
 if(only_debug) goto SEQ_PROG;
  
 for(stmt=first; stmt!=out; stmt=next) {
    cur_st = stmt;                             //printf("TranslateBlock %d  %d\n",stmt->lineNumber(), stmt->variant());
    next = stmt->lexNext();  
    switch(stmt->variant()) {
       case CONTROL_END:
       case CONTAINS_STMT:
       case RETURN_STAT:     
       case STOP_STAT:            
       case PAUSE_NODE: 
       case ENTRY_STAT: 
            break;

       case SWITCH_NODE:           // SELECT CASE ...
       case ARITHIF_NODE:          // Arithmetical IF
       case IF_NODE:               // IF... THEN
       case WHILE_NODE:            // DO WHILE (...)
       case CASE_NODE:             // CASE ...
       case ELSEIF_NODE:           // ELSE IF... 
            ChangeDistArrayRef(stmt->expr(0));
            break; 
      
       case LOGIF_NODE:            // Logical IF 
                     
            ChangeDistArrayRef(stmt->expr(0));
            break;  //continue; // to next statement

       case FORALL_STAT:           // FORALL statement
               //stmt=stmt->lexNext(); //  statement that is a part of FORALL statement         
            break;
               // continue; 

       case GOTO_NODE:             // GO TO
            break;

       case COMGOTO_NODE:          // Computed GO TO
            ChangeDistArrayRef(stmt->expr(1));
            break;

       case ASSIGN_STAT:           // Assign statement
            if(IN_COMPUTE_REGION && !inparloop && !in_on) /*ACC*/
                 TestDvmObjectAssign(stmt);    
            ChangeDistArrayRef_Left(stmt->expr(0));   // left part
            ChangeDistArrayRef(stmt->expr(1));   // right part
            break;

       case PROC_STAT:             // CALL
           {SgExpression *el; 
            // looking through the arguments list
            for(el=stmt->expr(0); el; el=el->rhs())            
              ChangeArg_DistArrayRef(el);   // argument
            }            
            break;

       case ALLOCATE_STMT:
            if(!IN_COMPUTE_REGION) 
            { AllocatableArrayRegistration(stmt);
              //stmt=cur_st;
            }
            break;
           
       case DEALLOCATE_STMT:
            break; 

       case DVM_IO_MODE_DIR:
            IoModeDirective(stmt,io_modes_str,error_msg);
            Extract_Stmt(stmt); // extracting DVM-directive
	    break;
 
       case OPEN_STAT:
            Open_Statement(stmt,io_modes_str,error_msg);
            break;  
       case CLOSE_STAT:
            Close_Statement(stmt,error_msg);
            break; //continue;   
       case INQUIRE_STAT:  
            Inquiry_Statement(stmt,error_msg);
            break;  
       case BACKSPACE_STAT:
       case ENDFILE_STAT:
       case REWIND_STAT:
            FilePosition_Statement(stmt, error_msg);
            break;
       case WRITE_STAT:
       case READ_STAT:
	    ReadWrite_Statement(stmt, error_msg);
            break;
       case PRINT_STAT:
            Any_IO_Statement(stmt);
            ReadWritePrint_Statement(stmt, error_msg);
            break;
       case DVM_CP_CREATE_DIR:      /*Check Point*/
            CP_Create_Statement(stmt, error_msg);
            break;
       case DVM_CP_SAVE_DIR:
            CP_Save_Statement(stmt, error_msg);
            break;
       case DVM_CP_LOAD_DIR:
            CP_Load_Statement(stmt, error_msg);
            break;
       case DVM_CP_WAIT_DIR:
            CP_Wait(stmt, error_msg);
            break;                   /*Check Point*/
       case FOR_NODE:
            ChangeDistArrayRef(stmt->expr(0));
            ChangeDistArrayRef(stmt->expr(1));
            break;
       case DVM_ON_DIR:
            if(stmt->expr(0)->symbol() && HEADER(stmt->expr(0)->symbol())) 
               in_on = stmt;                 
            break;
       case DVM_END_ON_DIR:
            if(in_on)
            {
               ReplaceOnByIf(in_on,stmt);
               Extract_Stmt(in_on); // extracting DVM-directive (ON) 
               in_on = NULL;  
            }           
            Extract_Stmt(stmt);  // extracting DVM-directive (END_ON)

            break;
       default:                         
            break;      
    }
   }
  return; /* podd 07.06.11*/

SEQ_PROG:
 for(stmt=first; stmt!=out ; stmt=stmt->lexNext()) {
    cur_st = stmt;
    switch(stmt->variant()) {
       case ALLOCATE_STMT:            
            AllocatableArrayRegistration(stmt);
            stmt=cur_st;           
            break;           
       case WRITE_STAT:
       case READ_STAT:
       case PRINT_STAT: 
            if(perf_analysis) 
              stmt = Any_IO_Statement(stmt);
            break;

       default:                         
            break;      
    }
  }

}

SgStatement *CreateCopyOfExecPartOfProcedure()
{ 
  if(!debug_regim || dbg_if_regim <= 1) return(NULL);
  
  return( cur_func->copyPtr() );
}


void  InsertCopyOfExecPartOfProcedure(SgStatement *stc)
{ SgStatement *stmt, *stend, *ifst, *cur;
     // cur = new SgStatement(DVM_DEBUG_DIR);
  ifst = new SgIfStmt(*DebugIfNotCondition(), *new SgStatement(CONT_STAT));
  first_exec->insertStmtBefore(*ifst,*first_exec->controlParent()); 
  stend=stc->lastNodeOfStmt();
  stmt = stend->lexPrev();
  if(stmt->variant()!=RETURN_STAT)
     stmt->insertStmtAfter(*new SgStatement(RETURN_STAT),*stend->controlParent());

  for(stmt=stc; !isSgExecutableStatement(stmt); stmt=stmt->lexNext())
   {;}
  
  cur = ifst->lexNext();
  cur->insertStmtAfter(*stmt);
  cur->extractStmt();
  TranslateBlock(ifst);

     // for(stmt=first_exec; stmt != stend; stmt=stmt->nextInChildList())  
     //stmt=BLOB_VALUE(BLOB_NEXT(BIF_BLOB1(stmt->thebif)))
     // {  stc = stmt->copyPtr();
}

int lookForDVMdirectivesInBlock(SgStatement *first,SgStatement *last,int contains[] )
{ SgStatement *stmt;
  int dvm_dir=0;
  contains[0]=0; 
  contains[1]=0;  
  for(stmt=first; stmt ; stmt=stmt->lexNext()) {
    switch(stmt->variant()) {
       case CONTAINS_STMT:
       case ENTRY_STAT:
            contains[0]=1;
            goto END__;            
            break;

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
       case DVM_IO_MODE_DIR: 
       case DVM_LOCALIZE_DIR:
       case DVM_SHADOW_ADD_DIR: 
       case DVM_TEMPLATE_CREATE_DIR:  
       case DVM_TEMPLATE_DELETE_DIR:
            dvm_dir = 1; 
            break;

       case OPEN_STAT:
       case CLOSE_STAT:
       case INQUIRE_STAT:
       case BACKSPACE_STAT:
       case ENDFILE_STAT:
       case REWIND_STAT:
            contains[1]=1;
            break;
       default:
            if(isACCdirective(stmt))     /*ACC*/
              dvm_dir = 1; 
            break;     
    }
   if(stmt == last) break;
  }
END__:
 return(dvm_dir);  
}

int IsGoToStatement(SgStatement *stmt)
{int vrnt;
 vrnt=stmt->variant();
 return(vrnt==GOTO_NODE || vrnt==COMGOTO_NODE || vrnt==ARITHIF_NODE);
}

void CopyDvmBegin(SgStatement *entry, SgStatement *first_dvm_exec, SgStatement *last)
{ SgStatement *stmt, *current, *cpst;
  current = entry;
  for(stmt=first_dvm_exec->lexNext(); stmt && stmt != last; stmt=stmt->lexNext())
  {   
    cpst = &(stmt->copy());
    current->insertStmtAfter(*cpst); 
    current = cpst;
  }
}

void DoStmtsForENTRY(SgStatement *first_dvm_exec, SgStatement *last_dvm_entry)
{stmt_list *stl;
 for(stl=entry_list; stl; stl=stl->next)
  CopyDvmBegin(stl->st,first_dvm_exec,last_dvm_entry);
}

void UnparseFunctionsOfFile(SgFile *f,FILE *fout)
{
  SgStatement *stat,*stmt;
       //int i,numfun;
       //int i;
       //i=0;
       //printf("Unparse Functions\n");
// grab the first statement in the file.
  stat = f->firstStatement(); // file header 
       //numfun = f->numberOfFunctions(); //  number of functions
       // function is program unit accept BLOCKDATA and MODULE (F90),i.e. 
       // PROGRAM, SUBROUTINE, FUNCTION
       // for(i = 0; i < numfun; i++) { 
       //   func = f -> functions(i);
   for( stmt=stat->lexNext();stmt;stmt=stmt->lexNext())
   {   //printf("function %d: %s \n", i++,stmt->symbol()->identifier()); 
     fprintf(fout,"%s",UnparseBif_Char(stmt->thebif,FORTRAN_LANG)); //or C_LANG
       //printf("end function %d \n", i);
       //i++;
     stmt=stmt->lastNodeOfStmt();
   }  
}

void StructureProcessing(SgStatement *stmt)
{ SgStatement *st,*vd, *next_st;
  
  next_st=stmt->lexNext(); 
  while(next_st)
  {  st = next_st;
     //printf("%d",st->lineNumber());
     next_st=next_st->lexNext();
     //printf(" : %d\n",next_st->lineNumber());
     switch(st->variant()) 
     { case(VAR_DECL):
         if(only_debug)
         {
           VarDeclaration(st);
           break;
         }
         vd=st; 
         while(vd)
           vd=ProcessVarDecl(vd);
         break;;
       case(CONTROL_END):
         return;
       case(DVM_SHADOW_DIR):
           {SgExpression *el;
            SgExpression **she = new (SgExpression *);
            SgSymbol *ar;
            int nw=0;
            if(only_debug)
            {
               st->extractStmt();
               break;
            }
            // calculate lengh of shadow_list
            for(el = st->expr(1); el; el=el->rhs())
               nw++;
            *she = st->expr(1);
            for(el = st->expr(0); el; el=el->rhs()){ // array name list
               ar = el->lhs()->symbol();  //array name
               ar->addAttribute(SHADOW_WIDTH, (void *) she,                                                                  sizeof(SgExpression *));
               if (nw!=Rank(ar)) // wrong shadow width list
                Error("Length of shadow-edge-list is not equal to the rank of array '%s'", ar->identifier(), 88, st);
	    }
            st->extractStmt();
            break; 

           }   

       case(DVM_DISTRIBUTE_DIR):
            if( !only_debug && (st->expr(1) || st->expr(2)))
              err("Only a distribute-directive of kind DISTRIBUTE:: is permitted in a derived type definition",337,st);   
            st->extractStmt();
            break; 

       case(DVM_ALIGN_DIR):
            if(!only_debug && (st->expr(1) || st->expr(2)))
              err("Only an align-directive of kind ALIGN:: is permitted in a derived type definition",337,st);   
            st->extractStmt();
            break; 

       case(DVM_VAR_DECL):
           { SgExpression *el;
             if(only_debug)
             {
                st->extractStmt();
                break;
             }
         
             for(el = st->expr(2); el; el=el->rhs()) // attribute list
	      switch(el->lhs()->variant()) {
	          case (ALIGN_OP):
                        if(el->lhs()->lhs() || el->lhs()->rhs())
                          err("Only an align-directive of kind ALIGN:: is permitted in a derived type definition",337,st);   
                        break;
                  case (DISTRIBUTE_OP):
                        if(el->lhs()->lhs() || el->lhs()->rhs())
                          err("Only a distribute-directive of kind DISTRIBUTE:: is permitted in a derived type definition",337,st);   
                        break;
                  case (SHADOW_OP):
                       {SgExpression *eln;
                        SgExpression **she = new (SgExpression *);
                        SgSymbol *ar;
                        int nw=0;
                        // calculate lengh of shadow_list
                        for(eln = el->lhs()->lhs() ; eln; eln=eln->rhs())
                           nw++;
                        *she = el->lhs()->lhs(); //shadow specification
                        for(eln = st->expr(0); eln; eln=eln->rhs()){ // array name list
                          ar = eln->lhs()->symbol();  //array name
                          ar->addAttribute(SHADOW_WIDTH, (void *) she,                                                                  sizeof(SgExpression *)); 
                          if (nw!=Rank(ar)) // wrong shadow width list
                            Error("Length of shadow-edge-list is not equal to the rank of array '%s'", ar->identifier(), 88,st);
                        }
                        break;                               
                       }
                  case (DYNAMIC_OP):
                  default: 
                      break;        
              }
              st->extractStmt();
              break;
           } 
       case(DVM_DYNAMIC_DIR):
         st->extractStmt();
         break; 
       default:
         break;  
     }
  }
  
}  

SgStatement *ProcessVarDecl(SgStatement *vd)
{ SgExpression *el, *elb, *e, *e2;
  SgSymbol *s;
  SgType *t;
  SgStatement *std;
  int ia;
  el=vd->expr(0);
  elb=NULL;
  while(el)
  {
     s = el->lhs()->symbol();
     if(!s) s=el->lhs()->lhs()->symbol(); // there is initialisation:POINTST_OP/ASSGN_OP
     if(!s) return(NULL);     
     ia = s->attributes();
     if(!(ia & DISTRIBUTE_BIT) && !(ia & ALIGN_BIT))
     { elb=el;  
       el=el->rhs();
     } else
       break;
  }
  if(!el) 
  {
    VarDeclaration(vd);
    return(NULL); 
  }
  if(elb)
  { elb->setRhs(NULL);
    std = &(vd->copy());
    std->setExpression(0,*vd->expr(0));
    vd->insertStmtBefore(*std);
    VarDeclaration(std);
  } 
  
  if(!(ia & POINTER_BIT))
         //Error("Inconsistent declaration of identifier '%s'",s->identifier(),16,vd); 
     Warning("DISTRIBUTE or ALIGN attribute dictates POINTER attribute  '%s'",s->identifier(),336,vd); 
  //create new statement for s and insert before statement vd
         // new SgVarDeclStmt(SgExpression &varRefValList, SgExpression &attributeList, SgType &type);
   e = el->lhs()->symbol() ? el->lhs() : el->lhs()->lhs();
   e=new SgExprListExp(e->copy());
   e->lhs()->setLhs(new SgExpression(DDOT));
           //e->setRhs(NULL);
   e2= new SgExprListExp(*new SgExpression(POINTER_OP));
   if(len_DvmType) 
   {  SgExpression *le;
      le = new SgExpression(LEN_OP);
      le->setLhs(new SgValueExp(len_DvmType));
      t = new SgType(T_INT, le, SgTypeInt());

   } else
       t = SgTypeInt();

   std = new SgVarDeclStmt(*e,*e2,*t);
   vd->insertStmtBefore(*std);
   if(el->rhs())
   {  vd->setExpression(0,*(el->rhs()));
      return(vd);
   } else
   {  vd->extractStmt();
      return(NULL);
   }
}

void MarkCoeffsAsUsed()
{    symb_list *sl;
     coeffs * c;
     for(sl=dsym; sl; sl=sl->next) 
     {   c = AR_COEFFICIENTS(sl->symb); //((coeffs *) sl->symb-> attributeValue(0,ARRAY_COEF));
         c->use = 1;
     } 
}

int isInternalOrModuleProcedure(SgStatement *header_st)
{
 if((header_st->variant()==FUNC_HEDR || header_st->variant()==PROC_HEDR) &&
    (header_st->controlParent()->variant() == MODULE_STMT || header_st->controlParent()->variant() != GLOBAL) )
    return 1;
 else 
    return 0;

}

int TestMaxDims(SgExpression *list, SgSymbol *ar, SgStatement *stmt)
{
   int ndim = 0;
   SgExpression *el;
   for( el=list; el; el=el->rhs())
      ndim++; 
   if(ndim>MAX_DIMS)
   {
      if(stmt)
         Error("Too many dimensions specified for '%s'",ar->identifier(),43,stmt); 
      return 0;
   }
   else
      return 1;      
}


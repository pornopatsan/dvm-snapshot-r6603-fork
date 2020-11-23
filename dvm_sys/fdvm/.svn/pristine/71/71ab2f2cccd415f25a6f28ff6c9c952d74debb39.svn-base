/*********************************************************************/
/*  Fortran DVM+OpenMP+ACC                                           */
/*                                                                   */
/*                   ACC Directive Processing                        */
/*********************************************************************/
#include "acc_data.h"

#define Nintent 6
#define DELTA 3
#define Nhandler 3
#define SAVE_LABEL_ID 1

extern int opt_base;
extern fragment_list *cur_fragment;
local_part_list *lpart_list;

static int dvmh_targets, has_io_stmt;
static int targets[Ndev];
static int has_region, in_arg_list, analyzing, has_max_minloc, for_shadow_compute;
//static char *fname_gpu;

static SgStatement *cur_in_block, *cur_in_source, *mod_gpu_end;
static SgStatement *call_kernel;
static SgExpression *dvm_array_list, *do_st_list, *indexing_info_list;
static SgExpression *argument_list, *base_mem_list, *coeff_list, *gpu_coeff_list, *registered_uses_list;
static SgExpression *red_var_list, *formal_red_offset_list, *red_offset_list, *copy_uses_list;
static SgConstantSymb *device_const[Ndev], *const_LONG, *intent_const[Nintent], *handler_const[Nhandler];
static SgSymbol *red_offset_symb, *sync_proc_symb, *mem_use_loc_array[8];
static SgSymbol *adapter_symb, *hostproc_symb, *s_offset_type, *s_of_cudaindex_type;
static symb_list *acc_func_list, *acc_registered_list, *non_dvm_list, *parallel_on_list, *tie_list;
static symb_list *assigned_var_list, *range_index_list, *acc_array_list_whole;
static SgSymbol *Imem_k, *Rmem_k, *Dmem_k, *Cmem_k, *DCmem_k, *Lmem_k, *Chmem_k;
static SgSymbol *fdim3;
static SgSymbol *s_ibof, *s_CudaIndexType_k, *s_warpsize, *s_blockDims;
static SgSymbol *s_rest_blocks, *s_cur_blocks, *s_add_blocks, *s_begin[MAX_LOOP_LEVEL];
static SgSymbol *s_end[MAX_LOOP_LEVEL], *s_blocksS_k[MAX_LOOP_LEVEL];
static SgType *type_DvmType, *type_CudaIndexType, *type_with_len_DvmType, *type_FortranDvmType, *CudaIndexType_k;
static int loopIndexCount;


//------ C ----------
static const char *red_kernel_func_names[] = {
    NULL,
    "__dvmh_blockReduceSum", "__dvmh_blockReduceProd",
    "__dvmh_blockReduceMax", "__dvmh_blockReduceMin",
    "__dvmh_blockReduceAND", "__dvmh_blockReduceOR",
    "__dvmh_blockReduceNEQ", "__dvmh_blockReduceEQ",
    "__dvmh_blockReduceMaxLoc", "__dvmh_blockReduceMinLoc",
    "__dvmh_blockReduceSumN", "__dvmh_blockReduceProdN",
    "__dvmh_blockReduceMaxN", "__dvmh_blockReduceMinN",
    "__dvmh_blockReduceANDN", "__dvmh_blockReduceORN",
    "__dvmh_blockReduceNEQN", "__dvmh_blockReduceEQN"
};
static const char *fermiPreprocDir = "CUDA_FERMI_ARCH";
static SgSymbol *s_CudaIndexType, *s_CudaOffsetTypeRef, *s_DvmType;
static SgStatement *end_block, *end_info_block;

int warpSize = 32;
reduction_operation_list *red_struct_list;
symb_list *shared_list, *acc_call_list, *by_value_list;

void InitializeACC()
{
    mod_gpu_symb = NULL;
    mod_gpu = NULL;
    block_C = NULL;
    info_block = NULL;
    //fname_gpu = filenameACC();
    t_dim3 = Type_dim3();
    s_threadidx = s_blockidx = s_blockdim = s_griddim = s_warpsize = NULL;
    s_ibof = NULL;
    s_blockDims = NULL;
    sync_proc_symb = NULL;
    acc_array_list = NULL;
    cur_in_source = NULL;
    kernel_st = NULL;
    in_arg_list = 0;
    shared_list = NULL;
    fdim3 = new SgSymbol(FUNCTION_NAME, "dim3", *(current_file->firstStatement()));
    RGname_list = NULL;
    type_DvmType = NULL;
    type_FortranDvmType = NULL;
    type_CudaIndexType = NULL;
    type_with_len_DvmType = NULL;
    declaration_cmnt = NULL;
    dvmh_targets = options.isOn(NO_CUDA) ? HOST_DEVICE : HOST_DEVICE | CUDA_DEVICE;

    SpecialSymbols.insert(std::pair<char, const char*>('\n', "\\n\"\n\""));
    SpecialSymbols.insert(std::pair<char, const char*>('"', "\\\""));
    SpecialSymbols.insert(std::pair<char, const char*>('\\', "\\\\"));
}

char *filenameACC()
{
    char *name;
    int i;
    name = (char *)malloc((unsigned)(strlen(fin_name) + 1));

    strcpy(name, fin_name);
    for (i = strlen(name) - 1; i >= 0; i--)
    {
        if (name[i] == '.')
        {
            name[i] = '\0';
            break;
        }
    }
    return(name);
}

char *filename_short(SgStatement *st)
{
    char *name;
    int i;
    name = (char *)malloc((unsigned)(strlen(st->fileName()) + 1));
    strcpy(name, st->fileName());

    for (i = strlen(name) - 1; i >= 0; i--)
    {
        if (name[i] == '/' || name[i] == '\\')
        {
            name = &name[i + 1];
            break;
        }
    }
    int l = strlen(name);
    for (i = 0; i < l; i++)
    {
        if (name[i] == '.')
        {
            name[i] = '\0';
            break;
        }
    }
    for (i = strlen(name) - 1; i >= 0; i--)
    {
        if (isupper(name[i]))
            name[i] = tolower(name[i]);
    }

    l = strlen(name);
    for (int i = 0; i < l; i++) 
    {
        char c = name[i];
        if (!( (c >= 'a' && c <= 'z') || c == '_' || ( c >= '0' && c <= '9') ))
            name[i] = '_';
    }

    return(name);
}

char *ChangeFtoCuf(const char *fout_name)
{
    char *name;
    int i;

    name = (char *)malloc((unsigned)(strlen(fout_name) + 4 + 13 + 1));
    strcpy(name, fout_name);
    for (i = strlen(name) - 1; i >= 0; i--)
    {
        /*    if ( name[i] == '.' )
            {  name[i+1] = 'c';
            name[i+2] = 'u';
            name[i+3] = 'f';
            name[i+4] = '\0';
            break;
            }
            */
        if (name[i] == '.')
            break;
    }
    strcpy(name + i, "_cuda_kernels.cuf");
    return(name);
}

char *ChangeFto_C_Cu(const char *fout_name)
{
    char *name;
    int i;

    name = (char *)malloc((unsigned)(strlen(fout_name) + 3 + 14 + 1));
    strcpy(name, fout_name);
    for (i = strlen(name) - 1; i >= 0; i--)
    {    /*
           if ( name[i] == '.' )
           {  name[i+1] = 'c';
           name[i+2] = 'u';
           name[i+3] = '\0';
           break;
           }
           */
        if (name[i] == '.')
        {
            name[i] = '\0';
            break;
        }
    }
    //sprintf(name[i],"%s_cuda_handlers.cu",name);
    if (options.isOn(C_CUDA))
        strcpy(name + i, "_cuda.cu");
    else
        strcpy(name + i, "_cuda_handlers.cu");
    return(name);
}

char *ChangeFto_cpp(const char *fout_name)
{
    char *name;
    int i;

    name = (char *)malloc((unsigned)(strlen(fout_name) + 4 + 5 + 1));
    strcpy(name, fout_name);
    for (i = strlen(name) - 1; i >= 0; i--)
    {
        if (name[i] == '.')
        {
            name[i] = '\0';
            break;
        }
    }
    strcpy(name + i, "_cuda.cpp");
    return(name);
}

char *ChangeFto_info_C(const char *fout_name)
{
    char *name;
    int i;

    name = (char *)malloc((unsigned)(strlen(fout_name) + 2 + 10 + 1));
    strcpy(name, fout_name);
    for (i = strlen(name) - 1; i >= 0; i--)
    {
        if (name[i] == '.')
            break;
    }
    strcpy(name + i, "_cuda_info.c");
    return(name);
}


void InitializeInFuncACC()
{
    int i;
    maxgpu = 0;          /*ACC*/
    sym_gpu = NULL;      /*ACC*/
    cur_region = NULL;   /*ACC*/

    for (i = 0; i < Ntp; i++)
    {
        gpu_mem_use[i] = 0; /*ACC*/
    }
    for (i = 0; i < 8; i++)
    {
        mem_use_loc_array[i] = 0; /*ACC*/
    }
    gpu_mem_use[Integer] = 1;
    nred_gpu = 1;
    maxred_gpu = 0;
    red_offset_symb = NULL;

    acc_func_list = NULL;
    has_region = 0;
    for (i = 0; i < Ndev; i++)
    {
        device_const[i] = NULL; /*ACC*/
    }

    for (i = 0; i < Nintent; i++)
    {
        intent_const[i] = NULL; /*ACC*/
    }

    for (i = 0; i < Nhandler; i++)
    {
        handler_const[i] = NULL; /*ACC*/
    }
    for (i = 0; i < Nregim; i++)
    {
        region_const[i] = NULL; /*ACC*/
    }
    //if(region_compare)
    //RegionRegimConst(REGION_COMPARE_DEBUG); //region_const[REGION_COMPARE_DEBUG] = < SgConstSymb *>

    acc_return_list = NULL;      /*ACC*/
    acc_registered_list = NULL;  /*ACC*/
    registered_uses_list = NULL; /*ACC*/
}

int GeneratedForCuda()
{
    return (kernel_st || cuda_functions ? 1 : 0);
}



void TempVarACC(SgStatement * func) {

    SgValueExp M1(1), M0(0);
    SgExpression  *MN = new SgExpression(
        DDOT, NULL, NULL, NULL);
    SgExpression  *M01 = new SgExpression(DDOT, &M0.copy(), &M1.copy(), NULL);
    SgArrayType *typearray;
    SgExpression *MD;

    if (len_DvmType)
        const_LONG = new SgConstantSymb("LDVMH", *func, *new SgValueExp(len_DvmType));

    typearray = new SgArrayType(*SgTypeInt());
    gpubuf = new SgVariableSymb("gpu000", *typearray, *func);

    MD = (func->variant() == PROG_HEDR) ? MN : M01;

    typearray = new SgArrayType(*SgTypeInt());
    typearray->addRange(*MD);
    Imem_gpu = new SgVariableSymb("i0000g", *typearray, *func);

    typearray = new SgArrayType(*SgTypeFloat());
    typearray->addRange(*MD);
    Rmem_gpu = new SgVariableSymb("r0000g", *typearray, *func);

    typearray = new SgArrayType(*SgTypeDouble());
    typearray->addRange(*MD);
    Dmem_gpu = new SgVariableSymb("d0000g", *typearray, *func);

    typearray = new SgArrayType(*SgTypeBool());
    typearray->addRange(*MD);
    Lmem_gpu = new SgVariableSymb("l0000g", *typearray, *func);

    typearray = new SgArrayType(*SgTypeComplex(current_file));
    typearray->addRange(*MD);
    Cmem_gpu = new SgVariableSymb("c0000g", *typearray, *func);

    typearray = new SgArrayType(*SgTypeDoubleComplex(current_file));
    typearray->addRange(*MD);
    DCmem_gpu = new SgVariableSymb("dc000g", *typearray, *func);

    typearray = new SgArrayType(*SgTypeChar());
    typearray->addRange(*MD);
    Chmem_gpu = new SgVariableSymb("ch000g", *typearray, *func);
    // if(func->variant()==PROG_HEDR)
    // {  SYMB_ATTR(Imem_gpu->thesymb)= SYMB_ATTR(Imem_gpu->thesymb) | ALLOCATABLE_BIT;
    //    SYMB_ATTR(Dmem_gpu->thesymb)= SYMB_ATTR(Dmem_gpu->thesymb) | ALLOCATABLE_BIT;
    // }

}

void AddExternStmtToBlock_C()
{
    SgStatement *stmt = NULL;
    int ln;
    symb_list *sl = NULL;
    if (!RGname_list)
        return;
    for (sl = RGname_list, ln = 0; sl; sl = sl->next, ln++)
    if (!ln)
        stmt = makeExternSymbolDeclaration(&(sl->symb->copy()));
    else
        addDeclExpList(sl->symb, stmt->expr(0));


    cur_in_block->insertStmtBefore(*stmt, *block_C);  //10.12.13
    //block_C->insertStmtAfter(*stmt,*block_C); 
}


int isDestroyable(SgSymbol *s)
{
    if (!CURRENT_SCOPE(s))
        return(0);
    if (s->attributes() & PARAMETER_BIT)
        return(0);
    if ((s->attributes() & SAVE_BIT) || saveall || IN_DATA(s))
        return(0);
    if (IN_COMMON(s) || IS_DUMMY(s))
        return(0);
    return(1);
}

int isLocal(SgSymbol *s)
{
    if (!CURRENT_SCOPE(s))
        return(0);
    if ((s->attributes() & SAVE_BIT) || saveall || IN_DATA(s))
        return(0);
    if (IN_COMMON(s) || IS_DUMMY(s))
        return(0);

    return(1);
}

SgExpression *ACC_GroupRef(int ind)
{
    SgExpression *res;
    res = DVM000(ind);
    if (IN_COMPUTE_REGION || parloop_by_handler)  //BY_HANDLER
    {
        int *id = new int;
        *id = ind + 3;
        res->addAttribute(ACROSS_GROUP_IND, (void *)id, sizeof(int));
    }

    return res;
}

/*
SgSymbol*GpuBaseSymbolForLocArray(int n)
{ SgSymbol *base;
SgArrayType *typearray;
SgExpression *MD;
SgValueExp M1(1),M0(0);
SgExpression  *MN = new SgExpression(DDOT,NULL,NULL,NULL);
SgExpression  *M01 =  new SgExpression(DDOT,&M0.copy(),&M1.copy(),NULL);
char *name;
name = new char[7];
sprintf(name,"i%d000g", n);
typearray = new SgArrayType(*SgTypeInt());
MD = (cur_func->variant()==PROG_HEDR) ? MN : new SgValueExp(n);
typearray-> addRange(*MD);
MD =(cur_func->variant()==PROG_HEDR) ? MN : M01;
typearray-> addRange(*MD);
base = new SgVariableSymb(name, *typearray, *cur_func);
return(base);
}
*/
/*
SgSymbol*KernelBaseSymbolForLocArray(int n)
{ SgSymbol *base;
SgArrayType *typearray;
SgExpression *MD;
SgValueExp M1(1),M0(0);
SgExpression  *M01 =  new SgExpression(DDOT,&M0.copy(),&M1.copy(),NULL);
char *name;
name = new char[7];
sprintf(name,"i%d000m", n);
typearray = new SgArrayType(*SgTypeInt());
MD = new SgValueExp(n);
typearray-> addRange(*MD);
typearray-> addRange(*M01);
base = new SgVariableSymb(name, *typearray, *kernel_st);
return(base);
}
*/
/*
SgSymbol* DerivedTypeGpuBaseSymbol(SgSymbol *stype,SgType *t)
{
char *name;
SgSymbol *sn;
SgArrayType *typearray;
SgValueExp M0(0), M1(1);
SgExpression  *MD;
SgExpression  *MN = new SgExpression(DDOT,NULL,NULL,NULL);
SgExpression  *M01 =  new SgExpression(DDOT,&M0.copy(),&M1.copy(),NULL);
name = new char[80];
sprintf(name,"%s0000g",stype->identifier());
MD = (IN_MAIN_PROGRAM) ? MN : M01;
typearray = new SgArrayType(*t);
typearray-> addRange(*MD);
sn = new SgVariableSymb(name, *typearray, *cur_func);
return(sn);
}
*/
/*
SgSymbol* GpuHeaderSymbol(SgSymbol *ar)
{
char *name;
SgSymbol *sn;
SgArrayType *typearray;
SgValueExp M0(0);
SgExpression  *rnk =  new SgValueExp(Rank(ar)+DELTA);
//name = new char[80];
name =  (char *) malloc((unsigned)(strlen(ar->identifier())+4+1));
sprintf(name,"%s_gpu",ar->identifier());
typearray = new SgArrayType(*SgTypeInt());
typearray-> addRange(*rnk);
sn = new SgVariableSymb(name, *typearray, *cur_func);
return(sn);
}
*/

SgType *Type_dim3()
{
    SgSymbol *sdim3 = new SgSymbol(TYPE_NAME, "dim3", *(current_file->firstStatement()));
    SgFieldSymb *sx = new SgFieldSymb("x", *SgTypeInt(), *sdim3);
    SgFieldSymb *sy = new SgFieldSymb("y", *SgTypeInt(), *sdim3);
    SgFieldSymb *sz = new SgFieldSymb("z", *SgTypeInt(), *sdim3);
    SYMB_NEXT_FIELD(sx->thesymb) = sy->thesymb;
    SYMB_NEXT_FIELD(sy->thesymb) = sz->thesymb;
    SYMB_NEXT_FIELD(sz->thesymb) = NULL;

    SgType *tstr = new SgType(T_STRUCT);
    TYPE_COLL_FIRST_FIELD(tstr->thetype) = sx->thesymb;
    sdim3->setType(tstr);

    SgType *td = new SgType(T_DERIVED_TYPE);
    TYPE_SYMB_DERIVE(td->thetype) = sdim3->thesymb;
    TYPE_SYMB(td->thetype) = sdim3->thesymb;

    return(td);
}

SgType *FortranDvmType()
{
    SgType *t;
    if (type_FortranDvmType)
        return(type_FortranDvmType);
    if (len_DvmType)
    {
        SgExpression *le;
        le = new SgExpression(LEN_OP);
        le->setLhs(new SgValueExp(len_DvmType));
        t = new SgType(T_INT, le, NULL);

    }
    else
        t = SgTypeInt();
    type_FortranDvmType = t;
    return(type_FortranDvmType);
}

void DeviceTypeConsts()
{
    if (device_const[HOST]) return;
    device_const[HOST] = new SgConstantSymb("DEVICE_TYPE_HOST", *cur_func, *new SgValueExp(HOST));
    device_const[CUDA] = new SgConstantSymb("DEVICE_TYPE_CUDA", *cur_func, *new SgValueExp(CUDA));
}

SgSymbol *DeviceTypeConst(int i)
{
    if (device_const[i])
        return(device_const[i]);
    switch (i)
    {
    case HOST:
        device_const[HOST] = new SgConstantSymb("DEVICE_TYPE_HOST", *cur_func, *new SgValueExp(HOST));
        break;
    case CUDA:
        device_const[CUDA] = new SgConstantSymb("DEVICE_TYPE_CUDA", *cur_func, *new SgValueExp(CUDA));
        break;
    }
    return(device_const[i]);
}


void HandlerTypeConsts()
{
    if (handler_const[HANDLER_TYPE_PARALLEL]) return;
    handler_const[HANDLER_TYPE_PARALLEL] = new SgConstantSymb("HANDLER_TYPE_PARALLEL", *cur_func, *new SgValueExp(HANDLER_TYPE_PARALLEL));
    handler_const[HANDLER_TYPE_MASTER] = new SgConstantSymb("HANDLER_TYPE_MASTER", *cur_func, *new SgValueExp(HANDLER_TYPE_MASTER));
}

SgSymbol *HandlerTypeConst(int i)
{
    if (handler_const[i])
        return(handler_const[i]);
    switch (i)
    {
    case HANDLER_TYPE_PARALLEL:
        handler_const[HANDLER_TYPE_PARALLEL] = new SgConstantSymb("HANDLER_TYPE_PARALLEL", *cur_func, *new SgValueExp(HANDLER_TYPE_PARALLEL));
        break;
    case HANDLER_TYPE_MASTER:
        handler_const[HANDLER_TYPE_MASTER] = new SgConstantSymb("HANDLER_TYPE_MASTER", *cur_func, *new SgValueExp(HANDLER_TYPE_MASTER));
        break;
    }
    return(handler_const[i]);
}

SgSymbol *RegionRegimConst(int regim)
{
    if (region_const[regim]) return(region_const[regim]);
    if (regim == REGION_ASYNC)
        region_const[REGION_ASYNC] = new SgConstantSymb("REGION_ASYNC", *cur_func, *new SgValueExp(REGION_ASYNC));
    else if (regim == REGION_COMPARE_DEBUG)
        region_const[REGION_COMPARE_DEBUG] = new SgConstantSymb("REGION_COMPARE_DEBUG", *cur_func, *new SgValueExp(REGION_COMPARE_DEBUG));
    return(region_const[regim]);
}


SgSymbol *IntentConst(int intent)
{
    const char *name;

    if (intent_const[intent])
        return(intent_const[intent]);

    switch (intent)
    {
    case(INTENT_IN) : name = "INTENT_IN";     break;
    case(INTENT_OUT) : name = "INTENT_OUT";    break;
    case(INTENT_INOUT) : name = "INTENT_INOUT";  break;
    case(INTENT_LOCAL) : name = "INTENT_LOCAL";  break;
    case(INTENT_INLOCAL) : name = "INTENT_INLOCAL"; break;
    case(EMPTY) : name = "EMPTY";         break;
    default:              name = "";              break;
    }

    intent_const[intent] = new SgConstantSymb(name, *cur_func, *new SgValueExp(intent));

    return(intent_const[intent]);
}

SgSymbol *ArraySymbol(char *name, SgType *basetype, SgExpression *range, SgStatement *scope)
{
    SgSymbol *ar;
    SgArrayType *typearray;

    typearray = new SgArrayType(*basetype);
    if (range)
        typearray->addRange(*range);
    ar = new SgVariableSymb(name, *typearray, *scope);
    return(ar);
}

SgSymbol *ArraySymbol(const char *name, SgType *basetype, SgExpression *range, SgStatement *scope)
{
    SgSymbol *ar;
    SgArrayType *typearray;

    typearray = new SgArrayType(*basetype);
    if (range)
        typearray->addRange(*range);
    ar = new SgVariableSymb(name, *typearray, *scope);
    return(ar);
}


SgSymbol *KernelSymbol(SgStatement *st_do)
{
    SgSymbol *sk;
    ++nkernel;

    char *kname = (char *)malloc((unsigned)(strlen(st_do->fileName())) + 38);
    if (inparloop)
        sprintf(kname, "%s_%s_%d_cuda_kernel", "loop", filename_short(st_do), st_do->lineNumber());
    else
        sprintf(kname, "%s_%s_%d_cuda_kernel", "sequence", filename_short(st_do), st_do->lineNumber());

    sk = new SgSymbol(PROCEDURE_NAME, kname, *mod_gpu);
    if (options.isOn(C_CUDA))
        sk->setType(C_VoidType());
    return(sk);
}

SgSymbol *HostProcSymbol(SgStatement *st_do)
{
    SgSymbol *s;
    char *sname = (char *)malloc((unsigned)(strlen(st_do->fileName())) + 30);
    if (inparloop)
        sprintf(sname, "%s_%s_%d_host", "loop", filename_short(st_do), st_do->lineNumber());
    else
        sprintf(sname, "%s_%s_%d_host", "sequence", filename_short(st_do), st_do->lineNumber());
    s = new SgSymbol(PROCEDURE_NAME, sname, *current_file->firstStatement());
    acc_func_list = AddToSymbList(acc_func_list, s);
    return(s);
}

SgSymbol *IndirectFunctionSymbol(SgStatement *stmt, char *name)
{
    char *sname = (char *)malloc((unsigned)(strlen(stmt->fileName())) + 40);    
    sprintf(sname, "indirect_%s_%s_%d", name, filename_short(stmt), stmt->lineNumber());
    SgSymbol *s = new SgSymbol(PROCEDURE_NAME, sname, *current_file->firstStatement());
    acc_func_list = AddToSymbList(acc_func_list, s);
    return(s);
}

SgSymbol *GPUModuleSymb(SgStatement *global_st)
{
    SgSymbol *mod_symb;
    char *modname;

    modname = (char *)malloc((unsigned)(strlen(global_st->fileName()) + 8 + 1));
    sprintf(modname, "dvm_gpu_%s", filename_short(global_st)); 
    mod_symb = new SgSymbol(MODULE_NAME, modname, *global_st);
    return(mod_symb);
}

SgSymbol *HostAcrossProcSymbol(SgSymbol *sHostProc, int dependency)
{
    SgSymbol *s;
    char *sname = (char *)malloc((unsigned)(strlen(sHostProc->identifier())) + 5);
    sprintf(sname, "%s_%d", sHostProc->identifier(), dependency);
    s = new SgSymbol(PROCEDURE_NAME, sname, *current_file->firstStatement());
    acc_func_list = AddToSymbList(acc_func_list, s);
    return(s);
}

SgSymbol *CudaforSymb(SgStatement *global_st)
{
    SgSymbol *cudafor_symb;
    cudafor_symb = new SgSymbol(MODULE_NAME, "cudafor", *global_st);
    return(cudafor_symb);
}

/*
SgSymbol *KernelArgumentSymbol(int n)
{char *name;
SgSymbol *sn;
name = new char[80];
sprintf(name,"dbv_goto00%d", n);
sn = new SgVariableSymb(name,*t,*cur_func);
if_goto = AddToSymbList(if_goto, sn);
return(sn);
}
*/

/*
SgSymbol *Var_Offset_Symbol(SgSymbol *var)
{
if(!red_offset_symb)
red_offset_symb = new SgVariableSymb("red_offset",*new SgArrayType(*IndexType()),*cur_func);

return(red_offset_symb);
}
*/

SgSymbol *RedCountSymbol(SgStatement *scope)
{
    //if(red_count_symb) return;

    return(new SgVariableSymb("red_count", *SgTypeInt(), *scope)); // IndexType()

}


SgSymbol *OverallBlocksSymbol()
{
    SgType *type;
    type = options.isOn(C_CUDA) ? C_CudaIndexType() : FortranDvmType();
    return(new SgVariableSymb("overall_blocks", *type, *kernel_st));

}

void BeginEndBlocksSymbols(int pl_rank)
{
    int i;
    char *name = new char[20];
    SgType *type;
    for (i = MAX_LOOP_LEVEL; i; i--)
    {
        s_begin[i - 1] = NULL;
        s_end[i - 1] = NULL;
        s_blocksS_k[i - 1] = NULL;
    }
    type = options.isOn(C_CUDA) ? C_Derived_Type(s_CudaIndexType_k) : CudaIndexType();
    for (i = 1; i <= pl_rank; i++)
    {
        sprintf(name, "begin_%d", i);
        s_begin[i - 1] = new SgVariableSymb(TestAndCorrectName(name), *type, *kernel_st);
        sprintf(name, "end_%d", i);
        s_end[i - 1] = new SgVariableSymb(TestAndCorrectName(name), *type, *kernel_st);
        sprintf(name, "blocks_%d", i);
        s_blocksS_k[i - 1] = new SgVariableSymb(TestAndCorrectName(name), *type, *kernel_st);
    }

}

/*
SgSymbol *RedOffsetSymbolInKernel(SgSymbol *s)
{ char *name;
SgSymbol *soff;

name =  (char *) malloc((unsigned)(strlen(s->identifier())+8));
//strcpy (name,s->identifier());
sprintf(name,"%s_offset",s->identifier());
soff = new SgVariableSymb(name, *IndexType(), *kernel_st);

return(soff);
}
*/
/*
SgSymbol *RedOffsetSymbolInKernel_ToList(SgSymbol *s)
{ char *name;
SgSymbol *soff;
SgExpression *ell, *el;
name =  (char *) malloc((unsigned)(strlen(s->identifier())+8));
sprintf(name,"%s_offset",s->identifier());
soff = new SgVariableSymb(name, *IndexType(), *kernel_st);
ell = new SgExprListExp(*new SgVarRefExp(*soff));
if(!formal_red_offset_list)
formal_red_offset_list = ell;
else
{ el = formal_red_offset_list;
while( el->rhs())
el=el->rhs();
el->setRhs(ell);
}
return(soff);
}

*/

SgStatement * MakeStructDecl(SgSymbol *strc)
{
    SgStatement *typedecl, *st1, *st2;
    SgSymbol *sf;
    typedecl = new SgDeclarationStatement(STRUCT_DECL);
    typedecl->setSymbol(*strc);
    sf = FirstTypeField(strc->type());
    st1 = sf->makeVarDeclStmt();
    typedecl->insertStmtAfter(*st1, *typedecl);
    sf = ((SgFieldSymb *)sf)->nextField();
    st2 = sf->makeVarDeclStmt();
    st1->insertStmtAfter(*st2, *typedecl);
    return(typedecl);

    /*
      sf = =((SgFieldSymb *)sf)->nextField();
      for(sf=FirstTypeField(s->type());sf;sf=((SgFieldSymb *)sf)->nextField())

      SYMB_NEXT_FIELD(sz->thesymb) = NULL;

      SgType *tstr = new SgType(T_STRUCT);
      TYPE_COLL_FIRST_FIELD(tstr->thetype)=  sx->thesymb;
      SymbMapping
      */
}

/*
int isIntrinsicFunction(SgSymbol *sf)
{
if(IntrinsicInd(sf) == -1)
return(0);
else
return( 1);
}


int IntrinsicInd(SgSymbol *sf)
{ int i;
for(i=0; i<MAX_INTRINSIC_NUM; i++)
{  if(! intrinsic_name[i] )
break;
//printf("%d   %s = %s\n", i, intrinsic_name[i], sf->identifier());

if(!strcmp(sf->identifier(),intrinsic_name[i]))
return(i);
}
return(-1);
}
*/

void DeclareVarGPU(SgStatement *lstat, SgType *tlen)
{
    SgStatement *st;
    SgExpression *eatr, *el, *eel;
    int i;

    // declare created procedures(C-functions) as EXTERNAL

    if (acc_func_list)
    {
        symb_list *sl;
        SgExpression *el, *eel;
        st = new SgStatement(EXTERN_STAT);
        el = new SgExprListExp(*new SgVarRefExp(acc_func_list->symb));
        for (sl = acc_func_list->next; sl; sl = sl->next)
        {
            eel = new SgExprListExp(*new SgVarRefExp(sl->symb));
            eel->setRhs(*el);
            el = eel;
        }
        st->setExpression(0, *el);

        lstat->insertStmtAfter(*st);
    }

    // declare INTENT constants

    for (i = Nintent - 1, el = NULL; i >= 0; i--)
    if (intent_const[i])
    {
        eel = new SgExprListExp(*new SgRefExp(CONST_REF, *intent_const[i]));
        eel->setRhs(el);
        el = eel;
    }
    if (el)
    {
        st = fdvm[0]->makeVarDeclStmt();
        st->setExpression(0, *el);
        if (len_DvmType)
            st->expr(1)->setType(tlen);
        eatr = new SgExprListExp(*new SgExpression(PARAMETER_OP));
        st->setExpression(2, *eatr);
        lstat->insertStmtAfter(*st);
    }
    // declare CUDA constants

    for (i = Ndev - 1, el = NULL; i; i--)
    if (device_const[i])
    {
        eel = new SgExprListExp(*new SgRefExp(CONST_REF, *device_const[i]));
        eel->setRhs(el);
        el = eel;
    }
    if (el)
    {
        st = fdvm[0]->makeVarDeclStmt();
        st->setExpression(0, *el);
        if (len_DvmType)
            st->expr(1)->setType(tlen);
        eatr = new SgExprListExp(*new SgExpression(PARAMETER_OP));
        st->setExpression(2, *eatr);
        lstat->insertStmtAfter(*st);
    }

    // declare Handler constants   /* OpenMP * /

    for (i = Nhandler - 1, el = NULL; i; i--)
    if (handler_const[i])
    {
        eel = new SgExprListExp(*new SgRefExp(CONST_REF, *handler_const[i]));
        eel->setRhs(el);
        el = eel;
    }
    if (el)
    {
        st = fdvm[0]->makeVarDeclStmt();
        st->setExpression(0, *el);
        if (len_DvmType)
            st->expr(1)->setType(tlen);
        eatr = new SgExprListExp(*new SgExpression(PARAMETER_OP));
        st->setExpression(2, *eatr);
        lstat->insertStmtAfter(*st);
    }




    // declare REGION-REGIM constants

    for (i = Nregim - 1, el = NULL; i; i--)
    if (region_const[i])
    {
        eel = new SgExprListExp(*new SgRefExp(CONST_REF, *region_const[i]));
        eel->setRhs(el);
        el = eel;
    }
    if (el)
    {
        st = fdvm[0]->makeVarDeclStmt();
        st->setExpression(0, *el);
        if (len_DvmType)
            st->expr(1)->setType(tlen);
        eatr = new SgExprListExp(*new SgExpression(PARAMETER_OP));
        st->setExpression(2, *eatr);
        lstat->insertStmtAfter(*st);
    }

}

/************************************************************************************/
/*        Data Region                                                               */
/************************************************************************************/
void EnterDataRegionForAllocated(SgStatement *stmt)
{SgExpression *al;
 for(al=stmt->expr(0); al; al=al->rhs()) 
   EnterDataRegion(al->lhs(),stmt);   
 
 allocated_list = AddListToList(allocated_list,&stmt->expr(0)->copy()); 
}

void EnterDataRegion(SgExpression *ale,SgStatement *stmt)
{ SgExpression *e,*size;
  SgSymbol *ar;
  
  e = &(ale->copy());
  if(isSgRecordRefExp(e))
  {  
     SgExpression *alce = RightMostField(e);
     alce->setLhs(NULL);
     ar = alce->symbol();
  }  else
  {
     e->setLhs(NULL);
     ar = e->symbol();
  }
/*
  SgType *t = ar->type(); 
  if(isSgArrayType(t))
  {
     t = t->baseType(); 
     size = &(*SizeFunction(ar,0) * (*ConstRef_F95(TypeSize(t))));
  } else
     size = ConstRef_F95(TypeSize(t));   
  InsertNewStatementAfter(DataEnter(e,size),cur_st,cur_st->controlParent());
*/
  InsertNewStatementAfter(DataEnter(e,ConstRef(0)),cur_st,cur_st->controlParent());   
}

void ExitDataRegion(SgExpression *ale,SgStatement *stmt)
{ SgExpression *e,*size;
  SgSymbol *ar,*ar2;
  
  e = &(ale->copy()); 
  if(isSgRecordRefExp(e))
  {  
     SgExpression *alce = RightMostField(e);
     alce->setLhs(NULL);  
     ar = LeftMostField(e)->symbol();
     
          //if(!(ar2 = GetTypeField(RightMostField(e->lhs())->symbol(),RightMostField(e)->symbol())))
     ar2 = RightMostField(e)->symbol();
      

                  //printf("==%s %d\n",ar->identifier(), TYPE_COLL_FIRST_FIELD(ar->type()->symbol()->type()->thetype)->attr);
                 //ar->type()->symbol()->type()->firstField()->identifier());// ->type()->symbol()->type()->variant());
  }  else
  {
     e->setLhs(NULL);
     ar = ar2 = e->symbol();
  }
   
                 //  printf("%s  %d %d %d\n",ar->identifier(),ar->attributes() & POINTER_BIT, ar->attributes(),e->rhs()->symbol()->variant());
  if(isLocal(ar) && !IS_POINTER_F90(ar2))
    doLogIfForAllocated(e,stmt);
               
}

void UnregisterVariables(int begin_block)
{
    stmt_list *stl;
    int is;
    if (IN_MAIN_PROGRAM)
        return;
    for (stl = acc_return_list; stl; stl = stl->next)
    {  
       is = ExitDataRegionForAllocated(stl->st, begin_block);
       ExitDataRegionForLocalVariables(stl->st, is || begin_block); 
    }
}

/*
void InsertDestroyBlock(SgStatement *st)  
{
    SgExpression *el;
    symb_list *sl;

    if (st->lexNext()->lineNumber() == 0)  // there are inserted (by EndOfProgramUnit()) statements
        st = st->lexNext(); // to insert new statements after dvmlf() call
    for (el = registered_uses_list; el; el = el->rhs())
    {
        if (!el->lhs()) continue;
        if (el->lhs()->symbol()->variant() != CONST_NAME && isLocal(el->lhs()->symbol()) && !IS_ALLOCATABLE(el->lhs()->symbol()))   //  //!(el->lhs()->symbol()->attributes() & PARAMETER_BIT) )
            st->insertStmtAfter(*DestroyScalar(new SgVarRefExp(el->lhs()->symbol())));
    }
    for (sl = acc_registered_list; sl; sl = sl->next)
    {
        if (sl->symb->variant() != CONST_NAME && isLocal(sl->symb))      //&& !IS_ALLOCATABLE(sl->symb)       //!(sl->symb->attributes() & PARAMETER_BIT))
        {
            if (HEADER(sl->symb))
                st->insertStmtAfter(*DestroyArray(HeaderRef(sl->symb)));
            else if (!IS_ALLOCATABLE(sl->symb))
                st->insertStmtAfter(*DestroyScalar(new SgVarRefExp(sl->symb)));
        }
    }

}
*/

void EnterDataRegionForLocalVariables(SgStatement *st)
{
    SgExpression *el;
    symb_list *sl;
    
    for (el = registered_uses_list; el; el = el->rhs())
    {
        if (!el->lhs()) continue;
        if (el->lhs()->symbol()->variant() != CONST_NAME && isLocal(el->lhs()->symbol()) && !IS_ALLOCATABLE(el->lhs()->symbol()))   //  //!(el->lhs()->symbol()->attributes() & PARAMETER_BIT) )
            st->insertStmtAfter(*DataEnter(new SgVarRefExp(el->lhs()->symbol()),ConstRef(0)),*st->controlParent());
    }
    for (sl = acc_registered_list; sl; sl = sl->next)
    {
        if (sl->symb->variant() != CONST_NAME && isLocal(sl->symb) && !IS_ALLOCATABLE(sl->symb) && !HEADER(sl->symb))       //!(sl->symb->attributes() & PARAMETER_BIT))        
            st->insertStmtAfter(*DataEnter(new SgVarRefExp(sl->symb),ConstRef(0)),*st->controlParent());
    }

}
 
void ExitDataRegionForLocalVariables(SgStatement *st, int is)
{
    SgExpression *el;
    symb_list *sl;
 
    for (el = registered_uses_list; el; el = el->rhs())
    {   
        if (!el->lhs()) continue;
        if (el->lhs()->symbol()->variant() != CONST_NAME && isLocal(el->lhs()->symbol()) && !IS_ALLOCATABLE(el->lhs()->symbol()))   //  //!(el->lhs()->symbol()->attributes() & PARAMETER_BIT) )
        {
            if (!is++)
               LINE_NUMBER_BEFORE(st,st);                   
            InsertNewStatementBefore(DataExit(new SgVarRefExp(el->lhs()->symbol()),0),st);
        }
    }
    for (sl = acc_registered_list; sl; sl = sl->next)
    {
        if (sl->symb->variant() != CONST_NAME && isLocal(sl->symb) && !IS_ALLOCATABLE(sl->symb) && !HEADER(sl->symb))       //!(sl->symb->attributes() & PARAMETER_BIT))        
        {  
            if (!is++)
               LINE_NUMBER_BEFORE(st,st);                     
            InsertNewStatementBefore(DataExit(new SgVarRefExp(sl->symb),0),st);
        }
    }
}

void ExtractCopy(SgExpression *elist)
{
  SgExpression *el;
  SgExpression *e = elist->lhs();
  if(!e) return;
  for (el = elist->rhs(); el; el = el->rhs())
     if(el->lhs() && ExpCompare(e,el->lhs()))
        el->setLhs(NULL);  
}

void CleanAllocatedList()
{
//the same allocated_list items are deleted  
  SgExpression *el;
  for (el = allocated_list; el; el = el->rhs())
     ExtractCopy(el); 
  for (el = allocated_list; el; )
     if(el->rhs() && !el->rhs()->lhs())
        el->setRhs(el->rhs()->rhs());
     else
        el = el->rhs();     
}

int ExitDataRegionForAllocated(SgStatement *st,int begin_block) 
{
    SgExpression *el;
  
    if (TestLocal(allocated_list))
    {
       if(!begin_block)
          LINE_NUMBER_BEFORE(st,st);
    } else
       return(0);
    CleanAllocatedList();
    for (el = allocated_list; el; el = el->rhs())      
        ExitDataRegion(el->lhs(),st); 
    return(1);   
}

int TestLocal(SgExpression *list)
{
    SgExpression *el; 
    SgSymbol *s;
    for (el = list; el; el = el->rhs())   
    {
       s = isSgRecordRefExp(el->lhs()) ? LeftMostField(el->lhs())->symbol() : el->lhs()->symbol();
       if(isLocal(s))
          return(1);
    }  
    return (0);
}
/**********************************************************************************/

int isACCdirective(SgStatement *stmt)
{
    switch (stmt->variant()) {

        //          case(ACC_DATA_REGION_DIR):
        //          case(ACC_END_DATA_REGION_DIR):
        //          case(ACC_REGION_DO_DIR):
        //          case(ACC_DO_DIR):
        //          case(ACC_UPDATE_DIR):

    case(ACC_REGION_DIR) :
    case(ACC_END_REGION_DIR) :
    case(ACC_ACTUAL_DIR) :
    case(ACC_GET_ACTUAL_DIR) :
    case(ACC_CHECKSECTION_DIR) :
    case(ACC_END_CHECKSECTION_DIR) :
                                   return(stmt->variant());
    default:
        return(0);
    }
}

SgStatement *ACC_Directive(SgStatement *stmt)
{
    if (!ACC_program)       // by option -noH regime
        return(stmt);                                
    switch (stmt->variant()) {
    case(ACC_REGION_DIR) :
        return(ACC_REGION_Directive(stmt));

    case(ACC_END_REGION_DIR) :
        return(ACC_END_REGION_Directive(stmt));


    case(ACC_ACTUAL_DIR) :
        return(ACC_ACTUAL_Directive(stmt));

    case(ACC_GET_ACTUAL_DIR) :
        return(ACC_GET_ACTUAL_Directive(stmt));

    case(ACC_CHECKSECTION_DIR) :
        if (!IN_COMPUTE_REGION)
            err("Misplaced directive", 103, stmt);
        in_checksection = 1;
        acc_array_list = NULL;
        return(stmt);
    case(ACC_END_CHECKSECTION_DIR) :
        in_checksection = 0;
        return(stmt);
    default:
        return(stmt);
    }

}

void ACC_ROUTINE_Directive(SgStatement *stmt)
{
    if( options.isOn(NO_CUDA) )
        return;
    int control_variant =  stmt->controlParent()->controlParent()->variant();
    if (control_variant == INTERFACE_STMT || control_variant == INTERFACE_OPERATOR || control_variant == INTERFACE_ASSIGNMENT)
    {
        stmt->controlParent()->symbol()->addAttribute(ROUTINE_ATTR, (void*)1, 0);
        return;
    }
    else if (control_variant != GLOBAL)
    {
        err("Misplaced directive",103,stmt);
        return;
    }      
    if (!mod_gpu_symb)
        CreateGPUModule();
    int targets = stmt->expr(0) ? TargetsList(stmt->expr(0)->lhs()) : dvmh_targets;
    targets = targets & dvmh_targets;
    SgSymbol *s = stmt->controlParent()->symbol();
    if(!s)
        return; 
    if(targets & CUDA_DEVICE)
        MarkAsCalled(s);
    MarkAsRoutine(s);
    return;
}

SgStatement *ACC_ACTUAL_Directive(SgStatement *stmt)
{
    SgExpression *e, *el;
    SgSymbol *s;
    int ilow, ihigh;

    LINE_NUMBER_AFTER(stmt, stmt);

    if (!stmt->expr(0))
    {
        doCallAfter(ActualAll());   //inserting after current statement
        return(cur_st);
    }

    for (el = stmt->expr(0); el; el = el->rhs())
    {
        e = el->lhs();
        s = e->symbol();
        if (isSgVarRefExp(e))
        {
            doCallAfter(ActualScalar(s));
            continue;
        }
        if (isSgArrayRefExp(e) && isSgArrayType(s->type()))
        {
            if (HEADER(s)) //is distributed array reference
            {
                if (!e->lhs())   //whole array
                {
                    doCallAfter(ActualArray(s));   //inserting after current statement
                    continue;
                }
                else
                {
                    if(INTERFACE_RTS2)                  
                        doCallAfter(ActualSubArray_2(s, Rank(s), SectionBoundsList(e)));           
                    else
                    {
                        ilow = ndvm;
                        ihigh = SectionBounds(e);
                        doCallAfter(ActualSubArray(s, ilow, ihigh)); //inserting after current statement 
                     }
                }
            }
            else
            {//if(isSgArrayType(s->type()))  //may be T_STRING
                //Warning("%s is not DVM-array",s->identifier(),606,cur_region->region_dir);
                //doCallAfter(ActualScalar(s));
                //continue;
                if (!e->lhs())  //whole array
                    doCallAfter(ActualScalar(s));   //inserting after current statement
                else
                {
                    if(INTERFACE_RTS2)                  
                        doCallAfter(ActualSubVariable_2(s, Rank(s), SectionBoundsList(e)));           
                    else
                    {
                        ilow = ndvm;
                        ihigh = SectionBounds(e);
                        doCallAfter(ActualSubVariable(s, ilow, ihigh)); //inserting after current statement
                    } 
                }
            }
            continue;
        }
        /* scalar in list is variable name !!!
         if(isSgRecordrefExp(e) || e->variant()==ARRAY_OP)  //structure component or substring
         {   Warning ("%s is not DVM-array",e->lhs()->symbol()->identifier(),606,stmt);
         doCallAfter(ActualScalar(e->lhs()->symbol()));
         continue;
         }
         */
        err("Illegal element of list",636, stmt);
        break;
    }
    return(cur_st);
}

SgStatement *ACC_GET_ACTUAL_Directive(SgStatement *stmt)
{
    SgExpression *el, *e;
    SgSymbol *s;
    int ilow, ihigh;

    LINE_NUMBER_AFTER(stmt, stmt);

    if (!stmt->expr(0))
    {
        doCallAfter(GetActualAll());   //inserting after current statement
        return(cur_st);
    }
    for (el = stmt->expr(0); el; el = el->rhs())
    {
        e = el->lhs();
        s = e->symbol();
        if (isSgVarRefExp(e))
        {
            doCallAfter(GetActualScalar(s));   //inserting after current statement
            continue;
        }
        if (isSgArrayRefExp(e) && isSgArrayType(s->type()))   // array reference
        {
            if (HEADER(s)) //is distributed array reference

            {                   
                if (!e->lhs())  //whole array
                    doCallAfter(GetActualArray(HeaderRef(s)));   //inserting after current statement
                else
                {
                    if(INTERFACE_RTS2)                  
                        doCallAfter(GetActualSubArray_2(s, Rank(s), SectionBoundsList(e)));           
                    else
                    {
                        ilow = ndvm;
                        ihigh = SectionBounds(e);
                        doCallAfter(GetActualSubArray(s, ilow, ihigh)); //inserting after current statement
                    } 
                }
            }
            else  // is not distributed array reference
            {
                if (!e->lhs())  //whole array
                    doCallAfter(GetActualScalar(s));   //inserting after current statement
                else
                {
                    if(INTERFACE_RTS2)                  
                        doCallAfter(GetActualSubVariable_2(s, Rank(s), SectionBoundsList(e)));           
                    else
                    {
                        ilow = ndvm;
                        ihigh = SectionBounds(e);
                        doCallAfter(GetActualSubVariable(s, ilow, ihigh)); //inserting after current statement
                    } 
                }
            }
            continue;
        }
        err("Illegal element of list",636, stmt);
        break;
    }   
    return(cur_st);
}


SgStatement *ACC_END_REGION_Directive(SgStatement *stmt)
{

    dvm_debug = (cur_fragment && cur_fragment->dlevel) ? 1 : 0; //permit dvm-debugging

    if (!cur_region || cur_region->is_data)
    {
        err("Unmatched directive", 182, stmt);
        return(stmt);
    }
    if (cur_region->region_dir->controlParent() != stmt->controlParent())
        err("Misplaced directive", 103, stmt); //region must be a block
    if (in_checksection)
        err("Missing END HOSTSECTION directive in region", 571, stmt);

    //!!!printf("END REGION No:%d begin:%d  end:%d\n",cur_region->No,cur_region->region_dir->lineNumber(), stmt->lineNumber());
    LINE_NUMBER_AFTER(stmt, stmt);
    stmt->lexNext()->addComment(EndRegionComment(cur_region->region_dir->lineNumber()));
    DeleteNonDvmArrays();
    InsertNewStatementAfter(EndRegion(cur_region->No), cur_st, stmt->controlParent());
    //cur_st->addComment(EndRegionComment(cur_region->region_dir->lineNumber()));

    SET_DVM(cur_region->No);  //SET_GPU(cur_region->No);
    region_list *p = cur_region;
    cur_region = cur_region->next;
    free(p);
    return(cur_st);
}


SgStatement *ACC_REGION_Directive(SgStatement *stmt)
{
    SgExpression *eop, *el, *tl;
    int intent, irgn, user_targets, region_targets;

    // inhibit dvm-debugging inside region !
    dvm_debug = 0;

    // initialization
    has_region = 1;
    user_targets = 0;

    in_checksection = 0;

    if (inparloop)
        err("Misplaced directive", 103, stmt);
    if (cur_region && !cur_region->is_data)
        err("Nested compute regions are not permitted", 601, stmt);
    if(rma)
        err("REGION directive within the scope of REMOTE_ACCESS directive", 631, stmt);
    irgn = ndvm++;
    NewRegion(stmt, irgn, 0);
    if(AnalyzeRegion(stmt)==1)   // AnalyzeRegion creates uses list for region
    {   // no END REGION directive 
        cur_region = cur_region->next; //closing region
        dvm_debug = (cur_fragment && cur_fragment->dlevel) ? 1 : 0; //permit dvm-debugging
        return(cur_st);
    }
               //printf("REGION No:%d begin:%d  %d\n",cur_region->No,cur_region->region_dir->lineNumber(), stmt->lineNumber());
    LINE_NUMBER_AFTER(stmt, stmt);
    //DoHeadersForNonDvmArrays();
    non_dvm_list = NULL;
    by_value_list = NULL;

    doAssignTo_After(DVM000(irgn), RegionCreate(0));   //RegionCreate((region_compare ? REGION_COMPARE_DEBUG : 0))); 
    cur_st->addComment(RegionComment(stmt->lineNumber()));
    where = cur_st;
    for (el = stmt->expr(0); el; el = el->rhs())
    {
        eop = el->lhs();
        if (eop->variant() == ACC_TARGETS_OP)
        {
            user_targets =  TargetsList(eop->lhs());
         /*
            for (tl = eop->lhs(); tl; tl = tl->rhs())
            if (tl->lhs()->variant() == ACC_CUDA_OP)
                //targets[CUDA] = 1;
                user_targets = user_targets | CUDA_DEVICE;
            else if (tl->lhs()->variant() == ACC_HOST_OP)
                //targets[HOST] = 1;
                user_targets = user_targets | HOST_DEVICE;
            //targets_on = 1;
          */
            continue;
        }
        if (eop->variant() == ACC_ASYNC_OP)
        {
            RegionRegimConst(REGION_ASYNC);
            err("Clause ASYNC is not implemented yet", 579, stmt);
            continue;
        }
        switch (eop->variant())
        {
        case(ACC_INOUT_OP) : intent = INTENT_INOUT;   break;
        case(ACC_IN_OP) : intent = INTENT_IN;      break;
        case(ACC_OUT_OP) : intent = INTENT_OUT;     break;
        case(ACC_LOCAL_OP) : intent = INTENT_LOCAL;   break;
        case(ACC_INLOCAL_OP) : intent = INTENT_INLOCAL; break;
        default:                     intent = 0;
            err("Illegal clause in dvmh-directive", 600, stmt);
            continue;//break;
        }
        RegisterVariablesInRegion(eop->lhs(), intent, irgn);
    }

    RegisterUses(irgn);
    RegisterDvmArrays(irgn);

    if (user_targets != 0)
    {
        region_targets = user_targets & dvmh_targets;
        if (region_targets == 0)
            region_targets = HOST_DEVICE;
        if (region_targets != user_targets)
            Warning("Demoting targets for region to %s", DevicesString(region_targets), 611, stmt);
        if ((cur_region->targets & region_targets) != region_targets)
            Error("Impossible to execute region on %s", DevicesString(user_targets), 612, stmt);
        cur_region->targets = region_targets;
    }
    else
    {
        if (cur_region->targets != dvmh_targets)
            Warning("Demoting targets for region to %s", DevicesString(cur_region->targets), 611, stmt);
    }

    //if(!targets_on)
    //   for(i=Ndev-1; i; i--)   // set targets by default
    //     targets[i]=1;
    //if(options.isOn(NO_CUDA)) // by option -noCuda
    //    targets[CUDA] = 0;

    InsertNewStatementAfter(RegionForDevices(irgn, DevicesExpr(cur_region->targets)), cur_st, cur_st->controlParent());

    //InsertNewStatementAfter(StartRegion(irgn),cur_st,cur_st->controlParent());  /*22.11.12*/


    // creating lists of registered variables in procedure
    if (!IN_MAIN_PROGRAM)
    {
        acc_registered_list = SymbolListsUnion(acc_registered_list, acc_array_list);
        registered_uses_list = ExpressionListsUnion(registered_uses_list, uses_list);
    }

    return(cur_st);
}

int TargetsList(SgExpression *tgs)
{
    SgExpression *tl;
    int user_targets = 0;
    for (tl = tgs; tl; tl = tl->rhs())
        if (tl->lhs()->variant() == ACC_CUDA_OP)
            user_targets = user_targets | CUDA_DEVICE;
        else if (tl->lhs()->variant() == ACC_HOST_OP)
            user_targets = user_targets | HOST_DEVICE;
    return (user_targets); 
}

void RegisterVariablesInRegion(SgExpression *evl, int intent, int irgn)
{
    SgExpression *el, *e;
    SgSymbol *s;
    int ilow, ihigh;

    for (el = evl; el; el = el->rhs())
    {
        e = el->lhs();
        s = e->symbol();
        if (e->variant() == CONST_REF || s->attributes() & PARAMETER_BIT)
        {
            by_value_list = AddNewToSymbList(by_value_list, s);
            continue;
        }
        if (isSgVarRefExp(e))
        {   //Warning("%s is not DVM-array",s->identifier(),606,cur_region->region_dir); //!!!
            MarkAsRegistered(s);
            if (!isInUsesList(s))
            {
                by_value_list = AddNewToSymbList(by_value_list, s);
                continue;
            }

            if (intent == INTENT_IN && (CorrectIntent(e)) == INTENT_IN)
            {
                by_value_list = AddNewToSymbList(by_value_list, s);
                continue;
            }
            else
            {
                if(INTERFACE_RTS2)
                   doCallAfter(RegionRegisterScalar(irgn, IntentConst(intent), s)); 
                else
                {
                   doCallAfter(RegisterScalar(irgn, IntentConst(intent), s));
                   doCallAfter(SetVariableName(irgn, s));
                }
            }
            continue;
        }
        if (isSgArrayRefExp(e))
        {
            if (isSgArrayType(s->type())) //is array reference or is not string

            {
                if (!HEADER(s) && !isIn_acc_array_list(s) && !isInSymbList(s, tie_list))  //reduction array is not included in acc_array_list and not registered
                    //!!!  && !HEADER_OF_REPLICATED(s) is wrong: may be used in previous region as not reduction array          
                {      //doCallAfter(RegisterScalar(irgn,IntentConst(intent),s));  //must be destroyed!!!              
                    //Warning("%s is not DVM-array",s->identifier(),606,cur_region->region_dir);
                    continue;
                }

                MarkAsRegistered(s);

                if (!HEADER(s) && HEADER_OF_REPLICATED(s) && *HEADER_OF_REPLICATED(s) == 0)
                    HeaderForNonDvmArray(s, cur_region->region_dir); //creating header (HEADER_OF_REPLICATED) for non-dvm array

                if (!e->lhs())  //whole array
                {
                    if(INTERFACE_RTS2)                     
                        doCallAfter(RegionRegisterArray(irgn, IntentConst(intent), s));
                    else
                    { 
                        doCallAfter(RegisterArray(irgn, IntentConst(intent), s));
                        doCallAfter(SetArrayName(irgn, s));
                    }
                    continue;
                }
                else
                {                    
                    if(INTERFACE_RTS2)                    
                        doCallAfter(RegionRegisterSubArray(irgn, IntentConst(intent), s, SectionBoundsList(e)));           
                    else
                    {
                        ilow = ndvm;
                        ihigh = SectionBounds(e);
                        doCallAfter(RegisterSubArray(irgn, IntentConst(intent), s, ilow, ihigh));                      
                        doCallAfter(SetArrayName(irgn, s));
                    }
                    continue;
                }
                //if( !HEADER(s) )       // deleting created header for RTS
                //  doAssignStmtAfter(DeleteObject(DVM000(*HEADER_OF_REPLICATED(s))));
            }
            else  // scalar variable of type character*(n)
            {
                MarkAsRegistered(s);
                if(INTERFACE_RTS2) 
                    doCallAfter(RegionRegisterScalar(irgn, IntentConst(intent), s)); 
                else
                {
                    doCallAfter(RegisterScalar(irgn, IntentConst(intent), s));                
                    doCallAfter(SetVariableName(irgn, s));
                }
                continue;
            }

        }
    }
}

void RegisterUses(int irgn)
{
    SgExpression *el;

    for (el = uses_list; el; el = el->rhs())
    {    
        if (el->lhs()->variant() == CONST_REF || el->lhs()->symbol()->attributes() & PARAMETER_BIT) // is named constant 
        {
            by_value_list = AddNewToSymbList(by_value_list, el->lhs()->symbol());
            continue;
        }
        if (*VAR_INTENT(el) == EMPTY) continue;  // is registered early by user specification in REGION directive
        
        if (*VAR_INTENT(el) == INTENT_IN)   // this variable doesn't need to be registered
        { // inserting call dvmh_get_actual_variable() before dvm000(i) = region_create()
            where->insertStmtBefore(*GetActualScalar(el->lhs()->symbol()), *cur_region->region_dir->controlParent());
            by_value_list = AddNewToSymbList(by_value_list, el->lhs()->symbol());
            continue;
        }
        if(INTERFACE_RTS2)
            doCallAfter(RegionRegisterScalar(irgn, IntentConst(*VAR_INTENT(el)), el->lhs()->symbol()));
        else
        {
            doCallAfter(RegisterScalar(irgn, IntentConst(*VAR_INTENT(el)), el->lhs()->symbol()));
            doCallAfter(SetVariableName(irgn, el->lhs()->symbol()));
        }

    }
}

void RegisterDvmArrays(int irgn)
{
    symb_list *sl;

    for (sl = acc_array_list; sl; sl = sl->next)
    {
        // is not registered yet
        if ((sl->symb->attributes() & USE_IN_BIT) || (sl->symb->attributes() & USE_OUT_BIT))
        {
            if (!HEADER(sl->symb))
                HeaderForNonDvmArray(sl->symb, cur_region->region_dir); //creating header (HEADER_OF_REPLICATED) for non-dvm array 
            if(INTERFACE_RTS2)
                doCallAfter(RegionRegisterArray(irgn, IntentConst(IntentMode(sl->symb)), sl->symb));
            else
            {
                doCallAfter(RegisterArray(irgn, IntentConst(IntentMode(sl->symb)), sl->symb));            
                doCallAfter(SetArrayName(irgn, sl->symb));
            }
        }
    }
    for (sl = parallel_on_list; sl; sl = sl->next)
    {
        if (sl->symb)
        {
            if (!HEADER(sl->symb))
                HeaderForNonDvmArray(sl->symb, cur_region->region_dir); //creating header (HEADER_OF_REPLICATED) for non-dvm array in TIE-clause

            if(INTERFACE_RTS2)
                doCallAfter(RegionRegisterArray(irgn, IntentConst(EMPTY), sl->symb));
            else
            {     
                doCallAfter(RegisterArray(irgn, IntentConst(EMPTY), sl->symb));
                doCallAfter(SetArrayName(irgn, sl->symb));
            }
        }
    }
}

int IntentMode(SgSymbol *s)
{
    int intent = 0;
    symb_list *sl;
    if ((s->attributes() & USE_IN_BIT) && (s->attributes() & USE_OUT_BIT))
    {
        intent = INTENT_INOUT;
        SYMB_ATTR(s->thesymb) = SYMB_ATTR(s->thesymb) & ~USE_IN_BIT;
        SYMB_ATTR(s->thesymb) = SYMB_ATTR(s->thesymb) & ~USE_OUT_BIT;
    }
    else if (s->attributes() & USE_IN_BIT)
    {
        intent = INTENT_IN;
        SYMB_ATTR(s->thesymb) = SYMB_ATTR(s->thesymb) & ~USE_IN_BIT;
    }
    else if (s->attributes() & USE_OUT_BIT)
    {
        intent = INTENT_INOUT;           //14.03.12 OUT=>INOUT
        SYMB_ATTR(s->thesymb) = SYMB_ATTR(s->thesymb) & ~USE_OUT_BIT;
    }
    if ((sl = isInSymbList(s, parallel_on_list)))
        sl->symb = NULL;  // clear corresponding element of parallel_on_list  

    return(intent);
}

void MarkAsRegistered(SgSymbol *s)
{
    SgExpression *use;


    if (HEADER(s) || HEADER_OF_REPLICATED(s)) //is distributed array 
    {
        IntentMode(s);  //clear  INTENT bits
        return;
    }
    if ((use = isInUsesList(s)) != 0)
        *VAR_INTENT(use) = EMPTY; //set INTENT attribute value to 0
    return;
}

int CorrectIntent(SgExpression *e)
{
    SgExpression *el, *eop;
    int intent = INTENT_IN;
    for (el = cur_region->region_dir->expr(0); el; el = el->rhs())
    {
        eop = el->lhs();
        switch (eop->variant())
        {
        case(ACC_INOUT_OP) : if (isInExprList(e, eop->lhs()))  {
                                 intent = INTENT_INOUT;   return(intent);
        }
                             continue;

        case(ACC_OUT_OP) : if (isInExprList(e, eop->lhs()))  {
                               intent = INTENT_OUT;     return(intent);
        }
                           continue;

        case(ACC_LOCAL_OP) : if (isInExprList(e, eop->lhs()))  {
                                 intent = INTENT_LOCAL;   return(intent);
        }
                             continue;

        case(ACC_INLOCAL_OP) : if (isInExprList(e, eop->lhs()))  {
                                   intent = INTENT_INLOCAL; return(intent);
        }
                               continue;

        default:                    continue;
        }
    }
    return(intent);
}

void doNotForCuda()
{
    cur_region->targets = cur_region->targets & ~CUDA_DEVICE;
}

int isForCudaRegion()
{
    if (cur_region && cur_region->targets & CUDA_DEVICE)
        return(1);
    else
        return(0);
}

char * DevicesString(int targets)
{
    char *str = new char[20];
    str[0] = '\0';
    if (targets & HOST_DEVICE)
        strcpy(str, "HOST ");
    if (targets & CUDA_DEVICE)
        strcat(str, "CUDA");
    return(str);
}

SgExpression *DevicesExpr(int targets)
{
    SgExpression *de = NULL, *e;
    if (targets & HOST_DEVICE)
        de = new SgVarRefExp(DeviceTypeConst(HOST));   //device_const[HOST]);
    if (targets & CUDA_DEVICE)
    {
        e = new SgVarRefExp(DeviceTypeConst(CUDA));    //device_const[CUDA]);
        de = de ? IorFunction(de, e) : e;
    }
    return(de);
}

/*
SgExpression *DevicesExpr(int targets[])
{int i;
SgExpression *de,*e;
for(i=Ndev-1,de=NULL; i; i--)
if (targets[i])
{   e = new SgVarRefExp(device_const[i]);
de = de ? IorFunction(de,e) : e;
}
return(de);
}
*/
SgExpression *HandlerExpr() /* OpenMP */
{
    int i;
    SgExpression *de, *e;
    if (has_max_minloc)
        return(ConstRef(0));

    for (i = Nhandler - 1, de = NULL; i; i--)
    {
        e = new SgVarRefExp(HandlerTypeConst(i));  //handler_const[i]);
        de = de ? IorFunction(de, e) : e;
    }
    return(de);
}

int isIn_acc_array_list(SgSymbol *s)
{
    symb_list *sl;
    if (!s)
        return (0);
    for (sl = acc_array_list; sl; sl = sl->next)
    if (sl->symb == s)
        return(1);
    return(0);
}

void NewRegion(SgStatement *stmt, int n, int data_flag)
{
    region_list * curreg;
    curreg = new region_list;
    curreg->is_data = data_flag;
    curreg->No = n;
    curreg->region_dir = stmt;
    curreg->cur_do_dir = NULL;
    curreg->Lnums = 0;
    curreg->next = cur_region;
    curreg->targets = dvmh_targets;
    cur_region = curreg;
    return;
}

void FlagStatement(SgStatement *st)
{
    st->addAttribute(STATEMENT_GROUP, (void*)1, 0);
}

void MarkAsInsertedStatement(SgStatement *st)
{
    st->addAttribute(INSERTED_STATEMENT, (void*)1, 0);
}

void DeleteNonDvmArrays()
{
    symb_list *sl;
    for (sl = non_dvm_list; sl; sl = sl->next)
    if (HEADER_OF_REPLICATED(sl->symb))
    {        //doCallAfter(  DestroyArray(DVM000(*HEADER_OF_REPLICATED(sl->symb))));
        SgExpression *header_ref = DVM000(*HEADER_OF_REPLICATED(sl->symb));
        doCallAfter(INTERFACE_RTS2 ? ForgetHeader(header_ref) : DeleteObject_H(header_ref));
        *HEADER_OF_REPLICATED(sl->symb) = 0;
    }
}

void StoreLowerBoundsOfNonDvmArray(SgSymbol *ar)
// generating assign statements to
//store lower bounds of array in Header(rank+3:2*rank+2)

{
    int i, rank, ind;
    SgExpression *le;
    rank = Rank(ar);
    ind = *HEADER_OF_REPLICATED(ar);
    for (i = 0; i < rank; i++)
    {
        le = Exprn(LowerBound(ar, i));
        doAssignTo_After(DVM000(ind + rank + 2 + i), le);  //header_ref(ar,rank+3+i)
    }
}

SgExpression *HeaderForArrayInParallelDir(SgSymbol *ar, SgStatement *st)
{
    SgExpression *head = NULL;
    if(HEADER(ar)) 
      return HeaderRef(ar);     
    if(st->expr(0))
    {
       Error("'%s' isn't distributed array", ar->identifier(), 72, st);   
       return NULL;
    }
    if(HEADER_OF_REPLICATED(ar) && *HEADER_OF_REPLICATED(ar) != 0)
       return DVM000(*HEADER_OF_REPLICATED(ar));
    if(!HEADER_OF_REPLICATED(ar)) 
    {
       int *id = new int;
       *id = 0;
       ar->addAttribute(REPLICATED_ARRAY, (void *)id, sizeof(int));      
    }   
    *HEADER_OF_REPLICATED(ar) = ndvm;
    HeaderForNonDvmArray(ar, st);
    return DVM000(*HEADER_OF_REPLICATED(ar));
}

int HeaderForNonDvmArray(SgSymbol *s, SgStatement *stat)
{
    int dvm_ind, static_sign, re_sign, rank, i;
    SgExpression *size_array;

    // creating list of non-dvm-arrays for deleting after region
    if (IN_COMPUTE_REGION)
        non_dvm_list = AddNewToSymbList(non_dvm_list, s);

    rank = Rank(s);
    dvm_ind = ndvm;  //header index
    if (IN_COMPUTE_REGION)
        *HEADER_OF_REPLICATED(s) = dvm_ind;
    ndvm += 2 * rank + DELTA;   // extended header
    if(INTERFACE_RTS2)
    {
        doCallAfter(CreateDvmArrayHeader_2(s, DVM000(dvm_ind), rank, doShapeList(s,stat)));
        if (TestType_RTS2(s->type()->baseType()) == -1)
             Error("Array reference of illegal type in region: %s ", s->identifier(), 583, stat);
        return (dvm_ind);
    }  
    //store lower bounds of array in Header(rank+3:2*rank+2)
    for (i = 0; i < rank; i++)
        doAssignTo_After(DVM000(dvm_ind + rank + 2 + i), Calculate(LowerBound(s, i)));  //header_ref(ar,rank+3+i)    

    static_sign = 1; // staticSign
    size_array = DVM000(ndvm);
    re_sign = 0;     // created array may not be redistributed 

    doCallAfter(CreateDvmArrayHeader(s, DVM000(dvm_ind), size_array, rank, static_sign, re_sign));
    if (TypeIndex(s->type()->baseType()) == -1)
        Error("Array reference of illegal type in region: %s ", s->identifier(), 583, stat);
    where = cur_st;
    doSizeFunctionArray(s, stat);
    cur_st = where;
    return (dvm_ind);
}

void DoHeadersForNonDvmArrays()
{
    symb_list *sl;
    int dvm_ind, static_sign, re_sign, rank, i;
    SgExpression *size_array;
    SgStatement *save = cur_st;
    non_dvm_list = NULL;
    if(!INTERFACE_RTS2)
      cur_st = dvm_parallel_dir->lexNext();
    for (sl = acc_array_list; sl; sl = sl->next)
      if (!HEADER(sl->symb))
      {
        non_dvm_list = AddToSymbList(non_dvm_list, sl->symb); // creating list of non-dvm-arrays for deleting after region
        rank = Rank(sl->symb);
        dvm_ind = ndvm;  //header index
        // adding the attribute  REPLICATED_ARRAY to non-dvm-array
        if (!HEADER_OF_REPLICATED(sl->symb))
        {
            int *id = new int;
            *id = 0;
            sl->symb->addAttribute(REPLICATED_ARRAY, (void *)id, sizeof(int));
        }
        // adding the attribute  DUMMY_ARRAY to non-dvm-array 
        if (!DUMMY_FOR_ARRAY(sl->symb))
        {
            SgSymbol **dummy = new (SgSymbol *);
            *dummy = NULL;
            sl->symb->addAttribute(DUMMY_ARRAY, (void*)dummy, sizeof(SgSymbol *));
        }
        if(*HEADER_OF_REPLICATED(sl->symb) != 0)
            continue;
        *HEADER_OF_REPLICATED(sl->symb) = dvm_ind;
        ndvm += 2 * rank + DELTA;   // extended header
        if(INTERFACE_RTS2)
        {
            doCallAfter(CreateDvmArrayHeader_2(sl->symb, DVM000(dvm_ind), rank, doShapeList(sl->symb,dvm_parallel_dir)));
            if (TestType_RTS2(sl->symb->type()->baseType()) == -1)
                Error("Array reference of illegal type in region: %s ", sl->symb->identifier(), 583, dvm_parallel_dir);
            continue;
        }  

        //store lower bounds of array in Header(rank+3:2*rank+2)
        for (i = 0; i < rank; i++)
            doAssignTo_After(DVM000(dvm_ind + rank + 2 + i), Calculate(LowerBound(sl->symb, i)));  //header_ref(ar,rank+3+i)    

        static_sign = 1; // staticSign
        size_array = DVM000(ndvm);
        re_sign = 0;     // aligned array may not be redistributed 

        doCallAfter(CreateDvmArrayHeader(sl->symb, DVM000(dvm_ind), size_array, rank, static_sign, re_sign));
        if (TypeIndex(sl->symb->type()->baseType()) == -1)
            Error("Array reference of illegal type in parallel loop: %s", sl->symb->identifier(), 583, dvm_parallel_dir);

        where = cur_st;
        doSizeFunctionArray(sl->symb, dvm_parallel_dir);
        cur_st = where;
      }
    if(!INTERFACE_RTS2)
      cur_st = save;
}

int AnalyzeRegion(SgStatement *reg_dir) //AnalyzeLoopBody()  AnalyzeBlock()
{
    SgStatement *stmt, *save, *begin;
    int analysis_err = 0;
    uses_list = NULL;
    acc_array_list = NULL;
    parallel_on_list = NULL;
    tie_list = NULL;
    save = cur_st;
    analyzing = 1;
    
    for (stmt = reg_dir->lexNext(); stmt; stmt = stmt->lexNext())
    {
        cur_st = stmt;

        // does statement belong to statement group of region?
        if (stmt->controlParent() == reg_dir->controlParent() && !in_checksection && !inparloop
            && stmt->variant() != DVM_PARALLEL_ON_DIR && stmt->variant() != OMP_PARALLEL_DIR
            && stmt->variant() != ACC_CHECKSECTION_DIR && stmt->variant() != ACC_END_CHECKSECTION_DIR
            && stmt->variant() != ACC_END_REGION_DIR
            && stmt->variant() != DVM_INTERVAL_DIR && stmt->variant() != DVM_ENDINTERVAL_DIR
           // && stmt->variant() != DVM_ON_DIR && stmt->variant() != DVM_END_ON_DIR
            && stmt->variant() != FORMAT_STAT && stmt->variant() != DATA_DECL)
            FlagStatement(stmt);   // statement belongs to statement group of region  
        // add attribute STATEMENT_GROUP

        switch (stmt->variant())
        {
            // FORMAT_STAT, ENTRY_STAT, DATA_DECL may appear among executable statements
        case ENTRY_STAT: //error
        case CONTAINS_STMT: //error
        case RETURN_STAT:
            err("Illegal statement in region", 578, cur_st);
            continue;
        case STOP_STAT:
            warn("STOP statement in region", 578, cur_st);
            doNotForCuda();
        case FORMAT_STAT:
        case DATA_DECL:
            continue;
        case CONTROL_END:
            if (stmt->controlParent() == cur_func)
            {
                err("Missing END REGION directive", 603, stmt);
                analysis_err = 1;
                goto END_ANALYS;
            }
            else
                break;
        case ASSIGN_STAT:           // Assign statement               
            RefInExpr(stmt->expr(1), _READ_);
            RefInExpr(stmt->expr(0), _WRITE_);
            break;

        case POINTER_ASSIGN_STAT:           // Pointer assign statement               
            RefInExpr(stmt->expr(1), _READ_);   // ???? _READ_ ????
            RefInExpr(stmt->expr(0), _WRITE_);
            break;

        case WHERE_NODE:
            RefInExpr(stmt->expr(0), _READ_);
            RefInExpr(stmt->expr(1), _WRITE_);
            RefInExpr(stmt->expr(2), _READ_);
            break;

        case WHERE_BLOCK_STMT:
        case SWITCH_NODE:           // SELECT CASE ...
        case ARITHIF_NODE:          // Arithmetical IF
        case IF_NODE:               // IF... THEN
        case CASE_NODE:             // CASE ...
        case ELSEIF_NODE:           // ELSE IF...
        case LOGIF_NODE:            // Logical IF
        case WHILE_NODE:            // DO WHILE (...) 
            RefInExpr(stmt->expr(0), _READ_);
            break;

        case COMGOTO_NODE:          // Computed GO TO
            RefInExpr(stmt->expr(1), _READ_);
            break;

        case PROC_STAT:              // CALL
            Call(stmt->symbol(), stmt->expr(0));
            break;

        case FOR_NODE:
            //!!!stmt->symbol()
            RefInExpr(new SgVarRefExp(stmt->symbol()), _WRITE_);
            RefInExpr(stmt->expr(0), _READ_);
            RefInExpr(stmt->expr(1), _READ_);
            break;

        case FORALL_NODE:
        case FORALL_STAT:
            err("FORALL statement", 7, stmt);
            break;

        case ALLOCATE_STMT:
            err("Illegal statement in compute region", 578, cur_st);
            //err("ALLOCATE/DEALLOCATE statement in parallel loop",588,stmt);
            //RefInExpr(stmt->expr(0), _NUL_);           
            break;

        case DEALLOCATE_STMT:
            err("Illegal statement in compute region", 578, cur_st);
            //err("ALLOCATE/DEALLOCATE statement in parallel loop",588,stmt);
            break;

        case DVM_IO_MODE_DIR:
             continue;
        case OPEN_STAT:
        case CLOSE_STAT:
        case INQUIRE_STAT:
            {SgExpression *ioc[NUM__O];
             control_list_open(stmt->expr(1), ioc); // control_list analysis
       /* 
        if (!io_err && !inparloop) {
            err("Illegal elements in control list", 185, stmt);
            break;
        }
        if (ioc[ERR_] && !inparloop){
            err("END= and ERR= specifiers are illegal in FDVM", 186, stmt);
            break;
        }
       */
             //warn("Input/Output statement in region",587,stmt);
             RefInControlList_Inquire(ioc, NUM__O);
             doNotForCuda();
             break;
            }
        case BACKSPACE_STAT:
        case ENDFILE_STAT:
        case REWIND_STAT:
            {SgExpression *ioc[NUM__R];
             control_list1(stmt->expr(1), ioc); // control_list analysis
       /*
        if (!io_err && !inparloop) {
            err("Illegal elements in control list", 185, stmt);
            break;
        }
        if ((ioc[END_] || ioc[ERR_]) && !inparloop)
            err("END= and ERR= specifiers are not allowed in FDVM", 186, stmt);
       */
             //warn("Input/Output statement in region",587,stmt);
             RefInControlList(ioc, NUM__R);
             doNotForCuda();
             break;
            }
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
            {SgExpression *ioc[NUM__R];

             // analizes IO control list and sets on ioc[]                                   
             IOcontrol(stmt->expr(1), ioc, stmt->variant());
      /*
        if (!io_err && !inparloop){
            err("Illegal elements in control list", 185, stmt);
            break;
        }
        if ((ioc[END_] || ioc[ERR_] || ioc[EOR_]) && !inparloop){
            err("END=, EOR= and ERR= specifiers are illegal in FDVM", 186, stmt);
            break;
        }
       */
            //warn("Input/Output statement in region",587,stmt);
            RefInControlList(ioc, NUM__R);
            RefInIOList(stmt->expr(0), (stmt->variant() == READ_STAT ? _WRITE_ : _READ_));
            doNotForCuda();
            break;
           }

        case DVM_PARALLEL_ON_DIR:
            if(!TestParallelDirective(stmt,0,0,NULL))
               continue;     // directive is ignored
            inparloop = 1;
            dvm_parallel_dir = stmt;
               
            ParallelOnList(stmt);  // add target array reference to list 
            TieList(stmt);
            par_do = stmt->lexNext();
            while (par_do->variant() != FOR_NODE)
                par_do = par_do->lexNext();
            DoPrivateList(stmt);

            red_struct_list = NULL;
            CreateStructuresForReductions(DoReductionOperationList(stmt));    
            continue;

        case ACC_END_REGION_DIR:   //end of compute region
            //if(reg_dir->controlParent() == stmt->controlParent())
            goto END_ANALYS;

        case ACC_REGION_DIR:
            err("Nested compute regions are not permitted", 601, stmt);
            //continue;
            goto END_ANALYS;

        case ACC_CHECKSECTION_DIR:
            // omitting statements until section end
            begin = stmt;
            while (stmt && stmt->variant() != ACC_END_CHECKSECTION_DIR && stmt->variant() != ACC_END_REGION_DIR)
            {
                if (stmt->variant() == ACC_ACTUAL_DIR || stmt->variant() == ASSIGN_STAT || stmt->variant() == DVM_PARALLEL_ON_DIR)
                    err("llegal statement/directive in the range of host-section", 572, stmt);
                stmt = stmt->lexNext();
            }
            if (stmt->variant() == ACC_END_CHECKSECTION_DIR)
            {
                if (begin->controlParent() != stmt->controlParent())
                    err("Misplaced directive", 103, stmt); // section must be a block
                continue;
            }

            err("Missing END HOSTSECTION directive in region", 571, stmt);
            if (stmt->variant() != ACC_END_REGION_DIR)
            {
                stmt = stmt->lexPrev();

                continue;
            }
            else
                goto END_ANALYS;

        case ACC_END_CHECKSECTION_DIR:
            err("Unmatched directive", 182, stmt);
            continue;

        case DVM_ON_DIR:
             RefInExpr(stmt->expr(0), _READ_);
             continue;   
        case DVM_END_ON_DIR:     
             continue;

        case ACC_GET_ACTUAL_DIR:
        case ACC_ACTUAL_DIR:

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
            //       case DVM_INTERVAL_DIR:
            //       case DVM_ENDINTERVAL_DIR:
        case DVM_OWN_DIR:
        case DVM_DEBUG_DIR:
        case DVM_ENDDEBUG_DIR:
        case DVM_TRACEON_DIR:
        case DVM_TRACEOFF_DIR:
        case DVM_BARRIER_DIR:
        case DVM_CHECK_DIR:
        case DVM_TASK_REGION_DIR:
        case DVM_END_TASK_REGION_DIR:
            //case DVM_ON_DIR:
            //case DVM_END_ON_DIR:
        case DVM_MAP_DIR:
        case DVM_RESET_DIR:
        case DVM_PREFETCH_DIR:
        case DVM_PARALLEL_TASK_DIR:
        case DVM_LOCALIZE_DIR:
        case DVM_SHADOW_ADD_DIR:
            err("Illegal DVMH-directive in compute region", 577, stmt);
            continue;
        default:
            break;
        }
        {SgStatement *end_stmt;
        end_stmt = isSgLogIfStmt(stmt->controlParent()) ? stmt->controlParent() : stmt;
      
        if (inparloop && isParallelLoopEndStmt(end_stmt,par_do)) //end of parallel loop
        {  
            inparloop = 0; dvm_parallel_dir = NULL; private_list = NULL;  cur_region->cur_do_dir = NULL;
            red_struct_list = NULL;
        }
        }

    } //end for
END_ANALYS:
    cur_st = save;
    analyzing = 0;
    return(analysis_err);
}

int  WithAcrossClause()
{
    SgExpression *el;
    // looking through the specification list
    for (el = dvm_parallel_dir->expr(1); el; el = el->rhs())
    {
        if (el->lhs()->variant() == ACROSS_OP)
            return(1);
    }
    return(0);
}

void ACC_ParallelLoopEnd(SgStatement *pardo)
{   
    AddRemoteAccessBufferList_ToArrayList(); // add to acc_array_list remote_access buffer array symbols

    if (options.isOn(O_HOST))  //dvm-array references in host handler are not linearised (do not changed)
        for_host = 0;

    if (cur_region && cur_region->targets & CUDA_DEVICE)  //if(targets[CUDA]) 
    {
        SgStatement* cuda_kernel = NULL;

        if (WithAcrossClause())
            // creating Cuda-handlers and Cuda-kernels for loop with ACROSS clause.
            Create_C_Adapter_Function_Across(adapter_symb);
        else
        {
            for (unsigned k = 0; k < countKernels; ++k)
            {
                loop_body = CopyOfBody.top();
                CopyOfBody.pop();

                //enabled analysis for each parallel loop for CUDA
                if (options.isOn(LOOP_ANALYSIS))
                    currentLoop = new Loop(loop_body, options.isOn(OPT_EXP_COMP), options.isOn(GPU_IRR_ACC));

                std::string new_kernel_symb = kernel_symb->identifier();
                if (rtTypes[k] == rt_INT)
                    new_kernel_symb += "_int";
                else if (rtTypes[k] == rt_LONG)
                    new_kernel_symb += "_long";
                else if (rtTypes[k] == rt_LLONG)
                    new_kernel_symb += "_llong";

                SgSymbol *kernel_symbol = new SgSymbol(PROCEDURE_NAME, new_kernel_symb.c_str(), *mod_gpu);
                if (options.isOn(C_CUDA))
                    kernel_symbol->setType(C_VoidType());

                if (options.isOn(GPU_O1)) //optimization by option -gpuO1
                {
                    AnalyzeReturnGpuO1 infoGpuO1 = analyzeLoopBody(NON_ACROSS_TYPE);
                    int InternalPosition = -1;
                    for (size_t i = 0; i < infoGpuO1.allArrayGroup.size(); ++i)
                    {
                        for (size_t k = 0; k < infoGpuO1.allArrayGroup[i].allGroups.size(); ++k)
                        {
                            if (infoGpuO1.allArrayGroup[i].allGroups[k].tableNewVars.size() != 0)
                            {
                                InternalPosition = infoGpuO1.allArrayGroup[i].allGroups[k].position;
                                break;
                            }
                        }
                    }

                    if (InternalPosition == -1)
                    {
                        if (k == 0)
                            Create_C_Adapter_Function(adapter_symb);  //creating Cuda-handler for loop
                        cuda_kernel = CreateLoopKernel(kernel_symbol, indexTypeInKernel(rtTypes[k])); //creating Cuda-kernel for loop
                    }
                    else // don't work yet, because only gpuO1 lvl1 enable
                    {
                        if (k == 0)
                            Create_C_Adapter_Function(adapter_symb, InternalPosition);  //creating Cuda-handler for loop with gpuO1
                        cuda_kernel = CreateLoopKernel(kernel_symbol, infoGpuO1, indexTypeInKernel(rtTypes[k])); //creating optimal Cuda-kernel for loop with gpuO1
                    }

                }
                else
                {
                    if (k == 0)
                        Create_C_Adapter_Function(adapter_symb);  //creating Cuda-handler for loop
                    cuda_kernel = CreateLoopKernel(kernel_symbol, indexTypeInKernel(rtTypes[k])); //creating Cuda-kernel for loop				
                }

                if (newVars.size() != 0)
                {
                    correctPrivateList(RESTORE);
                    newVars.clear();
                }

                if (options.isOn(RTC))
                {
                    acc_call_list = ACC_RTC_ExpandCallList(acc_call_list);
                    if (options.isOn(C_CUDA))
                        ACC_RTC_ConvertCudaKernel(cuda_kernel, kernel_symbol->identifier());
                    else
                        ACC_RTC_AddCalledProcedureComment(kernel_symbol);

                    RTC_FKernelArgs.push_back((SgFunctionCallExp *)kernel_st->expr(0));                    
                }

                if (options.isOn(LOOP_ANALYSIS))
                    delete currentLoop;
            }

            if (options.isOn(RTC))
                ACC_RTC_CompleteAllParams();
        }
    }

    // creating host-handler for loop anyway
    if (!WithAcrossClause())
       Create_Host_Loop_Subroutine(hostproc_symb, 0);
    else
    {
        Create_Host_Across_Loop_Subroutine(hostproc_symb);
        first_do_par->extractStmt();
    }

    dvm_ar = NULL;
    if (cur_region)
        cur_region->cur_do_dir = NULL;

    dvm_parallel_dir = NULL;
    return;
}


void ACC_RenewParLoopHeaderVars(SgStatement *first_do, int nloop)
{
    SgStatement *st;
    int i;
    SgForStmt *stdo;
    SgExpression *el, *e;
    SgSymbol *s;

    uses_list = NULL;
    acc_array_list = NULL;
    // looking through the loop nest 
    for (st = first_do, i = 0; i < nloop; st = st->lexNext(), i++)
    {
        stdo = isSgForStmt(st);
        if (!stdo)
            break;
        RefIn_LoopHeaderExpr(stdo->start(), st);
        RefIn_LoopHeaderExpr(stdo->end(), st);
        RefIn_LoopHeaderExpr(stdo->step(), st);
    }
    
    for (el = uses_list; el; el = el->rhs())
    {
        e = el->lhs();
        s = e->symbol();

        if (isSgVarRefExp(e))
        {
            doCallAfter(GetActualScalar(s));   //inserting after current statement
            continue;
        }
        if (isSgArrayRefExp(e))
        {
            if (HEADER(s) || HEADER_OF_REPLICATED(s) && *HEADER_OF_REPLICATED(s) != 0) //is distributed array reference

            {
               doCallAfter(GetActualArray(HEADER(s) ? HeaderRef(s) : DVM000(*HEADER_OF_REPLICATED(s))));   //inserting after current statement
               continue;
            }
            else
            {
               doCallAfter(GetActualScalar(s));   //inserting after current statement
               continue;
            }
        }
    }
    uses_list = NULL;
    return;
}
void CorrectUsesList()
{
    SgExpression *el, *e;
    symb_list *sl,*slp;
    for(el = uses_list, e=NULL; el; el = el->rhs())
    {      
        if(IS_BY_USE(el->lhs()->symbol()))
        { //deleting from list 
          if(e) 
          {
             e->setRhs(el->rhs());  
             el = e;
          }
          else
             uses_list=el->rhs();
        }
        else 
          e = el;
    }
        acc_array_list_whole = CopySymbList(acc_array_list); //to create full base list
        for (sl = acc_array_list,slp = NULL; sl; sl = sl->next)
          if(IS_BY_USE(sl->symb))
             if(slp)
             {
               slp->next = sl->next;
               sl = slp;
             }
             else
               acc_array_list = sl->next;
          else
             slp = sl;   
}               


void ACC_CreateParallelLoop(int ipl, SgStatement *first_do, int nloop, SgStatement *par_dir, SgExpression *clause[], int interface)
{
    int first, last;
    SgStatement *dost;

    if(in_checksection)
        return;            

    ReplaceCaseStatement(first_do);
    FormatAndDataStatementExport(par_dir, first_do);
                      //!printf("loop on gpu %d\n",first_do->lineNumber() );
    dvm_parallel_dir = par_dir;
    first_do_par = first_do;

    if (options.isOn(O_HOST))  //dvm-array references in host handler are not linearised (do not changed)
        for_host = 1;

    // making structures for reductions
    red_struct_list = NULL;
    CreateStructuresForReductions(clause[REDUCTION_] ? clause[REDUCTION_]->lhs() : NULL);

    // creating private_list
    private_list = clause[PRIVATE_] ? clause[PRIVATE_]->lhs() : NULL;
    dost = InnerMostLoop(first_do, nloop);
        
    // error checking
    CompareReductionAndPrivateList();
    TestPrivateList();
    // removing different names of the same variable "by use"
    RemovingDifferentNamesOfVar(first_do);
    // creating uses_list 
    assigned_var_list = NULL;
    for_shadow_compute = clause[SHADOW_COMPUTE_] ? 1 : 0;  // for optimization of shadow_compute 
    uses_list = UsesList(dost->lexNext(), lastStmtOfDo(dost)); 
    RefInExpr(IsRedBlack(nloop), _READ_);   // add to uses_list variables used in  start-expression  of redblack loop
    UsesInPrivateArrayDeclarations(private_list);  // add to uses_list variables used in private array declarations
    if(USE_STATEMENTS_ARE_REQUIRED) // || !IN_COMPUTE_REGION)
        CorrectUsesList();
    for_shadow_compute = 0;
    if (assigned_var_list)
        Error("Variables assign to: %s", SymbListString(assigned_var_list), 586, dvm_parallel_dir);

    // creating replicated arrays for non-dvm-arrays outside regions
    if (!cur_region)
        DoHeadersForNonDvmArrays();

    if (!mod_gpu_symb)
        CreateGPUModule();
                           
    if (!block_C)
        Create_C_extern_block();                        

    if (!info_block)
        Create_info_block();

    adapter_symb = AdapterSymbol(first_do);

    // add #define for adapter name
    block_C->addComment(DefineComment(adapter_symb->identifier()));

    hostproc_symb = HostProcSymbol(first_do);

    kernel_symb = KernelSymbol(first_do);

    loop_body = CopyBodyLoopForCudaKernel(first_do, nloop);

    // for TRACE in acc_f2c.cpp
    number_of_loop_line = first_do->lineNumber();

    // creating buffers for remote_access references (after creating GPU module)
    if (rma && !rma->rmout && !rma->rml->symbol())  // there is synchronous REMOTE_ACCESS clause in PARALLEL directive
        CreateRemoteAccessBuffers();
    if (cur_region)
    {
        // is first loop of compute region
        first = (cur_region->Lnums == 0) ? 1 : 0;
        (cur_region->Lnums)++;

        // is last loop of compute region
        last = (first_do->lastNodeOfStmt()->lexNext()->variant() == ACC_END_REGION_DIR) ? 1 : 0;
        //END_REGION directive follows last statement of parallel loop
    }
    // ---------------------------------------------------
    // Generating statements for loop in source program unit

    if (clause[SHADOW_COMPUTE_] && cur_region) // optimization of SHADOW_COMPUTE in REGION        
        doStatementsForShadowCompute(ipl,interface);     // is based on the result of UsesList()

    doStatementsToPerformByHandler(ipl, adapter_symb, hostproc_symb, 1, interface); // registration of hahdlers and performing with them
                                                                                        
    return;
}


SgStatement *ACC_CreateStatementGroup(SgStatement *first_st)
{
    SgStatement *last_st, *st, *st_end;
    last_st = st = st_end = NULL;
    SgStatement* cuda_kernel = NULL;

    first_do_par = first_st;          
    for (st = first_st; IN_STATEMENT_GROUP(st); st = st->lexNext())
    {                       //printf("begin %d %d\n",st->lineNumber(),st->variant());      
        if (st->variant() == LOGIF_NODE)
            LogIf_to_IfThen(st);
        if (st->variant() == SWITCH_NODE)
            ReplaceCaseStatement(st);
        if ((st->variant() == FOR_NODE) || (st->variant() == WHILE_NODE))
            st = lastStmtOfDo(st);
        else if (st->variant() == IF_NODE)
            st = lastStmtOfIf(st);
        else
            st = st->lastNodeOfStmt();
        last_st = st;
    }
    
    if (!TestGroupStatement(first_st, last_st))
        return(last_st);
    
    // creating uses_list
    uses_list = UsesList(first_st, last_st);

    if (!mod_gpu_symb)
        CreateGPUModule();

    if (!block_C)
        Create_C_extern_block();
    // !!! loop for subgroups of statement group 
    // (subgroup of statements without dvm-array references, statement with dvm-array references )             
    adapter_symb = AdapterSymbol(first_st);
    // add #define for adapter name
    block_C->addComment(DefineComment(adapter_symb->identifier()));

    hostproc_symb = HostProcSymbol(first_st);

    kernel_symb = KernelSymbol(first_st);
    
    // ---------------------------------------------------
    // Generating statements for block (sequence) in source program unit
    cur_st = first_st->lexPrev();//last_st;
    //doStatementsInSourceProgramUnit(first_st, 0, NULL, NULL, adapter_symb, hostproc_symb, 0, NULL, NULL, NULL, NULL);
    doStatementsToPerformByHandler(CreateLoopForSequence(first_st),adapter_symb, hostproc_symb, 0, 1); 
    st_end = cur_st; 
    // ---------------------------------------------------
    if ((cur_region->targets & CUDA_DEVICE)) //if(targets[CUDA])
    {
        // Generating Kernel
        for_kernel = 1;

        for (unsigned k = 0; k < countKernels; ++k)
        {
            std::string new_kernel_symb = kernel_symb->identifier();
            if (rtTypes[k] == rt_INT)
                new_kernel_symb += "_int";
            else if (rtTypes[k] == rt_LONG)
                new_kernel_symb += "_long";
            else if (rtTypes[k] == rt_LLONG)
                new_kernel_symb += "_llong";

            SgSymbol *kernel_symbol = new SgSymbol(PROCEDURE_NAME, new_kernel_symb.c_str(), *mod_gpu);
            if (options.isOn(C_CUDA))
                kernel_symbol->setType(C_VoidType());

            cuda_kernel = CreateKernel_ForSequence(kernel_symbol, first_st, last_st, indexTypeInKernel(rtTypes[k]));

            if (newVars.size() != 0)
            {
                correctPrivateList(RESTORE);
                newVars.clear();
            }

            if (options.isOn(RTC))
            {
                acc_call_list = ACC_RTC_ExpandCallList(acc_call_list);
                if (options.isOn(C_CUDA))
                    ACC_RTC_ConvertCudaKernel(cuda_kernel, kernel_symbol->identifier());
                else
                    ACC_RTC_AddCalledProcedureComment(kernel_symbol);

                RTC_FKernelArgs.push_back((SgFunctionCallExp *)kernel_st->expr(0));                
            }
        }               

        for_kernel = 0;

        // Generating  Adapter (handler) Function
        Create_C_Adapter_Function_For_Sequence(adapter_symb, first_st);

        if (options.isOn(RTC))
            ACC_RTC_CompleteAllParams();
    }
    // Generating host-handler anyway

    Create_Host_Sequence_Subroutine(hostproc_symb, first_st, last_st);

    // return last statement of block    

    return(st_end);
}

int TestGroupStatement(SgStatement *first, SgStatement *last)
{
    SgStatement *st, *end;
    int test = 1;
    has_io_stmt = 0;
    end = last->lexNext();
    for (st = first; st != end; st = st->lexNext())
    if (!TestOneGroupStatement(st))
        test = 0;
    return(test);
}

int TestOneGroupStatement(SgStatement *stmt)
{
    if (isExecutableDVMHdirective(stmt) && stmt->variant() != DVM_ON_DIR && stmt->variant() != DVM_END_ON_DIR)
    {
        err("Misplaced directive", 103, stmt);
        return 0;
    }
    if (stmt->variant() == DATA_DECL || stmt->variant() == FORMAT_STAT)
    {
        err("Illegal statement in the range of region", 576, stmt);
        return 0;
    }
    switch (stmt->variant()) {
    case OPEN_STAT:
    case CLOSE_STAT:
    case INQUIRE_STAT:
    case BACKSPACE_STAT:
    case ENDFILE_STAT:
    case REWIND_STAT:
    case WRITE_STAT:
    case READ_STAT:
    case PRINT_STAT:
        has_io_stmt = 1;
        break;
    }
    return 1;
}


void doStatementsForShadowCompute(int ilh, int interface)
{
    symb_list *sl;

    for (sl = acc_array_list; sl; sl = sl->next)
    {
        if (HEADER(sl->symb))
        {
            if (isOutArray(sl->symb))
                doCallAfter(interface==1 ? LoopShadowCompute_H(ilh, HeaderRef(sl->symb)) : LoopShadowCompute_Array(ilh, HeaderRef(sl->symb))  );
                //doCallAfter(interface==1 ? LoopShadowCompute_H(ilh, HeaderRef(sl->symb)) : LoopShadowCompute_Array(ilh, Register_Array_H2(HeaderRef(sl->symb)))  );
            MarkAsRegistered(sl->symb);
        }
    }
    return;
}


int CreateLoopForSequence(SgStatement *first)
{
        LINE_NUMBER_AFTER(first,cur_st); 
        cur_st->addComment(SequenceComment(first->lineNumber())); 
        int il = ndvm;    
        doAssignStmtAfter(LoopCreate_H(cur_region->No, 0));
        return (il);
}

void  doStatementsToPerformByHandler(int ilh, SgSymbol *adapter_symb, SgSymbol *hostproc_symb,int is_parloop,int interface)
{   SgExpression *arg_list, *base_list, *copy_uses_list, *copy_arg_list, *red_dim_list, *red_bound_list;
    int numb, numb_r, numb_b;
    SgStatement *st_register;

    copy_uses_list = uses_list ? &(uses_list->copy()) : NULL;  //!!!
    base_list = options.isOn(O_HOST) && inparloop ? AddrArgumentList() : BaseArgumentList(); //before ArrayArgumentList call where: dummy_ar=>ar in acc_array_list  
    arg_list = is_parloop ? RemoteAccessHeaderList() : NULL;
    arg_list = AddListToList(arg_list, ArrayArgumentList());
    copy_arg_list = arg_list ? &(arg_list->copy()) : NULL;
    red_dim_list = DimSizeListOfReductionArrays();
    red_bound_list = BoundListOfReductionArrays();
    numb_b = ListElemNumber(red_bound_list);
    numb_r = ListElemNumber(red_dim_list);
    numb = ListElemNumber(arg_list) + ListElemNumber(uses_list);
     
// register CUDA-handler
    if (cur_region && (cur_region->targets & CUDA_DEVICE))  //if(targets[CUDA])  
    {
    
        arg_list = AddListToList(arg_list, copy_uses_list);
        arg_list = AddListToList(arg_list, red_dim_list);
        if(interface == 1)
        {
            InsertNewStatementAfter(RegisterHandler_H(ilh, DeviceTypeConst(CUDA), ConstRef(0), adapter_symb->next(), 0, numb + numb_r), cur_st, cur_st->controlParent()); /* OpenMP */
            AddListToList(cur_st->expr(0), arg_list);
        } else
        {   
            SgExpression *efun = HandlerFunc(adapter_symb->next(), numb + numb_r, arg_list);
            InsertNewStatementAfter(RegisterHandler_H2(ilh, DeviceTypeConst(CUDA), ConstRef(0), efun), cur_st, cur_st->controlParent()); /* OpenMP */      
        }
    }
        //base_list = options.isOn(O_HOST) && inparloop ? addr_list : BaseArgumentList();
    numb = numb + ListElemNumber(base_list);
// register HOST-handler
    int iht = ndvm;
    doAssignStmtAfter(new SgValueExp(0));
    copy_arg_list = AddListToList(copy_arg_list, base_list);
    copy_uses_list = uses_list ? &(uses_list->copy()) : NULL;
    copy_arg_list = AddListToList(copy_arg_list, copy_uses_list);
    copy_arg_list = AddListToList(copy_arg_list, red_bound_list);


    if(interface == 1)
    {
        InsertNewStatementAfter(RegisterHandler_H(ilh, DeviceTypeConst(HOST), DVM000(iht), hostproc_symb, 0, numb+numb_b), cur_st, cur_st->controlParent());  /* OpenMP */
        AddListToList(cur_st->expr(0), copy_arg_list);
    } else
    {
        SgExpression *efun = HandlerFunc(hostproc_symb, numb+numb_b, copy_arg_list);
        InsertNewStatementAfter(RegisterHandler_H2(ilh, DeviceTypeConst(HOST), DVM000(iht), efun), cur_st, cur_st->controlParent());  /* OpenMP */
    }
    cur_st->addComment(OpenMpComment_HandlerType(iht));
// perform by handler    
    InsertNewStatementAfter((interface==1 ? LoopPerform_H(ilh) : LoopPerform_H2(ilh)), cur_st, cur_st->controlParent());
    if (is_parloop)  //inparloop
        cur_st->setComments("! Loop execution\n");
    else
        cur_st->setComments("! Execution\n");       
}

SgExpression *DimSizeListOfReductionArrays()
{//create dimmesion size list for reduction arrays
    reduction_operation_list *rsl;
    int idim;
    SgExpression *ell, *el, *arg, *arg_list;

    if (!red_list)
        return(NULL);
    arg_list = NULL;
    for (rsl = red_struct_list; rsl; rsl = rsl->next)
    {
        if (rsl->redvar_size == -1) //reduction variable is array with passed dimension's sizes      
        {
            el = NULL;
            for (idim = Rank(rsl->redvar); idim; idim--)
            {
                arg = ArrayDimSize(rsl->redvar, idim);
                if (arg && arg->variant() == STAR_RANGE)
                    //arg = SizeFunction(rsl->redvar,idim);
                    Error("Assumed-size array: %s", rsl->redvar->identifier(), 162, dvm_parallel_dir);
                else
                    arg = SizeFunctionWithKind(rsl->redvar, idim, len_DvmType);
                ell = new SgExprListExp(*arg);
                ell->setRhs(el);
                el = ell;
            }
            arg_list = AddListToList(arg_list, el);
            el = NULL;
            for (idim = Rank(rsl->redvar); idim; idim--)
            {
                arg = DvmType_Ref(LBOUNDFunction(rsl->redvar, idim));
                ell = new SgExprListExp(*arg);
                ell->setRhs(el);
                el = ell;
            }
            arg_list = AddListToList(arg_list, el);            
        }
    }

    return(arg_list);
}

SgExpression *isConstantBound(SgSymbol *rv, int i, int isLower)
{
  SgExpression *bound;
  bound = isLower ? Calculate(LowerBound(rv,i)) : Calculate(UpperBound(rv,i));
  if(bound->isInteger())
     return bound;
  else
     return NULL;
}

SgExpression *CreateBoundListOfArray(SgSymbol *ar)
{
    SgExpression *sl = NULL;
    SgSymbol *low_s, *upper_s, *new_ar;
    SgExpression *up_bound, *low_bound;
    int i;
    if(!isSgArrayType(ar->type()))
        return (sl);
    for(i=0;i<Rank(ar); i++) 
    {    
        if(!isConstantBound(ar,i,1))         
           sl = AddListToList( sl,  new SgExprListExp(LowerBound(ar,i)->copy()) );
      
        if(!isConstantBound(ar,i,0)) 
           sl = AddListToList( sl,  new SgExprListExp(UpperBound(ar,i)->copy()) );
    }
    return(sl);
}

SgExpression * BoundListOfReductionArrays()
{
    reduction_operation_list *rl;
    SgExpression *bound_list = NULL;
    for (rl = red_struct_list; rl; rl = rl->next)
    {
        if (rl->redvar_size != 0)
            bound_list = AddListToList(bound_list, CreateBoundListOfArray(rl->redvar)); 
    } 
    return  bound_list;
}

void  ReplaceCaseStatement(SgStatement *first)
{
  SgStatement *stmt, *last_st;
  last_st=lastStmtOf(first);
  for(stmt= first; stmt != last_st; stmt=stmt->lexNext())
  {
     if(stmt->variant() == CASE_NODE)
        //ConstantExpansionInExpr(stmt->expr(0));
        stmt->setExpression(0,*ReplaceParameter(stmt->expr(0)));
  }
}

void FormatAndDataStatementExport(SgStatement *par_dir, SgStatement *first_do)
{
    SgStatement *stmt, *last, *st;
    last = lastStmtOfDo(first_do);
    last = last->lexNext();

    for (stmt = first_do; stmt != last;)
    {
        st = stmt;
        stmt = stmt->lexNext();
        if (st->variant() == DATA_DECL || st->variant() == FORMAT_STAT)
        {
            st->extractStmt();
            par_dir->insertStmtBefore(*st, *par_dir->controlParent());
        }
    }

}

void    CreateStructuresForReductions(SgExpression *red_op_list)
{           
    SgExpression  *er = NULL, *ev = NULL, *ered = NULL, *loc_var_ref = NULL, *en = NULL, *esize = NULL;

    reduction_operation_list *rl = NULL;
    has_max_minloc = 0;

    for (er = red_op_list; er; er = er->rhs())
    {
        ered = er->lhs(); //  reduction  (variant==ARRAY_OP)
        ev = ered->rhs(); // reduction variable reference for reduction operations except MINLOC,MAXLOC 
        loc_var_ref = NULL;

        if (isSgArrayRefExp(ev) && !ev->lhs()) //whole array
            esize = ArrayLengthInElems(ev->symbol(), NULL, 0); 
        else
            esize = NULL;

        if (isSgExprListExp(ev)) //MAXLOC,MINLOC     
        {
            ev = ev->lhs(); // reduction variable reference         
            loc_var_ref = ered->rhs()->rhs()->lhs();        //location array reference
            en = ered->rhs()->rhs()->rhs()->lhs(); // number of elements in location array
            loc_el_num = LocElemNumber(en);
            has_max_minloc = 1;
        }

        // create reduction structure and add to red_struct_list
        {
            reduction_operation_list *redstruct = new reduction_operation_list;

            redstruct->redvar = ev->symbol();
            redstruct->locvar = loc_var_ref ? loc_var_ref->symbol() : NULL;

            redstruct->number = loc_el_num;
            redstruct->redvar_size = esize ? (esize->isInteger() ? esize->valueInteger() : -1) : 0;
            redstruct->array_red_size = redstruct->redvar_size;

            if (Rank(redstruct->redvar) > 1 || redstruct->redvar_size > 16)
                redstruct->redvar_size = -1;
            if (redstruct->redvar_size == -1)
            {
                if (loc_var_ref)
                    Error("Wrong reduction variable %s", ev->symbol()->identifier(), 151, dvm_parallel_dir);
                else if (analyzing)
                    Warning("Reduction variable %s is array of unknown(large) size", ev->symbol()->identifier(), 597, dvm_parallel_dir);
            }
            redstruct->next = NULL;
            redstruct->dimSize_arg = NULL;
            redstruct->lowBound_arg = NULL;
            redstruct->red_host = NULL;
            if (!red_struct_list)
                red_struct_list = rl = redstruct;
            else
            {
                rl->next = redstruct;
                rl = redstruct;
            }
        }
    }
}

void CompareReductionAndPrivateList()
{
    reduction_operation_list *rsl;
    if (!red_struct_list)
        return;
    for (rsl = red_struct_list; rsl; rsl = rsl->next)
    {
        if (isPrivate(rsl->redvar))
            Error("'%s' in REDUCTION and PRIVATE clause", rsl->redvar->identifier(), 609, dvm_parallel_dir);
        if (rsl->locvar && isPrivate(rsl->locvar))
            Error("'%s' in REDUCTION and PRIVATE clause", rsl->locvar->identifier(), 609, dvm_parallel_dir);
    }
    return;
}

void TestPrivateList()
{
    SgExpression *el, *el2;
    for (el = private_list; el; el = el->rhs())
    {
        for (el2 = el->rhs(); el2; el2 = el2->rhs())
        if (ORIGINAL_SYMBOL(el->lhs()->symbol()) == ORIGINAL_SYMBOL(el2->lhs()->symbol()))
            Error("'%s' appears twice in PRIVATE clause", el->lhs()->symbol()->identifier(), 610, dvm_parallel_dir);
    }
    return;
}

void ReplaceSymbolInExpr(SgExpression *e,SgSymbol *symb)
{
    if(!e) return;
    if(isSgVarRefExp(e) || isSgArrayRefExp(e))
    {
       if(ORIGINAL_SYMBOL(e->symbol()) == ORIGINAL_SYMBOL(symb) && e->symbol() != symb)
          e->setSymbol(symb);
       return;
    }
    ReplaceSymbolInExpr(e->lhs(),symb);
    ReplaceSymbolInExpr(e->rhs(),symb);
    return;
}

void ReplaceSymbolInLoop (SgStatement *first, SgSymbol *symb)
{
    SgStatement *last=lastStmtOfDo(first);
    SgStatement *stmt;
    for( stmt=first; stmt!=last; stmt=stmt->lexNext())
    {
        ReplaceSymbolInExpr(stmt->expr(0), symb);
	ReplaceSymbolInExpr(stmt->expr(1), symb);
	ReplaceSymbolInExpr(stmt->expr(2), symb);
    }
}

void RemovingDifferentNamesOfVar(SgStatement *first)
{
    SgExpression *el;
    for (el = private_list; el; el = el->rhs())
    {   
        if(IS_BY_USE(el->lhs()->symbol()))             
            ReplaceSymbolInLoop(first,el->lhs()->symbol());
    }
    reduction_operation_list *rsl;
    for (rsl = red_struct_list; rsl; rsl = rsl->next)
    {
        if (IS_BY_USE(rsl->redvar))
            ReplaceSymbolInLoop(first,rsl->redvar);
        if (rsl->locvar &&  IS_BY_USE(rsl->locvar))
            ReplaceSymbolInLoop(first,rsl->locvar);
    }	
}

void ACC_ReductionVarsAreActual()
{
    reduction_operation_list *rl;

    for (rl = red_struct_list; rl; rl = rl->next)
    {
        doCallAfter(ActualScalar(rl->redvar));
        if (rl->locvar)
            doCallAfter(ActualScalar(rl->locvar));
    }
}

void CreateRemoteAccessBuffers()
{
    SgExpression *el;
    rem_var *remv;
    coeffs *scoef;
    for (el = rma->rml; el; el = el->rhs())
    {
        remv = (rem_var *)(el->lhs())->attributeValue(0, REMOTE_VARIABLE);
        if(!remv) continue; // error case: illegal reference in REMOTE_ACCESS directive/clause 
        remv->buffer = RemoteAccessBufferInKernel(el->lhs()->symbol(), remv->ncolon);
        // creating variables used for optimisation buffer references in parallel loop
        scoef = new coeffs;
        CreateCoeffs(scoef, remv->buffer);
        // scoef = BufferCoeffs(remv->buffer,el->lhs()->symbol());
        // adding the attribute (ARRAY_COEF) to  buffer symbol
        remv->buffer->addAttribute(ARRAY_COEF, (void*)scoef, sizeof(coeffs));
    }
    return;
}

SgSymbol *CreateReplicatedArray(SgSymbol *s)
{
    SgSymbol *ar;

    ar = DummyReplicatedArray(s, Rank(s));

    // renewing attribute DUMMY_ARRAY of symbol s
    *DUMMY_FOR_ARRAY(s) = ar;

    return(ar);
}

/*
void ACC_RegisterDvmBuffer(SgExpression *bufref, int buffer_rank)
{
    SgStatement *call;
    int ilow, j;
    ilow = ndvm;
    for (j = buffer_rank; j; j--)
        doAssignStmtAfter(&(*new SgValueExp(-2147483647) - *new SgValueExp(1)));
    call = RegisterBufferArray(cur_region->No, IntentConst(INTENT_LOCAL), bufref, ilow, ilow);
    cur_st->insertStmtAfter(*call);
    cur_st = call;
    return;
}
*/

void ACC_Before_Loadrb(SgExpression *bufref)
{
    SgStatement *call;
    call = RegionBeforeLoadrb(bufref);
    cur_st->insertStmtAfter(*call);
    cur_st = call;
    return;
}

void ACC_Region_After_Waitrb(SgExpression *bufref)
{
    SgStatement *call;
    if (!cur_region)
        return;
    call = RegionAfterWaitrb(cur_region->No, bufref);
    cur_st->insertStmtAfter(*call);
    cur_st = call;
    return;
}

void ACC_StoreLowerBoundsOfDvmBuffer(SgSymbol *s, SgExpression *dim[], int dim_num[], int rank, int ibuf, SgStatement *stmt)
// generating assign statements to
//store lower bounds of dvm-array in Header(rank+3:2*rank+2) of remote_access buffer

{
    int i;


    if (IS_POINTER(s))
        Error("Fortran 77 dynamic array  %s. Obsolescent feature.", s->identifier(), 575, stmt);

    for (i = 0; i < rank; i++)
    {
        if (dim[i]->variant() == DDOT)   //  ':'
            doAssignTo_After(DVM000(ibuf + rank + 2 + i), header_ref(s, rank + 3 + dim_num[i]));
        else                            // a*I+b   depends on do-variable of parallel loop
        {
            warn("Remote_Access Reference depends on do-variable of parallel loop", 575, stmt);
            doAssignTo_After(DVM000(ibuf + rank + 2 + i), BufferLowerBound(dim[i]));
        }
    }

}

SgExpression *BufferLowerBound(SgExpression *ei)
{
    SgSymbol *dovar;
    SgExpression *e, *do_start;
    dovar = (*IS_DO_VARIABLE_USE(ei))->symbol(); //printf("%s\n",dovar->identifier()); return(new SgValueExp(0));
    do_start = DoStart(dovar);  //redblack ???
    e = &(ei->copy());
    e = ReplaceIndexRefByLoopLowerBound(e, dovar, do_start);  //e->unparsestdout();
    return(e);
}

SgExpression *DoStart(SgSymbol *dovar)
{
    SgStatement *st;
    SgExpression *estart;

    for (st = par_do; st->variant() == FOR_NODE; st = st->lexNext())  //first_do_par not initialized yet
    {
        if (st->symbol() == dovar)
        {
            estart = &((SgForStmt *)st)->start()->copy();   // estart->unparsestdout();
            if (!isSgArrayRefExp(estart)) //redblack
            {
                warn("Remote_access for redblack", 575, st);
                estart = estart->lhs();
            }
            return(estart);
        }
    }
    return(DVM000(0)); //may not be
}

SgExpression  *ReplaceIndexRefByLoopLowerBound(SgExpression *e, SgSymbol *dovar, SgExpression *estart)
{
    if (!e)
        return(e);
    if (isSgVarRefExp(e) && e->symbol() == dovar)
        return(&(estart->copy()));
    e->setLhs(ReplaceIndexRefByLoopLowerBound(e->lhs(), dovar, estart));
    e->setRhs(ReplaceIndexRefByLoopLowerBound(e->rhs(), dovar, estart));
    return(e);
}


void ACC_UnregisterDvmBuffers()
{
    SgExpression *el;
    rem_var *remv;

    if (rma && !rma->rmout && !rma->rml->symbol()) // there is synchronous REMOTE_ACCESS clause in PARALLEL directive
    for (el = rma->rml; el; el = el->rhs())
    {
        remv = (rem_var *)(el->lhs())->attributeValue(0, REMOTE_VARIABLE);
        if(!remv) continue; // error case: illegal reference in REMOTE_ACCESS directive/clause 
        doCallAfter(RegionDestroyRb(cur_region->No, DVM000(remv->index)));
    }
}

void ACC_ShadowCompute(SgExpression *shadow_compute_list, SgStatement *st_shcmp)
{
    // if(shadow_compute_list)
    return;
}

SgExpression *SectionBoundsList(SgExpression *are)
{
    SgExpression *el, *einit[MAX_DIMS], *elast[MAX_DIMS], *bounds_list=NULL;
    SgSymbol *ar = are->symbol(); 
    int rank = Rank(ar);
    int i;
    for (el = are->lhs(), i = 0; el; el = el->rhs(), i++) 
        if(i<MAX_DIMS) {
           Doublet(el->lhs(), ar, i, einit, elast);
           bounds_list = AddElementToList(bounds_list, DvmType_Ref(Calculate(elast[i])));
           bounds_list = AddElementToList(bounds_list, DvmType_Ref(Calculate(einit[i])));
        }
    if (i != rank)
        Error("Wrong number of subscripts specified for '%s'", ar->identifier(), 140, cur_st);    

    return (bounds_list);
}

int SectionBounds(SgExpression *are)
{
    SgExpression *el, *einit[MAX_DIMS], *elast[MAX_DIMS];    //,*estep[MAX_DIMS];
    SgSymbol *ar;
    int init, i, j, rank;
    init = ndvm;
    ar = are->symbol();
    rank = Rank(ar);
    if (!are->lhs()) {  // A => A(:,:, ...,:)
        for (j = rank; j; j--)
            doAssignStmtAfter(&SgUMinusOp(*new SgValueExp(1073741824) * *new SgValueExp(2)));

        return(init);
    }
    if(!TestMaxDims(are->lhs(),ar,cur_st))
        return (0);
    for (el = are->lhs(), i = 0; el; el = el->rhs(), i++)
        Doublet(el->lhs(), ar, i, einit, elast);
    if (i != rank){
        Error("Wrong number of subscripts specified for '%s'", ar->identifier(), 140, cur_st);
        return(0);
    }

    for (j = i; j; j--)
        doAssignStmtAfter(Calculate(einit[j - 1]));
    for (j = i; j; j--)
        doAssignStmtAfter(Calculate(elast[j - 1]));
    //for(j=i; j; j--)
    //     doAssignStmtAfter(estep[j-1]); 
    return(init + rank);
}

void Doublet(SgExpression *e, SgSymbol *ar, int i, SgExpression *einit[], SgExpression *elast[])
{
    SgValueExp c1(1), c0(0);

    if (e->variant() != DDOT) { //is not doublet
        einit[i] = e;                       //&(*e-*Exprn(LowerBound(ar,i)));
        elast[i] = einit[i];

        return;
    }
    // is doublet

    if (!e->lhs())
        einit[i] = &c1.copy();
    else
        einit[i] = e->lhs();                 //&(*(e->lhs())-*Exprn(LowerBound(ar,i)));
    if (!e->rhs())
        elast[i] = Exprn(UpperBound(ar, i));  // &(*Exprn(UpperBound(ar,i))-*Exprn(LowerBound(ar,i)));
    else
        elast[i] = e->rhs();                 //&(*(e->rhs())-*Exprn(LowerBound(ar,i)));   

    return;
}



SgExpression *ArrayArgumentList()
{
    symb_list *sl;
    SgExpression *el, *ell, *list;
    // create dvm-array list for parallel loop
    if (!acc_array_list)
        return(NULL);

    el = list = NULL;
    for (sl = acc_array_list; sl; sl = sl->next)
    {
        if (HEADER(sl->symb))
        {
            ell = new SgExprListExp(*new SgArrayRefExp(*(sl->symb)));
        }
        else  if (HEADER_OF_REPLICATED(sl->symb))
        {
            ell = new SgExprListExp(*DVM000(*HEADER_OF_REPLICATED(sl->symb)));
            sl->symb = CreateReplicatedArray(sl->symb);
        }
        else
            return(list);  //error
        if (el)
        {
            el->setRhs(ell);
            el = ell;
        }
        else
            list = el = ell;

    }
    return(list);
}


SgExpression *RemoteAccessHeaderList()
{
    SgExpression *el, *l, *rma_list;
    rem_var *remv;
    rma_list = NULL;
    if (rma && !rma->rmout && !rma->rml->symbol()) // there is synchronous REMOTE_ACCESS clause in PARALLEL directive
    for (el = rma->rml; el; el = el->rhs())
    {
        remv = (rem_var *)(el->lhs())->attributeValue(0, REMOTE_VARIABLE);
        if(!remv) continue; // error case: illegal reference in REMOTE_ACCESS directive/clause 
        l = new SgExprListExp(*DVM000(remv->index));
        l->setRhs(rma_list);
        rma_list = l;
        //rma_list = AddListToList(rma_list, l );
    }
    return(rma_list);
}

void AddRemoteAccessBufferList_ToArrayList()
{
    SgExpression *el;
    rem_var *remv;
    if (rma && !rma->rmout && !rma->rml->symbol()) // there is synchronous REMOTE_ACCESS clause in PARALLEL directive
    for (el = rma->rml; el; el = el->rhs())
    {
        remv = (rem_var *)(el->lhs())->attributeValue(0, REMOTE_VARIABLE);
        if(!remv) continue; // error case: illegal reference in REMOTE_ACCESS directive/clause 
        acc_array_list = AddNewToSymbList(acc_array_list, remv->buffer);
    }
    return;
}

SgExpression *AddNewToBaseList(SgExpression *base_list, SgSymbol *symb)
{
    SgExpression *el, *l;

    for (l = base_list; l; l = l->rhs())
    if (baseMemory(symb->type()->baseType()) == l->lhs()->symbol()) //baseMemory(l->lhs()->symbol()->type()->baseType()) )
        break;
    if (!l)
    {
        el = new SgExprListExp(*new SgArrayRefExp(*baseMemory(symb->type()->baseType())));
        el->setRhs(base_list);
        base_list = el;
    }
    return(base_list);
}

SgExpression *ElementOfBaseList(SgExpression *base_list, SgSymbol *symb)
{
    SgExpression *el = NULL, *l;

    for (l = base_list; l; l = l->rhs())
    if (baseMemory(symb->type()->baseType()) == l->lhs()->symbol()) //baseMemory(l->lhs()->symbol()->type()->baseType()) )
        break;
    if (!l)
        el = new SgExprListExp(*new SgArrayRefExp(*baseMemory(symb->type()->baseType())));

    return(el);
}


SgExpression *BaseArgumentList()
{
    symb_list *sl, *array_list;
    SgExpression *el, *l, *base_list = NULL;
    // create memory base list
    array_list = NULL;
    // create remote_access objects list
    if (rma && !rma->rmout && !rma->rml->symbol()) // there is synchronous REMOTE_ACCESS clause in PARALLEL directive
    for (el = rma->rml; el; el = el->rhs())
        array_list = AddToSymbList(array_list, el->lhs()->symbol());

    if (array_list)
    {
        base_list = ElementOfBaseList(NULL, array_list->symb);
        for (sl = array_list->next; sl; sl = sl->next)
        {
            l = ElementOfBaseList(base_list, sl->symb);
            if (l)
            {
                l->setRhs(base_list);
                base_list = l;
            }
        }
    }
    array_list = USE_STATEMENTS_ARE_REQUIRED ? acc_array_list_whole : acc_array_list; 
    if (!base_list && array_list)
        base_list = ElementOfBaseList(NULL, array_list->symb);
    for (sl = array_list; sl; sl = sl->next)
    {
        l = ElementOfBaseList(base_list, sl->symb);
        if (l)
        {
            l->setRhs(base_list);
            base_list = l;
        }
    }

    return(base_list);

}



SgExpression *FirstDvmArrayAddress(SgSymbol *ar, int ind)
{
    SgExpression *ae;
    ae = ind ? DVM000(ind) : new SgArrayRefExp(*ar, *new SgValueExp(Rank(ar) + 2));
    return (new SgArrayRefExp(*baseMemory(ar->type()->baseType()), *ae));
}

SgExpression *ElementOfAddrArgumentList(SgSymbol *s)
{
    SgExpression *ae;
    if (HEADER(s))
        ae = new SgArrayRefExp(*s, *new SgValueExp(Rank(s) + 2));
    else if (HEADER_OF_REPLICATED(s))
        ae = DVM000(*HEADER_OF_REPLICATED(s) + Rank(s) + 1);
    else
        ae = DVM000(1); //error
    return(new SgExprListExp(*new SgArrayRefExp(*baseMemory(s->type()->baseType()), *ae)));
}

SgExpression *AddrArgumentList()
{
    symb_list *sl;
    SgExpression *el, *l, *addr_list = NULL, *ae, *rem_list = NULL;
    rem_var *remv;

    // create array address list
    if (acc_array_list)
    {
        addr_list = el = ElementOfAddrArgumentList(acc_array_list->symb);

        for (sl = acc_array_list->next; sl; sl = sl->next)
        {
            l = ElementOfAddrArgumentList(sl->symb);
            el->setRhs(l);
            el = l;
        }
    }
    // create remote_access buffer address list and add it to addr_list
    if (rma && !rma->rmout && !rma->rml->symbol()) // there is synchronous REMOTE_ACCESS clause in PARALLEL directive
    for (el = rma->rml; el; el = el->rhs())
    {
        remv = (rem_var *)(el->lhs())->attributeValue(0, REMOTE_VARIABLE);
        if(!remv) continue;  // error case: illegal reference in REMOTE_ACCESS directive/clause 
        ae = DVM000(remv->index + remv->ncolon + 1);
        l = new SgExprListExp(*new SgArrayRefExp(*baseMemory(el->lhs()->symbol()->type()->baseType()), *ae));
        l->setRhs(rem_list);
        rem_list = l;
    }

    addr_list = AddListToList(rem_list, addr_list);
    return(addr_list);
}

SgStatement *DoStmt(SgStatement *first_do, int i)
{
    SgStatement *stmt;
    int ind;
    for (stmt = first_do, ind = 1; ind < i; ind++)
        stmt = stmt->lexNext();
    return(stmt);
}

void CreateRegionVarList()
{
    SgStatement *reg_dir;
    SgExpression *el, *eop;
    reg_dir = cur_region->region_dir;
    dvm_array_list = NULL;
    do_st_list = NULL;
    for (el = reg_dir->expr(0); el; el = el->rhs())
    {
        eop = el->lhs();
        //dvm_array_list = AddToVarRefList(dvm_array_list,eop->lhs());
        dvm_array_list = AddListToList(dvm_array_list, eop->lhs());
    }
}


SgStatement *InnerMostLoop(SgStatement *dost, int nloop)
{
    int i;
    SgStatement *stmt;
    for (i = nloop - 1, stmt = dost; i; i--)
        stmt = stmt->lexNext();
    return(stmt);
}

void UsesInPrivateArrayDeclarations(SgExpression *privates)
{
    SgExpression *el;
    SgArrayType *tp;
    for (el=privates; el; el=el->rhs())
        if(el->lhs()->symbol() && (tp=isSgArrayType(el->lhs()->symbol()->type())))
            RefInExpr(tp->getDimList(),_READ_); 
}

SgExpression *UsesList(SgStatement *first, SgStatement *last) //AnalyzeLoopBody()  AnalyzeBlock()
{
    SgStatement *stmt, *save;

    uses_list = NULL;
    acc_array_list = NULL;
    acc_call_list = NULL;
    save = cur_st;

    for (stmt = first; stmt != last->lexNext(); stmt = stmt->lexNext())
    {
        cur_st = stmt;          //!printf("in useslist line %d\n",stmt->lineNumber()); 
        if (stmt->lineNumber() == 0)  //inserted debug statement
            continue;

        // FORMAT_STAT, ENTRY_STAT, DATA_DECL may appear among executable statements
        switch (stmt->variant())
        {
        case ASSIGN_STAT:           // Assign statement               
            RefInExpr(stmt->expr(1), _READ_);
            RefInExpr(stmt->expr(0), _WRITE_);
            break;

        case POINTER_ASSIGN_STAT:           // Pointer assign statement               
            RefInExpr(stmt->expr(1), _READ_);   // ???? _READ_ ????
            RefInExpr(stmt->expr(0), _WRITE_);
            break;

        case WHERE_NODE:
            RefInExpr(stmt->expr(0), _READ_);
            RefInExpr(stmt->expr(1), _WRITE_);
            RefInExpr(stmt->expr(2), _READ_);
            break;

        case WHERE_BLOCK_STMT:
        case SWITCH_NODE:           // SELECT CASE ...
        case ARITHIF_NODE:          // Arithmetical IF
        case IF_NODE:               // IF... THEN
        case CASE_NODE:             // CASE ...
        case ELSEIF_NODE:           // ELSE IF...
        case LOGIF_NODE:            // Logical IF
        case WHILE_NODE:            // DO WHILE (...) 
            RefInExpr(stmt->expr(0), _READ_);
            break;

        case COMGOTO_NODE:          // Computed GO TO
            RefInExpr(stmt->expr(1), _READ_);
            break;

        case PROC_STAT:              // CALL
            //err("Call statement in parallel loop",589,stmt);
            Call(stmt->symbol(), stmt->expr(0));
            break;

        case FOR_NODE:
            if (inparloop && !isPrivate(stmt->symbol()))
                assigned_var_list = AddNewToSymbListEnd(assigned_var_list, stmt->symbol());
            //Error("Index variable %s should be specified as private",stmt->symbol()->identifier(),585,stmt);
            if (!inparloop)
                RefInExpr(new SgVarRefExp(stmt->symbol()), _WRITE_);
            RefInExpr(stmt->expr(0), _READ_);
            RefInExpr(stmt->expr(1), _READ_);
            break;

        case FORALL_NODE:
        case FORALL_STAT:
            //err("FORALL statement",7,stmt); 
            break;

        case ALLOCATE_STMT:
            //err("ALLOCATE/DEALLOCATE statement in region",588,stmt);
            //RefInExpr(stmt->expr(0), _NUL_);           
            break;

        case DEALLOCATE_STMT:
            //err("ALLOCATE/DEALLOCATE statement in region",588,stmt);
            break;
        case OPEN_STAT:
        case CLOSE_STAT:
        case INQUIRE_STAT:
        {SgExpression *ioc[NUM__O];
        control_list_open(stmt->expr(1), ioc); // control_list analysis            
        RefInControlList_Inquire(ioc, NUM__O);
        break;
        }
        case BACKSPACE_STAT:
        case ENDFILE_STAT:
        case REWIND_STAT:
        {SgExpression *ioc[NUM__R];
        control_list1(stmt->expr(1), ioc); // control_list analysis
        RefInControlList(ioc, NUM__R);
        break;
        }
        case WRITE_STAT:
        case READ_STAT:
        case PRINT_STAT:
        {SgExpression *ioc[NUM__R];
        // analyzes IO control list and sets on ioc[]                               
        IOcontrol(stmt->expr(1), ioc, stmt->variant());
        RefInControlList(ioc, NUM__R);
        RefInIOList(stmt->expr(0), (stmt->variant() == READ_STAT ? _WRITE_ : _READ_));
        break;
        }
        default:
            break;
        }


    } //end for
    cur_st = save;
    return(uses_list);
}

void Add_Use_Module_Attribute()
{
   if(!USE_STATEMENTS_ARE_REQUIRED)
   {
      int *index = new int;
      *index = 0;   
      first_do_par->addAttribute(MODULE_USE, (void *) index, sizeof(int));  
   }
}

void RefInExpr(SgExpression *e, int mode)
{
    int i;
    SgExpression *el, *use;
    if (!e)
        return;

    if (!analyzing && inparloop && mode == _WRITE_ && !isSgArrayRefExp(e) && e->symbol()  && !isPrivate(e->symbol()) && !isReductionVar(e->symbol()) )  //  && !HEADER(e->symbol()) && !IS_CONSISTENT(e->symbol())
        //Error("Assign to %s",e->symbol()->identifier(),586,cur_st);
        assigned_var_list = AddNewToSymbListEnd(assigned_var_list, e->symbol());

    //if(e->variant() == CONST_REF && isInUsesList(e->symbol()) != NULL)
    //   return;
    if (e->variant() == VAR_REF || e->variant() == CONST_REF || e->variant() == ARRAY_REF && e->symbol()->type()->variant() == T_STRING)
    {                                //!printf("refinExpr: var %s\n",e->symbol()->identifier());
        SgType *tp = e->symbol()->type();
        if (tp->variant() == T_DERIVED_TYPE && (IS_BY_USE(tp->symbol()) || IS_BY_USE(e->symbol())))
            Add_Use_Module_Attribute();
        if (inparloop && isParDoIndexVar(e->symbol()))   //index of parallel loop
            return;
        if (inparloop && isPrivate(e->symbol()))
            return;
        if (inparloop && isReductionVar(e->symbol()))
            return;
        
        if ((use = isInUsesList(e->symbol())) != 0)
        {                              //!printf("RefInExpr  2 (is in list) %d\n",VAR_INTENT(use));
                                       //uses_list ->unparsestdout();  printf("\n");
            *VAR_INTENT(use) = WhatMode(*VAR_INTENT(use), mode);
            return;
        }
        
        i = tp->variant();
        
        if (inparloop && !analyzing)            
            if (i == T_DERIVED_TYPE && !IS_BY_USE(tp->symbol()) && !IS_BY_USE(e->symbol())  || (i == T_STRING && TypeSize(tp) != 1))    //|| i==T_COMPLEX || i==T_DCOMPLEX
            {
               Error("Variable reference %s of illegal type in parallel loop", e->symbol()->identifier(), 583, cur_st);            
            }
        use = new SgExprListExp(*e);
        uses_list = AddListToList(uses_list, use);
        {
            int *id = new int;
            *id = WhatMode(mode,mode); 
            use->addAttribute(INTENT_OF_VAR, (void *)id, sizeof(int));
        }        
        return;
    }

    if (isSgArrayRefExp(e))
    {          //!printf("refinExpr: array %s\n",e->symbol()->identifier());
        for (el = e->lhs(), i = 1; el; el = el->rhs(), i++)
            RefInExpr(el->lhs(), _READ_);  //Index(el->lhs(),use,i);
        SgType *tp = e->symbol()->type();
        if (tp->variant()==T_ARRAY && tp->baseType()->variant()==T_DERIVED_TYPE && (IS_BY_USE(tp->baseType()->symbol()) || IS_BY_USE(e->symbol())))
            Add_Use_Module_Attribute();

        if (HEADER(e->symbol()))    //dvm-array
        {
            if (!analyzing && inparloop && mode != _WRITE_  && isRemAccessRef(e))
                return;
            if (inparloop && isPrivate(e->symbol()))
                return;
            acc_array_list = AddNewToSymbList(acc_array_list, e->symbol());
            if (analyzing || for_shadow_compute)
                MarkArraySymbol(e->symbol(), mode);
            return;
        }
        // non-dvm-array

        if (inparloop && isPrivate(e->symbol()))
            return;
        if (inparloop && isReductionVar(e->symbol()))
            return;

        acc_array_list = AddNewToSymbList(acc_array_list, e->symbol());

        if (analyzing)
        {
            MarkArraySymbol(e->symbol(), mode);
            // adding the attribute  REPLICATED_ARRAY to non-dvm-array
            if (!HEADER_OF_REPLICATED(e->symbol()))
            {
                int *id = new int;
                *id = 0;
                e->symbol()->addAttribute(REPLICATED_ARRAY, (void *)id, sizeof(int));
            }
            // adding the attribute  DUMMY_ARRAY to non-dvm-array 
            if (!DUMMY_FOR_ARRAY(e->symbol()))
            {
                SgSymbol **dummy = new (SgSymbol *);
                *dummy = NULL;
                e->symbol()->addAttribute(DUMMY_ARRAY, (void*)dummy, sizeof(SgSymbol *));
            }
        }
        return;
    }

    if (isSgFunctionCallExp(e))
    {
        Call(e->symbol(), e->lhs());
        //err("Function Call  in parallel loop",589,cur_st);
        return;
    }
    if (e->variant() == ARRAY_OP)
    {
        if (inparloop && !analyzing)
            Error("Substring reference %s in parallel loop", e->lhs()->symbol()->identifier(), 583, cur_st);
        RefInExpr(e->lhs(), mode);
        RefInExpr(e->rhs(), _READ_);
        return;
    }
    if (isSgRecordRefExp(e))
    {
        SgExpression *estr = LeftMostField(e);
        if(analyzing)
          doNotForCuda(); 
        SgExpression *erec = e;
        while(isSgRecordRefExp(erec))
        {
             RefInExpr(RightMostField(erec)->lhs(),_READ_);
             erec = erec->lhs();
        }                  
        RefInExpr(erec->lhs(),_READ_);
        SgType *tp =  estr->symbol()->type(); 
        if(isSgArrayType(tp))
           tp = tp->baseType(); 
        if(IS_BY_USE(tp->symbol()) || IS_BY_USE(estr->symbol()))
        {
          Warning("Structure component reference %s in parallel loop/region", estr->symbol()->identifier(), 582, cur_st);
          Add_Use_Module_Attribute();
          //printf("structure reference:: %s of TYPE %s\n", estr->symbol()->identifier(),estr->symbol()->type()->symbol()->identifier());
        }
        else
          Error("Structure component reference %s in parallel loop/region", estr->symbol()->identifier(), 582, cur_st);
             //StructureRef(e,mode);
        RefInExpr(estr,mode);        
        return;
    }

    RefInExpr(e->lhs(), mode);
    RefInExpr(e->rhs(), mode);

    return;
}

void RefIn_LoopHeaderExpr(SgExpression *e, SgStatement *dost)
{
    SgExpression *el, *use;

    if (!e)
        return;
    if (e->variant() == VAR_REF)
    {
        if ((use = isInUsesList(e->symbol())) != 0)
            return;

        use = new SgExprListExp(*e);
        uses_list = AddListToList(uses_list, use);
        return;
    }

    if (isSgArrayRefExp(e))
    {
        for (el = e->lhs(); el; el = el->rhs())
            RefIn_LoopHeaderExpr(el->lhs(), dost);
        
        if(!(use= isInUsesList(e->symbol())))
        {
          use = new SgExprListExp(*new SgArrayRefExp(*e->symbol()));
          uses_list = AddListToList(uses_list,use);
        }
          
        // Warning("Array reference %s in parallel loop",e->symbol()->identifier(),584,dost);

        return;
    }

    if (e->variant() == ARRAY_OP)
    {
        Warning("Substring reference %s in parallel loop", e->symbol()->identifier(), 583, dost);
        RefIn_LoopHeaderExpr(e->lhs(), dost);
        RefIn_LoopHeaderExpr(e->rhs(), dost);
        return;
    }
    if (isSgRecordRefExp(e))
    {   
        SgSymbol *s = LeftMostField(e)->symbol();
        Warning("Structure component reference %s in parallel loop/region", s->identifier(), 582, dost);
        if(!(use= isInUsesList(s)))
        {
          use = new SgExprListExp(*new SgVarRefExp(*s));
          uses_list = AddListToList(uses_list,use);
        }       
        return;
    }

    RefIn_LoopHeaderExpr(e->lhs(), dost);
    RefIn_LoopHeaderExpr(e->rhs(), dost);

    return;
}

void RefInControlList(SgExpression *eoc[], int n)
{
    int i;
    if (!eoc[UNIT_]) // PRINT
        ;
    else if (eoc[UNIT_]->type()->variant() == T_INT)  //external file
        RefInExpr(eoc[UNIT_], _READ_);
    else  // internal file = variable of character type
        RefInExpr(eoc[UNIT_], _WRITE_);
    for (i = 1; i < n; i++)
    if (i == IOSTAT_)
        RefInExpr(eoc[i], _WRITE_);
    else
        RefInExpr(eoc[i], _READ_);
}

void RefInControlList_Inquire(SgExpression *eoc[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    if (i == U_ || i == ER_ || i == FILE_)
        RefInExpr(eoc[i], _READ_);
    else
        RefInExpr(eoc[i], _WRITE_);
}

void RefInIOList(SgExpression *iol, int mode)
{
    SgExpression *el, *e;
    for (el = iol; el; el = el->rhs()) {
        e = el->lhs();  // list item
        if (analyzing)
            ReplaceFuncCall(e);
        if (isSgExprListExp(e)) // implicit loop in output list
            e = e->lhs();
        if (isSgIOAccessExp(e))
            RefInImplicitLoop(e, mode);
        else
            RefInExpr(e, mode);  //RefInIOitem(e,mode);     
    }

}

void RefInImplicitLoop(SgExpression *eim, int mode)
{
    SgExpression *ell, *e;
    if (isSgExprListExp(eim->lhs()))
    for (ell = eim->lhs(); ell; ell = ell->rhs())  //looking through item list of implicit loop
    {
        e = ell->lhs();
        if (isSgExprListExp(e)) // implicit loop in output list
            e = e->lhs();
        if (isSgIOAccessExp(e))
            RefInImplicitLoop(e, mode);
        else
            RefInExpr(e, mode);
    }
    else
        RefInExpr(eim->lhs(), mode);

    return;
}

/*void RefInIOitem(SgExpression *e, int mode)
{}*/

int WhatMode(int mode, int mode_new)
{   //17.08.16
    if (mode == mode_new && mode == _READ_)    
        return(mode);
    else
        return(_READ_WRITE_);

}

void MarkArraySymbol(SgSymbol *ar, int mode)
{
    if (mode == _READ_)
        SYMB_ATTR(ar->thesymb) = SYMB_ATTR(ar->thesymb) | USE_IN_BIT;
    else if (mode == _WRITE_)
        SYMB_ATTR(ar->thesymb) = SYMB_ATTR(ar->thesymb) | USE_OUT_BIT;
    else if (mode == _READ_WRITE_)
    {
        SYMB_ATTR(ar->thesymb) = SYMB_ATTR(ar->thesymb) | USE_IN_BIT;
        SYMB_ATTR(ar->thesymb) = SYMB_ATTR(ar->thesymb) | USE_OUT_BIT;
    }
}

int isOutArray(SgSymbol *s)
{
    if (s->attributes() & USE_OUT_BIT)
        return(1);
    else
        return(0);
}

int isPrivate(SgSymbol *s)
{
    SgExpression *el;
    for (el = private_list; el; el = el->rhs())
    {
        if (ORIGINAL_SYMBOL(el->lhs()->symbol()) == ORIGINAL_SYMBOL(s))
            return(1);
    }
    return(0);
}

int isPrivateInRegion(SgSymbol *s)
{
    if (IN_COMPUTE_REGION && inparloop && isPrivate(s))
        return(1);
    else
        return(0);
}

int is_acc_array(SgSymbol *s)
{
    if (HEADER(s) && isIn_acc_array_list(s) ||
        DUMMY_FOR_ARRAY(s) && isIn_acc_array_list(*DUMMY_FOR_ARRAY(s)))
        return 1;
    else
        return 0;
}

int isReductionVar(SgSymbol *s)
{
    reduction_operation_list *rl;
    for (rl = red_struct_list; rl; rl = rl->next)
    { 
        if(ORIGINAL_SYMBOL(rl->redvar) == ORIGINAL_SYMBOL(s))
            return(1);
        if (rl->locvar && ORIGINAL_SYMBOL(rl->locvar) == ORIGINAL_SYMBOL(s))
            return(1);
    }
    return(0);
}

SgExpression *isInUsesList(SgSymbol *s)
{
    
    SgExpression *el;
    for (el = uses_list; el; el = el->rhs())
    {
        if (el->lhs()->symbol() == s)
            return(el);
    }
    return(NULL);
}

SgExpression *isInUsesListByChar(const char *symb)
{

    SgExpression *el;
    for (el = uses_list; el; el = el->rhs())
    {
        if (strcmp(el->lhs()->symbol()->identifier(), symb) == 0)
            return(el);
    }
    return(NULL);
}

int isParDoIndexVar(SgSymbol *s)
{
    SgExpression *vl;
    if (!dvm_parallel_dir)
        return(0);
    for (vl = dvm_parallel_dir->expr(2); vl; vl = vl->rhs())
    {
        if (vl->lhs()->symbol() == s)
            return(1);
    }
    return(0);
}

int isByValue(SgSymbol *s)
{
    return(isInByValueList(s));
}

int isInByValueList(SgSymbol *s)
{
    symb_list *sl;
	for (sl = by_value_list; sl; sl = sl->next)
	{
		if (sl->symb == s)
			return(1);
	}
    return(0);
}

SgExpression *DoReductionOperationList(SgStatement *par)
{
    SgExpression *el;

    // looking through the specification list of PARALLEL directive
    for (el = par->expr(1); el; el = el->rhs())
    if (el->lhs()->variant() == REDUCTION_OP)
    {
        return (el->lhs()->lhs());
    }
    return(NULL);
}

void ParallelOnList(SgStatement *par)
{
    if(par->expr(0))
       parallel_on_list = AddNewToSymbList(parallel_on_list, par->expr(0)->symbol());
}

void TieList(SgStatement *par)
{
    SgExpression *el, *es;
    for(el=par->expr(1); el; el=el->rhs()) 
       if(el->lhs()->variant() == ACC_TIE_OP)            // TIE specification
       {
         for(es=el->lhs()->lhs(); es; es=es->rhs())
         {
            SgSymbol *s = es->lhs()->symbol();
            if (!HEADER(s) && !HEADER_OF_REPLICATED(s))
            {
                int *id = new int;
                *id = 0;
                s->addAttribute(REPLICATED_ARRAY, (void *)id, sizeof(int));
            }

            tie_list = AddNewToSymbList(tie_list, s);
            parallel_on_list = AddNewToSymbList(parallel_on_list, s);
         }
         return;
       }
}

void DoPrivateList(SgStatement *par)
{
    SgExpression *el;
    private_list = NULL;

    // looking through the specification list of PARALLEL directive
    for (el = par->expr(1); el; el = el->rhs())
    if (el->lhs()->variant() == ACC_PRIVATE_OP)
    {
        private_list = el->lhs()->lhs();
        break;
    }
    UsesInPrivateArrayDeclarations(private_list);
}

void CreatePrivateAndUsesVarList()
{
    SgExpression *el, *eop;
    SgStatement *do_dir;

    private_list = NULL;
    //uses_list = NULL; 
    do_dir = cur_region->cur_do_dir;
    if (!do_dir)
        return;

    for (el = do_dir->expr(0); el; el = el->rhs())
    {
        eop = el->lhs();
        if (eop->variant() == ACC_PRIVATE_OP)
        {   //private_list = AddToVarRefList(private_list,eop->lhs());
            private_list = AddListToList(private_list, eop->lhs());
            continue;
        }
        /*
           if(eop->variant()==ACC_USES_OP)
           {   //uses_list = AddToVarRefList(uses_list,eop->lhs());
           uses_list = AddListToList(uses_list,eop->lhs());
           continue;
           }
           */
    }

    /*
    // compare two list
    for(el=private_list; el; el=el->rhs())
    {
    for(el2=uses_list; el2; el2=el2->rhs())
    if(el2->lhs()->symbol() == el->lhs()->symbol() && el2->lhs()->symbol()->variant()==VAR_REF)
    Error("%s in USES and PRIVATE clause",el->lhs()->symbol()->identifier(),605,do_dir);
    }
    */
    return;
}

SgSymbol *FunctionResultVar(SgStatement *func)
{
    if (func->expr(0))
        return(func->expr(0)->symbol());
    else
        return(func->symbol());
}


void Argument(SgExpression *e, int i, SgSymbol *s)
{
    int variant;
    if(e->variant() == LABEL_ARG) return; //!!! illegal
    if(e->variant() == KEYWORD_ARG) 
        Argument(e->rhs(), findParameterNumber(ProcedureSymbol(s), NODE_STR(e->lhs()->thellnd)), s);
    if (e->variant() == CONST_REF)
    {
        RefInExpr(e, _READ_);
        return;
    }
    if (isSgVarRefExp(e))
    {
        variant = e->symbol()->variant();  /*printf("argument %s\n", e->symbol()->identifier());*/
        if ((variant == FUNCTION_NAME && e->symbol() != FunctionResultVar(cur_func)) || variant == PROCEDURE_NAME || variant == ROUTINE_NAME)
            return;
        RefInExpr(e, isInParameter(ProcedureSymbol(s),i) ? _READ_ : _READ_WRITE_); 
        return;
    }
    else if (isSgArrayRefExp(e))
    {
        RefInExpr(e, _READ_WRITE_);
        return;
    }
    else if (e->variant() == ARRAY_OP)
    {
        RefInExpr(e->lhs(), _READ_WRITE_);
        RefInExpr(e->rhs(), _READ_);
        return;
    }
    else
    {
        RefInExpr(e, _READ_);
        return;
    }
}


void Call(SgSymbol *s, SgExpression *e)
{
    SgExpression *el;
    int i;

    if (DECL(s) == 2)    //is statement function
    {
        RefInExpr(e, _READ_);
        if (inparloop && analyzing)
            Error("Call of statement function  %s in parallel loop", s->identifier(), 581, cur_st);

        if (IN_STATEMENT_GROUP(cur_st) && analyzing)
            Error("Call of statement function  %s in region", s->identifier(), 581, cur_st);
        return;
    }
    if (IsInternalProcedure(s) && analyzing)
        Error(" Call of the procedure %s in a region, which is internal/module procedure", s->identifier(), 580, cur_st);
   
    if (!isUserFunction(s) && isIntrinsicFunctionName(s->identifier())) //IsNoBodyProcedure(s)
    {
        RefInExpr(e, _READ_);
        return;
    }

//if (inparloop || IN_STATEMENT_GROUP(cur_st))
//{
    if (analyzing)
    {  
        if ((!IsPureProcedure(s) && (s->variant() != FUNCTION_NAME || !options.isOn(NO_PURE_FUNC))) || IS_BY_USE(s))
        {
            Warning(" Call of the procedure %s in a region, which is not pure or is module procedure", s->identifier(), 580, cur_st);
            doNotForCuda();
        }
    }
    else
    {
        if (IN_COMPUTE_REGION && isForCudaRegion() && (IsPureProcedure(s) || (s->variant() == FUNCTION_NAME && options.isOn(NO_PURE_FUNC)) ))  //pure procedure call from the region witch is preparing for CUDA-device        
            MarkAsCalled(s);
        acc_call_list = AddNewToSymbList(acc_call_list, s);
    }
//}
    if (!e)  //argument list is absent
        return;
    in_arg_list++;
    for (el = e, i = 0; el; el = el->rhs(), i++)
        Argument(el->lhs(), i, s);
    in_arg_list--;

    return;
}

SgExpression * AddListToList(SgExpression *list, SgExpression *el)
{
    SgExpression  *l;

    //adding the expression list 'el' to the expression list 'list'

    if (!list) {
        list = el;

    }
    else {
        for (l = list; l->rhs(); l = l->rhs())
            ;
        l->setRhs(el);
    }
    return(list);
}


SgExpression * ExpressionListsUnion(SgExpression *list, SgExpression *alist)
{
    SgExpression  *l, *el, *first;

    //adding the expression list 'alist' to the expression list 'list' without repeating

    if (!list)
        return(alist);

    first = list;

    for (el = alist; el;)
    if (isInExprList(el->lhs(), first))
        el = el->rhs();
    else
    {
        l = el;
        el = el->rhs();
        l->setRhs(list);
        list = l;
        //AddListToList(list,l);
    }

    return(list);
}

SgExpression *isInExprList(SgExpression *e, SgExpression *list)
{
    SgExpression *el;
    SgSymbol *s;
    s = e->symbol();
    if (!s)
        return(NULL);
    for (el = list; el; el = el->rhs())
    {
        if (el->lhs() && el->lhs()->symbol() == s)
            return(el);
    }
    return(NULL);

}


symb_list *SymbolListsUnion(symb_list *slist1, symb_list *slist2)
{
    symb_list  *l, *sl, *first;

    //adding the symbol list 'slist2' to the symbol list 'slist1' without repeating

    if (!slist1)
        return(slist2);

    first = slist1;

    for (sl = slist2; sl;)
    if (isInSymbList(sl->symb, first) != NULL)
        sl = sl->next;
    else
    {
        l = sl;
        sl = sl->next;
        l->next = slist1;
        slist1 = l;

    }

    return(slist1);
}

symb_list *isInSymbList(SgSymbol *s, symb_list *slist)
{
    symb_list *sl;
    for (sl = slist; sl; sl = sl->next)
    if (sl->symb == s)
        return(sl);
    return(NULL);
}

int ListElemNumber(SgExpression *list)
{
    SgExpression  *l;
    int n = 0;
    if (!list) return(0);
    for (l = list; l; l = l->rhs())
        n = n + 1;
    return(n);
}

SgExpression * AddToVarRefList(SgExpression *list, SgExpression *list2)
{
    SgExpression  *l, *el;

    //adding the expression 'el' to the expression list 'list'
    for (el = list2; el; el = el->rhs())
    if (!list) {
        list = el;
        el->setRhs(NULL);
    }
    else {
        for (l = list; l; l = l->rhs())
        {
            if (l->lhs()->symbol() == el->lhs()->symbol() && el->lhs()->variant() == VAR_REF)
                continue;
        }
        el->setRhs(list);
        list = el;
    }
    return(list);
}


void AddToRedVarList(SgExpression *ev, int i)
{
    SgExpression *el, *el1;
    el1 = new SgExprListExp(*ev);
    //el2 = new SgExprListExp(*new SgArrayRefExp(*red_offset_symb,*new SgValueExp(i)));
    if (!red_var_list)
    {
        red_var_list = el1;
        //el1 -> setRhs(el2);
        return;
    }
    el = red_var_list;
    while (el->rhs())
        el = el->rhs();
    el->setRhs(el1);
    //el1 -> setRhs(el2);
    return;
}


SgExpression *CreateActualLocationList(SgSymbol *locvar, int numb)
{
    SgExprListExp *sl, *sll;
    int i;
    if (!locvar) return(NULL);

    sl = NULL;
    for (i = numb; i; i--)
    {
        sll = new SgExprListExp(*new SgArrayRefExp(*locvar, *LocVarIndex(locvar, i)));
        sll->setRhs(sl);
        sl = sll;
    }
    return(sl);
}

/*
SgExpression *CreateRedOffsetVarList()
{ SgExpression *el,*newl,*ell;
SgSymbol *s,*soff;
reduction_operation_list *rsl;
//char *name;
formal_red_offset_list = newl= NULL;
//for(el=red_var_list;el;el=el->rhs())
for(rsl=red_struct_list;rsl;rsl=rsl->next)
{  //s =el->lhs()->symbol();
s = rsl->redvar;
soff = RedOffsetSymbolInKernel(s);
ell = new SgExprListExp(*new SgVarRefExp(*soff));
if(!formal_red_offset_list)
formal_red_offset_list = newl = ell;
else
{   newl->setRhs(ell);
newl = ell;
}
if(rsl->locvar)
{ soff = RedOffsetSymbolInKernel(rsl->locvar);
ell = new SgExprListExp(*new SgVarRefExp(*soff));
newl->setRhs(ell);
newl = ell;
}
}
return(formal_red_offset_list);
}
*/
/*
void AddFormalArg_For_LocArrays()
{ SgExpression *el;
reduction_operation_list *rsl;

el = formal_red_offset_list;
if(!el) return;

while(el->rhs())
el=el->rhs();

//el - last element of formal_red_offset_list

for(rsl=red_struct_list;rsl;rsl=rsl->next)
{
if(rsl->locvar)
{
el->setRhs(rsl->formal_arg);
while(el->rhs())
el=el->rhs();
}
}
}
*/
/*
void AddActualArg_For_LocArrays()
{ //add to red_var_list (to end of argument list)
SgExpression *el;
reduction_operation_list *rsl;

el = red_var_list;
if(!el) return;

while(el->rhs())
el=el->rhs();

//el - last element of red_var_list

for(rsl=red_struct_list;rsl;rsl=rsl->next)
{
if(rsl->locvar)
{
el->setRhs(rsl->actual_arg);
while(el->rhs())
el=el->rhs();
}
}
}
*/
/*
SgExpression *FindUsesInFormalArgumentList()
{ SgExpression *el,*cl;
cl = kernel_st->expr(0);
//cl->unparsestdout(); printf("COPY END\n");
for(el=argument_list,cl = kernel_st->expr(0); el!=uses_list && el!=red_var_list; el=el->rhs(),cl = cl->rhs())
;

return(cl);
}
*/

SgType *IndexType()
{
    return(SgTypeInt()); //!!!!!
}

int KindOfIndexType()
{
    return(4); //!!!!!
}

SgType *CudaIndexType()
{
    SgType *type;
    if (undefined_Tcuda)
        return(FortranDvmType());

    type = new SgType(T_INT);
    TYPE_KIND_LEN(type->thetype) = (new SgExpression(KIND_OP, new SgValueExp(4), NULL, NULL))->thellnd;
    return(type); //!!!!!
}

SgType *CudaOffsetType()
{
    SgType *type;
    if (!undefined_Tcuda)
        return(FortranDvmType());

    type = new SgType(T_INT);
    TYPE_KIND_LEN(type->thetype) = (new SgExpression(KIND_OP, new SgValueExp(4), NULL, NULL))->thellnd;
    return(type); //!!!!!
}

int KindOfCudaIndexType()
{
    return(4); //!!!!!
}

SgStatement *CopyBlockToKernel(SgStatement *first_st, SgStatement *last_st)
{
    SgStatement *st, *st_end, *last, *st_copy;
    int no;
    st_end = kernel_st->lastNodeOfStmt();
    for (st = first_st; IN_STATEMENT_GROUP(st); st = st->lexNext())
    {
        if ((st->variant() == FOR_NODE) || (st->variant() == WHILE_NODE))
        {
            last = LastStatementOfDoNest(st);
            if (last != (st->lastNodeOfStmt()) || last->variant() == LOGIF_NODE)
            {
                last = ReplaceBy_DO_ENDDO(st, last); //ReplaceLabelOfDoStmt(st,last, GetLabel());
                //ReplaceDoNestLabel_Above(last,first_do,GetLabel());
            }
        }
        st_copy = st->copyPtr();

        st_end->insertStmtBefore(*st_copy, *kernel_st);
        //replace label identification (it's not correct!!!)
        if (st->hasLabel())
        {
            no = LABEL_STMTNO(st->label()->thelabel);
            LABEL_STMTNO(st_copy->label()->thelabel) = no;
        }
        if ((st->variant() == FOR_NODE) || (st->variant() == WHILE_NODE))
            st = lastStmtOfDo(st); //last_st
        //      else if(st->variant() == IF_NODE && st->lastNodeOfStmt()->variant()==ELSEIF_NODE)

        else
            st = st->lastNodeOfStmt();

    }
    if (options.isOn(C_CUDA))
        kernel_st->lexNext()->addComment("// Sequence of statements\n");
    else
        kernel_st->lexNext()->addComment("! Sequence of statements\n");

    return(kernel_st->lexNext());
}


void TransferBlockToHostSubroutine(SgStatement *first_st, SgStatement *last_st, SgStatement *st_end)
{
    first_st->addComment("! Sequence of statements\n");
    TransferStatementGroup(first_st,last_st,st_end);
    TranslateFromTo(first_st,st_end,1);
}

/*
void LookTroughTheStatementOfSequenceForDvmAssign(SgStatement *st,SgStatement *stend)
{ SgStatement *stmt;

for(stmt=st; stmt!=stend; stmt=stmt->lexNext())
if( st->variant()==ASSIGN_STAT && isDistObject(st->expr(0)) )
{ if( !isSgArrayType(st->expr(0)->type())){ //array element
ReplaceByIfWithTestFunction(TranslateBlock (st));
} else

}
*/

void TestDvmObjectAssign(SgStatement *st)
{
    if (isDistObject(st->expr(0)))
    {
        if (!isSgArrayType(st->expr(0)->type())) //array element
            ReplaceAssignByIfForRegion(st);
        else //array section or whole array
            err("Illegal statement in the range of region ", 576, st);
    }
}

void ReplaceAssignByIfForRegion(SgStatement *stmt)
{
    ReplaceContext(stmt);


    ReplaceAssignByIf(stmt);

}

SgStatement *CopyBodyLoopForCudaKernel(SgStatement *first_do, int nloop)
{
    int ndo;
    SgStatement *st, *copy_st;
    //!printf("loop rank = %d\n",nloop);
    for (st = first_do, ndo = 0; ndo < nloop; st = ((SgForStmt *)st)->body())
        ndo++;
    if (dvm_debug)
    while (st->lineNumber() == 0) //inserted debug statement
        st = st->lexNext();
    //if(nloop>3)
    //err("Not implemented yet.Rank of loop is greater than 3.",599,first_do);
    //!printf("in copy body\n"); 
    copy_st = st->copyBlockPtr(SAVE_LABEL_ID); //&(st->copy());

    //create loop body copies
    unsigned stackSize = CopyOfBody.size();
    for (size_t i = 0; i < stackSize; ++i)
        CopyOfBody.pop();
    for (int i = 0; i < countKernels * nloop; ++i)
        CopyOfBody.push(st->copyBlockPtr(SAVE_LABEL_ID));

    return(copy_st);
}

/*!!!
SgStatement *CopyBodyLoopToKernel(SgStatement *first_do)
{ SgExpression *vl,*dovar,*erb;
int nloop, ndo;
SgStatement *st,*copy_st,*stend,*last, *stk, *for_st;
SgSymbol *sind;
SgForStmt *stdo;

// looking through the do_variables list
vl = dvm_parallel_dir->expr(2); // do_variables list
for(dovar=vl,nloop=0; dovar; dovar=dovar->rhs())
nloop++;
//!!!printf("nloop:%d\n",nloop);
// looking through the loop nest
erb=NULL;
for(st=first_do,ndo=0; ndo<nloop; st=((SgForStmt *)st)->body())
{   //!!!printf("line number: %d,  %d\n",st->lineNumber(),((SgForStmt *)st)->body()->lineNumber());
if(((SgForStmt *)st)->start()->variant()==ADD_OP) //redblack scheme
{ erb = ((SgForStmt *)st)->start()->rhs(); // MOD function call
erb = &(erb->lhs()->lhs()->copy());   //first argument of MOD function call
erb-> setLhs(new SgVarRefExp(st->symbol()));
for_st = st;
}
ndo++;
}
//!!!printf("line number of st: %d,  %d\n",st->lineNumber(), st);
if(nloop>3)
err("Not implemented yet.Rank of loop is greater 3.",599,first_do);


//  copy_st = &first_do->copy();
//  cur_in_kernel->insertStmtAfter(*copy_st);

//  for(st=copy_st,ndo=0; ndo<nloop-1; st=st->lexNext())
//     ndo++;

//  while(ndo--)
//  { //sind = st->symbol();
//    last = st->lastNodeOfStmt();
//    if(last->variant()!=CONTROL_END)
//  continue;
//    {InsertNewStatementAfter(new SgStatement(CONTROL_END),last,st);
//     last=
//    st-> setVariant(IF_NODE);
//    st->setExpression(0,*KernelCondition(st->symbol(),ndo));
//    BIF_LL2(st->thebif) = NULL;
//     BIF_LL3(st->thebif) = NULL;
//    st=st->controlParent();
//  }


copy_st=st->copyBlockPtr(); //&(st->copy());
if(erb)
{   st = new SgIfStmt(*ConditionForRedBlack(erb),*copy_st);
copy_st = st;
}

last = cur_in_kernel->lexNext();
cur_in_kernel->insertStmtAfter(*copy_st, *cur_in_kernel);
copy_st->addComment("! Loop body\n");
stk = erb ? last->lexPrev()->lexPrev(): last->lexPrev();
if(stk->variant()==CONTROL_END )
if(stk->hasLabel())
stk->setVariant(CONT_STAT);
else
stk->extractStmt();


//last = cur_in_kernel->controlParent()->lastNodeOfStmt();
//last = copy_st->lastNodeOfStmt();
//  last = last->lexPrev();
// if(last->variant()==CONTROL_END && last->controlParent()==cur_in_kernel->controlParent())
//    last->extractStmt();
//copy_st->extractStmt();

return(last);
}
*/


/*
SgExpression *TypeSizeCExpr(SgType *type)
{ int size;
size = TypeSize(type);
// if integer,real,doublepresision, but no complex,bool
return(& SgSizeOfOp(*new SgTypeRefExp(*type)));
}
*/

char *ParallelLoopComment(int line)
{
    char *cmnt = new char[35];
    sprintf(cmnt, "! Parallel loop (line %d)\n", line);
    return(cmnt);
}

char *OpenMpComment_InitFlags(int idvm)
{
    char *cmnt = new char[80];
    sprintf(cmnt, "!$    %s = %s \n", UnparseExpr(DVM000(idvm)), UnparseExpr(&(*DVM000(idvm) + *new SgValueExp(8))));
    return(cmnt);
}

char *OpenMpComment_HandlerType(int idvm)
{
    char *cmnt = new char[80];
    sprintf(cmnt, "!$    %s = %s \n", UnparseExpr(DVM000(idvm)), UnparseExpr(HandlerExpr()));
    return(cmnt);
}

char *SequenceComment(int line)
{
    char *cmnt = new char[60];
    sprintf(cmnt, "! Sequence of statements (line %d)\n", line);
    return(cmnt);
}

char *RegionComment(int line)
{
    char *cmnt = new char[35];
    sprintf(cmnt, "! Start region (line %d)\n", line);
    return(cmnt);
}

char *EndRegionComment(int line)
{
    char *cmnt = new char[35];
    sprintf(cmnt, "! Region end (line %d)\n", line);
    return(cmnt);
}

char *Host_LoopHandlerComment()
{
    char *cmnt = new char[100];
    sprintf(cmnt, "!     Host handler for loop on line %d \n\n", first_do_par->lineNumber());
    return(cmnt);
}

char *Host_SequenceHandlerComment(int lineno)
{
    char *cmnt = new char[120];
    sprintf(cmnt, "!     Host handler for sequence of statements on line %d \n\n", lineno);
    return(cmnt);
}

char *Indirect_ProcedureComment(int lineno)
{
    char *cmnt = new char[130];
    sprintf(cmnt, "!     Indirect distribution: procedures for statement on line %d \n\n", lineno);
    return(cmnt);
}

char *CommentLine(const char *txt)
{
    char *cmnt;
    cmnt = (char *)malloc((unsigned)(strlen(txt) + 5));
    if (options.isOn(C_CUDA))
        sprintf(cmnt, "// %s", txt);
    else
        sprintf(cmnt, "! %s\n", txt);

    return(cmnt);
}

char *IncludeComment(const char *txt)
{
    char *cmnt;
    cmnt = (char *)malloc((unsigned)(strlen(txt) + 12));
    sprintf(cmnt, "#include %s\n", txt);
    return(cmnt);
}

char *DefineComment(char *txt)
{
    char *cmnt;
    cmnt = (char *)malloc((unsigned)(2 * strlen(txt) + 12));
    sprintf(cmnt, "#define %s %s", txt, txt);
    cmnt[2 * strlen(txt) + 8] = '\n';
    cmnt[2 * strlen(txt) + 9] = '\0';
    return(cmnt);
}

const char *CudaIndexTypeComment()
{
    const char *cmnt = NULL;

    cmnt = "typedef int __indexTypeInt; \n"
        "typedef long long __indexTypeLLong;\n";

    return cmnt;
}

char *CalledProcedureComment(const char *txt, SgSymbol *symb)
{
    char *cmnt = new char[strlen(txt) + strlen(symb->identifier()) + 20];
    char *tmp = aks_strlowr(txt);
    sprintf(cmnt, "//DVMH_CALLS %s:%s\n", symb->identifier(), tmp);
    delete []tmp;
    return(cmnt);
}


SgExpression *ThreadsGridSize(SgSymbol *s_threads)
{
    SgExpression *tgs;
    tgs = &((*new SgRecordRefExp(*s_threads, "x")) * (*new SgRecordRefExp(*s_threads, "y")) * (*new SgRecordRefExp(*s_threads, "z")));
    return(tgs);
}

SgSymbol *isSymbolWithSameNameInTable(SgSymbol *first_in, char *name)
{
    SgSymbol *s;
    for (s = first_in; s; s = s->next())
    {
        if (!strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

/***************************************************************************************/
/*    Unparsing To .cuf and .cu File                                                   */
/***************************************************************************************/

void UnparseTo_CufAndCu_Files(SgFile *f, FILE *fout_cuf, FILE *fout_C_cu, FILE *fout_info) /*ACC*/
{
    SgStatement *stat, *stmt;

    if (!mod_gpu) return;

    if (!GeneratedForCuda())   //if(options.isOn(NO_CUDA) || !kernel_st)
    {
        if (info_block)
            info_block->extractStmt();
        if (block_C_Cuda)
            block_C_Cuda->extractStmt();
        mod_gpu->extractStmt();
        if(block_C)
            block_C->extractStmt();
        return;
    }

    if (options.isOn(C_CUDA))
    {
        // unparsing info_block to fout_info
        if (info_block)
        {
            fprintf(fout_info, "%s", UnparseBif_Char(info_block->thebif, C_LANG));
            info_block->extractStmt();
        }
        // unparsing C-Cuda  block to fout_C_cu
        //block_C_Cuda->setVariant(EXTERN_C_STAT);  //10.12.13
        if ( block_C_Cuda)
        {
            fprintf(fout_C_cu, "%s", UnparseBif_Char(block_C_Cuda->thebif, C_LANG));
            block_C_Cuda->extractStmt();
        }
        // unparsing Module of C-Cuda-kernels to fout_C_cu
        //mod_gpu ->setVariant(EXTERN_C_STAT);  //10.12.13//26.12.14
        fprintf(fout_C_cu, "%s", UnparseBif_Char(mod_gpu->thebif, C_LANG));
        mod_gpu->extractStmt();
        // unparsing C Adapter Functions to fout_C_cu
        if (block_C)
        {
            block_C->setVariant(EXTERN_C_STAT);
            fprintf(fout_C_cu, "%s", UnparseBif_Char(block_C->thebif, C_LANG));
            block_C->extractStmt();
        }
        return;
    }

    // grab the first statement in the file.
    stat = f->firstStatement(); // file header
    stmt = stat->lexNext();

    // unparsing info_block to fout_info
    if (info_block)
    {
        fprintf(fout_info, "%s", UnparseBif_Char(info_block->thebif, C_LANG));
        info_block->extractStmt();
    }
    // unparsing C Adapter Functions to fout_C_cu   (!! C before Fortran because tabulation )
    //block_C->setSymbol(*mod_gpu_symb);
    if (block_C)
    {
        block_C->setVariant(EXTERN_C_STAT);
        fprintf(fout_C_cu, "%s", UnparseBif_Char(block_C->thebif, C_LANG));
        block_C->extractStmt();
    }
    // unparsing Module of Fortran-Cuda-kernels to fout_cuf (!!Fortran after C because tabulation)
    fprintf(fout_cuf, "%s", UnparseBif_Char(mod_gpu->thebif, FORTRAN_LANG));
    mod_gpu->extractStmt();

    /*
      while( stmt!=mod_gpu)
      {   printf("function C: %s \n", stmt->expr(0)->symbol()->identifier());
      fprintf(fout_C_cu,"%s",UnparseBif_Char(stmt->thebif,C_LANG));
      st_func = stmt;
      stmt=stmt->lastNodeOfStmt()->lexNext();
      st_func->extractStmt();
      }
      */

}

void UnparseForDynamicCompilation(FILE *fout_cpp)
{
    SgStatement *stmt;
    stmt = mod_gpu->lexNext();
    while (stmt->variant() != CONTROL_END)
    {   //printf("%d\n",stmt->variant());
        BIF_CMNT(stmt->thebif) = NULL;
        char *unp_buf = UnparseBif_Char(stmt->thebif, C_LANG);
        //char *buff = new char[strlen(unp_buf) + 1];
        //sprintf(buff, "const char *%s = ""extern ""C""  %s"";""", stmt->symbol()->identifier(),unp_buf); 
        fprintf(fout_cpp, "const char *%s = \"extern \"C\"  %s\";\n\n", stmt->symbol()->identifier(), unp_buf);
        //delete []buff;
        stmt = stmt->lastNodeOfStmt()->lexNext(); //printf("%d\n",stmt->variant());
    }

}

/***************************************************************************************/
/*    Creating New File                                                                */
/***************************************************************************************/
int Create_New_File(char *file_name, SgFile *file, char *fout_name)

{
    SgFile *fcuf;
    FILE *fout;
    char *new_file_name, *dep_file_name;
    int ll;
    // old file
    printf("  in 0 Create_New_File\n");
    mod_gpu->extractStmt();
    ll = strlen(file_name) + 1;
    dep_file_name = (char *)malloc((unsigned)ll);
    strcpy(dep_file_name, file_name);
    *(dep_file_name + ll - 3) = 'd';
    *(dep_file_name + ll - 2) = 'e';
    *(dep_file_name + ll - 1) = 'p';
    file->saveDepFile(dep_file_name);

    // new file
    printf("  in 1 Create_New_File\n");
    fcuf = new SgFile(0, "dvm_gpu");

    fcuf->firstStatement()->insertStmtAfter(*mod_gpu);
    fcuf->saveDepFile("dvm_gpu.dep");
    printf(" in 2  Create_New_File\n");
    //fcuf->unparsestdout();

    new_file_name = (char *)malloc((unsigned)(strlen(file_name) + 10));
    sprintf(new_file_name, "dvm_gpu_%s", fout_name);

    if ((fout = fopen(new_file_name, "w")) == NULL) {
        (void)fprintf(stderr, "Can't open file %s for write\n", new_file_name);
        return 1;
    }
    fcuf->unparse(fout);
    fclose(fout);
    printf(" in 3  Create_New_File \n");
    return 0;
}

/***************************************************************************************/
/*ACC*/
/*   Creating and Inserting New Statement in the Program                               */
/* (Fortran Language, .f file)                                                         */
/***************************************************************************************/
/*
void InsertUseStatementForGpuModule()
{
if((fmask[LOOP_GPU] == 0) && (fmask[LOOPNS_GPU] == 0) )   // has been generated kernels
return;
SgStatement * st_use = new SgStatement(USE_STMT);
st_use->setSymbol(*mod_gpu_symb);
if(cur_func->controlParent()->variant() == MODULE_STMT)
cur_func->controlParent()->insertStmtAfter(*st_use,*cur_func->controlParent());
else
cur_func->insertStmtAfter(*st_use,*cur_func);
}
*/

SgStatement *doIfThenConstrForLoop_GPU(SgExpression *ref, SgStatement *endhost, SgStatement *dowhile)
{
    SgStatement *ifst;
    // SgExpression *ea;
    // creating
    //          IF ( ref .EQ. 0) THEN
    //                <endhost>
    //          ELSE
    //                <dowhile>
    //          ENDIF 
    // 

    ifst = new SgIfStmt(SgEqOp(*ref, *new SgValueExp(0)), *endhost, *dowhile);
    cur_st->insertStmtAfter(*ifst, *cur_st->controlParent());

    //  ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
    return(ifst);
}


SgExpression * TranslateReductionToOpenmp(SgExpression *reduction_clause)  /* OpenMP */
{
    SgExprListExp *explist, *OpenMPReductions;
    SgExpression *clause;
    SgExprListExp *red_max, *red_min, *red_sum, *red_product;
    SgExprListExp *red_and, *red_eqv, *red_neqv;
    SgExprListExp *red_or;
    int i, length;
    red_max = red_min = red_sum = red_product = red_or = red_and = red_eqv = red_neqv = NULL;
    OpenMPReductions = NULL;
    explist = isSgExprListExp(reduction_clause);
    if (explist == NULL) return NULL;
    length = explist->length();
    for (i = 0; i < length; i++) {
        clause = explist->elem(i);
        switch (clause->variant()) {
        case ARRAY_OP: {
                           if ((clause->lhs() != NULL) && (clause->rhs() != NULL)) {
                               if (clause->lhs()->variant() == KEYWORD_VAL) {
                                   char *reduction_name = NODE_STRING_POINTER(clause->lhs()->thellnd);
                                   if (!strcmp(reduction_name, "max")) {
                                       if (red_max != NULL) red_max->append(*clause->rhs());
                                       else red_max = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "min")) {
                                       if (red_min != NULL) red_min->append(*clause->rhs());
                                       else red_min = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "sum")) {
                                       if (red_sum != NULL) red_sum->append(*clause->rhs());
                                       else red_sum = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "product")) {
                                       if (red_product != NULL) red_product->append(*clause->rhs());
                                       else red_product = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "or")) {
                                       if (red_or != NULL) red_or->append(*clause->rhs());
                                       else red_or = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "and")) {
                                       if (red_and != NULL) red_and->append(*clause->rhs());
                                       else red_and = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "eqv")) {
                                       if (red_eqv != NULL) red_eqv->append(*clause->rhs());
                                       else red_eqv = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "neqv")) {
                                       if (red_neqv != NULL) red_neqv->append(*clause->rhs());
                                       else red_neqv = new SgExprListExp(*clause->rhs());
                                       continue;
                                   }
                                   if (!strcmp(reduction_name, "maxloc")) {
                                       return NULL;
                                   }
                                   if (!strcmp(reduction_name, "minloc")) {
                                       return NULL;
                                   }
                               }

                           }
                           break;
        }

        }
    }
    SgKeywordValExp *kwd;
    SgExpression *ddot;
    SgExpression *red;
    if (red_max != NULL) {
        kwd = new SgKeywordValExp("max");
        ddot = new SgExpression(DDOT, kwd, red_max, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_min != NULL) {
        kwd = new SgKeywordValExp("min");
        ddot = new SgExpression(DDOT, kwd, red_min, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_sum != NULL) {
        kwd = new SgKeywordValExp("+");
        ddot = new SgExpression(DDOT, kwd, red_sum, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_product != NULL) {
        kwd = new SgKeywordValExp("*");
        ddot = new SgExpression(DDOT, kwd, red_product, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_eqv != NULL) {
        kwd = new SgKeywordValExp(".eqv.");
        ddot = new SgExpression(DDOT, kwd, red_eqv, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_neqv != NULL) {
        kwd = new SgKeywordValExp(".neqv.");
        ddot = new SgExpression(DDOT, kwd, red_neqv, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_or != NULL) {
        kwd = new SgKeywordValExp(".or.");
        ddot = new SgExpression(DDOT, kwd, red_or, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    if (red_and != NULL) {
        kwd = new SgKeywordValExp(".and.");
        ddot = new SgExpression(DDOT, kwd, red_and, NULL);
        red = new SgExpression(OMP_REDUCTION, ddot, NULL, NULL);
        if (!OpenMPReductions) OpenMPReductions = new SgExprListExp(*red);
        else OpenMPReductions->append(*red);
    }
    return OpenMPReductions;
}

SgStatement *Interface(SgSymbol *s)
{
    SgStatement *interface = hasInterface(s); 
    if (!interface)
    {
        interface = getInterface(s);
        if (isForCudaRegion())
        { 
            SaveInterface(s,interface); 
            MarkAsUserProcedure(s);
        } 
    }
    return interface;
}

SgStatement *getInterface(SgSymbol *s)
{
    enum { SEARCH_INTERFACE, CHECK_INTERFACE, FIND_NAME };

    SgStatement *searchStmt = cur_func->lexNext();
    SgStatement *tmp;
    const char *funcName = s->identifier();
    const char *toCmp;    int mode = SEARCH_INTERFACE;
    //search internal function
    searchStmt = cur_func->lexNext();
    //search interface in declare section
    while (searchStmt && !isSgExecutableStatement(searchStmt))
    {
        switch (mode)
        {
        case SEARCH_INTERFACE:
            if (searchStmt->variant() != INTERFACE_STMT)
                searchStmt = searchStmt->lexNext();
            else
                mode = CHECK_INTERFACE;
            break;
        case CHECK_INTERFACE:
            if (searchStmt->symbol())
                toCmp = searchStmt->symbol()->identifier();
            else
                toCmp = "";

            if (searchStmt->symbol() && strcmp(toCmp, funcName) != 0)
            {
                searchStmt = searchStmt->lastNodeOfStmt()->lexNext();
                mode = SEARCH_INTERFACE;
            }
            else
            {
                if(searchStmt->symbol())
                {
                    // not implemented yet for Cuda!!!!
                    if(isForCudaRegion() && analyzing)
                    {  
                        Warning("Generic interface is not supported for Cuda: %s",searchStmt->symbol()->identifier(),647,searchStmt);
                        doNotForCuda();
                    }
                    return searchStmt;
                }
                else
                {
                    mode = FIND_NAME;
                    searchStmt = searchStmt->lexNext();
                }
            }
            break;
        case FIND_NAME:
            if (searchStmt->variant() == FUNC_HEDR || searchStmt->variant() == PROC_HEDR)
            {
                if (!strcmp(searchStmt->symbol()->identifier(), funcName))
                    return searchStmt;
                else
                    searchStmt = searchStmt->lastNodeOfStmt()->lexNext();
            }
            else if (searchStmt->variant() == MODULE_PROC_STMT)
                searchStmt = searchStmt->lastNodeOfStmt()->lexNext();
            else if (searchStmt->variant() == CONTROL_END)
            {
                mode = SEARCH_INTERFACE;
                searchStmt = searchStmt->lexNext();
            }
            break;
        }
    }
    return NULL;
}

/*
SgStatement *checkInternal(SgSymbol *s)
{
    enum { SEARCH_INTERNAL, SEARCH_CONTAINS };

    SgStatement *searchStmt = cur_func->lexNext();
    SgStatement *tmp;
    const char *funcName = s->identifier();
        int mode = SEARCH_CONTAINS;

    //search internal function
    while (searchStmt)
    {
        switch (mode)
        {
        case SEARCH_CONTAINS:
            if (searchStmt->variant() == CONTAINS_STMT)
                mode = SEARCH_INTERNAL;
            searchStmt = searchStmt->lastNodeOfStmt()->lexNext();
            break;
        case SEARCH_INTERNAL:
            if (searchStmt->variant() == CONTROL_END)
                return NULL;
            else if (!strcmp(searchStmt->symbol()->identifier(), funcName))
                return searchStmt;
            else
                searchStmt = searchStmt->lastNodeOfStmt()->lexNext();
            break;
        }
    }
    return NULL;
}
*/

void TestRoutineAttribute(SgSymbol *s, SgStatement *routine_interface)
{
    if (isForCudaRegion() && FromOtherFile(s) && !routine_interface)
        Error("Interface with ROUTINE specification is required for %s", s->identifier(), 646, routine_interface ? routine_interface : cur_func);
}

/*
int LookForRoutineDir( SgStatement *interfaceFunc )
{
    SgStatement *st;
    for(st=interfaceFunc->lexNext(); st->variant() != CONTROL_END; st=st->lexNext())
                if(st->variant() == ACC_ROUTINE_DIR)
                    return 1; 
    return 0;
}
*/

void CreateCalledFunctionDeclarations(SgStatement *st_hedr)
{
    symb_list *sl;
    SgStatement *contStmt = st_hedr->lastNodeOfStmt();
    int has_routine_attr = 0;

    for (sl = acc_call_list; sl; sl = sl->next)
    {
        if ((sl->symb->variant() == FUNCTION_NAME || sl->symb->variant() == PROCEDURE_NAME || sl->symb->variant() == INTERFACE_NAME) && !IS_BY_USE(sl->symb))
        {
            SgStatement *interfaceFunc = getInterface(sl->symb);
            if (interfaceFunc != NULL)
            {    
                if(interfaceFunc->variant() == INTERFACE_STMT)  
                    st_hedr->insertStmtAfter(interfaceFunc->copy(), *st_hedr);
                else
                { 
                    SgStatement *block = new SgStatement(INTERFACE_STMT);
                    block->insertStmtAfter(*new SgStatement(CONTROL_END), *block);
                    block->insertStmtAfter(interfaceFunc->copy(), *block);
                    st_hedr->insertStmtAfter(*block, *st_hedr);
                    if (isForCudaRegion() && HAS_ROUTINE_ATTR(interfaceFunc->symbol())) 
                        has_routine_attr = 1;
                }
            }
          /*
            else if (interfaceFunc = checkInternal(sl->symb))
            {
                if (contStmt->variant() == CONTROL_END)
                {
                    contStmt->insertStmtBefore(*new SgStatement(CONTAINS_STMT));
                    contStmt = contStmt->lexPrev();
                }
                contStmt->insertStmtAfter(interfaceFunc->copy(), *st_hedr);
            }
          */ 
            else if(sl->symb->variant() == FUNCTION_NAME)
                st_hedr->insertStmtAfter(*sl->symb->makeVarDeclStmt(), *st_hedr);
            TestRoutineAttribute(sl->symb, has_routine_attr ? interfaceFunc : NULL);
        }
    }
}

void CreateUseStatements(SgStatement *st_hedr)
{
  CreateUseStatementsForCalledProcedures(st_hedr);
  CreateUseStatementsForDerivedTypes(st_hedr);
}

void CreateUseStatementsForCalledProcedures(SgStatement *st_hedr)
{
    symb_list *sl;
    SgStatement *st_use, *stmt;

    for (sl = acc_call_list; sl; sl = sl->next)
    {
        SgSymbol *sf = ORIGINAL_SYMBOL(sl->symb); //SourceProcedureSymbol(sl->symb);
        stmt = sf->scope();
        if (stmt->variant() == MODULE_STMT)
        {
            st_use = new SgStatement(USE_STMT);
            st_use->setSymbol(*stmt->symbol());
            st_use->setExpression(0, *new SgExpression(ONLY_NODE, new SgVarRefExp(sl->symb), NULL, NULL));
            st_hedr->insertStmtAfter(*st_use, *st_hedr);
        }
    }
}

void CreateUseStatementsForDerivedTypes(SgStatement *st_hedr)
{
  SgStatement *st, *st_copy, *cur=st_hedr, *from_hedr = cur_func;
  if(USE_STATEMENTS_ARE_REQUIRED)
  {
    while (from_hedr->variant() != GLOBAL)
    {       
       for(st=from_hedr->lexNext(); st->variant()==USE_STMT; st=st->lexNext())
       {
         st_copy=&st->copy();
         cur->insertStmtAfter(*st_copy,*st_hedr); 
         cur = st_copy;
       }
       from_hedr = from_hedr->controlParent();
    } 
  }
}

SgStatement *CreateHostProcedure(SgSymbol *sHostProc)
{
    SgStatement *st_hedr, *st_end;

    st_hedr = new SgStatement(PROC_HEDR);
    st_hedr->setSymbol(*sHostProc);
    st_hedr->setExpression(2, *new SgExpression(RECURSIVE_OP));
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*sHostProc);
    if (!cur_in_source)
        cur_in_source = (*FILE_LAST_STATEMENT(current_file->firstStatement()))->lexNext(); //empty statement inserted after last statement of file
    //mod_gpu ? mod_gpu->lastNodeOfStmt() : current_file->firstStatement();      
    cur_in_source->insertStmtAfter(*st_hedr, *current_file->firstStatement());
    st_hedr->insertStmtAfter(*st_end, *st_hedr);
    st_hedr->setVariant(PROS_HEDR);

    cur_in_source = st_end;
    return(st_hedr);

}

SgStatement *Create_Host_Across_Loop_Subroutine(SgSymbol *sHostProc)
{
    SgStatement *stmt = NULL, *st_end = NULL, *st_hedr = NULL, *cur = NULL, *last_decl = NULL;
    SgExpression *ae = NULL, *arg_list = NULL, *el = NULL, *de = NULL, *tail = NULL, *baseMem_list = NULL;
    SgSymbol *s_loop_ref = NULL, *sarg = NULL, *h_first = NULL, *hl = NULL;
    symb_list *sl = NULL;
    SgType *tdvm = NULL;
    int ln;
    char *name = NULL;    

    SgExprListExp *list = isSgExprListExp(dvm_parallel_dir->expr(2)); // do_variables list
    SgSymbol *sHostAcrossProc;
    symb_list *acc_acr_call_list = NULL;
    for (int i = 0; i < list->length(); i++) 
    {
        sHostAcrossProc = HostAcrossProcSymbol(sHostProc, i + 1);
        Create_Host_Loop_Subroutine(sHostAcrossProc, i + 1);
        acc_acr_call_list = AddToSymbList(acc_acr_call_list, sHostAcrossProc);
    }
    sHostAcrossProc = HostAcrossProcSymbol(sHostProc, 0);
    Create_Host_Loop_Subroutine(sHostAcrossProc, -1);
    acc_acr_call_list = AddToSymbList(acc_acr_call_list, sHostAcrossProc);

    // create Host procedure header and end 

    st_hedr = CreateHostProcedure(sHostProc);
    st_hedr->addComment(Host_LoopHandlerComment());
    st_end = st_hedr->lexNext();

    // create  dummy argument list
    // loop_ref,<dvm_array_headers>,<dvm_array_bases>,<uses> 
    tdvm = FortranDvmType();

    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *tdvm, *st_hedr);

    ae = new SgVarRefExp(s_loop_ref);
    arg_list = new SgExprListExp(*ae);
    st_hedr->setExpression(0, *arg_list);

    // add  dvm-array-header list
    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    { 
        sarg = DummyDvmHeaderSymbol(sl->symb,st_hedr);
        ae = new SgArrayRefExp(*sarg);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            h_first = sarg;
    }

    // add  dvm-array-address list 
    if (options.isOn(O_HOST))
    {
        tail = arg_list;
        for (sl = acc_array_list, hl = h_first; sl; sl = sl->next, hl = hl->next())
        {
            sarg = DummyDvmArraySymbol(sl->symb, hl);
            ae = new SgArrayRefExp(*sarg);
            arg_list->setRhs(*new SgExprListExp(*ae));
            arg_list = arg_list->rhs();
        }
        tail = tail->rhs();
    }
    else
        // create memory base list and add it to the dummy argument list
    {
        baseMem_list = tail = CreateBaseMemoryList();
        AddListToList(arg_list, baseMem_list);
    }

    // add use's list to dummy argument list
    if (uses_list)
    {
        AddListToList(arg_list, copy_uses_list = &(uses_list->copy()));
        if (!tail)
            tail = copy_uses_list;
    }

    // add bounds of reduction arrays to dummy argument list
    if(red_list)
    {    
        SgExpression * red_bound_list;
        AddListToList(arg_list, red_bound_list = DummyListForReductionArrays(st_hedr));
        if(!tail)
           tail = red_bound_list;         
    }

    // create  get_dependency_mask function declaration 
    stmt = fdvm[GET_DEP_MASK_F]->makeVarDeclStmt();
    stmt->expr(1)->setType(tdvm);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    last_decl = cur = stmt;

    // create called functions declarations
    CreateCalledFunctionDeclarations(st_hedr);

    for (sl = acc_acr_call_list; sl; sl = sl->next)
    {
        if (sl->symb->variant() == PROCEDURE_NAME) {
            stmt = new SgStatement(EXTERN_STAT);
            el = new SgExprListExp(*new SgVarRefExp(sl->symb));
            stmt->setExpression(0, *el);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
    }

    // create dummy argument declarations

    for (el = tail; el; el = el->rhs())
    {
        stmt = el->lhs()->symbol()->makeVarDeclStmt();
        ConstantSubstitutionInTypeSpec(stmt->expr(1));
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }

    el = st_hedr->expr(0);
    stmt = el->lhs()->symbol()->makeVarDeclStmt();
    st_hedr->insertStmtAfter(*stmt, *st_hedr);
    de = stmt->expr(0);
    //for(el=el->rhs(); el!=baseMem_list && el!=copy_uses_list; el=el->rhs())
    for (el = el->rhs(); el != tail; el = el->rhs())
    {             //printf("%s \n",el->lhs()->symbol()->identifier());
        de->setRhs(new SgExprListExp(*el->lhs()->symbol()->makeDeclExpr()));
        de = de->rhs();
    }

    SgSymbol *which_run = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("which_run"), *tdvm, *st_hedr);
    stmt = which_run->makeVarDeclStmt();
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    // generate IMPLICIT NONE statement
    st_hedr->insertStmtAfter(*new SgStatement(IMPL_DECL), *st_hedr);

    // generate USE statements for called module procedures
    CreateUseStatements(st_hedr);

    SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[GET_DEP_MASK_F]);
    fe->addArg(*new SgVarRefExp(s_loop_ref));
    SgFunctionCallExp *fen = new SgFunctionCallExp(*new SgFunctionSymb(FUNCTION_NAME, "not", *SgTypeBool(), *cur_func));
    fen->addArg(*fe);
    SgVarRefExp *which_run_expr = new SgVarRefExp(which_run);
    stmt = new SgAssignStmt(*which_run_expr, *fen);
    st_end->insertStmtBefore(*stmt, *st_hedr);
    //stmt = PrintStat(which_run_expr);
    //st_end->insertStmtBefore(*stmt, *st_hedr);

    SgIfStmt *ifstmt = NULL;
    SgStatement *falsestmt = NULL;
    int i = 0;
    for (sl = acc_acr_call_list; sl; sl = sl->next)
    {
        SgFunctionSymb *sbtest = new SgFunctionSymb(FUNCTION_NAME, "btest", *SgTypeBool(), *cur_func);
        if (sl->symb->variant() == PROCEDURE_NAME) {
            SgFunctionCallExp *fbtest = new SgFunctionCallExp(*sbtest);
            fbtest->addArg(*which_run_expr);
            fbtest->addArg(*new SgValueExp(i - 1));
            if (i != 0)
            {
                SgCallStmt *truestmt = new SgCallStmt(*sl->symb, *st_hedr->expr(0));
                ifstmt = new SgIfStmt(*fbtest, *truestmt, *falsestmt);
                falsestmt = ifstmt;
            }
            else {
                falsestmt = new SgCallStmt(*sl->symb, *st_hedr->expr(0));
            }
            i++;
        }
    }
    if (ifstmt) st_end->insertStmtBefore(*ifstmt, *st_hedr);
    return(st_hedr);
}

SgStatement *Create_Host_Loop_Subroutine(SgSymbol *sHostProc, int dependency)
{
    SgStatement *stmt = NULL, *st_end = NULL, *st_hedr = NULL, *cur = NULL, *last_decl = NULL, *ass = NULL;
    SgStatement *alloc = NULL;
    SgStatement *paralleldo = NULL;
    SgStatement *firstdopar = NULL;
    SgExprListExp *parallellist = NULL;
    SgExprListExp *omp_dolist = NULL;
    SgExprListExp *omp_perflist = NULL;
    SgExpression *ae, *arg_list = NULL, *el = NULL, *de = NULL, *tail = NULL, *baseMem_list = NULL;
    SgSymbol *s_loop_ref = NULL, *sarg = NULL, *h_first = NULL, *hl = NULL;
    SgSymbol *s_lgsc = NULL; /* OpenMP */
    SgVarRefExp *v_lgsc = NULL;   /* OpenMP */
    SgSymbol *s = NULL, *s_low_bound = NULL, *s_high_bound = NULL, *s_step = NULL;
    symb_list *sl = NULL;
    SgType *tdvm = NULL;
    int ln, lrank, addopenmp;
    char *name;
    tail = NULL;
    addopenmp = 1;    /* OpenMP */

    // create Host procedure header and end            

    st_hedr = CreateHostProcedure(sHostProc);
    st_hedr->addComment(Host_LoopHandlerComment());
    st_end = st_hedr->lexNext();

    // create  dummy argument list
    // loop_ref,<dvm_array_headers>,<dvm_array_bases>,<uses> 

    tdvm = FortranDvmType();

    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *tdvm, *st_hedr);

    ae = new SgVarRefExp(s_loop_ref);
    arg_list = new SgExprListExp(*ae);
    st_hedr->setExpression(0, *arg_list);

    // add  dvm-array-header list
    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    {                               //printf("%s\n",sl->symb->identifier());
        sarg = DummyDvmHeaderSymbol(sl->symb,st_hedr);
        ae = new SgArrayRefExp(*sarg);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            h_first = sarg;
    }

    // add  dvm-array-address list 
    if (options.isOn(O_HOST))        
    {
        tail = arg_list;
        for (sl = acc_array_list, hl = h_first; sl; sl = sl->next, hl = hl->next())
        {
            sarg = DummyDvmArraySymbol(sl->symb, hl);
            ae = new SgArrayRefExp(*sarg);
            arg_list->setRhs(*new SgExprListExp(*ae));
            arg_list = arg_list->rhs();
        }
        tail = tail->rhs();
    }
    else
        // create memory base list and add it to the dummy argument list
    {
        baseMem_list = tail = CreateBaseMemoryList();
        AddListToList(arg_list, baseMem_list);
    }

    // add use's list to dummy argument list
    if (uses_list)
    {
        AddListToList(arg_list, copy_uses_list = &(uses_list->copy()));
        if (!tail)
            tail = copy_uses_list;
    }
    if(red_list)
    {    
        SgExpression * red_bound_list;
        AddListToList(arg_list, red_bound_list = DummyListForReductionArrays(st_hedr));
        if(!tail)
           tail = red_bound_list;         
    }

    // create external statement
    stmt = new SgStatement(EXTERN_STAT);
    el = new SgExprListExp(*new SgVarRefExp(fdvm[FILL_BOUNDS]));
    if (red_list)
    {
        SgExpression *eel;
        eel = new SgExprListExp(*new SgVarRefExp(fdvm[RED_INIT]));
        eel->setRhs(*el);
        el = eel;
        eel = new SgExprListExp(*new SgVarRefExp(fdvm[RED_POST]));
        eel->setRhs(*el);
        el = eel;
    }
    stmt->setExpression(0, *el);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    last_decl = cur = stmt;

    // create called functions declarations
    CreateCalledFunctionDeclarations(st_hedr);

    // create  get_slot_count function declaration           /* OpenMP */
    stmt = fdvm[SLOT_COUNT]->makeVarDeclStmt();
    stmt->expr(1)->setType(tdvm);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    //  <lgsc_var_for_OpenMp>
    s_lgsc = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("lgsc"), *tdvm, *st_hedr);  /* OpenMP */
    v_lgsc = new SgVarRefExp(*s_lgsc);  /* OpenMP */
    stmt = s_lgsc->makeVarDeclStmt();
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    if (omp_perf) /* OpenMP */
    {
	//SgVarRefExp *varDvmhstring = new SgVarRefExp(fdvm[STRING]);
	SgVarRefExp *varThreadID = new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "dvmh_threadid",tdvm,st_hedr));
	SgVarRefExp *varStmtID = new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "dvmh_stmtid",tdvm,st_hedr));
	//SgExpression *exprFilenameType = new SgExpression(LEN_OP);
	//exprFilenameType->setLhs(new SgValueExp((int)(strlen(dvm_parallel_dir->fileName())+1)));
	//SgType *typeFilename = new SgType(T_STRING,exprFilenameType,SgTypeChar());
	//SgVarRefExp *varFilename = new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "dvmh_filename",typeFilename,st_hedr));
	//stmt=varFilename->symbol()->makeVarDeclStmt();
	//stmt->expr(0)->setLhs(FileNameInitialization(stmt->expr(0)->lhs(),dvm_parallel_dir->fileName()));
	//stmt->setVariant(VAR_DECL_90);
	//stmt->setlineNumber(-1);
	//st_hedr->insertStmtAfter(*stmt, *st_hedr);
	//stmt=varDvmhstring->symbol()->makeVarDeclStmt();
	//stmt->setlineNumber(-1);
	//st_hedr->insertStmtAfter(*stmt, *st_hedr);
	//SgExprListExp *funcList = new SgExprListExp(*varDvmhstring);
	SgExprListExp *funcList = new SgExprListExp(*new SgVarRefExp(fdvm[OMP_STAT_BP]));
	//funcList->append(*new SgVarRefExp(fdvm[OMP_STAT_BP]));
	funcList->append(*new SgVarRefExp(fdvm[OMP_STAT_AP]));
	funcList->append(*new SgVarRefExp(fdvm[OMP_STAT_BL]));
	funcList->append(*new SgVarRefExp(fdvm[OMP_STAT_AL]));
	if (dependency == -1) {
		funcList->append(*new SgVarRefExp(fdvm[OMP_STAT_BS]));
		funcList->append(*new SgVarRefExp(fdvm[OMP_STAT_AS]));
	}
	stmt = new SgStatement(EXTERN_STAT);
	stmt->setExpression(0, *funcList);
	stmt->setlineNumber(-1);
	st_hedr->insertStmtAfter(*stmt, *st_hedr);
	omp_perflist = new SgExprListExp(*new SgVarRefExp(s_loop_ref)); /* OpenMP */
	omp_perflist->append(*varStmtID); /* OpenMP */
	omp_perflist->append(*varThreadID); /* OpenMP */
	//omp_perflist->append(*ConstRef_F95(dvm_parallel_dir->lineNumber())); /* OpenMP */
	//omp_perflist->append(*DvmhString(varFilename));
	SgSymbol *symCommon =new SgSymbol (VARIABLE_NAME,"dvmh_common");
	stmt = new SgStatement (OMP_THREADPRIVATE_DIR);
	SgExpression *exprThreadprivate = new SgExpression (OMP_THREADPRIVATE);
	exprThreadprivate->setLhs (*new SgExprListExp (*new SgVarRefExp (*symCommon)));
	stmt->setExpression (0, *exprThreadprivate);
	st_hedr->insertStmtAfter(*stmt, *st_hedr);
	SgExpression *exprCommon = new SgExpression (COMM_LIST);
	exprCommon->setSymbol (*symCommon);
	exprCommon->setLhs (*varThreadID);
	stmt = new SgStatement(COMM_STAT);
	stmt->setExpression (0, *exprCommon);
	stmt->setlineNumber(-1);
	st_hedr->insertStmtAfter(*stmt, *st_hedr);
	stmt = varStmtID->symbol()->makeVarDeclStmt();
	stmt->setlineNumber(-1);
	st_hedr->insertStmtAfter(*stmt, *st_hedr);
	stmt = varThreadID->symbol()->makeVarDeclStmt();
	stmt->setlineNumber(-1);
	st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }

    parallellist = new SgExprListExp(*new SgExpression(OMP_NUM_THREADS, v_lgsc, NULL, NULL)); /* OpenMP */

    // create reduction variables declarations and 
    // generate 'loop_red_init' and 'loop_red_post' function calls

    //looking through the reduction list
    if (red_list)
    {
        int nr;
        SgExpression *ev, *ered, *er, *red;
        SgSymbol *loc_var;
        reduction_operation_list *rl;

        red = TranslateReductionToOpenmp(red_list);   /* OpenMP */
        if (red != NULL) parallellist->append(*red);  /* OpenMP */
        else addopenmp = 0;                           /* OpenMP */
        for (rl = red_struct_list,nr = 1; rl; rl = rl->next, nr++)
        {   
            if (rl->locvar)   
            {
                DeclareSymbolInHostHandler(rl->locvar, st_hedr, rl->locvar);  
                //stmt = rl->locvar->makeVarDeclStmt();
                //ConstantSubstitutionInTypeSpec(stmt->expr(1)); 
                //st_hedr->insertStmtAfter(*stmt, *st_hedr);
            }
            SgSymbol *sred =  rl->redvar_size != 0 ? rl->red_host : rl->redvar;
            DeclareSymbolInHostHandler(rl->redvar, st_hedr, sred);  

            // generate loop_red_init and loop_red_post function calls 
            stmt = LoopRedInit_HH(s_loop_ref, nr, sred, rl->locvar);
            cur->insertStmtAfter(*stmt, *st_hedr);
            cur = stmt;
            stmt = LoopRedPost_HH(s_loop_ref, nr, sred, rl->locvar);
            st_end->insertStmtBefore(*stmt, *st_hedr);

        }
    }

    // create local variables and it's declarations: 
    // <loop_index_variables>,<private_variables>,[<dvm-constansts>],<array-coefficients>,<loop-bounds-variables><lgsc_var_for_OpenMp>


    //    <loop-bounds-variables>
    lrank = ParLoopRank();
    SgArrayType *typearray = new SgArrayType(*tdvm);
    typearray->addRange(*new SgValueExp(lrank));
	if (addopenmp == 1) {
		if (dependency == -1) { /* OpenMP */
			omp_dolist = new SgExprListExp(*new SgExpression(OMP_SCHEDULE, new SgKeywordValExp("static"), NULL, NULL)); /* OpenMP */
		} else {
			omp_dolist = new SgExprListExp(*new SgExpression(OMP_SCHEDULE, new SgKeywordValExp("runtime"), NULL, NULL)); /* OpenMP */
		    // XXX: 'collapse' clause does not work properly          
			if ((dependency == 0) && (collapse_loop_count > 1)) {  /* OpenMP */
				omp_dolist->append(*new SgExpression(OMP_COLLAPSE, new SgValueExp(collapse_loop_count < lrank ? collapse_loop_count : lrank), NULL, NULL));  /* OpenMP */
 			}/* OpenMP */
		}
	}

    s_low_bound = s = new SgSymbol(VARIABLE_NAME, "boundsLow", *typearray, *st_hedr);
    s_high_bound = new SgSymbol(VARIABLE_NAME, "boundsHigh", *typearray, *st_hedr);
    s_step = new SgSymbol(VARIABLE_NAME, "stepsIgnore", *typearray, *st_hedr);

    stmt = s->makeVarDeclStmt();
    stmt->expr(1)->setType(tdvm);
    el = new SgExprListExp(*new SgArrayRefExp(*s_high_bound, *new SgValueExp(lrank)));
    el->setRhs(new SgExprListExp(*new SgArrayRefExp(*s_step, *new SgValueExp(lrank))));
    stmt->expr(0)->setRhs(el);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    //    <array-coefficients>
    if (!options.isOn(O_HOST))
        DeclareArrayCoefficients(st_hedr);

    //   <private_variables>
    if ((addopenmp == 1) && (private_list != NULL)) parallellist->append(*new SgExpression(OMP_PRIVATE, new SgExprListExp(*private_list), NULL, NULL));  /* OpenMP */
    for (el = private_list; el; el = el->rhs())
    {
        SgSymbol *sp = el->lhs()->symbol(); 
        //if(HEADER(sp)) // dvm-array is declared as dummy argument
        //  continue; 
        DeclareSymbolInHostHandler(sp, st_hedr, NULL);
    }
    //   <loop_index_variables>     
    SgExprListExp *indexes = NULL; /* OpenMP */
    for (el = dvm_parallel_dir->expr(2); el; el = el->rhs())
    {
        if (isPrivate(el->lhs()->symbol()))    // is declared as private 
            continue;
        stmt = el->lhs()->symbol()->makeVarDeclStmt();
        ConstantSubstitutionInTypeSpec(stmt->expr(1));    
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
        if (addopenmp == 1) {/* OpenMP */
            if (indexes != NULL) indexes->append(*el->lhs());  /* OpenMP */
            else indexes = new SgExprListExp(*el->lhs());      /* OpenMP */
        }   /* OpenMP */
    }

    if ((addopenmp == 1) && (indexes != NULL)) parallellist->append(*new SgExpression(OMP_PRIVATE, indexes, NULL, NULL));  /* OpenMP */

    // create dummy argument declarations

    for (el = tail; el; el = el->rhs())
    {
        stmt = el->lhs()->symbol()->makeVarDeclStmt();
        ConstantSubstitutionInTypeSpec(stmt->expr(1)); 
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }

    el = st_hedr->expr(0);   
    stmt = el->lhs()->symbol()->makeVarDeclStmt();
    st_hedr->insertStmtAfter(*stmt, *st_hedr);
    de = stmt->expr(0);
    //for(el=el->rhs(); el!=baseMem_list && el!=copy_uses_list; el=el->rhs())
    for (el = el->rhs(); el != tail; el = el->rhs())
    {             //printf("%s \n",el->lhs()->symbol()->identifier());
        de->setRhs(new SgExprListExp(*el->lhs()->symbol()->makeDeclExpr()));
        de = de->rhs();
    }

    // generate IMPLICIT NONE statement
    st_hedr->insertStmtAfter(*new SgStatement(IMPL_DECL), *st_hedr);

    // generate USE statements for called module procedures
    CreateUseStatements(st_hedr);

    // generate call statement of 'loop_fill_bounds'
    stmt = LoopFillBounds_HH(s_loop_ref, s_low_bound, s_high_bound, s_step);
    last_decl->insertStmtAfter(*stmt, *st_hedr);
    if (cur == last_decl)
        cur = stmt;
    // copying headers elements to array coefficients 
    if (!options.isOn(O_HOST)) {
        CopyHeaderElems(last_decl);
        if (dependency == 0) dvm_ar = NULL;
    }

    // inserting parallel loop nest
    // first_do_par - first DO statement of parallel loop nest

    // replace loop nest 
    ReplaceDoNestLabel_Above(LastStatementOfDoNest(first_do_par), first_do_par, GetLabel());
    ReplaceLoopBounds(first_do_par, lrank, s_low_bound, s_high_bound);

    //stmt = first_do_par->extractStmt();
    if (dependency == 0) firstdopar = stmt = first_do_par->extractStmt();
    else firstdopar = stmt = first_do_par->copyPtr();
    cur->insertStmtAfter(*stmt, *st_hedr); 


 
    if (addopenmp == 1) { /* OpenMP */
		SgCallStmt *stDvmhstat = NULL;
		SgStatement *omp_do = new SgStatement(OMP_DO_DIR); /* OpenMP */
		SgStatement *omp_parallel = new SgStatement(OMP_PARALLEL_DIR);  /* OpenMP */
		SgStatement *omp_endparallel = new SgStatement(OMP_END_PARALLEL_DIR); /* OpenMP */
		SgStatement *omp_enddo = new SgStatement(OMP_END_DO_DIR); /* OpenMP */
		SgForStmt *stdo = isSgForStmt(firstdopar); /* OpenMP */
		SgStatement *lastdo=LastStatementOfDoNest(stdo);
		cur->insertStmtAfter(*omp_parallel, *st_hedr); /* OpenMP */
		if (omp_perf) {/* OpenMP */
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_BP],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */
			cur->insertStmtAfter(*stDvmhstat, *st_hedr); /* OpenMP */	
		}
		lastdo->insertStmtAfter(*omp_endparallel); /* OpenMP */
		if (omp_perf) {/* OpenMP */
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_AL],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */
			lastdo->insertStmtAfter(*stDvmhstat);/* OpenMP */
		}/* OpenMP */
		omp_parallel->setExpression(0, *parallellist);/* OpenMP */
		omp_do->setExpression(0, *omp_dolist);/* OpenMP */
		omp_enddo->setExpression(0, *new SgExprListExp(*new SgExpression(OMP_NOWAIT)));  /* OpenMP */
		ass = new SgAssignStmt(*v_lgsc, *LoopGetSlotCount_HH(s_loop_ref)); /* OpenMP */
        if (!dependency) {
		omp_parallel->insertStmtAfter(*omp_do); /* OpenMP */           
		lastdo->insertStmtAfter(*omp_enddo); /* OpenMP */
	} else if (isSgForStmt(firstdopar->lexNext())) {  /* OpenMP */
            int step = 1;  /* OpenMP */
            SgSymbol *s_iam = NULL;  /* OpenMP */
            SgExpression *e_iam = NULL;  /* OpenMP */
            SgSymbol *s_ilimit = NULL;  /* OpenMP */
            SgExpression *e_ilimit = NULL;  /* OpenMP */
            SgSymbol *s_isync = NULL;  /* OpenMP */
            SgExpression *e_isync = NULL;  /* OpenMP */
            SgSymbol *omp_get_thread_num = NULL;  /* OpenMP */
            SgStatement *vardecl = NULL;  /* OpenMP */
            SgExprListExp *exprlist = NULL;  /* OpenMP */
            SgForStmt *second_do_par = isSgForStmt(firstdopar->lexNext());  /* OpenMP */
            SgStatement *assign;        /* OpenMP */
            SgStatement  *allocatablestmt;        /* OpenMP */
            ConvertLoopWithLabelToEnddoLoop(firstdopar); /* OpenMP */
            if (dependency == -1) { /* OpenMP */
                SgFunctionCallExp *fmin = new SgFunctionCallExp(*new SgFunctionSymb(FUNCTION_NAME, "min", *SgTypeInt(), *cur_func));  /* OpenMP */
                if (second_do_par->step()) {  /* OpenMP */
                    if (second_do_par->step()->isInteger())  /* OpenMP */
                        step = second_do_par->step()->valueInteger();  /* OpenMP */
                    else  /* OpenMP */
                        step = 0;  /* OpenMP */
                }  /* OpenMP */
                s_iam = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("iam"), *stdo->symbol()->type(), *st_hedr);  /* OpenMP */
                e_iam = new SgVarRefExp(*s_iam);  /* OpenMP */
                s_isync = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("isync"), *new SgArrayType(*stdo->symbol()->type()), *st_hedr);  /* OpenMP */
                e_isync = new SgVarRefExp(*s_isync);  /* OpenMP */
                s_ilimit = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("ilimit"), *stdo->symbol()->type(), *st_hedr);  /* OpenMP */
                e_ilimit = new SgVarRefExp(*s_ilimit);  /* OpenMP */
                omp_get_thread_num = new SgSymbol(FUNCTION_NAME, "omp_get_thread_num", *tdvm, *st_hedr);  /* OpenMP */
                allocatablestmt = new SgStatement(ALLOCATABLE_STMT);  /* OpenMP */
                allocatablestmt->setExpression(0, *new SgExprListExp(*new SgArrayRefExp(*s_isync, *new SgExpression(DDOT)))); /* OpenMP */
                allocatablestmt->setlineNumber(-1);  /* OpenMP */
                last_decl->insertStmtAfter(*allocatablestmt, *st_hedr);  /* OpenMP */
                vardecl = s_isync->makeVarDeclStmt(); /* OpenMP */
                ConstantSubstitutionInTypeSpec(vardecl->expr(1)); 
                vardecl->setlineNumber(-1);  /* OpenMP */
                last_decl->insertStmtAfter(*vardecl, *st_hedr);  /* OpenMP */
                vardecl = s_iam->makeVarDeclStmt();  /* OpenMP */
                ConstantSubstitutionInTypeSpec(vardecl->expr(1)); 
                vardecl->setlineNumber(-1);  /* OpenMP */
                last_decl->insertStmtAfter(*vardecl, *st_hedr);  /* OpenMP */
                vardecl = s_ilimit->makeVarDeclStmt();  /* OpenMP */
                ConstantSubstitutionInTypeSpec(vardecl->expr(1)); 
                vardecl->setlineNumber(-1);  /* OpenMP */
                last_decl->insertStmtAfter(*vardecl, *st_hedr);  /* OpenMP */
                vardecl = omp_get_thread_num->makeVarDeclStmt();  /* OpenMP */
                vardecl->setlineNumber(-1);  /* OpenMP */
                last_decl->insertStmtAfter(*vardecl, *st_hedr);  /* OpenMP */
                exprlist = new SgExprListExp(*e_iam);  /* OpenMP */
                exprlist->append(*e_ilimit);  /* OpenMP */
                parallellist->append(*new SgExpression(OMP_PRIVATE, exprlist, NULL, NULL));  /* OpenMP */
                //SgVarRefExp *e_loop = new SgVarRefExp(stdo->symbol());    /* OpenMP */		
		if (omp_perf) {/* OpenMP */	
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_AS],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */	
			omp_parallel->insertStmtAfter(*stDvmhstat); /* OpenMP */
		}
                omp_parallel->insertStmtAfter(*new SgStatement(OMP_BARRIER_DIR)); /* OpenMP */				
		if (omp_perf) {/* OpenMP */	
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_BS],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */
			omp_parallel->insertStmtAfter(*stDvmhstat); /* OpenMP */
		}
		assign = new SgAssignStmt(*new SgArrayRefExp(*s_isync, *e_iam), *new SgValueExp(0));  /* OpenMP */
                assign->setlineNumber(-1);  /* OpenMP */
                omp_parallel->insertStmtAfter(*assign); /* OpenMP */
                assign = new SgAssignStmt(*e_iam, *new SgFunctionCallExp(*omp_get_thread_num));  /* OpenMP */
                assign->setlineNumber(-1);  /* OpenMP */
                omp_parallel->insertStmtAfter(*assign); /* OpenMP */
                fmin->addArg(*v_lgsc - *new SgValueExp(1));
                if (step > 0) {  /* OpenMP */
                    if (step == 1) {
                        fmin->addArg(*second_do_par->end() - *second_do_par->start() /*+ *new SgValueExp(1)*/);
                    }
                    else {
                        SgValueExp *estep = new SgValueExp(step);
                        fmin->addArg((*second_do_par->end() - *second_do_par->start()) / *estep /*+ *new SgValueExp(1)*/);
                    }
                }
                else {  /* OpenMP */
                    if (step == -1) {
                        fmin->addArg(*second_do_par->start() - *second_do_par->end() /*+ *new SgValueExp(1)*/);
                    }
                    else {
                        SgValueExp *estep = new SgValueExp(step);
                        fmin->addArg((*second_do_par->start() - *second_do_par->end()) / *estep /*+ *new SgValueExp(1)*/);
                    }
                }
                assign = new SgAssignStmt(*e_ilimit, *fmin);  /* OpenMP */
                assign->setlineNumber(-1);  /* OpenMP */
                omp_parallel->insertStmtAfter(*assign); /* OpenMP */
                alloc = new SgStatement(DEALLOCATE_STMT);  /* OpenMP */
                alloc->setExpression(0, *new SgArrayRefExp(*s_isync));  /* OpenMP */
                alloc->setlineNumber(-1);  /* OpenMP */
                omp_endparallel->insertStmtAfter(*alloc, *st_hedr); /* OpenMP */
                alloc = new SgStatement(ALLOCATE_STMT);  /* OpenMP */
                alloc->setExpression(0, *new SgArrayRefExp(*s_isync, *new SgExpression(DDOT, new SgValueExp(0), &(*v_lgsc - *new SgValueExp(1)), NULL)));  /* OpenMP */
                alloc->setlineNumber(-1);  /* OpenMP */
                firstdopar->insertStmtAfter(*omp_do); /* OpenMP */
                omp_do->lexNext()->lastNodeOfStmt()->insertStmtAfter(*omp_enddo);
                SgStatement *flushst = new SgStatement(OMP_FLUSH_DIR);
                flushst->setExpression(0, *new SgExprListExp(*e_isync));
                SgExpression *e_isynciam = new SgArrayRefExp(*s_isync, *e_iam - *new SgValueExp(1));
                SgWhileStmt *whilest = new SgWhileStmt(SgEqOp(*e_isynciam, *new SgValueExp(0)).copy(), *flushst);
                whilest->setlineNumber(-1);  /* OpenMP */
                whilest->lastNodeOfStmt()->setlineNumber(-1);  /* OpenMP */
                SgIfStmt *ifstmt = new SgIfStmt(*e_iam > *new SgValueExp(0) && *e_iam <= *e_ilimit, *whilest);
                ifstmt->setlineNumber(-1);  /* OpenMP */
                ifstmt->lastNodeOfStmt()->setlineNumber(-1);  /* OpenMP */
		if (omp_perf) {/* OpenMP */	
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_AS],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */	
			firstdopar->insertStmtAfter(*stDvmhstat, *firstdopar); /* OpenMP */
		}
		firstdopar->insertStmtAfter(*ifstmt, *firstdopar);  /* OpenMP */								
		if (omp_perf) {/* OpenMP */	
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_BS],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */	
			firstdopar->insertStmtAfter(*stDvmhstat, *firstdopar); /* OpenMP */
		}
		assign = new SgAssignStmt(*e_isynciam, *new SgValueExp(0));  /* OpenMP */
                assign->setlineNumber(-1);  /* OpenMP */
                whilest->lastNodeOfStmt()->insertStmtAfter(*assign); /* OpenMP */
                assign->insertStmtAfter(flushst->copy()); /* OpenMP */
                e_isynciam = new SgArrayRefExp(*s_isync, *e_iam);  /* OpenMP */
                whilest = new SgWhileStmt(SgEqOp(*e_isynciam, *new SgValueExp(1)).copy(), flushst->copy());  /* OpenMP */
                whilest->setlineNumber(-1);  /* OpenMP */
                whilest->lastNodeOfStmt()->setlineNumber(-1);  /* OpenMP */
                ifstmt = new SgIfStmt(*e_iam < *e_ilimit, *whilest);  /* OpenMP */
                ifstmt->setlineNumber(-1);  /* OpenMP */
                ifstmt->lastNodeOfStmt()->setlineNumber(-1);  /* OpenMP */				
		if (omp_perf) {/* OpenMP */	
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_AS],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */	
			omp_enddo->insertStmtAfter(*stDvmhstat); /* OpenMP */
		}
		omp_enddo->insertStmtAfter(*ifstmt);  /* OpenMP */
		if (omp_perf) {/* OpenMP */	
			stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_BS],*omp_perflist);/* OpenMP */
			stDvmhstat->setlineNumber(-1);/* OpenMP */	
			omp_enddo->insertStmtAfter(*stDvmhstat); /* OpenMP */
		}
		assign = new SgAssignStmt(*e_isynciam, *new SgValueExp(1));  /* OpenMP */
                assign->setlineNumber(-1);  /* OpenMP */
                whilest->lastNodeOfStmt()->insertStmtAfter(*assign); /* OpenMP */
                assign->insertStmtAfter(flushst->copy()); /* OpenMP */
            }
            else {
                firstdopar = firstdopar->lexPrev();  /* OpenMP */
                for (int i = 1; i < dependency && firstdopar; i++) {  /* OpenMP */
                    firstdopar = firstdopar->lexNext();  /* OpenMP */
                }  /* OpenMP */
                if (isSgForStmt(firstdopar) || firstdopar->variant() == OMP_PARALLEL_DIR) {  /* OpenMP */
                    firstdopar->insertStmtAfter(*omp_do); /* OpenMP */
                    omp_do->lexNext()->lastNodeOfStmt()->insertStmtAfter(*omp_enddo);  /* OpenMP */
                }  /* OpenMP */
            }  /* OpenMP */
            if (alloc != NULL) cur->insertStmtAfter(*alloc, *st_hedr); /* OpenMP */
            ass->setlineNumber(-1); /* OpenMP */
        }  /* OpenMP */
	cur->insertStmtAfter(*ass, *st_hedr); /* OpenMP */
	if (omp_perf) {/* OpenMP */	
		stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_BL],*omp_perflist);/* OpenMP */
		stDvmhstat->setlineNumber(-1);/* OpenMP */	
		omp_parallel->insertStmtAfter(*stDvmhstat); /* OpenMP */
		stDvmhstat = new SgCallStmt(*fdvm[OMP_STAT_AP],*omp_perflist);/* OpenMP */
		stDvmhstat->setlineNumber(-1);/* OpenMP */
		omp_endparallel->insertStmtAfter(*stDvmhstat);/* OpenMP */	
	}/* OpenMP */
    }   /* OpenMP */


    return(st_hedr);
}

SgStatement *Create_Host_Sequence_Subroutine(SgSymbol *sHostProc, SgStatement *first_st, SgStatement *last_st)
{
    SgStatement *stmt, *st_end, *st_hedr;
    SgExpression *ae, *arg_list, *el, *de, *tail, *baseMem_list;
    SgSymbol  *s_loop_ref, *sarg, *h_first;

    symb_list *sl;
    SgType *tdvm;
    int ln, host_ndvm, save_maxdvm;

    //create Host Procedure header and end
    st_hedr = CreateHostProcedure(sHostProc);
    st_hedr->addComment(Host_SequenceHandlerComment(first_st->lineNumber()));
    st_end = st_hedr->lexNext();

    // create  dummy argument list
    // loop_ref,<dvm_array_headers>,<dvm_array_bases>,<uses> 
    tdvm = FortranDvmType();

    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *tdvm, *st_hedr);
    loop_ref_symb = s_loop_ref;  //assign to global for function HasLocalElement(), called from ReplaseAssignByIf()

    ae = new SgVarRefExp(s_loop_ref);
    arg_list = new SgExprListExp(*ae);
    st_hedr->setExpression(0, *arg_list);

    // add  dvm-array-header list
    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    {          //printf("%s\n",sl->symb->identifier()); 
        SgArrayType *typearray = new SgArrayType(*tdvm);
        typearray->addRange(*new SgValueExp(Rank(sl->symb) + 2));
        sarg = new SgSymbol(VARIABLE_NAME, sl->symb->identifier(), *typearray, *st_hedr);
        ae = new SgArrayRefExp(*sarg);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            h_first = sarg;
    }

    // create memory base list and add it to the dummy argument list
    baseMem_list = tail = CreateBaseMemoryList();
    AddListToList(arg_list, baseMem_list);

    // add use's list to dummy argument list
    if (uses_list)
        AddListToList(arg_list, copy_uses_list = &(uses_list->copy()));
    if (!tail)
        tail = copy_uses_list;    

    // create called functions declarations
    CreateCalledFunctionDeclarations(st_hedr);

    // create dummy argument declarations

    for (el = tail; el; el = el->rhs())
    {
        stmt = el->lhs()->symbol()->makeVarDeclStmt();
        ConstantSubstitutionInTypeSpec(stmt->expr(1)); 
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }

    el = st_hedr->expr(0);
    stmt = el->lhs()->symbol()->makeVarDeclStmt();
    st_hedr->insertStmtAfter(*stmt, *st_hedr);
    de = stmt->expr(0);

    for (el = el->rhs(); el && el != tail; el = el->rhs())
    {             //printf("%s \n",el->lhs()->symbol()->identifier());
        de->setRhs(new SgExprListExp(*el->lhs()->symbol()->makeDeclExpr()));
        de = de->rhs();
    }


    // inserting sequence of statements
    index_array_symb = NULL;
    host_ndvm = ndvm;
    save_maxdvm = maxdvm; maxdvm = 0;
    TransferBlockToHostSubroutine(first_st, last_st, st_end);
    dvm_ar = NULL;


    // declare indexArray if needed  for dvm-array references in left part of assign statement
    if (index_array_symb)
    {
        stmt = index_array_symb->makeVarDeclStmt();
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }
    // declare dvm000 array
    if (host_ndvm < maxdvm)
    {
        stmt = dvm000SymbolForHost(host_ndvm, st_hedr)->makeVarDeclStmt();
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }
    maxdvm = save_maxdvm;

    // create loop_has_element() / dvmh_loop_has_element() function declaration 
    int fVariant = INTERFACE_RTS2 ? HAS_ELEMENT_2 : HAS_ELEMENT;  
    if (fmask[fVariant])
    {
        fmask[fVariant] = 0;
        stmt = fdvm[fVariant]->makeVarDeclStmt();
        stmt->expr(1)->setType(FortranDvmType());
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }

    // create tstio() function declaration 
    if (has_io_stmt)
    {
        stmt = fdvm[TSTIOP]->makeVarDeclStmt();
        stmt->expr(1)->setType(FortranDvmType());
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
        if(options.isOn(IO_RTS))
        {
           stmt = fdvm[FTN_CONNECTED]->makeVarDeclStmt();
           stmt->expr(1)->setType(FortranDvmType());
           st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
    }
    // generate IMPLICIT NONE statement
    st_hedr->insertStmtAfter(*new SgStatement(IMPL_DECL), *st_hedr);

    // generate USE statements for called module procedures
    CreateUseStatements(st_hedr);

    return(st_hedr);
}

SgExpression *FillerDummyArgumentList(symb_list *paramList,SgStatement *st_hedr)
{
  symb_list *sl;
  SgExpression *dummy_arg_list=NULL;
    
  for (sl = paramList; sl; sl = sl->next)
  {          //printf("%s\n",sl->symb->identifier());
      if(isSgArrayType(sl->symb->type()))
      {
        SgSymbol *shedr = DummyDvmHeaderSymbol(sl->symb,st_hedr);
        SgExpression *ae = new SgArrayRefExp(*shedr);
        dummy_arg_list = AddListToList(dummy_arg_list,new SgExprListExp(*ae));
        ae = new SgArrayRefExp(*DummyDvmArraySymbol(sl->symb, shedr));
        dummy_arg_list = AddListToList(dummy_arg_list,new SgExprListExp(*ae)); 
      }
      else
        dummy_arg_list = AddListToList(dummy_arg_list,new SgExprListExp(*new SgVarRefExp(sl->symb)));
  }
  return dummy_arg_list;

}

SgStatement * makeSymbolDeclarationWithInit_F90(SgSymbol *s, SgExpression *einit)
{    
    SgStatement *st = s->makeVarDeclStmt();
    st->setVariant(VAR_DECL_90);
    SgExpression *e =  &SgAssignOp(*new SgVarRefExp(s), *einit);
    st->setExpression(0, *new SgExprListExp(*e));    
    return(st);
}

SgSymbol *LoopIndex(SgStatement *body, SgStatement *func)
{
    loopIndexCount++;
    char *sname = (char *)malloc(6+10+1); 
    sprintf(sname, "%s%d", "subexp", loopIndexCount);
    SgSymbol *si = new SgSymbol(VARIABLE_NAME, sname, *func);
    range_index_list = AddToSymbList(range_index_list, si);
    return si; 
}

SgStatement *CreateLoopForRange(SgStatement *body, SgExpression *eRange, SgExpression *e, int flag_filler, SgStatement *func)
{
    SgSymbol *s_index = LoopIndex(body,func); 
    SgStatement *loop = new SgForStmt(*s_index, *eRange->lhs(), *eRange->rhs(), *body); 
    if(flag_filler)
        if(isSgAssignStmt(body) && !e)        
            ((SgAssignStmt *) body)->replaceRhs(*new SgVarRefExp(*s_index));
        else
            e->setLhs(*new SgVarRefExp(*s_index));

    return loop;
}

SgStatement *CreateLoopNestForElement(SgStatement *body, SgExpression *edrv, SgExpression *e, int flag_filler, SgStatement *func)
{
    if(isSgArrayRefExp(edrv))
    {
       for(SgExpression *el=edrv->lhs(); el; el=el->rhs())
           body = CreateLoopNestForElement(body, el->lhs(), el, flag_filler, func);
    }
    else if(isSgSubscriptExp(edrv))
    {    body = CreateLoopForRange(body, edrv, e, flag_filler, func);
         body = CreateLoopNestForElement(body, edrv->lhs(), e, flag_filler, func);
         body = CreateLoopNestForElement(body, edrv->rhs(), e, flag_filler, func);
    }
    else
       return body;

    return (body);
}

SgStatement * CreateBodyForElememt(SgSymbol *s_elemCount,SgSymbol *s_elemBuf,SgSymbol *s_elemIndex, SgExpression *edrv, int flag_filler)
{
    SgExpression *e = flag_filler ? new SgVarRefExp(*s_elemIndex) : new SgVarRefExp(*s_elemCount); 
    SgStatement *body = new SgAssignStmt(*e,*e + *new SgValueExp(1));

    if(flag_filler)
    {
        SgStatement *st = new SgAssignStmt(*new SgArrayRefExp(*s_elemBuf,*new SgVarRefExp(*s_elemIndex)),*edrv); //*DvmType_Ref(edrv));
        st->setLexNext(*body);
        body = st;
    }
    return (body);
}

SgStatement *CreateLoopBody_Indirect(SgSymbol *s_elemCount,SgSymbol *s_elemBuf,SgSymbol *s_elemIndex,SgExpression *derived_elem_list,int flag_filler)
{ 
    SgStatement *loop_body = NULL,*current_st=NULL;
    for(SgExpression *el=derived_elem_list; el; el=el->rhs())
    {        
        SgStatement *body = CreateBodyForElememt(s_elemCount,s_elemBuf,s_elemIndex, el->lhs(), flag_filler);
        body = CreateLoopNestForElement(body,el->lhs(),NULL,flag_filler,s_elemCount->scope());
        if(loop_body)
            current_st -> setLexNext(*body);
        else
            loop_body = body;      
        current_st = body;
        while(current_st->lexNext())
            current_st = current_st->lexNext();        
    }
    return (loop_body); 
}

SgStatement *CreateLoopNest_Indirect(SgSymbol *s_low_bound, SgSymbol *s_high_bound, symb_list *dummy_index_list, SgStatement *body)
{   SgStatement *stl = body; 
    symb_list *sl = dummy_index_list;
    int i = 0;
    for ( ; sl; sl=sl->next)
          i++;
    for (sl= dummy_index_list; sl; sl=sl->next,i--)  
        stl = new SgForStmt(*sl->symb, *new SgArrayRefExp(*s_low_bound,*new SgValueExp(i)), *new SgArrayRefExp(*s_high_bound,*new SgValueExp(i)), *stl);
    return (stl);
}

void CreateProcedureBody_Indirect(SgStatement *after,SgSymbol *s_low_bound,SgSymbol *s_high_bound,symb_list *dummy_index_list,SgSymbol *s_elemBuf,SgSymbol *s_elemCount,SgSymbol *s_elemIndex,SgExpression *derived_elem_list,int flag_filler)
{
    loopIndexCount = 0;
    range_index_list = NULL;
    after->insertStmtAfter(*CreateLoopNest_Indirect(s_low_bound,s_high_bound,dummy_index_list,CreateLoopBody_Indirect(s_elemCount,s_elemBuf,s_elemIndex,derived_elem_list,flag_filler)),*after->controlParent());
}

SgStatement *CreateIndirectDistributionProcedure(SgSymbol *sProc,symb_list *paramList,symb_list *dummy_index_list,SgExpression *derived_elem_list,int flag_filler)
{
    SgSymbol *s;
    // create procedure header and end

    SgStatement *st_hedr = CreateHostProcedure(sProc);
    SgStatement *st_end = st_hedr->lexNext();

    // create  dummy argument list
    // elemCount/elemBuf,boundsLow,boundsHigh 
    SgType *tdvm = FortranDvmType();
    SgExpression  *MD = new SgExpression(DDOT, new SgValueExp(0), new SgKeywordValExp("*"), NULL);
    SgArrayType *typearray = new SgArrayType(*tdvm);
    typearray->addRange(*MD);
    SgSymbol *s_elemBuf   = new SgSymbol(VARIABLE_NAME, "elemBuf",   *typearray, *st_hedr);
    SgSymbol *s_elemCount = new SgSymbol(VARIABLE_NAME, "elemCount", *tdvm, *st_hedr);
    
    s = flag_filler ? s_elemBuf : s_elemCount;
    SgExpression *ae = new SgVarRefExp(s);
    SgExpression *arg_list = NULL; //new SgExprListExp(*ae);

    //    <loop-bounds-variables>
   
    SgExpression *aster_expr = new SgKeywordValExp("*");
    SgArrayType *typearray_1 = new SgArrayType(*tdvm);
    typearray_1 -> addRange(* aster_expr);  //( * new SgValueExp(lrank));
    SgSymbol *s_low_bound  = new SgSymbol(VARIABLE_NAME, "boundsLow",  *typearray_1, *st_hedr);
    SgSymbol *s_high_bound = new SgSymbol(VARIABLE_NAME, "boundsHigh", *typearray_1, *st_hedr);

    arg_list = AddElementToList(arg_list, new SgArrayRefExp(*s_high_bound));
    arg_list = AddElementToList(arg_list, new SgArrayRefExp(*s_low_bound));
    arg_list = AddElementToList(arg_list,ae);
    SgExpression *dummy_list = FillerDummyArgumentList(paramList,st_hedr);
    AddListToList(arg_list,dummy_list);
    st_hedr->setExpression(0, *arg_list);
    SgSymbol *s_elemIndex  = new SgSymbol(VARIABLE_NAME, "elemIndex", *tdvm, *st_hedr);

    // make declarations

    SgExpression *el=NULL; 
    SgStatement *stmt=NULL, *st_cur=st_hedr;
    for (el = dummy_list; el; el = el->rhs())
    {
       stmt = el->lhs()->symbol()->makeVarDeclStmt();
       ConstantSubstitutionInTypeSpec(stmt->expr(1)); 
       st_cur->insertStmtAfter(*stmt, *st_hedr);
       st_cur = stmt;
    }
    stmt = s->makeVarDeclStmt();
    stmt->expr(1)->setType(tdvm);
    el = new SgExprListExp(*new SgArrayRefExp(*s_low_bound, *aster_expr));
    el->setRhs(new SgExprListExp(*new SgArrayRefExp(*s_high_bound, *aster_expr)));
    stmt->expr(0)->setRhs(el);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    // make declarations of dummy-idexes and s_elemIndex
    for(symb_list *sl=dummy_index_list; sl; sl=sl->next)
       AddListToList(el,new SgExprListExp(*new SgVarRefExp(*sl->symb)));
     
    if(flag_filler)
    {
       stmt = makeSymbolDeclarationWithInit_F90(s_elemIndex,new SgValueExp(0));
       st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }
    // make procedure body 

    SgStatement *cur = st_end->lexPrev();
    CreateProcedureBody_Indirect(cur,s_low_bound,s_high_bound,dummy_index_list,s_elemBuf,s_elemCount,s_elemIndex,derived_elem_list,flag_filler);

    // add range indexes declarations (to declaration statement for dummy indexes)

    for(symb_list *sl=range_index_list; sl; sl=sl->next)
        AddListToList(el,new SgExprListExp(*new SgVarRefExp(*sl->symb))); 
    
    return (st_hedr);
}

SgSymbol *dvm000SymbolForHost(int host_dvm, SgStatement *hedr)
{                           
    SgArrayType *typearray = new SgArrayType(*FortranDvmType());
    typearray->addRange(*new SgExpression(DDOT, new SgValueExp(host_dvm), new SgValueExp(maxdvm), NULL));
    return(new SgVariableSymb("dvm000", *typearray, *hedr));

}

void ReplaceLoopBounds(SgStatement *first_do, int lrank, SgSymbol *s_low_bound, SgSymbol *s_high_bound)
{
    SgStatement *st;
    SgForStmt *stdo;

    int i;
    // looking through the loop nest 
    for (st = first_do, i = 0; i < lrank; st = st->lexNext(), i++)
    {
        stdo = isSgForStmt(st);
        if (!stdo)
            break;
        if (isSgArrayRefExp(stdo->start()))
            stdo->setStart(*new SgArrayRefExp(*s_low_bound, *new SgValueExp(1 + i)));
        else
        {
            stdo->start()->setLhs(new SgArrayRefExp(*s_low_bound, *new SgValueExp(1 + i)));
            stdo->start()->rhs()->lhs()->lhs()->setLhs(new SgArrayRefExp(*s_low_bound, *new SgValueExp(1 + i)));
        }
        if (isSgArrayRefExp(stdo->end()))
            stdo->setEnd(*new SgArrayRefExp(*s_high_bound, *new SgValueExp(1 + i)));
        else
            stdo->end()->setLhs(new SgArrayRefExp(*s_high_bound, *new SgValueExp(1 + i)));
        if (!stdo->step())
            continue;
        int istep = IntStepForHostHandler(stdo->step());
        SgExpression *estep = (istep ? new SgValueExp(istep) : stdo->step());
        stdo->setStep(*estep);
    }
}

void ReplaceArrayBoundsInDeclaration(SgExpression *e)
{
    SgExpression *el;
    for (el = e->lhs(); el; el = el->rhs())
        el->setLhs(CalculateArrayBound(el->lhs(), e->symbol(), 1));
}

int fromModule(SgExpression *e)
{
   if(!e) return 0;

   if(isSgVarRefExp(e) || e->variant()==CONST_REF)
   {  
      if(IS_BY_USE(e->symbol()) || e->symbol()->scope()->variant()==MODULE_STMT)
      {  
         Add_Use_Module_Attribute();
         return 1;
      }
      else
         return 0;
   } 
   if(isSgArrayRefExp(e))
   {  
      if (e->symbol()->type()->variant()==T_ARRAY && e->symbol()->type()->baseType()->variant()==T_DERIVED_TYPE && (IS_BY_USE(e->symbol()->type()->baseType()->symbol()) || IS_BY_USE(e->symbol())))
      {
         Add_Use_Module_Attribute();
         return 1;
      }
      else
         return 0;
   }
   if(isSgRecordRefExp(e))
   {  
      SgExpression *estr = LeftMostField(e);
                             
      if(IS_BY_USE(estr->symbol()->type()->symbol()) || IS_BY_USE(estr->symbol()))          
      {
          Add_Use_Module_Attribute(); 
          return 1;
      }
      else
          return 0;
      //fromModule(estr);
   }
   if(isSgSubscriptExp(e))
      return (fromModule(e->lhs()) && fromModule(e->rhs()));

   if((!e->lhs() || fromModule(e->lhs())) && (!e->rhs() || fromModule(e->rhs())))
      return 1;
   
   return 0;
}

int fromUsesList(SgExpression *e)
{
   if(!e) return 1;
   SgSymbol *s = e->symbol();
   if(s && !isInUsesList(s)) return 0;
   return fromUsesList(e->lhs()) && fromUsesList(e->rhs());
}

SgSymbol *DeclareSymbolInHostHandler(SgSymbol *var, SgStatement *st_hedr, SgSymbol *loc_var)
{   
    SgSymbol *s = var;
    if(!var) return s;
    if(USE_STATEMENTS_ARE_REQUIRED && IS_BY_USE(var))
       return s;
   
    if (!loc_var && isSgArrayType(s->type()))
       s = ArraySymbolInHostHandler(s, st_hedr);
    else if(loc_var)
       s = loc_var ;

    SgStatement *stmt = s->makeVarDeclStmt();
    if(IS_POINTER_F90(s))
       stmt->setExpression(2,*new SgExpression(POINTER_OP));

    ConstantSubstitutionInTypeSpec(stmt->expr(1));    
    st_hedr->insertStmtAfter(*stmt, *st_hedr);
    return s;
}

int ExplicitShape(SgExpression *eShape)
{
   SgExpression *el;
   SgSubscriptExp *sbe;
   for(el=eShape; el; el=el->rhs())
   { 
      SgExpression *uBound =  (sbe=isSgSubscriptExp(el->lhs())) ? sbe->ubound() : el->lhs();
      if(uBound && uBound->variant()!=STAR_RANGE) 
            continue;
      else
            return 0;
    }              
    return 1;
}

SgSymbol *ArraySymbolInHostHandler(SgSymbol *ar, SgStatement *scope)
{
    SgSymbol *soff;
    SgExpression *edim;
    int rank, i;

    rank = Rank(ar);
    soff = ArraySymbol(ar->identifier(), ar->type()->baseType(), NULL, scope);
    if(!ExplicitShape(isSgArrayType(ar->type())->getDimList()))
        Error("Illegal array bound of private array %s", ar->identifier(), 442, dvm_parallel_dir);

    for (i = 0; i < rank; i++)
    {
        edim = ((SgArrayType *)(ar->type()))->sizeInDim(i);
             //if( IS_BY_USE(ar) || !fromUsesList(edim) && !fromModule(edim) )
             //   edim = CalculateArrayBound(edim, ar, 1); 
        ((SgArrayType *)(soff->type()))->addRange(edim->copy());
    }
    return(soff);
}

void DeclareArrayCoefficients(SgStatement *after)
{
    symb_list *sl;
    SgStatement  *dst;
    SgExpression *e, *el;
    int i, rank;
    coeffs *c;

    for (sl = acc_array_list, el = NULL; sl; sl = sl->next)
    {
        c = AR_COEFFICIENTS(sl->symb);
        rank = Rank(sl->symb);
        for (i = 2; i <= rank; i++)
        {  // doAssignTo_After(new SgVarRefExp(*(c->sc[i])), header_ref(sl->symb,i));
            e = new SgExprListExp(*(c->sc[i])->makeDeclExpr());
            e->setRhs(el);
            el = e;
        }
        e = opt_base ? (&(*header_ref(sl->symb, rank + 2) + *new SgVarRefExp(*(c->sc[1])))) : header_ref(sl->symb, rank + 2);
        //doAssignTo_After(new SgVarRefExp(*(c->sc[rank+2])), e);
        e = new SgExprListExp(*(c->sc[rank + 2])->makeDeclExpr());
        e->setRhs(el);
        el = e;
    }
    if (el)
    {
        dst = after->expr(0)->lhs()->symbol()->makeVarDeclStmt(); // creates INTEGER[*8] name, then name is removed
        dst->setExpression(0, *el);
        after->insertStmtAfter(*dst);
    }

}

SgExpression *CreateBaseMemoryList()
{
    symb_list *sl;
    SgExpression *base_list, *l, *el;
    SgValueExp M0(0);
    SgExpression  *MD = new SgExpression(DDOT, &M0.copy(), new SgKeywordValExp("*"), NULL);

    // create memory base list looking through the acc_array_list
    
    sl = USE_STATEMENTS_ARE_REQUIRED ? MergeSymbList(acc_array_list_whole, acc_array_list) : acc_array_list;
    if (!sl) return(NULL);
    base_list = new SgExprListExp(*new SgArrayRefExp(*baseMemory(sl->symb->type()->baseType())));

    for (sl = sl->next; sl; sl = sl->next)
    {
        for (l = base_list; l; l = l->rhs())
        {       //printf("%d   %d\n",sl->symb->type()->baseType()->variant(),l->lhs()->symbol()->type()->baseType()->variant());  
            if (baseMemory(sl->symb->type()->baseType()) == l->lhs()->symbol())
                //baseMemory(l->lhs()->symbol()->type()->baseType()) )
                break;
        }

        if (!l)
        {
            el = new SgExprListExp(*new SgArrayRefExp(*baseMemory(sl->symb->type()->baseType())));
            el->setRhs(base_list);
            base_list = el;
        }
    }

    for (l = base_list; l; l = l->rhs())
    {
        SgSymbol *sb = &(l->lhs()->symbol()->copy());
        SYMB_SCOPE(sb->thesymb) = cur_in_source->controlParent()->thebif;
        SgArrayType *typearray = new SgArrayType(*l->lhs()->symbol()->type()->baseType());
        typearray->addRange(*MD);       //Dimension(NULL,1,1);
        sb->setType(typearray);
        l->lhs()->setSymbol(sb);
    }
    return(base_list);
}

SgExpression *CreateArrayAdrList(SgSymbol *header_symb, SgStatement *st_host)
{
    symb_list *sl;
    SgExpression *adr_list = NULL;
    int i, rank;
    SgSymbol *sarg, *hl;

    // create array address list looking through the acc_array_list
    sl = acc_array_list;
    if (!sl) return(NULL);
    adr_list = new SgExprListExp(*new SgArrayRefExp(*DummyDvmArraySymbol(sl->symb, header_symb)));

    for (sl = acc_array_list->next, hl = header_symb->next(); sl; sl = sl->next, hl = hl->next())
    {
        SgArrayType *typearray = new SgArrayType(*sl->symb->type()->baseType());
        rank = Rank(sl->symb);
        for (i = 1; i < rank; i++)
            typearray->addRange(*Dimension(hl, i, rank));
        typearray->addRange(*Dimension(hl, rank, rank));

        sarg = DummyDvmArraySymbol(sl->symb, hl);
        adr_list->setRhs(*new SgExprListExp(*new SgArrayRefExp(*sarg)));
        adr_list = adr_list->rhs();
        /*
         el = new SgExprListExp(*new SgArrayRefExp(*sarg));
         el->setRhs(adr_list);
         adr_list = el;
         */
    }
    return(adr_list);
}

SgSymbol *HeaderSymbolForHandler(SgSymbol *ar)
{
 SgSymbol *shead;
 if(HEADER_FOR_HANDLER(ar))
    shead = *HEADER_FOR_HANDLER(ar);
 else 
 {
    shead =  DummyDvmHeaderSymbol(ar,cur_func);
    SgSymbol **s_attr = new (SgSymbol *);
    *s_attr = shead;
    ar->addAttribute(HANDLER_HEADER, (void*)s_attr, sizeof(SgSymbol *));
 }
 return (shead);
}

SgExpression *FirstArrayElementSubscriptsForHandler(SgSymbol *ar)
{//generating reference AR(L1,...,Ln), where Li - lower bound of i-th dimension
 // Li = AR_header(rank+2+i)
 int i;
 SgExpression *esl=NULL, *el=NULL;
 SgExpression *bound[MAX_DIMS], *ebound;
 
 SgSymbol *shead = HeaderSymbolForHandler(ar);
 int rank = Rank(ar); 
 for (i = rank; i; i--)
    bound[i-1] = Calculate(LowerBound(ar,i-1));  
 for (i = rank; i; i--) {
    if(bound[i-1]->isInteger() && !IS_BY_USE(ar))
       ebound =  new SgValueExp(bound[i-1]->valueInteger());
    else
       ebound =  new SgArrayRefExp(*shead,*new SgExprListExp(*new SgValueExp(rank+2+i)));
    esl = new SgExprListExp(*ebound); 
    esl->setRhs(el);
    el = esl;
 }
 return(el);
}


SgSymbol *DummyDvmHeaderSymbol(SgSymbol *ar, SgStatement *st_hedr)
{
    SgArrayType *typearray = new SgArrayType(*FortranDvmType());
    typearray->addRange(*new SgValueExp(2*Rank(ar) + 2));
    char *name = options.isOn(O_HOST) ? Header_DummyArgName(ar) : ar->identifier();
    return (new SgSymbol(VARIABLE_NAME, name, *typearray, *st_hedr));
}

SgSymbol *DummyDvmArraySymbol(SgSymbol *ar, SgSymbol *header_symb)
{
    SgArrayType *typearray = new SgArrayType(*ar->type()->baseType());
    int i, rank;
    rank = Rank(ar);
    for (i = 1; i < rank; i++)
        typearray->addRange(*Dimension(header_symb, i, rank));
    typearray->addRange(*Dimension(header_symb, rank, rank));
    return(new SgSymbol(VARIABLE_NAME, ar->identifier(), *typearray, *header_symb->scope()));
}

SgExpression *Dimension(SgSymbol *hs, int i, int rank)
{
    SgValueExp M0(0), M1(1);
    //SgExpression  *MD =  new SgExpression(DDOT,&M0.copy(),new SgKeywordValExp("*"),NULL); 
    SgExpression  *me;


    if (i == rank)
        return(new SgExpression(DDOT, &M0.copy(), new SgKeywordValExp("*"), NULL));
    if (i == 1)
        return(new SgExpression(DDOT, &M0.copy(), &(*new SgArrayRefExp(*hs, *new SgValueExp(rank)) - M1), NULL));
    //me = new SgArrayRefExp(*hs,*new SgValueExp(rank));
    //for(j=rank; j>rank-i+2; j--)
    //me = &(*me * *new SgArrayRefExp(*hs,*new SgValueExp(j-1)) );
    me = new SgArrayRefExp(*hs, *new SgValueExp(rank - i + 2));
    return(new SgExpression(DDOT, &M0.copy(), &(*new SgArrayRefExp(*hs, *new SgValueExp(rank - i + 1)) / (*me) - M1), NULL));

}

SgExpression *ConstRef_F95(int ic)
{
    SgExpression *kind, *ce;

    ce = new SgValueExp(ic);
    if (len_DvmType && !type_with_len_DvmType)
    {
        type_with_len_DvmType = new SgType(T_INT);
        kind = new SgValueExp(len_DvmType);
        TYPE_KIND_LEN(type_with_len_DvmType->thetype) = kind->thellnd;
    }
    if (len_DvmType)
        ce->setType(type_with_len_DvmType);

    return(ce);
}

SgExpression *DvmType_Ref(SgExpression *e) 
{
    if (e->variant() == INT_VAL)
        return(ConstRef_F95(((SgValueExp *)e)->intValue()));
    return( len_DvmType ? TypeFunction(SgTypeInt(),e,new SgValueExp(len_DvmType) ) : e);
}

SgSymbol *indexArraySymbol(SgSymbol *ar)
{
    if (index_array_symb)
        return(index_array_symb);

    //creating new symbol

    index_array_symb = ArraySymbol("indexArray", FortranDvmType(), new SgValueExp(MaxArrayRank()), cur_in_source->controlParent());

    return(index_array_symb);

}

char *Header_DummyArgName(SgSymbol *s)
{
    char *name;

    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 6));
    sprintf(name, "%s_head", s->identifier());
    return(TestAndCorrectName(name));
}

int ParLoopRank()
{
    int nloop;
    SgExpression *dovar;

    // looking through the do_variables list

    for (dovar = dvm_parallel_dir->expr(2), nloop = 0; dovar; dovar = dovar->rhs())
        nloop++;
    return(nloop);
}

int MaxArrayRank()
{
    symb_list *sl;
    int max_rank = 0;
    int rank;
    for (sl = acc_array_list; sl; sl = sl->next)
    {
        rank = Rank(sl->symb);
        max_rank = (max_rank < rank) ? rank : max_rank;
    }
    return(max_rank);
}

int OneSteps(int nl, SgStatement *nest)
{
    int i;
    SgExpression *dostep, *ec;
    SgStatement *stdo;
    // looking through the loop nest

    for (stdo = nest, i = nl; i; stdo = stdo->lexNext(), i--)
    {
        dostep = ((SgForStmt *)stdo)->step();
        if (!dostep) continue;   //by default do_step == 1
        ec = Calculate(dostep);
        if (ec->isInteger() && ec->valueInteger() == 1) // do_step == 1
            continue;
        break;
    }
    if (i == 0)     //all do_step == 1
        return(1);
    else
        return(0);
}

int IConstStep(SgStatement *stdo)
{
    SgExpression *dostep, *ec;
    dostep = ((SgForStmt *)stdo)->step();
    if (!dostep)
        return(1);   //by default do_step == 1
    if (((SgForStmt *)stdo)->start()->variant() == ADD_OP) //redblack scheme
        return(1);
    if (dostep->variant() == INT_VAL)
        return(((SgValueExp *)dostep)->intValue());    //NODE_INT_CST_LOW (dostep->thellnd);
    ec = Calculate(dostep);
    if (ec->isInteger())
        return(ec->valueInteger());

    err("Non constant do step is not implemented yet", 593, stdo);
    return(0);
}


int TestParLoopSteps(SgStatement *first_do, int n)
{
    int i;
    SgExpression *dostep, *ec;
    SgStatement *stdo;
    for (i = n, stdo = first_do; i; i--, stdo = stdo->lexNext())
    {
        dostep = ((SgForStmt *)stdo)->step();
        if (!dostep)
            continue;   //by default do_step == 1
        if (((SgForStmt *)stdo)->start()->variant() == ADD_OP) //redblack scheme
            continue;
        if (dostep->variant() == INT_VAL)
        {
            if (((SgValueExp *)dostep)->intValue() == 1)
                continue;
            else
                return(0);
        }
        ec = Calculate(dostep);
        if (ec->isInteger())
        {
            if (ec->valueInteger() == 1)
                continue;
            else
                return(0);
        }
        return(0);
    }
    return(1);
}

int IntStepForHostHandler(SgExpression *dostep)
{
    SgExpression *ec;
    if (!dostep)
        return(1);   //by default do_step == 1
    ec = Calculate(ReplaceParameter(dostep));
    if (ec->isInteger())
        return(ec->valueInteger());
    return(0);
}

void ConstantSubstitutionInTypeSpec(SgExpression *e)
{
  SgType *t = e->type(); 
  if(!TYPE_KIND_LEN(t->thetype)) return;
  if(t->selector()->variant()==INT_VAL) return;
  SgType *new_t= &(t->copy());
  TYPE_KIND_LEN(new_t->thetype) = ReplaceParameter(new_t->selector())->thellnd;
  e->setType(new_t);
  return;
}

char * BoundName(SgSymbol *s, int i, int isLower)
{
    char *name = new char[strlen(s->identifier()) + 13];
    if(isLower)
      sprintf(name, "lbound%d_%s", i, s->identifier());
    else
      sprintf(name, "ubound%d_%s", i, s->identifier());
    name = TestAndCorrectName(name);
    return(name);
}

SgSymbol *DummyBoundSymbol(SgSymbol *rv, int i, int isLower, SgStatement *st_hedr)
{
  SgExpression *bound;
  bound = isLower ? Calculate(LowerBound(rv,i)) : Calculate(UpperBound(rv,i));
  if(bound->isInteger())
     return NULL;
  return(new SgVariableSymb(BoundName(rv, i+1, isLower), *SgTypeInt(), *st_hedr));
}

SgExpression *CreateDummyBoundListOfArray(SgSymbol *ar, reduction_operation_list *rl, SgStatement *st_hedr)
{
    SgExpression *sl = NULL;
    SgSymbol *low_s, *upper_s, *new_ar;
    SgExpression *up_bound, *low_bound;
    SgArrayType *typearray;
    int i;
    if(isSgArrayType(ar->type()))
    {
        new_ar = ArraySymbol(ar->identifier(), ar->type()->baseType(), NULL, st_hedr);
        typearray = isSgArrayType(new_ar->type());
    }
    if(rl)
        rl->red_host = new_ar;
    for(i=0; i<Rank(ar); i++) 
    {    
        if(low_s = DummyBoundSymbol(ar,i,1,st_hedr )) 
           sl = AddListToList( sl,  new SgExprListExp(*(low_bound = new SgVarRefExp(low_s))) );
      
        if(upper_s = DummyBoundSymbol(ar, i, 0, st_hedr)) 
           sl = AddListToList( sl,  new SgExprListExp(*(up_bound = new SgVarRefExp(upper_s))) );

        typearray->addRange(*new SgExpression(DDOT, low_s ? low_bound : Calculate(LowerBound(ar,i)), upper_s ? up_bound : Calculate(UpperBound(ar,i)))
); 
    }

    return(sl);
}

SgExpression * DummyListForReductionArrays(SgStatement *st_hedr)
{
    reduction_operation_list *rl;
    SgExpression *dummy_list = NULL;
    for (rl = red_struct_list; rl; rl = rl->next)
    {
        if (rl->redvar_size != 0)
            dummy_list = AddListToList(dummy_list, CreateDummyBoundListOfArray(rl->redvar,rl,st_hedr)); 
    } 
    return  dummy_list;
}

/***************************************************************************************/
/*ACC*/
/*   Creating and Inserting New Statement in the Program                               */
/* (Fortran Language, .cuf file)                                                       */
/***************************************************************************************/

SgSymbol *SyncthreadsSymbol()
{
    if (sync_proc_symb)
        return(sync_proc_symb);
    if (options.isOn(C_CUDA))
        sync_proc_symb = new SgSymbol(PROCEDURE_NAME, "__syncthreads", *mod_gpu);
    else
        sync_proc_symb = new SgSymbol(PROCEDURE_NAME, "syncthreads", *mod_gpu);
    return(sync_proc_symb);
}

void CudaVars()
{
    if (s_threadidx)
        return;
    s_threadidx = new SgVariableSymb("threadIdx", *t_dim3, *mod_gpu);
    s_blockidx = new SgVariableSymb("blockIdx", *t_dim3, *mod_gpu);
    s_blockdim = new SgVariableSymb("blockDim", *t_dim3, *mod_gpu);
    s_griddim = new SgVariableSymb("gridDim", *t_dim3, *mod_gpu);
    s_warpsize = new SgVariableSymb("warpSize", *SgTypeInt(), *mod_gpu);
}

void SymbolOfCudaOffsetType()
{
    s_offset_type = new SgVariableSymb("symb_offset", *CudaOffsetType(), *mod_gpu);
}

void SymbolOfCudaIndexType()
{
    s_of_cudaindex_type = new SgVariableSymb("symb_cudaindex", *CudaIndexType(), *mod_gpu);
}

void KernelWorkSymbols()
{
    char *name;

    if (s_ibof) return;
    name = TestAndCorrectName("ibof");
    s_ibof = new SgVariableSymb(name, *SgTypeInt(), *mod_gpu);
    if (s_blockDims) return;
    name = TestAndCorrectName("blockDims");
    s_blockDims = new SgVariableSymb(name, *SgTypeInt(), *mod_gpu);
    return;
}


void KernelBloksSymbol()
{
    SgValueExp M1(1), M0(0);
    SgExpression  *M01 = new SgExpression(DDOT, &M0.copy(), &M1.copy(), NULL);

    if (s_blocks_k) return;

    if (options.isOn(C_CUDA))
    {
        s_CudaIndexType_k = new SgSymbol(TYPE_NAME, "CudaIndexType", *mod_gpu);
        CudaIndexType_k = C_Derived_Type(s_CudaIndexType_k);
        s_blocks_k = ArraySymbol(TestAndCorrectName("blocks"), CudaIndexType_k, (SgExpression *)&M0, mod_gpu);
        s_rest_blocks = new SgVariableSymb(TestAndCorrectName("rest_blocks"), CudaIndexType_k, mod_gpu);
        s_cur_blocks = new SgVariableSymb(TestAndCorrectName("cur_blocks"), CudaIndexType_k, mod_gpu);
        s_add_blocks = new SgVariableSymb(TestAndCorrectName("add_blocks"), CudaIndexType_k, mod_gpu);
    }
    else
    {
        s_blocks_k = ArraySymbol(TestAndCorrectName("blocks"), CudaIndexType(), M01, mod_gpu);
        s_rest_blocks = new SgVariableSymb(TestAndCorrectName("rest_blocks"), CudaIndexType(), mod_gpu);
        s_cur_blocks = new SgVariableSymb(TestAndCorrectName("cur_blocks"), CudaIndexType(), mod_gpu);
        s_add_blocks = new SgVariableSymb(TestAndCorrectName("add_blocks"), CudaIndexType(), mod_gpu);
    }
    return;
}

void KernelBaseMemorySymbols()
{
    SgValueExp M1(1), M0(0);
    SgExpression  *M01 = new SgExpression(DDOT, &M0.copy(), &M1.copy(), NULL);
    //SgArrayType *typearray;

    Imem_k = ArraySymbol("i0000m", SgTypeInt(), M01, mod_gpu);
    Rmem_k = ArraySymbol("r0000m", SgTypeFloat(), M01, mod_gpu);
    Dmem_k = ArraySymbol("d0000m", SgTypeDouble(), M01, mod_gpu);

    Lmem_k = ArraySymbol("l0000m", SgTypeBool(), M01, mod_gpu);
    Cmem_k = ArraySymbol("c0000m", SgTypeComplex(current_file), M01, mod_gpu);
    DCmem_k = ArraySymbol("dc000m", SgTypeDoubleComplex(current_file), M01, mod_gpu);
    Chmem_k = ArraySymbol("ch000m", SgTypeChar(), M01, mod_gpu);
}

SgSymbol *FormalLocationSymbol(SgSymbol *locvar, int i)
{
    SgType *type;
    char *name;

    name = (char *)malloc((unsigned)(strlen(locvar->identifier()) + 6));
    sprintf(name, "%s__%d", locvar->identifier(), i);
    type = isSgArrayType(locvar->type()) ? (locvar->type()->baseType()) : locvar->type();
    if (options.isOn(C_CUDA))
        type = C_Type(type);
    return(new SgVariableSymb(name, *type, *kernel_st));
}

SgSymbol *FormalDimSizeSymbol(SgSymbol *var, int i)
{
    SgType *type;

    type = options.isOn(C_CUDA) ? C_DvmType() : FortranDvmType();
    return(new SgVariableSymb(DimSizeName(var, i), *type, *kernel_st));
}

SgSymbol *FormalLowBoundSymbol(SgSymbol *var, int i)
{
    SgType *type;

    type = options.isOn(C_CUDA) ? C_DvmType() : FortranDvmType();
    return(new SgVariableSymb(BoundName(var, i, 1), *type, *kernel_st));
}

SgType *Type_For_Red_Loc(SgSymbol *redsym, SgSymbol *locsym, SgType *redtype, SgType *loctype)
{
    char *tname;
    tname = (char *)malloc((unsigned)(strlen(redsym->identifier()) + (strlen(locsym->identifier()) + 7)));
    sprintf(tname, "%s_%s_type", redsym->identifier(), locsym->identifier());

    SgSymbol *stype = new SgSymbol(TYPE_NAME, tname, *kernel_st);
    SgFieldSymb *sred = new SgFieldSymb(redsym->identifier(), *redtype, *stype);
    SgFieldSymb *sloc = new SgFieldSymb(locsym->identifier(), *loctype, *stype);

    SYMB_NEXT_FIELD(sred->thesymb) = sloc->thesymb;

    SYMB_NEXT_FIELD(sloc->thesymb) = NULL;

    SgType *tstr = new SgType(T_STRUCT);
    TYPE_COLL_FIRST_FIELD(tstr->thetype) = sred->thesymb;
    stype->setType(tstr);

    SgType *td = new SgType(T_DERIVED_TYPE);
    TYPE_SYMB_DERIVE(td->thetype) = stype->thesymb;
    TYPE_SYMB(td->thetype) = stype->thesymb;

    return(td);
}

SgSymbol *RedBlockSymbolInKernel(SgSymbol *s, SgType *type)
{
    char *name;
    SgSymbol *sb;
    SgValueExp M0(0);
    SgExpression  *MD = new SgExpression(DDOT, &M0.copy(), new SgKeywordValExp("*"), NULL);
    SgArrayType *typearray;
    SgType *tp;
    int i = 1;
    if (!type)
    {
        tp = s->type()->baseType();
        if (options.isOn(C_CUDA))
            tp = C_Type(tp);
        typearray = new SgArrayType(*tp);
    }
    else if (isSgArrayType(s->type()))
        typearray = (SgArrayType *)&(s->type()->copy());
    else
        typearray = new SgArrayType(*type);

    if (!options.isOn(C_CUDA))
        typearray->addRange(*MD);
    else
        typearray->addDimension(NULL);

    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 8));

    sprintf(name, "%s_block", s->identifier());

    while (isSameNameShared(name))
        sprintf(name, "%s_block%d", s->identifier(), i++);

    sb = new SgVariableSymb(name, *typearray, *kernel_st); // scope may be mod_gpu
    shared_list = AddToSymbList(shared_list, sb);

    return(sb);
}

SgSymbol *RedFunctionSymbolInKernel(char *name)
{
    return(new SgFunctionSymb(FUNCTION_NAME, name, *SgTypeInt(), *kernel_st));
}

SgSymbol *isSameNameShared(char *name)
{
    symb_list *sl;
    for (sl = shared_list; sl; sl = sl->next)
    {
        if (!strcmp(sl->symb->identifier(), name))
            return(sl->symb);
    }
    return(NULL);
}


SgSymbol *IndVarInKernel(SgSymbol *s)
{
    char *name;
    SgSymbol *soff;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 4));
    sprintf(name, "%s__1", s->identifier());
    soff = new SgVariableSymb(name, *IndexType(), *kernel_st);
    return(soff);
}

SgSymbol *IndexSymbolForRedVarInKernel(int i)
{
    char *name = new char[10];
    SgSymbol *soff;

    sprintf(name, "k_k%d", i);
    soff = new SgVariableSymb(TestAndCorrectName(name), *IndexType(), *kernel_st);
    return(soff);
}

SgSymbol *RemoteAccessBufferInKernel(SgSymbol *ar, int rank)
{
    int i = 1;
    int j;
    int *index = new int;
    char *name;
    SgSymbol *sn;
    SgArrayType *typearray;

    SgExpression  *rnk = new SgValueExp(rank + DELTA);
    name = (char *)malloc((unsigned)(strlen(ar->identifier()) + 4 + 3 + 1));
    sprintf(name, "%s_rma", ar->identifier());
    typearray = new SgArrayType(*ar->type()->baseType());
    for (j = rank; j; j--)
        typearray->addRange(*rnk);
    while (isSameNameBuffer(name, rma->rml))
        sprintf(name, "%s_rma%d", ar->identifier(), i++);
    sn = new SgVariableSymb(name, *typearray, *mod_gpu);

    *index = 1;
    // adding the attribute (ARRAY_HEADER) to buffer symbol
    sn->addAttribute(ARRAY_HEADER, (void*)index, sizeof(int));

    return(sn);
}

SgSymbol *DummyReplicatedArray(SgSymbol *ar, int rank)
{//int i = 1;
    int j;
    int *index = new int;
    char *name;
    SgSymbol *sn;
    SgArrayType *typearray;
    coeffs *scoef = new coeffs;

    SgExpression  *rnk = new SgValueExp(rank + DELTA);
    name = (char *)malloc((unsigned)(strlen(ar->identifier()) + 1));
    sprintf(name, "%s", ar->identifier());
    typearray = new SgArrayType(*ar->type()->baseType());
    for (j = rank; j; j--)
        typearray->addRange(*rnk);
    sn = new SgVariableSymb(name, *typearray, *mod_gpu);

    *index = 1;
    // adding the attribute (ARRAY_HEADER) to buffer symbol
    sn->addAttribute(ARRAY_HEADER, (void*)index, sizeof(int));
    // creating variables used for optimisation buffer references in parallel loop
    CreateCoeffs(scoef, ar);

    // adding the attribute (ARRAY_COEF) to  buffer symbol
    sn->addAttribute(ARRAY_COEF, (void*)scoef, sizeof(coeffs));

    return(sn);
}


SgSymbol *isSameNameBuffer(char *name, SgExpression *rml)
{
    SgExpression *el;
    rem_var *remv;
    for (el = rml; el; el = el->rhs())
    {
        remv = (rem_var *)(el->lhs())->attributeValue(0, REMOTE_VARIABLE);       
        if (remv && remv->buffer && !strcmp(remv->buffer->identifier(), name))
            return(remv->buffer);
    }
    return(NULL);
}
/*
coeffs *BufferCoeffs(SgSymbol *sbuf,SgSymbol *ar)
{int i,r,i0;
char *name;
coeffs *scoef = new coeffs;
r=Rank(ar);
i0 = opt_base ? 1 : 2;
//if(opt_loop_range) i0=0;
for(i=i0;i<=r+2;i++)
{ name = new char[80];
sprintf(name,"%s%s%d",sbuf->identifier(),"000",i);
scoef->sc[i] =  new SgVariableSymb(name, *SgTypeInt(), *cur_func);
//printf("%s",(scoef->sc[i])->identifier());
}
scoef->use = 0;
return(scoef);
}
*/

SgSymbol *RedGridSymbolInKernel(SgSymbol *s, int n, SgExpression *dimSizeArgs, int is_red_or_loc_var)
{
    char *name;
    SgSymbol *soff;
    SgType *type;
    SgValueExp M1(1), M0(0);
    SgExpression  *M01 = new SgExpression(DDOT, &M0.copy(), new SgKeywordValExp("*"), NULL);

    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 6));
    sprintf(name, "%s_grid", s->identifier());
    type = isSgArrayType(s->type()) ? s->type()->baseType() : s->type();
    if (options.isOn(C_CUDA))
        type = C_Type(type); //C_PointerType(C_Type(type));
    if (is_red_or_loc_var == 1) // for reduction variable        
    {
        if (n > 0)
        {
            if (options.isOn(C_CUDA))
                soff = ArraySymbol(name, type, (SgExpression *)&M0, kernel_st);
            else
            {
                soff = ArraySymbol(name, type, new SgExpression(DDOT, &M0.copy(), &(*new SgVarRefExp(s_overall_blocks) - M1.copy()), NULL), kernel_st);
                ((SgArrayType *)(soff->type()))->addRange(*new SgValueExp(n));
            }
        }
        else if (n < 0)
        {
            if (options.isOn(C_CUDA))
                soff = ArraySymbol(name, type, (SgExpression *)&M0, kernel_st);
            else
            {
                SgExpression *sl;
                soff = ArraySymbol(name, type, new SgExpression(DDOT, &M0.copy(), &(*new SgVarRefExp(s_overall_blocks) - M1.copy()), NULL), kernel_st);
                for (sl = dimSizeArgs; sl; sl = sl->rhs())
                    ((SgArrayType *)(soff->type()))->addRange(sl->lhs()->copy());
            }
        }
        else
            soff = options.isOn(C_CUDA) ? ArraySymbol(name, type, (SgExpression *)&M0, kernel_st) : ArraySymbol(name, type, M01, kernel_st);
    }
    else //for location variable
    {
        if (options.isOn(C_CUDA))
            soff = ArraySymbol(name, type, (SgExpression *)&M0, kernel_st);
        else
        {
            soff = ArraySymbol(name, type, new SgValueExp(n), kernel_st);
            ((SgArrayType *)(soff->type()))->addRange(*M01);
        }
    }

    return(soff);
}

SgSymbol *RedInitValSymbolInKernel(SgSymbol *s, SgExpression *dimSizeArgs)
{
    char *name;
    SgSymbol *soff;
    SgType *type;
    SgExpression *sl;

    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 6));
    sprintf(name, "%s_init", s->identifier());
    type = isSgArrayType(s->type()) ? s->type()->baseType() : s->type();
    if (options.isOn(C_CUDA))
        type = C_PointerType(C_Type(type));

    soff = ArraySymbol(name, type, NULL, kernel_st);
    for (sl = dimSizeArgs; sl; sl = sl->rhs())
        ((SgArrayType *)(soff->type()))->addRange(sl->lhs()->copy());

    return(soff);
}

SgSymbol *RedVariableSymbolInKernel(SgSymbol *s, SgExpression *dimSizeArgs)
{
    char *name;
    SgSymbol *soff;
    SgType *type;
    SgExpression *sl, *edim;
    int i, rank;
    rank = Rank(s);
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 1));
    sprintf(name, "%s", s->identifier());
    type = isSgArrayType(s->type()) ? s->type()->baseType() : s->type();
    if (options.isOn(C_CUDA))
        type = C_Type(type);
    if (rank > 0)
    {
        if (options.isOn(C_CUDA))
        {
            type = C_PointerType(type);
            return(new SgVariableSymb(name, *type, *kernel_st));
        }
        soff = ArraySymbol(name, type, NULL, kernel_st);
    }
    else
        return(new SgVariableSymb(name, *type, *kernel_st));
    if (!dimSizeArgs)
    {
        if (!options.isOn(C_CUDA))
        {
            for (i = 0; i < rank; i++)
            {
                edim = ((SgArrayType *)(s->type()))->sizeInDim(i);
                edim = CalculateArrayBound(edim, s, 0);
                if (edim)
                    ((SgArrayType *)(soff->type()))->addRange(edim->copy());
            }
        }
        else
        {
            for (i = rank - 1; i >= 0; i--)
            {
                edim = ((SgArrayType *)(s->type()))->sizeInDim(i);
                edim = CalculateArrayBound(edim, s, 0);
                if (edim)
                    ((SgArrayType *)(soff->type()))->addRange(edim->copy());
            }
        }
    }
    else
    {
        for (sl = dimSizeArgs, i = 0; sl; sl = sl->rhs(), i++)
        {
            edim = ((SgArrayType *)(s->type()))->sizeInDim(i);

            //if(edim->variant() == CONST_REF) {
            //  SgConstantSymb * sc =  isSgConstantSymb(edim->symbol());
            //  edim = Calculate(sc->constantValue());
            //}
            edim = Calculate(edim);
            if (edim->variant() == INT_VAL)
                ((SgArrayType *)(soff->type()))->addRange(edim->copy());
            else
                ((SgArrayType *)(soff->type()))->addRange(sl->lhs()->copy());
        }
    }
    return(soff);
}

SgSymbol *SymbolInKernel(SgSymbol *s)
{
    char *name;
    SgSymbol *soff;
    SgType *type;
    SgExpression *edim;
    int i, rank;

    if (!isSgArrayType(s->type()))  //scalar variable
    {
        if (!options.isOn(C_CUDA))
            return s;
        else
            return new SgVariableSymb(s->identifier(), *C_Type(s->type()), *kernel_st);
    }
    rank = Rank(s);
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 1));
    sprintf(name, "%s", s->identifier());
    type = isSgArrayType(s->type()) ? s->type()->baseType() : s->type();
    if (options.isOn(C_CUDA))
        type = C_Type(type);
    soff = ArraySymbol(name, type, NULL, kernel_st);
    if (!options.isOn(C_CUDA))
    for (i = 0; i < rank; i++)
    {
        edim = ((SgArrayType *)(s->type()))->sizeInDim(i);
        edim = CalculateArrayBound(edim, s, 1);
        if (edim)
            ((SgArrayType *)(soff->type()))->addRange(edim->copy());
    }
    else
    for (i = rank - 1; i >= 0; i--)
    {
        edim = ((SgArrayType *)(s->type()))->sizeInDim(i);
        edim = CalculateArrayBound(edim, s, 1);
        if (edim)
            ((SgArrayType *)(soff->type()))->addRange(edim->copy());
    }

    return(soff);
}

SgExpression *CalculateArrayBound(SgExpression *edim, SgSymbol *ar, int flag_private)
{
    SgSubscriptExp *sbe;
    SgExpression *low;
    if (!edim && flag_private)
    {
        Error("Illegal array bound of private/reduction array %s", ar->identifier(), 442, dvm_parallel_dir);
        return (edim);
    }
    if ((sbe = isSgSubscriptExp(edim)) != NULL){    //DDOT

        if (!sbe->ubound() && flag_private)
        {
            Error("Illegal array bound of private/reduction array %s", ar->identifier(), 442, dvm_parallel_dir);
            return(edim);
        }

        if (options.isOn(C_CUDA) && for_kernel)
        {
            low = CalculateArrayBound(sbe->lbound(), ar, flag_private);
            if (!low)
                low = new SgValueExp(1);
            edim = CalculateArrayBound(&((sbe->ubound()->copy()) - (low->copy()) + *new SgValueExp(1)), ar, flag_private);
            return(edim);
        }
        else
        {
            edim = new SgExpression(DDOT);
            edim->setLhs(CalculateArrayBound(sbe->lbound(), ar, flag_private));
            edim->setRhs(CalculateArrayBound(sbe->ubound(), ar, flag_private));
            return(edim);
        }
    }
    else
    {
        edim = Calculate(edim);
        if (edim->variant() != INT_VAL && flag_private )
            Error("Illegal array bound of private/reduction array %s", ar->identifier(), 442, dvm_parallel_dir);
        return (edim);
    }
}


SgSymbol *LocalPartSymbolInKernel(SgSymbol *ar)
{
    char *name;
    SgSymbol *s_part;
    SgValueExp M0(0);
    SgExpression  *M2R = new SgExpression(DDOT, &M0.copy(), new SgValueExp(2 * Rank(ar) - 1), NULL);
    name = (char *)malloc((unsigned)(strlen(ar->identifier()) + 6));
    sprintf(name, "%s_part", ar->identifier());

    s_part = ArraySymbol(name, CudaIndexType(), M2R, kernel_st);
    return(s_part);
}


SgSymbol *LocalPartArray(SgSymbol *ar)
{
    local_part_list *pl;
    for (pl = lpart_list; pl; pl = pl->next)
    if (pl->dvm_array == ar)
        return(pl->local_part);
    //creating local part array
    pl = new local_part_list;
    pl->dvm_array = ar;
    pl->local_part = LocalPartSymbolInKernel(ar);
    pl->next = lpart_list;
    lpart_list = pl;
    return(pl->local_part);
}

SgExpression *LocalityConditionInKernel(SgSymbol *ar, SgExpression *ei[])
{
    SgExpression *cond;
    int N, i;
    SgSymbol *part;

    N = Rank(ar);

    //           ar_part(0) .le. ei[N-1] .and.      ar_part(1) .ge. ei[N-1]
    // .and.     ar_part(2) .le. ei[N-2] .and.      ar_part(3) .ge. ei[N-2]
    //     . . .
    // .and. ar_part(2*N-2) .le. ei[0]   .and.  ar_part(2*N-1) .ge. ei[0]   

    part = LocalPartArray(ar);

    cond = &operator && (operator <= (*VECTOR_REF(part, 0), *ei[N - 1]), operator >= (*VECTOR_REF(part, 1), *ei[N - 1]));
    for (i = 1; i < N; i++)
        cond = &operator && (*cond, operator && (operator <= (*VECTOR_REF(part, 2 * i), *ei[N - 1 - i]), operator >= (*VECTOR_REF(part, 2 * i + 1), *ei[N - 1 - i])));

    return(cond);

}

void InsertInKernel_NewStatementAfter(SgStatement *stat, SgStatement *current, SgStatement *cp)
{
    SgStatement *st;

    st = current;
    if (current->variant() == LOGIF_NODE)   // Logical IF
        st = current->lexNext();
    if (cp->variant() == LOGIF_NODE)
        LogIf_to_IfThen(cp);
    st->insertStmtAfter(*stat, *cp);
    cur_in_kernel = stat;
}

SgExpression *ConditionForRedBlack(SgExpression *erb)
{
    return(&SgEqOp(*IandFunction(erb, new SgValueExp(1)), *new SgValueExp(0)));
}

SgExpression *KernelCondition(SgSymbol *sind, SgSymbol *sblock, int level)
{
    SgExpression *cond;
    int N;
    // i .le. blocks(ibof + N), N = 1 + 2*level 

    N = 1 + 2 * level;
    cond = &operator <= (*new SgVarRefExp(sind), *blocksRef(sblock, N)); //  *new SgArrayRefExp(*base, (*new SgVarRefExp(s_ibof)+(*new SgValueExp(N)))  ) );
    return(cond);
}

SgExpression *KernelCondition2(SgStatement *dost, int level)
{
    SgExpression *cond = NULL;
    SgSymbol *sind = NULL;
    int istep;
    // <ind_level> .le. end_<level>

    sind = dost->symbol();
    istep = IConstStep(dost);
    if (istep >= 0)
        cond = &operator <= (*new SgVarRefExp(sind), *new SgVarRefExp(s_end[level - 1]));
    else if (istep < 0)
        cond = &operator >= (*new SgVarRefExp(sind), *new SgVarRefExp(s_end[level - 1]));
    //else   !!! not implemented
    return(cond);
}

SgExpression *KernelConditionWithDoStep(SgStatement *stdo, SgSymbol *sblock, int level)
{
    SgExpression *cond = NULL;
    SgSymbol *sind = stdo->symbol();
    int N, istep;

    // i .le. blocks(ibof + N), N = 1 + 2*level , do-step is literal constant > 0  
    // i .ge. blocks(ibof + N), N = 1 + 2*level , do-step is literal constant < 0
    // (<do-step> .gt.0 and i .le. blocks(ibof+N)) .or.  (<do-step> .lt.0 and i .ge. blocks(ibof+N)), otherwise

    N = 1 + 2 * level;
    //do_step = ((SgForStmt *)stdo)->step();
    istep = IConstStep(stdo);
    if (istep >= 0)
        cond = &operator <= (*new SgVarRefExp(sind), *blocksRef(sblock, N));
    else if (istep < 0)
        cond = &operator >= (*new SgVarRefExp(sind), *blocksRef(sblock, N));
    //else   !!! not implemented

    return(cond);
}


SgStatement *doIfThenConstrForKernel(SgExpression *cond, SgStatement *if_st)
{
    SgStatement *if_res = NULL;
    // SgExpression *ea;
    // creating
    //          IF ( <cond>) THEN
    //                <if_st>
    //          ENDIF 
    // 

    if_res = new SgIfStmt(*cond, *if_st);

    //  ifst->lexNext()->extractStmt(); // extracting CONTINUE statement
    return(if_res);
}


void CreateGPUModule()
{
    SgStatement *fileHeaderSt = NULL;
    SgStatement *st_mod = NULL, *st_end = NULL;

    fileHeaderSt = current_file->firstStatement();
    if (mod_gpu_symb)
        return;

    mod_gpu_symb = GPUModuleSymb(fileHeaderSt);

    st_mod = new SgStatement(MODULE_STMT);
    st_mod->setSymbol(*mod_gpu_symb);
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*mod_gpu_symb);
    fileHeaderSt->insertStmtAfter(*st_mod, *fileHeaderSt);
    st_mod->insertStmtAfter(*st_end, *st_mod);
    //!!!st_use = new SgStatement(USE_STMT);
    //!!!st_use->setSymbol(*CudaforSymb(fileHeaderSt));
    //!!!st_mod->insertStmtAfter(*st_use,*st_mod);
    if (options.isOn(C_CUDA))
        st_mod->insertStmtAfter(*new SgStatement(COMMENT_STAT), *st_mod);
    else
        st_mod->insertStmtAfter(*new SgStatement(CONTAINS_STMT), *st_mod);
    mod_gpu = st_mod;
    cur_in_mod = st_mod->lexNext();
    //cur_in_mod = options.isOn(C_CUDA) ? st_mod : st_mod->lexNext(); // contains statement or module statement
    mod_gpu_end = st_end;  // end of module 

    CudaVars();
    SymbolOfCudaIndexType();             

    KernelBaseMemorySymbols();
    KernelBloksSymbol();
    KernelWorkSymbols();
    return;
}


//---------------------------------------------------------------------------------
// create CUDA kernel 
SgStatement *CreateLoopKernel(SgSymbol *skernel, SgType *indexTypeInKernel)
{
    int nloop;
    SgStatement *st = NULL, *st_end = NULL;
    SgExpression *fe = NULL;
    SgSymbol *s_red_count_k = NULL;

    if (!skernel)
        return(NULL);
    nloop = ParLoopRank();

    // create kernel procedure for loop in Fortran-Cuda language or kernel function in C_Cuda     
    // creating Header and End Statement of Kernel
    if (options.isOn(C_CUDA))
    {
        kernel_st = Create_C_Kernel_Function(skernel);
        fe = kernel_st->expr(0);
    }
    else
        kernel_st = CreateKernelProcedure(skernel);

    kernel_st->addComment(LoopKernelComment());
    st_end = kernel_st->lexNext();
    cur_in_kernel = st = kernel_st;

    // creating variables and making structures for reductions
    CompleteStructuresForReductionInKernel();

    if (red_list)
        s_red_count_k = RedCountSymbol(kernel_st);

    if (options.isOn(NO_BL_INFO))
    {
        BeginEndBlocksSymbols(nloop);
    }

    // create  dummy argument list of kernel:
    if (options.isOn(C_CUDA))
        fe->setLhs(CreateKernelDummyList(NULL, indexTypeInKernel));
    else
        // create dummy argument list and add it to kernel header statement (Fortran-Cuda)
        kernel_st->setExpression(0, *CreateKernelDummyList(s_red_count_k, indexTypeInKernel));

    // generating block of index variables calculation 
    if (!options.isOn(NO_BL_INFO))
    {
        st = Assign_To_ibof(nloop);
        cur_in_kernel->insertStmtAfter(*st, *kernel_st);
        cur_in_kernel = st;
    }

    // generating assign statements for MAXLOC, MINLOC reduction operations and array reduction operations
    if (red_list)
        Do_Assign_For_Loc_Arrays();   //the statements are inserted after kernel_st


    // looking through the loop nest
    // generate block to calculate values of thread's loop variables
    //vl = stmt->expr(2); // do_variables list
    CreateBlockForCalculationThreadLoopVariables();

    for_kernel = 1;

    // inserting loop body to innermost IF statement of BlockForCalculationThreadLoopVariables
    {
        SgStatement *stk, *last, *block, *st;
        SaveLineNumbers(loop_body);
        block = CreateIfForRedBlack(loop_body, nloop);
        last = cur_in_kernel->lexNext();

        cur_in_kernel->insertStmtAfter(*block, *cur_in_kernel); //cur_in_kernel is innermost IF statement
        if (options.isOn(C_CUDA))
            block->addComment("// Loop body");
        else
            block->addComment("! Loop body\n");

        // correct copy of loop_body (change or extract last statement of block if it is CONTROL_END)
        stk = (block != loop_body) ? last->lexPrev()->lexPrev() : last->lexPrev();

        if (stk->variant() == CONTROL_END)
        {
            if (stk->hasLabel() || stk == loop_body)  // when body of DO_ENDDO loop is empty, stk == loop_body 
                stk->setVariant(CONT_STAT);
            else
            {
                st = stk->lexPrev();
                stk->extractStmt();
                stk = st;
            }
        }
        ReplaceExitCycleGoto(block, stk);

        last = cur_st;

        TranslateBlock(cur_in_kernel);

        if (options.isOn(C_CUDA))
        {
            swapDimentionsInprivateList();
            std::vector < std::stack < SgStatement*> > zero = std::vector < std::stack < SgStatement*> >(0);
            Translate_Fortran_To_C(cur_in_kernel, cur_in_kernel->lastNodeOfStmt(), zero, 0);
        }

        cur_st = last;
    }

    // generating reduction calculation blocks
    if (red_list)
        CreateReductionBlocks(st_end, nloop, red_list, s_red_count_k);

    // make declarations
    if (options.isOn(C_CUDA))
        MakeDeclarationsForKernel_On_C(indexTypeInKernel);
    else // Fortran-Cuda
        MakeDeclarationsForKernel(s_red_count_k, indexTypeInKernel);

    // inserting IMPLICIT NONE
    if (!options.isOn(C_CUDA)) // Fortran-Cuda
        kernel_st->insertStmtAfter(*new SgStatement(IMPL_DECL), *kernel_st);

    for_kernel = 0;

    return kernel_st;
}

SgExpression *CreateKernelDummyList(SgSymbol *s_red_count_k, std::vector<SgSymbol*> &lowI, std::vector<SgSymbol*> &highI, std::vector<SgSymbol*> &stepI)
{
    SgExpression  *arg_list, *ae;
    //SgExpression *eln = new SgExprListExp();
    //int pl_rank = ParLoopRank();

    arg_list = NULL;

    arg_list = AddListToList(CreateArrayDummyList(), CreateRedDummyList());
    //   base_ref + <array_coeffs> ...
    // + <red_var[_1]> [+red_var_2+...+red_var_M] + <red>_grid  [ + <loc_var_1>...<loc_var_N>] 

    if (s_red_count_k)                                           //[+ 'red_count']
    {
        ae = new SgExprListExp(*new SgVarRefExp(s_red_count_k));
        arg_list = AddListToList(arg_list, ae);
    }
    //[+ 'overall_blocks']
    if (s_overall_blocks)
    {
        ae = new SgExprListExp(*new SgVarRefExp(s_overall_blocks));
        arg_list = AddListToList(arg_list, ae);
    }
    if (uses_list)
        arg_list = AddListToList(arg_list, CreateUsesDummyList()); //[+ <uses> ]

    for (size_t i = 0; i < lowI.size(); ++i)
    {
        ae = new SgExprListExp(*new SgVarRefExp(lowI[i]));
        arg_list = AddListToList(arg_list, ae);
        ae = new SgExprListExp(*new SgVarRefExp(highI[i]));
        arg_list = AddListToList(arg_list, ae);
        ae = new SgExprListExp(*new SgVarRefExp(stepI[i]));
        arg_list = AddListToList(arg_list, ae);
    }
    return(arg_list);
}

void MakeDeclarationsForKernelGpuO1(SgSymbol *red_count_symb, SgType *idxTypeInKernel)
{
    SgExpression *var;
    SgStatement *st;

    // declare called functions
    DeclareCalledFunctions();

    // declare index variablex for reduction array
    for (var = kernel_index_var_list; var; var = var->rhs())
    {
        st = var->lhs()->symbol()->makeVarDeclStmt();
        kernel_st->insertStmtAfter(*st);
    }

    // declare do_variables
    DeclareDoVars();

    // declare private(local in kernel) variables
    DeclarePrivateVars();

    // declare dummy arguments:
    // declare reduction dummy arguments
    DeclareDummyArgumentsForReductions(red_count_symb, idxTypeInKernel);

    // declare array coefficients
    //TODO: add type
    DeclareArrayCoeffsInKernel(NULL);

    // declare bases for arrays 
    DeclareArrayBases();

    // declare  variables, used in loop
    DeclareUsedVars();
}

void MakeDeclarationsForKernel_On_C_GpuO1()
{
    // declare do_variables
    DeclareDoVars();

    // declare private(local in kernel) variables    
    DeclarePrivateVars();

    // declare  variables, used in loop and passed by reference:
    // <type> &<name> = *p_<name>; 
    DeclareUsedVars();
}

// TODO: replace type CudaIndexType by __indexTypeInt and __indexTypeLLong
SgStatement *CreateLoopKernel(SgSymbol *skernel, AnalyzeReturnGpuO1 &infoGpuO1, SgType *idxTypeInKernel) // create CUDA kernel with gpuO1	 
{
    int nloop;
    SgStatement *st, *st_end;
    SgExpression *fe = NULL;
    SgSymbol *s_red_count_k = NULL;

    if (!skernel)
        return(NULL);
    nloop = ParLoopRank();

    // create kernel procedure for loop in Fortran-Cuda language or kernel function in C_Cuda     
    // creating Header and End Statement of Kernel
    if (options.isOn(C_CUDA))
    {
        kernel_st = Create_C_Kernel_Function(skernel);
        fe = kernel_st->expr(0);
    }
    else
        kernel_st = CreateKernelProcedure(skernel);

    kernel_st->addComment(LoopKernelComment());
    st_end = kernel_st->lexNext();
    cur_in_kernel = st = kernel_st;

    // creating variables and making structures for reductions
    CompleteStructuresForReductionInKernel();

    if (red_list)
        s_red_count_k = RedCountSymbol(kernel_st);

    std::vector<SgSymbol* > idxs;
    SgExpression *expr = dvm_parallel_dir->expr(2);
    while (expr)
    {
        idxs.push_back(expr->lhs()->symbol());
        expr = expr->rhs();
    }
    int InternalPosition = -1;
    for (size_t i = 0; i < infoGpuO1.allArrayGroup.size(); ++i)
    {
        for (size_t k = 0; k < infoGpuO1.allArrayGroup[i].allGroups.size(); ++k)
        {
            if (infoGpuO1.allArrayGroup[i].allGroups[k].tableNewVars.size() != 0)
            {
                InternalPosition = infoGpuO1.allArrayGroup[i].allGroups[k].position;
                break;
            }
        }
    }
    // generating if block of index variables 
    SgIfStmt *beforeIf = NULL;
    SgIfStmt *inIf = NULL;
    SgIfStmt *afterIf = NULL;
    SgForStmt *doSt = NULL;

    SgStatement *st3 = new SgStatement(IF_NODE);
    SgStatement *st4 = new SgStatement(IF_NODE);
    SgStatement *st5 = new SgStatement(IF_NODE);
    SgStatement *st6 = new SgStatement(IF_NODE);

    std::vector<SgSymbol*> stepI;
    std::vector<SgSymbol*> lowI;
    std::vector<SgSymbol*> highI;
    const char *cuda_block[3] = { "z", "y", "x" };

    {
        SgIfStmt *ifSt = NULL;
        for (int i = 0, k = 0; i < nloop; ++i)
        {
            char *bufStep = new char[strlen(idxs[i]->identifier()) + 16];
            char *bufLow = new char[strlen(idxs[i]->identifier()) + 16];
            char *bufHigh = new char[strlen(idxs[i]->identifier()) + 16];

            bufStep[0] = bufLow[0] = bufHigh[0] = '\0';
            strcat(bufStep, idxs[i]->identifier());
            strcat(bufStep, "_step");
            strcat(bufLow, idxs[i]->identifier());
            strcat(bufLow, "_low");
            strcat(bufHigh, idxs[i]->identifier());
            strcat(bufHigh, "_high");

            if (options.isOn(C_CUDA))
            {
                stepI.push_back(new SgSymbol(VARIABLE_NAME, bufStep, *C_DvmType(), *kernel_st));
                lowI.push_back(new SgSymbol(VARIABLE_NAME, bufLow, *C_DvmType(), *kernel_st));
                highI.push_back(new SgSymbol(VARIABLE_NAME, bufHigh, *C_DvmType(), *kernel_st));
            }
            else
            {
                stepI.push_back(new SgSymbol(VARIABLE_NAME, bufStep));
                lowI.push_back(new SgSymbol(VARIABLE_NAME, bufLow));
                highI.push_back(new SgSymbol(VARIABLE_NAME, bufHigh));
            }

            if (i != nloop - 1 - InternalPosition)
            {
                if (k == 0)
                {
                    ifSt = new SgIfStmt(IF_NODE);
                    ifSt->setExpression(0, *new SgVarRefExp(*idxs[i]) <= *new SgVarRefExp(*highI[i]));
                    st = ifSt;
                    k++;
                }
                else
                    ifSt = new SgIfStmt(*new SgVarRefExp(*idxs[i]) <= *new SgVarRefExp(*highI[i]), *ifSt);
            }
        }
        cur_in_kernel->insertStmtAfter(*ifSt, *kernel_st);
        cur_in_kernel = st;

        SgStatement *keyAssign = AssignStatement(new SgVarRefExp(idxs[nloop - 1 - InternalPosition]), new SgVarRefExp(lowI[nloop - 1 - InternalPosition]));

        for (int i = 0, k = 0; i < nloop; ++i, ++k)
        {
            if (i != nloop - 1 - InternalPosition)
            {
                if (options.isOn(C_CUDA))
                    st = AssignStatement(new SgVarRefExp(*idxs[i]), &(*new SgVarRefExp(*stepI[i]) * ((*new SgRecordRefExp(*s_blockidx, cuda_block[k])) *
                    *new SgRecordRefExp(*s_blockdim, cuda_block[k]) + *new SgRecordRefExp(*s_threadidx, cuda_block[k])) +
                    *new SgVarRefExp(*lowI[i])));
                else
                    st = AssignStatement(new SgVarRefExp(*idxs[i]), &(*new SgVarRefExp(*stepI[i]) * ((*new SgRecordRefExp(*s_blockidx, cuda_block[k]) - *new SgValueExp(1)) *
                    *new SgRecordRefExp(*s_blockdim, cuda_block[k]) + *new SgRecordRefExp(*s_threadidx, cuda_block[k]) - *new SgValueExp(1)) +
                    *new SgVarRefExp(*lowI[i])));
                ifSt->insertStmtBefore(*st, *kernel_st);
            }
        }

        st = new SgStatement(IF_NODE);
        doSt = new SgForStmt(*idxs[nloop - 1 - InternalPosition], *new SgVarRefExp(*lowI[nloop - 1 - InternalPosition]), *new SgVarRefExp(*highI[nloop - 1 - InternalPosition]), *new SgVarRefExp(*stepI[nloop - 1 - InternalPosition]), *st);
        cur_in_kernel->insertStmtAfter(*doSt);
        cur_in_kernel = doSt;
        st->deleteStmt();

        SgStatement *st1 = new SgStatement(IF_NODE);
        SgStatement *st2 = new SgStatement(IF_NODE);
        beforeIf = new SgIfStmt(*new SgVarRefExp(*stepI[nloop - 1 - InternalPosition]) > *new SgValueExp(0), *st1, *st2);
        inIf = new SgIfStmt(*new SgVarRefExp(*stepI[nloop - 1 - InternalPosition]) > *new SgValueExp(0), *st3, *st4);
        afterIf = new SgIfStmt(*new SgVarRefExp(*stepI[nloop - 1 - InternalPosition]) > *new SgValueExp(0), *st5, *st6);

        for (size_t i = 0; i < infoGpuO1.allArrayGroup.size(); ++i)
        {
            for (size_t k = 0; k < infoGpuO1.allArrayGroup[i].allGroups.size(); ++k)
            {
                if (infoGpuO1.allArrayGroup[i].allGroups[k].position == InternalPosition)
                {
                    for (size_t m = 0; m < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr.size(); ++m)
                    {
                        for (size_t p = 0; p < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsBeforePlus.size(); ++p)
                            beforeIf->insertStmtAfter(*infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsBeforePlus[p]->copyPtr());
                        for (size_t p = 0; p < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsBeforeMinus.size(); ++p)
                            beforeIf->falseBody()->insertStmtBefore(*infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsBeforeMinus[p]->copyPtr());

                        for (size_t p = 0; p < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsInForPlus.size(); ++p)
                            inIf->insertStmtAfter(*infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsInForPlus[p]);
                        for (size_t p = 0; p < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsInForMinus.size(); ++p)
                            inIf->falseBody()->insertStmtBefore(*infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.loadsInForMinus[p]);

                        size_t sizeP = infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.swapsDown.size() - 1;
                        for (size_t p = 0; p < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.swapsDown.size(); ++p)
                            afterIf->insertStmtAfter(*infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.swapsDown[sizeP - p]);
                        for (size_t p = 0; p < infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.swapsUp.size(); ++p)
                            afterIf->falseBody()->insertStmtBefore(*infoGpuO1.allArrayGroup[i].allGroups[k].allPosGr[m].replaceInfo.swapsUp[p]);
                    }
                }
            }
        }
        doSt->insertStmtBefore(*beforeIf);
        st1->deleteStmt();
        st2->deleteStmt();
        beforeIf->insertStmtBefore(*keyAssign);
    }

    // create  dummy argument list of kernel:
    if (options.isOn(C_CUDA))
        fe->setLhs(CreateKernelDummyList(NULL, lowI, highI, stepI));
    else // create dummy argument list and add it to kernel header statement (Fortran-Cuda)		 
        kernel_st->setExpression(0, *CreateKernelDummyList(s_red_count_k, lowI, highI, stepI));

    // generating assign statements for MAXLOC, MINLOC reduction operations and array reduction operations
    if (red_list)
        Do_Assign_For_Loc_Arrays();   //the statements are inserted after kernel_st

    //CreateBlockForCalculationThreadLoopVariables();

    for_kernel = 1;

    // inserting loop body to innermost IF statement of BlockForCalculationThreadLoopVariables

    {
        SgStatement *stk, *last, *block, *st;
        SaveLineNumbers(loop_body);
        block = CreateIfForRedBlack(loop_body, nloop);
        last = cur_in_kernel->lexNext();

        cur_in_kernel->insertStmtAfter(*block, *cur_in_kernel); //cur_in_kernel is innermost IF statement
        if (options.isOn(C_CUDA))
            block->addComment("// Loop body");
        else
            block->addComment("! Loop body\n");

        // correct copy of loop_body (change or extract last statement of block if it is CONTROL_END)
        stk = (block != loop_body) ? last->lexPrev()->lexPrev() : last->lexPrev();

        if (stk->variant() == CONTROL_END)
        {
            if (stk->hasLabel())
                stk->setVariant(CONT_STAT);
            else
            {
                st = stk->lexPrev();
                stk->extractStmt();
                stk = st;
            }
        }

        ReplaceExitCycleGoto(block, stk);

        last = cur_st;

        doSt->insertStmtAfter(*inIf, *doSt);
        doSt->lastExecutable()->insertStmtAfter(*afterIf, *doSt);
        st3->deleteStmt();
        st4->deleteStmt();
        st5->deleteStmt();
        st6->deleteStmt();

        cur_in_kernel = beforeIf;
        TranslateBlock(cur_in_kernel);
        TranslateBlock(doSt);

        if (options.isOn(C_CUDA))
        {
            swapDimentionsInprivateList();
            std::vector < std::stack < SgStatement*> > zero = std::vector < std::stack < SgStatement*> >(0);
            Translate_Fortran_To_C(cur_in_kernel->controlParent(), cur_in_kernel->controlParent()->lastNodeOfStmt(), zero, 0);
        }

        cur_st = last;
    }

    // generating reduction calculation blocks
    if (red_list)
        CreateReductionBlocks(st_end, nloop, red_list, s_red_count_k);

    // make declarations
    if (options.isOn(C_CUDA))
        MakeDeclarationsForKernel_On_C_GpuO1();
    else // Fortran-Cuda
        MakeDeclarationsForKernelGpuO1(s_red_count_k, idxTypeInKernel);

    if (!options.isOn(C_CUDA))
    {
        for (size_t i = 0; i < lowI.size(); ++i)
        {
            if (i == 0)
            {
                st = lowI[i]->makeVarDeclStmt();
                st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
                kernel_st->insertStmtAfter(*st);
            }
            else
                addDeclExpList(lowI[i], st->expr(0));
        }

        for (size_t i = 0; i < highI.size(); ++i)
        {
            if (i == 0)
            {
                st = highI[i]->makeVarDeclStmt();
                st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
                kernel_st->insertStmtAfter(*st);
            }
            else
                addDeclExpList(highI[i], st->expr(0));
        }

        for (size_t i = 0; i < stepI.size(); ++i)
        {
            if (i == 0)
            {
                st = stepI[i]->makeVarDeclStmt();
                st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
                kernel_st->insertStmtAfter(*st);
            }
            else
                addDeclExpList(stepI[i], st->expr(0));
        }
    }
    // inserting IMPLICIT NONE
    if (!options.isOn(C_CUDA)) // Fortran-Cuda
        kernel_st->insertStmtAfter(*new SgStatement(IMPL_DECL), *kernel_st);

    for_kernel = 0;

    return(kernel_st);
}

void ReplaceExitCycleGoto(SgStatement *block, SgStatement *stk)
{
    SgStatement *stmt, *last, *new_st;

    SgLabel *last_lab = NULL;
    SgLabel *lb;
    stmt_list *labeled_list = NULL;
    int label_flag = 0;
    int i, pl_rank;

    pl_rank = ParLoopRank();
    last = stk->lexNext();
    for (stmt = block; stmt != last; stmt = stmt->lexNext())
    { // do list of  statement with label 
        if (stmt->hasLabel())
            labeled_list = addToStmtList(labeled_list, stmt);

    }
    for (stmt = block; stmt != last; stmt = stmt->lexNext())
    {
        if (isSgGotoStmt(stmt) && !IsInLabelList(((SgGotoStmt *)stmt)->branchLabel(), labeled_list) || isSgCycleStmt(stmt) && !isInLoop(stmt) || isSgExitStmt(stmt) && !isInLoop(stmt))
        {
            label_flag = 1;  break;
        }

        if (isSgArithIfStmt(stmt))
        {
            SgExpression *lbe = stmt->expr(1);
            for (i = 0; lbe; lbe = lbe->rhs(), i++)
            {
                lb = ((SgLabelRefExp *)(lbe->lhs()))->label();
                if (!IsInLabelList(lb, labeled_list))
                {
                    label_flag = 1;  break;
                }
            }
        }
        if (isSgAssignedGotoStmt(stmt) || isSgComputedGotoStmt(stmt))
        {
            SgExpression *lbe = stmt->expr(0);
            for (i = 0; lbe; lbe = lbe->rhs(), i++)
            {
                lb = ((SgLabelRefExp *)(lbe->lhs()))->label();
                if (!IsInLabelList(lb, labeled_list))
                {
                    label_flag = 1;  break;
                }
            }
        }

    }

    if (!label_flag) return;  //printf("enddo 3\n");
    if (stk->variant() == CONT_STAT && stk->hasLabel())
        last_lab = stk->label();
    else
    {
        last_lab = GetLabel();
        if (stk->variant() == CONT_STAT)
            stk->setLabel(*last_lab);
        else
        {
            new_st = new SgStatement(CONT_STAT);
            stk->insertStmtAfter(*new_st);
            new_st->setLabel(*last_lab);
        }
    }

    for (stmt = block; stmt != last; stmt = stmt->lexNext())
    {
        if (isSgGotoStmt(stmt) && !IsInLabelList((lb = ((SgGotoStmt *)stmt)->branchLabel()), labeled_list))
        {
            if (testLabelUse(lb, pl_rank, stmt))
                stmt->setExpression(2, *new SgLabelRefExp(*last_lab));
            continue;
        }
        if (isSgCycleStmt(stmt) && !isInLoop(stmt) || isSgExitStmt(stmt) && !isInLoop(stmt))
        {
            new_st = new SgGotoStmt(*last_lab);
            (stmt->lexPrev())->insertStmtAfter(*new_st, *stmt->controlParent());
            if (stmt->hasLabel())
                new_st->setLabel(*stmt->label());
            if (stmt->comments())
                new_st->setComments(stmt->comments());
            stmt->extractStmt();
            stmt = new_st;
            continue;
        }

        if (isSgArithIfStmt(stmt))
        {
            SgExpression *lbe = stmt->expr(1);
            for (i = 0; lbe; lbe = lbe->rhs(), i++)
            {
                lb = ((SgLabelRefExp *)(lbe->lhs()))->label();
                if (!IsInLabelList(lb, labeled_list) && testLabelUse(lb, pl_rank, stmt))
                    lbe->setLhs(new SgLabelRefExp(*last_lab));
            }
            continue;
        }
        if (isSgAssignedGotoStmt(stmt) || isSgComputedGotoStmt(stmt))
        {
            SgExpression *lbe = stmt->expr(0);
            for (i = 0; lbe; lbe = lbe->rhs(), i++)
            {
                lb = ((SgLabelRefExp *)(lbe->lhs()))->label();
                if (!IsInLabelList(lb, labeled_list) && testLabelUse(lb, pl_rank, stmt))
                    lbe->setLhs(new SgLabelRefExp(*last_lab));
            }
            continue;
        }
    }

}

int IsParDoLabel(SgLabel *lab, int pl_rank)
{
    SgStatement *stmt;
    int i;
    for (i = pl_rank, stmt = first_do_par; i; i--, stmt = stmt->lexNext())
    if (((SgForStmt *)stmt)->endOfLoop() == lab)
        return(1);
    return(0);
}

int IsInLabelList(SgLabel *lab, stmt_list *labeled_list)
{
    stmt_list *stl;
    for (stl = labeled_list; stl; stl = stl->next)
    if (stl->st->label() == lab)
        return(1);
    return(0);
}

int isInLoop(SgStatement *stmt)
{
    SgStatement *parent = stmt->controlParent();
    while (parent->variant() != FOR_NODE && parent->variant() != WHILE_NODE)
    if (parent == current_file->firstStatement())
        return(0);
    else
        parent = parent->controlParent();
    return(1);

}

int testLabelUse(SgLabel *lb, int pl_rank, SgStatement *stmt)
{
    char buf[5];
    if (!IsParDoLabel(lb, pl_rank))
    {
        sprintf(buf, "%d", (int)LABEL_STMTNO(lb->thelabel));
        Error("Label %s out of parallel loop range", buf, 38, stmt);
        return 0;
    }
    return 1;
}

SgStatement *CreateKernelProcedure(SgSymbol *skernel)
{
    SgStatement *st, *st_end;
    SgExpression *e;

    st = new SgStatement(PROC_HEDR);
    st->setSymbol(*skernel);
    e = new SgExpression(ACC_ATTRIBUTES_OP, new SgExpression(ACC_GLOBAL_OP), NULL, NULL);
    //e ->setRhs(new SgExpression(ACC_GLOBAL_OP));
    st->setExpression(2, *e);
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*skernel);

    cur_in_mod->insertStmtAfter(*st, *mod_gpu);
    st->insertStmtAfter(*st_end, *st);
    st->setVariant(PROS_HEDR);

    cur_in_mod = st_end;

    return(st);
}

SgStatement * CreateKernel_ForSequence(SgSymbol *kernel_symb, SgStatement *first_st, SgStatement *last_st, SgType *idxTypeInKernel)
{
    SgStatement  *block_copy;
    SgExpression *arg_list;
    kernel_st = (!options.isOn(C_CUDA)) ? CreateKernelProcedure(kernel_symb) : Create_C_Kernel_Function(kernel_symb);
    kernel_st->addComment(SequenceKernelComment(first_st->lineNumber()));

    // transferring sequence of statements in kernel                                             
    block_copy = CopyBlockToKernel(first_st, last_st);

    lpart_list = NULL;

    TranslateBlock(kernel_st);

    if (options.isOn(C_CUDA))
    {
        swapDimentionsInprivateList();
        std::vector < std::stack < SgStatement*> > zero = std::vector < std::stack < SgStatement*> >(0);
        Translate_Fortran_To_C(kernel_st, kernel_st->lastNodeOfStmt(), zero, 0);
    }

    // create dummy argument list and add it to kernel header statement
    arg_list = CreateKernelDummyList_ForSequence(idxTypeInKernel);
    if (arg_list)
    {
        if (options.isOn(C_CUDA))
            kernel_st->expr(0)->setLhs(arg_list);
        else
            kernel_st->setExpression(0, *arg_list);
    }

    // make declarations
    MakeDeclarationsInKernel_ForSequence(idxTypeInKernel);


    if (!options.isOn(C_CUDA))  // Fortran-Cuda                                                       
        // inserting IMPLICIT NONE
        kernel_st->insertStmtAfter(*new SgStatement(IMPL_DECL), *kernel_st);

    return(kernel_st);
}


SgExpression *IsRedBlack(int nloop)
{
    SgExpression *erb;
    SgStatement *st;
    int ndo;
    // looking through the loop nest for redblack scheme
    erb = NULL;
    for (st = first_do_par, ndo = 0; ndo < nloop; st = ((SgForStmt *)st)->body(), ndo++)
    {
        if (((SgForStmt *)st)->start()->variant() == ADD_OP) //redblack scheme
        {
            return(((SgForStmt *)st)->start()->rhs()->lhs()->lhs()->rhs());
        }

    }

    return(NULL);

}

void  CreateBlockForCalculationThreadLoopVariables()
{
    int nloop, i, i1;
    SgStatement *if_st = NULL, *dost = NULL, *ass = NULL, *stmt = NULL;
    nloop = ParLoopRank();


    if (!options.isOn(NO_BL_INFO))
    {
        if (options.isOn(C_CUDA))
            cur_in_kernel->addComment("// Calculate each thread's loop variables' values");
        else
            cur_in_kernel->addComment("! Calculate each thread's loop variables' values\n");

        for (i = 0; i<nloop - 3; i++)
        {
            dost = DoStmt(first_do_par, i + 1);
            ass = Assign_To_IndVar(dost, i, nloop, s_blocks_k);
            cur_in_kernel->insertStmtAfter(*ass, *kernel_st);
            cur_in_kernel = ass;
        }
        i1 = i;
        if_st = new SgStatement(CONT_STAT);
        i = nloop;
        while (i>i1)
        {
            dost = DoStmt(first_do_par, i);   //sind = Do_Var(i,vl);
            if_st = new SgIfStmt(*KernelConditionWithDoStep(dost, s_blocks_k, i - 1), *if_st); //new SgIfStmt( *KernelCondition(dost->symbol(),s_blocks_k,i-1), *if_st);
            i--;
        }
        cur_in_kernel->insertStmtAfter(*if_st, *kernel_st);
        cur_in_kernel = if_st;

        i = i1;
        //dost =  first_do_par; 
        while (i < nloop)
        {
            ass = Assign_To_IndVar(dost, i, nloop, s_blocks_k);
            if_st->insertStmtBefore(*ass, *if_st->controlParent());
            if_st = if_st->lexNext();
            dost = dost->lexNext();
            i++;
        }

        //dost = dost->controlParent();                       
        cur_in_kernel = ass->lexNext();  //innermost IF statement
        cur_in_kernel->lexNext()->extractStmt(); //extracting CONTINUE statement
        return;
    }

    //without_blocks_info
    cur_in_kernel = stmt = kernel_st->lastNodeOfStmt()->lexPrev();

    if_st = new SgStatement(CONT_STAT);
    i = nloop;
    while (i)
    {
        dost = DoStmt(first_do_par, i);
        if_st = new SgIfStmt(*KernelCondition2(dost, i), *if_st);
        i--;
    }
    cur_in_kernel->insertStmtAfter(*if_st, *kernel_st);
    cur_in_kernel = if_st;

    dost = first_do_par;
    i = 1;
    while (i <= nloop)
    {
        ass = Assign_To_rest_blocks(i - 1);
        if_st->insertStmtBefore(*ass, *if_st->controlParent());
        ass = Assign_To_cur_blocks(i - 1, nloop);
        if_st->insertStmtBefore(*ass, *if_st->controlParent());
        ass = Assign_To_IndVar2(dost, i, nloop);
        if_st->insertStmtBefore(*ass, *if_st->controlParent());
        if_st = if_st->lexNext();
        dost = dost->lexNext();
        i++;
    }

    if (options.isOn(C_CUDA))
        stmt->lexNext()->addComment("// Calculate each thread's loop variables' values");
    else
        stmt->lexNext()->addComment("! Calculate each thread's loop variables' values\n");

    cur_in_kernel = ass->lexNext();  //innermost IF statement
    cur_in_kernel->lexNext()->extractStmt(); //extracting CONTINUE statement

    return;
}

SgStatement *CreateIfForRedBlack(SgStatement *loop_body, int nloop)
{
    SgExpression *erb;
    SgStatement *st;
    int ndo;
    // looking through the loop nest for redblack scheme
    erb = NULL;
    for (st = first_do_par, ndo = 0; ndo < nloop; st = ((SgForStmt *)st)->body())
    {          //!printf("---line number: %d,  %d\n",st->lineNumber(),((SgForStmt *)st)->body()->lineNumber());
        if (((SgForStmt *)st)->start()->variant() == ADD_OP) //redblack scheme
        {
            erb = ((SgForStmt *)st)->start()->rhs();  // MOD function call (after replacing for dvm realisation)
            erb = &(erb->lhs()->lhs()->copy());   //first argument of MOD function call 
            erb->setLhs(new SgVarRefExp(st->symbol()));
        }
        ndo++;
    }
    //!!!printf("line number of st: %d,  %d\n",st->lineNumber(), st);

    if (erb)
    {
        st = new SgIfStmt(*ConditionForRedBlack(erb), *loop_body);
        return(st);
    }
    else
        return(loop_body);

}

SgExpression *CreateKernelDummyList(SgSymbol *s_red_count_k, SgType *idxTypeInKernel)
{
    SgExpression *arg_list, *ae;
    SgExpression *eln = new SgExprListExp();
    int pl_rank = ParLoopRank();
    int i;
    arg_list = NULL;

    arg_list = AddListToList(CreateArrayDummyList(idxTypeInKernel), CreateRedDummyList());
    //   base_ref + <array_coeffs> ...
    // + <red_var[_1]> [+red_var_2+...+red_var_M] + <red>_grid  [ + <loc_var_1>...<loc_var_N>] 

    // + 'blocks' [ or begin_1, end_1,...,begin_<N>,end_<N>,blocks_1,...,blocks_<N-1>,add_blocks ]
    if (!options.isOn(NO_BL_INFO))
    {
        SgArrayType *tmpType = new SgArrayType(*idxTypeInKernel);
        SgSymbol *copy_s_blocks_k = new SgSymbol(s_blocks_k->variant(), s_blocks_k->identifier(), tmpType, s_blocks_k->scope());

        ae = options.isOn(C_CUDA) ? new SgExprListExp(*new SgArrayRefExp(*copy_s_blocks_k, *eln)) : new SgExprListExp(*new SgArrayRefExp(*copy_s_blocks_k));   // + 'blocks'
        //ae = options.isOn(C_CUDA) ? new SgExprListExp(*new SgPointerDerefExp(*new SgVarRefExp(copy_s_blocks_k))) : new SgExprListExp(*new SgVarRefExp(copy_s_blocks_k));
        arg_list = AddListToList(arg_list, ae);

    }
    else  //without blocks_info
    {
        SgSymbol *copy_s_begin, *copy_s_end, *copy_s_blocks, *copy_s_add_blocks;
        for (i = 0; i < pl_rank; i++)
        {
            copy_s_begin = new SgSymbol(s_begin[i]->variant(), s_begin[i]->identifier(), idxTypeInKernel, s_begin[i]->scope());
            ae = new SgVarRefExp(*copy_s_begin);
            ae = new SgExprListExp(*ae);
            if (i == 0)
                indexing_info_list = ae;
            arg_list = AddListToList(arg_list, ae);

            copy_s_end = new SgSymbol(s_end[i]->variant(), s_end[i]->identifier(), idxTypeInKernel, s_end[i]->scope());
            ae = new SgVarRefExp(*copy_s_end);
            ae = new SgExprListExp(*ae);
            arg_list = AddListToList(arg_list, ae);
        }

        for (i = 0; i < pl_rank - 1; i++)
        {
            copy_s_blocks = new SgSymbol(s_blocksS_k[i]->variant(), s_blocksS_k[i]->identifier(), idxTypeInKernel, s_blocksS_k[i]->scope());
            ae = new SgVarRefExp(*copy_s_blocks);
            ae = new SgExprListExp(*ae);
            arg_list = AddListToList(arg_list, ae);
        }

        copy_s_add_blocks = new SgSymbol(s_add_blocks->variant(), s_add_blocks->identifier(), idxTypeInKernel, s_add_blocks->scope());
        ae = new SgVarRefExp(*copy_s_add_blocks);
        ae = new SgExprListExp(*ae);
        arg_list = AddListToList(arg_list, ae);

        indexing_info_list = &(indexing_info_list->copy());
    }
    if (s_red_count_k)                                           //[+ 'red_count']
    {
        ae = new SgExprListExp(*new SgVarRefExp(s_red_count_k));
        arg_list = AddListToList(arg_list, ae);
    }
    //[+ 'overall_blocks']
    if (s_overall_blocks)
    {
        SgSymbol *copy_overall = new SgSymbol(s_overall_blocks->variant(), s_overall_blocks->identifier(), idxTypeInKernel, s_overall_blocks->scope());
        ae = new SgExprListExp(*new SgVarRefExp(copy_overall));
        arg_list = AddListToList(arg_list, ae);
    }
    if (uses_list)
        arg_list = AddListToList(arg_list, CreateUsesDummyList()); //[+ <uses> ]

    return arg_list;
}


SgExpression *CreateKernelDummyList_ForSequence(SgType *idxTypeInKernel)
{
    SgExpression *arg_list;

    arg_list = NULL;

    arg_list = AddListToList(CreateArrayDummyList(idxTypeInKernel), CreateLocalPartList(idxTypeInKernel));
    //   base_ref + <array_coeffs> ...
    // + <local_part>...  

    if (uses_list)
        arg_list = AddListToList(arg_list, CreateUsesDummyList()); // [ <uses> ]
    return(arg_list);

}

SgSymbol *KernelDummyArray(SgSymbol *s)
{
    SgArrayType *typearray;
    SgType *type;
    //SgExpression  *MD =  new SgExpression(DDOT,new SgValueExp(0),new SgValueExp(1),NULL); 

    type = isSgArrayType(s->type()) ? s->type()->baseType() : s->type();

    //if(options.isOn(C_CUDA))
    //{ type = C_PointerType(C_Type(type));

    //}
    //else
    if (options.isOn(C_CUDA))
        type = C_Type(type);
    typearray = new SgArrayType(*type);
    typearray->addDimension(NULL);
    type = typearray;

    return(new SgSymbol(VARIABLE_NAME, s->identifier(), *type, *kernel_st));

}

SgSymbol *KernelDummyVar(SgSymbol *s)
{
    SgType *type;
    type = options.isOn(C_CUDA) ? C_Type(s->type()) : s->type();
    return(new SgSymbol(VARIABLE_NAME, s->identifier(), *type, *kernel_st));
}


SgSymbol *KernelDummyPointerVar(SgSymbol *s)
{
    char *name;
    SgSymbol *sp;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 2 + 1));
    sprintf(name, "p_%s", s->identifier());
    sp = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(name), *C_PointerType(C_Type(s->type())), *kernel_st);

    // adding the attribute  DUMMY_ARG to symbol of user program 
    if (!DUMMY_ARG(s))
    {
        SgSymbol **dummy = new (SgSymbol *);
        *dummy = sp;
        s->addAttribute(DUMMY_ARGUMENT, (void*)dummy, sizeof(SgSymbol *));
    }
    return(sp);

}

SgExpression * dvm_coef(SgSymbol *ar, int i)
{ //coeffs *c;
    //c = AR_COEFFICIENTS(ar);
    if (options.isOn(C_CUDA))
    {
        SgSymbol *s_dummy_coef = new SgSymbol(VARIABLE_NAME, AR_COEFFICIENTS(ar)->sc[i]->identifier(), *CudaIndexType_k, *kernel_st);
        return(new SgVarRefExp(*s_dummy_coef));
    }

    return(new SgVarRefExp(*(AR_COEFFICIENTS(ar)->sc[i])));

}

SgSymbol *KernelDummyLocalPart(SgSymbol *s)
{
    SgArrayType *typearray;
    SgType *type;

    // for C_Cuda
    typearray = new SgArrayType(*CudaIndexType_k);
    typearray->addDimension(NULL);
    type = typearray;

    return(new SgSymbol(VARIABLE_NAME, s->identifier(), *type, *kernel_st));

}


SgExpression *CreateArrayDummyList()
{
    symb_list *sl;
    SgExpression *ae, *coef_list, *edim;
    int n, d;
    SgExpression *arg_list = NULL;

    edim = new SgExprListExp();     // [] dimension

    for (sl = acc_array_list; sl; sl = sl->next)                 // + base_ref + <array_coeffs>
    {
        SgSymbol *s_dummy;
        s_dummy = KernelDummyArray(sl->symb);
        if (options.isOn(C_CUDA))
            ae = new SgArrayRefExp(*s_dummy, *edim); // new SgPointerDerefExp(* new SgVarRefExp(s_dummy));
        else
            ae = new SgArrayRefExp(*s_dummy);
        ae->setType(s_dummy->type());   //for C_Cuda
        ae = new SgExprListExp(*ae);
        //   ae = new SgPointerDerefExp(*ae);      //   ae->setLhs(*edim);
        arg_list = AddListToList(arg_list, ae);
        coef_list = NULL;
        if (Rank(sl->symb) == 0)      //remote_access buffer may be of rank 0   
            continue;
        d = options.isOn(AUTO_TFM) ? 0 : 1;    //inparloop ? 0 : 1;
        for (n = Rank(sl->symb) - d; n > 0; n--)
        {
            ae = new SgExprListExp(*dvm_coef(sl->symb, n + 1));
            coef_list = AddListToList(coef_list, ae);
        }

        arg_list = AddListToList(arg_list, coef_list);
    }
    return(arg_list);

}

SgExpression *CreateUsesDummyList()
{
    SgSymbol *s_dummy, *s;
    SgExpression *el, *ae;
    SgExpression *arg_list = NULL;

    for (el = uses_list; el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (options.isOn(C_CUDA) && !isByValue(s))
        {
            s_dummy = KernelDummyPointerVar(s);
            ae = new SgPointerDerefExp(*new SgVarRefExp(*s_dummy));
        }
        else
        {
            s_dummy = KernelDummyVar(s);
            ae = new SgVarRefExp(*s_dummy);
        }
        ae = new SgExprListExp(*ae);
        arg_list = AddListToList(arg_list, ae);
    }
    return(arg_list);
}


SgExpression *CreateRedDummyList()
{
    reduction_operation_list *rsl;
    SgExpression *ae, *arg_list, *loc_list;
    arg_list = NULL;
    for (rsl = red_struct_list; rsl; rsl = rsl->next)         // + <red_var[_1]> [+red_var_2+...+red_var_M]  + <red>_grid  [ + <loc_var_1>...<loc_var_N>]  [ + <loc>_grid> ]
    {
        if (rsl->locvar)
        {
            //ae = C_Cuda ? new SgExprListExp(*new SgPointerDerefExp(*new SgVarRefExp(rsl->loc_grid))) : new SgExprListExp(*new SgVarRefExp(rsl->loc_grid));
            if (options.isOn(C_CUDA))
            {
                ae = new SgArrayRefExp(*rsl->loc_grid, *new SgExprListExp());
                ae->setType(rsl->loc_grid->type());
            }
            else
                ae = new SgVarRefExp(rsl->loc_grid);
            ae = new SgExprListExp(*ae);
            loc_list = AddListToList(&(rsl->formal_arg->copy()), ae);
        }
        else
            loc_list = NULL;
        if (rsl->redvar_size > 0)   // reduction array of known size (constant bounds)
            arg_list = AddListToList(arg_list, &(rsl->value_arg->copy()));
        else if (rsl->redvar_size == 0)
        {
            ae = new SgExprListExp(*new SgVarRefExp(KernelDummyVar(rsl->redvar)));
            arg_list = AddListToList(arg_list, ae);
        }
        else                       // reduction array of unknown size
        {
            arg_list = AddListToList(arg_list, &(rsl->dimSize_arg->copy()));
            arg_list = AddListToList(arg_list, &(rsl->lowBound_arg->copy()));
        }
        if (options.isOn(C_CUDA))
        {
            ae = new SgArrayRefExp(*rsl->red_grid, *new SgExprListExp());
            ae->setType(rsl->red_grid->type());
        }
        else
            ae = new SgVarRefExp(rsl->red_grid);
        ae = new SgExprListExp(*ae);
        arg_list = AddListToList(arg_list, ae);
        if (rsl->redvar_size < 0)
        {
            if (options.isOn(C_CUDA))
            {
                ae = new SgArrayRefExp(*rsl->red_init, *new SgExprListExp());
                //XXX ues correct type from red_grid, changed reduction scheme to atomic, Kolganov 06.02.2020
                ae->setType(rsl->red_grid->type());
                ae = new SgExprListExp(*ae);
            }
            else
                ae = new SgExprListExp(*new SgVarRefExp(rsl->red_init));
            arg_list = AddListToList(arg_list, ae);
        }
        arg_list = AddListToList(arg_list, loc_list);
    }
    return(arg_list);
}

SgExpression *CreateLocalPartList()
{
    local_part_list *pl;
    SgExpression *ae;
    SgExpression *arg_list = NULL;
    for (pl = lpart_list; pl; pl = pl->next)                 // + <local_part>
    {
        if (options.isOn(C_CUDA))
            ae = new SgExprListExp(*new SgArrayRefExp(*KernelDummyLocalPart(pl->local_part), *new SgExprListExp())); //<local_part>[]
        else
            ae = new SgExprListExp(*new SgArrayRefExp(*pl->local_part));
        arg_list = AddListToList(arg_list, ae);
    }
    return(arg_list);

}


SgExpression *CoefficientList()
{
    symb_list *sl;
    SgExpression *ae;
    int n, d;
    SgExpression *coef_list = NULL;
    for (sl = acc_array_list; sl; sl = sl->next)
    {
        if (Rank(sl->symb) == 0)      //remote_access buffer may be of rank 0   
            continue;
        d = options.isOn(AUTO_TFM) ? 0 : 1; //inparloop ? 0 : 1;
        for (n = Rank(sl->symb) - d; n > 0; n--)
        {
            ae = new SgExprListExp(*dvm_coef(sl->symb, n + 1));
            coef_list = AddListToList(coef_list, ae);
        }

    }
    return(coef_list);

}

SgExpression *ArrayRefList()
{
    symb_list *sl;
    SgExpression *ae;
    SgExpression *ar_list = NULL;

    for (sl = acc_array_list; sl; sl = sl->next)
    {
        ae = new SgExprListExp(*new SgArrayRefExp(*sl->symb));
        ar_list = AddListToList(ar_list, ae);
    }
    return(ar_list);
}

void MakeDeclarationsForKernel(SgSymbol *red_count_symb, SgType *idxTypeInKernel)
{
    SgExpression *var, *eatr, *edev;
    SgStatement *st;

    // declare called functions
    DeclareCalledFunctions();

    // declare index variablex for reduction array
    for (var = kernel_index_var_list; var; var = var->rhs())
    {
        st = var->lhs()->symbol()->makeVarDeclStmt();
        kernel_st->insertStmtAfter(*st);
    }

    // declare variable 'ibof' or cur_blocks,rest_blocks (without blocks_info)
    if (!options.isOn(NO_BL_INFO))
        st = s_ibof->makeVarDeclStmt();

    else // without_blocks_info
    {
        SgSymbol *copy_s_rest_blocks = new SgSymbol(s_rest_blocks->variant(), s_rest_blocks->identifier(), idxTypeInKernel, s_rest_blocks->scope());
        st = copy_s_rest_blocks->makeVarDeclStmt();
        st->expr(0)->setRhs(new SgExprListExp(*new SgVarRefExp(s_cur_blocks)));
    }
    kernel_st->insertStmtAfter(*st);

    // declare do_variables
    DeclareDoVars();

    // declare private(local in kernel) variables    
    DeclarePrivateVars();

    // declare dummy arguments:
    eatr = new SgExprListExp(*new SgExpression(ACC_VALUE_OP));
    edev = new SgExprListExp(*new SgExpression(ACC_DEVICE_OP));

    // declare reduction dummy arguments
    DeclareDummyArgumentsForReductions(red_count_symb, idxTypeInKernel);

    if (!options.isOn(NO_BL_INFO))
    {
        // declare blocks variable (see CudaIndexType type in util.h)
        SgSymbol *copy_s_blocks_k = ArraySymbol(s_blocks_k->identifier(), idxTypeInKernel, new SgExpression(DDOT, new SgValueExp(0), new SgKeywordValExp("*"), NULL), s_blocks_k->scope());
        st = copy_s_blocks_k->makeVarDeclStmt();      // of CudaIndexType
        st->setExpression(2, *edev);
        kernel_st->insertStmtAfter(*st);
        st->addComment("! Loop bounds array\n");
    }
    else // without_blocks_info
    {
        // declare begin_k,end_k,blocks_k variables (see CudaIndexType type in util.h)
        SgSymbol *copy_s_blocks_k = new SgSymbol(s_blocks_k->variant(), s_blocks_k->identifier(), idxTypeInKernel, s_blocks_k->scope());
        st = copy_s_blocks_k->makeVarDeclStmt();      // of CudaIndexType
        st->setExpression(2, *eatr);
        st->setExpression(0, *indexing_info_list);
        kernel_st->insertStmtAfter(*st);
        st->addComment("! Indexing info\n");
    }

    // declare array coefficients
    DeclareArrayCoeffsInKernel(idxTypeInKernel);

    // declare bases for arrays 
    DeclareArrayBases();

    // declare  variables, used in loop
    DeclareUsedVars();
}

void MakeDeclarationsForKernel_On_C(SgType *idxTypeInKernel)
{
    SgStatement *st;

    // declare variable 'ibof' or cur_blocks,rest_blocks (without blocks_info)
    if (!options.isOn(NO_BL_INFO))
        st = Declaration_Statement(s_ibof);
    else // without_blocks_info
    {
        SgSymbol *copy_symb;

        copy_symb = new SgSymbol(s_rest_blocks->variant(), s_rest_blocks->identifier(), idxTypeInKernel, s_rest_blocks->scope());
        st = Declaration_Statement(copy_symb);

        copy_symb = new SgSymbol(s_cur_blocks->variant(), s_cur_blocks->identifier(), idxTypeInKernel, s_cur_blocks->scope());
        addDeclExpList(copy_symb, st->expr(0));
    }
    kernel_st->insertStmtAfter(*st);

    // declare do_variables
    DeclareDoVars(idxTypeInKernel); 

    // declare private(local in kernel) variables 
    DeclarePrivateVars();

    // declare  variables, used in loop and passed by reference:
    // <type> &<name> = *p_<name>;  
    DeclareUsedVars();
}

void MakeDeclarationsInKernel_ForSequence(SgType *idxTypeInKernel)
{
    if (options.isOn(C_CUDA))
    {
        DeclareUsedVars();
        DeclareInternalPrivateVars();        
    }
    else
    {
        // in Fortran-Cuda language
        // declare called functions
        DeclareCalledFunctions();

        // declaring dummy arguments
        // declare array coefficients
        DeclareArrayCoeffsInKernel(idxTypeInKernel);

        // declare bases for arrays 
        DeclareArrayBases();

        // declare local part variables
        DeclareLocalPartVars(idxTypeInKernel);

        // declare  variables, used in sequence
        DeclareUsedVars();
    }
}

void DeclareCalledFunctions()
{
    SgStatement *st = NULL;
    symb_list *sl;
    // declare called functions in Fortran_Cuda kernel
    for (sl = acc_call_list; sl; sl = sl->next)
    if (sl->symb->variant() == FUNCTION_NAME && !IS_BY_USE(sl->symb))
    {
        st = sl->symb->makeVarDeclStmt();
        kernel_st->insertStmtAfter(*st, *kernel_st);
    }
    if (st)
        st->addComment("! Called functions\n");

}


// declare DO cariables of parallel loop nest in kernel 
void DeclareDoVars()
{
    SgExpression *el;
    SgStatement *st;
    SgSymbol *s;
    // declare do_variables of parallel loop nest       
    for (el=dvm_parallel_dir->expr(2); el; el=el->rhs())
    {
        s = el->lhs()->symbol();
        if (options.isOn(C_CUDA))
            s =  new SgVariableSymb(s->identifier(), *C_Type(s->type()), *kernel_st); 
        st = Declaration_Statement(s);
        kernel_st->insertStmtAfter(*st);
    }
    if (options.isOn(C_CUDA))
        st->addComment("// Local needs");
    else
        st->addComment("! Local needs\n");    

}

void DeclareLocalPartVars(SgType *idxTypeInKernel)
{
    SgExpression *edev = NULL;
    local_part_list *pl = NULL;
    SgStatement *st = NULL;

    edev = new SgExprListExp(*new SgExpression(ACC_DEVICE_OP));

    // declare local-part variables 
    for (pl = lpart_list; pl; pl = pl->next)
    {
        st = pl->local_part->makeVarDeclStmt();
        st->expr(1)->setType(idxTypeInKernel);
        st->setExpression(2, *edev);
        kernel_st->insertStmtAfter(*st);
    }
    if (lpart_list)
        st->addComment("! Local parts of arrays\n");
}

void DeclareLocalPartVars()
{
    SgExpression *edev = NULL;
    local_part_list *pl = NULL;
    SgStatement *st = NULL;

    edev = new SgExprListExp(*new SgExpression(ACC_DEVICE_OP));

    // declare local-part variables 
    for (pl = lpart_list; pl; pl = pl->next)
    {
        st = pl->local_part->makeVarDeclStmt();
        st->setExpression(2, *edev);
        kernel_st->insertStmtAfter(*st);        
    }
    if (lpart_list)
        st->addComment("! Local parts of arrays\n");
}

void DeclareArrayCoeffsInKernel(SgType *idxTypeInKernel)
{ // declare array coefficients
    SgExpression *el = NULL, *eatr = NULL;
    SgStatement *st = NULL;

    if (acc_array_list && (el = CoefficientList()))
    {
        eatr = new SgExprListExp(*new SgExpression(ACC_VALUE_OP));
        st = idxTypeInKernel->symbol()->makeVarDeclStmt();   // of CudaIndexType       
        st->setExpression(2, *eatr);
        kernel_st->insertStmtAfter(*st);
        st->addComment("! Array coefficients\n");
        st->setExpression(0, *el);
    }
}

void DeclareArrayBases()
{
    // declare bases for arrays 
    if (acc_array_list)
    {
        SgStatement *st = NULL;
        SgExpression *array_list = NULL, *alist = NULL, *edim = NULL, *edev = NULL;
        SgSymbol *ar = NULL;
        //SgSymbol *baseMem = NULL;

        // make attribute DIMENSION(0:*) 
        edim = new SgExpression(DIMENSION_OP);
        edim->setLhs(new SgExpression(DDOT, new SgValueExp(0), new SgKeywordValExp("*"), NULL, NULL));
        edim = new SgExprListExp(*edim);
        // make attribute DEVICE
        edev = new SgExprListExp(*new SgExpression(ACC_DEVICE_OP));

        array_list = ArrayRefList();
        while (array_list)
        {
            ar = array_list->lhs()->symbol();
            //baseMem = baseMemory(ar->type()->baseType());
            st = ar->makeVarDeclStmt();
            edim->setRhs(edev);
            st->setExpression(2, *edim);
            kernel_st->insertStmtAfter(*st);
            alist = array_list;
            st->setExpression(0, *alist);
            //while (alist->rhs() && baseMemory(alist->rhs()->lhs()->symbol()->type()->baseType()) == baseMem)
            //    alist = alist->rhs();
            array_list = array_list->rhs();
            alist->setRhs(NULL);
        }
        st->addComment("! Bases for arrays\n");
    }
}

void DeclareInternalPrivateVars()
{
    SgStatement *st = NULL;
    for (unsigned i = 0; i < newVars.size(); ++i)
    {
        SgVarRefExp *e = new SgVarRefExp(*newVars[i]);
        if (!(isParDoIndexVar(e->symbol())))
        {
            st = Declaration_Statement(SymbolInKernel(e->symbol()));
            kernel_st->insertStmtAfter(*st);
        }

    }

    if (st)
    {
        if (options.isOn(C_CUDA))
            st->addComment("// Internal private variables");
        else
            st->addComment("! Internal private variables\n");
    }
}

void DeclarePrivateVars()
{
    SgStatement *st = NULL;
    SgExpression *var = NULL;
    // declare private(local in kernel) variables    
    for (var = private_list; var; var = var->rhs())
    {
        if (isParDoIndexVar(var->lhs()->symbol()))  continue; // declared as index variable of parallel loop
        //if (HEADER(var->lhs()->symbol()))  continue; // dvm-array declared as dummy argument
        st = Declaration_Statement(SymbolInKernel(var->lhs()->symbol()));
        kernel_st->insertStmtAfter(*st);
    }
    if (!st)
        return;

    if (options.isOn(C_CUDA))
        st->addComment("// Private variables");
    else
        st->addComment("! Private variables\n");
}

void DeclareUsedVars()
{
    SgSymbol *s = NULL, *sn = NULL;
    SgExpression *var = NULL, *eatr = NULL, *edev = NULL;
    SgStatement *st = NULL;

    if (options.isOn(C_CUDA))

    {
        for (var = uses_list; var; var = var->rhs())
        {
            s = var->lhs()->symbol();
            if (!isByValue(s)) // passing argument by reference
                // <type> &<name> = *p_<name>;
            {
                sn = new SgSymbol(VARIABLE_NAME, s->identifier(), C_ReferenceType(C_Type(s->type())), kernel_st);
                st = makeSymbolDeclarationWithInit(sn, &SgDerefOp(*new SgVarRefExp(**DUMMY_ARG(s))));
                kernel_st->insertStmtAfter(*st);
            }
        }
        if (st)
            st->addComment("// Used values");
        return;
    }

    // Fortran-Cuda

    eatr = new SgExprListExp(*new SgExpression(ACC_VALUE_OP));
    edev = new SgExprListExp(*new SgExpression(ACC_DEVICE_OP));
    for (var = uses_list; var; var = var->rhs())
    {
        s = var->lhs()->symbol();
        if (!isByValue(s)) // passing argument by reference
        {
            st = s->makeVarDeclStmt();
            st->setExpression(2, *edev);
            kernel_st->insertStmtAfter(*st);
            continue;
        }
        if (s->variant() == CONST_NAME)
            s = new SgSymbol(VARIABLE_NAME, s->identifier(), s->type(), kernel_st);
        st = s->makeVarDeclStmt();
        st->setExpression(2, *eatr);
        kernel_st->insertStmtAfter(*st);
    }

    if (st)
        st->addComment("! Used values\n");
}

void DeclareDummyArgumentsForReductions(SgSymbol *red_count_symb, SgType *idxTypeInKernel)

// declare reduction dummy arguments

{
    reduction_operation_list *rsl = NULL;
    SgExpression *eatr = NULL, *edev = NULL, *el = NULL;
    SgStatement *st = NULL;

    eatr = new SgExprListExp(*new SgExpression(ACC_VALUE_OP));
    edev = new SgExprListExp(*new SgExpression(ACC_DEVICE_OP));

    for (rsl = red_struct_list; rsl; rsl = rsl->next)
    {
        for (el = rsl->formal_arg; el; el = el->rhs())    // location array values for MAXLOC/MINLOC
        {
            st = el->lhs()->symbol()->makeVarDeclStmt();
            st->setExpression(2, *eatr);
            kernel_st->insertStmtAfter(*st);
        }

        for (el = rsl->value_arg; el; el = el->rhs())      // reduction variable is array of known size
        {
            st = el->lhs()->symbol()->makeVarDeclStmt();
            st->setExpression(2, *eatr);
            kernel_st->insertStmtAfter(*st);
        }
        if (rsl->redvar_size == 0)                      // reduction variable is scalar
        {
            st = rsl->redvar->makeVarDeclStmt();
            st->setExpression(2, *eatr);
            kernel_st->insertStmtAfter(*st);
        }

        if (rsl->redvar_size < 0)                      // reduction variable is  array of unknown size
        {
            st = rsl->red_init->makeVarDeclStmt();
            st->setExpression(2, *edev);
            kernel_st->insertStmtAfter(*st);
        }

    }
    if (red_struct_list)
        st->addComment("! Initial reduction values\n");

    st = NULL;
    for (rsl = red_struct_list; rsl; rsl = rsl->next)
    {
        for (el = rsl->dimSize_arg; el; el = el->rhs())    // reduction variable is array of unknown size
        {
            st = el->lhs()->symbol()->makeVarDeclStmt();
            st->setExpression(2, *eatr);
            kernel_st->insertStmtAfter(*st);
        }
        for (el = rsl->lowBound_arg; el; el = el->rhs())    // reduction variable is array of unknown size
        {
            st = el->lhs()->symbol()->makeVarDeclStmt();
            st->setExpression(2, *eatr);
            kernel_st->insertStmtAfter(*st);
        }
    }
    if (st)
        st->addComment("! Bounds of reduction arrays \n");


    // declare red_count variable
    if (red_count_symb)
    {
        st = red_count_symb->makeVarDeclStmt();
        st->setExpression(2, *eatr);
        kernel_st->insertStmtAfter(*st);
        st->addComment("! Number of threads to perform reduction\n");
    }

    // declare overall_blocks variable
    if (s_overall_blocks)
    {
        SgSymbol *copy_overall = new SgSymbol(s_overall_blocks->variant(), s_overall_blocks->identifier(), idxTypeInKernel, s_overall_blocks->scope());
        st = copy_overall->makeVarDeclStmt();
        st->setExpression(2, *eatr);
        kernel_st->insertStmtAfter(*st);
        st->addComment("! Number of blocks to perform reduction \n");
    }

    // declare arrays to collect reduction values
    for (rsl = red_struct_list; rsl; rsl = rsl->next)
    {
        if (rsl->loc_grid)
        {
            st = rsl->loc_grid->makeVarDeclStmt();
            st->setExpression(2, *edev);
            kernel_st->insertStmtAfter(*st);
        }

        st = rsl->red_grid->makeVarDeclStmt();
        st->setExpression(2, *edev);
        kernel_st->insertStmtAfter(*st);
    }
    if (red_struct_list)
        st->addComment("! Array to collect reduction values\n");
}


SgStatement *AssignStatement(SgExpression *le, SgExpression *re)
{
    SgStatement *ass = NULL;
    if (options.isOn(C_CUDA))   // in C Language
        ass = new SgCExpStmt(SgAssignOp(*le, *re));
    else         // in Fortan Language
        ass = new SgAssignStmt(*le, *re);
    return(ass);
}

SgStatement *FunctionCallStatement(SgSymbol *sf)
{
    SgStatement *stmt = NULL;
    if (options.isOn(C_CUDA))   // in C Language
        stmt = new SgCExpStmt(*new SgFunctionCallExp(*sf));
    else         // in Fortan Language
        stmt = new SgCallStmt(*sf);
    return(stmt);
}

SgStatement *Declaration_Statement(SgSymbol *s)
{
    SgStatement *stmt = NULL;
    if (options.isOn(C_CUDA))   // in C Language
        stmt = makeSymbolDeclaration(s);
    else         // in Fortan Language
        stmt = s->makeVarDeclStmt();
    return(stmt);
}

SgStatement *Assign_To_ibof(int rank)
{
    SgStatement *ass = NULL;
    // ibof = (blockIdx%x - 1) * <rank*2>  for Fortran-Cuda
    // or
    // ibof = blockIdx%x  * <rank*2>       for C_Cuda
    ass = AssignStatement(new SgVarRefExp(s_ibof), ExpressionForIbof(rank));
    return(ass);
}

SgExpression *ExpressionForIbof(int rank)
{
    if (options.isOn(C_CUDA))
        // blockIdx%x * <rank*2>
        return(&
        ((*new SgRecordRefExp(*s_blockidx, "x")) * (*new SgValueExp(rank * 2))));
    else
        // (blockIdx%x - 1) * <rank*2>
        return(&
        ((*new SgRecordRefExp(*s_blockidx, "x") - (*new SgValueExp(1))) * (*new SgValueExp(rank * 2))));
}

SgStatement *Assign_To_rest_blocks(int i)
{
    SgStatement *ass = NULL;
    SgExpression *e = NULL;
    // if i=0
    //   rest_blocks = blockIdx%x - 1   for Fortran-Cuda
    // or
    //   rest_blocks = blockIdx%x       for C_Cuda
    //if i>0
    //   rest_blocks=rest_blocks - cur_blocks*blocks_i
    if (i == 0)
    {
        e = &(*new SgVarRefExp(s_add_blocks) + *new SgRecordRefExp(*s_blockidx, "x"));
        e = options.isOn(C_CUDA) ? e : &(*e - *new SgValueExp(1));
    }
    else
        e = &(*new SgVarRefExp(s_rest_blocks) - *new SgVarRefExp(s_cur_blocks) * (*new SgVarRefExp(s_blocksS_k[i - 1])));

    ass = AssignStatement(new SgVarRefExp(s_rest_blocks), e);
    return(ass);
}

SgStatement *Assign_To_cur_blocks(int i, int nloop)
{
    SgStatement *ass = NULL;
    SgExpression *e = NULL;
    // cur_blocks = rest_blocks / blocks_i     i=0,1,2,...nloop-2
    // or
    // cur_blocks = rest_blocks                i = nloop-1
    e = i != nloop - 1 ? &(*new SgVarRefExp(s_rest_blocks) / *new SgVarRefExp(s_blocksS_k[i])) : new SgVarRefExp(s_rest_blocks);
    ass = AssignStatement(new SgVarRefExp(s_cur_blocks), e);
    return(ass);
}


SgStatement *Assign_To_IndVar(SgStatement *dost, int il, int nloop, SgSymbol *sblock)
{
    SgExpression *thr = NULL, *re = NULL;
    SgSymbol *indvar = NULL;
    SgStatement *ass = NULL;
    int H, ist;
    // H == 2
    // <sind> = blocks(ibof + <2*il>) + (threadIdx%x - 1) [ * <do_step> ]  , il=0,1,2
    // or for C_Cuda
    // <sind> = blocks(ibof + <2*il>) +  threadIdx%x  [ * <do_step> ]  , il=0,1,2

    H = 2;
    if (il == nloop - 1)
        thr = new SgRecordRefExp(*s_threadidx, "x");
    else if (il == (nloop - 2))
        thr = new SgRecordRefExp(*s_threadidx, "y");
    else if (il == nloop - 3)
        thr = new SgRecordRefExp(*s_threadidx, "z");
    indvar = dost->symbol();
    if (il >= nloop - 3)
    {
        re = options.isOn(C_CUDA) ? thr : &(*thr - (*new SgValueExp(1)));
        //estep=((SgForStmt *)dost)->step();
        //if( estep && ( ist=IConstStep(estep)) != 1  )
        if ((ist = IConstStep(dost)) != 1)
            *re = *re * (*new SgValueExp(ist));
        *re = (*blocksRef(sblock, H*il)) + (*re);
    }
    else
        re = blocksRef(sblock, H*il);

    ass = AssignStatement(new SgVarRefExp(indvar), re);
    return(ass);
}

SgStatement *Assign_To_IndVar2(SgStatement *dost, int i, int nloop)
{
    SgStatement *ass = NULL;
    SgExpression *e = NULL, *step_e = NULL, *eth = NULL, *es = NULL;

    int ist;
    // i = 1,...,nloop                   

    e = new SgVarRefExp(s_begin[i - 1]);
    if ((ist = IConstStep(dost)) != 1)
        step_e = new SgValueExp(ist);

    if (i == nloop)
        // ind_i = begin_i + (cur_blocks*blockDim%x + threadIdx%x [- 1]) [ * step_i ]
    {
        eth = ThreadIdxRefExpr("x");
        if (currentLoop && currentLoop->irregularAnalysisIsOn())
            es = &((*new SgVarRefExp(s_cur_blocks) * *new SgRecordRefExp(*s_blockdim, "x") + *eth) / *new SgValueExp(warpSize));
        else
            es = &(*new SgVarRefExp(s_cur_blocks) * *new SgRecordRefExp(*s_blockdim, "x") + *eth);
        es = step_e == NULL ? es : &(*es * *step_e);
        e = &(*e + *es);
    }
    else if (i == nloop - 1)
        // ind_i = begin_i + (cur_blocks*blockDim%y + threadIdx%y [- 1]) [ * step_i ]
    {
        eth = ThreadIdxRefExpr("y");
        es = &(*new SgVarRefExp(s_cur_blocks) * *new SgRecordRefExp(*s_blockdim, "y") + *eth);
        es = step_e == NULL ? es : &(*es * *step_e);
        e = &(*e + *es);
    }
    else if (i == nloop - 2)
        // ind_i = begin_i + (cur_blocks*blockDim%z + threadIdx%z [- 1]) [ * step_i ]
    {
        eth = ThreadIdxRefExpr("z");
        es = &(*new SgVarRefExp(s_cur_blocks) * *new SgRecordRefExp(*s_blockdim, "z") + *eth);
        es = step_e == NULL ? es : &(*es * *step_e);
        e = &(*e + *es);
    }
    else // 1 <= i <= nloop - 3
        // ind_i = begin_i + cur_blocks [ * step_i ]
    {
        es = new SgVarRefExp(s_cur_blocks);
        es = step_e == NULL ? es : &(*es * *step_e);
        e = &(*e + *es);
    }
    ass = AssignStatement(new SgVarRefExp(dost->symbol()), e);
    return(ass);

}

SgExpression *IbaseRef(SgSymbol *base, int ind)
{
    return(new SgArrayRefExp(*base, (*new SgVarRefExp(s_ibof) + (*new SgValueExp(ind)))));
}

SgExpression *blocksRef(SgSymbol *sblock, int ind)
{
    return(new SgArrayRefExp(*sblock, (*new SgVarRefExp(s_ibof) + (*new SgValueExp(ind)))));
}

/*!!!
void InsertDoWhileForRedCount(SgStatement *cp)
{ // inserting after statement cp (DO_WHILE) the block for red_count calculation:
//             red_count = 1
//             do while (red_count * 2 .lt. threads%x * threads%y * threads%z)
//                red_count = red_count * 2
//             end do

SgStatement *st_while, *ass;
SgExpression *cond;

RedCountSymbol();

// red_count * 2 .lt. threads%x * threads%y * threads%z
cond= & operator < ( *new SgVarRefExp(red_count_symb) * (*new SgValueExp(2)), *ThreadsGridSize(s_threads));
// insert do while loop
ass = new SgAssignStmt(*new SgVarRefExp(red_count_symb), (*new SgVarRefExp(red_count_symb))*(*new SgValueExp(2)));
st_while = new SgWhileStmt(*cond,*ass);
cp->insertStmtAfter(*st_while,*cp);
//  insert:           red_count = 1
ass = new SgAssignStmt(*new SgVarRefExp(red_count_symb), *new SgValueExp(1));
cp->insertStmtAfter(*ass,*cp);
}
*/

SgExpression *ThreadIdxRefExpr(char *xyz)
{
    if (options.isOn(C_CUDA))
        return(new SgRecordRefExp(*s_threadidx, xyz));
    else
        return(&(*new SgRecordRefExp(*s_threadidx, xyz) - *new SgValueExp(1)));
}

SgExpression *ThreadIdxRefExpr(const char *xyz)
{
    if (options.isOn(C_CUDA))
        return(new SgRecordRefExp(*s_threadidx, xyz));
    else
        return(&(*new SgRecordRefExp(*s_threadidx, xyz) - *new SgValueExp(1)));
}

SgExpression *BlockIdxRefExpr(char *xyz)
{
    if (!options.isOn(NO_BL_INFO))
    {
        if (options.isOn(C_CUDA))
            return(new SgRecordRefExp(*s_blockidx, xyz));
        else
            return(&(*new SgRecordRefExp(*s_blockidx, xyz) - *new SgValueExp(1)));
    }
    // without blocks_info
    if (options.isOn(C_CUDA))
        return(&(*new SgVarRefExp(s_add_blocks) + *new SgRecordRefExp(*s_blockidx, xyz)));
    else
        return(&(*new SgVarRefExp(s_add_blocks) + *new SgRecordRefExp(*s_blockidx, xyz) - *new SgValueExp(1)));
}

SgExpression *BlockIdxRefExpr(const char *xyz)
{
    if (!options.isOn(NO_BL_INFO))
    {
        if (options.isOn(C_CUDA))
            return(new SgRecordRefExp(*s_blockidx, xyz));
        else
            return(&(*new SgRecordRefExp(*s_blockidx, xyz) - *new SgValueExp(1)));
    }
    // without blocks_info
    if (options.isOn(C_CUDA))
        return(&(*new SgVarRefExp(s_add_blocks) + *new SgRecordRefExp(*s_blockidx, xyz)));
    else
        return(&(*new SgVarRefExp(s_add_blocks) + *new SgRecordRefExp(*s_blockidx, xyz) - *new SgValueExp(1)));
}

void CreateReductionBlocks(SgStatement *stat, int nloop, SgExpression *red_op_list, SgSymbol *red_count_symb)
{
    SgStatement *newst = NULL, *ass = NULL, *dost = NULL;
    SgExpression *er = NULL, *re = NULL;
    SgSymbol *i_var = NULL, *j_var = NULL;
    reduction_operation_list *rsl = NULL;
    int n = 0;

    formal_red_grid_list = NULL;

    // index variables
    dost = DoStmt(first_do_par, nloop);
    i_var = dost->symbol();

    if (!options.isOn(C_CUDA))
    {
        if (nloop > 1)
            j_var = dost->controlParent()->symbol();
        else
        {
            j_var = IndVarInKernel(i_var);
            newst = Declaration_Statement(j_var);
            kernel_st->insertStmtAfter(*newst, *kernel_st);
        }
    }

    // declare '<red_var>_block' array for each reduction var
    // <i_var> = threadIdx%x -1 + [ (threadIdx%y - 1) * blockDim%x [ + (threadIdx%z - 1) * blockDim%x * blockDim%y ] ]
    // or C_Cuda
    // <i_var> = threadIdx%x + [ threadIdx%y * blockDim%x [ + threadIdx%z * blockDim%x * blockDim%y ] ]

    //re = & ( *new SgRecordRefExp(*s_threadidx,"x") - *new SgValueExp(1) );
    re = ThreadIdxRefExpr("x");
    if (options.isOn(C_CUDA))
    {
        re = &(*re + (*ThreadIdxRefExpr("y")) * (*new SgRecordRefExp(*s_blockdim, "x")));
        re = &(*re + (*ThreadIdxRefExpr("z")) * (*new SgRecordRefExp(*s_blockdim, "x") * (*new SgRecordRefExp(*s_blockdim, "y"))));
    }
    else
    {
        if (nloop > 1)
            //re = &( *re + ((*new SgRecordRefExp(*s_threadidx,"y")) - (*new SgValueExp(1))) * (*new SgRecordRefExp(*s_blockdim,"x")));
            re = &(*re + (*ThreadIdxRefExpr("y")) * (*new SgRecordRefExp(*s_blockdim, "x")));
        if (nloop > 2)
            //re = &( *re + ((*new SgRecordRefExp(*s_threadidx,"z")) - (*new SgValueExp(1))) * (*new SgRecordRefExp(*s_blockdim,"x") * (*new SgRecordRefExp(*s_blockdim,"y"))));
            re = &(*re + (*ThreadIdxRefExpr("z")) * (*new SgRecordRefExp(*s_blockdim, "x") * (*new SgRecordRefExp(*s_blockdim, "y"))));
    }
    ass = AssignStatement(new SgVarRefExp(i_var), re);

    if (options.isOn(C_CUDA))
        ass->addComment("// Reduction");
    else
        ass->addComment("! Reduction\n");

    //looking through the reduction_op_list

    SgIfStmt *if_st = NULL;
    SgIfStmt *if_del = NULL;
    SgIfStmt *if_new = NULL;
    int declArrayVars = 1;

    if (options.isOn(C_CUDA))
        if_st = new SgIfStmt(SgEqOp(*new SgVarRefExp(i_var) % *new SgVarRefExp(s_warpsize), *new SgValueExp(0)));

    bool assInserted = false;
    for (er = red_op_list, rsl = red_struct_list, n = 1; er; er = er->rhs(), rsl = rsl->next, n++)
    {
        if (rsl->redvar_size != 0 && rsl->redvar_size <= 0) // array of [UNknown size] or arrays that have [ > 16 elems]
            continue;

        if (!assInserted)
        {
            stat->insertStmtBefore(*ass, *stat->controlParent());
            assInserted = true;
        }

        if (options.isOn(C_CUDA))
            ReductionBlockInKernel_On_C_Cuda(stat, i_var, er->lhs(), rsl, if_st, if_del, if_new, declArrayVars);
        else
            ReductionBlockInKernel(stat, nloop, i_var, j_var, er->lhs(), rsl, red_count_symb, n);
    }

    
    if (options.isOn(C_CUDA) && assInserted)
        stat->insertStmtBefore(*if_st, *stat->controlParent());
}

char* getMultipleTypeName(SgType *base, int num)
{
    char dnum = '0' + num;
    char *ret = new char[32];
    ret[0] = '\0';

    if (base->variant() == SgTypeChar()->variant())
        strcat(ret, "char");
    else if (base->variant() == SgTypeInt()->variant())
        strcat(ret, "int");
    else if (base->variant() == SgTypeDouble()->variant())
        strcat(ret, "double");
    else if (base->variant() == SgTypeFloat()->variant())
        strcat(ret, "float");

    int len = strlen(ret);
    if (len != 0 && num > 0)
    {
        ret[len] = dnum;
        ret[len + 1] = '\0';
    }
    return ret;
}

void ReductionBlockInKernel_On_C_Cuda(SgStatement *stat, SgSymbol *i_var, SgExpression *ered, reduction_operation_list *rsl,
    SgIfStmt *if_st, SgIfStmt *&delIf, SgIfStmt *&newIf, int &declArrayVars, bool withGridReduction, bool across)
{
    SgStatement *newst;
    SgFunctionCallExp *fun_ref = NULL;

    SgExpression *ex = &(*new SgVarRefExp(i_var) / *new SgVarRefExp(s_warpsize));
    // blockDim.x * blockDim.y * blockDim.z / warpSize
    SgExpression *ex1 = &(*new SgRecordRefExp(*s_blockdim, "x") * *new SgRecordRefExp(*s_blockdim, "y") * *new SgRecordRefExp(*s_blockdim, "z") / *new SgVarRefExp(s_warpsize));
    // blockDim.x * blockDim.y * blockDim.z
    SgExpression *ex2 = &(*new SgRecordRefExp(*s_blockdim, "x") * *new SgRecordRefExp(*s_blockdim, "y") * *new SgRecordRefExp(*s_blockdim, "z"));

    if (rsl->redvar_size != 0) // array reduction
    {
        if (rsl->redvar_size > 0) // array of known size 
        {
            char *funcName = new char[256];

            //declare red_var variable
            if (rsl->array_red_size > 0)
            {
                SgSymbol *s = rsl->redvar;
                SgArrayType *arrT = new SgArrayType(*C_Type(s->type()->baseType()));
                arrT->addRange(*new SgValueExp(rsl->array_red_size));
                SgSymbol *forDecl = new SgVariableSymb(rsl->redvar->identifier(), *arrT, *kernel_st);
                newst = Declaration_Statement(forDecl);
                kernel_st->insertStmtAfter(*newst, *kernel_st);
            }
            else
            {
                newst = Declaration_Statement(RedVariableSymbolInKernel(rsl->redvar, NULL));
                kernel_st->insertStmtAfter(*newst, *kernel_st);
            }

            funcName[0] = '\0';
            strcat(funcName, RedFunctionInKernelC((const int)RedFuncNumber(ered->lhs()), rsl->redvar_size, 0));
            SgExpression *tmplArgs = new SgExpression(CONS, new SgTypeRefExp(*C_Type(rsl->redvar->type())), new SgValueExp(rsl->redvar_size), NULL);

            fun_ref = new SgFunctionCallExp(*RedFunctionSymbolInKernel(funcName));
            fun_ref->addArg(*new SgVarRefExp(rsl->redvar));
            fun_ref->setRhs(tmplArgs);
            stat->insertStmtBefore(*new SgCExpStmt(*fun_ref), *stat->controlParent());

            int idx = 0;
            for (int k = 0; k < rsl->redvar_size; ++k)
            {
                newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *new SgVarRefExp(s_overall_blocks) * *new SgValueExp(idx)
                    + *BlockIdxRefExpr("x") * *ex1 + *ex), new SgArrayRefExp(*rsl->redvar, *new SgValueExp(idx)));
                idx++;
                if_st->lastExecutable()->insertStmtAfter(*newst);
            }
        }
        else // array of [UNknown size] or arrays that have [ > 16 elems]
        {
            int rank = Rank(rsl->redvar);

            if (rsl->array_red_size < 1)
            {
                char *newN = new char[strlen(rsl->redvar->identifier()) + 9];
                newN[0] = '\0';
                strcat(newN, "__addr_");
                strcat(newN, rsl->redvar->identifier());
                SgSymbol *tmp = new SgSymbol(VARIABLE_NAME, newN, C_DvmType(), kernel_st);
                newst = Declaration_Statement(tmp);
                newst->addDeclSpec(BIT_CUDA_SHARED);
                kernel_st->insertStmtAfter(*newst, *kernel_st);

                // insert IF-block with new stmts 
                SgArrayType *arr = new SgArrayType(*C_Type(rsl->redvar->type()->baseType()));
                SgExpression *dims = RedVarUpperBound(rsl->dimSize_arg, 1);
                for (int i = 2; i <= rank; ++i)
                    dims = &(*dims * *RedVarUpperBound(rsl->dimSize_arg, i));
                // new type[ num * blockDims]
                arr->addDimension(&(*dims * *new SgRecordRefExp(*s_blockdim, "x") * *new SgRecordRefExp(*s_blockdim, "y") * *new SgRecordRefExp(*s_blockdim, "z")));
                SgNewExp *newEx = new SgNewExp(*arr);

                if (newIf)
                    newIf->lastExecutable()->insertStmtAfter(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(rsl->redvar), *newEx)));
                else
                {
                    // i = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * (blockDim.x * blockDim.y);
                    SgStatement *idx = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(i_var),
                        *new SgRecordRefExp(*s_threadidx, "x") + *new SgRecordRefExp(*s_threadidx, "y") * *new SgRecordRefExp(*s_blockdim, "x") + *new SgRecordRefExp(*s_threadidx, "z") * *new SgRecordRefExp(*s_blockdim, "x")* *new SgRecordRefExp(*s_blockdim, "y")));
                    newIf = new SgIfStmt(SgEqOp(*new SgVarRefExp(i_var), *new SgValueExp(0)), *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(rsl->redvar), *newEx)));

                    kernel_st->lexNext()->insertStmtAfter(*FunctionCallStatement(SyncthreadsSymbol()));
                    kernel_st->lexNext()->insertStmtAfter(*newIf);
                    kernel_st->lexNext()->insertStmtAfter(*idx);
                    idx->addComment(" // Allocate memory for reduction");
                }

                SgPointerType *pointer = new SgPointerType(*C_Type(rsl->redvar->type()->baseType()));
                SgReferenceType *ref = new SgReferenceType(*C_DvmType());
                newIf->lastExecutable()->insertStmtAfter(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(tmp), *new SgCastExp(*ref, *new SgVarRefExp(rsl->redvar)))));
                newIf->lastNodeOfStmt()->lexNext()->insertStmtAfter(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(rsl->redvar), *new SgVarRefExp(rsl->redvar) + *new SgVarRefExp(i_var))));
                newIf->lastNodeOfStmt()->lexNext()->insertStmtAfter(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(rsl->redvar), *new SgCastExp(*pointer, *new SgVarRefExp(tmp)))));


                // insert IF-block with delete stmts
                SgDeleteExp *delEx = new SgDeleteExp(*new SgVarRefExp(rsl->redvar));
                if (delIf)
                    delIf->lastExecutable()->insertStmtAfter(*new SgCExpStmt(*delEx));
                else
                {
                    delIf = new SgIfStmt(SgEqOp(*new SgVarRefExp(i_var), *new SgValueExp(0)), *new SgCExpStmt(*delEx));
                    newst = FunctionCallStatement(SyncthreadsSymbol());

                    if_st->lastNodeOfStmt()->insertStmtAfter(*delIf);
                    if_st->lastNodeOfStmt()->insertStmtAfter(*newst);
                    newst->addComment(" // Deallocate memory for reduction");
                }
            }

            //declare red_var variable
            if (rsl->array_red_size > 0)
            {
                SgSymbol *s = rsl->redvar;
                SgArrayType *arrT = new SgArrayType(*C_Type(s->type()->baseType()));
                arrT->addRange(*new SgValueExp(rsl->array_red_size));
                SgSymbol *forDecl = new SgVariableSymb(rsl->redvar->identifier(), *arrT, *kernel_st);
                newst = Declaration_Statement(forDecl);
                kernel_st->insertStmtAfter(*newst, *kernel_st);
            }
            else
            {
                newst = Declaration_Statement(RedVariableSymbolInKernel(rsl->redvar, NULL));
                kernel_st->insertStmtAfter(*newst, *kernel_st);
            }

            for (int i = declArrayVars; i <= rank; ++i)
            {
                newst = Declaration_Statement(IndexLoopVar(i)); //declare red_varIDX variable
                kernel_st->insertStmtAfter(*newst, *kernel_st);
            }
            declArrayVars = MAX(declArrayVars, rank);


            char *funcName = new char[256];
            SgExpression *tmplArgs;

            funcName[0] = '\0';
            strcat(funcName, RedFunctionInKernelC((const int)RedFuncNumber(ered->lhs()), rsl->array_red_size, 0));
            if (rsl->array_red_size > 1)
                tmplArgs = new SgExpression(CONS, new SgTypeRefExp(*C_Type(rsl->redvar->type())), new SgValueExp(rsl->array_red_size), NULL);
            else
                tmplArgs = new SgExpression(CONS, new SgTypeRefExp(*C_Type(rsl->redvar->type())), RedVarUpperBound(rsl->dimSize_arg, 1), NULL);

            fun_ref = new SgFunctionCallExp(*RedFunctionSymbolInKernel(funcName));
            fun_ref->addArg(*new SgVarRefExp(rsl->redvar));
            if (rsl->array_red_size > 0)
                fun_ref->setRhs(tmplArgs);
            else
            {
                // blockDims
                fun_ref->addArg(*new SgRecordRefExp(*s_blockdim, "x") * *new SgRecordRefExp(*s_blockdim, "y") * *new SgRecordRefExp(*s_blockdim, "z"));
                SgExpression *dims = RedVarUpperBound(rsl->dimSize_arg, 1);
                for (int i = 2; i <= rank; ++i)
                    dims = &(*dims * *RedVarUpperBound(rsl->dimSize_arg, i));
                fun_ref->addArg(*dims);
            }
            stat->insertStmtBefore(*new SgCExpStmt(*fun_ref), *stat->controlParent());

            if (rsl->array_red_size > 1)
            {
                int idx = 0;
                for (int k = 0; k < rsl->array_red_size; ++k)
                {
                    newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *new SgVarRefExp(s_overall_blocks) * *new SgValueExp(idx)
                        + *BlockIdxRefExpr("x") * *ex1 + *ex), new SgArrayRefExp(*rsl->redvar, *new SgValueExp(idx)));
                    idx++;
                    if_st->lastExecutable()->insertStmtAfter(*newst);
                }
            }
            else
            {
                SgExpression *linearIdx = new SgVarRefExp(IndexLoopVar(1));
                for (int i = 2; i <= rank; ++i)
                {
                    SgExpression *dims = RedVarUpperBound(rsl->dimSize_arg, 1);
                    for (int k = 2; k < i; ++k)
                        dims = &(*dims * *RedVarUpperBound(rsl->dimSize_arg, k));
                    linearIdx = &(*linearIdx + *new SgVarRefExp(IndexLoopVar(i)) * *dims);
                }
                newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *new SgVarRefExp(s_overall_blocks) * *linearIdx
                    + *BlockIdxRefExpr("x") * *ex1 + *ex), new SgArrayRefExp(*rsl->redvar, *linearIdx * *ex2));
                if_st->lastExecutable()->insertStmtAfter(*doLoopNestForReductionArray(rsl, newst));
            }
        }
    }
    else if (rsl->locvar) // maxloc/minloc reduction scalar
    {
        SgType *decl;
        int rank = rsl->number;

        if (rank > 1)
        {
            SgArrayType *arrT = new SgArrayType(*C_Type(rsl->locvar->type()));
            arrT->addDimension(new SgValueExp(rank));
            decl = arrT;
        }
        else
            decl = C_Type(rsl->locvar->type());
        newst = Declaration_Statement(new SgVariableSymb(rsl->locvar->identifier(), *decl, *kernel_st)); //declare location variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);

        //  __dvmh_blockReduce<op>Loc(<red_var>, <loc_var>)
        fun_ref = new SgFunctionCallExp(*RedFunctionSymbolInKernel((char *)RedFunctionInKernelC((const int)RedFuncNumber(ered->lhs()), 1, rsl->number)));
        fun_ref->addArg(*new SgVarRefExp(*rsl->redvar));
        if (rsl->number == 1)
            fun_ref->addArg(SgAddrOp(*new SgVarRefExp(*rsl->locvar)));
        else
            fun_ref->addArg(*new SgVarRefExp(*rsl->locvar));

        SgExpression *tmplArgs = new SgExpression(CONS, new SgTypeRefExp(*C_Type(rsl->redvar->type())),
            new SgExpression(CONS, new SgTypeRefExp(*C_Type(rsl->locvar->type())), new SgValueExp(rsl->number), NULL), NULL);
        fun_ref->setRhs(tmplArgs);

        stat->insertStmtBefore(*new SgCExpStmt(*fun_ref), *stat->controlParent());

        newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x") * *ex1 + *ex), new SgVarRefExp(rsl->redvar));
        if_st->insertStmtAfter(*newst);

        if (rsl->number > 1)
        {
            for (int i = 0; i < rsl->number; ++i)
            {
                newst = AssignStatement(new SgArrayRefExp(*rsl->loc_grid, *new SgValueExp(rsl->number) * (*BlockIdxRefExpr("x") * *ex1 + *ex) + *new SgValueExp(i)), new SgArrayRefExp(*rsl->locvar, *new SgValueExp(i)));
                if_st->lastExecutable()->insertStmtAfter(*newst);
            }
        }
        else
        {
            newst = AssignStatement(new SgArrayRefExp(*rsl->loc_grid, *BlockIdxRefExpr("x") * *ex1 + *ex), new SgVarRefExp(*rsl->locvar));
            if_st->lastExecutable()->insertStmtAfter(*newst);
        }

    }
    else    // scalar reduction
    {
        //  <red_var> = __dvmh_blockReduce<red-op>(<red_var>)
        fun_ref = new SgFunctionCallExp(*RedFunctionSymbolInKernel((char *)RedFunctionInKernelC(RedFuncNumber(ered->lhs()), 1, 0)));
        fun_ref->addArg(*new SgVarRefExp(*rsl->redvar));
        newst = AssignStatement(new SgVarRefExp(*rsl->redvar), fun_ref);
        stat->insertStmtBefore(*newst, *stat->controlParent());

        if (withGridReduction)
        {
            SgExpression* gridRef = NULL;
            if (across)
                gridRef = new SgArrayRefExp(*rsl->red_grid, *ex);
            else
                gridRef = new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x") * *ex1 + *ex);

            SgExpression* redRef = new SgVarRefExp(rsl->redvar);
            int redVar = RedFuncNumber(ered->lhs());
            if (redVar == 1) // sum
                newst = AssignStatement(gridRef, &(gridRef->copy() + *redRef));
            if (redVar == 2) // product
                newst = AssignStatement(gridRef, &(gridRef->copy() * *redRef));
            if (redVar == 3) // max
            {
                SgFunctionCallExp* fCall = new SgFunctionCallExp(*new SgSymbol(FUNCTION_NAME, "max"));
                fCall->addArg(gridRef->copy());
                fCall->addArg(*redRef);
                newst = AssignStatement(gridRef, fCall);
            }
            if (redVar == 4) // min
            {
                SgFunctionCallExp* fCall = new SgFunctionCallExp(*new SgSymbol(FUNCTION_NAME, "min"));
                fCall->addArg(gridRef->copy());
                fCall->addArg(*redRef);
                newst = AssignStatement(gridRef, fCall);
            }
            if (redVar == 5) // and
                newst = AssignStatement(gridRef, new SgExpression(BITAND_OP, &gridRef->copy(), redRef));
            if (redVar == 6) // or
                newst = AssignStatement(gridRef, new SgExpression(BITOR_OP, &gridRef->copy(), redRef));

#ifdef INTEL_LOGICAL_TYPE
            if (redVar == 7) // neqv
                newst = AssignStatement(gridRef, new SgExpression(XOR_OP, &gridRef->copy(), redRef));
            if (redVar == 8) // eqv
                newst = AssignStatement(gridRef, new SgExpression(BIT_COMPLEMENT_OP, new SgExpression(XOR_OP, &gridRef->copy(), redRef), NULL));            
#else
            if (redVar == 7) // neqv
                newst = AssignStatement(gridRef, &(gridRef->copy() != *redRef));
            if (redVar == 8) // eqv
                newst = AssignStatement(gridRef, &(gridRef->copy() == *redRef));
#endif
        }
        else
            newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x") * *ex1 + *ex), new SgVarRefExp(rsl->redvar));
        if_st->insertStmtAfter(*newst);
    }
}

void ReductionBlockInKernel(SgStatement *stat, int nloop, SgSymbol *i_var, SgSymbol *j_var, SgExpression *ered, reduction_operation_list *rsl, SgSymbol *red_count_symb, int n)
{
    SgStatement *ass = NULL, *newst = NULL, *current = NULL, *if_st = NULL, *while_st = NULL, *typedecl = NULL, *st = NULL, *do_st = NULL;
    SgExpression *le = NULL, *re = NULL, *eatr = NULL, *cond = NULL, *ev = NULL;
    SgSymbol *red_var = NULL, *red_var_k = NULL, *s_block = NULL, *loc_var = NULL, *sf = NULL;
    SgType *rtype = NULL;
    int i, ind;
    loc_el_num = 0;
    //call syncthreads() for second, third,... reduction operation (n>1) 
    if (n > 1)
    {
        newst = FunctionCallStatement(SyncthreadsSymbol());
        stat->insertStmtBefore(*newst, *stat->controlParent());
    }
    // analys of reduction operation 
    // ered - reduction operation (variant==ARRAY_OP)
    ev = ered->rhs(); // reduction variable reference for reduction operations except MINLOC,MAXLOC  
    if (isSgExprListExp(ev))    // for MAXLOC,MINLOC            
    {
        loc_var = ev->rhs()->lhs()->symbol();  //location array reference
        ev = ev->lhs(); // reduction variable reference 
    }
    else
        loc_var = NULL;

    //              <red_var>_block([ k,] i) = <red_var>       [k=LowerBound:UpperBound]
    // or for MAXLOC,MINLOC
    //              <red_var>_block(i)%<red_var>    = <red_var>
    //              <red_var>_block(i)%<loc_var>(1) = <loc_var>(1)
    //             [<red_var>_block(i)%<loc_var>(2) = <loc_var>(2) ]
    //                    .   .   .
    // create and declare array '<red_var>_block'     
    red_var = ev->symbol();

    if (rsl->locvar)
    {
        newst = Declaration_Statement(RedVariableSymbolInKernel(rsl->locvar, NULL)); //declare location variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
        //SymbolChange_InBlock(new SgSymbol(VARIABLE_NAME,"aaaa",rsl->locvar->type(),kernel_st),rsl->locvar,cur_in_kernel,cur_in_kernel->lastNodeOfStmt());
    }


    if (rsl->redvar_size != 0)
    {
        red_var_k = RedVariableSymbolInKernel(rsl->redvar, rsl->dimSize_arg);
        newst = Declaration_Statement(red_var_k); //declare reduction variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }
    rtype = (rsl->redvar_size >= 0) ? TypeOfRedBlockSymbol(ered) : NULL;

    s_block = RedBlockSymbolInKernel(red_var, rtype);

    newst = Declaration_Statement(s_block);

    if (options.isOn(C_CUDA)) // in C Language
        newst->addDeclSpec(BIT_CUDA_SHARED | BIT_EXTERN);
    else      // in Fortran Language
    {
        eatr = new SgExprListExp(*new SgExpression(ACC_SHARED_OP));
        newst->setExpression(2, *eatr);
    }

    kernel_st->insertStmtAfter(*newst, *kernel_st);

    // create assign statement[s]     
    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
    {
        typedecl = MakeStructDecl(rtype->symbol());
        kernel_st->insertStmtAfter(*typedecl, *kernel_st);
        sf = RedVarFieldSymb(s_block);
        le = RedLocVar_Block_Ref(s_block, i_var, NULL, new SgVarRefExp((sf)));
        re = new SgVarRefExp(red_var);
        ass = AssignStatement(le, re);
        stat->insertStmtBefore(*ass, *stat->controlParent());
        for (i = 1; i <= rsl->number; i++)
        {
            ind = options.isOn(C_CUDA) ? i - 1 : i;
            le = RedLocVar_Block_Ref(s_block, i_var, NULL, new SgArrayRefExp(*((SgFieldSymb *)sf)->nextField(), *new SgValueExp(ind)));
            if (isSgArrayType(rsl->locvar->type()))
                re = new SgArrayRefExp(*(rsl->locvar), *LocVarIndex(rsl->locvar, i));
            else
                re = new SgVarRefExp(*(rsl->locvar));
            ass = AssignStatement(le, re);
            stat->insertStmtBefore(*ass, *stat->controlParent());
        }
    }
    else if (rsl->redvar_size > 0) //reduction variable is array of known size

    for (i = 0; i < rsl->redvar_size; i++)
    {
        SgExpression *red_ind;
        red_ind = RedVarIndex(red_var, i);
        le = RedVar_Block_2D_Ref(s_block, i_var, red_ind);
        re = new SgArrayRefExp(*red_var, *red_ind);
        ass = AssignStatement(le, re);
        stat->insertStmtBefore(*ass, *stat->controlParent());
    }

    else  if (rsl->redvar_size == 0) //reduction variable is scalar      
    {
        le = RedVar_Block_Ref(s_block, i_var);
        re = new SgVarRefExp(red_var);
        ass = AssignStatement(le, re);
        stat->insertStmtBefore(*ass, *stat->controlParent());
    }
    else                             //reduction variable is array of unknown size
    {
        le = RedVar_Block_Ref(s_block, i_var);
        re = new SgArrayRefExp(*rsl->redvar, *SubscriptListOfRedArray(rsl->redvar));
        ass = AssignStatement(le, re);
        // create loop nest and insert it before 'stat'
        do_st = doLoopNestForReductionArray(rsl, ass);
        stat->insertStmtBefore(*do_st, *stat->controlParent());
        while (do_st->variant() == FOR_NODE)
            do_st = do_st->lexNext();
        stat = do_st->lexNext(); // CONTROL_END of innermost loop
    }

    //call syncthreads()    
    newst = FunctionCallStatement(SyncthreadsSymbol());
    stat->insertStmtBefore(*newst, *stat->controlParent());

    // [if (i .lt. red_count) then ]  // for last reduction of loop /*24.10.12*/
    //         if (i + red_count .lt. blockDim%x [* blockDim%y [* blockDim%z]])  then
    //            <red_var>_block([ k,] i) = <red_op> (<red_var>_block([ k,] i), <red_var>_block([ k,] i + red_count)) [k=LowerBound:UpperBound]
    //         end if
    // [  endif ]

    //  or for MAXLOC,MINLOC
    // [if (i .lt. red_count) then ]  // for last reduction of loop /*24.10.12*/
    //         if (i + red_count .lt. blockDim%x [* blockDim%y [* blockDim%z]])  then
    //            if(<red_var>_block(i + red_count)%<red_var> .gt. <red_var>_block(i)%<red_var>) then//MAXLOC
    //              <red_var>_block(i)%<red_var>    = <red_var>_block(i + red_count)%<red_var>
    //              <red_var>_block(i)%<loc_var>(1) = <red_var>_block(i + red_count)%<loc_var>(1)
    //             [<red_var>_block(i)%<loc_var>(2) = <red_var>_block(i + red_count)%<loc_var>(2) ]
    //                           .   .   .
    //            endif
    //         endif
    //  [ endif ]
    re = new SgRecordRefExp(*s_blockdim, "x");
    if (nloop > 1)
        re = &(*re * (*new SgRecordRefExp(*s_blockdim, "y")));
    if (nloop > 2)
        re = &(*re * (*new SgRecordRefExp(*s_blockdim, "z")));
    cond = &operator < ((*new SgVarRefExp(i_var) + *new SgVarRefExp(red_count_symb)), *re);

    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
        newst = RedOp_If(i_var, s_block, ered, red_count_symb, rsl->number);
    else
        newst = RedOp_Assign(i_var, s_block, ered, red_count_symb, 0);
    if_st = new SgIfStmt(*cond, *newst);
    if (rsl->redvar_size > 0)
    for (i = 1; i < rsl->redvar_size; i++)
    {
        newst->insertStmtAfter(*(ass = RedOp_Assign(i_var, s_block, ered, red_count_symb, i)), *if_st);
        newst = ass;
    }
    if (!rsl->next && rsl->redvar_size >= 0) //last reduction of loop, not array of unknown size 
    {
        cond = &operator < (*new SgVarRefExp(i_var), *new SgVarRefExp(red_count_symb));
        newst = new SgIfStmt(*cond, *if_st);
        stat->insertStmtBefore(*newst, *stat->controlParent());
    }
    else
        stat->insertStmtBefore(*if_st, *stat->controlParent());

    //         j = red_count / 2
    ass = AssignStatement(new SgVarRefExp(j_var), &(*new SgVarRefExp(red_count_symb) / *new SgValueExp(2)));
    if (!rsl->next && rsl->redvar_size >= 0) //last reduction of loop, not array of unknown size
        if_st->insertStmtAfter(*ass, *newst);
    //!!!if_st->insertStmtAfter(*ass,*stat->controlParent()); //!!!if_st->insertStmtAfter(*ass,*newst);
    else
        stat->insertStmtBefore(*ass, *stat->controlParent());
    current = ass;
    //!!!last = ass->lexNext();

    //         if (i .eq. 0) then
    //            <red_var>_grid( blockIdx%x - 1,[ m]) = <red_var>_block([ k,] 0)     [k=LowerBound:UpperBound, m=1,...]
    //         endif
    //
    //               or for MAXLOC,MINLOC
    //
    //         if (i .eq. 0) then
    //            <red_var>_grid   (blockIdx%x [ - 1 ] ) = <red_var>_block(0)%<red_var>
    //            <loc_var>_grid(1, blockIdx%x  - 1  ) = <red_var>_block(0)%<loc_var>(1)   or if C_Cuda  <loc_var>_grid[(L-1)*blockIdx%x] =     <red_var>_block(0)%<loc_var>[0]
    //            <loc_var>_grid(2, blockIdx%x  - 1  ) = <red_var>_block(0)%<loc_var>(2)   or if C_Cuda  <loc_var>_grid[(L-1)*blockIdx%x + 1] = <red_var>_block(0)%<loc_var>[1]
    //                  .    .    .
    //          
    //         endif

    cond = &SgEqOp(*new SgVarRefExp(i_var), *new SgValueExp(0));
    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
        //newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *new SgRecordRefExp(*s_blockidx,"x") - *new SgValueExp(1) ) ,RedLocVar_Block_Ref(s_block,NULL,NULL,new SgVarRefExp((sf)))); 
        newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x")), RedLocVar_Block_Ref(s_block, NULL, NULL, new SgVarRefExp((sf))));
    else
    {
        if (rsl->redvar_size > 0)
            //newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *new SgRecordRefExp(*s_blockidx,"x") - *new SgValueExp(1) , *new SgValueExp(1)) , new SgArrayRefExp(*s_block, *RedVarIndex(red_var,0),*new SgValueExp(0))); 
            newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x"), *new SgValueExp(1)), new SgArrayRefExp(*s_block, *RedVarIndex(red_var, 0), *new SgValueExp(0)));
        else if (rsl->redvar_size == 0)
            //newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *new SgRecordRefExp(*s_blockidx,"x") - *new SgValueExp(1) ) , new SgArrayRefExp(*s_block, *new SgValueExp(0)));         
            newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x")), new SgArrayRefExp(*s_block, *new SgValueExp(0)));
        else
            newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x"), *SubscriptListOfRedArray(red_var)), new SgArrayRefExp(*s_block, *new SgValueExp(0)));
    }

    if_st = new SgIfStmt(*cond, *newst);
    if (rsl->redvar_size > 0)
    for (i = 1; i < rsl->redvar_size; i++)
    {
        //ass = AssignStatement(new SgArrayRefExp(*rsl->red_grid,*new SgRecordRefExp(*s_blockidx,"x") - *new SgValueExp(1), *new SgValueExp(i+1) ) , new SgArrayRefExp(*s_block, *RedVarIndex(red_var,i),*new SgValueExp(0))); 
        ass = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr("x"), *new SgValueExp(i + 1)), new SgArrayRefExp(*s_block, *RedVarIndex(red_var, i), *new SgValueExp(0)));
        newst->insertStmtAfter(*ass, *if_st);
        newst = ass;
    }
    current->insertStmtAfter(*if_st, *current->controlParent());
    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
    {
        st = newst;
        for (i = 1; i <= rsl->number; i++)
        {
            ind = options.isOn(C_CUDA) ? i - 1 : i;
            re = RedLocVar_Block_Ref(s_block, NULL, NULL, new SgArrayRefExp(*((SgFieldSymb *)sf)->nextField(), *new SgValueExp(ind)));
            //le = new SgArrayRefExp(*rsl->loc_grid, *new SgValueExp(ind), *new SgRecordRefExp(*s_blockidx,"x") - *new SgValueExp(1) );
            if (options.isOn(C_CUDA))
                le = new SgArrayRefExp(*rsl->loc_grid, *LinearIndex(ind, rsl->number));
            else
                le = new SgArrayRefExp(*rsl->loc_grid, *new SgValueExp(ind), *BlockIdxRefExpr("x"));
            ass = AssignStatement(le, re);
            st->insertStmtAfter(*ass, *if_st);
            st = ass;
        }
    }

    //         do while(j .ge. 1)
    //            call syncthreads()
    //            if (i .lt. j) then
    //
    //               <red_var>_block([ k,] i) = <red_op>(<red_var>_block([ k,] i), <red_var>_block([ k,] i + j))
    //
    //               or for MAXLOC,MINLOC
    //
    //             if(<red_var>_block(i + j)%<red_var> .gt. <red_var>_block(i)%<red_var>) then //MAXLOC
    //              <red_var>_block(i)%<red_var>    = <red_var>_block(i + j)%<red_var>
    //              <red_var>_block(i)%<loc_var>(1) = <red_var>_block(i + j)%<loc_var>(1)
    //             [<red_var>_block(i)%<loc_var>(2) = <red_var>_block(i + j)%<loc_var>(2) ]
    //                           .   .   .
    //            endif

    //            end if
    //         end do

    cond = &operator >=(*new SgVarRefExp(j_var), *new SgValueExp(1));
    newst = FunctionCallStatement(SyncthreadsSymbol());
    while_st = new SgWhileStmt(*cond, *newst);
    current->insertStmtAfter(*while_st, *current->controlParent());
    current = newst;
    ass = AssignStatement(new SgVarRefExp(j_var), &(*new SgVarRefExp(j_var) / *new SgValueExp(2)));
    current->insertStmtAfter(*ass, *while_st);
    cond = &operator < (*new SgVarRefExp(i_var), *new SgVarRefExp(j_var));
    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
        newst = RedOp_If(i_var, s_block, ered, j_var, rsl->number);
    else
        newst = RedOp_Assign(i_var, s_block, ered, j_var, 0);

    //!ass = RedOp_Assign(i_var,s_block,ered,j_var);        
    if_st = new SgIfStmt(*cond, *newst);
    if (rsl->redvar_size > 0)       // reduction variable is array
    for (i = 1; i < rsl->redvar_size; i++)
    {
        newst->insertStmtAfter(*(ass = RedOp_Assign(i_var, s_block, ered, j_var, i)), *if_st);
        newst = ass;
    }

    current->insertStmtAfter(*if_st, *while_st);

}

SgExpression * LinearIndex(int ind, int L)
{
    SgExpression * e;
    if (L != 1)
        e = &(*new SgValueExp(L) * *BlockIdxRefExpr("x"));
    else
        e = BlockIdxRefExpr("x");
    if (ind)
        e = &(*e + *new SgValueExp(ind));
    return(e);
}

SgExpression *Red_grid_index(SgSymbol *sind)
{
    SgExpression *e1, *e2;
    e1 = new SgRecordRefExp(*s_blockidx, "x");
    e2 = &(*new SgVarRefExp(s_blockDims) / *new SgVarRefExp(s_warpsize));
    e1 = &(*e1 * *e2);
    e2 = &(*new SgVarRefExp(sind) / *new SgVarRefExp(s_warpsize));
    e1 = &(*e1 + *e2);
    return(e1);
}


SgType *TypeOfRedBlockSymbol(SgExpression *ered)
{
    SgExpression *ev, *el, *en, *ec;
    SgType *type, *loc_type;
    SgArrayType *typearray;
    int num_el = 0;
    ev = ered->rhs();
    if (!isSgExprListExp(ev))
        return(options.isOn(C_CUDA) ? C_Type(ev->symbol()->type()) : ev->symbol()->type());
    // MAXLOC,MINLOC 
    el = ev->rhs()->lhs();
    en = ev->rhs()->rhs()->lhs();
    // calculation number of location array, assign to global variable  'loc_el_num' 
    ec = Calculate(en);
    if (ec->isInteger())
        loc_el_num = num_el = ec->valueInteger();
    else
        Error("Can not calculate number of elements in array %s", el->symbol()->identifier(), 595, dvm_parallel_dir);

    ev = ev->lhs(); // reduction variable reference
    type = ev->symbol()->type();
    if (isSgArrayType(type))
        type = type->baseType();
    if (options.isOn(C_CUDA))
        type = C_Type(type);
    loc_type = el->symbol()->type();
    if (isSgArrayType(loc_type))
        loc_type = loc_type->baseType();
    if (options.isOn(C_CUDA))
        loc_type = C_Type(loc_type);

    typearray = new SgArrayType(*loc_type);

    typearray->addRange(*new SgValueExp(num_el));

    return(Type_For_Red_Loc(ev->symbol(), el->symbol(), type, typearray));

}

const char* RedFunctionInKernelC(const int num_red, const unsigned num_E = 1, const unsigned num_IE = 1)
{
    const char *retVal = NULL;

    if (num_red == 1) // sum
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_SUM];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_SUM_N];
    }
    else if (num_red == 2)  // product
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_PROD];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_PROD_N];
    }
    else if (num_red == 3)  // max
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_MAX];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_MAX_N];
    }
    else if (num_red == 4)  // min
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_MIN];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_MIN_N];
    }
    else if (num_red == 5)  // and
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_AND];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_AND_N];
    }
    else if (num_red == 6)  // or
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_OR];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_OR_N];
    }
    else if (num_red == 7)  // neqv
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_NEQ];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_NEQ_N];
    }
    else if (num_red == 8)  // eqv
    {
        if (num_E == 1)
            retVal = red_kernel_func_names[red_EQ];
        else if (num_E > 1)
            retVal = red_kernel_func_names[red_EQ_N];
    }
    else if (num_red == 9)  // maxloc
    {
        if (num_E == 1)
        {
            if (num_IE >= 1)
                retVal = red_kernel_func_names[red_MAXL];
        }

    }
    else if (num_red == 10)  // minloc
    {
        if (num_E == 1)
        {
            if (num_IE >= 1)
                retVal = red_kernel_func_names[red_MINL];
        }
    }

    return retVal;
}

SgStatement *RedOp_Assign(SgSymbol *i_var, SgSymbol *s_block, SgExpression *ered, SgSymbol *d, int k)
{
    SgExpression *le = NULL, *re = NULL, *op1 = NULL, *op2 = NULL, *eind = NULL, *red_ind = NULL;
    int num_red;
    // <red_var>_block([ k,] i) = <red_op> (<red_var>_block([ k,] i), <red_var>_block([ k,] i + d))
    // k = LowerBound:UpperBound
    if (Rank(s_block) == 1)
    {
        red_ind = NULL; le = RedVar_Block_Ref(s_block, i_var);
    }
    else
    {
        red_ind = RedVarIndex(s_block, k); le = RedVar_Block_2D_Ref(s_block, i_var, red_ind);
    }
    num_red = RedFuncNumber(ered->lhs());
    if (num_red > 8)   // MAXLOC => 9,MINLOC =>10
        num_red -= 6; // MAX => 3,MIN =>4      
    op1 = &(le->copy());                   //RedVar_Block_Ref(s_block,i_var);

    eind = &(*new SgVarRefExp(i_var) + *new SgVarRefExp(d));

    op2 = red_ind ? new SgArrayRefExp(*s_block, *red_ind, *eind) : new SgArrayRefExp(*s_block, *eind);

    switch (num_red) {
    case(1) :   //sum
        re = &(*op1 + *op2);
        break;
    case(2) :   //product
        re = &(*op1 * *op2);
        break;
    case(3) :   //max
        re = MaxFunction(op1, op2);
        break;
    case(4) :   //min
        re = MinFunction(op1, op2);
        break;
    case(5) :   //and
        if (options.isOn(C_CUDA))
            re = new SgExpression(BITAND_OP, op1, op2, NULL);
        else
            re = new SgExpression(AND_OP, op1, op2, NULL);
        break;
    case(6) :   //or
        if (options.isOn(C_CUDA))
            re = new SgExpression(BITOR_OP, op1, op2, NULL);
        else
            re = new SgExpression(OR_OP, op1, op2, NULL);
        break;
    case(7) :   //neqv
        if (options.isOn(C_CUDA))
            re = new SgExpression(XOR_OP, op1, op2, NULL);
        else
            re = new SgExpression(NEQV_OP, op1, op2, NULL);
        break;
    case(8) :   //eqv
        if (options.isOn(C_CUDA))
            re = new SgUnaryExp(BIT_COMPLEMENT_OP, *new SgExpression(XOR_OP, op1, op2, NULL));
        else
            re = new SgExpression(EQV_OP, op1, op2, NULL);
        break;
    default:
        break;
    }
    return(AssignStatement(le, re));
}

SgStatement * GenRedOpAssignStatement(int num_red, SgExpression *op1, SgExpression *op2, SgExpression *le)
{
    SgExpression *re = NULL;
    switch (num_red) {
    case(1) :   //sum
        re = &(*op1 + *op2);
        break;
    case(2) :   //product
        re = &(*op1 * *op2);
        break;
    case(3) :   //max
        re = MaxFunction(op1, op2);
        break;
    case(4) :   //min
        re = MinFunction(op1, op2);
        break;
    case(5) :   //and
        re = new SgExpression(AND_OP, op1, op2, NULL);
        break;
    case(6) :   //or
        re = new SgExpression(OR_OP, op1, op2, NULL);
        break;
    case(7) :   //neqv
        re = new SgExpression(NEQV_OP, op1, op2, NULL);
        break;
    case(8) :   //eqv
        re = new SgExpression(EQV_OP, op1, op2, NULL);
        break;
    default:
        break;
    }
    return(new SgAssignStmt(*le, *re));
}

SgStatement *RedOp_If(SgSymbol *i_var, SgSymbol *s_block, SgExpression *ered, SgSymbol *d, int num)
{
    SgExpression *cond = NULL, *le = NULL, *re = NULL;
    SgSymbol *sf = NULL;
    SgStatement *ass = NULL, *if_st = NULL, *st = NULL;
    int num_red, i, ind;

    sf = RedVarFieldSymb(s_block);
    re = RedLocVar_Block_Ref(s_block, i_var, NULL, new SgVarRefExp((sf)));
    le = RedLocVar_Block_Ref(s_block, i_var, d, new SgVarRefExp((sf)));

    num_red = RedFuncNumber(ered->lhs());
    if (num_red == 9)   // MAXLOC => 9
        cond = &operator > (*le, *re);
    else if (num_red == 10) // MINLOC =>10 
        cond = &operator < (*le, *re);
    le = RedLocVar_Block_Ref(s_block, i_var, NULL, new SgVarRefExp((sf)));
    re = RedLocVar_Block_Ref(s_block, i_var, d, new SgVarRefExp((sf)));
    ass = AssignStatement(le, re);
    if_st = new SgIfStmt(*cond, *ass);
    st = ass;

    for (i = 0; i < num; i++)
    {
        ind = options.isOn(C_CUDA) ? i : i + 1;
        le = RedLocVar_Block_Ref(s_block, i_var, NULL, new SgArrayRefExp(*((SgFieldSymb *)sf)->nextField(), *new SgValueExp(ind)));
        re = RedLocVar_Block_Ref(s_block, i_var, d, new SgArrayRefExp(*((SgFieldSymb *)sf)->nextField(), *new SgValueExp(ind)));
        ass = AssignStatement(le, re);
        st->insertStmtAfter(*ass, *if_st);
        st = ass;
    }

    return(if_st);
}

SgExpression *RedVar_Block_Ref(SgSymbol *sblock, SgSymbol *sind)
{ // <red_var>_block(i)   
    //if(sblock->type()->baseType()->variant() != T_DERIVED_TYPE)

    return(new SgArrayRefExp(*sblock, *new SgVarRefExp(sind)));
}

SgExpression *RedVar_Block_2D_Ref(SgSymbol *sblock, SgSymbol *sind, SgExpression *redind)
{ // <red_var>_block(k,i)   if reduction variable is array

    SgExpression *eind;
    eind = new SgExprListExp(*redind);
    eind->setRhs(new SgExprListExp(*new SgVarRefExp(sind)));

    return(new SgArrayRefExp(*sblock, *eind));
}

SgExpression *RedLocVar_Block_Ref(SgSymbol *sblock, SgSymbol *sind, SgSymbol *d, SgExpression *field)
{ // <red_var>_<loc_var_>block(i+d)%<field_name>   or  <red_var>_<loc_var_>block(0)%<field_name>
    SgExpression *se, *rref;
    if (!d && !sind)  // index = 1
        se = new SgArrayRefExp(*sblock, *new SgValueExp(0));
    else if (!d)
        se = new SgArrayRefExp(*sblock, *new SgVarRefExp(sind));
    else
        se = new SgArrayRefExp(*sblock, *new SgVarRefExp(sind) + *new SgVarRefExp(d));
    rref = new SgExpression(RECORD_REF);

    NODE_OPERAND0(rref->thellnd) = se->thellnd;
    NODE_OPERAND1(rref->thellnd) = field->thellnd;
    NODE_TYPE(rref->thellnd) = field->type()->thetype;
    return(rref);
    //return(  new SgRecordRefExp(*new SgArrayRefExp(*sblock, *new SgVarRefExp(sind)),*field));
}

SgSymbol *RedVarFieldSymb(SgSymbol *s_block)
{
    return(FirstTypeField(s_block->type()->baseType()->symbol()->type()));

}

void Do_Assign_For_Loc_Arrays()
{
    reduction_operation_list *rl;
    int i;
    SgExpression *eind, *el;
    SgStatement *curst, *ass, *dost;

    if (!red_list) return;
    ass = NULL;
    curst = kernel_st;
    for (rl = red_struct_list; rl; rl = rl->next)
    {
        if (!rl->locvar && rl->redvar_size == 0)
            continue;
        if (rl->redvar_size > 0)
            for (i = 0, el = rl->value_arg; i < rl->redvar_size && el; i++, el = el->rhs())
            {
                eind = !options.isOn(C_CUDA) ? &(*new SgValueExp(i) + (*LowerBound(rl->redvar, 0))) : new SgValueExp(i);
                eind = Calculate(eind);
                //ass = new SgAssignStmt( *new SgArrayRefExp( *rl->redvar,*eind),   el->lhs()->copy() ); 
                ass = AssignStatement(new SgArrayRefExp(*rl->redvar, *eind), &(el->lhs()->copy()));
                curst->insertStmtAfter(*ass, *kernel_st);
                curst = ass;
            }

        if (rl->redvar_size < 0)
        {
            if (options.isOn(C_CUDA))
            {
                //XXX changed reduction scheme to atomic, Kolganov 06.02.2020
                //eind = LinearFormForRedArray(rl->redvar, SubscriptListOfRedArray(rl->redvar), rl);
                //ass = AssignStatement(new SgArrayRefExp(*rl->redvar, *eind), new SgArrayRefExp(*rl->red_init, *eind));
            }
            else
            {
                ass = AssignStatement(new SgArrayRefExp(*rl->redvar, *SubscriptListOfRedArray(rl->redvar)), new SgArrayRefExp(*rl->red_init, *SubscriptListOfRedArray(rl->redvar)));

                //XXX move this block to this condition, Kolganov 06.02.2020
                dost = doLoopNestForReductionArray(rl, ass);
                curst->insertStmtAfter(*dost, *kernel_st);
                curst = dost->lastNodeOfStmt();
            }
        }

        if (rl->locvar)
        {
            for (i = 0, el = rl->formal_arg; i < rl->number && el; i++, el = el->rhs())
            {
                if (isSgArrayType(rl->locvar->type()))
                {
                    if (options.isOn(C_CUDA)) // in C Language
                        eind = new SgValueExp(i);
                    else       // in Fortran Language
                        eind = Calculate(&(*new SgValueExp(i) + (*LowerBound(rl->locvar, 0))));
                    // ass = new SgAssignStmt( *new SgArrayRefExp( *rl->locvar,*eind), el->lhs()->copy() );
                    ass = AssignStatement(new SgArrayRefExp(*rl->locvar, *eind), &(el->lhs()->copy()));
                }
                else
                    //ass = new SgAssignStmt( *new SgVarRefExp( *rl->locvar), el->lhs()->copy() );
                    ass = AssignStatement(new SgVarRefExp(*rl->locvar), &(el->lhs()->copy()));
                curst->insertStmtAfter(*ass, *kernel_st);
                curst = ass;
            }
        }
    }
    if (ass)
        kernel_st->lexNext()->addComment(CommentLine("Fill local variable with passed values"));
}

SgStatement *doLoopNestForReductionArray(reduction_operation_list *rl, SgStatement *ass)
{
    SgStatement *dost;

    int rank, i;
    // creating loop nest
    //  do kkN = 1,dimSizeN
    //     . . .
    //  do kk1 = 1,dimSize1
    //    <ass>
    //  enddo
    //     . . .
    //  enddo    
    rank = Rank(rl->redvar);
    dost = ass;
    for (i = 1; i <= rank; i++)
    {
        if (options.isOn(C_CUDA))
            dost = new SgForStmt(&SgAssignOp(*new SgVarRefExp(IndexLoopVar(i)), *new SgValueExp(0)),
            &(*new SgVarRefExp(IndexLoopVar(i)) < *RedVarUpperBound(rl->dimSize_arg, i)),
            &SgAssignOp(*new SgVarRefExp(IndexLoopVar(i)), *new SgVarRefExp(IndexLoopVar(i)) + *new SgValueExp(1)), dost);
        else
            dost = new SgForStmt(IndexLoopVar(i), new SgValueExp(1), RedVarUpperBound(rl->dimSize_arg, i), NULL, dost);
    }

    return(dost);
}

SgExpression *SubscriptListOfRedArray(SgSymbol *ar)
{
    int rank, j;
    SgExpression *list, *el;
    rank = Rank(ar); j = 1;
    list = el = &kernel_index_var_list->copy();
    while (j != rank)
    {
        el = el->rhs(); j++;
    }
    el->setRhs(NULL);
    return(list);
}

SgSymbol *IndexLoopVar(int i)
{
    int j = 1;
    SgExpression *ell = kernel_index_var_list;

    while (j != i)
    {
        ell = ell->rhs(); j++;
    }
    return(ell->lhs()->symbol());
}


SgExpression *RedVarUpperBound(SgExpression *el, int i)
{
    int j = 1;
    SgExpression *ell = el;

    while (j != i)
    {
        ell = ell->rhs(); j++;
    }
    return(&ell->lhs()->copy());
}


SgExpression *LocVarIndex(SgSymbol *sl, int i)
{ // i = 1,...
    int ind;
    SgExpression *ec;
    if (!isSgArrayType(sl->type()))
        return(new SgValueExp(i));
    ec = Calculate(LowerBound(sl, 0));
    if (!ec->isInteger())
    {
        Error("Can not calculate lower bound of array %s", sl->identifier(), 594, dvm_parallel_dir);
        return(new SgValueExp(i));
    }
    ind = options.isOn(C_CUDA) ? i - 1 : i - 1 + (ec->valueInteger());
    return(new SgValueExp(ind));

}


SgExpression *RedVarIndex(SgSymbol *sl, int i)
{// i=0,...
    SgExpression *ec;
    int ind;
    ec = Calculate(LowerBound(sl, 0));
    if (!ec->isInteger())
    {
        Error("Can not calculate lower bound of array %s", sl->identifier(), 594, dvm_parallel_dir);
        return(new SgValueExp(i));
    }
    ind = options.isOn(C_CUDA) ? i : i + (ec->valueInteger());
    return(new SgValueExp(ind));

}
/*
SgExpression *RedGridIndex(SgSymbol *sl,int i)
{ SgExpression *eind;
if(Rank(sl)==0)
eind = &(*new SgRecordRefExp(*s_blockidx,"x") - *new SgValueExp(1));
else
eind = new
}
*/

SgExpression *LinearFormForRedArray(SgSymbol *ar, SgExpression *el, reduction_operation_list *rsl)
{
    int i, n;
    SgExpression *elin, *e;
    // el - subscript list (I1,I2,...In), n - rank of reduction array 

    // generating                         
    //                n   
    //         I1 + SUMMA(DimSize(k-1) * Ik)
    //               k=2
  
    n = Rank(rsl->redvar);
    if (!el)     // there aren't any subscripts
        return(new SgValueExp(0));

    if (rsl->dimSize_arg == NULL)
        return(el);

    elin = ToInt(el->lhs());
    for (e = el->rhs(), i = 1; e; e = e->rhs(), i++)
        elin = &(*elin + (*ToInt(e->lhs()) * *coefProd(i, rsl->dimSize_arg))); //  + Ik * DimSize(k-1)

    //XXX changed reduction scheme to atomic, Kolganov 19.03.2020
    /*if (rsl->array_red_size <= 0)
        elin = &(*elin * *BlockDimsProduct());*/
    return(new SgExprListExp(*elin));
}

SgExpression *coefProd(int i, SgExpression *ec)
{
    SgExpression *e, *coef;
    int j;
    e = &(ec->lhs()->copy());
    for (coef = ec->rhs(), j = 2; coef && j <= i; coef = coef->rhs(), j++)
        e = &(*e * coef->lhs()->copy());
    return(e);
}

SgExpression *BlockDimsProduct()
{
    return &(*new SgRecordRefExp(*s_blockdim, "x") * *new SgRecordRefExp(*s_blockdim, "y") * *new SgRecordRefExp(*s_blockdim, "z"));
}

SgExpression *LowerShiftForArrays (SgSymbol *ar, int i) 
{
    SgExpression *e = isConstantBound(ar, i, 1);
    if(!e)
        e = &(((SgExprListExp *)red_struct_list->lowBound_arg)->elem(i)->copy());
    return e; 
}
 
SgExpression *UpperShiftForArrays (SgSymbol *ar, int i) 
{
    SgExpression *e = isConstantBound(ar, i, 0);
    if(!e)
        e = new SgValueExp(1);
    return e; 
}
   
void CompleteStructuresForReductionInKernel()
{
    reduction_operation_list *rl;
    int max_rank = 0;
    int r;
    s_overall_blocks = NULL;
    //!printf("redstruct _0\n");
    for (rl = red_struct_list; rl; rl = rl->next)
    {
        rl->value_arg = CreateFormalLocationList(rl->redvar, rl->redvar_size);
        rl->formal_arg = CreateFormalLocationList(rl->locvar, rl->number);
        //!printf("redstruct _1\n");
        //rl->actual_arg = rl->locvar ? CreateActualLocationList(rl->locvar,rl->number) : NULL;

        if (!s_overall_blocks && rl->redvar_size != 0)
            s_overall_blocks = OverallBlocksSymbol();
        if (rl->redvar_size < 0)
        {
            rl->dimSize_arg  = CreateFormalDimSizeList(rl->redvar);
            rl->lowBound_arg = CreateFormalLowBoundList(rl->redvar);
            //XXX changed reduction scheme to atomic, Kolganov 06.02.2020
            //rl->red_init = RedInitValSymbolInKernel(rl->redvar, rl->dimSize_arg); // after CreateFormalDimSizeList()
            rl->red_init = rl->redvar;
        }
        else
        {
            rl->dimSize_arg = NULL;
            rl->lowBound_arg = NULL;
            rl->red_init = NULL;
        }
        rl->red_grid = RedGridSymbolInKernel(rl->redvar, rl->redvar_size, rl->dimSize_arg, 1); // after CreateFormalDimSizeList()
        rl->loc_grid = rl->locvar ? RedGridSymbolInKernel(rl->locvar, rl->number, NULL, 0) : NULL;
        //!printf("redstruct _2\n");
        r = Rank(rl->redvar);
        max_rank = max_rank < r ? r : max_rank;
    }

    kernel_index_var_list = CreateIndexVarList(max_rank);
}

SgExpression *CreateIndexVarList(int N)
{
    int i;
    SgExprListExp *list = NULL;
    SgExprListExp *el;
    if (N == 0) return(NULL);
    for (i = N; i; i--)
    {
        el = new SgExprListExp(*new SgVarRefExp(IndexSymbolForRedVarInKernel(i)));
        el->setRhs(list);
        list = el;
    }
    return(list);
}

SgExpression *CreateFormalLocationList(SgSymbol *locvar, int numb)
{
    SgExprListExp *sl, *sll;
    int i;
    if (!locvar || numb <= 0) return(NULL);
    sl = NULL;
    for (i = numb; i; i--)
    {
        sll = new SgExprListExp(*new SgVarRefExp(FormalLocationSymbol(locvar, i)));
        sll->setRhs(sl);
        sl = sll;
    }

    return(sl);
}

SgExpression *CreateFormalDimSizeList(SgSymbol *var)
{
    SgExprListExp *sl, *sll;
    int i;
    sl = NULL;
    for (i = Rank(var); i; i--)
    {
        sll = new SgExprListExp(*new SgVarRefExp(FormalDimSizeSymbol(var, i)));
        sll->setRhs(sl);
        sl = sll;
    }
    return(sl);
}

SgExpression *CreateFormalLowBoundList(SgSymbol *var)
{
    SgExprListExp *sl, *sll;
    int i;
    sl = NULL;
    for (i = Rank(var); i; i--)
    {
        sll = new SgExprListExp(*new SgVarRefExp(FormalLowBoundSymbol(var, i)));
        sll->setRhs(sl);
        sl = sll;
    }
    return(sl);
}

char *LoopKernelComment()
{
    char *cmnt = new char[100];
    if (options.isOn(C_CUDA)) // in C Language
        sprintf(cmnt, "//--------------------- Kernel for loop on line %d ---------------------\n", first_do_par->lineNumber());
    else   // in Fortran Language
        sprintf(cmnt, "!----------------------- Kernel for loop on line %d -----------------------\n\n", first_do_par->lineNumber());
    return(cmnt);
}

char *SequenceKernelComment(int lineno)
{
    char *cmnt = new char[150];
    if (options.isOn(C_CUDA)) // in C Language
        sprintf(cmnt, "//--------------------- Kernel for sequence of statements on line %d ---------------------\n", lineno);
    else   // in Fortran Language
        sprintf(cmnt, "!----------------------- Kernel for sequence of statements on line %d -----------------------\n\n", lineno);
    return(cmnt);
}

void SymbolChange_InBlock(SgSymbol *snew, SgSymbol *sold, SgStatement *first_st, SgStatement *last_st)
{
    SgStatement *st;
    if (!snew || !sold)  return;
    for (st = first_st; st != last_st; st = st->lexNext())
    {
        if (st->symbol() && st->symbol() == sold)
            st->setSymbol(*snew);
        //printf("----%d\n", st->lineNumber());
        SymbolChange_InExpr(snew, sold, st->expr(0));
        SymbolChange_InExpr(snew, sold, st->expr(1));
        SymbolChange_InExpr(snew, sold, st->expr(2));
    }
}

void SymbolChange_InExpr(SgSymbol *snew, SgSymbol *sold, SgExpression *e)
{
    if (!e) return;
    if (isSgVarRefExp(e) || isSgArrayRefExp(e) || e->variant() == CONST_REF)
    {
        if (e->symbol() == sold)
            e->setSymbol(*snew);
        //printf("%s %d   %s %d \n",e->symbol()->identifier(),e->symbol()->id(),sold->identifier(),sold->id());
        return;
    }
    SymbolChange_InExpr(snew, sold, e->lhs());
    SymbolChange_InExpr(snew, sold, e->rhs());
}

void SaveLineNumbers(SgStatement *stat_copy)
{
    SgStatement *stmt, *dost, *st;

    dost = DoStmt(first_do_par, ParLoopRank());


    for (stmt = stat_copy, st = dost->lexNext(); stmt; stmt = stmt->lexNext(), st = st->lexNext())
    {                                           //printf("----loop %d\n",st->lineNumber());
        BIF_LINE(stmt->thebif) = st->lineNumber();
    }
}
/***************************************************************************************/
/*ACC*/
/*   Creating  C-Cuda Kernel Function                                                  */
/*   and Inserting New Statements                                                      */
/***************************************************************************************/
SgStatement *Create_C_Kernel_Function(SgSymbol *sF)

// create kernel for loop in C-Cuda language
{
    SgStatement *st_hedr, *st_end;
    SgExpression *fe;

    // create fuction header
    st_hedr = new SgStatement(FUNC_HEDR);
    st_hedr->setSymbol(*sF);
    fe = new SgFunctionRefExp(*sF);
    fe->setSymbol(*sF);
    st_hedr->setExpression(0, *fe);
    st_hedr->addDeclSpec(BIT_CUDA_GLOBAL);

    // create end of function 
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*sF);

    // inserting      
    mod_gpu_end->insertStmtBefore(*st_hedr, *mod_gpu);
    st_hedr->insertStmtAfter(*st_end, *st_hedr);

    cur_in_mod = st_end;
    return(st_hedr);
}

/***************************************************************************************/
/*ACC*/
/*   Creating  C Program Unit                                                          */
/*   and Inserting New Statements                                                      */
/* (C Language, adapter procedure, .cu file)                                           */
/***************************************************************************************/
SgType *Cuda_Index_Type()
{
    SgSymbol *st = new SgSymbol(TYPE_NAME, "CudaIndexType", options.isOn(C_CUDA) ? *block_C_Cuda : *block_C);
    SgType *t_dsc;
    if (undefined_Tcuda)
        t_dsc = new SgDescriptType(*C_Derived_Type(s_DvmType), BIT_TYPEDEF); //BIT_TYPEDEF | BIT_LONG);  
    else
        t_dsc = new SgDescriptType(*SgTypeInt(), BIT_TYPEDEF);

    st->setType(t_dsc);
    s_CudaIndexType = st;

    //SgType *td = new SgType(T_DERIVED_TYPE); 
    //TYPE_SYMB_DERIVE(td->thetype) = sdim3->thesymb;
    //TYPE_SYMB(td->thetype) = sdim3->thesymb;
    //define TYPE_LONG_SHORT(NODE) ((NODE)->entry.descriptive.long_short_flag)
    //define TYPE_MODE_FLAG(NODE) ((NODE)->entry.descriptive.mod_flag)
    //define TYPE_STORAGE_FLAG(NODE) ((NODE)->entry.descriptive.storage_flag)
    //define TYPE_ACCESS_FLAG(NODE) ((NODE)->entry.descriptive.access_flag)

    return(t_dsc);
}

SgType *Dvmh_Type()
{
    SgSymbol *st = new SgSymbol(TYPE_NAME, "DvmType", options.isOn(C_CUDA) ? *block_C_Cuda : *block_C);

    SgType *t_dsc = new SgDescriptType(*C_BaseDvmType(), BIT_TYPEDEF | BIT_LONG);

    st->setType(t_dsc);
    s_DvmType = st;

    return(t_dsc);
}

SgType *DvmhLoopRef_Type()
{    // DvmhLoopRef => DvmType  in RTS  05.11.16
    SgSymbol *st = new SgSymbol(TYPE_NAME, "DvmType", options.isOn(C_CUDA) ? *block_C_Cuda : *block_C);

    SgType *t_dsc = new SgDescriptType(*C_Derived_Type(s_DvmType), BIT_TYPEDEF);
    //new SgDescriptType(*C_BaseDvmType(), BIT_TYPEDEF | BIT_LONG);

    st->setType(t_dsc);
    s_DvmhLoopRef = st;

    //SgType *td = new SgType(T_DERIVED_TYPE); 
    //TYPE_SYMB_DERIVE(td->thetype) = sdim3->thesymb;
    //TYPE_SYMB(td->thetype) = sdim3->thesymb;
    //define TYPE_LONG_SHORT(NODE) ((NODE)->entry.descriptive.long_short_flag)
    //define TYPE_MODE_FLAG(NODE) ((NODE)->entry.descriptive.mod_flag)
    //define TYPE_STORAGE_FLAG(NODE) ((NODE)->entry.descriptive.storage_flag)
    //define TYPE_ACCESS_FLAG(NODE) ((NODE)->entry.descriptive.access_flag)

    return(t_dsc);
}

SgType *CudaOffsetTypeRef_Type()
{
    SgSymbol *st = new SgSymbol(TYPE_NAME, "CudaOffsetTypeRef", options.isOn(C_CUDA) ? *block_C_Cuda : *block_C);

    SgType *t_dsc = new SgDescriptType(*C_Derived_Type(s_DvmType), BIT_TYPEDEF);

    st->setType(t_dsc);
    s_CudaOffsetTypeRef = st;

    return(t_dsc);
}

SgType *C_Derived_Type(SgSymbol *styp)
{
    return(new SgDerivedType(*styp));
}
SgType * C_VoidType()
{
    return(new SgType(T_VOID));
}

SgType * C_LongType()
{
    return(new SgDescriptType(*SgTypeInt(), BIT_LONG));
}

SgType * C_LongLongType()
{
    return(new SgDescriptType(*new SgType(T_LONG), BIT_LONG));
}

SgType * C_DvmType()
{
    if (!type_DvmType)
        type_DvmType = C_Derived_Type(s_DvmType);
    return(type_DvmType);

}

SgType * C_BaseDvmType()
{
    if (bind == 0 && len_DvmType == 8)  // size of long == 4
        return(new SgType(T_LONG));
    else
        return(SgTypeInt());
}

SgType * C_CudaIndexType()
{
    if (!type_CudaIndexType)
        type_CudaIndexType = C_Derived_Type(s_CudaIndexType);
    return(type_CudaIndexType);

}
/*
SgSymbol *CudaIndexConst(int iconst)
{
char name[10];
if(iconst == rt_INT)
name = "rt_INT";
else if(iconst == rt_LONG)
name = "rt_LONG";
else
name = "rt_LLONG";
return ( new SgVariableSymb(name,SgTypeInt(),block_C) );
}
*/

SgSymbol *CudaIndexConst()
{
    const char *name;
    int len;
    if (undefined_Tcuda)
        len = TypeSize(FortranDvmType());
    else
        len = 4;
    if (len == 4)
        name = "rt_INT";
    else if (len == 8)
        name = "rt_LONG";
    else
        name = "rt_LLONG";

    return (new SgVariableSymb(name, SgTypeInt(), block_C));

}

SgType *C_PointerType(SgType *type)
{
    return(new SgPointerType(type));
}


SgType *C_ReferenceType(SgType *type)
{
    return(new SgReferenceType(*type));
}

void CreateComplexTypeSymbols(SgStatement *st_bl)
{
    s_cmplx = new SgSymbol(TYPE_NAME, "cmplx2", *st_bl);
    s_dcmplx = new SgSymbol(TYPE_NAME, "dcmplx2", *st_bl);
}

SgType *C_Type(SgType *type)
{
    SgType *tp;
    int len;
    tp = isSgArrayType(type) ? type->baseType() : type;
    len = TypeSize(tp);
    switch (tp->variant()) {

    case T_INT:    //if(IS_INTRINSIC_TYPE(tp))
        //   return(tp);
        if (len == 4)
        {
            if (bind == 1)
                return(SgTypeInt());
            else                             //if (bind==0)
                return C_LongType();
        }
        else if (len == 8)
        {
            if (bind == 1)
                return C_LongType();
            else                             // if (bind==0)
                return C_LongLongType();
        }
        else if (len == 2)
            return(new SgDescriptType(*SgTypeInt(), BIT_SHORT));
        else if (len == 1)
            return(SgTypeChar());
        break;


    case T_FLOAT:  if (IS_INTRINSIC_TYPE(tp))
        return(tp);
                   else if (len == 8)
                       return(SgTypeDouble());
                   else if (len == 4)
                       return(SgTypeFloat());
                   break;

    case T_BOOL:
        if (len == 8)
        {
            if (bind == 1)
                return C_LongType();
            else                             // if (bind==0)
                return C_LongLongType();
        }
        else if (len == 4)
        {
            if (bind == 1)
                return(SgTypeInt());
            else                             //if (bind==0)
                return C_LongType();
        }
        else if (len == 2)
            return(new SgDescriptType(*SgTypeInt(), BIT_SHORT));
        else if (len == 1)
            return(SgTypeChar());
        break;
    case T_DOUBLE:   return (tp);
    case T_COMPLEX:  return(C_Derived_Type(s_cmplx));
    case T_DCOMPLEX: return(C_Derived_Type(s_dcmplx));
    case T_DERIVED_TYPE: 
        if (tp->symbol()->identifier() != std::string("uint4")) // for __dvmh_rand_state
            err("Illegal type of used or reduction variable", 499, first_do_par);
        return(tp);    //return (SgTypeInt());
    case T_CHAR:
    case T_STRING:   
        if (len == 1)
            return (SgTypeChar());
        break;
    default: 
        err("Illegal type of used or reduction variable", 499, first_do_par);
        return (SgTypeInt());
    }

    err("Illegal type of used or reduction variable", 499, first_do_par);
    return (SgTypeInt());
}

SgSymbol *AdapterSymbol(SgStatement *st_do)
{
    SgSymbol *s, *sc;
    char *aname, *namef;

    aname = (char *)malloc((unsigned)(strlen(st_do->fileName()) + 30));
    if (inparloop)
        sprintf(aname, "%s_%s_%d_cuda_", "loop", filename_short(st_do), st_do->lineNumber());
    else
        sprintf(aname, "%s_%s_%d_cuda_", "sequence", filename_short(st_do), st_do->lineNumber());
    s = new SgSymbol(FUNCTION_NAME, aname, *C_VoidType(), *block_C); //*current_file->firstStatement()); 

    namef = (char *)malloc((unsigned)strlen(aname) + 1);
    //strncpy(namef,aname,strlen(aname)-1);
    strcpy(namef, aname);
    namef[strlen(aname) - 1] = '\0';
    sc = new SgSymbol(PROCEDURE_NAME, namef, *current_file->firstStatement());
    if (cur_region && cur_region->targets & CUDA_DEVICE)
        acc_func_list = AddToSymbList(acc_func_list, sc);

    return(s);
}

void ChangeAdapterName(SgSymbol *s)
//deleting last symbol "_" 
{
    char *name;
    name = s->identifier();
    name[strlen(name) - 1] = '\0';
}

/*--------------------------*/

SgSymbol *isSameRedVar(char *name)
{
    reduction_operation_list *rl;

    for (rl = red_struct_list; rl; rl = rl->next)
    {
        if (rl->redvar && !strcmp(rl->redvar->identifier(), name))
            return(rl->redvar);
        if (rl->locvar && !strcmp(rl->locvar->identifier(), name))
            return(rl->locvar);
    }
    return(NULL);
}

SgSymbol *isSameRedVar_c(const char *name)
{
    reduction_operation_list *rl;

    for (rl = red_struct_list; rl; rl = rl->next)
    {
        if (rl->redvar && !strcmp(rl->redvar->identifier(), name))
            return(rl->redvar);
        if (rl->locvar && !strcmp(rl->locvar->identifier(), name))
            return(rl->locvar);
    }
    return(NULL);
}

SgSymbol *isSameUsedVar(char *name)
{
    SgExpression *el;
    SgSymbol *s;

    for (el = uses_list; el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSameUsedVar_c(const char *name)
{
    SgExpression *el;
    SgSymbol *s;

    for (el = uses_list; el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSamePrivateVar(char *name)
{
    SgExpression *el;
    SgSymbol *s;

    for (el = private_list; el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSamePrivateVar_c(const char *name)
{
    SgExpression *el;
    SgSymbol *s;

    for (el = private_list; el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSameIndexVar(char *name)
{
    SgExpression *el;
    SgSymbol *s;
    if (!dvm_parallel_dir)
        return(NULL);

    for (el = dvm_parallel_dir->expr(2); el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSameIndexVar_c(const char *name)
{
    SgExpression *el;
    SgSymbol *s;
    if (!dvm_parallel_dir)
        return(NULL);

    for (el = dvm_parallel_dir->expr(2); el; el = el->rhs())
    {
        s = el->lhs()->symbol();
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSameArray(char *name)
{
    symb_list *sl;
    SgSymbol *s;

    for (sl = acc_array_list; sl; sl = sl->next)
    {
        s = sl->symb;
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSameArray_c(const char *name)
{
    symb_list *sl;
    SgSymbol *s;

    for (sl = acc_array_list; sl; sl = sl->next)
    {
        s = sl->symb;
        if (s && !strcmp(s->identifier(), name))
            return(s);
    }
    return(NULL);
}

SgSymbol *isSameNameInLoop(char *name)
{
    SgSymbol *s;
    s = isSameUsedVar(name);
    if (s) return(s);
    s = isSameRedVar(name);
    if (s) return(s);
    s = isSameArray(name);
    if (s) return(s);
    s = isSamePrivateVar(name);
    if (s) return(s);
    s = isSameIndexVar(name);
    return(s);
}
SgSymbol *isSameNameInLoop_c(const char *name)
{
    SgSymbol *s;
    s = isSameUsedVar_c(name);
    if (s) return(s);
    s = isSameRedVar_c(name);
    if (s) return(s);
    s = isSameArray_c(name);
    if (s) return(s);
    s = isSamePrivateVar_c(name);
    if (s) return(s);
    s = isSameIndexVar_c(name);
    return(s);
}


char *TestAndCorrectName(char *name)
{
    SgSymbol *s;

    while ((s = isSameNameInLoop(name)))
    {
        name = (char *)malloc((unsigned)(strlen(name) + 2));
        sprintf(name, "%s_", s->identifier());
    }
    return(name);
}

char *TestAndCorrectName(const char *name)
{
    SgSymbol *s = NULL;
    char *ret = new char[strlen(name) + 1];
    strcpy(ret,name);
    while ((s = isSameNameInLoop_c(ret)))
    {
        ret = (char *)malloc((unsigned)(strlen(name) + 2));
        sprintf(ret, "%s_", s->identifier());
    }
    return ret;
}

/*-------------------------------*/

char *GpuHeaderName(SgSymbol *s)
{
    char *name;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 3));
    sprintf(name, "d_%s", s->identifier());
    return(TestAndCorrectName(name));
}

SgSymbol *GpuHeaderSymbolInAdapter(SgSymbol *ar, SgStatement *st_hedr)
{
    SgArrayType *typearray = new SgArrayType(*C_DvmType());
    typearray->addRange(*new SgValueExp(Rank(ar) + DELTA));
    return(new SgSymbol(VARIABLE_NAME, GpuHeaderName(ar), *typearray, *st_hedr));
}

SgSymbol *GpuBaseSymbolInAdapter(SgSymbol *ar, SgStatement *st_hedr)
{
    char *name;
    name = (char *)malloc((unsigned)(strlen(ar->identifier()) + 6));
    sprintf(name, "%s_base", ar->identifier());
    name = TestAndCorrectName(name);
    return(new SgSymbol(VARIABLE_NAME, name, *C_PointerType(C_VoidType()), *st_hedr));
}

SgSymbol *GpuScalarAdrSymbolInAdapter(SgSymbol *s, SgStatement *st_hedr)
{
    char *name;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 5));
    sprintf(name, "%s_dev", s->identifier());
    name = TestAndCorrectName(name);
    return(new SgSymbol(VARIABLE_NAME, name, *C_PointerType(C_VoidType()), *st_hedr));
}


SgSymbol *GreedSymbolForRedInAdapter(SgSymbol *s, SgStatement *st_hedr)
{
    char *name;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 6));
    sprintf(name, "%s_grid", s->identifier());
    name = TestAndCorrectName(name);
    return(new SgSymbol(VARIABLE_NAME, name, *C_PointerType(C_VoidType()), *st_hedr));
}

SgSymbol *InitValSymbolForRedInAdapter(SgSymbol *s, SgStatement *st_hedr)
{
    char *name;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 6));
    sprintf(name, "%s_init", s->identifier());
    name = TestAndCorrectName(name);
    return(new SgSymbol(VARIABLE_NAME, name, *C_PointerType(C_VoidType()), *st_hedr));
}

SgSymbol *DeviceNumSymbol(SgStatement *st_hedr)
{
    char *name;
    name = TestAndCorrectName("device_num");
    return(new SgSymbol(VARIABLE_NAME, name, *C_DvmType(), *st_hedr));
}

SgSymbol *doDeviceNumVar(SgStatement *st_hedr, SgStatement *st_exec, SgSymbol *s_dev_num, SgSymbol *s_loop_ref)
{
    SgStatement *ass;
    SgExpression *le;
    if (s_dev_num)   return(s_dev_num);

    s_dev_num = DeviceNumSymbol(st_hedr);

    st_exec->insertStmtBefore(*makeSymbolDeclaration(s_dev_num), *st_hedr);
    le = new SgVarRefExp(s_dev_num);
    ass = AssignStatement(le, GetDeviceNum(s_loop_ref));
    st_exec->insertStmtBefore(*ass, *st_hedr);
    ass->addComment("// Get device number");

    return(s_dev_num);
}

char * DimSizeName(SgSymbol *s, int i)
{
    char *name;
    name = (char *)malloc((unsigned)(strlen(s->identifier()) + 10));
    sprintf(name, "dim%d_%s", i, s->identifier());
    name = TestAndCorrectName(name);
    return(name);
}

void Create_C_extern_block()
{
    SgStatement *fileHeaderSt;
    SgStatement *st_mod, *st_end;

    fileHeaderSt = current_file->firstStatement();
    if (block_C)
        return;
    //mod_gpu_symb = GPUModuleSymb(fileHeaderSt);

    if (options.isOn(C_CUDA))
    {
        st_mod = new SgStatement(MODULE_STMT);
        st_end = new SgStatement(CONTROL_END);
        fileHeaderSt->insertStmtAfter(*st_mod, *fileHeaderSt);
        st_mod->insertStmtAfter(*st_end, *st_mod);
        block_C_Cuda = st_mod;
        //Typedef_Stmts(st_end);   //10.12.13 
        TypeSymbols(st_end);
        if(INTERFACE_RTS2)
            st_mod->addComment(IncludeComment("<dvmhlib2.h>"));      
        st_mod->addComment(IncludeComment("<dvmhlib_cuda.h>\n#define dcmplx2 Complex<double>\n#define cmplx2 Complex<float>"));
        st_mod->addComment(CudaIndexTypeComment());
    }

    st_mod = new SgStatement(MODULE_STMT);
    //st_mod->setSymbol(*mod_gpu_symb);
    st_end = new SgStatement(CONTROL_END);
    //st_end->setSymbol(*mod_gpu_symb);
    fileHeaderSt->insertStmtAfter(*st_mod, *fileHeaderSt);
    st_mod->insertStmtAfter(*st_end, *st_mod);

    block_C = st_mod;
    cur_in_block = st_mod;
    end_block = st_end;
    if (!options.isOn(C_CUDA))        // for Fortran-Cuda
    {      //Typedef_Stmts(end_block); //10.12.13
        TypeSymbols(end_block);
        block_C->addComment(IncludeComment("<dvmhlib_cuda.h>"));
        if(INTERFACE_RTS2)
            block_C->addComment(IncludeComment("<dvmhlib2.h>"));
        block_C->addComment(CudaIndexTypeComment());
    }
    block_C->addComment("#ifdef _MS_F_\n");

    //Prototypes();   //10.12.13
    //cur_in_block = Create_Init_Cuda_Function(); 
    //cur_in_block = cur_in_block->lexNext();

    cur_in_block = Create_Empty_Stat(); // empty line

    CreateComplexTypeSymbols(options.isOn(C_CUDA) ? block_C_Cuda : block_C);

    return;
}

void Create_info_block()
{
    SgStatement *fileHeaderSt;
    SgStatement *st_mod, *st_end;

    fileHeaderSt = current_file->firstStatement();
    if (info_block)
        return;

    st_mod = new SgStatement(MODULE_STMT);
    st_end = new SgStatement(CONTROL_END);
    fileHeaderSt->insertStmtAfter(*st_mod, *fileHeaderSt);
    st_mod->insertStmtAfter(*st_end, *st_mod);
    info_block = st_mod;
    end_info_block = st_end;
    //info_block->insertStmtAfter(*(s_DvmType->makeVarDeclStmt()),*info_block); //10.12.13
    info_block->addComment(IncludeComment("<dvmhlib.h>"));
    return;
}

void TypeSymbols(SgStatement *end_bl)
{
    Dvmh_Type();
    Cuda_Index_Type();
    DvmhLoopRef_Type();
    CudaOffsetTypeRef_Type();
    s_cudaStream = new SgSymbol(TYPE_NAME, "cudaStream_t", *end_bl);
}

void Typedef_Stmts(SgStatement *end_bl)
{

    Dvmh_Type();
    Cuda_Index_Type();
    DvmhLoopRef_Type();
    CudaOffsetTypeRef_Type();

    /*  10.12.13
     st = s_DvmType->makeVarDeclStmt();
     end_bl-> insertStmtBefore(*st,*end_bl->controlParent());
     st = s_CudaIndexType->makeVarDeclStmt();
     end_bl-> insertStmtBefore(*st,*end_bl->controlParent());
     st = s_DvmhLoopRef->makeVarDeclStmt();
     end_bl-> insertStmtBefore(*st,*end_bl->controlParent());
     st = s_CudaOffsetTypeRef->makeVarDeclStmt();
     end_bl-> insertStmtBefore(*st,*end_bl->controlParent());
     */
}

void Prototypes()
{
    SgSymbol *sf, *sarg;
    SgStatement *st;
    SgExpression *fref, *ae, *el, *arg_list, *devref, *dvmdesc, *dvmHdesc, *hloop, *rednum, *redNumRef, *base, *outThreads, *outStream;
    SgType *typ, *typ1;
    SgArrayType *typearray;
    SgValueExp M0(0);
    // generating prototypes:

    //
    //void *dvmh_get_natural_base_(DvmType *deviceRef, DvmType dvmDesc[]);

    sf = fdvm[GET_BASE];
    sf->setType(*C_PointerType(C_VoidType()));
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    fref->setType(*C_PointerType(C_VoidType()));
    //fref = new SgPointerDerefExp(*fref);
    st = new SgStatement(VAR_DECL);
    //st=sf->makeVarDeclStmt();
    st->setExpression(0, *new SgExprListExp(*new SgPointerDerefExp(*fref)));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----*/
    sarg = new SgSymbol(VARIABLE_NAME, "deviceRef", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    devref = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*devref);

    typearray = new SgArrayType(*C_DvmType());
    typearray->addRange(M0);   // addDimension(NULL);
    sarg = new SgSymbol(VARIABLE_NAME, "dvmDesc", *typearray, *block_C);
    ae = new SgArrayRefExp(*sarg);
    ae->setType(*typearray);
    el = new SgExpression(EXPR_LIST);
    el->setLhs(NULL);
    ae->setLhs(*el);
    dvmdesc = ae;
    arg_list->setRhs(*new SgExprListExp(*ae));

    fref->setLhs(arg_list);

    //
    //void *dvmh_get_device_adr_(DvmType *deviceRef, void *variable);

    sf = fdvm[GET_DEVICE_ADDR];
    sf->setType(*C_PointerType(C_VoidType()));
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    fref->setType(*C_PointerType(C_VoidType()));
    //fref = new SgPointerDerefExp(*fref);
    st = new SgStatement(VAR_DECL);
    //st=sf->makeVarDeclStmt();
    st->setExpression(0, *new SgExprListExp(*new SgPointerDerefExp(*fref)));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----*/
    sarg = new SgSymbol(VARIABLE_NAME, "deviceRef", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    devref = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*devref);

    sarg = new SgSymbol(VARIABLE_NAME, "variable", *C_VoidType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_VoidType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

    fref->setLhs(arg_list);

    //
    // void dvmh_fill_header_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[]);

    sf = fdvm[FILL_HEADER];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = sf->makeVarDeclStmt();
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(devref->copy());
    fref->setLhs(arg_list);

    sarg = new SgSymbol(VARIABLE_NAME, "base", *C_VoidType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_VoidType()));
    ae = base = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    arg_list->setRhs(*new SgExprListExp(dvmdesc->copy()));
    arg_list = arg_list->rhs();

    typearray = new SgArrayType(*C_DvmType());
    typearray->addRange(M0);
    sarg = new SgSymbol(VARIABLE_NAME, "dvmhDesc", *typearray, *block_C);
    ae = dvmHdesc = new SgArrayRefExp(*sarg);
    ae->setType(*typearray);
    el = new SgExpression(EXPR_LIST);
    el->setLhs(NULL);
    ae->setLhs(*el);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    // void dvmh_fill_header_ex_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[], DvmType *outTypeOfTransformation, DvmType extendedParams[]);

    sf = fdvm[FILL_HEADER_EX];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = sf->makeVarDeclStmt();
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(devref->copy());
    fref->setLhs(arg_list);

    sarg = new SgSymbol(VARIABLE_NAME, "base", *C_VoidType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_VoidType()));
    ae = base = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    arg_list->setRhs(*new SgExprListExp(dvmdesc->copy()));
    arg_list = arg_list->rhs();
    arg_list->setRhs(*new SgExprListExp(dvmHdesc->copy()));
    arg_list = arg_list->rhs();

    sarg = new SgSymbol(VARIABLE_NAME, "outTypeOfTransformation", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "extendedParams", *dvmHdesc->symbol()->type(), *block_C);
    ae = &(dvmHdesc->copy());
    ae->setSymbol(*sarg);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    // void *dvmh_apply_offset(DvmType dvmDesc[], void *base, DvmType dvmhDesc[]);

    // sf = fdvm[APPLY_OFFSET];
    // sf->setType(*C_PointerType(C_VoidType()));
    // fref =  new SgFunctionRefExp(*sf);
    // fref->setSymbol(*sf);
    // fref->setType(*C_PointerType(C_VoidType()));
    // st = new SgStatement(VAR_DECL);
    // st->setExpression(0,*new SgExprListExp(*new SgPointerDerefExp(*fref)));

    // end_block-> insertStmtBefore(*st,*block_C);

    /* ----argument list-----  */
    // arg_list = new SgExprListExp(dvmdesc->copy());
    // fref->setLhs(arg_list);
    // arg_list->setRhs(*new SgExprListExp(base->copy()));
    // arg_list = arg_list->rhs();
    // arg_list->setRhs(*new SgExprListExp(dvmHdesc->copy()));

    //
    // DvmType loop_cuda_do(DvmhLoopRef *InDvmhLoop, dim3 *OutBlocks, IndexType **InOutBlocks);

    sf = fdvm[DO_CUDA];
    sf->setType(*C_DvmType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
    sarg = new SgSymbol(VARIABLE_NAME, "InDvmhLoop", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    hloop = ae;
    arg_list = new SgExprListExp(*ae);
    fref->setLhs(arg_list);


    typ = C_PointerType(t_dim3);
    sarg = new SgSymbol(VARIABLE_NAME, "OutBlocks", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    sarg = new SgSymbol(VARIABLE_NAME, "OutThreads", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    outThreads = new SgPointerDerefExp(*ae);

    s_cudaStream = new SgSymbol(TYPE_NAME, "cudaStream_t", *block_C);
    typ = C_PointerType(C_Derived_Type(s_cudaStream));
    sarg = new SgSymbol(VARIABLE_NAME, "OutStream", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    outStream = new SgPointerDerefExp(*ae);

    typ1 = C_PointerType(C_Derived_Type(s_CudaIndexType));
    typ = C_PointerType(typ1);
    sarg = new SgSymbol(VARIABLE_NAME, "InOutBlocks", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    ae->setType(typ1);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    //
    //void loop_cuda_register_red(DvmhLoopRef *InDvmhLoop, DvmType InRedNum, void **ArrayPtr, void **LocPtr);
    sf = fdvm[RED_CUDA];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    sarg = new SgSymbol(VARIABLE_NAME, "InRedNum", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    rednum = ae;
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    typ1 = C_PointerType(C_VoidType());
    typ = C_PointerType(typ1);
    sarg = new SgSymbol(VARIABLE_NAME, "ArrayPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    ae->setType(typ1);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    sarg = new SgSymbol(VARIABLE_NAME, "LocPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    ae->setType(typ1);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    // void loop_cuda_register_red_(DvmhLoopRef *InDvmhLoop, Dvmtype *InRedNumRef, void *InDeviceArrayBaseAddr, void *InDeviceLocBaseAddr,CudaOffsetTypeRef *ArrayOffsetPtr, CudaOffsetTypeRef *LocOffsetPtr);
    sf = fdvm[REGISTER_RED];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    sarg = new SgSymbol(VARIABLE_NAME, "InRedNumRef", *C_PointerType(C_DvmType()), *block_C);
    ae = new SgVarRefExp(sarg);
    ae = new SgPointerDerefExp(*ae);
    redNumRef = ae;
    arg_list->setRhs(*new SgExprListExp(*ae));

    arg_list = arg_list->rhs();

    typ = C_PointerType(C_VoidType());
    sarg = new SgSymbol(VARIABLE_NAME, "InDeviceArrayBaseAddr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    sarg = new SgSymbol(VARIABLE_NAME, "InDeviceLocBaseAddr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    typ = C_PointerType(C_Derived_Type(s_CudaOffsetTypeRef));
    sarg = new SgSymbol(VARIABLE_NAME, "ArrayOffsetPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    sarg = new SgSymbol(VARIABLE_NAME, "LocOffsetPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    // void loop_red_init(DvmhLoopRef *InDvmhLoop, Dvmtype *InRedNumRef, void *arrayPtr, void *locPtr);
    sf = fdvm[RED_INIT_C];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    //sarg=new SgSymbol(VARIABLE_NAME,"InRedNumRef",*C_PointerType(C_DvmType()),*block_C);
    //ae =  new SgVarRefExp(sarg);
    //ae =  new SgPointerDerefExp(*ae);
    //arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list->setRhs(*new SgExprListExp(redNumRef->copy()));
    arg_list = arg_list->rhs();

    typ = C_PointerType(C_VoidType());
    sarg = new SgSymbol(VARIABLE_NAME, "arrayPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    sarg = new SgSymbol(VARIABLE_NAME, "locPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    // void loop_cuda_red_init(DvmhLoopRef *InDvmhLoop, Dvmtype InRedNum, void *arrayPtr, void *locPtr, void **devArrayPtr, void **devLocPtr);
    arg_list = fref->lhs();  // argument list of loop_red_init()
    sf = fdvm[CUDA_RED_INIT];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */

    fref->setLhs(arg_list->copy()); // copying argument list of loop_red_init() function
    arg_list = fref->lhs();
    //renewing second argument: Dvmtype *InRedNumRef => Dvmtype InRedNum
    sarg = new SgSymbol(VARIABLE_NAME, "InRedNum", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    arg_list->rhs()->setLhs(*ae);
    while (arg_list->rhs() != 0)
        arg_list = arg_list->rhs();
    typ1 = C_PointerType(C_VoidType());
    typ = C_PointerType(typ1);
    sarg = new SgSymbol(VARIABLE_NAME, "devArrayPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    ae->setType(typ1);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "devLocPtr", *typ, *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    ae->setType(typ1);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    //
    // void loop_cuda_red_prepare_((DvmhLoopRef *InDvmhLoop, Dvmtype *InRedNumRef, DvmType *InCountRef, DvmType *InFillFlagRef);
    sf = fdvm[RED_PREPARE];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    arg_list->setRhs(*new SgExprListExp(redNumRef->copy()));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "InCountRef", *C_PointerType(C_DvmType()), *block_C);
    ae = new SgVarRefExp(sarg);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "InFillFlagRef", *C_PointerType(C_DvmType()), *block_C);
    ae = new SgVarRefExp(sarg);
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    // void loop_red_finish_(DvmhLoopRef *InDvmhLoop, Dvmtype *InRedNumRef);
    sf = fdvm[RED_FINISH];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    arg_list->setRhs(*new SgExprListExp(redNumRef->copy()));


    //
    // void loop_cuda_shared_needed(DvmhLoopRef *InDvmhLoop, DvmType *count);
    // sf = fdvm[SHARED_NEEDED];
    // sf->setType(*C_VoidType());
    // fref =  new SgFunctionRefExp(*sf);
    // fref->setSymbol(*sf);
    // st = new SgStatement(VAR_DECL);
    // st->setExpression(0,*new SgExprListExp(*fref));

    // end_block-> insertStmtBefore(*st,*block_C);

    /* ----argument list-----  */
    // arg_list = new SgExprListExp(hloop->copy());
    // fref->setLhs(arg_list);

    // sarg=new SgSymbol(VARIABLE_NAME,"countRef",*C_PointerType(C_DvmType()),*block_C);
    // ae =  new SgVarRefExp(sarg);
    // ae = new SgPointerDerefExp(*ae);
    // arg_list->setRhs(*new SgExprListExp(*ae));
    // arg_list = arg_list->rhs();

    // CudaIndexType *loop_cuda_get_local_part(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[]);

    sf = fdvm[GET_LOCAL_PART];
    typ = C_PointerType(C_Derived_Type(s_CudaIndexType));
    sf->setType(*typ);  //*C_PointerType(C_Derived_Type(s_CudaIndexType)));

    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    fref->setType(*typ);

    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*new SgPointerDerefExp(*fref)));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    arg_list->setRhs(*new SgExprListExp(dvmdesc->copy()));
    arg_list = arg_list->rhs();

    //DvmType loop_get_device_num_(DvmhLoopRef *InDvmhLoop)
    sf = fdvm[GET_DEVICE_NUM];
    sf->setType(*C_DvmType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    //DvmType loop_cuda_get_red_step_(DvmhLoopRef *InDvmhLoop)
    sf = fdvm[GET_OVERALL_STEP];
    sf->setType(*C_DvmType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    //
    //DvmType loop_get_dependency_mask_(DvmhLoopRef *InDvmhLoop)
    sf = fdvm[GET_DEP_MASK];
    sf->setType(*C_DvmType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);

    //
    //void dvmh_cuda_replicate_(void *addr, DvmType *recordSize, DvmType *quantity, void *devPtr)
    sf = fdvm[CUDA_REPLICATE];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    sarg = new SgSymbol(VARIABLE_NAME, "addr", *C_VoidType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_VoidType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*ae);
    fref->setLhs(arg_list);
    sarg = new SgSymbol(VARIABLE_NAME, "recordSize", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "quantity", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "devPtr", *C_VoidType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_VoidType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    //
    //DvmType DvmType loop_cuda_transform_(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[], DvmhLoopRef *backFlagRef, DvmType dvmhDesc[], DvmType addressingParams[]);
    // sf = fdvm[CUDA_TRANSFORM];
    // sf->setType(*C_DvmType());
    // fref =  new SgFunctionRefExp(*sf);
    // fref->setSymbol(*sf);
    // st = new SgStatement(VAR_DECL);
    // st->setExpression(0,*new SgExprListExp(*fref));

    // end_block-> insertStmtBefore(*st,*block_C);

    /* ----argument list-----  */
    // arg_list = new SgExprListExp(hloop->copy());
    // fref->setLhs(arg_list);
    // arg_list->setRhs(*new SgExprListExp(dvmdesc->copy()));
    // arg_list = arg_list->rhs();
    // typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
    // sarg=new SgSymbol(VARIABLE_NAME,"backFlagRef",*typ,*block_C);
    // ae =  new SgVarRefExp(sarg);
    // ae->setType(typ);
    // ae = new SgPointerDerefExp(*ae);
    // arg_list->setRhs( *new SgExprListExp(*ae));
    // arg_list = arg_list->rhs();
    // arg_list->setRhs(*new SgExprListExp(dvmHdesc->copy()));
    // arg_list = arg_list->rhs();
    // sarg=new SgSymbol(VARIABLE_NAME,"addressingParams",*dvmHdesc->symbol()->type(),*block_C);
    // ae = &(dvmHdesc->copy());
    // ae->setSymbol(*sarg);
    // arg_list->setRhs(*new SgExprListExp(*ae));

    //
    //DvmType DvmType loop_cuda_autotransform_(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[]);
    sf = fdvm[CUDA_AUTOTRANSFORM];
    sf->setType(*C_DvmType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);
    arg_list->setRhs(*new SgExprListExp(dvmdesc->copy()));
    arg_list = arg_list->rhs();

    //
    //void loop_cuda_get_config_(DvmhLoopRef *InDvmhLoop, DvmType *InSharedPerThread, DvmType *InRegsPerThread, dim3 *OutThreads, cudaStream_t *OutStream, DvmType *OutSharedPerBlock);
    sf = fdvm[GET_CONFIG];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    arg_list = new SgExprListExp(hloop->copy());
    fref->setLhs(arg_list);
    sarg = new SgSymbol(VARIABLE_NAME, "InSharedPerThread", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "InRegsPerThread", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    arg_list->setRhs(*new SgExprListExp(outThreads->copy()));
    arg_list = arg_list->rhs();
    arg_list->setRhs(*new SgExprListExp(outStream->copy()));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "OutSharedPerBlock", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

    //
    //void loop_fill_bounds_(DvmhLoopRef *InDvmhLoop, DvmType idxL[], DvmType idxH[], DvmType steps[]);
    if (options.isOn(NO_BL_INFO))
    {
        sf = fdvm[FILL_BOUNDS_C];
        sf->setType(*C_VoidType());
        fref = new SgFunctionRefExp(*sf);
        fref->setSymbol(*sf);
        st = new SgStatement(VAR_DECL);
        st->setExpression(0, *new SgExprListExp(*fref));

        end_block->insertStmtBefore(*st, *block_C);

        /* ----argument list-----  */
        arg_list = new SgExprListExp(hloop->copy());
        fref->setLhs(arg_list);
        typearray = new SgArrayType(*C_DvmType());
        typearray->addRange(M0);
        sarg = new SgSymbol(VARIABLE_NAME, "idxL", *typearray, *block_C);
        ae = new SgArrayRefExp(*sarg);
        ae->setType(*typearray);
        el = new SgExpression(EXPR_LIST);
        el->setLhs(NULL);
        ae->setLhs(*el);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        sarg = new SgSymbol(VARIABLE_NAME, "idxH", *typearray, *block_C);
        ae = &(ae->copy());
        ae->setSymbol(sarg);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        sarg = new SgSymbol(VARIABLE_NAME, "steps", *typearray, *block_C);
        ae = &(ae->copy());
        ae->setSymbol(sarg);
        arg_list->setRhs(*new SgExprListExp(*ae));
    }

    //
    //void dvmh_change_filled_bounds(DvmType *low, DvmType *high, DvmType *idx, DvmType n, DvmType dep, DvmType type_of_run, DvmType *idxs);
    sf = fdvm[CHANGE_BOUNDS];
    sf->setType(*C_VoidType());
    fref = new SgFunctionRefExp(*sf);
    fref->setSymbol(*sf);
    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*fref));

    end_block->insertStmtBefore(*st, *block_C);

    /* ----argument list-----  */
    sarg = new SgSymbol(VARIABLE_NAME, "low", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*ae);
    fref->setLhs(arg_list);
    sarg = new SgSymbol(VARIABLE_NAME, "high", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "idx", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "n", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_DvmType());
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "dep", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_DvmType());
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "type_of_run", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_DvmType());
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();
    sarg = new SgSymbol(VARIABLE_NAME, "idxs", *C_DvmType(), *block_C);
    ae = new SgVarRefExp(sarg);
    ae->setType(C_PointerType(C_DvmType()));
    ae = new SgPointerDerefExp(*ae);
    arg_list->setRhs(*new SgExprListExp(*ae));

}

SgStatement *Create_Empty_Stat()
{
    SgStatement *st;

    st = new SgStatement(COMMENT_STAT);
    end_block->insertStmtBefore(*st, *block_C);

    return(st);
}



SgStatement *Create_Init_Cuda_Function()
{
    SgStatement *st, *st_end;
    SgSymbol *sf;
    SgExpression *e;
    st = new SgStatement(FUNC_HEDR);
    sf = new SgSymbol(FUNCTION_NAME, "init_cuda_", *C_VoidType(), *block_C);
    st->setSymbol(*sf);
    e = new SgFunctionRefExp(*sf);
    e->setSymbol(*sf);
    st->setExpression(0, *e);
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*sf);

    end_block->insertStmtBefore(*st, *block_C);
    st->insertStmtAfter(*st_end, *st);
    return(st);
}

SgStatement *Create_C_Function(SgSymbol *sF)
{
    SgStatement *st_hedr, *st_end;
    SgExpression *fe;

    // create fuction header
    st_hedr = new SgStatement(FUNC_HEDR);
    st_hedr->setSymbol(*sF);
    fe = new SgFunctionRefExp(*sF);
    fe->setSymbol(*sF);
    st_hedr->setExpression(0, *fe);

    // create end of function 
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*sF);

    // inserting      
    end_block->insertStmtBefore(*st_hedr, *block_C);
    st_hedr->insertStmtAfter(*st_end, *st_hedr);

    return(st_hedr);
}

// TODO: __indexTypeInt and __indexTypeLLong
SgStatement *Create_C_Adapter_Function(SgSymbol *sadapter, int InternalPosition)
{
    // !!ATTENTION!! gpuO1 lvl2 disabled
    return(NULL);
}

SgStatement *Create_C_Adapter_Function(SgSymbol *sadapter)
{
    symb_list *sl;
    SgStatement *st_hedr, *st_end, *stmt, *do_while, *first_exec, *st_base = NULL, *st_call;
    SgExpression *fe, *ae, *arg_list, *el, *e, *er;
    SgExpression *espec;
    SgFunctionCallExp *fcall;
    //SgStatement *fileHeaderSt;
    SgSymbol *s_loop_ref, *sarg, *s, *sb, *sg, *sdev, *h_first, *hgpu_first, *base_first, *red_first, *uses_first, *scalar_first;
    SgSymbol *s_stream = NULL, *s_blocks = NULL, *s_threads = NULL, *s_blocks_info = NULL, *s_red_count = NULL, *s_tmp_var = NULL;
    SgSymbol *s_dev_num = NULL, *s_shared_mem = NULL, *s_regs = NULL, *s_blocksS = NULL, *s_idxL = NULL, *s_idxH = NULL, *s_idxTypeInKernel = NULL;
    SgSymbol *s_num_of_red_blocks = NULL, *s_fill_flag = NULL, *s_red_num = NULL, *s_restBlocks = NULL, *s_addBlocks = NULL, *s_overallBlocks = NULL;
    SgSymbol *s_max_blocks;
    SgType *typ = NULL;
    int ln, num, i, uses_num, shared_mem_count, has_red_array, use_device_num;
    char *define_name;
    int pl_rank = ParLoopRank();
    h_first = hgpu_first = base_first = red_first = uses_first = scalar_first = NULL;
    has_red_array = 0;  use_device_num = 0;
    s_dev_num = NULL;
    s_shared_mem = NULL;

    // create function header 
    st_hedr = Create_C_Function(sadapter);
    st_end = st_hedr->lexNext();
    fe = st_hedr->expr(0);
    st_hedr->addComment(Cuda_LoopHandlerComment());
    first_exec = st_end;

    // create  dummy argument list:
    // loop_ref,<dvm-array-headers>,<uses>,<reduction_array_bounds>

    typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *typ, *st_hedr);

    ae = new SgVarRefExp(s_loop_ref);                 //loop_ref
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*ae);
    fe->setLhs(arg_list);
                                   
    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)  // headers
    {                             //printf("%s\n",sl->symb->identifier());
        SgArrayType *typearray = new SgArrayType(*C_DvmType()); //(*C_LongType()); 
        typearray->addDimension(NULL);
        sarg = new SgSymbol(VARIABLE_NAME, sl->symb->identifier(), *typearray, *st_hedr);
        ae = new SgArrayRefExp(*sarg);
        ae->setType(*typearray);
        el = new SgExpression(EXPR_LIST);
        el->setLhs(NULL);
        ae->setLhs(*el);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            h_first = sarg;
    }
    for (el = uses_list, ln = 0; el; el = el->rhs(), ln++)    // uses
    {
        s = el->lhs()->symbol();
        typ = C_PointerType(C_Type(s->type()));
        sarg = new SgSymbol(VARIABLE_NAME, s->identifier(), *typ, *st_hedr);

        if (isByValue(s))
            SYMB_ATTR(sarg->thesymb) = SYMB_ATTR(sarg->thesymb) | USE_IN_BIT;
        ae = UsedValueRef(s, sarg);
        ae->setType(typ);
        ae = new SgPointerDerefExp(*ae);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            uses_first = sarg;
    }
    uses_num = ln;

    if (red_list)
    {
        reduction_operation_list *rsl;  //create dimmesion size list for reduction arrays
        int idim;
        SgExpression *ell;
        SgType *t;
        for (rsl = red_struct_list; rsl; rsl = rsl->next)
        {
            if (rsl->redvar_size == -1) //reduction variable is array with passed dimension's sizes      
            {
                el = NULL;
                t = C_PointerType(C_DvmType());
                for (idim = Rank(rsl->redvar); idim; idim--)
                {
                    sarg = new SgSymbol(VARIABLE_NAME, BoundName(rsl->redvar, idim, 1), *t, *st_hedr);
                    ae = new SgVarRefExp(sarg);
                    ae->setType(t);
                    el = AddElementToList(el, new SgPointerDerefExp(*ae));
                } 
                rsl->lowBound_arg = el;   
                el = NULL;
                for (idim = Rank(rsl->redvar); idim; idim--)
                {
                    sarg = new SgSymbol(VARIABLE_NAME, DimSizeName(rsl->redvar, idim), *t, *st_hedr);
                    ae = new SgVarRefExp(sarg);
                    ae->setType(t);
                    el = AddElementToList(el, new SgPointerDerefExp(*ae));
                   /*
                    ell = new SgExprListExp(*new SgPointerDerefExp(*ae));
                    ell->setRhs(el);
                    el = ell;
                    */
                }
                rsl->dimSize_arg = el;
                /*arg_list->setRhs(el->copy());*/
                arg_list = AddListToList(arg_list,&rsl->dimSize_arg->copy());
                arg_list = AddListToList(arg_list,&rsl->lowBound_arg->copy());

                while (arg_list->rhs() != 0)
                    arg_list = arg_list->rhs();
            }
        }
    }

    // create variable's declarations: <dvm_array_headers>,<dvm_array_bases>,<scalar_device_addr>,<reduction_variables>,blocks_info [ or blocksS,idxL,idxH ],stream,blocks,threads
    if (red_list)
    {
        reduction_operation_list *rsl;
        s_shared_mem = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("shared_mem"), *C_DvmType(), *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
        if(!options.isOn(C_CUDA))
        {
            s_red_count = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("red_count"), *SgTypeInt(), *st_hedr);
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        s_red_num = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("red_num"), *C_DvmType(), *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
        if (options.isOn(NO_BL_INFO))  // without blocks_info, by option -noBI
        {
            s_num_of_red_blocks = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_of_red_blocks"), *C_DvmType(), *st_hedr);
            addDeclExpList(s, stmt->expr(0));
            s_fill_flag = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("fill_flag"), *C_DvmType(), *st_hedr);
            addDeclExpList(s, stmt->expr(0));
        }

        //looking through the reduction_op_list
        for (er = red_list, rsl = red_struct_list, ln = 0; er; er = er->rhs(), rsl = rsl->next, ln++)
        {
            SgExpression *ered = NULL, *ev = NULL, *en = NULL, *loc_var_ref = NULL;
            SgSymbol *sred = NULL, *sgrid = NULL, *s_loc_var = NULL, *sgrid_loc = NULL, *sinit = NULL;
            int is_array;
            SgType *loc_type = NULL, *btype = NULL;

            loc_var_ref = NULL;  s_loc_var = NULL; is_array = 0;
            ered = er->lhs();    //  reduction  (variant==ARRAY_OP)
            //nop =RedFuncNumber(ered->lhs());
            ev = ered->rhs(); // reduction variable reference for reduction operations except MINLOC,MAXLOC 
            if (isSgExprListExp(ev))
            {
                ev = ev->lhs(); // reduction variable reference         
                loc_var_ref = ered->rhs()->rhs()->lhs();        //location array reference
                en = ered->rhs()->rhs()->rhs()->lhs(); // number of elements in location array
                loc_el_num = LocElemNumber(en);
                loc_type = loc_var_ref->symbol()->type();
            }
            else if (isSgArrayRefExp(ev) && !ev->lhs()) //whole array
                is_array = 1;
               
            s = sred = &(ev->symbol()->copy());
            SYMB_SCOPE(s->thesymb) = st_hedr->thebif;
            if (rsl->redvar_size > 0)
            {
                SgArrayType *typearray = new SgArrayType(*C_Type(ev->symbol()->type()));
                typearray->addRange(*ArrayLengthInElems(ev->symbol(), NULL, 0));
                s->setType(*typearray);

            }
            else if (rsl->redvar_size < 0)
                s->setType(C_PointerType(C_Type(ev->symbol()->type())));
            else
                s->setType(C_Type(ev->symbol()->type()));
            //stmt = (rsl->redvar_size < 0) ? makeSymbolDeclarationWithInit(s, MallocExpr(s, rsl->dimSize_arg)) : makeSymbolDeclaration(s);
            if (rsl->redvar_size >= 0)
            {
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);
            }
            if (!ln)
                red_first = s;
            s = sgrid = GreedSymbolForRedInAdapter(s, st_hedr);
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
            if (rsl->redvar_size < 0)
            {
                s = sinit = InitValSymbolForRedInAdapter(sred, st_hedr);
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);
            }
            s_loc_var = sgrid_loc = NULL;
            if (loc_var_ref)
            {
                s = s_loc_var = &(loc_var_ref->symbol()->copy());
                if (isSgArrayType(loc_type))
                    btype = loc_type->baseType();
                else
                    btype = loc_type;
                //!printf("__112\n");    
                SgArrayType *typearray = new SgArrayType(*C_Type(btype));
                typearray->addRange(*new SgValueExp(loc_el_num));
                s_loc_var->setType(*typearray);
                SYMB_SCOPE(s->thesymb) = st_hedr->thebif;
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);

                s = sgrid_loc = GreedSymbolForRedInAdapter(s, st_hedr);
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);
            }
             
            /*--- executable statements: register reductions in RTS ---*/
            e = &SgAssignOp(*new SgVarRefExp(s_red_num), *new SgValueExp(ln + 1));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            if (!ln)
            {
                stmt->addComment("// Register reduction for CUDA-execution");
                first_exec = stmt;
            }

            //XXX swap pointers, changed reduction scheme to atomic, Kolganov 06.02.2020
            if (rsl->redvar_size < 0)
                std::swap(sgrid, sinit);

            stmt = new SgCExpStmt(*RegisterReduction(s_loop_ref, s_red_num, sgrid, sgrid_loc));
            st_end->insertStmtBefore(*stmt, *st_hedr);                //!printf("__1131 %d\n",s_loc_var);
            e = (rsl->redvar_size >= 0) ? InitReduction(s_loop_ref, s_red_num, sred, s_loc_var) :
                                      CudaInitReduction(s_loop_ref, s_red_num, sinit, NULL);  //sred, s_loc_var,
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
              
        }
    }
    if (!options.isOn(NO_BL_INFO))
    {
        s_blocks_info = s = new SgSymbol(VARIABLE_NAME, "blocks_info", *C_PointerType(C_VoidType()), *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }
    else
    {
        s_blocksS = s = ArraySymbol(TestAndCorrectName("blocksS"), C_DvmType(), new SgValueExp(pl_rank), st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
        s_restBlocks = s = new SgSymbol(VARIABLE_NAME, "restBlocks", *C_Derived_Type(s_cudaStream), *st_hedr);
        addDeclExpList(s, stmt->expr(0));
		s_max_blocks = s = new SgSymbol(VARIABLE_NAME, "maxBlocks", *C_DvmType(), *st_hedr);
		addDeclExpList(s, stmt->expr(0));
        s_addBlocks = s = new SgSymbol(VARIABLE_NAME, "addBlocks", *C_Derived_Type(s_cudaStream), *st_hedr);
        addDeclExpList(s, stmt->expr(0));
        s_overallBlocks = s = new SgSymbol(VARIABLE_NAME, "overallBlocks", *C_Derived_Type(s_cudaStream), *st_hedr);
        addDeclExpList(s, stmt->expr(0));
        s_idxL = s = ArraySymbol(TestAndCorrectName("idxL"), C_DvmType(), new SgValueExp(pl_rank), st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
        s_idxH = s = ArraySymbol(TestAndCorrectName("idxH"), C_DvmType(), new SgValueExp(pl_rank), st_hedr);
        addDeclExpList(s, stmt->expr(0));
    }
    s_stream = s = new SgSymbol(VARIABLE_NAME, "stream", *C_Derived_Type(s_cudaStream), *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    s_blocks = s = new SgSymbol(VARIABLE_NAME, "blocks", *t_dim3, *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    s_threads = s = new SgSymbol(VARIABLE_NAME, "threads", *t_dim3, *st_hedr);
    addDeclExpList(s, stmt->expr(0));

    s_idxTypeInKernel = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxTypeInKernel"), *C_DvmType(), *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    for (s = uses_first, ln = 0; ln < uses_num; s = s->next(), ln++)    // uses
    if (!(s->attributes() & USE_IN_BIT))   // passing to kernel scalar argument by reference
    {
        sdev = GpuScalarAdrSymbolInAdapter(s, st_hedr);   // creating new symbol for address in device
        if (!scalar_first)
        {
            scalar_first = sdev;
            stmt = makeSymbolDeclaration(sdev);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(sdev, stmt->expr(0));
    }

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    {
        s = GpuHeaderSymbolInAdapter(sl->symb, st_hedr);
        if (!ln)
        {
            hgpu_first = s;
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(s, stmt->expr(0));
    }
      
    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    {
        s = GpuBaseSymbolInAdapter(sl->symb, st_hedr);
        if (!ln)
        {
            base_first = s;
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(s, stmt->expr(0));
    }
    num = ln;

    // create execution part


    /* -------- call dvmh_get_device_addr(long *deviceRef, void *variable) ----*/
    for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)    // uses
    if (!(s->attributes() & USE_IN_BIT))   // passing to kernel scalar argument by reference
    {
        s_dev_num = doDeviceNumVar(st_hedr, first_exec, s_dev_num, s_loop_ref);
        e = &SgAssignOp(*new SgVarRefExp(sdev), *GetDeviceAddr(s_dev_num, s));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
            stmt->addComment("// Get device addresses");
        sdev = sdev->next();
    }


    /* -------- call dvmh_get_natural_base(long *deviceRef, long dvmDesc[] ) ----*/

    for (s = h_first, sb = base_first, ln = 0; ln < num; s = s->next(), sb = sb->next(), ln++)
    {
        s_dev_num = doDeviceNumVar(st_hedr, first_exec, s_dev_num, s_loop_ref);
        e = &SgAssignOp(*new SgVarRefExp(sb), *GetNaturalBase(s_dev_num, s));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
        {
            stmt->addComment("// Get 'natural' bases");
            st_base = stmt; // save for inserting loop_cuda_autotransform_() before
        }
    }

    /* -------- call loop_cuda_autotransform_(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[] ) ----*/

    if (options.isOn(AUTO_TFM)) // for option -noTfm  calls are not generated
    {
        for (s = h_first, ln = 0; ln < num; s = s->next(), ln++)
        {
            e = CudaAutoTransform(s_loop_ref, s);
            stmt = new SgCExpStmt(*e);
            st_base->insertStmtBefore(*stmt, *st_hedr);  // insert before getting bases for arrays 
            if (!ln)
                stmt->addComment("// Autotransform arrays");
        }
    }
    /* -------- call  dvmh_fill_header_(long *deviceRef, void *base, long dvmDesc[], long dvmhDesc[]);----*/

    for (s = h_first, sg = hgpu_first, sb = base_first, ln = 0; ln < num; s = s->next(), sg = sg->next(), sb = sb->next(), ln++)
    {
        e = FillHeader(s_dev_num, sb, s, sg);
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
            stmt->addComment("// Fill 'device' headers");
    }

    if (options.isOn(RTC))
    {
        /* -------- call  loop_cuda_rtc_set_lang_(loop_ref, lang); ------------*/
        if (options.isOn(C_CUDA))
            stmt = new SgCExpStmt(*RtcSetLang(s_loop_ref, 1));
        else
            stmt = new SgCExpStmt(*RtcSetLang(s_loop_ref, 0));
        st_end->insertStmtBefore(*stmt, *st_hedr);
        stmt->addComment("// Set CUDA language for launching kernels in RTC");
    }

    /* -------- call  loop_guess_index_type_(loop_ref); ------------*/
    stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_idxTypeInKernel), *GuessIndexType(s_loop_ref)));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Guess index type in CUDA kernel");

    SgFunctionCallExp *sizeofL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofLL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofI = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));

    sizeofL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long")));
    sizeofLL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long long")));
    sizeofI->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "int")));

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
        &&
        SgEqOp(*sizeofL, *sizeofI),
        *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
        &&
        SgEqOp(*sizeofL, *sizeofLL),
        *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LLONG")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    /* -------- call loop_cuda_get_config_(DvmhLoopRef *InDvmhLoop, DvmType *InSharedPerThread, DvmType *InRegsPerThread, dim3 *OutThreads, cudaStream_t *OutStream,DvmType *OutSharedPerBlock) ----*/

    e = &SgAssignOp(*new SgVarRefExp(s_threads), *dim3FunctionCall(0));
    stmt = new SgCExpStmt(*e);
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Get CUDA configuration parameters");

    shared_mem_count = MaxRedVarSize(red_list);
    if (shared_mem_count)
    {
        if (!options.isOn(C_CUDA))
        {
            e = &SgAssignOp(*new SgVarRefExp(s_shared_mem), *new SgValueExp(shared_mem_count));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
        else
        {
            char *tmp = new char[strlen("ifdef ") + strlen(fermiPreprocDir) + 1];
            tmp[0] = 0;
            strcat(tmp, "#ifdef ");
            strcat(tmp, fermiPreprocDir);
            st_end->insertStmtBefore(*PreprocessorDirective(tmp), *st_hedr);
            e = &SgAssignOp(*new SgVarRefExp(s_shared_mem), *new SgValueExp(shared_mem_count));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            st_end->insertStmtBefore(*PreprocessorDirective("#else"), *st_hedr);
            e = &SgAssignOp(*new SgVarRefExp(s_shared_mem), *new SgValueExp(0));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
            st_end->insertStmtBefore(*PreprocessorDirective("#endif"), *st_hedr);
        }
    }
        
    SgSymbol *s_regs_int, *s_regs_llong;

    std::string define_name_int = kernel_symb->identifier();
    std::string define_name_long = kernel_symb->identifier();

    define_name_int += "_int_regs";
    define_name_long += "_llong_regs";

    s_regs_int = new SgSymbol(VARIABLE_NAME, define_name_int.c_str(), *C_DvmType(), *block_C);
    s_regs_llong = new SgSymbol(VARIABLE_NAME, define_name_long.c_str(), *C_DvmType(), *block_C);

    SgStatement *config_int = new SgCExpStmt(*GetConfig(s_loop_ref, s_shared_mem, s_regs_int, s_threads, s_stream, s_shared_mem));
    SgStatement *config_long = new SgCExpStmt(*GetConfig(s_loop_ref, s_shared_mem, s_regs_llong, s_threads, s_stream, s_shared_mem));

    RGname_list = AddNewToSymbList(RGname_list, s_regs_int);
    RGname_list = AddNewToSymbList(RGname_list, s_regs_llong);
                
    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(*s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT"))), *config_int, *config_long);
    st_end->insertStmtBefore(*stmt, *st_hedr);
        
    /* generating for info_block
    define_name = RegisterConstName();
    stmt = ifdef_dir(define_name);
    end_info_block->insertStmtBefore(*stmt,*info_block);
    s_regs_info = &(s_regs->copy());
    SYMB_SCOPE(s_regs_info->thesymb) = info_block->thebif;
    stmt = makeSymbolDeclarationWithInit(s_regs_info, new SgVarRefExp(new SgSymbol(VARIABLE_NAME, define_name)));
    end_info_block->insertStmtBefore(*stmt, *info_block);
    stmt = else_dir();
    end_info_block->insertStmtBefore(*stmt,*info_block);
    stmt = makeSymbolDeclarationWithInit(s_regs_info, new SgValueExp(0));
    end_info_block->insertStmtBefore(*stmt, *info_block);
    stmt = endif_dir();
    end_info_block->insertStmtBefore(*stmt,*info_block); */
    

    /* --------- call cuda-kernel ----*/
    espec = CreateBlocksThreadsSpec(shared_mem_count, s_blocks, s_threads, s_stream, s_shared_mem);

    fcall = CallKernel(kernel_symb, espec);

    /* --------- add argument list to kernel call ----*/
    for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
    {
        e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
        fcall->addArg(*e);
        for (i = NumberOfCoeffs(sg); i>0; i--)
            fcall->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
    }
    if (red_list)
    {
        reduction_operation_list *rsl;
        for (rsl = red_struct_list, s = red_first; rsl; rsl = rsl->next)  //s!=s_blocks_info
        {
            if (rsl->redvar_size == 0) //reduction variable is scalar 
            {
                if (options.isOn(RTC))
                {
                    SgVarRefExp *toAdd = new SgVarRefExp(s);
                    toAdd->addAttribute(RTC_NOT_REPLACE);
                    fcall->addArg(*toAdd);
                }
                else
                    fcall->addArg(*new SgVarRefExp(s));
            }
            else if (rsl->redvar_size > 0)
            {
                int i;
                has_red_array = 1;
                for (i = 0; i < rsl->redvar_size; i++)
                    fcall->addArg(*new SgArrayRefExp(*s, *new SgValueExp(i)));
            }
            else
            {
                has_red_array = 1;
                for (el = rsl->dimSize_arg; el; el = el->rhs())
                    fcall->addArg(el->lhs()->copy());
                for (el = rsl->lowBound_arg; el; el = el->rhs())
                    fcall->addArg(el->lhs()->copy());
            }
            s = s->next();
            //if (rsl->redvar_size < 0)  s = s->next(); // to omit symbol for 'malloc'
            // <red-var_grid> symbol to collect reduction values   
            e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(rsl->redvar->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(s));
            fcall->addArg(*e); s = s->next();
            if (rsl->redvar_size < 0)
            {// <red-var_init> symbol for initial values of reduction array
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(rsl->redvar->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(s));
                fcall->addArg(*e); s = s->next();
            }
            //if(isSgExprListExp(er->lhs()->rhs())) //MAXLOC,MINLOC
            if (rsl->locvar)  //MAXLOC,MINLOC
            {
                int i;
                for (i = 0; i < rsl->number; i++)
                    fcall->addArg(*new SgArrayRefExp(*s, *new SgValueExp(i)));
                s = s->next();
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(rsl->locvar->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(s));
                fcall->addArg(*e); s = s->next();
            }
        }
    }

    if (!options.isOn(NO_BL_INFO))
    {
        if (options.isOn(C_CUDA))
            e = new SgVarRefExp(s_blocks_info);
        else
            e = new SgCastExp(*C_PointerType(new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(s_blocks_info));
        fcall->addArg(*e);   //'bloks_info'

    }
    else  //without blocks_info
    {
        for (i = 0; i < pl_rank; i++)
        {
            fcall->addArg(*new SgArrayRefExp(*s_idxL, *new SgValueExp(i)));     //'idxL[...]'
            fcall->addArg(*new SgArrayRefExp(*s_idxH, *new SgValueExp(i)));     //'idxH[...]'
        }
        for (i = 1; i < pl_rank; i++)
            fcall->addArg(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i)));  //'blocksS[...]'
        fcall->addArg(*new SgVarRefExp(*s_addBlocks));                         //'addBlocks'      
    }

    if (red_list)
    {
        if(!options.isOn(C_CUDA))
            fcall->addArg(*new SgVarRefExp(s_red_count));         //'red_count'
        if (has_red_array)
        {
            if (!options.isOn(NO_BL_INFO))
                fcall->addArg(*GetOverallStep(s_loop_ref));
            else
                fcall->addArg(*new SgVarRefExp(*s_num_of_red_blocks));
        }
    }

    for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
    if (s->attributes() & USE_IN_BIT)
        fcall->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
    else
    {                                                   // passing argument by reference to kernel
        e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? s->type()->baseType() : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
        fcall->addArg(*e);
        sdev = sdev->next();
    }


    if (!options.isOn(NO_BL_INFO))
    {
        //insert kernel call
        stmt = createKernelCallsInCudaHandler(fcall, s_loop_ref, s_idxTypeInKernel, s_blocks);

        /* ------- WHILE (loop_cuda_do(DvmhLoopRef *InDvmhLoop, dim3 *OutBlocks, dim3 *OutThreads, cudaStream_t *OutStream, CudaIndexType **InOutBlocks) != 0) ----*/
        e = LoopDoCuda(s_loop_ref, s_blocks, s_threads, s_stream, s_blocks_info, s_idxTypeInKernel);
        do_while = new SgWhileStmt(SgNeqOp(*e, *new SgValueExp(0)), *stmt);

        st_end->insertStmtBefore(*do_while, *st_hedr);
        do_while->addComment("// GPU execution");

        /* ------ block for reductions ----*/
        if (red_list && !options.isOn(C_CUDA))  //if(red_op_list)
            InsertDoWhileForRedCount_C(do_while, s_threads, s_red_count);

    }
    else //without blocks-info
    {
        //loop_fill_bounds_(loop_ref,idxL,idxH,0);
        e = FillBounds(s_loop_ref, s_idxL, s_idxH, NULL);
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);

        // blocksS[i] = ...           i=0,...,pl_rank-1
        for (i = pl_rank - 1; i >= 0; i--)
        {
            stmt = AssignBlocksSElement(i, pl_rank, s_blocksS, s_idxL, s_idxH, s_threads);
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        // overallBlocks = blocksS[0];
        // restBlocks = overallBlocks;
        // addBlocks = 0;
        // blocks = dim3(1,1,1);

        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*s_overallBlocks), *new SgArrayRefExp(*s_blocksS, *new SgValueExp(0))));
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (currentLoop && currentLoop->irregularAnalysisIsOn())
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*s_restBlocks), *new SgVarRefExp(*s_overallBlocks) * *new SgValueExp(warpSize)));
        else
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*s_restBlocks), *new SgVarRefExp(*s_overallBlocks)));
        st_end->insertStmtBefore(*stmt, *st_hedr);
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*s_addBlocks), *new SgValueExp(0)));
        st_end->insertStmtBefore(*stmt, *st_hedr);
        e = &SgAssignOp(*new SgVarRefExp(s_blocks), *dim3FunctionCall(1));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);

        //    stmt = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_blocks,"x"),*new SgArrayRefExp(*s_blocksS,*new SgValueExp(0)))); 
        //    st_end->insertStmtBefore(*stmt,*st_hedr);
        //    stmt = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_blocks,"y"),*new SgValueExp(1))); 
        //    st_end->insertStmtBefore(*stmt,*st_hedr);
        //    stmt = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_blocks,"z"),*new SgValueExp(1))); 
        //    st_end->insertStmtBefore(*stmt,*st_hedr);

        /* ------ block for prepare reductions ----*/
        if (red_list)
        {
            InsertAssignForReduction(st_end, s_num_of_red_blocks, s_fill_flag, s_overallBlocks, s_threads);
            if(!options.isOn(C_CUDA))
                InsertDoWhileForRedCount_C(st_end, s_threads, s_red_count);
            InsertPrepareReductionCalls(st_end, s_loop_ref, s_num_of_red_blocks, s_fill_flag, s_red_num);
        }
        //insert kernel call
        st_call = createKernelCallsInCudaHandler(fcall, s_loop_ref, s_idxTypeInKernel, s_blocks);

		
		SgFunctionCallExp *getProp = new SgFunctionCallExp(*new SgSymbol(FUNCTION_NAME, "loop_cuda_get_device_prop"));
		getProp->addArg(*new SgVarRefExp(s_loop_ref));
		getProp->addArg(*new SgKeywordValExp("CUDA_MAX_GRID_X"));

		stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*s_max_blocks), *getProp));
		st_end->insertStmtBefore(*stmt, *st_hedr);

        if (currentLoop && currentLoop->irregularAnalysisIsOn())
        {
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*s_max_blocks), *new SgVarRefExp(*s_max_blocks) / *new SgValueExp(warpSize) * *new SgValueExp(warpSize)));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        //e = & operator > ( *new SgVarRefExp(s_restBlocks), 
        do_while = new SgWhileStmt(operator > (*new SgVarRefExp(s_restBlocks), *new SgValueExp(0)), *st_call);
        st_end->insertStmtBefore(*do_while, *st_hedr);
        do_while->addComment("// GPU execution");
		stmt = IfForHeader(s_restBlocks, s_blocks, s_max_blocks);
        st_call->insertStmtBefore(*stmt, *do_while);
        stmt = new SgCExpStmt(*new SgExpression(MINUS_ASSGN_OP, new SgVarRefExp(*s_restBlocks), new SgRecordRefExp(*s_blocks, "x"), NULL));
        st_call->insertStmtAfter(*stmt, *do_while);
        stmt = new SgCExpStmt(operator += (*new SgVarRefExp(*s_addBlocks), *new SgRecordRefExp(*s_blocks, "x")));
        st_call->insertStmtAfter(*stmt, *do_while);
        /* ------ block for finish reductions ----*/
        if (red_list)
            InsertFinishReductionCalls(st_end, s_loop_ref, s_red_num);
    }

    return(st_hedr);
}


SgStatement *Create_C_Adapter_Function_For_Sequence(SgSymbol *sadapter, SgStatement *first_st)
{
    symb_list *sl = NULL;
    SgStatement *st_hedr = NULL, *st_end = NULL, *stmt = NULL, *do_while = NULL, *st_base = NULL;
    SgExpression *fe = NULL, *ae = NULL, *arg_list = NULL, *el = NULL, *e = NULL;
    SgExpression *espec = NULL;
    SgFunctionCallExp *fcall = NULL;
    //SgStatement *fileHeaderSt;
    SgSymbol *s_loop_ref = NULL, *sarg = NULL, *s = NULL, *sb = NULL, *sg = NULL, *sdev = NULL, *h_first = NULL;
    SgSymbol *hgpu_first = NULL, *base_first = NULL, *uses_first = NULL, *scalar_first = NULL;
    SgSymbol *s_stream = NULL, *s_blocks = NULL, *s_threads = NULL, *s_dev_num = NULL, *s_idxTypeInKernel = NULL;
    SgType *typ = NULL;
    int ln, num, i, uses_num;

    // create fuction header
    st_hedr = Create_C_Function(sadapter);
    st_end = st_hedr->lexNext();
    fe = st_hedr->expr(0);
    st_hedr->addComment(Cuda_SequenceHandlerComment(first_st->lineNumber()));

    // create  dummy argument list:
    // loop_ref,<dvm-array-headers>,<uses>

    typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *typ, *st_hedr);
    ae = new SgVarRefExp(s_loop_ref);                 //loop_ref
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*ae);
    fe->setLhs(arg_list);

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)  // headers
    {  //printf("%s\n",sl->symb->identifier()); 
        SgArrayType *typearray = new SgArrayType(*C_DvmType());
        //typearray -> addRange(*new SgValueExp(Rank(sl->symb)+2));
        sarg = new SgSymbol(VARIABLE_NAME, sl->symb->identifier(), *typearray, *st_hedr);
        ae = new SgArrayRefExp(*sarg);
        ae->setType(*typearray);
        el = new SgExpression(EXPR_LIST);
        el->setLhs(NULL);
        ae->setLhs(*el);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            h_first = sarg;
    }
    for (el = uses_list, ln = 0; el; el = el->rhs(), ln++)    // uses
    {
        s = el->lhs()->symbol();
        typ = C_PointerType(C_Type(s->type()));
        sarg = new SgSymbol(VARIABLE_NAME, s->identifier(), *typ, *st_hedr);
        if (isByValue(s))
            SYMB_ATTR(sarg->thesymb) = SYMB_ATTR(sarg->thesymb) | USE_IN_BIT;

        ae = UsedValueRef(s, sarg);
        ae->setType(typ);
        ae = new SgPointerDerefExp(*ae);
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
        if (!ln)
            uses_first = sarg;
    }
    uses_num = ln;

    // create variable's declarations: <dvm_array_headers>,<dvm_array_bases>,<scalar_device_addr>,stream,blocks,threads

    s_stream = s = new SgSymbol(VARIABLE_NAME, "stream", *C_Derived_Type(s_cudaStream), *st_hedr);
    stmt = makeSymbolDeclaration(s); /*stmt = s->makeVarDeclStmt(); */
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    s_blocks = s = new SgSymbol(VARIABLE_NAME, "blocks", *t_dim3, *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    s_threads = s = new SgSymbol(VARIABLE_NAME, "threads", *t_dim3, *st_hedr);
    addDeclExpList(s, stmt->expr(0));

    s_idxTypeInKernel = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxTypeInKernel"), *C_DvmType(), *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    for (s = uses_first, ln = 0; ln < uses_num; s = s->next(), ln++)    // uses
    if (!(s->attributes() & USE_IN_BIT))   // passing to kernel scalar argument by reference
    {
        sdev = GpuScalarAdrSymbolInAdapter(s, st_hedr);   // creating new symbol for address in device
        if (!scalar_first)
        {
            scalar_first = sdev;
            stmt = makeSymbolDeclaration(sdev);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(sdev, stmt->expr(0));
    }

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    {
        s = GpuHeaderSymbolInAdapter(sl->symb, st_hedr);
        if (!ln)
        {
            hgpu_first = s;
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(s, stmt->expr(0));
    }


    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ln++)
    {
        s = GpuBaseSymbolInAdapter(sl->symb, st_hedr);
        if (!ln)
        {
            base_first = s;
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(s, stmt->expr(0));
    }
    num = ln;

    // create execution part

    /* -------- call dvmh_get_device_addr(DvmType *deviceRef, void *variable) ----*/
    for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)    // uses
    if (!(s->attributes() & USE_IN_BIT))   // passing to kernel scalar argument by reference
    {
        s_dev_num = doDeviceNumVar(st_hedr, st_end, s_dev_num, s_loop_ref);
        e = &SgAssignOp(*new SgVarRefExp(sdev), *GetDeviceAddr(s_dev_num, s));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
            stmt->addComment("// Get device addresses");
        sdev = sdev->next();
    }

    /* -------- call dvmh_get_natural_base(DvmType *deviceRef, DvmType dvmDesc[]) ----*/

    for (s = h_first, sb = base_first, ln = 0; ln < num; s = s->next(), sb = sb->next(), ln++)
    {
        s_dev_num = doDeviceNumVar(st_hedr, st_end, s_dev_num, s_loop_ref);
        e = &SgAssignOp(*new SgVarRefExp(sb), *GetNaturalBase(s_dev_num, s));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
        {
            stmt->addComment("// Get 'natural' bases");
            st_base = stmt; // save for inserting loop_cuda_autotransform_() before
        }
    }

    /* -------- call loop_cuda_autotransform_(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[] ) ----*/
    if (options.isOn(AUTO_TFM)) // for option -noTfm  calls are not generated
    {
        for (s = h_first, ln = 0; ln < num; s = s->next(), ln++)
        {
            e = CudaAutoTransform(s_loop_ref, s);
            stmt = new SgCExpStmt(*e);
            st_base->insertStmtBefore(*stmt, *st_hedr);  // insert before getting bases for arrays 
            if (!ln)
                stmt->addComment("// Autotransform arrays");
        }
    }
    /* -------- call  dvmh_fill_header_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[]);----*/

    for (s = h_first, sg = hgpu_first, sb = base_first, ln = 0; ln < num; s = s->next(), sg = sg->next(), sb = sb->next(), ln++)
    {
        e = FillHeader(s_dev_num, sb, s, sg);
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
            stmt->addComment("// Fill 'device' headers");
    }

    /* -------- call   loop_guess_index_type_(loop_ref); ------------*/
    stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_idxTypeInKernel), *GuessIndexType(s_loop_ref)));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Guess index type in CUDA kernel");

    SgFunctionCallExp *sizeofL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofLL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofI = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));

    sizeofL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long")));
    sizeofLL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long long")));
    sizeofI->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "int")));

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
        &&
        SgEqOp(*sizeofL, *sizeofI),
        *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
        &&
        SgEqOp(*sizeofL, *sizeofLL),
        *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LLONG")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    if (lpart_list)  // there are dvm-array references in left part of assign statement
    {
        local_part_list *pl;

        for (pl = lpart_list; pl; pl = pl->next)
        {
            pl->local_part = new SgVariableSymb(pl->local_part->identifier(), *C_PointerType(C_VoidType()), *st_hedr);
            stmt = makeSymbolDeclarationWithInit(pl->local_part, GetLocalPart(s_loop_ref, pl->dvm_array, s_idxTypeInKernel));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
    }

    /* -------- call loop_cuda_get_config_(DvmhLoopRef *InDvmhLoop, DvmType *InSharedPerThread, DvmType *InRegsPerThread, dim3 *OutThreads, cudaStream_t *OutStream,DvmType *OutSharedPerBlock) ----*/

    e = &SgAssignOp(*new SgVarRefExp(s_threads), *dim3FunctionCall(0));
    stmt = new SgCExpStmt(*e);
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Get CUDA configuration parameters");

    e = GetConfig(s_loop_ref, NULL, NULL, s_threads, s_stream, NULL);
    stmt = new SgCExpStmt(*e);
    st_end->insertStmtBefore(*stmt, *st_hedr);

    /* --------- call cuda-kernel ----*/
    espec = CreateBlocksThreadsSpec(0, s_blocks, s_threads, s_stream, NULL);

    fcall = CallKernel(kernel_symb, espec);

    /* --------- add argument list to kernel call ----*/
    // bases and coefficients for arrays  
    for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
    {
        e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
        fcall->addArg(*e);
        for (i = NumberOfCoeffs(sg); i>0; i--)
            fcall->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
    }

    if (lpart_list)                                         // local parts for dvm-arrays
    {
        local_part_list *pl;

        for (pl = lpart_list; pl; pl = pl->next)
        {
            if (options.isOn(C_CUDA))
            {
                e = new SgVarRefExp(pl->local_part);                
                SgAttribute *att = new SgAttribute(1, NULL, 777, *new SgSymbol(VARIABLE_NAME), 777);
                e->addAttribute(att);
            }
            else
                e = new SgCastExp(*C_PointerType(new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(pl->local_part));
            fcall->addArg(*e);            
        }
    }

    for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
    if (s->attributes() & USE_IN_BIT)
        fcall->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
    else
    {                                                   // passing argument by reference to kernel        
        e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? s->type()->baseType() : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
        fcall->addArg(*e);
        sdev = sdev->next();
    }

    // insetr kernel call
    stmt = createKernelCallsInCudaHandler(fcall, s_loop_ref, s_idxTypeInKernel, s_blocks);
    /* ------- WHILE (loop_cuda_do(DvmhLoopRef *InDvmhLoop, dim3 *OutBlocks, dim3 *OutThreads, cudaStream_t *OutStream, CudaIndexType **InOutBlocks) != 0) ----*/

    e = LoopDoCuda(s_loop_ref, s_blocks, s_threads, s_stream, NULL, CudaIndexConst());
    do_while = new SgWhileStmt(SgNeqOp(*e, *new SgValueExp(0)), *stmt);
    st_end->insertStmtBefore(*do_while, *st_hedr);
    do_while->addComment("// GPU execution");

    return(st_hedr);
}

SgStatement *AssignBlocksSElement(int i, int pl_rank, SgSymbol *s_blocksS, SgSymbol *s_idxL, SgSymbol *s_idxH, SgSymbol *s_threads)
{
    SgExpression *e;
    int istep;
    istep = IConstStep(DoStmt(first_do_par, i + 1));
    // idxH[i] - idxL[i] + 1
    e = &(*new SgArrayRefExp(*s_idxH, *new SgValueExp(i)) - *new SgArrayRefExp(*s_idxL, *new SgValueExp(i)));
    if (istep != 1)
        // (idxH[i] - idxL[i] + 1)/step[i]
        e = &((*e + *new SgValueExp(1)) / *new SgValueExp(istep));
    if (istep == 1)
    {
        if (i == pl_rank - 1)
            // blocksS[i]= (idxH[i] - idxL[i]  + threads.x ) / threads.x;
            e = &((*e + *new SgRecordRefExp(*s_threads, "x")) / *new SgRecordRefExp(*s_threads, "x"));

        if (i == pl_rank - 2)
            // blocksS[i] =  blocksS[i+1] * ((idxH[i] - idxL[i]  + threads.y ) / threads.y);    
            e = &(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i + 1)) * ((*e + *new SgRecordRefExp(*s_threads, "y")) / *new SgRecordRefExp(*s_threads, "y")));
        if (i == pl_rank - 3)
            // blocksS[i] =  blocksS[i+1] * ((idxH[i] - idxL[i]  + threads.z ) / threads.z);
            e = &(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i + 1)) * ((*e + *new SgRecordRefExp(*s_threads, "z")) / *new SgRecordRefExp(*s_threads, "z")));
        if (i <= pl_rank - 4)
            //blocksS[i]=  blocksS[i+1]* (idxH[i] - idxL[i] + 1 );
            e = &(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i + 1)) * (*e + *new SgValueExp(1)));
    }
    else
    {
        if (i == pl_rank - 1)
            // blocksS[i]= (idxH[i] - idxL[i] + 1)/step[i]  + threads.x - 1) / threads.x;
            e = &((*e + *new SgRecordRefExp(*s_threads, "x") - *new SgValueExp(1)) / *new SgRecordRefExp(*s_threads, "x"));
        if (i == pl_rank - 2)
            // blocksS[i] =  blocksS[i+1] * (((idxH[i] - idxL[i] + 1)/step[i] + threads.y - 1) / threads.y);     step==1
            e = &(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i + 1)) * ((*e + *new SgRecordRefExp(*s_threads, "y") - *new SgValueExp(1)) / *new SgRecordRefExp(*s_threads, "y")));
        if (i == pl_rank - 3)
            // blocksS[i] =  blocksS[i+1] * (((idxH[i] - idxL[i] + 1)/step[i] + threads.z - 1 ) / threads.z);
            e = &(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i + 1)) * ((*e + *new SgRecordRefExp(*s_threads, "z") - *new SgValueExp(1)) / *new SgRecordRefExp(*s_threads, "z")));
        if (i <= pl_rank - 4)
            //blocksS[i] =  blocksS[i+1] *   ((idxH[i] - idxL[i] + 1)/step[i]);
            e = &(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i + 1)) * *e);
    }
    return  new SgCExpStmt(SgAssignOp(*new SgArrayRefExp(*s_blocksS, *new SgValueExp(i)), *e));
}

SgStatement *IfForHeader(SgSymbol *s_restBlocks, SgSymbol *s_blocks, SgSymbol *s_max_blocks)
{
    //             if (restBlocks <= max_blocks)
    //                 blocks.x = restBlocks;
    //             else
    //                 blocks.x = max_blocks;
    SgStatement *if_st, *stTrue, *stFalse;
    SgExpression *restBlocksRef, *blocksRef, *cond;
    restBlocksRef = new SgVarRefExp(s_restBlocks);
	blocksRef = new SgVarRefExp(s_blocks);

	cond = &(*restBlocksRef <= (*new SgVarRefExp(s_max_blocks)));
    stTrue = new SgCExpStmt(SgAssignOp(*blocksRef, *restBlocksRef));
	stFalse = new SgCExpStmt(SgAssignOp(*blocksRef, *new SgVarRefExp(s_max_blocks)));
	if_st = new SgIfStmt(*cond, *stTrue, *stFalse);

    return if_st;
}

void InsertDoWhileForRedCount_C(SgStatement *cp, SgSymbol *s_threads, SgSymbol *s_red_count)
{  
    // inserting after statement cp (DO_WHILE) the block for red_count calculation:
    //             red_count = 1;
    //             while (red_count * 2 < threads%x * threads%y * threads%z)
    //                red_count *= 2;
    //            
    SgStatement *st_while, *ass;
    SgExpression *cond, *asse;
    // red_count * 2 .lt. threads%x * threads%y * threads%z
    cond = &operator < (*new SgVarRefExp(s_red_count) * (*new SgValueExp(2)), *ThreadsGridSize(s_threads));
    // insert do while loop
    //ass = new SgAssignStmt(*new SgVarRefExp(red_count_symb), (*new SgVarRefExp(red_count_symb))*(*new SgValueExp(2)));
    asse = &operator *= (*new SgVarRefExp(s_red_count), *new SgValueExp(2));
    ass = new SgCExpStmt(*asse);
    st_while = new SgWhileStmt(*cond, *ass);
    if (cp->variant() == WHILE_NODE)
    cp->insertStmtAfter(*st_while, *cp);
    else
    cp->insertStmtBefore(*st_while, *cp->controlParent());
    //  insert:           red_count = 1
    ass = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_red_count), *new SgValueExp(1)));
    st_while->insertStmtBefore(*ass, *st_while->controlParent()); 
    return;
  
   
  /*
    // !!!!!!!!!!!!! DEPRECATED BLOCK !!!!!!!!!!!!!!!!!!!!!!
    // inserting after statement cp (DO_WHILE) the block for red_count calculation:
    //             red_count = 1;
    SgStatement *ass = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_red_count), *new SgValueExp(1)));
    if (cp->variant() == WHILE_NODE)
        cp->insertStmtAfter(*ass, *cp);
    else
        cp->insertStmtBefore(*ass, *cp->controlParent());
    // !!!!!!!!!!!!! END OF DEPRECATED !!!!!!!!!!!!!!!!!!!!!!
  */
}

void InsertAssignForReduction(SgStatement *st_where, SgSymbol *s_num_of_red_blocks, SgSymbol *s_fill_flag, SgSymbol *s_overallBlocks, SgSymbol *s_threads)
{
    // inserting before statement 'st_where' the block of assignments:
    SgStatement *ass;
    // for C_Cuda:
    //             num_of_red_blocks = overallBlocks * (threads.x * threads.y * threads.z / warpSize);  
    // for Fortran_Cuda:
    //             num_of_red_blocks = overallBlocks;

    SgExpression *re = new SgVarRefExp(*s_overallBlocks); 
    if(options.isOn(C_CUDA))
        re = &(*re * (*new SgRecordRefExp(*s_threads, "x") * *new SgRecordRefExp(*s_threads, "y") * *new SgRecordRefExp(*s_threads, "z") / *new SgValueExp(warpSize)));
    ass = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_num_of_red_blocks), *re));
    st_where->insertStmtBefore(*ass, *st_where->controlParent());
    ass->addComment("// Prepare reduction");

    // fill_flag = 0; 
    ass = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_fill_flag), *new SgValueExp(0)));
    st_where->insertStmtBefore(*ass, *st_where->controlParent());
}

void  InsertPrepareReductionCalls(SgStatement *st_where, SgSymbol *s_loop_ref, SgSymbol *s_num_of_red_blocks, SgSymbol *s_fill_flag, SgSymbol *s_red_num)
{ // inserting before statement 'st_where'
    SgStatement *stmt;
    int ln;
    reduction_operation_list *rsl;
    // red_num = <reduction_operation_number>
    // loop_cuda_red_prepare_(loop_ref, &(red_num), &(num_of_red_blocks), &(fill_flag));
    //looking through the reduction_op_list
    for (rsl = red_struct_list, ln = 0; rsl; rsl = rsl->next, ln++)
    {
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_red_num), *new SgValueExp(ln + 1)));
        st_where->insertStmtBefore(*stmt, *st_where->controlParent());

        //XXX changed reduction scheme to atomic, Kolganov 06.02.2020
        if (rsl->redvar_size < 0)
            stmt = new SgCExpStmt(*PrepareReduction(s_loop_ref, s_red_num, s_num_of_red_blocks, s_fill_flag, 1, 1));
        else
            stmt = new SgCExpStmt(*PrepareReduction(s_loop_ref, s_red_num, s_num_of_red_blocks, s_fill_flag));
        st_where->insertStmtBefore(*stmt, *st_where->controlParent());
    }
}

void InsertFinishReductionCalls(SgStatement *st_where, SgSymbol *s_loop_ref, SgSymbol *s_red_num)
{ // inserting before statement 'st_where'
    SgStatement *stmt;
    int ln;
    reduction_operation_list *rsl;
    // red_num = <reduction_operation_number>
    // loop_red_finish_(loop_ref, &(red_num), &(num_of_red_blocks), &(fill_flag));
    //looking through the reduction_op_list
    for (rsl = red_struct_list, ln = 0; rsl; rsl = rsl->next, ln++)
    {
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_red_num), *new SgValueExp(ln + 1)));
        st_where->insertStmtBefore(*stmt, *st_where->controlParent());
        if (!ln)
            stmt->addComment("// Finish reduction");
        stmt = new SgCExpStmt(*FinishReduction(s_loop_ref, s_red_num));
        st_where->insertStmtBefore(*stmt, *st_where->controlParent());
    }
}

int    MaxRedVarSize(SgExpression *red_op_list)
{
    reduction_operation_list *rsl;
    SgExpression  *ev, *er, *ered, *el, *en;
    int   max, size, num_el, size_loc;
    SgType *type;

    max = 0; el = NULL;
    if (!red_op_list) return(max);

    //looking through the reduction_op_list
    for (er = red_op_list, rsl = red_struct_list; er; er = er->rhs(), rsl = rsl->next)
    {
        ered = er->lhs();    //  reduction  (variant==ARRAY_OP)
        ev = ered->rhs(); // reduction variable reference for reduction operations except MINLOC,MAXLOC 

        if (isSgExprListExp(ev))
        {
            el = ev->rhs()->lhs();
            en = ev->rhs()->rhs()->lhs();

            ev = ev->lhs(); // reduction variable reference              
        }
        type = ev->symbol()->type();

        if (isSgArrayType(type))
            type = type->baseType();

        size = TypeSize(type);
        //esize = TypeSizeCExpr(type);
        if (rsl->redvar_size > 0) // reduction variable is array
        {
            if (options.isOn(C_CUDA))
                size = size;
            else
                size = size * rsl->redvar_size;
        }

        if (el) // MAXLOC,MINLOC
        {
            num_el = rsl->number;
            // calculation number of location array 
            // ec = Calculate(en);
            // if(ec->isInteger())
            //  num_el = ec->valueInteger();

            type = el->symbol()->type();
            if (isSgArrayType(type))
                type = type->baseType();

            size_loc = TypeSize(type) * num_el;

            //      if(size % 8 == 0)
            //         size_loc = ( size_loc % 8 == 0 ) ? size_loc : (size_loc / 8 ) * 8 + 8;
            //      else if(size % 4 == 0)
            //         size_loc = ( size_loc % 4 == 0 ) ? size_loc : (size_loc / 4 ) * 4 + 4;
            //      else if(size % 2 == 0)
            //         size_loc = ( size_loc % 2 == 0 ) ? size_loc : (size_loc / 2 ) * 2 + 2;

            size = size + size_loc;
            size = (size % 8 == 0) ? size : (size / 8) * 8 + 8;
        }
        max = (max < size) ? size : max;
    }
    return(max);
}


SgExpression *CreateBlocksThreadsSpec(int size, SgSymbol *s_blocks, SgSymbol *s_threads, SgSymbol *s_stream, SgSymbol *s_shared_mem)
{
    SgExprListExp *el, *ell, *elm;
    SgExpression *mult;
    el = new SgExprListExp(*new SgVarRefExp(s_blocks));
    ell = new SgExprListExp(*new SgVarRefExp(s_threads));
    el->setRhs(ell);
    //size==0  - parallel loop without reduction clause
    // size - shared memory size per one thread
    if (size)
        mult = new SgVarRefExp(s_shared_mem);
    else
        mult = new SgValueExp(size);
    elm = new SgExprListExp(*mult); //shared memory size per one block
    ell->setRhs(elm);
    ell = new SgExprListExp(*new SgVarRefExp(s_stream));
    elm->setRhs(ell);
    return((SgExpression *)el);
}

SgExpression *MallocExpr(SgSymbol *var, SgExpression *eldim)
{
    SgExpression *e, *el;
    //e = new SgValueExp(TypeSize(var->type()->baseType()));
    e = &SgSizeOfOp(*new SgTypeRefExp(*C_Type(var->type()->baseType())));
    for (el = eldim; el; el = el->rhs())                       // sizeof(<red-var-type>)* *N1...* *Nk
        e = &(*e * el->lhs()->copy());
    e = mallocFunction(e, block_C);                 // malloc(sizeof(<red-var-type>)* *N1...* *Nk)
    e = new SgCastExp(*C_PointerType(C_Type(var->type()->baseType())), *e);
    // (<red-var-type> *) malloc(sizeof(<red-var-type>)* *N1...* *Nk)
    return(e);
}

int NumberOfCoeffs(SgSymbol *sg)
{
    SgArrayType *typearray;
    SgExpression *esize;
    int d;
    typearray = isSgArrayType(sg->type());
    if (!typearray) return(0);
    esize = typearray->sizeInDim(0);
    if (((SgValueExp *)esize)->intValue() == 0) return(0);  //remote_acces buffer of 1 element
    d = options.isOn(AUTO_TFM) ? 0 : 1; //inparloop ? 0 : 1;  //ACROSS_MOD_IN_KERNEL ? 0 : 1; //WithAcrossClause() 
    return(((SgValueExp *)esize)->intValue() - DELTA - d);
}

SgStatement * makeSymbolDeclaration(SgSymbol *s)
{
    SgStatement * st;

    st = new SgStatement(VAR_DECL);
    st->setExpression(0, *new SgExprListExp(*SgMakeDeclExp(s, s->type())));

    return(st);
}

SgStatement * makeExternSymbolDeclaration(SgSymbol *s)
{
    SgStatement * st;

    st = new SgStatement(VAR_DECL);

    st->setExpression(0, *new SgExprListExp(*SgMakeDeclExp(s, new SgDescriptType(*s->type(), BIT_EXTERN))));

    return(st);
}

SgStatement * makeSymbolDeclarationWithInit(SgSymbol *s, SgExpression *einit)
{
    SgStatement * st;
    SgExpression *e;
    st = new SgStatement(VAR_DECL);
    e = &SgAssignOp(*SgMakeDeclExp(s, s->type()), *einit);
    st->setExpression(0, *new SgExprListExp(*e));

    return(st);
}

//       stmt = makeSymbolDeclaration_T(st_hedr);
//       st_end->insertStmtBefore(*stmt,*st_hedr);

SgStatement * makeSymbolDeclaration_T(SgStatement *st_hedr)
{
    SgStatement * st;
    SgExpression *e;
    SgSymbol *s;
    SgSymbol * sc = new SgSymbol(VARIABLE_NAME, "cuda_ptr", *C_PointerType(SgTypeFloat()), *st_hedr);
    st = new SgStatement(VAR_DECL);
    SgDerivedCollectionType *tmpT = new SgDerivedCollectionType(*new SgSymbol(VARIABLE_NAME, "device_ptr"), *SgTypeFloat());
    s = new SgSymbol(VARIABLE_NAME, "dev_ptr", *tmpT, *st_hedr);

    e = new SgExpression(CLASSINIT_OP);
    e->setLhs(SgMakeDeclExp(s, s->type()));
    e->setRhs(new SgExprListExp(*new SgVarRefExp(sc)));
    st->setExpression(0, *new SgExprListExp(*e));

    return(st);
}


SgExpression * addDeclExpList(SgSymbol *s, SgExpression *el)
{
    SgExpression *e, *l;
    e = new SgExprListExp(*SgMakeDeclExp(s, s->type()));
    for (l = el; l->rhs(); l = l->rhs())
        ;
    l->setRhs(e);
    return(e);

}

SgExpression *UsedValueRef(SgSymbol *susg, SgSymbol *s)
{
    if (isSgArrayType(susg->type()))
        Error("Array %s is used in loop, not implemented yet for GPU", susg->identifier(), 591, first_do_par);
    if (susg->type()->variant() == T_DERIVED_TYPE)
        Error("Variable %s of derived type is used in loop, not implemented yet for GPU", susg->identifier(), 590, first_do_par);
    return(new SgVarRefExp(s));
}

char *Cuda_LoopHandlerComment()
{
    char *cmnt = new char[100];
    sprintf(cmnt, "//    CUDA handler for loop on line %d \n", first_do_par->lineNumber());
    //sprintf(cmnt,"//********************* CUDA handler for loop on line %d *********************\n",first_do_par->lineNumber());
    return(cmnt);
}

char *Cuda_SequenceHandlerComment(int lineno)
{
    char *cmnt = new char[150];
    sprintf(cmnt, "//    CUDA handler for sequence of statements on line %d \n", lineno);
    //sprintf(cmnt,"//********************* CUDA handler for sequence of statements on line %d *********************\n",first_do_par->lineNumber());
    return(cmnt);
}

SgExpression *dim3FunctionCall(int i)
{
    SgFunctionCallExp *fe = new SgFunctionCallExp(*fdim3);

    fe->addArg(*new SgValueExp(i));
    fe->addArg(*new SgValueExp(i));
    fe->addArg(*new SgValueExp(i));
    return fe;
}

char *RegisterConstName()
{
    char *name = new char[strlen(kernel_symb->identifier()) + 6];
    name[0] = '\0';
    strcat(name, aks_strupr(kernel_symb->identifier()));
    strcat(name, "_REGS");
    return(name);

}

char *Up_regs_Symbol_Name(SgSymbol *s_regs)
{
    char *name = new char[strlen(s_regs->identifier()) + 1];
    name[0] = '\0';
    strcat(name, aks_strupr(s_regs->identifier()));
    return(name);

}

void GenerateStmtsForInfoFile()
{
    SgStatement *stmt, *end_if_dir;
    char *define_name;
    symb_list *sl;
    //SgSymbol *s_regs_info;
    if (!RGname_list || !info_block)
        return;
    for (sl = RGname_list; sl; sl = sl->next)
    {
        // generating for info_block

        end_if_dir = endif_dir();
        info_block->insertStmtAfter(*end_if_dir, *info_block);
        define_name = Up_regs_Symbol_Name((sl->symb));
        stmt = ifdef_dir(define_name);
        end_if_dir->insertStmtBefore(*stmt, *info_block);
        //s_regs_info = &(sl->symb->copy());
        //SYMB_SCOPE(sl->symb->thesymb) = info_block->thebif; 
        stmt = makeSymbolDeclarationWithInit(sl->symb, new SgVarRefExp(new SgSymbol(VARIABLE_NAME, define_name)));
        end_if_dir->insertStmtBefore(*stmt, *info_block);
        stmt = else_dir();
        end_if_dir->insertStmtBefore(*stmt, *info_block);
        stmt = makeSymbolDeclarationWithInit(sl->symb, new SgValueExp(0));
        end_if_dir->insertStmtBefore(*stmt, *info_block);
    }

}

void GenerateEndIfDir()
{
    if (block_C)
        block_C->addComment("#endif\n");
}

void GenerateDeclarationDir()
{
    if (block_C)
        block_C->addComment(declaration_cmnt);
}

#undef Nintent
#undef DELTA 
#undef Nhandler 
#undef SAVE_LABEL_ID

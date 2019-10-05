#include "dvm.h"
#include "aks_structs.h"
#include "acc_data.h"

using namespace std;

// all flags
#define LongT C_DvmType()
#define debugMode 0
#define kerneloff 0

// extern variables
extern reduction_operation_list *red_struct_list;
extern symb_list *shared_list, *acc_func_list;
extern symb_list *RGname_list;
extern symb_list *acc_call_list;
extern vector<SgSymbol *> loopVars;

// extern functions
extern SgStatement *Create_C_Function(SgSymbol*);
extern SgExpression *RedPost(SgSymbol*, SgSymbol*, SgSymbol*, SgSymbol*);
extern SgSymbol *GreedSymbolForRedInAdapter(SgSymbol *, SgStatement *);
extern SgSymbol *GpuHeaderSymbolInAdapter(SgSymbol *, SgStatement *);
extern SgSymbol *GpuBaseSymbolInAdapter(SgSymbol *, SgStatement *);
extern SgExpression *CudaReplicate(SgSymbol *, SgSymbol *, SgSymbol *, SgSymbol *);
extern SgStatement *IncludeLine(char*);
extern void optimizeLoopBodyForOne(vector<newInfo> &allNewInfo);
extern void searchIdxs(vector<acrossInfo> &allInfo, SgExpression *st);

// local functions
ArgsForKernel **Create_C_Adapter_Function_Across_variants(SgSymbol*, SgSymbol*, const int, const int, const int, SageSymbols**, SageSymbols**);
ArgsForKernel **Create_C_Adapter_Function_Across_OneThread(SgSymbol*, SgSymbol*, const int, const int);
symb_list* AddToSymbList(symb_list*, SgSymbol*);
symb_list* AddNewToSymbList(symb_list*, SgSymbol*);
void CreateReductionBlocksAcross(SgStatement*, int, SgExpression*, SgSymbol*);
void CompleteStructuresForReductionInKernelAcross(void);
void DeclarationOfReductionBlockInKernelAcross(SgExpression *ered, reduction_operation_list *rsl);
void DeclarationCreateReductionBlocksAcross(int, SgExpression*);
AnalyzeReturnGpuO1 analyzeLoopBody(int type);

// local static variables
static SgSymbol *red_first;
static bool declaration_include = true;
static bool createBodyKernel = false;
static bool createConvert_XY = true;
static const int numLoopVars = 16;
static bool ifReadLvlMode = false;
static vector <stack <SgStatement*> > copyOfBody;
static vector<SgSymbol*> allRegNames;
static unsigned countOfCopies;
static vector<ParamsForAllVariants> allVariants;

static const char *funcDvmhConvXYfortVer = "       attributes(device) subroutine dvmh_convert_XY_int(x,y,Rx,Ry,slash,idx)\n      implicit none\n      integer ,value:: x\n      integer ,value:: y\n      integer ,value:: Rx\n      integer ,value:: Ry\n      integer ,value:: slash\n      integer ,device:: idx  \n	  \n      if(slash .eq. 0) then\n	     if(Rx .eq. Ry) then\n		    if(x + y .lt. Rx) then\n			   idx = y + (1+x+y)*(x+y)/2\n            else\n			   idx = Rx*(Rx-1)+x-(2*Rx-x-y-1)*(2*Rx-x-y-2)/2\n            endif		 \n         elseif(Rx .lt. Ry) then\n		    if(x + y .lt. Rx) then\n               idx = y + ((1+x+y)*(x+y)) / 2\n            elseif(x + y .lt. Ry) then\n               idx = ((1+Rx)*Rx) / 2 + Rx - x - 1 + Rx * (x+y-Rx)\n            else\n               idx = Rx*Ry-Ry+y-(((Rx+Ry-y-x-1)*(Rx+Ry-y-x-2))/2)\n            endif\n         else\n		    if(x + y .lt. Ry) then\n                idx = x + (1+x+y)*(x+y) / 2\n            elseif(x + y .lt. Rx) then\n                idx = (1+Ry)*Ry/2 + (Ry-y-1) + Ry * (x+y-Ry)\n            else\n                idx = Rx*Ry-Rx+x-((Rx+Ry-y-x-1)*(Rx+Ry-y-x-2)/2)\n            endif\n         endif\n      else\n	     if(Rx .eq. Ry) then\n            if(x + Rx-1-y .lt. Rx) then\n                idx = Rx-1-y + (x+Rx-y)*(x+Rx-1-y)/2\n            else\n                idx = Rx*(Rx-1) + x - (Rx-x+y)*(Rx-x+y-1)/2\n            endif\n         elseif(Rx .lt. Ry) then\n            if(x + Ry-1-y .lt. Rx) then		\n                idx = Ry-1-y + ((x+Ry-y)*(x+Ry-1-y)) / 2\n            elseif(x + Ry-1-y .lt. Ry) then\n                idx = ((1+Rx)*Rx)/2+Rx-x-1+Rx*(x+Ry-1-y-Rx)\n            else\n                idx = Rx*Ry-1-y-(((Rx+y-x)*(Rx+y-x-1))/2)\n            endif\n         else\n            if(x + Ry-1-y .lt. Ry) then\n                idx = x + (1+x+Ry-1-y)*(x+Ry-1-y)/2\n            elseif(x + Ry-1-y .lt. Rx) then\n                idx = (1+Ry)*Ry/2 + y + Ry * (x-y-1)\n            else\n                idx = Rx*Ry-Rx+x-((Rx+y-x)*(Rx+y-x-1)/2)\n            endif\n         endif\n      endif\n      end subroutine\n";
static const char *funcDvmhConvXYfortVerLong = "       attributes(device) subroutine dvmh_convert_XY_llong(x,y,Rx,Ry,slash,idx)\n      implicit none\n      integer*8 ,value:: x\n      integer*8 ,value:: y\n      integer*8 ,value:: Rx\n      integer*8 ,value:: Ry\n      integer*8 ,value:: slash\n      integer*8 ,device:: idx  \n	  \n      if(slash .eq. 0) then\n	     if(Rx .eq. Ry) then\n		    if(x + y .lt. Rx) then\n			   idx = y + (1+x+y)*(x+y)/2\n            else\n			   idx = Rx*(Rx-1)+x-(2*Rx-x-y-1)*(2*Rx-x-y-2)/2\n            endif		 \n         elseif(Rx .lt. Ry) then\n		    if(x + y .lt. Rx) then\n               idx = y + ((1+x+y)*(x+y)) / 2\n            elseif(x + y .lt. Ry) then\n               idx = ((1+Rx)*Rx) / 2 + Rx - x - 1 + Rx * (x+y-Rx)\n            else\n               idx = Rx*Ry-Ry+y-(((Rx+Ry-y-x-1)*(Rx+Ry-y-x-2))/2)\n            endif\n         else\n		    if(x + y .lt. Ry) then\n                idx = x + (1+x+y)*(x+y) / 2\n            elseif(x + y .lt. Rx) then\n                idx = (1+Ry)*Ry/2 + (Ry-y-1) + Ry * (x+y-Ry)\n            else\n                idx = Rx*Ry-Rx+x-((Rx+Ry-y-x-1)*(Rx+Ry-y-x-2)/2)\n            endif\n         endif\n      else\n	     if(Rx .eq. Ry) then\n            if(x + Rx-1-y .lt. Rx) then\n                idx = Rx-1-y + (x+Rx-y)*(x+Rx-1-y)/2\n            else\n                idx = Rx*(Rx-1) + x - (Rx-x+y)*(Rx-x+y-1)/2\n            endif\n         elseif(Rx .lt. Ry) then\n            if(x + Ry-1-y .lt. Rx) then		\n                idx = Ry-1-y + ((x+Ry-y)*(x+Ry-1-y)) / 2\n            elseif(x + Ry-1-y .lt. Ry) then\n                idx = ((1+Rx)*Rx)/2+Rx-x-1+Rx*(x+Ry-1-y-Rx)\n            else\n                idx = Rx*Ry-1-y-(((Rx+y-x)*(Rx+y-x-1))/2)\n            endif\n         else\n            if(x + Ry-1-y .lt. Ry) then\n                idx = x + (1+x+Ry-1-y)*(x+Ry-1-y)/2\n            elseif(x + Ry-1-y .lt. Rx) then\n                idx = (1+Ry)*Ry/2 + y + Ry * (x-y-1)\n            else\n                idx = Rx*Ry-Rx+x-((Rx+y-x)*(Rx+y-x-1)/2)\n            endif\n         endif\n      endif\n      end subroutine\n" ;

// local variables
SgStatement *kernelScope, *block;

static inline int pow(int n)
{
    int tmp = 1;
    tmp = tmp << n;
    return tmp;
}

static void setDvmDebugLvl()
{
    char *s = getenv("DVMH_LOGLEVEL");
    if (!ifReadLvlMode && s != NULL)
    {
        sscanf(s, "%d", &DVM_DEBUG_LVL);
        ifReadLvlMode = true;
    }
}

static inline void mywarn(const char *str)
{
#if debugMode
        printf("%s\n", str);
#endif    
}

static char *getLoopLine(const char *sadapter)
{
    char *newLine = new char[strlen(sadapter) + 16];
    newLine[0] = '\0';
    strcat(newLine, "loop on line ");
    int k = (int)strlen(newLine);
    int i = (int)strlen(sadapter) - 1 - 6;

    for (; sadapter[i] != '_'; i--);

    for (i++; sadapter[i] != '_'; i++, k++)
    {
        newLine[k] = sadapter[i];
    }
    newLine[k] = '\\';
    newLine[k + 1] = 'n';
    newLine[k + 2] = '\0';
    return newLine;
}

// generating function call (specially for across):
//loop_cuda_register_red(DvmhLoopRef *InDvmhLoop, DvmType InRedNum, void **ArrayPtr, void **LocPtr)
static SgExpression *RegisterReduction_forAcross(SgSymbol *s_loop_ref, SgSymbol *s_var_num, SgSymbol *s_red, SgSymbol *s_loc)
{
    SgExpression *eloc;
    SgFunctionCallExp *fe = new SgFunctionCallExp(*fdvm[RED_CUDA]);

    fe->addArg(*new SgVarRefExp(s_loop_ref));

    fe->addArg(*new SgVarRefExp(s_var_num));
    fe->addArg(*new SgCastExp(*C_PointerType(C_PointerType(SgTypeVoid())), SgAddrOp(*new SgVarRefExp(*s_red))));
    if (s_loc != NULL)
        eloc = &(SgAddrOp(*new SgVarRefExp(*s_loc)));
    else
        eloc = new SgValueExp(0);
    fe->addArg(*eloc);

    return fe;
}

SgStatement* makeSymbolDeclarationWithInit_T(SgSymbol *Init, SgSymbol *Value)
{
    SgStatement *st;
    SgExpression *e;
    st = new SgStatement(VAR_DECL);

    e = new SgExpression(CLASSINIT_OP);
    e->setLhs(SgMakeDeclExp(Init, Init->type()));
    e->setRhs(new SgExprListExp(*new SgVarRefExp(Value)));
    st->setExpression(0, *new SgExprListExp(*e));

    return st;
}

SgExpression *CreateBlocksThreadsSpec(SgSymbol *s_shared, SgSymbol *s_blocks, SgSymbol *s_threads, SgSymbol *s_stream)
{
    SgExprListExp *el, *ell, *elm;
    SgExpression *mult;
    el = new SgExprListExp(*new SgVarRefExp(s_blocks));
    ell = new SgExprListExp(*new SgVarRefExp(s_threads));
    el->setRhs(ell);
    mult = new SgVarRefExp(s_shared);
    elm = new SgExprListExp(*mult);
    ell->setRhs(elm);
    ell = new SgExprListExp(*new SgVarRefExp(s_stream));
    elm->setRhs(ell);
    return ((SgExpression *)el);
}

SgExpression* CreateBlocksThreadsSpec(int size, SgSymbol *s_blocks, SgSymbol *s_threads)
{
    SgExprListExp *el, *ell, *elm;
    SgExpression *mult;

    el = new SgExprListExp(*new SgVarRefExp(s_blocks));
    ell = new SgExprListExp(*new SgVarRefExp(s_threads));
    el->setRhs(ell);
    //size==0  - parallel loop without reduction clause  
    mult = size ? &((*ThreadsGridSize(s_threads)) * (*new SgValueExp(size))) : new SgValueExp(size);
    elm = new SgExprListExp(*mult);
    ell->setRhs(elm);
    return((SgExpression *)el);
}

SgExpression* CreateBlocksThreadsSpec(SgSymbol *s_blocks, SgSymbol *s_threads)
{
    SgExprListExp *el, *ell;
    el = new SgExprListExp(*new SgVarRefExp(s_blocks));
    ell = new SgExprListExp(*new SgVarRefExp(s_threads));
    el->setRhs(ell);
    return((SgExpression *)el);
}

static void getDefaultCudaBlock(int &x, int &y, int &z, int loopDep, int loopIndep)
{
    if (options.isOn(AUTO_TFM))
    {
        if (loopDep == 0)
        {
            if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 14; z = 1;
            } else {
                x = 32; y = 7; z = 2;
            }
        }
        else if (loopDep == 1)
        {
            if (loopIndep == 0) {
                x = 1; y = 1; z = 1;
            } else if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 5; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        }
        else if (loopDep == 2)
        {
            if (loopIndep == 0) {
                x = 32; y = 1; z = 1;
            } else if (loopIndep == 1){
                x = 32; y = 4; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        }
        else if (loopDep >= 3)
        {
            if (loopIndep == 0) {
                x = 32; y = 5; z = 1;
            } else {
                x = 32; y = 5; z = 2;
            }
        }
    }
    else
    {
        if (loopDep == 0)
        {
            if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 14; z = 1;
            } else {
                x = 32; y = 7; z = 2;
            }
        }
        else if (loopDep == 1)
        {
            if (loopIndep == 0) {
                x = 1; y = 1; z = 1;
            } else if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 8; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        }
        else if (loopDep == 2)
        {
            if (loopIndep == 0) {
                x = 32; y = 1; z = 1;
            } else if (loopIndep == 1) {
                x = 32; y = 4; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        }
        else if (loopDep >= 3)
        {
            if (loopIndep == 0) {
                x = 8; y = 4; z = 1;
            } else {
                x = 8; y = 4; z = 2;
            }
        }
    }
}

static const char *getKeyWordType(SgType *inType)
{
    const char *ret = NULL;

    if (inType->baseType()->variant() == SgTypeFloat()->variant())
        ret = "float";
    else if (inType->baseType()->variant() == SgTypeDouble()->variant())
        ret = "double";
    else if (inType->baseType()->variant() == SgTypeInt()->variant())
        ret = "int";
    else if (inType->baseType()->variant() == SgTypeBool()->variant())
        ret = "bool";
    else if (inType->baseType()->variant() == SgTypeChar()->variant())
        ret = "char";
    else if (inType->baseType()->variant() == SgTypeVoid()->variant())
        ret = "void";
    return ret;
}

static int getSizeOf()
{
    unsigned ret = 1;
    for (SgExpression *er = red_list; er; er = er->rhs())
    {
        SgExpression *red_expr_ref = er->lhs()->rhs(); // reduction variable reference
        SgType *inType = red_expr_ref->type();
        if (inType->variant() == SgTypeFloat()->variant())
            ret = MAX(ret, sizeof(float));
        else if (inType->variant() == SgTypeDouble()->variant())
            ret = MAX(ret, sizeof(double));
        else if (inType->variant() == SgTypeInt()->variant())
            ret = MAX(ret, sizeof(int));
        else if (inType->variant() == SgTypeBool()->variant())
            ret = MAX(ret, sizeof(bool));
        else if (inType->variant() == SgTypeChar()->variant())
            ret = MAX(ret, sizeof(char));
    }
    return (int)ret;
}

SgStatement *CreateKernelProcedureDevice(SgSymbol *skernel)
{
    SgStatement *st, *st_end;
    SgExpression *e;

    st = new SgStatement(PROC_HEDR);
    st->setSymbol(*skernel);
    e = new SgExpression(ACC_ATTRIBUTES_OP, new SgExpression(ACC_DEVICE_OP), NULL, NULL);
    //e ->setRhs(new SgExpression(ACC_GLOBAL_OP));
    st->setExpression(2, *e);
    st_end = new SgStatement(CONTROL_END);
    st_end->setSymbol(*skernel);

    cur_in_mod->insertStmtAfter(*st, *mod_gpu);
    st->insertStmtAfter(*st_end, *st);
    st->setVariant(PROS_HEDR);

    cur_in_mod = st_end;

    return st;
}

SgStatement* AssignStatement(SgExpression &lhs, SgExpression &rhs)
{
    SgStatement *st;
    if (options.isOn(C_CUDA))
        st = new SgCExpStmt(SgAssignOp(lhs, rhs));
    else
        st = new SgAssignStmt(lhs, rhs);
    return st;
}

SgSymbol *createVariantOfSAdapter(SgSymbol *sadapter, char *variant)
{
    SgSymbol *s_adapter, *s_tmp;
    char *oldName = sadapter->identifier();
    char *correctName = new char[strlen(oldName) + strlen(variant) + 1];
    correctName[0] = '\0';
    strcat(correctName, oldName);
    strcat(correctName, variant);
    s_adapter = new SgSymbol(FUNCTION_NAME, correctName, *C_VoidType(), *block_C);
    s_tmp = new SgSymbol(PROCEDURE_NAME, correctName, *current_file->firstStatement());    

    return s_adapter;
}

SgSymbol *createVariantOfKernelSymbol(SgSymbol *kernel_symb, char *variant)
{
    SgSymbol *sk;
    char *oldName = kernel_symb->identifier();
    char *correctName = new char[strlen(oldName) + strlen(variant) + 1];
    correctName[0] = '\0';
    strcat(correctName, oldName);
    strcat(correctName, variant);

    sk = new SgSymbol(PROCEDURE_NAME, correctName, *mod_gpu);
    if (options.isOn(C_CUDA))
        sk->setType(C_VoidType());
    return sk;
}

void createNewAdapter(SgSymbol *sadapter, ParamsForAllVariants &newVar, char *str)
{
    SgSymbol *s_adapter;
    char *nameOfNewSAdapter;

    nameOfNewSAdapter = new char[strlen(sadapter->identifier()) + strlen(str) + 1];
    nameOfNewSAdapter[0] = '\0';
    strcat(nameOfNewSAdapter, sadapter->identifier());
    s_adapter = createVariantOfSAdapter(sadapter, str);
    strcat(nameOfNewSAdapter, str);
    newVar.nameOfNewSAdapter = nameOfNewSAdapter;
    newVar.s_adapter = s_adapter;
}

void createNewKernel(SgSymbol *kernel_symb, ParamsForAllVariants &newVar, char *str)
{
    SgSymbol *s_ks;
    char *nameOfNewSK;

    nameOfNewSK = new char[strlen(kernel_symb->identifier()) + strlen(str) + 1];
    nameOfNewSK[0] = '\0';
    strcat(nameOfNewSK, kernel_symb->identifier());
    s_ks = createVariantOfKernelSymbol(kernel_symb, str);
    strcat(nameOfNewSK, str);
    newVar.nameOfNewKernelSymb = nameOfNewSK;
    newVar.s_kernel_symb = s_ks;
}

static SgSymbol* getSymbByNum(SageSymbols *allSymb, int place)
{
    SageSymbols *tmp = allSymb;
    for (int i = 0; ; ++i)
    {
        if (i == place)
            return tmp->symb;
        else
            tmp = tmp->next;
    }
}

static int getLongByType(int type[], int num)
{
    int ret = 0;
    int p = 1;
    for (int i = num - 1; i >= 0; i--)
    {
        ret += type[i] * p;
        p = p << 1;
    }
    return ret;
}

static int countBit(int num)
{
    int ret = 0;
    while (num != 0)
    {
        if ((num & 1) == 1)
            ret++;
        num = num >> 1;
    }
    return ret;
}

static void generateAllBitmasks(int dep, int all, vector<int> &out)
{
    if (dep == all)
        out.push_back(pow(all) - 1);
    else
    {
        int maxVar = pow(all);
        for (int i = 1; i < maxVar; ++i)
        {
            if (countBit(i) == dep)
                out.push_back(i);
        }
    }
}

static void GetAllCombinations2(vector<ParamsForAllVariants> &allVariants, SgSymbol *sadapter, SgSymbol *kernel_symb, int numAcr, int sizeOfAllSymb, SageSymbols *allSymb)
{
    int *bitmask = new int[(unsigned)sizeOfAllSymb];

    char *tmpstrAdapter = new char[16];
    char *tmpstrKernel = new char[16];
    tmpstrAdapter[0] = '\0';
    tmpstrKernel[0] = '\0';

    ParamsForAllVariants newVar;
    newVar.allDims = sizeOfAllSymb;
    newVar.loopSymb = new SageSymbols*[numLoopVars];
    newVar.loopAcrossSymb = new SageSymbols*[numLoopVars];
    newVar.nameOfNewSAdapter = NULL;
    newVar.s_adapter = NULL;
    newVar.acrossV = numAcr;
    newVar.loopV = newVar.allDims - newVar.acrossV;
    for (int bit = 0; bit < sizeOfAllSymb; ++bit)
    {
        if (bit > sizeOfAllSymb - numAcr - 1)
            bitmask[bit] = 1;
        else
            bitmask[bit] = 0;
    }
    newVar.type = getLongByType(bitmask, sizeOfAllSymb);

    sprintf(tmpstrAdapter, "%d", newVar.type);
    strcat(tmpstrAdapter, "_case");
    sprintf(tmpstrKernel, "_%d", newVar.type);
    strcat(tmpstrKernel, "_case");

    createNewAdapter(sadapter, newVar, tmpstrAdapter);
    createNewKernel(kernel_symb, newVar, tmpstrKernel);

    int k = 0;
    for (int r = 0; r < sizeOfAllSymb; ++r)
    {
        if (r < numAcr)
        {
            newVar.loopAcrossSymb[r] = new SageSymbols();
            newVar.loopAcrossSymb[r]->across_left = newVar.loopAcrossSymb[r]->across_right = 0;
            newVar.loopAcrossSymb[r]->symb = getSymbByNum(allSymb, sizeOfAllSymb - r - 1);
            newVar.loopAcrossSymb[r]->len = sizeOfAllSymb - r - 1;
        }
        else
        {
            newVar.loopSymb[k] = new SageSymbols();
            newVar.loopSymb[k]->across_left = newVar.loopSymb[k]->across_right = 0;
            newVar.loopSymb[k]->symb = getSymbByNum(allSymb, sizeOfAllSymb - r - 1);
            newVar.loopSymb[k]->len = sizeOfAllSymb - r - 1;
            k++;
        }
    }
    allVariants.push_back(newVar);

    delete[]bitmask;
}

static void GetAllVariants2(vector<ParamsForAllVariants> &allVariants, SgSymbol *sadapter, SgSymbol *kernel_symb)
{
    int allDims = 0, acrossV = 0;

    SageAcrossInfo *Info = GetLoopsWithParAndAcrDir();
    SageSymbols *allSymb = GetSymbInParalell(&allDims, dvm_parallel_dir->expr(2));
    SageArrayIdxs *idxInfo = Info->idx->next;
    while (idxInfo && (acrossV < allDims))
    {
        for (int i = 0; i < idxInfo->dim && (acrossV < allDims); ++i)
        {
            if (idxInfo->symb[i]->across_left != 0 || idxInfo->symb[i]->across_right != 0)
                acrossV++;
        }
        idxInfo = idxInfo->next;
    }

    // correct dependencies lvl only for ACROSS with one dep
    SgStatement *st = loop_body;
    
    SgExpression* dvmDir = dvm_parallel_dir->expr(1);
    vector<acrossInfo> allInfo;
    bool nextStep = true;
    loopVars.clear();

    while (dvmDir)
    {
        SgExpression *t = dvmDir->lhs();
        if (t->variant() == ACROSS_OP)
        {
            vector<SgExpression*> toAnalyze;
            SgExpression* list = t->lhs();
            while (list)
            {
                if (list->lhs()->variant() == ARRAY_REF)
                    toAnalyze.push_back(list->lhs());
                else if (list->lhs()->variant() == ARRAY_OP)
                {
                    if (list->lhs()->lhs()->variant() == ARRAY_REF)
                        toAnalyze.push_back(list->lhs()->lhs());
                }
                list = list->rhs();
            }
            
            for (int i = 0; i < toAnalyze.size(); ++i)
            {
                SgExpression* array = toAnalyze[i];

                acrossInfo tmpI;
                tmpI.nameOfArray = array->symbol()->identifier();
                tmpI.symbol = array->symbol();
                tmpI.allDim = 0;
                tmpI.widthL = 0;
                tmpI.widthR = 0;
                tmpI.acrossPos = 0;
                tmpI.acrossNum = 0;

                SgExpression* tt = array->lhs();
                int position = 0;
                while (tt)
                {
                    bool here = true;
                    if (tt->lhs()->lhs()->valueInteger() != 0)
                    {
                        tmpI.acrossPos = position;
                        tmpI.acrossNum++;
                        tmpI.widthL = (-1) * tt->lhs()->lhs()->valueInteger();
                        here = false;
                    }

                    if (tt->lhs()->rhs()->valueInteger() != 0)
                    {
                        tmpI.acrossPos = position;
                        if (here)
                            tmpI.acrossNum++;
                        tmpI.widthR = tt->lhs()->rhs()->valueInteger();
                    }
                    position++;
                    tt = tt->rhs();
                }

                for (int i = 0; i < position; ++i)
                {
                    tmpI.dims.push_back(0);
                    tmpI.symbs.push_back(NULL);
                }
                allInfo.push_back(tmpI);
            }
            break;
        }
        dvmDir = dvmDir->rhs();
    }

    for (size_t i = 0; i < allInfo.size(); ++i)
    {
        if (allInfo[i].acrossNum > 1)
        {
            nextStep = false;
            break;
        }
    }
    
    if (nextStep)
    {
        SgExpression* dvmDir = dvm_parallel_dir->expr(2);
        while (dvmDir)
        {
            loopVars.push_back(dvmDir->lhs()->symbol());
            dvmDir = dvmDir->rhs();
        }

        while (st)
        {
            for (int i = 0; i < 3; ++i)
                if (st->expr(i))
                    searchIdxs(allInfo, st->expr(i));
            st = st->lexNext();
        }

        for (size_t i = 0; i < allInfo.size(); ++i)
        {
            if (allInfo[i].symbs[allInfo[i].acrossPos] == NULL)
            {
                nextStep = false;
                break;
            }
        }

        if (nextStep)
        {
            vector<char*> uniqSymbs;

            uniqSymbs.push_back(allInfo[0].symbs[allInfo[0].acrossPos]->identifier() );
            for (size_t i = 1; i < allInfo.size(); ++i)
            {
                bool uniq = true;
                char *cmpd = allInfo[i].symbs[allInfo[i].acrossPos]->identifier();
                for (size_t k = 0; k < uniqSymbs.size(); ++k)
                {
                    if (strcmp(uniqSymbs[k], cmpd) == 0)
                    {
                        uniq = false;
                        break;
                    }
                }
                if (uniq)
                {
                    uniqSymbs.push_back(cmpd);
                }
            }

            acrossV = MIN((int)uniqSymbs.size(), allDims);
        }
    }
    for (int i = 1; i <= acrossV; ++i)
        GetAllCombinations2(allVariants, sadapter, kernel_symb, i, allDims, allSymb);
}

/*void printAllVars(vector<ParamsForAllVariants> &vectorT)
{
    for (size_t i = 0; i < vectorT.size(); ++i)
    {
        printf("acrossV = %d  loopV = %d  alldims = %d\n", vectorT[i].acrossV, vectorT[i].loopV, vectorT[i].allDims);
        printf("nameOfKernel = %s  nameOfAdapt = %s  \n", vectorT[i].nameOfNewKernelSymb, vectorT[i].nameOfNewSAdapter);
        for (int k = 0; k < vectorT[i].loopV; ++k)
        {
            printf("%s, L = %d, R = %d, len= %d\n", vectorT[i].loopSymb[k]->symb->identifier(), vectorT[i].loopSymb[k]->across_left, vectorT[i].loopSymb[k]->across_right, vectorT[i].loopSymb[k]->len);
        }
        for (int k = 0; k < vectorT[i].acrossV; ++k)
        {
            printf("%s, L = %d, R = %d, len= %d\n", vectorT[i].loopAcrossSymb[k]->symb->identifier(), vectorT[i].loopAcrossSymb[k]->across_left, vectorT[i].loopAcrossSymb[k]->across_right, vectorT[i].loopAcrossSymb[k]->len);
        }
        printf("\n");
    }
    printf("\n");
}*/

ArgsForKernel *Create_C_Adapter_Function_Across(SgSymbol *sadapter)
{
    ArgsForKernel **retValueForKernel = NULL;
    createBodyKernel = true;

    // clear information
    allRegNames.clear(); 

    SgStatement *st_hedr, *st_end, *first_exec, *stmt;
    vector<SgStatement*> cuda_kernel;
    SgExpression *fe, *ae, *el, *arg_list;
    SgType *typ;
    SgSymbol *s_loop_ref, *sarg, *s;
    symb_list *sl;
    vector<SgSymbol *> argsForVariantFunction;

    setDvmDebugLvl();

    mywarn("start: getAllVars");
    allVariants.clear();

    GetAllVariants2(allVariants, sadapter, kernel_symb);
    mywarn("  end: getAllVars");

    cuda_kernel.resize(countKernels);

    if (options.isOn(ONE_THREAD))
    {
        int num = 0;
        SageSymbols *tmpStr = GetSymbInParalell(&num, dvm_parallel_dir->expr(2));
        retValueForKernel = Create_C_Adapter_Function_Across_OneThread(sadapter, kernel_symb, num, 0);

        for (unsigned t = 0; t < countKernels; ++t)
        {
            loop_body = CopyOfBody.top();
            CopyOfBody.pop();

            currentLoop = new Loop(loop_body, options.isOn(OPT_EXP_COMP));

            num = 0;
            tmpStr = GetSymbInParalell(&num, dvm_parallel_dir->expr(2));
            SgType *typeParams = indexTypeInKernel(rtTypes[t]);

            for (int i = 0; i < num; ++i)
            {
                char *str = new char[64];
                char *addL = new char[64];
                str[0] = addL[0] = '\0';
                retValueForKernel[t]->otherVarsForOneTh.push_back(tmpStr->symb);
                strcat(str, tmpStr->symb->identifier());
                strcat(str, "_");

                strcat(addL, str);
                strcat(addL, "low");
                retValueForKernel[t]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, addL, typeParams, kernel_symb->scope()));

                addL[0] = '\0';
                strcat(addL, str);
                strcat(addL, "high");
                retValueForKernel[t]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, addL, typeParams, kernel_symb->scope()));

                addL[0] = '\0';
                strcat(addL, str);
                strcat(addL, "idx");
                retValueForKernel[t]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, addL, typeParams, kernel_symb->scope()));
                tmpStr = tmpStr->next;
            }
            
            string kernel_symbNew = kernel_symb->identifier();
            if (rtTypes[t] == rt_INT)
                kernel_symbNew += "_int";
            else if (rtTypes[t] == rt_LONG)
                kernel_symbNew += "_long";
            else if (rtTypes[t] == rt_LLONG)
                kernel_symbNew += "_llong";
            
            cuda_kernel[t] = CreateLoopKernelAcross(new SgSymbol(FUNCTION_NAME, kernel_symbNew.c_str(), *C_VoidType(), *block_C), retValueForKernel[t], indexTypeInKernel(rtTypes[t]));
            if (options.isOn(RTC))
            {
                acc_call_list = ACC_RTC_ExpandCallList(acc_call_list);
                if (options.isOn(C_CUDA))
                    ACC_RTC_ConvertCudaKernel(cuda_kernel[t], kernel_symbNew.c_str());
                else
                    ACC_RTC_AddCalledProcedureComment(kernel_symb);

                RTC_FKernelArgs.push_back((SgFunctionCallExp*)cuda_kernel[t]->expr(0));
            }

            delete currentLoop;
        }
        if (options.isOn(RTC))
            ACC_RTC_CompleteAllParams();
    }
    else
    {
        mywarn("start: create all VARIANTS");
        // if only type ~ 1 across symb
        bool ifOne = true;
        for (size_t i = 0; i < allVariants.size(); ++i)
        {
            if (allVariants[i].acrossV != 1)
                ifOne = false;

            if ((unsigned)allVariants[i].acrossV == countOfCopies + 1)
                countOfCopies++;
        }
        // set global if true
        if (ifOne)
            dontGenConvertXY = true;
        else
            dontGenConvertXY = false;

        for (size_t i = 0; i < allVariants.size(); ++i)
        {
#if debugMode
                printf("%d case\n", allVariants[i].type);
#endif
            ParamsForAllVariants tmp = allVariants[i];
                        
            for (unsigned k = 0; k < countKernels; ++k)
            {
                loop_body = CopyOfBody.top();
                CopyOfBody.pop();

                if (options.isOn(AUTO_TFM))
                    currentLoop = new Loop(loop_body, true);

                string kernel_symb = tmp.s_kernel_symb->identifier();
                if (rtTypes[k] == rt_INT)
                    kernel_symb += "_int";
                else if (rtTypes[k] == rt_LONG)
                    kernel_symb += "_long";
                else if (rtTypes[k] == rt_LLONG)
                    kernel_symb += "_llong";

                if (tmp.acrossV == 1 && tmp.type == 1)
                {
                    if (k == 0) // create CUDA handler once
                        retValueForKernel = Create_C_Adapter_Function_Across_variants(tmp.s_adapter, tmp.s_kernel_symb, tmp.loopV, tmp.acrossV, tmp.allDims, tmp.loopSymb, tmp.loopAcrossSymb);
                    cuda_kernel[k] = CreateLoopKernelAcross(new SgSymbol(FUNCTION_NAME, kernel_symb.c_str(), *C_VoidType(), *block_C), retValueForKernel[k], tmp.acrossV, indexTypeInKernel(rtTypes[k]));
                    if (options.isOn(RTC))
                        acc_call_list = ACC_RTC_ExpandCallList(acc_call_list);
                }
                else if (tmp.acrossV != 1 && (tmp.type == 3 || tmp.type == 7 || tmp.type > 14))
                {
                    // optimized loop body
                    if (options.isOn(GPU_O1))
                        analyzeLoopBody(ACROSS_TYPE);

                    if (k == 0) // create CUDA handler once
                        retValueForKernel = Create_C_Adapter_Function_Across_variants(tmp.s_adapter, tmp.s_kernel_symb, tmp.loopV, tmp.acrossV, tmp.allDims, tmp.loopSymb, tmp.loopAcrossSymb);
                    cuda_kernel[k] = CreateLoopKernelAcross(new SgSymbol(FUNCTION_NAME, kernel_symb.c_str(), *C_VoidType(), *block_C), retValueForKernel[k], tmp.acrossV, indexTypeInKernel(rtTypes[k]));
                    if (options.isOn(RTC))
                    {
                        acc_call_list = ACC_RTC_ExpandCallList(acc_call_list);
                        if (!options.isOn(C_CUDA) && options.isOn(AUTO_TFM))
                        {
                            if (strstr(kernel_symb.c_str(), "_llong") != NULL)
                                acc_call_list = AddNewToSymbList(acc_call_list, createNewFunctionSymbol("dvmh_convert_XY_llong"));
                            else if (strstr(kernel_symb.c_str(), "_int") != NULL)
                                acc_call_list = AddNewToSymbList(acc_call_list, createNewFunctionSymbol("dvmh_convert_XY_int"));                            
                        }
                    }
                }

                if (newVars.size() != 0)
                {
                    correctPrivateList(RESTORE);
                    newVars.clear();
                }
                if (options.isOn(AUTO_TFM))
                    delete currentLoop;
            }
            if (options.isOn(RTC))
            {
                for (unsigned diff = 0; diff < RTC_FCall.size() / countKernels; ++diff)
                {
                    for (unsigned k = 0; k < countKernels; ++k)
                        RTC_FKernelArgs.push_back((SgFunctionCallExp*)cuda_kernel[k]->expr(0));
                }

                for (unsigned k = 0; k < countKernels; ++k)
                {
                    string kernel_symb = tmp.s_kernel_symb->identifier();
                    if (rtTypes[k] == rt_INT)
                        kernel_symb += "_int";
                    else if (rtTypes[k] == rt_LONG)
                        kernel_symb += "_long";
                    else if (rtTypes[k] == rt_LLONG)
                        kernel_symb += "_llong";
                                        
                    if (options.isOn(C_CUDA))
                        ACC_RTC_ConvertCudaKernel(cuda_kernel[k], kernel_symb.c_str());
                    else
                        ACC_RTC_AddCalledProcedureComment(new SgSymbol(VARIABLE_NAME, kernel_symb.c_str()));
                }

                ACC_RTC_CompleteAllParams();
            }
        }

        
        mywarn("  end: create all VARIANTS");

        //create new control function
        st_hedr = Create_C_Function(sadapter);
        st_end = st_hedr->lexNext();
        fe = st_hedr->expr(0);
        st_hedr->addComment(Cuda_LoopHandlerComment());
        first_exec = st_end;
        mywarn("start: create  dummy argument list ");

        // create  dummy argument list: loop_ref, <dvm-array-headers>, <uses>
        typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
        s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *typ, *st_hedr);
        argsForVariantFunction.push_back(s_loop_ref);

        ae = new SgVarRefExp(s_loop_ref);                 //loop_ref
        ae->setType(typ);
        ae = new SgPointerDerefExp(*ae);
        arg_list = new SgExprListExp(*ae);
        fe->setLhs(arg_list);

        for (sl = acc_array_list; sl; sl = sl->next)  // <dvm-array-headers>
        {
            SgArrayType *typearray = new SgArrayType(*C_DvmType());
            typearray->addDimension(NULL);
            sarg = new SgSymbol(VARIABLE_NAME, sl->symb->identifier(), *typearray, *st_hedr);
            argsForVariantFunction.push_back(sarg);
            ae = new SgArrayRefExp(*sarg);
            ae->setType(*typearray);
            el = new SgExpression(EXPR_LIST);
            el->setLhs(NULL);
            ae->setLhs(*el);
            arg_list->setRhs(*new SgExprListExp(*ae));
            arg_list = arg_list->rhs();
        }

        for (el = uses_list; el; el = el->rhs())    // <uses>    
        {
            s = el->lhs()->symbol();
            typ = C_PointerType(C_Type(s->type()));
            sarg = new SgSymbol(VARIABLE_NAME, s->identifier(), *typ, *st_hedr);
            argsForVariantFunction.push_back(sarg);
            if (isByValue(s))
                SYMB_ATTR(sarg->thesymb) = SYMB_ATTR(sarg->thesymb) | USE_IN_BIT;
            ae = UsedValueRef(s, sarg);
            ae->setType(typ);
            ae = new SgPointerDerefExp(*ae);
            arg_list->setRhs(*new SgExprListExp(*ae));
            arg_list = arg_list->rhs();
        }
        mywarn("  end: create  dummy argument list ");

        mywarn("start: create  IF BLOCK ");
        SgSymbol *which_run = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("which_run"), *C_Type(SgTypeInt()), *st_hedr);
        stmt = makeSymbolDeclaration(which_run);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(which_run), *GetDependencyMask(s_loop_ref)));
        st_end->insertStmtBefore(*stmt, *st_hedr);

        char *str = new char[64];
        str[0] = '\0';

        strcat(str, "which_run in ");
        strncat(str, sadapter->identifier(), strlen(sadapter->identifier()) - 6);
        strcat(str, " is %d\\n");
        SgFunctionCallExp *tmpF2 = new SgFunctionCallExp(*createNewFunctionSymbol("printf"));
        tmpF2->addArg(*new SgValueExp(str));
        tmpF2->addArg(*new SgVarRefExp(which_run));
        if (DVM_DEBUG_LVL > 5)
            st_end->insertStmtBefore(*new SgCExpStmt(*tmpF2), *st_hedr);

        SgSymbol *s_cudaEvent = new SgSymbol(TYPE_NAME, "cudaEvent_t", *block_C);
        SgSymbol *cudaEventStart = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("start"), *C_Derived_Type(s_cudaEvent), *st_hedr);
        SgSymbol *cudaEventStop = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("stop"), *C_Derived_Type(s_cudaEvent), *st_hedr);
        SgSymbol *gpuTime = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("gpuTime"), *SgTypeFloat(), *st_hedr);
        SgSymbol *minGpuTime = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("minGpuTime"), *SgTypeFloat(), *st_hedr);
        SgSymbol *s_i = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("__s_i"), *C_Type(SgTypeInt()), *st_hedr);
        SgSymbol *s_k = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("__s_k"), *C_Type(SgTypeInt()), *st_hedr);
        SgSymbol *min_s_i = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("min_s_i"), *C_Type(SgTypeInt()), *st_hedr);
        SgSymbol *min_s_k = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("min_s_k"), *C_Type(SgTypeInt()), *st_hedr);
        SgSymbol *max_cuda_block = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("__max_cuda_block"), *C_Type(SgTypeInt()), *st_hedr);
        SgWhileStmt *whileSt = NULL;
        SgWhileStmt *whileSt1 = NULL;

        SgIfStmt *if_st;
        vector<vector<int> > allVarForIfBlock;
        vector<SgFunctionCallExp *> allFuncCalls;

        for (size_t k = 0; k < allVariants.size(); ++k)
        {
            SgFunctionCallExp *funcCall;

            if ((size_t)allVariants[k].acrossV > allVarForIfBlock.size() &&
                (allVariants[k].type == 1 || allVariants[k].type == 3 || allVariants[k].type == 7 || allVariants[k].type > 14))
            {
                vector<int> tmp;
                generateAllBitmasks(allVariants[k].acrossV, allVariants[k].allDims, tmp);
                allVarForIfBlock.push_back(tmp);
                funcCall = new SgFunctionCallExp(*createNewFunctionSymbol(allVariants[k].nameOfNewSAdapter));
                for (size_t i = 0; i < argsForVariantFunction.size(); ++i)
                {
                    funcCall->addArg(*new SgVarRefExp(argsForVariantFunction[i]));
                }
                funcCall->addArg(*new SgVarRefExp(which_run));
                allFuncCalls.push_back(funcCall);
            }
        }

        if (options.isOn(SPEED_TEST_L0))
        {
            stmt = makeSymbolDeclarationWithInit(s_i, new SgValueExp(16));
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclarationWithInit(s_k, new SgValueExp(1));
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclaration(min_s_i);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclaration(min_s_k);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclaration(max_cuda_block);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclarationWithInit(minGpuTime, new SgValueExp(99999));
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclaration(gpuTime);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclaration(cudaEventStart);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            stmt = makeSymbolDeclaration(cudaEventStop);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            SgFunctionCallExp *eventF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventCreate"));
            eventF->addArg(SgAddrOp(*new SgVarRefExp(cudaEventStart)));
            st_end->insertStmtBefore(*new SgCExpStmt(*eventF), *st_hedr);
            
            eventF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventCreate"));
            eventF->addArg(SgAddrOp(*new SgVarRefExp(cudaEventStop)));
            st_end->insertStmtBefore(*new SgCExpStmt(*eventF), *st_hedr);

            SgFunctionCallExp *tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("printf"));
            tmpF->addArg(*new SgValueExp(getLoopLine(sadapter->identifier())));
            st_end->insertStmtBefore(*new SgCExpStmt(*tmpF), *st_hedr);


            tmpF2 = new SgFunctionCallExp(*createNewFunctionSymbol("MAX"));
            tmpF2->addArg(*new SgVarRefExp(allRegNames[0]));
            if (allRegNames.size() == 1)
                tmpF2->addArg(*new SgVarRefExp(allRegNames[0]));
            else
                tmpF2->addArg(*new SgVarRefExp(allRegNames[1]));

            for (size_t i = 2; i < allRegNames.size(); ++i)
            {
                SgFunctionCallExp *tmpF1 = new SgFunctionCallExp(*createNewFunctionSymbol("MAX"));
                tmpF1->addArg(*tmpF2);
                tmpF1->addArg(*new SgVarRefExp(allRegNames[i]));
                tmpF2 = tmpF1;
            }

            tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("MIN"));
            tmpF->addArg(*new SgValueExp(384));
            tmpF->addArg(*new SgValueExp(65535) / *tmpF2);

            tmpF2 = tmpF;
            st_end->insertStmtBefore(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(max_cuda_block), *tmpF2)), *st_hedr);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_i), *new SgVarRefExp(s_i) + *new SgValueExp(16)));
            whileSt = new SgWhileStmt(*new SgVarRefExp(s_i) < *new SgValueExp(257), *stmt);
            st_hedr->lastExecutable()->insertStmtAfter(*whileSt, *st_hedr);
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_k), *new SgVarRefExp(s_k) + *new SgValueExp(1)));
            whileSt1 = new SgWhileStmt(*new SgVarRefExp(s_k) < *new SgValueExp(17), *stmt);
            whileSt->insertStmtAfter(*whileSt1);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_k), *new SgValueExp(1)));
            whileSt->insertStmtAfter(*stmt);
        }

        for (size_t i = 0; i < allVarForIfBlock.size(); ++i)
        {
            SgExpression *e = NULL;
            for (size_t k = 0; k < allVarForIfBlock[i].size(); ++k)
            {
                if (k == 0)
                    e = &(SgEqOp(*new SgVarRefExp(which_run), *new SgValueExp(allVarForIfBlock[i][k])));
                else
                    e = &(*e || SgEqOp(*new SgVarRefExp(which_run), *new SgValueExp(allVarForIfBlock[i][k])));
            }
            if (options.isOn(SPEED_TEST_L0))
            {
                allFuncCalls[i]->addArg(*new SgVarRefExp(s_i));
                allFuncCalls[i]->addArg(*new SgVarRefExp(s_k));
            }
            stmt = new SgCExpStmt(*allFuncCalls[i]);
            if_st = new SgIfStmt(*e, *stmt);
            if (!options.isOn(SPEED_TEST_L0))
                st_end->insertStmtBefore(*if_st, *st_hedr);
            else
            {
                whileSt1->lastExecutable()->insertStmtBefore(*if_st);
            }
        }
        
        tmpF2 = new SgFunctionCallExp(*createNewFunctionSymbol("printf"));
        tmpF2->addArg(*new SgValueExp("It may be wrong!!\\n"));

        if (DVM_DEBUG_LVL > 5)
        {
            if_st = new SgIfStmt(SgEqOp(*new SgVarRefExp(which_run), *new SgValueExp(0)), *new SgCExpStmt(*tmpF2));
            st_end->insertStmtBefore(*if_st, *st_hedr);
        }

        if (options.isOn(SPEED_TEST_L0))
        {
            SgFunctionCallExp *tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventRecord"));
            tmpF->addArg(*new SgVarRefExp(cudaEventStart));
            tmpF->addArg(*new SgValueExp(0));
            whileSt1->insertStmtAfter(*new SgCExpStmt(*tmpF));

            tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventRecord"));
            tmpF->addArg(*new SgVarRefExp(cudaEventStop));
            tmpF->addArg(*new SgValueExp(0));
            whileSt1->lastExecutable()->insertStmtBefore(*new SgCExpStmt(*tmpF));

            tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventSynchronize"));
            tmpF->addArg(*new SgVarRefExp(cudaEventStop));
            whileSt1->lastExecutable()->insertStmtBefore(*new SgCExpStmt(*tmpF));

            tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventElapsedTime"));
            tmpF->addArg(SgAddrOp(*new SgVarRefExp(gpuTime)));
            tmpF->addArg(*new SgVarRefExp(cudaEventStart));
            tmpF->addArg(*new SgVarRefExp(cudaEventStop));
            whileSt1->lastExecutable()->insertStmtBefore(*new SgCExpStmt(*tmpF));

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(min_s_i), *new SgVarRefExp(s_i)));
            if_st = new SgIfStmt(*new SgVarRefExp(gpuTime) < *new SgVarRefExp(minGpuTime), *stmt);
            whileSt1->lastExecutable()->insertStmtBefore(*if_st);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(min_s_k), *new SgVarRefExp(s_k)));
            if_st->insertStmtAfter(*stmt);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(minGpuTime), *new SgVarRefExp(gpuTime)));
            if_st->insertStmtAfter(*stmt);

            if (options.isOn(SPEED_TEST_L1))
            {
                tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("printf"));
                tmpF->addArg(*new SgValueExp("      cuda-block [%d, %d] with time - %f ms\\n"));
                tmpF->addArg(*new SgVarRefExp(s_i));
                tmpF->addArg(*new SgVarRefExp(s_k));
                tmpF->addArg(*new SgVarRefExp(gpuTime));
                whileSt1->lastExecutable()->insertStmtBefore(*new SgCExpStmt(*tmpF));
            }

            tmpF = new SgFunctionCallExp(*createNewFunctionSymbol("printf"));
            tmpF->addArg(*new SgValueExp(" minimum time = %f ms, optimal cuda-block = [%d, %d]\\n\\n"));
            tmpF->addArg(*new SgVarRefExp(minGpuTime));
            tmpF->addArg(*new SgVarRefExp(min_s_i));
            tmpF->addArg(*new SgVarRefExp(min_s_k));
            st_end->insertStmtBefore(*new SgCExpStmt(*tmpF), *st_hedr);

            SgFunctionCallExp *eventF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventDestroy"));
            eventF->addArg(*new SgVarRefExp(cudaEventStart));
            st_end->insertStmtBefore(*new SgCExpStmt(*eventF), *st_hedr);

            eventF = new SgFunctionCallExp(*createNewFunctionSymbol("cudaEventDestroy"));
            eventF->addArg(*new SgVarRefExp(cudaEventStop));
            st_end->insertStmtBefore(*new SgCExpStmt(*eventF), *st_hedr);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_k), *new SgVarRefExp(s_k) + *new SgValueExp(1)));
            SgContinueStmt *contST = new SgContinueStmt();

            if_st = new SgIfStmt(*new SgVarRefExp(s_k) * *new SgVarRefExp(s_i) > *new SgVarRefExp(max_cuda_block), *contST);
            whileSt1->insertStmtAfter(*if_st);
            if_st->insertStmtAfter(*stmt);
        }

        mywarn("  end: create  IF BLOCK ");
    }

    return NULL;
}

ArgsForKernel** Create_C_Adapter_Function_Across_OneThread(SgSymbol *sadapter, SgSymbol *kernel_symb, const int loopV, const int acrossV)
{
#if debugMode
        warn("PARALLEL directive with ACROSS clause in region", 420, dvm_parallel_dir);
#endif

    SgSymbol **reduction_ptr;
    SgSymbol *lowI, *highI, *idxI;
    symb_list *sl;
    SgStatement *st_hedr, *st_end, *stmt, *first_exec;
    SgExpression *fe, *ae, *arg_list, *el, *e, *espec, *er;
    SgSymbol *s_loop_ref, *sarg, *s, *sb, *sg, *sdev, *h_first, *hgpu_first, *base_first, *uses_first, *scalar_first;
    SgSymbol *s_blocks, *s_threads, *s_dev_num, *s_tmp_var, *idxTypeInKernel;
    SgType *typ;
    SgFunctionCallExp *funcCall;
    vector<char*> dvm_array_headers;
    int ln, num, uses_num, has_red_array, use_device_num, num_of_red_arrays = 0;

    // init block
    reduction_ptr = NULL;
    lowI = highI = idxI = h_first = hgpu_first = base_first = red_first = uses_first = scalar_first = NULL;
    s_loop_ref = sarg = s = sb = sg = sdev = h_first = s_blocks = s_threads = s_dev_num = s_tmp_var = NULL;
    sl = NULL;
    typ = NULL;
    funcCall = NULL;
    st_hedr = st_end = stmt = first_exec = NULL;
    fe = ae = arg_list = el = e = espec = er = NULL;
    ln = num = uses_num = has_red_array = use_device_num = num_of_red_arrays = 0;
    // end of init block

    mywarn("start: create fuction header ");
    // create fuction header 
    st_hedr = Create_C_Function(sadapter);
    st_hedr->addComment(Cuda_LoopHandlerComment());
    st_end = st_hedr->lexNext();
    fe = st_hedr->expr(0);

    first_exec = st_end;

    mywarn("  end: create fuction header ");
    mywarn("start: create  dummy argument list ");

    // create  dummy argument list: loop_ref, <dvm-array-headers>, <uses>   
    typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *typ, *st_hedr);

    ae = new SgVarRefExp(s_loop_ref);                 //loop_ref
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*ae);
    fe->setLhs(arg_list);

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ++ln)  // <dvm-array-headers>
    {
        SgArrayType *typearray = new SgArrayType(*C_DvmType());
        typearray->addDimension(NULL);
        sarg = new SgSymbol(VARIABLE_NAME, sl->symb->identifier(), *typearray, *st_hedr);
        dvm_array_headers.push_back(sl->symb->identifier());
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

    for (el = uses_list, ln = 0; el; el = el->rhs(), ++ln)    // <uses>    
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

    mywarn("  end: create  dummy argument list ");

    if (red_list) // reduction section
    {
        mywarn("start: in reduction section ");

        s_tmp_var = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("tmpVar"), *C_DvmType(), *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        //looking through the reduction_op_list
        for (er = red_list; er; er = er->rhs())
            num_of_red_arrays++;

        reduction_ptr = new SgSymbol*[num_of_red_arrays];

        for (er = red_list, ln = 0; er; er = er->rhs(), ++ln)
        {
            SgExpression *ered, *ev, *en, *loc_var_ref;
            SgSymbol *sred, *s_loc_var, *sgrid_loc;
            int is_array;
            SgType *loc_type = NULL, *btype = NULL;

            loc_var_ref = NULL;
            s_loc_var = NULL;
            is_array = 0;
            ered = er->lhs();    //  reduction  (variant==ARRAY_OP)
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
            if (is_array)
            {
                SgArrayType *typearray = new SgArrayType(*C_Type(ev->symbol()->type()));
                typearray->addRange(*ArrayLengthInElems(ev->symbol(), NULL, 0));
                s->setType(*typearray);
            }
            else
                s->setType(C_Type(ev->symbol()->type()));

            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
            if (!ln)
                red_first = s;

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

            //!printf("__113\n"); 
            /*--- executable statements: register reductions in RTS ---*/
            e = &SgAssignOp(*new SgVarRefExp(s_tmp_var), *new SgValueExp(ln+1));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            if (!ln)
            {
                stmt->addComment("// Register reduction for CUDA-execution");
                first_exec = stmt;
            }
            stmt = new SgCExpStmt(*InitReduction(s_loop_ref, s_tmp_var, sred, s_loc_var));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        for (er = red_list, ln = 0; er; er = er->rhs(), ++ln)
        {
            char *buf_tmp = new char[8];
            sprintf(buf_tmp, "%d", ln);
            reduction_ptr[ln] = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "cuda_ptr_"), buf_tmp)), *C_PointerType(C_Type(er->lhs()->rhs()->symbol()->type())), *st_hedr);
            st_hedr->insertStmtAfter(*makeSymbolDeclaration(reduction_ptr[ln]), *st_hedr);
            delete[]buf_tmp;

            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("cudaMalloc"));
            funcCall->addArg(*new SgCastExp(*C_PointerType(C_PointerType(SgTypeVoid())), SgAddrOp(*new SgVarRefExp(reduction_ptr[ln]))));
            funcCall->addArg(SgSizeOfOp(*new SgKeywordValExp(getKeyWordType(reduction_ptr[ln]->type()))));
            stmt = new SgCExpStmt(*funcCall);
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        mywarn("  end: out reduction section ");
    }

    mywarn("start: create vars ");

    // create type for static arrays
    SgArrayType *tpArr = new SgArrayType(*LongT);
    SgValueExp *dimSize = new SgValueExp(loopV + acrossV + 2);
    tpArr->addDimension(dimSize);

    lowI = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("lowI"), *LongT, *st_hedr);
    s->setType(tpArr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    highI = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("highI"), *LongT, *st_hedr);
    s->setType(tpArr);
    addDeclExpList(s, stmt->expr(0));

    idxI = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxI"), *LongT, *st_hedr);
    s->setType(tpArr);
    addDeclExpList(s, stmt->expr(0));

    idxTypeInKernel = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxTypeInKernel"), *LongT, *st_hedr);
    addDeclExpList(s, stmt->expr(0));

    mywarn("  end: create vars ");
    mywarn("start: create assigns");

    s_blocks = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("blocks"), *t_dim3, *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    s_threads = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("threads"), *t_dim3, *st_hedr);
    addDeclExpList(s, stmt->expr(0));

    for (s = uses_first, ln = 0; ln < uses_num; s = s->next(), ++ln)    // uses
    if (!(s->attributes() & USE_IN_BIT))   // passing to kernel scalar argument by reference
    {
        sdev = GpuScalarAdrSymbolInAdapter(s, st_hedr);   // creating new symbol for address in device
        if (!ln)
        {
            scalar_first = sdev;
            stmt = makeSymbolDeclaration(sdev);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(sdev, stmt->expr(0));
    }

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ++ln)
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

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ++ln)
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


    /* -------- call dvmh_get_device_addr(long *deviceRef, void *variable) ----*/
    for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ++ln)    // uses
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
            stmt->addComment("// Get natural bases");
    }
    /* -------- call  dvmh_fill_header_(long *deviceRef, void *base, long dvmDesc[], long dvmhDesc[]);----*/

    for (s = h_first, sg = hgpu_first, sb = base_first, ln = 0; ln < num; s = s->next(), sg = sg->next(), sb = sb->next(), ln++)
    {
        e = FillHeader(s_dev_num, sb, s, sg);
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        if (!ln)
            stmt->addComment("// Fill device headers");
    }

    /* -------- call  loop_fill_bounds_(loop_ref, lowI, highI, idxI); ----*/

    stmt = new SgCExpStmt(*FillBounds(s_loop_ref, lowI, highI, idxI));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Get bounds");
    mywarn("  end: create assigns");

    stmt = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_blocks, "x"), *new SgValueExp(1)));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Start counting");

    stmt = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_threads, "x"), *new SgValueExp(1)));
    st_end->insertStmtBefore(*stmt, *st_hedr);

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

    /* -------- call   loop_guess_index_type_(loop_ref); ------------*/
    stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(idxTypeInKernel), *GuessIndexType(s_loop_ref)));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Guess index type in CUDA kernel");


    SgFunctionCallExp *sizeofL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofLL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofI = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));

    sizeofL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long")));
    sizeofLL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long long")));
    sizeofI->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "int")));

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
        &&
        SgEqOp(*sizeofL, *sizeofI),
        *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
        &&
        SgEqOp(*sizeofL, *sizeofLL),
        *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LLONG")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    /* args for kernel */
    {
        espec = CreateBlocksThreadsSpec(s_blocks, s_threads);
        funcCall = CallKernel(kernel_symb, espec);

        for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
        {
            e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
            funcCall->addArg(*e);
            for (int i = NumberOfCoeffs(sg); i>0; i--)
                funcCall->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
        }
        if (red_list)
        {
            reduction_operation_list *rsl;
            int i = 0;
            for (rsl = red_struct_list, s = red_first; rsl; rsl = rsl->next, ++i)  //s!=s_blocks_info
            {
                if (rsl->redvar_size == 0) //reduction variable is scalar
                {
                    if (options.isOn(RTC))
                    {
                        SgVarRefExp *toAdd = new SgVarRefExp(s);
                        toAdd->addAttribute(RTC_NOT_REPLACE);
                        funcCall->addArg(*toAdd);
                    }
                    else
                        funcCall->addArg(*new SgVarRefExp(s));                    
                }
                else
                {
                    int i;
                    has_red_array = 1;
                    for (i = 0; i < rsl->redvar_size; i++)
                        funcCall->addArg(*new SgArrayRefExp(*s, *new SgValueExp(i)));
                }
                s = s->next();
                if (options.isOn(C_CUDA))
                    funcCall->addArg(*new SgVarRefExp(reduction_ptr[i]));
                else
                    funcCall->addArg(*new SgCastExp(*C_PointerType(new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(reduction_ptr[i])));
            }
        }
        for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
        {
            if (s->attributes() & USE_IN_BIT)
                funcCall->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
            else
            {                                                   // passing argument by reference to kernel
                SgType *tp = NULL;
                if (s->type()->hasBaseType())
                    tp = s->type()->baseType();
                else
                    tp = s->type();
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? tp : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
                funcCall->addArg(*e);
                sdev = sdev->next();
            }
        }

        for (int i = 0; i < acrossV + loopV; ++i)
        {
            funcCall->addArg(*new SgArrayRefExp(*lowI, *new SgValueExp(i)));
            funcCall->addArg(*new SgArrayRefExp(*highI, *new SgValueExp(i)));
            funcCall->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(i)));
        }
    }

    stmt = createKernelCallsInCudaHandler(funcCall, s_loop_ref, idxTypeInKernel, s_blocks);
    st_end->insertStmtBefore(*stmt, *st_hedr);

    if (red_list)
    {
        ln = 0;
        for (er = red_list; er; er = er->rhs(), ++ln)
        {
            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("cudaMemcpy"));
            funcCall->addArg(SgAddrOp(*new SgVarRefExp(&(er->lhs()->rhs()->symbol()->copy()))));
            funcCall->addArg(*new SgVarRefExp(reduction_ptr[ln]));
            funcCall->addArg(SgSizeOfOp(*new SgKeywordValExp(getKeyWordType(reduction_ptr[ln]->type()))));
            funcCall->addArg(*new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "cudaMemcpyDeviceToHost")));
            stmt = new SgCExpStmt(*funcCall);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            e = &SgAssignOp(*new SgVarRefExp(*s_tmp_var), *new SgValueExp(ln+1));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            stmt = new SgCExpStmt(*RedPost(s_loop_ref, s_tmp_var, &(er->lhs()->rhs()->symbol()->copy()), NULL)); // loop_red_post_
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
        ln = 0;
        for (er = red_list; er; er = er->rhs(), ++ln)
        {
            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("cudaFree"));
            funcCall->addArg(*new SgVarRefExp(reduction_ptr[ln]));
            stmt = new SgCExpStmt(*funcCall);
            st_end->insertStmtBefore(*stmt, *st_hedr);
            if (ln == 0)
                stmt->addComment("// Free temporary variables");
        }
    }
    // create args for kernel and return it    
    ArgsForKernel **argsKernel = new ArgsForKernel*[countKernels];
    for (unsigned i = 0; i < countKernels; ++i)
    {
        argsKernel[i] = new ArgsForKernel();
        argsKernel[i]->st_header = st_hedr;
    }

    delete[]reduction_ptr;
    mywarn("  end Adapter Function");
    return argsKernel;
}

static inline void insertReductionArgs(SgSymbol **reduction_ptr, SgSymbol **reduction_loc_ptr, 
                                       SgSymbol **reduction_symb, SgSymbol **reduction_loc_symb,
                                       SgFunctionCallExp *funcCallKernel, int &has_red_array)
{
    reduction_operation_list *rsl;
    SgSymbol *s;
    SgExpression *e;

    int i = 0;
    for (rsl = red_struct_list, s = red_first; rsl; rsl = rsl->next, ++i)  //s!=s_blocks_info
    {
        if (rsl->redvar_size == 0) //reduction variable is scalar    
        {
            if (options.isOn(RTC))
            {
                SgVarRefExp *toAdd = new SgVarRefExp(reduction_symb[i]);
                toAdd->addAttribute(RTC_NOT_REPLACE);
                funcCallKernel->addArg(*toAdd);
            }
            else
                funcCallKernel->addArg(*new SgVarRefExp(reduction_symb[i]));
        }
        else //TODO!!
        {
            has_red_array = 1;
            for (int k = 0; k < rsl->redvar_size; ++k)
                funcCallKernel->addArg(*new SgArrayRefExp(*reduction_symb[i], *new SgValueExp(k)));
        }

        if (options.isOn(C_CUDA))
            funcCallKernel->addArg(*new SgVarRefExp(reduction_ptr[i]));
        else
            funcCallKernel->addArg(*new SgCastExp(*C_PointerType(new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(reduction_ptr[i])));

        //TODO!!
        if (rsl->locvar)  //MAXLOC,MINLOC
        {
            for (int i = 0; i < rsl->number; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*s, *new SgValueExp(i)));
            s = s->next();
            e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(rsl->locvar->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(s));
            funcCallKernel->addArg(*e);
            s = s->next();
        }
    }
}

ArgsForKernel** Create_C_Adapter_Function_Across_variants(SgSymbol *sadapter, SgSymbol *kernel_symb, const int loopV, const int acrossV,
                                                          const int allDims, SageSymbols **loopSymb, SageSymbols **loopAcrossSymb)
{
#if debugMode
        warn("PARALLEL directive with ACROSS clause in region", 420, dvm_parallel_dir);
#endif

    SgSymbol **num_elems = new SgSymbol*[allDims + 1];
    SgSymbol **reduction_ptr = NULL, **reduction_loc_ptr = NULL, **addressingParams = NULL;
    SgSymbol **reduction_symb = NULL, **reduction_loc_symb = NULL;
    SgSymbol *lowI, *highI, *idxI, *bIdxs;
    SgSymbol *elem, *red_blocks, *shared_mem, *stream_t;
    SgSymbol *M, *N, *M1, *M2, *M3, *q, *diag, *Emax, *Emin, *Allmin, *SE, *var1, *var2, *var3;
    SgSymbol *acrossBase[numLoopVars], *loopBase[numLoopVars], **outTypeOfTransformation = NULL;
    SgSymbol *nums[3], *steps = NULL;
    const char *s_cuda_var[3] = { "x", "y", "z" };

    symb_list *sl;
    SgStatement *st_hedr, *st_end, *stmt, *first_exec;
    SgExpression *fe, *ae, *arg_list, *el, *e, *espec, *ex, *er;
    SgSymbol *s_loop_ref, *sarg, *s, *sb, *sg, *sdev, *h_first, *hgpu_first, *base_first, *uses_first, *scalar_first;
    SgSymbol *s_blocks, *s_threads, *s_dev_num, *s_tmp_var, *type_of_run, *s_i = NULL, *s_k = NULL, *s_tmp_var_1;
    SgSymbol *idxTypeInKernel;
    SgType *typ;
    SgFunctionCallExp *funcCall, *funcCallKernel;
    vector<char*> dvm_array_headers;
    int ln, num, uses_num, has_red_array, use_device_num, num_of_red_arrays;

    // init block
    lowI = highI = idxI = elem = red_blocks = shared_mem = stream_t = bIdxs = NULL;
    M = N = M1 = M2 = M3 = q = diag = Emax = Emin = Allmin = SE = var1 = var2 = var3 = NULL;
    s_loop_ref = sarg = s = sb = sg = sdev = h_first = NULL;
    hgpu_first = base_first = uses_first = scalar_first = NULL;
    s_blocks = s_threads = s_dev_num = s_tmp_var = s_tmp_var_1 = NULL;
    typ = NULL;
    funcCall = funcCallKernel = NULL;
    sl = NULL;
    type_of_run = NULL;
    st_hedr = st_end = stmt = first_exec = NULL;
    fe = ae = arg_list = el = e = espec = ex = er = NULL;
    ln = num = uses_num = has_red_array = use_device_num = num_of_red_arrays = 0;
    //end of init block

    mywarn("start: create fuction header ");
    // create fuction header 
    st_hedr = Create_C_Function(sadapter);
    st_hedr->addComment(Cuda_LoopHandlerComment());
    st_end = st_hedr->lexNext();
    fe = st_hedr->expr(0);
    first_exec = st_end;
    if (declaration_include)
    {          
        declaration_cmnt = "#include <cstdio>\n#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))\n#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))";
        declaration_include = false;
    }

    mywarn("  end: create fuction header ");
    mywarn("start: create  dummy argument list ");

    // create  dummy argument list: loop_ref, <dvm-array-headers>, <uses>   
    typ = C_PointerType(C_Derived_Type(s_DvmhLoopRef));
    s_loop_ref = new SgSymbol(VARIABLE_NAME, "loop_ref", *typ, *st_hedr);

    ae = new SgVarRefExp(s_loop_ref);                 //loop_ref
    ae->setType(typ);
    ae = new SgPointerDerefExp(*ae);
    arg_list = new SgExprListExp(*ae);
    fe->setLhs(arg_list);

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ++ln)  // <dvm-array-headers>
    {
        SgArrayType *typearray = new SgArrayType(*C_DvmType());
        typearray->addDimension(NULL);
        sarg = new SgSymbol(VARIABLE_NAME, sl->symb->identifier(), *typearray, *st_hedr);
        dvm_array_headers.push_back(sl->symb->identifier());
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

    for (el = uses_list, ln = 0; el; el = el->rhs(), ++ln)    // <uses>    
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

    type_of_run = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("type_of_run"), *LongT, *st_hedr);
    ae = new SgVarRefExp(type_of_run);
    ae->setType(LongT);
    arg_list->setRhs(*new SgExprListExp(*ae));
    arg_list = arg_list->rhs();

    if (options.isOn(SPEED_TEST_L0))
    {
        s_i = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("__s_i"), *C_Type(SgTypeInt()), *st_hedr);
        ae = new SgVarRefExp(s_i);
        ae->setType(C_Type(SgTypeInt()));
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();

        s_k = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("__s_k"), *C_Type(SgTypeInt()), *st_hedr);
        ae = new SgVarRefExp(s_k);
        ae->setType(C_Type(SgTypeInt()));
        arg_list->setRhs(*new SgExprListExp(*ae));
        arg_list = arg_list->rhs();
    }

    mywarn("  end: create  dummy argument list ");
    if (red_list) // reduction section
    {
        mywarn("start: in reduction section ");
        s_tmp_var = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("tmpVar"), *C_DvmType(), *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        s_tmp_var_1 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("tmpVar1"), *C_DvmType(), *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        //looking through the reduction_op_list
        for (er = red_list; er; er = er->rhs())
            num_of_red_arrays++;

        reduction_ptr = new SgSymbol*[num_of_red_arrays];
        reduction_symb = new SgSymbol*[num_of_red_arrays];

        reduction_loc_ptr = new SgSymbol*[num_of_red_arrays];
        reduction_loc_symb = new SgSymbol*[num_of_red_arrays];

        for (er = red_list, ln = 0; er; er = er->rhs(), ++ln)
        {
            SgExpression *ered, *ev, *en, *loc_var_ref;
            SgSymbol *sred, *s_loc_var, *sgrid_loc;
            int is_array;
            SgType *loc_type = NULL, *btype = NULL;

            loc_var_ref = NULL;
            s_loc_var = NULL;
            is_array = 0;
            ered = er->lhs();    //  reduction  (variant==ARRAY_OP)
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
            if (is_array)
            {
                SgArrayType *typearray = new SgArrayType(*C_Type(ev->symbol()->type()));
                typearray->addRange(*ArrayLengthInElems(ev->symbol(), NULL, 0));
                s->setType(*typearray);
            }
            else
                s->setType(C_Type(ev->symbol()->type()));

            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
            reduction_symb[ln] = s;
            if (!ln)
                red_first = s;

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

            //!printf("__113\n"); 
            /*--- executable statements: register reductions in RTS ---*/
            e = &SgAssignOp(*new SgVarRefExp(s_tmp_var), *new SgValueExp(ln+1));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            if (!ln)
            {
                stmt->addComment("// Register reduction for CUDA-execution");
                first_exec = stmt;
            }

            char *buf_tmp = new char[8];
            sprintf(buf_tmp, "%d", ln);
            reduction_ptr[ln] = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "cuda_ptr_"), buf_tmp)), *C_PointerType(C_Type(ev->symbol()->type())), *st_hedr);
            st_hedr->insertStmtAfter(*makeSymbolDeclaration(reduction_ptr[ln]), *st_hedr);
            delete[]buf_tmp;

            if (s_loc_var)
                reduction_loc_ptr[ln] = sgrid_loc;
            else
                reduction_loc_ptr[ln] = NULL;

            // create loop_cuda_register_red()
            stmt = new SgCExpStmt(*RegisterReduction_forAcross(s_loop_ref, s_tmp_var, reduction_ptr[ln], reduction_loc_ptr[ln]));
            st_end->insertStmtBefore(*stmt, *st_hedr);
            // create loop_red_init_()
            stmt = new SgCExpStmt(*InitReduction(s_loop_ref, s_tmp_var, sred, s_loc_var));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        mywarn("  end: out reduction section ");
    }

    mywarn("start: create vars ");

    // create type for static arrays
    SgArrayType *tpArr = new SgArrayType(*LongT);
    SgValueExp *dimSize = new SgValueExp(loopV + acrossV + 2);
    tpArr->addDimension(dimSize);

    if (red_list)
    {
        red_blocks = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_of_red_blocks"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }

    lowI = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("lowI"), *LongT, *st_hedr);
    s->setType(tpArr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    highI = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("highI"), *LongT, *st_hedr);
    s->setType(tpArr);
    addDeclExpList(s, stmt->expr(0));

    idxI = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxI"), *LongT, *st_hedr);
    s->setType(tpArr);
    addDeclExpList(s, stmt->expr(0));

    idxTypeInKernel = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxTypeInKernel"), *LongT, *st_hedr);    
    addDeclExpList(s, stmt->expr(0));

    if (options.isOn(GPU_O0))
    {
        steps = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("steps"), *LongT, *st_hedr);
        s->setType(tpArr);
        addDeclExpList(s, stmt->expr(0));
    }

    bIdxs = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("idxs"), *LongT, *st_hedr);
    s->setType(tpArr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    if (options.isOn(AUTO_TFM))
    {
        // create type for static arrays for addresingParams, size = 5
        SgArrayType *tpArr = new SgArrayType(*LongT);
        SgValueExp *dimSize = new SgValueExp(7);
        tpArr->addDimension(dimSize);

        addressingParams = new SgSymbol*[dvm_array_headers.size()];
        outTypeOfTransformation = new SgSymbol*[dvm_array_headers.size()];
        char *tmpS = new char[64];
        for (size_t i = 0; i < dvm_array_headers.size(); ++i)
        {
            tmpS[0] = '\0';
            strcat(tmpS, dvm_array_headers[i]);
            strcat(tmpS, "_addressingParams");
            addressingParams[i] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), *LongT, *st_hedr);
            s->setType(tpArr);
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);

            tmpS[0] = '\0';
            strcat(tmpS, dvm_array_headers[i]);
            strcat(tmpS, "_outTypeOfTfm");
            outTypeOfTransformation[i] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), *LongT, *st_hedr);
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
    }

    if (acrossV == 1) // ACROSS with one dependence: create variables
    {
        SgStatement **stmts = new SgStatement*[MIN(loopV, 3) * 2];
        for (int k = 0, k1 = MIN(loopV, 3); k < MIN(loopV, 3); ++k, ++k1)
        {
            nums[k] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_"), s_cuda_var[k])), *LongT, *st_hedr);
            stmts[k] = makeSymbolDeclaration(s);

            num_elems[k] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_elem_"), s_cuda_var[k])), *LongT, *st_hedr);
            stmts[k1] = makeSymbolDeclaration(s);
        }
        for (int k = 0; k < MIN(loopV, 3) * 2; ++k)
            st_hedr->insertStmtAfter(*stmts[k], *st_hedr);

        if (loopV > 3)
        {
            for (int k = 0; k < loopV - 2; ++k)
            {
                char *tmp = new char[10];
                sprintf(tmp, "%d", k);
                num_elems[k + 3] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_elem_z_"), tmp)), *LongT, *st_hedr);
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);
                delete[]tmp;
            }
        }

        delete[]stmts;
    }
    else if (acrossV == 2) // ACROSS with two dependence: create variables
    {
        M = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("M"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        N = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("N"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        elem = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("elem"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        diag = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("diag"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        q = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("q"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        SgStatement **stmts = new SgStatement*[(MIN(loopV + 1, 3) - 1) * 2];
        for (int k = 1, k1 = MIN(loopV + 1, 3) - 1; k < MIN(loopV + 1, 3); ++k, ++k1)
        {
            nums[k] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_"), s_cuda_var[k])), *LongT, *st_hedr);
            stmts[k - 1] = makeSymbolDeclaration(s);

            num_elems[k] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_elem_"), s_cuda_var[k])), *LongT, *st_hedr);
            stmts[k1] = makeSymbolDeclaration(s);
        }

        nums[0] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_x"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        for (int i = 0; i < (MIN(loopV + 1, 3) - 1) * 2; ++i)
            st_hedr->insertStmtAfter(*stmts[i], *st_hedr);
        delete[]stmts;

        if (loopV > 2)
        {
            for (int k = 0; k < loopV - 1; ++k)
            {
                char *tmp = new char[10];
                sprintf(tmp, "%d", k);
                num_elems[k + 3] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_elem_z_"), tmp)), *LongT, *st_hedr);
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);
                delete[]tmp;
            }
        }
    }
    else if (acrossV >= 3) // ACROSS with three dependence: create variables
    {
        nums[0] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_x"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        nums[1] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_y"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        if (loopV > 0)
        {
            nums[2] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_z"), *LongT, *st_hedr);
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
            for (int k = 0; k < loopV; ++k)
            {
                char *tmp = new char[10];
                sprintf(tmp, "%d", k);
                num_elems[k] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[16], "num_elem_z_"), tmp)), *LongT, *st_hedr);
                stmt = makeSymbolDeclaration(s);
                st_hedr->insertStmtAfter(*stmt, *st_hedr);
                delete[]tmp;
            }

            num_elems[loopV] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_elem_z"), *LongT, *st_hedr);
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }

        M1 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Mi"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        M2 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Mj"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        M3 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Mk"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        Emax = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Emax"), *LongT, *st_hedr);
        stmt = makeSymbolDeclaration(s);
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        Emin = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Emin"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        Allmin = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Allmin"), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));

        SE = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("SE"), *LongT, *st_hedr);
        stmt = makeSymbolDeclarationWithInit(s, new SgValueExp(1));
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        diag = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("diag"), *LongT, *st_hedr);
        stmt = makeSymbolDeclarationWithInit(s, new SgValueExp(1));
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        var1 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("var1"), *LongT, *st_hedr);
        stmt = makeSymbolDeclarationWithInit(s, new SgValueExp(1));
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        var2 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("var2"), *LongT, *st_hedr);
        stmt = makeSymbolDeclarationWithInit(s, new SgValueExp(0));
        st_hedr->insertStmtAfter(*stmt, *st_hedr);

        var3 = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("var3"), *LongT, *st_hedr);
        stmt = makeSymbolDeclarationWithInit(s, new SgValueExp(0));
        st_hedr->insertStmtAfter(*stmt, *st_hedr);
    }
    // create indxs
    for (int i = 0; i < acrossV; ++i)
    {
        acrossBase[i] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[20], "base_"),
                                         loopAcrossSymb[i]->symb->identifier())), *LongT, *st_hedr);
        if (i == 0)
        {
            stmt = makeSymbolDeclaration(s);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(s, stmt->expr(0));
    }
    for (int i = 0; i < loopV; ++i)
    {
        loopBase[i] = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(strcat(strcpy(new char[20], "base_"),
                                       loopSymb[i]->symb->identifier())), *LongT, *st_hedr);
        addDeclExpList(s, stmt->expr(0));
    }
    // end

    mywarn("  end: create vars ");
    mywarn("start: create assigns");

    s_blocks = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("blocks"), *t_dim3, *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    s_threads = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("threads"), *t_dim3, *st_hedr);
    addDeclExpList(s, stmt->expr(0));

    shared_mem = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("shared_mem"), *LongT, *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    stream_t = s = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("stream"), *C_Derived_Type(s_cudaStream), *st_hedr);
    stmt = makeSymbolDeclaration(s);
    st_hedr->insertStmtAfter(*stmt, *st_hedr);

    for (s = uses_first, ln = 0; ln < uses_num; s = s->next(), ++ln)    // uses
    if (!(s->attributes() & USE_IN_BIT))   // passing to kernel scalar argument by reference
    {
        sdev = GpuScalarAdrSymbolInAdapter(s, st_hedr);   // creating new symbol for address in device
        if (!ln)
        {
            scalar_first = sdev;
            stmt = makeSymbolDeclaration(sdev);
            st_hedr->insertStmtAfter(*stmt, *st_hedr);
        }
        else
            addDeclExpList(sdev, stmt->expr(0));
    }

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ++ln)
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

    for (sl = acc_array_list, ln = 0; sl; sl = sl->next, ++ln)
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

    /* call DvmType loop_cuda_autotransform_(DvmhLoopRef *InDvmhLoop, DvmType dvmDesc[]); */
    if (options.isOn(AUTO_TFM))
    {
        s = h_first;
        for (size_t i = 0; i < dvm_array_headers.size(); ++i, s = s->next())
        {
            stmt = new SgCExpStmt(*CudaAutoTransform(s_loop_ref, s));
            st_end->insertStmtBefore(*stmt, *st_hedr);
            if (!i)
                stmt->addComment("// Autotransform all arrays");
        }
    }

    /* -------- call dvmh_get_device_addr(long *deviceRef, void *variable) ----*/
    for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ++ln)    // uses
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
            stmt->addComment("// Get natural bases");
    }

    /* call dvmh_fill_header_ex_(DvmType *deviceRef, void *base, DvmType dvmDesc[], DvmType dvmhDesc[], DvmType *outTypeOfTransformation, DvmType extendedParams[]);*/
    if (options.isOn(AUTO_TFM))
    {
        for (s = h_first, sg = hgpu_first, sb = base_first, ln = 0; ln < num; s = s->next(), sg = sg->next(), sb = sb->next(), ln++)
        {
            stmt = new SgCExpStmt(*FillHeader_Ex(s_dev_num, sb, s, sg, outTypeOfTransformation[ln], addressingParams[ln]));
            st_end->insertStmtBefore(*stmt, *st_hedr);
            if (!ln)
                stmt->addComment("// Fill device headers");
        }
    }
    /* -------- call  dvmh_fill_header_(long *deviceRef, void *base, long dvmDesc[], long dvmhDesc[]);----*/
    else
    {
        for (s = h_first, sg = hgpu_first, sb = base_first, ln = 0; ln < num; s = s->next(), sg = sg->next(), sb = sb->next(), ln++)
        {
            e = FillHeader(s_dev_num, sb, s, sg);
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
            if (!ln)
                stmt->addComment("// Fill device headers");
        }
    }
    /* -------- call  loop_fill_bounds_(loop_ref, lowI, highI, idxI); ----*/

    stmt = new SgCExpStmt(*FillBounds(s_loop_ref, lowI, highI, idxI));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Get bounds");

    /* -------- call  dvmh_change_filled_bounds(low, high, idx, n, dep, type_of_run, idxs); ----*/
    if (acrossV == 1 || acrossV == 2 || acrossV >= 3)
    {
        char *name = new char[16];
        name[0] = '\0';
        sprintf(name, "%d", acrossV + loopV);
        SgSymbol *tmp_1 = new SgSymbol(VARIABLE_NAME, name);
        name[0] = '\0';
        sprintf(name, "%d", acrossV);
        SgSymbol *tmp_2 = new SgSymbol(VARIABLE_NAME, name);

        stmt = new SgCExpStmt(*ChangeFilledBounds(lowI, highI, idxI, tmp_1, tmp_2, type_of_run, bIdxs));
        st_end->insertStmtBefore(*stmt, *st_hedr);
        stmt->addComment("// Swap bounds");

        delete[]name;
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

    /* -------- call   loop_guess_index_type_(loop_ref); ------------*/
    stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(idxTypeInKernel), *GuessIndexType(s_loop_ref)));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Guess index type in CUDA kernel");

    SgFunctionCallExp *sizeofL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofLL = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));
    SgFunctionCallExp *sizeofI = new SgFunctionCallExp(*createNewFunctionSymbol("sizeof"));

    sizeofL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long")));
    sizeofLL->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "long long")));
    sizeofI->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "int")));

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
                         &&
                        SgEqOp(*sizeofL, *sizeofI),
         *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LONG")))
                         && 
                        SgEqOp(*sizeofL, *sizeofLL),
         *new SgCExpStmt(SgAssignOp(*new SgVarRefExp(idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_LLONG")))));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    
    /* -------- call  loop_cuda_get_config_(loop_ref, &shared_mem, &reg_per_th, &threads, &stream, &shared_mem); ------------*/
    SgFunctionCallExp *tmpFunc = new SgFunctionCallExp(*createNewFunctionSymbol("dim3"));
    int x = 0, y = 0, z = 0;
    getDefaultCudaBlock(x, y, z, acrossV, loopV);
    tmpFunc->addArg(*new SgValueExp(x));
    tmpFunc->addArg(*new SgValueExp(y));
    tmpFunc->addArg(*new SgValueExp(z));

    stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_threads), *tmpFunc));
    st_end->insertStmtBefore(*stmt, *st_hedr);
    stmt->addComment("// Get CUDA configuration params");

    if (loopV > 0 && red_list)
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*shared_mem), *new SgValueExp(getSizeOf())));
    else
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*shared_mem), *new SgValueExp(0)));
    st_end->insertStmtBefore(*stmt, *st_hedr);

    string define_name_int = kernel_symb->identifier();
    string define_name_long = kernel_symb->identifier();
    
    define_name_int += "_int_regs";
    define_name_long += "_llong_regs";

    SgStatement *config_int = new SgCExpStmt(*GetConfig(s_loop_ref, shared_mem, new SgSymbol(VARIABLE_NAME, define_name_int.c_str()), s_threads, stream_t, shared_mem));
    SgStatement *config_long = new SgCExpStmt(*GetConfig(s_loop_ref, shared_mem, new SgSymbol(VARIABLE_NAME, define_name_long.c_str()), s_threads, stream_t, shared_mem));

    stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(*idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT"))), *config_int, *config_long);
    st_end->insertStmtBefore(*stmt, *st_hedr);

    // collect names, all _REGS constant
    RGname_list = AddNewToSymbList(RGname_list, new SgSymbol(VARIABLE_NAME, define_name_int.c_str(), C_DvmType(), st_hedr));
    allRegNames.push_back(new SgSymbol(VARIABLE_NAME, define_name_int.c_str()));

    RGname_list = AddNewToSymbList(RGname_list, new SgSymbol(VARIABLE_NAME, define_name_long.c_str(), C_DvmType(), st_hedr));
    allRegNames.push_back(new SgSymbol(VARIABLE_NAME, define_name_long.c_str()));

    tmpFunc = new SgFunctionCallExp(*createNewFunctionSymbol("dim3"));
    if (options.isOn(SPEED_TEST_L0))
    {
        tmpFunc->addArg(*new SgVarRefExp(s_i));
        tmpFunc->addArg(*new SgVarRefExp(s_k));
        tmpFunc->addArg(*new SgValueExp(z));
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_threads), *tmpFunc));
        st_end->insertStmtBefore(*stmt, *st_hedr);
    }

    if (acrossV == 1) // ACROSS with one dependence: create variables
    {
        //SgStatement **stmts = new SgStatement*[MIN(loopV, 3) * 2];
        for (int k = 0; k < MIN(loopV, 3); ++k)
        {
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[k]), *new SgRecordRefExp(*s_threads, (char*)s_cuda_var[k])));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
    }
    else if (acrossV == 2) // ACROSS with two dependence: create variables
    {
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[0]), *new SgRecordRefExp(*s_threads, "x")));
        st_end->insertStmtBefore(*stmt, *st_hedr);

        for (int k = 1; k < MIN(loopV + 1, 3); ++k)
        {
            if (k == 1)
            {
                stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[k]), *new SgRecordRefExp(*s_threads, "y")));
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            else
            {
                stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[k]), *new SgRecordRefExp(*s_threads, "z")));
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
        }
    }
    else if (acrossV >= 3) // ACROSS with three dependence: create variables
    {
        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[0]), *new SgRecordRefExp(*s_threads, "x")));
        st_end->insertStmtBefore(*stmt, *st_hedr);

        stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[1]), *new SgRecordRefExp(*s_threads, "y")));
        st_end->insertStmtBefore(*stmt, *st_hedr);

        if (loopV > 0)
        {
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*nums[2]), *new SgRecordRefExp(*s_threads, "z")));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
    }

    mywarn("  end: create assigns");

    espec = CreateBlocksThreadsSpec(shared_mem, s_blocks, s_threads, stream_t);

    if (acrossV == 1) // ACROSS with one dependence: generate method
    {
        mywarn("start: in start across 1");
        SgFunctionCallExp *f = new SgFunctionCallExp(*createNewFunctionSymbol("dim3"));
        f->addArg(*new SgValueExp(1));
        f->addArg(*new SgValueExp(1));
        f->addArg(*new SgValueExp(1));

        e = &SgAssignOp(*new SgVarRefExp(s_blocks), *f);
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);
        stmt->addComment("//Start method");

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);

        {
            int *idx = new int[loopV];
            SgExpression *mult_z = NULL;
            for (int k = 0; k < MIN(2, loopV); ++k)
            {
                SgStatement *st1;
                idx[k] = loopSymb[k]->len;

                e = &SgAssignOp(*new SgVarRefExp(loopBase[k]), *new SgArrayRefExp(*lowI, *new SgValueExp(idx[k])));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);

                SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));

                funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                f1->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(idx[k])));
                funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(idx[k])) - *new SgArrayRefExp(*highI, *new SgValueExp(idx[k]))));
                e = &(*funcCall + *f1);
                st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*num_elems[k]), *e / *f1));
                st_end->insertStmtBefore(*st1, *st_hedr);

                st1 = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_blocks, (char *)s_cuda_var[k]),
                    *new SgVarRefExp(*num_elems[k]) / *new SgVarRefExp(nums[k]) +
                    SgNeqOp(*new SgVarRefExp(*num_elems[k]) % *new SgVarRefExp(nums[k]), *new SgValueExp(0))));
                st_end->insertStmtBefore(*st1, *st_hedr);

                e = &SgAssignOp(*new SgRecordRefExp(*s_threads, (char *)s_cuda_var[k]), *new SgVarRefExp(*nums[k]));
                st_end->insertStmtBefore(*new SgCExpStmt(*e), *st_hedr);
            }

            if (loopV > 3)
            {
                for (int k = 2; k < loopV; ++k)
                {
                    SgStatement *st1;
                    idx[k] = loopSymb[k]->len;

                    e = &SgAssignOp(*new SgVarRefExp(loopBase[k]), *new SgArrayRefExp(*lowI, *new SgValueExp(idx[k])));
                    stmt = new SgCExpStmt(*e);
                    st_end->insertStmtBefore(*stmt, *st_hedr);

                    SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));

                    funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                    f1->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(idx[k])));
                    funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(idx[k])) - *new SgArrayRefExp(*highI, *new SgValueExp(idx[k]))));
                    e = &(*funcCall + *f1);
                    st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*num_elems[k + 1]), *e / *f1));
                    st_end->insertStmtBefore(*st1, *st_hedr);

                    if (k == 2)
                        mult_z = &(*new SgVarRefExp(*num_elems[k + 1]));
                    else
                        mult_z = &((*mult_z) * (*new SgVarRefExp(*num_elems[k + 1])));
                }
                stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*num_elems[2]), *mult_z));
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            else if (loopV > 2)
            {
                SgStatement *st1;
                int k = 2;
                idx[k] = loopSymb[k]->len;

                e = &SgAssignOp(*new SgVarRefExp(loopBase[k]), *new SgArrayRefExp(*lowI, *new SgValueExp(idx[k])));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);

                SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));

                funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                f1->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(idx[k])));
                funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(idx[k])) - *new SgArrayRefExp(*highI, *new SgValueExp(idx[k]))));
                e = &(*funcCall + *f1);
                st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(*num_elems[k]), *e / *f1));
                st_end->insertStmtBefore(*st1, *st_hedr);
            }

            if (loopV > 2)
            {
                stmt = new SgCExpStmt(SgAssignOp(*new SgRecordRefExp(*s_blocks, (char *)s_cuda_var[2]),
                    *new SgVarRefExp(*num_elems[2]) / *new SgVarRefExp(nums[2]) +
                    SgNeqOp(*new SgVarRefExp(*num_elems[2]) % *new SgVarRefExp(nums[2]), *new SgValueExp(0))));
                st_end->insertStmtBefore(*stmt, *st_hedr);

                e = &SgAssignOp(*new SgRecordRefExp(*s_threads, (char *)s_cuda_var[2]), *new SgVarRefExp(*nums[2]));
                st_end->insertStmtBefore(*new SgCExpStmt(*e), *st_hedr);
            }

            delete[]idx;
        }

        mywarn("  end: out start across 1");

        if (red_list)
        {
            mywarn("strat: in red section");
            if (loopV != 0)
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), *new SgRecordRefExp(*s_blocks, "x") * *new SgRecordRefExp(*s_blocks, "y") * *new SgRecordRefExp(*s_blocks, "z"));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            else
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), *new SgValueExp(1));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_tmp_var_1), *new SgValueExp(1)));
            st_end->insertStmtBefore(*stmt, *st_hedr);

            for (er = red_list, ln = 0; er; er = er->rhs(), ++ln)
            {
                e = &SgAssignOp(*new SgVarRefExp(s_tmp_var), *new SgValueExp(ln+1));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);

                stmt = new SgCExpStmt(*PrepareReduction(s_loop_ref, s_tmp_var, red_blocks, s_tmp_var_1));
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }

            mywarn("  end: out red section");
        }

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgVarRefExp(acrossBase[0])
            + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)));
        stmt = new SgCExpStmt(*e);


        if (options.isOn(C_CUDA) || options.isOn(GPU_O0) == false)
        {
            SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            SgFunctionCallExp *f2 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            f1->addArg(*new SgArrayRefExp(*highI, *new SgValueExp(loopAcrossSymb[0]->len)) - *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)));
            f2->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)));

            e = &SgAssignOp(*new SgArrayRefExp(*highI, *new SgValueExp(loopAcrossSymb[0]->len)), (*f1 + *f2) / *f2);
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
        
        if (options.isOn(GPU_O0))
        {
            e = &SgAssignOp(*new SgArrayRefExp(*steps, *new SgArrayRefExp(*bIdxs, *new SgValueExp(0))), *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            for (int i = 0; i < loopV; ++i)
            {
                e = &SgAssignOp(*new SgArrayRefExp(*steps, *new SgArrayRefExp(*bIdxs, *new SgValueExp((int)(i + 1)))), *new SgValueExp(0));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
        }

        mywarn("start: in adding args section");

        /* args for kernel */
        {
            funcCallKernel = CallKernel(kernel_symb, espec);

            for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
            {
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
                funcCallKernel->addArg(*e);
                for (int i = NumberOfCoeffs(sg); i > 0; i--)
                    funcCallKernel->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
            }

            if (red_list)
                insertReductionArgs(reduction_ptr, reduction_loc_ptr, reduction_symb, reduction_loc_symb, funcCallKernel, has_red_array);

            for (int k = 0; k < MIN(loopV, 2); ++k)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            if (loopV == 3)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[2]));
            else if (loopV > 3)
            for (int k = 3; k < loopV + 1; ++k)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            for (int i = 0; i < acrossV; ++i)
            {
                if (i == 0 && options.isOn(RTC)) // across base is modifiable value
                {
                    SgVarRefExp *toAdd = new SgVarRefExp(acrossBase[i]);
                    toAdd->addAttribute(RTC_NOT_REPLACE);
                    funcCallKernel->addArg(*toAdd);
                }
                else
                    funcCallKernel->addArg(*new SgVarRefExp(acrossBase[i]));
            }
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgVarRefExp(loopBase[i]));
            for (int i = 0; i < acrossV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[i]->len)));

            for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
            {
                if (s->attributes() & USE_IN_BIT)
                    funcCallKernel->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
                else
                {                                                   // passing argument by reference to kernel
                    SgType *tp = NULL;
                    if (s->type()->hasBaseType())
                        tp = s->type()->baseType();
                    else
                        tp = s->type();
                    e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? tp : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
                    funcCallKernel->addArg(*e);
                    sdev = sdev->next();
                }
            }
            funcCallKernel->addArg(*new SgVarRefExp(type_of_run));
            for (int i = 0; i < acrossV + loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*bIdxs, *new SgValueExp(i)));

            char *cond_ = new char[strlen("cond_") + strlen(loopAcrossSymb[0]->symb->identifier()) + 1];
            cond_[0] = '\0';
            strcat(cond_, "cond_");
            strcat(cond_, loopAcrossSymb[0]->symb->identifier());
                        
            if (options.isOn(GPU_O0))
            {
                funcCallKernel->addArg(*new SgArrayRefExp(*highI, *new SgValueExp(loopAcrossSymb[0]->len)));
                for (int i = loopV - 1; i >= 0; i--)
                    funcCallKernel->addArg(*new SgArrayRefExp(*steps, *new SgValueExp(loopSymb[i]->len)));
                funcCallKernel->addArg(*new SgArrayRefExp(*steps, *new SgValueExp(loopAcrossSymb[0]->len)));
            }
            
        }
        mywarn("  end: out adding args section");
        
        stmt = createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks);

        if (options.isOn(GPU_O0))
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
        {
            SgSymbol *tmpV = new SgSymbol(VARIABLE_NAME, "int tmpV");
            SgSymbol *tmpV1 = new SgSymbol(VARIABLE_NAME, "tmpV");	
            SgExprListExp *expr = new SgExprListExp();
            expr->setLhs(SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgVarRefExp(acrossBase[0]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len))));
            expr->setRhs(new SgExprListExp());
            expr->rhs()->setLhs(SgAssignOp(*new SgVarRefExp(tmpV1), *new SgVarRefExp(tmpV1) + *new SgValueExp(1)));
            SgForStmt *simple;
            simple = new SgForStmt(&SgAssignOp(*new SgVarRefExp(tmpV), *new SgValueExp(0)), &(*new SgVarRefExp(tmpV1) < *new SgArrayRefExp(*highI, *new SgValueExp(loopAcrossSymb[0]->len))), expr, stmt);
            st_end->insertStmtBefore(*simple);
        }
    }
    else if (acrossV == 2) // ACROSS with two dependence: generate method
    {
        // attantion!! need to add flag for support all cases
        if (loopV != 0)
        {
            SgSymbol *tmp = nums[0];
            nums[0] = nums[1];
            nums[1] = tmp;

            const char *tmpS = s_cuda_var[0];
            s_cuda_var[0] = s_cuda_var[1];
            s_cuda_var[1] = tmpS;
        }

        mywarn("strat: alloc mem");
        {
            int idx[2];
            SgStatement *st1, *st2;
            idx[1] = loopAcrossSymb[1]->len;
            idx[0] = loopAcrossSymb[0]->len;
            SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));

            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            f1->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(idx[0])));
            funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(idx[0])) - *new SgArrayRefExp(*highI, *new SgValueExp(idx[0]))));
            e = &(*funcCall + *new SgValueExp(1));
            st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(M), *e / *f1 + SgNeqOp(*e % *f1, *new SgValueExp(0))));

            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            f1->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(idx[1])));
            funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(idx[1])) - *new SgArrayRefExp(*highI, *new SgValueExp(idx[1]))));
            e = &(*funcCall + *new SgValueExp(1));
            st2 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(N), *e / *f1 + SgNeqOp(*e % *f1, *new SgValueExp(0))));

            st_end->insertStmtBefore(*st1, *st_hedr);
            st_end->insertStmtBefore(*st2, *st_hedr);
            st1->addComment("// Count used variables");
        }

        // count num_elem_y and num_elem_z
        if (loopV > 0)
        {
            SgFunctionCallExp *tempF = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[0]->len)) - *new SgArrayRefExp(*highI, *new SgValueExp(loopSymb[0]->len))));
            tempF->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[0]->len)));
            e = &SgAssignOp(*new SgVarRefExp(num_elems[1]), (*funcCall + *new SgValueExp(1)) / *tempF + SgNeqOp((*funcCall + *new SgValueExp(1)) % *tempF, *new SgValueExp(0)));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            SgExpression **e_z = new SgExpression*[loopV - 1];
            for (int k = 0; k < loopV - 1; ++k)
            {
                SgFunctionCallExp *tempF = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[k + 1]->len)) - *new SgArrayRefExp(*highI, *new SgValueExp(loopSymb[k + 1]->len))));
                tempF->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[k + 1]->len)));
                e_z[k] = &((*funcCall + *new SgValueExp(1)) / *tempF + SgNeqOp((*funcCall + *new SgValueExp(1)) % *tempF, *new SgValueExp(0)));
            }
            if (loopV > 2)
            {
                for (int k = 0; k < loopV - 1; ++k)
                {
                    e = &SgAssignOp(*new SgVarRefExp(num_elems[k + 3]), *e_z[k]);
                    stmt = new SgCExpStmt(*e);
                    st_end->insertStmtBefore(*stmt, *st_hedr);

                    if (k == 0)
                        e_z[0] = new SgVarRefExp(num_elems[k + 3]);
                    else
                        e_z[0] = &(*(e_z[0]) * (*new SgVarRefExp(num_elems[k + 3])));
                }
            }

            if (loopV > 1)
            {
                e = &SgAssignOp(*new SgVarRefExp(num_elems[2]), *e_z[0]);
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            delete[]e_z;
        }
        funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("dim3"));
        funcCall->addArg(*new SgVarRefExp(nums[0]));
        for (int k = 1; k < MIN(loopV + 1, 3); ++k)
        {
            funcCall->addArg(*new SgVarRefExp(nums[k]));
        }

        e = &SgAssignOp(*new SgVarRefExp(s_blocks), *funcCall);
        st_end->insertStmtBefore(*new SgCExpStmt(*e), *st_hedr);

        for (int k = 1; k < MIN(loopV + 1, 3); ++k)
        {
            e = new SgExpression(NOTEQL_OP, &(*new SgVarRefExp(num_elems[k]) % *new SgVarRefExp(nums[k])), new SgValueExp(0), s);
            e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[k]), *new SgVarRefExp(num_elems[k]) / *new SgVarRefExp(nums[k]) + *e);
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("MIN"));
        funcCall->addArg(*new SgVarRefExp(M));
        funcCall->addArg(*new SgVarRefExp(N));
        e = &SgAssignOp(*new SgVarRefExp(q), *funcCall);
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);

        mywarn("  end: alloc mem");

        if (red_list)
        {
            mywarn("strat: in red section");
            if (loopV == 0)
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), *new SgVarRefExp(q));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            else if (loopV == 1)
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), (*new SgVarRefExp(q) / *new SgVarRefExp(nums[0]) +
                    SgNeqOp(*new SgVarRefExp(q) % *new SgVarRefExp(nums[0]), *new SgValueExp(0))) *
                    *new SgRecordRefExp(*s_blocks, "y"));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            else
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), (*new SgVarRefExp(q) / *new SgVarRefExp(nums[0]) +
                    SgNeqOp(*new SgVarRefExp(q) % *new SgVarRefExp(nums[0]), *new SgValueExp(0))) *
                    *new SgRecordRefExp(*s_blocks, "y") * *new SgRecordRefExp(*s_blocks, "z"));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }

            e = &SgAssignOp(*new SgVarRefExp(s_tmp_var_1), *new SgValueExp(1));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            for (er = red_list, ln = 0; er; er = er->rhs(), ++ln)
            {
                e = &SgAssignOp(*new SgVarRefExp(s_tmp_var), *new SgValueExp(ln+1));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);

                stmt = new SgCExpStmt(*PrepareReduction(s_loop_ref, s_tmp_var, red_blocks, s_tmp_var_1));
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }

            mywarn("  end: out red section");
        }

        mywarn("strat: init bases");
        // init bases
        for (int i = 0; i < acrossV; ++i)
        {
            e = &SgAssignOp(*new SgVarRefExp(acrossBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[i]->len)));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
            if (i == 0)
                stmt->addComment("// Start SOR method here");
        }
        for (int i = 0; i < loopV; ++i)
        {
            e = &SgAssignOp(*new SgVarRefExp(loopBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[i]->len)));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }

        mywarn("  end: init bases");
        mywarn("start: block1");

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        st_end->insertStmtBefore(*stmt, *st_hedr);

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(diag) + *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);

        SgWhileStmt *while_st = new SgWhileStmt(*new SgVarRefExp(diag) <= *new SgVarRefExp(q), *stmt);
        st_end->insertStmtBefore(*while_st, *st_hedr);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgVarRefExp(acrossBase[0]) +
            *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)));
        stmt = new SgCExpStmt(*e);


        while_st->insertStmtAfter(*stmt);
        /* --------- add argument list to kernel call ----*/
        {
            funcCallKernel = CallKernel(kernel_symb, espec);
            for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
            {
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
                funcCallKernel->addArg(*e);
                for (int i = NumberOfCoeffs(sg); i>0; i--)
                    funcCallKernel->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
            }
            if (red_list)
                insertReductionArgs(reduction_ptr, reduction_loc_ptr, reduction_symb, reduction_loc_symb, funcCallKernel, has_red_array);

            if (options.isOn(RTC)) // diag is modifiable value
            {
                SgVarRefExp *toAdd = new SgVarRefExp(diag);
                toAdd->addAttribute(RTC_NOT_REPLACE);
                funcCallKernel->addArg(*toAdd);
            }
            else
                funcCallKernel->addArg(*new SgVarRefExp(diag));

            if (loopV > 2)
            for (int k = 1; k < loopV + 2; ++k)
            {
                if (loopV > 2 && k == 2)
                    continue;
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            }
            else if (loopV > 0)
            for (int k = 1; k < loopV + 1; ++k)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            for (int i = 0; i < acrossV; ++i)
            {
                if (i <= 1 && options.isOn(RTC)) // across base is modifiable value
                {
                    SgVarRefExp *toAdd = new SgVarRefExp(acrossBase[i]);
                    toAdd->addAttribute(RTC_NOT_REPLACE);
                    funcCallKernel->addArg(*toAdd);
                }
                else
                    funcCallKernel->addArg(*new SgVarRefExp(acrossBase[i]));
            }
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgVarRefExp(loopBase[i]));
            for (int i = 0; i < acrossV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[i]->len)));

            for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
            {
                if (s->attributes() & USE_IN_BIT)
                    funcCallKernel->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
                else
                {                                                   // passing argument by reference to kernel
                    SgType *tp = NULL;
                    if (s->type()->hasBaseType())
                        tp = s->type()->baseType();
                    else
                        tp = s->type();
                    e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? tp : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
                    funcCallKernel->addArg(*e);
                    sdev = sdev->next();
                }
            }

            if (options.isOn(AUTO_TFM))
            {
                for (size_t i = 0; i < dvm_array_headers.size(); ++i)
                {
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(0)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(1)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(2)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(3)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(4)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(5)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(6)));
                    funcCallKernel->addArg(*new SgVarRefExp(*outTypeOfTransformation[i]));
                }
            }
            funcCallKernel->addArg(*new SgVarRefExp(type_of_run));
            for (int i = 0; i < acrossV + loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*bIdxs, *new SgValueExp(i)));
        }

        stmt = createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks);
        while_st->insertStmtAfter(*stmt);

        mywarn("  end: block1");
        mywarn("start: block2");

        ex = new SgExpression(NOTEQL_OP, &(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[0])), new SgValueExp(0), s);
        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[0]) + *ex);
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt);

        e = &SgAssignOp(*new SgVarRefExp(*diag), *new SgVarRefExp(*diag) + *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);

        funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
        funcCall->addArg(*new SgVarRefExp(*M) - *new SgVarRefExp(*N));
        SgWhileStmt *while_st1 = new SgWhileStmt(*new SgVarRefExp(diag) < *funcCall, *stmt);
        SgWhileStmt *while_st2 = new SgWhileStmt(*new SgVarRefExp(diag) < *funcCall, stmt->copy());
        SgWhileStmt *while_st3 = new SgWhileStmt(*new SgVarRefExp(diag) < *new SgVarRefExp(M) + *new SgVarRefExp(N), stmt->copy());
        SgWhileStmt *while_st4 = new SgWhileStmt(*new SgVarRefExp(diag) < *new SgVarRefExp(M) + *new SgVarRefExp(N), stmt->copy());
        SgIfStmt *if_st = new SgIfStmt(*new SgVarRefExp(*N) < *new SgVarRefExp(*M), *while_st3, *while_st4);
        st_end->insertStmtBefore(*if_st, *st_hedr);

        e = &SgAssignOp(*new SgVarRefExp(*elem), *new SgVarRefExp(q) - *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        if_st->insertStmtAfter(*stmt);

        if_st->falseBody()->insertStmtBefore(stmt->copy());
        if_st->falseBody()->insertStmtBefore(*while_st2);
        if_st->falseBody()->insertStmtBefore(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(diag), *new SgValueExp(0))));

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgVarRefExp(acrossBase[0])
            - *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)));
        stmt = new SgCExpStmt(*e);
        if_st->insertStmtAfter(*stmt);
        if_st->falseBody()->insertStmtBefore(stmt->copy());

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(q) + *funcCall + *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        if_st->lexNext()->insertStmtAfter(*stmt);
        if_st->falseBody()->lexNext()->lexNext()->lexNext()->insertStmtAfter(stmt->copy(), *if_st);
        e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[1]->len)) +
            *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)));
        stmt = new SgCExpStmt(*e);
        if_st->insertStmtAfter(*stmt);
        if_st->falseBody()->insertStmtBefore(stmt->copy());

        if_st->insertStmtAfter(*while_st1);
        if_st->insertStmtAfter(*new SgCExpStmt(SgAssignOp(*new SgVarRefExp(diag), *new SgValueExp(0))));


        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgVarRefExp(acrossBase[0]) +
            *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)));
        stmt = new SgCExpStmt(*e);
        while_st1->insertStmtAfter(*stmt);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) +
            *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)));
        stmt = new SgCExpStmt(*e);

        while_st2->insertStmtAfter(*stmt);
        while_st3->insertStmtAfter(stmt->copy());
        while_st4->insertStmtAfter(stmt->copy());

        mywarn("  end: block2");
        mywarn("start: block3");

        e = &SgAssignOp(*new SgVarRefExp(*elem), *new SgVarRefExp(*elem) - *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        while_st3->lastExecutable()->insertStmtAfter(*stmt);
        while_st4->lastExecutable()->insertStmtAfter(stmt->copy());

        /* --------- add argument list to kernel call ----*/
        {
            funcCallKernel = CallKernel(kernel_symb, espec);
            for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
            {
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
                funcCallKernel->addArg(*e);
                for (int i = NumberOfCoeffs(sg); i > 0; i--)
                    funcCallKernel->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
            }
            if (red_list)
                insertReductionArgs(reduction_ptr, reduction_loc_ptr, reduction_symb, reduction_loc_symb, funcCallKernel, has_red_array);

            if (options.isOn(RTC)) // q is modifiable value
            {
                SgVarRefExp *toAdd = new SgVarRefExp(q);
                toAdd->addAttribute(RTC_NOT_REPLACE);
                funcCallKernel->addArg(*toAdd);
            }
            else
                funcCallKernel->addArg(*new SgVarRefExp(q));

            if (loopV > 2)
            for (int k = 1; k < loopV + 2; ++k)
            {
                if (loopV > 2 && k == 2)
                    continue;
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            }
            else if (loopV > 0)
            for (int k = 1; k < loopV + 1; ++k)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            for (int i = 0; i < acrossV; ++i)
            {
                if (i <= 1 && options.isOn(RTC)) // across base is modifiable value
                {
                    SgVarRefExp *toAdd = new SgVarRefExp(acrossBase[i]);
                    toAdd->addAttribute(RTC_NOT_REPLACE);
                    funcCallKernel->addArg(*toAdd);
                }
                else
                    funcCallKernel->addArg(*new SgVarRefExp(acrossBase[i]));
            }
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgVarRefExp(loopBase[i]));
            for (int i = 0; i < acrossV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[i]->len)));

            for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
            {
                if (s->attributes() & USE_IN_BIT)
                    funcCallKernel->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
                else
                {                                                   // passing argument by reference to kernel
                    SgType *tp = NULL;
                    if (s->type()->hasBaseType())
                        tp = s->type()->baseType();
                    else
                        tp = s->type();
                    e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? tp : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
                    funcCallKernel->addArg(*e);
                    sdev = sdev->next();
                }
            }

            if (options.isOn(AUTO_TFM))
            {
                for (size_t i = 0; i < dvm_array_headers.size(); ++i)
                {
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(0)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(1)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(2)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(3)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(4)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(5)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(6)));
                    funcCallKernel->addArg(*new SgVarRefExp(*outTypeOfTransformation[i]));
                }
            }
            funcCallKernel->addArg(*new SgVarRefExp(type_of_run));
            for (int i = 0; i < acrossV + loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*bIdxs, *new SgValueExp(i)));
        }

        while_st1->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));
        while_st2->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));

        mywarn("  end: block3");

        /* --------- add argument list to kernel call ----*/
        {
            funcCallKernel = CallKernel(kernel_symb, espec);

            for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
            {
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
                funcCallKernel->addArg(*e);
                for (int i = NumberOfCoeffs(sg); i>0; i--)
                    funcCallKernel->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
            }
            if (red_list)
                insertReductionArgs(reduction_ptr, reduction_loc_ptr, reduction_symb, reduction_loc_symb, funcCallKernel, has_red_array);

            if (options.isOn(RTC)) // elem is modifiable value
            {
                SgVarRefExp *toAdd = new SgVarRefExp(elem);
                toAdd->addAttribute(RTC_NOT_REPLACE);
                funcCallKernel->addArg(*toAdd);
            }
            else
                funcCallKernel->addArg(*new SgVarRefExp(elem));

            if (loopV > 2)
            for (int k = 1; k < loopV + 2; ++k)
            {
                if (loopV > 2 && k == 2)
                    continue;
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            }
            else if (loopV > 0)
            for (int k = 1; k < loopV + 1; ++k)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[k]));
            for (int i = 0; i < acrossV; ++i)
            {
                if (i <= 1 && options.isOn(RTC)) // across base is modifiable value
                {
                    SgVarRefExp *toAdd = new SgVarRefExp(acrossBase[i]);
                    toAdd->addAttribute(RTC_NOT_REPLACE);
                    funcCallKernel->addArg(*toAdd);
                }
                else
                    funcCallKernel->addArg(*new SgVarRefExp(acrossBase[i]));
            }
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgVarRefExp(loopBase[i]));
            for (int i = 0; i < acrossV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[i]->len)));

            for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
            {
                if (s->attributes() & USE_IN_BIT)
                    funcCallKernel->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
                else
                {                                                   // passing argument by reference to kernel
                    SgType *tp = NULL;
                    if (s->type()->hasBaseType())
                        tp = s->type()->baseType();
                    else
                        tp = s->type();
                    e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? tp : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
                    funcCallKernel->addArg(*e);
                    sdev = sdev->next();
                }
            }

            if (options.isOn(AUTO_TFM))
            {
                for (size_t i = 0; i < dvm_array_headers.size(); ++i)
                {
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(0)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(1)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(2)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(3)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(4)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(5)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(6)));
                    funcCallKernel->addArg(*new SgVarRefExp(*outTypeOfTransformation[i]));
                }
            }
            funcCallKernel->addArg(*new SgVarRefExp(type_of_run));
            for (int i = 0; i < acrossV + loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*bIdxs, *new SgValueExp(i)));
        }

        while_st3->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));
        while_st4->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));


        ex = new SgExpression(MOD_OP, new SgVarRefExp(q), new SgVarRefExp(nums[0]), s);
        ex = new SgExpression(NOTEQL_OP, ex, new SgValueExp(0), s);
        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *new SgVarRefExp(q) / *new SgVarRefExp(nums[0]) + *ex);
        while_st1->insertStmtAfter(*new SgCExpStmt(*e));
        while_st2->insertStmtAfter(*new SgCExpStmt(*e));

        SgExpression *ex1 = &(*new SgVarRefExp(*elem));
        ex = new SgExpression(MOD_OP, ex1, new SgVarRefExp(nums[0]), s);
        ex = new SgExpression(NOTEQL_OP, ex, new SgValueExp(0), s);
        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *ex1 / *new SgVarRefExp(nums[0]) + *ex);
        while_st3->insertStmtAfter(*new SgCExpStmt(*e));
        while_st4->insertStmtAfter(*new SgCExpStmt(*e));
    }
    else if (acrossV >= 3) // ACROSS with three or more dependence: generate method
    {
        // attantion!! need to add flag for support all cases
        if (loopV != 0)
        {
            SgSymbol *tmp = nums[0];
            nums[0] = nums[2];
            nums[2] = tmp;

            const char *tmpS = s_cuda_var[0];
            s_cuda_var[0] = s_cuda_var[2];
            s_cuda_var[2] = tmpS;
        }

        /* --------- add argument list to kernel call ----*/
        {
            funcCallKernel = CallKernel(kernel_symb, espec);
            for (sg = hgpu_first, sb = base_first, sl = acc_array_list, ln = 0; ln<num; sg = sg->next(), sb = sb->next(), sl = sl->next, ln++)
            {
                e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? C_Type(sl->symb->type()) : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sb));
                funcCallKernel->addArg(*e);
                for (int i = NumberOfCoeffs(sg); i>0; i--)
                    funcCallKernel->addArg(*new SgArrayRefExp(*sg, *new SgValueExp(i)));
            }
            if (red_list)
                insertReductionArgs(reduction_ptr, reduction_loc_ptr, reduction_symb, reduction_loc_symb, funcCallKernel, has_red_array);

            for (int i = 0; i < acrossV; ++i)
            {
                if (options.isOn(RTC)) // across base is modifiable value
                {
                    SgVarRefExp *toAdd = new SgVarRefExp(acrossBase[i]);
                    toAdd->addAttribute(RTC_NOT_REPLACE);
                    funcCallKernel->addArg(*toAdd);
                }
                else
                    funcCallKernel->addArg(*new SgVarRefExp(acrossBase[i]));
            }
            for (int i = 0; i < loopV; ++i)
                    funcCallKernel->addArg(*new SgVarRefExp(loopBase[i]));
            for (int i = 0; i < acrossV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[i]->len)));

            for (s = uses_first, sdev = scalar_first, ln = 0; ln < uses_num; s = s->next(), ln++)  // uses
            {
                if (s->attributes() & USE_IN_BIT)
                    funcCallKernel->addArg(SgDerefOp(*new SgVarRefExp(*s)));   // passing argument by value to kernel
                else
                {                                                   // passing argument by reference to kernel
                    SgType *tp = NULL;
                    if (s->type()->hasBaseType())
                        tp = s->type()->baseType();
                    else
                        tp = s->type();
                    e = new SgCastExp(*C_PointerType(options.isOn(C_CUDA) ? tp : new SgDescriptType(*SgTypeChar(), BIT_SIGNED)), *new SgVarRefExp(sdev));
                    funcCallKernel->addArg(*e);
                    sdev = sdev->next();
                }
            }
            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("MIN"));
            funcCall->addArg(*new SgVarRefExp(M1));
            funcCall->addArg(*new SgVarRefExp(M2));

            if (options.isOn(RTC)) // diag and SE are modifiable value
            {
                SgVarRefExp *toAdd = new SgVarRefExp(diag);
                toAdd->addAttribute(RTC_NOT_REPLACE);
                funcCallKernel->addArg(*toAdd);

                toAdd = new SgVarRefExp(SE);
                toAdd->addAttribute(RTC_NOT_REPLACE);
                funcCallKernel->addArg(*toAdd);
            }
            else
            {
                funcCallKernel->addArg(*new SgVarRefExp(diag));
                funcCallKernel->addArg(*new SgVarRefExp(SE));
            }
            funcCallKernel->addArg(*new SgVarRefExp(var1));
            funcCallKernel->addArg(*new SgVarRefExp(var2));
            funcCallKernel->addArg(*new SgVarRefExp(var3));            
            funcCallKernel->addArg(*new SgVarRefExp(Emax));
            funcCallKernel->addArg(*new SgVarRefExp(Emin));
            funcCallKernel->addArg(*funcCall);
            funcCallKernel->addArg(*new SgVarRefExp(M1) > *new SgVarRefExp(M2));
            if (loopV > 0)
            for (int i = 0; i < loopV; ++i)
                funcCallKernel->addArg(*new SgVarRefExp(num_elems[i]));

            if (options.isOn(AUTO_TFM))
            {
                for (size_t i = 0; i < dvm_array_headers.size(); ++i)
                {
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(0)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(1)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(2)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(3)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(4)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(5)));
                    funcCallKernel->addArg(*new SgArrayRefExp(*addressingParams[i], *new SgValueExp(6)));
                    funcCallKernel->addArg(*new SgVarRefExp(*outTypeOfTransformation[i]));
                }
            }
            funcCallKernel->addArg(*new SgVarRefExp(type_of_run));
            for (int i = 0; i < acrossV + loopV; ++i)
                funcCallKernel->addArg(*new SgArrayRefExp(*bIdxs, *new SgValueExp(i)));
        }

        {
            int idx[3];
            SgStatement *st1;
            for (int i = 0; i < 3; ++i)
                idx[i] = loopAcrossSymb[i]->len;

            for (int i = 0; i < 3; ++i)
            {
                SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(M1), *e / *f1 + SgNeqOp(*e % *f1, *new SgValueExp(0))));
                f1->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(idx[i])));
                funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(idx[i])) - *new SgArrayRefExp(*highI, *new SgValueExp(idx[i]))));
                e = &(*funcCall + *new SgValueExp(1));

                if (i == 0)
                    st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(M1), *e / *f1 + SgNeqOp(*e % *f1, *new SgValueExp(0))));
                else if (i == 1)
                    st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(M2), *e / *f1 + SgNeqOp(*e % *f1, *new SgValueExp(0))));
                else
                    st1 = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(M3), *e / *f1 + SgNeqOp(*e % *f1, *new SgValueExp(0))));
                st_end->insertStmtBefore(*st1, *st_hedr);
                if (i == 0)
                    st1->addComment("// Count used variables");
            }

            SgFunctionCallExp *f1 = new SgFunctionCallExp(*createNewFunctionSymbol("MIN"));
            SgFunctionCallExp *f2 = new SgFunctionCallExp(*createNewFunctionSymbol("MIN"));
            f1->addArg(*new SgVarRefExp(M1));
            f1->addArg(*new SgVarRefExp(M2));
            f2->addArg(*f1);
            f2->addArg(*new SgVarRefExp(M3));
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(Allmin), *f2));
            st_end->insertStmtBefore(*stmt, *st_hedr);

            f2 = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            f2->addArg(*new SgVarRefExp(M1) - *new SgVarRefExp(M2));

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(Emin), *f1));
            st_end->insertStmtBefore(*stmt, *st_hedr);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(Emax), *f1 + *f2 + *new SgValueExp(1)));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
        // count num_elem_z
        if (loopV > 0)
        {
            SgFunctionCallExp *tempF = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[0]->len)) - *new SgArrayRefExp(*highI, *new SgValueExp(loopSymb[0]->len))));
            tempF->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[0]->len)));
            e = &SgAssignOp(*new SgVarRefExp(num_elems[0]), (*funcCall + *new SgValueExp(1)) / *tempF + SgNeqOp((*funcCall + *new SgValueExp(1)) % *tempF, *new SgValueExp(0)));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            if (loopV > 1)
            {
                SgExpression **e_z = new SgExpression*[loopV - 1];
                for (int k = 0; k < loopV - 1; ++k)
                {
                    SgFunctionCallExp *tempF = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                    funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                    funcCall->addArg((*new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[k + 1]->len)) - *new SgArrayRefExp(*highI, *new SgValueExp(loopSymb[k + 1]->len))));
                    tempF->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopSymb[k + 1]->len)));
                    e_z[k] = &((*funcCall + *new SgValueExp(1)) / *tempF + SgNeqOp((*funcCall + *new SgValueExp(1)) % *tempF, *new SgValueExp(0)));
                }

                for (int k = 0; k < loopV - 1; ++k)
                {
                    e = &SgAssignOp(*new SgVarRefExp(num_elems[k + 1]), *e_z[k]);
                    stmt = new SgCExpStmt(*e);
                    st_end->insertStmtBefore(*stmt, *st_hedr);

                    if (k == 0)
                        e_z[0] = &(*new SgVarRefExp(num_elems[0]) * (*new SgVarRefExp(num_elems[k + 1])));
                    else
                        e_z[0] = &(*(e_z[0]) * (*new SgVarRefExp(num_elems[k + 1])));
                }

                e = &SgAssignOp(*new SgVarRefExp(num_elems[loopV]), *e_z[0]);
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);

                delete[]e_z;
            }
            else
            {
                e = &SgAssignOp(*new SgVarRefExp(num_elems[loopV]), *new SgVarRefExp(num_elems[0]));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
        }

        funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("dim3"));
        if (loopV > 0)
        {
            funcCall->addArg(*new SgVarRefExp(num_elems[loopV]) / *new SgVarRefExp(*nums[2]) + SgNeqOp(*new SgVarRefExp(num_elems[loopV]) % *new SgVarRefExp(*nums[2]), *new SgValueExp(0)));
            funcCall->addArg(*new SgVarRefExp(nums[1]));
            funcCall->addArg(*new SgVarRefExp(nums[0]));
        }
        else
        {
            funcCall->addArg(*new SgVarRefExp(nums[0]));
            funcCall->addArg(*new SgVarRefExp(nums[1]));
        }

        e = &SgAssignOp(*new SgVarRefExp(s_blocks), *funcCall);
        st_end->insertStmtBefore(*new SgCExpStmt(*e), *st_hedr);

        if (red_list)
        {
            mywarn("strat: in red section");
            if (loopV == 0)
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), *new SgVarRefExp(Emin) * *new SgVarRefExp(Allmin));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
            else if (loopV > 0)
            {
                e = &SgAssignOp(*new SgVarRefExp(*red_blocks), (*new SgVarRefExp(Emin) / *new SgVarRefExp(nums[0]) +
                    SgNeqOp(*new SgVarRefExp(Emin) % *new SgVarRefExp(nums[0]), *new SgValueExp(0))) *
                    (*new SgVarRefExp(Allmin) / *new SgVarRefExp(nums[1]) +
                    SgNeqOp(*new SgVarRefExp(Allmin) % *new SgVarRefExp(nums[1]), *new SgValueExp(0)))
                    * *new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[2]));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(s_tmp_var_1), *new SgValueExp(1)));
            st_end->insertStmtBefore(*stmt, *st_hedr);

            for (er = red_list, ln = 0; er; er = er->rhs(), ++ln)
            {
                e = &SgAssignOp(*new SgVarRefExp(s_tmp_var), *new SgValueExp(ln+1));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);

                stmt = new SgCExpStmt(*PrepareReduction(s_loop_ref, s_tmp_var, red_blocks, s_tmp_var_1));
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }

            mywarn("  end: out red section");
        }

        int flag_comment = 0;
        for (int i = 3; i < acrossV; ++i)
        {
            e = &SgAssignOp(*new SgVarRefExp(acrossBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[i]->len)));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);
            if (i - 3 == 0)
            {
                stmt->addComment("// Start method");
                flag_comment = 1;
            }
        }

        if (acrossV == 3)
        {
            for (int i = 0; i < MIN(3, acrossV); ++i)
            {
                e = &SgAssignOp(*new SgVarRefExp(acrossBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[i]->len)));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
                if (i == 0 && flag_comment == 0)
                    stmt->addComment("// Start method");
            }

            for (int i = 0; i < loopV; ++i)
            {
                e = &SgAssignOp(*new SgVarRefExp(loopBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[i]->len)));
                stmt = new SgCExpStmt(*e);
                st_end->insertStmtBefore(*stmt, *st_hedr);
            }
        }
        SgWhileStmt *main_while_st = NULL;
        SgStatement *main_stmt = NULL;
        bool first = true;
        if (acrossV > 3)
        {
            SgWhileStmt *tmp;
            for (int i = 3; i < acrossV; ++i)
            {
                e = &SgAssignOp(*new SgVarRefExp(acrossBase[i]), *new SgVarRefExp(acrossBase[i]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
                stmt = new SgCExpStmt(*e);
                SgExpression *e1 = NULL;
                SgFunctionCallExp *func = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
                func->addArg(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)));
                e1 = &(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[i]->len)) / *func);
                if (first)
                {
                    main_while_st = new SgWhileStmt(*e1 * *new SgVarRefExp(acrossBase[i]) <= *e1 * *new SgArrayRefExp(*highI, *new SgValueExp(loopAcrossSymb[i]->len)), *stmt);
                    first = false;
                }
                else
                {
                    tmp = new SgWhileStmt(*new SgVarRefExp(acrossBase[i]) <= *new SgArrayRefExp(*highI, *new SgValueExp(loopAcrossSymb[i]->len)), *stmt);
                    main_while_st->insertStmtAfter(*tmp);
                    main_while_st = tmp;
                }
                main_stmt = stmt;
            }
            st_end->insertStmtBefore(*main_while_st, *st_hedr);

            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(SE), *new SgValueExp(1)));
            main_stmt->insertStmtBefore(*stmt, *main_while_st);
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(diag), *new SgValueExp(1)));
            main_stmt->insertStmtBefore(*stmt, *main_while_st);
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(var1), *new SgValueExp(1)));
            main_stmt->insertStmtBefore(*stmt, *main_while_st);
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(var2), *new SgValueExp(0)));
            main_stmt->insertStmtBefore(*stmt, *main_while_st);
            stmt = new SgCExpStmt(SgAssignOp(*new SgVarRefExp(var3), *new SgValueExp(0)));
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

            for (int i = 0; i < MIN(3, acrossV); ++i)
            {
                e = &SgAssignOp(*new SgVarRefExp(acrossBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[i]->len)));
                stmt = new SgCExpStmt(*e);
                main_stmt->insertStmtBefore(*stmt, *main_while_st);
            }

            for (int i = 0; i < loopV; ++i)
            {
                e = &SgAssignOp(*new SgVarRefExp(loopBase[i]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopSymb[i]->len)));
                stmt = new SgCExpStmt(*e);
                main_stmt->insertStmtBefore(*stmt, *main_while_st);
            }
        }

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(diag) + *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);

        SgWhileStmt *while_st = new SgWhileStmt(*new SgVarRefExp(diag) <= *new SgVarRefExp(Allmin), *stmt);
        if (acrossV == 3)
            st_end->insertStmtBefore(*while_st, *st_hedr);
        else
            main_stmt->insertStmtBefore(*while_st, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[2]), *new SgVarRefExp(acrossBase[2]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[2]->len)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt);                
        while_st->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));

        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[1]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[1]) + SgNeqOp(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[1]), *new SgValueExp(0)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt);

        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[0]) + SgNeqOp(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[0]), *new SgValueExp(0)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt);

        e = &SgAssignOp(*new SgVarRefExp(var1), *new SgValueExp(0));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(var2), *new SgValueExp(0));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(var3), *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        { // while for if block
            e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(diag) + *new SgValueExp(1));
            stmt = new SgCExpStmt(*e);

            SgWhileStmt *while_st = new SgWhileStmt(SgNeqOp(*new SgVarRefExp(diag) - *new SgValueExp(1), *new SgVarRefExp(M3)), *stmt);

            e = &SgAssignOp(*new SgVarRefExp(acrossBase[2]), *new SgVarRefExp(acrossBase[2]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[2]->len)));
            stmt = new SgCExpStmt(*e);
            while_st->insertStmtAfter(*stmt);
            while_st->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));

            e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[1]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[1]) + SgNeqOp(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[1]), *new SgValueExp(0)));
            stmt = new SgCExpStmt(*e);
            while_st->insertStmtAfter(*stmt);

            e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *new SgVarRefExp(Emin) / *new SgVarRefExp(nums[0]) + SgNeqOp(*new SgVarRefExp(Emin) % *new SgVarRefExp(nums[0]), *new SgValueExp(0)));
            stmt = new SgCExpStmt(*e);
            while_st->insertStmtAfter(*stmt);

            SgIfStmt *if_st = new SgIfStmt(*new SgVarRefExp(M3) > *new SgVarRefExp(Emin), *while_st);
            if (acrossV == 3)
                st_end->insertStmtBefore(*if_st, *st_hedr);
            else
                main_stmt->insertStmtBefore(*if_st, *main_while_st);

            e = &SgAssignOp(*new SgVarRefExp(*diag), *new SgVarRefExp(*Allmin) + *new SgValueExp(1));
            stmt = new SgCExpStmt(*e);
            if_st->insertStmtAfter(*stmt);

            e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[1]->len)) * (*new SgVarRefExp(M1) <= *new SgVarRefExp(M2)) + *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)) * (*new SgVarRefExp(M1) > *new SgVarRefExp(M2)));
            stmt = new SgCExpStmt(*e);
            if_st->insertStmtAfter(*stmt);

            e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)) * (*new SgVarRefExp(M1) <= *new SgVarRefExp(M2)) + *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[1]->len)) * (*new SgVarRefExp(M1) > *new SgVarRefExp(M2)));
            stmt = new SgCExpStmt(*e);
            if_st->insertStmtAfter(*stmt);
        }

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(M3));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[1]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[1]) + SgNeqOp(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[1]), *new SgValueExp(0)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *new SgVarRefExp(Emin) / *new SgVarRefExp(nums[0]) + SgNeqOp(*new SgVarRefExp(Emin) % *new SgVarRefExp(nums[0]), *new SgValueExp(0)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(SE), *new SgValueExp(2));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), (*new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len))) * (*new SgVarRefExp(M1) <= *new SgVarRefExp(M2)) + (*new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[1]->len)) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len))) * (*new SgVarRefExp(M1) > *new SgVarRefExp(M2)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[1]->len)) * (*new SgVarRefExp(M1) <= *new SgVarRefExp(M2)) + *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)) * (*new SgVarRefExp(M1) > *new SgVarRefExp(M2)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[2]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[2]->len)) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[2]->len)) * (*new SgVarRefExp(M3) - *new SgValueExp(1)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(SE), *new SgVarRefExp(SE) + *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);

        while_st = new SgWhileStmt(SgNeqOp(*new SgVarRefExp(M1) + *new SgVarRefExp(M2) - *new SgVarRefExp(Allmin), *new SgVarRefExp(SE) - *new SgValueExp(1)), *stmt);
        if (acrossV == 3)
            st_end->insertStmtBefore(*while_st, *st_hedr);
        else
            main_stmt->insertStmtBefore(*while_st, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgVarRefExp(acrossBase[0]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)) * (*new SgVarRefExp(M1) <= *new SgVarRefExp(M2)) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) * (*new SgVarRefExp(M1) > *new SgVarRefExp(M2)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt, *while_st);
        while_st->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));

        e = &SgAssignOp(*new SgVarRefExp(var1), *new SgValueExp(0));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(var2), *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(var3), *new SgValueExp(0));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(Allmin) - *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[0]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[0]->len)) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[0]->len)) * (*new SgVarRefExp(M1) - *new SgValueExp(1)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgArrayRefExp(*lowI, *new SgValueExp(loopAcrossSymb[1]->len)) * (*new SgVarRefExp(M1) > *new SgVarRefExp(M2)) + *new SgVarRefExp(acrossBase[1]) * (*new SgVarRefExp(M1) <= *new SgVarRefExp(M2)));
        stmt = new SgCExpStmt(*e);
        if (acrossV == 3)
            st_end->insertStmtBefore(*stmt, *st_hedr);
        else
            main_stmt->insertStmtBefore(*stmt, *main_while_st);

        { // if block
            funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
            funcCall->addArg(*new SgVarRefExp(*Emin) - *new SgVarRefExp(M3));
            SgExpression *e1 = NULL, *e2 = NULL;
            SgIfStmt *if_st1 = NULL;

            e1 = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) + *new SgVarRefExp(*Emax) - *new SgVarRefExp(*Emin) - *new SgValueExp(1));
            e2 = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) - *new SgVarRefExp(*Emax) + *new SgVarRefExp(*Emin) + *new SgValueExp(1));

            if_st1 = new SgIfStmt(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) > *new SgValueExp(0), *new SgCExpStmt(*e1), *new SgCExpStmt(*e2));

            SgIfStmt *if_st = new SgIfStmt(*new SgVarRefExp(*M1) <= *new SgVarRefExp(*M2) && *new SgVarRefExp(*M3) > *new SgVarRefExp(*Emin), *if_st1);

            e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)));
            stmt = new SgCExpStmt(*e);
            if_st = new SgIfStmt(*new SgVarRefExp(*M1) > *new SgVarRefExp(*M2) && *new SgVarRefExp(*M3) > *new SgVarRefExp(*Emin), *stmt, *if_st);

            e1 = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) + *new SgVarRefExp(*Emax) - *new SgVarRefExp(*Emin) - *new SgValueExp(1) + *funcCall);
            e2 = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) - *new SgVarRefExp(*Emax) + *new SgVarRefExp(*Emin) + *new SgValueExp(1) + *new SgVarRefExp(M3) - *new SgVarRefExp(*Emin));

            if_st1 = new SgIfStmt(*new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) > *new SgValueExp(0), *new SgCExpStmt(*e1), *new SgCExpStmt(*e2));
            
            if_st = new SgIfStmt(*new SgVarRefExp(*M1) <= *new SgVarRefExp(*M2) && *new SgVarRefExp(*M3) <= *new SgVarRefExp(*Emin), *if_st1, *if_st);

            e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)) + *funcCall);
            stmt = new SgCExpStmt(*e);
            if_st = new SgIfStmt(*new SgVarRefExp(*M1) > *new SgVarRefExp(*M2) && *new SgVarRefExp(*M3) <= *new SgVarRefExp(*Emin), *stmt, *if_st);

            if (acrossV == 3)
                st_end->insertStmtBefore(*if_st, *st_hedr);
            else
                main_stmt->insertStmtBefore(*if_st, *main_while_st);
        }

        e = &SgAssignOp(*new SgVarRefExp(diag), *new SgVarRefExp(diag) - *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);

        while_st = new SgWhileStmt(SgNeqOp(*new SgVarRefExp(diag), *new SgValueExp(0)), *stmt);
        if (acrossV == 3)
            st_end->insertStmtBefore(*while_st, *st_hedr);
        else
            main_stmt->insertStmtBefore(*while_st, *main_while_st);

        e = &SgAssignOp(*new SgVarRefExp(acrossBase[1]), *new SgVarRefExp(acrossBase[1]) + *new SgArrayRefExp(*idxI, *new SgValueExp(loopAcrossSymb[1]->len)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt, *while_st);

        e = &SgAssignOp(*new SgVarRefExp(SE), *new SgVarRefExp(SE) + *new SgValueExp(1));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt, *while_st);
        while_st->insertStmtAfter(*createKernelCallsInCudaHandler(funcCallKernel, s_loop_ref, idxTypeInKernel, s_blocks));

        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[1]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[1]) + SgNeqOp(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[1]), *new SgValueExp(0)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt, *while_st);

        e = &SgAssignOp(*new SgRecordRefExp(*s_blocks, (char*)s_cuda_var[0]), *new SgVarRefExp(diag) / *new SgVarRefExp(nums[0]) + SgNeqOp(*new SgVarRefExp(diag) % *new SgVarRefExp(nums[0]), *new SgValueExp(0)));
        stmt = new SgCExpStmt(*e);
        while_st->insertStmtAfter(*stmt, *while_st);
    }

    // !!! Global for all cases !!!
    if (red_list)
    {
        ln = 0;
        for (er = red_list; er; er = er->rhs(), ++ln)
        {
            //SgExpression *red_expr_ref = er->lhs()->rhs(); // reduction variable reference
            num = RedFuncNumber(er->lhs()->lhs()); // type of reduction

            e = &SgAssignOp(*new SgVarRefExp(*s_tmp_var), *new SgValueExp(ln+1));
            stmt = new SgCExpStmt(*e);
            st_end->insertStmtBefore(*stmt, *st_hedr);

            stmt = new SgCExpStmt(*FinishReduction(s_loop_ref, s_tmp_var));
            st_end->insertStmtBefore(*stmt, *st_hedr);
        }
    }
    
    // create args for kernel and return it    
    ArgsForKernel **argsKernel = new ArgsForKernel*[countKernels];
    const int rtTypes[] = { rt_INT, rt_LLONG };

    for (unsigned ck = 0; ck < countKernels; ++ck)
    {
        argsKernel[ck] = new ArgsForKernel();

        argsKernel[ck]->st_header = st_hedr;
        argsKernel[ck]->cond_ = NULL;

        SgType *typeParams = indexTypeInKernel(rtTypes[ck]);

        if (acrossV == 1)
        {
            char *cond_ = new char[strlen("cond_") + strlen(loopAcrossSymb[0]->symb->identifier()) + 1];
            cond_[0] = '\0';
            strcat(cond_, "cond_");
            strcat(cond_, loopAcrossSymb[0]->symb->identifier());
            argsKernel[ck]->cond_ = new SgSymbol(VARIABLE_NAME, cond_, typeParams, st_hedr);

            char *st = new char[strlen("steps_") + strlen(loopAcrossSymb[0]->symb->identifier()) + 1];
            st[0] = '\0';
            strcat(st, "steps_");
            strcat(st, loopAcrossSymb[0]->symb->identifier());
            argsKernel[ck]->steps.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(st), typeParams, st_hedr));
            for (int i = 0; i < loopV; ++i)
            {
                st = new char[strlen("steps_") + strlen(loopSymb[i]->symb->identifier()) + 1];
                st[0] = '\0';
                strcat(st, "steps_");
                strcat(st, loopSymb[i]->symb->identifier());
                argsKernel[ck]->steps.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(st), typeParams, st_hedr));
            }
        }

        if (acrossV != 1 && options.isOn(AUTO_TFM))
        {
            char *tmpS = new char[64];
            for (size_t i = 0; i < dvm_array_headers.size(); ++i)
            {
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_x_axis");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_offset_x");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_Rx");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_y_axis");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_offset_y");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_Ry");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                tmpS[0] = '\0';
                strcat(tmpS, dvm_array_headers[i]);
                strcat(tmpS, "_slash");
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(tmpS), typeParams, st_hedr));
                argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(outTypeOfTransformation[i]->identifier()), typeParams, st_hedr));
            }
            argsKernel[ck]->arrayNames = dvm_array_headers;
        }

        if (acrossV == 2)
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("num_elem_across"), typeParams, st_hedr));
        else if (acrossV >= 3)
        {
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("max_z"), typeParams, st_hedr));
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("SE"), typeParams, st_hedr)); // SE
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("var1"), typeParams, st_hedr)); // var1
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("var2"), typeParams, st_hedr)); // var2
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("var3"), typeParams, st_hedr)); // var3
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Emax"), typeParams, st_hedr)); // Emax
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("Emin"), typeParams, st_hedr)); // Emin
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("min_ij"), typeParams, st_hedr));
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("swap_ij"), typeParams, st_hedr));
        }

        char *str = new char[32];
        for (int i = 0; i < acrossV; ++i)
        {
            argsKernel[ck]->acrossS.push_back(new SgSymbol(VARIABLE_NAME, acrossBase[i]->identifier(), typeParams, st_hedr)); // acrossBase[i]
            argsKernel[ck]->symb.push_back(loopAcrossSymb[i]);
            strcpy(str, "step");
            strcat(str, strchr(acrossBase[i]->identifier(), '_'));
            argsKernel[ck]->idxAcross.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(str), typeParams, st_hedr));
        }
        for (int i = 0; i < loopV; ++i)
        {
            argsKernel[ck]->notAcrS.push_back(new SgSymbol(VARIABLE_NAME, loopBase[i]->identifier(), typeParams, st_hedr)); // loopBase[i]
            argsKernel[ck]->nSymb.push_back(loopSymb[i]);
            strcpy(str, "step");
            strcat(str, strchr(loopBase[i]->identifier(), '_'));
            argsKernel[ck]->idxNotAcross.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(str), typeParams, st_hedr));
            strcpy(str, "num_elem");
            strcat(str, strchr(loopBase[i]->identifier(), '_'));
            argsKernel[ck]->sizeVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(str), typeParams, st_hedr));
        }

        if (acrossV == 1 || acrossV == 2 || acrossV >= 3)
        {
            argsKernel[ck]->otherVars.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName("type_of_run"), typeParams, st_hedr));
            char *t = new char[32];
            for (int i = 0; i < acrossV + loopV; ++i)
            {
                char p[8];
                sprintf(p, "%d", i);
                t[0] = '\0';
                strcat(t, "idxs_");
                strcat(t, p);
                argsKernel[ck]->baseIdxsInKer.push_back(new SgSymbol(VARIABLE_NAME, TestAndCorrectName(t), typeParams, st_hedr));
            }
            delete[]t;
        }

        delete[]str;
        
    }
    // end of creation args for kernel

    delete[]reduction_loc_ptr;
    delete[]reduction_loc_symb;
    delete[]reduction_ptr;
    delete[]reduction_symb;
    delete[]num_elems;
    mywarn("  end Adapter Function");
    return argsKernel;
}

void MakeDeclarationsForKernel_On_C_Across(SgType *indexType)
{
    // declare do_variables
    DeclareDoVars();

    // declare private(local in kernel) variables 
    DeclarePrivateVars();

    // declare  variables, used in loop and passed by reference:
    // <type> &<name> = *p_<name>;  
    DeclareUsedVars();
}

void MakeDeclarationsForKernelAcross(SgType *indexType)
{
#if debugMode
        mywarn("strat: MakeDeclarations Function");
#endif
    
    // declare do_variables
    DeclareDoVars();

    // declare private(local in kernel) variables   
    DeclarePrivateVars();

    // declare dummy arguments:

    // declare reduction dummy arguments
    DeclareDummyArgumentsForReductions(NULL, indexType);

    // declare array coefficients
    DeclareArrayCoeffsInKernel(indexType);

    // declare bases for arrays 
    DeclareArrayBases();

    // declare  variables, used in loop
    DeclareUsedVars();

#if debugMode
        mywarn("  end: MakeDeclarations Function");
#endif
}

SgExpression *CreateKernelDummyListAcross(ArgsForKernel *argsKer, SgType *idxTypeInKernel) //SgSymbol *s_red_count_k,
{
#if debugMode
        mywarn("strat: CreateKernelDummyListAcross Function");
#endif

    SgExpression *arg_list, *ae;
    arg_list = NULL;

    arg_list = AddListToList(CreateArrayDummyList(idxTypeInKernel), CreateRedDummyList());
    //   base_ref + <array_coeffs> ...
    // + <red_var[_1]> [+red_var_2+...+red_var_M] + <red>_grid  [ + <loc_var_1>...<loc_var_N>] 

    // + 'blocks'
    if (argsKer->symb.size() < 3)
    {
        for (list<SgSymbol*>::iterator it = argsKer->sizeVars.begin(); it != argsKer->sizeVars.end(); ++it)
        {
            ae = new SgExprListExp(*new SgVarRefExp(*it));
            arg_list = AddListToList(arg_list, ae);
        }
    }

    for (list<SgSymbol*>::iterator it = argsKer->acrossS.begin(); it != argsKer->acrossS.end(); ++it)
    {
        ae = new SgExprListExp(*new SgVarRefExp(*it));
        arg_list = AddListToList(arg_list, ae);
    }
    for (list<SgSymbol*>::iterator it = argsKer->notAcrS.begin(); it != argsKer->notAcrS.end(); ++it)
    {
        ae = new SgExprListExp(*new SgVarRefExp(*it));
        arg_list = AddListToList(arg_list, ae);
    }
    
    for (list<SgSymbol*>::iterator it = argsKer->idxAcross.begin(); it != argsKer->idxAcross.end(); ++it)
    {
        ae = new SgExprListExp(*new SgVarRefExp(*it));
        arg_list = AddListToList(arg_list, ae);
    }
    
    for (list<SgSymbol*>::iterator it = argsKer->idxNotAcross.begin(); it != argsKer->idxNotAcross.end(); ++it)
    {
        ae = new SgExprListExp(*new SgVarRefExp(*it));
        arg_list = AddListToList(arg_list, ae);
    }
    //ae = options.isOn(C_CUDA) ? new SgExprListExp(*new SgArrayRefExp(*s_blocks_k,*eln)) : new SgExprListExp(*new SgArrayRefExp(*s_blocks_k));   // + 'blocks'
    //              //ae = options.isOn(C_CUDA) ? new SgExprListExp(*new SgPointerDerefExp(*new SgVarRefExp(s_blocks_k))) : new SgExprListExp(*new SgVarRefExp(s_blocks_k));      
    //arg_list = AddListToList(arg_list,ae);
    //if(s_red_count_k)                                           //[+ 'red_count']
    //{  ae = new SgExprListExp(*new SgVarRefExp(s_red_count_k));
    //   arg_list = AddListToList(arg_list,ae);
    //}                                                         
    //                                                            //[+ 'overall_blocks']
    //if(s_overall_blocks)
    //{  ae = new SgExprListExp(*new SgVarRefExp(s_overall_blocks));
    //   arg_list = AddListToList(arg_list,ae);
    //}   
    if (uses_list)
        arg_list = AddListToList(arg_list, CreateUsesDummyList()); //[+ <uses> ]

    if (argsKer->symb.size() >= 3)
    {
        for (list<SgSymbol*>::iterator it = argsKer->sizeVars.begin(); it != argsKer->sizeVars.end(); ++it)
        {
            ae = new SgExprListExp(*new SgVarRefExp(*it));
            arg_list = AddListToList(arg_list, ae);
        }
    }

    if (argsKer->acrossS.size() != 1)
    {
        for (size_t i = 0; i < argsKer->otherVars.size(); ++i)
        {
            ae = new SgExprListExp(*new SgVarRefExp(argsKer->otherVars[i]));
            arg_list = AddListToList(arg_list, ae);
        }
    }
    else if (argsKer->otherVars.size() != 0)
    {
        ae = new SgExprListExp(*new SgVarRefExp(argsKer->otherVars[argsKer->otherVars.size() - 1]));
        arg_list = AddListToList(arg_list, ae);
    }

    for (size_t i = 0; i < argsKer->baseIdxsInKer.size(); ++i)
    {
        ae = new SgExprListExp(*new SgVarRefExp(argsKer->baseIdxsInKer[i]));
        arg_list = AddListToList(arg_list, ae);
    }

    if (argsKer->cond_ != NULL && options.isOn(GPU_O0))
    {
        SgSymbol *tmp = argsKer->cond_;
        arg_list = AddListToList(arg_list, new SgExprListExp(*new SgVarRefExp(tmp)));

        for (size_t i = 0; i < argsKer->steps.size(); ++i)
        {
            SgSymbol *tmp = argsKer->steps[i];
            arg_list = AddListToList(arg_list, new SgExprListExp(*new SgVarRefExp(tmp)));
        }
    }

#if debugMode
        mywarn("  end: CreateKernelDummyListAcross Function");
#endif

    return arg_list;
}

SgStatement *CreateLoopKernelAcross(SgSymbol *skernel, ArgsForKernel *argsKer, SgType *idxTypeInKernel)
{
#if debugMode
        mywarn("strat: CreateLoopKernelAcross");
#endif

    ACROSS_MOD_IN_KERNEL = 1;

#if kerneloff
        return NULL;
#endif

    int nloop = 0;
    SgStatement *st = NULL, *st_end = NULL;
    SgExpression *fe = NULL;
    SgSymbol *tid = NULL, *s_red_count_k = NULL;
    SgIfStmt *if_st = NULL;
    SgType *longType = idxTypeInKernel;

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
    cur_in_kernel = st = kernelScope = kernel_st;

    // !!creating variables and making structures for reductions
    CompleteStructuresForReductionInKernel();

    if (red_list)
        s_red_count_k = RedCountSymbol(kernel_st);

    // create  dummy argument list of kernel:
    if (options.isOn(C_CUDA))
        fe->setLhs(CreateKernelDummyListAcross(argsKer, longType)); //s_red_count_k, 
    else
        // create dummy argument list and add it to kernel header statement (Fortran-Cuda)
        kernel_st->setExpression(0, *CreateKernelDummyListAcross(argsKer, longType)); //s_red_count_k, 

    // generating block of index variables calculation 

#if debugMode
        mywarn("start: block4");
#endif

    if (options.isOn(C_CUDA))
        tid = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_x"), *longType, *cur_in_kernel);
    else
        tid = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_x"), *longType, *cur_in_kernel);

    if (options.isOn(C_CUDA))
        st = AssignStatement(*new SgVarRefExp(*tid), (*new SgRecordRefExp(*s_blockidx, "x")) *
        *new SgRecordRefExp(*s_blockdim, "x") + *new SgRecordRefExp(*s_threadidx, "x"));
    else
        st = AssignStatement(*new SgVarRefExp(*tid), (*new SgRecordRefExp(*s_blockidx, "x") - *new SgValueExp(1)) *
        *new SgRecordRefExp(*s_blockdim, "x") + *new SgRecordRefExp(*s_threadidx, "x") - *new SgValueExp(1));

    cur_in_kernel->insertStmtAfter(*st, *kernel_st);
    cur_in_kernel = st;

    size_t size = argsKer->otherVarsForOneTh.size();
    size_t size1 = argsKer->otherVars.size();
    SgForStmt *for_st = NULL, *inner_for_st = NULL;
    SgFunctionCallExp *funcAbs = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
    funcAbs->addArg(*new SgVarRefExp(argsKer->otherVars[size1 - 1]));
    SgExpression *sign = &(*new SgVarRefExp(argsKer->otherVars[size1 - 1]) / *funcAbs);

    if (options.isOn(C_CUDA))
        for_st = new SgForStmt(&SgAssignOp(*new SgVarRefExp(argsKer->otherVarsForOneTh[size - 1]), *new SgVarRefExp(argsKer->otherVars[size1 - 3])), &(*sign * *new SgVarRefExp(argsKer->otherVarsForOneTh[size - 1]) <= *sign * *new SgVarRefExp(argsKer->otherVars[size1 - 2])), &SgAssignOp(*new SgVarRefExp(argsKer->otherVarsForOneTh[size - 1]), *new SgVarRefExp(argsKer->otherVarsForOneTh[size - 1]) + *new SgVarRefExp(argsKer->otherVars[size1 - 1])), NULL);
    else
        for_st = new SgForStmt(argsKer->otherVarsForOneTh[size - 1], new SgVarRefExp(argsKer->otherVars[size1 - 3]), new SgVarRefExp(argsKer->otherVars[size1 - 2]), new SgVarRefExp(argsKer->otherVars[size1 - 1]), NULL);		
    inner_for_st = for_st;

    for (int i = size - 2; i >= 0; i--)
    {
        SgForStmt *tmp = for_st;
        funcAbs = new SgFunctionCallExp(*createNewFunctionSymbol("abs"));
        funcAbs->addArg(*new SgVarRefExp(argsKer->otherVars[3 * i + 2]));
        sign = &(*new SgVarRefExp(argsKer->otherVars[3 * i + 2]) / *funcAbs);

        if (options.isOn(C_CUDA))
            for_st = new SgForStmt(&SgAssignOp(*new SgVarRefExp(argsKer->otherVarsForOneTh[i]), *new SgVarRefExp(argsKer->otherVars[3 * i])), &(*sign * *new SgVarRefExp(argsKer->otherVarsForOneTh[i]) <= *sign * *new SgVarRefExp(argsKer->otherVars[3 * i + 1])), &(SgAssignOp(*new SgVarRefExp(argsKer->otherVarsForOneTh[i]), *new SgVarRefExp(argsKer->otherVarsForOneTh[i]) + *new SgVarRefExp(argsKer->otherVars[3 * i + 2]))), NULL);
        else
            for_st = new SgForStmt(argsKer->otherVarsForOneTh[i], new SgVarRefExp(argsKer->otherVars[3 * i]), new SgVarRefExp(argsKer->otherVars[3 * i + 1]), new SgVarRefExp(argsKer->otherVars[3 * i + 2]), NULL);
        for_st->insertStmtAfter(*tmp);
    }

    if_st = new SgIfStmt(SgEqOp(*new SgVarRefExp(*tid), *new SgValueExp(0)), *for_st);
    cur_in_kernel->insertStmtAfter(*if_st, *kernel_st);    

#if debugMode
        mywarn("  end: block4");
        mywarn("start: block5");
#endif

    // generating assign statements for MAXLOC, MINLOC reduction operations
    if (red_list)
        Do_Assign_For_Loc_Arrays();

    // inserting loop body to innermost IF statement of BlockForCalculationThreadLoopVariables

#if debugMode
        mywarn("  end: block5");
        mywarn("strat: inserting loop body");
#endif

    vector<SgSymbol*> forDeclarationInKernel;


    {
        SgStatement *stk, *last;
        block = CreateIfForRedBlack(loop_body, nloop);

        last = inner_for_st->lastNodeOfStmt();
        inner_for_st->insertStmtAfter(*block); //cur_in_kernel is innermost IF statement

        if (options.isOn(C_CUDA))
        {
            if (block->comments() == NULL)
                block->addComment("// Loop body");
        }
        else
            block->addComment("! Loop body\n");

        // correct copy of loop_body (change or extract last statement of block if it is CONTROL_END)
        if (block != loop_body)
            stk = last->lexPrev()->lexPrev();
        else
            stk = last->lexPrev();

        if (stk->variant() == CONTROL_END)
        {
            if (stk->hasLabel())
                stk->setVariant(CONT_STAT);
            else
                stk->extractStmt();
        }

        for_kernel = 1;
        last = cur_st;

        TranslateBlock(inner_for_st);
        if (options.isOn(C_CUDA))
        {
            //get info of arrays in private and locvar lists
            swapDimentionsInprivateList();
            vector < stack < SgStatement*> > zero = vector < stack < SgStatement*> >(0);
            Translate_Fortran_To_C(inner_for_st, inner_for_st->lastNodeOfStmt(), zero, 0);
        }
                
        cur_st = last;
        createBodyKernel = false;
    }
    
#if debugMode
        mywarn("  end: inserting loop body");
        mywarn("start: create reduction block");
#endif

    if (red_list)
    {
        int num;
        reduction_operation_list *tmp_list = red_struct_list;
        for (SgExpression *er = red_list; er; er = er->rhs())
        {
            num = 0;
            SgExpression *red_expr_ref = er->lhs()->rhs(); // reduction variable reference
            num = RedFuncNumber(er->lhs()->lhs()); // type of reduction

            SgSymbol *redGrid = new SgSymbol(VARIABLE_NAME, tmp_list->red_grid->identifier());
            redGrid->setType(*new SgArrayType(*tmp_list->red_grid->type()));

            st = AssignStatement(*new SgArrayRefExp(*redGrid, *new SgValueExp(0)), *red_expr_ref);
            if_st->lastExecutable()->insertStmtAfter(*st);
            tmp_list = tmp_list->next;
        }
    }
#if debugMode
        mywarn("  end: create reduction block");
#endif

    // make declarations
    if (options.isOn(C_CUDA))
        MakeDeclarationsForKernel_On_C_Across(idxTypeInKernel);
    else // Fortran-Cuda
        MakeDeclarationsForKernelAcross(idxTypeInKernel);
    for_kernel = 0;

    st = tid->makeVarDeclStmt();
    kernel_st->insertStmtAfter(*st);

    if (!options.isOn(C_CUDA))
    {
        for (size_t i = 0; i < argsKer->otherVars.size(); ++i)
        {
            st = argsKer->otherVars[i]->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }
    }
#if debugMode
        mywarn("  end: CreateLoopKernelAcross");
#endif

    ACROSS_MOD_IN_KERNEL = 0;
    return kernel_st;
}

SgStatement *CreateLoopKernelAcross(SgSymbol *skernel, ArgsForKernel *argsKer, int acrossNum, SgType *idxTypeInKernel)
{
#if debugMode
        mywarn("strat: CreateLoopKernelAcross");
#endif

    ACROSS_MOD_IN_KERNEL = 1;

#if kerneloff
        return NULL;
#endif

    int nloop;
    SgStatement *st = NULL, *st_end = NULL;
    SgExpression *e = NULL, *fe = NULL;
    SgSymbol *tid = NULL, *tid1 = NULL, *tid2 = NULL, *s_red_count_k = NULL, *coords = NULL;
    SgIfStmt *if_st = NULL, *if_st1 = NULL, *if_st2 = NULL;
    SgForStmt *mainFor = NULL;
    SgSymbol *tmpvar1 = NULL;    
    SgExpression **leftExprs, **rightExprs;
    SgType *longType = idxTypeInKernel;
        
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

    if (!options.isOn(C_CUDA) && createConvert_XY && options.isOn(AUTO_TFM))
    {
        kernel_st->addComment("!------------- dvmh_convert_XY() function ------------\n");
        kernel_st->addComment(funcDvmhConvXYfortVerLong);
        kernel_st->addComment(funcDvmhConvXYfortVer);

        createConvert_XY = false;
    }
    kernel_st->addComment(LoopKernelComment());

    st_end = kernel_st->lexNext();
    cur_in_kernel = st = kernelScope = kernel_st;

    // !!creating variables and making structures for reductions	
    CompleteStructuresForReductionInKernelAcross();

    if (red_list)
        s_red_count_k = RedCountSymbol(kernel_st);

    // create  dummy argument list of kernel:
    if (options.isOn(C_CUDA))
        fe->setLhs(CreateKernelDummyListAcross(argsKer, idxTypeInKernel)); // s_red_count_k,
    else
        // create dummy argument list and add it to kernel header statement (Fortran-Cuda)
        kernel_st->setExpression(0, *CreateKernelDummyListAcross(argsKer, idxTypeInKernel)); // s_red_count_k,

    // generating block of index variables calculation 

#if debugMode
        mywarn("start: block4");
#endif

    SgArrayType *tpArr = new SgArrayType(*longType);
    SgValueExp *dimSize = new SgValueExp((int)(argsKer->symb.size() + argsKer->nSymb.size()));
    tpArr->addDimension(dimSize);
    
    coords = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("coords"), *longType, *cur_in_kernel);
    coords->setType(tpArr);

    tid = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_x"), *longType, *cur_in_kernel);
    if (argsKer->symb.size() < 3)
    {
        if (argsKer->nSymb.size() == 1)
            tid1 = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_y"), *longType, *cur_in_kernel);
        else if (argsKer->nSymb.size() >= 2)
        {
            tid1 = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_y"), *longType, *cur_in_kernel);
            tid2 = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_z"), *longType, *cur_in_kernel);
        }
    }
    else if (argsKer->symb.size() >= 3)
    {
        tid1 = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_y"), *longType, *cur_in_kernel);
        if (argsKer->nSymb.size() > 0)
            tid2 = new SgSymbol(VARIABLE_NAME, TestAndCorrectName("id_z"), *longType, *cur_in_kernel);
    }
    if (options.isOn(C_CUDA))
        st = AssignStatement(*new SgVarRefExp(*tid), (*new SgRecordRefExp(*s_blockidx, "x")) *
        *new SgRecordRefExp(*s_blockdim, "x") + *new SgRecordRefExp(*s_threadidx, "x"));
    else
        st = AssignStatement(*new SgVarRefExp(*tid), (*new SgRecordRefExp(*s_blockidx, "x") - *new SgValueExp(1)) *
        *new SgRecordRefExp(*s_blockdim, "x") + *new SgRecordRefExp(*s_threadidx, "x") - *new SgValueExp(1));

    cur_in_kernel->insertStmtAfter(*st, *kernel_st);
    cur_in_kernel = st;

    if (argsKer->symb.size() == 1)
    {
        if (argsKer->nSymb.size() == 2)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y")) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;
        }
        else if (argsKer->nSymb.size() >= 3)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y")) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;

            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid2), (*new SgRecordRefExp(*s_blockidx, "z")) *
                *new SgRecordRefExp(*s_blockdim, "z") + *new SgRecordRefExp(*s_threadidx, "z"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid2), (*new SgRecordRefExp(*s_blockidx, "z") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "z") + *new SgRecordRefExp(*s_threadidx, "z") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;
        }
    }
    else if (argsKer->symb.size() == 2)
    {
        if (argsKer->nSymb.size() == 1)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y")) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;
        }
        else if (argsKer->nSymb.size() >= 2)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y")) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;

            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid2), (*new SgRecordRefExp(*s_blockidx, "z")) *
                *new SgRecordRefExp(*s_blockdim, "z") + *new SgRecordRefExp(*s_threadidx, "z"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid2), (*new SgRecordRefExp(*s_blockidx, "z") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "z") + *new SgRecordRefExp(*s_threadidx, "z") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;
        }
    }
    else if (argsKer->symb.size() >= 3)
    {
        if (options.isOn(C_CUDA))
            st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y")) *
            *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y"));
        else
            st = AssignStatement(*new SgVarRefExp(*tid1), (*new SgRecordRefExp(*s_blockidx, "y") - *new SgValueExp(1)) *
            *new SgRecordRefExp(*s_blockdim, "y") + *new SgRecordRefExp(*s_threadidx, "y") - *new SgValueExp(1));
        cur_in_kernel->insertStmtAfter(*st, *kernel_st);
        cur_in_kernel = st;

        if (argsKer->nSymb.size() > 0)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(*new SgVarRefExp(*tid2), (*new SgRecordRefExp(*s_blockidx, "z")) *
                *new SgRecordRefExp(*s_blockdim, "z") + *new SgRecordRefExp(*s_threadidx, "z"));
            else
                st = AssignStatement(*new SgVarRefExp(*tid2), (*new SgRecordRefExp(*s_blockidx, "z") - *new SgValueExp(1)) *
                *new SgRecordRefExp(*s_blockdim, "z") + *new SgRecordRefExp(*s_threadidx, "z") - *new SgValueExp(1));
            cur_in_kernel->insertStmtAfter(*st, *kernel_st);
            cur_in_kernel = st;
        }
    }

#if debugMode
        mywarn("  end: block4");
        mywarn("start: block5");
#endif

    if (argsKer->symb.size() == 1) // body for 1 dependence
    {
        int idx_exprs = 0;
        int count_of_dims = argsKer->nSymb.size() + argsKer->symb.size();

        list<SageSymbols*>::iterator itAcr = argsKer->symb.begin();
        list<SageSymbols*>::iterator it = argsKer->nSymb.begin();
        list<SgSymbol*>::iterator itAcrS = argsKer->acrossS.begin();
        list<SgSymbol*>::iterator itS = argsKer->notAcrS.begin();
        list<SgSymbol*>::iterator it_sizeV = argsKer->sizeVars.begin();
        list<SgSymbol*>::iterator itIdxAcr = argsKer->idxAcross.begin();
        list<SgSymbol*>::iterator itIdx = argsKer->idxNotAcross.begin();


        leftExprs = new SgExpression*[count_of_dims];
        rightExprs = new SgExpression*[count_of_dims];

        e = &(*new SgVarRefExp(*itAcrS));
        st = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *e);

        leftExprs[idx_exprs] = &(*new SgVarRefExp((*itAcr)->symb));
        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS));
        idx_exprs++;

        if (argsKer->nSymb.size() == 1)
        {
            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid) *
                *new SgVarRefExp(*itIdx));

            leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdx));
            idx_exprs++;
        }
        else if (argsKer->nSymb.size() == 2)
        {
            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid) *
                *new SgVarRefExp(*itIdx));

            leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdx));
            idx_exprs++;

            it++;
            itIdx++;
            itS++;

            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) *
                *new SgVarRefExp(*itIdx));

            leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*itIdx));
            idx_exprs++;

            it++;
            itIdx++;
            itS++;
        }
        else if (argsKer->nSymb.size() >= 3)
        {
            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid) *
                *new SgVarRefExp(*itIdx));

            leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdx));
            idx_exprs++;

            it++;
            itIdx++;
            itS++;

            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) *
                *new SgVarRefExp(*itIdx));

            leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*itIdx));
            idx_exprs++;

            it++;
            itIdx++;
            itS++;

            SgExpression *e_z1, *e_z2, *tmp_exp;
            it_sizeV = argsKer->sizeVars.begin();
            it_sizeV++;
            it_sizeV++;
            if (argsKer->nSymb.size() > 3)
            {
                SgFunctionCallExp *funCall = new SgFunctionCallExp(*createNewFunctionSymbol("mod"));
                e_z1 = new SgVarRefExp(*it_sizeV);
                funCall->addArg(*new SgVarRefExp(*tid2));
                funCall->addArg(*e_z1);
                tmp_exp = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));

                st = AssignStatement(*new SgVarRefExp((*it)->symb), *tmp_exp);

                leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
                rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                idx_exprs++;

                it++;
                itS++;
                itIdx++;
                it_sizeV++;
                e_z2 = new SgVarRefExp(*it_sizeV);
                it_sizeV++;
                for (unsigned i = 0; i < argsKer->nSymb.size() - 3; ++i, it++, itS++, itIdx++)
                {
                    SgFunctionCallExp *funCall = new SgFunctionCallExp(*createNewFunctionSymbol("mod"));
                    if (i == argsKer->nSymb.size() - 4)
                        tmp_exp = &(*new SgVarRefExp(*itS) + ((*new SgVarRefExp(*tid2) / *e_z1)) * *new SgVarRefExp(*itIdx));
                    else
                    {
                        funCall->addArg((*new SgVarRefExp(*tid2) / *e_z1));
                        funCall->addArg(*e_z2);
                        tmp_exp = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                    }

                    st = AssignStatement(*new SgVarRefExp((*it)->symb), *tmp_exp);

                    leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
                    rightExprs[idx_exprs] = &(tmp_exp->copy());
                    idx_exprs++;

                    e_z1 = &(*e_z1 * *e_z2);
                    if (i != argsKer->nSymb.size() - 4)
                    {
                        e_z2 = new SgVarRefExp(*it_sizeV);
                        it_sizeV++;
                    }
                }
            }
            else
            {
                st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid2) *
                    *new SgVarRefExp(*itIdx));

                leftExprs[idx_exprs] = &(*new SgVarRefExp((*it)->symb));
                rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid2) * *new SgVarRefExp(*itIdx));
                idx_exprs++;

                it++;
                itIdx++;
                itS++;
            }
        }

        if (options.isOn(C_CUDA))
            st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0])), &(rightExprs[0]->copy()));
        else
            st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0]) + *new SgValueExp(1)), &(rightExprs[0]->copy()));

        // main IF
        it_sizeV = argsKer->sizeVars.begin();
        if (argsKer->nSymb.size() == 0)
            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgValueExp(1), *st);
        else if (argsKer->nSymb.size() == 1)
            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp(*it_sizeV), *st);
        else if (argsKer->nSymb.size() == 2)
        {
            SgSymbol *tmp = *it_sizeV;
            it_sizeV++;
            SgSymbol *tmp1 = *it_sizeV;

            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp(tmp) &&
                *new SgVarRefExp(*tid1) < *new SgVarRefExp(tmp1), *st);
        }
        else if (argsKer->nSymb.size() >= 3)
        {
            SgSymbol *tmp = *it_sizeV;
            it_sizeV++;
            SgSymbol *tmp1 = *it_sizeV;
            it_sizeV++;

            SgExpression *if_mult = NULL;
            for (unsigned i = 0; i < argsKer->nSymb.size() - 2; ++i)
            {
                if (i == 0)
                    if_mult = new SgVarRefExp(*it_sizeV);
                else
                    if_mult = &((*if_mult) * *new SgVarRefExp(*it_sizeV));
                it_sizeV++;
            }
            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp(tmp) &&
                *new SgVarRefExp(*tid1) < *new SgVarRefExp(tmp1) && *new SgVarRefExp(*tid2) < *if_mult, *st);
        }

        for (size_t i = 1; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[i])), &(rightExprs[i]->copy()));
            else
                st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[i]) + *new SgValueExp(1)), &(rightExprs[i]->copy()));
            if_st->lastExecutable()->insertStmtAfter(*st);
        }

        for (size_t i = 0; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(&(leftExprs[i]->copy()), new SgArrayRefExp(*coords, *new SgValueExp((int)(i))));
            else
                st = AssignStatement(&(leftExprs[i]->copy()), new SgArrayRefExp(*coords, *new SgValueExp((int)(i + 1))));
            if_st->lastExecutable()->insertStmtAfter(*st);
        }

        if (options.isOn(GPU_O0))
        {
            SgSymbol *cond_s = argsKer->cond_;
            tmpvar1 = new SgSymbol(VARIABLE_NAME, "tmpV");
            SgExprListExp *listAss = new SgExprListExp();
            SgExprListExp *tmp = listAss;
            listAss->setLhs(&SgAssignOp(leftExprs[0]->copy(), (*(&leftExprs[0]->copy())) + *new SgVarRefExp(argsKer->steps[0])));
            for (size_t i = 1; i < argsKer->baseIdxsInKer.size(); ++i)
            {
                tmp->setRhs(new SgExprListExp());
                tmp = (SgExprListExp*)tmp->rhs();
                tmp->setLhs(&SgAssignOp(leftExprs[i]->copy(), (*(&leftExprs[i]->copy())) + *new SgVarRefExp(argsKer->steps[i])));
            }
            tmp->setRhs(new SgExprListExp());
            tmp = (SgExprListExp*)tmp->rhs();
            tmp->setLhs(&SgAssignOp(*new SgVarRefExp(tmpvar1), *new SgVarRefExp(tmpvar1) + *new SgValueExp(1)));

            if (options.isOn(C_CUDA))
                mainFor = new SgForStmt(&SgAssignOp(*new SgVarRefExp(tmpvar1), *new SgValueExp(1)), &(*new SgVarRefExp(tmpvar1) <= *new SgVarRefExp(*cond_s)), listAss, NULL);
            else
                mainFor = new SgForStmt(tmpvar1, &(rightExprs[0]->copy()), new SgVarRefExp(cond_s), new SgVarRefExp(*itIdxAcr), NULL);
            if_st->lastExecutable()->insertStmtAfter(*mainFor);
        }

        cur_in_kernel->insertStmtAfter(*if_st, *kernel_st);
        if (options.isOn(GPU_O0))
            cur_in_kernel = mainFor->lastExecutable();
        else
            cur_in_kernel = if_st->lastExecutable();

        if (!options.isOn(C_CUDA) && options.isOn(GPU_O0))
        {
            for (size_t i = 0; i < argsKer->baseIdxsInKer.size(); ++i)
                mainFor->lastExecutable()->insertStmtAfter(*AssignStatement(*&leftExprs[i]->copy(), (*(&leftExprs[i]->copy())) + *new SgVarRefExp(argsKer->steps[i])), *mainFor);			
        }

        delete[]leftExprs;
        delete[]rightExprs;
    }
    else if (argsKer->symb.size() == 2) // body for 2 dependence
    {
        // attantion!! adding to support all variants!!
        if (argsKer->nSymb.size() != 0)
        {
            SgSymbol *tmp = tid1;
            tid1 = tid;
            tid = tmp;
        }

        SgExpression **leftExprs, **rightExprs;
        int idx_exprs = 0;
        int count_of_dims = argsKer->nSymb.size() + argsKer->symb.size();
        leftExprs = new SgExpression*[count_of_dims];
        rightExprs = new SgExpression*[count_of_dims];

        list<SageSymbols*>::iterator itAcr = argsKer->symb.begin();
        list<SageSymbols*>::iterator it = argsKer->nSymb.begin();
        list<SgSymbol*>::iterator itAcrS = argsKer->acrossS.begin();
        list<SgSymbol*>::iterator itS = argsKer->notAcrS.begin();
        list<SgSymbol*>::iterator it_sizeV = argsKer->sizeVars.begin();
        list<SgSymbol*>::iterator itIdxAcr = argsKer->idxAcross.begin();
        list<SgSymbol*>::iterator itIdx = argsKer->idxNotAcross.begin();

        e = &(*new SgVarRefExp(*itAcrS) - *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdxAcr));
        st = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *e);
        leftExprs[idx_exprs] = new SgVarRefExp((*itAcr)->symb);
        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS) - *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdxAcr));
        idx_exprs++;

        itAcr++;
        itAcrS++;
        itIdxAcr++;

        e = &(*new SgVarRefExp(*itAcrS) + *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdxAcr));
        st = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *e);
        leftExprs[idx_exprs] = new SgVarRefExp((*itAcr)->symb);
        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS) + *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdxAcr));
        idx_exprs++;

        itAcr++;
        itAcrS++;
        itIdxAcr++;

        if (argsKer->nSymb.size() == 1)
        {
            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) *
                *new SgVarRefExp(*itIdx));
            leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*itIdx));
            idx_exprs++;
        }
        else if (argsKer->nSymb.size() >= 2)
        {
            st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) *
                *new SgVarRefExp(*itIdx));
            leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*itIdx));
            idx_exprs++;

            it++;
            itIdx++;
            itS++;

            SgExpression *e_z1, *e_z2, *tmp_exp;
            it_sizeV = argsKer->sizeVars.begin();
            it_sizeV++;
            it_sizeV++;
            if (argsKer->nSymb.size() > 2)
            {
                SgFunctionCallExp *funCall = new SgFunctionCallExp(*createNewFunctionSymbol("mod"));
                e_z1 = new SgVarRefExp(*it_sizeV);
                funCall->addArg(*new SgVarRefExp(*tid2));
                funCall->addArg(*e_z1);
                tmp_exp = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));

                st = AssignStatement(*new SgVarRefExp((*it)->symb), *tmp_exp);
                leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
                rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                idx_exprs++;

                it++;
                itS++;
                itIdx++;
                it_sizeV++;
                e_z2 = new SgVarRefExp(*it_sizeV);
                it_sizeV++;
                for (; it != argsKer->nSymb.end(); it++, itS++, itIdx++)
                {
                    SgFunctionCallExp *funCall = new SgFunctionCallExp(*createNewFunctionSymbol("mod"));
                    it++;
                    if (it == argsKer->nSymb.end())
                    {
                        tmp_exp = &(*new SgVarRefExp(*itS) + ((*new SgVarRefExp(*tid2) / *e_z1)) * *new SgVarRefExp(*itIdx));
                        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + ((*new SgVarRefExp(*tid2) / *e_z1)) * *new SgVarRefExp(*itIdx));
                    }
                    else
                    {
                        funCall->addArg((*new SgVarRefExp(*tid2) / *e_z1));
                        funCall->addArg(*e_z2);
                        tmp_exp = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                    }
                    it--;

                    st = AssignStatement(*new SgVarRefExp((*it)->symb), *tmp_exp);
                    leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
                    idx_exprs++;

                    e_z1 = &(*e_z1 * *e_z2);
                    it++;
                    if (it != argsKer->nSymb.end())
                    {
                        e_z2 = new SgVarRefExp(*it_sizeV);
                        it_sizeV++;
                    }
                    it--;
                }
            }
            else
            for (; it != argsKer->nSymb.end(); it++, itS++, itIdx++)
            {
                st = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(*tid2) *
                    *new SgVarRefExp(*itIdx));
                leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
                rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(*tid2) * *new SgVarRefExp(*itIdx));
                idx_exprs++;
            }
        }

        if (options.isOn(C_CUDA))
            st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0])), &(rightExprs[0]->copy()));
        else
            st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0]) + *new SgValueExp(1)), &(rightExprs[0]->copy()));
        // main IF
        it_sizeV = argsKer->sizeVars.begin();
        if (argsKer->nSymb.size() == 0)
            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp(*it_sizeV), *st);
        else if (argsKer->nSymb.size() == 1)
        {
            SgSymbol *tmp = *it_sizeV;
            it_sizeV++;
            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp(tmp) &&
                *new SgVarRefExp(*tid1) < *new SgVarRefExp(*it_sizeV), *st);
        }
        else if (argsKer->nSymb.size() >= 2)
        {
            SgExpression *tmp_exp;
            SgSymbol *tmp = *it_sizeV;
            it_sizeV++;
            SgSymbol *tmp1 = *it_sizeV;
            it_sizeV++;
            tmp_exp = new SgVarRefExp(*it_sizeV);
            it_sizeV++;
            for (; it_sizeV != argsKer->sizeVars.end(); it_sizeV++)
                tmp_exp = &((*tmp_exp) * *new SgVarRefExp(*it_sizeV));
            if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp(tmp) &&
                *new SgVarRefExp(*tid1) < *new SgVarRefExp(tmp1) &&
                *new SgVarRefExp(*tid2) < *tmp_exp, *st);
        }

        for (size_t i = 1; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[i])), &(rightExprs[i]->copy()));
            else
                st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[i]) + *new SgValueExp(1)), &(rightExprs[i]->copy()));
            if_st->lastExecutable()->insertStmtAfter(*st);
        }

        for (size_t i = 0; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(&(leftExprs[i]->copy()), new SgArrayRefExp(*coords, *new SgValueExp((int)(i))));
            else
                st = AssignStatement(&(leftExprs[i]->copy()), new SgArrayRefExp(*coords, *new SgValueExp((int)(i + 1))));
            if_st->lastExecutable()->insertStmtAfter(*st);
        }

        cur_in_kernel->insertStmtAfter(*if_st, *kernel_st);
        cur_in_kernel = if_st->lastExecutable();
        delete[]leftExprs;
        delete[]rightExprs;
    }
    else if (argsKer->symb.size() >= 3) // body for >3 dependence
    {
        // attantion!! adding to support all variants!! не проверено

        if (argsKer->nSymb.size() >= 1)
        {
            SgSymbol *tmp = tid2;
            tid2 = tid;
            tid = tmp;
        }

        SgStatement *st, *st1;
        SgSymbol *max_z, *se, *emax, *emin, *v1, *v2, *v3, *min_ij, *swap_ij, *i, *j;
        SgSymbol **num_elems;
        SgIfStmt *if_st3;
        list<SageSymbols*>::iterator itAcr = argsKer->symb.begin();
        list<SageSymbols*>::iterator it = argsKer->nSymb.begin();
        list<SgSymbol*>::iterator itAcrS = argsKer->acrossS.begin();
        list<SgSymbol*>::iterator itS = argsKer->notAcrS.begin();
        list<SgSymbol*>::iterator it_sizeV = argsKer->sizeVars.begin();
        list<SgSymbol*>::iterator itIdxAcr = argsKer->idxAcross.begin();
        list<SgSymbol*>::iterator itIdx = argsKer->idxNotAcross.begin();

        SgExpression **leftExprs, **rightExprs;
        int idx_exprs = 0;
        int count_of_dims = argsKer->nSymb.size() + argsKer->symb.size();
        leftExprs = new SgExpression*[count_of_dims];
        rightExprs = new SgExpression*[count_of_dims];

        num_elems = new SgSymbol*[argsKer->nSymb.size()];
        max_z = *it_sizeV;
        it_sizeV++;
        se = *it_sizeV;
        it_sizeV++;
        v1 = *it_sizeV;
        it_sizeV++;
        v2 = *it_sizeV;
        it_sizeV++;
        v3 = *it_sizeV;
        it_sizeV++;
        emax = *it_sizeV;
        it_sizeV++;
        emin = *it_sizeV;
        it_sizeV++;
        min_ij = *it_sizeV;
        it_sizeV++;
        swap_ij = *it_sizeV;
        it_sizeV++;

        for (size_t i = 0; i < argsKer->nSymb.size(); ++i)
        {
            num_elems[i] = *it_sizeV;
            it_sizeV++;
        }

        e = &(*new SgVarRefExp(*itAcrS) - *new SgVarRefExp(*tid) * *new SgVarRefExp(*itIdxAcr));

        st = AssignStatement(*new SgVarRefExp(*itAcrS), *new SgVarRefExp(*itAcrS) - *new SgVarRefExp(*itIdxAcr) *
            (*new SgVarRefExp(*se) + *new SgVarRefExp(*tid1) - *new SgVarRefExp(*emin)));

        itAcrS++;
        itIdxAcr++;
        st1 = AssignStatement(*new SgVarRefExp(*itAcrS), *new SgVarRefExp(*itAcrS) + *new SgVarRefExp(*itIdxAcr) *
            (*new SgVarRefExp(*se) + *new SgVarRefExp(*tid1) - *new SgVarRefExp(*emin)));

        if_st2 = new SgIfStmt(SgEqOp(*new SgVarRefExp(*v3), *new SgValueExp(1)) && *new SgVarRefExp(emin) < *new SgVarRefExp(tid1) + *new SgVarRefExp(se), *st1);
        if_st2->insertStmtAfter(*st);

        SgFunctionCallExp *funcCall = new SgFunctionCallExp(*createNewFunctionSymbol("min"));
        funcCall->addArg(*new SgVarRefExp(*se) + *new SgVarRefExp(*tid1));

        itAcrS--;
        itIdxAcr--;

        if_st = new SgIfStmt(*new SgVarRefExp(*tid) < *new SgVarRefExp((*itAcr)->symb), *if_st2);
        if (argsKer->nSymb.size() == 0)
            if_st3 = new SgIfStmt(*new SgVarRefExp(*tid1) < *new SgVarRefExp(*max_z), *if_st);
        else
        {
            SgExpression *tmp = new SgVarRefExp(num_elems[0]);
            for (size_t i = 1; i < argsKer->nSymb.size(); ++i)
            {
                tmp = &(*tmp * *new SgVarRefExp(num_elems[i]));
            }
            if_st3 = new SgIfStmt(*new SgVarRefExp(*tid1) < *new SgVarRefExp(*max_z) && *new SgVarRefExp(*tid2) < *tmp, *if_st);
        }
        cur_in_kernel->insertStmtAfter(*if_st3, *kernel_st);
        cur_in_kernel = if_st->lexNext();

        st1 = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *new SgVarRefExp(*min_ij));

        st = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *new SgValueExp(2) * *new SgVarRefExp(*min_ij) - *new SgVarRefExp(se) -
            *new SgVarRefExp(tid1) + *new SgVarRefExp(emax) - *new SgVarRefExp(emin) - *new SgValueExp(1));

        if_st1 = new SgIfStmt(*new SgVarRefExp(*tid1) + *new SgVarRefExp(se) < *new SgVarRefExp(*emax), *st1, *st);

        st1 = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *new SgVarRefExp(*tid1) + *new SgVarRefExp(se));

        if_st1 = new SgIfStmt(*new SgVarRefExp(*tid1) + *new SgVarRefExp(se) < *new SgVarRefExp(*emin), *st1, *if_st1);
        if_st3->insertStmtAfter(*if_st1);

        i = (*itAcr)->symb;
        st1 = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *new SgVarRefExp(*itAcrS) + ((*new SgVarRefExp(tid1) *
            (*new SgVarRefExp(v1) + *new SgVarRefExp(v3)) - *new SgVarRefExp(tid))) * *new SgVarRefExp(*itIdxAcr));

        leftExprs[idx_exprs] = new SgVarRefExp((*itAcr)->symb);
        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS) + ((*new SgVarRefExp(tid1) *
            (*new SgVarRefExp(v1) + *new SgVarRefExp(v3)) - *new SgVarRefExp(tid))) * *new SgVarRefExp(*itIdxAcr));
        idx_exprs++;


        itAcrS++;
        itIdxAcr++;
        itAcr++;

        j = (*itAcr)->symb;
        st1 = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *new SgVarRefExp(*itAcrS) + (*new SgVarRefExp(tid1) *
            *new SgVarRefExp(v2) + *new SgVarRefExp(tid)) * *new SgVarRefExp(*itIdxAcr));

        leftExprs[idx_exprs] = new SgVarRefExp((*itAcr)->symb);
        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS) + (*new SgVarRefExp(tid1) *
            *new SgVarRefExp(v2) + *new SgVarRefExp(tid)) * *new SgVarRefExp(*itIdxAcr));
        idx_exprs++;

        itAcrS++;
        itIdxAcr++;
        itAcr++;

        st1 = AssignStatement(*new SgVarRefExp((*itAcr)->symb), *new SgVarRefExp(*itAcrS) - *new SgVarRefExp(tid1) *
            *new SgVarRefExp(*itIdxAcr));

        leftExprs[idx_exprs] = new SgVarRefExp((*itAcr)->symb);
        rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS) - *new SgVarRefExp(tid1) * *new SgVarRefExp(*itIdxAcr));
        idx_exprs++;

        if (argsKer->symb.size() > 3)
        {
            for (size_t i = 0; i < argsKer->symb.size() - 3; ++i)
            {
                itAcrS++;
                itIdxAcr++;
                itAcr++;

                leftExprs[idx_exprs] = new SgVarRefExp((*itAcr)->symb);
                rightExprs[idx_exprs] = &(*new SgVarRefExp(*itAcrS));
                idx_exprs++;
            }
        }

        if (argsKer->nSymb.size() == 1)
        {
            st1 = AssignStatement(*new SgVarRefExp((*it)->symb), *new SgVarRefExp(*itS) + *new SgVarRefExp(tid2) *
                *new SgVarRefExp(*itIdx));

            leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *new SgVarRefExp(tid2) * *new SgVarRefExp(*itIdx));
            idx_exprs++;
        }
        else if (argsKer->nSymb.size() > 1)
        {
            SgExpression *e_z1, *e_z2, *tmp_exp;
            SgFunctionCallExp *funCall = new SgFunctionCallExp(*createNewFunctionSymbol("mod"));
            e_z1 = new SgVarRefExp(num_elems[0]);
            funCall->addArg(*new SgVarRefExp(*tid2));
            funCall->addArg(*e_z1);
            tmp_exp = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));

            st = AssignStatement(*new SgVarRefExp((*it)->symb), *tmp_exp);

            leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
            rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
            idx_exprs++;

            it++;
            itS++;
            itIdx++;
            e_z2 = new SgVarRefExp(num_elems[1]);
            for (int count = 2; it != argsKer->nSymb.end(); it++, itS++, itIdx++, ++count)
            {
                SgFunctionCallExp *funCall = new SgFunctionCallExp(*createNewFunctionSymbol("mod"));
                it++;
                if (it == argsKer->nSymb.end())
                {
                    tmp_exp = &(*new SgVarRefExp(*itS) + ((*new SgVarRefExp(*tid2) / *e_z1)) * *new SgVarRefExp(*itIdx));
                    rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + ((*new SgVarRefExp(*tid2) / *e_z1)) * *new SgVarRefExp(*itIdx));
                }
                else
                {
                    funCall->addArg((*new SgVarRefExp(*tid2) / *e_z1));
                    funCall->addArg(*e_z2);
                    tmp_exp = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                    rightExprs[idx_exprs] = &(*new SgVarRefExp(*itS) + *funCall * *new SgVarRefExp(*itIdx));
                }
                it--;

                st = AssignStatement(*new SgVarRefExp((*it)->symb), *tmp_exp);

                leftExprs[idx_exprs] = new SgVarRefExp((*it)->symb);
                idx_exprs++;

                e_z1 = &(*e_z1 * *e_z2);
                it++;
                if (it != argsKer->nSymb.end())
                {
                    e_z2 = new SgVarRefExp(num_elems[count]);
                }
                it--;
            }
        }

        if (options.isOn(C_CUDA))
            st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0])), &(rightExprs[0]->copy()));
        else
            st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0]) + *new SgValueExp(1)), &(rightExprs[0]->copy()));
        // insert into MAIN If
        if_st->lastExecutable()->insertStmtAfter(*st);

        for (size_t i = 1; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[i])), &(rightExprs[i]->copy()));
            else
                st = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[i]) + *new SgValueExp(1)), &(rightExprs[i]->copy()));
            if_st->lastExecutable()->insertStmtAfter(*st);
        }

        //insert swap block
        if (options.isOn(C_CUDA))
        {
            SgExpression *firstElem = new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0]));
            SgExpression *secondElem = new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[1]));

            if_st2 = new SgIfStmt(*new SgVarRefExp(swap_ij) * *new SgVarRefExp(v3), *new SgCExpStmt(*firstElem ^= *secondElem ^= *firstElem ^= *secondElem));
        }
        else
        {
            st1 = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0]) + *new SgValueExp(1)), new SgVarRefExp(v3));
            if_st2 = new SgIfStmt(*new SgVarRefExp(swap_ij) * *new SgVarRefExp(v3), *st1);

            st1 = AssignStatement(new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[1]) + *new SgValueExp(1)), 
                                  new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[0]) + *new SgValueExp(1)));
            if_st2->insertStmtAfter(*st1);

            st1 = AssignStatement(new SgVarRefExp(v3), new SgArrayRefExp(*coords, *new SgVarRefExp(argsKer->baseIdxsInKer[1]) + *new SgValueExp(1)));
            if_st2->insertStmtAfter(*st1);
        }
        if_st->lastExecutable()->insertStmtAfter(*if_st2);

        for (size_t i = 0; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (options.isOn(C_CUDA))
                st = AssignStatement(&(leftExprs[i]->copy()), new SgArrayRefExp(*coords, *new SgValueExp((int)(i))));
            else
                st = AssignStatement(&(leftExprs[i]->copy()), new SgArrayRefExp(*coords, *new SgValueExp((int)(i + 1))));
            if_st->lastExecutable()->insertStmtAfter(*st);
        }
        delete[]leftExprs;
        delete[]rightExprs;

        cur_in_kernel = if_st->lastExecutable();
    }

    // generating assign statements for MAXLOC, MINLOC reduction operations
    if (red_list)
        Do_Assign_For_Loc_Arrays();

    // inserting loop body to innermost IF statement of BlockForCalculationThreadLoopVariables

#if debugMode
        mywarn("  end: block5");
        mywarn("strat: inserting loop body");
#endif

    SgStatement *currStForInsetGetXY = cur_in_kernel;
    vector<SgSymbol*> forDeclarationInKernel;
    set<char *> uniqueNames;
    
    // create, insert, optimize and convert loop_body into kernel
    {
        SgStatement *stk, *last;
        vector<newInfo> allNewInfo;
        
        if (argsKer->symb.size() == 1)
        {
            if (options.isOn(GPU_O0))
                optimizeLoopBodyForOne(allNewInfo);
            oneCase = true;
        }
        else
            oneCase = false;


        block = CreateIfForRedBlack(loop_body, nloop);
        last = cur_in_kernel->lexNext();		

        if (argsKer->symb.size() == 1 && allNewInfo.size() != 0 && options.isOn(GPU_O0)) //insert needed assigns
        {
            list<SgSymbol*>::iterator itIdxAcr = argsKer->idxAcross.begin();
            SgIfStmt *ifSt = new SgIfStmt(*new SgVarRefExp(*itIdxAcr) > *new SgValueExp(0), *&allNewInfo[0].loadsBeforePlus[0]->copy(), *&allNewInfo[0].loadsBeforeMinus[0]->copy());
            for (size_t i = 0; i < allNewInfo.size(); ++i)
            {
                if (i == 0)
                {
                    for (size_t k = 1; k < allNewInfo[i].loadsBeforePlus.size(); ++k)
                    {
                        ifSt->insertStmtAfter(*&allNewInfo[i].loadsBeforePlus[k]->copy(), *ifSt);
                        ifSt->falseBody()->insertStmtBefore(*&allNewInfo[i].loadsBeforeMinus[k]->copy(), *ifSt);
                    }
                }
                else
                {
                    for (size_t k = 0; k < allNewInfo[i].loadsBeforePlus.size(); ++k)
                    {
                        ifSt->insertStmtAfter(*&allNewInfo[i].loadsBeforePlus[k]->copy(), *ifSt);
                        ifSt->falseBody()->insertStmtBefore(*&allNewInfo[i].loadsBeforeMinus[k]->copy(), *ifSt);
                    }
                }				
            }
            mainFor->insertStmtBefore(*ifSt);
        }
                
        if (argsKer->symb.size() == 1 && options.isOn(GPU_O0))
            cur_in_kernel->insertStmtAfter(*block, *mainFor); //cur_in_kernel is innermost FOR stmt
        else
            cur_in_kernel->insertStmtAfter(*block, *if_st); //cur_in_kernel is innermost IF statement

        if (options.isOn(C_CUDA))
        {            
            if (block->comments() == NULL)
                block->addComment("// Loop body");
        }
        else
            block->addComment("! Loop body\n");

        // correct copy of loop_body (change or extract last statement of block if it is CONTROL_END)
        if (block != loop_body)
            stk = last->lexPrev()->lexPrev();
        else
            stk = last->lexPrev();

        if (stk->variant() == CONTROL_END)
        {
            if (stk->hasLabel())
                stk->setVariant(CONT_STAT);
            else
                stk->extractStmt();
        }

        for_kernel = 1;
        last = cur_st;

        if (argsKer->symb.size() == 1 && allNewInfo.size() != 0 && options.isOn(GPU_O0)) //insert needed assigns
        {
            list<SgSymbol*>::iterator itIdxAcr = argsKer->idxAcross.begin();
            SgIfStmt *ifSt = new SgIfStmt(*new SgVarRefExp(*itIdxAcr) > *new SgValueExp(0), *&allNewInfo[0].loadsInForPlus[0]->copy(), *&allNewInfo[0].loadsInForMinus[0]->copy());

            for (size_t i = 0; i < allNewInfo.size(); ++i)
            {
                size_t k;
                if (i == 0)
                    k = 1;
                else
                    k = 0;
                for (; k < allNewInfo[i].loadsInForPlus.size(); ++k)
                {
                    ifSt->insertStmtAfter(*&allNewInfo[i].loadsInForPlus[k]->copy(), *ifSt);
                    ifSt->falseBody()->insertStmtBefore(*&allNewInfo[i].loadsInForMinus[k]->copy(), *ifSt);
                }
            }
            mainFor->insertStmtAfter(*ifSt);


            for (size_t i = 0; i < allNewInfo.size(); ++i)
            {
                if (options.isOn(C_CUDA))
                {
                    for (size_t k = 0; k < allNewInfo[i].stores.size(); ++k)
                        mainFor->lastExecutable()->insertStmtAfter(*&allNewInfo[i].stores[k]->copy());
                }
                else
                {
                    for (size_t k = 0; k < allNewInfo[i].stores.size(); ++k)
                        mainFor->lastExecutable()->lexPrev()->lexPrev()->insertStmtBefore(*&allNewInfo[i].stores[k]->copy());						
                }
            }			

            size_t k = allNewInfo[0].swapsUp.size() - 1;
            ifSt = new SgIfStmt(*new SgVarRefExp(*itIdxAcr) > *new SgValueExp(0), *&allNewInfo[0].swapsDown[k]->copy(), *&allNewInfo[0].swapsUp[k]->copy());
            for (size_t i = 0; i < allNewInfo.size(); ++i)
            {
                size_t last;
                if (i == 0)
                    last = allNewInfo[i].swapsUp.size() - 1;
                else
                    last = allNewInfo[0].swapsUp.size();
                for (size_t k = 0; k < last; ++k)
                {
                    ifSt->insertStmtAfter(*&allNewInfo[i].swapsDown[last - 1 - k]->copy(), *ifSt);
                    ifSt->falseBody()->insertStmtBefore(*&allNewInfo[i].swapsUp[last - 1 - k]->copy(), *ifSt);
                }
            }
            mainFor->lastExecutable()->insertStmtAfter(*ifSt);
        }

        // insert dvmh_convert_XY calls directly into loop_body if some array accesses depend on its definitions (inserting right before accesses)
        if (options.isOn(AUTO_TFM))
        {
            if (acrossNum != 1)
            {
                map<SgSymbol*, Array*>& arrays = currentLoop->getArrays();
                string funcDvmhConvXYname_type = funcDvmhConvXYname;
                if (!options.isOn(C_CUDA))
                {
                    if (strcmp(idxTypeInKernel->symbol()->identifier(), indexTypeInKernel(rt_INT)->symbol()->identifier()) == 0)
                        funcDvmhConvXYname_type += "_int";
                    else if (strcmp(idxTypeInKernel->symbol()->identifier(), indexTypeInKernel(rt_LONG)->symbol()->identifier()) == 0)
                        funcDvmhConvXYname_type += "_long";
                    else if (strcmp(idxTypeInKernel->symbol()->identifier(), indexTypeInKernel(rt_LLONG)->symbol()->identifier()) == 0)
                        funcDvmhConvXYname_type += "_llong";
                }
                for (map<SgSymbol*, Array*>::iterator it = arrays.begin(); it != arrays.end(); ++it)
                {
                    Array* array = it->second;
                    set<SgSymbol*>& privateList = currentLoop->getPrivateList();
                    if (privateList.find(it->first) == privateList.end())
                    {
                        for (map<string, Access*>::iterator it2 = array->getAccesses().begin(); it2 != array->getAccesses().end(); ++it2)
                            analyzeArrayIndxs(array->getSymbol(), it2->second->getSubscripts());
                        int numSymb = 0;
                        for (size_t i1 = 0; i1 < argsKer->arrayNames.size(); ++i1)
                            if (strcmp(argsKer->arrayNames[i1], array->getSymbol()->identifier()) == 0)
                            {
                                numSymb = (int)i1;
                                break;
                            }
                        array->generateAssigns(
                            new SgVarRefExp(argsKer->otherVars[8 * numSymb + 1]),
                            new SgVarRefExp(argsKer->otherVars[8 * numSymb + 4]),
                            new SgVarRefExp(argsKer->otherVars[8 * numSymb + 2]),
                            new SgVarRefExp(argsKer->otherVars[8 * numSymb + 5]),
                            new SgVarRefExp(argsKer->otherVars[8 * numSymb + 6]));
                        SgIfStmt* ifSt = NULL, *if1case = NULL, *if2case = NULL;
                        TfmInfo& tfmInfo = array->getTfmInfo();
                        map<SgStatement*, vector<SgFunctionCallExp*> >& ifCalls = tfmInfo.ifCalls;
                        map<SgStatement*, vector<SgFunctionCallExp*> >& elseCalls = tfmInfo.elseCalls;
                        SgSymbol* x_axis = argsKer->otherVars[8 * numSymb];
                        SgSymbol* y_axis = argsKer->otherVars[8 * numSymb + 3];
                        int tfsDim1 = tfmInfo.transformDims[0];
                        int tfsDim2 = tfmInfo.transformDims[1];
                        for (map<SgStatement*, vector<SgFunctionCallExp*> >::iterator it = ifCalls.begin(); it != ifCalls.end(); ++it)
                        {
                            if (it->first == NULL)
                                continue;
                            if (ifCalls[it->first].size() > 0)
                            {
                                if (options.isOn(C_CUDA))
                                {
                                    if2case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim2)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim1))), *new SgCExpStmt(*(elseCalls[it->first][0])));
                                    if1case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim1)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim2))), *new SgCExpStmt(*(ifCalls[it->first][0])), *if2case);
                                    ifSt = new SgIfStmt(SgEqOp(*new SgVarRefExp(argsKer->otherVars[8 * numSymb + 7]), *new SgValueExp(2)), *if1case);
                                }
                                else
                                {
                                    if2case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim2)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim1))),
                                        *new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(elseCalls[it->first][0]->args())));
                                    if1case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim1)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim2))),
                                        *new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(ifCalls[it->first][0]->args())), *if2case);
                                    ifSt = new SgIfStmt(SgEqOp(*new SgVarRefExp(argsKer->otherVars[8 * numSymb + 7]), *new SgValueExp(2)), *if1case);
                                }
                            }

                            for (size_t k = 1; k < ifCalls[it->first].size(); ++k)
                            {
                                if (options.isOn(C_CUDA))
                                {
                                    if1case->insertStmtAfter(*new SgCExpStmt(*(ifCalls[it->first][k])));
                                    if2case->insertStmtAfter(*new SgCExpStmt(*(elseCalls[it->first][k])));
                                }
                                else
                                {
                                    if1case->insertStmtAfter(*new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(ifCalls[it->first][k]->args())));
                                    if2case->insertStmtAfter(*new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(elseCalls[it->first][k]->args())));
                                }
                            }

                            if (ifSt != NULL)
                            {
                                if (loop_body == it->first)
                                    loop_body->insertStmtBefore(*ifSt);
                                else
                                {
                                    for (SgStatement* stmt = loop_body; stmt != NULL; stmt = stmt->lexNext())
                                    {
                                        if (stmt->lexNext() == it->first)
                                        {
                                            stmt->insertStmtAfter(*ifSt);
                                            break;
                                        }
                                    }
                                }
                            }
                            ifSt = NULL;
                        }
                    }
                }
            }
        }

        TranslateBlock(if_st);

        if (options.isOn(C_CUDA))
        {
            //get info of arrays in private and locvar lists
            swapDimentionsInprivateList();
            if (argsKer->symb.size() == 1 && options.isOn(GPU_O0))
            {
                Translate_Fortran_To_C(mainFor->lexPrev()->controlParent());
                Translate_Fortran_To_C(mainFor, mainFor->lastNodeOfStmt(), copyOfBody, 0); //countOfCopies
            }
            else
                Translate_Fortran_To_C(if_st, if_st->lastNodeOfStmt(), copyOfBody, 0); // countOfCopies
        }
                
        cur_st = last;
        if (createBodyKernel == false)
            createBodyKernel = true;

    }

    //insert dvmh_convert_XY before loop_body if its arguments depend only on loop indices
    if (options.isOn(AUTO_TFM))
    {
#if debugMode
        mywarn("strat: inserting transform calls");
#endif
        if (acrossNum != 1)
        {
            map<SgSymbol*, Array*>& arrays = currentLoop->getArrays();
            string funcDvmhConvXYname_type = funcDvmhConvXYname;
            if (!options.isOn(C_CUDA))
            {
                if (strcmp(idxTypeInKernel->symbol()->identifier(), indexTypeInKernel(rt_INT)->symbol()->identifier()) == 0)
                    funcDvmhConvXYname_type += "_int";
                else if (strcmp(idxTypeInKernel->symbol()->identifier(), indexTypeInKernel(rt_LONG)->symbol()->identifier()) == 0)
                    funcDvmhConvXYname_type += "_long";
                else if (strcmp(idxTypeInKernel->symbol()->identifier(), indexTypeInKernel(rt_LLONG)->symbol()->identifier()) == 0)
                    funcDvmhConvXYname_type += "_llong";
            }
            for (map<SgSymbol*, Array*>::iterator it = arrays.begin(); it != arrays.end(); ++it)
            {
                Array *array = it->second;
                set<SgSymbol*>& privateList = currentLoop->getPrivateList();
                if (privateList.find(it->first) == privateList.end())
                {
                    int numSymb = 0;
                    for (size_t i1 = 0; i1 < argsKer->arrayNames.size(); ++i1)
                    if (strcmp(argsKer->arrayNames[i1], array->getSymbol()->identifier()) == 0)
                    {
                        numSymb = (int)i1;
                        break;
                    }
                    SgIfStmt* ifSt = NULL, *if1case = NULL, *if2case = NULL;
                    TfmInfo& tfmInfo = array->getTfmInfo();
                    vector<SgFunctionCallExp*>& ifCalls = tfmInfo.ifCalls[NULL];
                    vector<SgFunctionCallExp*>& elseCalls = tfmInfo.elseCalls[NULL];
                    SgSymbol* x_axis = argsKer->otherVars[8 * numSymb];
                    SgSymbol* y_axis = argsKer->otherVars[8 * numSymb + 3];
                    int tfsDim1 = tfmInfo.transformDims[0];
                    int tfsDim2 = tfmInfo.transformDims[1];

                    if (ifCalls.size() > 0)
                    if (options.isOn(C_CUDA))
                    {
                        if2case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim2)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim1))), *new SgCExpStmt(*(elseCalls[0])));
                        if1case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim1)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim2))), *new SgCExpStmt(*(ifCalls[0])), *if2case);
                        ifSt = new SgIfStmt(SgEqOp(*new SgVarRefExp(argsKer->otherVars[8 * numSymb + 7]), *new SgValueExp(2)), *if1case);
                    }
                    else
                    {
                        if2case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim2)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim1))),
                            *new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(elseCalls[0]->args())));
                        if1case = new SgIfStmt((SgEqOp(*new SgVarRefExp(x_axis->copy()), *new SgValueExp(tfsDim1)) && SgEqOp(*new SgVarRefExp(y_axis->copy()), *new SgValueExp(tfsDim2))),
                            *new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(ifCalls[0]->args())), *if2case);
                        ifSt = new SgIfStmt(SgEqOp(*new SgVarRefExp(argsKer->otherVars[8 * numSymb + 7]), *new SgValueExp(2)), *if1case);
                    }
                    for (size_t k = 1; k < ifCalls.size(); ++k)
                    {
                        if (options.isOn(C_CUDA))
                        {
                            if1case->insertStmtAfter(*new SgCExpStmt(*(ifCalls[k])));
                            if2case->insertStmtAfter(*new SgCExpStmt(*(elseCalls[k])));
                        }
                        else
                        {
                            if1case->insertStmtAfter(*new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(ifCalls[k]->args())));
                            if2case->insertStmtAfter(*new SgCallStmt(*createNewFunctionSymbol(funcDvmhConvXYname_type.c_str()), *(elseCalls[k]->args())));
                        }
                    }
                    if (ifSt != NULL)
                        currStForInsetGetXY->insertStmtAfter(*ifSt);

                    vector<SgStatement*>& zeroSt = tfmInfo.zeroSt;
                    for (size_t k = 0; k < zeroSt.size(); ++k)
                        currStForInsetGetXY->insertStmtAfter(zeroSt[k]->copy());

                    vector<SgSymbol*>& coef = tfmInfo.coefficients;
                    for (unsigned z = 0; z < coef.size(); ++z)
                        forDeclarationInKernel.push_back(&(coef[z]->copy()));
                }
            }
        }

#if debugMode
        mywarn("end: inserting transform calls");
#endif
    }

#if debugMode
        mywarn("  end: inserting loop body");
        mywarn("start: create reduction block");
#endif

    if (red_list && argsKer->nSymb.size() == 0)
    {
        int num;
        reduction_operation_list *tmp_list = red_struct_list;
        int needComment = 1;
        for (SgExpression *er = red_list; er; er = er->rhs())
        {
            num = 0;
            int flag_func_call = 1;
            SgExpression *red_expr_ref = er->lhs()->rhs(); // reduction variable reference
            SgExpression *loc_var_ref = NULL, *en = NULL;
            int loc_el_num = 0;
            if (isSgExprListExp(red_expr_ref))
            {
                red_expr_ref = red_expr_ref->lhs(); // reduction variable reference         
                loc_var_ref = er->lhs()->rhs()->rhs()->lhs(); //location array reference
                en = er->lhs()->rhs()->rhs()->rhs()->lhs(); // number of elements in location array
                loc_el_num = LocElemNumber(en);
            }
            num = RedFuncNumber(er->lhs()->lhs()); // type of reduction
            const char *str_operation = NULL;
            if (num == 1)
                flag_func_call = 0; // +
            else if (num == 2)
                flag_func_call = 0; // *
            else if (num == 3)
                str_operation = "max";
            else if (num == 4)
                str_operation = "min";
            else if (num == 5)
                flag_func_call = 0; // and
            else if (num == 6)
                flag_func_call = 0; // or
            else if (num == 7)
                flag_func_call = 0; // !=
            else if (num == 8)
                flag_func_call = 0; // ==
            else if (num == 9)
                flag_func_call = 0; // maxloc
            else if (num == 10)
                flag_func_call = 0; // minloc
            if (flag_func_call == 1)
            {
                SgFunctionCallExp *funcCall = new SgFunctionCallExp(*createNewFunctionSymbol(str_operation));
                if (argsKer->symb.size() < 3)
                {
                    SgSymbol *redGrid = new SgSymbol(VARIABLE_NAME, tmp_list->red_grid->identifier());
                    redGrid->setType(*new SgArrayType(*tmp_list->red_grid->type()));

                    funcCall->addArg(*new SgArrayRefExp(*redGrid, *new SgVarRefExp(*tid)));
                    funcCall->addArg(*red_expr_ref);
                    st = AssignStatement(*new SgArrayRefExp(*redGrid, *new SgVarRefExp(*tid)), *funcCall);
                }
                else
                {
                    SgSymbol *redGrid = new SgSymbol(VARIABLE_NAME, tmp_list->red_grid->identifier());
                    redGrid->setType(*new SgArrayType(*tmp_list->red_grid->type()));

                    list<SgSymbol*>::iterator it_sizeV = argsKer->sizeVars.begin();
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    SgSymbol *emin = *it_sizeV;
                    funcCall->addArg(*new SgArrayRefExp(*redGrid, *new SgVarRefExp(*tid) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*emin)));
                    funcCall->addArg(*red_expr_ref);
                    st = AssignStatement(*new SgArrayRefExp(*redGrid, *new SgVarRefExp(*tid) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*emin)), *funcCall);
                }
            }
            else
            {
                SgExpression *e1 = NULL;
                if (argsKer->symb.size() < 3)
                {
                    e1 = new SgVarRefExp(*tid);
                }
                else
                {
                    list<SgSymbol*>::iterator it_sizeV = argsKer->sizeVars.begin();
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    it_sizeV++;
                    SgSymbol *emin = *it_sizeV;
                    e1 = &(*new SgVarRefExp(*tid) + *new SgVarRefExp(*tid1) * *new SgVarRefExp(*emin));
                }
                e = NULL;
                SgIfStmt *ifSt = NULL;
                SgSymbol *redGrid = new SgSymbol(VARIABLE_NAME, tmp_list->red_grid->identifier());
                redGrid->setType(*new SgArrayType(*tmp_list->red_grid->type()));
                if (num == 1)
                    e = &(*new SgArrayRefExp(*redGrid, *e1) + *red_expr_ref);
                else if (num == 2)
                    e = &(*new SgArrayRefExp(*redGrid, *e1) * *red_expr_ref);
                else if (num == 5)
                    e = &(*new SgArrayRefExp(*redGrid, *e1) && *red_expr_ref);
                else if (num == 6)
                    e = &(*new SgArrayRefExp(*redGrid, *e1) || *red_expr_ref);
                else if (num == 7)
                    e = &SgNeqOp(*new SgArrayRefExp(*redGrid, *e1), *red_expr_ref);
                else if (num == 8)
                    e = &SgEqOp(*new SgArrayRefExp(*redGrid, *e1), *red_expr_ref);
                else if (num == 9 || num == 10)
                {
                    st = AssignStatement(*new SgArrayRefExp(*redGrid, *e1), *red_expr_ref);
                    ifSt = new SgIfStmt(*red_expr_ref > *new SgArrayRefExp(*redGrid, *e1), *st);
                    for (int i = loc_el_num - 1; i >= 0; i--)
                    {
                        SgSymbol *locGrid = new SgSymbol(VARIABLE_NAME, tmp_list->loc_grid->identifier());
                        redGrid->setType(*new SgArrayType(*tmp_list->loc_grid->type()));

                        if (options.isOn(C_CUDA))
                            st = AssignStatement(*new SgArrayRefExp(*locGrid, *new SgValueExp(i), *e1), *new SgArrayRefExp(*loc_var_ref->symbol(), *new SgValueExp(i)));
                        else
                            st = AssignStatement(*new SgArrayRefExp(*locGrid, *new SgValueExp(i + 1), *e1), *new SgArrayRefExp(*loc_var_ref->symbol(), *new SgValueExp(i + 1)));
                        ifSt->insertStmtAfter(*st);
                    }
                }

                if (num != 9 && num != 10)
                    st = AssignStatement(*new SgArrayRefExp(*redGrid, *e1), *e);
                else
                    st = ifSt;
            }
            if (argsKer->symb.size() < 3)
                if_st->lastExecutable()->insertStmtAfter(*st, *if_st);
            else
                if_st->lastExecutable()->insertStmtAfter(*st);
            tmp_list = tmp_list->next;
            if (needComment == 1)
            {
                if (options.isOn(C_CUDA))
                    st->addComment("// Reduction");
                else
                    st->addComment("! Reduction\n");
                needComment = 0;
            }
        }
        
        DeclarationCreateReductionBlocksAcross(nloop, red_list);
    }
    else if (red_list && argsKer->nSymb.size() > 0) // generating reduction calculation blocks
    {
        st = AssignStatement(*new SgVarRefExp(tid), *new SgRecordRefExp(*s_blockdim, "x") * *new SgRecordRefExp(*s_blockdim, "y") *
            *new SgRecordRefExp(*s_blockdim, "z") / *new SgValueExp(2));
        st_end->insertStmtBefore(*st, *kernel_st);
        SgSymbol *copy_symb = new SgSymbol(*tid);
        CreateReductionBlocksAcross(st_end, nloop, red_list, copy_symb); //s_red_count_k
    }

#if debugMode
        mywarn(" end: create reduction block");
#endif

    // make declarations
    if (options.isOn(C_CUDA))
        MakeDeclarationsForKernel_On_C_Across(idxTypeInKernel);
    else // Fortran-Cuda
        MakeDeclarationsForKernelAcross(idxTypeInKernel);
    for_kernel = 0;

    st = coords->makeVarDeclStmt();
    kernel_st->insertStmtAfter(*st);

    st = tid->makeVarDeclStmt();
    kernel_st->insertStmtAfter(*st);

    if (tmpvar1 != NULL)
        addDeclExpList(tmpvar1, st->expr(0));

    if (options.isOn(AUTO_TFM))
    {
        for (size_t i = 0; i < forDeclarationInKernel.size(); ++i)
            addDeclExpList(forDeclarationInKernel[i], st->expr(0));
    }
    if (argsKer->symb.size() == 1)
    {
        if (argsKer->nSymb.size() == 2)
            addDeclExpList(tid1, st->expr(0));
        else if (argsKer->nSymb.size() >= 3)
        {
            addDeclExpList(tid1, st->expr(0));
            addDeclExpList(tid2, st->expr(0));
        }
    }
    else if (argsKer->symb.size() == 2)
    {
        if (argsKer->nSymb.size() == 1)
            addDeclExpList(tid1, st->expr(0));
        else if (argsKer->nSymb.size() >= 2)
        {
            addDeclExpList(tid1, st->expr(0));
            addDeclExpList(tid2, st->expr(0));
        }
    }
    else if (argsKer->symb.size() >= 3)
    {
        addDeclExpList(tid1, st->expr(0));
        if (argsKer->nSymb.size() > 0)
            addDeclExpList(tid2, st->expr(0));
    }

    if (!options.isOn(C_CUDA))
    {
        for (list<SgSymbol*>::iterator it1 = argsKer->sizeVars.begin(); it1 != argsKer->sizeVars.end(); ++it1)
        {
            st = (*it1)->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }

        for (list<SgSymbol*>::iterator it = argsKer->acrossS.begin(); it != argsKer->acrossS.end(); ++it)
        {
            st = (*it)->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }

        for (list<SgSymbol*>::iterator it = argsKer->notAcrS.begin(); it != argsKer->notAcrS.end(); ++it)
        {
            st = (*it)->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }
        
        for (list<SgSymbol*>::iterator it = argsKer->idxAcross.begin(); it != argsKer->idxAcross.end(); ++it)
        {
            st = (*it)->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }
        
        for (list<SgSymbol*>::iterator it = argsKer->idxNotAcross.begin(); it != argsKer->idxNotAcross.end(); ++it)
        {
            st = (*it)->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }

        for (size_t i = 0; i < argsKer->otherVars.size() / 8 * 8; i += 8)
        {
            st = argsKer->otherVars[i]->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
            addDeclExpList(argsKer->otherVars[i + 3], st->expr(0));

            st = argsKer->otherVars[i + 1]->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
            addDeclExpList(argsKer->otherVars[i + 4], st->expr(0));

            st = argsKer->otherVars[i + 2]->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
            addDeclExpList(argsKer->otherVars[i + 5], st->expr(0));

            st = argsKer->otherVars[i + 6]->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
            addDeclExpList(argsKer->otherVars[i + 7], st->expr(0));
        }

        if (argsKer->otherVars.size() != 0 && argsKer->otherVars.size() % 8 != 0)
        {
            st = argsKer->otherVars[argsKer->otherVars.size() - 1]->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
        }

        for (size_t i = 0; i < argsKer->baseIdxsInKer.size(); ++i)
        {
            if (i == 0)
            {
                st = argsKer->baseIdxsInKer[i]->makeVarDeclStmt();
                st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
                kernel_st->insertStmtAfter(*st);
            }
            else
                addDeclExpList(argsKer->baseIdxsInKer[i], st->expr(0));
        }

        if (argsKer->cond_ != NULL)
        {
            st = argsKer->cond_->makeVarDeclStmt();
            st->setExpression(2, *new SgExprListExp(*new SgExpression(ACC_VALUE_OP)));
            kernel_st->insertStmtAfter(*st);
            for (size_t i = 0; i < argsKer->steps.size(); ++i)
                addDeclExpList(argsKer->steps[i], st->expr(0));
        }
    }
#if debugMode
        mywarn("  end: CreateLoopKernelAcross");
#endif

    // inserting IMPLICIT NONE
    if (!options.isOn(C_CUDA)) // Fortran-Cuda
        kernel_st->insertStmtAfter(*new SgStatement(IMPL_DECL), *kernel_st);

    ACROSS_MOD_IN_KERNEL = 0;
    return(kernel_st);
}


// -------------------------- Reduction block for Across ---------------------------- //

void CompleteStructuresForReductionInKernelAcross()
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

        if (!s_overall_blocks && rl->redvar_size != 0)
            s_overall_blocks = OverallBlocksSymbol();
        if (rl->redvar_size < 0)
        {
            rl->dimSize_arg = CreateFormalDimSizeList(rl->redvar);
            rl->red_init = RedInitValSymbolInKernel(rl->redvar, rl->dimSize_arg); // after CreateFormalDimSizeList()
        }
        else
        {
            rl->dimSize_arg = NULL;
            rl->red_init = NULL;
        }
        rl->red_grid = RedGridSymbolInKernel(rl->redvar, rl->redvar_size, rl->dimSize_arg, 1); // after CreateFormalDimSizeList()
        rl->loc_grid = rl->locvar ? RedGridSymbolInKernel(rl->locvar, rl->number, NULL, 0) : NULL;

        r = Rank(rl->redvar);
        max_rank = max_rank < r ? r : max_rank;
    }

    kernel_index_var_list = CreateIndexVarList(max_rank);
}

SgSymbol *RedBlockSymbolInKernelAcross(SgSymbol *s, SgType *type)
{
    char *name = NULL;
    SgSymbol *sb = NULL;
    SgValueExp M0(0);
    SgExpression  *MD = new SgExpression(DDOT, &M0.copy(), new SgKeywordValExp("*"), NULL);
    SgArrayType *typearray;
    int i = 1;

    if (!type)
        typearray = new SgArrayType(*s->type()->baseType());
    else if (isSgArrayType(s->type()))
        typearray = (SgArrayType *)&(s->type()->copy());
    else
        typearray = new SgArrayType(*type);

    if (!options.isOn(C_CUDA))
        typearray->addRange(*MD);
    else
        typearray->addDimension(NULL);

    name = new char[strlen(s->identifier()) + 8];
    sprintf(name, "%s_block", s->identifier());

    while (isSameNameShared(name))
        sprintf(name, "%s_block%d", s->identifier(), i++);

    sb = new SgVariableSymb(name, *typearray, *kernel_st); // scope may be mod_gpu
#if 0
    shared_list = AddToSymbList(shared_list, sb);
#endif
    delete[]name;

    return sb;
}

void DeclarationOfReductionBlockInKernelAcross(SgExpression *ered, reduction_operation_list *rsl)
{
    SgStatement *ass, *newst, *current, *if_st, *while_st, *typedecl, *st, *do_st;
    SgExpression *le, *re, *eatr, *cond, *ev;
    SgSymbol *red_var, *red_var_k, *s_block, *loc_var, *sf;
    SgType *rtype;
    int i, ind;

    //init block
    ass = newst = current = if_st = while_st = typedecl = st = do_st = NULL;
    le = re = eatr = cond = ev = NULL;
    red_var = red_var_k = s_block = loc_var = sf = NULL;
    rtype = NULL;
    i = ind = loc_el_num = 0;
    //end of init block

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
        newst = Declaration_Statement(rsl->locvar); //declare location variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }

    if (rsl->redvar_size > 0)
    {
        newst = Declaration_Statement(rsl->redvar); //declare reduction variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }
    else if (rsl->redvar_size < 0)
    {
        red_var_k = RedVariableSymbolInKernel(rsl->redvar, rsl->dimSize_arg);
        newst = Declaration_Statement(red_var_k); //declare reduction variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }
    rtype = (rsl->redvar_size >= 0) ? TypeOfRedBlockSymbol(ered) : NULL;

    s_block = RedBlockSymbolInKernelAcross(red_var, rtype);

    newst = Declaration_Statement(s_block);

    if (options.isOn(C_CUDA))
        newst->addDeclSpec(BIT_CUDA_SHARED | BIT_EXTERN);
    else
    {
        eatr = new SgExprListExp(*new SgExpression(ACC_SHARED_OP));
        newst->setExpression(2, *eatr);
    }

    kernel_st->insertStmtAfter(*newst, *kernel_st);

    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
    {
        typedecl = MakeStructDecl(rtype->symbol());
        kernel_st->insertStmtAfter(*typedecl, *kernel_st);
    }
}

static SgExpression *BlockIdxRefExpr_(const char *xyz)
{
    if (options.isOn(C_CUDA))
        return(new SgRecordRefExp(*s_blockidx, xyz));
    else
        return(&(*new SgRecordRefExp(*s_blockidx, xyz) - *new SgValueExp(1)));
}

void ReductionBlockInKernelAcross(SgStatement *stat, SgSymbol *i_var, SgSymbol *j_var, SgExpression *ered, reduction_operation_list *rsl, SgSymbol *red_count_symb)
{
    SgStatement *ass, *newst, *current, *if_st, *while_st, *typedecl, *st, *do_st;
    SgExpression *le, *re, *eatr, *cond, *ev;
    SgSymbol *red_var, *red_var_k, *s_block, *loc_var, *sf;
    SgType *rtype;
    int i, ind;

    //init block
    ass = newst = current = if_st = while_st = typedecl = st = do_st = NULL;
    le = re = eatr = cond = ev = NULL;
    red_var = red_var_k = s_block = loc_var = sf = NULL;
    rtype = NULL;
    i = ind = loc_el_num = 0;
    //end of init block

    //call syncthreads() for second, third,... reduction operation (n>1) 
    /*if(n>1)
    {
    newst = FunctionCallStatement(SyncthreadsSymbol());
    stat->insertStmtBefore(*newst,*stat->controlParent());
    }*/
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
        newst = Declaration_Statement(rsl->locvar); //declare location variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }

    if (rsl->redvar_size > 0)
    {
        newst = Declaration_Statement(rsl->redvar); //declare reduction variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }
    else if (rsl->redvar_size < 0)
    {
        red_var_k = RedVariableSymbolInKernel(rsl->redvar, rsl->dimSize_arg);
        newst = Declaration_Statement(red_var_k); //declare reduction variable
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }
    rtype = (rsl->redvar_size >= 0) ? TypeOfRedBlockSymbol(ered) : NULL;

    s_block = RedBlockSymbolInKernelAcross(red_var, rtype);

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
    {
        for (i = 0; i < rsl->redvar_size; i++)
        {
            SgExpression *red_ind;
            red_ind = RedVarIndex(red_var, i);
            le = RedVar_Block_2D_Ref(s_block, i_var, red_ind);
            re = new SgArrayRefExp(*red_var, *red_ind);
            ass = AssignStatement(le, re);
            stat->insertStmtBefore(*ass, *stat->controlParent());
        }
    }
    else if (rsl->redvar_size == 0) //reduction variable is scalar      
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

    /*re = new SgRecordRefExp(*s_blockdim,"x");
    if(nloop>1)
    re = &(*re * (*new SgRecordRefExp(*s_blockdim,"y")));
    if(nloop>2)
    re = &(*re * (*new SgRecordRefExp(*s_blockdim,"z")));
    cond= & operator < ( ( *new SgVarRefExp(i_var) + *new SgVarRefExp(red_count_symb)) , *re );

    if(isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
    newst = RedOp_If(i_var,s_block,ered,red_count_symb,rsl->number);
    else
    newst = RedOp_Assign(i_var,s_block,ered,red_count_symb,0);
    if_st = new SgIfStmt( *cond, *newst);
    if(rsl->redvar_size > 0)
    for(i = 1; i < rsl->redvar_size; ++i)
    {
    newst->insertStmtAfter(*(ass=RedOp_Assign(i_var,s_block,ered,red_count_symb,i)), *if_st);
    newst = ass;
    }
    if(!rsl->next && rsl->redvar_size >=0 ) //last reduction of loop, not array of unknown size
    {
    cond= & operator < ( *new SgVarRefExp(i_var), *new SgVarRefExp(red_count_symb));
    newst = new SgIfStmt( *cond, *if_st);
    stat->insertStmtBefore(*newst,*stat->controlParent());
    }
    else
    stat->insertStmtBefore(*if_st,*stat->controlParent());
    */

    /*  old variant */
    //         j = red_count / 2
    //ass = AssignStatement(new SgVarRefExp(j_var),&(*new SgVarRefExp(red_count_symb) / *new SgValueExp(2)));
    /*  old variant */

    //       j = red_count 
    ass = AssignStatement(new SgVarRefExp(j_var), &(*new SgVarRefExp(red_count_symb)));
    //if(!rsl->next && rsl->redvar_size >= 0 ) //last reduction of loop, not array of unknown size
    //	if_st->insertStmtAfter(*ass,*newst);										
    //else
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
    //            <loc_var>_grid(1, blockIdx%x [ - 1 ] ) = <red_var>_block(0)%<loc_var>(1)
    //            <loc_var>_grid(2, blockIdx%x [ - 1 ] ) = <red_var>_block(0)%<loc_var>(2)
    //                  .    .    .
    //          
    //         endif

    int flag_func_call = 1;
    int num = RedFuncNumber(ered->lhs()); // type of reduction
    SgExpression *e = NULL;
    const char *str_operation = NULL;
    if (num == 1)
        flag_func_call = 0; // +
    else if (num == 2)
        flag_func_call = 0; // *
    else if (num == 3)
        str_operation = "max";
    else if (num == 4)
        str_operation = "min";
    else if (num == 5)
        flag_func_call = 0; // and
    else if (num == 6)
        flag_func_call = 0; // or
    else if (num == 7)
        flag_func_call = 0; // !=
    else if (num == 8)
        flag_func_call = 0; // ==
    else if (num == 9)
        flag_func_call = 0; // maxloc
    else if (num == 10)
        flag_func_call = 0; // minloc

    SgSymbol *redGrid = new SgSymbol(VARIABLE_NAME, rsl->red_grid->identifier());
    redGrid->setType(*new SgArrayType(*rsl->red_grid->type()));

    cond = &SgEqOp(*new SgVarRefExp(i_var), *new SgValueExp(0));
    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC	 
        newst = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr_("x") + (*BlockIdxRefExpr_("y") + *BlockIdxRefExpr_("z") * *new SgRecordRefExp(*s_griddim, "y")) * *new SgRecordRefExp(*s_griddim, "x")), RedLocVar_Block_Ref(s_block, NULL, NULL, new SgVarRefExp((sf))));
    else
    {
        if (rsl->redvar_size > 0)
            e = new SgArrayRefExp(*redGrid, *BlockIdxRefExpr_("x") + (*BlockIdxRefExpr_("y") + *BlockIdxRefExpr_("z") * *new SgRecordRefExp(*s_griddim, "y")) * *new SgRecordRefExp(*s_griddim, "x"), *new SgValueExp(1));
        else if (rsl->redvar_size == 0)
            e = new SgArrayRefExp(*redGrid, *BlockIdxRefExpr_("x") + (*BlockIdxRefExpr_("y") + *BlockIdxRefExpr_("z") * *new SgRecordRefExp(*s_griddim, "y")) * *new SgRecordRefExp(*s_griddim, "x"));
        else
            e = new SgArrayRefExp(*redGrid, *BlockIdxRefExpr_("x") + (*BlockIdxRefExpr_("y") + *BlockIdxRefExpr_("z") * *new SgRecordRefExp(*s_griddim, "y")) * *new SgRecordRefExp(*s_griddim, "x"), *SubscriptListOfRedArray(red_var));
        if (flag_func_call == 1)
        {
            SgFunctionCallExp *funcCall = new SgFunctionCallExp(*createNewFunctionSymbol(str_operation));
            funcCall->addArg(*e);
            funcCall->addArg(*new SgArrayRefExp(*s_block, *new SgValueExp(0)));
            newst = AssignStatement(e, funcCall);
        }
        else
        {
            SgExpression *e_1 = new SgArrayRefExp(*s_block, *new SgValueExp(0));
            if (num == 1)
                newst = AssignStatement(e, &(*e + *e_1));
            else if (num == 2)
                newst = AssignStatement(e, &(*e * *e_1));
            else if (num == 5)
                newst = AssignStatement(e, &(*e && *e_1));
            else if (num == 6)
                newst = AssignStatement(e, &(*e || *e_1));
            else if (num == 7)
                newst = AssignStatement(e, &SgNeqOp(*e, *e_1));
            else if (num == 8)
                newst = AssignStatement(e, &SgEqOp(*e, *e_1));
        }
    }

    if_st = new SgIfStmt(*cond, *newst);
    if (rsl->redvar_size > 0)
    for (i = 1; i < rsl->redvar_size; ++i)
    {
        ass = AssignStatement(new SgArrayRefExp(*rsl->red_grid, *BlockIdxRefExpr_("x") + (*BlockIdxRefExpr_("y") + *BlockIdxRefExpr_("z") * *new SgRecordRefExp(*s_griddim, "y")) * *new SgRecordRefExp(*s_griddim, "x"), *new SgValueExp(i + 1)), new SgArrayRefExp(*s_block, *RedVarIndex(red_var, i), *new SgValueExp(0)));
        newst->insertStmtAfter(*ass, *if_st);
        newst = ass;
    }
    current->insertStmtAfter(*if_st, *current->controlParent());

    if (isSgExprListExp(ered->rhs())) //MAXLOC,MINLOC
    {
        st = newst;
        for (i = 1; i <= rsl->number; ++i)
        {
            ind = options.isOn(C_CUDA) ? i - 1 : i;
            re = RedLocVar_Block_Ref(s_block, NULL, NULL, new SgArrayRefExp(*((SgFieldSymb *)sf)->nextField(), *new SgValueExp(ind)));
            le = new SgArrayRefExp(*rsl->loc_grid, *new SgValueExp(ind), *BlockIdxRefExpr_("x") + (*BlockIdxRefExpr_("y") + *BlockIdxRefExpr_("z") * *new SgRecordRefExp(*s_griddim, "y")) * *new SgRecordRefExp(*s_griddim, "x"));
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
    //!printf("  block 6\n");
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

    if_st = new SgIfStmt(*cond, *newst);
    if (rsl->redvar_size > 0)       // reduction variable is array
    {
        for (i = 1; i < rsl->redvar_size; ++i)
        {
            newst->insertStmtAfter(*(ass = RedOp_Assign(i_var, s_block, ered, j_var, i)), *if_st);
            newst = ass;
        }
    }
    current->insertStmtAfter(*if_st, *while_st);
}

void DeclarationCreateReductionBlocksAcross(int nloop, SgExpression *red_op_list)
{
    SgStatement *newst, *dost;
    SgExpression *er;
    SgSymbol *i_var, *j_var;
    reduction_operation_list *rsl;
    int n;

    formal_red_grid_list = NULL;

    // index variables
    dost = DoStmt(first_do_par, nloop);
    i_var = dost->symbol();
    if (nloop > 1)
        j_var = dost->controlParent()->symbol();
    else
    {
        j_var = IndVarInKernel(i_var);
        newst = j_var->makeVarDeclStmt();
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }

    //looking through the reduction_op_list
    for (er = red_op_list, rsl = red_struct_list, n = 1; er; er = er->rhs(), rsl = rsl->next, n++)
    {
        DeclarationOfReductionBlockInKernelAcross(er->lhs(), rsl);
    }
}

void CreateReductionBlocksAcross(SgStatement *stat, int nloop, SgExpression *red_op_list, SgSymbol *red_count_symb)
{
    SgStatement *newst, *ass, *dost;
    SgExpression *er, *re;
    SgSymbol *i_var, *j_var;
    reduction_operation_list *rsl;
    int n;

    formal_red_grid_list = NULL;

    // index variables
    dost = DoStmt(first_do_par, nloop);
    i_var = dost->symbol();
    if (nloop > 1)
        j_var = dost->controlParent()->symbol();
    else 
    {
        j_var = IndVarInKernel(i_var);
        newst = j_var->makeVarDeclStmt();
        kernel_st->insertStmtAfter(*newst, *kernel_st);
    }
    //create symbol 'syncthreads'
    // declare '<red_var>_block' array for each reduction var
    // <i_var> = threadIdx%x -1 + [ (threadIdx%y - 1) * blockDim%x [ + (threadIdx%z - 1) * blockDim%x * blockDim%y ] ]
    // or C_Cuda
    // <i_var> = threadIdx%x + [ threadIdx%y * blockDim%x [ + threadIdx%z * blockDim%x * blockDim%y ] ]     

    re = ThreadIdxRefExpr("x");
    if (nloop > 1)
        re = &(*re + (*ThreadIdxRefExpr("y")) * (*new SgRecordRefExp(*s_blockdim, "x")));
    if (nloop > 2)
        re = &(*re + (*ThreadIdxRefExpr("z")) * (*new SgRecordRefExp(*s_blockdim, "x") * (*new SgRecordRefExp(*s_blockdim, "y"))));
    ass = AssignStatement(new SgVarRefExp(i_var), re);
    stat->insertStmtBefore(*ass, *stat->controlParent());
    if (options.isOn(C_CUDA))
        ass->addComment("// Reduction");
    else
        ass->addComment("! Reduction\n");

    //looking through the reduction_op_list

    for (er = red_op_list, rsl = red_struct_list, n = 1; er; er = er->rhs(), rsl = rsl->next, n++)
    {
        ReductionBlockInKernelAcross(stat, i_var, j_var, er->lhs(), rsl, red_count_symb);
    }
}

//end of Reduction block for Across

#undef LongT
#undef debugMode
#undef kerneloff
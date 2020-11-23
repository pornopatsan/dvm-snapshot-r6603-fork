#include "dvm.h"
#include "acc_data.h"
#include "calls.h"

//TMP:
extern symb_list *acc_call_list, *by_value_list;

// create comments of call procedures from each kernel in file _info.c
// if -FTN_Cuda option selected
void ACC_RTC_AddCalledProcedureComment(SgSymbol *symbK)
{
    symb_list *sl;
    int len = 0;
    for (sl = acc_call_list; sl; sl = sl->next)
        len = len + strlen(sl->symb->identifier()) + 1;

    char *list_txt = new  char[len + 1];
    list_txt[0] = '\0';
    for (sl = acc_call_list; sl; sl = sl->next)
    {
        strcat(list_txt, " ");
        strcat(list_txt, sl->symb->identifier());
    }
    info_block->addComment(CalledProcedureComment(list_txt, symbK));

}

// complete rtc launch parameters from cuda-handlers
void ACC_RTC_CompleteAllParams()
{
    for (unsigned fc = 0; fc < RTC_FCall.size(); ++fc)
    {
        SgFunctionCallExp *fCall = RTC_FKernelArgs[fc];
        if (fCall->variant() == EXPR_LIST) // if Fortran CUDA
        {
            fCall = new SgFunctionCallExp(*createNewFunctionSymbol(""));
            SgExpression *tmp = RTC_FKernelArgs[fc];
            while (tmp)
            {
                fCall->addArg(*tmp->lhs());
                tmp = tmp->rhs();
            }
        }

        SgExpression *argList = RTC_FArgs[fc];
        for (int k = 0; k < fCall->numberOfArgs(); ++k)
        {
            SgExpression *currArg = fCall->arg(k);
            bool dontCast = false;

            if (currArg->variant() == DEREF_OP)
                currArg = currArg->lhs();

            if (currArg->symbol() == NULL)
            {
                RTC_FCall[fc]->addArg(*new SgValueExp("<!!! NULL !!!>"));
                argList = argList->rhs();
                continue;
            }
            std::string tmpN = currArg->symbol()->identifier();
            bool isarray = isSgArrayType(currArg->symbol()->type());
            bool ispointer = isSgPointerType(currArg->symbol()->type());
            bool notbyval = true;
            symb_list *sl;
            for (sl = by_value_list; sl; sl = sl->next)
            {
                if (strcmp(sl->symb->identifier(), currArg->symbol()->identifier()) == 0)
                {
                    notbyval = false;
                    break;
                }
            }

            bool isinuser = isInUsesListByChar(currArg->symbol()->identifier());
            if (isarray || ispointer || notbyval && isinuser)
            {
                RTC_FCall[fc]->addArg(*new SgValueExp(""));
                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_POINTER")));
                RTC_FCall[fc]->addArg(*argList->lhs());
            }
            else
            {
                SgType *tmp = currArg->symbol()->type();

                if (tmp->hasBaseType())
                    tmp->baseType();

                unsigned UnFlag = ((SgDescriptType*)tmp)->modifierFlag() & BIT_UNSIGNED;

                SgAttribute *attr = argList->lhs()->getAttribute(0);
                bool toAdd = false;
                if (attr != NULL)
                {
                    if (attr->getAttributeType() == RTC_NOT_REPLACE)
                        RTC_FCall[fc]->addArg(*new SgValueExp(""));
                    else
                        toAdd = true;
                }
                else
                    toAdd = true;

                if (toAdd)
                {
                    if (options.isOn(C_CUDA))
                        RTC_FCall[fc]->addArg(*new SgValueExp(currArg->symbol()->identifier()));
                    else
                    {
                        // PGI adds to scalars n__V_ !!
                        std::string tmp = "n__V_";
                        tmp += aks_strlowr(currArg->symbol()->identifier());
                        RTC_FCall[fc]->addArg(*new SgValueExp(tmp.c_str()));
                    }
                }

                if (tmp->equivalentToType(C_Type(SgTypeChar())) || tmp->equivalentToType(SgTypeChar()))
                {
                    if (UnFlag)
                        RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_UCHAR")));
                    else
                        RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_CHAR")));
                }
                else if (tmp->equivalentToType(C_Type(SgTypeInt())) || (tmp->equivalentToType(SgTypeInt())))
                {
                    if (isSgDescriptType(tmp))
                    {
                        SgDescriptType *t = (SgDescriptType*)tmp;
                        int flag = t->modifierFlag();
                        if ((flag & BIT_LONG) != 0)
                        {
                            if (UnFlag)
                                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_ULONG")));
                            else
                                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_LONG")));
                        }
                        else if ((flag & BIT_SHORT) != 0)
                        {
                            if (UnFlag)
                                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_USHORT")));
                            else
                                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_SHORT")));
                        }
                        else
                        {
                            if (UnFlag)
                                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_UINT")));
                            else
                                RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_INT")));
                        }
                    }
                    else
                    {
                        if (UnFlag)
                            RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_UINT")));
                        else
                            RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_INT")));
                    }
                }
                else if (tmp->equivalentToType(C_LongType()))
                {
                    if (UnFlag)
                        RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_ULONG")));
                    else
                        RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_LONG")));
                }
                else if (tmp->equivalentToType(C_LongLongType()))
                {
                    if (UnFlag)
                        RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_ULLONG")));
                    else
                        RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_LLONG")));
                }
                else if (tmp->equivalentToType(C_Type(SgTypeFloat())) || tmp->equivalentToType(SgTypeFloat()))
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_FLOAT")));
                else if (tmp->equivalentToType(C_Type(SgTypeDouble())) || tmp->equivalentToType(SgTypeDouble()))
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_DOUBLE")));
                else if (tmp->equivalentToType(indexTypeInKernel(rt_INT)))
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_INT")));
                else if (tmp->equivalentToType(indexTypeInKernel(rt_LONG)))
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_LONG")));
                else if (tmp->equivalentToType(indexTypeInKernel(rt_LLONG)))
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_LLONG")));
                else if (tmp->equivalentToType(C_Derived_Type(s_cmplx)))
                {
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_FLOAT_COMPLEX")));

                    SgSymbol *symb = createNewFunctionSymbol("real");
                    RTC_FCall[fc]->addArg(*new SgFunctionCallExp(*symb, *new SgExpression(EXPR_LIST, argList->lhs(), NULL, NULL)));

                    symb = createNewFunctionSymbol("imag");
                    RTC_FCall[fc]->addArg(*new SgFunctionCallExp(*symb, *new SgExpression(EXPR_LIST, argList->lhs(), NULL, NULL)));
                    dontCast = true;
                }
                else if (tmp->equivalentToType(C_Derived_Type(s_dcmplx)))
                {
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_DOUBLE_COMPLEX")));

                    SgSymbol *symb = createNewFunctionSymbol("real");
                    RTC_FCall[fc]->addArg(*new SgFunctionCallExp(*symb, *new SgExpression(EXPR_LIST, argList->lhs(), NULL, NULL)));

                    symb = createNewFunctionSymbol("imag");
                    RTC_FCall[fc]->addArg(*new SgFunctionCallExp(*symb, *new SgExpression(EXPR_LIST, argList->lhs(), NULL, NULL)));
                    dontCast = true;
                }
                else
                {
                    RTC_FCall[fc]->addArg(*new SgVarRefExp(new SgSymbol(VARIABLE_NAME, "rt_UNKNOWN")));
                    fprintf(stderr, "Warning[-rtc]: unknown type with variant %d for kernel lauch\n", tmp->variant());
                }

                if (dontCast == false)
                    RTC_FCall[fc]->addArg(*new SgCastExp(*tmp, *argList->lhs()));
            }

            argList = argList->rhs();
        }
    }

    RTC_FKernelArgs.clear();
    RTC_FArgs.clear();
    RTC_FCall.clear();
}

// convert unparse buffer for RTC call
char* _RTC_convertUnparse(const char* inBuf)
{
    int count = 0;
    for (unsigned i = 0; i < strlen(inBuf); ++i)
    {
        if (SpecialSymbols.find(inBuf[i]) != SpecialSymbols.end())
            count += strlen(SpecialSymbols[inBuf[i]]);
    }

    std::string strBuf = "";

    for (unsigned i = 0; i < strlen(inBuf); ++i)
    {
        if (SpecialSymbols.find(inBuf[i]) != SpecialSymbols.end())
        {
            const char *tmp = SpecialSymbols[inBuf[i]];
            for (unsigned k1 = 0; k1 < strlen(tmp); ++k1)
                strBuf.push_back(tmp[k1]);
        }
        else
            strBuf.push_back(inBuf[i]);
    }

    strBuf += "#undef dcmplx2\\n\"\n\"#undef cmplx2\\n";
    char *newBuf = new char[strlen(strBuf.c_str()) + 1];
    strcpy(newBuf, strBuf.c_str());

    return newBuf;
}

// convert cuda kernel to static const char*
void ACC_RTC_ConvertCudaKernel(SgStatement *cuda_kernel, const char *kernelName)
{
    if (cuda_kernel != NULL)
    {
        cuda_kernel->addComment("#define dcmplx2 Complex<double>\n#define cmplx2 Complex<float>\nextern \"C\"\n");
        char *buf = copyOfUnparse(UnparseBif_Char(cuda_kernel->thebif, C_LANG));
        char *newBuf = _RTC_convertUnparse(buf);
        
        SgPointerType *arrType = new SgPointerType(*C_Type(SgTypeChar()));

        SgSymbol *cuda_kernel_code = new SgSymbol(VARIABLE_NAME, kernelName, arrType, mod_gpu);
        SgStatement *decl = makeSymbolDeclarationWithInit(cuda_kernel_code, new SgValueExp(newBuf));

        decl->addDeclSpec(BIT_CONST);
        decl->addDeclSpec(BIT_STATIC);
        cuda_kernel->insertStmtBefore(*decl);
        if(acc_call_list)
        {
            symb_list **call_list = new (symb_list *);
            *call_list = acc_call_list;
            decl->addAttribute(RTC_CALLS, (void*)call_list, sizeof(symb_list *));
        }
        cuda_kernel->deleteStmt();
        delete[] buf;
    }
}

static symb_list *_RTC_addCalledToList(symb_list *call_list, graph_node *gnode)
{
    edge *gedge;

    for (gedge = gnode->to_called; gedge; gedge = gedge->next)
    	if(gedge->to->st_header)
        {   call_list = AddNewToSymbList(call_list, gedge->to->symb);
            call_list = _RTC_addCalledToList(call_list, gedge->to);
	}
    
    return call_list;
}

symb_list *ACC_RTC_ExpandCallList(symb_list *call_list)
{
    symb_list *sl;
    for (sl = call_list; sl; sl = sl->next)
    {
        if (!ATTR_NODE(sl->symb))
            continue;
        call_list = _RTC_addCalledToList(call_list, GRAPHNODE(sl->symb));
    }
    return call_list;
}

char* _RTC_PrototypesForKernel(symb_list *call_list)
{
    SgStatement *st = NULL;
    symb_list *sl = call_list;
    st = FunctionPrototype(GRAPHNODE(sl->symb)->st_copy->symbol());
    st->addDeclSpec(BIT_CUDA_DEVICE);
    st->addDeclSpec(BIT_STATIC);
    st->addComment("#define dcmplx2 Complex<double>\n#define cmplx2 Complex<float>\n");
    char *buffer = copyOfUnparse(UnparseBif_Char(st->thebif, C_LANG));
    for (sl = call_list->next; sl; sl = sl->next)
    {
        st = FunctionPrototype(GRAPHNODE(sl->symb)->st_copy->symbol());
        st->addDeclSpec(BIT_CUDA_DEVICE);
        st->addDeclSpec(BIT_STATIC);

        char *unp_buf = UnparseBif_Char(st->thebif, C_LANG);
        char *buf = new char[strlen(buffer) + strlen(unp_buf) + 1];
        strcpy(buf, buffer);
        strcat(buf, unp_buf);
        delete[] buffer;
        buffer = buf;
    }
    return (buffer);
}

void _RTC_UnparsedFunctionsToKernelConst(SgStatement *stmt)
{
    if (CALLED_FUNCTIONS(stmt) == NULL)
        return;

    symb_list *call_list = *CALLED_FUNCTIONS(stmt);

    graph_node * gnode = NULL;  
    char *buffer = _RTC_PrototypesForKernel(call_list);

    for (; call_list; call_list = call_list->next)
    {
        gnode = GRAPHNODE(call_list->symb);
        char *unp_buf = UnparseBif_Char(gnode->st_copy->thebif, C_LANG);
        char *buf = new char[strlen(unp_buf) + strlen(buffer) + 1];
        //buf[0] = '\0';
        strcpy(buf, buffer);
        strcat(buf, unp_buf);
        delete[] buffer;
        buffer = buf;
    }
    buffer = _RTC_convertUnparse(buffer);
  
    char *kernel_buf = ((SgValueExp *)((SgVarDeclStmt *)stmt)->initialValue(0))->stringValue();
    char *allBuf = new char[strlen(kernel_buf) + strlen(buffer) + 1];
    strcpy(allBuf, buffer);
    strcat(allBuf, kernel_buf);
    ((SgVarDeclStmt *)stmt)->setInitialValue(0, *new SgValueExp(allBuf));
    delete[] kernel_buf;
    delete[] buffer;
}


void ACC_RTC_AddFunctionsToKernelConsts(SgStatement *first_kernel_const)
{
    SgStatement *stmt = mod_gpu, *next = NULL;

    for (stmt = first_kernel_const; stmt; stmt = stmt->lexNext())
        _RTC_UnparsedFunctionsToKernelConst(stmt);
    stmt = mod_gpu;
    next = mod_gpu->lexNext();

    // extracting  function copies  
    //while(next->variant() !=  VAR_DECL)

    while (next != first_kernel_const)
    {
        stmt = next;
        next = next->lastNodeOfStmt()->lexNext();
        stmt->extractStmt();
    }

}

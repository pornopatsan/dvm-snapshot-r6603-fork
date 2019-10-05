#include "acc_data.h"


extern SgStatement *kernelScope;
static int indexGenerator = 0;

SgExpression* analyzeArrayIndxs(SgSymbol *array, SgExpression *listIdx)
{
    SgSymbol *varName = NULL;
    char *strNum = new char[32];
    char *strArray, *newStr;

    if (listIdx == NULL || !autoTransform || dontGenConvertXY || oneCase)
        return NULL;
    else
    {
        strArray = array->identifier();
        newStr = new char[strlen(strArray) + 32];

        Array *tArray = currentLoop->getArray(strArray);
        if (tArray)
        {
            char *charEx = NULL;
            SgSymbol *tSymb = tArray->findAccess(listIdx, charEx);
            if (tSymb == NULL)
            {
                newStr[0] = '\0';
                strcat(newStr, strArray);
                strcat(newStr, "_");
                sprintf(strNum, "%d", (int) indexGenerator);
                indexGenerator++;
                strcat(newStr, strNum);

                if (C_Cuda)
                    varName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(newStr), *C_DvmType(), *kernelScope);
                else
                {
                    if (undefined_Tcuda)
                    {
                        SgExpression *le;
                        le = new SgExpression(LEN_OP);
                        le->setLhs(new SgValueExp(8));
                        varName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(newStr), *new SgType(T_INT, le, SgTypeInt()), *kernelScope);
                    }
                    else
                        varName = new SgSymbol(VARIABLE_NAME, TestAndCorrectName(newStr), *SgTypeInt(), *kernelScope);
                }

                tArray->addNewCoef(listIdx, charEx, varName);
            }
            else
                varName = tSymb;
        }
    }

    delete[]strNum;
    return new SgVarRefExp(varName);
}
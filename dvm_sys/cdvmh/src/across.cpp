#include "converter.h"

#include <cstdio>

#include "aux_visitors.h"
#include "messages.h"

namespace cdvmh {

static void getDefaultCudaBlock(int &x, int &y, int &z, int loopDep, int loopIndep, bool autoTfm) {
    if (autoTfm) {
        if (loopDep == 0) {
            if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 14; z = 1;
            } else {
                x = 32; y = 7; z = 2;
            }
        } else if (loopDep == 1) {
            if (loopIndep == 0) {
                x = 1; y = 1; z = 1;
            } else if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 5; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        } else if (loopDep == 2) {
            if (loopIndep == 0) {
                x = 32; y = 1; z = 1;
            } else if (loopIndep == 1){
                x = 32; y = 4; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        } else if (loopDep >= 3) {
            if (loopIndep == 0) {
                x = 32; y = 5; z = 1;
            } else {
                x = 32; y = 5; z = 2;
            }
        }
    } else {
        if (loopDep == 0) {
            if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 14; z = 1;
            } else {
                x = 32; y = 7; z = 2;
            }
        } else if (loopDep == 1) {
            if (loopIndep == 0) {
                x = 1; y = 1; z = 1;
            } else if (loopIndep == 1) {
                x = 256; y = 1; z = 1;
            } else if (loopIndep == 2) {
                x = 32; y = 8; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        } else if (loopDep == 2) {
            if (loopIndep == 0) {
                x = 32; y = 1; z = 1;
            } else if (loopIndep == 1) {
                x = 32; y = 4; z = 1;
            } else {
                x = 16; y = 8; z = 2;
            }
        } else if (loopDep >= 3) {
            if (loopIndep == 0) {
                x = 8; y = 4; z = 1;
            } else {
                x = 8; y = 4; z = 2;
            }
        }
    }
}

void ConverterASTVisitor::genAcrossCudaCaseKernel(int dep_number, const KernelDesc &kernelDesc, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
        std::string handlerTemplateDecl, std::string &kernelText)
{
    std::string caseKernelName = kernelDesc.kernelName;
    std::string caseKernelFormalParams;
    std::string caseKernelFormalParams_arrays;
    std::string caseKernelFormalParams_scalars;
    std::string caseKernelBody;
    std::string indexT = kernelDesc.indexT;
    std::string indent = indentStep;
    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);

    bool isAcross = (isSequentialPart ? false : curPragma->acrosses.size() > 0);
    bool autoTfm = opts.autoTfm;
//    bool prepareDiag = isAcross && autoTfm && (curPragma->acrosses.size() > 1 || curPragma->acrosses[0].getDepCount() > 1);
    bool prepareDiag = isAcross && autoTfm && ( dep_number > 1 );

    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(parLoopBodyStmt);
    prohibitedNames = collectNamesVisitor.getNames();
    for (int i = 0; i < (int)outerParams.size(); i++)
        prohibitedNames.insert(outerParams[i]->getName().str());
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
        prohibitedNames.insert((*it)->getName().str());
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++)
        prohibitedNames.insert((*it)->getName().str());

    // get unique names
    std::string coords = getUniqueName( "coords", &prohibitedNames );

    // get outer params
    std::map<std::string, std::string> scalarPtrs;
    std::map<std::string, std::vector<std::string> > dvmCoefs;
    std::map<std::string, std::map<std::string, std::string> > dvmDiagInfos;
    std::map<std::string, std::string> redGrid;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            dvmCoefs[varState->name].clear();
            for (int j = 0; j < varState->headerArraySize; j++)
                dvmCoefs[varState->name].push_back(getUniqueName(varState->name + "_hdr" + toStr(j), &prohibitedNames));
            if (prepareDiag) {
                std::map<std::string, std::string> &m = dvmDiagInfos[varState->name];
                m.clear();
                m["tfmType"] = getUniqueName(varState->name + "_tfmType", &prohibitedNames);
                m["xAxis"] = getUniqueName(varState->name + "_xAxis", &prohibitedNames);
                m["yAxis"] = getUniqueName(varState->name + "_yAxis", &prohibitedNames);
                m["Rx"] = getUniqueName(varState->name + "_Rx", &prohibitedNames);
                m["Ry"] = getUniqueName(varState->name + "_Ry", &prohibitedNames);
                m["xOffset"] = getUniqueName(varState->name + "_xOffset", &prohibitedNames);
                m["yOffset"] = getUniqueName(varState->name + "_yOffset", &prohibitedNames);
            }
        } else {
            scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames);
        }
    }
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++) {
        VarState *varState = &varStates[*it];
        redGrid[varState->name] = getUniqueName(varState->name + "_grid", &prohibitedNames);
    }

    if ( dep_number == 1 ) {
        int n_cuda_dims = std::min( loopRank - dep_number, 3 );
        char cuda_letter_first = n_cuda_dims == 3 ? 'z' : n_cuda_dims == 2 ? 'y' : 'x';
        char cuda_letter_second = cuda_letter_first - 1;
        char cuda_letter_third = cuda_letter_first - 2;

        caseKernelBody += indent + "/* Parameters */\n";
        for (int i = 0; i < (int)outerParams.size(); i++) {
            VarState *varState = &varStates[outerParams[i]];
            int rank = varState->rank;
            std::string refName = varState->name;
//            printf( "%s\n", refName.c_str() );
            if (varState->isArray) {
                // XXX: Not so good solution, maybe
                std::string devBaseName = refName + "_base";

                if (rank > 1) {
                    std::string elemT = varState->baseTypeStr; // TODO: Add 'const' where appropriate
                    std::string ptrT = elemT + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT" : "");
                    caseKernelFormalParams_arrays += ", " + ptrT + " " + devBaseName;
                    caseKernelBody += indent + (autoTfm ? (prepareDiag ? "DvmhDiagonalizedArrayHelper" : "DvmhPermutatedArrayHelper") : "DvmhArrayHelper") + "<" +
                            toStr(rank) + ", " + elemT + ", " + ptrT + ", " + indexT + "> " + refName + "(" + devBaseName;
                    caseKernelBody += ", DvmhArrayCoefficients<" + toStr(autoTfm ? rank : rank - 1) + ", " + indexT +">(";
                    std::string coefList;
                    for (int j = 1; j <= rank; j++) {
                        int hdrIdx = j;
                        std::string coefName = dvmCoefs[refName][hdrIdx];
                        if (j < rank || autoTfm) {
                            caseKernelFormalParams_arrays += ", " + indexT + " " + coefName;
                            coefList += ", " + coefName;
                        }
                    }
                    trimList(coefList);
                    caseKernelBody += coefList + ")";
                    if (prepareDiag) {
                        std::map<std::string, std::string> &m = dvmDiagInfos[refName];
                        caseKernelBody += ", DvmhDiagInfo<" + indexT + ">(";
                        caseKernelFormalParams_arrays += ", int " + m["tfmType"];
                        caseKernelBody += m["tfmType"];
                        caseKernelFormalParams_arrays += ", int " + m["xAxis"];
                        caseKernelBody += ", " + m["xAxis"];
                        caseKernelFormalParams_arrays += ", int " + m["yAxis"];
                        caseKernelBody += ", " + m["yAxis"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["Rx"];
                        caseKernelBody += ", " + m["Rx"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["Ry"];
                        caseKernelBody += ", " + m["Ry"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["xOffset"];
                        caseKernelBody += ", " + m["xOffset"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["yOffset"];
                        caseKernelBody += ", " + m["yOffset"];
                        caseKernelBody += ")";
                    }
                    caseKernelBody += ");\n";
                } else {
                    caseKernelFormalParams_arrays += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + refName;
                }
            } else {
                // TODO: Add 'use' case for variables
                caseKernelFormalParams_scalars += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + scalarPtrs[refName];
                caseKernelBody += indent + varState->baseTypeStr + " &" + (varState->canBeRestrict ? " DVMH_RESTRICT_REF " : "") + refName +
                        " = *" + scalarPtrs[refName] + ";\n";
            }
        }
        caseKernelFormalParams += caseKernelFormalParams_arrays + caseKernelFormalParams_scalars;
        caseKernelBody += "\n";

        // handle reduction params
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            // TODO: Add support for reduction arrays
            ClauseReduction *red = &curPragma->reductions[i];
            std::string epsGrid = redGrid[red->arrayName];
            VarDecl *vd = seekVarDecl(red->arrayName);
            checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
            VarState *varState = &varStates[vd];
            checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

            caseKernelFormalParams += ", " + varState->baseTypeStr + " " + red->arrayName;
            caseKernelFormalParams += ", " + varState->baseTypeStr + " " + epsGrid + "[]";
            if (red->isLoc()) {
                std::string locGrid = redGrid[red->locName];
                VarDecl *lvd = seekVarDecl(red->locName);
                checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *locVarState = &varStates[lvd];
                checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());

                caseKernelFormalParams += ", " + locVarState->baseTypeStr + " " + red->locName;
                caseKernelFormalParams += ", " + locVarState->baseTypeStr + " " + locGrid + "[]";

            }
        }

        for ( int i = dep_number; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " num_elem_" + toStr( i );
        }

        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " base_" + toStr( i );
        }
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " step_" + toStr( i );
        }
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " idxs_" + toStr( i );
        }
        trimList( caseKernelFormalParams );
        // caseKernelFormalParams is done

        caseKernelBody += indent + "/* User variables - loop index variables and other private variables */\n";
        for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++) {
            VarState *varState = &varStates[*it];
            caseKernelBody += indent + varState->genDecl() + ";\n";
        }
        caseKernelBody += "\n";

        // shared memory for reduction
        if ( loopRank > dep_number ) {
            for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                // TODO: Add support for reduction arrays
                ClauseReduction *red = &curPragma->reductions[i];
                std::string redType = red->redType;
                std::string epsGrid = redGrid[red->arrayName];
                VarDecl *vd = seekVarDecl(red->arrayName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                caseKernelBody += indent + "extern __shared__ " + varState->baseTypeStr + " " + red->arrayName + "_block[];\n";
            }
            caseKernelBody += "\n";
        }

        caseKernelBody += indent + "/* Computation parameters */\n";
        caseKernelBody += indent + indexT + " " + coords + "[" + toStr( loopRank ) + "];\n";
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseKernelBody += indent + indexT + " id_" + toStr( letter ) + " = blockIdx." + toStr( letter ) +
                    " * blockDim." + toStr( letter ) + " + threadIdx." + toStr( letter ) + ";\n";
        }
        caseKernelBody += "\n";

        caseKernelBody += indent + "/* Execute one iteration */\n";
        if ( loopRank > 1 ) {
            caseKernelBody += indent + "if (id_" + toStr( cuda_letter_first ) + " < num_elem_1";

            if ( n_cuda_dims >= 2 ) {
                caseKernelBody += " && id_" + toStr( cuda_letter_second ) + " < num_elem_2";
            }

            if ( n_cuda_dims == 3 ) {
                caseKernelBody += " && id_" + toStr( cuda_letter_third ) + " < ";
                for ( int i = 3; i < loopRank; ++i ) {
                    if ( i == 3 ) {
                        caseKernelBody += "num_elem_" + toStr( i );
                    } else {
                        caseKernelBody += " * num_elem_" + toStr( i );
                    }
                }
            }
        } else {
            caseKernelBody += indent + "if (1";
        }

        caseKernelBody += ") {\n";
        indent += indentStep;

        caseKernelBody += indent + coords + "[idxs_0] = base_0;\n";
        for ( int i = 1; i < loopRank; ++i ) {
            std::string product;
            for ( int j = i + 1; j < loopRank; ++j ) {
                if ( j == i + 1 ) {
                    product += "num_elem_" + toStr( j );
                } else {
                    product += " * num_elem_" + toStr( j );
                }
            }
            std::string product_division = product.length() == 0 ? "" : " / (" + product + ")";

            char letter = i > 2 ? cuda_letter_third : i > 1 ? cuda_letter_second : cuda_letter_first;
            std::string expression;

            if ( i < 3 ) {
                expression = "id_" + toStr( letter );
            } else if ( i == 3 ) {
                expression = "id_" + toStr( letter ) + product_division;
            } else if ( i > 3 && i < loopRank - 1 ) {
                expression = "(id_" + toStr( letter ) + product_division + ") % num_elem_" + toStr( i );
            } else if ( i == loopRank - 1 ) {
                expression = "id_" + toStr( letter ) + " % num_elem_" + toStr( i );
            }

            caseKernelBody += indent + coords + "[idxs_" + toStr( i ) + "] = base_" +
                    toStr( i ) + " + (" + expression + ") * step_" + toStr( i ) + ";\n";
        }

        for ( int i = 0; i < ( int )loopVars.size(); ++i ) {
            std::string varName = loopVars[ i ].name;
            caseKernelBody += indent + varName + " = " + coords + "[" + toStr( i ) + "];\n";
        }
        caseKernelBody += "\n";

        // insert loop body
        std::string loopBody = convertToString(parLoopBodyStmt);
        if ( !isa< CompoundStmt >( parLoopBodyStmt ) ) {
            caseKernelBody += indent + "{\n";
            indent += indentStep;
        }
        int lastPos = 0;
        while (loopBody.find('\n', lastPos) != std::string::npos) {
            int nlPos = loopBody.find('\n', lastPos);
            caseKernelBody += indent + loopBody.substr(lastPos, nlPos - lastPos + 1);
            lastPos = nlPos + 1;
            if (lastPos >= (int)loopBody.size())
                break;
        }
        if (lastPos < (int)loopBody.size())
            caseKernelBody += indent + loopBody.substr(lastPos) + "\n";
        if (!isFull(parLoopBodyStmt)) {
            caseKernelBody[caseKernelBody.size() - 1] = ';';
            caseKernelBody += "\n";
        }
        if (!isa<CompoundStmt>(parLoopBodyStmt)) {
            indent = subtractIndent( indent );
            caseKernelBody += indent + "}\n";
        }
        caseKernelBody += "\n";
        // finish inserting

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";

        // start reduction
        if ( reductions.size() > 0 ) {
            if ( loopRank == dep_number ) {
                caseKernelBody += indent + "/* Reduction */\n";
                for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                    // TODO: Add support for reduction arrays
                    ClauseReduction *red = &curPragma->reductions[i];
                    std::string redType = red->redType;
                    std::string epsGrid = redGrid[red->arrayName];
                    VarDecl *vd = seekVarDecl(red->arrayName);
                    checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                    bool modefun = redType == "rf_MIN" || redType == "rf_MAX";
                    std::string op;
                    if ( redType == "rf_SUM" ) {
                        op = "+";
                    } else if ( redType == "rf_PRODUCT" ) {
                        op = "*";
                    } else if ( redType == "rf_MAX" ) {
                        op = "max";
                    } else if ( redType == "rf_MIN" ) {
                        op = "min";
                    } else if ( redType == "rf_AND" ) {
                        op = "&&";
                    } else if ( redType == "rf_OR" ) {
                        op = "||";
                    } else if ( redType == "rf_EQV" ) {
                        op = "==";
                    } else if ( redType == "rf_NEQV" ) {
                        op = "!=";
                    }

                    if ( modefun ) {
                        caseKernelBody += indent + epsGrid + "[id_" + cuda_letter_first + "] = " + op + "(" + epsGrid + "[id_x], " + red->arrayName + ");\n";
                    } else {
                        caseKernelBody += indent + epsGrid + "[id_" + cuda_letter_first + "] = " + epsGrid + "[id_x] " + op + " " + red->arrayName + ";\n";
                    }
                }
            } else {
                caseKernelBody += indent + "/* Reduction */\n";
                caseKernelBody += indent + "id_" + cuda_letter_first + " = blockDim.x * blockDim.y * blockDim.z / 2;\n";
                caseKernelBody += indent + loopVars[ 0 ].name + " = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * (blockDim.x * blockDim.y);\n";

                for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                    // TODO: Add support for reduction arrays
                    ClauseReduction *red = &curPragma->reductions[i];
                    std::string redType = red->redType;
                    std::string epsGrid = redGrid[red->arrayName];
                    VarDecl *vd = seekVarDecl(red->arrayName);
                    checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                    bool modefun = redType == "rf_MIN" || redType == "rf_MAX";
                    std::string op;
                    if ( redType == "rf_SUM" ) {
                        op = "+";
                    } else if ( redType == "rf_PRODUCT" ) {
                        op = "*";
                    } else if ( redType == "rf_MAX" ) {
                        op = "max";
                    } else if ( redType == "rf_MIN" ) {
                        op = "min";
                    } else if ( redType == "rf_AND" ) {
                        op = "&&";
                    } else if ( redType == "rf_OR" ) {
                        op = "||";
                    } else if ( redType == "rf_EQV" ) {
                        op = "==";
                    } else if ( redType == "rf_NEQV" ) {
                        op = "!=";
                    }

                    caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " + red->arrayName + ";\n";
                    caseKernelBody += indent + "__syncthreads();\n";
                    caseKernelBody += indent + loopVars[ 1 ].name + " = id_" + cuda_letter_first + ";\n";
                    caseKernelBody += indent + "while (" + loopVars[ 1 ].name + " >= 1) {\n";
                    indent += indentStep;

                    caseKernelBody += indent + "__syncthreads();\n";
                    caseKernelBody += indent + "if (" + loopVars[ 0 ].name + " < " + loopVars[ 1 ].name + ") {\n";
                    indent += indentStep;

                    if ( modefun ) {
                        caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " +
                                op + "(" + red->arrayName + "_block[" + loopVars[ 0 ].name + "], " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + " + " + loopVars[ 1 ].name + "]);\n";
                    } else {
                        caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + "] " + op + " " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + " + " + loopVars[ 1 ].name + "];\n";
                    }

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";
                    caseKernelBody += indent + loopVars[ 1 ].name + " = " + loopVars[ 1 ].name + " / 2;\n";

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";

                    caseKernelBody += indent + "if (" + loopVars[ 0 ].name + " == 0) {\n";
                    indent += indentStep;

                    std::string index_expr = "blockIdx.x + (blockIdx.y + blockIdx.z * gridDim.y) * gridDim.x";
                    if ( modefun ) {
                        caseKernelBody += indent + epsGrid + "[" + index_expr + "] = " +
                                op + "(" + epsGrid + "[" + index_expr + "], " +
                                red->arrayName + "_block[0]);\n";
                    } else {
                        caseKernelBody += indent + epsGrid + "[" + index_expr + "] = " +
                                epsGrid + "[" + index_expr + "] " + op + " " +
                                red->arrayName + "_block[0];\n";
                    }

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";
                }
            }
        }
    } else if ( dep_number == 2 ) {
        int n_cuda_dims = 1 + ( loopRank >= dep_number + 1 ? 1 : 0 ) + ( loopRank > dep_number + 1 ? 1 : 0 );
        char cuda_letter_first = n_cuda_dims == 3 ? 'z' : n_cuda_dims == 2 ? 'y' : 'x';
        char cuda_letter_second = cuda_letter_first - 1;
        char cuda_letter_third = cuda_letter_first - 2;

        caseKernelBody += indent + "/* Parameters */\n";
        for (int i = 0; i < (int)outerParams.size(); i++) {
            VarState *varState = &varStates[outerParams[i]];
            int rank = varState->rank;
            std::string refName = varState->name;
            if (varState->isArray) {
                // XXX: Not so good solution, maybe
                std::string devBaseName = refName + "_base";

                if (rank > 1) {
                    std::string elemT = varState->baseTypeStr; // TODO: Add 'const' where appropriate
                    std::string ptrT = elemT + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT" : "");
                    caseKernelFormalParams_arrays += ", " + ptrT + " " + devBaseName;
                    caseKernelBody += indent + (autoTfm ? (prepareDiag ? "DvmhDiagonalizedArrayHelper" : "DvmhPermutatedArrayHelper") : "DvmhArrayHelper") + "<" +
                            toStr(rank) + ", " + elemT + ", " + ptrT + ", " + indexT + "> " + refName + "(" + devBaseName;
                    caseKernelBody += ", DvmhArrayCoefficients<" + toStr(autoTfm ? rank : rank - 1) + ", " + indexT +">(";
                    std::string coefList;
                    for (int j = 1; j <= rank; j++) {
                        int hdrIdx = j;
                        std::string coefName = dvmCoefs[refName][hdrIdx];
                        if (j < rank || autoTfm) {
                            caseKernelFormalParams_arrays += ", " + indexT + " " + coefName;
                            coefList += ", " + coefName;
                        }
                    }
                    trimList(coefList);
                    caseKernelBody += coefList + ")";
                    if (prepareDiag) {
                        std::map<std::string, std::string> &m = dvmDiagInfos[refName];
                        caseKernelBody += ", DvmhDiagInfo<" + indexT + ">(";
                        caseKernelFormalParams_arrays += ", int " + m["tfmType"];
                        caseKernelBody += m["tfmType"];
                        caseKernelFormalParams_arrays += ", int " + m["xAxis"];
                        caseKernelBody += ", " + m["xAxis"];
                        caseKernelFormalParams_arrays += ", int " + m["yAxis"];
                        caseKernelBody += ", " + m["yAxis"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["Rx"];
                        caseKernelBody += ", " + m["Rx"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["Ry"];
                        caseKernelBody += ", " + m["Ry"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["xOffset"];
                        caseKernelBody += ", " + m["xOffset"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["yOffset"];
                        caseKernelBody += ", " + m["yOffset"];
                        caseKernelBody += ")";
                    }
                    caseKernelBody += ");\n";
                } else {
                    caseKernelFormalParams_arrays += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + refName;
                }
            } else {
                // TODO: Add 'use' case for variables
                caseKernelFormalParams_scalars += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + scalarPtrs[refName];
                caseKernelBody += indent + varState->baseTypeStr + " &" + (varState->canBeRestrict ? " DVMH_RESTRICT_REF " : "") + refName +
                        " = *" + scalarPtrs[refName] + ";\n";
            }
        }
        caseKernelFormalParams += caseKernelFormalParams_arrays + caseKernelFormalParams_scalars;
        caseKernelBody += "\n";

        // handle reduction params
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            // TODO: Add support for reduction arrays
            ClauseReduction *red = &curPragma->reductions[i];
            std::string epsGrid = redGrid[red->arrayName];
            VarDecl *vd = seekVarDecl(red->arrayName);
            checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
            VarState *varState = &varStates[vd];
            checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

            caseKernelFormalParams += ", " + varState->baseTypeStr + " " + red->arrayName;
            caseKernelFormalParams += ", " + varState->baseTypeStr + " " + epsGrid + "[]";
            if (red->isLoc()) {
                std::string locGrid = redGrid[red->locName];
                VarDecl *lvd = seekVarDecl(red->locName);
                checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *locVarState = &varStates[lvd];
                checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());

                caseKernelFormalParams += ", " + locVarState->baseTypeStr + " " + red->locName;
                caseKernelFormalParams += ", " + locVarState->baseTypeStr + " " + locGrid + "[]";

            }
        }

        caseKernelFormalParams += ", DvmType num_elem_across";

        for ( int i = dep_number; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " num_elem_" + toStr( i );
        }

        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " base_" + toStr( i );
        }
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " step_" + toStr( i );
        }
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " idxs_" + toStr( i );
        }
        trimList( caseKernelFormalParams );
        // caseKernelFormalParams is done

        caseKernelBody += indent + "/* User variables - loop index variables and other private variables */\n";
        for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++) {
            VarState *varState = &varStates[*it];
            caseKernelBody += indent + varState->genDecl() + ";\n";
        }
        caseKernelBody += "\n";

        // shared memory for reduction
        if ( loopRank > dep_number ) {
            for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                // TODO: Add support for reduction arrays
                ClauseReduction *red = &curPragma->reductions[i];
                std::string redType = red->redType;
                std::string epsGrid = redGrid[red->arrayName];
                VarDecl *vd = seekVarDecl(red->arrayName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                caseKernelBody += indent + "extern __shared__ " + varState->baseTypeStr + " " + red->arrayName + "_block[];\n";
            }
            caseKernelBody += "\n";
        }

        caseKernelBody += indent + "/* Computation parameters */\n";
        caseKernelBody += indent + indexT + " " + coords + "[" + toStr( loopRank ) + "];\n";
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseKernelBody += indent + indexT + " id_" + toStr( letter ) + " = blockIdx." + toStr( letter ) +
                    " * blockDim." + toStr( letter ) + " + threadIdx." + toStr( letter ) + ";\n";
        }
        caseKernelBody += "\n";

        caseKernelBody += indent + "/* Execute one iteration */\n";
        caseKernelBody += indent + "if (id_" + toStr( cuda_letter_first ) + " < num_elem_across";

        if ( n_cuda_dims >= 2 ) {
            caseKernelBody += " && id_" + toStr( cuda_letter_second ) + " < num_elem_2";
        }

        if ( n_cuda_dims == 3 ) {
            caseKernelBody += " && id_" + toStr( cuda_letter_third ) + " < ";
            for ( int i = 3; i < loopRank; ++i ) {
                if ( i == 3 ) {
                    caseKernelBody += "num_elem_" + toStr( i );
                } else {
                    caseKernelBody += " * num_elem_" + toStr( i );
                }
            }
        }

        caseKernelBody += ") {\n";
        indent += indentStep;
        caseKernelBody += indent + coords + "[idxs_0] = base_0 - id_" + toStr( cuda_letter_first ) + " * step_0;\n";
        caseKernelBody += indent + coords + "[idxs_1] = base_1 + id_" + toStr( cuda_letter_first ) + " * step_1;\n";
        for ( int i = 2; i < loopRank; ++i ) {
            std::string product;
            for ( int j = i + 1; j < loopRank; ++j ) {
                if ( j == i + 1 ) {
                    product += "num_elem_" + toStr( j );
                } else {
                    product += " * num_elem_" + toStr( j );
                }
            }
            std::string product_division = product.length() == 0 ? "" : " / (" + product + ")";

            char letter = i > 2 ? cuda_letter_third : cuda_letter_second;
            std::string expression;

            if ( i < 3 ) {
                expression = "id_" + toStr( letter );
            } else if ( i == 3 ) {
                expression = "id_" + toStr( letter ) + product_division;
            } else if ( i > 3 && i < loopRank - 1 ) {
                expression = "(id_" + toStr( letter ) + product_division + ") % num_elem_" + toStr( i );
            } else if ( i == loopRank - 1 ) {
                expression = "id_" + toStr( letter ) + " % num_elem_" + toStr( i );
            }

            caseKernelBody += indent + coords + "[idxs_" + toStr( i ) + "] = base_" +
                    toStr( i ) + " + (" + expression + ") * step_" + toStr( i ) + ";\n";
        }

        for ( int i = 0; i < ( int )loopVars.size(); ++i ) {
            std::string varName = loopVars[ i ].name;
            caseKernelBody += indent + varName + " = " + coords + "[" + toStr( i ) + "];\n";
        }
        caseKernelBody += "\n";

        // insert loop body
        std::string loopBody = convertToString(parLoopBodyStmt);
        if ( !isa< CompoundStmt >( parLoopBodyStmt ) ) {
            caseKernelBody += indent + "{\n";
            indent += indentStep;
        }
        int lastPos = 0;
        while (loopBody.find('\n', lastPos) != std::string::npos) {
            int nlPos = loopBody.find('\n', lastPos);
            caseKernelBody += indent + loopBody.substr(lastPos, nlPos - lastPos + 1);
            lastPos = nlPos + 1;
            if (lastPos >= (int)loopBody.size())
                break;
        }
        if (lastPos < (int)loopBody.size())
            caseKernelBody += indent + loopBody.substr(lastPos) + "\n";
        if (!isFull(parLoopBodyStmt)) {
            caseKernelBody[caseKernelBody.size() - 1] = ';';
            caseKernelBody += "\n";
        }
        if (!isa<CompoundStmt>(parLoopBodyStmt)) {
            indent = subtractIndent( indent );
            caseKernelBody += indent + "}\n";
        }
        caseKernelBody += "\n";
        // finish inserting

        // start reduction
        if ( reductions.size() > 0 ) {
            if ( loopRank == dep_number ) {
                caseKernelBody += indent + "/* Reduction */\n";
                for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                    // TODO: Add support for reduction arrays
                    ClauseReduction *red = &curPragma->reductions[i];
                    std::string redType = red->redType;
                    std::string epsGrid = redGrid[red->arrayName];
                    VarDecl *vd = seekVarDecl(red->arrayName);
                    checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                    bool modefun = redType == "rf_MIN" || redType == "rf_MAX";
                    std::string op;
                    if ( redType == "rf_SUM" ) {
                        op = "+";
                    } else if ( redType == "rf_PRODUCT" ) {
                        op = "*";
                    } else if ( redType == "rf_MAX" ) {
                        op = "max";
                    } else if ( redType == "rf_MIN" ) {
                        op = "min";
                    } else if ( redType == "rf_AND" ) {
                        op = "&&";
                    } else if ( redType == "rf_OR" ) {
                        op = "||";
                    } else if ( redType == "rf_EQV" ) {
                        op = "==";
                    } else if ( redType == "rf_NEQV" ) {
                        op = "!=";
                    }

                    if ( modefun ) {
                        caseKernelBody += indent + epsGrid + "[id_" + toStr( cuda_letter_first ) + "] = " + op + "(" + epsGrid + "[id_x], " + red->arrayName + ");\n";
                    } else {
                        caseKernelBody += indent + epsGrid + "[id_" + toStr( cuda_letter_first ) + "] = " + epsGrid + "[id_x] " + op + " " + red->arrayName + ";\n";
                    }
                }
            } else {
                caseKernelBody += indent + "/* Reduction */\n";
                caseKernelBody += indent + "id_" + toStr( cuda_letter_first ) + " = blockDim.x * blockDim.y * blockDim.z / 2;\n";
                caseKernelBody += indent + loopVars[ 0 ].name + " = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * (blockDim.x * blockDim.y);\n";

                for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                    // TODO: Add support for reduction arrays
                    ClauseReduction *red = &curPragma->reductions[i];
                    std::string redType = red->redType;
                    std::string epsGrid = redGrid[red->arrayName];
                    VarDecl *vd = seekVarDecl(red->arrayName);
                    checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                    bool modefun = redType == "rf_MIN" || redType == "rf_MAX";
                    std::string op;
                    if ( redType == "rf_SUM" ) {
                        op = "+";
                    } else if ( redType == "rf_PRODUCT" ) {
                        op = "*";
                    } else if ( redType == "rf_MAX" ) {
                        op = "max";
                    } else if ( redType == "rf_MIN" ) {
                        op = "min";
                    } else if ( redType == "rf_AND" ) {
                        op = "&&";
                    } else if ( redType == "rf_OR" ) {
                        op = "||";
                    } else if ( redType == "rf_EQV" ) {
                        op = "==";
                    } else if ( redType == "rf_NEQV" ) {
                        op = "!=";
                    }

                    caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " + red->arrayName + ";\n";
                    caseKernelBody += indent + "__syncthreads();\n";
                    caseKernelBody += indent + loopVars[ 1 ].name + " = id_" + toStr( cuda_letter_first ) + ";\n";
                    caseKernelBody += indent + "while (" + loopVars[ 1 ].name + " >= 1) {\n";
                    indent += indentStep;

                    caseKernelBody += indent + "__syncthreads();\n";
                    caseKernelBody += indent + "if (" + loopVars[ 0 ].name + " < " + loopVars[ 1 ].name + ") {\n";
                    indent += indentStep;

                    if ( modefun ) {
                        caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " +
                                op + "(" + red->arrayName + "_block[" + loopVars[ 0 ].name + "], " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + " + " + loopVars[ 1 ].name + "]);\n";
                    } else {
                        caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + "] " + op + " " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + " + " + loopVars[ 1 ].name + "];\n";
                    }

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";
                    caseKernelBody += indent + loopVars[ 1 ].name + " = " + loopVars[ 1 ].name + " / 2;\n";

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";

                    caseKernelBody += indent + "if (" + loopVars[ 0 ].name + " == 0) {\n";
                    indent += indentStep;

                    std::string index_expr = "blockIdx.x + (blockIdx.y + blockIdx.z * gridDim.y) * gridDim.x";
                    if ( modefun ) {
                        caseKernelBody += indent + epsGrid + "[" + index_expr + "] = " +
                                op + "(" + epsGrid + "[" + index_expr + "], " +
                                red->arrayName + "_block[0]);\n";
                    } else {
                        caseKernelBody += indent + epsGrid + "[" + index_expr + "] = " +
                                epsGrid + "[" + index_expr + "] " + op + " " +
                                red->arrayName + "_block[0];\n";
                    }

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";
                }
            }
        }

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";
        caseKernelBody += "\n";
    } else if ( dep_number >= 3 ) {
        int n_cuda_dims = 2 + ( loopRank > dep_number ? 1 : 0 );
        char cuda_letter_first = n_cuda_dims == 3 ? 'z' : n_cuda_dims == 2 ? 'y' : 'x';
        char cuda_letter_second = cuda_letter_first - 1;
        char cuda_letter_third = cuda_letter_first - 2;

        caseKernelBody += indent + "/* Parameters */\n";
        for (int i = 0; i < (int)outerParams.size(); i++) {
            VarState *varState = &varStates[outerParams[i]];
            int rank = varState->rank;
            std::string refName = varState->name;
            if (varState->isArray) {
                // XXX: Not so good solution, maybe
                std::string devBaseName = refName + "_base";

                if (rank > 1) {
                    std::string elemT = varState->baseTypeStr; // TODO: Add 'const' where appropriate
                    std::string ptrT = elemT + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT" : "");
                    caseKernelFormalParams_arrays += ", " + ptrT + " " + devBaseName;
                    caseKernelBody += indent + (autoTfm ? (prepareDiag ? "DvmhDiagonalizedArrayHelper" : "DvmhPermutatedArrayHelper") : "DvmhArrayHelper") + "<" +
                            toStr(rank) + ", " + elemT + ", " + ptrT + ", " + indexT + "> " + refName + "(" + devBaseName;
                    caseKernelBody += ", DvmhArrayCoefficients<" + toStr(autoTfm ? rank : rank - 1) + ", " + indexT +">(";
                    std::string coefList;
                    for (int j = 1; j <= rank; j++) {
                        int hdrIdx = j;
                        std::string coefName = dvmCoefs[refName][hdrIdx];
                        if (j < rank || autoTfm) {
                            caseKernelFormalParams_arrays += ", " + indexT + " " + coefName;
                            coefList += ", " + coefName;
                        }
                    }
                    trimList(coefList);
                    caseKernelBody += coefList + ")";
                    if (prepareDiag) {
                        std::map<std::string, std::string> &m = dvmDiagInfos[refName];
                        caseKernelBody += ", DvmhDiagInfo<" + indexT + ">(";
                        caseKernelFormalParams_arrays += ", int " + m["tfmType"];
                        caseKernelBody += m["tfmType"];
                        caseKernelFormalParams_arrays += ", int " + m["xAxis"];
                        caseKernelBody += ", " + m["xAxis"];
                        caseKernelFormalParams_arrays += ", int " + m["yAxis"];
                        caseKernelBody += ", " + m["yAxis"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["Rx"];
                        caseKernelBody += ", " + m["Rx"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["Ry"];
                        caseKernelBody += ", " + m["Ry"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["xOffset"];
                        caseKernelBody += ", " + m["xOffset"];
                        caseKernelFormalParams_arrays += ", " + indexT + " " + m["yOffset"];
                        caseKernelBody += ", " + m["yOffset"];
                        caseKernelBody += ")";
                    }
                    caseKernelBody += ");\n";
                } else {
                    caseKernelFormalParams_arrays += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + refName;
                }
            } else {
                // TODO: Add 'use' case for variables
                caseKernelFormalParams_scalars += ", " + varState->baseTypeStr + " *" + (varState->canBeRestrict ? " DVMH_RESTRICT " : "") + scalarPtrs[refName];
                caseKernelBody += indent + varState->baseTypeStr + " &" + (varState->canBeRestrict ? " DVMH_RESTRICT_REF " : "") + refName +
                        " = *" + scalarPtrs[refName] + ";\n";
            }
        }
        caseKernelBody += "\n";
        caseKernelFormalParams += caseKernelFormalParams_arrays + caseKernelFormalParams_scalars;

        // handle reduction params
        for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
            // TODO: Add support for reduction arrays
            ClauseReduction *red = &curPragma->reductions[i];
            std::string epsGrid = redGrid[red->arrayName];
            VarDecl *vd = seekVarDecl(red->arrayName);
            checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
            VarState *varState = &varStates[vd];
            checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

            caseKernelFormalParams += ", " + varState->baseTypeStr + " " + red->arrayName;
            caseKernelFormalParams += ", " + varState->baseTypeStr + " " + epsGrid + "[]";
            if (red->isLoc()) {
                std::string locGrid = redGrid[red->locName];
                VarDecl *lvd = seekVarDecl(red->locName);
                checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *locVarState = &varStates[lvd];
                checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());

                caseKernelFormalParams += ", " + locVarState->baseTypeStr + " " + red->locName;
                caseKernelFormalParams += ", " + locVarState->baseTypeStr + " " + locGrid + "[]";

            }
        }

        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " base_" + toStr( i );
        }
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " step_" + toStr( i );
        }

        caseKernelFormalParams += ", DvmType max_z, DvmType SE";
        caseKernelFormalParams += ", DvmType var1, DvmType var2, DvmType var3";
        caseKernelFormalParams += ", DvmType Emax, DvmType Emin";
        caseKernelFormalParams += ", DvmType min_01";
        caseKernelFormalParams += ", DvmType swap_01";

        for ( int i = dep_number; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " num_elem_" + toStr( i );
        }

        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFormalParams += ", " + indexT + " idxs_" + toStr( i );
        }
        trimList( caseKernelFormalParams );
        // caseKernelFormalParams is done

        caseKernelBody += indent + "/* User variables - loop index variables and other private variables */\n";
        for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++) {
            VarState *varState = &varStates[*it];
            caseKernelBody += indent + varState->genDecl() + ";\n";
        }
        caseKernelBody += "\n";

        // shared memory for reduction
        if ( loopRank > dep_number ) {
            for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                // TODO: Add support for reduction arrays
                ClauseReduction *red = &curPragma->reductions[i];
                std::string redType = red->redType;
                std::string epsGrid = redGrid[red->arrayName];
                VarDecl *vd = seekVarDecl(red->arrayName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                caseKernelBody += indent + "extern __shared__ " + varState->baseTypeStr + " " + red->arrayName + "_block[];\n";
            }
            caseKernelBody += "\n";
        }

        caseKernelBody += indent + "/* Computation parameters */\n";
        caseKernelBody += indent + indexT + " " + coords + "[" + toStr( loopRank ) + "];\n";
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseKernelBody += indent + indexT + " id_" + toStr( letter ) + " = blockIdx." + toStr( letter ) +
                    " * blockDim." + toStr( letter ) + " + threadIdx." + toStr( letter ) + ";\n";
        }
        caseKernelBody += "\n";

        caseKernelBody += indent + "/* Execute one iteration */\n";
        caseKernelBody += indent + "if (id_" + toStr( cuda_letter_second ) + " < max_z";

        if ( n_cuda_dims == 3 ) {
            caseKernelBody += " && id_" + toStr( cuda_letter_third ) + " < ";
            for ( int i = 3; i < loopRank; ++i ) {
                if ( i == 3 ) {
                    caseKernelBody += "num_elem_" + toStr( i );
                } else {
                    caseKernelBody += " * num_elem_" + toStr( i );
                }
            }
        }

        caseKernelBody += ") {\n";
        indent += indentStep;

        caseKernelBody += indent + "if (id_" + toStr( cuda_letter_second ) + " + SE < Emin) {\n";
        indent += indentStep;

        caseKernelBody += indent + loopVars[ 0 ].name + " = id_" + toStr( cuda_letter_second ) + " + SE;\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "} else {\n";
        indent += indentStep;

        caseKernelBody += indent + "if (id_" + toStr( cuda_letter_second ) + " + SE < Emax) {\n";
        indent += indentStep;

        caseKernelBody += indent + loopVars[ 0 ].name + " = min_01;\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "} else {\n";
        indent += indentStep;

        caseKernelBody += indent + loopVars[ 0 ].name + " = 2 * min_01 - SE - id_" + toStr( cuda_letter_second ) + " + Emax - Emin - 1;\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";

//        indent = subtractIndent( indent );
//        caseKernelBody += indent + "}\n";

        caseKernelBody += indent + "if (id_" + toStr( cuda_letter_first ) + " < " + loopVars[ 0 ].name + ") {\n";
        indent += indentStep;

        caseKernelBody += indent + "if (var3 == 1 && Emin < id_" + toStr( cuda_letter_second ) + " + SE) {\n";
        indent += indentStep;

        caseKernelBody += indent + "base_0 = base_0 - step_0 * (SE + id_" + toStr( cuda_letter_second ) + " - Emin);\n";
        caseKernelBody += indent + "base_1 = base_1 + step_1 * (SE + id_" + toStr( cuda_letter_second ) + " - Emin);\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";

        caseKernelBody += indent + coords + "[idxs_0] = base_0 + (id_" + toStr( cuda_letter_second ) + " * (var1 + var3) - id_" + toStr( cuda_letter_first ) + ") * step_0;\n";
        caseKernelBody += indent + coords + "[idxs_1] = base_1 + (id_" + toStr( cuda_letter_second ) + " * var2 + id_" + toStr( cuda_letter_first ) + ") * step_1;\n";
        caseKernelBody += indent + coords + "[idxs_2] = base_2 - id_" + toStr( cuda_letter_second ) + " * step_2;\n";
        for ( int i = 3; i < dep_number; ++i ) {
            caseKernelBody += indent + coords + "[idxs_" + toStr( i ) + "] = base_" + toStr( i ) + ";\n";
        }

        for ( int i = dep_number; i < loopRank; ++i ) {
            std::string product;
            for ( int j = i + 1; j < loopRank; ++j ) {
                if ( j == i + 1 ) {
                    product += "num_elem_" + toStr( j );
                } else {
                    product += " * num_elem_" + toStr( j );
                }
            }

            std::string product_division = product.length() == 0 ? "" : " / (" + product + ")";

            char letter = cuda_letter_third;
            std::string expression;

            if ( i == 3 ) {
                expression = "id_" + toStr( letter ) + product_division;
            } else if ( i > 3 && i < loopRank - 1 ) {
                expression = "(id_" + toStr( letter ) + product_division + ") % num_elem_" + toStr( i );
            } else if ( i == loopRank - 1 ) {
                expression = "id_" + toStr( letter ) + " % num_elem_" + toStr( i );
            }

            caseKernelBody += indent + coords + "[idxs_" + toStr( i ) + "] = base_" +
                    toStr( i ) + " + (" + expression + ") * step_" + toStr( i ) + ";\n";
        }

        caseKernelBody += indent + "if (swap_01 * var3) {\n";
        indent += indentStep;

        caseKernelBody += indent + "var3 = " + coords + "[idxs_1];\n";
        caseKernelBody += indent + coords + "[idxs_1] = " + coords + "[idxs_0];\n";
        caseKernelBody += indent + coords + "[idxs_0] = var3;\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";

        for ( int i = 0; i < ( int )loopVars.size(); ++i ) {
            std::string varName = loopVars[ i ].name;
            caseKernelBody += indent + varName + " = " + coords + "[" + toStr( i ) + "];\n";
        }

        // insert loop body
        std::string loopBody = convertToString(parLoopBodyStmt);
        if ( !isa< CompoundStmt >( parLoopBodyStmt ) ) {
            caseKernelBody += indent + "{\n";
            indent += indentStep;
        }
        int lastPos = 0;
        while (loopBody.find('\n', lastPos) != std::string::npos) {
            int nlPos = loopBody.find('\n', lastPos);
            caseKernelBody += indent + loopBody.substr(lastPos, nlPos - lastPos + 1);
            lastPos = nlPos + 1;
            if (lastPos >= (int)loopBody.size())
                break;
        }
        if (lastPos < (int)loopBody.size())
            caseKernelBody += indent + loopBody.substr(lastPos) + "\n";
        if (!isFull(parLoopBodyStmt)) {
            caseKernelBody[caseKernelBody.size() - 1] = ';';
            caseKernelBody += "\n";
        }
        if (!isa<CompoundStmt>(parLoopBodyStmt)) {
            indent = subtractIndent( indent );
            caseKernelBody += indent + "}\n";
        }
        caseKernelBody += "\n";
        // finish inserting

        // start reduction
        if ( reductions.size() > 0 ) {
            if ( loopRank == dep_number ) {
                caseKernelBody += indent + "/* Reduction */\n";
                for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                    // TODO: Add support for reduction arrays
                    ClauseReduction *red = &curPragma->reductions[i];
                    std::string redType = red->redType;
                    std::string epsGrid = redGrid[red->arrayName];
                    VarDecl *vd = seekVarDecl(red->arrayName);
                    checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                    bool modefun = redType == "rf_MIN" || redType == "rf_MAX";
                    std::string op;
                    if ( redType == "rf_SUM" ) {
                        op = "+";
                    } else if ( redType == "rf_PRODUCT" ) {
                        op = "*";
                    } else if ( redType == "rf_MAX" ) {
                        op = "max";
                    } else if ( redType == "rf_MIN" ) {
                        op = "min";
                    } else if ( redType == "rf_AND" ) {
                        op = "&&";
                    } else if ( redType == "rf_OR" ) {
                        op = "||";
                    } else if ( redType == "rf_EQV" ) {
                        op = "==";
                    } else if ( redType == "rf_NEQV" ) {
                        op = "!=";
                    }

                    std::string index_expr = "[id_" + toStr( cuda_letter_first ) + " + " + "id_" + toStr( cuda_letter_second ) + " * Emin" + "]";
                    if ( modefun ) {
                        caseKernelBody += indent + epsGrid + index_expr + " = " + op + "(" + epsGrid + index_expr + ", " + red->arrayName + ");\n";
                    } else {
                        caseKernelBody += indent + epsGrid + index_expr + " = " + epsGrid + index_expr + " " + op + " " + red->arrayName + ";\n";
                    }
                }
            } else {
                caseKernelBody += indent + "/* Reduction */\n";
                caseKernelBody += indent + "id_" + toStr( cuda_letter_first ) + " = blockDim.x * blockDim.y * blockDim.z / 2;\n";
                caseKernelBody += indent + loopVars[ 0 ].name + " = threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * (blockDim.x * blockDim.y);\n";

                for (int i = 0; i < (int)curPragma->reductions.size(); i++) {
                    // TODO: Add support for reduction arrays
                    ClauseReduction *red = &curPragma->reductions[i];
                    std::string redType = red->redType;
                    std::string epsGrid = redGrid[red->arrayName];
                    VarDecl *vd = seekVarDecl(red->arrayName);
                    checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *varState = &varStates[vd];
                    checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                    bool modefun = redType == "rf_MIN" || redType == "rf_MAX";
                    std::string op;
                    if ( redType == "rf_SUM" ) {
                        op = "+";
                    } else if ( redType == "rf_PRODUCT" ) {
                        op = "*";
                    } else if ( redType == "rf_MAX" ) {
                        op = "max";
                    } else if ( redType == "rf_MIN" ) {
                        op = "min";
                    } else if ( redType == "rf_AND" ) {
                        op = "&&";
                    } else if ( redType == "rf_OR" ) {
                        op = "||";
                    } else if ( redType == "rf_EQV" ) {
                        op = "==";
                    } else if ( redType == "rf_NEQV" ) {
                        op = "!=";
                    }

                    caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " + red->arrayName + ";\n";
                    caseKernelBody += indent + "__syncthreads();\n";
                    caseKernelBody += indent + loopVars[ 1 ].name + " = id_" + toStr( cuda_letter_first ) + ";\n";
                    caseKernelBody += indent + "while (" + loopVars[ 1 ].name + " >= 1) {\n";
                    indent += indentStep;

                    caseKernelBody += indent + "__syncthreads();\n";
                    caseKernelBody += indent + "if (" + loopVars[ 0 ].name + " < " + loopVars[ 1 ].name + ") {\n";
                    indent += indentStep;

                    if ( modefun ) {
                        caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " +
                                op + "(" + red->arrayName + "_block[" + loopVars[ 0 ].name + "], " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + " + " + loopVars[ 1 ].name + "]);\n";
                    } else {
                        caseKernelBody += indent + red->arrayName + "_block[" + loopVars[ 0 ].name + "] = " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + "] " + op + " " +
                                red->arrayName + "_block[" + loopVars[ 0 ].name + " + " + loopVars[ 1 ].name + "];\n";
                    }

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";
                    caseKernelBody += indent + loopVars[ 1 ].name + " = " + loopVars[ 1 ].name + " / 2;\n";

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";

                    caseKernelBody += indent + "if (" + loopVars[ 0 ].name + " == 0) {\n";
                    indent += indentStep;

                    std::string index_expr = "blockIdx.x + (blockIdx.y + blockIdx.z * gridDim.y) * gridDim.x";
                    if ( modefun ) {
                        caseKernelBody += indent + epsGrid + "[" + index_expr + "] = " +
                                op + "(" + epsGrid + "[" + index_expr + "], " +
                                red->arrayName + "_block[0]);\n";
                    } else {
                        caseKernelBody += indent + epsGrid + "[" + index_expr + "] = " +
                                epsGrid + "[" + index_expr + "] " + op + " " +
                                red->arrayName + "_block[0];\n";
                    }

                    indent = subtractIndent( indent );
                    caseKernelBody += indent + "}\n";
                }
            }
        }

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseKernelBody += indent + "}\n";
    }

    kernelText += handlerTemplateDecl;
    kernelText += "__global__ void " + caseKernelName + "(" + caseKernelFormalParams + ") {\n";
    kernelText += caseKernelBody;
    kernelText += "}\n";
    kernelText += "\n";
}

void ConverterASTVisitor::genAcrossCudaCaseHandler(
    int dep_number, std::string baseHandlerName, std::string caseHandlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
    std::string handlerTemplateDecl, std::string handlerTemplateSpec, std::string &caseHandlerText , std::string &cudaInfoText )
{
    std::string indent = indentStep;
    std::string caseHandlerFormalParams;
    std::string caseHandlerBody;

    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);

    // generate kernels
    std::string caseKernelText;
    std::vector<KernelDesc> kernelsAvailable;
    kernelsAvailable.push_back( KernelDesc( caseHandlerName, "int" ) );
    kernelsAvailable.push_back( KernelDesc( caseHandlerName, "long long" ) );
    int case_number = ( 1 << dep_number ) - 1;
    kernelsAvailable.at( 0 ).kernelName = baseHandlerName + "_kernel_" + toStr( case_number ) + "_case_int";
    kernelsAvailable.at( 0 ).regsVar = kernelsAvailable.at( 0 ).kernelName + "_regs";
    kernelsAvailable.at( 1 ).kernelName = baseHandlerName + "_kernel_" + toStr( case_number ) + "_case_llong";
    kernelsAvailable.at( 1 ).regsVar = kernelsAvailable.at( 1 ).kernelName + "_regs";
    for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
        genAcrossCudaCaseKernel( dep_number, kernelsAvailable[ i ], outerParams, loopVars, handlerTemplateDecl, caseKernelText );
    }

    // get prohibited names
    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(parLoopBodyStmt);
    prohibitedNames = collectNamesVisitor.getNames();
    for (int i = 0; i < (int)outerParams.size(); i++)
        prohibitedNames.insert(outerParams[i]->getName().str());
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
        prohibitedNames.insert((*it)->getName().str());
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++)
        prohibitedNames.insert((*it)->getName().str());

    // get unique names
    std::string pLoopRef = getUniqueName( "pLoopRef", &prohibitedNames );
    std::string dependencyMask = getUniqueName( "dependencyMask", &prohibitedNames );
    std::string tmpVar = getUniqueName( "tmpVar", &prohibitedNames );
    std::string tmpV = getUniqueName( "tmpV", &prohibitedNames );
    std::string loop_ref = getUniqueName( "loop_ref", &prohibitedNames );
    std::string device_num = getUniqueName( "device_num", &prohibitedNames );
    std::string boundsLow = getUniqueName( "boundsLow", &prohibitedNames );
    std::string boundsHigh = getUniqueName( "boundsHigh", &prohibitedNames );
    std::string loopSteps = getUniqueName( "loopSteps", &prohibitedNames );
    std::string idxs = getUniqueName( "idxs", &prohibitedNames );
    std::string stream = getUniqueName( "stream", &prohibitedNames );
    std::string kernelIndexT = getUniqueName( "kernelIndexT", &prohibitedNames );
    std::string threads = getUniqueName( "threads", &prohibitedNames );
    std::string shared_mem = getUniqueName( "shared_mem", &prohibitedNames );
    std::string blocks = getUniqueName( "blocks", &prohibitedNames );
    std::string q = getUniqueName( "q", &prohibitedNames );
    std::string num_of_red_blocks = getUniqueName( "num_of_red_blocks", &prohibitedNames );
    std::string diag = getUniqueName( "diag", &prohibitedNames );
    std::string elem = getUniqueName( "elem", &prohibitedNames );

    std::string Allmin = getUniqueName( "Allmin", &prohibitedNames );
    std::string Emin = getUniqueName( "Emin", &prohibitedNames );
    std::string Emax = getUniqueName( "Emax", &prohibitedNames );
    std::string var1 = getUniqueName( "var1", &prohibitedNames );
    std::string var2 = getUniqueName( "var2", &prohibitedNames );
    std::string var3 = getUniqueName( "var3", &prohibitedNames );
    std::string SE = getUniqueName( "SE", &prohibitedNames );

    // get outer params
    std::map<std::string, std::string> dvmHeaders;
    std::map<std::string, std::string> dvmDevHeaders;
    std::map<std::string, std::string> scalarPtrs;
    std::map<std::string, std::vector<std::string> > dvmCoefs;
    std::map<std::string, std::string> redGrid;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            dvmHeaders[varState->name] = getUniqueName(varState->name + "_hdr", &prohibitedNames);
            dvmDevHeaders[varState->name] = getUniqueName(varState->name + "_devHdr", &prohibitedNames);
            dvmCoefs[varState->name].clear();
            for (int j = 0; j < varState->headerArraySize; j++)
                dvmCoefs[varState->name].push_back(getUniqueName(varState->name + "_hdr" + toStr(j), &prohibitedNames));
        } else {
            scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames);
        }
    }

    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++) {
        VarState *varState = &varStates[*it];
        redGrid[varState->name] = getUniqueName(varState->name + "_grid", &prohibitedNames);
    }

    // get formal params
    caseHandlerFormalParams += "DvmType* " + pLoopRef;
    for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
        VarState *varState = &varStates[ outerParams[ i ] ];
        std::string refName = varState->name;
        if ( varState->isArray ) {
            std::string hdrName = dvmHeaders[ refName ];
            caseHandlerFormalParams += ", DvmType " + hdrName + "[]";
        } else {
            caseHandlerFormalParams += ", " + varState->baseTypeStr + "* " + scalarPtrs[ refName ];
        }
    }
    caseHandlerFormalParams += ", DvmType " + dependencyMask;

    // get case handler body

    caseHandlerBody += indent + "/* "+ toStr( dep_number ) + " dependencies */\n";
    if ( dep_number == 1 ) {
//        int n_cuda_dims = 1 + ( loopRank >= dep_number + 1 ? 1 : 0 ) + ( loopRank > dep_number + 1 ? 1 : 0 );
        int n_cuda_dims = std::min( loopRank - dep_number, 3 );

        if ( !opts.autoTfm ) {
            caseHandlerBody += indent + "DvmType " + tmpVar + ";\n";
            caseHandlerBody += "\n";
        }

        caseHandlerBody += indent + "/* Loop references and device number */\n";
        caseHandlerBody += indent + "DvmType " + loop_ref + " = *" + pLoopRef + ";\n";
        caseHandlerBody += indent + "DvmType " + device_num + " = dvmh_loop_get_device_num_C(" + loop_ref + ");\n";
        caseHandlerBody += "\n";

        // handle arrays
        caseHandlerBody += indent + "/* Parameters */\n";

        std::string caseKernelFactParams_arrays;
        if ( opts.autoTfm ) {
            // autotransform case
            for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
                VarState *varState = &varStates[ outerParams[ i ] ];
                std::string refName = varState->name;
                if ( varState->isArray ) {
                    std::string arrName = varState->name;
                    std::string arrType = varState->baseTypeStr;
                    std::string hdrName = dvmHeaders[ refName ];
                    std::string devHdrName = dvmDevHeaders[ refName ];
                    std::string extendedParamsName = arrName + "_extendedParams";
                    std::string typeOfTransformName = arrName + "_typeOfTransform";

                    caseHandlerBody += indent + "dvmh_loop_autotransform_C(" + loop_ref + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + arrType + " *" + arrName + " = (" + arrType + "*)" +
                            "dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + "DvmType " + devHdrName + "[" + toStr( varState->headerArraySize ) + "];\n";
                    caseHandlerBody += indent + "DvmType " + extendedParamsName + "[7];\n";
                    caseHandlerBody += indent + "DvmType " + typeOfTransformName + " = dvmh_fill_header_C(" +
                            device_num + ", " + arrName + ", " + hdrName + ", " + devHdrName + ", " + extendedParamsName + ");\n";
                    caseHandlerBody += indent + "assert(" + typeOfTransformName + " == 0 || " +
                            typeOfTransformName + " == 1 || " +
                            typeOfTransformName + " == 2);\n";

                    caseHandlerBody += "\n";

                    // create kernel arrays fact parameters
                    caseKernelFactParams_arrays += ", " + arrName;
                    if ( varState->rank > 1 ) {
                        int iter_num = opts.autoTfm ? varState->rank : varState->rank - 1;
                        for ( int i = 1; i <= iter_num; ++i ) {
                            caseKernelFactParams_arrays += ", " + devHdrName + "[" + toStr( i ) + "]";
                        }
                    }
                    if ( opts.autoTfm && dep_number > 1 ) {
                        std::string extendedParamsName = arrName + "_extendedParams";
                        caseKernelFactParams_arrays += ", " + arrName + "_typeOfTransform";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[0]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[3]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[2]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[5]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[1]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[4]";
//                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[6]";
                    }
                }
            }
        } else {
            // usual case
            for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
                VarState *varState = &varStates[ outerParams[ i ] ];
                std::string refName = varState->name;
                if ( varState->isArray ) {
                    std::string arrName = varState->name;
                    std::string arrType = varState->baseTypeStr;
                    std::string hdrName = dvmHeaders[ refName ];
                    std::string devHdrName = dvmDevHeaders[ refName ];

                    caseHandlerBody += indent + "DvmType " + devHdrName + "[" + toStr( varState->headerArraySize ) + "];\n";
                    caseHandlerBody += indent + arrType + " *" + arrName + " = " +
                            "(" + arrType + "*)dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + tmpVar + " = dvmh_fill_header_C(" + device_num +
                            ", " + arrName + ", " + hdrName + ", " + devHdrName + ", 0);\n";
                    caseHandlerBody += indent + "assert(" + tmpVar + " == 0 || " + tmpVar + " == 1);\n";
                    caseHandlerBody += "\n";

                    // create kernel arrays fact parameters
                    caseKernelFactParams_arrays += ", " + arrName;
                    int iter_num = opts.autoTfm ? varState->rank : varState->rank - 1;
                    for ( int i = 1; i <= iter_num; ++i ) {
                        caseKernelFactParams_arrays += ", " + devHdrName + "[" + toStr( i ) + "]";
                    }
                }
            }
        }

        // handler scalars
        std::string caseKernelFactParams_scalars;
        for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
            VarState *varState = &varStates[ outerParams[ i ] ];
            std::string refName = varState->name;
            if ( !varState->isArray ) {
                std::string varType = varState->baseTypeStr;
                std::string ptrName = scalarPtrs[ refName ];

                caseHandlerBody += indent + varType + " *" + refName + " = (" + varType + "*)dvmh_get_device_addr_C(" +
                        device_num + ", " + ptrName + ");\n";

                // create scalars fact parameters
                caseKernelFactParams_scalars += ", " + refName;
            }
        }
        caseHandlerBody += "\n";

        // handle bounds and steps
        caseHandlerBody += indent + "/* Supplementary variables for loop handling */\n";
        caseHandlerBody += indent + "DvmType " + boundsLow + "[" + toStr( loopRank ) + "]";
        caseHandlerBody += ", " + boundsHigh + "[" + toStr( loopRank ) + "]";
        caseHandlerBody += ", " + loopSteps + "[" + toStr( loopRank ) + "];\n";
        caseHandlerBody += indent + "DvmType " + idxs + "[" + toStr( loopRank ) + "];\n";
        caseHandlerBody += indent + "cudaStream_t " + stream + ";\n";
        caseHandlerBody += "\n";

        std::string caseKernelFactParams_loopSteps;
        std::string caseKernelFactParams_idxs;
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFactParams_loopSteps += ", " + loopSteps + "[" + toStr( i ) + "]";
            caseKernelFactParams_idxs += ", " + idxs + "[" + toStr( i ) + "]";
        }

        // Choose index type for CUDA kernel
        caseHandlerBody += indent + "/* Choose index type for CUDA kernel */\n";
        caseHandlerBody += indent + "int " + kernelIndexT + " = dvmh_loop_guess_index_type_C(" + loop_ref + ");\n";
        caseHandlerBody += indent + "if (" + kernelIndexT + " == rt_LONG) " + kernelIndexT + " = (sizeof(long) <= sizeof(int) ? rt_INT : rt_LLONG);\n";
        caseHandlerBody += indent + "assert(" + kernelIndexT + " == rt_INT || " + kernelIndexT + " == rt_LLONG);\n";
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "/* Fill loop bounds */\n";
        caseHandlerBody += indent + "dvmh_loop_fill_bounds_C(" + loop_ref + ", " + boundsLow + ", " + boundsHigh + ", " + loopSteps + ");\n";
//        caseHandlerBody += indent + "printf( \"%d\\n\", dependencyMask );\n";
//        caseHandlerBody += indent + "printf( \"%d %d %d\\n\", boundsHigh[ 0 ], boundsHigh[ 1 ], boundsHigh[ 2 ] );\n";
        caseHandlerBody += indent + "dvmh_change_filled_bounds2_C(" + boundsLow + ", " + boundsHigh + ", " + loopSteps + ", " + toStr( loopRank ) + ", " +
//                toStr( dep_number ) + ", " +
                dependencyMask + ", " + idxs + ");\n";
//        caseHandlerBody += indent + "printf( \"%d %d %d\\n\", boundsHigh[ 0 ], boundsHigh[ 1 ], boundsHigh[ 2 ] );\n";
//        caseHandlerBody += indent + "printf( \"%d %d %d\\n\", idxs[ 0 ], idxs[ 1 ], idxs[ 2 ] );\n";
        caseHandlerBody += "\n";

        // Get CUDA configuration parameters
        caseHandlerBody += indent + "/* Get CUDA configuration parameters */\n";
        int threads_conf_x = 0;
        int threads_conf_y = 0;
        int threads_conf_z = 0;
        getDefaultCudaBlock(threads_conf_x, threads_conf_y, threads_conf_z, dep_number, loopRank - dep_number, opts.autoTfm);
        std::string threads_conf =
                toStr( threads_conf_x ) + ", " +
                toStr( threads_conf_y ) + ", " +
                toStr( threads_conf_z );

        caseHandlerBody += indent + "dim3 " + threads + " = dim3(" + threads_conf + ");\n";
//        caseHandlerBody += indent + "dim3 " + threads + " = dim3(0, 0, 0);\n";
        int shared_mem_num = loopRank > dep_number ? 8 * ( int )curPragma->reductions.size() : 0;
        caseHandlerBody += indent + "DvmType " + shared_mem + " = " + toStr( shared_mem_num ) + ";\n";
        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string regsVar = kernelsAvailable[ i ].regsVar;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;
            cudaInfoText += "#ifdef " + toUpper(regsVar) + "\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = " + toUpper(regsVar) + ";\n";
            cudaInfoText += "#else\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = 0;\n";
            cudaInfoText += "#endif\n";
            caseHandlerBody += indent + "extern DvmType " + regsVar + ";\n";
            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") dvmh_loop_cuda_get_config_C(" + loop_ref + ", " + shared_mem + ", " +
                    regsVar + ", &" +
                    threads + ", &" + stream + ", &" + shared_mem + ");\n";
        }
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "/* Calculate computation distribution parameters */\n";
        // allocating cuda threads
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseHandlerBody += indent + "DvmType num_" + letter + " = threads." + toStr( letter ) + ";\n";
        }
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "dim3 " + blocks + " = dim3(1, 1, 1);\n";
        std::string caseKernelFactParams_base;
        std::string caseKernelFactParams_num_elem_indep;
        for ( int i = 0; i < loopRank; ++i ) {
            caseHandlerBody += indent + "int " + "base_" + toStr( i ) + " = " + boundsLow + "[" + toStr( i ) + "];\n";
            caseKernelFactParams_base += ", base_" + toStr( i );
            if ( i >= 1 ) {
                char letter = 'x' + std::min( i - 1, 2 );
                std::string letter_suffix = toStr( letter ) + ( i > 2 ? "_" + toStr( i - 3 ) : "" );
                std::string boundsLow_i = boundsLow + "[" + toStr( i ) + "]";
                std::string boundsHigh_i = boundsHigh + "[" + toStr( i ) + "]";
                std::string loopSteps_i = loopSteps + "[" + toStr( i ) + "]";
                caseHandlerBody += indent + "int " + "num_elem_" + letter_suffix + " = " +
                        "(abs(" + boundsLow_i + " - " + boundsHigh_i + ")" + " + " +
                        "abs(" + loopSteps_i + "))" + " / " + "abs(" + loopSteps_i + ")" +
                        ";\n";
                caseKernelFactParams_num_elem_indep += ", num_elem_" + letter_suffix;
            }
            if ( i == 1 || i == 2 ) {
                char letter = 'x' + std::min( i - 1, 2 );
                caseHandlerBody += indent + blocks + "." + toStr( letter ) + " = " + "(" +
                        "num_elem_" + toStr( letter ) + " + " + "num_" + toStr( letter ) + " - 1" + ")" +
                        " / " + "num_" + toStr( letter ) +
                        ";\n";
                caseHandlerBody += indent + threads + "." + toStr( letter ) + " = " + "num_" + toStr( letter ) + ";\n";
            }
        }
        if ( loopRank >= 4 ) {
            char letter = 'z';
            caseHandlerBody += indent + "int " + "num_elem_" + toStr( letter ) + " = ";
            int zsize = loopRank - 3;
            for ( int i = 0; i < zsize; ++i ) {
                if ( i != 0 ) {
                    caseHandlerBody += " * ";
                }
                caseHandlerBody += "num_elem_" + toStr( letter ) + "_" + toStr( i );
            }
            caseHandlerBody += ";\n";
            caseHandlerBody += indent + blocks + "." + toStr( letter ) + " = " + "(" +
                    "num_elem_" + toStr( letter ) + " + " + "num_" + toStr( letter ) + " - 1" + ")" +
                    " / " + "num_" + toStr( letter ) +
                    ";\n";
            caseHandlerBody += indent + threads + "." + toStr( letter ) + " = " + "num_" + toStr( letter ) + ";\n";
        }
        {
            std::string boundsLow_0 = boundsLow + "[0]";
            std::string boundsHigh_0 = boundsHigh + "[0]";
            std::string loopSteps_0 = loopSteps + "[0]";
            caseHandlerBody += indent + boundsHigh_0 + " = " +
                    "(abs(" + boundsLow_0 + " - " + boundsHigh_0 + ")" + " + " +
                    "abs(" + loopSteps_0 + "))" + " / " + "abs(" + loopSteps_0 + ")" +
                    ";\n";
        }
        caseHandlerBody += "\n";

        // start reduction
        std::string caseKernelFactParams_reduction;
        if ( curPragma->reductions.size() > 0 ) {
            caseHandlerBody += indent + "/* Reductions-related stuff */\n";
            caseHandlerBody += indent + "DvmType " + num_of_red_blocks + " = " + "blocks.x * blocks.y * blocks.z" + ";\n";

            for ( int i = 0; i < ( int )curPragma->reductions.size(); ++i ) {
                ClauseReduction *red = &curPragma->reductions[ i ];
                std::string epsGrid = redGrid[red->arrayName];

                VarDecl *vd = seekVarDecl(red->arrayName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                caseHandlerBody += indent + varState->baseTypeStr + " " + red->arrayName + ";\n";
                caseHandlerBody += indent + varState->baseTypeStr + " *" + epsGrid + ";\n";

                caseKernelFactParams_reduction += ", " + red->arrayName + ", " + epsGrid;

                if (red->isLoc()) {
                    std::string locGrid = redGrid[red->locName];
                    VarDecl *lvd = seekVarDecl(red->locName);
                    checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *locVarState = &varStates[lvd];
                    checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());
                    caseHandlerBody += indent + locVarState->baseTypeStr + " " + red->locName + ";\n";
                    caseHandlerBody += indent + locVarState->baseTypeStr + " *" + locGrid + ";\n";

                    caseHandlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", (void **)&" +
                            locGrid + ");\n";
                    caseHandlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
                } else {
                    caseHandlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", 0);\n";
                    caseHandlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
                }
                caseHandlerBody += indent + "dvmh_loop_cuda_red_prepare_C(" + loop_ref + ", " + toStr(i + 1) + ", " + num_of_red_blocks + ", 1);\n";
            }
            caseHandlerBody += "\n";
        }

        caseHandlerBody += indent + "for ( int " + tmpV + " = 0; " + tmpV + " < " + boundsHigh + "[0]; " +
                "base_0 = base_0 + " + loopSteps + "[0], " + tmpV + " = " + tmpV + " + 1) {\n";

        indent += indentStep;

        // generate kernel call
        std::string caseKernelFactParams = "";

        // pass parameters
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        // end reduction
        if ( curPragma->reductions.size() > 0 ) {
            caseHandlerBody += indent + "/* Finish reduction */\n";
            for ( int i = 0; i < ( int )curPragma->reductions.size(); ++i ) {
                caseHandlerBody += indent + "dvmh_loop_cuda_red_finish_C(" + loop_ref + ", " + toStr( i + 1 ) + ");\n";
            }
        }
    } else if ( dep_number == 2 ) {
        int n_cuda_dims = 1 + ( loopRank >= dep_number + 1 ? 1 : 0 ) + ( loopRank > dep_number + 1 ? 1 : 0 );

        if ( !opts.autoTfm ) {
            caseHandlerBody += indent + "DvmType " + tmpVar + ";\n";
            caseHandlerBody += "\n";
        }

        caseHandlerBody += indent + "/* Loop references and device number */\n";
        caseHandlerBody += indent + "DvmType " + loop_ref + " = *" + pLoopRef + ";\n";
        caseHandlerBody += indent + "DvmType " + device_num + " = dvmh_loop_get_device_num_C(" + loop_ref + ");\n";
        caseHandlerBody += "\n";

        // handle arrays
        caseHandlerBody += indent + "/* Parameters */\n";

        std::string caseKernelFactParams_arrays;
        if ( opts.autoTfm ) {
            // autotransform case
            for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
                VarState *varState = &varStates[ outerParams[ i ] ];
                std::string refName = varState->name;
                if ( varState->isArray ) {
                    std::string arrName = varState->name;
                    std::string arrType = varState->baseTypeStr;
                    std::string hdrName = dvmHeaders[ refName ];
                    std::string devHdrName = dvmDevHeaders[ refName ];
                    std::string extendedParamsName = arrName + "_extendedParams";
                    std::string typeOfTransformName = arrName + "_typeOfTransform";

                    caseHandlerBody += indent + "dvmh_loop_autotransform_C(" + loop_ref + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + arrType + " *" + arrName + " = (" + arrType + "*)" +
                            "dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + "DvmType " + devHdrName + "[" + toStr( varState->headerArraySize ) + "];\n";
                    caseHandlerBody += indent + "DvmType " + extendedParamsName + "[7];\n";
                    caseHandlerBody += indent + "DvmType " + typeOfTransformName + " = dvmh_fill_header_C(" +
                            device_num + ", " + arrName + ", " + hdrName + ", " + devHdrName + ", " + extendedParamsName + ");\n";
                    caseHandlerBody += indent + "assert(" + typeOfTransformName + " == 0 || " +
                            typeOfTransformName + " == 1 || " +
                            typeOfTransformName + " == 2);\n";

                    caseHandlerBody += "\n";

                    // create kernel arrays fact parameters
                    caseKernelFactParams_arrays += ", " + arrName;
                    int iter_num = opts.autoTfm ? varState->rank : varState->rank - 1;
                    for ( int i = 1; i <= iter_num; ++i ) {
                        caseKernelFactParams_arrays += ", " + devHdrName + "[" + toStr( i ) + "]";
                    }
                    if ( opts.autoTfm && dep_number > 1 ) {
                        std::string extendedParamsName = arrName + "_extendedParams";
                        caseKernelFactParams_arrays += ", " + arrName + "_typeOfTransform";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[0]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[3]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[2]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[5]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[1]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[4]";
//                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[6]";
                    }
                }
            }
        } else {
            // usual case
            for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
                VarState *varState = &varStates[ outerParams[ i ] ];
                std::string refName = varState->name;
                if ( varState->isArray ) {
                    std::string arrName = varState->name;
                    std::string arrType = varState->baseTypeStr;
                    std::string hdrName = dvmHeaders[ refName ];
                    std::string devHdrName = dvmDevHeaders[ refName ];

                    caseHandlerBody += indent + "DvmType " + devHdrName + "[" + toStr( varState->headerArraySize ) + "];\n";
                    caseHandlerBody += indent + arrType + " *" + arrName + " = " +
                            "(" + arrType + "*)dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + tmpVar + " = dvmh_fill_header_C(" + device_num +
                            ", " + arrName + ", " + hdrName + ", " + devHdrName + ", 0);\n";
                    caseHandlerBody += indent + "assert(" + tmpVar + " == 0 || " + tmpVar + " == 1);\n";
                    caseHandlerBody += "\n";

                    // create kernel arrays fact parameters
                    caseKernelFactParams_arrays += ", " + arrName;
                    int iter_num = opts.autoTfm ? varState->rank : varState->rank - 1;
                    for ( int i = 1; i <= iter_num; ++i ) {
                        caseKernelFactParams_arrays += ", " + devHdrName + "[" + toStr( i ) + "]";
                    }
                }
            }
        }

        // handler scalars
        std::string caseKernelFactParams_scalars;
        for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
            VarState *varState = &varStates[ outerParams[ i ] ];
            std::string refName = varState->name;
            if ( !varState->isArray ) {
                std::string varType = varState->baseTypeStr;
                std::string ptrName = scalarPtrs[ refName ];

                caseHandlerBody += indent + varType + " *" + refName + " = (" + varType + "*)dvmh_get_device_addr_C(" +
                        device_num + ", " + ptrName + ");\n";

                // create scalars fact parameters
                caseKernelFactParams_scalars += ", " + refName;
            }
        }
        caseHandlerBody += "\n";

        // handle bounds and steps
        caseHandlerBody += indent + "/* Supplementary variables for loop handling */\n";
        caseHandlerBody += indent + "DvmType " + boundsLow + "[" + toStr( loopRank ) + "]";
        caseHandlerBody += ", " + boundsHigh + "[" + toStr( loopRank ) + "]";
        caseHandlerBody += ", " + loopSteps + "[" + toStr( loopRank ) + "];\n";
        caseHandlerBody += indent + "DvmType " + idxs + "[" + toStr( loopRank ) + "];\n";
        caseHandlerBody += indent + "cudaStream_t " + stream + ";\n";
        caseHandlerBody += "\n";

        std::string caseKernelFactParams_loopSteps;
        std::string caseKernelFactParams_idxs;
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFactParams_loopSteps += ", " + loopSteps + "[" + toStr( i ) + "]";
            caseKernelFactParams_idxs += ", " + idxs + "[" + toStr( i ) + "]";
        }

        // Choose index type for CUDA kernel
        caseHandlerBody += indent + "/* Choose index type for CUDA kernel */\n";
        caseHandlerBody += indent + "int " + kernelIndexT + " = dvmh_loop_guess_index_type_C(" + loop_ref + ");\n";
        caseHandlerBody += indent + "if (" + kernelIndexT + " == rt_LONG) " + kernelIndexT + " = (sizeof(long) <= sizeof(int) ? rt_INT : rt_LLONG);\n";
        caseHandlerBody += indent + "assert(" + kernelIndexT + " == rt_INT || " + kernelIndexT + " == rt_LLONG);\n";
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "/* Fill loop bounds */\n";
        caseHandlerBody += indent + "dvmh_loop_fill_bounds_C(" + loop_ref + ", " + boundsLow + ", " + boundsHigh + ", " + loopSteps + ");\n";
        caseHandlerBody += indent + "dvmh_change_filled_bounds2_C(" + boundsLow + ", " + boundsHigh + ", " + loopSteps + ", " + toStr( loopRank ) + ", " +
                dependencyMask + ", " + idxs + ");\n";
        caseHandlerBody += "\n";

        // Get CUDA configuration parameters
        caseHandlerBody += indent + "/* Get CUDA configuration parameters */\n";
        int threads_conf_x = 0;
        int threads_conf_y = 0;
        int threads_conf_z = 0;
        getDefaultCudaBlock(threads_conf_x, threads_conf_y, threads_conf_z, dep_number, loopRank - dep_number, opts.autoTfm);
        std::string threads_conf =
                toStr( threads_conf_x ) + ", " +
                toStr( threads_conf_y ) + ", " +
                toStr( threads_conf_z );

        caseHandlerBody += indent + "dim3 " + threads + " = dim3(" + threads_conf + ");\n";
//        caseHandlerBody += indent + "dim3 " + threads + " = dim3(0, 0, 0);\n";
        int shared_mem_num = loopRank > dep_number ? 8 * ( int )curPragma->reductions.size() : 0;
        caseHandlerBody += indent + "DvmType " + shared_mem + " = " + toStr( shared_mem_num ) + ";\n";
        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string regsVar = kernelsAvailable[ i ].regsVar;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;
            cudaInfoText += "#ifdef " + toUpper(regsVar) + "\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = " + toUpper(regsVar) + ";\n";
            cudaInfoText += "#else\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = 0;\n";
            cudaInfoText += "#endif\n";
            caseHandlerBody += indent + "extern DvmType " + regsVar + ";\n";
            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") dvmh_loop_cuda_get_config_C(" + loop_ref + ", " + shared_mem + ", " +
                    regsVar + ", &" +
                    threads + ", &" + stream + ", &" + shared_mem + ");\n";
        }
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "/* Calculate computation distribution parameters */\n";
        // allocating cuda threads
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseHandlerBody += indent + "DvmType num_" + letter + " = threads." + toStr( letter ) + ";\n";
        }
        caseHandlerBody += "\n";

        // dependent dims
        for ( int i = 0; i < dep_number; ++i ) {
            caseHandlerBody += indent + "int M" + toStr( i ) + " = (" + boundsHigh + "[" + toStr( i ) + "] - " +
                    boundsLow + "[" + toStr( i ) + "]) / " + loopSteps + "[" + toStr( i ) + "] + 1;\n";
        }

        // independent dims
        std::string caseKernelFactParams_num_elem_indep;
        for ( int i = dep_number; i < loopRank; ++i ) {
            caseHandlerBody += indent + "DvmType num_elem_" + toStr( i ) + " = (" + boundsHigh + "[" + toStr( i ) + "] - " +
                    boundsLow + "[" + toStr( i ) + "]) / " + loopSteps + "[" + toStr( i ) + "] + 1;\n";
            caseKernelFactParams_num_elem_indep += ", num_elem_" + toStr( i );
        }

        if ( n_cuda_dims >= 2 ) {
            caseHandlerBody += indent + "DvmType num_elem_y = num_elem_2;\n";
        }

        if ( n_cuda_dims == 3 ) {
            caseHandlerBody += indent + "DvmType num_elem_z = ";
            for ( int i = 3; i < loopRank; ++i ) {
                if ( i != 3 ) {
                    caseHandlerBody += ", ";
                }
                caseHandlerBody += "num_elem_" + toStr( i );
            }
            caseHandlerBody += ";\n";
        }
        caseHandlerBody += "\n";

        // determine blocks
        caseHandlerBody += indent + "dim3 " + blocks + " = dim3(";
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            if ( i != 0 ) {
                caseHandlerBody += ", ";
            }
            caseHandlerBody += "num_" + toStr( letter );
        }
        caseHandlerBody += ");\n";

        for ( int i = 1; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseHandlerBody += indent + "blocks." + toStr( letter ) + " = (num_elem_" + toStr( letter ) +
                    " + num_" + toStr( letter ) + " - 1) / num_" + toStr( letter ) + ";\n";
        }
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "DvmType " + q + " = min(M0, M1);\n";
        caseHandlerBody += "\n";

        // start reduction
        std::string caseKernelFactParams_reduction;
        if ( curPragma->reductions.size() > 0 ) {
            caseHandlerBody += indent + "/* Reductions-related stuff */\n";
            if ( loopRank == dep_number ) {
                caseHandlerBody += indent + "DvmType " + num_of_red_blocks + " = " + q + ";\n";
            } else if ( n_cuda_dims == 2 ) {
                caseHandlerBody += indent + "DvmType " + num_of_red_blocks + " = ((" + q + " + num_y - 1) / num_y) * blocks.y;\n";
            } else if ( n_cuda_dims == 3 ) {
                caseHandlerBody += indent + "DvmType " + num_of_red_blocks + " = ((" + q + " + num_y - 1) / num_y) * blocks.y * blocks.z;\n";
            }

            for ( int i = 0; i < ( int )curPragma->reductions.size(); ++i ) {
                ClauseReduction *red = &curPragma->reductions[ i ];
                std::string epsGrid = redGrid[red->arrayName];

                VarDecl *vd = seekVarDecl(red->arrayName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                caseHandlerBody += indent + varState->baseTypeStr + " " + red->arrayName + ";\n";
                caseHandlerBody += indent + varState->baseTypeStr + " *" + epsGrid + ";\n";

                caseKernelFactParams_reduction += ", " + red->arrayName + ", " + epsGrid;

                if (red->isLoc()) {
                    std::string locGrid = redGrid[red->locName];
                    VarDecl *lvd = seekVarDecl(red->locName);
                    checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *locVarState = &varStates[lvd];
                    checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());
                    caseHandlerBody += indent + locVarState->baseTypeStr + " " + red->locName + ";\n";
                    caseHandlerBody += indent + locVarState->baseTypeStr + " *" + locGrid + ";\n";

                    caseHandlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", (void **)&" +
                            locGrid + ");\n";
                    caseHandlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
                } else {
                    caseHandlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", 0);\n";
                    caseHandlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
                }
                caseHandlerBody += indent + "dvmh_loop_cuda_red_prepare_C(" + loop_ref + ", " + toStr(i + 1) + ", " + num_of_red_blocks + ", 1);\n";
            }
            caseHandlerBody += "\n";
        }

        std::string caseKernelFactParams_base;
        for ( int i = 0; i < loopRank; ++i ) {
            caseHandlerBody += indent + "int " + "base_" + toStr( i ) + " = " + boundsLow + "[" + toStr( i ) + "];\n";
            caseKernelFactParams_base += ", base_" + toStr( i );
        }
        caseHandlerBody += "\n";

        // GPU execution
        caseHandlerBody += indent + "/* GPU execution */\n";

        // first part, first loop
        caseHandlerBody += indent + "int " + diag + " = 1;\n";
        caseHandlerBody += indent + "while (" + diag + " <= " + q + ") {\n";

        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + diag + " + num_x - 1) / num_x;\n";

        // generate kernel call
        std::string caseKernelFactParams = "";

        // pass parameters
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += ", " + diag;
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_0 = base_0 + " + loopSteps + "[0];\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";

        indent = subtractIndent( indent );

        caseHandlerBody += indent + "}\n";
        caseHandlerBody += "\n";

        // second part
        caseHandlerBody += indent + "int " + elem + ";\n";
        caseHandlerBody += indent + "if (M0 < M1) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_0 = base_0 - " + loopSteps + "[0];\n";
        caseHandlerBody += indent + "base_1 = " + boundsLow + "[1] + " + loopSteps + "[1];\n";
        caseHandlerBody += indent + diag + " = 0;\n";

        // second part, first loop
        caseHandlerBody += indent + "while (" + diag + " < M1 - M0) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + q + " + num_x - 1) / num_x;\n";

        caseKernelFactParams = "";
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += ", " + q;
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1];\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        caseHandlerBody += indent + diag + " = " + q + " + (M1 - M0) + 1;\n";
        caseHandlerBody += indent + elem + " = " + q + " - 1;\n";

        // second part, second loop
        caseHandlerBody += indent + "while (" + diag + " < M0 + M1) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + elem + " + num_x - 1) / num_x;\n";

        caseKernelFactParams = "";
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += ", " + elem;
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );
        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1];\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";
        caseHandlerBody += indent + elem + " = " + elem + " - 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "} else {\n";
        indent += indentStep;

        // third part, first loop

        caseHandlerBody += indent + diag + " = 0;\n";
        caseHandlerBody += indent + "while (" + diag + " < M0 - M1) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + q + " + num_x - 1) / num_x;\n";

        caseKernelFactParams = "";
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += ", " + q;
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );
        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_0 = base_0 + " + loopSteps + "[0];\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        caseHandlerBody += indent + "base_0 = base_0 - " + loopSteps + "[0];\n";
        caseHandlerBody += indent + "base_1 = " + boundsLow + "[1] + " + loopSteps + "[1];\n";
        caseHandlerBody += indent + diag + " = " + q + " + (M0 - M1) + 1;\n";
        caseHandlerBody += indent + elem + " = " + q + " - 1;\n";

        // third part, second loop

        caseHandlerBody += indent + "while (" + diag + " < M0 + M1) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + elem + " + num_x - 1) / num_x;\n";

        caseKernelFactParams = "";
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += ", " + elem;
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );
        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1];\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";
        caseHandlerBody += indent + elem + " = " + elem + " - 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";
        caseHandlerBody += "\n";

        // end reduction
        if ( curPragma->reductions.size() > 0 ) {
            caseHandlerBody += indent + "/* Finish reduction */\n";
            for ( int i = 0; i < ( int )curPragma->reductions.size(); ++i ) {
                caseHandlerBody += indent + "dvmh_loop_cuda_red_finish_C(" + loop_ref + ", " + toStr( i + 1 ) + ");\n";
            }
        }
    } else if ( dep_number >= 3 ) {
        int n_cuda_dims = 2 + ( loopRank > dep_number + 1 ? 1 : 0 );

        if ( !opts.autoTfm ) {
            caseHandlerBody += indent + "DvmType " + tmpVar + ";\n";
            caseHandlerBody += "\n";
        }

        caseHandlerBody += indent + "/* Loop references and device number */\n";
        caseHandlerBody += indent + "DvmType " + loop_ref + " = *" + pLoopRef + ";\n";
        caseHandlerBody += indent + "DvmType " + device_num + " = dvmh_loop_get_device_num_C(" + loop_ref + ");\n";
        caseHandlerBody += "\n";

        // handle arrays
        caseHandlerBody += indent + "/* Parameters */\n";

        std::string caseKernelFactParams_arrays;
        if ( opts.autoTfm ) {
            // autotransform case
            for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
                VarState *varState = &varStates[ outerParams[ i ] ];
                std::string refName = varState->name;
                if ( varState->isArray ) {
                    std::string arrName = varState->name;
                    std::string arrType = varState->baseTypeStr;
                    std::string hdrName = dvmHeaders[ refName ];
                    std::string devHdrName = dvmDevHeaders[ refName ];
                    std::string extendedParamsName = arrName + "_extendedParams";
                    std::string typeOfTransformName = arrName + "_typeOfTransform";

                    caseHandlerBody += indent + "dvmh_loop_autotransform_C(" + loop_ref + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + arrType + " *" + arrName + " = (" + arrType + "*)" +
                            "dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + "DvmType " + devHdrName + "[" + toStr( varState->headerArraySize ) + "];\n";
                    caseHandlerBody += indent + "DvmType " + extendedParamsName + "[7];\n";
                    caseHandlerBody += indent + "DvmType " + typeOfTransformName + " = dvmh_fill_header_C(" +
                            device_num + ", " + arrName + ", " + hdrName + ", " + devHdrName + ", " + extendedParamsName + ");\n";
                    caseHandlerBody += indent + "assert(" + typeOfTransformName + " == 0 || " +
                            typeOfTransformName + " == 1 || " +
                            typeOfTransformName + " == 2);\n";

                    caseHandlerBody += "\n";

                    // create kernel arrays fact parameters
                    caseKernelFactParams_arrays += ", " + arrName;
                    int iter_num = opts.autoTfm ? varState->rank : varState->rank - 1;
                    for ( int i = 1; i <= iter_num; ++i ) {
                        caseKernelFactParams_arrays += ", " + devHdrName + "[" + toStr( i ) + "]";
                    }
                    if ( opts.autoTfm && dep_number > 1 ) {
                        std::string extendedParamsName = arrName + "_extendedParams";
                        caseKernelFactParams_arrays += ", " + arrName + "_typeOfTransform";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[0]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[3]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[2]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[5]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[1]";
                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[4]";
//                        caseKernelFactParams_arrays += ", " + extendedParamsName + "[6]";
                    }
                }
            }
        } else {
            // usual case
            for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
                VarState *varState = &varStates[ outerParams[ i ] ];
                std::string refName = varState->name;
                if ( varState->isArray ) {
                    std::string arrName = varState->name;
                    std::string arrType = varState->baseTypeStr;
                    std::string hdrName = dvmHeaders[ refName ];
                    std::string devHdrName = dvmDevHeaders[ refName ];

                    caseHandlerBody += indent + "DvmType " + devHdrName + "[" + toStr( varState->headerArraySize ) + "];\n";
                    caseHandlerBody += indent + arrType + " *" + arrName + " = " +
                            "(" + arrType + "*)dvmh_get_natural_base_C(" + device_num + ", " + hdrName + ");\n";
                    caseHandlerBody += indent + tmpVar + " = dvmh_fill_header_C(" + device_num +
                            ", " + arrName + ", " + hdrName + ", " + devHdrName + ", 0);\n";
                    caseHandlerBody += indent + "assert(" + tmpVar + " == 0 || " + tmpVar + " == 1);\n";
                    caseHandlerBody += "\n";

                    // create kernel arrays fact parameters
                    caseKernelFactParams_arrays += ", " + arrName;
                    int iter_num = opts.autoTfm ? varState->rank : varState->rank - 1;
                    for ( int i = 1; i <= iter_num; ++i ) {
                        caseKernelFactParams_arrays += ", " + devHdrName + "[" + toStr( i ) + "]";
                    }
                }
            }
        }

        // handler scalars
        std::string caseKernelFactParams_scalars;
        for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
            VarState *varState = &varStates[ outerParams[ i ] ];
            std::string refName = varState->name;
            if ( !varState->isArray ) {
                std::string varType = varState->baseTypeStr;
                std::string ptrName = scalarPtrs[ refName ];

                caseHandlerBody += indent + varType + " *" + refName + " = (" + varType + "*)dvmh_get_device_addr_C(" +
                        device_num + ", " + ptrName + ");\n";

                // create scalars fact parameters
                caseKernelFactParams_scalars += ", " + refName;
            }
        }
        caseHandlerBody += "\n";

        // handle bounds and steps
        caseHandlerBody += indent + "/* Supplementary variables for loop handling */\n";
        caseHandlerBody += indent + "DvmType " + boundsLow + "[" + toStr( loopRank ) + "]";
        caseHandlerBody += ", " + boundsHigh + "[" + toStr( loopRank ) + "]";
        caseHandlerBody += ", " + loopSteps + "[" + toStr( loopRank ) + "];\n";
        caseHandlerBody += indent + "DvmType " + idxs + "[" + toStr( loopRank ) + "];\n";
        caseHandlerBody += indent + "cudaStream_t " + stream + ";\n";
        caseHandlerBody += "\n";

        std::string caseKernelFactParams_loopSteps;
        std::string caseKernelFactParams_idxs;
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFactParams_loopSteps += ", " + loopSteps + "[" + toStr( i ) + "]";
            caseKernelFactParams_idxs += ", " + idxs + "[" + toStr( i ) + "]";
        }

        // Choose index type for CUDA kernel
        caseHandlerBody += indent + "/* Choose index type for CUDA kernel */\n";
        caseHandlerBody += indent + "int " + kernelIndexT + " = dvmh_loop_guess_index_type_C(" + loop_ref + ");\n";
        caseHandlerBody += indent + "if (" + kernelIndexT + " == rt_LONG) " + kernelIndexT + " = (sizeof(long) <= sizeof(int) ? rt_INT : rt_LLONG);\n";
        caseHandlerBody += indent + "assert(" + kernelIndexT + " == rt_INT || " + kernelIndexT + " == rt_LLONG);\n";
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "/* Fill loop bounds */\n";
        caseHandlerBody += indent + "dvmh_loop_fill_bounds_C(" + loop_ref + ", " + boundsLow + ", " + boundsHigh + ", " + loopSteps + ");\n";
        caseHandlerBody += indent + "dvmh_change_filled_bounds2_C(" + boundsLow + ", " + boundsHigh + ", " + loopSteps + ", " + toStr( loopRank ) + ", " +
                dependencyMask + ", " + idxs + ");\n";
        caseHandlerBody += "\n";

        // Get CUDA configuration parameters
        caseHandlerBody += indent + "/* Get CUDA configuration parameters */\n";
        int threads_conf_x = 0;
        int threads_conf_y = 0;
        int threads_conf_z = 0;
        getDefaultCudaBlock(threads_conf_x, threads_conf_y, threads_conf_z, dep_number, loopRank - dep_number, opts.autoTfm);
        std::string threads_conf =
                toStr( threads_conf_x ) + ", " +
                toStr( threads_conf_y ) + ", " +
                toStr( threads_conf_z );

        caseHandlerBody += indent + "dim3 " + threads + " = dim3(" + threads_conf + ");\n";
//        caseHandlerBody += indent + "dim3 " + threads + " = dim3(0, 0, 0);\n";
        int shared_mem_num = loopRank > dep_number ? 8 * ( int )curPragma->reductions.size() : 0;
        caseHandlerBody += indent + "DvmType " + shared_mem + " = " + toStr( shared_mem_num ) + ";\n";
        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string regsVar = kernelsAvailable[ i ].regsVar;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;
            cudaInfoText += "#ifdef " + toUpper(regsVar) + "\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = " + toUpper(regsVar) + ";\n";
            cudaInfoText += "#else\n";
            cudaInfoText += indent + "DvmType " + regsVar + " = 0;\n";
            cudaInfoText += "#endif\n";
            caseHandlerBody += indent + "extern DvmType " + regsVar + ";\n";
            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") dvmh_loop_cuda_get_config_C(" + loop_ref + ", " + shared_mem + ", " +
                    regsVar + ", &" +
                    threads + ", &" + stream + ", &" + shared_mem + ");\n";
        }
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "/* Calculate computation distribution parameters */\n";
        // allocating cuda threads
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            caseHandlerBody += indent + "DvmType num_" + letter + " = threads." + toStr( letter ) + ";\n";
        }
        caseHandlerBody += "\n";

        // dependent dims
        for ( int i = 0; i < 3; ++i ) {
            caseHandlerBody += indent + "int M" + toStr( i ) + " = (" + boundsHigh + "[" + toStr( i ) + "] - " +
                    boundsLow + "[" + toStr( i ) + "]) / " + loopSteps + "[" + toStr( i ) + "] + 1;\n";
        }
        caseHandlerBody += indent + "int " + Allmin + " = min(min(M0, M1), M2);\n";
        caseHandlerBody += indent + "int " + Emin + " = min(M0, M1);\n";
        caseHandlerBody += indent + "int " + Emax + " = min(M0, M1) + abs(M0 - M1) + 1;\n";

        // independent dims
        std::string caseKernelFactParams_num_elem_indep;
        for ( int i = dep_number; i < loopRank; ++i ) {
            caseHandlerBody += indent + "DvmType num_elem_" + toStr( i ) + " = (" + boundsHigh + "[" + toStr( i ) + "] - " +
                    boundsLow + "[" + toStr( i ) + "]) / " + loopSteps + "[" + toStr( i ) + "] + 1;\n";
            caseKernelFactParams_num_elem_indep += ", num_elem_" + toStr( i );
        }

        if ( n_cuda_dims == 3 ) {
            caseHandlerBody += indent + "DvmType num_elem_z = ";
            for ( int i = 4; i < loopRank; ++i ) {
                if ( i != 4 ) {
                    caseHandlerBody += ", ";
                }
                caseHandlerBody += "num_elem_" + toStr( i );
            }
            caseHandlerBody += indent + ";\n";
        }
        caseHandlerBody += "\n";

        caseHandlerBody += indent + "int " + var1 + " = 1;\n";
        caseHandlerBody += indent + "int " + var2 + " = 0;\n";
        caseHandlerBody += indent + "int " + var3 + " = 0;\n";
        caseHandlerBody += indent + "int " + diag + " = 1;\n";
        caseHandlerBody += indent + "int " + SE + " = 1;\n";

        // determine blocks
        caseHandlerBody += indent + "dim3 " + blocks + " = dim3(";
        for ( int i = 0; i < n_cuda_dims; ++i ) {
            char letter = 'x' + i;
            if ( i != 0 ) {
                caseHandlerBody += ", ";
            }
            caseHandlerBody += "num_" + toStr( letter );
        }
        caseHandlerBody += ");\n";
        caseHandlerBody += "\n";

        if ( n_cuda_dims == 3 ) {
            caseHandlerBody += indent + "blocks.z = (num_elem_z + num_z - 1) / num_z;\n";
            caseHandlerBody += "\n";
        }

        // start reduction
        std::string caseKernelFactParams_reduction;
        if ( curPragma->reductions.size() > 0 ) {
            caseHandlerBody += indent + "/* Reductions-related stuff */\n";
            if ( loopRank == dep_number ) {
                caseHandlerBody += indent + "DvmType " + num_of_red_blocks + " = " + Emin + " * " + Allmin + ";\n";
            } else {
                caseHandlerBody += indent + "DvmType " + num_of_red_blocks + " = ((" + Emin + " + num_x - 1) / num_x)" +
                        " * ((" + Allmin + " + num_y - 1) / num_y) * blocks.z;\n";
            }

            for ( int i = 0; i < ( int )curPragma->reductions.size(); ++i ) {
                ClauseReduction *red = &curPragma->reductions[ i ];
                std::string epsGrid = redGrid[red->arrayName];

                VarDecl *vd = seekVarDecl(red->arrayName);
                checkIntErrN(vd && reductions.find(vd) != reductions.end(), 97, red->arrayName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                VarState *varState = &varStates[vd];
                checkDirErrN(!varState->isArray, 4417, varState->name.c_str());

                caseHandlerBody += indent + varState->baseTypeStr + " " + red->arrayName + ";\n";
                caseHandlerBody += indent + varState->baseTypeStr + " *" + epsGrid + ";\n";

                caseKernelFactParams_reduction += ", " + red->arrayName + ", " + epsGrid;

                if (red->isLoc()) {
                    std::string locGrid = redGrid[red->locName];
                    VarDecl *lvd = seekVarDecl(red->locName);
                    checkIntErrN(lvd && reductions.find(lvd) != reductions.end(), 97, red->locName.c_str(), curPragma->fileName.c_str(), curPragma->line);
                    VarState *locVarState = &varStates[lvd];
                    checkDirErrN(!locVarState->isArray, 4417, locVarState->name.c_str());
                    caseHandlerBody += indent + locVarState->baseTypeStr + " " + red->locName + ";\n";
                    caseHandlerBody += indent + locVarState->baseTypeStr + " *" + locGrid + ";\n";

                    caseHandlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", (void **)&" +
                            locGrid + ");\n";
                    caseHandlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", " + (locVarState->isArray ? "" : "&") + red->locName + ");\n";
                } else {
                    caseHandlerBody += indent + "dvmh_loop_cuda_register_red_C(" + loop_ref + ", " + toStr(i + 1) + ", (void **)&" + epsGrid + ", 0);\n";
                    caseHandlerBody += indent + "dvmh_loop_red_init_C(" + loop_ref + ", " + toStr(i + 1) + ", " + (varState->isArray ? "" : "&") + red->arrayName +
                            ", 0);\n";
                }
                caseHandlerBody += indent + "dvmh_loop_cuda_red_prepare_C(" + loop_ref + ", " + toStr(i + 1) + ", " + num_of_red_blocks + ", 1);\n";
            }
            caseHandlerBody += "\n";
        }

        // GPU execution
        caseHandlerBody += indent + "/* GPU execution */\n";

        if ( dep_number > 3 ) {
            for ( int i = 3; i < dep_number; ++i ) {
                caseHandlerBody += indent + "int " + "base_" + toStr( i ) + " = " + boundsLow + "[" + toStr( i ) + "];\n";
            }
            caseHandlerBody += indent + "while (base_3 <= " + boundsHigh + "[3]) {\n";
            indent += indentStep;

            caseHandlerBody += indent + var1 + " = 1;\n";
            caseHandlerBody += indent + var2 + " = 0;\n";
            caseHandlerBody += indent + var3 + " = 0;\n";
            caseHandlerBody += indent + diag + " = 1;\n";
            caseHandlerBody += indent + SE + " = 1;\n";

        }

//        std::string caseKernelFactParams_base;
        for ( int i = 0; i <= 2; ++i ) {
            caseHandlerBody += indent + "int " + "base_" + toStr( i ) + " = " + boundsLow + "[" + toStr( i ) + "];\n";
//            caseKernelFactParams_base += ", base_" + toStr( i );
        }
        for ( int i = dep_number; i < loopRank; ++i ) {
            caseHandlerBody += indent + "int " + "base_" + toStr( i ) + " = " + boundsLow + "[" + toStr( i ) + "];\n";
//            caseKernelFactParams_base += ", base_" + toStr( i );
        }
        std::string caseKernelFactParams_base;
        for ( int i = 0; i < loopRank; ++i ) {
            caseKernelFactParams_base += ", base_" + toStr( i );
        }
        caseHandlerBody += "\n";

        // first part, first loop
        caseHandlerBody += indent + diag + " = 1;\n";
        caseHandlerBody += indent + "while (" + diag + " <= " + Allmin + ") {\n";

        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + diag + " + num_x - 1) / num_x;\n";
        caseHandlerBody += indent + "blocks.y = (" + diag + " + num_y - 1) / num_y;\n";

        // generate kernel call
        std::string caseKernelFactParams = "";

        // pass parameters
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += ", " + diag + ", " + SE;
        caseKernelFactParams += ", " + var1 + ", " + var2 + ", " + var3;
        caseKernelFactParams += ", " + Emax + ", " + Emin;
        caseKernelFactParams += ", min(M0, M1)";
        caseKernelFactParams += ", M0 > M1";
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_2 = base_2 + " + loopSteps + "[2];\n";
//        caseHandlerBody += indent + "printf( \"loop 1\\n\" );\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";

        indent = subtractIndent( indent );

        caseHandlerBody += indent + "}\n";
        caseHandlerBody += "\n";

        // second stage

        caseHandlerBody += indent + var1 + " = 0;\n";
        caseHandlerBody += indent + var2 + " = 0;\n";
        caseHandlerBody += indent + var3 + " = 1;\n";

        caseHandlerBody += indent + "if (M2 > " + Emin + ") {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_0 = " + boundsLow + "[0] * (M0 <= M1) + " + boundsLow + "[1] * (M0 > M1);\n";
        caseHandlerBody += indent + "base_1 = " + boundsLow + "[1] * (M0 <= M1) + " + boundsLow + "[0] * (M0 > M1);\n";
        caseHandlerBody += indent + diag + " = Allmin + 1;\n";
        caseHandlerBody += indent + "while (" + diag + " - 1 != M2) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + Emin + " + num_x - 1) / num_x;\n";
        caseHandlerBody += indent + "blocks.y = (" + diag + " + num_y - 1) / num_y;\n";

        // generate kernel call
        caseKernelFactParams = "";

        // pass parameters
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += ", " + diag + ", " + SE;
        caseKernelFactParams += ", " + var1 + ", " + var2 + ", " + var3;
        caseKernelFactParams += ", " + Emax + ", " + Emin;
        caseKernelFactParams += ", min(M0, M1)";
        caseKernelFactParams += ", M0 > M1";
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + "base_2 = base_2 + " + loopSteps + "[2];\n";
//        caseHandlerBody += indent + "printf( \"loop 2\\n\" );\n";
        caseHandlerBody += indent + diag + " = " + diag + " + 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";
        caseHandlerBody += "\n";

        // third stage
        caseHandlerBody += indent + diag + " = M2;\n";
        caseHandlerBody += indent + "blocks.y = (" + diag + " + num_y - 1) / num_y;\n";
        caseHandlerBody += indent + "blocks.x = (" + Emin + " + num_x - 1) / num_x;\n";
        caseHandlerBody += indent + SE + " = 2;\n";

        caseHandlerBody += indent + "base_0 = (" + boundsLow + "[0] + " + loopSteps + "[0]) * (M0 <= M1) + (" +
                boundsLow + "[1] + " + loopSteps + "[1]) * (M0 > M1);\n";
        caseHandlerBody += indent + "base_1 = " + boundsLow + "[1] * (M0 <= M1) + " +
                boundsLow + "[0] * (M0 > M1);\n";
        caseHandlerBody += indent + "base_2 = " + boundsLow + "[2] + " + loopSteps + "[2] * (M2 - 1);\n";

        caseHandlerBody += indent + "while (M0 + M1 - " + Allmin + " != " + SE + " - 1) {\n";
        indent += indentStep;

        // generate kernel call
        caseKernelFactParams = "";

        // pass parameters
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += ", " + diag + ", " + SE;
        caseKernelFactParams += ", " + var1 + ", " + var2 + ", " + var3;
        caseKernelFactParams += ", " + Emax + ", " + Emin;
        caseKernelFactParams += ", min(M0, M1)";
        caseKernelFactParams += ", M0 > M1";
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }
        caseHandlerBody += indent + "base_0 = base_0 + " + loopSteps + "[0] * (M0 <= M1) + " + loopSteps + "[1] * (M0 > M1);\n";
//        caseHandlerBody += indent + "printf( \"loop 3\\n\" );\n";
        caseHandlerBody += indent + SE + " = " + SE + " + 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";
        caseHandlerBody += "\n";

        // forth stage
        caseHandlerBody += indent + var1 + " = 0;\n";
        caseHandlerBody += indent + var2 + " = 1;\n";
        caseHandlerBody += indent + var3 + " = 0;\n";
        caseHandlerBody += indent + diag + " = " + Allmin + " - 1;\n";
        caseHandlerBody += indent + "base_0 = " + boundsLow + "[0] + " + loopSteps + "[0] * (M0 - 1);\n";
        caseHandlerBody += indent + "base_1 = " + boundsLow + "[1] * (M0 > M1) + base_1 * (M0 <= M1);\n";

        caseHandlerBody += indent + "if (M0 > M1 && M2 <= " + Emin + ") {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1] + abs(" + Emin + " - M2);\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "} else {\n";
        indent += indentStep;

        caseHandlerBody += indent + "if (M0 <= M1 && M2 <= " + Emin + ") {\n";
        indent += indentStep;

        caseHandlerBody += indent + "if (" + loopSteps + "[1] > 0) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1] + " + Emax + " - " + Emin + " - 1 + "
                + "abs(" + Emin + " - M2);\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "} else {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1] - " + Emax + " + " + Emin + " + 1 + "
                + "M2 - " + Emin + ";\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "} else {\n";
        indent += indentStep;

        caseHandlerBody += indent + "if (M0 > M1 && M2 > " + Emin + ") {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1];\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "} else {\n";
        indent += indentStep;

        caseHandlerBody += indent + "if (M0 <= M1 && M2 > " + Emin + ") {\n";
        indent += indentStep;

        caseHandlerBody += indent + "if (" + loopSteps + "[1] > 0) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1] + " + Emax + " - " + Emin + " - 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "} else {\n";
        indent += indentStep;

        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1] - " + Emax + " + " + Emin + " + 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";

        caseHandlerBody += indent + "while (" + diag + " != 0) {\n";
        indent += indentStep;

        caseHandlerBody += indent + "blocks.x = (" + diag + " + num_x - 1) / num_x;\n";
        caseHandlerBody += indent + "blocks.y = (" + diag + " + num_y - 1) / num_y;\n";

        // generate kernel call
        caseKernelFactParams = "";

        // pass parameters
        caseKernelFactParams += caseKernelFactParams_arrays;
        caseKernelFactParams += caseKernelFactParams_scalars;
        caseKernelFactParams += caseKernelFactParams_reduction;
        caseKernelFactParams += caseKernelFactParams_base;
        caseKernelFactParams += caseKernelFactParams_loopSteps;
        caseKernelFactParams += ", " + diag + ", " + SE;
        caseKernelFactParams += ", " + var1 + ", " + var2 + ", " + var3;
        caseKernelFactParams += ", " + Emax + ", " + Emin;
        caseKernelFactParams += ", min(M0, M1)";
        caseKernelFactParams += ", M0 > M1";
        caseKernelFactParams += caseKernelFactParams_num_elem_indep;
        caseKernelFactParams += caseKernelFactParams_idxs;
        trimList( caseKernelFactParams );

        for ( int i = 0; i < ( int )kernelsAvailable.size(); i++ ) {
            std::string caseKernelName = kernelsAvailable[ i ].kernelName;
            std::string rtIndexT = kernelsAvailable[ i ].rtIndexT;

            caseHandlerBody += indent + "if (" + kernelIndexT + " == " + rtIndexT + ") " +
                    caseKernelName + "<<<" + blocks + ", " + threads + ", " + shared_mem + ", " + stream + ">>>(" +
                    caseKernelFactParams + ");\n";
        }

        caseHandlerBody += indent + SE + " = " + SE + " + 1;\n";
        caseHandlerBody += indent + "base_1 = base_1 + " + loopSteps + "[1];\n";
//        caseHandlerBody += indent + "printf( \"loop 4\\n\" );\n";
        caseHandlerBody += indent + diag + " = " + diag + " - 1;\n";

        indent = subtractIndent( indent );
        caseHandlerBody += indent + "}\n";
        caseHandlerBody += "\n";

        if ( dep_number > 3 ) {
            caseHandlerBody += indent + "base_3 = base_3 + " + loopSteps + "[3];\n";

            indent = subtractIndent( indent );
            caseHandlerBody += indent + "}\n";

            for ( int i = 4; i < dep_number; ++i ) {
                caseHandlerBody += indent + "base_" + toStr( i ) + " = base_" + toStr( i ) + " + " + loopSteps + "[" + toStr( i ) + "];\n";
            }
        }

        // end reduction
        if ( curPragma->reductions.size() > 0 ) {
            caseHandlerBody += indent + "/* Finish reduction */\n";
            for ( int i = 0; i < ( int )curPragma->reductions.size(); ++i ) {
                caseHandlerBody += indent + "dvmh_loop_cuda_red_finish_C(" + loop_ref + ", " + toStr( i + 1 ) + ");\n";
            }
        }
    }

    // get case handler text
    caseHandlerText += caseKernelText;

    caseHandlerText += ( fileCtx.getInputFile().CPlusPlus ? handlerTemplateDecl : "extern \"C\" " );
    caseHandlerText += "void " + caseHandlerName + "(" + caseHandlerFormalParams + ") {\n";
    caseHandlerText += caseHandlerBody;
    caseHandlerText += "}\n\n";
}

void ConverterASTVisitor::genAcrossCudaHandler(std::string handlerName, const std::vector<VarDecl *> &outerParams, const std::vector<LoopVarDesc> &loopVars,
    std::string handlerTemplateDecl, std::string handlerTemplateSpec, std::string &handlerFormalParams, std::string &handlerBody,
    std::string &caseHandlers , std::string &cudaInfoText)
{
    std::string indent = indentStep;
    PragmaParallel *curPragma = curParallelPragma;
    bool isSequentialPart = curPragma == 0;
    int loopRank = (isSequentialPart ? 0 : curPragma->rank);

    // get prohibited names
    std::set<std::string> prohibitedNames;
    CollectNamesVisitor collectNamesVisitor(comp);
    collectNamesVisitor.TraverseStmt(parLoopBodyStmt);
    prohibitedNames = collectNamesVisitor.getNames();
    for (int i = 0; i < (int)outerParams.size(); i++)
        prohibitedNames.insert(outerParams[i]->getName().str());
    for (std::set<VarDecl *>::iterator it = outerPrivates.begin(); it != outerPrivates.end(); it++)
        prohibitedNames.insert((*it)->getName().str());
    for (std::set<VarDecl *>::iterator it = reductions.begin(); it != reductions.end(); it++)
        prohibitedNames.insert((*it)->getName().str());

    // get unique names
    std::string pLoopRef = getUniqueName( "pLoopRef", &prohibitedNames );
    std::string dependency_mask = getUniqueName( "dependency_mask", &prohibitedNames );
    std::string dependency_mask_tmp = getUniqueName( "dependency_mask_tmp", &prohibitedNames );
    std::string dependency_num = getUniqueName( "dependency_num", &prohibitedNames );

    // get outer params
    std::map<std::string, std::string> dvmHeaders;
    std::map<std::string, std::string> dvmDevHeaders;
    std::map<std::string, std::string> scalarPtrs;
    std::map<std::string, std::vector<std::string> > dvmCoefs;
    for (int i = 0; i < (int)outerParams.size(); i++) {
        VarState *varState = &varStates[outerParams[i]];
        if (varState->isArray) {
            dvmHeaders[varState->name] = getUniqueName(varState->name + "_hdr", &prohibitedNames);
            dvmDevHeaders[varState->name] = getUniqueName(varState->name + "_devHdr", &prohibitedNames);
            dvmCoefs[varState->name].clear();
            for (int j = 0; j < varState->headerArraySize; j++)
                dvmCoefs[varState->name].push_back(getUniqueName(varState->name + "_hdr" + toStr(j), &prohibitedNames));
        } else {
            scalarPtrs[varState->name] = getUniqueName(varState->name + "_ptr", &prohibitedNames);
        }
    }

    // get formal params
    std::string typicalCaseHandlerFactParams;
    handlerFormalParams += "DvmType *" + pLoopRef;
    typicalCaseHandlerFactParams += pLoopRef;
    for ( int i = 0; i < ( int )outerParams.size(); ++i ) {
        VarState *varState = &varStates[ outerParams[ i ] ];
        std::string refName = varState->name;
        if ( varState->isArray ) {
            std::string hdrName = dvmHeaders[ refName ];

            handlerFormalParams += ", DvmType " + hdrName + "[]";
            typicalCaseHandlerFactParams += ", " + hdrName;
        } else {
            handlerFormalParams += ", " + varState->baseTypeStr + " *" + scalarPtrs[ refName ];
            typicalCaseHandlerFactParams += ", " + scalarPtrs[ refName ];
        }
    }
    typicalCaseHandlerFactParams += ", " + dependency_mask;

    // get body
    handlerBody += indent + "/* Get number of dependencies */\n";
    handlerBody += indent + "int " + dependency_mask + " = dvmh_loop_get_dependency_mask_C(*" + pLoopRef + ")" + ";\n";
    handlerBody += indent + "int " + dependency_mask_tmp + " = " + dependency_mask + ";\n";
    handlerBody += indent + "int " + dependency_num + " = 0;\n";
    handlerBody += indent + "while(" + dependency_mask_tmp + ") {\n";

    indent += indentStep;
    handlerBody += indent + dependency_mask_tmp + " &= (" + dependency_mask_tmp + " - 1);\n";
    handlerBody += indent + "++" + dependency_num + ";\n";
    indent = subtractIndent( indent );

    handlerBody += indent + "}\n\n";

    handlerBody += indent + "/* Run the corresponding handler */\n";
//    int max_dependencies_number = loopRank;
//    printf( "acrosses size %d\n", curPragma->acrosses.size() );
    int max_dependencies_number = curPragma->acrosses[ 0 ].getDepCount();
    for ( int i = 1; i <= max_dependencies_number; ++i ) {
        if ( i != 1 ) {
            handlerBody += indent + "else if (" + dependency_num + " == " + toStr( i ) + ") {\n";
        } else {
            handlerBody += indent + "if (" + dependency_num + " == " + toStr( i ) + ") {\n";
        }
        indent += indentStep;

        // insert case handler call
        int case_handler_name_number = (1 << i) - 1;
        std::string case_handler_name = handlerName + "_" + toStr( case_handler_name_number ) + "_case";
        std::string case_handler_fact_params = typicalCaseHandlerFactParams;
        handlerBody += indent + case_handler_name + "(" + case_handler_fact_params + ");\n";

        // generate case handler
        int dep_number = i;
        std::string case_handler_text = "";
        genAcrossCudaCaseHandler( dep_number, handlerName, case_handler_name, outerParams, loopVars,
                              handlerTemplateDecl, handlerTemplateSpec, case_handler_text, cudaInfoText );
        caseHandlers += case_handler_text;

        indent = subtractIndent( indent );
        handlerBody += indent + "}\n";
    }
}

}

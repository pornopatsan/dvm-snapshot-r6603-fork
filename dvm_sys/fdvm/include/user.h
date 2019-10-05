/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#ifndef SAGEXXUSER_H 
#define SAGEXXUSER_H 1

#define CPLUS_
#include "macro.h"
#undef CPLUS_

// For C/C++ parser internals
#include "vpc.h"

// For the fortran parser internals
#include "fdvm.h"

// All the "C" functions from the Rennes toolbox
#include "extcxx_low.h"

class SgProject;
class SgFile;
class SgStatement;
class SgExpression;
class SgLabel;
class SgSymbol;
class SgType;
class SgUnaryExp;
class SgClassSymb;
class SgVarDeclStmt;
class SgVarRefExp; /* ajm: I think they should all be here! @$!@ */

// All the externs (from libSage++.C) used in libSage++.h
#include "sage++extern.h"

#define SORRY Message("Sorry, not implemented yet",0)

// Prototype definitions  for all the functions in libSage++.C
#include "sage++proto.h"


// dont delete needed in libSage++.h
#define USER
#include "libSage++.h"

#endif /* ndef SAGEXXUSER_H */

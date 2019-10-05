/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/
#include "leak_detector.h"
#include <stdio.h>
#include <stdlib.h>

#include <map>
#include <string>

#ifndef __GNUC__

#else
extern "C" void abort(void);
extern "C" void exit(int status);
/*# pragma implementation*/
#endif

#define CPLUS_
#include "macro.h"
#undef CPLUS_
#include "vpc.h"
#include "f90.h"

#include "extcxx_low.h"
extern "C" int number_of_ll_node;

#undef USER

#if __SPF
extern "C" void addToCollection(const int line, const char *file, void *pointer, int type);
extern "C" void removeFromCollection(void *pointer);
extern std::map<PTR_BFND, std::pair<std::string, int> > sgStats;
extern std::map<PTR_LLND, std::pair<std::string, int> > sgExprs;
extern void addToGlobalBufferAndPrint(const std::string &toPrint);
#endif

//
// define for having the debugging
//
//define  DEBUGLIB 1
#define  MAX_FILES 1000
//
//
// Array to keep track of table for a file
//
//

void **tablebfnd[MAX_FILES];
void **tablellnd[MAX_FILES];
void **tabletype[MAX_FILES];
void **tablesymbol[MAX_FILES];
void **tablelabel[MAX_FILES];

int numtablebfnd[MAX_FILES];
int numtablellnd[MAX_FILES];
int numtabletype[MAX_FILES];
int numtablesymbol[MAX_FILES];
int numtablelabel[MAX_FILES];


////////////////////////////  ATTRIBUTES /////////////////////////////////
// Array to keep track of the attributes for statement, symbol, ...
///////////////////////////////////////////////////////////////////////////

class SgAttribute;

SgAttribute **tablebfndAttribute[MAX_FILES];
SgAttribute **tablellndAttribute[MAX_FILES];
SgAttribute **tabletypeAttribute[MAX_FILES];
SgAttribute **tablesymbolAttribute[MAX_FILES];
SgAttribute **tablelabelAttribute[MAX_FILES];

int numtablebfndAttribute[MAX_FILES];
int numtablellndAttribute[MAX_FILES];
int numtabletypeAttribute[MAX_FILES];
int numtablesymbolAttribute[MAX_FILES];
int numtablelabelAttribute[MAX_FILES];



//
// Table definition for attributes 
// 
//


SgAttribute **fileTableAttribute;
int allocatedForfileTableAttribute;
SgAttribute **bfndTableAttribute;
int allocatedForbfndTableAttribute;
SgAttribute **llndTableAttribute;
int allocatedForllndTableAttribute;
SgAttribute **typeTableAttribute;
int allocatedFortypeTableAttribute;
SgAttribute **symbolTableAttribute;
int allocatedForsymbolTableAttribute;
SgAttribute **labelTableAttribute;
int allocatedForlabelTableAttribute;

///////////////////////////////// END ATTRIBUTES ///////////////////////////


static int CurrentFileNumber = 0;

//
// Table for making link between the nodes and the classes
// Take the id and return a pointer
//

void **fileTableClass;
int allocatedForfileTableClass;
void **bfndTableClass;
int allocatedForbfndTableClass;
void **llndTableClass;
int allocatedForllndTableClass;
void **typeTableClass;
int allocatedFortypeTableClass;
void **symbolTableClass;
int allocatedForsymbolTableClass;
void **labelTableClass;
int allocatedForlabelTableClass;


//
// Some definition for this module
//
#define ALLOCATECHUNK 10000

#define SORRY Message("Sorry, not implemented yet",0)

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


//
// Set of function to care about the table management
//

void InitializeTable()
{
    int i;
    for (i = 0; i < MAX_FILES; i++)
    {
        tablebfnd[i] = NULL;
        tablellnd[i] = NULL;
        tabletype[i] = NULL;
        tablesymbol[i] = NULL;
        tablelabel[i] = NULL;

        numtablebfnd[i] = 0;
        numtablellnd[i] = 0;
        numtabletype[i] = 0;
        numtablesymbol[i] = 0;
        numtablelabel[i] = 0;

        // FOR ATTRIBUTES;
        tablebfndAttribute[i] = NULL;
        tablellndAttribute[i] = NULL;
        tabletypeAttribute[i] = NULL;
        tablesymbolAttribute[i] = NULL;
        tablelabelAttribute[i] = NULL;

        numtablebfndAttribute[i] = 0;
        numtablellndAttribute[i] = 0;
        numtabletypeAttribute[i] = 0;
        numtablesymbolAttribute[i] = 0;
        numtablelabelAttribute[i] = 0;
    }


    fileTableClass = NULL;
    bfndTableClass = NULL;
    llndTableClass = NULL;
    typeTableClass = NULL;
    symbolTableClass = NULL;
    labelTableClass = NULL;
    allocatedForfileTableClass = 0;
    allocatedForbfndTableClass = 0;
    allocatedForllndTableClass = 0;
    allocatedFortypeTableClass = 0;
    allocatedForsymbolTableClass = 0;
    allocatedForlabelTableClass = 0;

    // FOR ATTRIBUTES;
    fileTableAttribute = NULL;
    bfndTableAttribute = NULL;
    llndTableAttribute = NULL;
    typeTableAttribute = NULL;
    symbolTableAttribute = NULL;
    labelTableAttribute = NULL;
    allocatedForfileTableAttribute = 0;
    allocatedForbfndTableAttribute = 0;
    allocatedForllndTableAttribute = 0;
    allocatedFortypeTableAttribute = 0;
    allocatedForsymbolTableAttribute = 0;
    allocatedForlabelTableAttribute = 0;
}


void SwitchToFile(int i)
{
    if (i >= MAX_FILES)
    {
        Message("Too many files", 0);
        exit(1);
    }

    tablebfnd[CurrentFileNumber] = bfndTableClass;
    tablellnd[CurrentFileNumber] = llndTableClass;
    tabletype[CurrentFileNumber] = typeTableClass;
    tablesymbol[CurrentFileNumber] = symbolTableClass;
    tablelabel[CurrentFileNumber] = labelTableClass;

    numtablebfnd[CurrentFileNumber] = allocatedForbfndTableClass;
    numtablellnd[CurrentFileNumber] = allocatedForllndTableClass;
    numtabletype[CurrentFileNumber] = allocatedFortypeTableClass;
    numtablesymbol[CurrentFileNumber] = allocatedForsymbolTableClass;
    numtablelabel[CurrentFileNumber] = allocatedForlabelTableClass;

    bfndTableClass = tablebfnd[i];
    llndTableClass = tablellnd[i];
    typeTableClass = tabletype[i];
    symbolTableClass = tablesymbol[i];
    labelTableClass = tablelabel[i];

    allocatedForbfndTableClass = numtablebfnd[i];
    allocatedForllndTableClass = numtablellnd[i];
    allocatedFortypeTableClass = numtabletype[i];
    allocatedForsymbolTableClass = numtablesymbol[i];
    allocatedForlabelTableClass = numtablelabel[i];

    // FOR ATTRIBUTES
    tablebfndAttribute[CurrentFileNumber] = bfndTableAttribute;
    tablellndAttribute[CurrentFileNumber] = llndTableAttribute;
    tabletypeAttribute[CurrentFileNumber] = typeTableAttribute;
    tablesymbolAttribute[CurrentFileNumber] = symbolTableAttribute;
    tablelabelAttribute[CurrentFileNumber] = labelTableAttribute;

    numtablebfndAttribute[CurrentFileNumber] = allocatedForbfndTableAttribute;
    numtablellndAttribute[CurrentFileNumber] = allocatedForllndTableAttribute;
    numtabletypeAttribute[CurrentFileNumber] = allocatedFortypeTableAttribute;
    numtablesymbolAttribute[CurrentFileNumber] = allocatedForsymbolTableAttribute;
    numtablelabelAttribute[CurrentFileNumber] = allocatedForlabelTableAttribute;

    bfndTableAttribute = tablebfndAttribute[i];
    llndTableAttribute = tablellndAttribute[i];
    typeTableAttribute = tabletypeAttribute[i];
    symbolTableAttribute = tablesymbolAttribute[i];
    labelTableAttribute = tablelabelAttribute[i];

    allocatedForbfndTableAttribute = numtablebfndAttribute[i];
    allocatedForllndTableAttribute = numtablellndAttribute[i];
    allocatedFortypeTableAttribute = numtabletypeAttribute[i];
    allocatedForsymbolTableAttribute = numtablesymbolAttribute[i];
    allocatedForlabelTableAttribute = numtablelabelAttribute[i];
    CurrentFileNumber = i;
}

/////////////////////////////////////////// FOR ATTRIBUTES //////////////////////////////////


// add a chunk to the size 
void ReallocatefileTableAttribute()
{
  int i;
  SgAttribute **pt;
  
  pt  =  new SgAttribute *[allocatedForfileTableAttribute + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForfileTableAttribute + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  
  for (i=0 ; i < allocatedForfileTableAttribute; i++)
    {
      pt[i] = fileTableAttribute[i];
    }
  if (allocatedForfileTableAttribute)
  {
#ifdef __SPF   
      removeFromCollection(fileTableAttribute);
#endif
      delete fileTableAttribute;
  }
  fileTableAttribute = pt;
  allocatedForfileTableAttribute = allocatedForfileTableAttribute + ALLOCATECHUNK;
}


void ReallocatebfndTableAttribute()
{
  int i;
  SgAttribute **pt;
  
  pt  =  new SgAttribute *[allocatedForbfndTableAttribute + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForbfndTableAttribute + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForbfndTableAttribute; i++)
    {
      pt[i] = bfndTableAttribute[i];
    }
  if (allocatedForbfndTableAttribute)
  {
#ifdef __SPF   
      removeFromCollection(bfndTableAttribute);
#endif
      delete bfndTableAttribute;
  }
  bfndTableAttribute = pt;
  allocatedForbfndTableAttribute = allocatedForbfndTableAttribute + ALLOCATECHUNK;
}


void ReallocatellndTableAttribute()
{
  int i;
  SgAttribute **pt;
  
  pt  =  new SgAttribute *[allocatedForllndTableAttribute + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForllndTableAttribute + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForllndTableAttribute; i++)
    {
      pt[i] = llndTableAttribute[i];
    }
  if (allocatedForllndTableAttribute)
  {
#ifdef __SPF   
      removeFromCollection(llndTableAttribute);
#endif
      delete llndTableAttribute;
  }
  llndTableAttribute = pt;
  allocatedForllndTableAttribute = allocatedForllndTableAttribute + ALLOCATECHUNK;
}

void ReallocatesymbolTableAttribute()
{
  int i;
  SgAttribute **pt;
  
  pt  =  new SgAttribute *[allocatedForsymbolTableAttribute + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForsymbolTableAttribute + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForsymbolTableAttribute; i++)
    {
      pt[i] = symbolTableAttribute[i];
    }
  if (allocatedForsymbolTableAttribute)
  {
#ifdef __SPF   
      removeFromCollection(symbolTableAttribute);
#endif
      delete symbolTableAttribute;
  }
  symbolTableAttribute = pt;
  allocatedForsymbolTableAttribute = allocatedForsymbolTableAttribute + ALLOCATECHUNK;
}


void ReallocatelabelTableAttribute()
{
  int i;
  SgAttribute **pt;
  
  pt  =  new SgAttribute *[allocatedForlabelTableAttribute + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForlabelTableAttribute + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForlabelTableAttribute; i++)
    {
      pt[i] = labelTableAttribute[i];
    }
  if (allocatedForlabelTableAttribute)
  {
#ifdef __SPF   
      removeFromCollection(labelTableAttribute);
#endif
      delete labelTableAttribute;
  }
  labelTableAttribute = pt;
  allocatedForlabelTableAttribute = allocatedForlabelTableAttribute + ALLOCATECHUNK;
}


void ReallocatetypeTableAttribute()
{
  int i;
  SgAttribute **pt;
  
  pt  =  new SgAttribute *[allocatedFortypeTableAttribute + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedFortypeTableAttribute + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedFortypeTableAttribute; i++)
    {
      pt[i] = typeTableAttribute[i];
    }
  if (allocatedFortypeTableAttribute)
  {
#ifdef __SPF   
      removeFromCollection(typeTableAttribute);
#endif
      delete typeTableAttribute;
  }
  typeTableAttribute = pt;
  allocatedFortypeTableAttribute = allocatedFortypeTableAttribute + ALLOCATECHUNK;
}


void 
SetMappingInTableForBfndAttribute(PTR_BFND bif, SgAttribute *pt)
{
  if (!bif)
    return ;
  while (allocatedForbfndTableAttribute <= BIF_ID(bif))
    {
      ReallocatebfndTableAttribute();
    }
  bfndTableAttribute[BIF_ID(bif)] = pt;
}


void 
SetMappingInTableForTypeAttribute(PTR_TYPE type, SgAttribute *pt)
{
  if (!type)
    return ;
  while (allocatedFortypeTableAttribute <= TYPE_ID(type))
    {
      ReallocatetypeTableAttribute();
    }
  typeTableAttribute[TYPE_ID(type)] = pt;
}


void 
SetMappingInTableForSymbolAttribute(PTR_SYMB symb, SgAttribute *pt)
{
  if (!symb)
    return ;
  while (allocatedForsymbolTableAttribute <= SYMB_ID(symb))
    {
      ReallocatesymbolTableAttribute();
    }
  symbolTableAttribute[SYMB_ID(symb)] = pt;
}

void 
SetMappingInTableForLabelAttribute(PTR_LABEL lab, SgAttribute *pt)
{
  if (!lab)
    return ;
  while (allocatedForlabelTableAttribute <= LABEL_ID(lab))
    {
      ReallocatelabelTableAttribute();
    }
  labelTableAttribute[LABEL_ID(lab)] = pt;
}



void 
SetMappingInTableForLlndAttribute(PTR_LLND ll, SgAttribute *pt)
{
  if (!ll)
    return ;
  while (allocatedForllndTableAttribute <= NODE_ID(ll))
    {
      ReallocatellndTableAttribute();
    }
  llndTableAttribute[NODE_ID(ll)] = pt;
}


void 
SetMappingInTableForFileAttribute(PTR_FILE file, SgAttribute *pt)
{
  int id;
  if (!file)
    return ;
  id = GetFileNum(FILE_FILENAME(file));
  while (allocatedForfileTableAttribute <= id)
    {
      ReallocatefileTableAttribute();
    }
  fileTableAttribute[id] = pt;
}


SgAttribute *
GetMappingInTableForSymbolAttribute(PTR_SYMB symb)
{
  int id;
  if (!symb)
    return NULL;
  id = SYMB_ID(symb);
  if (allocatedForsymbolTableAttribute <= id)    
    {
      return NULL;
    }
  return  symbolTableAttribute[id];
}



SgAttribute *
GetMappingInTableForLabelAttribute(PTR_LABEL lab)
{
  int id;
  if (!lab)
    return NULL;
  id = LABEL_ID(lab);
  if (allocatedForlabelTableAttribute <= id)    
    {
      return NULL;
    }
  return  labelTableAttribute[id];
}


SgAttribute *
GetMappingInTableForBfndAttribute(PTR_BFND bf)
{
  int id;
  if (!bf)
    return NULL;
  id = BIF_ID(bf);
  if (allocatedForbfndTableAttribute <= id)    
    {
      return NULL;
    }
  return bfndTableAttribute[id];
}


SgAttribute *
GetMappingInTableForTypeAttribute(PTR_TYPE t)
{
  int id;
  if (!t)
    return NULL;
  id = TYPE_ID(t);
  if (allocatedFortypeTableAttribute <= id)    
    {
      return NULL;
    }
  return  typeTableAttribute[id];
}


SgAttribute *
GetMappingInTableForLlndAttribute(PTR_LLND ll)
{
  int id;
  if (!ll)
    return NULL;
  id = NODE_ID(ll);
  if (allocatedForllndTableAttribute <= id)    
    {
      return NULL;
    }
  return  llndTableAttribute[id];
}


SgAttribute *
GetMappingInTableForFileAttribute(PTR_FILE file)
{
  int id;
  if (!file)
    return NULL;
  id = GetFileNum(FILE_FILENAME(file));
  if (allocatedForfileTableAttribute <= id)
    {
      return NULL;
    }
  return  fileTableAttribute[id];
}



//////////////////////////////////END ATTRIBUTE STUFFS/////////////////////////////////////////

// add a chunk to the size 
void ReallocatefileTableClass()
{
  int i;
  void **pt;
  
  pt  =  new void *[allocatedForfileTableClass + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForfileTableClass + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  
  for (i=0 ; i < allocatedForfileTableClass; i++)
    {
      pt[i] = fileTableClass[i];
    }
  if (allocatedForfileTableClass)
  {
#ifdef __SPF   
      removeFromCollection(fileTableClass);
#endif
      delete fileTableClass;
  }
  fileTableClass = pt;
  allocatedForfileTableClass = allocatedForfileTableClass + ALLOCATECHUNK;
}


void ReallocatebfndTableClass()
{
  int i;
  void **pt;
  
  pt  =  new void *[allocatedForbfndTableClass + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForbfndTableClass + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForbfndTableClass; i++)
    {
      pt[i] = bfndTableClass[i];
    }
  if (allocatedForbfndTableClass)
  {
#ifdef __SPF   
      removeFromCollection(bfndTableClass);
#endif
      delete bfndTableClass;
  }
  bfndTableClass = pt;
  allocatedForbfndTableClass = allocatedForbfndTableClass + ALLOCATECHUNK;
}


void ResetbfndTableClass()
{
  int i;
  
  for (i=0 ; i < allocatedForbfndTableClass; i++)
    {
//      delete bfndTableClass[i];
      bfndTableClass[i] = NULL;
    }
}

void ReallocatellndTableClass()
{
    int i;
    void **pt;

    pt = new void *[allocatedForllndTableClass + ALLOCATECHUNK];
#ifdef __SPF   
    addToCollection(__LINE__, __FILE__, pt, 2);
#endif
    for (i = 0; i < allocatedForllndTableClass + ALLOCATECHUNK; i++)
        pt[i] = NULL;
    for (i = 0; i < allocatedForllndTableClass; i++)
    {
        pt[i] = llndTableClass[i];
    }
    if (allocatedForllndTableClass)
    {
#ifdef __SPF   
        removeFromCollection(llndTableClass);
#endif
        delete llndTableClass;
    }
    llndTableClass = pt;
    allocatedForllndTableClass = allocatedForllndTableClass + ALLOCATECHUNK;
}

void ReallocatesymbolTableClass()
{
  int i;
  void **pt;
  
  pt  =  new void *[allocatedForsymbolTableClass + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForsymbolTableClass + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForsymbolTableClass; i++)
    {
      pt[i] = symbolTableClass[i];
    }
  if (allocatedForsymbolTableClass)
  {
#ifdef __SPF   
      removeFromCollection(symbolTableClass);
#endif
      delete symbolTableClass;
  }
  symbolTableClass = pt;
  allocatedForsymbolTableClass = allocatedForsymbolTableClass + ALLOCATECHUNK;
}


void ReallocatelabelTableClass()
{
  int i;
  void **pt;
  
  pt  =  new void *[allocatedForlabelTableClass + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedForlabelTableClass + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedForlabelTableClass; i++)
    {
      pt[i] = labelTableClass[i];
    }
  if (allocatedForlabelTableClass)
  {
#ifdef __SPF   
      removeFromCollection(labelTableClass);
#endif
      delete labelTableClass;
  }
  labelTableClass = pt;
  allocatedForlabelTableClass = allocatedForlabelTableClass + ALLOCATECHUNK;
}


void ReallocatetypeTableClass()
{
  int i;
  void **pt;
  
  pt  =  new void *[allocatedFortypeTableClass + ALLOCATECHUNK];
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 2);
#endif
  for (i=0; i<allocatedFortypeTableClass + ALLOCATECHUNK; i++)
    pt[i] = NULL;
  for (i=0 ; i < allocatedFortypeTableClass; i++)
    {
      pt[i] = typeTableClass[i];
    }
  if (allocatedFortypeTableClass)
  {
#ifdef __SPF   
      removeFromCollection(typeTableClass);
#endif
      delete typeTableClass;
  }
  typeTableClass = pt;
  allocatedFortypeTableClass = allocatedFortypeTableClass + ALLOCATECHUNK;
}


void RemoveFromTableType(void * pt)
{
  int i;
  for (i=0 ; i < allocatedFortypeTableClass; i++)
    {
      if (typeTableClass[i] == pt)
        {
          typeTableClass[i] = NULL;
          return;
        }
    }
}


void RemoveFromTableSymb(void * pt)
{
  int i;
  for (i=0 ; i < allocatedForsymbolTableClass; i++)
    {
      if (symbolTableClass[i] == pt)
        {
          symbolTableClass[i] = NULL;
          return;
        }
    }
}



void RemoveFromTableBfnd(void * pt)
{
  int i;
  for (i=0 ; i < allocatedForbfndTableClass; i++)
    {
      if (bfndTableClass[i] == pt)
        {
          bfndTableClass[i] = NULL;
          return;
        }
    }
}


void RemoveFromTableFile(void * pt)
{
  int i;
  for (i=0 ; i < allocatedForfileTableClass; i++)
    {
      if (fileTableClass[i] == pt)
        {
          fileTableClass[i] = NULL;
          return;
        }
    }
}

// forward, to be defined later in this file;
void RemoveFromTableLlnd(void * pt); 


void RemoveFromTableLabel(void * pt)
{
    int i;
    for (i = 0; i < allocatedForlabelTableClass; i++)
    {
        if (labelTableClass[i] == pt)
        {
            labelTableClass[i] = NULL;
            return;
        }
    }
}

void SetMappingInTableForBfnd(PTR_BFND bif, void *pt)
{
    if (!bif)
        return;
    while (allocatedForbfndTableClass <= BIF_ID(bif))
    {
        ReallocatebfndTableClass();
    }
#if __SPF
    std::map<PTR_BFND, std::pair<std::string, int> >::iterator it = sgStats.find(bif);
    if (it != sgStats.end())
    {
        char buf[512];
        sprintf(buf, "Internal error at line %d and file libSage++.cpp, this place was occupied\n", __LINE__);
        addToGlobalBufferAndPrint(buf);
        throw(-1);
    }
#endif
    bfndTableClass[BIF_ID(bif)] = pt;
}


void SetMappingInTableForType(PTR_TYPE type, void *pt)
{
    if (!type)
        return;
    while (allocatedFortypeTableClass <= TYPE_ID(type))
    {
        ReallocatetypeTableClass();
    }
    typeTableClass[TYPE_ID(type)] = pt;
}


void SetMappingInTableForSymb(PTR_SYMB symb, void *pt)
{
    if (!symb)
        return;
    while (allocatedForsymbolTableClass <= SYMB_ID(symb))
    {
        ReallocatesymbolTableClass();
    }
    symbolTableClass[SYMB_ID(symb)] = pt;
}

void SetMappingInTableForLabel(PTR_LABEL lab, void *pt)
{
    if (!lab)
        return;
    while (allocatedForlabelTableClass <= LABEL_ID(lab))
    {
        ReallocatelabelTableClass();
    }
    labelTableClass[SYMB_ID(lab)] = pt;
}

void SetMappingInTableForLlnd(PTR_LLND ll, void *pt)
{
    if (!ll)
        return;
    while (allocatedForllndTableClass <= NODE_ID(ll))
    {
        ReallocatellndTableClass();
    }
#if __SPF
    std::map<PTR_LLND, std::pair<std::string, int> >::iterator it = sgExprs.find(ll);
    if (it != sgExprs.end())
    {
        char buf[512];
        sprintf(buf, "Internal error at line %d and file libSage++.cpp, this place was occupied\n", __LINE__);
        addToGlobalBufferAndPrint(buf);
        throw(-1);
    }
#endif
    llndTableClass[NODE_ID(ll)] = pt;
}


void SetMappingInTableForFile(PTR_FILE file, void *pt)
{
    int id;
    if (!file)
        return;
    id = GetFileNum(FILE_FILENAME(file));
    while (allocatedForfileTableClass <= id)
    {
        ReallocatefileTableClass();
    }
    fileTableClass[id] = pt;
}


SgSymbol *GetMappingInTableForSymbol(PTR_SYMB symb)
{
  int id;
  if (!symb)
    return NULL;
  id = SYMB_ID(symb);
  if (allocatedForsymbolTableClass <= id)    
    {
      return NULL;
    }
  return (SgSymbol *) symbolTableClass[id];
}



SgLabel *
GetMappingInTableForLabel(PTR_LABEL lab)
{
  int id;
  if (!lab)
    return NULL;
  id = LABEL_ID(lab);
  if (allocatedForlabelTableClass <= id)    
    {
      return NULL;
    }
  return (SgLabel *) labelTableClass[id];
}


SgStatement *
GetMappingInTableForBfnd(PTR_BFND bf)
{
  int id;
  if (!bf)
    return NULL;
  id = BIF_ID(bf);
  if (allocatedForbfndTableClass <= id)    
    {
      return NULL;
    }
  return (SgStatement *) bfndTableClass[id];
}


SgType *
GetMappingInTableForType(PTR_TYPE t)
{
  int id;
  if (!t)
    return NULL;
  id = TYPE_ID(t);
  if (allocatedFortypeTableClass <= id)    
    {
      return NULL;
    }
  return (SgType *) typeTableClass[id];
}


SgExpression *
GetMappingInTableForLlnd(PTR_LLND ll)
{
    int id;
    if (!ll)
        return NULL;
    id = NODE_ID(ll);
    if (allocatedForllndTableClass <= id)
    {
        return NULL;
    }
    return (SgExpression *)llndTableClass[id];
}


SgFile *
GetMappingInTableForFile(PTR_FILE file)
{
  int id;
  if (!file)
    return NULL;
  id = GetFileNum(FILE_FILENAME(file));
  if (allocatedForfileTableClass <= id)
    {
      return NULL;
    }
  return (SgFile *) fileTableClass[id];
}


//Fortran and C++ Structures
//
// There several families of classes here.
//   Projects-    which correspond to a collection of parsed
//                source files.
//   Files -      which corresponds to an individual source file
//   Statements-  Fortran or C statements
//   Expressions- Fortran or C expression trees.
//   Symbols-     Symbol Table entries.
//   Types-       Each symbol has a type which lives in a type table.
//   Labels-      Statement labels in fortran or C
//   Dependences- Data Dependence Class 
//
//  naming convention: Classnames begin with Sg (for Sage)
//  class functions begin with a lower case and have first letters
//  of words in Caps likeThisWord.
//  
//  In general functions return references when ever possible.  
//
//
// ************* Project and File Types ******************
// the sage fortran 90 and c++ parsers generate files with
// a .dep extension.  A project is a file with a .proj extension
// that consists of a list of .dep files that make the basis
// of the project.  The following describes the
// basic mechanisms to access and modify the structures
// The class hierarch is as follows:
//
//SgProject     = the class representing multi source file projects
//
//SgFile        = the basic source file object.
//   - SgFortranFile  = the subclass for Fortran sources
//   - SgCFile        = the subclass for C files.
//
// ******************************************************************

// forward ref
SgStatement * BfndMapping(PTR_BFND bif);
SgExpression * LlndMapping(PTR_LLND llin);
SgSymbol * SymbMapping(PTR_SYMB symb);
SgType * TypeMapping(PTR_TYPE ty);
SgLabel * LabelMapping(PTR_LABEL label);

// As you can see, some statements are specifically Fortran and
// some apply only to C and C++.
//

// the generic statement class has functions to access or modify any
// property of a given statement.

SgProject *CurrentProject;

#include "libSage++.h"


//
// checking if correct; (better for garbage collecting that way)....
//
void RemoveFromTableLlnd(void * pt)
{
  SgExpression *pte;

  if (!pt)  return;

  pte = (SgExpression *) pt;
  if (pte->thellnd)
    llndTableClass[NODE_ID(pte->thellnd)] = NULL;
}


//
// Some Mapping stuff
//
SgStatement * BfndMapping(PTR_BFND bif)
{
  SgStatement *pt = NULL;
  if (!bif)
    {
      return pt;
    }
  pt = GetMappingInTableForBfnd(bif);
  if (pt)
    return pt;
  else
    {
      pt = new SgStatement(bif);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    }  
  return pt;
}


//
// Some mapping stuff
//

SgExpression * LlndMapping(PTR_LLND llin)
{
  SgExpression *pt;
  if (!llin)
    return NULL;
  pt = GetMappingInTableForLlnd(llin);
  if (pt)
    return pt;
  else
    {
      pt = new SgExpression(llin);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    } 
  return pt;
}


SgSymbol * SymbMapping(PTR_SYMB symb)
{
  SgSymbol *pt = NULL;
  if (!symb)
    {
      return pt;
    }
  pt = GetMappingInTableForSymbol(symb);
  if (pt)
    return pt;
  else
    {
      pt = new SgSymbol(symb);      
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    }  
  return pt;
}

SgType * TypeMapping(PTR_TYPE ty)
{
  SgType *pt = NULL;
  
  if (!ty)
    return NULL;
  pt = GetMappingInTableForType(ty);
  if (pt)
    return pt;
  else
    {
      pt = new SgType(ty);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    } 
  return pt;
}



SgLabel * LabelMapping(PTR_LABEL label)
{
  SgLabel *pt = NULL;
  if (!label)
    {
      return pt;
    }
  pt = GetMappingInTableForLabel(label);
  if (pt)
    return pt;
  else
    {
      pt = new SgLabel(label);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    }  
  return pt;
}



SgValueExp * isSgValueExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case INT_VAL:
    case BOOL_VAL:  /*podd 3.12.11*/
    case CHAR_VAL:
    case FLOAT_VAL:
    case DOUBLE_VAL:
    case STRING_VAL:
    case COMPLEX_VAL:
    case KEYWORD_VAL:
      return (SgValueExp *) pt;
    default:
      return NULL;
    }
}



SgKeywordValExp * isSgKeywordValExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case KEYWORD_VAL:
      return (SgKeywordValExp *) pt;
    default:
      return NULL;
    }
}


SgUnaryExp & makeAnUnaryExpression(int code,PTR_LLND ll1);

// I didn't understand what this function does.
// Should be modified to use LlndMapping.

SgExpression & SgUnaryExp::operand()
{
  PTR_LLND ll;
  SgExpression *pt = NULL;
  
  ll = NODE_OPERAND0(thellnd);
  if (!ll)
    ll = NODE_OPERAND1(thellnd);
  pt = GetMappingInTableForLlnd(ll);
  if (pt)
    return *pt;
  else
    {
      pt = new SgExpression(ll);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    } 
  return *pt;
}

// Other handy constructors
SgUnaryExp &SgDerefOp(SgExpression &e)
  {return makeAnUnaryExpression(DEREF_OP,e.thellnd);}

SgUnaryExp &SgAddrOp(SgExpression &e)
  {return makeAnUnaryExpression(ADDRESS_OP,e.thellnd);}

SgUnaryExp &SgUMinusOp(SgExpression &e)
  {return makeAnUnaryExpression(MINUS_OP,e.thellnd);}

SgUnaryExp &SgUPlusOp(SgExpression &e)
  {return makeAnUnaryExpression(UNARY_ADD_OP,e.thellnd);}

SgUnaryExp &SgPrePlusPlusOp(SgExpression &e)
  {return makeAnUnaryExpression(PLUSPLUS_OP,e.thellnd);}

SgUnaryExp &SgPreMinusMinusOp(SgExpression &e)
  {return makeAnUnaryExpression(MINUSMINUS_OP,e.thellnd);}

SgUnaryExp &SgPostPlusPlusOp(SgExpression &e)
  { SgUnaryExp *pt;
    pt =  &makeAnUnaryExpression(PLUSPLUS_OP,e.thellnd);
    
    NODE_OPERAND1(pt->thellnd) = NODE_OPERAND0(pt->thellnd);
    NODE_OPERAND0(pt->thellnd) = 0;
    return *pt;
  }
SgUnaryExp &SgPostMinusMinusOp(SgExpression &e)
  {
    SgUnaryExp *pt;
    pt =  &makeAnUnaryExpression(MINUSMINUS_OP,e.thellnd);
    
    NODE_OPERAND1(pt->thellnd) = NODE_OPERAND0(pt->thellnd);
    NODE_OPERAND0(pt->thellnd) = 0;
    return *pt;
  }
SgUnaryExp &SgBitCompfOp(SgExpression &e)
  {return makeAnUnaryExpression(BIT_COMPLEMENT_OP,e.thellnd);}
SgUnaryExp &SgNotOp(SgExpression &e)
  {return makeAnUnaryExpression(NOT_OP,e.thellnd);}
SgUnaryExp &SgSizeOfOp(SgExpression &e)
  {return makeAnUnaryExpression(SIZE_OP,e.thellnd);}


// Add type-checking here.
SgUnaryExp &
makeAnUnaryExpression(int code,PTR_LLND ll1)
{
  PTR_LLND ll;
  SgUnaryExp *pt = NULL;

  ll = newExpr(code,NODE_TYPE(ll1),ll1);
  pt = new SgUnaryExp(ll);
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, pt, 1);
#endif
  return *pt;
}

SgUnaryExp * isSgUnaryExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DEREF_OP:
    case ADDRESS_OP:
    case SIZE_OP:
    case MINUS_OP:
    case UNARY_ADD_OP:
    case PLUSPLUS_OP:
    case MINUSMINUS_OP:
    case BIT_COMPLEMENT_OP:
    case NOT_OP:
      return (SgUnaryExp *) pt;
    default:
      return NULL;
    }
}

SgCastExp * isSgCastExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case CAST_OP:
      return (SgCastExp *) pt;
    default:
      return NULL;
    }
}

SgDeleteExp * isSgDeleteExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DELETE_OP:
      return (SgDeleteExp *) pt;
    default:
      return NULL;
    }
}

SgNewExp * isSgNewExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case NEW_OP:
      return (SgNewExp *) pt;
    default:
      return NULL;
    }
}

SgExpression & SgExprIfExp::conditional()
{// expr 1
   PTR_LLND ll;
   SgExpression *pt = NULL;
 
 ll = NODE_OPERAND0(thellnd);
 pt = GetMappingInTableForLlnd(ll);
 if (pt)
   return *pt;
 else
   {
     pt = new SgExpression(ll);
#ifdef __SPF   
     addToCollection(__LINE__, __FILE__, pt, 1);
#endif
   } 
 return *pt;
}

SgExpression & SgExprIfExp::trueExp()
{// expr 2
   PTR_LLND ll = NULL,ll2;
 SgExpression *pt = NULL;
 ll2 = NODE_OPERAND1(thellnd);
 if (ll2)
   ll = NODE_OPERAND0(ll2);
 else
   Message("pb in SgExprIfExp",0);
 pt = GetMappingInTableForLlnd(ll);
 if (pt)
   return *pt;
 else
   {
     pt = new SgExpression(ll);
#ifdef __SPF   
     addToCollection(__LINE__, __FILE__, pt, 1);
#endif
   } 
 return *pt;
}

SgExpression & SgExprIfExp::falseExp()
{// expr 3
   PTR_LLND ll = NULL,ll2;
 SgExpression *pt = NULL;
 ll2 = NODE_OPERAND1(thellnd);
 if (ll2)
   ll = NODE_OPERAND1(ll2);
 else
   Message("pb in SgExprIfExp",0);
 pt = GetMappingInTableForLlnd(ll);
 if (pt)
   return *pt;
 else
   {
     pt = new SgExpression(ll);
#ifdef __SPF   
     addToCollection(__LINE__, __FILE__, pt, 1);
#endif
   } 
 return *pt;
}

void SgExprIfExp::setTrueExp(SgExpression &t)
{
  PTR_LLND ll;
  ll = NODE_OPERAND1(thellnd);
  if (ll)
    NODE_OPERAND0(ll) = t.thellnd;
  else
    {
      NODE_OPERAND1(thellnd)= newExpr(EXPR_IF_BODY,NULL,t.thellnd,NULL);
    }
}

void SgExprIfExp::setFalseExp(SgExpression &f)
{
  PTR_LLND ll;
  ll = NODE_OPERAND1(thellnd);
  if (ll)
    NODE_OPERAND1(ll) = f.thellnd;
  else
    {
      NODE_OPERAND1(thellnd)= newExpr(EXPR_IF_BODY,NULL,NULL,f.thellnd);
    }
}

SgExprIfExp * isSgExprIfExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case EXPR_IF:
      return (SgExprIfExp *) pt;
    default:
      return NULL;
    }
}

SgFunctionCallExp * isSgFunctionCallExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case FUNC_CALL:
      return (SgFunctionCallExp *) pt;
    default:
      return NULL;
    }
}

SgFuncPntrExp * isSgFuncPntrExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case FUNCTION_OP:
      return (SgFuncPntrExp *) pt;
    default:
      return NULL;
    }
}


void SgExprListExp::linkToEnd(SgExpression &arg)
{
  PTR_LLND  lptr;
  lptr = Follow_Llnd(thellnd,2);
  NODE_OPERAND1(lptr) = arg.thellnd;
}


SgExprListExp * isSgExprListExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case EXPR_LIST:
      return (SgExprListExp *) pt;
    default:
      return NULL;
    }
}


SgProject::SgProject(const char * proj_file_name)
{
    // first let init the library we need
    if (!proj_file_name)
    {
        Message("Cannot open project: no file specified", 0);
        exit(1);

    }
    if (open_proj_toolbox(proj_file_name, proj_file_name) < 0)
    {
        fprintf(stderr, "%s   ", proj_file_name);
#if __SPF
        throw -98;
#else
        Message("Cannot open project", 0);
        exit(1);
#endif
    }
    Init_Tool_Box();

    // we have to initialize some specific data for this interface 
    CurrentProject = this;
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


SgFile &SgProject::file(int i)
{
    PTR_FILE file;
    SgFile *pt = NULL;
    file = GetFileWithNum(i);
    SetCurrentFileTo(file);
    SwitchToFile(GetFileNumWithPt(file));
    if (!file)
    {
        Message("SgProject::file; File not found", 0);
        return *pt;
    }
    pt = GetMappingInTableForFile(file);
    if (!pt)
    {
        pt = new SgFile(FILE_FILENAME(file));
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, pt, 1);
#endif

    }

    current_file_id = i;
    current_file = pt;

    return *pt;
}





//  #ifdef NOT_YET_IMPLEMENTED  (No #ifdef because it is used later... PHB)
void SgProject::addFile(char *)
{
  SORRY;
}
//#endif

#ifdef NOT_YET_IMPLEMENTED
void SgProject::deleteFile(SgFile * file)
{
   SORRY;
   return;
}
#endif

const char* SgFile::filename()
{
    return filept->filename;
}

SgFile::SgFile(char * dep_file_name)
{
    filept = GetPointerOnFile(dep_file_name);
    SetCurrentFileTo(filept);
    SwitchToFile(GetFileNumWithPt(filept));
    if (!filept)
    {
        Message("File not found in SgFile; added", 0);
        if (CurrentProject)
            CurrentProject->addFile(dep_file_name);
    }
    SetMappingInTableForFile(filept, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgFile::~SgFile()
{
#if __SPF
    removeFromCollection(this);
#endif
    RemoveFromTableFile((void *)this);
}

SgFile::SgFile(SgFile &f)
{
    filept = f.filept;
#ifndef __SPF
    Message("SgFile: copy constructor not allowed", 0);
#endif

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

extern "C"{
  int new_empty_file(int, const char *);
}

SgFile::SgFile(int Language, const char * dep_file_name)
{

    if (new_empty_file(Language, dep_file_name) == 0)
        Message("create failed", 0);

    filept = GetPointerOnFile(dep_file_name);
    SetCurrentFileTo(filept);
    SwitchToFile(GetFileNumWithPt(filept));
    if (!filept)
    {
        Message("File not found in SgFile; failed!", 0);
        return;
    }
    SetMappingInTableForFile(filept, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

std::map<std::string, std::pair<SgFile*, int> > SgFile::files;
int SgFile::switchToFile(const std::string &name)
{
    std::map<std::string, std::pair<SgFile*, int> >::iterator it = files.find(name);
    if (it == files.end())
        return -1;
    else
    {
        if (current_file_id != it->second.second)
        {
            SgFile *file = &(CurrentProject->file(it->second.second));
            current_file_id = it->second.second;
            current_file = file;
        }
    }

    return it->second.second;
}

void SgFile::addFile(const std::pair<SgFile*, int> &toAdd)
{
    files[toAdd.first->filename()] = toAdd;
}


std::map<int, std::map<std::pair<std::string, int>, SgStatement*> > SgStatement::statsByLine;
std::map<SgExpression*, SgStatement*> SgStatement::parentStatsForExpression;
bool SgStatement::consistentCheckIsActivated = false;

void SgStatement::checkConsistence()
{
#if __SPF
    if (consistentCheckIsActivated && fileID != current_file_id && fileID != -1)
    {
        const int var = variant();
        if (var < 950) // not SPF DIRS
        {
            //unparsestdout();
            char buf[512];
            sprintf(buf, "Internal error at line %d and file libSage++.cpp, file id was inconsistent: current id = %d, was id = %d\n", __LINE__, current_file_id, fileID);
            addToGlobalBufferAndPrint(buf);
            throw(-1);
        }
    }
#endif
}

void SgStatement::updateStatsByLine(std::map<std::pair<std::string, int>, SgStatement*> &toUpdate)
{
    PTR_BFND node = current_file->firstStatement()->thebif;
    for (; node; node = node->thread)
    {
        SgStatement *st = BfndMapping(node);
        toUpdate[std::make_pair(st->fileName(), st->lineNumber())] = st;
    }
}

SgStatement* SgStatement::getStatementByFileAndLine(const std::string &fName, const int lineNum)
{
    const int fildID = SgFile::switchToFile(fName);
    std::map<int, std::map<std::pair<std::string, int>, SgStatement*> >::iterator itID = statsByLine.find(fildID);
    if (itID == statsByLine.end())
        itID = statsByLine.insert(itID, std::make_pair(fildID, std::map<std::pair<std::string, int>, SgStatement*>()));

    if (itID->second.size() == 0)
        updateStatsByLine(itID->second);
    
    std::map<std::pair<std::string, int>, SgStatement*>::iterator itPair = itID->second.find(make_pair(fName, lineNum));
    if (itPair == itID->second.end())
        return NULL;
    else
        return itPair->second;
}

void SgStatement::updateStatsByExpression(SgStatement *where, SgExpression *what)
{
    if (what)
    {
        parentStatsForExpression[what] = where;

        updateStatsByExpression(where, what->lhs());
        updateStatsByExpression(where, what->rhs());
    }
}

void SgStatement::updateStatsByExpression()
{
    SgFile* save = current_file;
    const int save_id = current_file_id;

    for (int i = 0; i < CurrentProject->numberOfFiles(); ++i)
    {
        SgFile *file = &(CurrentProject->file(i));
        current_file_id = i;
        current_file = file;

        PTR_BFND node = current_file->firstStatement()->thebif;
        for (; node; node = node->thread)
        {
            SgStatement *st = BfndMapping(node);
            for (int z = 0; z < 3; ++z)
                updateStatsByExpression(st, st->expr(z));
        }
    }

    CurrentProject->file(save_id);
    current_file_id = save_id;
    current_file = save;
}

SgStatement* SgStatement::getStatmentByExpression(SgExpression *toFind)
{
    if (parentStatsForExpression.size() == 0)
        updateStatsByExpression();

    std::map<SgExpression*, SgStatement*>::iterator itS = parentStatsForExpression.find(toFind);
    if (itS == parentStatsForExpression.end())
        return NULL;
    else
        return itS->second;
}

SgStatement* SgFile::functions(int i)
{
  PTR_BFND bif;
  SgStatement *pt = NULL;

  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  bif = getFunctionNumHeader(i);
  if (!bif)
    {
      Message("SgFile::functions; Function not found",0);
      return pt;
    }
  pt = GetMappingInTableForBfnd(bif);
  if (pt)
    return pt;
  else
    {
      pt = new SgStatement(bif);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    }  
  return pt;
}



SgStatement *SgFile::getStruct(int i)
{  
  PTR_BFND bif;
  SgStatement *pt = NULL;

  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  bif = getStructNumHeader(i);
  if (!bif)
    {
      Message("SgFile::getStruct; Struct not found",0);
        return pt;
    }
  pt = GetMappingInTableForBfnd(bif);
  if (pt)
    return pt;
  else
    {
      pt = new SgStatement(bif);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    }  
  return pt;
}



SgStatement::SgStatement(int variant)
{
    if (!isABifNode(variant))
    {
        Message("Attempt to create a bif node with a variant that is not", 0);
        // arbitrary choice for the variant
        thebif = (PTR_BFND)newNode(BASIC_BLOCK);
    }
    else
        thebif = (PTR_BFND)newNode(variant);
    SetMappingInTableForBfnd(thebif, (void *)this);

    fileID = current_file_id;
    project = CurrentProject;
    unparseIgnore = false;
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgStatement::SgStatement(SgStatement &s)
{
#ifndef __SPF
    Message("SgStatement: copy constructor not allowed", 0);
#endif
    thebif = s.thebif;

#if __SPF
    fileID = s.getFileId();
    project = s.getProject();
    unparseIgnore = s.getUnparseIgnore();

    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


SgStatement::~SgStatement()
{
#if __SPF
    removeFromCollection(this);
#endif
    RemoveFromTableBfnd((void *)this);
}

void SgStatement::insertStmtAfter(SgStatement &s,SgStatement &cp)
{
#ifdef __SPF
    checkConsistence();
#endif
    insertBfndListIn(s.thebif,thebif,cp.thebif); 
}
  

SgStatement::SgStatement(PTR_BFND bif)
{
    thebif = bif;
    SetMappingInTableForBfnd(thebif, (void *)this);

    fileID = current_file_id;
    project = CurrentProject;
    unparseIgnore = false;
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


SgExpression * SgStatement::expr(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
  PTR_LLND ll;
  switch (i)
    {
    case 0:
      ll = BIF_LL1(thebif);
      break;
    case 1:
      ll = BIF_LL2(thebif);
      break;
    case 2:
      ll = BIF_LL3(thebif);
      break;
    default:
       ll = BIF_LL1(thebif);
      Message("A bif node can only have 3 expressions (0,1,2)",BIF_LINE(thebif));
    }
  return LlndMapping(ll);
}




SgLabel *SgStatement::label()
{
#ifdef __SPF
    checkConsistence();
#endif
    PTR_LABEL lab;
    SgLabel *pt = NULL;
    lab = BIF_LABEL(thebif);
    if (!lab)
    {
        //      Message("The bif has no label",BIF_LINE(thebif));
        return pt;
    }
    pt = GetMappingInTableForLabel(lab);
    if (pt)
        return pt;
    else
    {
        pt = new SgLabel(lab);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, pt, 1);
#endif
    }
    return pt;
}

void SgStatement::setExpression(int i, SgExpression &e)
{
#ifdef __SPF
    checkConsistence();
#endif
    switch (i)
    {
    case 0:
        BIF_LL1(thebif) = e.thellnd;
        break;
    case 1:
        BIF_LL2(thebif) = e.thellnd;
        break;
    case 2:
        BIF_LL3(thebif) = e.thellnd;
        break;
    default:
        Message("A bif node can only have 3 expressions (0, 1, 2)", BIF_LINE(thebif));
    }
}

void SgStatement::setExpression(int i, SgExpression *e)
{
#ifdef __SPF
    checkConsistence();
#endif
    switch (i)
    {
    case 0:
        if (e)
            BIF_LL1(thebif) = e->thellnd;
        else
            BIF_LL1(thebif) = NULL;
        break;
    case 1:
        if (e)
            BIF_LL2(thebif) = e->thellnd;
        else
            BIF_LL2(thebif) = NULL;
        break;
    case 2:
        if (e)
            BIF_LL3(thebif) = e->thellnd;
        else
            BIF_LL3(thebif) = NULL;
        break;
    default:
        Message("A bif node can only have 3 expressions (0, 1, 2)", BIF_LINE(thebif));
    }
}


SgStatement* SgStatement::nextInChildList()
{
#ifdef __SPF
    checkConsistence();
#endif
    PTR_BLOB blob;
    SgStatement *x;

    if (BIF_CP(thebif))
    {
        blob = lookForBifInBlobList(BIF_BLOB1(BIF_CP(thebif)), thebif);
        if (!blob)
            blob = lookForBifInBlobList(BIF_BLOB2(BIF_CP(thebif)), thebif);
        if (blob)
            blob = BLOB_NEXT(blob);
        if (blob)
            x = BfndMapping(BLOB_VALUE(blob));
        else x = NULL;
    }
    else
        x = NULL;

    return x;
}

std::string SgStatement::sunparse()
{    
#ifdef __SPF
    checkConsistence();
#endif
    return std::string(unparse());
}


#ifdef NOT_YET_IMPLEMENTED
int  SgStatement::numberOfComments()  
{
  SORRY;
  return 0;
}     
#endif

void SgStatement::addComment(const char *com)
{
#ifdef __SPF
    checkConsistence();
#endif
    LibAddComment(thebif,com);
}

void SgStatement::addComment(char *com)
{
#ifdef __SPF
    checkConsistence();
#endif
    LibAddComment(thebif,com);
}
 
#ifdef NOT_YET_IMPLEMENTED
int  SgStatement::hasAnnotations()
{
  SORRY;
  return 0;
}
#endif

int  SgStatement::IsSymbolInScope(SgSymbol &symb)
{
#ifdef __SPF
    checkConsistence();
#endif
    return  LibIsSymbolInScope(thebif,symb.thesymb);
}

int  SgStatement::IsSymbolReferenced(SgSymbol &symb)   
{
#ifdef __SPF
    checkConsistence();
#endif
    return LibIsSymbolReferenced(thebif,symb.thesymb);
}   

SgExpression::~SgExpression()
{
#if __SPF
    removeFromCollection(this);
#endif
    RemoveFromTableLlnd((void *)this);
}

SgExpression::SgExpression(SgExpression &e)
{
#ifndef __SPF
    Message("SgExpression: copy constructor not allowed", 0);
#endif
    thellnd = e.thellnd;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgExpression::SgExpression(int variant)
{
    if (!isALoNode(variant))
    {
        Message("Attempt to create a low level node with a variant that is not", 0);
        // arbitrary choice for the variant
        thellnd = (PTR_LLND)newNode(EXPR_LIST);
    }
    else
        thellnd = (PTR_LLND)newNode(variant);
    SetMappingInTableForLlnd(thellnd, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


SgExpression::SgExpression(PTR_LLND ll)
{
    thellnd = ll;
    SetMappingInTableForLlnd(thellnd, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgExpression::SgExpression(int variant, SgExpression &lhs, SgExpression &rhs,
    SgSymbol &s, SgType &type)
{
    if (!isALoNode(variant))
    {
        Message("Attempt to create a low level node with a variant that is not", 0);
        // arbitrary choice for the variant
        thellnd = (PTR_LLND)newNode(EXPR_LIST);
    }
    else
        thellnd = (PTR_LLND)newNode(variant);
    SetMappingInTableForLlnd(thellnd, (void *)this);
    NODE_OPERAND0(thellnd) = lhs.thellnd;
    NODE_OPERAND1(thellnd) = rhs.thellnd;
    NODE_SYMB(thellnd) = s.thesymb;
    NODE_TYPE(thellnd) = type.thetype;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

/* Pointer constructor by ajm 26-Jan-94. */
 SgExpression::SgExpression(int variant, SgExpression *lhs, SgExpression *rhs,
     SgSymbol *s, SgType *type)
 {
     if (!isALoNode(variant))
     {
         Message("Attempt to create a low level node with a variant that is not", 0);
         // arbitrary choice for the variant
         thellnd = (PTR_LLND)newNode(EXPR_LIST);
     }
     else
         thellnd = (PTR_LLND)newNode(variant);
     SetMappingInTableForLlnd(thellnd, (void *)this);
     NODE_OPERAND0(thellnd) = ((lhs != 0) ? lhs->thellnd : 0);
     NODE_OPERAND1(thellnd) = ((rhs != 0) ? rhs->thellnd : 0);
     NODE_SYMB(thellnd) = ((s != 0) ? s->thesymb : 0);

     /* If we ever get T_NOTYPE, put that here. */
     NODE_TYPE(thellnd) = ((type != 0) ? type->thetype : 0);

#if __SPF
     addToCollection(__LINE__, __FILE__, this, 1);
#endif
 }

 SgExpression::SgExpression(int variant, SgExpression *lhs, SgExpression *rhs,
     SgSymbol *s)
 {
     if (!isALoNode(variant))
     {
         Message("Attempt to create a low level node with a variant that is not", 0);
         // arbitrary choice for the variant
         thellnd = (PTR_LLND)newNode(EXPR_LIST);
     }
     else
         thellnd = (PTR_LLND)newNode(variant);
     SetMappingInTableForLlnd(thellnd, (void *)this);
     NODE_OPERAND0(thellnd) = ((lhs != 0) ? lhs->thellnd : 0);
     NODE_OPERAND1(thellnd) = ((rhs != 0) ? rhs->thellnd : 0);
     NODE_SYMB(thellnd) = ((s != 0) ? s->thesymb : 0);

#if __SPF
     addToCollection(__LINE__, __FILE__, this, 1);
#endif
 }

SgSymbol *SgExpression::symbol()
{
     /* Value expressions do not have valid symbol pointers */
     if ( isSgValueExp (this) )
          return NULL;
     else
          return SymbMapping(NODE_SYMB(thellnd));
}




SgExpression *SgExpression::operand(int i)
{
  PTR_LLND ll;
  switch (i)
    {
    case 1:
      ll = NODE_OPERAND0(thellnd);
      break;
    case 2:
      ll =  NODE_OPERAND1(thellnd);
      break;
    default:
      ll = NODE_OPERAND0(thellnd);
      Message("A ll node can only have 2 child (1,2)",0);
    }
  return LlndMapping(ll);
}

std::string SgExpression::sunparse()
{
    return std::string(unparse());
}


#define ERR_TOOMANYSYMS -1

int SgExpression::linearRepresentation(int *coeff, SgSymbol **symb, int *cst, int size)
{
    const int maxElem = 300;
    PTR_SYMB *ts = new PTR_SYMB[maxElem];
    int i;
    if (!symb || !coeff || !cst)
        return 0;
    if (size > maxElem)
    {
        Message(" Too many symbols in linearRepresentation ", 0);
        return ERR_TOOMANYSYMS;
    }
    for (i = 0; i < size; i++)
        ts[i] = symb[i]->thesymb;

    int retVal = buildLinearRep(thellnd, coeff, ts, size, cst);
    delete ts;
    return retVal;
}



#ifdef NOT_YET_IMPLEMENTED
SgExpression *SgExpression::normalForm(int n, SgSymbol *s)
{
 SORRY;
 return (SgExpression *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
SgExpression *SgExpression::coefficient(SgSymbol &s)
{
  SORRY;
  return (SgExpression *) NULL;
}
#endif

int SgExpression::isInteger()
{
    int *res;
    int resul = 0;
    res = evaluateExpression(thellnd);
    if (res[0] != -1)
    {
        resul = 1;
    }
#ifdef __SPF   
    removeFromCollection(res);
#endif
    free(res);
    return resul;
}

int SgExpression::valueInteger()
{
    int *res;
    int resul = 0;
    res = evaluateExpression(thellnd);
    if (res[0] != -1)
    {
        resul = res[1];
    }
#ifdef __SPF   
    removeFromCollection(res);
#endif
    free(res);
    return resul;
}

SgExpression &
makeAnBinaryExpression(int code,SgExpression *ll1,SgExpression *ll2)
{
  //SgExpression *resul = NULL;
  if (ll1 && ll2)
    return *LlndMapping(newExpr(code,NODE_TYPE(ll1->thellnd),ll1->thellnd,ll2->thellnd));
  else
    if (ll1) 
      return *LlndMapping(newExpr(code,NODE_TYPE(ll1->thellnd),ll1->thellnd,NULL));
    else
      if (ll2)
        return *LlndMapping(newExpr(code,NODE_TYPE(ll2->thellnd),NULL,ll2->thellnd));
      else
        return *LlndMapping(newExpr(code,NULL,NULL,NULL));
  //return *resul; never reached
}


SgExpression &
makeAnBinaryExpression(int code,PTR_LLND ll1,PTR_LLND ll2)
{

  return *LlndMapping(newExpr(code,NODE_TYPE(ll1),ll1,ll2));
}

SgExpression &operator + ( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(ADD_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &operator - ( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(SUBT_OP,lhs.thellnd,rhs.thellnd);}
 
SgExpression &operator * ( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(MULT_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &operator / ( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(DIV_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &operator % ( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(MOD_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &operator <<( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(LSHIFT_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &operator >>( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(RSHIFT_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &operator < ( SgExpression &lhs, SgExpression &rhs)
{
  return makeAnBinaryExpression(LT_OP,lhs.thellnd,rhs.thellnd);
} 

SgExpression &operator > ( SgExpression &lhs, SgExpression &rhs)
{
  return makeAnBinaryExpression(GT_OP,lhs.thellnd,rhs.thellnd);
} 


SgExpression &operator <= ( SgExpression &lhs, SgExpression &rhs)
{
  if (CurrentProject->Fortranlanguage())
    return makeAnBinaryExpression(LTEQL_OP,lhs.thellnd,rhs.thellnd);
  else
    return makeAnBinaryExpression(LE_OP,lhs.thellnd,rhs.thellnd);
} 

SgExpression &operator >= ( SgExpression &lhs, SgExpression &rhs)
{
  if (CurrentProject->Fortranlanguage())
    return makeAnBinaryExpression(GTEQL_OP,lhs.thellnd,rhs.thellnd);
  else
    return makeAnBinaryExpression(GE_OP,lhs.thellnd,rhs.thellnd);
} 

SgExpression& operator &( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(BITAND_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator |( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(BITOR_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator &&( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(AND_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator ||( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(OR_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator +=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(PLUS_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator &=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(AND_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator *=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(MULT_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator /=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(DIV_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator %=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(MOD_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator ^=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(XOR_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator <<=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(LSHIFT_ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression& operator >>=( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(RSHIFT_ASSGN_OP,lhs.thellnd,rhs.thellnd);}

SgExpression& operator==(SgExpression &lhs, SgExpression &rhs)
{ return SgEqOp(lhs, rhs); }

SgExpression& operator!=(SgExpression &lhs, SgExpression &rhs)
{ return SgNeqOp(lhs, rhs); }

SgExpression &SgAssignOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(ASSGN_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &    SgEqOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(EQ_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &   SgNeqOp( SgExpression &lhs, SgExpression &rhs)
{
  if (CurrentProject->Fortranlanguage())
    return makeAnBinaryExpression(NOTEQL_OP,lhs.thellnd,rhs.thellnd);
  else
    return makeAnBinaryExpression(NE_OP,lhs.thellnd,rhs.thellnd);
} 

SgExpression &SgExprListOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(EXPR_LIST,lhs.thellnd,rhs.thellnd);} 

SgExpression &  SgRecRefOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(RECORD_REF,lhs.thellnd,rhs.thellnd);} 

SgExpression & SgPointStOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(POINTST_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &   SgScopeOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(SCOPE_OP,lhs.thellnd,rhs.thellnd);} 

SgExpression &    SgDDotOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(DDOT,lhs.thellnd,rhs.thellnd);} 

SgExpression & SgBitNumbOp( SgExpression &lhs, SgExpression &rhs)
{return makeAnBinaryExpression(BIT_NUMBER,lhs.thellnd,rhs.thellnd);} 






// For correctness of symbol creation, it is
// necessary to have a symbol table of some form to
// ensure there are no duplicate symbols being 
// created.

SgSymbol::SgSymbol(int variant, const char *name)
{
    if (!isASymbNode(variant))
    {
        Message("Attempt to create a symbol node with a variant that is not", 0);
        // arbitrary choice for the variant
        thesymb = newSymbol(VARIABLE_NAME, name, NULL);
    }
    else
        thesymb = newSymbol(variant, name, NULL);

    SYMB_TYPE(thesymb) = GetAtomicType(T_INT);
    SetMappingInTableForSymb(thesymb, (void *)this);

    fileID = current_file_id;
    project = CurrentProject;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgSymbol::SgSymbol(int variant)
{
    if (!isASymbNode(variant))
    {
        Message("Attempt to create a symbol node with a variant that is not", 0);
        // arbitrary choice for the variant
        thesymb = newSymbol(VARIABLE_NAME, NULL, NULL);
    }
    else
        thesymb = newSymbol(variant, NULL, NULL);
    SYMB_TYPE(thesymb) = GetAtomicType(T_INT);
    SetMappingInTableForSymb(thesymb, (void *)this);

    fileID = current_file_id;
    project = CurrentProject;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgSymbol::SgSymbol(PTR_SYMB symb)
{
    thesymb = symb;
    SetMappingInTableForSymb(thesymb, (void *)this);

    fileID = current_file_id;
    project = CurrentProject;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

#if __SPF
SgSymbol::SgSymbol(const SgSymbol &s)
{
    thesymb = s.thesymb;

    fileID = s.fileID;
    project = s.project;
//    Message("SgSymbol: no copy constructor allowed", 0);
    addToCollection(__LINE__, __FILE__, this, 1);
}
#endif

SgSymbol::SgSymbol(int variant, const char *identifier, SgType &t, SgStatement &scope)
 {
     if (!isASymbNode(variant))
     {
         Message("Attempt to create a symbol node with a variant that is not", 0);
         // arbitrary choice for the variant
         thesymb = newSymbol(VARIABLE_NAME, identifier, NULL);
     }
     else
         thesymb = newSymbol(variant, identifier, NULL);

     SYMB_TYPE(thesymb) = t.thetype;
     SYMB_SCOPE(thesymb) = scope.thebif;
     SetMappingInTableForSymb(thesymb, (void *)this);

     fileID = current_file_id;
     project = CurrentProject;

#if __SPF
     addToCollection(__LINE__, __FILE__, this, 1);
#endif
 }

 SgSymbol::SgSymbol(int variant, const char *identifier, SgType *t, SgStatement *scope)
 {
     if (!isASymbNode(variant))
     {
         Message("Attempt to create a symbol node with a variant that is not", 0);
         // arbitrary choice for the variant
         thesymb = newSymbol(VARIABLE_NAME, identifier, NULL);
     }
     else
         thesymb = newSymbol(variant, identifier, NULL);

     if (t != 0)
     {
         SYMB_TYPE(thesymb) = t->thetype;
     }
     else
     {
         SYMB_TYPE(thesymb) = 0;
     }

     if (scope != 0)
     {
         SYMB_SCOPE(thesymb) = scope->thebif;
     }
     else
     {
         SYMB_SCOPE(thesymb) = 0;
     }

     SetMappingInTableForSymb(thesymb, (void *)this);

     fileID = current_file_id;
     project = CurrentProject;

#if __SPF
     addToCollection(__LINE__, __FILE__, this, 1);
#endif
 }

 SgSymbol::SgSymbol(int variant, const char *identifier, SgStatement &scope)
 {
     if (!isASymbNode(variant))
     {
         Message("Attempt to create a symbol node with a variant that is not", 0);
         // arbitrary choice for the variant
         thesymb = newSymbol(VARIABLE_NAME, identifier, NULL);
     }
     else
         thesymb = newSymbol(variant, identifier, NULL);

     SYMB_TYPE(thesymb) = GetAtomicType(T_INT);
     SYMB_SCOPE(thesymb) = scope.thebif;
     SetMappingInTableForSymb(thesymb, (void *)this);

     fileID = current_file_id;
     project = CurrentProject;

#if __SPF
     addToCollection(__LINE__, __FILE__, this, 1);
#endif
 }


 SgSymbol::SgSymbol(int variant, const char *identifier, SgStatement *scope)
 {
     if (!isASymbNode(variant))
     {
         Message("Attempt to create a symbol node with a variant that is not", 0);
         // arbitrary choice for the variant
         thesymb = newSymbol(VARIABLE_NAME, identifier, NULL);
     }
     else
         thesymb = newSymbol(variant, identifier, NULL);

     SYMB_TYPE(thesymb) = GetAtomicType(T_INT);
     SYMB_SCOPE(thesymb) = (scope == 0) ? 0 : scope->thebif;
     SetMappingInTableForSymb(thesymb, (void *)this);

     fileID = current_file_id;
     project = CurrentProject;

#if __SPF
     addToCollection(__LINE__, __FILE__, this, 1);
#endif
 }

 SgSymbol::~SgSymbol()
 {
#if __SPF
     removeFromCollection(this);
#endif
     RemoveFromTableSymb((void *)this);
 }

SgStatement *SgSymbol::declaredInStmt()
{
  return BfndMapping(LibWhereIsSymbDeclare(thesymb));
  
}

int SgSymbol::attributes()
{
    return SYMB_ATTR(thesymb);
}

void SgSymbol::setAttribute(int attribute)
{
    SYMB_ATTR(thesymb) |= attribute;
}

void SgSymbol::removeAttribute(int attribute)
{
    SYMB_ATTR(thesymb) ^= attribute;
}

SgStatement *SgSymbol::body()
{
  PTR_BFND  bif = NULL;
  PTR_TYPE type;
  // there is a function low_level.c that does it.
  if ((SYMB_CODE(thesymb) == COLLECTION_NAME) ||
      (SYMB_CODE(thesymb) == CLASS_NAME)||
	(SYMB_CODE(thesymb) == TECLASS_NAME))
    {
      type = SYMB_TYPE(thesymb);
      if (type)
        {
          bif = TYPE_COLL_ORI_CLASS(type);
        } else
          {
            Message("Body of collection or class not found",0);
          }
    } else
      {
        if ((SYMB_CODE(thesymb) == FUNCTION_NAME) ||
            (SYMB_CODE(thesymb) == PROGRAM_NAME) ||
            (SYMB_CODE(thesymb) == PROCEDURE_NAME) ||
            (SYMB_CODE(thesymb) == MEMBER_FUNC))
          {
	 	bif = SYMB_FUNC_HEDR(thesymb); // needed, otherwise breaks pC++
		if (!bif)
        	 bif = getFunctionHeader(thesymb);
          } else
            {
              Message("Body not found, may not be implemented yet",0);
              SORRY;
            }	
      }
  
  return BfndMapping(bif);
}




SgType::SgType(int variant)
{
    if (!isATypeNode(variant))
    {
        Message("Attempt to create a type node with a variant that is not", 0);
        // arbitrary choice for the variant
        thetype = (PTR_TYPE)newNode(T_INT);
    }
    else
        thetype = (PTR_TYPE)newNode(variant);
    SetMappingInTableForType(thetype, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


/* This code by Andrew Mauer (ajm) */
/*
  maskDescriptors:

  This routine strips many descriptive type traits which you are probably
  not interested in cloning for variable declarations, etc.

  Returns the getTrueType of the base type being described IF there
  are no descriptors which are not masked out. The following masks
  can be specified as an optional second argument:
        MASK_NO_DESCRIPTORS: Do not mask out anything.
        MASK_MOST_DESCRIPTORS: Only leave in: signed, unsigned, short, long,
	                                      const, volatile.
	MASK_ALL_DESCRIPTORS: Mask out everything. 

  If you build your own mask, you should make sure that the traits
  you want to set out have their bits UN-set, and the rest should have
  their bits set. The complementation (~) operator is a good one to use.

  See libSage++.h, where the MASK_*_DESCRIPTORS variables are defined.
*/

/* Thanks a lot for the stupid $@!@$ #ifdef USER in libSage++.h */
class SgDerivedType;
SgDescriptType *isSgDescriptType(SgType *pt);
SgPointerType *isSgPointerType(SgType *pt);
SgArrayType *isSgArrayType(SgType *pt);
SgDerivedType *isSgDerivedType(SgType *pt);

SgType *SgType::maskDescriptors (int mask)
{
     if ( ! isSgDescriptType(this))
	  return this;
  
     int current_bits_set = isSgDescriptType(this)->modifierFlag();
     
     if ( (current_bits_set & mask ) == 0 )
     {
	  return this->baseType()->getTrueType(mask,0);
     }
     else if ( current_bits_set != (current_bits_set & mask) )
     {
	  /* Mask has changed bits set. Need to build the new type
	     with the unwanted bits masked off. */
	  
	  SgDescriptType *t_new = isSgDescriptType(&this->copy());
	  
	  t_new->setModifierFlag( current_bits_set & mask );
	  
	  return t_new;
     }
     else
     {
	  return this;
     }
}

/* This code by Andrew Mauer (ajm) */
/*
  getTrueType:

  Since Sage stores dereferenced pointers as PTR(-1) -> PTR(1) -> BASE_TYPE,
  we may need to follow the chain of dereferencing to find the type
  which we expect.

  This code currently assumes that:
  o If you follow the dereferencing pointer (PTR(-1)), you find another
  pointer type or an array type. 

  We do NOT assume that the following situation cannot occur:
      PTR(-1) -> PTR(-1) -> PTR(1) -> PTR(1) -> PTR(-1) -> PTR(1)

  This means there may be more pointers to follow after we come to
  an initial "equilibrium".

  ALGORITHM:

  T_POINTER:
     [WARNING: No consideration is given to pointers with attributes
     (ls_flags) set. For instance, a const pointer is treated the same
     as any other pointer.]
     
     1. Return the same type we got if it is not a pointer type or
     the pointer is not a dereferencing pointer type.

     2. Repeat { get next pointer , add its indirection to current total }
     until the current total is 0. We have reached an equilibrium, so
     the next type will not necessarily be a pointer type.

     3. Check the next type for further indirection with another call
     to getTrueType.

  T_DESCRIPT:
     Returns the result of maskDescriptors called with the given type and mask.

  T_ARRAY:
     If the array has zero dimensions, we pass over it. This type arose
     for me in the following situation:
          double x[2];
	  x[1] = 0;
     
  T_DERIVED_TYPE:
     If we have been told to follow typedefs, get the type of the
     symbol from which this type is derived from, and continue digging.
     Otherwise return this type.


  HITCHES:
     Some programs may dereference a T_ARRAY as a pointer, so we need
     to be prepared to deal with that.
  */

SgType *SgType::getTrueType (int mask, int follow_typedefs)
{
     switch (this->variant())
     {
       case T_POINTER:
       {
	    SgType *next = NULL;
	    SgType *current = NULL;
	    int current_indirection;

	    current = this;
	    
	    current_indirection =
		 isSgPointerType(current)->indirection();

	    if (current_indirection > 0)
		 return this;

	    while (current_indirection < 0)
	    {
		 // Get next type
		 next = current->baseType();

		 if ( isSgPointerType (next) )
		 {
		      // add indirection to current
		      current_indirection +=
			   isSgPointerType(next)->indirection();
		 }
		 else if ( isSgArrayType (next) )
		 {
		      /* One level of indirection for each dimension. */
		      current_indirection +=
			   isSgArrayType(next)->dimension();
		 }
		 else
		 {
		      /* Don't know what's going on. Fix me.
		         This includes the case of ptr not having
			 a base type, so next = NULL. */
		      abort();
		 }
		 current = next;
	    }

	    return next->getTrueType(mask, follow_typedefs);
       }
       //break;
	  
       case T_DESCRIPT:
	    return this->maskDescriptors (mask);
            //break;
       case T_DERIVED_TYPE:
       {
	    if ( follow_typedefs )
	    {
		 SgDerivedType *derived_type = isSgDerivedType (this);

		 return
		      (derived_type->typeName()->type())
		      ->getTrueType(mask, follow_typedefs);
	    }
	    else
	    {
		 return this;
	    }
	    //break;
       }
       case T_ARRAY:
       {
	    SgArrayType *the_array = isSgArrayType(this);
	    if (the_array->dimension() == 0)
	    {
		 return the_array->baseType()->getTrueType(mask,
							   follow_typedefs);
	    }
	    else
	    {
		 return this;
	    }
       }
       default:
            return this;
            //break;
     }
}


SgType *SgTypeInt()
{
  return TypeMapping(GetAtomicType(T_INT));
}


SgType *SgTypeChar()
{
  return TypeMapping(GetAtomicType(T_CHAR));
}

SgType *SgTypeFloat()
{
   return TypeMapping(GetAtomicType(T_FLOAT));
}

SgType *SgTypeDouble()
{
  return TypeMapping(GetAtomicType(T_DOUBLE));
}

SgType *SgTypeVoid()
{
  return TypeMapping(GetAtomicType(T_VOID));
}

SgType *SgTypeBool()
{
  return TypeMapping(GetAtomicType(T_BOOL));
}

SgType *SgTypeDefault()
{
  return TypeMapping(GetAtomicType(DEFAULT));
}



//
//
// Subclass for reference to symbol
//
//


SgRefExp * isSgRefExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case CONST_REF:
    case TYPE_REF:
    case INTERFACE_REF:
      return (SgRefExp *) pt;
    default:
      return NULL;
    }
}

#ifdef NOT_YET_IMPLEMENTED
SgExpression * SgVarRefExp::progatedValue()
 {
   SORRY;     // if scalar propogation worked
   return (SgExpression *) NULL;
 }
#endif


SgVarRefExp * isSgVarRefExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case VAR_REF:
      return (SgVarRefExp *) pt;
    default:
      return NULL;
    }
}

SgThisExp * isSgThisExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case THIS_NODE:
      return (SgThisExp *) pt;
    default:
      return NULL;
    }
}


SgArrayRefExp * isSgArrayRefExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case ARRAY_REF:
      return (SgArrayRefExp *) pt;
    default:
      return NULL;
    }
}



SgPntrArrRefExp * isSgPntrArrRefExp(SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case ARRAY_OP:
      return (SgPntrArrRefExp *) pt;
    default:
      return NULL;
    }
}

SgPointerDerefExp * isSgPointerDerefExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DEREF_OP:
      return (SgPointerDerefExp *) pt;
    default:
      return NULL;
    }
}


SgRecordRefExp * isSgRecordRefExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case RECORD_REF:
      return (SgRecordRefExp *) pt;
    default:
      return NULL;
    }
}

SgStructConstExp* isSgStructConstExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case STRUCTURE_CONSTRUCTOR:
      return (SgStructConstExp *) pt;
    default:
      return NULL;
    }
}

SgConstExp* isSgConstExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case CONSTRUCTOR_REF:
      return (SgConstExp *) pt;
    default:
      return NULL;
    }
}


SgVecConstExp * isSgVecConstExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case VECTOR_CONST:
      return (SgVecConstExp *) pt;
    default:
      return NULL;
    }
}

SgInitListExp * isSgInitListExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case INIT_LIST:
      return (SgInitListExp *) pt;
    default:
      return NULL;
    }
}

SgObjectListExp * isSgObjectListExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case EQUI_LIST:
    case NAMELIST_LIST:
    case COMM_LIST:
      return (SgObjectListExp *) pt;
    default:
      return NULL;
    }
}


SgAttributeExp * isSgAttributeExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case PARAMETER_OP:
    case PUBLIC_OP:
    case PRIVATE_OP:
    case ALLOCATABLE_OP:
    case DIMENSION_OP:
    case EXTERNAL_OP:
    case IN_OP:
    case OUT_OP:
    case INOUT_OP:
    case INTRINSIC_OP:
    case POINTER_OP:
    case OPTIONAL_OP:
    case SAVE_OP:
    case TARGET_OP:
      return (SgAttributeExp *) pt;
    default:
      return NULL;
    }
}



SgKeywordArgExp * isSgKeywordArgExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case KEYWORD_ARG:
      return (SgKeywordArgExp *) pt;
    default:
      return NULL;
    }
}

SgSubscriptExp* isSgSubscriptExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DDOT:
      return (SgSubscriptExp *) pt;
    default:
      return NULL;
    }
}

SgUseOnlyExp * isSgUseOnlyExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case ONLY_NODE:
      return (SgUseOnlyExp *) pt;
    default:
      return NULL;
    }
}

SgUseRenameExp * isSgUseRenameExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case RENAME_NODE:
      return (SgUseRenameExp *) pt;
    default:
      return NULL;
    }
}


SgSpecPairExp * isSgSpecPairExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case SPEC_PAIR:
      return (SgSpecPairExp *) pt;
    default:
      return NULL;
    }
}

SgIOAccessExp * isSgIOAccessExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case IOACCESS:
      return (SgIOAccessExp *) pt;
    default:
      return NULL;
    }
}


SgImplicitTypeExp * isSgImplicitTypeExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case IMPL_TYPE:
      return (SgImplicitTypeExp *) pt;
    default:
      return NULL;
    }
}

SgTypeExp * isSgTypeExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case TYPE_OP:
      return (SgTypeExp *) pt;
    default:
      return NULL;
    }
}

SgSeqExp * isSgSeqExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case SEQ:
      return (SgSeqExp *) pt;
    default:
      return NULL;
    }
}

SgStringLengthExp * isSgStringLengthExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case LEN_OP:
      return (SgStringLengthExp *) pt;
    default:
      return NULL;
    }
}

SgDefaultExp * isSgDefaultExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DEFAULT:
      return (SgDefaultExp *) pt;
    default:
      return NULL;
    }
}


SgLabelRefExp * isSgLabelRefExp (SgExpression *pt)
{

  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case LABEL_REF:
      return (SgLabelRefExp *) pt;
    default:
      return NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                                                           //
// We add the subclass for statements  here.                                 //
// Need more comment and so on ........                                      //
// Reorganizing that file may be necessary sometimes                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////



SgProgHedrStmt * isSgProgHedrStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROC_HEDR:
    case FUNC_HEDR:
    case PROG_HEDR:
      return (SgProgHedrStmt *) pt;
    default:
      return NULL;
    }
}

SgProcHedrStmt * isSgProcHedrStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case FUNC_HEDR:
    case PROC_HEDR:
      return (SgProcHedrStmt *) pt;
    default:
      return NULL;
    }
}

SgFunctionType *isSgFunctionType(SgType *);

SgExpression *SgMakeDeclExp(SgSymbol *sym, SgType *t) {
    SgExpression *s = NULL;
    int first = 1, done = 0;
    SgType *tsave = t;
    if ((sym != NULL) && (t != NULL))
        sym->setType(*t);
    while ((!done) && (t != NULL)) {
        // printf("loop var = %d\n", t->variant());
        switch (t->variant()) {
        case T_POINTER:
            if (first) {
                s = new SgVarRefExp(sym);
#ifdef __SPF   
                addToCollection(__LINE__, __FILE__, s, 1);
#endif
                s->setType(*tsave);
            }
            s = &SgDerefOp(*s);
            s->setType(*t); // this is wrong but it is consistant with parser.
            t = t->baseType();
            // s->setType(*t); this should be correct, but because of paser..
            first = 0;
            break;
        case T_REFERENCE:
            if (first) {
                s = new SgVarRefExp(sym);
#ifdef __SPF   
                addToCollection(__LINE__, __FILE__, s, 1);
#endif
                s->setType(*tsave);
            }
            s = &SgAddrOp(*s);
            s->setType(*t); // this is wrong but it is consistant with parser.
            t = t->baseType();
            // s->setType(*t); this should be correct, but because of paser..
            first = 0;
            break;
        case T_ARRAY: {
            SgArrayType *art = isSgArrayType(t);
            if (first) {
                s = new SgArrayRefExp(*sym, *(art->getDimList()));
#ifdef __SPF   
                addToCollection(__LINE__, __FILE__, s, 1);
#endif
            }
            else {
                s = new SgPntrArrRefExp(*s, *(art->getDimList()));
#ifdef __SPF   
                addToCollection(__LINE__, __FILE__, s, 1);
#endif
            }
            t = t->baseType();
            s->setType(*tsave);
            first = 0;
        }
                      break;
        case T_FUNCTION: {
            SgFunctionType *f = isSgFunctionType(t);
            if (s == NULL) { Message("error in AddArg", 0); return NULL; }
            s = new SgFuncPntrExp(*s);
#ifdef __SPF   
            addToCollection(__LINE__, __FILE__, s, 1);
#endif
            t = f->returnedValue();
            s->setType(*t);
            first = 0;
        }
                         break;
        case T_DESCRIPT:
            t = t->baseType();
            break;
        default:
            done = 1;
            if (first) {
                s = new SgVarRefExp(sym);
#ifdef __SPF   
                addToCollection(__LINE__, __FILE__, s, 1);
#endif
                s->setType(*tsave);
            }
            first = 0;
            break;
        }
    }
    return s;
}

SgExpression * SgFuncPntrExp::AddArg(SgSymbol *f,  char *name, SgType &t)
    // to add a parameter to pointer
    // to a function or to a pointer to an array of functions
{
  PTR_SYMB symb;
  SgExpression *arg = NULL;
  SgSymbol *s;
  if(!f) Message("SgFuncPntrExp::AddArg: must have non-null funct. symb",0 );
  s = new SgVariableSymb(name, t, *f->scope()); //create the variable with scope
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, s, 1);
#endif
  symb = s->thesymb;
  appendSymbToArgList(f->thesymb,symb); 

  if(LibFortranlanguage()){
        Message("Fortran function args do not have arg lists", 0);
	}
 else{
        arg = SgMakeDeclExp(s, &t); 
	NODE_OPERAND1(this->thellnd) = 
	    addToExprList(NODE_OPERAND1(this->thellnd),arg->thellnd);
      }
 return arg;
}
	
SgExpression * SgProcHedrStmt::AddArg(char *name, SgType &t)
{
  PTR_SYMB symb;
  PTR_LLND ll;
  SgExpression *arg;
  SgSymbol *s;

  s = new SgVariableSymb(name, t, *this); //create the variable with scope
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, s, 1);
#endif
  symb = s->thesymb;
  appendSymbToArgList(BIF_SYMB(thebif),symb); 

  if(LibFortranlanguage()){
        arg = new SgVarRefExp(*s);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, arg, 1);
#endif
 	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg->thellnd);
        declareAVar(symb,thebif);
	}
 else{
        arg = SgMakeDeclExp(s, &t); 
        ll = BIF_LL1(thebif);
        ll = NODE_OPERAND0(ll);
	NODE_OPERAND0(ll) = addToExprList(NODE_OPERAND0(ll),arg->thellnd);
      }
 return arg;
}

SgExpression * SgProcHedrStmt::AddArg(char *name, SgType &t, SgExpression &init)
{
  PTR_SYMB symb;
  PTR_LLND ll;
  SgExpression *arg, *ref;
  SgSymbol *s;

  if(LibFortranlanguage()){
     Message("no initializer allowed for fortran parameters",0);
     }

  s = new SgVariableSymb(name, t, *this); //create the variable with scope
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, s, 1);
#endif
  symb = s->thesymb;
  appendSymbToArgList(BIF_SYMB(thebif),symb); 
  ref = SgMakeDeclExp(s, &t);
  arg = &SgAssignOp(*ref, init);
  ll = BIF_LL1(thebif);
  ll = NODE_OPERAND0(ll);
  NODE_OPERAND0(ll) = addToExprList(NODE_OPERAND0(ll),arg->thellnd);
  return arg;
}

SgFuncHedrStmt * isSgFuncHedrStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case FUNC_HEDR:
      return (SgFuncHedrStmt *) pt;
    default:
      return NULL;
    }
}

#ifdef NOT_YET_IMPLEMENTED
class  SgModuleStmt: public SgStatement{
        // Fortran 90 Module statement
        // variant ==  MODULE_STMT
     public:
        SgModuleStmt(SgSymbol &moduleName, SgStatement &body):SgStatement(MODULE_STMT)
          {
            SORRY;
          };
        SgModuleStmt(SgSymbol &moduleName):SgStatement(PROG_HEDR)
          {
            SORRY;
          };
        ~SgModuleStmt(){RemoveFromTableBfnd((void *) this);};

        SgSymbol *moduleName()
          {
            SORRY;
          };               // module name 
        void setName(SgSymbol &symbol)
          {
            SORRY;
          };        // set module name 

        int numberOfSpecificationStmts()
          {
            SORRY;
          };
        int numberOfRoutinesDefined()
          {
            SORRY;
          };
        int numberOfFunctionsDefined()
          {
            SORRY;
          };
        int numberOfSubroutinesDefined()
          {
            SORRY;
          };

        SgStatement *specificationStmt(int i)
          {
            SORRY;
          };
        SgStatement *routine(int i)
          {
            SORRY;
          };
        SgStatement *function(int i)
          {
            SORRY;
          };
        SgStatement *subroutine(int i)
          {
            SORRY;
          };

        int isSymbolInScope(SgSymbol &symbol)
          {
            SORRY;
          };
        int isSymbolDeclaredHere(SgSymbol &symbol)
          {
            SORRY;
          };

        SgSymbol &addVariable(SgType &T, char *name)
          {
            SORRY;
          }; 
                                        //add a declaration for new variable

        SgStatement *addCommonBlock(char *blockname, int noOfVars,
                                    SgSymbol *Vars)
          {
            SORRY;
          }; // add a new common block
};


SgModuleStmt * isSgModuleStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case MODULE_STMT:
      return (SgModuleStmt *) pt;
    default:
      return NULL;
    }
}


class  SgInterfaceStmt: public SgStatement{
        // Fortran 90 Operator Interface Statement
        // variant == INTERFACE_STMT
     public:
        SgInterfaceStmt(SgSymbol &name, SgStatement &body, SgStatement &scope):SgStatement(INTERFACE_STMT)
          {
            SORRY;
          };
        ~SgInterfaceStmt(){RemoveFromTableBfnd((void *) this);};

        SgSymbol *interfaceName()
          {
            SORRY;
          };               // interface name if given
        int setName(SgSymbol &symbol)
          {
            SORRY;
          };           // set interface name 

        int numberOfSpecificationStmts()
          {
            SORRY;
          };

        SgStatement *specificationStmt(int i)
          {
            SORRY;
          };

        int isSymbolInScope(SgSymbol &symbol)
          {
            SORRY;
          };
        int isSymbolDeclaredHere(SgSymbol &symbol)
          {
            SORRY;
          };
};


SgInterfaceStmt * isSgInterfaceStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case INTERFACE_STMT:
      return (SgInterfaceStmt *) pt;
    default:
      return NULL;
    }
}


class  SgBlockDataStmt: public SgStatement{
        // Fortran Block Data statement
        // variant == BLOCK_DATA
     public:
        SgBlockDataStmt(SgSymbol &name, SgStatement &body):SgStatement(BLOCK_DATA)
          {
            BIF_SYMB(thebif) = name.thesymb;
            insertBfndListIn(body.thebif,thebif,thebif);            
          };
        ~SgBlockDataStmt(){RemoveFromTableBfnd((void *) this);};

        SgSymbol *name()  // block data name if given
          { return SymbMapping(BIF_SYMB(thebif)); };
        int setName(SgSymbol &symbol)
          { 
            BIF_SYMB(thebif) = symbol.thesymb;
            return 1;
          };           // set block data name 

        int isSymbolInScope(SgSymbol &symbol)
          {
            SORRY;
          };
        int isSymbolDeclaredHere(SgSymbol &symbol)
          {
            SORRY;
          };
};



SgBlockDataStmt * isSgBlockDataStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case BLOCK_DATA:
      return (SgBlockDataStmt *) pt;
    default:
      return NULL;
    }
}
#endif

SgClassStmt * isSgClassStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case CLASS_DECL:
    case TECLASS_DECL:
    case STRUCT_DECL:
    case UNION_DECL:
    case ENUM_DECL:
    case COLLECTION_DECL:
      return (SgClassStmt *) pt;
    default:
      return NULL;
    }
}


SgStructStmt * isSgStructStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case STRUCT_DECL:
      return (SgStructStmt *) pt;
    default:
      return NULL;
    }
}


SgUnionStmt * isSgUnionStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case UNION_DECL:
      return (SgUnionStmt *) pt;
    default:
      return NULL;
    }
}

SgEnumStmt * isSgEnumStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ENUM_DECL:
      return (SgEnumStmt *) pt;
    default:
      return NULL;
    }
}

SgCollectionStmt * isSgCollectionStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case COLLECTION_DECL:
      return (SgCollectionStmt *) pt;
    default:
      return NULL;
    }
}


SgBasicBlockStmt * isSgBasicBlockStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case BASIC_BLOCK:
      return (SgBasicBlockStmt *) pt;
    default:
      return NULL;
    }
}



SgForStmt * isSgForStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case FOR_NODE :
      return (SgForStmt *) pt;
    default:
      return NULL;
    }
}

SgProcessDoStmt * isSgProcessDoStmt (SgStatement *pt)
{
 
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROCESS_DO_STAT :
      return (SgProcessDoStmt *) pt;
    default:
      return NULL;
    }
}

SgWhileStmt * isSgWhileStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case WHILE_NODE:
      return (SgWhileStmt *) pt;
    default:
      return NULL;
    }
}

SgDoWhileStmt * isSgDoWhileStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case DO_WHILE_NODE:
      return (SgDoWhileStmt *) pt;
    default:
      return NULL;
    }
}

SgLogIfStmt * isSgLogIfStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case LOGIF_NODE:
      return (SgLogIfStmt *) pt;
    default:
      return NULL;
    }
}


SgIfStmt * isSgIfStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case IF_NODE:
      return (SgIfStmt *) pt;
    default:
      return NULL;
    }
}

#ifdef NOT_YET_IMPLEMENTED
class  SgIfElseIfStmt: public SgIfStmt {
        // For Fortran if then elseif .. elseif ... case
        // variant == ELSEIF_NODE
     public:
        SgIfElseIfStmt(SgExpression &condList, SgStatement &blockList,
                       SgSymbol &constructName):SgIfStmt(ELSEIF_NODE)
          {
            SORRY;
          }; 
        int numberOfConditionals()
          {
            SORRY;
          };       // the number of conditionals
        SgStatement *body(int b)
          {
            SORRY;
          };          // block b
        void setBody(int b)
          {
            SORRY;
          };              // sets block 
        SgExpression *conditional(int i)
          {
            SORRY;
          }; // the i-th conditional
        void setConditional(int i)
          {
            SORRY;
          };       // sets the i-th conditional
        void addClause(SgExpression &cond, SgStatement &block)
          {
            SORRY;
          };
        void removeClause(int b)
          {
            SORRY;
          };          // removes block b and it's conditional

};


SgIfElseIfStmt * isSgIfElseIfStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ELSEIF_NODE:
      return (SgIfElseIfStmt *) pt;
    default:
      return NULL;
    }
}
#endif

SgArithIfStmt * isSgArithIfStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ARITHIF_NODE:
      return (SgArithIfStmt *) pt;
    default:
      return NULL;
    }
}

SgWhereStmt * isSgWhereStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case WHERE_NODE:
      return (SgWhereStmt *) pt;
    default:
      return NULL;
    }
}


SgWhereBlockStmt * isSgWhereBlockStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case WHERE_BLOCK_STMT:
      return (SgWhereBlockStmt *) pt;
    default:
      return NULL;
    }
}


SgSwitchStmt * isSgSwitchStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case SWITCH_NODE:
      return (SgSwitchStmt *) pt;
    default:
      return NULL;
    }
}



SgCaseOptionStmt * isSgCaseOptionStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case CASE_NODE:
      return (SgCaseOptionStmt *) pt;
    default:
      return NULL;
    }
}

// ******************** Leaf Executable Nodes ***********************


SgExecutableStatement* isSgExecutableStatement(SgStatement *pt)
{
    if (!pt)
        return NULL;
    if (!isADeclBif(BIF_CODE(pt->thebif)))
    {
#if __SPF
        const int var = pt->variant();
        if (var == CONTROL_END)
        {
            SgStatement *cp = pt->controlParent();
            if (cp->variant() == PROG_HEDR || cp->variant() == PROC_HEDR || cp->variant() == FUNC_HEDR)
                return (SgExecutableStatement*)pt;
            else
                return isSgExecutableStatement(cp);
        }
        else if (var == DVM_INHERIT_DIR || var == DVM_ALIGN_DIR || var == DVM_DYNAMIC_DIR ||
                 var == DVM_DISTRIBUTE_DIR || var == DVM_VAR_DECL || var == DVM_SHADOW_DIR ||
                 var == DVM_HEAP_DIR || var == DVM_CONSISTENT_DIR || var == DVM_POINTER_DIR ||
                 var == HPF_TEMPLATE_STAT || var == HPF_PROCESSORS_STAT || var == DVM_TASK_DIR || 
                 var == DVM_INDIRECT_GROUP_DIR || var == DVM_REMOTE_GROUP_DIR || var == DVM_REDUCTION_GROUP_DIR ||
                 var == DVM_CONSISTENT_GROUP_DIR || var == DVM_ASYNCID_DIR)
            return NULL;
        else
            return (SgExecutableStatement*)pt;
#else
        return (SgExecutableStatement*)pt;
#endif
    }
    else
    {
#if __SPF
        const int var = pt->variant();
        if (var == SPF_PARALLEL_DIR)
            return (SgExecutableStatement *)pt;
        if (var == SPF_ANALYSIS_DIR || var == SPF_PARALLEL_REG_DIR)
            return isSgExecutableStatement(pt->lexNext());
        if (var == SPF_END_PARALLEL_REG_DIR)
            return isSgExecutableStatement(pt->lexPrev());
        if (var == SPF_TRANSFORM_DIR)
        {
            SgExpression *ex = pt->expr(0);
            while (ex)
            {
                if (ex->lhs()->variant() == SPF_NOINLINE_OP)
                    return NULL;
                else if (ex->lhs()->variant() == SPF_FISSION_OP || ex->lhs()->variant() == SPF_PRIVATES_EXPANSION_OP)
                    return (SgExecutableStatement *)pt;

                ex = ex->rhs();
            }
        }

        if (var == DVM_PARALLEL_ON_DIR || var == ACC_REGION_DIR || var == ACC_END_REGION_DIR || var == DVM_EXIT_INTERVAL_DIR)
            return (SgExecutableStatement *)pt;
        if (var == DVM_INTERVAL_DIR)
            return isSgExecutableStatement(pt->lexNext());
        if (var == DVM_ENDINTERVAL_DIR)
            return isSgExecutableStatement(pt->lexPrev());
        if (var == DVM_BARRIER_DIR)
            return (SgExecutableStatement *)pt;
        if (var == DVM_INHERIT_DIR)
            return NULL;
        if (var == DVM_INHERIT_DIR || var == DVM_ALIGN_DIR || var == DVM_DYNAMIC_DIR ||
            var == DVM_DISTRIBUTE_DIR || var == DVM_VAR_DECL || var == DVM_SHADOW_DIR ||
            var == DVM_HEAP_DIR || var == DVM_CONSISTENT_DIR || var == DVM_POINTER_DIR)
            return NULL;
#endif
        return NULL;
    }
}

SgAssignStmt * isSgAssignStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ASSIGN_STAT:
      return (SgAssignStmt *) pt;
    default:
      return NULL;
    }
}


SgCExpStmt * isSgCExpStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case EXPR_STMT_NODE:
      return (SgCExpStmt *) pt;
    default:
      return NULL;
    }
}


SgPointerAssignStmt * isSgPointerAssignStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case POINTER_ASSIGN_STAT:
      return (SgPointerAssignStmt *) pt;
    default:
      return NULL;
    }
}

SgHeapStmt * isSgHeapStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ALLOCATE_STMT:
    case DEALLOCATE_STMT:
      return (SgHeapStmt *) pt;
    default:
      return NULL;
    }
}
   
SgNullifyStmt * isSgNullifyStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case NULLIFY_STMT:
      return (SgNullifyStmt *) pt;
    default:
      return NULL;
    }
}

SgContinueStmt * isSgContinueStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case CONT_STAT:
      return (SgContinueStmt *) pt;
    default:
      return NULL;
    }
}
   

SgControlEndStmt * isSgControlEndStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case CONTROL_END :
      return (SgControlEndStmt *) pt;
    default:
      return NULL;
    }
}


SgBreakStmt * isSgBreakStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case BREAK_NODE:
      return (SgBreakStmt *) pt;
    default:
      return NULL;
    }
}


SgCycleStmt * isSgCycleStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case CYCLE_STMT:
      return (SgCycleStmt *) pt;
    default:
      return NULL;
    }
}


SgReturnStmt * isSgReturnStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case RETURN_NODE:
    case RETURN_STAT:
      return (SgReturnStmt *) pt;
    default:
      return NULL;
    }
}

SgExitStmt * isSgExitStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case EXIT_STMT:
      return (SgExitStmt *) pt;
    default:
      return NULL;
    }
}

SgGotoStmt * isSgGotoStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case GOTO_NODE:
      return (SgGotoStmt *) pt;
    default:
      return NULL;
    }
}


SgLabelListStmt * isSgLabelListStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case COMGOTO_NODE:
    case  ASSGOTO_NODE:
      return (SgLabelListStmt *) pt;
    default:
//      SORRY;
      return NULL;
    }
}


SgAssignedGotoStmt * isSgAssignedGotoStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ASSGOTO_NODE:
      return (SgAssignedGotoStmt *) pt;
    default:
      return NULL;
    }
}

SgComputedGotoStmt * isSgComputedGotoStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case COMGOTO_NODE:
      return (SgComputedGotoStmt *) pt;
    default:
      return NULL;
    }
}

SgStopOrPauseStmt * isSgStopOrPauseStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case STOP_STAT:
      return (SgStopOrPauseStmt *) pt;
    default:
      return NULL;
    }
}

SgCallStmt* isSgCallStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROC_STAT:
      return (SgCallStmt *) pt;
    default:
      return NULL;
    }
}

SgProsHedrStmt* isSgProsHedrStmt (SgStatement *pt) /* Fortran M */
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROS_HEDR:
      return (SgProsHedrStmt *) pt;
    default:
      return NULL;
    }
}

SgProsCallStmt* isSgProsCallStmt (SgStatement *pt) /* Fortran M */
{
 
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROS_STAT:
      return (SgProsCallStmt *) pt;
    default:
      return NULL;
    }
}

SgProsCallLctn* isSgProsCallLctn (SgStatement *pt) /* Fortran M */
{
 
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROS_STAT_LCTN:
      return (SgProsCallLctn *) pt;
    default:
      return NULL;
    }
}

SgProsCallSubm* isSgProsCallSubm (SgStatement *pt) /* Fortran M */
{
 
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROS_STAT_SUBM:
      return (SgProsCallSubm *) pt;
    default:
      return NULL;
    }
}

SgIOStmt * isSgIOStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case 0:
      return (SgIOStmt *) pt;
    default:
      SORRY;
      return NULL;
    }
}


SgInputOutputStmt * isSgInputOutputStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case READ_STAT:
    case WRITE_STAT:
    case PRINT_STAT:
      return (SgInputOutputStmt *) pt;
    default:
      return NULL;
    }
}

SgIOControlStmt::SgIOControlStmt(int variant, SgExpression &controlSpecifierList):SgExecutableStatement(variant)
{
  switch (variant){
  case OPEN_STAT:
  case CLOSE_STAT:
  case INQUIRE_STAT:
  case BACKSPACE_STAT:
  case REWIND_STAT:
  case ENDFILE_STAT:
  case FORMAT_STAT:
    break;
  default:
    Message("illegal variant for SgIOControlStmt",0);
  }
  
  BIF_LL2(thebif) = controlSpecifierList.thellnd;
}

SgIOControlStmt * isSgIOControlStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
      case OPEN_STAT:
    case CLOSE_STAT:
    case INQUIRE_STAT:
    case BACKSPACE_STAT:
    case REWIND_STAT:
    case ENDFILE_STAT:
    case FORMAT_STAT:
      return (SgIOControlStmt *) pt;
    default:
      return NULL;
    }
}

// ******************** Declaration Nodes ***************************

SgDeclarationStatement * isSgDeclarationStatement (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case VAR_DECL:
    case VAR_DECL_90:
    case ENUM_DECL:
    case STRUCT_DECL:
    case CLASS_DECL:
    case TECLASS_DECL:
    case COLLECTION_DECL:
      return (SgDeclarationStatement *) pt;
    default:
      return NULL;
    }
}

// the complete initial value ASSGN_OP expression ofthe i-th variable
// from Michael Golden
SgExpression * SgVarDeclStmt::completeInitialValue(int i)
{ 
  PTR_LLND varRefExp;
  SgExpression *x;

  varRefExp = getPositionInExprList(BIF_LL1(thebif),i);
  if (varRefExp == LLNULL)
    x = NULL;
  else if (NODE_CODE(varRefExp) == ASSGN_OP)
    x = LlndMapping(varRefExp);
  else 
    x = NULL;

  return x;
}


// sets the initial value ofthe i-th variable
// an alternative way to initialize variables. The low-level node 
// (VAR_REF or ARRAY_REF) is replaced by a ASSIGN_OP low-level node.
void SgVarDeclStmt::setInitialValue(int i, SgExpression &initVal) // sets the initial value ofthe i-th variable
{
  int j;
  SgExpression *list, *varRef;
  list = this->expr(0);
  for(j = 0; j < i; j++) if(list) list = list->rhs();
  if(!list) return;
  varRef = list->lhs();
  if(!varRef) return;
  if(varRef->variant() == ASSGN_OP){
	 varRef->setRhs(initVal);
	 return;
	}
  SgExpression &e = SgAssignOp(*varRef, initVal);
  list->setLhs(e);
  return;
}
  
// method below contributed by Michael Golden 
// removes the initial value of the i-ith declaration
 void SgVarDeclStmt::clearInitialValue(int i)
 {
     int j;
     SgExpression *list, *varRef;  
 
     list = this->expr(0);
     for(j = 0; j < i; j++) 
       if (list) 
 	list = list->rhs();
     if(!list) 
       return;
     varRef = list->lhs();
     if(!varRef) 
       return;
     
     /* If there is an assignment here, then change it to just the LHS */
     /* Which is the variable itself                                   */
     if (varRef->variant() == ASSGN_OP) 
       list->setLhs(*(varRef->lhs()));
 
 
 }


SgVarDeclStmt * isSgVarDeclStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case VAR_DECL:
      return (SgVarDeclStmt *) pt;
    default:
      return NULL;
    }
}


SgIntentStmt * isSgIntentStmt (SgStatement *pt) /* Fortran M */
{
 
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case INTENT_STMT:
      return (SgIntentStmt *) pt;
    default:
      return NULL;
    }
}


SgVarListDeclStmt::SgVarListDeclStmt(int variant, SgExpression &):SgDeclarationStatement(variant)
          {
            switch (variant) {
              case INTENT_STMT:
              case OPTIONAL_STMT:
              case SAVE_DECL:
              case PUBLIC_STMT:
              case PRIVATE_STMT:
              case EXTERN_STAT:
              case INTRIN_STAT:
              case DIM_STAT:
              case ALLOCATABLE_STMT:
              case POINTER_STMT:
              case TARGET_STMT:
              case MODULE_PROC_STMT:
                   break;
              default:
                   Message("Illegal variant for SgVarListDeclStmt",0);
            };

//            findStatementAttribute(variant, attribute);
//            BIF_LL1(thesymb) = symbolrefList.thellnd;
//            setSymbolAttributesInVarRefList(BIF_LL1(thesymb));
            SORRY;
          }

SgVarListDeclStmt::SgVarListDeclStmt(int variant, SgSymbol &, SgStatement &):SgDeclarationStatement(variant)
          {
            switch (variant) {
              case INTENT_STMT:
              case OPTIONAL_STMT:
              case SAVE_DECL:
              case PUBLIC_STMT:
              case PRIVATE_STMT:
              case EXTERN_STAT:
              case INTRIN_STAT:
              case DIM_STAT:
              case ALLOCATABLE_STMT:
              case POINTER_STMT:
              case TARGET_STMT:
              case MODULE_PROC_STMT:
                   break;
              default:
                   Message("Illegal variant for SgVarListDeclStmt",0);
            };

//            findStatementAttribute(variant,attribute);
//            BIF_LL1(thesymb) = symbolList.thellnd;
//            setSymbolAttributesInVarRefList(BIF_LL1(thesymb));
            SORRY;
          }

SgVarListDeclStmt * isSgVarListDeclStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case INTENT_STMT:
    case OPTIONAL_STMT:
    case SAVE_DECL:
    case PUBLIC_STMT:
    case PRIVATE_STMT:
    case EXTERN_STAT:
    case INTRIN_STAT:
    case DIM_STAT:
    case ALLOCATABLE_STMT:
    case POINTER_STMT:
    case TARGET_STMT:
    case MODULE_PROC_STMT:
    case PROCESSORS_STAT:
    case STATIC_STMT:
      return (SgVarListDeclStmt *) pt;
    default:
      return NULL;
    }
}
     
     

SgStructureDeclStmt * isSgStructureDeclStmtSgStructureDeclStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case STRUCT_DECL:
      return (SgStructureDeclStmt *) pt;
    default:
      return NULL;
    }
}

SgNestedVarListDeclStmt::SgNestedVarListDeclStmt(int variant, SgExpression &listOfVarList):SgDeclarationStatement(VAR_DECL)
{
  int listVariant;
  
  switch (variant) {
  case NAMELIST_STAT:
    listVariant = NAMELIST_LIST;
    break;
  case EQUI_STAT:
    listVariant = EQUI_LIST;
    break;
  case COMM_STAT:
    listVariant = COMM_LIST;
    break;
  case PROS_COMM:  /* Fortran M */
    listVariant = COMM_LIST;
    break;
  default:
    Message("Illegal variant in SgNestedVarListDeclStmt",0);
  };
  BIF_CODE(thebif) = variant;
//            checkIfListOfVariant(listVariant, listOfVarList);            
  listVariant = listVariant; SORRY;
  BIF_LL1(thebif) = listOfVarList.thellnd;
}

SgNestedVarListDeclStmt * isSgNestedVarListDeclStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case NAMELIST_STAT:
    case EQUI_STAT:
    case PROS_COMM:
    case COMM_STAT:
      return (SgNestedVarListDeclStmt *) pt;
    default:
      return NULL;
    }
}

     

SgParameterStmt * isSgParameterStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PARAM_DECL:
      return (SgParameterStmt *) pt;
    default:
      return NULL;
    }
}
    

SgImplicitStmt * isSgImplicitStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case IMPL_DECL:
      return (SgImplicitStmt *) pt;
    default:
      return NULL;
    }
}


SgInportStmt * isSgInportStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case INPORT_DECL:
      return (SgInportStmt *) pt;
    default:
      return NULL;
    }
}
 
 
SgOutportStmt * isSgOutportStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case OUTPORT_DECL:
      return (SgOutportStmt *) pt;
    default:
      return NULL;
    }
}
 
 
SgChannelStmt * isSgChannelStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case CHANNEL_STAT:
      return (SgChannelStmt *) pt;
    default:
      return NULL;
    }
}


SgMergerStmt * isSgMergerStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case MERGER_STAT:
      return (SgMergerStmt *) pt;
    default:
      return NULL;
    }
}
 

SgMoveportStmt * isSgMoveportStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case MOVE_PORT:
      return (SgMoveportStmt *) pt;
    default:
      return NULL;
    }
}
 

SgSendStmt * isSgSendStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case SEND_STAT:
      return (SgSendStmt *) pt;
    default:
      return NULL;
    }
}
 

SgReceiveStmt * isSgReceiveStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case RECEIVE_STAT:
      return (SgReceiveStmt *) pt;
    default:
      return NULL;
    }
}
 

SgEndchannelStmt * isSgEndchannelStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case ENDCHANNEL_STAT:
      return (SgEndchannelStmt *) pt;
    default:
      return NULL;
    }
}
 

SgProbeStmt * isSgProbeStmt(SgStatement *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case PROBE_STAT:
      return (SgProbeStmt *) pt;
    default:
      return NULL;
    }
}


SgProcessorsRefExp * isSgProcessorsRefExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case PROCESSORS_REF:
      return (SgProcessorsRefExp *) pt;
    default:
      return NULL;
    }
}


SgPortTypeExp * isSgPortTypeExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case PORT_TYPE_OP:
    case INPORT_TYPE_OP:
    case OUTPORT_TYPE_OP:
      return (SgPortTypeExp *) pt;
    default:
      return NULL;
    }
}

SgInportExp * isSgInportExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case INPORT_NAME:
      return (SgInportExp *) pt;
    default:
      return NULL;
    }
}

SgOutportExp * isSgOutportExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case OUTPORT_NAME:
      return (SgOutportExp *) pt;
    default:
      return NULL;
    }
}

SgFromportExp * isSgFromportExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case FROMPORT_NAME:
      return (SgFromportExp *) pt;
    default:
      return NULL;
    }
}

SgToportExp * isSgToportExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case TOPORT_NAME:
      return (SgToportExp *) pt;
    default:
      return NULL;
    }
}

SgIO_statStoreExp * isSgIO_statStoreExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case IOSTAT_STORE:
      return (SgIO_statStoreExp *) pt;
    default:
      return NULL;
    }
}

SgEmptyStoreExp * isSgEmptyStoreExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case EMPTY_STORE:
      return (SgEmptyStoreExp *) pt;
    default:
      return NULL;
    }
}

SgErrLabelExp * isSgErrLabelExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case ERR_LABEL:
      return (SgErrLabelExp *) pt;
    default:
      return NULL;
    }
}

SgEndLabelExp * isSgEndLabelExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case END_LABEL:
      return (SgEndLabelExp *) pt;
    default:
      return NULL;
    }
}

SgDataImpliedDoExp * isSgDataImpliedDoExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DATA_IMPL_DO:
      return (SgDataImpliedDoExp *) pt;
    default:
      return NULL;
    }
}

SgDataEltExp * isSgDataEltExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DATA_ELT:
      return (SgDataEltExp *) pt;
    default:
      return NULL;
    }
}

SgDataSubsExp * isSgDataSubsExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DATA_SUBS:
      return (SgDataSubsExp *) pt;
    default:
      return NULL;
    }
}

SgDataRangeExp * isSgDataRangeExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case DATA_RANGE:
      return (SgDataRangeExp *) pt;
    default:
      return NULL;
    }
}

SgIconExprExp * isSgIconExprExp(SgExpression *pt) /* Fortran M */
{
  if (!pt)
    return NULL;
  switch(NODE_CODE(pt->thellnd))
    {
    case ICON_EXPR:
      return (SgIconExprExp *) pt;
    default:
      return NULL;
    }
}




#ifdef NOT_YET_IMPLEMENTED
class SgUseStmt: public SgDeclarationStatement{
       // Fortran 90 module usuage statement
       // variant = USE_STMT
   public:
     SgUseStmt(SgSymbol &moduleName, SgExpression &renameList, SgStatement &scope):SgDeclarationStatement(USE_STMT)
          {
            SORRY;
          };
          // renameList must be a list of low-level nodes of variant RENAME_NODE
     ~SgUseStmt(){RemoveFromTableBfnd((void *) this);};

     int isOnly()
          {
            SORRY;
          };
     SgSymbol *moduleName()
          {
            SORRY;
          };
     void setModuleName(SgSymbol &moduleName)
          {
            SORRY;
          };
     int numberOfRenames()
          {
            SORRY;
          };
     SgExpression *renameNode(int i)
          {
            SORRY;
          };
     void  addRename(SgSymbol &localName, SgSymbol &useName)
          {
            SORRY;
          };
     void  addRenameNode(SgExpression &renameNode)
          {
            SORRY;
          };
     void  deleteRenameNode(int i)
          {
            SORRY;
          };
     void deleteTheRenameNode(SgExpression &renameNode)
          {
            SORRY;
          };
};


SgUseStmt * isSgUseStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case USE_STMT:
      return (SgUseStmt *) pt;
    default:
      return NULL;
    }
}



class  SgStmtFunctionStmt: public SgDeclarationStatement{
        // Fortran statement function declaration
        // variant == STMTFN_DECL
     public:        
        SgStmtFunctionStmt(SgSymbol &name, SgExpression &args, SgStatement Body):SgDeclarationStatement(STMTFN_DECL)
          {
            SORRY;
          };
        ~SgStmtFunctionStmt(){RemoveFromTableBfnd((void *) this);};

        SgSymbol *name()
          {
            SORRY;
          };
        void setName(SgSymbol &name)
          {
            SORRY;
          };
        SgType *type()
          {
            SORRY;
          };
        int numberOfParameters()
          {
            SORRY;
          };       // the number of parameters
        SgSymbol *parameter(int i)
          {
            SORRY;
          };     // the i-th parameter
};      

class  SgMiscellStmt: public SgDeclarationStatement{
        // Fortran 90 simple miscellaneous statements
        // variant == CONTAINS_STMT, PRIVATE_STMT, SEQUENCE_STMT
     public:        
        SgMiscellStmt(int variant):SgDeclarationStatement(variant) {}
        ~SgMiscellStmt(){RemoveFromTableBfnd((void *) this);};
};      



SgStmtFunctionStmt * isSgStmtFunctionStmt (SgStatement *pt)
{

  if (!pt)
    return NULL;
  switch(BIF_CODE(pt->thebif))
    {
    case STMTFN_DECL:
      return (SgStmtFunctionStmt *) pt;
    default:
      return NULL;
    }
}
#endif

//
//
// More stuffs for types and symbols
//
//


SgVariableSymb * isSgVariableSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case VARIABLE_NAME:
      return (SgVariableSymb *) pt;
    default:
      return NULL;
    }
}


SgConstantSymb * isSgConstantSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case CONST_NAME :
      return (SgConstantSymb *) pt;
    default:
      return NULL;
    }
}

SgFunctionSymb::SgFunctionSymb(int variant):SgSymbol(variant)
{
  switch (variant) {
  case PROGRAM_NAME:
  case PROCEDURE_NAME:
  case FUNCTION_NAME:
  case MEMBER_FUNC:
    break;
  default:
    Message("SgFunctionSymb variant invalid",0);
  }
}

SgFunctionSymb::SgFunctionSymb(int variant, char *identifier, SgType &t, 
                               SgStatement &scope):SgSymbol(variant,identifier,t,scope)
{
  switch (variant) {
  case PROGRAM_NAME:
  case PROCEDURE_NAME:
  case FUNCTION_NAME:
  case MEMBER_FUNC:
    break;
  default:
    Message("SgFunctionSymb variant invalid",0);
  }
  SYMB_TYPE(thesymb) = t.thetype;
}

SgFunctionSymb::SgFunctionSymb(int variant, const char *identifier, SgType &t,
    SgStatement &scope) :SgSymbol(variant, identifier, t, scope)
{
    switch (variant) {
    case PROGRAM_NAME:
    case PROCEDURE_NAME:
    case FUNCTION_NAME:
    case MEMBER_FUNC:
        break;
    default:
        Message("SgFunctionSymb variant invalid", 0);
    }
    SYMB_TYPE(thesymb) = t.thetype;
}

SgExpression * SgFunctionRefExp::AddArg( char *name, SgType &t)
    // to add a formal parameter to a function symbol.
{
  PTR_SYMB symb;
  SgExpression *arg = NULL;
  SgSymbol *s;
  SgSymbol *f = this->funName();
  if(!f){
    Message("SgFunctionRefExp::AddArg: no symbol for function_ref", 0);
  }
  s = new SgVariableSymb(name, t, *f->scope()); //create the variable with scope
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, s, 1);
#endif
  symb = s->thesymb;
  appendSymbToArgList(f->thesymb,symb); 

  if(LibFortranlanguage()){
        Message("Fortran function protos do not have arg lists", 0);
	}
 else{
        arg = SgMakeDeclExp(s, &t); 
	NODE_OPERAND0(this->thellnd) = 
	    addToExprList(NODE_OPERAND0(this->thellnd),arg->thellnd);
      }
 return arg;
}

SgFunctionSymb * isSgFunctionSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case PROGRAM_NAME:
    case PROCEDURE_NAME:
    case FUNCTION_NAME:
    case MEMBER_FUNC:
      return (SgFunctionSymb *) pt;
    default:
      return NULL;
    }
}
            

SgMemberFuncSymb * isSgMemberFuncSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case MEMBER_FUNC:
      return (SgMemberFuncSymb *) pt;
    default:
      return NULL;
    }
}

SgFieldSymb * isSgFieldSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case ENUM_NAME:
    case FIELD_NAME:
      return (SgFieldSymb *) pt;
    default:
      return NULL;
    }
}


SgClassSymb * isSgClassSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case CLASS_NAME:
    case TECLASS_NAME:
    case UNION_NAME:
    case STRUCT_NAME:
    case COLLECTION_NAME:
      return (SgClassSymb *) pt;
    default:
      return NULL;
    }
}

#ifdef NOT_YET_IMPLEMENTED
class SgTypeSymb: public SgSymbol{
        // a C typedef.  the type() function returns the base type.
        // variant == TYPE_NAME
      public:
        SgTypeSymb(char *name, SgType &baseType):SgSymbol(TYPE_NAME)
          {
            SORRY;
          };
        SgType &baseType()
          {
            SORRY;
          };
        ~SgTypeSymb(){RemoveFromTableSymb((void *) this);};
};


SgTypeSymb * isSgTypeSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case TYPE_NAME:
      return (SgTypeSymb *) pt;
    default:
      return NULL;
    }
}
#endif

SgLabelSymb * isSgLabelSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case LABEL_NAME:
      return (SgLabelSymb *) pt;
    default:
      return NULL;
    }
}

SgLabelVarSymb * isSgLabelVarSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case LABEL_NAME:
      return (SgLabelVarSymb *) pt;
    default:
      return NULL;
    }
}


SgExternalSymb * isSgExternalSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case ROUTINE_NAME:
      return (SgExternalSymb *) pt;
    default:
      return NULL;
    }
}

SgConstructSymb * isSgConstructSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case CONSTRUCT_NAME:
      return (SgConstructSymb *) pt;
    default:
      return NULL;
    }
}

SgInterfaceSymb * isSgInterfaceSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case INTERFACE_NAME:
      return (SgInterfaceSymb *) pt;
    default:
      return NULL;
    }
}



SgModuleSymb * isSgModuleSymb (SgSymbol *pt)
{

  if (!pt)
    return NULL;
  switch(SYMB_CODE(pt->thesymb))
    {
    case MODULE_NAME:
      return (SgModuleSymb *) pt;
    default:
      return NULL;
    }
}

// ********************* Types *******************************


SgArrayType * isSgArrayType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_ARRAY:
      return (SgArrayType *) pt;
    default:
      return NULL;
    }
}

#ifdef NOT_YET_IMPLEMENTED
class SgClassType: public SgType{
        // a C struct or Fortran Record, a C++ class, a C Union and a C Enum
        // and a pC++ collection.  note: derived classes are another type.
        // this type is very simple.  it only contains the standard type
        // info from SgType and a pointer to the class declaration stmt
        // and a pointer to the symbol that is the first field in the struct.
        // variant == T_STRUCT, T_ENUM, T_CLASS, T_TECLASS T_ENUM, T_COLLECTION
    public:
        // why is struct_decl needed. No appropriate field found.
        // assumes that first_field has been declared as
        // FIELD_NAME and the remaining fields have been stringed to it.
        SgClassType(int variant, char *name, SgStatement &struct_decl, int num_fields,
                     SgSymbol &first_field):SgType(variant)
          { 
            
            SORRY;
          };
        SgStatement &structureDecl()
          {
            SORRY;
          };
        SgSymbol *firstFieldSymb()
          { return SymbMapping(TYPE_FIRST_FIELD(thetype)); };
        SgSymbol *fieldSymb(int i)
          { return SymbMapping(GetThOfFieldListForType(thetype, i)); }
        int numberOfFields()
          { return lenghtOfFieldListForType(thetype); }
        ~SgClassType(){RemoveFromTableType((void *) this);};
};
        

SgClassType * isSgClassType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_STRUCT:
    case T_ENUM:
    case T_CLASS:
    case T_TECLASS:
    case T_COLLECTION:
      return (SgClassType *) pt;
    default:
      return NULL;
    }
}
#endif

SgPointerType::SgPointerType(SgType &base_type):SgType(T_POINTER)
{ TYPE_BASE(thetype) = base_type.thetype; }

SgPointerType::SgPointerType(SgType *base_type):SgType(T_POINTER)
{ TYPE_BASE(thetype) = base_type->thetype; }

SgPointerType * isSgPointerType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_POINTER:
      return (SgPointerType *) pt;
    default:
      return NULL;
    }
}


SgReferenceType * isSgReferenceType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_REFERENCE:
      return (SgReferenceType *) pt;
    default:
      return NULL;
    }
}


SgFunctionType * isSgFunctionType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_FUNCTION:
      return (SgFunctionType *) pt;
    default:
      return NULL;
    }
}




SgDerivedType * isSgDerivedType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_DERIVED_TYPE:
      return (SgDerivedType *) pt;
    default:
      return NULL;
    }
}

SgDerivedClassType * isSgDerivedClassType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_DERIVED_CLASS:
      return (SgDerivedClassType *) pt;
    default:
      return NULL;
    }
}


SgDescriptType * isSgDescriptType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_DESCRIPT:
      return (SgDescriptType *) pt;
    default:
      return NULL;
    }
}



SgDerivedCollectionType * isSgDerivedCollectionType (SgType *pt)
{

  if (!pt)
    return NULL;
  switch(TYPE_CODE(pt->thetype))
    {
    case T_DERIVED_COLLECTION:
      return (SgDerivedCollectionType *) pt;
    default:
      return NULL;
    }
}

// perhaps this function can use LlndMapping
SgExpression * SgSubscriptExp::lbound()
{
  PTR_LLND ll = NULL;
  ll = NODE_OPERAND0(thellnd);
  if (ll && (NODE_CODE(ll) == DDOT))
    ll = NODE_OPERAND0(ll);
  return LlndMapping(ll);
}

SgExpression * SgSubscriptExp::ubound()
{
  PTR_LLND ll = NULL;

  ll = NODE_OPERAND0(thellnd);
  if (ll && (NODE_CODE(ll) == DDOT))
    ll = NODE_OPERAND1(ll);
  else
    ll = NODE_OPERAND1(thellnd);
  return LlndMapping(ll);
}

SgExpression * SgSubscriptExp::step()
{
  PTR_LLND ll = NULL;
  ll = NODE_OPERAND0(thellnd);
  if (ll && (NODE_CODE(ll) == DDOT))
    ll = NODE_OPERAND1(thellnd);
  else
    ll = makeInt(1);
  return LlndMapping(ll);
}

//
// miscelleanous functions
//

// return a symbol with the name;
// if where is NULL the first symbol, whose name matches, found is returned;
// if where is non NULL the first symbol which scope included  where 
// is returned; as an example getSymbol("foo", GLOBAL_NODE)
// returns only the symbol named foo with scope = GLOBAL_NODE;

SgSymbol *getSymbol(char *name, SgStatement *where)
{
  if (where)
    return SymbMapping(getSymbolWithNameInScope(name, where->thebif));
  else
    return SymbMapping(getSymbolWithNameInScope(name,NULL));
}

void SgSymbol::declareTheSymbol(SgStatement &st)
{ 
    SgClassStmt *cl = NULL;
    SgFuncHedrStmt *fh = NULL;
    SgSymbol *fsym;
    if(LibFortranlanguage()){
	declareAVar(thesymb, st.thebif); 
	}
    else{
       SgType *t = this->type();
       SgExpression *e = SgMakeDeclExp(this, t );
       SYMB_SCOPE(this->thesymb) = st.thebif;
       SgStatement *hdr = &st;
       while( (hdr->variant() != GLOBAL) &&
             ((cl = isSgClassStmt(hdr)) == NULL) && 
             ((fh = isSgFuncHedrStmt(hdr)) == NULL)) 
	         hdr = hdr->controlParent();
       if(cl){
            if((fsym = cl->name()) != NULL)
	        appendSymbToArgList(fsym->thesymb,this->thesymb);
	    }
       if(fh){
	    if((fsym = &(fh->name())) != NULL)
	        appendSymbToArgList(fsym->thesymb,this->thesymb);
	    }
       e = new SgExprListExp(*e);
#ifdef __SPF   
       addToCollection(__LINE__, __FILE__, e, 1);
#endif
       SgVarDeclStmt *s = new SgVarDeclStmt(*e,  *t);
#ifdef __SPF   
       addToCollection(__LINE__, __FILE__, s, 1);
#endif
       st.insertStmtAfter(*s, *s->controlParent());
       }
 }

SgExpression *SgSymbol::makeDeclExpr()
{ 
    if(LibFortranlanguage()){
	return LlndMapping(makeDeclExp(thesymb)); 
	}
    else return SgMakeDeclExp(this,  this->type());
} 

SgVarDeclStmt *SgSymbol::makeVarDeclStmt()
{
   if(LibFortranlanguage()){
       return 
           isSgVarDeclStmt(BfndMapping(makeDeclStmt(thesymb)));
	   }
   else{
       SgType *t = this->type();
       SgExpression *e = SgMakeDeclExp(this, t );
       e = new SgExprListExp(*e);
#ifdef __SPF   
       addToCollection(__LINE__, __FILE__, e, 1);
#endif
       SgVarDeclStmt *s = new SgVarDeclStmt(*e,  *t);
#ifdef __SPF   
       addToCollection(__LINE__, __FILE__, s, 1);
#endif
       return s;
       }
   }

SgVarDeclStmt *SgSymbol::makeVarDeclStmtWithParamList
           (SgExpression &parlist)
{ return 
       isSgVarDeclStmt
         (BfndMapping(makeDeclStmtWPar(thesymb, parlist.thellnd)));} 


//
//
//
// Main file for debug purpose, check the routines in the
// in this file
//
//
//

#ifdef DEBUGLIB
main()
{
  SgProject project("test.proj");
  SgFile    file("simple.f");
  SgValueExp c1(1), c2(2), c3(3), c100(100);
  SgExpression *pt;
  SgVarRefExp  *e1, *e2, *e3, *e4;
  SgStatement *themain, *first, *firstex, *last;
  SgFuncHedrStmt *ptfunc;
  SgSymbol *ptsymb;
  SgSymbol *i1;
  SgSymbol *i2;
  SgSymbol *i3;
  SgSymbol *i4;
  SgSymbol *anarray;
  SgAssignStmt *stmt, *stmt1;
  SgIfStmt *anif;
  SgStatement *anotherif;
  SgWhileStmt *awhile;
  SgForStmt *afor;
  SgReturnStmt *areturn;
  SgCallStmt *afuncall;
  SgArrayType *typearray;
  SgType basetype(T_FLOAT);


  printf("There is %d files in that project\n",project.numberOfFiles());
  first = (file.firstStatement());
  themain = (file.mainProgram());

  ptfunc = new SgFuncHedrStmt("funct1");

  ptsymb = new SgVariableSymb("var1");
  pt = new SgVarRefExp(*ptsymb);
  ptfunc->AddArg(*pt);

  ptsymb = new SgVariableSymb("var2");
  pt = new SgVarRefExp(*ptsymb);
  ptfunc->AddArg(*pt);

  first->insertStmtAfter(*ptfunc);

  // lets add a statement to that function 
  i1 = new SgVariableSymb("i1");
  i1->declareTheSymbol(*ptfunc);
  e1 = new SgVarRefExp(*i1);

  i2 = new SgVariableSymb("i2");
  i2->declareTheSymbol(*ptfunc);
  e2 = new SgVarRefExp(*i2);

  i3 = new SgVariableSymb("i3");
  i3->declareTheSymbol(*ptfunc);
  e3 = new SgVarRefExp(*i3);
  
  i4 = new SgVariableSymb("i4");
  i4->declareTheSymbol(*ptfunc);
  e4 = new SgVarRefExp(*i4);

  firstex = (ptfunc->lastDeclaration());
  stmt = new SgAssignStmt((*e1), (*e2) + ((*e3) + c1) * (*e4));

  stmt1 = new SgAssignStmt(*e2,*e3);

  anif = new SgIfStmt(c1 > c2 , *stmt1, stmt->copy());
  anotherif = &(anif->copy());

  awhile = new SgWhileStmt( (*e4)< c2 , anif->copy());

  afor = new SgForStmt(* i1, c1, c2, c3, awhile->copy());
  areturn = new SgReturnStmt();

  afuncall = new SgCallStmt(*ptfunc->symbol());
  afuncall->addArg(c1.copy());
  afuncall->addArg(c2.copy());
  afuncall->addArg(c3.copy());
  
// let insert what we have created 
  firstex->insertStmtAfter(*anif);
  firstex->insertStmtAfter(stmt->copy());
  firstex->insertStmtAfter(*awhile);
  firstex->insertStmtAfter(*afor);

  last = (ptfunc->lastExecutable());
  last->insertStmtAfter(*areturn);

  
  themain->insertStmtAfter(*anotherif);
  themain->insertStmtAfter(*afuncall);

// Let's try array
   typearray = new SgArrayType(basetype);
   typearray->addRange(c1);
   typearray->addRange(c2);
   typearray->addRange(c3);
   anarray = new SgVariableSymb("Array1",*typearray);
   anarray->declareTheSymbol(*ptfunc);

// make an array expression
   pt =  new SgArrayRefExp(*anarray,*e1,*e2,*e3);
   stmt = new SgAssignStmt((*pt), (*e2) + ((*pt) + c1) * (*pt));
   firstex->insertStmtAfter(*stmt);

// unparse the file
  file.unparsestdout();
  file.saveDepFile("debug.dep");
  
}
#endif


// SgReturnStmt--inlines

SgReturnStmt::SgReturnStmt(SgExpression &returnValue):SgExecutableStatement(RETURN_NODE)
{
  BIF_LL1(thebif) = returnValue.thellnd;
  if (CurrentProject->Fortranlanguage())
    {
      Message("Fortran return does not have expression",0);
      BIF_CODE(thebif) = RETURN_STAT;
    }
}

SgReturnStmt::SgReturnStmt():SgExecutableStatement(RETURN_NODE)
{
  if (CurrentProject->Fortranlanguage())
    BIF_CODE(thebif) = RETURN_STAT;
}



/////////////////////////// METHOD FOR ATTRIBUTES (IN A SEPARATE FILES????) ///////////////


SgAttribute::SgAttribute(int t, void *pt, int size, SgStatement &st, int)
{
    type = t;
    data = pt;
    dataSize = size;
    next = NULL;
    // enum typenode { BIFNODE, LLNODE, SYMBNODE, TYPENODE, BLOBNODE,
    //                    BLOB1NODE};
    typeNode = BIFNODE;
    ptToSage = (void *)&st;
    fileNumber = CurrentFileNumber;
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgAttribute::SgAttribute(int t, void *pt, int size, SgSymbol &st, int)
{
    type = t;
    data = pt;
    dataSize = size;
    next = NULL;
    typeNode = SYMBNODE;
    ptToSage = (void *)&st;
    fileNumber = CurrentFileNumber;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgAttribute::SgAttribute(int t, void *pt, int size, SgExpression &st, int)
{
    type = t;
    data = pt;
    dataSize = size;
    next = NULL;
    typeNode = LLNODE;
    ptToSage = (void *)&st;
    fileNumber = CurrentFileNumber;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgAttribute::SgAttribute(int t, void *pt, int size, SgType &st, int)
{
    type = t;
    data = pt;
    dataSize = size;
    next = NULL;
    typeNode = TYPENODE;
    ptToSage = (void *)&st;
    fileNumber = CurrentFileNumber;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgAttribute::SgAttribute(int t, void *pt, int size, SgLabel &st, int)
{
    type = t;
    data = pt;
    dataSize = size;
    next = NULL;
    typeNode = LABEL;
    ptToSage = (void *)&st;
    fileNumber = CurrentFileNumber;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgAttribute::SgAttribute(int t, void *pt, int size, SgFile &st, int)
{
    type = t;
    data = pt;
    dataSize = size;
    next = NULL;
    typeNode = FILENODE;
    ptToSage = (void *)&st;
    fileNumber = CurrentFileNumber;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgAttribute::~SgAttribute()
{
#if __SPF
    removeFromCollection(this);
#endif
}

int SgAttribute::getAttributeType()
{
  return type;
}

void SgAttribute::setAttributeType(int t)
{
  type = t;
}

void *SgAttribute::getAttributeData()
{
  return data;
}

void *SgAttribute::setAttributeData(void *d)
{
  void *temp;
  temp = data;
  data = d;
  return temp;
}

int SgAttribute::getAttributeSize()
{
  return dataSize;
}

void SgAttribute::setAttributeSize(int s)
{
  dataSize = s;
}

typenode SgAttribute::getTypeNode()
{
  return typeNode;
}

void *SgAttribute::getPtToSage()
{
  return ptToSage;
}
 
void  SgAttribute::setPtToSage(void *sa)
{
  ptToSage = sa;
}

void  SgAttribute::resetPtToSage()
{
  ptToSage = NULL;
}

void  SgAttribute::setPtToSage(SgStatement &st)
{
  ptToSage = (void *) &st;
  typeNode = BIFNODE; 
  
}

void  SgAttribute::setPtToSage(SgSymbol &st)
{
  ptToSage = (void *) &st;
  typeNode = SYMBNODE;
}

void  SgAttribute::setPtToSage(SgExpression &st)
{
  ptToSage = (void *) &st;
  typeNode =  LLNODE;
}

void  SgAttribute::setPtToSage(SgType &st)
{
  ptToSage = (void *) &st;
  typeNode = TYPENODE;
}

void SgAttribute::setPtToSage(SgLabel &st)
{
  ptToSage = (void *) &st;
  typeNode = LABEL;
}

void SgAttribute::setPtToSage(SgFile &st)
{
  ptToSage = (void *) &st;
  typeNode = FILENODE;
}

SgStatement *SgAttribute::getStatement()
{
  if (typeNode == BIFNODE)
    return (SgStatement *) ptToSage;
  else
    return NULL;
}

SgExpression *SgAttribute::getExpression()
{
  if (typeNode == LLNODE)
    return (SgExpression *) ptToSage;  
  else
    return NULL;
}

SgSymbol  *SgAttribute::getSgSymbol()
{
  if (typeNode == SYMBNODE)
    return (SgSymbol  *) ptToSage;
  else
    return NULL;
}

SgType  *SgAttribute::getType()
{
  if (typeNode == TYPENODE)
    return (SgType  *) ptToSage;
  else
    return NULL;
}

SgLabel *SgAttribute::getLabel()
{
  if (typeNode == LABEL)
    return (SgLabel  *) ptToSage;
  else
    return NULL;
}

SgFile *SgAttribute::getFile()
{
  if (typeNode == FILENODE)
    return (SgFile  *) ptToSage;
  else
    return NULL;
}

int SgAttribute::getfileNumber()
{
  return fileNumber;
}

SgAttribute *SgAttribute::copy()
{
  return NULL;
}

SgAttribute *SgAttribute::getNext()
{
  return next;
}
  
void SgAttribute::setNext(SgAttribute *s)
{
  next = s;
}

int SgAttribute::listLenght()
{
  SgAttribute *first;
  int nb = 0;

  first = this;
  while (first)
    {
      nb++;
      first = first->getNext();
    }
  return nb;
}

SgAttribute *SgAttribute::getInlist(int num)
{
  SgAttribute *first;
  int nb = 0;

  first = this;
  while (first)
    {
      if (nb == num)
        return first;
      nb++;
      first = first->getNext();
    }
  return NULL;
}


void SgAttribute::save(FILE *file)
{
  SgStatement *stat;
  SgSymbol *symb;
  SgExpression *exp;
  SgType *ty;
  int id = 0;
  int i;
  char *pt;
  char c1,c2,c;
  unsigned int mask = 15;

  if (!file) return;
  
  switch (typeNode)
    {
    case BIFNODE:
      stat = (SgStatement *) ptToSage;
      id = stat->id();
      break;
    case  SYMBNODE:
      symb = (SgSymbol *) ptToSage;
      id = symb->id();
      break;
    case LLNODE:
      exp = (SgExpression *) ptToSage;
        id = exp->id();
      break;  
    case TYPENODE:
      ty = (SgType * ) ptToSage;
      id = ty->id();
      break;
    case BLOBNODE:
    case BLOB1NODE:
    case LABEL:
    case FILENODE:
        break;
    default:
        break;
    }
  fprintf(file,"ID %d typeNode %d FileNum %d TYPE %d DATASIZE %d\n",id,typeNode,fileNumber,type,dataSize);

  if (dataSize && data)
    { // simple way of storing the data in ascii form;
      pt = (char *) data;
      for (i = 0; i<dataSize; i++)
        {
          c = pt[i];
          c1 = (c & mask) + 'a';
          c2 = c >> 4;
          c2 = (c2  & mask) + 'a';
          fprintf(file,"%c%c",c1,c2);
        }
      fprintf(file,"\n");
    }
}



void SgAttribute::save(FILE *file,void (*savefunction)(void *dat, FILE *f))
{
  SgStatement *stat;
  SgSymbol *symb;
  SgExpression *exp;
  SgType *ty;
  int id = 0;

  if (!file || !savefunction) return;
  
  switch (typeNode)
    {
    case BIFNODE:
      stat = (SgStatement *) ptToSage;
      id = stat->id();
      break;
    case  SYMBNODE:
      symb = (SgSymbol *) ptToSage;
      id = symb->id();
      break;
    case LLNODE:
      exp = (SgExpression *) ptToSage;
        id = exp->id();
      break;  
    case TYPENODE:
      ty = (SgType * ) ptToSage;
      id = ty->id();
      break;
    case BLOBNODE:
    case BLOB1NODE:
    case LABEL:
    case FILENODE:
        break;
    default:
        break;
    }
  fprintf(file,"ID %d typeNode %d FileNum %d TYPE %d DATASIZE %d\n",id,typeNode,fileNumber,type,dataSize);
  (*savefunction)(data,file);
}


  
///////////////////// ATTRIBUTES METHODS FOR FILES /////////////////////////////////

void SgFile::saveAttributes(char *file)
{
  int i;
  int nba;
  SgAttribute *att;
  FILE *outfilea;

  if (!file)
    return;
  outfilea = fopen(file,"w");
  if (!outfilea)
    {
      Message("Cannot open output file; unparsing stdout",0);
      outfilea = stdout;
    }
  nba = this->numberOfAttributes();
  fprintf(outfilea,"%d\n",nba);
  for (i=0 ; i< nba; i++)
    {
      att = this->attribute(i);
      if (att)
        att->save(outfilea);
    }
  fclose(outfilea);
}
 

void SgFile::saveAttributes(char *file, void  (*savefunction)(void *dat,FILE *f))
{
  int i;
  int nba;
  SgAttribute *att;
  FILE *outfilea;

  if (!file)
    return;
  outfilea = fopen(file,"w");
  if (!outfilea)
    {
      Message("Cannot open output file; unparsing stdout",0);
      outfilea = stdout;
    }
  nba = this->numberOfAttributes();
  fprintf(outfilea,"%d\n",nba);
  for (i=0 ; i< nba; i++)
    {
      att = this->attribute(i);
      if (att)
        att->save(outfilea,savefunction);
    }
  fclose(outfilea);
}
 


void SgFile::readAttributes(char *file)
{
  int i,j;
  int nba = 0;
  FILE *infilea;
  char *str;
  char buf1[64],buf2[64],buf3[64],buf4[64],buf5[64];
  int id, tn,f,t,ds;
  char c1,c2,c;
  SgStatement *stat;
  PTR_BFND bf;

  if (!file)
    return;
  infilea = fopen(file,"r");
  if (!infilea)
    {
      Message("Cannot open input file",0);
      return;
    }
  // first read the number of attributes;
  fscanf(infilea,"%d", &nba);
  for (i=0; i< nba; i++)
    {
      fscanf(infilea,"%s%d%s%d%s%d%s%d%s%d",
             buf1,&id,buf2,&tn,buf3,&f,buf4,&t,buf5,&ds);
      str = NULL;
      if (ds)
        {
          // skip return;
          fscanf(infilea,"%c",&c1);
          //read the data;
          str = new char[ds];
#ifdef __SPF   
          addToCollection(__LINE__, __FILE__, str, 2);
#endif
          for (j=0;j<ds; j++)
            {
              fscanf(infilea,"%c%c",&c1,&c2);
              c1 = c1 - 'a';
              c2 = c2 - 'a';
              c2 = c2 << 4;
              c = c1 + c2;
              str[j] = c;
            }
        }
      // now allocate the attribute;
      switch (tn)
        {
        case BIFNODE:
          stat = NULL;
          bf = Get_bif_with_id(id);
          if (bf)
            stat = (SgStatement *) GetMappingInTableForBfnd(bf);
          if (stat)
            stat->addAttribute(t, (void *) str,ds);
          break;
        case  SYMBNODE:
          break;
        case LLNODE:
          break;  
        case TYPENODE:
          break;
        }
    }
}


void SgFile::readAttributes(char *file, void * (*readfunction)(FILE *f))
{
  int i;
  int nba = 0;
  FILE *infilea;
  void *str;
  char buf1[64],buf2[64],buf3[64],buf4[64],buf5[64];
  int id, tn,f,t,ds;
  char c1;
  SgStatement *stat;
  PTR_BFND bf;

  if (!file)
    return;
  infilea = fopen(file,"r");
  if (!infilea)
    {
      Message("Cannot open input file",0);
      return;
    }
  // first read the number of attributes;
  fscanf(infilea,"%d", &nba);
  for (i=0; i< nba; i++)
    {
      fscanf(infilea,"%s%d%s%d%s%d%s%d%s%d",
             buf1,&id,buf2,&tn,buf3,&f,buf4,&t,buf5,&ds);
      str = NULL;
      fscanf(infilea,"%c",&c1);
      // read the attributes;
      str = (*readfunction)(infilea);
      // now allocate the attribute;
      switch (tn)
        {
        case BIFNODE:
          stat = NULL;
          bf = Get_bif_with_id(id);
          if (bf)
            stat = (SgStatement *) GetMappingInTableForBfnd(bf);
          if (stat)
            stat->addAttribute(t, (void *) str,ds);
          break;
        case  SYMBNODE:
          break;
        case LLNODE:
          break;  
        case TYPENODE:
          break;
        }
    }
}

int  SgFile::numberOfAttributes()
{
  int i;
  int nb = 0;

  for (i=0 ; i < allocatedForfileTableAttribute; i++)
    {
      if (fileTableAttribute[i])
        nb = nb + fileTableAttribute[i]->listLenght();
    }
  for (i=0 ; i < allocatedForbfndTableAttribute; i++)
    {
      if (bfndTableAttribute[i])
        nb = nb + bfndTableAttribute[i]->listLenght();
    }

  for (i=0 ; i < allocatedForllndTableAttribute; i++)
    {
      if (llndTableAttribute[i])
        nb = nb + llndTableAttribute[i]->listLenght();
    }

  for (i=0 ; i < allocatedForsymbolTableAttribute; i++)
    {
      if (symbolTableAttribute[i])
        nb = nb + symbolTableAttribute[i]->listLenght();
    }

  for (i=0 ; i < allocatedForlabelTableAttribute; i++)
    {
      if (labelTableAttribute[i])
        nb = nb + labelTableAttribute[i]->listLenght();
    }
  return nb;
}

SgAttribute *SgFile::attribute(int num)
{
  int i;
  int nb = 0;

  // to be optimize later, not very efficient for large amout of attribute.
    for (i=0 ; i < allocatedForfileTableAttribute; i++)
      {
        if (fileTableAttribute[i])
          {
            if ((nb <= num+1) && (num+1 <= nb + fileTableAttribute[i]->listLenght()))
              {
                return fileTableAttribute[i]->getInlist(num - nb);
              }
            nb = nb + fileTableAttribute[i]->listLenght();
          }
      }
  for (i=0 ; i < allocatedForbfndTableAttribute; i++)
    {
      if (bfndTableAttribute[i])
          {
            if ((nb <= num+1) && (num+1 <= nb + bfndTableAttribute[i]->listLenght()))
              {
                return bfndTableAttribute[i]->getInlist(num - nb);
              }
            nb = nb + bfndTableAttribute[i]->listLenght();
          }
    }
  
  for (i=0 ; i < allocatedForllndTableAttribute; i++)
    {
      if (llndTableAttribute[i])
        {
            if ((nb <= num+1) && (num+1 <= nb + llndTableAttribute[i]->listLenght()))
              {
                return llndTableAttribute[i]->getInlist(num - nb);
              }
            nb = nb + llndTableAttribute[i]->listLenght();
          }
    }

  for (i=0 ; i < allocatedForsymbolTableAttribute; i++)
    {
      if (symbolTableAttribute[i])
          {
            if ((nb <= num+1) && (num+1 <= nb + symbolTableAttribute[i]->listLenght()))
              {
                return symbolTableAttribute[i]->getInlist(num - nb);
              }
            nb = nb + symbolTableAttribute[i]->listLenght();
          }
    }

  for (i=0 ; i < allocatedForlabelTableAttribute; i++)
    {
      if (labelTableAttribute[i])
        {
          if ((nb <= num+1) && (num+1 <= nb + labelTableAttribute[i]->listLenght()))
            {
              return labelTableAttribute[i]->getInlist(num - nb);
            }
          nb = nb + labelTableAttribute[i]->listLenght();
        }
    }
  return NULL;
}  

////////////////// NOW the function for ATTRIBUTES IN THE CLASS /////////////////////

////////////////// ATTRIBUTE FOR SgFile /////////////////////
// Kataev 15.07.2013

int SgFile::numberOfFileAttributes()
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForFileAttribute(filept);
  if (!first)
    return 0;
  while (first)
    {
      first = first->getNext();
      nb++;
    }
  return nb;
}


int SgFile::numberOfAttributes(int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForFileAttribute(filept);
  if (!first)
    return 0;
  while (first)
    {
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return nb;
}



SgAttribute *SgFile::getAttribute(int i)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForFileAttribute(filept);
  if (!first)
    return NULL;
  while (first)
    {
      if (nb == i)
        return first;
      first = first->getNext();
      nb++;
    }
  return NULL;
}


SgAttribute *SgFile::getAttribute(int i, int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForFileAttribute(filept);
  if (!first)
    return NULL;
  while (first)
    {
      if ((nb == i) && (first->getAttributeType() == type))
        return first;
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return NULL;
}

void *SgFile::attributeValue(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

 
void *SgFile::attributeValue(int i, int type)
{
  SgAttribute *first;

  if ( (first = getAttribute(i,type)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

int SgFile::attributeType(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeType();
  else
    return 0;
}


void *SgFile::deleteAttribute(int i)
{
    SgAttribute *tobedel, *before, *after;
  void *data = NULL;

  tobedel = getAttribute(i);
  if (!tobedel) return NULL;
  
  if (i > 0)
    {
      before = getAttribute(i-1);
      before->setNext(tobedel->getNext());
      data = tobedel->getAttributeData();
#ifdef __SPF   
      removeFromCollection(tobedel);
#endif
      delete tobedel;
    } else
      {
        after = tobedel->getNext();
        SetMappingInTableForFileAttribute(filept,after);
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
      }
	
  return data;
}

void SgFile::addAttribute(int type, void *a, int size)
{
  SgAttribute *first, *last;
  first = GetMappingInTableForFileAttribute(filept);
  if (!first)
    {
      first = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, first, 1);
#endif
      SetMappingInTableForFileAttribute(filept,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, last, 1);
#endif
        first->setNext(last);
      }
}


void SgFile::addAttribute(SgAttribute *att)
{
  SgAttribute *first, *last;
  if (!att) return;
  first = GetMappingInTableForFileAttribute(filept);
  if (!first)
    {
      first = att;
      SetMappingInTableForFileAttribute(filept,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = att;
        first->setNext(last);
      }
}


void SgFile::addAttribute(int type)
{
  addAttribute(type, NULL, 0);
}


void SgFile::addAttribute(void *a, int size)
{
  addAttribute(0, a, size);
}



int SgStatement::numberOfAttributes()
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;
    int nb = 0;
    first = GetMappingInTableForBfndAttribute(thebif);
    if (!first)
        return 0;
    while (first)
    {
        first = first->getNext();
        nb++;
    }
    return nb;
}


int SgStatement::numberOfAttributes(int type)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;
    int nb = 0;
    first = GetMappingInTableForBfndAttribute(thebif);
    if (!first)
        return 0;
    while (first)
    {
        if (first->getAttributeType() == type)
            nb++;
        first = first->getNext();
    }
    return nb;
}

SgAttribute *SgStatement::getAttribute(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;
    int nb = 0;
    first = GetMappingInTableForBfndAttribute(thebif);
    if (!first)
        return NULL;
    while (first)
    {
        if (nb == i)
            return first;
        first = first->getNext();
        nb++;
    }
    return NULL;
}


SgAttribute *SgStatement::getAttribute(int i, int type)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;
    int nb = 0;
    first = GetMappingInTableForBfndAttribute(thebif);
    if (!first)
        return NULL;
    while (first)
    {
        if ((nb == i) && (first->getAttributeType() == type))
            return first;
        if (first->getAttributeType() == type)
            nb++;
        first = first->getNext();
    }
    return NULL;
}

void *SgStatement::attributeValue(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;

    if ((first = getAttribute(i)) != 0)
        return first->getAttributeData();
    else
        return NULL;
}

 
void *SgStatement::attributeValue(int i, int type)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;

    if ((first = getAttribute(i, type)) != 0)
        return first->getAttributeData();
    else
        return NULL;
}

int  SgStatement::attributeType(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first;

    if ((first = getAttribute(i)) != 0)
        return first->getAttributeType();
    else
        return 0;
}


void *SgStatement::deleteAttribute(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *tobedel, *before, *after;
    void *data = NULL;

    tobedel = getAttribute(i);
    if (!tobedel) return NULL;

    if (i > 0)
    {
        before = getAttribute(i - 1);
        before->setNext(tobedel->getNext());
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
    }
    else
    {
        after = tobedel->getNext();
        SetMappingInTableForBfndAttribute(thebif, after);
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
    }

    return data;
}

void SgStatement::addAttribute(int type, void *a, int size)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgAttribute *first, *last;
    first = GetMappingInTableForBfndAttribute(thebif);
    if (!first)
    {
        first = new SgAttribute(type, a, size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, first, 1);
#endif
        SetMappingInTableForBfndAttribute(thebif, first);
    }
    else
    {
        while (first->getNext())
        {
            first = first->getNext();
        }
        last = new SgAttribute(type, a, size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, last, 1);
#endif
        first->setNext(last);
    }
}

void SgStatement::addAttributeTree(SgAttribute *firstAtt)
{
    if (!firstAtt) 
        return;
    SetMappingInTableForBfndAttribute(thebif, firstAtt);
}

void SgStatement::addAttribute(SgAttribute *att)
{
  SgAttribute *first, *last;
  if (!att) return;
  first = GetMappingInTableForBfndAttribute(thebif);
  if (!first)
    {
      first = att;
      SetMappingInTableForBfndAttribute(thebif,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = att;
        first->setNext(last);
      }
}


void SgStatement::addAttribute(int type)
{
    addAttribute(type, NULL, 0);
}

void SgStatement::addAttribute(void *a, int size)
{
    addAttribute(0, a, size);
}


  

////////////////// ATTRIBUTE FOR SgExpression /////////////////////


int SgExpression::numberOfAttributes()
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLlndAttribute(thellnd);
  if (!first)
    return 0;
  while (first)
    {
      first = first->getNext();
      nb++;
    }
  return nb;
}


int SgExpression::numberOfAttributes(int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLlndAttribute(thellnd);
  if (!first)
    return 0;
  while (first)
    {
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return nb;
}



SgAttribute *SgExpression::getAttribute(int i)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLlndAttribute(thellnd);
  if (!first)
    return NULL;
  while (first)
    {
      if (nb == i)
        return first;
      first = first->getNext();
      nb++;
    }
  return NULL;
}


SgAttribute *SgExpression::getAttribute(int i, int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLlndAttribute(thellnd);
  if (!first)
    return NULL;
  while (first)
    {
      if ((nb == i) && (first->getAttributeType() == type))
        return first;
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return NULL;
}

void *SgExpression::attributeValue(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

 
void *SgExpression::attributeValue(int i, int type)
{
  SgAttribute *first;

  if ( (first = getAttribute(i,type)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

int  SgExpression::attributeType(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeType();
  else
    return 0;
}


void *SgExpression::deleteAttribute(int i)
{
    SgAttribute *tobedel, *before, *after;
  void *data = NULL;

  tobedel = getAttribute(i);
  if (!tobedel) return NULL;
  
  if (i > 0)
    {
      before = getAttribute(i-1);
      before->setNext(tobedel->getNext());
      data = tobedel->getAttributeData();
#ifdef __SPF   
      removeFromCollection(tobedel);
#endif
      delete tobedel;
    } else
      {
        after = tobedel->getNext();
        SetMappingInTableForLlndAttribute(thellnd,after);
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
      }
	
  return data;
}

void SgExpression::addAttribute(int type, void *a, int size)
{
  SgAttribute *first, *last;
  first = GetMappingInTableForLlndAttribute(thellnd);
  if (!first)
    {
      first = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, first, 1);
#endif
      SetMappingInTableForLlndAttribute(thellnd,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, last, 1);
#endif
        first->setNext(last);
      }
}


void SgExpression::addAttribute(SgAttribute *att)
{
  SgAttribute *first, *last;
  if (!att) return;
  first = GetMappingInTableForLlndAttribute(thellnd);
  if (!first)
    {
      first = att;
      SetMappingInTableForLlndAttribute(thellnd,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = att;
        first->setNext(last);
      }
}

void SgExpression::addAttributeTree(SgAttribute* firstAtt)
{
    if (!firstAtt)
        return;
    SetMappingInTableForLlndAttribute(thellnd, firstAtt);
}

void SgExpression::addAttribute(int type)
{
  addAttribute(type, NULL, 0);
}


void SgExpression::addAttribute(void *a, int size)
{
  addAttribute(0, a, size);
}



////////////////// ATTRIBUTE FOR SgSymbol /////////////////////


int SgSymbol::numberOfAttributes()
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForSymbolAttribute(thesymb);
  if (!first)
    return 0;
  while (first)
    {
      first = first->getNext();
      nb++;
    }
  return nb;
}


int SgSymbol::numberOfAttributes(int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForSymbolAttribute(thesymb);
  if (!first)
    return 0;
  while (first)
    {
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return nb;
}



SgAttribute *SgSymbol::getAttribute(int i)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForSymbolAttribute(thesymb);
  if (!first)
    return NULL;
  while (first)
    {
      if (nb == i)
        return first;
      first = first->getNext();
      nb++;
    }
  return NULL;
}


SgAttribute *SgSymbol::getAttribute(int i, int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForSymbolAttribute(thesymb);
  if (!first)
    return NULL;
  while (first)
    {
      if ((nb == i) && (first->getAttributeType() == type))
        return first;
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return NULL;
}

void *SgSymbol::attributeValue(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

 
void *SgSymbol::attributeValue(int i, int type)
{
  SgAttribute *first;

  if ( (first = getAttribute(i,type)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

int  SgSymbol::attributeType(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeType();
  else
    return 0;
}


void *SgSymbol::deleteAttribute(int i)
{
    SgAttribute *tobedel, *before, *after;
  void *data = NULL;

  tobedel = getAttribute(i);
  if (!tobedel) return NULL;
  
  if (i > 0)
    {
      before = getAttribute(i-1);
      before->setNext(tobedel->getNext());
      data = tobedel->getAttributeData();
#ifdef __SPF   
      removeFromCollection(tobedel);
#endif
      delete tobedel;
    } else
      {
        after = tobedel->getNext();
        SetMappingInTableForSymbolAttribute(thesymb,after);
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
      }
	
  return data;
}

void SgSymbol::addAttribute(int type, void *a, int size)
{
  SgAttribute *first, *last;
  first = GetMappingInTableForSymbolAttribute(thesymb);
  if (!first)
    {
      first = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, first, 1);
#endif
      SetMappingInTableForSymbolAttribute(thesymb,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, last, 1);
#endif
        first->setNext(last);
      }
}


void SgSymbol::addAttribute(SgAttribute *att)
{
  SgAttribute *first, *last;
  if (!att) return;
  first = GetMappingInTableForSymbolAttribute(thesymb);
  if (!first)
    {
      first = att;
      SetMappingInTableForSymbolAttribute(thesymb,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = att;
        first->setNext(last);
      }
}


void SgSymbol::addAttribute(int type)
{
  addAttribute(type, NULL, 0);
}


void SgSymbol::addAttribute(void *a, int size)
{
  addAttribute(0, a, size);
}


void SgSymbol::changeName(const char *name)
{
    if (name)
    {
        if (SYMB_IDENT(thesymb))
        {
#ifdef __SPF
            removeFromCollection(SYMB_IDENT(thesymb));
#endif
            free(SYMB_IDENT(thesymb));
        }

        char *str = (char *)xmalloc(strlen(name) + 1);
        strcpy(str, name);
        SYMB_IDENT(thesymb) = str;
    }
}


////////////////// ATTRIBUTE FOR SgType /////////////////////


int SgType::numberOfAttributes()
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForTypeAttribute(thetype);
  if (!first)
    return 0;
  while (first)
    {
      first = first->getNext();
      nb++;
    }
  return nb;
}


int SgType::numberOfAttributes(int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForTypeAttribute(thetype);
  if (!first)
    return 0;
  while (first)
    {
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return nb;
}



SgAttribute *SgType::getAttribute(int i)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForTypeAttribute(thetype);
  if (!first)
    return NULL;
  while (first)
    {
      if (nb == i)
        return first;
      first = first->getNext();
      nb++;
    }
  return NULL;
}


SgAttribute *SgType::getAttribute(int i, int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForTypeAttribute(thetype);
  if (!first)
    return NULL;
  while (first)
    {
      if ((nb == i) && (first->getAttributeType() == type))
        return first;
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return NULL;
}

void *SgType::attributeValue(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

 
void *SgType::attributeValue(int i, int type)
{
  SgAttribute *first;

  if ( (first = getAttribute(i,type)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

int  SgType::attributeType(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeType();
  else
    return 0;
}


void *SgType::deleteAttribute(int i)
{
    SgAttribute *tobedel, *before, *after;
  void *data = NULL;

  tobedel = getAttribute(i);
  if (!tobedel) return NULL;
  
  if (i > 0)
    {
      before = getAttribute(i-1);
      before->setNext(tobedel->getNext());
      data = tobedel->getAttributeData();
#ifdef __SPF   
      removeFromCollection(tobedel);
#endif
      delete tobedel;
    } else
      {
        after = tobedel->getNext();
        SetMappingInTableForTypeAttribute(thetype,after);
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
      }
	
  return data;
}

void SgType::addAttribute(int type, void *a, int size)
{
  SgAttribute *first, *last;
  first = GetMappingInTableForTypeAttribute(thetype);
  if (!first)
    {
      first = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, first, 1);
#endif
      SetMappingInTableForTypeAttribute(thetype,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, last, 1);
#endif
        first->setNext(last);
      }
}


void SgType::addAttribute(SgAttribute *att)
{
  SgAttribute *first, *last;
  if (!att) return;
  first = GetMappingInTableForTypeAttribute(thetype);
  if (!first)
    {
      first = att;
      SetMappingInTableForTypeAttribute(thetype,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = att;
        first->setNext(last);
      }
}


void SgType::addAttribute(int type)
{
  addAttribute(type, NULL, 0);
}


void SgType::addAttribute(void *a, int size)
{
  addAttribute(0, a, size);
}

////////////////// ATTRIBUTE FOR SgLabel /////////////////////
// Kataev 21.03.2013

SgLabel::SgLabel(SgLabel &lab)
{
#ifndef __SPF
    Message("SgLabel: copy constructor not allowed", 0);
#endif
    thelabel = lab.thelabel;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgLabel::SgLabel(PTR_LABEL lab)
{
    thelabel = lab;
    SetMappingInTableForLabel(thelabel, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgLabel::SgLabel(int i)
{
    thelabel = (PTR_LABEL)newNode(LABEL_KIND);
    LABEL_STMTNO(thelabel) = i;
    SetMappingInTableForLabel(thelabel, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgLabel::~SgLabel()
{
#if __SPF
    removeFromCollection(this);
#endif
    RemoveFromTableLabel((void *)this);
}

int SgLabel::numberOfAttributes()
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLabelAttribute(thelabel);
  if (!first)
    return 0;
  while (first)
    {
      first = first->getNext();
      nb++;
    }
  return nb;
}


int SgLabel::numberOfAttributes(int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLabelAttribute(thelabel);
  if (!first)
    return 0;
  while (first)
    {
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return nb;
}



SgAttribute *SgLabel::getAttribute(int i)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLabelAttribute(thelabel);
  if (!first)
    return NULL;
  while (first)
    {
      if (nb == i)
        return first;
      first = first->getNext();
      nb++;
    }
  return NULL;
}


SgAttribute *SgLabel::getAttribute(int i, int type)
{
  SgAttribute *first;
  int nb = 0;
  first = GetMappingInTableForLabelAttribute(thelabel);
  if (!first)
    return NULL;
  while (first)
    {
      if ((nb == i) && (first->getAttributeType() == type))
        return first;
      if (first->getAttributeType() == type)
        nb++;
      first = first->getNext();
    }
  return NULL;
}

void *SgLabel::attributeValue(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

 
void *SgLabel::attributeValue(int i, int type)
{
  SgAttribute *first;

  if ( (first = getAttribute(i,type)) != 0)
    return first->getAttributeData();
  else
  return NULL;
}

int SgLabel::attributeType(int i)
{
  SgAttribute *first;

  if ( (first = getAttribute(i)) != 0)
    return first->getAttributeType();
  else
    return 0;
}


void *SgLabel::deleteAttribute(int i)
{
    SgAttribute *tobedel, *before, *after;
  void *data = NULL;

  tobedel = getAttribute(i);
  if (!tobedel) return NULL;
  
  if (i > 0)
    {
      before = getAttribute(i-1);
      before->setNext(tobedel->getNext());
      data = tobedel->getAttributeData();
#ifdef __SPF   
      removeFromCollection(tobedel);
#endif
      delete tobedel;
    } else
      {
        after = tobedel->getNext();
        SetMappingInTableForLabelAttribute(thelabel,after);
        data = tobedel->getAttributeData();
#ifdef __SPF   
        removeFromCollection(tobedel);
#endif
        delete tobedel;
      }
	
  return data;
}

void SgLabel::addAttribute(int type, void *a, int size)
{
  SgAttribute *first, *last;
  first = GetMappingInTableForLabelAttribute(thelabel);
  if (!first)
    {
      first = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, first, 1);
#endif
      SetMappingInTableForLabelAttribute(thelabel,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = new SgAttribute(type,a,size, *this, CurrentFileNumber);
#ifdef __SPF   
        addToCollection(__LINE__, __FILE__, last, 1);
#endif
        first->setNext(last);
      }
}


void SgLabel::addAttribute(SgAttribute *att)
{
  SgAttribute *first, *last;
  if (!att) return;
  first = GetMappingInTableForLabelAttribute(thelabel);
  if (!first)
    {
      first = att;
      SetMappingInTableForLabelAttribute(thelabel,first);
    } else
      {
        while (first->getNext())
          {
            first = first->getNext();
          }
        last = att;
        first->setNext(last);
      }
}


void SgLabel::addAttribute(int type)
{
  addAttribute(type, NULL, 0);
}


void SgLabel::addAttribute(void *a, int size)
{
  addAttribute(0, a, size);
}

////////////////////////////////////////////////////////////////////////
// This routines performa garbage collection on Expression Statements //
// not to use simultaneously with the data dependence information that//
// creates nodes not to be removed                                    //
// This use the attribute mechanism                                   //
// two flags are used, one the user can set to avoid a node to be     //
// garbage                                                            //
// #define NOGARBAGE_ATTRIBUTE                                        //
// the following one internal to the system                           //
// #define GARBAGE_ATTRIBUTE                                          //
// return the number of nodes collected                               //
////////////////////////////////////////////////////////////////////////


void  saveattXXXGarbage (void *dat,FILE *f)
{
  int *t;
  if (!dat || !f)
    return;

  t = (int *) dat;
  fprintf(f,"Value of the attributes---> %d  %d\n",t[0], t[1]);
  
}

void markExpression(SgExpression *exp)
{
  int *garinfo;

  if (!exp) return;
  if (!isALoNode(exp->variant())) 
    {
      Message("Trying to mark a non Expression Node in Garbage Collection",0);
      return;
    }

  garinfo = (int *) exp->attributeValue(0,GARBAGE_ATTRIBUTE);
  if (garinfo[1]) return; // avoid looping, already visited (necessary???);
  garinfo[0]++;
  garinfo[1] = 1; // visited;
  
  markExpression(exp->lhs());
  markExpression(exp->rhs());
}

int SgFile::expressionGarbageCollection(int deleteExpressionNode, int verbose)
{
  
  SgExpression *exp, *previous, *def, *use, *ann;
  SgStatement *stmt;
  SgSymbol *symb;
  SgType *type;
  int *garinfo;
  int i,j;
  SgConstantSymb *cstsymb;
  SgArrayType *arr;
  int nbatt, typeat;
  int curident;
  PTR_LLND last = NULL;
  int nbdeleted = 0;

  if (verbose)
    printf("garbage collection in process, please wait (did you had coffee yet?)\n");

  if (deleteExpressionNode)
    setFreeListForExpressionNode();    
  else
    resetFreeListForExpressionNode();
  
  for (exp = this->firstExpression(); exp; exp = exp->nextInExprTable())
    {
      garinfo = new int[2];
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, garinfo, 2);
#endif
      garinfo[0] = 0;
      garinfo[1] = 0;
      exp->addAttribute(GARBAGE_ATTRIBUTE,(void *) garinfo, 2*sizeof(int));
    }

  for (stmt = this->firstStatement(); stmt; stmt = stmt->lexNext())
    {
      markExpression(stmt->expr(0));
      markExpression(stmt->expr(1));
      markExpression(stmt->expr(2));
      def = (SgExpression *) stmt->attributeValue(0,DEFINEDLIST_ATTRIBUTE);
      markExpression(def);
      use = (SgExpression *) stmt->attributeValue(0,USEDLIST_ATTRIBUTE);
      markExpression(use);
      nbatt = stmt->numberOfAttributes();
      for (j = 0; j < nbatt ; j++)
        {
          typeat = stmt->attributeType(j);
          if (typeat == ANNOTATION_EXPR_ATTRIBUTE)
            {
              ann = (SgExpression *) stmt->attributeValue(j);
              markExpression(ann);
            }
        }  
    }

  // needs more, to be completed later;

  for (symb = this->firstSymbol(); symb; symb = symb->next())
    {
      // according to the type symbol, it may have pointer to a llnd;
      if ( (cstsymb = isSgConstantSymb(symb)) != 0)
        {
          markExpression(cstsymb->constantValue());
        }
    }

  for (type = this->firstType(); type; type = type->next())
    {
      if ( (arr = isSgArrayType(type)) != 0)
        {
          for (i = 0; i < arr->dimension(); i++)
            markExpression(type->length());
        }      
      if ((type->variant() != DEFAULT) && isAtomicType(type->variant()))
        {
          // check for the range; an mark it;
          markExpression(type->length());
        }
    }
  // actually remove the nodes;
  // this->saveAttributes("markedNODES",saveattXXXGarbage);  For debug purpose;
  previous = this->firstExpression();
  if (previous)
    {
      // keep the first one to avoid to much trouble;
      // to be removed later.
      for (exp = previous->nextInExprTable(); exp; exp = exp->nextInExprTable())
        {
          if (!isALoNode(exp->variant())  || (exp->variant() == DEFAULT))
            {
              Message("Trying to USE a non Expression Node in Garbage Collection",0);
            }
          if (!exp->getAttribute(0,NOGARBAGE_ATTRIBUTE))
            {
              garinfo = (int *) exp->attributeValue(0,GARBAGE_ATTRIBUTE);
              if (!garinfo[0])
                {
                  // remove the node;
                  // first remove all the attribute;
#ifdef __SPF   
                  removeFromCollection(garinfo);
#endif
                  delete garinfo;
                  // removes all the attributes;
                  while (exp->deleteAttribute(0));
                  // now delete the node from the data base;
                  NODE_NEXT(previous->thellnd) = NODE_NEXT(exp->thellnd);
                  curident = exp->id();                  
                  libFreeExpression(exp->thellnd);
                  llndTableClass[curident] = NULL;
#ifdef __SPF   
                  removeFromCollection(exp);
#endif
                  delete exp;
                  exp = previous;
                  nbdeleted++;
                } else
                  previous = exp;
            } else
              previous = exp;
        }
      // now remove the garbage attribute for all nodes;
      previous = this->firstExpression();
      for (exp = previous; exp; exp = exp->nextInExprTable())
        {
          if (!isALoNode(exp->variant())  || (exp->variant() == DEFAULT)) 
            {
              Message("Trying to USE (1) a non Expression Node in Garbage Collection",0);
            }
          nbatt = exp->numberOfAttributes();
          for (j = 0; j < nbatt ; j++)
            {
              typeat = exp->attributeType(j);
              if (typeat == GARBAGE_ATTRIBUTE)
                {
                  garinfo = (int *) exp->attributeValue(0,GARBAGE_ATTRIBUTE);      
#ifdef __SPF   
                  removeFromCollection(garinfo);
#endif
                  delete garinfo;
                  exp->deleteAttribute(j);
                  j--;
                }
            }
        }

      // needs also to update the llnode numbers;
      // no need to check the table,  already allocated;
      curident = 1;
      previous = this->firstExpression();
      for (exp = previous; exp; exp = exp->nextInExprTable())
        {
          if (!isALoNode(exp->variant())  || (exp->variant() == DEFAULT)) 
            {
              Message("Trying to USE (1) a non Expression Node in Garbage Collection",0);
            }
          last = exp->thellnd;
          llndTableAttribute[curident] = llndTableAttribute[NODE_ID(exp->thellnd)];
	  NODE_ID(exp->thellnd) =  curident;      
          llndTableClass[curident] = (void *) exp;
          curident++;
        }
      number_of_ll_node = curident-1;
      CUR_FILE_NUM_LLNDS() =  curident-1;
      CUR_FILE_CUR_LLND()  = last;
    }
  return nbdeleted;
}

////////////////////////////  TEMPLATE RELATED STUFF /////////////////////////

SgTemplateStmt::SgTemplateStmt(SgExpression *arglist)
  :SgStatement(TEMPLATE_FUNDECL){
  if(arglist)
     BIF_LL1(thebif) = arglist->thellnd;
   // probably should change the scope of the symbols in this list.
}
SgExpression * SgTemplateStmt::AddArg(char *name, SgType &t){
   // returns decl expr created.  if name == null this is a type arg
  PTR_SYMB symb;
  SgExpression *arg;
  SgSymbol *s;

  s = new SgVariableSymb(name, t, *this); //create the variable with scope
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, s, 1);
#endif
  symb = s->thesymb;
  appendSymbToArgList(BIF_SYMB(thebif),symb); 
  arg = SgMakeDeclExp(s, &t); 
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg->thellnd);
 return arg;
}

SgExpression * SgTemplateStmt::AddArg(char *name, SgType &t, 
   SgExpression &init)
{
  PTR_SYMB symb;
  PTR_LLND ll;
  SgExpression *arg, *ref;
  SgSymbol *s;

  if(name == NULL){
      name = new char;
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, name, 1);
#endif
      *name = (char) 0;
      }
  s = new SgVariableSymb(name, t, *this); //create the variable with scope
#ifdef __SPF   
  addToCollection(__LINE__, __FILE__, s, 1);
#endif
  symb = s->thesymb;
  appendSymbToArgList(BIF_SYMB(thebif),symb); 
  ref = SgMakeDeclExp(s, &t);
  arg = &SgAssignOp(*ref, init);
  ll = BIF_LL1(thebif);
  ll = NODE_OPERAND0(ll);
  NODE_OPERAND0(ll) = addToExprList(NODE_OPERAND0(ll),arg->thellnd);
  return arg;
}

int SgTemplateStmt::numberOfArgs(){
    return exprListLength(BIF_LL1(thebif));
}
SgExpression * SgTemplateStmt::arg(int i){
	return LlndMapping(getPositionInExprList(BIF_LL1(thebif), i));
}
SgExpression * SgTemplateStmt::argList(){
       return LlndMapping(BIF_LL1(thebif));
}
void SgTemplateStmt::addFunction(SgFuncHedrStmt &theTemplateFunc){
             this->insertStmtAfter(theTemplateFunc,*this);
}
void SgTemplateStmt::addClass(SgClassStmt &theTemplateClass){
             this->insertStmtAfter(theTemplateClass,*this);
}
SgFuncHedrStmt * SgTemplateStmt::isFunction(){
   PTR_BLOB blob;
   SgStatement *x;
      blob = lookForBifInBlobList(BIF_BLOB1(BIF_CP(thebif)), thebif);
      if (!blob)
        return NULL;
      x = BfndMapping(BLOB_VALUE(blob));
      return isSgFuncHedrStmt(x);
}
SgClassStmt * SgTemplateStmt::isClass(){
   PTR_BLOB blob;
   SgStatement *x;
      blob = lookForBifInBlobList(BIF_BLOB1(BIF_CP(thebif)), thebif);
      if (!blob)
        return NULL;
      x = BfndMapping(BLOB_VALUE(blob));
      return isSgClassStmt(x);
}

//- the T_DERIVED_TEMPLATE class functions 

SgDerivedTemplateType::SgDerivedTemplateType(SgExpression *arg_vals, 
                            SgSymbol *classname): SgType(T_DERIVED_TEMPLATE){
              if(classname)
		TYPE_TEMPL_NAME(thetype) = classname->thesymb;
              if(arg_vals)
		TYPE_TEMPL_ARGS(thetype) = arg_vals->thellnd;

}
SgExpression * SgDerivedTemplateType::argList(){
     return LlndMapping(TYPE_TEMPL_ARGS(thetype));
}

void SgDerivedTemplateType::addArg(SgExpression *arg){
 TYPE_TEMPL_ARGS(thetype) = 
    addToExprList(TYPE_TEMPL_ARGS(thetype),arg->thellnd);
}

int SgDerivedTemplateType::numberOfArgs(){
    return exprListLength(TYPE_TEMPL_ARGS(thetype));
}
SgExpression  * SgDerivedTemplateType::arg(int i){
    return LlndMapping(getPositionInExprList(TYPE_TEMPL_ARGS(thetype), i));
}
void SgDerivedTemplateType::setName(SgSymbol &s){
     TYPE_TEMPL_NAME(thetype) = s.thesymb;
}
SgSymbol * SgDerivedTemplateType::typeName(){
     return SymbMapping(TYPE_TEMPL_NAME(thetype));
}

//////////////////////////////////////  ADDED  GENERIC METHODS /////////////////////

SgStatement::SgStatement(int code, SgLabel *lab, SgSymbol *symb, SgExpression *e1, SgExpression *e2, SgExpression *e3)
{

    thebif = (PTR_BFND)newNode(code);
    BIF_SYMB(thebif) = NULL;
    BIF_LL1(thebif) = NULL;
    BIF_LL2(thebif) = NULL;
    BIF_LL3(thebif) = NULL;
    BIF_LABEL(thebif) = NULL;

    if (lab)
        BIF_LABEL(thebif) = lab->thelabel;
    if (symb)
        BIF_SYMB(thebif) = symb->thesymb;
    if (e1)
        BIF_LL1(thebif) = e1->thellnd;
    if (e2)
        BIF_LL2(thebif) = e2->thellnd;
    if (e3)
        BIF_LL3(thebif) = e3->thellnd;

    // this should be function of low_level.c
    switch (BIF_CODE(thebif))
    { // node that can be a bif control parent 
    case  GLOBAL:
    case  PROG_HEDR:
    case  PROC_HEDR:
    case  PROS_HEDR:
    case  BASIC_BLOCK:
    case  IF_NODE:
    case WHERE_BLOCK_STMT:
    case  LOOP_NODE:
    case  FOR_NODE:
    case  FORALL_NODE:
    case  WHILE_NODE:
    case  CDOALL_NODE:
    case  SDOALL_NODE:
    case  DOACROSS_NODE:
    case  CDOACROSS_NODE:
    case  FUNC_HEDR:
    case  ENUM_DECL:
    case  STRUCT_DECL:
    case  UNION_DECL:
    case  CLASS_DECL:
    case  TECLASS_DECL:
    case  COLLECTION_DECL:
    case  SWITCH_NODE:
    case   EXTERN_C_STAT:
        addControlEndToStmt(thebif);
        break;
    }

    fileID = current_file_id;
    project = CurrentProject;
    unparseIgnore = false;
#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


SgType::SgType(int var, SgExpression *len, SgType *base)
{
    if (!isATypeNode(var))
    {
        Message("Attempt to create a type node with a variant that is not", 0);
        // arbitrary choice for the variant
        thetype = (PTR_TYPE)newNode(T_INT);
    }
    else
        thetype = (PTR_TYPE)newNode(var);

    if (len)
    {
        TYPE_RANGES(thetype) = len->thellnd;
    }
    if (base)
    {
        TYPE_BASE(thetype) = base->thetype;
    }
    SetMappingInTableForType(thetype, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgType::SgType(int var, SgSymbol *symb, SgExpression *len, SgType *base)
{
    if (!isATypeNode(var))
    {
        Message("Attempt to create a type node with a variant that is not", 0);
        // arbitrary choice for the variant
        thetype = (PTR_TYPE)newNode(T_INT);
    }
    else
        thetype = (PTR_TYPE)newNode(var);

    if (len)
    {
        TYPE_RANGES(thetype) = len->thellnd;
    }
    if (base)
    {
        TYPE_BASE(thetype) = base->thetype;
    }
    if (symb)
    {
        TYPE_SYMB(thetype) = symb->thesymb;
    }
    SetMappingInTableForType(thetype, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgType::SgType(int var, SgSymbol *symb)
{
    if (!isATypeNode(var))
    {
        Message("Attempt to create a type node with a variant that is not", 0);
        // arbitrary choice for the variant
        thetype = (PTR_TYPE)newNode(T_INT);
    }
    else
        thetype = (PTR_TYPE)newNode(var);
    if (symb)
    {
        TYPE_SYMB_DERIVE(thetype) = symb->thesymb;
    }
    SetMappingInTableForType(thetype, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgType::SgType(int var, SgSymbol *firstfield, SgStatement *structstmt)
{
    if (!isATypeNode(var))
    {
        Message("Attempt to create a type node with a variant that is not", 0);
        // arbitrary choice for the variant
        thetype = (PTR_TYPE)newNode(T_INT);
    }
    else
        thetype = (PTR_TYPE)newNode(var);

    if (structstmt)
        TYPE_COLL_ORI_CLASS(thetype) = structstmt->thebif;
    if (firstfield)
        TYPE_COLL_FIRST_FIELD(thetype) = firstfield->thesymb;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgType::SgType(PTR_TYPE type)
{
    thetype = type;
    SetMappingInTableForType(thetype, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgType::SgType(SgType &t)
{
    thetype = t.thetype;
#ifndef __SPF
    Message("SgType: no copy constructor allowed", 0);
#endif

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}

SgType::~SgType()
{
#if __SPF
    removeFromCollection(this);
#endif
}

SgSymbol::SgSymbol(int variant, const char *identifier, SgType *type, SgStatement *scope, SgSymbol *structsymb, SgSymbol *nextfield)
{
    if (!isASymbNode(variant))
    {
        Message("Attempt to create a symbol node with a variant that is not", 0);
        // arbitrary choice for the variant
        thesymb = newSymbol(VARIABLE_NAME, identifier, NULL);
    }
    else
        thesymb = newSymbol(variant, identifier, NULL);

    if (type)
        SYMB_TYPE(thesymb) = type->thetype;

    if (scope)
        SYMB_SCOPE(thesymb) = scope->thebif;

    if (structsymb)
    {
        if (variant == MEMBER_FUNC)
            SYMB_MEMBER_BASENAME(thesymb) = structsymb->thesymb;
        else
            SYMB_FIELD_BASENAME(thesymb) = structsymb->thesymb;
    }

    if (nextfield)
    {
        if (variant == FIELD_NAME)
            SYMB_NEXT_FIELD(thesymb) = nextfield->thesymb;
        else
            SYMB_MEMBER_NEXT(thesymb) = nextfield->thesymb;
    }
    SetMappingInTableForSymb(thesymb, (void *)this);

    fileID = current_file_id;
    project = CurrentProject;

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


SgExpression::SgExpression(int variant, char *str)
{
    if (!isALoNode(variant))
    {
        Message("Attempt to create a low level node with a variant that is not", 0);
        // arbitrary choice for the variant
        thellnd = (PTR_LLND)newNode(EXPR_LIST);
    }
    else
        thellnd = (PTR_LLND)newNode(variant);
    NODE_STR(thellnd) = str;
    SetMappingInTableForLlnd(thellnd, (void *)this);

#if __SPF
    addToCollection(__LINE__, __FILE__, this, 1);
#endif
}


///// a supoort routine for the sage code generator //////


SgLabel* getLabel(int id)
{
  PTR_LABEL lab;

  // first check its there;
  if ( (lab = Get_label_with_id(id)) != 0)
    return LabelMapping(lab);
  else
  {
      SgLabel *ret = new SgLabel(id);
#ifdef __SPF   
      addToCollection(__LINE__, __FILE__, ret, 1);
#endif
      return ret;
  }
}


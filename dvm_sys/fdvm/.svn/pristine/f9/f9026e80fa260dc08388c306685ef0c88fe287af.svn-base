/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

#ifndef LIBSAGEXX_H
#define LIBSAGEXX_H 1

#include <string.h>
#include <map>
#include <string>
#include <algorithm>

/* includes the attributes data structure */

#include "attributes.h"

/**************************************************************
File: libSage++.h
Included in: sage++user.h and libSage++.C

Purpose:It contains all the class definitions and the inline
definitions in Sage++. The start of each class and the start of inlines
in each class are easily identifiable. For example the SgProject class
definition starts with class  SgProject (note the 2 spaces between
class and SgProject) and the comment line preceding the inline
declarations of SgProject is something like // SgProject--inlines.
Sections of the include file are within a #ifdef USER  #endif. Those sections
are included only in sage++user.h and not in libSage++.C. Sections of
the include file are within a #if 0  #endif.  These refer to the unimplemented
portions of Sage++ library.
***************************************************************/

#if __SPF
extern "C" void removeFromCollection(void *pointer);
extern void addToGlobalBufferAndPrint(const std::string& toPrint);
#endif

class  SgProject {
  public:
  inline SgProject(SgProject &);
  SgProject(const char *proj_file_name);
  SgProject(const char *proj_file_name, char **files_list, int no);
  inline ~SgProject();
  inline int numberOfFiles();
  SgFile &file(int i);   
  inline char *fileName(int i); 
  inline int Fortranlanguage(); 
  inline int Clanguage();
  void addFile(char * dep_file_name);
  void deleteFile(SgFile * file);
};

class  SgFile {
private:
    static std::map<std::string, std::pair<SgFile*, int> > files;

public:
    PTR_FILE filept;
    SgFile(char* file_name); // the file must exist.
    SgFile(int Language, const char* file_name); // for new empty file objects.
    ~SgFile();
    SgFile(SgFile &);
    inline int languageType();
    inline void saveDepFile(const char *dep_file);
    inline void unparse(FILE *filedisc);
    inline void unparsestdout();
    inline void unparseS(FILE *filedisc, int size);
    const char* filename();

    inline SgStatement *mainProgram();
    SgStatement *functions(int i);
    inline int numberOfFunctions();
    SgStatement *getStruct(int i);
    inline int numberOfStructs();

    inline SgStatement *firstStatement();
    inline SgSymbol *firstSymbol();
    inline SgType *firstType();
    inline SgExpression *firstExpression();

    inline SgExpression *SgExpressionWithId(int i);
    inline SgStatement *SgStatementWithId(int id);
    inline SgStatement *SgStatementAtLine(int lineno);
    inline SgSymbol *SgSymbolWithId(int id);
    inline SgType *SgTypeWithId(int id);
    // for attributes;
    void saveAttributes(char *file);
    void saveAttributes(char *file, void(*savefunction)(void *dat, FILE *f));
    void readAttributes(char *file);
    void readAttributes(char *file, void * (*readfunction)(FILE *f));
    int numberOfAttributes();
    SgAttribute *attribute(int i);

    /***** Kataev 15.07.2013 *****/
    int numberOfFileAttributes();
    int numberOfAttributes(int type); // of a specified type;
    void *attributeValue(int i);
    int  attributeType(int i);
    void *attributeValue(int i, int type); // only considering one type attribute
    void *deleteAttribute(int i);
    void addAttribute(int type, void *a, int size); // void * can be NULL;
    void addAttribute(int type); //void * is NULL;
    void addAttribute(void *a, int size); //no type specifed;
    void addAttribute(SgAttribute *att);
    SgAttribute *getAttribute(int i);
    SgAttribute *getAttribute(int i, int type);
    /*****************************/

    int expressionGarbageCollection(int deleteExpressionNode, int verbose);
    //int SgFile::expressionGarbageCollection(int deleteExpressionNode, int verbose);

    static int switchToFile(const std::string &name);
    static void addFile(const std::pair<SgFile*, int> &toAdd);    
};


extern SgFile *current_file;    //current file
extern int current_file_id;     //number of current file 

// Discuss about control parent, BIF structure etc
class  SgStatement 
{
private:
    int fileID;
    SgProject *project;
    bool unparseIgnore;

    static std::string currProcessFile;
    static int currProcessLine;
    static bool consistentCheckIsActivated;
    // fileID -> [ map<FileName, line>, SgSt*]
    static std::map<int, std::map<std::pair<std::string, int>, SgStatement*> > statsByLine;
    static void updateStatsByLine(std::map<std::pair<std::string, int>, SgStatement*> &toUpdate);
    static std::map<SgExpression*, SgStatement*> parentStatsForExpression;
    static void updateStatsByExpression();
    static void updateStatsByExpression(SgStatement *where, SgExpression *what);

    void checkConsistence();

#ifdef __SPF
    // DEPRECATED IN SAPFOR!!
    inline void setControlParent(SgStatement& s) { BIF_CP(thebif) = s.thebif; };
    inline void setControlParent(SgStatement* s)
    {
        if (s != 0)
            BIF_CP(thebif) = s->thebif;
        else
            BIF_CP(thebif) = 0;
    }
#endif
public:
    PTR_BFND thebif;
    SgStatement(int variant);
    SgStatement(PTR_BFND bif);
    SgStatement(int code, SgLabel *lab, SgSymbol *symb, SgExpression *e1 = NULL, SgExpression *e2 = NULL, SgExpression *e3 = NULL);
    SgStatement(SgStatement &);
    // info about statement
    inline int lineNumber();          // source text line number
    inline int localLineNumber();
    inline int id();                  // unique id;
    inline int variant();             // the type of the statement
    SgExpression *expr(int i); // i = 0,1,2 returns the i-th expression.

    inline int hasSymbol();  // returns TRUE if tmt has symbol, FALSE otherwise
    // returns the symbol field. Used by loop headers to point to the
    // loop variable symbol; Used by function and subroutine headers to
    // point to the function or subroutine name.
    SgSymbol *symbol();        // returns the symbol field.
    inline char *fileName();
    inline void setFileName(char *newFile);

    inline int hasLabel();     // returns 1 if there is a label on the stmt.
    SgLabel *label();          // the label

    // modifying the info.
    inline void setlineNumber(const int n); // change the line number info
    inline void setLocalLineNumber(const int n);
    inline void setId(int n);         // cannot change the id info
    inline void setVariant(int n);    // change the type of the statement
    void setExpression(int i, SgExpression &e); // change the i-th expression
    void setExpression(int i, SgExpression *e); // change the i-th expression
    inline void setLabel(SgLabel &l); // change the label
    inline void deleteLabel(); // delete label
    inline void setSymbol(SgSymbol &s); // change the symbol

    // Control structure
    inline SgStatement *lexNext();   // the next statement in lexical order.
    inline SgStatement *lexPrev();   // the previous stmt in lexical order.
    inline SgStatement *controlParent(); // the enclosing control statement

    inline void setLexNext(SgStatement &s); // change the lexical ordering

#ifndef __SPF
    // change the control parent.
    // DEPRECATED IN SAPFOR!!
    inline void setControlParent(SgStatement& s) { BIF_CP(thebif) = s.thebif; };
    inline void setControlParent(SgStatement* s)
    {
        if (s != 0)
            BIF_CP(thebif) = s->thebif;
        else
            BIF_CP(thebif) = 0;
    }
#endif
  // Access statement using the tree structure
  // Describe BLOB lists here?

    inline int numberOfChildrenList1();
    inline int numberOfChildrenList2();
    inline SgStatement *childList1(int i);
    inline SgStatement *childList2(int i);
    SgStatement *nextInChildList();

    inline SgStatement *lastDeclaration();
    inline SgStatement *lastExecutable();
    inline SgStatement *lastNodeOfStmt();
    inline SgStatement *nodeBefore();
    inline void insertStmtBefore(SgStatement &s, SgStatement &cp);
    void insertStmtAfter(SgStatement &s, SgStatement &cp);
#ifndef __SPF
    //DEPRECATED IN SAPFOR!!
    inline void insertStmtBefore(SgStatement& s) { insertBfndBeforeIn(s.thebif, thebif, NULL); }
    inline void insertStmtAfter(SgStatement& s) { insertBfndListIn(s.thebif, thebif, NULL); }
#endif
    inline SgStatement *extractStmt();
    inline SgStatement *extractStmtBody();
    inline void replaceWithStmt(SgStatement &s);
    inline void deleteStmt();
    inline SgStatement  &copy(void);
    inline SgStatement  *copyPtr(void);
    inline SgStatement  &copyOne(void);
    inline SgStatement  *copyOnePtr(void);
    inline SgStatement  &copyBlock(void);
    inline SgStatement  *copyBlockPtr(void);
    inline SgStatement  *copyBlockPtr(int saveLabelId);
    inline int isIncludedInStmt(SgStatement &s);
    inline void replaceSymbByExp(SgSymbol &symb, SgExpression &exp);
    inline void replaceSymbBySymb(SgSymbol &symb, SgSymbol &newsymb);
    inline void replaceSymbBySymbSameName(SgSymbol &symb, SgSymbol &newsymb);
    inline void replaceTypeInStmt(SgType &old, SgType &newtype);
    char* unparse();
    inline void unparsestdout();
    std::string sunparse(); //unparsing functions.
    inline char *comments();      //preceding comment lines.
    void addComment(const char *com);
    void addComment(char *com);
    /* ajm: setComments: set ALL of the node's comments */
    inline void setComments(char *comments);
    inline void setComments(const char *comments);
    inline void delComments();
    int numberOfComments(); //number of preceeding comments. CAREFUL! 

    int hasAnnotations();   //1 if there are annotations; 0 otherwise
    ~SgStatement();
    // These function must be removed. Doesn't make sense here.
    int IsSymbolInScope(SgSymbol &symb); // TRUE if symbol is in scope
    int IsSymbolReferenced(SgSymbol &symb);
    inline SgStatement *getScopeForDeclare(); // return where a variable can be declared;

    /////////////// FOR ATTRIBUTES //////////////////////////

    int numberOfAttributes();
    int numberOfAttributes(int type); // of a specified type;
    void *attributeValue(int i);
    int  attributeType(int i);
    void *attributeValue(int i, int type); // only considering one type attribute
    void *deleteAttribute(int i);
    void addAttribute(int type, void *a, int size); // void * can be NULL;
    void addAttribute(int type); //void * is NULL;
    void addAttribute(void *a, int size); //no type specifed;
    void addAttribute(SgAttribute *att);
    void addAttributeTree(SgAttribute *firstAtt);
    SgAttribute *getAttribute(int i);
    SgAttribute *getAttribute(int i, int type);

    //////////// FOR DECL_SPECS (friend, inline, extern, static) ////////////

    inline void addDeclSpec(int type);   //type should be one of BIT_EXTERN,
                                  //BIT_INLINE, BIT_FRIEND, BIT_STATIC
    inline void clearDeclSpec();        //resets the decl_specs field to zero
    inline int isFriend();               //returns non-zero if friend modifier set
                                  //returns zero otherwise
    inline int isInline();
    inline int isExtern();
    inline int isStatic();

    // new opportunities were added by Kolganov A.S. 16.04.2018
    inline int getFileId() const { return fileID; }
    inline void setFileId(const int newFileId) { fileID = newFileId; }

    inline SgProject* getProject() const { return project; }
    inline void setProject(SgProject *newProj) { project = newProj; }

    inline bool switchToFile()
    {
        if (fileID == -1 || project == NULL)
            return false;

        if (current_file_id != fileID)
        {
            SgFile* file = &(project->file(fileID));
            currProcessFile = file->filename();
            currProcessLine = 0;
        }
        return true;
    }

    inline SgFile* getFile() const 
    { 
        if (fileID == -1 || project == NULL)
            return NULL;
        else
            return &(project->file(fileID)); 
    }

    inline void setUnparseIgnore(bool flag) { unparseIgnore = flag; }
    inline bool getUnparseIgnore() const { return unparseIgnore; }

    static SgStatement* getStatementByFileAndLine(const std::string &fName, const int lineNum);
    static void cleanStatsByLine() { statsByLine.clear(); }

    static SgStatement* getStatmentByExpression(SgExpression*);
    static void cleanParentStatsForExprs() { parentStatsForExpression.clear(); }
    static void activeConsistentchecker() { consistentCheckIsActivated = true; }
    static void deactiveConsistentchecker() { consistentCheckIsActivated = false; }

    static void setCurrProcessFile(const std::string& name) { currProcessFile = name; }
    static void setCurrProcessLine(const int line) { currProcessLine = line; }
    static std::string getCurrProcessFile() { return currProcessFile; }
    static int getCurrProcessLine() { return currProcessLine; }
};

class  SgExpression
{    
public:
  PTR_LLND thellnd;
  // generic expression class.
  SgExpression(int variant, SgExpression &lhs, SgExpression &rhs, SgSymbol &s, SgType &type);
  SgExpression(int variant, SgExpression *lhs, SgExpression *rhs, SgSymbol *s, SgType *type);
  SgExpression(int variant, SgExpression *lhs, SgExpression *rhs, SgSymbol *s);
  SgExpression(int variant, SgExpression *lhs, SgExpression *rhs);
  SgExpression(int variant, SgExpression* lhs);

  // for some node in fortran
  SgExpression(int variant,char *str);

  SgExpression(int variant);
  SgExpression(PTR_LLND ll);
  SgExpression(SgExpression &);
  ~SgExpression();
  
  inline SgExpression *lhs();
  inline SgExpression *rhs();
  SgExpression *operand(int i);
  inline int variant();      
  inline SgType *type(); 
  SgSymbol *symbol();
  inline int id();
  inline SgExpression *nextInExprTable();
  
  inline void setLhs(SgExpression &e);
  inline void setLhs(SgExpression *e);
  inline void setRhs(SgExpression &e);
  inline void setRhs(SgExpression *e);
  inline void setSymbol(SgSymbol &s);
  inline void setSymbol(SgSymbol *s);
  inline void setType(SgType &t);
  inline void setType(SgType *t);
  inline void setVariant(int v);
  
  inline SgExpression &copy();
  inline SgExpression *copyPtr();
  char *unparse(); 
  std::string sunparse();
  inline void unparsestdout();
  inline SgExpression *IsSymbolInExpression(SgSymbol &symbol);
  inline void replaceSymbolByExpression(SgSymbol &symbol, SgExpression &expr);
  inline SgExpression *symbRefs();
  inline SgExpression *arrayRefs(); 
  int  linearRepresentation(int *coeff, SgSymbol **symb,int *cst, int size);
  SgExpression *normalForm(int n, SgSymbol *s);        
  SgExpression *coefficient(SgSymbol &s); 
  int isInteger();
  int valueInteger();

friend SgExpression &operator + ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator - ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator * ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator / ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator % ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator <<( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator >>( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator < ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator > ( SgExpression &lhs, SgExpression &rhs);
friend SgExpression &operator <= ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator >= ( SgExpression &lhs, SgExpression &rhs);
friend SgExpression &operator & ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator | ( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator &&( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator ||( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator +=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator &=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator *=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator /=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator %=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator ^=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator <<=( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &operator >>=( SgExpression &lhs, SgExpression &rhs);
friend SgExpression &operator ==(SgExpression &lhs, SgExpression &rhs);
friend SgExpression &operator !=(SgExpression &lhs, SgExpression &rhs);
friend SgExpression &SgAssignOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgEqOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgNeqOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgExprListOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgRecRefOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgPointStOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgScopeOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgDDotOp( SgExpression &lhs, SgExpression &rhs); 
friend SgExpression &SgBitNumbOp( SgExpression &lhs, SgExpression &rhs); 

  /////////////// FOR ATTRIBUTES //////////////////////////

  int numberOfAttributes();
  int numberOfAttributes(int type); // of a specified type;
  void *attributeValue(int i); 
  int  attributeType(int i); 
  void *attributeValue(int i,int type); // only considering one type attribute
  void *deleteAttribute(int i); 
  void addAttribute(int type, void *a, int size); // void * can be NULL;
  void addAttribute(int type); //void * is NULL;
  void addAttribute(void *a, int size); //no type specifed;
  void addAttribute(SgAttribute *att);
  SgAttribute *getAttribute(int i);
  SgAttribute *getAttribute(int i,int type);
  void addAttributeTree(SgAttribute* firstAtt);
};

class SgSymbol{
private:
    // copyed by Yashin 08.09.2018
    int fileID;
    SgProject *project;
    //

public:
  // basic class contains
  PTR_SYMB thesymb;
  SgSymbol(int variant, const char *identifier, SgType &t, SgStatement &scope);
  SgSymbol(int variant, const char *identifier, SgType *t, SgStatement *scope);
  SgSymbol(int variant, const char *identifier,  SgStatement &scope);
  SgSymbol(int variant, const char *identifier,  SgStatement *scope);
  SgSymbol(int variant, const char *identifier,   SgType *type, SgStatement *scope, SgSymbol *structsymb, SgSymbol *nextfield );

  SgSymbol(int variant, const char *name);
  SgSymbol(int variant);
  SgSymbol(PTR_SYMB symb);
#if __SPF
  SgSymbol(const SgSymbol &);
#endif
  ~SgSymbol();
  inline int variant();  
  inline int id();             // unique identifier
  inline char *identifier();   // the text name for the symbol.
  inline SgType *type();       // the type of the symbol
  inline void setType(SgType &t);       // the type of the symbol
  inline void setType(SgType *t);       // the type of the symbol
  inline SgStatement *scope(); // the SgControlStatement where defined.
  inline SgSymbol *next();     // next symbol reference.
  SgStatement *declaredInStmt();  // the declaration statement
  inline SgSymbol &copy();
  inline SgSymbol &copyLevel1(); // copy also parameters
  inline SgSymbol &copyLevel2(); // copy parameters, body also
  inline SgSymbol &copyAcrossFiles(SgStatement &where); // special copy to move things from a file.
  inline SgSymbol &copySubprogram(SgStatement &where); // special copy for inline expansion 07.06.06
  int attributes();    // the Fortran 90 attributes
  void setAttribute(int attribute);
  void removeAttribute(int attribute);
  void declareTheSymbol(SgStatement &st);
  inline void declareTheSymbolWithParamList
                          (SgStatement &st, SgExpression &parlist);
  SgExpression *makeDeclExpr();
  inline SgExpression *makeDeclExprWithParamList
                          (SgExpression &parlist);
  SgVarDeclStmt *makeVarDeclStmt();
  SgVarDeclStmt *makeVarDeclStmtWithParamList
                          (SgExpression &parlist);
 
  SgStatement *body(); // the body of the symbol if has one (like, function call, class,...)
  inline SgSymbol *moduleSymbol();  // module symbol reference  "by use"

  // new opportunities were added by Kolganov A.S. 16.04.2018 and copyed by Yashin 08.09.2018
  inline int getFileId() const { return fileID; }
  inline void setFileId(const int newFileId) { fileID = newFileId; }
  void changeName(const char *); // set new name for the symbol

  inline SgProject* getProject() const { return project; }
  inline void setProject(SgProject *newProj) { project = newProj; }

  inline bool switchToFile()
  {
      if (fileID == -1 || project == NULL)
          return false;

      if (current_file_id != fileID)
          SgFile *file = &(project->file(fileID));
      return true;
  }

  inline SgFile* getFile() const
  {
      if (fileID == -1 || project == NULL)
          return NULL;
      else
          return &(project->file(fileID));
  }
  //

  /////////////// FOR ATTRIBUTES //////////////////////////

  int numberOfAttributes();
  int numberOfAttributes(int type); // of a specified type;
  void *attributeValue(int i); 
  int  attributeType(int i); 
  void *attributeValue(int i,int type); // only considering one type attribute
  void *deleteAttribute(int i); 
  void addAttribute(int type, void *a, int size); // void * can be NULL;
  void addAttribute(int type); //void * is NULL;
  void addAttribute(void *a, int size); //no type specifed;
  void addAttribute(SgAttribute *att);
  SgAttribute *getAttribute(int i);
  SgAttribute *getAttribute(int i,int type);
};


/* This code by Andrew Mauer (ajm) */
/* These constants are used by SgType::maskDescriptors() and 
   SgType::getTrueType(). */

const int MASK_NO_DESCRIPTORS = ~0; /* all ones = keep everything */
const int MASK_MOST_DESCRIPTORS =  ( BIT_SIGNED | BIT_UNSIGNED
				     | BIT_LONG | BIT_SHORT
				     | BIT_CONST | BIT_VOLATILE );

const int MASK_ALL_DESCRIPTORS = 0; /* keep nothing */


class SgType{
public:
  PTR_TYPE thetype;
  SgType(int variant);
  SgType(int var, SgExpression *len,SgType *base);
  SgType(int var, SgSymbol *symb);
  SgType(int var, SgSymbol *firstfield, SgStatement *structstmt);
  SgType(int var, SgSymbol *symb, SgExpression *len, SgType *base);
  SgType(PTR_TYPE type);
  SgType(SgType &);
  ~SgType();
  inline int variant();
  inline int id();
  inline SgSymbol *symbol();
  inline SgType &copy();
  inline SgType *copyPtr();
  inline SgType *next();
  inline int isTheElementType();
  inline int equivalentToType(SgType &type);
  inline int equivalentToType(SgType *type);
  inline SgType *internalBaseType();
  inline int hasBaseType();
  inline SgType *baseType();
  inline SgExpression *length(); // update Kataev N.A. 30.08.2013
  inline SgExpression *selector(); // update Kataev N.A. 30.08.2013

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

   SgType *maskDescriptors (int mask);


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

  SgType *getTrueType (int mask = MASK_MOST_DESCRIPTORS, 
                       int follow_typedefs = 0);

  int numberOfAttributes();
  int numberOfAttributes(int type); // of a specified type;
  void *attributeValue(int i); 
  int  attributeType(int i); 
  void *attributeValue(int i,int type); // only considering one type attribute
  void *deleteAttribute(int i); 
  void addAttribute(int type, void *a, int size); // void * can be NULL;
  void addAttribute(int type); //void * is NULL;
  void addAttribute(void *a, int size); //no type specifed;
  void addAttribute(SgAttribute *att);
  SgAttribute *getAttribute(int i);
  SgAttribute *getAttribute(int i,int type);
};

// SgMakeDeclExp can be called by the user to generate declaration
// expressions from type strings.  it handles all C types.
SgExpression *SgMakeDeclExp(SgSymbol *sym, SgType *t);


class SgLabel{
public:
  PTR_LABEL thelabel;
  SgLabel(PTR_LABEL lab);
  SgLabel(SgLabel &);
  SgLabel(int i);
  inline int getLabNumber() { return (int)(thelabel->stateno); }
  inline int id();
  inline int getLastLabelVal();
  ~SgLabel();

  /***** Kataev 21.03.2013 *****/
  int numberOfAttributes();
  int numberOfAttributes(int type); // of a specified type;
  void *attributeValue(int i); 
  int  attributeType(int i); 
  void *attributeValue(int i,int type); // only considering one type attribute
  void *deleteAttribute(int i); 
  void addAttribute(int type, void *a, int size); // void * can be NULL;
  void addAttribute(int type); //void * is NULL;
  void addAttribute(void *a, int size); //no type specifed;
  void addAttribute(SgAttribute *att);
  SgAttribute *getAttribute(int i);
  SgAttribute *getAttribute(int i,int type);
  /*****************************/
};

class  SgValueExp: public SgExpression{
  // a value of one of the base types
  // variants: INT_VAL, CHAR_VAL, FLOAT_VAL, 
  //           DOUBLE_VAL, STRING_VAL, COMPLEX_VAL, KEYWORD_VAL
public:
  inline SgValueExp(bool value); // add for bool value (Kolganov, 26.11.2019)
  inline SgValueExp(int value);
  inline SgValueExp(char char_val);
  inline SgValueExp(float float_val);
  inline SgValueExp(double double_val);
  inline SgValueExp(float float_val, char*);
  inline SgValueExp(double double_val, char*);
  inline SgValueExp(char *string_val);
  inline SgValueExp(const char *string_val);
  inline SgValueExp(double real, double imaginary);
  inline SgValueExp(SgValueExp &real, SgValueExp &imaginary);
  inline void setValue(int int_val);
  inline void setValue(char char_val);
  inline void setValue(float float_val);
  inline void setValue(double double_val);
  inline void setValue(char *string_val);
  inline void setValue(double real, double im);
  inline bool boolValue(); // add for bool value (Kataev, 16.03.2013)
  inline void setValue(SgValueExp &real, SgValueExp & im);
  inline int intValue();
  inline char*  floatValue();
  inline char  charValue();
  inline char*  doubleValue();
  inline char * stringValue();
  inline SgExpression *realValue();
  inline SgExpression *imaginaryValue();
};

class  SgKeywordValExp: public SgExpression{
public:
  inline SgKeywordValExp(char *name);
  inline SgKeywordValExp(const char *name);
  inline char *value();
};


class  SgUnaryExp: public  SgExpression{
public:
  inline SgUnaryExp(PTR_LLND ll);
  inline SgUnaryExp(int variant, SgExpression & e);
  inline SgUnaryExp(int variant, int post, SgExpression & e);
  inline int post();
  SgExpression &operand();
};

class  SgCastExp: public SgExpression{
public:
  inline SgCastExp(PTR_LLND ll);
  inline SgCastExp(SgType &t, SgExpression &e);
  inline SgCastExp(SgType &t);
  inline ~SgCastExp();
};

// delete [size]  expr
// variant == DELETE_OP
class SgDeleteExp: public SgExpression{
public:
  inline SgDeleteExp(PTR_LLND ll);
  inline SgDeleteExp(SgExpression &size, SgExpression &expr);
  inline SgDeleteExp(SgExpression &expr);
  inline ~SgDeleteExp();
};

// new typename
// new typename (expr)
// variant == NEW_OP
class SgNewExp: public SgExpression{
public:
  inline SgNewExp(PTR_LLND ll);
  inline SgNewExp(SgType &t);
  inline SgNewExp(SgType &t, SgExpression &e);
#if 0
  SgExpression &numberOfArgs();
  SgExpression &argument(int i);
#endif
  ~SgNewExp();
};

// functions here can use LlndMapping perhaps.
class SgExprIfExp: public SgExpression{
  //  (expr1)? expr2 : expr3
  // variant == EXPR_IF
public:
  inline SgExprIfExp(PTR_LLND ll);
  inline SgExprIfExp(SgExpression &exp1,SgExpression &exp2, SgExpression &exp3);
  SgExpression &conditional();
  SgExpression &trueExp();
  SgExpression &falseExp();
  inline void setConditional(SgExpression &c);
  void setTrueExp(SgExpression &t);
  void setFalseExp(SgExpression &f);
  ~SgExprIfExp();
};

class SgFunctionRefExp: public SgExpression{
   // function_name(formal args)  - for function headers and protytpes.
   // variant = FUNCTION_REF
public:
  inline SgFunctionRefExp(PTR_LLND ll);
  inline SgFunctionRefExp(SgSymbol &fun);
  inline ~SgFunctionRefExp();
  inline SgSymbol *funName();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  SgExpression * AddArg(char *, SgType &);
};

class SgFunctionCallExp: public SgExpression{
  // function_name(expr1, expr2, ....)
  // variant == FUNC_CALL
public:
  inline SgFunctionCallExp(PTR_LLND ll);
  inline SgFunctionCallExp(SgSymbol &fun, SgExpression &paramList);
  inline SgFunctionCallExp(SgSymbol &fun);
  inline ~SgFunctionCallExp();
  inline SgSymbol *funName();
  inline SgExpression *args();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);
};

class SgFuncPntrExp: public SgExpression{
  // (functionpointer)(expr1,expr2,expr3)
  // variant == FUNCTION_OP
public:
  inline SgFuncPntrExp(PTR_LLND ll);
  inline SgFuncPntrExp(SgExpression &ptr);
  inline ~SgFuncPntrExp();
  inline SgExpression *funExp();
  inline void setFunExp(SgExpression &s);
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
  inline void addArg(SgExpression &arg);  // add an argument.
  SgExpression* AddArg(SgSymbol *thefunc, char *name, SgType &); 
              // add a formal parameter
              // to a pointer to a function prototype or parameter.
              // this returns the expression
};

class SgExprListExp: public SgExpression{
        // variant == EXPR_LIST
public:
  inline SgExprListExp(PTR_LLND ll);
  inline SgExprListExp();
  inline SgExprListExp(SgExpression &ptr);

  // create new constructor for every variant, 
  // added by Kolganov A.S. 31.10.2013
  inline SgExprListExp(int variant);

  inline ~SgExprListExp();
  inline int length();
  inline SgExpression *elem(int i);
  inline SgExprListExp *next();
  inline SgExpression *value();
  inline void setValue(SgExpression &ptr);      
  inline void append(SgExpression &arg);
  void linkToEnd(SgExpression &arg);
};

class  SgRefExp: public SgExpression{
  // Fortran name references
  // variant == CONST_REF, TYPE_REF, INTERFACE_REF
public:
  inline SgRefExp(PTR_LLND ll);
  inline SgRefExp(int variant, SgSymbol &s);
  inline ~SgRefExp();
};

class SgTypeRefExp: public SgExpression{
   // a reference to a type in a c++ template argument 
  public:
    inline SgTypeRefExp(SgType &t);
    inline ~SgTypeRefExp();
    inline SgType *getType();
};

class  SgVarRefExp: public SgExpression{
  // scalar variable reference or non-indexed array reference
  // variant == VAR_REF
public:
  inline SgVarRefExp (PTR_LLND ll);
  inline SgVarRefExp(SgSymbol &s);
  inline SgVarRefExp(SgSymbol *s);
  SgExpression *progatedValue(); // if scalar propogation worked
  inline ~SgVarRefExp();
};


class  SgThisExp: public SgExpression{
  // variant == THIS_NODE
public:
  inline SgThisExp (PTR_LLND ll);
  inline SgThisExp(SgType &t);
  inline ~SgThisExp();
};

class  SgArrayRefExp: public SgExpression{
  // an array reference
  // variant == ARRAY_REF
public:
  inline SgArrayRefExp(PTR_LLND ll);
  inline SgArrayRefExp(SgSymbol &s);
  inline SgArrayRefExp(SgSymbol &s, SgExpression &subscripts);
  inline SgArrayRefExp(SgSymbol &s, SgExpression &sub1,SgExpression &sub2);
  
  inline SgArrayRefExp(SgSymbol &s, SgExpression &sub1,SgExpression &sub2,SgExpression &sub3);
  
  inline SgArrayRefExp(SgSymbol &s, SgExpression &sub1,SgExpression &sub2,SgExpression &sub3,SgExpression &sub4);
  inline ~SgArrayRefExp();
  inline int numberOfSubscripts();  // the number of subscripts in reference
  inline SgExpression *subscripts();
  inline SgExpression *subscript(int i);
  inline void addSubscript(SgExpression &e);
  inline void setSymbol(SgSymbol &s);
};

// set NODE _TYPE.
class SgPntrArrRefExp: public SgExpression{
public:
  inline SgPntrArrRefExp(PTR_LLND ll);
  inline SgPntrArrRefExp(SgExpression &p);
  inline SgPntrArrRefExp(SgExpression &p, SgExpression &subscripts);
  inline SgPntrArrRefExp(SgExpression &p, int n, SgExpression &sub1, SgExpression &sub2);
  inline SgPntrArrRefExp(SgExpression &p, int n, SgExpression &sub1, SgExpression &sub2, SgExpression &sub3);
  inline SgPntrArrRefExp(SgExpression &p, int n, SgExpression &sub1, SgExpression &sub2, SgExpression &sub3, SgExpression &sub4);
  inline ~SgPntrArrRefExp();
  inline int dimension();  // the number of subscripts in reference
  inline SgExpression *subscript(int i);
  inline void addSubscript(SgExpression &e);
  inline void setPointer(SgExpression &p);
};

class SgPointerDerefExp: public SgExpression{
  // pointer dereferencing
  // variant == DEREF_OP
public:
  inline SgPointerDerefExp(PTR_LLND ll);
  inline SgPointerDerefExp(SgExpression &pointerExp);
  inline ~SgPointerDerefExp();
  inline SgExpression *pointerExp();
};

class SgRecordRefExp: public SgExpression{
  // a field reference of a structure
  // variant == RECORD_REF
public:
  inline SgRecordRefExp(PTR_LLND ll);
  inline SgRecordRefExp(SgSymbol &recordName, char *fieldName);
  inline SgRecordRefExp(SgExpression &recordExp, char *fieldName);
  inline SgRecordRefExp(SgSymbol &recordName, const char *fieldName);
  inline SgRecordRefExp(SgExpression &recordExp, const char *fieldName);
  inline ~SgRecordRefExp();
  inline SgSymbol *fieldName();
  inline SgSymbol *recordName();
  inline SgExpression *record();
};


class SgStructConstExp:  public SgExpression{
  // Fortran 90 structure constructor
  // variant == STRUCTURE_CONSTRUCTOR
public:
  inline SgStructConstExp(PTR_LLND ll);
  // further checks on values need to be done.
  inline SgStructConstExp(SgSymbol &structName, SgExpression &values);
  inline SgStructConstExp(SgExpression  &typeRef, SgExpression &values);
  inline ~SgStructConstExp();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
};

class SgConstExp:  public SgExpression{
public:
  inline SgConstExp(PTR_LLND ll);
  inline SgConstExp(SgExpression &values);
  inline ~SgConstExp();
  inline int numberOfArgs();
  inline SgExpression *arg(int i);
};

class SgVecConstExp: public SgExpression{
  // a vector constant of the form: [ expr1, expr2, expr3]
  // variant == VECTOR_CONST
public:
  inline SgVecConstExp(PTR_LLND ll);
  inline SgVecConstExp(SgExpression &expr_list);
  inline SgVecConstExp(int n, SgExpression *components);
  inline ~SgVecConstExp();
  
  inline SgExpression *arg(int i); // the i-th term
  inline int numberOfArgs();
  inline void setArg(int i, SgExpression &e);
};

class SgInitListExp: public SgExpression{
  // used for initializations.  form: { expr1,expr2,expr3}
  // variant == INIT_LIST
public:
  inline SgInitListExp(PTR_LLND ll);
  inline SgInitListExp(SgExpression &expr_list);
  inline SgInitListExp(int n, SgExpression *components);
  inline ~SgInitListExp();
  
  inline SgExpression *arg(int i); // the i-th term
  inline int numberOfArgs();
  inline void setArg(int i, SgExpression &e);
};

class SgObjectListExp: public SgExpression{
  // used for EQUIVALENCE, NAMELIST and COMMON statements
  // variant == EQUI_LIST, NAMELIST_LIST, COMM_LIST
public:
  inline SgObjectListExp(PTR_LLND ll);
  inline SgObjectListExp(int variant, SgSymbol &object, SgExpression &list);
  inline SgObjectListExp(int variant,SgExpression &objectRef, SgExpression &list);
  inline ~SgObjectListExp();
  inline SgSymbol *object();    //fix Kataev N.A. 20.03.2014
  inline SgObjectListExp * next( ); //add Kataev N.A. 20.03.2014
  inline SgExpression * body( ); //rename from objectRef( ) Kataev N.A. 20.03.2014
  inline int listLength();      // fix Kataev N.A. 20.03.2014
  inline SgExpression object( int i); //add Kataev N.A. 20.03.2014
  inline SgSymbol *symbol(int i);   // fix Kataev N.A. 20.03.2014
  inline SgExpression *body(int i);  // rename from objectRef( int) and fix Kataev N.A. 20.03.2014
};


class SgAttributeExp: public SgExpression{
  // Fortran 90 attributes
  // variant ==  PARAMETER_OP, PUBLIC_OP, PRIVATE_OP, ALLOCATABLE_OP,
  // DIMENSION_OP, EXTERNAL_OP, IN_OP, OUT_OP, INOUT_OP, INTRINSIC_OP, 
  // POINTER_OP, OPTIONAL_OP, SAVE_OP, TARGET_OP
public:
  inline SgAttributeExp(PTR_LLND ll);
  inline SgAttributeExp(int variant);
  inline ~SgAttributeExp();
};

class SgKeywordArgExp: public SgExpression{
  // Fortran 90 keyword argument
  // variant == KEYWORD_ARG
public:
  inline SgKeywordArgExp(PTR_LLND ll);
  inline SgKeywordArgExp(char *argName, SgExpression &exp);
  inline SgKeywordArgExp(const char *argName, SgExpression &exp);
  inline ~SgKeywordArgExp();
  //inline SgSymbol *arg(); does not work, always return NULL
  inline SgExpression *arg(); //! now return SgKeywordValueExp (Kataev N.A. 30.05.2013)
  inline SgExpression *value();
};

class SgSubscriptExp: public SgExpression{
  // Fortran 90 vector subscript expression
  // variant == DDOT
public:
  inline SgSubscriptExp(PTR_LLND ll);
  inline SgSubscriptExp(SgExpression &lbound, SgExpression &ubound, SgExpression &step);
  inline SgSubscriptExp(SgExpression &lbound, SgExpression &ubound);
  inline ~SgSubscriptExp();
  // perhaps this function can use LlndMapping
  SgExpression *lbound();
  SgExpression *ubound();
  SgExpression *step();
};

class SgUseOnlyExp: public SgExpression{
  // Fortran 90 USE statement ONLY attribute 
  // variant == ONLY_NODE
public:
  inline SgUseOnlyExp(PTR_LLND ll);
  inline SgUseOnlyExp(SgExpression &onlyList);
  inline ~SgUseOnlyExp();
  inline SgExpression *onlyList();
};

class SgUseRenameExp: public SgExpression{
  // Fortran 90 USE statement renaming
  // variant == RENAME_NODE
public:
  inline SgUseRenameExp(PTR_LLND ll);
  inline SgUseRenameExp(SgSymbol &newName, SgSymbol &oldName);
  inline ~SgUseRenameExp();
  inline SgSymbol *newName();
  inline SgSymbol *oldName();
  inline SgExpression *newNameExp();
  inline SgExpression *oldNameExp();
};


class SgSpecPairExp: public SgExpression{
  // Fortran default control arguments to Input/Output statements
  // variant == SPEC_PAIR
public:
  inline SgSpecPairExp(PTR_LLND ll);
  inline SgSpecPairExp(SgExpression &arg, SgExpression &value);
  inline SgSpecPairExp(SgExpression &arg);
  inline SgSpecPairExp(char *arg, char *value);
  inline ~SgSpecPairExp();
  inline SgExpression *arg();
  inline SgExpression *value();
};


//used for do-loop range representation also.
// this form needs to be standardized. 
class SgIOAccessExp: public SgExpression{
  // Fortran index variable bound instantiation
  // variant == IOACCESS
public:
  inline SgIOAccessExp(PTR_LLND ll);
  // type-checking on bounds needs to be done.
  // Float values are legal in some cases. check manual.
  inline SgIOAccessExp(SgSymbol &s, SgExpression lbound, SgExpression ubound, SgExpression step);
  inline SgIOAccessExp(SgSymbol &s, SgExpression lbound, SgExpression ubound);
  inline ~SgIOAccessExp();
};

class SgImplicitTypeExp: public SgExpression{
  // Fortran index variable bound instantiation
  // variant == IMPL_TYPE
public:
  inline SgImplicitTypeExp(PTR_LLND ll);
  inline SgImplicitTypeExp(SgType &type, SgExpression &rangeList);
  inline ~SgImplicitTypeExp();
  inline SgType *type();
  inline SgExpression *rangeList();
  inline char *alphabeticRange();
};

class SgTypeExp: public SgExpression{
  // Fortran type expression 
  // variant == TYPE_OP
public:
  inline SgTypeExp(PTR_LLND ll);
  inline SgTypeExp(SgType &type);
  inline ~SgTypeExp();
  inline SgType *type();
};

class SgSeqExp: public SgExpression{
  // Fortran index variable bound instantiation
  // variant == SEQ
public:
  inline SgSeqExp(PTR_LLND ll);
  inline SgSeqExp(SgExpression &exp1, SgExpression &exp2);
  inline ~SgSeqExp();
  inline SgExpression *front();
  inline SgExpression *rear();
};

class SgStringLengthExp: public SgExpression{
  // Fortran index variable bound instantiation
  // variant == LEN_OP
public:
  inline SgStringLengthExp(PTR_LLND ll);
  inline SgStringLengthExp(SgExpression &length);
  inline ~SgStringLengthExp();
  inline SgExpression *length();
};

class SgDefaultExp: public SgExpression {
  // Fortran default node
  // variant == DEFAULT
public:
  SgDefaultExp(PTR_LLND ll);
  SgDefaultExp();
  ~SgDefaultExp();
};

class SgLabelRefExp: public SgExpression{
  // Fortran label reference
  // variant == LABEL_REF
public:
  inline SgLabelRefExp(PTR_LLND ll);
  inline SgLabelRefExp(SgLabel &label);
  inline ~SgLabelRefExp();
  inline SgLabel *label();
};


class  SgProgHedrStmt: public SgStatement{
  // fortran Program block
  // variant == PROG_HEDR
public:
  inline SgProgHedrStmt(PTR_BFND bif);
  inline SgProgHedrStmt(int variant);
  inline SgProgHedrStmt(SgSymbol &name, SgStatement &Body);
  inline SgProgHedrStmt(SgSymbol &name);
  inline SgProgHedrStmt(char *name);
  inline SgSymbol &name();
  // added 15.08.2018 by A.S. Kolganov. <contains cp name>.funcName
  inline std::string nameWithContains()
  {
      std::string containsName = "";
      SgStatement *st_cp = this->controlParent();
      if (st_cp->variant() == PROC_HEDR || st_cp->variant() == PROG_HEDR || st_cp->variant() == FUNC_HEDR)
          containsName = st_cp->symbol()->identifier() + std::string(".");

      return containsName + this->symbol()->identifier();
  }

  inline void setName(SgSymbol &symbol); // set program name 

  inline int numberOfFunctionsCalled();  // the number of functions called
  inline SgSymbol *calledFunction(int i);// the i-th called function
  inline int numberOfStmtFunctions();   // the number of statement funcions;
  inline SgStatement *statementFunc(int i); // the i-th statement function;
  inline int numberOfEntryPoints();     // the number of entry points;
  inline SgStatement *entryPoint(int i); // the i-th entry point;
  inline int numberOfParameters();       // the number of parameters;       
  inline SgSymbol *parameter(int i);     // the i-th parameter  
  inline int numberOfSpecificationStmts();
  inline SgStatement *specificationStmt(int i);
  inline int numberOfExecutionStmts();
  inline SgStatement *executionStmt(int i);
  inline int numberOfInternalFunctionsDefined();
  inline SgStatement *internalFunction(int i);
  inline int numberOfInternalSubroutinesDefined();
  inline SgStatement *internalSubroutine(int i);
  inline int numberOfInternalSubProgramsDefined();
  inline SgStatement *internalSubProgram(int i);
  
#if 0
  SgSymbol &addVariable(SgType &T, char *name); 
                                        //add a declaration for new variable
  
  SgStatement &addCommonBlock(char *blockname, int noOfVars,
                              SgSymbol *Vars); // add a new common block
#endif        
  inline int isSymbolInScope(SgSymbol &symbol);
  inline int isSymbolDeclaredHere(SgSymbol &symbol);
  
  // global analysis data
  
  inline int numberOfVarsUsed();  // list of used variable access sections
  inline SgExpression *varsUsed(int i); // i-th var used section descriptor
  inline int numberofVarsMod();   // list of modifed variable access sections
  inline SgExpression *varsMod(int i);   // i-th var mod section descriptor
  inline ~SgProgHedrStmt();
};

class  SgProcHedrStmt: public SgProgHedrStmt{
  // Fortran subroutine
  // variant == PROC_HEDR
public:        
  inline SgProcHedrStmt(int variant);
  inline SgProcHedrStmt(SgSymbol &name, SgStatement &Body);
  inline SgProcHedrStmt(SgSymbol &name);
  inline SgProcHedrStmt(const char *name);
  inline void AddArg(SgExpression &arg); 
  SgExpression * AddArg(char *name, SgType &t); // returns decl expr created.
  SgExpression * AddArg(char *name, SgType &t, SgExpression &initializer);
  inline int isRecursive();  // 1 if recursive.;
  inline int numberOfEntryPoints();      // the number of entry points
                                         // other than the main, 0 for C funcs.
  inline SgStatement *entryPoint(int i);  // the i-th entry point
  // this is incorrect. Takes only subroutines calls into account.
  // Should be modified to take function calls into account too.
  inline int numberOfCalls();            // number of calls to this proc.
  inline SgStatement *call(int i);       // position of the i-th call.  
  inline ~SgProcHedrStmt();
};      


class  SgProsHedrStmt: public SgProgHedrStmt{
  // Fortran M process
  // variant == PROS_HEDR
public:        
  inline SgProsHedrStmt();
  inline SgProsHedrStmt(SgSymbol &name, SgStatement &Body);
  inline SgProsHedrStmt(SgSymbol &name);
  inline SgProsHedrStmt(char *name);
  inline void AddArg(SgExpression &arg);
  inline int numberOfCalls();            // number of calls to this proc.
  inline SgStatement *call(int i);       // position of the i-th call.  
  inline ~SgProsHedrStmt();
};      


class  SgFuncHedrStmt: public SgProcHedrStmt{
  // Fortran and C function.
  // variant == FUNC_HEDR
public:
  inline SgFuncHedrStmt(SgSymbol &name, SgStatement &Body);
  inline SgFuncHedrStmt(SgSymbol &name, SgType &type, SgStatement &Body);
  inline SgFuncHedrStmt(SgSymbol &name, SgSymbol &resultName, SgType &type, SgStatement &Body);
  inline SgFuncHedrStmt(SgSymbol &name);
  inline SgFuncHedrStmt(SgSymbol &name, SgExpression *exp);
  inline SgFuncHedrStmt(char *name);
  inline ~SgFuncHedrStmt();
  
  inline SgSymbol *resultName();  // name of result variable.;
  int  setResultName(SgSymbol &symbol); // set name of result variable.;
  
  inline SgType *returnedType();  // type of returned value
  inline void setReturnedType(SgType &type);  // set type of returned value
};

class SgClassStmt;

class SgTemplateStmt: public SgStatement{
  // This is a function template or class template
  // in both cases the variant is TEMPLATE_FUNDECL
public:
   SgTemplateStmt(SgExpression *arglist);
  SgExpression * AddArg(char *name, SgType &t); // returns decl expr created.
     // if name == NULL then this is a type reference.
  SgExpression * AddArg(char *name, SgType &t, SgExpression &initializer);
   int numberOfArgs();
   SgExpression *arg(int i);
   SgExpression *argList();
   void addFunction(SgFuncHedrStmt &theTemplateFunc);
   void addClass(SgClassStmt &theTemplateClass);
   SgFuncHedrStmt *isFunction();
   SgClassStmt *isClass();
};

#if 0
class  SgModuleStmt: public SgStatement{
  // Fortran 90 Module statement
  // variant ==  MODULE_STMT
public:
  SgModuleStmt(SgSymbol &moduleName, SgStatement &body);
  SgModuleStmt(SgSymbol &moduleName);
  ~SgModuleStmt();
  
  SgSymbol *moduleName();               // module name 
  void setName(SgSymbol &symbol);        // set module name 
  
  int numberOfSpecificationStmts();
  int numberOfRoutinesDefined();
  int numberOfFunctionsDefined();
  int numberOfSubroutinesDefined();
  
  SgStatement *specificationStmt(int i);
  SgStatement *routine(int i);
  SgStatement *function(int i);
  SgStatement *subroutine(int i);
  
  int isSymbolInScope(SgSymbol &symbol);
  int isSymbolDeclaredHere(SgSymbol &symbol);
  
  SgSymbol &addVariable(SgType &T, char *name); 
                                        //add a declaration for new variable
  
  SgStatement *addCommonBlock(char *blockname, int noOfVars,
                              SgSymbol *Vars); // add a new common block
};

class  SgInterfaceStmt: public SgStatement{
  // Fortran 90 Operator Interface Statement
  // variant == INTERFACE_STMT
public:
  SgInterfaceStmt(SgSymbol &name, SgStatement &body, SgStatement &scope);
  ~SgInterfaceStmt();
  
  SgSymbol *interfaceName();               // interface name if given
  int setName(SgSymbol &symbol);           // set interface name 
  
  int numberOfSpecificationStmts();
  
  SgStatement *specificationStmt(int i);
  
  int isSymbolInScope(SgSymbol &symbol);
  int isSymbolDeclaredHere(SgSymbol &symbol);
};

class  SgBlockDataStmt: public SgStatement{
  // Fortran Block Data statement
  // variant == BLOCK_DATA
public:
  SgBlockDataStmt(SgSymbol &name, SgStatement &body);
  ~SgBlockDataStmt();
  
  SgSymbol *name();  // block data name if given;
  int setName(SgSymbol &symbol);           // set block data name 
  
  int isSymbolInScope(SgSymbol &symbol);
  int isSymbolDeclaredHere(SgSymbol &symbol);
};

#endif


class SgClassStmt: public SgStatement{
  // C++ class statement
  //    class name : superclass_list ElementTypeOf collection_name {
  //            body
  //     } variables_list;
  // variant == CLASS_DECL
public:
  inline SgClassStmt(int variant);
  inline SgClassStmt(SgSymbol &name);
  inline ~SgClassStmt();
  inline int numberOfSuperClasses();
  inline SgSymbol *name();
  inline SgSymbol *superClass(int i);
  inline void setSuperClass(int i, SgSymbol &symb);
#if 0
  int numberOfVars();            // variables in variables_list
  SgExpression variable(int i);  // i-th variable in variable_list
  SgExpression collectionName(); // if an ElementType class.
  
  // body manipulation functions.
  int numberOfPublicVars();
  int numberOfPrivateVars();
  int numberOfProtectedVars();
  SgSymbol *publicVar(int i);
  SgSymbol *protectedVar(int i);
  SgSymbol *privateVar(int i);
  void addPublicVar(SgSymbol &s);
  void addPrivateVar(SgSymbol &s);
  void addProtectedVar(SgSymbol &s);
  int numberOfPublicFuns();
  int numberOfPrivateFuns();
  int numberOfProtectedFuns();
  SgStatement *publicFun(int i);
  SgStatement *protectedFun(int i);
  SgStatement *privateFun(int i);
  void addPublicFun(SgStatement &s);
  void addPrivateFun(SgStatement &s);
  void addProtectedFun(SgStatement &s);
#endif
};

class SgStructStmt: public SgClassStmt{
  // basic C++ structure
  // struct name ;
  //                   body
  //              } variables_list;
  // variant == STRUCT_DECL
public:
  // consider like a class.
  inline SgStructStmt();
  inline SgStructStmt(SgSymbol &name);
  inline ~SgStructStmt();
  
};


class SgUnionStmt: public SgClassStmt{
  // basic C++ structure
  // union name  {
  //     body
  //    } variables_list;
  // variant == UNION_DECL
public:
  // consider like a class.
  inline SgUnionStmt();
  inline SgUnionStmt(SgSymbol &name);
  inline ~SgUnionStmt();
};

class SgEnumStmt: public SgClassStmt{
  // basic C++ structure
  // enum name  {
  //     body
  //    } variables_list;
  // variant == ENUM_DECL
public:
  // consider like a class.
  inline SgEnumStmt();
  inline SgEnumStmt(SgSymbol &name);
  inline ~SgEnumStmt();
};

class SgCollectionStmt: public SgClassStmt{
  // basic C++ structure
  // collection name ;
  //                    body
  //                } variables_list;
  // variant == COLLECTION_DECL
public:
  inline SgCollectionStmt();
  inline SgCollectionStmt(SgSymbol &name);
  inline ~SgCollectionStmt();
#if 0
  int numberOfElemMethods();
  SgStatement *elementMethod(int i);
  void addElementMethod(SgStatement &s);
#endif
  inline SgStatement *firstElementMethod();
};

class SgBasicBlockStmt: public SgStatement{
  // in C we have: {  body; }
  // variant == BASIC_BLOCK
public:
  inline SgBasicBlockStmt();
  inline ~SgBasicBlockStmt();
};

// ********************* traditional control Structures ************
class  SgForStmt: public SgStatement{
  // for Fortran Do and C for();
  // variant = FOR_NODE
public:
  inline SgForStmt(SgSymbol &do_var, SgExpression &start, SgExpression &end,
            SgExpression &step, SgStatement &body);
  inline SgForStmt(SgSymbol *do_var, SgExpression *start, SgExpression *end,
            SgExpression *step, SgStatement *body);
  inline SgForStmt(SgSymbol &do_var, SgExpression &start, SgExpression &end,
            SgStatement &body);
  inline SgForStmt(SgExpression &start, SgExpression &end, SgExpression &step,
            SgStatement &body);

  inline SgForStmt(SgExpression *start, SgExpression *end, SgExpression *step, SgStatement *body);
#if __SPF
  inline SgSymbol* doName();
#else
  inline SgSymbol doName();
#endif                          // the name of the loop (for F90.)
  inline void setDoName(SgSymbol &doName);// sets the name of the loop(for F90)

  inline SgExpression *start();            
  inline void setStart(SgExpression &lbound);

  inline SgExpression *end();
  inline void setEnd(SgExpression &ubound);

  inline SgExpression *step();
  inline void setStep(SgExpression &step);  
  inline void interchangeNestedLoops(SgForStmt* loop);

  inline SgLabel *endOfLoop();

//SgExpression &bounds();   // bounds are returned as a triplet lb:ub;
//void setBounds(SgTripletOp &bounds); // bounds are passed as a triplet lb:ub;

  // body is returned with control end statement
  //   still attached. 
  inline SgStatement *body();
  // s is assumed to terminate with a
  //   control end statement.
  inline void set_body(SgStatement &s);
#if 0
  int replaceBody(SgStatement &s); // new body = s and lex successors.
  
  
  int numberOfInductVars();        // 1 if an induction variable can be found.
  SgSymbol *inductionVar(int i);   // i-th induction variable
  SgExpression *indVarRange(int i); // range of i-th ind. var.
#endif
  inline int isPerfectLoopNest();
  inline SgStatement *getNextLoop(); 
  inline SgStatement *getPreviousLoop();   // returns outer nested loop
  inline SgStatement *getInnermostLoop();  // returns innermost nested loop
#if 0
  int isLinearLoopNest();          // TRUE if the bound and step of the loops
                                   // in the loop nest are linear expressions
                                 // and use the index variables of the previous
                                // loops of the nest.
#endif
  inline int isEnddoLoop();            // TRUE if the loop ends with an Enddo
  inline int convertLoop();            // Convert the loop into a Good loop.
#if 0
  int isAssignLoop();          // TRUE if the body consists only of assignments
  int isAssignIfLoop();        // TRUE if the body consists only of assigments
  //             and conditional statements.
  //high level program transformations. 
  // Most are from SIGMA Toolbox by F.Bodin et al.
  // Semantics can be found in the above reference.
  int tiling_p(int i);     
  int tiling(int i, int tab[]);
  int stripMining(int i);
  SgStatement distributeLoop(int i);
  SgStatement distributeLoopSCC();
  SgStatement loopFusion(SgForStmt &loop);
  SgStatement unrollLoop(int i);
  int interchangeLoops(SgForStmt &loop);
  int interchangeWithLoop(int i);
  int normalized();
  int NormalizeLoop();
  int vectorize();
  int vectorizeNest();
  int ExpandScalar(SgSymbol &symbol, int i);
  int ScalarForwardSubstitution(SgSymbol &symbol);
  int pullStatementToFront(SgStatement &s);
  int pullStatementToEnd(SgStatement &s);
#endif
  inline ~SgForStmt();
};


class  SgProcessDoStmt: public SgStatement{
  // for Fortran M ProcessDo statement;
  // variant = PROCESS_DO_STAT
public:
  inline SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                         SgExpression &end, SgExpression &step,
                         SgLabel &endofloop, SgStatement &body);
  inline SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                         SgExpression &end, SgLabel &endofloop,
                         SgStatement &body);
  inline SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                         SgExpression &end, SgExpression &step,
                         SgStatement &body);
  inline SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                         SgExpression &end, SgStatement &body);
  //inline SgSymbol doName();
  inline void setDoName(SgSymbol &doName);
  inline SgExpression *start();            
  inline SgExpression *end();
  inline SgExpression *step();
  inline SgLabel *endOfLoop();
  // body is returned with control end statement
  //   still attached. 
  inline SgStatement *body();
  // s is assumed to terminate with a
  //   control end statement.
  inline void set_body(SgStatement &s);

#if 0
  int replaceBody(SgStatement &s); // new body = s and lex successors.
  
  
  int numberOfInductVars();        // 1 if an induction variable can be found.
  SgSymbol *inductionVar(int i);   // i-th induction variable
  SgExpression *indVarRange(int i); // range of i-th ind. var.
#endif

  inline int isPerfectLoopNest();
  inline SgStatement *getNextLoop(); 
  inline SgStatement *getPreviousLoop();   // returns outer nested loop
  inline SgStatement *getInnermostLoop();  // returns innermost nested loop
#if 0
  int isLinearLoopNest();          // TRUE if the bound and step of the loops
                                   // in the loop nest are linear expressions
                                 // and use the index variables of the previous
                                // loops of the nest.
#endif
  inline int isEnddoLoop();            // TRUE if the loop ends with an Enddo
  inline int convertLoop();            // Convert the loop into a Good loop.
#if 0
  int isAssignLoop();          // TRUE if the body consists only of assignments
  int isAssignIfLoop();        // TRUE if the body consists only of assignments
                               // and conditional statements.
  //high level program transformations. 
  // Most are from SIGMA Toolbox by F.Bodin et al.
  // Semantics can be found in the above reference.
  int tiling_p(int i);     
  int tiling(int i, int tab[]);
  int stripMining(int i);
  SgStatement distributeLoop(int i);
  SgStatement distributeLoopSCC();
  SgStatement loopFusion(SgForStmt &loop);
  SgStatement unrollLoop(int i);
  int interchangeLoops(SgForStmt &loop);
  int interchangeWithLoop(int i);
  int normalized();
  int NormalizeLoop();
  int vectorize();
  int vectorizeNest();
  int ExpandScalar(SgSymbol &symbol, int i);
  int ScalarForwardSubstitution(SgSymbol &symbol);
  int pullStatementToFront(SgStatement &s);
  int pullStatementToEnd(SgStatement &s);
#endif
  inline ~SgProcessDoStmt();
};


class  SgWhileStmt: public SgStatement{
  // for C while()
  // variant = WHILE_NODE
public:
  inline SgWhileStmt(int variant);
  inline SgWhileStmt(SgExpression &cond, SgStatement &body);

  // added by A.S.Kolganov 8.04.2015
  inline SgWhileStmt(SgExpression *cond, SgStatement *body);
  inline SgExpression *conditional();      // the while test
#if 0
  int numberOfInductVars(); // 1 if an induction variable can be found.
  SgSymbol *inductionVar(int i);    // i-th induction variable
  SgExpression *indVarRange(int i); // range of i-th ind. var.
#endif
  inline void replaceBody(SgStatement &s); // new body = s and lex successors.
  inline ~SgWhileStmt();

  // added by A.V.Rakov 16.03.2015
  inline SgStatement *body();
  
  inline SgLabel* endOfLoop( ); //label for end statement in Fortran 'do while' and 'do' loops (16.03.2013, Kataev)
};

class  SgDoWhileStmt: public SgWhileStmt{
  // For Fortran dowhile().. and C do{....) while();
  // variant = DO_WHILE_NODE
public:
  inline SgDoWhileStmt(SgExpression &cond, SgStatement &body);
  inline ~SgDoWhileStmt();
};

// forward reference;
class  SgIfStmt;

class  SgLogIfStmt: public SgStatement{
  // For Fortran logical if  - only one body statement allowed
  // variant == LOGIF_NODE
public:
  inline SgLogIfStmt(int variant);
  inline SgLogIfStmt(SgExpression &cond, SgStatement &s);
  inline SgStatement *body();  // returns reference to first stmt in the body
  inline SgExpression *conditional();  // the while test
  // check if the statement s is a single statement. 
  inline void setBody(SgStatement &s); // new body = s 
  // this code won't work, since after the addition false
  //   clause, it should become SgIfThenElse statement.
  inline void addFalseClause(SgStatement &s); // make it into if-then-else
  inline SgIfStmt *convertLogicIf();
  inline ~SgLogIfStmt();
};

class  SgIfStmt: public SgStatement{
  // For Fortran if then else and C if()
  // variant == IF_NODE
public:
  inline SgIfStmt(int variant);
  inline SgIfStmt(SgExpression &cond, SgStatement &trueBody, SgStatement &falseBody,
                  SgSymbol &construct_name);
  inline SgIfStmt(SgExpression &cond, SgStatement &trueBody, SgStatement &falseBody);
  inline SgIfStmt(SgExpression &cond, SgStatement &trueBody);

  // added by A.S. Kolganov 02.07.2014, updated 21.12.2014
  inline SgIfStmt(SgExpression &cond, SgStatement &body, int t);
  inline SgIfStmt(SgExpression &cond);

  // added by A.S. Kolganov 27.07.2018,
  inline void setBodies(SgStatement *trueBody, SgStatement *falseBody);
  inline SgStatement *trueBody();      // the first stmt in the True clause
  // SgBlock is needed? 
  inline SgStatement *trueBody(int i); // i-th stmt in True clause
  inline SgStatement *falseBody();     // the first stmt in the False
  inline SgStatement *falseBody(int i);// i-th statement of the body.
  inline SgExpression *conditional();  // the while test
  inline SgSymbol *construct_name();
  inline void replaceTrueBody(SgStatement &s);// new body=s and lex successors.
  inline void replaceFalseBody(SgStatement &s);//new body=s and lex successors.
  inline ~SgIfStmt();
};

#if 0
class  SgIfElseIfStmt: public SgIfStmt {
  // For Fortran if then elseif .. elseif ... case
  // variant == ELSEIF_NODE
public:
  SgIfElseIfStmt(SgExpression &condList, SgStatement &blockList, SgSymbol &constructName);
  SgIfElseIfStmt(SgExpression &condList, SgStatement &blockList);
  int numberOfConditionals();       // the number of conditionals
  SgStatement *body(int b);          // block b
  void setBody(int b);              // sets block 
  SgExpression *conditional(int i); // the i-th conditional
  void setConditional(int i);       // sets the i-th conditional
  void addClause(SgExpression &cond, SgStatement &block);
  void removeClause(int b);          // removes block b and it's conditional
  ~SgIfElseIfStmt();
};

inline SgIfElseIfStmt::~SgIfElseIfStmt() { RemoveFromTableBfnd((void *) this); }
#endif


class  SgArithIfStmt: public SgStatement{
  // For Fortran Arithementic if
  // variant == ARITHIF_NODE
public:
  inline SgArithIfStmt(int variant);
  inline SgArithIfStmt(SgExpression &cond, SgLabel &llabel, SgLabel &elabel, SgLabel &glabel);
  inline SgExpression *conditional();
  inline void set_conditional(SgExpression &cond);
  inline SgExpression *label(int i);  // the <, ==, and > goto labels. in order 0->2.
  inline void setLabel(SgLabel &label);
  inline ~SgArithIfStmt();
};

class  SgWhereStmt: public SgLogIfStmt{
  // fortran Where stmt
  // variant == WHERE_NODE
public:
  inline SgWhereStmt(SgExpression &cond, SgStatement &body);
  inline ~SgWhereStmt();
};

class  SgWhereBlockStmt: public SgIfStmt{
  // fortran Where - Elsewhere stmt
  // variant == WHERE_BLOCK_STMT
public:
  SgWhereBlockStmt(SgExpression &cond, SgStatement &trueBody, SgStatement &falseBody);
  ~SgWhereBlockStmt();
};


class  SgSwitchStmt: public SgStatement{
  // Fortran Case and C switch();
  // variant == SWITCH_NODE
public:
  inline SgSwitchStmt(SgExpression &selector, SgStatement &caseOptionList, SgSymbol &constructName);
  // added by A.V.Rakov 16.03.2015
  inline SgSwitchStmt(SgExpression &selector, SgStatement &caseOptionList);
  inline SgSwitchStmt(SgExpression &selector);
  inline ~SgSwitchStmt();
  inline SgExpression *selector();  // the switch selector
  inline void setSelector(SgExpression &cond);
  inline int numberOfCaseOptions();       // the number of cases
  inline SgStatement *caseOption(int i);  // i-th case block
  inline void addCaseOption(SgStatement &caseOption);
  // added by A.V.Rakov 16.03.2015
  inline SgStatement *defOption();
#if 0
  void  deleteCaseOption(int i);
#endif
};

class SgCaseOptionStmt: public SgStatement{
  // Fortran case option statement
  // variant == CASE_NODE
public:
  // added by A.S.Kolganov 18.07.2018
  inline SgCaseOptionStmt(SgExpression &caseRangeList, SgStatement &body);
  inline SgCaseOptionStmt(SgExpression &caseRangeList, SgStatement &body, SgSymbol &constructName);
  // added by A.V.Rakov 16.03.2015
  inline SgCaseOptionStmt(SgExpression &caseRangeList);
  inline ~SgCaseOptionStmt();

  inline SgExpression *caseRangeList();
  inline void setCaseRangeList(SgExpression &caseRangeList);
  inline SgExpression *caseRange(int i);
  inline void setCaseRange(int i, SgExpression &caseRange);
  inline SgStatement *body();
  inline void setBody(SgStatement &body);
};


class  SgExecutableStatement: public  SgStatement{
  // this is really a non-control, non-declaration stmt.
  // no special functions here.
public: 
  inline SgExecutableStatement(int variant);
};

class SgAssignStmt: public SgExecutableStatement{
  // Fortran assignment Statment
  // variant == ASSIGN_STAT
public:
  inline SgAssignStmt(int variant);
  inline SgAssignStmt(SgExpression &lhs, SgExpression &rhs);
  inline SgExpression *lhs();  // the left hand side
  inline SgExpression *rhs();  // the right hand side
  inline void replaceLhs(SgExpression &e); // replace lhs with e
  inline void replaceRhs(SgExpression &e); // replace rhs with e
#if 0
  SgExpression *varReferenced();
  SgExpression *varUsed();
  SgExpression *varDefined();
#endif
};


class  SgCExpStmt: public SgExecutableStatement{
  // C non-control expression Statment
  // variant == EXPR_STMT_NODE
public:
  inline SgCExpStmt(SgExpression &exp);
  inline SgCExpStmt(SgExpression &lhs, SgExpression &rhs);
  inline SgExpression *expr();  // the expression
  inline void replaceExpression(SgExpression &e); // replace exp with e
  inline ~SgCExpStmt();
};

class  SgPointerAssignStmt: public SgAssignStmt{
  // Fortran  pointer assignment statement
  // variant == POINTER_ASSIGN_STAT
public:
  inline SgPointerAssignStmt(SgExpression lhs, SgExpression rhs);
  inline ~SgPointerAssignStmt();
};

// heap and nullify statements can be sub-classes 
// of list executable statement class.
class  SgHeapStmt: public SgExecutableStatement{
  // Fortran heap space allocation and deallocation statements
  // variant == ALLOCATE_STMT or DEALLOCATE_STMT 
public:
  inline SgHeapStmt(int variant, SgExpression &allocationList, SgExpression &statVariable);
  inline ~SgHeapStmt();
  inline SgExpression *allocationList();
  inline void setAllocationList(SgExpression &allocationList);
  inline SgExpression *statVariable();
  inline void setStatVariable(SgExpression &statVar);
};

class  SgNullifyStmt: public SgExecutableStatement{
  // Fortran pointer initialization statement
  // variant == NULLIFY_STMT 
public:
  inline SgNullifyStmt(SgExpression &objectList);
  inline ~SgNullifyStmt();
  inline SgExpression *nullifyList();
  inline void setNullifyList(SgExpression &nullifyList);
};


class  SgContinueStmt: public SgExecutableStatement{
  // variant == CONT_STAT in Fortran and
  // variant == CONTINUE_NODE in C
public:
  inline SgContinueStmt();
  inline ~SgContinueStmt();
};

class  SgControlEndStmt: public SgExecutableStatement{
  // the end of a basic block
  // variant == CONTROL_END 
public:
  inline SgControlEndStmt(int variant);
  inline SgControlEndStmt();
  inline ~SgControlEndStmt();
};


class  SgBreakStmt: public SgExecutableStatement{
  // the end of a basic block
  // variant == BREAK_NODE 
public:
  inline SgBreakStmt();
  inline ~SgBreakStmt();
};

class  SgCycleStmt: public SgExecutableStatement{
  // the fortran 90 cycle statement
  // variant == CYCLE_STMT
public:
  inline SgCycleStmt(SgSymbol &symbol);
// added by A.S. Kolganov 20.12.2015
  inline SgCycleStmt();
  inline SgSymbol *constructName();  // the name of the loop to cycle
  inline void setConstructName(SgSymbol &constructName);
  inline ~SgCycleStmt();
};

class  SgReturnStmt: public SgExecutableStatement{
  // the return (expr) node
  // variant == RETURN_NODE//RETURN_STAT
public:
  SgReturnStmt(SgExpression &returnValue);
  SgReturnStmt();
  inline SgExpression *returnValue();
  inline void setReturnValue(SgExpression &retVal);
  inline ~SgReturnStmt();
};


class  SgExitStmt: public SgControlEndStmt{
  // the fortran 90 exit statement
  // variant == EXIT_STMT
public:
  inline SgExitStmt(SgSymbol &construct_name);
  inline ~SgExitStmt();
  inline SgSymbol *constructName();  // the name of the loop to cycle
  inline void setConstructName(SgSymbol &constructName);
};

class  SgGotoStmt: public SgExecutableStatement{
  // the fortran or C goto
  // variant == GOTO_NODE
public:
  inline SgGotoStmt(SgLabel &label);
  inline SgLabel *branchLabel();
#if 0
  SgStatement *target(); //the statement we go to
#endif
  inline ~SgGotoStmt();
};


class SgLabelListStmt: public SgExecutableStatement{
  // the fortran
  // statements containg a list of labels
public:
  SgLabelListStmt(int variant);
  int numberOfTargets();
  SgExpression *labelList();
  void setLabelList(SgExpression &labelList);
#if 0
  SgStatement *target(int i); //the statement we go to
#endif
};


class  SgAssignedGotoStmt: public SgLabelListStmt{
  // the fortran 
  // variant == ASSGOTO_NODE
public:
  SgAssignedGotoStmt(SgSymbol &symbol, SgExpression &labelList);
  SgSymbol *symbol();
  void setSymbol(SgSymbol &symb);
  ~SgAssignedGotoStmt();
};


class  SgComputedGotoStmt: public SgLabelListStmt{
  // the fortran goto
  // variant == COMGOTO_NODE
public:
  inline SgComputedGotoStmt(SgExpression &expr, SgLabel &label);
  inline void addLabel(SgLabel &label);
  inline SgExpression *exp();
  inline void setExp(SgExpression &exp);
  inline ~SgComputedGotoStmt();
};

class  SgStopOrPauseStmt: public SgExecutableStatement{
  // the fortran stop
  // variant == STOP_STAT
public:
  SgStopOrPauseStmt(int variant, SgExpression *expr);
  SgExpression *exp();
  void setExp(SgExpression &exp);
  ~SgStopOrPauseStmt();
};

class  SgCallStmt: public SgExecutableStatement{
  // the fortran call
  // variant == PROC_STAT
public:
  SgCallStmt(SgSymbol &name, SgExpression &args);
  SgCallStmt(SgSymbol &name);
  SgSymbol *name();    // name of subroutine being called
  int numberOfArgs();  // the number of arguement expressions
  void  addArg(SgExpression &arg);
  SgExpression *arg(int i); // the i-th argument expression
  ~SgCallStmt();
  
#if 0
  // global analysis functions
  int numberOfVarsUsed();
  SgExpression *varsUsed(int i);  // i-th region description
  int numberOfVarsMod();
  SgExpression *varsMod(int i);  // i-th region description
#endif
};  


class  SgProsCallStmt: public SgExecutableStatement{
  // the Fortran M process call
  // variant == PROS_STAT
public:
  SgProsCallStmt(SgSymbol &name, SgExprListExp &args);
  SgProsCallStmt(SgSymbol &name);
  SgSymbol *name();    // name of process being called
  int numberOfArgs();  // the number of arguement expressions
  void  addArg(SgExpression &arg);
  SgExprListExp *args();
  SgExpression *arg(int i); // the i-th argument expression
  ~SgProsCallStmt();
};
 
 
class  SgProsCallLctn: public SgExecutableStatement{
  // the Fortran M process call with location
  // variant == PROS_STAT_LCTN
public:
  SgProsCallLctn(SgSymbol &name, SgExprListExp &args, SgExprListExp &lctn);
  SgProsCallLctn(SgSymbol &name, SgExprListExp &lctn);
  SgSymbol *name();    // name of process being called
  int numberOfArgs();  // the number of arguement expressions
  void  addArg(SgExpression &arg);
  SgExprListExp *args();
  SgExpression *arg(int i); // the i-th argument expression
  SgExpression *location();
  ~SgProsCallLctn();
};
 

class  SgProsCallSubm: public SgExecutableStatement{
  // the Fortran M process call with submachine
  // variant == PROS_STAT_SUBM
public:
  SgProsCallSubm(SgSymbol &name, SgExprListExp &args, SgExprListExp &subm);
  SgProsCallSubm(SgSymbol &name, SgExprListExp &subm);
  SgSymbol *name();    // name of process being called
  int numberOfArgs();  // the number of arguement expressions
  void  addArg(SgExpression &arg);
  SgExprListExp *args();
  SgExpression *arg(int i); // the i-th argument expression
  SgExpression *submachine();
  ~SgProsCallSubm();
};
 

class  SgProcessesStmt: public SgStatement{
  // the Fortran M processes statement
  // variant == PROCESSES_STAT
public:
  inline SgProcessesStmt();
  inline ~SgProcessesStmt();
};
 

class  SgEndProcessesStmt: public SgStatement{
  // the Fortran M endprocesses statement 
  // variant == PROCESSES_END
public:
  inline SgEndProcessesStmt();
  inline ~SgEndProcessesStmt();
};


class SgPortTypeExp: public SgExpression{
  // variant == PORT_TYPE_OP, INPORT_TYPE_OP, or OUTPORT_TYPE_OP
public:
  inline SgPortTypeExp(SgType &type);
  inline SgPortTypeExp(SgType &type, SgExpression &ref);
  inline SgPortTypeExp(int variant, SgExpression &porttype);
  inline ~SgPortTypeExp();
  inline SgType *type();
  inline int numberOfRef();
  inline SgExpression *ref(); // return a ref or a port type
  inline SgPortTypeExp *next();
};
 
 
class SgInportStmt: public SgStatement
{
  // the Fortran M inport statement
  // variant == INPORT_DECL
public:
  inline SgInportStmt(SgExprListExp &name);
  inline SgInportStmt(SgExprListExp &name, SgPortTypeExp &porttype);
  inline ~SgInportStmt();
  inline void addname(SgExpression &name);
  inline int numberOfNames();
  inline SgExprListExp *names();
  inline SgExpression *name(int i);
  inline void addporttype(SgExpression &porttype);
  inline int numberOfPortTypes();
  inline SgPortTypeExp *porttypes();
  inline SgPortTypeExp *porttype(int i);
};

 
class SgOutportStmt: public SgStatement{
  // the Fortran M outport statement
  // variant == OUTPORT_DECL
public:
  inline SgOutportStmt(SgExprListExp &name);
  inline SgOutportStmt(SgExprListExp &name, SgPortTypeExp &porttype);
  inline ~SgOutportStmt();
  inline void addname(SgExpression &name);
  inline int numberOfNames();
  inline SgExprListExp *names();
  inline SgExpression *name(int i);
  inline void addporttype(SgExpression &porttype);
  inline int numberOfPortTypes();
  inline SgPortTypeExp *porttypes();
  inline SgPortTypeExp *porttype(int i);
};

 
class SgChannelStmt: public SgStatement{
  // the Fortran M channel statement
  // variant == CHANNEL_STAT
public:
  inline SgChannelStmt(SgExpression &outport, SgExpression &inport);
  inline SgChannelStmt(SgExpression &outport, SgExpression &inport,
                       SgExpression &io_or_err);
  inline SgChannelStmt(SgExpression &outport, SgExpression &inport,
                       SgExpression &iostore, SgExpression &errlabel);
  inline ~SgChannelStmt();
  inline SgExpression *outport();
  inline SgExpression *inport();
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
};
 

class SgMergerStmt: public SgStatement{
  // the Fortran M merger statement
  // variant == MERGER_STAT
public:
  inline SgMergerStmt(SgExpression &outport, SgExpression &inport);
  inline SgMergerStmt(SgExpression &outport, SgExpression &inport,
                     SgExpression &io_or_err);
  inline SgMergerStmt(SgExpression &outport, SgExpression &inport,
                     SgExpression &iostore, SgExpression &errlabel);
  inline ~SgMergerStmt();
  inline void addOutport(SgExpression &outport);
  inline void addIoStore(SgExpression &iostore); //can't add it before outports
  inline void addErrLabel(SgExpression &errlabel); //can't add it before iostore
  inline int numberOfOutports();
  inline SgExpression *outport(int i);
  inline SgExpression *inport();
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
};


class SgMoveportStmt: public SgStatement{
  // the Fortran M moveport statement
  // variant == MOVE_PORT
public:
  inline SgMoveportStmt(SgExpression &fromport, SgExpression &toport);
  inline SgMoveportStmt(SgExpression &fromport, SgExpression &toport,
                        SgExpression &io_or_err);
  inline SgMoveportStmt(SgExpression &fromport, SgExpression &toport,
                        SgExpression &iostore, SgExpression &errlabel);
  inline ~SgMoveportStmt();
  inline SgExpression *fromport();
  inline SgExpression *toport();
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
};
 

class SgSendStmt: public SgStatement{
  // the Fortran M send statement
  // variant == SEND_STAT
public:
  inline SgSendStmt(SgExpression &control, SgExprListExp &argument);
  inline SgSendStmt(SgExpression &outport, SgExprListExp &argument, SgExpression &io_or_err);
  inline SgSendStmt(SgExpression &outport, SgExprListExp &argument, SgExpression &iostore, SgExpression &errlabel);
  inline ~SgSendStmt();
  inline void addOutport(SgExpression &outport);
  inline void addIoStore(SgExpression &iostore); //can't add it before outports
  inline void addErrLabel(SgExpression &errlabel); //can't add it before iostore
  inline void addArgument(SgExpression &argument);
  inline int numberOfOutports();
  inline int numberOfArguments();
  inline SgExpression *controls();
  inline SgExpression *outport(int i);
  inline SgExprListExp *arguments();
  inline SgExpression *argument(int i);
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
};


class SgReceiveStmt: public SgStatement{
  // the Fortran M receive statement
  // variant == RECEIVE_STAT
public:
  inline SgReceiveStmt(SgExpression &control, SgExprListExp &argument);
  inline SgReceiveStmt(SgExpression &inport, SgExprListExp &argument,
                       SgExpression &e1);
  inline SgReceiveStmt(SgExpression &inport, SgExprListExp &argument,
                       SgExpression &e1, SgExpression &e2);
  inline SgReceiveStmt(SgExpression &inport, SgExprListExp &argument,
                       SgExpression &e1, SgExpression &e2, SgExpression &e3);
  inline ~SgReceiveStmt();
  inline void addInport(SgExpression &inport);
  inline void addIoStore(SgExpression &iostore);//can't add it before inports
  inline void addErrLabel(SgExpression &errlabel);//can't add it before iostore
  inline void addEndLabel(SgExpression &endlabel);//can't add it before errlabel
  inline void addArgument(SgExpression &argument);
  inline int numberOfInports();
  inline int numberOfArguments();
  inline SgExpression *controls();
  inline SgExpression *inport(int i);
  inline SgExprListExp *arguments();
  inline SgExpression *argument(int i);
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
  inline SgExpression *endLabel();
};



class SgEndchannelStmt: public SgStatement{
  // the Fortran M endchannel statement
  // variant == ENDCHANNEL_STAT
public:
  inline SgEndchannelStmt(SgExpression &outport);
  inline SgEndchannelStmt(SgExpression &outport, SgExpression &io_or_err);
  inline SgEndchannelStmt(SgExpression &outport, SgExpression &iostore,
                          SgExpression &errlabel);
  inline ~SgEndchannelStmt();
  inline void addOutport(SgExpression &outport);
  inline void addIoStore(SgExpression &iostore);//can't add it before outports
  inline void addErrLabel(SgExpression &errlabel);//can't add it before iostore
  inline int numberOfOutports();
  inline SgExpression *controls();
  inline SgExpression *outport(int i);
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
};


class SgProbeStmt: public SgStatement{
  // the Fortran M probe statement
  // variant == PROBE_STAT
public:
  inline SgProbeStmt(SgExpression &inport);
  inline SgProbeStmt(SgExpression &inport, SgExpression &e1);
  inline SgProbeStmt(SgExpression &inport, SgExpression &e1,
                     SgExpression &e2);
  inline SgProbeStmt(SgExpression &inport, SgExpression &e1,
                          SgExpression &e2, SgExpression &e3);
  inline ~SgProbeStmt();
  inline void addInport(SgExpression &inport);
  inline void addIoStore(SgExpression &iostore);//can't add before inports
  inline void addErrLabel(SgExpression &errlabel);//can't add before iostore
  inline void addEmptyStore(SgExpression &endlabel);//can't add before errlabel
  inline int numberOfInports();
  inline SgExpression *controls();
  inline SgExpression *inport(int i);
  inline SgExpression *ioStore();
  inline SgExpression *errLabel();
  inline SgExpression *emptyStore();
};


class SgProcessorsRefExp: public SgExpression{
  // variant == PROCESSORS_REF
public:
  inline SgProcessorsRefExp(PTR_LLND ll);
  inline SgProcessorsRefExp();
  inline SgProcessorsRefExp(SgExpression &subscripts);
  inline SgProcessorsRefExp(SgExpression &sub1,SgExpression &sub2);
 
  inline SgProcessorsRefExp(SgExpression &sub1,SgExpression &sub2,
                            SgExpression &sub3);
 
  inline SgProcessorsRefExp(SgExpression &sub1,SgExpression &sub2,
                            SgExpression &sub3,SgExpression &sub4);
  inline ~SgProcessorsRefExp();
  inline int numberOfSubscripts();  // the number of subscripts in reference
  inline SgExpression *subscripts();
  inline SgExpression *subscript(int i);
  inline void addSubscript(SgExpression &e);
};


class SgControlExp: public SgExpression{
  //parent of INPORT_NAME, OUTPORT_NAME, FROMPORT_NAME, TOPORT_NAME
  //          IOSTAT_STORE, EMPTY_STORE, ERR_LABEL, END_LABEL
public:
  inline SgControlExp(int variant);
  inline ~SgControlExp();
  inline SgExpression *exp();
};


class SgInportExp: public SgControlExp{
  // variant == INPORT_NAME
public:
  inline SgInportExp(SgExprListExp &exp);
  inline ~SgInportExp();
};


class SgOutportExp: public SgControlExp{
  // variant == OUTPORT_NAME
public:
  inline SgOutportExp(SgExprListExp &exp);
  inline ~SgOutportExp();
};


class SgFromportExp: public SgControlExp{
  // variant == FROMPORT_NAME
public:
  inline SgFromportExp(SgExprListExp &exp);
  inline ~SgFromportExp();
};


class SgToportExp: public SgControlExp{
  // variant == TOPORT_NAME
public:
  inline SgToportExp(SgExprListExp &exp);
  inline ~SgToportExp();
};


class SgIO_statStoreExp: public SgControlExp{
  // variant == IOSTAT_STORE
public:
  inline SgIO_statStoreExp(SgExprListExp &exp);
  inline ~SgIO_statStoreExp();
};


class SgEmptyStoreExp: public SgControlExp{
  // variant == EMPTY_STORE
public:
  inline SgEmptyStoreExp(SgExprListExp &exp);
  inline ~SgEmptyStoreExp();
};


class SgErrLabelExp: public SgControlExp{
  // variant == ERR_LABEL
public:
  inline SgErrLabelExp(SgExprListExp &exp);
  inline ~SgErrLabelExp();
};


class SgEndLabelExp: public SgControlExp{
  // variant == END_LABEL
public:
  inline SgEndLabelExp(SgExprListExp &exp);
  inline ~SgEndLabelExp();
};


class SgDataImpliedDoExp: public SgExpression{
  // variant == DATA_IMPL_DO
public:
  inline SgDataImpliedDoExp(SgExprListExp &dlist, SgSymbol &iname,
                            SgExprListExp &ilist);
  inline ~SgDataImpliedDoExp();
  inline void addDataelt(SgExpression &data);
  inline void addIconexpr(SgExpression &icon);
  inline SgSymbol *iname();
  inline int numberOfDataelt();
  inline SgExprListExp *dataelts();
  inline SgExprListExp *iconexprs(); /* only the first 3 elements in the
                                        iconexpr list are useful. They represent
                                        the initial value, the limit, and the 
                                        increment of the implied do expression
                                        respectively */
  inline SgExpression *dataelt(int i);
  inline SgExpression *init();
  inline SgExpression *limit();
  inline SgExpression *increment();
};
 

class SgDataEltExp: public SgExpression{
  // variant == DATA_ELT
public:
  inline SgDataEltExp(SgExpression &dataimplieddo);
  inline SgDataEltExp(SgSymbol &name, SgExpression &datasubs,
                      SgExpression &datarange);
  inline ~SgDataEltExp();
  inline SgExpression *dataimplieddo();
  inline SgSymbol *name();
  inline SgExpression *datasubs();
  inline SgExpression *datarange();
};


class SgDataSubsExp: public SgExpression{
  // variant == DATA_SUBS
public:
  inline SgDataSubsExp(SgExprListExp &iconexprlist);
  inline ~SgDataSubsExp();
  inline SgExprListExp *iconexprlist();
};


class SgDataRangeExp: public SgExpression{
  // variant == DATA_RANGE
public:
  inline SgDataRangeExp(SgExpression &iconexpr1, SgExpression &iconexpr2);
  inline ~SgDataRangeExp();
  inline SgExpression *iconexpr1();
  inline SgExpression *iconexpr2();
};


class SgIconExprExp: public SgExpression{
  // variant == ICON_EXPR
public:
  inline SgIconExprExp(SgExpression &expr);
  inline ~SgIconExprExp();
  inline SgExpression *expr();
};


class SgIOStmt: public SgExecutableStatement{
  // fortran input/output and their control statements
  // abstract class
public:
  inline SgIOStmt(int variant);
};

class SgInputOutputStmt: public SgIOStmt{
  // fortran input and output statements
  // variant = READ_STAT, WRITE_STATE, PRINT_STAT
public:
  inline SgInputOutputStmt(int variant, SgExpression &specList, SgExpression &itemList);
  inline SgExpression *specList();
  inline void setSpecList(SgExpression &specList);
  inline SgExpression *itemList();
  inline void setItemList(SgExpression &itemList);
  inline ~SgInputOutputStmt();
};

class SgIOControlStmt: public SgExecutableStatement{
  // fortran input/output control and editing statements
  // variant = OPEN_STAT, CLOSE_STAT, INQUIRE_STAT, BACKSPACE_STAT,
  // REWIND_STAT, ENDFILE_STAT, FORMAT_STAT
public:
  SgIOControlStmt(int variant, SgExpression &controlSpecifierList);
  inline SgExpression *controlSpecList();
  inline void setControlSpecList(SgExpression &controlSpecList);
  inline ~SgIOControlStmt();
};

// ******************** Declaration Nodes ***************************

class  SgDeclarationStatement: public SgStatement{
  // Declaration class
  // abstract class
public:
  inline SgDeclarationStatement(int variant);
  inline ~SgDeclarationStatement();
  
  inline SgExpression *varList();
  inline int numberOfVars();
  inline SgExpression *var(int i);
  inline void deleteVar(int i);
  inline void deleteTheVar(SgExpression &var);
  inline void addVar(SgExpression &exp);
};

class  SgVarDeclStmt: public SgDeclarationStatement{
  // Declaration Statement
  // variant == VAR_DECL
public:
  // varRefValList is a list of low-level nodes of
  // variants VAR_REFs or ARRAY_REFs or ASSIGN_OPs
  inline SgVarDeclStmt(SgExpression &varRefValList, SgExpression &attributeList, SgType &type);
  inline SgVarDeclStmt(SgExpression &varRefValList, SgType &type);
  inline SgVarDeclStmt(SgExpression &varRefValList);
  inline ~SgVarDeclStmt();
  inline SgType *type();  // the type;
  inline int numberOfAttributes(); // the number of F90 attributes;
  // the attributes are: PARAMETER_OP | PUBLIC_OP |
  //    PRIVATE_OP | ALLOCATABLE_OP | EXTERNAL_OP |
  //    OPTIONAL_OP | POINTER_OP | SAVE_OP TARGET_OP

  inline SgExpression* attribute(int i)
  {
      SgExpression* ex = LlndMapping(BIF_LL3(thebif));
      if (ex->variant() != EXPR_LIST)
          return NULL;

      SgExprListExp* list = (SgExprListExp*)ex;
      return list->elem(i);
  }

  inline bool addAttributeExpression(SgExpression* attr)
  {
      SgExpression* ex = LlndMapping(BIF_LL3(thebif));
      if (ex && ex->variant() != EXPR_LIST)
          return false;

      if (ex != NULL)
      {
          SgExprListExp* list = (SgExprListExp*)ex;
          list->append(*attr);
      }
      else
      {
          ex = new SgExpression(EXPR_LIST, attr, NULL);
          BIF_LL3(thebif) = ex->thellnd;
      }
      return true;
  }

  inline int numberOfSymbols();  // the number of variables declared;        
  inline SgSymbol *symbol(int i);
  
  inline void deleteSymbol(int i);
  inline void deleteTheSymbol(SgSymbol &symbol);
  inline SgExpression *initialValue(int i);  // the initial value ofthe i-th variable
  SgExpression *completeInitialValue(int i); // The complete ASSGN_OP
					// expression of the initial value *BW* from M. Golden
  void setInitialValue(int i, SgExpression &initVal); // sets the initial value ofthe i-th variable
  // an alternative way to initialize variables. The low-level node (VAR_REF or ARRAY_REF) is
  // replaced by a ASSIGN_OP low-level node.
  void clearInitialValue(int i);  // removes initial value of the i-th declaration 
};


class  SgIntentStmt: public SgDeclarationStatement{
  // the Fortran M Intent Statement
  // variant == INTENT_STMT
public:
  inline SgIntentStmt(SgExpression &varRefValList, SgExpression &attribute);
  inline ~SgIntentStmt();
  inline int numberOfArgs();  // the number of arguement expressions
  inline void  addArg(SgExpression &arg);
  inline SgExpression *args();
  inline SgExpression *arg(int i); // the i-th argument expression
  inline SgExpression *attribute();
};


class  SgVarListDeclStmt: public SgDeclarationStatement{
  // Declaration Statement
  // variant == OPTIONAL_STMT, SAVE_STMT, PUBLIC_STMT,
  // PRIVATE_STMT, EXTERNAL_STAT, INTRINSIC_STAT, DIM_STAT, 
  // ALLOCATABLE_STAT, POINTER_STAT, TARGET_STAT, MODULE_PROC_STMT,
  // PROCESSORS_STAT (for Fortran M processors statement)
public:
  SgVarListDeclStmt(int variant, SgExpression &symbolRefList);
  SgVarListDeclStmt(int variant, SgSymbol &symbolList, SgStatement &scope);

  inline ~SgVarListDeclStmt();
  
  inline int numberOfSymbols();
  inline SgSymbol *symbol(int i);
  inline void appendSymbol(SgSymbol &symbol);
  inline void deleteSymbol(int i);
  inline void deleteTheSymbol(SgSymbol &symbol);
};


class SgStructureDeclStmt: public SgDeclarationStatement{
  // Fortran 90 structure declaration statement
  // variant == STRUCT_DECL
public:
  SgStructureDeclStmt(SgSymbol &name, SgExpression &attributes, SgStatement &body);
  ~SgStructureDeclStmt();
  
#if 0
  int isPrivate();
  int isPublic();
  int isSequence();
#endif
};

class SgNestedVarListDeclStmt: public SgDeclarationStatement{
  // Declaration statement
  // variant == NAMELIST_STAT, EQUI_STAT, COMM_STAT,
  //            and  PROS_COMM for Fortran M
  // These statements have the format of a list of variable lists. For example,
  // EQUIVALENCE (A, C, D), (B, G, F), ....
public:
  SgNestedVarListDeclStmt(int variant, SgExpression &listOfVarList);
  // varList must be of low-level variant appropriate to variant. For example,
  // if the variant is COMM_STAT, listOfVarList must be of variant COMM_LIST.
  ~SgNestedVarListDeclStmt();
  
  SgExpression *lists();
  int numberOfLists();
  SgExpression *list(int i);
#if 0
  SgExpression *leadingVar(int i);
#endif
  void addList(SgExpression &list);
  void  addVarToList(SgExpression &varRef);
  void deleteList(int i);
  void deleteTheList(SgExpression &list);
  void deleteVarInList(int i, SgExpression &varRef);
  void deleteVarInTheList(SgExpression &list, SgExpression &varRef);
};

class SgParameterStmt: public SgDeclarationStatement{      
  // Fortran constants declaration statement
  // variant = PARAM_DECL
public:
  SgParameterStmt(SgExpression &constants, SgExpression &values);
  SgParameterStmt(SgExpression &constantsWithValues);
  ~SgParameterStmt();
  
  int numberOfConstants();    // the number of constants declared
  
  SgSymbol *constant(int i);  // the i-th variable
  SgExpression *value(int i); // the value of i-th variable
  
  void addConstant(SgSymbol &constant, SgExpression &value);
  void deleteConstant(int i);
  void deleteTheConstant(SgSymbol &constant);
};

class SgImplicitStmt: public SgDeclarationStatement{      
  // Fortran implicit type declaration statement
  // variant = IMPL_DECL
public:
  SgImplicitStmt(SgExpression& implicitLists);
  SgImplicitStmt(SgExpression* implicitLists);
  ~SgImplicitStmt();
  
  int numberOfImplicitTypes();  // the number of implicit types declared;
  SgType *implicitType(int i); // the i-th implicit type
  SgExpression *implicitRangeList(int i) ;
  void  appendImplicitNode(SgExpression &impNode);
#if 0
  void  addImplicitType(SgType Type, char alphabet[]);
  int deleteImplicitItem(int i);
  int deleteTheImplicitItem(SgExpression &implicitItem);
#endif
};
#if 0
class SgUseStmt: public SgDeclarationStatement{
  // Fortran 90 module usuage statement
  // variant = USE_STMT
public:
  SgUseStmt(SgSymbol &moduleName, SgExpression &renameList, SgStatement &scope);
  // renameList must be a list of low-level nodes of variant RENAME_NODE
  ~SgUseStmt();
  
  int isOnly();
  SgSymbol *moduleName();
  void setModuleName(SgSymbol &moduleName);
  int numberOfRenames();
  SgExpression *renameNode(int i);
  void  addRename(SgSymbol &localName, SgSymbol &useName);
  void  addRenameNode(SgExpression &renameNode);
  void  deleteRenameNode(int i);
  void deleteTheRenameNode(SgExpression &renameNode);
};




class  SgStmtFunctionStmt: public SgDeclarationStatement{
  // Fortran statement function declaration
  // variant == STMTFN_DECL
public:        
  SgStmtFunctionStmt(SgSymbol &name, SgExpression &args, SgStatement Body);
  ~SgStmtFunctionStmt();
  SgSymbol *name();
  void setName(SgSymbol &name);
  SgType *type();
  int numberOfParameters();       // the number of parameters
  SgSymbol *parameter(int i);     // the i-th parameter
};      

class  SgMiscellStmt: public SgDeclarationStatement{
  // Fortran 90 simple miscellaneous statements
  // variant == CONTAINS_STMT, PRIVATE_STMT, SEQUENCE_STMT
public:        
  SgMiscellStmt(int variant);
  ~SgMiscellStmt();
};      


#endif
//
//
// More stuffs for types and symbols
//
//


class SgVariableSymb: public SgSymbol{
  // a variable
  // variant = VARIABLE_NAME
public:
  inline SgVariableSymb(char *identifier, SgType &t, SgStatement &scope);
  inline SgVariableSymb(char *identifier, SgType *t, SgStatement *scope);
  inline SgVariableSymb(char *identifier, SgType &t);
  inline SgVariableSymb(char *identifier,  SgStatement &scope);
  inline SgVariableSymb(char *identifier,  SgStatement *scope);
  inline SgVariableSymb(char *identifier);
  inline SgVariableSymb(const char *identifier, SgType &t, SgStatement &scope);
  inline SgVariableSymb(const char *identifier, SgType *t, SgStatement *scope);
  inline SgVariableSymb(const char *identifier, SgType &t);
  inline SgVariableSymb(const char *identifier, SgStatement &scope);
  inline SgVariableSymb(const char *identifier, SgStatement *scope);
  inline SgVariableSymb(const char *identifier);
  inline ~SgVariableSymb();

  /* This function allocates and returns a new variable reference
     expression to this symbol. (ajm) */
  inline SgVarRefExp *varRef (void);

#if 0
  int isAttributeSet(int attribute);
  void setAttribute(int attribute);
  
  int numberOfUses();            // number of uses.
  SgStatement  *useStmt(int i);  // statement where i-th use occurs
  SgExpression *useExpr(int i);  // expression where i-th use occurs
  int numberOfDefs();
#endif
};

class SgConstantSymb: public SgSymbol{
  // a symbol for a constant object
  // variant == CONST_NAME 
public:
  inline SgConstantSymb(char *identifier, SgStatement &scope, 
                 SgExpression &value);
  inline SgConstantSymb(const char *identifier, SgStatement &scope,
      SgExpression &value);
  inline ~SgConstantSymb();
  inline SgExpression *constantValue();
};


class SgFunctionSymb: public SgSymbol{
  // a subroutine, function or main program
  // variant == PROGRAM_NAME, PROCEDURE_NAME, or FUNCTION_NAME
public:
  SgFunctionSymb(int variant);
  SgFunctionSymb(int variant, char *identifier, SgType &t, 
                 SgStatement &scope);
  SgFunctionSymb(int variant, const char *identifier, SgType &t,
      SgStatement &scope);
  ~SgFunctionSymb();
  void addParameter(int, SgSymbol &parameters);
  void insertParameter(int position, SgSymbol &symb);
  int numberOfParameters();
  SgSymbol *parameter(int i); 
  SgSymbol *result();
  void  setResult(SgSymbol &symbol);
#if 0
  int isRecursive();
  int setRecursive();
#endif
};


class SgMemberFuncSymb: public SgFunctionSymb{
  // a member function for a class or struct or collection
  // variant = MEMBER_FUNC
  // may be either MEMB_PRIVATE, MEMB_PUBLIC,
  // MEMP_METHOELEM or MEMB_PROTECTED
public:
  inline SgMemberFuncSymb(char *identifier, SgType &t, SgStatement &cla,
                          int status);
  inline ~SgMemberFuncSymb();
#if 0
  int status();       
  int isVirtual();       // 1 if virtual.
#endif
  inline int isMethodOfElement();
  inline SgSymbol *className();
  inline void setClassName(SgSymbol &symb);
};

class SgFieldSymb: public SgSymbol{
  // a field in an enum or in a struct.
  // variant == ENUM_NAME or FIELD_NAME
public:
  // no check is made to see if the field "identifier"
  //   already exists in the structure. 
  inline SgFieldSymb(char *identifier, SgType &t, SgSymbol &structureName);
  inline SgFieldSymb(const char *identifier, SgType &t, SgSymbol &structureName);
  inline ~SgFieldSymb();
  inline int offset();        // position in the structure
  inline SgSymbol *structureName();  // parent structure
  inline SgSymbol *nextField();
  inline int isMethodOfElement();
#if 0
  int isPrivate();
  int isSequence();
  void  setPrivate();
  void setSequence();
#endif
};

class SgClassSymb: public SgSymbol{
  // the class, union, struct and collection type.
  // variant == CLASS_NAME, UNION_NAME, STRUCT_NAME or COLLECTION_NAME
public:
  inline SgClassSymb(int variant, char *name, SgStatement &scope);
  inline ~SgClassSymb();
  inline int numberOfFields();
  inline SgSymbol *field(int i);
};

#if 0
class SgTypeSymb: public SgSymbol{
  // a C typedef.  the type() function returns the base type.
  // variant == TYPE_NAME
public:
  SgTypeSymb(char *name, SgType &baseType);
  SgType &baseType();
  ~SgTypeSymb();
};

#endif


class SgLabelSymb: public SgSymbol{
  // a C label name
  // variant == LABEL_NAME
public:
  inline SgLabelSymb(char *name);
  inline ~SgLabelSymb();
};


class SgLabelVarSymb: public SgSymbol{
  // a Fortran label variable for an assigned goto stmt
  // variant == LABEL_NAME
public:
  inline SgLabelVarSymb(char *name, SgStatement &scope);
  inline ~SgLabelVarSymb();
};

class SgExternalSymb: public SgSymbol{
  // for fortran external statement
  // variant == ROUTINE_NAME
public:
  inline SgExternalSymb(char *name, SgStatement &scope);
  inline ~SgExternalSymb();
};

class SgConstructSymb: public SgSymbol{
  // for fortran statement with construct names
  // variant == CONSTRUCT_NAME
public:
  inline SgConstructSymb(char *name, SgStatement &scope);
  inline ~SgConstructSymb();
};

// A lot of work needs to be done on this class.
class SgInterfaceSymb: public SgSymbol{
  // for fortran interface statement
  // variant == INTERFACE_NAME
public:
  inline SgInterfaceSymb(char *name, SgStatement &scope);
  inline ~SgInterfaceSymb();
};

// A lot of work needs to be done on this class.
class SgModuleSymb: public SgSymbol{
  // for fortran module statement
  // variant == MODULE_NAME
public:
  inline SgModuleSymb(char *name);
  inline ~SgModuleSymb();
};

// ********************* Types *******************************

class SgArrayType: public SgType{
  // A new array type is generated for each array.
  // variant == T_ARRAY
public:
  inline SgArrayType(SgType &base_type);
  inline int dimension();
  inline SgExpression *sizeInDim(int i);
  inline void addDimension(SgExpression *e);
  inline SgExpression * getDimList();
  inline SgType * baseType();
  inline void setBaseType(SgType &bt);
  inline void addRange(SgExpression &e);
  inline ~SgArrayType();
};


#if 0
class SgClassType: public SgType{
  // a C struct or Fortran Record, a C++ class, a C Union and a C Enum
  // and a pC++ collection.  note: derived classes are another type.
  // this type is very simple.  it only contains the standard type
  // info from SgType and a pointer to the class declaration stmt
  // and a pointer to the symbol that is the first field in the struct.
  // variant == T_STRUCT, T_ENUM, T_CLASS, T_ENUM, T_COLLECTION
public:
  // why is struct_decl needed. No appropriate field found.
  // assumes that first_field has been declared as
  // FIELD_NAME and the remaining fields have been stringed to it.
  SgClassType(int variant, char *name, SgStatement &struct_decl, int num_fields,
              SgSymbol &first_field);
  SgStatement &structureDecl();
  SgSymbol *firstFieldSymb();
  SgSymbol *fieldSymb(int i);
  ~SgClassType();
};

#endif


class SgPointerType: public SgType{
  // A pointer type contains only one hany bit of information:
  // the base type.
  // can also have a modifier like BIT_CONST BIT_GLOBAL. see SgDescriptType.
  // variant == T_POINTER
public:
  SgPointerType(SgType &base_type);
  SgPointerType(SgType *base_type);
  inline SgType *baseType();
  inline int indirection();
  inline void setIndirection(int);
  inline int modifierFlag();
  inline void setModifierFlag(int flag); 
  inline void setBaseType(SgType &baseType);
  inline ~SgPointerType();
};


class SgFunctionType: public SgType{
  // Function Types have a returned value type
  // variant == T_FUNCTION
public:
  SgFunctionType(SgType &return_val_type);
  SgType *returnedValue();
  void changeReturnedValue(SgType &rv);
  ~SgFunctionType();
};


class SgReferenceType: public SgType{
  // A reference (&xx in c+=) type contains only one hany bit of information:
  // the base type.
  // variant == T_REFERENCE
public:
  inline SgReferenceType(SgType &base_type);
  inline SgType *baseType();
  inline void setBaseType(SgType &baseType);
  inline ~SgReferenceType();
  inline int modifierFlag();
  inline void setModifierFlag(int flag); 
};

class SgDerivedType: public SgType{
  // for example:  typedef int integer;  go to the symbol table
  // for the base type and Id.
  // variant == T_DERIVED_TYPE
public:
  inline SgDerivedType(SgSymbol &type_name);
  inline SgSymbol * typeName();
  inline ~SgDerivedType();
};

class SgDerivedClassType: public SgType{
  // for example:  typedef int integer;  go to the symbol table
  // for the base type and Id.
  // variant == T_DERIVED_CLASS
public:
  inline SgDerivedClassType(SgSymbol &type_name);
  inline SgSymbol *typeName();
  inline ~SgDerivedClassType();
};

class SgDerivedTemplateType: public SgType{
   // this is the type for a template object: T_DERIVED_TEMPLATE
public:
   SgDerivedTemplateType(SgExpression *arg_vals, SgSymbol *classname);
   SgExpression *argList();
   void addArg(SgExpression *arg);
   int numberOfArgs();
   SgExpression  *arg(int i);
   void setName(SgSymbol &s);
   SgSymbol *typeName(); // the name of the template class.
};

class SgDescriptType: public SgType{
  // for example in C: long volatile int x; 
  // long and volatile are modifiers and there is a descriptor
  // type whose base type is the real type of x.
  // the modifier is an integer with bits set if the modifier
  // holds.
  // the bits are:
  // BIT_SYN, BIT_SHARED, BIT_PRIVATE, BIT_FUTURE, BIT_VIRTUAL, 
  // BIT_INLINE, BIT_UNSIGNED, BIT_SIGNED, BIT_LONG, BIT_SHORT,
  // BIT_VOLATILE, BIT_CONST, BIT_TYPEDEF, BIT_EXTERN, BIT_AUTO,
  // BIT_STATIC, BIT_REGISTER, BIT_FRIEND, BIT_GLOBAL, and more.
  //
  // variant = T_DESCRIPT
public:
  inline SgDescriptType(SgType &base_type, int bit_flag);
  inline int modifierFlag();
  inline void setModifierFlag(int flag);
  inline ~SgDescriptType();
};

class SgDerivedCollectionType: public SgType{
  // for example:
  // Collection DistributedArray {body1} ;
  // class object {body2} ;
  // DistributedArray<object>  X;
  // X is of type with variant = T_DERIVED_COLLECTION
public:
  inline SgDerivedCollectionType(SgSymbol &s, SgType &t);
  inline SgType *elementClass();
  inline void setElementClass(SgType &ty);
  inline SgSymbol *collectionName();
  inline SgStatement *createCollectionWithElemType();
  inline ~SgDerivedCollectionType();
};

// Class definition ends; Inline definitions begin

// SgProject--inlines

inline SgProject::~SgProject()
{
#if __SPF
    removeFromCollection(this);
#endif
}
inline SgProject::SgProject(SgProject &)
{ 
 Message("SgProject copy constructor not allowed",0);
#if __SPF
     {
         char buf[512];
         sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
         addToGlobalBufferAndPrint(buf);
     }
    throw -1;
#endif
}

inline int SgProject::numberOfFiles()
{ return LibnumberOfFiles(); }

inline char *SgProject::fileName(int i)
{
  PTR_FILE file;
  char * x;

  file = GetFileWithNum(i);
  SetCurrentFileTo(file);
  SwitchToFile(GetFileNumWithPt(file));
  if (!file)
    x = NULL;
  else
    x = FILE_FILENAME(file);
  return x;
}    

inline int SgProject::Fortranlanguage()
{ return LibFortranlanguage(); }

inline int SgProject::Clanguage()
{ return LibClanguage(); }


// SgFile--inlines
inline int SgFile::languageType()
{  return FILE_LANGUAGE(filept); }

inline void SgFile::saveDepFile(const char *dep_file)
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  LibsaveDepFile(dep_file);
// id may have change all the bifnode class are deleted....
  ResetbfndTableClass();
}

inline void SgFile::unparse(FILE *filedisc)
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  UnparseProgram(filedisc);
}

inline void SgFile::unparseS(FILE *filedisc, int size)
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  UnparseProgram_ThroughAllocBuffer(filedisc,filept,size);
}

     
inline void SgFile::unparsestdout()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  UnparseProgram(stdout);
}


inline SgStatement *SgFile::mainProgram()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  return BfndMapping(getMainProgram());
}

inline int SgFile::numberOfFunctions()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  return getNumberOfFunction();
}

inline int SgFile::numberOfStructs()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  return getNumberOfStruct();
}

inline SgStatement *SgFile::firstStatement()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  SgStatement* retVal = BfndMapping(getFirstStmt());
#ifdef __SPF
  if (retVal)
  {
      SgStatement::setCurrProcessFile(retVal->fileName());
      SgStatement::setCurrProcessLine(0);
  }
#endif
  return retVal;
}

inline SgSymbol *SgFile::firstSymbol()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  return SymbMapping(PROJ_FIRST_SYMB ());
}

inline SgExpression *SgFile::firstExpression()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  return LlndMapping(PROJ_FIRST_LLND ());
}

inline SgType *SgFile::firstType()
{
  SetCurrentFileTo(filept);
  SwitchToFile(GetFileNumWithPt(filept));
  return TypeMapping(PROJ_FIRST_TYPE ());
}


inline SgExpression *SgFile::SgExpressionWithId(int i)
{ return LlndMapping(Get_ll_with_id (i));}

inline SgStatement *SgFile::SgStatementWithId( int id)
{ return BfndMapping(Get_bif_with_id (id)); }
        
inline SgStatement *SgFile::SgStatementAtLine(int lineno)
{ return BfndMapping(rec_num_near_search(lineno));}

inline SgSymbol *SgFile::SgSymbolWithId( int id)
{ return SymbMapping(Get_Symb_with_id (id)); }

inline SgType *SgFile::SgTypeWithId( int id)
{ return TypeMapping(Get_type_with_id (id)); }



// SgStatement--inlines

inline int SgStatement::lineNumber()
{ return BIF_LINE(thebif); }

inline int SgStatement::localLineNumber()
{ return BIF_LOCAL_LINE(thebif); }

inline int  SgStatement::id()
{ return BIF_ID(thebif);}

inline int  SgStatement::variant()
{ return BIF_CODE(thebif); }

// inline functions should contain single return
// hence int x is needed.
inline int SgStatement::hasSymbol()
{
  int x;

  if (BIF_SYMB(thebif))
    x = TRUE;
  else 
    x = FALSE;

  return x;
}

inline SgSymbol * SgStatement::symbol()
{
#ifdef __SPF
    checkConsistence();
#endif
    return SymbMapping(BIF_SYMB(thebif)); 
}

inline char * SgStatement::fileName()
{ return BIF_FILE_NAME(thebif)->name; }

inline void SgStatement::setFileName(char *newFile)
{    
#ifdef __SPF
    checkConsistence();
#endif
    BIF_FILE_NAME(thebif)->name = newFile;
}

inline int SgStatement::hasLabel()
{
  int x;
  if (BIF_LABEL(thebif))
    x = TRUE;
  else
    x = FALSE;
  return x;
}

inline void SgStatement::setlineNumber(const int n)
{ BIF_LINE(thebif) = n; }

inline void SgStatement::setLocalLineNumber(const int n)
{ BIF_LOCAL_LINE(thebif) = n; }

inline void SgStatement::setId(int)
{ Message("Id cannot be changed",BIF_LINE(thebif)); }

inline void SgStatement::setVariant(int n)
{ BIF_CODE(thebif) = n; }

inline void  SgStatement::setLabel(SgLabel &l) 
{
#ifdef __SPF
    checkConsistence();
#endif
    BIF_LABEL(thebif) = l.thelabel; 
}

inline void  SgStatement::deleteLabel()
{
#ifdef __SPF
    checkConsistence();
#endif
    if (BIF_LABEL(thebif))
        BIF_LABEL(thebif)->stateno = -1;
    BIF_LABEL(thebif) = NULL;
}

inline void  SgStatement::setSymbol(SgSymbol &s)
{
#ifdef __SPF
    checkConsistence();
#endif
    BIF_SYMB(thebif) =  s.thesymb; 
}


inline SgStatement * SgStatement::lexNext()
{
#ifdef __SPF
    checkConsistence();
#endif
    SgStatement* retVal = BfndMapping(BIF_NEXT(thebif));
#ifdef __SPF
    if (retVal)
        setCurrProcessLine(retVal->lineNumber());
#endif
    return retVal;
}

inline SgStatement * SgStatement::lexPrev()
{
#ifdef __SPF
    checkConsistence();
#endif
    SgStatement* retVal = BfndMapping(getNodeBefore(thebif));
#ifdef __SPF
    if (retVal)
        setCurrProcessLine(retVal->lineNumber());
#endif
    return retVal;
}


inline SgStatement * SgStatement::controlParent()
{
#ifdef __SPF
    checkConsistence();
#endif
    if (this->variant() != GLOBAL)
        return BfndMapping(BIF_CP(thebif));
    else
        return 0;
}

inline int SgStatement::numberOfChildrenList1()
{
#ifdef __SPF
    checkConsistence();
#endif
    return (blobListLength(BIF_BLOB1(thebif)));
}

inline int SgStatement::numberOfChildrenList2()
{
#ifdef __SPF
    checkConsistence();
#endif
    return (blobListLength(BIF_BLOB2(thebif))); 
}

inline SgStatement * SgStatement::childList1(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(childfInBlobList(BIF_BLOB1(thebif),i)); 
}

inline SgStatement * SgStatement::childList2(int i)
{
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(childfInBlobList(BIF_BLOB2(thebif),i)); 
}


inline void  SgStatement::setLexNext(SgStatement &s)
{
#ifdef __SPF
    checkConsistence();
#endif
    BIF_NEXT(thebif) = s.thebif; 
}

inline SgStatement * SgStatement::lastDeclaration()
{
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(LiblastDeclaration(thebif)); 
}


inline SgStatement * SgStatement::lastExecutable()
{
#ifdef __SPF
    checkConsistence();
#endif
    PTR_BFND last;
    last = getLastNodeOfStmt(thebif);
    last = getNodeBefore(last);
    return BfndMapping(last);
}

inline SgStatement *SgStatement::lastNodeOfStmt()
{
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(getLastNodeOfStmt(thebif)); 
}

inline SgStatement *SgStatement::nodeBefore()
{
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(getNodeBefore(thebif)); 
}

inline void SgStatement::insertStmtBefore(SgStatement &s,SgStatement &cp )
{ 
#ifdef __SPF
    checkConsistence();
#endif
    insertBfndBeforeIn(s.thebif,thebif,cp.thebif); 
}


inline SgStatement * SgStatement::extractStmt()
{
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(LibextractStmt(thebif)); 
}

inline SgStatement *SgStatement::extractStmtBody()
{ 
#ifdef __SPF
    checkConsistence();
#endif
    return BfndMapping(LibextractStmtBody(thebif)); 
}

inline void  SgStatement::replaceWithStmt(SgStatement &s)
{
#ifdef __SPF
    checkConsistence();
#endif
    LibreplaceWithStmt(thebif,s.thebif); 
}

inline void  SgStatement::deleteStmt()
{
#ifdef __SPF
    checkConsistence();
#endif
    LibdeleteStmt(thebif); 
}

inline int SgStatement::isIncludedInStmt(SgStatement &s)
{
#ifdef __SPF
    checkConsistence();
#endif
    return isInStmt(thebif, s.thebif);
}

inline SgStatement &SgStatement::copy()
{
    return *copyPtr();
}

inline SgStatement *SgStatement::copyPtr()
{
#ifdef __SPF
    checkConsistence();
#endif
    SgStatement *copy = BfndMapping(duplicateStmtsNoExtract(thebif));

#ifdef __SPF
    copy->setProject(project);
    copy->setFileId(fileID);
#endif
    return copy; 
}

inline SgStatement & SgStatement::copyOne()
{
     return *copyOnePtr();
}

inline SgStatement * SgStatement::copyOnePtr()
{
#ifdef __SPF
    checkConsistence();
#endif
     SgStatement *new_stmt = BfndMapping(duplicateOneStmt(thebif));

     /* Hackery to make sure the control parent propagates correctly.
	Unfortunately, the copy function itself it badly broken. */

     new_stmt->setControlParent (this->controlParent());
#ifdef __SPF
     new_stmt->setProject(project);
     new_stmt->setFileId(fileID);
#endif
     return new_stmt;
}
  
inline SgStatement& SgStatement::copyBlock()
{ return *copyBlockPtr(); }

inline SgStatement *SgStatement::copyBlockPtr() 
{ return copyBlockPtr(0); }

inline SgStatement* SgStatement::copyBlockPtr(int saveLabelId)
{
#ifdef __SPF
    checkConsistence();
#endif
    SgStatement *new_stmt = BfndMapping(duplicateStmtsBlock(thebif, saveLabelId));
#ifdef __SPF
    new_stmt->setProject(project);
    new_stmt->setFileId(fileID);
#endif
    return new_stmt; 
}

inline void SgStatement::replaceSymbByExp(SgSymbol &symb, SgExpression &exp)
{
  LibreplaceSymbByExpInStmts(thebif, getLastNodeOfStmt(thebif), symb.thesymb, exp.thellnd);
}

inline void SgStatement::replaceSymbBySymb(SgSymbol &symb,SgSymbol &newsymb )
{
#ifdef __SPF
    checkConsistence();
#endif
    replaceSymbInStmts(thebif, getLastNodeOfStmt(thebif), symb.thesymb, newsymb.thesymb);
}

inline void SgStatement::replaceSymbBySymbSameName(SgSymbol &symb,SgSymbol &newsymb)
{
#ifdef __SPF
    checkConsistence();
#endif
    replaceSymbInStmtsSameName(thebif, getLastNodeOfStmt(thebif), symb.thesymb, newsymb.thesymb);
}

inline void SgStatement::replaceTypeInStmt(SgType &old, SgType &newtype)
{// do redundant work by should be ok go twice in member function 
#ifdef __SPF
    checkConsistence();
#endif
  if (BIF_SYMB(thebif))
    replaceTypeUsedInStmt(BIF_SYMB(thebif),thebif,old.thetype,newtype.thetype);
  else
    replaceTypeUsedInStmt(NULL,thebif,old.thetype,newtype.thetype);
}

inline void SgStatement::setComments(char *comments)
{
#ifdef __SPF
    checkConsistence();
#endif
     LibSetAllComments (thebif, comments);
}

inline void SgStatement::setComments(const char *comments)
{
#ifdef __SPF
    checkConsistence();
#endif
    LibSetAllComments(thebif, comments);
}

inline void SgStatement::delComments()
{
#ifdef __SPF
    checkConsistence();
#endif
    LibDelAllComments(thebif);
}


inline SgStatement *SgStatement::getScopeForDeclare()
{
  return BfndMapping(LibGetScopeForDeclare(thebif));
}

//Kataev 07.03.2013
inline char* SgStatement::unparse()
{
#ifdef __SPF
    checkConsistence();
#endif
	return UnparseBif_Char(thebif, 2); //2 - fortran language 
}

inline void SgStatement::unparsestdout()
{
    UnparseBif(thebif);
}

inline char* SgStatement::comments()
{
    char *x;

    if (BIF_CMNT(thebif))
        x = CMNT_STRING(BIF_CMNT(thebif));
    else
        x = NULL;

    return x;
}

inline void SgStatement::addDeclSpec(int type)
{
#ifdef __SPF
    checkConsistence();
#endif
    BIF_DECL_SPECS(thebif) = BIF_DECL_SPECS(thebif) | type;
}  

inline void SgStatement::clearDeclSpec()
{
#ifdef __SPF
    checkConsistence();
#endif
    BIF_DECL_SPECS(thebif) = 0;
}        

inline int SgStatement::isFriend()
{
  return (BIF_DECL_SPECS(thebif) & BIT_FRIEND);
}

inline int SgStatement::isInline()
{
  return (BIF_DECL_SPECS(thebif) & BIT_INLINE);
}

inline int SgStatement::isExtern()
{
  return (BIF_DECL_SPECS(thebif) & BIT_EXTERN);
}

inline int SgStatement::isStatic()
{
  return (BIF_DECL_SPECS(thebif) & BIT_STATIC);
}


// SgExpression--inlines
       
inline SgExpression *SgExpression::lhs()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression *SgExpression::rhs()
{  return LlndMapping(NODE_OPERAND1(thellnd)); }

inline SgExpression *SgExpression::nextInExprTable()
{  return LlndMapping(NODE_NEXT(thellnd)); }

inline int SgExpression::variant()
{  return NODE_CODE(thellnd); }

inline SgType *SgExpression::type()
{ return TypeMapping(NODE_TYPE(thellnd)); }

inline int SgExpression::id()
{ return NODE_ID(thellnd); }

inline void SgExpression::setLhs(SgExpression &e)
{ NODE_OPERAND0(thellnd) = e.thellnd; }

inline void SgExpression::setLhs(SgExpression *e)
{ NODE_OPERAND0(thellnd) = (e == 0) ? 0 : e->thellnd; }

inline void SgExpression::setRhs(SgExpression &e)
{ NODE_OPERAND1(thellnd) = e.thellnd; }

inline void SgExpression::setRhs(SgExpression *e)
{ NODE_OPERAND1(thellnd) = ( e == 0 ) ? 0 : e->thellnd; }

inline void SgExpression::setSymbol(SgSymbol &s)
{ NODE_SYMB(thellnd) = s.thesymb; }

inline void SgExpression::setSymbol(SgSymbol *s)
{ NODE_SYMB(thellnd) = ( s == 0 ) ? 0 : s->thesymb; }

inline void SgExpression::setType(SgType &t)
{ NODE_TYPE(thellnd) = t.thetype; }

inline void SgExpression::setType(SgType *t)
{ NODE_TYPE(thellnd) = (t == 0) ? 0 : t->thetype; }

inline void SgExpression::setVariant(int v)
{  
  Message("Variant of a low level node node should not be change",0);
  NODE_CODE(thellnd) = v; 
}

inline SgExpression &SgExpression::copy()
{  return *copyPtr(); }

inline SgExpression *SgExpression::copyPtr()
{  return LlndMapping(copyLlNode(thellnd)); }


inline SgExpression *SgExpression::IsSymbolInExpression(SgSymbol &symbol)
{ return LlndMapping(LibIsSymbolInExpression(thellnd, symbol.thesymb)); }

inline void SgExpression::replaceSymbolByExpression(SgSymbol &symbol, SgExpression &expr)
{ LibreplaceSymbByExp(thellnd, symbol.thesymb, expr.thellnd); }

inline SgExpression *SgExpression::arrayRefs()
{ return LlndMapping(LibarrayRefs(thellnd)); }

inline SgExpression *SgExpression::symbRefs()
{ return LlndMapping(LibsymbRefs(thellnd,NULL));}

//Kataev 07.03.2013, update 19.10.2013
inline char* SgExpression::unparse()
{
	return UnparseLLND_Char(thellnd);
}

inline void SgExpression::unparsestdout()
{ 
    UnparseLLND(thellnd);
    printf("\n");
}


// SgSymbol--inlines
inline int SgSymbol::variant()
{ return SYMB_CODE(thesymb); }

inline int SgSymbol::id()
{ return SYMB_ID(thesymb); }

inline char *SgSymbol::identifier()
{ return SYMB_IDENT(thesymb); }

inline SgType *SgSymbol::type()
{ return TypeMapping(SYMB_TYPE(thesymb)); }


inline void SgSymbol::setType(SgType &t)
{ SYMB_TYPE(thesymb) = t.thetype; }

inline void SgSymbol::setType(SgType *t)
{ SYMB_TYPE(thesymb) = (t == 0) ? 0 : t->thetype; }

inline SgStatement *SgSymbol::scope()
{ return BfndMapping(SYMB_SCOPE(thesymb)); }

inline SgSymbol *SgSymbol::next()
{ return SymbMapping(SYMB_NEXT(thesymb));}

inline SgSymbol &SgSymbol::copy() 
{
    SgSymbol *copy = SymbMapping(duplicateSymbol(thesymb));

#ifdef __SPF
    copy->setProject(project);
    copy->setFileId(fileID);
#endif
    return *copy;
}

inline SgSymbol &SgSymbol::copyLevel1() 
{
    SgSymbol *new_symb = SymbMapping(duplicateSymbolLevel1(thesymb));

#ifdef __SPF
    new_symb->setProject(project);
    new_symb->setFileId(fileID);
#endif
    return *new_symb;
}

inline SgSymbol &SgSymbol::copyLevel2() 
{
    SgSymbol *new_symb = SymbMapping(duplicateSymbolLevel2(thesymb));

#ifdef __SPF
    new_symb->setProject(project);
    new_symb->setFileId(fileID);
#endif
    return *new_symb;
}

inline SgSymbol &SgSymbol::copyAcrossFiles(SgStatement &where) 
{ 
  resetDoVarForSymb();
  SgSymbol *new_symb = SymbMapping(duplicateSymbolAcrossFiles(thesymb,where.thebif));

#ifdef __SPF
  new_symb->setProject(project);
  new_symb->setFileId(fileID);
#endif
  return *new_symb;
}

inline SgSymbol &SgSymbol::copySubprogram(SgStatement &where) 
{ 
  return *SymbMapping(duplicateSymbolOfRoutine(thesymb,where.thebif)); 
}

inline void SgSymbol::declareTheSymbolWithParamList
            (SgStatement &st, SgExpression &parlist)
{ declareAVarWPar(thesymb, parlist.thellnd, st.thebif); }


inline SgExpression *SgSymbol::makeDeclExprWithParamList
           (SgExpression &parlist)
{ return LlndMapping(makeDeclExpWPar(thesymb, parlist.thellnd));} 

inline SgSymbol *SgSymbol::moduleSymbol()
{ return SymbMapping(SYMB_BASE_NAME(thesymb));}

// SgType--inlines

inline int SgType::variant()
{ return TYPE_CODE(thetype); }

inline int SgType::id()
{ return TYPE_ID(thetype); }

inline SgSymbol *SgType::symbol()
{/* return SymbMapping(TYPE_SYMB_DERIVE(thetype));*/
 return SymbMapping(TYPE_SYMB(thetype));}

inline SgType &SgType::copy()
{ return *copyPtr(); }

inline SgType *SgType::copyPtr()
{ return TypeMapping(duplicateType(thetype));}

inline SgType *SgType::next()
{ return TypeMapping(TYPE_NEXT(thetype)); }

inline int SgType::isTheElementType()
{ return isElementType(thetype);}

inline int SgType::equivalentToType(SgType &type)
{ return isTypeEquivalent(thetype, type.thetype);}

inline int SgType::equivalentToType(SgType *type)
{
  if ( type == 0 )
    return 0;
  else
    return isTypeEquivalent(thetype, type->thetype);
}


inline SgType *SgType::internalBaseType()
{
  PTR_TYPE ty;
  ty = lookForInternalBasetype(thetype);
  return TypeMapping(ty);
}

inline int SgType::hasBaseType()
{
  return hasTypeBaseType(TYPE_CODE(thetype));
}

inline SgType *SgType::baseType()
{
  SgType * x;
  if (hasTypeBaseType(TYPE_CODE(thetype)))
    x = TypeMapping(TYPE_BASE(thetype));
  else
    x = NULL;

  return x;
}

/* update Kataev N.A. 30.08.2013
- add check for NULL range
*/
inline SgExpression *SgType::length()
{
	PTR_LLND lenExpr = TYPE_RANGES( thetype);
	
	return lenExpr ? LlndMapping(NODE_OPERAND0(lenExpr)) : NULL; 
}

inline SgExpression *SgType::selector()
{
    PTR_LLND kindExpr = TYPE_KIND_LEN(thetype);
    return kindExpr ? LlndMapping(TYPE_KIND_LEN(thetype)) : NULL;
}

// SgLabel--inlines
inline int SgLabel::id()
{ return LABEL_STMTNO(thelabel); }

inline int SgLabel::getLastLabelVal()
{ return getLastLabelId();}

// SgValueExp--inlines

inline SgValueExp::SgValueExp(bool value) :SgExpression(BOOL_VAL)
{
  NODE_TYPE(thellnd) = GetAtomicType(T_BOOL);
  NODE_BOOL_CST(thellnd) = value;
}

inline SgValueExp::SgValueExp(int value):SgExpression(INT_VAL)
{
  NODE_TYPE(thellnd) =  GetAtomicType(T_INT);
  NODE_INT_CST_LOW (thellnd) = value;
}

inline SgValueExp::SgValueExp(char char_val):SgExpression( CHAR_VAL)
{
  NODE_TYPE(thellnd) = GetAtomicType(T_CHAR);
  NODE_CHAR_CST(thellnd) = char_val;
}

inline SgValueExp::SgValueExp(float float_val, char *val) :SgExpression(FLOAT_VAL)
{
    NODE_STR(thellnd) = (char*)xmalloc((strlen(val) + 1)*sizeof(char));
    strcpy(NODE_STR(thellnd), val);
    NODE_TYPE(thellnd) = GetAtomicType(T_FLOAT);    
}

inline SgValueExp::SgValueExp(double double_val, char *val) :SgExpression(DOUBLE_VAL)
{
    NODE_STR(thellnd) = (char*)xmalloc((strlen(val) + 1)*sizeof(char));
    strcpy(NODE_STR(thellnd), val);
    NODE_TYPE(thellnd) = GetAtomicType(T_DOUBLE);
}

inline SgValueExp::SgValueExp(float float_val):SgExpression(FLOAT_VAL)
{
  char tmp[100]; // No doubles longer than 100 digits;
  sprintf (tmp,"%.8e",float_val);
  NODE_STR(thellnd) = (char*) xmalloc ((strlen(tmp) + 1)*sizeof(char));
  strcpy(NODE_STR(thellnd), tmp);
  NODE_TYPE(thellnd) = GetAtomicType(T_FLOAT);
  
}

inline SgValueExp::SgValueExp(double double_val):SgExpression(DOUBLE_VAL)
{
  char tmp[100]; // No doubles longer than 100 digits ;
  sprintf (tmp,"%.16e",double_val);
  NODE_STR(thellnd) = (char*) xmalloc ((strlen(tmp) + 1)*sizeof(char));
  strcpy(NODE_STR(thellnd), tmp);
  NODE_TYPE(thellnd) = GetAtomicType(T_DOUBLE);
}

inline SgValueExp::SgValueExp(char *string_val):SgExpression(STRING_VAL)
{
  NODE_TYPE(thellnd) = GetAtomicType(T_STRING);
  NODE_STRING_POINTER(thellnd) = string_val;
}

inline SgValueExp::SgValueExp(const char *string_val) :SgExpression(STRING_VAL)
{
    NODE_STR(thellnd) = (char*)xmalloc((strlen(string_val) + 1) * sizeof(char));
    strcpy(NODE_STR(thellnd), string_val);
    NODE_TYPE(thellnd) = GetAtomicType(T_STRING);
}

inline SgValueExp::SgValueExp(double real, double imaginary):SgExpression(COMPLEX_VAL)
{
  NODE_TYPE(thellnd) = GetAtomicType(T_COMPLEX);
  NODE_OPERAND0(thellnd) = SgValueExp(real).thellnd;
  NODE_OPERAND1(thellnd) = SgValueExp(imaginary).thellnd;
}

inline SgValueExp::SgValueExp(SgValueExp &real, SgValueExp &imaginary):SgExpression(COMPLEX_VAL)
{
  NODE_TYPE(thellnd) = GetAtomicType(T_COMPLEX);
  NODE_OPERAND0(thellnd) = real.thellnd;
  NODE_OPERAND1(thellnd) = imaginary.thellnd;
}

// are these setValue functions really needed?
// the user can simply say, SgValueExp(3.0) and
// get the same functionality, in most cases.
// Moreover, the code is wrong. The NODE_ CODE field
// must be checked.
inline  void SgValueExp::setValue(int int_val)
{
  NODE_INT_CST_LOW (thellnd) = int_val;
}

inline  void SgValueExp::setValue(char char_val)
{
  NODE_CHAR_CST(thellnd) = char_val;
}

inline  void SgValueExp::setValue(float float_val)
{
  char tmp[100]; // No doubles longer than 100 digits ;
  sprintf (tmp,"%e",float_val);
  if (!NODE_STR(thellnd))
    NODE_STR(thellnd) = (char*) xmalloc ((strlen(tmp) + 1)*sizeof(char));
  strcpy(NODE_STR(thellnd),tmp);
}

inline  void SgValueExp::setValue(double double_val)
{
  char tmp[100]; // No doubles longer than 100 digits ;
  sprintf (tmp,"%e",double_val);
  if (!NODE_STR(thellnd))
    NODE_STR(thellnd) = (char*) xmalloc ((strlen(tmp) + 1)*sizeof(char));
  strcpy(NODE_STR(thellnd),tmp);
}

inline  void SgValueExp::setValue(char *string_val)
{
  NODE_STRING_POINTER(thellnd) = string_val;
}

inline  void SgValueExp::setValue(double real, double im)
{
  NODE_OPERAND0(thellnd) = SgValueExp(real).thellnd;
  NODE_OPERAND1(thellnd) = SgValueExp(im).thellnd;
}

inline  void SgValueExp::setValue(SgValueExp &real, SgValueExp & im)
{
  NODE_OPERAND0(thellnd) = real.thellnd;
  NODE_OPERAND1(thellnd) = im.thellnd;
}

inline bool SgValueExp::boolValue()
{
  bool x;
  if (NODE_CODE(thellnd) != BOOL_VAL)
    {
      Message("message boolValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = false;
    }
  else
    x = NODE_BOOL_CST(thellnd);
  return x;
}

inline int SgValueExp::intValue()
{
  int x;
  if (NODE_CODE(thellnd) != INT_VAL)
    {
      Message("message initValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = 0;
    }
  else
    x = NODE_INT_CST_LOW (thellnd);
  return x;
}

inline char* SgValueExp::floatValue()
{
  char*  x;

  if (NODE_CODE(thellnd) != FLOAT_VAL)
    {
      Message("message floatValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = NULL;
    }
  else 
    x = NODE_FLOAT_CST(thellnd);

  return x;
}

inline char SgValueExp::charValue()
{
  char x;

  if (NODE_CODE(thellnd) != CHAR_VAL)
    {
      Message("message charValue not understood");
#ifdef __SPF
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = 0;
    }
  else
    x = NODE_CHAR_CST(thellnd);

  return x;
}

inline char*  SgValueExp::doubleValue()
{
  char*  x;

  if (NODE_CODE(thellnd) != DOUBLE_VAL)
    {
      Message("message doubleValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = NULL;
    }
  else
    x = NODE_DOUBLE_CST(thellnd);

  return x;
}

inline char * SgValueExp::stringValue()
{
  char *x;

  if (NODE_CODE(thellnd) != STRING_VAL)
    {
      Message("message stringValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = NULL;
    }
  else
    x = NODE_STRING_POINTER(thellnd);

  return x;
}

inline SgExpression * SgValueExp:: realValue()
{
  SgExpression *x;

  if (NODE_CODE(thellnd) != COMPLEX_VAL)
    {
      Message("message realValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = NULL;
    }
  else 
    x = LlndMapping(NODE_OPERAND0(thellnd));

  return x;
}

inline SgExpression * SgValueExp::imaginaryValue()
{
  SgExpression *x;

  if (NODE_CODE(thellnd) != COMPLEX_VAL)
    {
      Message("message imaginaryValue not understood");
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
      x = NULL;
    }
  else
    x = LlndMapping(NODE_OPERAND1(thellnd));

  return x;
}



// SgKeywordValExp--inlines
inline  SgKeywordValExp::SgKeywordValExp(char *name):SgExpression(KEYWORD_VAL)
{ NODE_STRING_POINTER(thellnd) = name; }

inline  SgKeywordValExp::SgKeywordValExp(const char *name):SgExpression(KEYWORD_VAL)
{
    NODE_STR(thellnd) = (char*)xmalloc((strlen(name) + 1) * sizeof(char));
    strcpy(NODE_STR(thellnd), name);
}

inline char * SgKeywordValExp::value()
{ return NODE_STRING_POINTER(thellnd); }


// SgUnaryExp--inlines

// In the code below, no type checking has been done. 
// Some of the parser code may be modified to do the type-checking.
// For example, SgUnaryExp(ADDRESS_OP, 2) should not
// be detected.
// the standard unary expressons
// variant:DEREF_OP      * expr
// variant:ADDRESS_OP    & expr
// variant:MINUS_OP      - expr
// variant:UNARY_ADD_OP  + expr
// variant:PLUSPLUS_OP   ++lhd  or rhs++
// variant:MINUSMINUS_OP --lhs  or rhs--
// variant:BIT_COMPLEMENT_OP  ~ expr
// variant:NOT_OP        ! expr
// variant:SIZE_OP       sizeof( expr)

inline SgUnaryExp::SgUnaryExp(PTR_LLND ll):SgExpression(ll)
{}
inline SgUnaryExp::SgUnaryExp(int variant, SgExpression & e):SgExpression(variant)
{
  NODE_OPERAND0(thellnd) = e.thellnd;
}

inline SgUnaryExp::SgUnaryExp(int variant, int post, SgExpression &e):SgExpression(variant)
{  // post =1 rhs++
  if (post)
    NODE_OPERAND1(thellnd) = e.thellnd;
  else
    NODE_OPERAND0(thellnd) = e.thellnd;
}

inline  int SgUnaryExp::post() // returns TRUE if a post inc or dec op.
{ if (NODE_OPERAND1(thellnd)) return TRUE; else return FALSE;}


// SgCastExp--inlines

inline SgCastExp::SgCastExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgCastExp::SgCastExp(SgType &t, SgExpression &e):SgExpression(CAST_OP)
{
  NODE_TYPE(thellnd) = t.thetype;
  NODE_OPERAND0(thellnd) = e.thellnd;
  // an experiment to fix the bernd bug.
  NODE_OPERAND1(thellnd) = (SgMakeDeclExp(NULL, &t))->thellnd;
}

inline SgCastExp::SgCastExp(SgType &t):SgExpression(CAST_OP)
{ NODE_TYPE(thellnd) = t.thetype; }

inline SgCastExp::~SgCastExp(){RemoveFromTableLlnd((void *) this);}


// SgDeleteExp--inlines

inline SgDeleteExp::SgDeleteExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgDeleteExp::SgDeleteExp(SgExpression &size,SgExpression &expr):SgExpression(DELETE_OP)
{
  NODE_OPERAND0(thellnd) = expr.thellnd;
  NODE_OPERAND1(thellnd) = size.thellnd;
}

inline SgDeleteExp::SgDeleteExp( SgExpression &expr):SgExpression(DELETE_OP)
{
  NODE_OPERAND0(thellnd) = expr.thellnd;
}

inline SgDeleteExp::~SgDeleteExp()
{ RemoveFromTableLlnd((void *) this); }



// SgNewExp--inlines


inline SgNewExp::SgNewExp(PTR_LLND ll):SgExpression(ll)
{}
 
inline SgNewExp::SgNewExp(SgType &t):SgExpression(NEW_OP)
{
  SgCastExp *pt;
   pt =  new SgCastExp(t);
   NODE_OPERAND0(thellnd) = pt->thellnd;
}
 
inline SgNewExp::SgNewExp(SgType &t, SgExpression &e):SgExpression(NEW_OP)
{
  SgCastExp *pt;
  pt =  new SgCastExp(t);
  NODE_OPERAND0(thellnd) = pt->thellnd;
  NODE_OPERAND1(thellnd) = e.thellnd;
}

inline SgNewExp::~SgNewExp()
{ RemoveFromTableLlnd((void *) this); }


// SgExprIfExp--inlines

inline SgExprIfExp::SgExprIfExp(PTR_LLND ll): SgExpression(ll)
{}

inline SgExprIfExp::SgExprIfExp(SgExpression &exp1, 
                                SgExpression &exp2, 
                                SgExpression &exp3):SgExpression(EXPR_IF)
{
  NODE_OPERAND0(thellnd)= exp1.thellnd;
  NODE_OPERAND1(thellnd)= newExpr(EXPR_IF_BODY,NODE_TYPE(exp2.thellnd),exp2.thellnd,exp3.thellnd);
}

inline void SgExprIfExp::setConditional(SgExpression &c)
{
  NODE_OPERAND0(thellnd) = c.thellnd;
}

// SgFunctionRefExp--inlines
inline SgFunctionRefExp::SgFunctionRefExp(PTR_LLND ll):SgExpression(ll)
{}
inline SgFunctionRefExp::SgFunctionRefExp(SgSymbol &fun):SgExpression(FUNCTION_REF)
{
  NODE_SYMB (thellnd) = fun.thesymb;
}
inline SgFunctionRefExp::~SgFunctionRefExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgSymbol *SgFunctionRefExp::funName()
{ return SymbMapping(NODE_SYMB(thellnd)); }

inline SgExpression * SgFunctionRefExp::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgFunctionRefExp::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgFunctionRefExp::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

// SgFunctionCallExp--inlines

inline SgFunctionCallExp::SgFunctionCallExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgFunctionCallExp::SgFunctionCallExp(SgSymbol &fun, SgExpression &paramList):SgExpression(FUNC_CALL)
{
  NODE_SYMB (thellnd) = fun.thesymb;
  NODE_OPERAND0(thellnd) = paramList.thellnd;
}

inline SgFunctionCallExp::SgFunctionCallExp(SgSymbol &fun):SgExpression(FUNC_CALL)
{
  NODE_SYMB (thellnd) = fun.thesymb;
}
inline SgFunctionCallExp::~SgFunctionCallExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgSymbol *SgFunctionCallExp::funName()
{ return SymbMapping(NODE_SYMB(thellnd)); }

inline SgExpression * SgFunctionCallExp::args()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  int SgFunctionCallExp::numberOfArgs()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgFunctionCallExp::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND0(thellnd),i));  }

inline void SgFunctionCallExp::addArg(SgExpression &arg)
{ NODE_OPERAND0(thellnd) = addToExprList(NODE_OPERAND0(thellnd),arg.thellnd); }



// SgFuncPntrExp--inlines

inline SgFuncPntrExp::SgFuncPntrExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgFuncPntrExp::SgFuncPntrExp(SgExpression &ptr):SgExpression(FUNCTION_OP)
{ NODE_OPERAND0(thellnd) = ptr.thellnd; }

inline SgFuncPntrExp::~SgFuncPntrExp(){RemoveFromTableLlnd((void *) this);}

inline SgExpression * SgFuncPntrExp::funExp()
{  return LlndMapping(NODE_OPERAND0(thellnd)); }

inline  void SgFuncPntrExp::setFunExp(SgExpression &s)
{ NODE_OPERAND0(thellnd) = s.thellnd; }

inline int SgFuncPntrExp::numberOfArgs()
{ return exprListLength(NODE_OPERAND1(thellnd)); }

inline  SgExpression * SgFuncPntrExp::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND1(thellnd),i)); }

inline void SgFuncPntrExp::addArg(SgExpression &arg)
{ NODE_OPERAND1(thellnd) = addToExprList(NODE_OPERAND1(thellnd),arg.thellnd);}
          


// SgExprListExp--inlines

// Kolganov A.S. 31.10.2013
inline SgExprListExp::SgExprListExp(int variant) :SgExpression(variant)
{}

inline SgExprListExp::SgExprListExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgExprListExp::SgExprListExp():SgExpression(EXPR_LIST)
{}

inline SgExprListExp::SgExprListExp(SgExpression &ptr):SgExpression(EXPR_LIST)
{ NODE_OPERAND0(thellnd) = ptr.thellnd; }

inline SgExprListExp::~SgExprListExp(){RemoveFromTableLlnd((void *) this);}

inline  int SgExprListExp::length()
{ return exprListLength(thellnd); }

inline  SgExpression * SgExprListExp::elem(int i)
{ return LlndMapping(getPositionInExprList(thellnd,i)); }

inline SgExprListExp * SgExprListExp::next()
{ return (SgExprListExp *) LlndMapping(NODE_OPERAND1(thellnd)); }

inline SgExpression * SgExprListExp::value()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline void SgExprListExp::setValue(SgExpression &ptr)
{ NODE_OPERAND0(thellnd) = ptr.thellnd; }

inline void SgExprListExp::append(SgExpression &arg)
{ thellnd = addToExprList(thellnd,arg.thellnd); }


// SgRefExp--inlines
inline SgRefExp::SgRefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgRefExp::SgRefExp(int variant, SgSymbol &s):SgExpression(variant)
{
  NODE_SYMB(thellnd) = s.thesymb;
  NODE_TYPE(thellnd) = SYMB_TYPE(s.thesymb);
}

inline SgRefExp::~SgRefExp()
{ RemoveFromTableLlnd((void *) this); }

// SgTypeRefExp -- inlines

inline SgTypeRefExp::SgTypeRefExp(SgType &t): SgExpression(TYPE_REF){
     NODE_TYPE(thellnd) = t.thetype;
}

inline SgType * SgTypeRefExp::getType(){
     return TypeMapping(NODE_TYPE(thellnd));
}

inline SgTypeRefExp::~SgTypeRefExp()
{ RemoveFromTableLlnd((void *) this); }

// SgVarRefExp--inlines

inline SgVarRefExp::SgVarRefExp (PTR_LLND ll):SgExpression(ll)
{}

inline SgVarRefExp::SgVarRefExp(SgSymbol &s):SgExpression(VAR_REF)
{
  NODE_TYPE(thellnd) = SYMB_TYPE(s.thesymb);
  NODE_SYMB(thellnd) = s.thesymb;
}
inline SgVarRefExp::SgVarRefExp(SgSymbol *s):SgExpression(VAR_REF)
{
  if(s){
  NODE_TYPE(thellnd) = SYMB_TYPE(s->thesymb);
  NODE_SYMB(thellnd) = s->thesymb;
  }
}

inline SgVarRefExp::~SgVarRefExp()
{ RemoveFromTableLlnd((void *) this); }


// SgThisExp--inlines

inline SgThisExp::SgThisExp (PTR_LLND ll):SgExpression(ll)
{}

inline SgThisExp::SgThisExp(SgType &t):SgExpression(THIS_NODE)
{ NODE_TYPE(thellnd) = t.thetype; }

inline SgThisExp::~SgThisExp()
{ RemoveFromTableLlnd((void *) this); }


// SgArrayRefExp--inlines

inline SgArrayRefExp::SgArrayRefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgArrayRefExp::SgArrayRefExp(SgSymbol &s):SgExpression(ARRAY_REF)
{
  PTR_SYMB symb;
  
  symb = s.thesymb;
  if (!arraySymbol(symb))
  {
      Message("Attempt to create an array ref with a symbol not of type array", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  NODE_SYMB(thellnd) = symb;
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb));
}

inline SgArrayRefExp::SgArrayRefExp(SgSymbol &s, SgExpression &subscripts):SgExpression(ARRAY_REF)
{
  PTR_SYMB symb;
  
  symb = s.thesymb;
  if (!arraySymbol(symb))
  {
      Message("Attempt to create an array ref with a symbol not of type array", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  
  NODE_SYMB(thellnd) = symb;
  if(NODE_CODE(subscripts.thellnd) == EXPR_LIST)
     NODE_OPERAND0(thellnd) = subscripts.thellnd;
  else
     NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),subscripts.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb));
}

inline SgArrayRefExp::SgArrayRefExp(SgSymbol &s, SgExpression &sub1,SgExpression &sub2):SgExpression(ARRAY_REF)
{
  PTR_SYMB symb;
  
  symb = s.thesymb;
  
  if (!arraySymbol(symb))
  {
      Message("Attempt to create an array ref with a symbol not of type array", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  NODE_SYMB(thellnd) = symb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub1.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub2.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb));
}


inline SgArrayRefExp::SgArrayRefExp(SgSymbol &s, SgExpression &sub1,SgExpression &sub2,SgExpression &sub3):SgExpression(ARRAY_REF)
{
  PTR_SYMB symb;
  
  symb = s.thesymb;
  
  if (!arraySymbol(symb))
  {
      Message("Attempt to create an array ref with a symbol not of type array", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  NODE_SYMB(thellnd) = symb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub1.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub2.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub3.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb));
}

inline SgArrayRefExp::SgArrayRefExp(SgSymbol &s, SgExpression &sub1,SgExpression &sub2,SgExpression &sub3,SgExpression &sub4):SgExpression(ARRAY_REF)
{
  PTR_SYMB symb;
  
  symb = s.thesymb;
  
  if (!arraySymbol(symb))
  {
      Message("Attempt to create an array ref with a symbol not of type array", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  NODE_SYMB(thellnd) = symb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub1.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub2.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub3.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub4.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb));
}

inline SgArrayRefExp::  ~SgArrayRefExp()
{ RemoveFromTableLlnd((void *) this); }

// the number of subscripts in reference
inline int SgArrayRefExp::numberOfSubscripts()
{ return exprListLength(NODE_OPERAND0(thellnd));}

inline SgExpression * SgArrayRefExp::subscripts()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgArrayRefExp::subscript(int i)
{
  PTR_LLND ll = NULL;
  ll = getPositionInExprList(NODE_OPERAND0(thellnd),i);
  return LlndMapping(ll);
}

inline void SgArrayRefExp::addSubscript(SgExpression &e)
{ NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),e.thellnd);}

inline void SgArrayRefExp::setSymbol(SgSymbol &s)
{ NODE_SYMB(thellnd) = s.thesymb;}


// SgProcessorsRefExp--inlines

inline SgProcessorsRefExp::SgProcessorsRefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgProcessorsRefExp::SgProcessorsRefExp():SgExpression(PROCESSORS_REF)
{
  SgSymbol *symb;
  
  symb = new SgSymbol(VARIABLE_NAME, "_PROCESSORS");
  NODE_SYMB(thellnd) = symb->thesymb;
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb->thesymb));
}

inline SgProcessorsRefExp::SgProcessorsRefExp(SgExpression &subscripts):SgExpression(PROCESSORS_REF)
{
  SgSymbol *symb;
  
  symb = new SgSymbol(VARIABLE_NAME, "_PROCESSORS");
  NODE_SYMB(thellnd) = symb->thesymb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),subscripts.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb->thesymb));
}

inline SgProcessorsRefExp::SgProcessorsRefExp(SgExpression &sub1,SgExpression &sub2):SgExpression(PROCESSORS_REF)
{
  SgSymbol *symb;
  
  symb = new SgSymbol(VARIABLE_NAME, "_PROCESSORS");
  NODE_SYMB(thellnd) = symb->thesymb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub1.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub2.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb->thesymb));
}


inline SgProcessorsRefExp::SgProcessorsRefExp(SgExpression &sub1,SgExpression &sub2,SgExpression &sub3):SgExpression(PROCESSORS_REF)
{
  SgSymbol *symb;
  
  symb = new SgSymbol(VARIABLE_NAME, "_PROCESSORS");
  NODE_SYMB(thellnd) = symb->thesymb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub1.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub2.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub3.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb->thesymb));
}

inline SgProcessorsRefExp::SgProcessorsRefExp(SgExpression &sub1,SgExpression &sub2,SgExpression &sub3,SgExpression &sub4):SgExpression(PROCESSORS_REF)
{
  SgSymbol *symb;
  
  symb = new SgSymbol(VARIABLE_NAME, "_PROCESSORS");
  NODE_SYMB(thellnd) = symb->thesymb;
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub1.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub2.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub3.thellnd);
  NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),sub4.thellnd);
  NODE_TYPE(thellnd) = lookForInternalBasetype(SYMB_TYPE(symb->thesymb));
}

inline SgProcessorsRefExp::  ~SgProcessorsRefExp()
{ RemoveFromTableLlnd((void *) this); }

// the number of subscripts in reference
inline int SgProcessorsRefExp::numberOfSubscripts()
{ return exprListLength(NODE_OPERAND0(thellnd));}

inline SgExpression * SgProcessorsRefExp::subscripts()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgProcessorsRefExp::subscript(int i)
{
  PTR_LLND ll = NULL;
  ll = getPositionInExprList(NODE_OPERAND0(thellnd),i);
  return LlndMapping(ll);
}

inline void SgProcessorsRefExp::addSubscript(SgExpression &e)
{ NODE_OPERAND0(thellnd) =  addToExprList(NODE_OPERAND0(thellnd),e.thellnd);}



// SgPntrArrRefExp--inlines

inline SgPntrArrRefExp::SgPntrArrRefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgPntrArrRefExp::SgPntrArrRefExp(SgExpression &p):SgExpression(ARRAY_OP)
{ NODE_OPERAND0(thellnd) = p.thellnd; }

inline SgPntrArrRefExp::SgPntrArrRefExp(SgExpression &p, SgExpression &subscripts):SgExpression(ARRAY_OP)
{
  NODE_OPERAND0(thellnd) = p.thellnd;
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),subscripts.thellnd);
}

inline SgPntrArrRefExp::SgPntrArrRefExp(SgExpression &p, int, SgExpression &sub1, SgExpression &sub2):SgExpression(ARRAY_OP)
{
  NODE_OPERAND0(thellnd) = p.thellnd;
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub1.thellnd);
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub2.thellnd);
}

inline SgPntrArrRefExp::SgPntrArrRefExp(SgExpression &p, int, SgExpression &sub1, SgExpression &sub2, SgExpression &sub3):SgExpression(ARRAY_OP)
{
  NODE_OPERAND0(thellnd) = p.thellnd;
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub1.thellnd);
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub2.thellnd);
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub3.thellnd);
}

inline SgPntrArrRefExp::SgPntrArrRefExp(SgExpression &p, int, SgExpression &sub1, SgExpression &sub2, SgExpression &sub3, SgExpression &sub4):SgExpression(ARRAY_OP)
{
  NODE_OPERAND0(thellnd) = p.thellnd;
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub1.thellnd);
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub2.thellnd);
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub3.thellnd);
  NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),sub4.thellnd);
}

inline SgPntrArrRefExp::~SgPntrArrRefExp()
{ RemoveFromTableLlnd((void *) this); }

inline int SgPntrArrRefExp::dimension()
{ return exprListLength(NODE_OPERAND1(thellnd)); }

inline SgExpression *SgPntrArrRefExp::subscript(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND1(thellnd),i)); }

inline void SgPntrArrRefExp::addSubscript(SgExpression &e)
{ NODE_OPERAND1(thellnd) =  addToExprList(NODE_OPERAND1(thellnd),e.thellnd); }

inline void SgPntrArrRefExp::setPointer(SgExpression &p)
{ NODE_OPERAND0(thellnd) = p.thellnd; }


// SgPointerDerefExp--inlines

inline SgPointerDerefExp::SgPointerDerefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgPointerDerefExp::SgPointerDerefExp(SgExpression &pointerExp):SgExpression(DEREF_OP)
{
  PTR_TYPE expType;
  
  expType = NODE_TYPE(pointerExp.thellnd);
  if (!pointerType(expType))
  {
      Message("Attempt to create SgPointerDerefExp with non pointer type", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }

  NODE_OPERAND0(thellnd) = pointerExp.thellnd;
  NODE_TYPE(thellnd) = lookForInternalBasetype(expType);
}

inline SgPointerDerefExp::~SgPointerDerefExp()
{ RemoveFromTableLlnd((void *) this);}


inline SgExpression * SgPointerDerefExp::pointerExp()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }


// SgRecprdRefExp--inlines

inline SgRecordRefExp::SgRecordRefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgRecordRefExp::SgRecordRefExp(SgSymbol &recordName, char *fieldName):SgExpression(RECORD_REF)
{ 
  PTR_SYMB recordSym, fieldSym;
  
  recordSym = recordName.thesymb;

  if ((fieldSym = getFieldOfStructWithName(fieldName, SYMB_TYPE(recordSym))) == SMNULL)
  {
      Message("No such field", 0);
#ifdef __SPF 
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }

  NODE_OPERAND0(thellnd) = newExpr(VAR_REF,SYMB_TYPE(recordName.thesymb), recordName.thesymb);
  NODE_OPERAND1(thellnd) = newExpr(VAR_REF,SYMB_TYPE(fieldSym), fieldSym);
  NODE_TYPE(thellnd) = SYMB_TYPE(fieldSym);
}

inline SgRecordRefExp::SgRecordRefExp(SgExpression &recordExp, char *fieldName):SgExpression(RECORD_REF)
{
  PTR_SYMB  fieldSym;

            
  if ((fieldSym = getFieldOfStructWithName(fieldName, NODE_TYPE(recordExp.thellnd))) == SMNULL)
  {
      Message("No such field", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  
  NODE_OPERAND0(thellnd) = recordExp.thellnd;
  NODE_OPERAND1(thellnd) = newExpr(VAR_REF,SYMB_TYPE(fieldSym),fieldSym);
  NODE_TYPE(thellnd) = SYMB_TYPE(fieldSym);
}

inline SgRecordRefExp::SgRecordRefExp(SgSymbol &recordName, const char *fieldName) :SgExpression(RECORD_REF)
{
    PTR_SYMB recordSym, fieldSym;

    recordSym = recordName.thesymb;

    if ((fieldSym = getFieldOfStructWithName(fieldName, SYMB_TYPE(recordSym))) == SMNULL)
    {
        Message("No such field", 0);
#ifdef __SPF   
        {
            char buf[512];
            sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
            addToGlobalBufferAndPrint(buf);
        }
        throw -1;
#endif
    }

    NODE_OPERAND0(thellnd) = newExpr(VAR_REF, SYMB_TYPE(recordName.thesymb), recordName.thesymb);
    NODE_OPERAND1(thellnd) = newExpr(VAR_REF, SYMB_TYPE(fieldSym), fieldSym);
    NODE_TYPE(thellnd) = SYMB_TYPE(fieldSym);
}

inline SgRecordRefExp::SgRecordRefExp(SgExpression &recordExp, const char *fieldName) :SgExpression(RECORD_REF)
{
    PTR_SYMB  fieldSym;


    if ((fieldSym = getFieldOfStructWithName(fieldName, NODE_TYPE(recordExp.thellnd))) == SMNULL)
    {
        Message("No such field", 0);
#ifdef __SPF   
        {
            char buf[512];
            sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
            addToGlobalBufferAndPrint(buf);
        }
        throw -1;
#endif
    }

    NODE_OPERAND0(thellnd) = recordExp.thellnd;
    NODE_OPERAND1(thellnd) = newExpr(VAR_REF, SYMB_TYPE(fieldSym), fieldSym);
    NODE_TYPE(thellnd) = SYMB_TYPE(fieldSym);
}

inline SgRecordRefExp::~SgRecordRefExp(){RemoveFromTableLlnd((void *) this);}

inline SgSymbol * SgRecordRefExp::fieldName()
{ return SymbMapping(NODE_SYMB(NODE_OPERAND1(thellnd))); }

inline SgSymbol * SgRecordRefExp::recordName()
{
  SgSymbol *x;

  if (NODE_CODE(NODE_OPERAND0(thellnd)) != VAR_REF)
    x = NULL;
  else 
    x = SymbMapping(NODE_SYMB(NODE_OPERAND0(thellnd)));
  
  return x;
}

inline SgExpression * SgRecordRefExp::record()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }



// SgStructConstExp--inlines

inline SgStructConstExp::SgStructConstExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgStructConstExp::SgStructConstExp(SgSymbol &structName, SgExpression &values):SgExpression(STRUCTURE_CONSTRUCTOR)
{
  NODE_OPERAND0(thellnd) = newExpr(TYPE_REF,SYMB_TYPE(structName.thesymb),structName.thesymb);
  NODE_OPERAND1(thellnd) = values.thellnd;
  NODE_TYPE(thellnd) = SYMB_TYPE(structName.thesymb);
}

inline SgStructConstExp::SgStructConstExp(SgExpression  &typeRef, SgExpression &values):SgExpression(STRUCTURE_CONSTRUCTOR)
{
  NODE_OPERAND0(thellnd) = typeRef.thellnd;
  NODE_OPERAND1(thellnd) = values.thellnd;
  NODE_TYPE(thellnd) = NODE_TYPE(typeRef.thellnd);
}

inline SgStructConstExp::~SgStructConstExp()
{ RemoveFromTableLlnd((void *) this); }

inline int SgStructConstExp::numberOfArgs()
{ return exprListLength(NODE_OPERAND1(thellnd)); }
 
inline SgExpression * SgStructConstExp::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND1(thellnd),i)); }


// SgConstExp--inlines

inline  SgConstExp::SgConstExp(PTR_LLND ll):SgExpression(ll)
{}

// NODE_ TYPE needs to be filled here.
// type-checking of values needs to be done.
inline  SgConstExp::SgConstExp(SgExpression &values):SgExpression(CONSTRUCTOR_REF)
{
  NODE_OPERAND0(thellnd) = values.thellnd;
}

inline SgConstExp::~SgConstExp(){RemoveFromTableLlnd((void *) this);}

inline int SgConstExp::numberOfArgs()
{ return exprListLength(NODE_OPERAND1(thellnd)); }

inline SgExpression * SgConstExp::arg(int i)
{ return LlndMapping(getPositionInExprList(NODE_OPERAND1(thellnd),i)); }



// SgVecConstExp--inlines

inline SgVecConstExp::SgVecConstExp(PTR_LLND ll):SgExpression(ll)
{}

#ifdef NOT_YET_IMPLEMENTED
inline SgVecConstExp::SgVecConstExp(SgExpression &expr_list):SgExpression(VECTOR_CONST)
{ SORRY; }
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgVecConstExp::SgVecConstExp(int n, SgExpression *components):SgExpression(VECTOR_CONST)
{ SORRY; }
#endif

inline SgVecConstExp::~SgVecConstExp()
{ RemoveFromTableLlnd((void *) this); }
        
#ifdef NOT_YET_IMPLEMENTED
inline SgExpression * SgVecConstExp::arg(int i)
{ 
  SORRY;
  return (SgExpression *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline int SgVecConstExp::numberOfArgs()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline void SgVecConstExp::setArg(int i, SgExpression &e)
{
  SORRY;
}
#endif



// SgInitListExp--inlines

inline SgInitListExp::SgInitListExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgInitListExp::SgInitListExp(SgExpression &expr_list):SgExpression(INIT_LIST)
{
  NODE_OPERAND0(thellnd)=expr_list.thellnd;
  NODE_TYPE(thellnd)=NODE_TYPE(expr_list.thellnd);
}

#ifdef NOT_YET_IMPLEMENTED
inline SgInitListExp::SgInitListExp(int n, SgExpression *components):SgExpression(INIT_LIST)
{
  SORRY;
}
#endif

inline SgInitListExp::~SgInitListExp()
{ RemoveFromTableLlnd((void *) this); }

        
#ifdef NOT_YET_IMPLEMENTED
inline SgExpression * SgInitListExp::arg(int i)
{
  SORRY;
  return (SgExpression *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline int SgInitListExp::numberOfArgs()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline void SgInitListExp::setArg(int i, SgExpression &e)
{
  SORRY;
}
#endif


// SgObjectListExp--inlines

inline SgObjectListExp::SgObjectListExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgObjectListExp::SgObjectListExp(int variant, SgSymbol &object, SgExpression &list):SgExpression(variant)
{
#ifdef AJM_SUGGESTS

// This is not what is expected in a COMMON block.
//  NODE_OPERAND0(thellnd) = newExpr(VAR_REF, SYMB_TYPE(object.thesymb), object.thesymb);
  NODE_SYMB(thellnd) = object.thesymb;
  NODE_OPERAND0(thellnd) = list.thellnd;

#else /* Original */

  NODE_OPERAND0(thellnd) = newExpr(VAR_REF, SYMB_TYPE(object.thesymb), object.thesymb);
  NODE_OPERAND1(thellnd) = list.thellnd;

#endif
}

inline SgObjectListExp::SgObjectListExp(int variant,SgExpression &objectRef, SgExpression &list):SgExpression(variant)
{
#ifdef AJM_SUGGESTS
// Not what a common block wants.
//  NODE_OPERAND0(thellnd) = objectRef.thellnd;
  NODE_SYMB(thellnd)=objectRef.symbol()->thesymb;
  NODE_OPERAND0(thellnd) = list.thellnd;
#else
  NODE_OPERAND0(thellnd) = objectRef.thellnd;
  NODE_OPERAND1(thellnd) = list.thellnd;
#endif
}

inline SgObjectListExp::~SgObjectListExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgSymbol * SgObjectListExp::object( )
{ return SymbMapping( NODE_SYMB(thellnd)); }

inline SgObjectListExp * SgObjectListExp::next( )
{ return static_cast< SgObjectListExp * >( LlndMapping(NODE_OPERAND1(thellnd))); }

inline SgExpression * SgObjectListExp::body( )
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline int SgObjectListExp::listLength()
{ return exprListLength(thellnd); }

inline SgSymbol * SgObjectListExp::symbol(int i)
{
    PTR_LLND tail;
    int len;
    for (len = 0, tail = thellnd; len < i && tail; tail = NODE_OPERAND1(tail), ++len);

    return SymbMapping(NODE_SYMB(tail));
}

inline SgExpression * SgObjectListExp::body(int i) 
{ return LlndMapping( getPositionInExprList(NODE_OPERAND1(thellnd),i)); }


// SgAttributeExp--inlines
inline SgAttributeExp::SgAttributeExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgAttributeExp::SgAttributeExp(int variant):SgExpression(variant)
{}

inline SgAttributeExp::~SgAttributeExp()
{ RemoveFromTableLlnd((void *) this); }


// SgKeywordArgExp--inlines

inline SgKeywordArgExp::SgKeywordArgExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgKeywordArgExp::SgKeywordArgExp(char *argName, SgExpression &exp):SgExpression(KEYWORD_ARG)
{ 
  NODE_OPERAND1(thellnd) = exp.thellnd;
  NODE_OPERAND0(thellnd) = SgKeywordValExp(argName).thellnd;
  NODE_TYPE(thellnd) = NODE_TYPE(exp.thellnd);
}

inline SgKeywordArgExp::SgKeywordArgExp(const char *argName, SgExpression &exp) :SgExpression(KEYWORD_ARG)
{
    NODE_OPERAND1(thellnd) = exp.thellnd;
    NODE_OPERAND0(thellnd) = SgKeywordValExp(argName).thellnd;
    NODE_TYPE(thellnd) = NODE_TYPE(exp.thellnd);
}

inline SgKeywordArgExp::~SgKeywordArgExp()
{ RemoveFromTableLlnd((void *) this); }
  
#if 0 //Kataev N.A. 30.05.2013
inline SgSymbol * SgKeywordArgExp::arg()
{ return SymbMapping(NODE_SYMB(thellnd)); }
#endif

inline SgExpression * SgKeywordArgExp::arg() //Kataev N.A. 30.05.2013
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgKeywordArgExp::value()
{ return LlndMapping(NODE_OPERAND1(thellnd)); } // fix bag: change NODE_OPERAND0 -> NODE_OPERAND1 (Kataev N.A. 30.05.2013)


// SgSubscriptExp--inlines

inline SgSubscriptExp::SgSubscriptExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgSubscriptExp::SgSubscriptExp(SgExpression &lbound, SgExpression &ubound, SgExpression &step):SgExpression(DDOT)
{
  PTR_LLND lb, ub, inc;
  
  lb = lbound.thellnd; ub = ubound.thellnd; inc = step.thellnd;
  if (!isIntegerType(lb) && !isIntegerType(ub) && !isIntegerType(inc))
  {
      Message("Non integer type for SgSubscriptExp", 0);
#ifdef __SPF  
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  
  NODE_OPERAND0(thellnd) = lbound.thellnd;
  NODE_OPERAND1(thellnd) = newExpr(DDOT,NULL,ubound.thellnd, step.thellnd);
}

inline SgSubscriptExp::SgSubscriptExp(SgExpression &lbound, SgExpression &ubound):SgExpression(DDOT)
{
  PTR_LLND lb, ub;
  
  lb = lbound.thellnd; ub = ubound.thellnd;
  if (!isIntegerType(lb) && !isIntegerType(ub))
  {
      Message("Non integer type for SgSubscriptExp", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  
  NODE_OPERAND0(thellnd) = lbound.thellnd;
  NODE_OPERAND1(thellnd) =  ubound.thellnd;
}

inline SgSubscriptExp:: ~SgSubscriptExp()
{ RemoveFromTableLlnd((void *) this);}

// SgUseOnlyExp--inlines

inline SgUseOnlyExp::SgUseOnlyExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgUseOnlyExp::SgUseOnlyExp(SgExpression &onlyList):SgExpression(ONLY_NODE)
{ NODE_OPERAND0(thellnd) = onlyList.thellnd; }

inline SgUseOnlyExp::~SgUseOnlyExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression * SgUseOnlyExp::onlyList()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }


inline SgUseRenameExp::SgUseRenameExp(PTR_LLND ll):SgExpression(ll)
{}

#ifdef NOT_YET_IMPLEMENTED
inline SgUseRenameExp::SgUseRenameExp(SgSymbol &newName, SgSymbol &oldName):SgExpression( RENAME_NODE)
{ SORRY; }
#endif

inline SgUseRenameExp::~SgUseRenameExp()
{ RemoveFromTableLlnd((void *) this); }


#ifdef NOT_YET_IMPLEMENTED
inline SgSymbol *SgUseRenameExp::newName()
{
  SORRY;
  return (SgSymbol *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgSymbol *SgUseRenameExp::oldName()
{
  SORRY;
  return (SgSymbol *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgExpression * SgUseRenameExp::newNameExp()
{
  SORRY;
  return (SgExpression *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgExpression * SgUseRenameExp::oldNameExp()
{
  SORRY;
  return (SgExpression *) NULL;
}
#endif


// SgSpecPairExp--inlines

inline SgSpecPairExp::SgSpecPairExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgSpecPairExp::SgSpecPairExp(SgExpression &arg, SgExpression &value):SgExpression(SPEC_PAIR)
{
  NODE_OPERAND0(thellnd) = arg.thellnd;
  NODE_OPERAND1(thellnd) = value.thellnd;
}

inline SgSpecPairExp::SgSpecPairExp(SgExpression &arg):SgExpression(SPEC_PAIR)
{ NODE_OPERAND0(thellnd) = arg.thellnd; }

inline SgSpecPairExp::SgSpecPairExp(char *arg, char *):SgExpression(SPEC_PAIR)
{
  NODE_OPERAND0(thellnd) = SgKeywordValExp(arg).thellnd;
  NODE_OPERAND1(thellnd) = SgKeywordValExp(arg).thellnd;
}

inline SgSpecPairExp::~SgSpecPairExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression *SgSpecPairExp::arg()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgSpecPairExp::value()
{ return LlndMapping(NODE_OPERAND1(thellnd)); }


// SgIOAccessExp--inlines

inline SgIOAccessExp::SgIOAccessExp(PTR_LLND ll):SgExpression(ll)
{}

// type-checking on bounds needs to be done.
// Float values are legal in some cases. check manual.
inline SgIOAccessExp::SgIOAccessExp(SgSymbol &s, SgExpression lbound, SgExpression ubound, SgExpression step):SgExpression(IOACCESS)
{ 
  NODE_SYMB(thellnd) = s.thesymb;
  NODE_OPERAND0(thellnd) = newExpr(SEQ,NULL, newExpr(DDOT,NULL, lbound.thellnd, ubound.thellnd), step.thellnd);
}

inline SgIOAccessExp::SgIOAccessExp(SgSymbol &s, SgExpression lbound, SgExpression ubound):SgExpression(IOACCESS)
{
  NODE_SYMB(thellnd) = s.thesymb;
  NODE_OPERAND0(thellnd) = newExpr(SEQ,NULL, newExpr(DDOT,NULL, lbound.thellnd, ubound.thellnd), NULL);
}

inline SgIOAccessExp::~SgIOAccessExp()
{ RemoveFromTableLlnd((void *) this); }


// SgImplicitTypExp--inlines

inline SgImplicitTypeExp::SgImplicitTypeExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgImplicitTypeExp::SgImplicitTypeExp(SgType &type, SgExpression &rangeList):SgExpression(IMPL_TYPE)
{
  NODE_TYPE(thellnd) = type.thetype;
  NODE_OPERAND0(thellnd) = rangeList.thellnd;
}

inline SgImplicitTypeExp::~SgImplicitTypeExp()
{ RemoveFromTableLlnd((void *) this);}

inline SgType * SgImplicitTypeExp::type()
{ return TypeMapping(NODE_TYPE(thellnd)); }

inline SgExpression * SgImplicitTypeExp::rangeList()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

#ifdef NOT_YET_IMPLEMENTED
inline char * SgImplicitTypeExp::alphabeticRange()
{
  SORRY;
  return (char *) NULL;
}
#endif


// SgTypeExp--inlines

inline SgTypeExp::SgTypeExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgTypeExp::SgTypeExp(SgType &type):SgExpression(TYPE_OP)
{ NODE_TYPE(thellnd) = type.thetype; }

inline SgTypeExp::~SgTypeExp()
{ RemoveFromTableLlnd((void *) this);}

inline SgType * SgTypeExp::type()
{ return TypeMapping( NODE_TYPE(thellnd)); }


// SgSeqExp--inlines

inline SgSeqExp::SgSeqExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgSeqExp::SgSeqExp(SgExpression &exp1, SgExpression &exp2):SgExpression(SEQ)
{
  NODE_OPERAND0(thellnd) = exp1.thellnd;
  NODE_OPERAND1(thellnd) = exp2.thellnd;
}

inline SgSeqExp::~SgSeqExp()
{ RemoveFromTableLlnd((void *) this);}

inline SgExpression * SgSeqExp::front()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression * SgSeqExp::rear()
{ return LlndMapping(NODE_OPERAND1(thellnd)); }



// SgStringLengthExp--inlines

inline SgStringLengthExp::SgStringLengthExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgStringLengthExp::SgStringLengthExp(SgExpression &length):SgExpression(LEN_OP)
{ NODE_OPERAND0(thellnd) = length.thellnd; }

inline SgStringLengthExp::~SgStringLengthExp()
{ RemoveFromTableLlnd((void *) this);}

inline SgExpression * SgStringLengthExp::length()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }



// SgDefaultExp--inlines

inline SgDefaultExp::SgDefaultExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgDefaultExp::SgDefaultExp():SgExpression(DEFAULT) 
{}

inline SgDefaultExp::~SgDefaultExp()
{ RemoveFromTableLlnd((void *) this); }


// SgLabelRefExp--inlines

inline SgLabelRefExp::SgLabelRefExp(PTR_LLND ll):SgExpression(ll)
{}

inline SgLabelRefExp::SgLabelRefExp(SgLabel &label):SgExpression(LABEL_REF)
{ NODE_LABEL(thellnd) = label.thelabel; }

inline SgLabelRefExp::~SgLabelRefExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgLabel * SgLabelRefExp::label()
{ return LabelMapping(NODE_LABEL(thellnd)); }


// SgProgHedrStmt--inlines


inline SgProgHedrStmt::SgProgHedrStmt(PTR_BFND bif):SgStatement(bif)
{}

inline SgProgHedrStmt::SgProgHedrStmt(int variant):SgStatement(variant)
{ addControlEndToStmt(thebif); }

inline SgProgHedrStmt::SgProgHedrStmt(SgSymbol &name, SgStatement &Body):SgStatement(PROG_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
  insertBfndListIn(Body.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

inline SgProgHedrStmt::SgProgHedrStmt(SgSymbol &name):SgStatement(PROG_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
  addControlEndToStmt(thebif);
}

inline SgProgHedrStmt::SgProgHedrStmt(char *name):SgStatement(PROG_HEDR)
{
  SgSymbol *proc;
  proc = new SgSymbol(PROGRAM_NAME, name); 
  SYMB_SCOPE(proc->thesymb) = PROJ_FIRST_BIF();
  SYMB_TYPE(proc->thesymb) = GetAtomicType(DEFAULT);
  BIF_SYMB(thebif) = proc->thesymb;
  addControlEndToStmt(thebif);
}

inline SgSymbol & SgProgHedrStmt::name()
{
  PTR_SYMB symb;
  SgSymbol *pt = NULL;
  symb = BIF_SYMB(thebif);
  if (!symb)
  {
      Message("The bif has no symbol", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
  else 
    {
      pt = GetMappingInTableForSymbol(symb);
      if (!pt)
        pt = new SgSymbol(symb);      
    }
  return *pt;
}     
   
inline void SgProgHedrStmt::setName(SgSymbol &symbol)
{ BIF_SYMB(thebif) = symbol.thesymb; }

#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::numberOfFunctionsCalled()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgSymbol * SgProgHedrStmt::calledFunction(int i)
{
  SORRY;
  return (SgSymbol *) NULL;
}
#endif

inline int SgProgHedrStmt::numberOfStmtFunctions()
{ return countInStmtNode1(thebif, STMTFN_STAT); }

inline SgStatement * SgProgHedrStmt::statementFunc(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif, STMTFN_STAT, i)); }

inline int SgProgHedrStmt::numberOfEntryPoints()
{ return countInStmtNode1(thebif, ENTRY_STAT); }

inline SgStatement * SgProgHedrStmt::entryPoint(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif, ENTRY_STAT, i)); }

inline int SgProgHedrStmt::numberOfParameters()
{ return lenghtOfParamList(BIF_SYMB(thebif)); }

inline SgSymbol * SgProgHedrStmt::parameter(int i)
{
  PTR_SYMB symb;
  symb = GetThParam(BIF_SYMB(thebif),i);
  return SymbMapping(symb);
}

        
#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::numberOfSpecificationStmts()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::numberOfExecutionStmts()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgStatement * SgProgHedrStmt::specificationStmt(int i)
{
  SORRY;
  return (SgStatement *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgStatement * SgProgHedrStmt::executionStmt(int i)
{
  SORRY;
  return (SgStatement *) NULL;
}
#endif
 
inline int SgProgHedrStmt::numberOfInternalFunctionsDefined()
{ return countInStmtNode1(thebif, FUNC_HEDR); }

inline int SgProgHedrStmt::numberOfInternalSubroutinesDefined()
{ return countInStmtNode1(thebif, PROC_HEDR); }

inline int SgProgHedrStmt::numberOfInternalSubProgramsDefined()
{
  return (countInStmtNode1(thebif, FUNC_HEDR) +
          countInStmtNode1(thebif, PROC_HEDR)) ;
}

#ifdef NOT_YET_IMPLEMENTED
inline SgStatement * SgProgHedrStmt::internalSubProgram(int i)
{
  SORRY;
  return (SgStatement *) NULL;
}
#endif

inline SgStatement * SgProgHedrStmt::internalFunction(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif, FUNC_HEDR, i)); }

inline SgStatement * SgProgHedrStmt::internalSubroutine(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif, PROC_HEDR, i)); }


#ifdef NOT_YET_IMPLEMENTED
SgSymbol &addVariable(SgType &T, char *name)
{
  SORRY;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
//add a declaration for new variable
SgStatement &addCommonBlock(char *blockname, int noOfVars,
                            SgSymbol *Vars)
{
  SORRY;
}
#endif
        
#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::isSymbolInScope(SgSymbol &symbol)
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::isSymbolDeclaredHere(SgSymbol &symbol)
{
  SORRY;
  return 0;
}
#endif

// global analysis data

#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::numberOfVarsUsed()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgExpression * SgProgHedrStmt::varsUsed(int i)
{
  SORRY;
  return (SgExpression *) NULL;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline int SgProgHedrStmt::numberofVarsMod()
{
  SORRY;
  return 0;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline SgExpression *varsMod(int i)
{
  SORRY;
  return (SgExpression *) NULL;
}
#endif

inline SgProgHedrStmt::~SgProgHedrStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgProcHedrStmt--inlines

inline SgProcHedrStmt::SgProcHedrStmt(int variant):SgProgHedrStmt(variant)
{ }

inline SgProcHedrStmt::SgProcHedrStmt(SgSymbol &name, SgStatement &Body):SgProgHedrStmt(PROC_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
    if(LibClanguage())
    {
        printf("SgProcHedrStmt: not a valid C construct. use FuncHedr\n");
	}
  name.thesymb->entry.proc_decl.proc_hedr = thebif;
  insertBfndListIn(Body.thebif,thebif,thebif);
}

inline SgProcHedrStmt::SgProcHedrStmt(SgSymbol &name):SgProgHedrStmt(PROC_HEDR)
{ BIF_SYMB(thebif) = name.thesymb;  
  name.thesymb->entry.proc_decl.proc_hedr = thebif;
    if(LibClanguage()){
        printf("SgProcHedrStmt: not a valid C construct. use FuncHedr\n");
	}
}

inline SgProcHedrStmt::SgProcHedrStmt(const char *name):SgProgHedrStmt(PROC_HEDR)
{
  SgSymbol *proc;
  proc = new SgSymbol(PROCEDURE_NAME, name); 
  SYMB_SCOPE(proc->thesymb) = PROJ_FIRST_BIF();
  SYMB_TYPE(proc->thesymb) = GetAtomicType(DEFAULT);
  BIF_SYMB(thebif) = proc->thesymb;
  proc->thesymb->entry.proc_decl.proc_hedr = thebif;
    if(LibClanguage()){
        printf("SgProcHedrStmt: not a valid C construct. use FuncHedr\n");
	}
        
}

inline void SgProcHedrStmt::AddArg(SgExpression &arg)
{
  PTR_SYMB symb;
  PTR_LLND ll;

  if(LibFortranlanguage())
 	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd);
  else{ 
        ll = BIF_LL1(thebif);
        ll = NODE_OPERAND0(ll);
	NODE_OPERAND0(ll) = addToExprList(NODE_OPERAND0(ll),arg.thellnd);
      }
  ll = giveLlSymbInDeclList(arg.thellnd);
  if (ll && (symb= NODE_SYMB(ll)))
    {
      appendSymbToArgList(BIF_SYMB(thebif),symb); 
      SYMB_SCOPE(symb) = thebif;
      if(LibFortranlanguage())
            declareAVar(symb,thebif);
    } 
  else
  {
      Message("bad symbol in SgProcHedrStmt::AddArg", 0);
#ifdef __SPF  
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
}


#ifdef NOT_YET_IMPLEMENTED
inline int SgProcHedrStmt::isRecursive()  // 1 if recursive.
{ 
  SORRY;
  return 0;
  //return isAttributeSet(BIF_SYMB(thebif), RECURSIVE_BIT); 
}
#endif

inline int SgProcHedrStmt::numberOfEntryPoints()
{ return countInStmtNode1(thebif,ENTRY_STAT); }

inline SgStatement * SgProcHedrStmt::entryPoint(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif,ENTRY_STAT,i)); }

// this is incorrect. Takes only subroutines calls into account.
// Should be modified to take function calls into account too.
inline int SgProcHedrStmt::numberOfCalls()
{ return countInStmtNode1(thebif,PROC_STAT); }

inline SgStatement * SgProcHedrStmt::call(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif,PROC_STAT,i)); }

inline SgProcHedrStmt::~SgProcHedrStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgProsHedrStmt--inlines

inline SgProsHedrStmt::SgProsHedrStmt():SgProgHedrStmt(PROS_HEDR)
{}

inline SgProsHedrStmt::SgProsHedrStmt(SgSymbol &name, SgStatement &Body)
                      :SgProgHedrStmt(PROS_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
  insertBfndListIn(Body.thebif,thebif,thebif);
}

inline SgProsHedrStmt::SgProsHedrStmt(SgSymbol &name):SgProgHedrStmt(PROS_HEDR)
{ BIF_SYMB(thebif) = name.thesymb; }

inline SgProsHedrStmt::SgProsHedrStmt(char *name):SgProgHedrStmt(PROS_HEDR)
{
  SgSymbol *pros;
  pros = new SgSymbol(PROCESS_NAME, name); 
  SYMB_SCOPE(pros->thesymb) = PROJ_FIRST_BIF();
  SYMB_TYPE(pros->thesymb) = GetAtomicType(DEFAULT);
  BIF_SYMB(thebif) = pros->thesymb;
}

inline void SgProsHedrStmt::AddArg(SgExpression &arg)
{
  PTR_SYMB symb;
  PTR_LLND ll;

  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd);
  ll = giveLlSymbInDeclList(arg.thellnd);
  if (ll && (symb= NODE_SYMB(ll)))
    {
      appendSymbToArgList(BIF_SYMB(thebif),symb); 
      SYMB_SCOPE(symb) = thebif;
      declareAVar(symb,thebif);
    } 
  else
  {
      Message("Pb in SgProsHedrStmt::AddArg", 0);
#ifdef __SPF   
      {
          char buf[512];
          sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
          addToGlobalBufferAndPrint(buf);
      }
      throw -1;
#endif
  }
}

inline int SgProsHedrStmt::numberOfCalls()
{ return countInStmtNode1(thebif,PROS_STAT); }

inline SgStatement * SgProsHedrStmt::call(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif,PROS_STAT,i)); }

inline SgProsHedrStmt::~SgProsHedrStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgFuncHedrStmt--inlines
inline SgFuncHedrStmt::SgFuncHedrStmt(SgSymbol &name, SgStatement &Body): 
   SgProcHedrStmt(FUNC_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
  if(LibClanguage()){
	SgExpression *fref = new SgExpression(FUNCTION_REF);
	fref->setSymbol(name);
  	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),fref->thellnd);
	}
  SYMB_FUNC_HEDR(name.thesymb) = thebif;
  insertBfndListIn(Body.thebif,thebif,thebif);
}

inline SgFuncHedrStmt::SgFuncHedrStmt(SgSymbol &name, SgType &type, SgStatement &Body): SgProcHedrStmt(FUNC_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
  if(LibClanguage()){
	SgExpression *fref = new SgExpression(FUNCTION_REF);
	fref->setSymbol(name);
  	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),fref->thellnd);
	}
  SYMB_TYPE(BIF_SYMB(thebif)) = type.thetype;
  SYMB_FUNC_HEDR(name.thesymb) = thebif;
  insertBfndListIn(Body.thebif,thebif,thebif);
}

inline SgFuncHedrStmt::SgFuncHedrStmt(SgSymbol &name, SgSymbol &resultName, 
                  SgType &type, SgStatement &Body): SgProcHedrStmt(FUNC_HEDR)
{
  BIF_SYMB(thebif) = name.thesymb;
  if(LibClanguage()){
	SgExpression *fref = new SgExpression(FUNCTION_REF);
	fref->setSymbol(name);
  	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),fref->thellnd);
	}
  SYMB_TYPE(BIF_SYMB(thebif)) = type.thetype;
  SYMB_DECLARED_NAME(BIF_SYMB(thebif)) = resultName.thesymb;
  SYMB_FUNC_HEDR(name.thesymb) = thebif;
  insertBfndListIn(Body.thebif,thebif,thebif);
}

inline SgFuncHedrStmt::SgFuncHedrStmt(SgSymbol &name): SgProcHedrStmt(FUNC_HEDR)
{ BIF_SYMB(thebif) = name.thesymb; 
  if(LibClanguage()){
	SgExpression *fref = new SgExpression(FUNCTION_REF);
	fref->setSymbol(name);
  	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),fref->thellnd);
	}
}

inline SgFuncHedrStmt::SgFuncHedrStmt(SgSymbol &name, SgExpression *exp): SgProcHedrStmt(FUNC_HEDR)
{ 
  BIF_SYMB(thebif) = name.thesymb; 
  if (exp)
    BIF_LL1(thebif) = exp->thellnd;
  SYMB_FUNC_HEDR(name.thesymb) = thebif;
}

inline SgFuncHedrStmt::SgFuncHedrStmt(char *name): SgProcHedrStmt(FUNC_HEDR)
{
  SgSymbol *proc;
  proc = new SgSymbol(FUNCTION_NAME, name); 
  if(LibClanguage()){
	SgExpression *fref = new SgExpression(FUNCTION_REF);
	fref->setSymbol(*proc);
  	BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),fref->thellnd);
	}
  SYMB_SCOPE(proc->thesymb) = PROJ_FIRST_BIF();
  SYMB_TYPE(proc->thesymb) = GetAtomicType(T_INT);
  SYMB_FUNC_HEDR(proc->thesymb) = thebif;
  BIF_SYMB(thebif) = proc->thesymb;
}

inline SgFuncHedrStmt::~SgFuncHedrStmt()
{ RemoveFromTableBfnd((void *) this); } 

inline SgType * SgFuncHedrStmt::returnedType()
{
  PTR_TYPE ty = NULL;
  if (BIF_SYMB(thebif))
    ty = SYMB_TYPE(BIF_SYMB(thebif));
  return TypeMapping(ty);
}

inline void SgFuncHedrStmt::setReturnedType(SgType &type)
{
  if (BIF_SYMB(thebif))
    SYMB_TYPE(BIF_SYMB(thebif)) = type.thetype;
}

inline SgSymbol * SgFuncHedrStmt::resultName()  // name of result variable.
{
  SgSymbol *x;

  if (BIF_SYMB(thebif) == SMNULL)
    x = NULL;
  else
    x = SymbMapping(SYMB_DECLARED_NAME(BIF_SYMB(thebif)));

  return x;
}

// Use Message to flag error and type it void?
inline int  SgFuncHedrStmt::setResultName(SgSymbol &symbol)// set name of result variable.
{
  int x;
  if (BIF_SYMB(thebif) == SMNULL)
    x = 0;
  else {
    SYMB_DECLARED_NAME(BIF_SYMB(thebif)) = symbol.thesymb;
    x = 1;
  }
  return x;
}



// SgClassStmt--inlines

inline SgClassStmt::SgClassStmt(int variant):SgStatement(variant)
{}

inline SgClassStmt::SgClassStmt(SgSymbol &name):SgStatement(CLASS_DECL)
{ BIF_SYMB(thebif) = name.thesymb; }

inline SgClassStmt::~SgClassStmt()
{ RemoveFromTableBfnd((void *) this); }

inline int SgClassStmt::numberOfSuperClasses()
{ return exprListLength(BIF_LL2(thebif)); }

inline SgSymbol * SgClassStmt::name()
{ return SymbMapping(BIF_SYMB(thebif)); }

inline SgSymbol * SgClassStmt::superClass(int i)
{
  PTR_LLND pt;
  SgSymbol *x;

  pt = getPositionInExprList(BIF_LL2(thebif),i);
  pt = giveLlSymbInDeclList(pt);
  if (pt)
    x = SymbMapping(NODE_SYMB(pt));
  else
    x = SymbMapping(NULL);

  return x;
}

inline void SgClassStmt::setSuperClass(int i, SgSymbol &symb)
{
  PTR_LLND pt;
  
  if (!BIF_LL2(thebif))
    {
      BIF_LL2(thebif) = addToExprList(BIF_LL2(thebif),newExpr(VAR_REF,NULL,symb.thesymb));
    } 
  else
    {
      pt = getPositionInExprList(BIF_LL2(thebif),i);
      pt = giveLlSymbInDeclList(pt);
      if (pt)
        NODE_SYMB(pt) = symb.thesymb;
      else
        BIF_LL2(thebif) = addToExprList(BIF_LL2(thebif),newExpr(VAR_REF,NULL,symb.thesymb));
    }
}


// SgStructStmt--inlines

inline SgStructStmt::SgStructStmt():SgClassStmt(STRUCT_DECL)
{}

inline SgStructStmt::SgStructStmt(SgSymbol &name):SgClassStmt(name)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_CODE(thebif) = STRUCT_DECL;
}

inline SgStructStmt::~SgStructStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgUnionStmt--inlines
// consider like a class.
inline SgUnionStmt::SgUnionStmt():SgClassStmt(UNION_DECL)
{}

inline SgUnionStmt::SgUnionStmt(SgSymbol &name):SgClassStmt(name)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_CODE(thebif) = UNION_DECL;
}

inline SgUnionStmt::~SgUnionStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgEnumStmt--inlines
// consider like a class.
inline SgEnumStmt::SgEnumStmt():SgClassStmt(ENUM_DECL)
{}

inline SgEnumStmt::SgEnumStmt(SgSymbol &name):SgClassStmt(name)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_CODE(thebif) = ENUM_DECL;
}

inline SgEnumStmt::~SgEnumStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgCollectionStmt--inlines

inline SgCollectionStmt::SgCollectionStmt():SgClassStmt(COLLECTION_DECL)
{}

inline SgCollectionStmt::SgCollectionStmt(SgSymbol &name):SgClassStmt(name)
{ BIF_CODE(thebif) = COLLECTION_DECL; }

inline SgCollectionStmt::~SgCollectionStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgStatement * SgCollectionStmt::firstElementMethod()
{ return BfndMapping(LibfirstElementMethod(thebif)); }


// SgBasicBlockStmt--inlines
inline SgBasicBlockStmt::SgBasicBlockStmt(): SgStatement(BASIC_BLOCK)
{}

inline SgBasicBlockStmt::~SgBasicBlockStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgForStmt--inlines
inline SgForStmt::SgForStmt(SgSymbol &do_var, SgExpression &start, SgExpression &end,
                            SgExpression &step, SgStatement &body):SgStatement(FOR_NODE)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_SYMB(thebif) = do_var.thesymb;
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(end.thellnd),start.thellnd,end.thellnd);
      BIF_LL2(thebif) = step.thellnd;
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}

inline SgForStmt::SgForStmt(SgSymbol *do_var, SgExpression *start, SgExpression *end,
            SgExpression *step, SgStatement *body):SgStatement(FOR_NODE)
{
  if (CurrentProject->Fortranlanguage())
    {
      if (do_var)
        BIF_SYMB(thebif) = do_var->thesymb;
      if (start && end)
        BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(end->thellnd),start->thellnd,end->thellnd);
      if (step)
        BIF_LL2(thebif) = step->thellnd;
      if (body)
        insertBfndListIn(body->thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}

inline SgForStmt::SgForStmt(SgSymbol &do_var, SgExpression &start, SgExpression &end
                            , SgStatement &body):SgStatement(FOR_NODE)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_SYMB(thebif) = do_var.thesymb;
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(end.thellnd),start.thellnd,end.thellnd);
      BIF_LL2(thebif) = NULL;
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}
// For C Statement;
// added by Kolganov A.S. 24.10.2013
inline SgForStmt::SgForStmt(SgExpression *start, SgExpression *end, SgExpression *step, SgStatement *body): SgStatement(FOR_NODE)
{
	if(start)
		BIF_LL1(thebif) = start->thellnd;
	if(end)
        BIF_LL2(thebif) = end->thellnd;
	if(step)
        BIF_LL3(thebif) = step->thellnd;

	if(body)
		insertBfndListIn(body->thebif, thebif, thebif);
    addControlEndToStmt(thebif);    
}

inline SgForStmt::SgForStmt(SgExpression &start, SgExpression &end,
                            SgExpression &step, SgStatement &body):SgStatement(FOR_NODE)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(end.thellnd),start.thellnd,end.thellnd);
      BIF_LL2(thebif) = step.thellnd;
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        BIF_LL1(thebif) = start.thellnd;
        BIF_LL2(thebif) = end.thellnd;
        BIF_LL3(thebif) = step.thellnd;
        insertBfndListIn(body.thebif,thebif,thebif);
        addControlEndToStmt(thebif);
      }
}

inline void SgForStmt::setDoName(SgSymbol &doName)
{ BIF_SYMB(thebif) = doName.thesymb; }  // sets the name of the loop (for F90.)

#if __SPF
inline SgSymbol* SgForStmt::doName()
{
    return symbol();
}
#else
inline SgSymbol SgForStmt::doName()
{
    return SgSymbol(BIF_SYMB(thebif));   // the name of the loop (for F90.)
}
#endif

inline SgExpression * SgForStmt::start()
{       
  SgExpression *x;

  if (CurrentProject->Fortranlanguage())
    {
      if ((BIF_LL1(thebif) != LLNULL) &&
          (NODE_CODE(BIF_LL1(thebif)) == DDOT))
         x = LlndMapping(NODE_OPERAND0(BIF_LL1(thebif)));
      else {
        x = NULL;
        SORRY;
      }
    } 
  else
      x = LlndMapping(BIF_LL1(thebif));

  return x;
}

inline void SgForStmt::setStart(SgExpression &lbound)
{

  if (CurrentProject->Fortranlanguage())
    {
      if ((BIF_LL1(thebif) != LLNULL) &&
          (NODE_CODE(BIF_LL1(thebif)) == DDOT))
        {
          NODE_OPERAND0(BIF_LL1(thebif)) =  lbound.thellnd;
        }
      else
        {
          BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(lbound.thellnd),lbound.thellnd,NULL);
        }
    }
  else 
    {
      BIF_LL1(thebif) = lbound.thellnd;
    }
}

inline SgExpression * SgForStmt::end()
{
  SgExpression *x;

  if (CurrentProject->Fortranlanguage())
    {
      if ((BIF_LL1(thebif) != LLNULL) &&
          (NODE_CODE(BIF_LL1(thebif)) == DDOT))
        x = LlndMapping(NODE_OPERAND1(BIF_LL1(thebif)));
      else {
        x = NULL;
        SORRY;
      }
    } 
  else  /* BW, change contributed by Michael Golden */
    {
      if (BIF_LL2(thebif) == LLNULL)
        x = NULL;
      else
        x = LlndMapping(BIF_LL2(thebif));
    }
  return x;
}

inline void SgForStmt::setEnd(SgExpression &ubound)
{
  if (CurrentProject->Fortranlanguage())
    {
      if ((BIF_LL1(thebif) != LLNULL) &&
          (NODE_CODE(BIF_LL1(thebif)) == DDOT))
        NODE_OPERAND1(BIF_LL1(thebif)) =  ubound.thellnd;
      else
        {
          BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(ubound.thellnd),NULL,ubound.thellnd);
        }
    }
  else 
    {
      BIF_LL2(thebif) = ubound.thellnd;
    }
}


inline SgLabel * SgForStmt::endOfLoop()
    { return LabelMapping(BIF_LABEL_USE(thebif)); }

inline SgExpression * SgForStmt::step()
{
  SgExpression *x;
  if (CurrentProject->Fortranlanguage())
    {
      x = LlndMapping(BIF_LL2(thebif));
    } 
  else  /* BW, change contributed by Michael Golden */
    {
      if (BIF_LL3(thebif) == LLNULL)
        x =  NULL;
      else
        x = LlndMapping(BIF_LL3(thebif));
    }

  return x;
}

inline void SgForStmt::setStep(SgExpression &step)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_LL2(thebif) = step.thellnd;
    }
  else
    {
      BIF_LL3(thebif) = step.thellnd;
    }
}

//added by Kolganov A.S. 27.10.2020
inline void SgForStmt::interchangeNestedLoops(SgForStmt* loop)
{
    std::swap(BIF_LL1(thebif), BIF_LL1(loop->thebif));
    std::swap(BIF_LL2(thebif), BIF_LL2(loop->thebif));
    std::swap(BIF_LL3(thebif), BIF_LL3(loop->thebif));
    std::swap(BIF_SYMB(thebif), BIF_SYMB(loop->thebif));
    std::swap(BIF_LABEL(thebif), BIF_LABEL(loop->thebif));
}

inline SgStatement * SgForStmt::body()
{
  PTR_BFND bif =NULL;
  
  if (BIF_BLOB1(thebif)) 
    bif = BLOB_VALUE(BIF_BLOB1(thebif));
            
  return BfndMapping(bif);
}

// s is assumed to terminate with a
//   control end statement.
inline void SgForStmt::set_body(SgStatement &s)
{
  BIF_BLOB1(thebif) = NULL;
  insertBfndListIn(s.thebif,thebif,thebif);
}

// False if the loop is not a prefect nest
// else returns size of the loop nest

inline int SgForStmt::isPerfectLoopNest()
{ return LibperfectlyNested (thebif); }

// returns inner nested loop
inline SgStatement * SgForStmt::getNextLoop()
{ return BfndMapping(LibgetNextNestedLoop (thebif)); }

// returns outer nested loop
inline SgStatement * SgForStmt::getPreviousLoop()
{ return BfndMapping(LibgetPreviousNestedLoop (thebif));  }

// returns innermost nested loop
inline SgStatement * SgForStmt::getInnermostLoop()
{ return BfndMapping(LibgetInnermostLoop (thebif)); }

// TRUE if the loop ends with an Enddo
inline int SgForStmt::isEnddoLoop()
{ return LibisEnddoLoop (thebif); }

// Convert the loop into a Good loop.
inline int SgForStmt::convertLoop()
{ return convertToEnddoLoop (thebif); }

inline SgForStmt::~SgForStmt()
{ RemoveFromTableBfnd((void *) this);}



// SgProcessDoStmt--inlines
inline SgProcessDoStmt::SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                                        SgExpression &end, SgExpression &step,
                                        SgLabel &endofloop, SgStatement &body)
                       :SgStatement(PROCESS_DO_STAT)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_SYMB(thebif) = do_var.thesymb;
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(start.thellnd),start.thellnd,end.thellnd);
      BIF_LL2(thebif) = step.thellnd;
      BIF_LABEL_USE(thebif) = endofloop.thelabel;
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}

inline SgProcessDoStmt::SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                                        SgExpression &end, SgLabel &endofloop,
                                        SgStatement &body)
                       :SgStatement(PROCESS_DO_STAT)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_SYMB(thebif) = do_var.thesymb;
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(start.thellnd),start.thellnd,end.
thellnd);
      BIF_LABEL_USE(thebif) = endofloop.thelabel;
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}

inline SgProcessDoStmt::SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                                        SgExpression &end, SgExpression &step,
                                        SgStatement &body)
                       :SgStatement(PROCESS_DO_STAT)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_SYMB(thebif) = do_var.thesymb;
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(start.thellnd),start.thellnd,end.
thellnd);
      BIF_LL2(thebif) = step.thellnd;
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}

inline SgProcessDoStmt::SgProcessDoStmt(SgSymbol &do_var, SgExpression &start,
                                        SgExpression &end, SgStatement &body)
                       :SgStatement(PROCESS_DO_STAT)
{
  if (CurrentProject->Fortranlanguage())
    {
      BIF_SYMB(thebif) = do_var.thesymb;
      BIF_LL1(thebif) = newExpr(DDOT,NODE_TYPE(start.thellnd),start.thellnd,end.
thellnd);
      insertBfndListIn(body.thebif,thebif,thebif);
      addControlEndToStmt(thebif);
    } else
      {
        SORRY;
      }
}


inline void SgProcessDoStmt::setDoName(SgSymbol &doName)
{ BIF_SYMB(thebif) = doName.thesymb; }

/*
inline SgSymbol SgProcessDoStmt::doName() 
{ return SgSymbol(BIF_SYMB(thebif)); } 
*/

inline SgExpression * SgProcessDoStmt::start()
{       
  SgExpression *x;

  if (CurrentProject->Fortranlanguage())
    {
      if ((BIF_LL1(thebif) != LLNULL) &&
          (NODE_CODE(BIF_LL1(thebif)) == DDOT))
         x = LlndMapping(NODE_OPERAND0(BIF_LL1(thebif)));
      else {
        x = NULL;
        SORRY;
      }
    } 
  else {
    x = NULL;
    SORRY;
  }

  return x;
}

inline SgExpression * SgProcessDoStmt::end()
{
  SgExpression *x;

  if (CurrentProject->Fortranlanguage())
    {
      if ((BIF_LL1(thebif) != LLNULL) &&
          (NODE_CODE(BIF_LL1(thebif)) == DDOT))
        x = LlndMapping(NODE_OPERAND1(BIF_LL1(thebif)));
      else {
        x = NULL;
        SORRY;
      }
    } 
  else {
    x = NULL;
    SORRY;
    }

  return x;
}

inline SgExpression * SgProcessDoStmt::step()
{
  SgExpression *x;
  if (CurrentProject->Fortranlanguage())
    {
      x = LlndMapping(BIF_LL2(thebif));
    } 
  else {
    x = NULL;
    SORRY;
    };

  return x;
}

inline SgLabel * SgProcessDoStmt::endOfLoop()
{ return LabelMapping(BIF_LABEL_USE(thebif)); }

inline SgStatement * SgProcessDoStmt::body()
{
  PTR_BFND bif =NULL;
  
  if (BIF_BLOB1(thebif)) 
    bif = BLOB_VALUE(BIF_BLOB1(thebif));
            
  return BfndMapping(bif);
}

// s is assumed to terminate with a
//   control end statement.
inline void SgProcessDoStmt::set_body(SgStatement &s)
{
  BIF_BLOB1(thebif) = NULL;
  insertBfndListIn(s.thebif,thebif,thebif);
}

// False if the loop is not a prefect nest
// else returns size of the loop nest

inline int SgProcessDoStmt::isPerfectLoopNest()
{ return LibperfectlyNested (thebif); }

// returns inner nested loop
inline SgStatement * SgProcessDoStmt::getNextLoop()
{ return BfndMapping(LibgetNextNestedLoop (thebif)); }

// returns outer nested loop
inline SgStatement * SgProcessDoStmt::getPreviousLoop()
{ return BfndMapping(LibgetPreviousNestedLoop (thebif));  }

// returns innermost nested loop
inline SgStatement * SgProcessDoStmt::getInnermostLoop()
{ return BfndMapping(LibgetInnermostLoop (thebif)); }

// TRUE if the loop ends with an Enddo
inline int SgProcessDoStmt::isEnddoLoop()
{ return LibisEnddoLoop (thebif); }

// Convert the loop into a Good loop.
inline int SgProcessDoStmt::convertLoop()
{ return convertToEnddoLoop (thebif); }

inline SgProcessDoStmt::~SgProcessDoStmt()
{ RemoveFromTableBfnd((void *) this);}



// SgWhileStmt--inlines

inline SgWhileStmt::SgWhileStmt(int variant):SgStatement(variant)
{}

inline SgWhileStmt::SgWhileStmt(SgExpression &cond, SgStatement &body):SgStatement(WHILE_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(body.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

//added by A.S.Kolganov 08.04.2015
inline SgWhileStmt::SgWhileStmt(SgExpression *cond, SgStatement *body) :SgStatement(WHILE_NODE)
{
    if (cond)
        BIF_LL1(thebif) = cond->thellnd;
    if (body)
        insertBfndListIn(body->thebif, thebif, thebif);
    addControlEndToStmt(thebif);
}

// the while test
inline SgExpression * SgWhileStmt::conditional()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgWhileStmt::replaceBody(SgStatement &s)
{
  BIF_BLOB1(thebif) = NULL;
  insertBfndListIn(s.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

// added by A.V.Rakov 16.03.2015
inline SgStatement * SgWhileStmt::body()
{
	PTR_BFND bif = NULL;

	if (BIF_BLOB1(thebif))
		bif = BLOB_VALUE(BIF_BLOB1(thebif));

	return BfndMapping(bif);
}

inline SgWhileStmt::~SgWhileStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgDoWhileStmt--inlines

inline SgDoWhileStmt::SgDoWhileStmt(SgExpression &cond, SgStatement &body): SgWhileStmt(DO_WHILE_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(body.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

inline SgDoWhileStmt::~SgDoWhileStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgLabel *SgWhileStmt::endOfLoop( )
{ 
	return LabelMapping(BIF_LABEL_USE(thebif)); 
}

// SgLofIfStmt--inlines

inline SgLogIfStmt::SgLogIfStmt(int variant):SgStatement(variant)
{}

inline SgLogIfStmt::SgLogIfStmt(SgExpression &cond, SgStatement &s):SgStatement(LOGIF_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(s.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

inline SgStatement * SgLogIfStmt::body()
{
  PTR_BFND bif =NULL;
  if (BIF_BLOB1(thebif))
    bif = BLOB_VALUE(BIF_BLOB1(thebif));
  return BfndMapping(bif);
}

inline SgExpression * SgLogIfStmt::conditional()
{  return LlndMapping(BIF_LL1(thebif)); }  // the while test

// check if the statement s is a single statement. 
inline void SgLogIfStmt::setBody(SgStatement &s)
{
  BIF_BLOB1(thebif) = NULL;
  insertBfndListIn(s.thebif,thebif,thebif);
}

// this code won't work, since after the addition false
//   clause, it should become SgIfThenElse statement.
inline void SgLogIfStmt::addFalseClause(SgStatement &s)
{
  appendBfndListToList2(s.thebif,thebif);
  addControlEndToList2(thebif);
}

//need a forward definition;
SgIfStmt * isSgIfStmt (SgStatement *pt);

inline SgIfStmt *SgLogIfStmt::convertLogicIf()
{
  LibconvertLogicIf(thebif);
  return isSgIfStmt(this);
}

inline SgLogIfStmt::~SgLogIfStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgIfStmt--inlines

inline SgIfStmt::SgIfStmt(int variant): SgStatement(variant)
{}

// added by A.S.Kolganov 02.07.2014
inline SgIfStmt::SgIfStmt(SgExpression &cond, SgStatement &body, int t) : SgStatement(IF_NODE)
{
	BIF_LL1(thebif) = cond.thellnd;
	if (t == 0) // only false body
		appendBfndListToList2(body.thebif, thebif);
	else if (t == 1) // only true body
		insertBfndListIn(body.thebif, thebif, thebif);
	addControlEndToStmt(thebif);
}
// added by A.S.Kolganov 21.12.2014
inline SgIfStmt::SgIfStmt(SgExpression &cond) : SgStatement(IF_NODE)
{
    BIF_LL1(thebif) = cond.thellnd;
    addControlEndToStmt(thebif);
}

inline SgIfStmt::SgIfStmt(SgExpression &cond, SgStatement &trueBody, SgStatement &falseBody, SgSymbol &construct_name):SgStatement(IF_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  BIF_SYMB(thebif) = construct_name.thesymb;
  insertBfndListIn(trueBody.thebif,thebif,thebif);
  appendBfndListToList2(falseBody.thebif,thebif);
  addControlEndToStmt(thebif);
}

inline SgIfStmt::SgIfStmt(SgExpression &cond, SgStatement &trueBody, SgStatement &falseBody):SgStatement(IF_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(trueBody.thebif,thebif,thebif);
  appendBfndListToList2(falseBody.thebif,thebif);
  addControlEndToStmt(thebif);
}

inline void SgIfStmt::setBodies(SgStatement *trueBody, SgStatement *falseBody)
{
    if (trueBody && falseBody)
    {
        insertBfndListIn(trueBody->thebif, thebif, thebif);
        appendBfndListToList2(falseBody->thebif, thebif);
        addControlEndToStmt(thebif);
    }
    else if (trueBody)
    {        
        insertBfndListIn(trueBody->thebif, thebif, thebif);
        addControlEndToStmt(thebif);
    }
}

inline SgIfStmt::SgIfStmt(SgExpression &cond, SgStatement &trueBody):SgStatement(IF_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(trueBody.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

// the first stmt in the True clause
inline SgStatement * SgIfStmt::trueBody()
{
  PTR_BFND bif = NULL;
  if (BIF_BLOB1(thebif))
    bif = BLOB_VALUE(BIF_BLOB1(thebif));
  return BfndMapping(bif);
}

// SgBlock is needed? 
// i-th stmt in True clause
inline SgStatement * SgIfStmt::trueBody(int i)
{
  PTR_BFND bif =NULL;
  if (BIF_BLOB1(thebif))
    bif = BLOB_VALUE(BIF_BLOB1(thebif));
  return BfndMapping(getStatementNumber(bif,i));
}

// the first stmt in the False
inline SgStatement * SgIfStmt::falseBody()
{
  PTR_BFND bif = NULL;
  if (BIF_BLOB2(thebif))
    bif = BLOB_VALUE(BIF_BLOB2(thebif));
  return BfndMapping(bif);
}

// i-th statement of the body.
inline SgStatement * SgIfStmt::falseBody(int i)
{
  PTR_BFND bif =NULL;
  if (BIF_BLOB2(thebif))
    bif = BLOB_VALUE(BIF_BLOB2(thebif));
  return BfndMapping(getStatementNumber(bif,i));
}

// the while test
inline SgExpression * SgIfStmt::conditional()
{ return LlndMapping(BIF_LL1(thebif)); }

inline SgSymbol * SgIfStmt::construct_name()
{ return SymbMapping(BIF_SYMB(thebif)); }

// new body=s and lex successors.
inline void SgIfStmt::replaceTrueBody(SgStatement &s)
{
  BIF_BLOB1(thebif) = NULL;
  insertBfndListIn(s.thebif,thebif,thebif);
}

// new body=s and lex successors.
inline void SgIfStmt::replaceFalseBody(SgStatement &s)
{
  BIF_BLOB2(thebif) = NULL;
  appendBfndListToList2(s.thebif,thebif);
  addControlEndToList2(thebif);
}

inline SgIfStmt::~SgIfStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgArithIfStmt--inlines

inline SgArithIfStmt::SgArithIfStmt(int variant):SgStatement(variant)
{}

inline SgArithIfStmt::SgArithIfStmt(SgExpression &cond, SgLabel &llabel, SgLabel &elabel, SgLabel &glabel):SgStatement(ARITHIF_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  BIF_LL2(thebif) = addLabelRefToExprList(BIF_LL2(thebif),llabel.thelabel);
  BIF_LL2(thebif) = addLabelRefToExprList(BIF_LL2(thebif),elabel.thelabel);
  BIF_LL2(thebif) = addLabelRefToExprList(BIF_LL2(thebif),glabel.thelabel);
}

inline SgExpression * SgArithIfStmt::conditional()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgArithIfStmt::set_conditional(SgExpression &cond)
{ BIF_LL1(thebif) = cond.thellnd; }

// the <, ==, and > goto labels. in order 0->2.
inline SgExpression * SgArithIfStmt::label(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif),i)); }

#ifdef NOT_YET_IMPLEMENTED
inline void SgArithIfStmt::setLabel(SgLabel &label)
{
  BIF_LL3(thebif) = addLabelRefToExprList(BIF_LL3(thebif) , label.thelabel);
  SORRY;
}
#endif

inline SgArithIfStmt::~SgArithIfStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgWhereStmt--inlines

inline SgWhereStmt::SgWhereStmt(SgExpression &cond, SgStatement &body):SgLogIfStmt(WHERE_NODE)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(body.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

inline SgWhereStmt::~SgWhereStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgWhereBlockStmt--inlines

inline SgWhereBlockStmt::SgWhereBlockStmt(SgExpression &cond, SgStatement &trueBody, SgStatement &falseBody):SgIfStmt(WHERE_BLOCK_STMT)
{
  BIF_LL1(thebif) = cond.thellnd;
  insertBfndListIn(trueBody.thebif,thebif,thebif);
  appendBfndListToList2(falseBody.thebif,thebif);
  // appendBfndListToList2 does not update BIF_ NEXT...
    addControlEndToList2(thebif);
}

inline SgWhereBlockStmt::~SgWhereBlockStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgSwitchStmt--inlines

inline SgSwitchStmt::SgSwitchStmt(SgExpression &selector, SgStatement &caseOptionList,
                                  SgSymbol &constructName):SgStatement(SWITCH_NODE)
{
  BIF_SYMB(thebif) = constructName.thesymb;
  BIF_LL1(thebif) = selector.thellnd;
  insertBfndListIn(caseOptionList.thebif,thebif,thebif);
}

// added by A.V.Rakov 16.03.2015
inline SgSwitchStmt::SgSwitchStmt(SgExpression &selector, SgStatement &caseOptionList) :SgStatement(SWITCH_NODE)
{
	BIF_LL1(thebif) = selector.thellnd;
	insertBfndListIn(caseOptionList.thebif, thebif, thebif);
}

// added by A.S. Kolganov 14.04.2015
inline SgSwitchStmt::SgSwitchStmt(SgExpression &selector) :SgStatement(SWITCH_NODE)
{
    BIF_LL1(thebif) = selector.thellnd;
}

inline SgSwitchStmt::~SgSwitchStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgExpression * SgSwitchStmt::selector()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void  SgSwitchStmt::setSelector(SgExpression &cond)
{ BIF_LL1(thebif) = cond.thellnd; }

// the number of cases
inline int SgSwitchStmt::numberOfCaseOptions()
{ return countInStmtNode1(thebif,CASE_NODE); }

// i-th case block
inline SgStatement * SgSwitchStmt::caseOption(int i)
{ return BfndMapping(GetcountInStmtNode1(thebif,CASE_NODE,i)); }

// added by A.V.Rakov 16.03.2015
inline SgStatement * SgSwitchStmt::defOption()
{ return BfndMapping(GetcountInStmtNode1(thebif, DEFAULT_NODE, 0)); }
inline void SgSwitchStmt::addCaseOption(SgStatement &caseOption)
{ insertBfndListIn(caseOption.thebif,thebif,thebif); }

#if 0 
// extractBifSectionBetween not defined
inline void  SgSwitchStmt::deleteCaseOption(int i)
{
  PTR_BFND pt;
  if ( pt = GetcountInStmtNode1(thebif,CASE_NODE,i))
    extractBifSectionBetween(pt,getLastNodeOfStmt(pt));
}
#endif


// SgCaseOptionStmt--inlines

inline SgCaseOptionStmt::SgCaseOptionStmt(SgExpression &caseRangeList, SgStatement &body) : SgStatement(CASE_NODE)
{
  BIF_LL1(thebif) = caseRangeList.thellnd;
  insertBfndListIn(body.thebif, thebif, thebif);
  addControlEndToStmt(thebif);
}

inline SgCaseOptionStmt::SgCaseOptionStmt(SgExpression &caseRangeList, SgStatement &body, 
                                          SgSymbol &constructName):SgStatement(CASE_NODE)
{
  BIF_SYMB(thebif) = constructName.thesymb;
  BIF_LL1(thebif) = caseRangeList.thellnd;
  insertBfndListIn(body.thebif,thebif,thebif);
  addControlEndToStmt(thebif);
}

inline SgCaseOptionStmt::SgCaseOptionStmt(SgExpression &caseRangeList) :SgStatement(CASE_NODE)
{
	BIF_LL1(thebif) = caseRangeList.thellnd;
	addControlEndToStmt(thebif);
}

inline SgCaseOptionStmt::~SgCaseOptionStmt()
{ RemoveFromTableBfnd((void *) this);}

inline SgExpression * SgCaseOptionStmt::caseRangeList()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgCaseOptionStmt::setCaseRangeList(SgExpression &caseRangeList)
{ BIF_LL1(thebif) = caseRangeList.thellnd; }

inline SgExpression * SgCaseOptionStmt::caseRange(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif),i));}

inline void SgCaseOptionStmt::setCaseRange(int, SgExpression &caseRange)
{ 
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),caseRange.thellnd); 
}

inline SgStatement * SgCaseOptionStmt::body()
{
  PTR_BFND bif =NULL;
  if (BIF_BLOB1(thebif))
    bif = BLOB_VALUE(BIF_BLOB1(thebif));
  return BfndMapping(bif);
}

inline void SgCaseOptionStmt::setBody(SgStatement &body)
{
  BIF_BLOB1(thebif) = NULL;
  insertBfndListIn(body.thebif,thebif,thebif);
}


// ******************** Leaf Executable Nodes ***********************

// SgExecutableStatement--inlines

inline SgExecutableStatement::SgExecutableStatement(int variant):SgStatement(variant)
{}

// SgAssignStmt--inlines

inline SgAssignStmt::SgAssignStmt(int variant):SgExecutableStatement(variant)
{}
inline SgAssignStmt::SgAssignStmt(SgExpression &lhs, SgExpression &rhs):SgExecutableStatement(ASSIGN_STAT)
{
  BIF_LL1(thebif) = lhs.thellnd;
  BIF_LL2(thebif) = rhs.thellnd;
}

inline SgExpression * SgAssignStmt::lhs()
{ return LlndMapping(BIF_LL1(thebif)); }

// the right hand side
inline SgExpression * SgAssignStmt::rhs()
{ return LlndMapping(BIF_LL2(thebif)); }

// replace lhs with e
inline void SgAssignStmt::replaceLhs(SgExpression &e)
{ BIF_LL1(thebif) = e.thellnd; }

// replace rhs with e
inline void SgAssignStmt::replaceRhs(SgExpression &e)
{ BIF_LL2(thebif) = e.thellnd; }


// SgCExpStmt--inlines
inline SgCExpStmt::SgCExpStmt(SgExpression &exp):SgExecutableStatement(EXPR_STMT_NODE)
{ BIF_LL1(thebif) = exp.thellnd; }

inline SgCExpStmt::SgCExpStmt(SgExpression &lhs, SgExpression &rhs):SgExecutableStatement(EXPR_STMT_NODE)
{ BIF_LL1(thebif) =addToExprList(BIF_LL1(thebif),newExpr(ASSGN_OP,NULL,lhs.thellnd,rhs.thellnd)); }

// the expression
inline SgExpression *SgCExpStmt::expr()
{ return LlndMapping(BIF_LL1(thebif)); }

// replace exp with e
inline void SgCExpStmt::replaceExpression(SgExpression &e)
{ BIF_LL1(thebif) = e.thellnd; }

inline SgCExpStmt::~SgCExpStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgPointerAssignStmt--inlines

inline SgPointerAssignStmt::SgPointerAssignStmt(SgExpression lhs, SgExpression rhs):SgAssignStmt(POINTER_ASSIGN_STAT)
{
  BIF_LL1(thebif) = lhs.thellnd;
  BIF_LL2(thebif) = rhs.thellnd;
}

inline SgPointerAssignStmt::~SgPointerAssignStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgHeapStmt--inlines

inline SgHeapStmt::SgHeapStmt(int variant, SgExpression &allocationList, SgExpression &statVariable):SgExecutableStatement(variant)
{
  BIF_LL1(thebif) = allocationList.thellnd;
  BIF_LL2(thebif) = statVariable.thellnd;
}

inline SgHeapStmt::~SgHeapStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgExpression * SgHeapStmt::allocationList()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgHeapStmt::setAllocationList(SgExpression &allocationList)
{ BIF_LL1(thebif) = allocationList.thellnd;}

inline SgExpression * SgHeapStmt::statVariable()
{ return LlndMapping(BIF_LL2(thebif)); }

inline void SgHeapStmt::setStatVariable(SgExpression &statVar)
{ BIF_LL2(thebif) = statVar.thellnd; }


// SgNullifyStmt--inlines

inline SgNullifyStmt::SgNullifyStmt(SgExpression &objectList):SgExecutableStatement(NULLIFY_STMT)
{ BIF_LL1(thebif) = objectList.thellnd; }

inline SgNullifyStmt::~SgNullifyStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgExpression * SgNullifyStmt::nullifyList()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgNullifyStmt::setNullifyList(SgExpression &nullifyList)
{ BIF_LL1(thebif) = nullifyList.thellnd; }


// SgContinueStmt--inlines

inline SgContinueStmt::SgContinueStmt():SgExecutableStatement(CONT_STAT)
{}
inline SgContinueStmt::~SgContinueStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgControlEndStmt--inlines

inline SgControlEndStmt::SgControlEndStmt(int variant):SgExecutableStatement(variant)
{}

inline SgControlEndStmt::SgControlEndStmt():SgExecutableStatement(CONTROL_END)
{}

inline SgControlEndStmt::~SgControlEndStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgBreakStmt--inlines

inline SgBreakStmt::SgBreakStmt():SgExecutableStatement(BREAK_NODE)
{}

inline SgBreakStmt::~SgBreakStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgCycleStmt--inlines


inline SgCycleStmt::SgCycleStmt(SgSymbol &symbol):SgExecutableStatement(CYCLE_STMT)
{ BIF_SYMB(thebif) = symbol.thesymb; }

// added by A.S. Kolganov 20.12.2015
inline SgCycleStmt::SgCycleStmt():SgExecutableStatement(CYCLE_STMT)
{ }

// the name of the loop to cycle
inline SgSymbol * SgCycleStmt::constructName()
{ return SymbMapping(BIF_SYMB(thebif)); }

inline void SgCycleStmt::setConstructName(SgSymbol &constructName)
{ BIF_SYMB(thebif) = constructName.thesymb; }

inline SgCycleStmt::~SgCycleStmt()
{ RemoveFromTableBfnd((void *) this); }


inline SgExpression * SgReturnStmt::returnValue()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgReturnStmt::setReturnValue(SgExpression &retVal)
{ BIF_LL1(thebif) = retVal.thellnd; }

inline SgReturnStmt::~SgReturnStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgExitStmt--inlines

inline SgExitStmt::SgExitStmt(SgSymbol &construct_name):SgControlEndStmt(EXIT_STMT)
{ BIF_SYMB(thebif) = construct_name.thesymb; }

inline SgExitStmt::~SgExitStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgSymbol * SgExitStmt::constructName()
{ return SymbMapping(BIF_SYMB(thebif)); }  // the name of the loop to cycle

inline void SgExitStmt::setConstructName(SgSymbol &constructName)
{ BIF_SYMB(thebif) = constructName.thesymb; }



// SgGotoStmt--inlines
inline SgGotoStmt::SgGotoStmt(SgLabel &label):SgExecutableStatement(GOTO_NODE)
{ BIF_LL3(thebif) = SgLabelRefExp(label).thellnd; }
/* Tried to fix a bug reported by anl's people.
   The following line is the original code.
{ BIF_LABEL(thebif) = label.thelabel; }
*/


inline SgLabel * SgGotoStmt::branchLabel()
{ SgLabelRefExp *e =  (SgLabelRefExp *) LlndMapping(BIF_LL3(thebif));
  return (e)?e->label(): (SgLabel *) NULL;
 }
	

inline SgGotoStmt::~SgGotoStmt(){RemoveFromTableBfnd((void *) this);}


// SgLabelListStmt--inlines

inline SgLabelListStmt::SgLabelListStmt(int variant):SgExecutableStatement(variant)
{}

inline int SgLabelListStmt::numberOfTargets()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgExpression * SgLabelListStmt::labelList()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgLabelListStmt::setLabelList(SgExpression &labelList)
{ BIF_LL1(thebif) = labelList.thellnd; }



// SgAssignedGotoStmt--inlines

inline SgAssignedGotoStmt::SgAssignedGotoStmt(SgSymbol &symbol, SgExpression &labelList):SgLabelListStmt(ASSGOTO_NODE)
{
  BIF_SYMB(thebif) = symbol.thesymb;
  BIF_LL1(thebif) = labelList.thellnd;
}

inline SgSymbol * SgAssignedGotoStmt::symbol()
{ return SymbMapping(BIF_SYMB(thebif)); }

inline void SgAssignedGotoStmt::setSymbol(SgSymbol &symb)
{ BIF_SYMB(thebif) = symb.thesymb; }

inline SgAssignedGotoStmt::~SgAssignedGotoStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgComputedGotoStmt--inlines

inline SgComputedGotoStmt::SgComputedGotoStmt(SgExpression &expr, SgLabel &label):SgLabelListStmt(COMGOTO_NODE)
{
  BIF_LL1(thebif) = addLabelRefToExprList(BIF_LL1(thebif),label.thelabel);
  BIF_LL2(thebif) = expr.thellnd;
}

inline void SgComputedGotoStmt::addLabel(SgLabel &label)
{
  BIF_LL1(thebif) = addLabelRefToExprList(BIF_LL1(thebif),label.thelabel); 
}

inline SgExpression * SgComputedGotoStmt::exp()
{ return LlndMapping(BIF_LL2(thebif)); }

inline void SgComputedGotoStmt::setExp(SgExpression &exp)
{ BIF_LL2(thebif) = exp.thellnd; }

inline SgComputedGotoStmt::~SgComputedGotoStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgStopOrPauseStmt--inlines

inline SgStopOrPauseStmt::SgStopOrPauseStmt(int variant, SgExpression *expr):SgExecutableStatement(variant)
{ 
if (expr)
  BIF_LL1(thebif) = expr->thellnd;
 }

inline SgExpression * SgStopOrPauseStmt::exp()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgStopOrPauseStmt::setExp(SgExpression &exp)
{ BIF_LL1(thebif) = exp.thellnd; }

inline SgStopOrPauseStmt::~SgStopOrPauseStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgCallStmt--inlines

inline SgCallStmt::SgCallStmt(SgSymbol &name, SgExpression &args):SgExecutableStatement(PROC_STAT)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_LL1(thebif) = args.thellnd;
}

inline SgCallStmt::SgCallStmt(SgSymbol &name):SgExecutableStatement(PROC_STAT)
{ BIF_SYMB(thebif) = name.thesymb; }

// name of subroutine being called
inline SgSymbol * SgCallStmt::name()
{ return SymbMapping(BIF_SYMB(thebif)); }

// the number of arguement expressions
inline int SgCallStmt::numberOfArgs()
{ return exprListLength(BIF_LL1(thebif)); }

inline void SgCallStmt::addArg(SgExpression &arg)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd); }

// the i-th argument expression
inline SgExpression * SgCallStmt::arg(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif), i)); }

inline SgCallStmt::~SgCallStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgProsCallStmt--inlines
 
inline SgProsCallStmt::SgProsCallStmt(SgSymbol &name, SgExprListExp &args):SgExecutableStatement(PROS_STAT)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_LL1(thebif) = args.thellnd;
}
 
inline SgProsCallStmt::SgProsCallStmt(SgSymbol &name):SgExecutableStatement(PROS_STAT)
{ BIF_SYMB(thebif) = name.thesymb; }

// name of process being called
inline SgSymbol * SgProsCallStmt::name()
{ return SymbMapping(BIF_SYMB(thebif)); }
 
// the number of arguement expressions
inline int SgProsCallStmt::numberOfArgs()
{ return exprListLength(BIF_LL1(thebif)); }
 
inline void SgProsCallStmt::addArg(SgExpression &arg)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd); }

inline SgExprListExp *SgProsCallStmt::args()
{ return (SgExprListExp *) LlndMapping(BIF_LL1(thebif)); }
 
// the i-th argument expression
inline SgExpression * SgProsCallStmt::arg(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif), i)); }
 
inline SgProsCallStmt::~SgProsCallStmt()
{ RemoveFromTableBfnd((void *) this); }
 
 
 
// SgProsCallLctn--inlines
 
inline SgProsCallLctn::SgProsCallLctn(SgSymbol &name, SgExprListExp &args,
                                      SgExprListExp &lctn)
                      :SgExecutableStatement(PROS_STAT_LCTN)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_LL1(thebif) = args.thellnd;
  BIF_LL2(thebif) = lctn.thellnd;
}
 
inline SgProsCallLctn::SgProsCallLctn(SgSymbol &name, SgExprListExp &lctn)
                      :SgExecutableStatement(PROS_STAT_LCTN)
{
   BIF_SYMB(thebif) = name.thesymb;
   BIF_LL2(thebif) = lctn.thellnd;
}
 
// name of process being called
inline SgSymbol * SgProsCallLctn::name()
{ return SymbMapping(BIF_SYMB(thebif)); }
 
// the number of arguement expressions
inline int SgProsCallLctn::numberOfArgs()
{ return exprListLength(BIF_LL1(thebif)); }
 
inline void SgProsCallLctn::addArg(SgExpression &arg)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd); }
 
inline SgExprListExp *SgProsCallLctn::args()
{ return (SgExprListExp *) LlndMapping(BIF_LL1(thebif)); }
 
// the i-th argument expression
inline SgExpression * SgProsCallLctn::arg(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif), i)); }
 
inline SgExpression * SgProsCallLctn::location()
{ return LlndMapping(BIF_LL2(thebif)); }

inline SgProsCallLctn::~SgProsCallLctn()
{ RemoveFromTableBfnd((void *) this); }
 
 

// SgProsCallSubm--inlines
 
inline SgProsCallSubm::SgProsCallSubm(SgSymbol &name, SgExprListExp &args,
                                      SgExprListExp &subm)
                      :SgExecutableStatement(PROS_STAT_SUBM)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_LL1(thebif) = args.thellnd;
  BIF_LL2(thebif) = subm.thellnd;
}
 
inline SgProsCallSubm::SgProsCallSubm(SgSymbol &name, SgExprListExp &subm)
                      :SgExecutableStatement(PROS_STAT_SUBM)
{
   BIF_SYMB(thebif) = name.thesymb;
   BIF_LL2(thebif) = subm.thellnd;
}
 
// name of process being called
inline SgSymbol * SgProsCallSubm::name()
{ return SymbMapping(BIF_SYMB(thebif)); }
 
// the number of arguement expressions
inline int SgProsCallSubm::numberOfArgs()
{ return exprListLength(BIF_LL1(thebif)); }
 
inline void SgProsCallSubm::addArg(SgExpression &arg)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd); }
 
inline SgExprListExp *SgProsCallSubm::args()
{ return (SgExprListExp *) LlndMapping(BIF_LL1(thebif)); }
 
// the i-th argument expression
inline SgExpression * SgProsCallSubm::arg(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif), i)); }

inline SgExpression * SgProsCallSubm::submachine()
{ return LlndMapping(BIF_LL2(thebif)); }
 
inline SgProsCallSubm::~SgProsCallSubm()
{ RemoveFromTableBfnd((void *) this); }
 
 

// SgProcessesStmt--inlines

inline SgProcessesStmt::SgProcessesStmt():SgStatement(PROCESSES_STAT)
{}

inline SgProcessesStmt::~SgProcessesStmt()
{ RemoveFromTableBfnd((void *) this); }
 


// SgEndProcessesStmt--inlines
 
inline SgEndProcessesStmt::SgEndProcessesStmt():SgStatement(PROCESSES_END)
{}
 
inline SgEndProcessesStmt::~SgEndProcessesStmt()
{ RemoveFromTableBfnd((void *) this); }
 
 

// SgInportStmt--inlines

inline SgInportStmt::SgInportStmt(SgExprListExp &name):SgStatement(INPORT_DECL)
{ BIF_LL1(thebif) = name.thellnd; }

inline SgInportStmt::SgInportStmt(SgExprListExp &name, SgPortTypeExp &porttype)
                    :SgStatement(INPORT_DECL)
{
  BIF_LL1(thebif) = name.thellnd;
  BIF_LL2(thebif) = porttype.thellnd;
}

inline SgInportStmt::~SgInportStmt()
{ RemoveFromTableBfnd((void *) this); }

inline void SgInportStmt::addname(SgExpression &name)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), name.thellnd); }

inline int SgInportStmt::numberOfNames()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgExprListExp * SgInportStmt::names()
{ return (SgExprListExp *) LlndMapping(BIF_LL1(thebif)); }

inline SgExpression *SgInportStmt::name(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif),i)); }

inline void SgInportStmt::addporttype(SgExpression &porttype)
{ BIF_LL2(thebif) = addToList(BIF_LL2(thebif), porttype.thellnd); }

inline int SgInportStmt::numberOfPortTypes()
{ return exprListLength(BIF_LL2(thebif)); }

inline SgPortTypeExp * SgInportStmt::porttypes()
{ return (SgPortTypeExp *) LlndMapping(BIF_LL2(thebif)); }

inline SgPortTypeExp * SgInportStmt::porttype(int i)
{ return (SgPortTypeExp *) LlndMapping(getPositionInList(BIF_LL2(thebif),i)); }



// SgOutportStmt--inlines
 
inline SgOutportStmt::SgOutportStmt(SgExprListExp &name)
                     :SgStatement(OUTPORT_DECL)
{ BIF_LL1(thebif) = name.thellnd; }
 
inline SgOutportStmt::SgOutportStmt(SgExprListExp &name,
                                    SgPortTypeExp &porttype)
                    :SgStatement(OUTPORT_DECL)
{
  BIF_LL1(thebif) = name.thellnd;
  BIF_LL2(thebif) = porttype.thellnd;
}
 
inline SgOutportStmt::~SgOutportStmt()
{ RemoveFromTableBfnd((void *) this); }
 
inline void SgOutportStmt::addname(SgExpression &name)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), name.thellnd); }
 
inline int SgOutportStmt::numberOfNames()
{ return exprListLength(BIF_LL1(thebif)); }
 
inline SgExprListExp * SgOutportStmt::names()
{ return (SgExprListExp *) LlndMapping(BIF_LL1(thebif)); }
 
inline SgExpression *SgOutportStmt::name(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif),i)); }

inline void SgOutportStmt::addporttype(SgExpression &porttype)
{ BIF_LL2(thebif) = addToList(BIF_LL2(thebif), porttype.thellnd); }
 
inline int SgOutportStmt::numberOfPortTypes()
{ return exprListLength(BIF_LL2(thebif)); }
 
inline SgPortTypeExp * SgOutportStmt::porttypes()
{ return (SgPortTypeExp *) LlndMapping(BIF_LL2(thebif)); }
 
inline SgPortTypeExp * SgOutportStmt::porttype(int i)
{ return (SgPortTypeExp *) LlndMapping(getPositionInList(BIF_LL2(thebif),i)); }

 

// SgChannelStmt--inlines

inline SgChannelStmt::SgChannelStmt(SgExpression &outport, SgExpression &inport)
                     :SgStatement(CHANNEL_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd);
}
 

inline SgChannelStmt::SgChannelStmt(SgExpression &outport, SgExpression &inport,
                                    SgExpression &io_or_err)
                     :SgStatement(CHANNEL_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), io_or_err.thellnd);
}
 

inline SgChannelStmt::SgChannelStmt(SgExpression &outport, SgExpression &inport,
                                  SgExpression &iostore, SgExpression &errlabel)
                     :SgStatement(CHANNEL_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd);
}
 

inline SgChannelStmt::~SgChannelStmt()
{ RemoveFromTableBfnd((void *) this); }


inline SgExpression * SgChannelStmt::outport()
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),0)); }


inline SgExpression * SgChannelStmt::inport()
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),1)); }


inline SgExpression * SgChannelStmt::ioStore()
{
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),2);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE) // must be ERR_LABEL
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}


inline SgExpression * SgChannelStmt::errLabel()
{
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),2);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // must be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if ((!ll) || (NODE_CODE(ll) != ERR_LABEL))
        return (SgExpression *) NULL;
      else
        return LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}



// SgMergerStmt--inlines

inline SgMergerStmt::SgMergerStmt(SgExpression &outport, SgExpression &inport):
                    SgStatement(MERGER_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd);
}


inline SgMergerStmt::SgMergerStmt(SgExpression &outport, SgExpression &inport,
                                SgExpression &io_or_err)
                   :SgStatement(MERGER_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), io_or_err.thellnd);
}


inline SgMergerStmt::SgMergerStmt(SgExpression &outport, SgExpression &inport,
                                SgExpression &iostore, SgExpression &errlabel):
                   SgStatement(MERGER_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd);
}


inline SgMergerStmt::~SgMergerStmt()
{ RemoveFromTableBfnd((void *) this); }


inline void SgMergerStmt::addOutport(SgExpression &outport)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), outport.thellnd); }
  

inline void SgMergerStmt::addIoStore(SgExpression &iostore)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd); }


inline void SgMergerStmt::addErrLabel(SgExpression &errlabel)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd); }


inline int SgMergerStmt::numberOfOutports()
{
  PTR_LLND ll = BIF_LL1(thebif);
  int n = 0;

  while (ll && (n != 3)) {
    if (( NODE_CODE(ll) == IOSTAT_STORE ) || ( NODE_CODE(ll) == ERR_LABEL ) ||
       ( NODE_CODE(ll) == INPORT_NAME ))
      n = n + 1;
    ll = NODE_OPERAND1(ll);
  };
  return (exprListLength(BIF_LL1(thebif)) - n);
  // double scanning the list may be improved
}


inline SgExpression * SgMergerStmt::outport(int i)
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),i)); }


inline SgExpression * SgMergerStmt::inport()
{
  int n = numberOfOutports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll) {
    return (SgExpression *) NULL;
  } else
    return  LlndMapping(ll);
}
 

inline SgExpression * SgMergerStmt::ioStore()
{
  int n = numberOfOutports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n+1);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE) //must be ERR_LABEL
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}
 

inline SgExpression * SgMergerStmt::errLabel()
{
  int n = numberOfOutports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n+1);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // imust be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if (!ll)
        return (SgExpression *) NULL;
      else
        if (NODE_CODE(ll) != ERR_LABEL)
          return (SgExpression *) NULL;
        else
          return  LlndMapping(ll);
    }
    else
      return  LlndMapping(ll);
}



// SgMoveportStmt--inlines

inline SgMoveportStmt::SgMoveportStmt(SgExpression &fromport,
                                      SgExpression &toport)
                     :SgStatement(MOVE_PORT)
{
  BIF_LL1(thebif) = fromport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), toport.thellnd);
}
 

inline SgMoveportStmt::SgMoveportStmt(SgExpression &fromport,
                                      SgExpression &toport,
                                      SgExpression &io_or_err)
                     :SgStatement(MOVE_PORT)
{
  BIF_LL1(thebif) = fromport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), toport.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), io_or_err.thellnd);
}
 

inline SgMoveportStmt::SgMoveportStmt(SgExpression &fromport,
                                      SgExpression &toport,
                                      SgExpression &iostore,
                                      SgExpression &errlabel)
                     :SgStatement(MOVE_PORT)
{
  BIF_LL1(thebif) = fromport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), toport.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd);
}
 

inline SgMoveportStmt::~SgMoveportStmt()
{ RemoveFromTableBfnd((void *) this); }


inline SgExpression * SgMoveportStmt::fromport()
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),0)); }


inline SgExpression * SgMoveportStmt::toport()
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),1)); }


inline SgExpression * SgMoveportStmt::ioStore()
{
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),2);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE) // must be ERR_LABEL
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}


inline SgExpression * SgMoveportStmt::errLabel()
{
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),2);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // must be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if ((!ll) || (NODE_CODE(ll) != ERR_LABEL))
        return (SgExpression *) NULL;
      else
        return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}



// SgSendStmt--inlines

inline SgSendStmt::SgSendStmt(SgExpression &control, SgExprListExp &argument):
                   SgStatement(SEND_STAT)
{
  BIF_LL1(thebif) = control.thellnd;
  BIF_LL2(thebif) = argument.thellnd;
}


inline SgSendStmt::SgSendStmt(SgExpression &outport, SgExprListExp &argument,
                              SgExpression &io_or_err): SgStatement(SEND_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), io_or_err.thellnd);
  BIF_LL2(thebif) = argument.thellnd;
}


inline SgSendStmt::SgSendStmt(SgExpression &outport, SgExprListExp &argument,
                              SgExpression &iostore, SgExpression &errlabel):
                   SgStatement(SEND_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd);
  BIF_LL2(thebif) = argument.thellnd;
}


inline SgSendStmt::~SgSendStmt()
{ RemoveFromTableBfnd((void *) this); }


inline void SgSendStmt::addOutport(SgExpression &outport)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), outport.thellnd); }
  

inline void SgSendStmt::addIoStore(SgExpression &iostore)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd); }


inline void SgSendStmt::addErrLabel(SgExpression &errlabel)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd); }


inline void SgSendStmt::addArgument(SgExpression &argument)
{ BIF_LL2(thebif) = addToExprList(BIF_LL2(thebif), argument.thellnd); }


inline int SgSendStmt::numberOfOutports()
{
  PTR_LLND ll = BIF_LL1(thebif);
  int n = 0;

  while (ll && (n != 2)) {
    if (( NODE_CODE(ll) == IOSTAT_STORE ) || ( NODE_CODE(ll) == ERR_LABEL ))
      n = n + 1;
    ll = NODE_OPERAND1(ll);
  };
  return (exprListLength(BIF_LL1(thebif)) - n);
  // double scanning the list may be improved
}


inline int SgSendStmt::numberOfArguments()
{ return exprListLength(BIF_LL2(thebif)); }


inline SgExpression * SgSendStmt::controls()
{ return LlndMapping(BIF_LL1(thebif)); }


inline SgExpression * SgSendStmt::outport(int i)
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),i)); }


inline SgExprListExp * SgSendStmt::arguments()
{ return (SgExprListExp *) LlndMapping(BIF_LL2(thebif)); }


inline SgExpression * SgSendStmt::argument(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL2(thebif),i)); }


inline SgExpression * SgSendStmt::ioStore()
{
  int n = numberOfOutports();
  PTR_LLND ll;

  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE)
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}


inline SgExpression * SgSendStmt::errLabel()
{
  int n = numberOfOutports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // must be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if ((!ll) || (NODE_CODE(ll) != ERR_LABEL))
        return (SgExpression *) NULL;
      else
        return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}



// SgReceiveStmt--inlines

inline SgReceiveStmt::SgReceiveStmt(SgExpression &control,
                                    SgExprListExp &argument)
                     :SgStatement(RECEIVE_STAT)
{
  BIF_LL1(thebif) = control.thellnd;
  BIF_LL2(thebif) = argument.thellnd;
}


inline SgReceiveStmt::SgReceiveStmt(SgExpression &inport,
                                    SgExprListExp &argument,
                                    SgExpression &e1):SgStatement(RECEIVE_STAT)
{
  BIF_LL1(thebif) = inport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e1.thellnd);
  BIF_LL2(thebif) = argument.thellnd;
}


inline SgReceiveStmt::SgReceiveStmt(SgExpression &inport,
                                    SgExprListExp &argument,
                                    SgExpression &e1,
                                    SgExpression &e2):SgStatement(RECEIVE_STAT)
{
  BIF_LL1(thebif) = inport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e1.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e2.thellnd);
  BIF_LL2(thebif) = argument.thellnd;
}


inline SgReceiveStmt::SgReceiveStmt(SgExpression &inport,
                                    SgExprListExp &argument,
                                    SgExpression &e1,
                                    SgExpression &e2,
                                    SgExpression &e3):SgStatement(RECEIVE_STAT)
{
  BIF_LL1(thebif) = inport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e1.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e2.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e3.thellnd);
  BIF_LL2(thebif) = argument.thellnd;
}
 

inline SgReceiveStmt::~SgReceiveStmt()
{ RemoveFromTableBfnd((void *) this); }


inline void SgReceiveStmt::addInport(SgExpression &inport)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd); }
  

inline void SgReceiveStmt::addIoStore(SgExpression &iostore)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd); }


inline void SgReceiveStmt::addErrLabel(SgExpression &errlabel)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd); }


inline void SgReceiveStmt::addEndLabel(SgExpression &endlabel)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), endlabel.thellnd); }
 

inline void SgReceiveStmt::addArgument(SgExpression &argument)
{ BIF_LL2(thebif) = addToExprList(BIF_LL2(thebif), argument.thellnd); }


inline int SgReceiveStmt::numberOfInports()
{
  PTR_LLND ll = BIF_LL1(thebif);
  int n = 0;

  while (ll && (n != 3)) {
    if (( NODE_CODE(ll) == IOSTAT_STORE ) || ( NODE_CODE(ll) == ERR_LABEL ) ||
       ( NODE_CODE(ll) == END_LABEL ))
      n = n + 1;
    ll = NODE_OPERAND1(ll);
  };
  return (exprListLength(BIF_LL1(thebif)) - n);
  // double scanning the list may be improved
}


inline int SgReceiveStmt::numberOfArguments()
{ return exprListLength(BIF_LL2(thebif)); }


inline SgExpression * SgReceiveStmt::controls()
{ return LlndMapping(BIF_LL1(thebif)); }


inline SgExpression * SgReceiveStmt::inport(int i)
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),i)); }


inline SgExprListExp * SgReceiveStmt::arguments()
{ return (SgExprListExp *) LlndMapping(BIF_LL2(thebif)); }


inline SgExpression * SgReceiveStmt::argument(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL2(thebif),i)); }


inline SgExpression * SgReceiveStmt::ioStore()
{
  int n = numberOfInports();
  PTR_LLND ll;

  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE)
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}


inline SgExpression * SgReceiveStmt::errLabel()
{
  int n = numberOfInports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // must be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if ((!ll) || (NODE_CODE(ll) != ERR_LABEL))
        return (SgExpression *) NULL;
      else
        return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}


inline SgExpression * SgReceiveStmt::endLabel()
{
  int n = numberOfInports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != END_LABEL) { // must be IOSTAT_STORE or ERR_LABEL
      ll = NODE_OPERAND1(ll);
      if (!ll)
        return (SgExpression *) NULL;
      else
        if (NODE_CODE(ll) != END_LABEL) { // must be ERR_LABEL
          ll = NODE_OPERAND1(ll);
          if ((!ll) || (NODE_CODE(ll) != END_LABEL))
            return (SgExpression *) NULL;
          else
            return  LlndMapping(ll);
        } else
          return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}


// SgEndchannelStmt--inlines

inline SgEndchannelStmt::SgEndchannelStmt(SgExpression &outport)
                        :SgStatement(ENDCHANNEL_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
}


inline SgEndchannelStmt::SgEndchannelStmt(SgExpression &outport,
                                          SgExpression &io_or_err)
                        :SgStatement(ENDCHANNEL_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), io_or_err.thellnd);
}


inline SgEndchannelStmt::SgEndchannelStmt(SgExpression &outport,
                                          SgExpression &iostore,
                                          SgExpression &errlabel)
                        :SgStatement(ENDCHANNEL_STAT)
{
  BIF_LL1(thebif) = outport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd);
}


inline SgEndchannelStmt::~SgEndchannelStmt()
{ RemoveFromTableBfnd((void *) this); }


inline void SgEndchannelStmt::addOutport(SgExpression &outport)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), outport.thellnd); }
  

inline void SgEndchannelStmt::addIoStore(SgExpression &iostore)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd); }


inline void SgEndchannelStmt::addErrLabel(SgExpression &errlabel)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd); }


inline int SgEndchannelStmt::numberOfOutports()
{
  PTR_LLND ll = BIF_LL1(thebif);
  int n = 0;

  while (ll && (n != 2)) {
    if (( NODE_CODE(ll) == IOSTAT_STORE ) || ( NODE_CODE(ll) == ERR_LABEL ))
      n = n + 1;
    ll = NODE_OPERAND1(ll);
  };
  return (exprListLength(BIF_LL1(thebif)) - n);
  // double scanning the list may be improved
}


inline SgExpression * SgEndchannelStmt::controls()
{ return LlndMapping(BIF_LL1(thebif)); }


inline SgExpression * SgEndchannelStmt::outport(int i)
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),i)); }


inline SgExpression * SgEndchannelStmt::ioStore()
{
  int n = numberOfOutports();
  PTR_LLND ll;

  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE)
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}


inline SgExpression * SgEndchannelStmt::errLabel()
{
  int n = numberOfOutports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // must be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if ((!ll) || (NODE_CODE(ll) != ERR_LABEL))
        return (SgExpression *) NULL;
      else
        return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}



// SgProbeStmt--inlines

inline SgProbeStmt::SgProbeStmt(SgExpression &inport):SgStatement(PROBE_STAT)
{ BIF_LL1(thebif) = inport.thellnd; }


inline SgProbeStmt::SgProbeStmt(SgExpression &inport, SgExpression &e1)
                   :SgStatement(PROBE_STAT)
{
  BIF_LL1(thebif) = inport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e1.thellnd);
}


inline SgProbeStmt::SgProbeStmt(SgExpression &inport, SgExpression &e1,
                                SgExpression &e2):SgStatement(PROBE_STAT)
{
  BIF_LL1(thebif) = inport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e1.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e2.thellnd);
}


inline SgProbeStmt::SgProbeStmt(SgExpression &inport, SgExpression &e1,
                                SgExpression &e2, SgExpression &e3)
                   :SgStatement(PROBE_STAT)
{
  BIF_LL1(thebif) = inport.thellnd;
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e1.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e2.thellnd);
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), e3.thellnd);
}
 

inline SgProbeStmt::~SgProbeStmt()
{ RemoveFromTableBfnd((void *) this); }


inline void SgProbeStmt::addInport(SgExpression &inport)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), inport.thellnd); }
  

inline void SgProbeStmt::addIoStore(SgExpression &iostore)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), iostore.thellnd); }


inline void SgProbeStmt::addErrLabel(SgExpression &errlabel)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), errlabel.thellnd); }


inline void SgProbeStmt::addEmptyStore(SgExpression &emptystore)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), emptystore.thellnd); }
 

inline int SgProbeStmt::numberOfInports()
{
  PTR_LLND ll = BIF_LL1(thebif);
  int n = 0;

  while (ll && (n != 3)) {
    if (( NODE_CODE(ll) == IOSTAT_STORE ) || ( NODE_CODE(ll) == ERR_LABEL ) ||
       ( NODE_CODE(ll) == EMPTY_STORE ))
      n = n + 1;
    ll = NODE_OPERAND1(ll);
  };
  return (exprListLength(BIF_LL1(thebif)) - n);
  // double scanning the list may be improved
}


inline SgExpression * SgProbeStmt::controls()
{ return LlndMapping(BIF_LL1(thebif)); }


inline SgExpression * SgProbeStmt::inport(int i)
{ return LlndMapping(getPositionInList(BIF_LL1(thebif),i)); }


inline SgExpression * SgProbeStmt::ioStore()
{
  int n = numberOfInports();
  PTR_LLND ll;

  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != IOSTAT_STORE)
      return (SgExpression *) NULL;
    else
      return  LlndMapping(ll);
}


inline SgExpression * SgProbeStmt::errLabel()
{
  int n = numberOfInports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != ERR_LABEL) { // must be IOSTAT_STORE
      ll = NODE_OPERAND1(ll);
      if ((!ll) || (NODE_CODE(ll) != ERR_LABEL)) // must be EMPTY_STORE
          return (SgExpression *) NULL;
      else
        return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}


inline SgExpression * SgProbeStmt::emptyStore()
{
  int n = numberOfInports();
  PTR_LLND ll;
 
  ll = getPositionInList(BIF_LL1(thebif),n);
  if (!ll)
    return (SgExpression *) NULL;
  else
    if (NODE_CODE(ll) != EMPTY_STORE) { // must be IOSTAT_STORE or ERR_LABEL
      ll = NODE_OPERAND1(ll);
      if (!ll)
        return (SgExpression *) NULL;
      else
        if (NODE_CODE(ll) != EMPTY_STORE) { // must be ERR_LABEL
          ll = NODE_OPERAND1(ll);
          if ((!ll) || (NODE_CODE(ll) != EMPTY_STORE))
            return (SgExpression *) NULL;
          else
            return  LlndMapping(ll);
        } else
          return  LlndMapping(ll);
    } else
      return  LlndMapping(ll);
}



// SgPortTypeExp--inlines

inline SgPortTypeExp::SgPortTypeExp(SgType &type):SgExpression(PORT_TYPE_OP)
{ NODE_TYPE(thellnd) = type.thetype; }


inline SgPortTypeExp::SgPortTypeExp(SgType &type, SgExpression &ref)
                     :SgExpression(PORT_TYPE_OP)
{
  NODE_TYPE(thellnd) = type.thetype;
  NODE_OPERAND0(thellnd) = ref.thellnd;
}


inline SgPortTypeExp::SgPortTypeExp(int variant, SgExpression &porttype)
                     :SgExpression(variant)
{ NODE_OPERAND0(thellnd) = porttype.thellnd; }


inline SgPortTypeExp::~SgPortTypeExp()
{ RemoveFromTableLlnd((void *) this); }


inline SgType * SgPortTypeExp::type()
{ return TypeMapping(NODE_TYPE(thellnd)); }

inline int SgPortTypeExp::numberOfRef()
{
  PTR_LLND ll = NODE_OPERAND0(thellnd);
  int n = 0;
  while (ll) {
      n = n + 1;
  ll = NODE_OPERAND1(ll);
  };
  return n;
}

inline SgExpression * SgPortTypeExp::ref()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgPortTypeExp * SgPortTypeExp::next()
{ return (SgPortTypeExp *) LlndMapping(NODE_OPERAND1(thellnd)); }


// SgControlExp--inlines
 
inline SgControlExp::SgControlExp(int variant):SgExpression(variant)
{}
 
inline SgControlExp::~SgControlExp()
{ RemoveFromTableLlnd((void *) this); }
 
inline SgExpression * SgControlExp::exp()
{  return LlndMapping(NODE_OPERAND0(thellnd)); }
 
 

// SgInportExp--inlines

inline SgInportExp::SgInportExp(SgExprListExp &exp):SgControlExp(INPORT_NAME)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }

inline SgInportExp::~SgInportExp()
{ RemoveFromTableLlnd((void *) this); }



// SgOutportExp--inlines
 
inline SgOutportExp::SgOutportExp(SgExprListExp &exp):SgControlExp(OUTPORT_NAME)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgOutportExp::~SgOutportExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgFromportExp--inlines
 
inline SgFromportExp::SgFromportExp(SgExprListExp &exp)
                     :SgControlExp(FROMPORT_NAME)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgFromportExp::~SgFromportExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgToportExp--inlines
 
inline SgToportExp::SgToportExp(SgExprListExp &exp):SgControlExp(TOPORT_NAME)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgToportExp::~SgToportExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgIO_statStoreExp--inlines
 
inline SgIO_statStoreExp::SgIO_statStoreExp(SgExprListExp &exp)
                       :SgControlExp(IOSTAT_STORE)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgIO_statStoreExp::~SgIO_statStoreExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgEmptyStoreExp--inlines
 
inline SgEmptyStoreExp::SgEmptyStoreExp(SgExprListExp &exp)
                       :SgControlExp(EMPTY_STORE)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgEmptyStoreExp::~SgEmptyStoreExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgErrLabelExp--inlines
 
inline SgErrLabelExp::SgErrLabelExp(SgExprListExp &exp):SgControlExp(ERR_LABEL)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgErrLabelExp::~SgErrLabelExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgEndLabelExp--inlines
 
inline SgEndLabelExp::SgEndLabelExp(SgExprListExp &exp):SgControlExp(END_LABEL)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }
 
inline SgEndLabelExp::~SgEndLabelExp()
{ RemoveFromTableLlnd((void *) this); }
 
 

// SgDataImpliedDoExp--inlines

inline SgDataImpliedDoExp::SgDataImpliedDoExp(SgExprListExp &dlist,
                                              SgSymbol &iname,
                                              SgExprListExp &ilist)
                          :SgExpression(DATA_IMPL_DO)
{
  NODE_OPERAND0(thellnd) = dlist.thellnd;
  NODE_SYMB(thellnd) = iname.thesymb;
  NODE_OPERAND1(thellnd) = ilist.thellnd;
}

inline SgDataImpliedDoExp::~SgDataImpliedDoExp()
{ RemoveFromTableLlnd((void *) this); }

inline void SgDataImpliedDoExp::addDataelt(SgExpression &data)
{ NODE_OPERAND0(thellnd) = addToList(NODE_OPERAND0(thellnd),data.thellnd); }

inline void SgDataImpliedDoExp::addIconexpr(SgExpression &icon)
{ NODE_OPERAND1(thellnd) = addToList(NODE_OPERAND1(thellnd),icon.thellnd); }

inline SgSymbol *SgDataImpliedDoExp::iname()
{ return SymbMapping(NODE_SYMB(thellnd)); }

inline int SgDataImpliedDoExp::numberOfDataelt()
{ return exprListLength(NODE_OPERAND0(thellnd)); }

inline SgExprListExp *SgDataImpliedDoExp::dataelts()
{ return (SgExprListExp *) LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression *SgDataImpliedDoExp::dataelt(int i)
{ return LlndMapping(getPositionInList(NODE_OPERAND0(thellnd),i));  }

inline SgExprListExp *SgDataImpliedDoExp::iconexprs()
{ return (SgExprListExp *) LlndMapping(NODE_OPERAND1(thellnd)); }

inline SgExpression *SgDataImpliedDoExp::init()
{ return LlndMapping(getPositionInList(NODE_OPERAND1(thellnd),0));  }

inline SgExpression *SgDataImpliedDoExp::limit()
{ return LlndMapping(getPositionInList(NODE_OPERAND1(thellnd),1));  }

inline SgExpression *SgDataImpliedDoExp::increment()
{ return LlndMapping(getPositionInList(NODE_OPERAND1(thellnd),2));  }



// SgDataEltExp--inlines

inline SgDataEltExp::SgDataEltExp(SgExpression &dataimplieddo)
                    :SgExpression(DATA_ELT)
{ NODE_OPERAND0(thellnd) = dataimplieddo.thellnd; }

inline SgDataEltExp::SgDataEltExp(SgSymbol &name, SgExpression &datasubs,
                                  SgExpression &datarange)
                    :SgExpression(DATA_ELT)
{
  NODE_SYMB(thellnd) = name.thesymb;
  NODE_OPERAND1(datasubs.thellnd) = datarange.thellnd;
  NODE_OPERAND0(thellnd) = datasubs.thellnd;
}

inline SgDataEltExp::~SgDataEltExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgSymbol *SgDataEltExp::name()
{ return SymbMapping(NODE_SYMB(thellnd)); }

inline SgExpression *SgDataEltExp::dataimplieddo()
{
  if (NODE_SYMB(thellnd) == NULL)
    return LlndMapping(NODE_OPERAND0(thellnd));
  else
    return NULL;
}

inline SgExpression *SgDataEltExp::datasubs()
{
  if (NODE_SYMB(thellnd) != NULL)
    if (NODE_CODE(NODE_OPERAND0(thellnd)) == DATA_SUBS)
      return LlndMapping(NODE_OPERAND0(thellnd));
    else
      return (SgExpression *) NULL;
  else
    return (SgExpression *) NULL;
}

inline SgExpression *SgDataEltExp::datarange()
{
  if (NODE_SYMB(thellnd) != NULL)
    if (NODE_CODE(NODE_OPERAND0(thellnd)) == DATA_RANGE)
      return LlndMapping(NODE_OPERAND0(thellnd));
    else
      if (NODE_OPERAND1(NODE_OPERAND0(thellnd)) != NULL)
        return LlndMapping(NODE_OPERAND1(NODE_OPERAND0(thellnd)));
      else
        return (SgExpression *) NULL;
  else
    return (SgExpression *) NULL;
}



// SgDataSubsExp--inlines

inline SgDataSubsExp::SgDataSubsExp(SgExprListExp &iconexprlist)
                     :SgExpression(DATA_SUBS)
{ NODE_OPERAND0(thellnd) = iconexprlist.thellnd; }

inline SgDataSubsExp::~SgDataSubsExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgExprListExp *SgDataSubsExp::iconexprlist()
{ return (SgExprListExp *) LlndMapping(NODE_OPERAND0(thellnd)); }



// SgDataRangeExp--inlines

inline SgDataRangeExp::SgDataRangeExp(SgExpression &iconexpr1,
                                      SgExpression &iconexpr2)
                      :SgExpression(DATA_RANGE)
{
  NODE_OPERAND0(thellnd) = iconexpr1.thellnd;
  NODE_OPERAND1(thellnd) = iconexpr2.thellnd;
}

inline SgDataRangeExp::~SgDataRangeExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression *SgDataRangeExp::iconexpr1()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }

inline SgExpression *SgDataRangeExp::iconexpr2()
{ return LlndMapping(NODE_OPERAND1(thellnd)); }



// SgIconExprExp--inlines

inline SgIconExprExp::SgIconExprExp(SgExpression &exp):SgExpression(ICON_EXPR)
{ NODE_OPERAND0(thellnd) = exp.thellnd; }

inline SgIconExprExp::~SgIconExprExp()
{ RemoveFromTableLlnd((void *) this); }

inline SgExpression *SgIconExprExp::expr()
{ return LlndMapping(NODE_OPERAND0(thellnd)); }



// SgIOStmt--inlines
inline SgIOStmt::SgIOStmt(int variant):SgExecutableStatement(variant)
{}


// SgInputOutputStmt--inlines
         
inline SgInputOutputStmt::SgInputOutputStmt(int variant, SgExpression &specList, SgExpression &itemList): SgIOStmt(variant) 
{
    if (variant != READ_STAT && variant != WRITE_STAT && variant != PRINT_STAT)
    {
        Message("illegal variant for SgInputOutputStmt", 0);
#ifdef __SPF   
        {
            char buf[512];
            sprintf(buf, "Internal error at line %d and file libSage++.h\n", __LINE__);
            addToGlobalBufferAndPrint(buf);
        }
        throw -1;
#endif
    }
  BIF_LL1(thebif) = itemList.thellnd;
  BIF_LL2(thebif) = specList.thellnd;
}
        
inline SgExpression * SgInputOutputStmt::specList()
{ return LlndMapping(BIF_LL2(thebif)); }

inline void SgInputOutputStmt::setSpecList(SgExpression &specList)
{ BIF_LL2(thebif) = specList.thellnd; }

inline SgExpression * SgInputOutputStmt::itemList()
{ return LlndMapping(BIF_LL1(thebif)); }

inline void SgInputOutputStmt::setItemList(SgExpression &itemList)
{ BIF_LL1(thebif) = itemList.thellnd; }

inline SgInputOutputStmt::~SgInputOutputStmt()
{ RemoveFromTableBfnd((void *) this); }



// SgIOControlStmt--inlines

inline SgExpression * SgIOControlStmt::controlSpecList()
{ return LlndMapping(BIF_LL2(thebif)); }

inline void SgIOControlStmt::setControlSpecList(SgExpression &controlSpecList)
{ BIF_LL2(thebif) = controlSpecList.thellnd; }

inline SgIOControlStmt::~SgIOControlStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgDeclarationStatement--inlines
inline SgDeclarationStatement::SgDeclarationStatement(int variant):SgStatement(variant)
{}

inline SgDeclarationStatement::~SgDeclarationStatement()
{ RemoveFromTableBfnd((void *) this); }

inline SgExpression * SgDeclarationStatement::varList()
{ return LlndMapping(BIF_LL1(thebif)); }

inline int SgDeclarationStatement::numberOfVars()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgExpression * SgDeclarationStatement::var(int i)
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif),i)); }

inline void SgDeclarationStatement::deleteVar(int i)
{ BIF_LL1(thebif) = deleteNodeInExprList(BIF_LL1(thebif), i); }

inline void SgDeclarationStatement::deleteTheVar(SgExpression &var)
{
 BIF_LL1(thebif) = deleteNodeWithItemInExprList(BIF_LL1(thebif),var.thellnd); 
}

inline void SgDeclarationStatement::addVar(SgExpression &exp)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), exp.thellnd); }



// SgVarDeclStmt--inlines

inline SgVarDeclStmt::SgVarDeclStmt(SgExpression &varRefValList, SgExpression &attributeList, SgType &type):SgDeclarationStatement(VAR_DECL)
{
  if ( CurrentProject->Fortranlanguage() )
    {
      BIF_LL1(thebif) = varRefValList.thellnd;
      BIF_LL2(thebif) = (PTR_LLND) newNode(TYPE_OP);
      NODE_TYPE(BIF_LL2(thebif)) =  type.thetype;
      BIF_LL3(thebif) = attributeList.thellnd;
    }
  else /* C or C++ */
    {
      BIF_LL1(thebif) = varRefValList.thellnd;
      NODE_TYPE(BIF_LL1(thebif)) = type.thetype;
    }
}

inline SgVarDeclStmt::SgVarDeclStmt(SgExpression &varRefValList, SgType &type):SgDeclarationStatement(VAR_DECL)
{
  if ( CurrentProject->Fortranlanguage ())
    {
      BIF_LL1(thebif) = varRefValList.thellnd;
      BIF_LL2(thebif) = newExpr(TYPE_OP,type.thetype);
      BIF_LL3(thebif) = LLNULL;
    }
  else /* C or C++ */
    {
      BIF_LL1(thebif) = varRefValList.thellnd;
      NODE_TYPE(BIF_LL1(thebif)) = type.thetype;
    }
}

inline SgVarDeclStmt::SgVarDeclStmt(SgExpression &varRefValList)
     :SgDeclarationStatement(VAR_DECL)
{
  if ( CurrentProject->Fortranlanguage ())
    {
      BIF_LL1(thebif) = varRefValList.thellnd;
      BIF_LL2(thebif) = LLNULL;
      BIF_LL3(thebif) = LLNULL;
    }
  else /* C or C++ */
    {
      BIF_LL1(thebif) = varRefValList.thellnd;
      NODE_TYPE(BIF_LL1(thebif)) = TYNULL;
    }
}

inline SgVarDeclStmt::~SgVarDeclStmt()
{ RemoveFromTableBfnd((void *) this); }

inline SgType * SgVarDeclStmt::type()  // the type
{ 
  SgType *x;

  if ( CurrentProject->Fortranlanguage() )
    {
      if (BIF_LL2(thebif))
	x = TypeMapping(NODE_TYPE(BIF_LL2(thebif))); 
      else
	x = NULL;
    }
  else /* C or C++ */
    {
       if (BIF_LL1(thebif))
	    x = TypeMapping(NODE_TYPE(BIF_LL1(thebif))); 
       else
	    x = NULL;
    }
  return x;
}


// the number of F90 attributes
inline int SgVarDeclStmt::numberOfAttributes()
{ return exprListLength(BIF_LL3(thebif)); }

// the number of variables declared
inline int SgVarDeclStmt::numberOfSymbols()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgSymbol * SgVarDeclStmt::symbol(int i)
{
  PTR_LLND  pt;
  PTR_SYMB symb = NULL;
  SgSymbol *x;

  pt = getPositionInExprList(BIF_LL1(thebif),i);
  if (pt)
    pt = giveLlSymbInDeclList(pt);
  if (pt && (symb= NODE_SYMB(pt)))
    {
      x = SymbMapping(symb);
    }
  else
    x = NULL;

  return x;
}
     
inline void SgVarDeclStmt::deleteSymbol(int i)
{ BIF_LL1(thebif) = deleteNodeInExprList(BIF_LL1(thebif),i); }

#ifdef NOT_YET_IMPLEMENTED
inline void SgVarDeclStmt::deleteTheSymbol(SgSymbol &symbol)
{ SORRY; }
#endif

// the initial value ofthe i-th variable
inline SgExpression * SgVarDeclStmt::initialValue(int i)
{ 
  PTR_LLND varRefExp;
  SgExpression *x;

  varRefExp = getPositionInExprList(BIF_LL1(thebif),i);
  if (varRefExp == LLNULL)
    x = NULL;
  else if (NODE_CODE(varRefExp) == ASSGN_OP)
    x = LlndMapping(NODE_OPERAND1(varRefExp));
  else 
    x = NULL;

  return x;
}


// SgIntentStmt--inlines

inline SgIntentStmt::SgIntentStmt(SgExpression &varRefValList,
                                  SgExpression &attribute)
                    :SgDeclarationStatement(INTENT_STMT)
{
  BIF_LL1(thebif) = varRefValList.thellnd;
  BIF_LL2(thebif) = attribute.thellnd;
}

inline SgIntentStmt::~SgIntentStmt()
{ RemoveFromTableBfnd((void *) this); }

inline int SgIntentStmt::numberOfArgs()  // the number of arguement expressions
{ return exprListLength(BIF_LL1(thebif)); }

inline void SgIntentStmt::addArg(SgExpression &arg)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),arg.thellnd); }

inline SgExpression * SgIntentStmt::args()
{ return LlndMapping(BIF_LL1(thebif)); }

inline SgExpression * SgIntentStmt::arg(int i) // the i-th argument expression
{ return LlndMapping(getPositionInExprList(BIF_LL1(thebif), i)); }

inline SgExpression * SgIntentStmt::attribute()
{ return LlndMapping(BIF_LL2(thebif)); }


// SgVarListDeclStmt--inlines

inline SgVarListDeclStmt::~SgVarListDeclStmt()
{ RemoveFromTableBfnd((void *) this); }

// the number of variables declared
inline int SgVarListDeclStmt::numberOfSymbols()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgSymbol * SgVarListDeclStmt::symbol(int i)   // the i-th variable
{ 
  PTR_LLND pt;
  SgSymbol *x;
  pt = getPositionInExprList(BIF_LL1(thebif),i);
  if (pt)
    x = SymbMapping(NODE_SYMB(pt)); 
  else
    x = NULL;

  return x;
}

inline void SgVarListDeclStmt::appendSymbol(SgSymbol &symbol)
{ 
  BIF_LL1(thebif) =  addSymbRefToExprList(BIF_LL1(thebif), symbol.thesymb); 
}

inline void SgVarListDeclStmt::deleteSymbol(int i)
{ BIF_LL1(thebif) = deleteNodeInExprList(BIF_LL1(thebif), i); }

#ifdef NOT_YET_IMPLEMENTED
inline void SgVarListDeclStmt::deleteTheSymbol(SgSymbol &symbol)
{ SORRY; }
#endif


// SgStructureDeclStmt--inlines

inline SgStructureDeclStmt::SgStructureDeclStmt(SgSymbol &name, SgExpression &attributes, SgStatement &body):SgDeclarationStatement(STRUCT_DECL)
{
  BIF_SYMB(thebif) = name.thesymb;
  BIF_LL1(thebif) = attributes.thellnd;
  insertBfndListIn(body.thebif,thebif,thebif);             
}

inline SgStructureDeclStmt::~SgStructureDeclStmt()
{ RemoveFromTableBfnd((void *) this); }


// SgNestedVarListDeclStmt--inlines

       
// varList must be of low-level variant appropriate to variant. For example,
// if the variant is COMM_STAT, listOfVarList must be of variant COMM_LIST.

inline SgNestedVarListDeclStmt::~SgNestedVarListDeclStmt()
{ RemoveFromTableBfnd((void *) this); }
 
inline SgExpression * SgNestedVarListDeclStmt::lists()
{ return LlndMapping(BIF_LL1(thebif)); }

inline int SgNestedVarListDeclStmt::numberOfLists()
{ return exprListLength(BIF_LL1(thebif)); }

inline SgExpression * SgNestedVarListDeclStmt::list(int i)
{ return LlndMapping(getPositionInExprList( BIF_LL1(thebif),i)); }

inline void SgNestedVarListDeclStmt::addList(SgExpression &list)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), list.thellnd); }

inline void SgNestedVarListDeclStmt::addVarToList(SgExpression &varRef)
{ 
  BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif),varRef.thellnd);
}

inline void SgNestedVarListDeclStmt::deleteList(int i)
{ 
  BIF_LL1(thebif) = deleteNodeInExprList(BIF_LL1(thebif), i);
}

#ifdef NOT_YET_IMPLEMENTED
inline void SgNestedVarListDeclStmt::deleteTheList(SgExpression &list)
{ 
  //            deleteNodeWithItemInExprList(BIF_LL1(thebif), list.thellnd);
  SORRY;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline void SgNestedVarListDeclStmt::deleteVarInList(int i, SgExpression &varRef)
{ 
  SORRY;
}
#endif

#ifdef NOT_YET_IMPLEMENTED
inline void SgNestedVarListDeclStmt::deleteVarInTheList(SgExpression &list, SgExpression &varRef)
{ 
  SORRY;
}
#endif


// SgParameterStmt--inlines

#ifdef NOT_YET_IMPLEMENTED
inline SgParameterStmt::SgParameterStmt(SgExpression &constants, SgExpression &values):SgDeclarationStatement(PARAM_DECL)
{
  //  PTR_LLND constantWithValues;
  
  //            constantWithValues = stringConstantsWithTheirValues(constants.thellnd, values.thellnd);
  //            BIF_LL1(thebif) = LlndMapping(constantWithValues);
  SORRY;
}
#endif

inline SgParameterStmt::SgParameterStmt(SgExpression &constantsWithValues):SgDeclarationStatement(PARAM_DECL)
{ BIF_LL1(thebif) = constantsWithValues.thellnd; }

inline SgParameterStmt::~SgParameterStmt()
{ RemoveFromTableBfnd((void *) this); }

// the number of constants declared
inline int SgParameterStmt::numberOfConstants()
{ return exprListLength(BIF_LL1(thebif)); }

// the i-th variable
inline SgSymbol * SgParameterStmt::constant(int i)
{ return SymbMapping(NODE_SYMB(getPositionInExprList(BIF_LL1(thebif),i))); }

// the value of i-th variable
inline SgExpression * SgParameterStmt::value(int i)
{ return LlndMapping(SYMB_VAL(NODE_SYMB(getPositionInExprList(BIF_LL1(thebif),i)))); }

#ifdef NOT_YET_IMPLEMENTED
inline void SgParameterStmt::addConstant(SgSymbol &constant, SgExpression &value)

{ 
  //            SgRefExp constNode(constant, value);
  //            BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), constNode.thellnd);
  SORRY;
};
#endif
  
inline void SgParameterStmt::deleteConstant(int i)
{ BIF_LL1(thebif) = deleteNodeInExprList(BIF_LL1(thebif), i); }

#ifdef NOT_YET_IMPLEMENTED
inline void SgParameterStmt::deleteTheConstant(SgSymbol &constant)
{ 
  // deleteNodeWithSymbolInExprList(i, BIF_LL1(thebif)); 
  SORRY;
}
#endif


// SgImplicitStmt--inlines

inline SgImplicitStmt::SgImplicitStmt(SgExpression &implicitLists):SgDeclarationStatement(IMPL_DECL)
{ BIF_LL1(thebif) = implicitLists.thellnd; }

inline SgImplicitStmt::SgImplicitStmt(SgExpression *implicitLists):SgDeclarationStatement(IMPL_DECL)
{
    if (implicitLists)
        BIF_LL1(thebif) = implicitLists->thellnd; 
}

inline SgImplicitStmt::~SgImplicitStmt()
{ RemoveFromTableBfnd((void *) this); }

// the number of implicit types declared
inline int SgImplicitStmt::numberOfImplicitTypes()
{ return exprListLength(BIF_LL1(thebif)); }

// the i-th implicit type
inline SgType * SgImplicitStmt::implicitType(int i)
{ 
  PTR_LLND pt;
  SgType *x;

  if ( (pt = getPositionInList(BIF_LL1(thebif),i)) &&
      NODE_OPERAND0(pt))
    x = TypeMapping(NODE_TYPE(NODE_OPERAND0(pt))); 
  else
    x = NULL;

  return x;
}

// the i-th implicit type's range list eg. (A-E, G)
inline SgExpression * SgImplicitStmt::implicitRangeList(int i)
{ 
  PTR_LLND pt;
  SgExpression *x;

  if ( (pt = getPositionInExprList(BIF_LL1(thebif),i)) &&
      NODE_OPERAND0(pt))
    x = LlndMapping(NODE_OPERAND0(pt)); 
  else
    x = NULL;

  return x;
}

inline void SgImplicitStmt::appendImplicitNode(SgExpression &impNode)
{ BIF_LL1(thebif) = addToExprList(BIF_LL1(thebif), impNode.thellnd); }



// SgVariableSymb--inlines


inline SgVariableSymb::SgVariableSymb(char *identifier, SgType &t, SgStatement &scope):SgSymbol(VARIABLE_NAME,identifier)
{
  SYMB_SCOPE(thesymb) = scope.thebif;
  SYMB_TYPE(thesymb) = t.thetype;
}

inline SgVariableSymb::SgVariableSymb(char *identifier, SgType *t, SgStatement *scope):SgSymbol(VARIABLE_NAME,identifier)
{
  if (scope)
    SYMB_SCOPE(thesymb) = scope->thebif;
  if (t)
    SYMB_TYPE(thesymb) = t->thetype;
}

inline SgVariableSymb::SgVariableSymb(char *identifier, 
                                SgType &t):SgSymbol(VARIABLE_NAME,identifier)
{ SYMB_TYPE(thesymb) = t.thetype; }


inline SgVariableSymb::SgVariableSymb(char *identifier,
                         SgStatement &scope):SgSymbol(VARIABLE_NAME,identifier)
{    SYMB_SCOPE(thesymb) = scope.thebif;}


inline SgVariableSymb::SgVariableSymb(char *identifier,
                         SgStatement *scope):SgSymbol(VARIABLE_NAME,identifier)
{    SYMB_SCOPE(thesymb) = scope->thebif;}


inline SgVariableSymb::SgVariableSymb(char *identifier):
                                   SgSymbol(VARIABLE_NAME,identifier)
{}

inline SgVariableSymb::SgVariableSymb(const char *identifier, SgType &t, SgStatement &scope) : SgSymbol(VARIABLE_NAME, identifier)
{
    SYMB_SCOPE(thesymb) = scope.thebif;
    SYMB_TYPE(thesymb) = t.thetype;
}

inline SgVariableSymb::SgVariableSymb(const char *identifier, SgType *t, SgStatement *scope) :SgSymbol(VARIABLE_NAME, identifier)
{
    if (scope)
        SYMB_SCOPE(thesymb) = scope->thebif;
    if (t)
        SYMB_TYPE(thesymb) = t->thetype;
}

inline SgVariableSymb::SgVariableSymb(const char *identifier,
    SgType &t) :SgSymbol(VARIABLE_NAME, identifier)
{
    SYMB_TYPE(thesymb) = t.thetype;
}


inline SgVariableSymb::SgVariableSymb(const char *identifier,
    SgStatement &scope) :SgSymbol(VARIABLE_NAME, identifier)
{
    SYMB_SCOPE(thesymb) = scope.thebif;
}


inline SgVariableSymb::SgVariableSymb(const char *identifier,
    SgStatement *scope) :SgSymbol(VARIABLE_NAME, identifier)
{
    SYMB_SCOPE(thesymb) = scope->thebif;
}


inline SgVariableSymb::SgVariableSymb(const char *identifier) :
SgSymbol(VARIABLE_NAME, identifier)
{}

inline SgVariableSymb::~SgVariableSymb()
{ RemoveFromTableSymb((void *) this); }

/* ajm */
inline SgVarRefExp *SgVariableSymb::varRef(void)
{
     return new SgVarRefExp (*this);
}


// SgConstantSymb--inlines

inline SgConstantSymb::SgConstantSymb(char *identifier, SgStatement &scope, 
                                   SgExpression &value):SgSymbol(CONST_NAME,identifier, scope)
{ SYMB_VAL(thesymb) = value.thellnd; }

inline SgConstantSymb::SgConstantSymb(const char *identifier, SgStatement &scope, 
                                   SgExpression &value):SgSymbol(CONST_NAME,identifier, scope)
{ SYMB_VAL(thesymb) = value.thellnd; }

inline SgConstantSymb::~SgConstantSymb()
{ RemoveFromTableSymb((void *) this); }

inline SgExpression * SgConstantSymb::constantValue()
{ return LlndMapping(SYMB_VAL(thesymb)); }


// SgFunctionSymb--inlines

inline SgFunctionSymb::~SgFunctionSymb()
{ RemoveFromTableSymb((void *) this); }

inline void SgFunctionSymb::addParameter(int, SgSymbol &parameters)
{ 
     SgSymbol *copy_symb = &(parameters.copy());
     SYMB_NEXT_DECL(copy_symb->thesymb) = 0;
     appendSymbToArgList (thesymb,copy_symb->thesymb);
}

inline void SgFunctionSymb::insertParameter(int position, SgSymbol &symb)
{ insertSymbInArgList (this->thesymb, position, symb.thesymb);  }

inline int SgFunctionSymb::numberOfParameters()
{ return lenghtOfParamList(thesymb); }

inline SgSymbol * SgFunctionSymb::parameter(int i)
{ return SymbMapping(GetThParam(thesymb,i)); }

inline SgSymbol * SgFunctionSymb::result()
{ return SymbMapping(SYMB_DECLARED_NAME(thesymb)); }

inline void SgFunctionSymb::setResult(SgSymbol &symbol)
{ SYMB_DECLARED_NAME(thesymb) = symbol.thesymb; }


// SgMemberFuncSymb--inlines
//  status = MEMB_<see above>;
inline SgMemberFuncSymb::SgMemberFuncSymb(char *identifier, SgType &t,
                                        SgStatement &cla, int status):
                       SgFunctionSymb(MEMBER_FUNC, identifier, t, cla)
{
  SYMB_ATTR(thesymb) = status;
  SYMB_MEMBER_BASENAME(thesymb) = BIF_SYMB(cla.thebif);
}

inline SgMemberFuncSymb::~SgMemberFuncSymb()
{ RemoveFromTableSymb((void *) this); }

inline int SgMemberFuncSymb::isMethodOfElement()
{
  int x;
  if ((int) SYMB_ATTR(thesymb) & (int) ELEMENT_FIELD)
    x = TRUE;
  else
    x = FALSE;

  return x;
}

// name of containing class;
inline SgSymbol * SgMemberFuncSymb::className()
{
  return SymbMapping(SYMB_MEMBER_BASENAME(thesymb));
}

// name of containing class
inline void SgMemberFuncSymb::setClassName(SgSymbol &symb)
{
  SYMB_MEMBER_BASENAME(thesymb) = symb.thesymb;
}
          

// SgFieldSymb--inlines

inline SgFieldSymb::SgFieldSymb(char *identifier, SgType &t, 
        SgSymbol &structureName):SgSymbol(FIELD_NAME,identifier)
{
      SYMB_TYPE(thesymb) = t.thetype; 
      SYMB_FIELD_BASENAME(thesymb) = structureName.thesymb; 
}

inline SgFieldSymb::SgFieldSymb(const char *identifier, SgType &t,
    SgSymbol &structureName) :SgSymbol(FIELD_NAME, identifier)
{
    SYMB_TYPE(thesymb) = t.thetype;
    SYMB_FIELD_BASENAME(thesymb) = structureName.thesymb; 
}

inline SgFieldSymb::~SgFieldSymb()
{ RemoveFromTableSymb((void *) this); }

// position in the structure
#ifdef NOT_YET_IMPLEMENTED
inline int SgFieldSymb::offset()
{
  // return positionOfFieldInStruct(thesymb, SYMB_BASE_NAME(thesymb));
  SORRY;
  return 0;
}
#endif

// parent structure
inline SgSymbol * SgFieldSymb::structureName()
{ return SymbMapping(SYMB_FIELD_BASENAME(thesymb)); }

inline SgSymbol * SgFieldSymb::nextField()
{ return SymbMapping(getClassNextFieldOrMember(thesymb)); }

inline int SgFieldSymb::isMethodOfElement()
{
  int x;

  if ((int) SYMB_ATTR(thesymb) & (int) ELEMENT_FIELD)
    x = TRUE;
  else
    x = FALSE;

  return x;
}


// SgClassSymb--inlines

inline SgClassSymb::SgClassSymb(int variant, char *name, 
         SgStatement &scope):SgSymbol(variant, name, scope)
{}

inline SgClassSymb::~SgClassSymb()
{ RemoveFromTableSymb((void *) this); }

// number of fields and member functions.
inline int SgClassSymb::numberOfFields()
{ return lenghtOfFieldList(thesymb);}

// the i-th field or member function.
inline SgSymbol * SgClassSymb::field(int i)
{ return SymbMapping(GetThOfFieldList(thesymb,i)); }


// SgLabelSymb--inlines

#ifdef NOT_YET_IMPLEMENTED
inline SgLabelSymb::SgLabelSymb(char *name):SgSymbol(LABEL_NAME)
{
  SORRY;
}
#endif

inline SgLabelSymb::~SgLabelSymb()
{ RemoveFromTableSymb((void *) this); }


inline SgLabelVarSymb::SgLabelVarSymb(char *name, SgStatement &scope):SgSymbol(LABEL_NAME, name, scope)
{}
  
inline SgLabelVarSymb::~SgLabelVarSymb()
{ RemoveFromTableSymb((void *) this); }


// SgExternalSymb--inlines
inline SgExternalSymb::SgExternalSymb(char *name, SgStatement &scope):SgSymbol(ROUTINE_NAME, name, scope)
{}

inline SgExternalSymb::~SgExternalSymb()
{ RemoveFromTableSymb((void *) this); }


// SgConstructSymb--inlines

inline SgConstructSymb::SgConstructSymb(char *name, SgStatement &scope):SgSymbol(CONSTRUCT_NAME, name, scope)
{}
        
inline SgConstructSymb::~SgConstructSymb()
{ RemoveFromTableSymb((void *) this); }


// SgInterfaceSymb--inlines

inline SgInterfaceSymb::SgInterfaceSymb(char *name, SgStatement &scope):SgSymbol(INTERFACE_NAME, name, scope)
{}

inline SgInterfaceSymb::~SgInterfaceSymb()
{ RemoveFromTableSymb((void *) this); }


// SgModuleSymb--inlines
inline SgModuleSymb::SgModuleSymb(char *name):SgSymbol(MODULE_NAME, name, *BfndMapping(getFirstStmt()))
{}

inline SgModuleSymb::~SgModuleSymb()
{ RemoveFromTableSymb((void *) this); }


// SgArrayType--inlines

inline SgArrayType::SgArrayType(SgType &base_type):SgType(T_ARRAY)
{ TYPE_BASE(thetype) = base_type.thetype; }

inline int SgArrayType::dimension()
{ return exprListLength(TYPE_RANGES(thetype)); }

inline SgExpression * SgArrayType::sizeInDim(int i)
{ return LlndMapping(getPositionInExprList(TYPE_RANGES(thetype),i)); }

inline SgType * SgArrayType::baseType()
{
 return TypeMapping(lookForInternalBasetype(thetype));
 // perhaps should be return TYPE_BASE(thetype);
}

inline void SgArrayType::setBaseType(SgType &bt)
{ TYPE_BASE(thetype) = bt.thetype; }

inline void SgArrayType::addDimension(SgExpression *e)
{
 if(!e){
  SgExprListExp *l = new SgExprListExp();
  TYPE_RANGES(thetype) = l->thellnd;
  }
 else
  TYPE_RANGES(thetype) = addToExprList(TYPE_RANGES(thetype),e->thellnd);
}
inline SgExpression * SgArrayType::getDimList()
{
  return LlndMapping(TYPE_RANGES(thetype));
}
inline void SgArrayType::addRange(SgExpression &e)
{
  TYPE_RANGES(thetype) = addToExprList(TYPE_RANGES(thetype),e.thellnd);
  // For C when adding range adding one level of pointer in basetype.
  // This routine should only be used to build a dereferencing expression
  // like x[i][j] and not a declaration.  use addDimension for that.
    if (!CurrentProject->Fortranlanguage())
      {
        PTR_TYPE type;
        type = (PTR_TYPE) newNode(T_POINTER);
        TYPE_BASE(type) = TYPE_BASE(thetype);
        TYPE_BASE(thetype) = type;
      }
}

inline SgArrayType::~SgArrayType()
{ RemoveFromTableType((void *) this); }


// SgPointerType--inlines

inline SgType * SgPointerType::baseType()
{ return TypeMapping(TYPE_BASE(thetype)); }

inline void SgPointerType::setBaseType(SgType &baseType)
{ TYPE_BASE(thetype) = baseType.thetype; }

inline int SgPointerType::indirection()
{ return TYPE_TEMPLATE_DUMMY1(thetype); }

inline void SgPointerType::setIndirection(int i)
{ TYPE_TEMPLATE_DUMMY1(thetype) = i; }

inline SgPointerType::~SgPointerType()
{ RemoveFromTableType((void *) this); }

inline int SgPointerType::modifierFlag()
{ return TYPE_TEMPLATE_DUMMY5(thetype); }

inline void SgPointerType::setModifierFlag(int flag) 
{ TYPE_TEMPLATE_DUMMY5(thetype) = TYPE_TEMPLATE_DUMMY5(thetype) | flag; }


// SgFunctionType-- inlines

inline SgFunctionType::SgFunctionType(SgType &ret_val):SgType(T_FUNCTION)
{ TYPE_BASE(thetype) = ret_val.thetype; }

inline SgType * SgFunctionType::returnedValue()
{ return TypeMapping(TYPE_BASE(thetype)); }

inline void SgFunctionType::changeReturnedValue(SgType &ret_val)
{ TYPE_BASE(thetype) = ret_val.thetype; }

inline SgFunctionType::~SgFunctionType()
{ RemoveFromTableType((void *) this); }

// SgReferenceType--inlines

inline SgReferenceType::SgReferenceType(SgType &base_type):SgType(T_REFERENCE)
{ TYPE_BASE(thetype) = base_type.thetype; }

inline SgType * SgReferenceType::baseType()
{ return TypeMapping(TYPE_BASE(thetype)); }

inline void SgReferenceType::setBaseType(SgType &baseType)
{ TYPE_BASE(thetype) = baseType.thetype; }

inline SgReferenceType::~SgReferenceType()
{ RemoveFromTableType((void *) this); }

inline int SgReferenceType::modifierFlag()
{ return TYPE_TEMPLATE_DUMMY5(thetype); }

inline void SgReferenceType::setModifierFlag(int flag) 
{ TYPE_TEMPLATE_DUMMY5(thetype) = TYPE_TEMPLATE_DUMMY5(thetype) | flag; }


// SgDerivedType--inlines

inline SgDerivedType::SgDerivedType(SgSymbol &type_name):SgType(T_DERIVED_TYPE)
{ TYPE_SYMB_DERIVE(thetype) = type_name.thesymb; }

inline SgSymbol * SgDerivedType::typeName()
{ return SymbMapping(TYPE_SYMB_DERIVE(thetype)); }

inline SgDerivedType::~SgDerivedType()
{ RemoveFromTableType((void *) this); }


// SgDerivedClassType--inlines

inline SgDerivedClassType::SgDerivedClassType(SgSymbol &type_name):SgType(T_DERIVED_CLASS)
{ TYPE_SYMB_DERIVE(thetype) = type_name.thesymb; }

inline SgSymbol * SgDerivedClassType::typeName()
{ return SymbMapping(TYPE_SYMB_DERIVE(thetype)); }

inline SgDerivedClassType::~SgDerivedClassType()
{ RemoveFromTableType((void *) this); }


// SgDescriptType--inlines


inline SgDescriptType::SgDescriptType(SgType &base_type, int bit_flag):SgType(T_DESCRIPT)
{
  TYPE_LONG_SHORT(thetype) =  bit_flag;
  TYPE_BASE(thetype) = base_type.thetype;
}

inline int SgDescriptType::modifierFlag()
{ return TYPE_LONG_SHORT(thetype); }

inline void SgDescriptType::setModifierFlag(int flag) 
{ TYPE_LONG_SHORT(thetype) = TYPE_LONG_SHORT(thetype) | flag; }

inline SgDescriptType::~SgDescriptType()
{ RemoveFromTableType((void *) this); }



// SgDerivedCollectionType--inlines

inline SgDerivedCollectionType::SgDerivedCollectionType(SgSymbol &s, SgType &t):SgType(T_DERIVED_COLLECTION)
{
  TYPE_COLL_BASE(thetype) = t.thetype;
  TYPE_SYMB_DERIVE(thetype) = s.thesymb;
}

inline SgType * SgDerivedCollectionType::elementClass() 
{ return TypeMapping(TYPE_COLL_BASE(thetype)); }

inline void SgDerivedCollectionType::setElementClass(SgType &ty)
{ TYPE_COLL_BASE(thetype) = ty.thetype; }

inline SgSymbol * SgDerivedCollectionType::collectionName()
{  return SymbMapping(TYPE_SYMB_DERIVE(thetype)); }

inline SgStatement * SgDerivedCollectionType::createCollectionWithElemType()
{
  return BfndMapping(LibcreateCollectionWithType(thetype,TYPE_COLL_BASE(thetype)));
}

inline SgDerivedCollectionType::~SgDerivedCollectionType()
{ RemoveFromTableType((void *) this); }

void InitializeTable();

#ifdef USER

SgType *SgTypeInt();
SgType *SgTypeChar();
SgType *SgTypeFloat();
SgType *SgTypeDouble();
SgType *SgTypeVoid();
SgType *SgTypeBool();
SgType *SgTypeDefault();

SgUnaryExp & SgDerefOp(SgExpression &e);
SgUnaryExp & SgAddrOp(SgExpression &e);
SgUnaryExp & SgUMinusOp(SgExpression &e);
SgUnaryExp & SgUPlusOp(SgExpression &e);
SgUnaryExp & SgPrePlusPlusOp(SgExpression &e);
SgUnaryExp & SgPreMinusMinusOp(SgExpression &e);
SgUnaryExp & SgPostPlusPlusOp(SgExpression &e);
SgUnaryExp & SgPostMinusMinusOp(SgExpression &e);
SgUnaryExp & SgBitCompfOp(SgExpression &e);
SgUnaryExp & SgNotOp(SgExpression &e);
SgUnaryExp & SgSizeOfOp(SgExpression &e);
SgUnaryExp & makeAnUnaryExpression(int code,PTR_LLND ll1);


SgValueExp *             isSgValueExp(SgExpression *pt);
SgKeywordValExp *        isSgKeywordValExp(SgExpression *pt);
SgUnaryExp *             isSgUnaryExp(SgExpression *pt);
SgCastExp *              isSgCastExp(SgExpression *pt);
SgDeleteExp *            isSgDeleteExp(SgExpression *pt);
SgNewExp *               isSgNewExp(SgExpression *pt);
SgExprIfExp *            isSgExprIfExp(SgExpression *pt);
SgFunctionCallExp *      isSgFunctionCallExp(SgExpression *pt);
SgFuncPntrExp *          isSgFuncPntrExp(SgExpression *pt);
SgExprListExp *          isSgExprListExp(SgExpression *pt);
SgRefExp *               isSgRefExp (SgExpression *pt);
SgVarRefExp *            isSgVarRefExp (SgExpression *pt);
SgThisExp *              isSgThisExp (SgExpression *pt);
SgArrayRefExp *          isSgArrayRefExp (SgExpression *pt);
SgPntrArrRefExp *        isSgPntrArrRefExp(SgExpression *pt);
SgPointerDerefExp *      isSgPointerDerefExp (SgExpression *pt);
SgRecordRefExp *         isSgRecordRefExp (SgExpression *pt);
SgStructConstExp*        isSgStructConstExp (SgExpression *pt);
SgConstExp*              isSgConstExp (SgExpression *pt);
SgVecConstExp *          isSgVecConstExp (SgExpression *pt);
SgInitListExp *          isSgInitListExp (SgExpression *pt);
SgObjectListExp *        isSgObjectListExp (SgExpression *pt);
SgAttributeExp *         isSgAttributeExp (SgExpression *pt);
SgKeywordArgExp *        isSgKeywordArgExp (SgExpression *pt);
SgSubscriptExp*          isSgSubscriptExp (SgExpression *pt);
SgUseOnlyExp *           isSgUseOnlyExp (SgExpression *pt);
SgUseRenameExp *         isSgUseRenameExp (SgExpression *pt);
SgSpecPairExp *          isSgSpecPairExp (SgExpression *pt);
SgIOAccessExp *          isSgIOAccessExp (SgExpression *pt);
SgImplicitTypeExp *      isSgImplicitTypeExp (SgExpression *pt);
SgTypeExp *              isSgTypeExp (SgExpression *pt);
SgSeqExp *               isSgSeqExp (SgExpression *pt);
SgStringLengthExp *      isSgStringLengthExp (SgExpression *pt);
SgDefaultExp *           isSgDefaultExp (SgExpression *pt);
SgLabelRefExp *          isSgLabelRefExp (SgExpression *pt);
SgProgHedrStmt *         isSgProgHedrStmt (SgStatement *pt);
SgProcHedrStmt *         isSgProcHedrStmt (SgStatement *pt);
SgFuncHedrStmt *         isSgFuncHedrStmt (SgStatement *pt);
SgClassStmt *            isSgClassStmt (SgStatement *pt);
SgStructStmt *           isSgStructStmt (SgStatement *pt);
SgUnionStmt *            isSgUnionStmt (SgStatement *pt);
SgEnumStmt *             isSgEnumStmt (SgStatement *pt);
SgCollectionStmt *       isSgCollectionStmt (SgStatement *pt);
SgBasicBlockStmt *       isSgBasicBlockStmt (SgStatement *pt);
SgForStmt *              isSgForStmt (SgStatement *pt);
SgWhileStmt *            isSgWhileStmt (SgStatement *pt);
SgDoWhileStmt *          isSgDoWhileStmt (SgStatement *pt);
SgLogIfStmt *            isSgLogIfStmt (SgStatement *pt);
SgIfStmt *               isSgIfStmt (SgStatement *pt);
SgArithIfStmt *          isSgArithIfStmt (SgStatement *pt);
SgWhereStmt *            isSgWhereStmt (SgStatement *pt);
SgWhereBlockStmt *       isSgWhereBlockStmt (SgStatement *pt);
SgSwitchStmt *           isSgSwitchStmt (SgStatement *pt);
SgCaseOptionStmt *       isSgCaseOptionStmt (SgStatement *pt);
SgExecutableStatement *  isSgExecutableStatement (SgStatement *pt);
SgAssignStmt *           isSgAssignStmt (SgStatement *pt);
SgCExpStmt *             isSgCExpStmt (SgStatement *pt);
SgPointerAssignStmt *    isSgPointerAssignStmt (SgStatement *pt);
SgHeapStmt *             isSgHeapStmt (SgStatement *pt);
SgNullifyStmt *          isSgNullifyStmt (SgStatement *pt);
SgContinueStmt *         isSgContinueStmt (SgStatement *pt);
SgControlEndStmt *       isSgControlEndStmt (SgStatement *pt);
SgBreakStmt *            isSgBreakStmt (SgStatement *pt);
SgCycleStmt *            isSgCycleStmt (SgStatement *pt);
SgReturnStmt *           isSgReturnStmt (SgStatement *pt);
SgExitStmt *             isSgExitStmt (SgStatement *pt);
SgGotoStmt *             isSgGotoStmt (SgStatement *pt);
SgLabelListStmt *        isSgLabelListStmt (SgStatement *pt);
SgAssignedGotoStmt *     isSgAssignedGotoStmt (SgStatement *pt);
SgComputedGotoStmt *     isSgComputedGotoStmt (SgStatement *pt);
SgStopOrPauseStmt *      isSgStopOrPauseStmt (SgStatement *pt);
SgCallStmt*              isSgCallStmt (SgStatement *pt);
SgProsHedrStmt *	 isSgProsHedrStmt (SgStatement *pt); /* Fortran M */
SgProcessDoStmt *        isSgProcessDoStmt (SgStatement *pt);  /* Fortran M */
SgProsCallStmt*          isSgProsCallStmt (SgStatement *pt); /* Fortran M */
SgProsCallLctn*          isSgProsCallLctn (SgStatement *pt); /* Fortran M */
SgProsCallSubm*          isSgProsCallSubm (SgStatement *pt); /* Fortran M */
SgInportStmt *           isSgInportStmt (SgStatement *pt); /* Fortran M */
SgOutportStmt *          isSgOutportStmt (SgStatement *pt); /* Fortran M */
SgIntentStmt *		 isSgIntentStmt (SgStatement *pt); /* Fortran M */
SgChannelStmt *          isSgChannelStmt (SgStatement *pt); /* Fortran M */
SgMergerStmt *           isSgMergerStmt (SgStatement *pt); /* Fortran M */
SgMoveportStmt *         isSgMoveportStmt (SgStatement *pt); /* Fortran M */
SgSendStmt *             isSgSendStmt (SgStatement *pt); /* Fortran M */
SgReceiveStmt *          isSgReceiveStmt (SgStatement *pt); /* Fortran M */
SgEndchannelStmt *       isSgEndchannelStmt (SgStatement *pt); /* Fortran M */
SgProbeStmt *            isSgProbeStmt (SgStatement *pt); /* Fortran M */
SgProcessorsRefExp *	 isSgProcessorsRefExp(SgExpression *pt); /* Fortran M */
SgPortTypeExp *          isSgPortTypeExp (SgExpression *pt); /* Fortran M */ 
SgInportExp *            isSgInportExp (SgExpression *pt); /* Fortran M */
SgOutportExp *           isSgOutportExp (SgExpression *pt); /* Fortran M */
SgFromportExp *          isSgFromportExp (SgExpression *pt); /* Fortran M */
SgToportExp *            isSgToportExp (SgExpression *pt); /* Fortran M */
SgIO_statStoreExp *      isSgIO_statStoreExp (SgExpression *pt); /* Fortran M */
SgEmptyStoreExp *        isSgEmptyStoreExp (SgExpression *pt); /* Fortran M */
SgErrLabelExp *          isSgErrLabelExp (SgExpression *pt); /* Fortran M */
SgEndLabelExp *          isSgEndLabelExp (SgExpression *pt); /* Fortran M */
SgDataImpliedDoExp *     isSgDataImpliedDoExp (SgExpression *pt);/* Fortran M */
SgDataEltExp *           isSgDataEltExp (SgExpression *pt); /* Fortran M */
SgDataSubsExp *          isSgDataSubsExp (SgExpression *pt); /* Fortran M */
SgDataRangeExp *         isSgDataRangeExp (SgExpression *pt); /* Fortran M */
SgIconExprExp *          isSgIconExprExp (SgExpression *pt); /* Fortran M */
SgIOStmt *               isSgIOStmt (SgStatement *pt);
SgInputOutputStmt *      isSgInputOutputStmt (SgStatement *pt);
SgIOControlStmt *        isSgIOControlStmt (SgStatement *pt);
SgDeclarationStatement * isSgDeclarationStatement (SgStatement *pt);
SgVarDeclStmt *          isSgVarDeclStmt (SgStatement *pt);
SgVarListDeclStmt *      isSgVarListDeclStmt (SgStatement *pt);
SgStructureDeclStmt *    isSgStructureDeclStmt (SgStatement *pt);
SgNestedVarListDeclStmt* isSgNestedVarListDeclStmt (SgStatement *pt);
SgParameterStmt *        isSgParameterStmt (SgStatement *pt);
SgImplicitStmt *         isSgImplicitStmt (SgStatement *pt);
SgVariableSymb *         isSgVariableSymb (SgSymbol *pt);
SgConstantSymb *         isSgConstantSymb (SgSymbol *pt);
SgFunctionSymb *         isSgFunctionSymb (SgSymbol *pt);
SgMemberFuncSymb *       isSgMemberFuncSymb (SgSymbol *pt);
SgFieldSymb *            isSgFieldSymb (SgSymbol *pt);
SgClassSymb *            isSgClassSymb (SgSymbol *pt);
SgLabelSymb *            isSgLabelSymb (SgSymbol *pt);
SgLabelVarSymb *         isSgLabelVarSymb (SgSymbol *pt);
SgExternalSymb *         isSgExternalSymb (SgSymbol *pt);
SgConstructSymb *        isSgConstructSymb (SgSymbol *pt);
SgInterfaceSymb *        isSgInterfaceSymb (SgSymbol *pt);
SgModuleSymb *           isSgModuleSymb (SgSymbol *pt);
SgArrayType *            isSgArrayType (SgType *pt);
SgPointerType *          isSgPointerType (SgType *pt);
SgFunctionType *         isSgFunctionType (SgType *pt);
SgReferenceType *        isSgReferenceType (SgType *pt);
SgDerivedType *          isSgDerivedType (SgType *pt);
SgDerivedClassType *     isSgDerivedClassType (SgType *pt);
SgDescriptType *         isSgDescriptType (SgType *pt);
SgDerivedCollectionType* isSgDerivedCollectionType (SgType *pt);
#endif

#endif /* ndef LIBSAGEXX_H */

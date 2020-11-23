/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

//  ----------------------------------
//  Darryl Brown
//  University of Oregon pC++/Sage++
//
//  sage++classhierarchy.h  -  the header file for the class classHierarchy.
//
//  a class(es) for inspecting the class hierarchy
//  of a sage++ project.
//  
//  ----------------------------------

//  ----------------------------------
//  To traverse the hierarcy of classes, the most obvious approach is
//  in the following example.  This example searches the tree for a given
//  class name and a hierarchy to search.  Note that this searches the whole
//  tree, not just the immediate children.
//
//   classHierarchy *findHierarchy(char *name, classHierarchy *h) {
//     classHierarchy *tmp, *depth;
//     
//     // initialize searchlist of hierarchy immediate children...;
//     // this returns the first hierarchy in the child list...;
//     tmp = (classHierarchy *) h->children->searchList();
//
//     while (tmp) {
//
//       // if they are the same, return the current hierarchy...;
//       if (strcmp(name, tmp->className) == 0) {
//         return tmp;
//       } else {
//         // search tmps children recursively, if not NULL, return that value...;
//         if (depth = findHierarchy(name, tmp)) {
//           return depth;
//         }
//       }
//       // get next item in list;
//       tmp = (classHierarchy *) h->children->nextItem();
//     }
//     //  if weve made it to here, it is not anywhere in the hierarchy,
//     //  so return NULL;
//     return NULL;
//   }
// 
//   -------------------------------------------------------
//   There is also a list of the classMembers for each class.  To traverse
//   that list, it is very similar, but more simple than the above example.
//   Here is an example of printing out each class member of a specific
//   member type (e.g. public function).
//
// virtual void printMemberType(memberType mt, classHierarchy *h) {
//   classMember *tmp;
//   
//   tmp = (classMember *) h->classMembers->searchList();
//
//   while (tmp) {
//     if (tmp->typeVariant == mt) {
//       tmp->print();
//     }
//     tmp = (classMember *) h->classMembers->nextItem();
//   }
// }
//


//  -------------------------------------------------------------
//  Forward declarations;
//
class relationList;   

//  -------------------------------------------------------------
//  Extern declarations
//
//
extern int strToType(char *s);
extern char *typeToStr(int ty);


// --------------------
//  type of class members...;
typedef enum {
  UNKNOWN_FUNC,
  PRIVATE_FUNC,
  PUBLIC_FUNC,
  PROTECTED_FUNC,
  ELEMENT_FUNC,
  UNKNOWN_VAR,
  PRIVATE_VAR,
  PUBLIC_VAR,
  PROTECTED_VAR,
  ELEMENT_VAR
  } memberType;

//  -------------------------------------------------------------
// the main class for accessing the class hierarchy within a sage++
// file.
class classHierarchy : public brk_basePtr {

 private:

  //  private functions
  virtual classHierarchy *findClassHierarchy(char *cl); 
    //returns the hierarchy of the class with className cl;
  classHierarchy *pushOnTop(SgClassStmt *clSt);
    // creates a new hierarchy for clSt (a class declarative statement);
    // and puts it at the highest level of the hierarchy (exclusively ;
    // for classes with no superclasses) ;
  virtual classHierarchy * storeInClassHierarchy (SgClassStmt *clSt);
    // creates a new hierarchy for the class declarative statement clSt;
    // and stores it where it fits in the hierarchy of classes.  It makes
    // use of the above two functions pushOnTop and findHierarchy.;
  void determineMembers(SgFile *aFile);
    // finds all members in a class, initializing publicVars, protectedVars,
    // privateVars, publicFuncs, protectedFuncs, and privateFuncs;
  void allocateLists();
    // allocates new relationList instances for member fields.;

 public:

  // members;
  relationList *parents;  // linked list of parents  ;
  relationList *children;  // linked list of children  ;
  relationList *classMembers;  // linked list of class vars and funcs ;
  char *className;   // contains the class name ;
  SgSymbol *classSymbol;   // contains the Sage symbol for the name;
  SgClassStmt *declaration;   // contains the Sage declaration of the class;

  // constructors;
  classHierarchy(void);
  classHierarchy(char * cn);
  classHierarchy(SgSymbol * cs);
  classHierarchy(SgClassStmt * clSt);

  // access functions;
  virtual void print(int tabs); // prints out this class after <tabs> tabs.;
  virtual void print();   // prints out this class after 0 tabs.;
  virtual void printAll(int tabs); 
     // prints out this class after <tabs> tabs, as well as all descendants;
  virtual void printAllCollections(int tabs); 
     // prints out this class if it is a collection    ;
     // after <tabs> tabs, as well as all descendants;
  virtual void printAll();   
    // prints out this class after 0 tabs, as well as all descendants;
  virtual void printMemberType(memberType mt);
    // prints out all member field/functions of type mt;
  classHierarchy *findMember (brk_basePtr *);   // look for this element and 
    // return the ptrNode that points to it;
  int numParents();  // returns the number of parents;
  int numChildren();  // returns the number of children ;
  void determineClassHierarchy(SgFile *aFile);
    // finds all classes in a file and stores them in a hierarchy.  It makes
    // use of private functions.  Typically, this is the only necessary 
    // function to call when trying to find out a class hierarchy for a file.
  int numberOfDescendants (void); 
    // returns the total number of all descendants;
  int numberOfParents (void);  
    // returns the number of parents of this class;
  int numberOfChildren (void); 
    // returns the number of direct children of this class;
  int isCollection();  
    // returns true if it is a collection, false if not a collection,
    // or if it is not known.;
  char *fileName();  // returns file name where this class is defined if known,
    // NULL if not known.;
  int lineNumber();  // returns line number where this class is defined if known,
    // -1 if not known.;
  virtual int compare(brk_basePtr *);
    // compares this heirarchy with another alphabetically using className;
  void sort ();   // sorts the list, elements must have compare function.,;
  void sort(int (* compareFunc) (brk_basePtr *, brk_basePtr *));
  
};

//  -------------------------------------------------------------
// the class implementing the linked list for 
class relationList : public brk_linkedList {

 public: 

  // constructor;
  relationList();  

  // access functions;
  virtual void printAll(int tNum);  // print all elements in list preceded by
    // tNum tabs AND print all descendants, incrementing tNum with each 
    // generation;
  virtual void printAll();  // as above, with tNum = 0;
};


//  -------------------------------------------------------------;
// For class variables & functions..;
class classMember : public brk_basePtr {

 public:

  // class vars
  memberType typeVariant; 
  SgStatement * declaration;
  SgSymbol * symbol;
  char * name;
  char * typeOf;
  SgType *memType;
  
  // access functions
  classMember(SgSymbol *sym, memberType tv);
  classMember(SgStatement *decl, memberType tv);
  virtual void print();
  virtual void print(int);
};



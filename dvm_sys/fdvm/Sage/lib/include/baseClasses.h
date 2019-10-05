//  ----------------------------------
//  Darryl Brown
//  University of Oregon pC++/Sage++
//
//  baseClasses.h  -  module for basic classes used by 
//       breakpoint modules.
//
//  
//  ----------------------------------

//if already included, skip this file...
#ifdef BASE_CL_ALREADY_INCLUDED
// do nothing;
#else 
#define BASE_CL_ALREADY_INCLUDED 1

 
//  -------------------------------------------------------------;
// this class is the base pointer type of all elements ;
// stored in linked lists;
class brk_basePtr {
 public:

  virtual void print();
    // this function should be overridden by later classes.;
  virtual void print(int);
    // this function should be overridden by later classes.;
  virtual void printToBuf(int, char *);
    // this function should be overridden by later classes.;
  virtual void print(int t, FILE *fptr);
    // this function should be overridden by later classes.;
  virtual void printAll();
    // this function should be overridden by later classes.;
  virtual void printAll(int);
    // this function should be overridden by later classes.;
#if 0
  virtual void printAll(int, FILE *);
    // this function should be overridden by later classes.;
  virtual void printAll(FILE *);
    // this function should be overridden by later classes.;
#endif
  int (* userCompare)(brk_basePtr *, brk_basePtr *);
    // this function should be overridden by later classes.;
  virtual int compare(brk_basePtr *);
    // this function should be overridden by later classes.;
  brk_basePtr();
};


//  -------------------------------------------------------------
// the nodes of the linked lists kept for children and parents of each class;
class brk_ptrNode : public brk_basePtr {
 public: 
  brk_ptrNode *next;  // next node;
  brk_ptrNode *prev;  // previous node;
  brk_basePtr *node;  // the ptr to the hierarchy at this node;

  // constructors;
  brk_ptrNode (void); 
  brk_ptrNode (brk_basePtr *h);
  virtual int compare(brk_basePtr *);
    // compares this heirarchy with another alphabetically using className;

};

//  -------------------------------------------------------------
// the class implementing the linked list for 
class brk_linkedList : public brk_basePtr {

 public: 

  brk_ptrNode *end;  // end of list;
  brk_ptrNode *start; // start of list;
  brk_ptrNode *current; // pointer to current element in list, 
    // used for traversal of list.;
  int length;  // length of list;

  // constructor;
  brk_linkedList();  

  // access functions;
  void push (brk_basePtr *h);  // push hierarchy h onto front of list;
  void pushLast (brk_basePtr *h);   // push hierarchy h onto back of list;
  brk_basePtr *pop (); // remove and return the first element in list;    
  brk_basePtr *popLast ();  // remove and return the last element in list;
  brk_basePtr *searchList ();  // begin traversal of list;
  brk_basePtr *nextItem();   // give the next item in list during traversal;
  brk_basePtr *remove (int i);  // remove & return the i-th element of list;
  brk_basePtr *getIth (int i);  // return the i-th element of list;
  brk_basePtr *insert(int i, brk_basePtr * p);  
    // insert *p at point i in list;
  brk_ptrNode *findMember (brk_basePtr *);   // look for this element and 
    // return the brk_ptrNode that points to it;
  int memberNum(brk_ptrNode *);   // what order does this element fall in list;

  virtual void print(int);  // print all elements;
  virtual void print(int, FILE *ftpr);  // print all elements;
  virtual void print();  // print all elements;
  virtual void printIth(int i);  // print i-th element of list;
  virtual void printToBuf(int, char *);
    // this function should be overridden by later classes.;
  void sort ();   // sorts the list, elements must have compare function.,;
  void sort(int (* compareFunc) (brk_basePtr *, brk_basePtr *));
  virtual void swap(brk_ptrNode *l, brk_ptrNode *r);
    // swaps these two basic elements
};


// ---------------------------------------------------
//   external declarations.
// ---------------------------------------------------

extern char * brk_stringSave(char * str);
extern int brk_strsame(char * str, char * str1);
extern void brk_printtabs(int tabs);
extern void brk_printtabs(int tabs, FILE *fptr);
// here is the endif 

#endif






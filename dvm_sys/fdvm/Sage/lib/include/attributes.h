////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines the data structure for attributes in sage
// attributes can be used to store any information for any statement, expression, symbol or types nodes
// F. Bodin Indiana July 94.
// 
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

class SgAttribute{
 private:
  // the attribute data;
  int type; // a label;
  void *data;  // the data;
  int dataSize; // the size of the data in bytes to allow data to be copied;
  SgAttribute *next; // to the next attribute of a statements (do that way or not??);
  // link to sage node, allow to go from an attribute to sage stuffs;
  typenode typeNode;   // indicates if SgStatement, SgExpression, ... ptToSage is pointed to;
  void *ptToSage; // pointer to SgStatement, SgExpression, ... ;
  int fileNumber; // the file methods;
// the methods to access the structure of an attributes;  
 public:
  SgAttribute(int t, void *pt, int size, SgStatement &st, int filenum);
  SgAttribute(int t, void *pt, int size, SgSymbol &st, int filenum);
  SgAttribute(int t, void *pt, int size, SgExpression &st, int filenum);
  SgAttribute(int t, void *pt, int size, SgType &st, int filenum);
  SgAttribute(int t, void *pt, int size, SgLabel &st, int filenum); //Kataev 21.03.2013
  SgAttribute(int t, void *pt, int size, SgFile &st, int filenum); //Kataev 15.07.2013
  ~SgAttribute();
  int getAttributeType();
  void setAttributeType(int t);
  void *getAttributeData();
  void *setAttributeData(void *d);
  int getAttributeSize();
  void setAttributeSize(int s);
  typenode getTypeNode();
  void *getPtToSage(); 
  void  setPtToSage(void *sa);
  void  resetPtToSage();
  void  setPtToSage(SgStatement &st);
  void  setPtToSage(SgSymbol &st);
  void  setPtToSage(SgExpression &st);
  void  setPtToSage(SgType &st);
  void  setPtToSage(SgLabel &st); //Kataev 21.03.2013
  void  setPtToSage(SgFile &st); //Kataev 15.07.2013
  SgStatement *getStatement();
  SgExpression *getExpression();
  SgSymbol  *getSgSymbol();
  SgType  *getType();
  SgLabel *getLabel(); //Kataev 21.03.2013
  SgFile *getFile(); //Kataev 15.07.2013
  int getfileNumber();
  SgAttribute *copy(); 
  SgAttribute *getNext();
  void setNext(SgAttribute *s);
  int listLenght();
  SgAttribute *getInlist(int num);
  void save(FILE *file);
  void save(FILE *file, void (*savefunction)(void *dat,FILE *f));
  
};



///////////////////////////////////////////////////////////////////////////////////////
// The ATTRIBUTE TYPE ALREADY USED
///////////////////////////////////////////////////////////////////////////////////////

#define DEPENDENCE_ATTRIBUTE   -1001
#define INDUCTION_ATTRIBUTE    -1002
#define ACCESS_ATTRIBUTE       -1003
#define DEPGRAPH_ATTRIBUTE     -1004
#define USEDLIST_ATTRIBUTE     -1005
#define DEFINEDLIST_ATTRIBUTE  -1006

#define NOGARBAGE_ATTRIBUTE    -1007
#define GARBAGE_ATTRIBUTE      -1008

// store the annotation expression; it is then visible from the
// garbage collection
#define ANNOTATION_EXPR_ATTRIBUTE   -1009




/**************************************************************\
* Fortran DVM                                                  * 
*                                                              *
*            Input/Output Statements Processing                *
\**************************************************************/
 
#include "dvm.h"
#define NO_ERROR_MSG 0

static const char *filePositionArgsStrings[] = { "unit", "fmt", "rec", "err", "iostat", "end", "nml", "eor", "size", "advance", "iomsg" };

// enum for new open/close
enum {UNIT_IO, ACCESS_IO, ACTION_IO, ASYNC_IO, BLANK_IO, DECIMAL_IO, DELIM_IO, ENCODING_IO, ERR_IO, FILE_IO,
  FORM_IO, IOSTAT_IO, IOMSG_IO, NEWUNIT_IO, PAD_IO, POSITION_IO, RECL_IO, ROUND_IO, SIGN_IO, STATUS_IO, DVM_MODE_IO, NUMB__CL };
static const char *openCloseArgStrings[] = { "unit", "access", "action", "async", "blank", "decimal", "delim",
  "encoding", "err", "file", "form", "iostat", "iomsg", "newunit", "pad", "position", "recl", "round", "sign",
  "status", "io_mode" };

enum { UNIT_RW, FMT_RW, NML_RW, ADVANCE_RW, ASYNC_RW, BLANK_RW, DECIMAL_RW, DELIM_RW, END_RW, EOR_RW, ERR_RW, ID_RW,
  IOMSG_RW, IOSTAT_RW, PAD_RW, POS_RW, REC_RW, ROUND_RW, SIGN_RW, SIZE_RW, NUMB__RW };
static const char *readWriteArgStrings[] = { "unit", "fmt", "nml", "advance", "async", "blank", "decimal", "delim", "end", "eor", "err", "id", "iomsg", "iostat", "pad", "pos", "rec", "round", "sign", "size"};

int Check_ReadWritePrint(SgExpression *ioc[], SgStatement *stmt, int error_msg);
void Replace_ReadWritePrint( SgExpression *ioc[], SgStatement *stmt);

int TestIOList(SgExpression *iol, SgStatement *stmt, int error_msg)
{SgExpression *el,*e;
int tst=1;
for (el=iol;el;el=el->rhs()) {    
   e = el->lhs();  // list item
   ReplaceFuncCall(e); 
   if(isSgExprListExp(e)) // implicit loop in output list
     e = e->lhs();
   if(isSgIOAccessExp(e)) {
      tst=ImplicitLoopTest(e,stmt,error_msg) ? tst : 0; 
   }
   else 
      tst=IOitemTest(e,stmt,error_msg) ? tst : 0;     
 }
return (tst);
}

int ImplicitLoopTest(SgExpression *eim, SgStatement *stmt, int error_msg)
{int tst =1;
 SgExpression *ell, *e;
 if(isSgExprListExp(eim->lhs()))
   for (ell = eim->lhs();ell;ell=ell->rhs()){ //looking through item list of implicit loop
     e = ell->lhs();
     if(isSgExprListExp(e)) // implicit loop in output list
       e = e->lhs();
     if(isSgIOAccessExp(e)){
       tst=ImplicitLoopTest(e,stmt,error_msg) ? tst : 0; 
     }
     else 
       tst=IOitemTest(e,stmt,error_msg) ? tst : 0;    
   }
 else
       tst=IOitemTest(eim->lhs(),stmt,error_msg) ? tst : 0;   
 return(tst);
}

int IOitemTest(SgExpression *e, SgStatement *stmt, int error_msg)
{int tst=1;
 if(!e) return(1);
 if(isSgArrayRefExp(e)){
   if( HEADER(e->symbol())) {
       if(error_msg)
         Error("Illegal I/O list item: %s",e->symbol()->identifier(),192,stmt);
       return (0);
   } else
       return(1);
 }
 if(e->variant()  == ARRAY_OP) //substring
       return(IOitemTest(e->lhs(),stmt,error_msg));
 if(isSgVarRefExp(e) || isSgValueExp(e))
    return(1);
 tst=IOitemTest(e->lhs(),stmt,error_msg) ? tst : 0;  
 tst=IOitemTest(e->rhs(),stmt,error_msg) ? tst : 0;   
 return(tst);
}      

SgStatement *Any_IO_Statement(SgStatement *stmt) 
{ SgStatement *last;
  ReplaceContext(stmt); 
  if(!IN_COMPUTE_REGION)
     LINE_NUMBER_BEFORE(stmt,stmt);
  SgExpression *ioEnd[3];
  if(hasEndErrControlSpecifier(stmt, ioEnd))
     ReplaceStatementWithEndErrSpecifier(stmt,ioEnd);           	    
  if(perf_analysis){
     InsertNewStatementBefore(St_Biof(),stmt);
     InsertNewStatementAfter ((last = St_Eiof()),stmt,stmt->controlParent());
     cur_st = stmt;
     return(last); 
  } 
  return(stmt);
}

void IoModeDirective(SgStatement *stmt, char io_modes_str[], int error_msg)
{
  SgExprListExp *modes = isSgExprListExp(stmt->expr(0));
  int imode = 0;
  if (!options.isOn(IO_RTS)) {
     if(error_msg)
        warn("Directive IO_MODE is ignored, -ioRTS option should be specified",623,stmt);
     return;
  }
  for (imode = 0; imode < modes->length(); ++imode) {
     SgExpression *mode = modes->elem(imode);
     if (mode->variant() == PARALLEL_OP)
        io_modes_str[imode] = 'p';
     else if (mode->variant() == ACC_LOCAL_OP)
	io_modes_str[imode] = 'l';
     else if (mode->variant() == ACC_ASYNC_OP)
	io_modes_str[imode] = 's';
     else
        if(error_msg)
           err("Illegal elements in IO_MODE directive", 460, stmt);
  }
  io_modes_str[imode] = '\0';
  if (stmt->lexNext()->variant() != OPEN_STAT) {
     if(error_msg)
        err("Misplaced directive: no OPEN statement after IO_MODE statement", 103, stmt);
     io_modes_str[0]='\0';
  }
}

void Open_Statement(SgStatement *stmt, char io_modes_str[], int error_msg)
{
  Any_IO_Statement(stmt);
  if(options.isOn(IO_RTS) && io_modes_str[0] != '\0')
     Open_RTS(stmt, io_modes_str, error_msg);
  else
     OpenClose(stmt,error_msg);
}

void Open_RTS(SgStatement* stmt, char* io_modes_str, int error_msg) {
  SgExpression *ioc[40];
  int io_err = control_list_open_new(stmt->expr(1), ioc);
  if(!io_err)
  {
	if( error_msg )
	  err("Illegal elements in control list", 185, stmt);
	return;
  }
  
  bool suitableForNewIO = checkArgsOpen(ioc, stmt, error_msg, io_modes_str);
  if (!suitableForNewIO) return;
  Dvmh_Open(ioc, io_modes_str);
  io_modes_str[0]='\0';
}

void Close_Statement(SgStatement *stmt, int error_msg)
{
  Any_IO_Statement(stmt);
  if(options.isOn(IO_RTS))
     Close_RTS(stmt,error_msg);
  else
     OpenClose(stmt,error_msg);
}

void Close_RTS(SgStatement *stmt, int error_msg)
{
  SgExpression *ioc[NUMB__CL];
  int io_err = control_list_close_new(stmt->expr(1), ioc);
  if(!io_err)
  {
	if( error_msg )
	{
	  if (!ioc[UNIT_IO])
		err("UNIT not specified in close statement", 456, stmt);
	  else
		err("Illegal elements in control list", 185, stmt);
	}
	return;
  }
  
  bool suitableForNewIO = checkArgsClose(ioc, stmt, error_msg);
  
  // generate If construct:
  //     if (dvmh_ftn_connected (args) then <by_RTS2> else <BY_IO_Fortran> endif
  SgStatement *ifst = IfConnected(stmt,ioc[UNIT_],suitableForNewIO);
  SgStatement *last = ifst->lastNodeOfStmt();  //stmt->lexNext();
  //true body
  Dvmh_Close(ioc);
  
  //false body
  NewOpenClose(stmt);
  cur_st = last;
}


void OpenClose(SgStatement *stmt, int error_msg)
{
  SgExpression *ioc[NUM__O];  
  int io_err=control_list_open(stmt->expr(1),ioc); // control_list analisys
  if(error_msg)
     Check_Control_IO_Statement(io_err,ioc,stmt,error_msg); 
  if(!options.isOn(READ_ALL))
     Replace_IO_Statement(ioc,stmt); 
  cur_st = stmt;
  return;
}

void NewOpenClose(SgStatement *stmt)
{
  SgExpression *ioc[NUM__O];  
  int io_err=control_list_open(stmt->expr(1),ioc); // control_list analisys
  io_err = Check_Control_IO_Statement(io_err,ioc,stmt,NO_ERROR_MSG);
  if(io_err)
     ReplaceByStop(io_err,stmt);
  else
     Replace_IO_Statement(ioc,stmt); 
  return;
}

void Replace_IO_Statement(SgExpression *ioc[],SgStatement *stmt)
{
  cur_st = stmt; 
  if(ioc[IOSTAT_])  // there is keyed argument IOSTAT 
     InsertSendIOSTAT(ioc[IOSTAT_]);
  ReplaceByIfStmt(stmt);     
}

void ReplaceByStop(int io_err, SgStatement *stmt)
{
  SgStatement *new_stmt = new SgStatement(STOP_STAT);
  stmt->insertStmtAfter(*new_stmt,*stmt->controlParent());
  char num3s[4];
  format_num(io_err, num3s);
  char *buff = new char[strlen(stmt->fileName()) + 75]; 
  sprintf(buff, "Illegal IO statement, error %s on line %d of %s", num3s,stmt->lineNumber(), stmt->fileName());
  new_stmt = new SgStatement(PRINT_STAT);
  new_stmt->setExpression(0,*new SgExprListExp(*new SgValueExp(buff)));
  SgExpression *ecl = new SgExpression(SPEC_PAIR,new SgKeywordValExp("fmt"),new SgKeywordValExp("*"),NULL);
  new_stmt->setExpression(1,*new SgExprListExp(*ecl));
  stmt->insertStmtAfter(*new_stmt,*stmt->controlParent());
  stmt-> extractStmt(); //extract IO statement
  return;
}

int Check_Control_IO_Statement(int io_err, SgExpression *ioc[], SgStatement *stmt, int error_msg)
{
  if( !io_err )
  {
     if( error_msg ) 
	err("Illegal elements in control list", 185,stmt);
     else
	return (185);
  }
  if( ioc[ERR_] )
  {
     if( error_msg )
	err("END= and ERR= specifiers are illegal in FDVM", 186,stmt);
     else
        return (186);  
  }	
  if( inparloop && (ioc[IOSTAT_] || stmt->variant() == INQUIRE_STAT) || stmt->variant() == READ_STAT) //(stmt->variant() == INQUIRE_STAT &&  ? (SgExpression *) 1 : ioc[IOSTAT_]) && inparloop )
  {
     if( error_msg)
	err("Illegal I/O statement in the range of parallel loop/region", 184,stmt);
     else
        return (184);
  }
  return(0);
}

void Inquiry_Statement(SgStatement *stmt, int error_msg)
{
  Any_IO_Statement(stmt);
  if(options.isOn(IO_RTS))
    ; // Inquiry_RTS(stmt);
  else
     Inquiry(stmt,error_msg);
}

void Inquiry(SgStatement *stmt, int error_msg)
{
  SgExpression *ioc[NUM__O+1];
  int io_err;
  io_err=control_list_inquire(stmt->expr(1),ioc);  // control list analysis
  if(error_msg)
     Check_Control_IO_Statement(io_err,ioc,stmt,error_msg); 
  cur_st = stmt;
  InsertSendInquire(ioc);
  ReplaceByIfStmt(stmt); 
  cur_st = stmt;            
}

void FilePosition_Statement(SgStatement *stmt, int error_msg)
{
  Any_IO_Statement(stmt);
  // RTS BACKSPACE isn't implemented!
  if(options.isOn(IO_RTS))
	FilePosition_RTS(stmt, error_msg);
  else
	FilePosition(stmt,error_msg);
}

void FilePosition_RTS(SgStatement* stmt, int error_msg) {
  
  SgExpression *ioc[NUM__R];
  int io_err = control_list1(stmt->expr(1), ioc);
  // FIXME: it would be better to replace this error to control_list1
  if (!ioc[UNIT_]) {
    if (error_msg)
      err("Unit argument not specified in IO-statement", 456, stmt);
    return;
  }
  if(!io_err)
  {
	if( error_msg )
	  err("Illegal elements in control list", 185, stmt);
	return;
  }
  
  bool suitableForNewIO = checkArgsEnfileRewind(ioc, stmt, error_msg);
  
  // generate If construct:
  //     if (dvmh_ftn_connected (args) then <by_RTS2> else <BY_IO_Fortran> endif
  SgStatement *ifst = IfConnected(stmt,ioc[UNIT_],suitableForNewIO);
  SgStatement *last = ifst->lastNodeOfStmt();  //stmt->lexNext();
  //true body
  Dvmh_FilePosition(ioc, stmt->variant());
  
  //false body
  NewFilePosition(stmt);  //Replace_IO_Statement(ioc,stmt); 
  cur_st = last;
}


void FilePosition(SgStatement *stmt, int error_msg)
{
  SgExpression *ioc[NUM__R];
  
  int io_err;
  io_err = control_list1(stmt->expr(1),ioc); // control_list analisys
  if(error_msg)
     Check_Control_IO_Statement(io_err,ioc,stmt,error_msg); 
  Replace_IO_Statement(ioc,stmt); 
  cur_st = stmt;
  return;
}

void NewFilePosition(SgStatement *stmt)
{
  SgExpression *ioc[NUM__R];
  int io_err = control_list1(stmt->expr(1),ioc); // control_list analisys
  io_err = Check_Control_IO_Statement(io_err,ioc,stmt,NO_ERROR_MSG);
  if(io_err)
     ReplaceByStop(io_err,stmt);
  else
     Replace_IO_Statement(ioc,stmt); 
  return;
}

void ReadWrite_Statement(SgStatement *stmt, int error_msg)
{
  Any_IO_Statement(stmt);
  if(options.isOn(IO_RTS))
    ReadWrite_RTS(stmt,error_msg);
  else
    ReadWritePrint_Statement(stmt,error_msg);
}

void NewReadWritePrint_Statement(SgStatement *stmt)
{
  SgExpression *ioc[NUM__R];

  int io_err= IOcontrol(stmt->expr(1),ioc,stmt->variant());  //control_list1(stmt->expr(1),ioc); // control_list analisys
  io_err = Check_Control_IO_Statement(io_err,ioc,stmt,NO_ERROR_MSG);
  if(!io_err) 
     io_err = Check_ReadWritePrint(ioc,stmt,NO_ERROR_MSG); 
  if(io_err)
     ReplaceByStop(io_err,stmt);
  else
     Replace_ReadWritePrint(ioc, stmt);
  return;
}

void ReadWrite_RTS(SgStatement *stmt, int error_msg)
{
  SgExpression *ioc[NUMB__RW];
  int io_err = control_list_rw(stmt->expr(1),ioc);
  if(!io_err)
  {
    if( error_msg ) {
      if (!ioc[UNIT_RW])
        err("UNIT not specified in read/write statement", 456, stmt);
      else
        err("Illegal elements in control list", 185, stmt);
    }
    return;
  }
	
  bool suitableForNewIO = checkArgsRW(ioc, stmt, error_msg);
	
  // generate If construct:
  //     if (dvmh_ftn_connected (args) then <by_RTS2> else <BY_IO_Fortran> endif
  SgStatement *ifst = IfConnected(stmt,ioc[UNIT_],suitableForNewIO);
  SgStatement *last = ifst->lastNodeOfStmt();  //stmt->lexNext();

  //true body
  Dvmh_ReadWrite(ioc, stmt);
	
  //false body
  NewReadWritePrint_Statement(stmt);
  cur_st = last;
}

int FixError(const char *str, int ierr, SgSymbol *s, SgStatement *stmt, int error_msg)
{
  if(error_msg) { 
      if(s)
         Error(str,s->identifier(),ierr,stmt);
      else
         err(str,ierr,stmt);
      return (-1);
  }
   else
      return(ierr);
}

int Check_ReadWritePrint(SgExpression *ioc[], SgStatement *stmt, int error_msg)
{
  if(ioc[END_] || ioc[ERR_] || ioc[EOR_])
     return FixError("END=, EOR= and ERR= specifiers are illegal in FDVM",186,NULL,stmt,error_msg);
 
  if(ioc[UNIT_] && (ioc[UNIT_]->type()->variant() == T_STRING) && ioc[UNIT_]->symbol() && HEADER(ioc[UNIT_]->symbol())) 
     return FixError("'%s' is distributed array",148,ioc[UNIT_]->symbol(),stmt,error_msg);

  if(ioc[FMT_]) 
  {            
     SgKeywordValExp *kwe = isSgKeywordValExp(ioc[FMT_]);
     if(kwe && strcmp(kwe->value(),"*"))
        return FixError("Invalid format specification",189,NULL,stmt,error_msg); 
  } 
  SgExpression *iol = stmt->expr(0);  //  I/O list     
  SgExpression *e;
  if(iol && (e = isSgArrayRefExp(iol->lhs())) &&  (HEADER(iol->lhs()->symbol()))) 
  {                                 // first item is distributed array refference
     if (iol->rhs() )  // there are other items in I/O-list
        return FixError("Illegal I/O list ",190,NULL,stmt,error_msg);
 
     if(ioc[IOSTAT_] ) 
        return FixError("IOSTAT= specifier is illegal in I/O of distributed array", 187,NULL,stmt,error_msg); 

     if(ioc[FMT_] && !isSgKeywordValExp(ioc[FMT_]) || ioc[NML_] ) 
        return FixError("I/O of distributed array controlled by format specification or NAMELIST is not supported in FDVM", 191,NULL,stmt,error_msg);

     if(ioc[UNIT_] && (ioc[UNIT_]->type()->variant() == T_STRING) && ioc[UNIT_]->symbol()) //I/O to internal file
        return FixError("'%s' is distributed array",148,e->symbol(),stmt,error_msg);

     if(IN_COMPUTE_REGION && !inparloop && !in_checksection ) 
        return FixError("Illegal statement in the range of region (not implemented yet)", 576,NULL,stmt,error_msg);
  }
  else {
     if( iol && !TestIOList(iol,stmt,error_msg) && !error_msg) // check I/O list
        return (192);
  }
  return(0);
}

void Replace_ReadWritePrint( SgExpression *ioc[], SgStatement *stmt)
// READ, WRITE, PRINT statements

{           
            SgExpression *e, *iol;
            int IOtype;

            cur_st = stmt; 
                       
            // analizes UNIT specifier                                  
            if(ioc[UNIT_] && (ioc[UNIT_]->type()->variant() == T_STRING)) {
               SgKeywordValExp *kwe;  
               if((kwe=isSgKeywordValExp(ioc[UNIT_])) && (!strcmp(kwe->value(),"*")))
                                                                       //"*" - system unit
                   ;
               else  // I/O to internal file           
                 return;                 
	    }   
            
            // analizes format specifier and determines type of I/O
            if(ioc[FMT_]) {
            
               SgKeywordValExp *kwe = isSgKeywordValExp(ioc[FMT_]);
               if(kwe) // Format
                  if(!strcmp(kwe->value(),"*"))
                     IOtype = 1; // formatted IO, controlled by IO-list  
                  else                      
                     return;   // illegal format specifier ??
                  
               else
                     IOtype = 2; // formatted IO, controlled by format
                                 // specification or NAMELIST
            }
            else
                     IOtype = 3; // unformatted IO
            if(ioc[NML_])
              IOtype = 2; // formatted IO, controlled by  NAMELIST

           //looking through the IO-list
            iol = stmt->expr(0);
            if(!iol) {  // input list is absent
               Replace_IO_Statement(ioc,stmt);  
               return; 
            } 
            if((e = isSgArrayRefExp(iol->lhs())) &&  (HEADER(iol->lhs()->symbol()))) {
                                  // first item is distributed array refference
                if (iol->rhs())  // error: there are other items in I/O-list
                   return;
                if(!e->lhs() && IOtype != 2)  //whole array and format=* or unformatted                               
                       IO_ThroughBuffer(e->symbol(),stmt);                    
                   else 
                       return;    //error
                   
             }
             else { // replicated variable list
	       if(!TestIOList(iol,stmt,NO_ERROR_MSG))
                  return;  
                if (ioc[IOSTAT_] || (stmt->variant() == READ_STAT)) {
		   
                   if(stmt->variant() == READ_STAT)
                      InsertSendInputList(iol,ioc[IOSTAT_],stmt);
                   else
                      InsertSendIOSTAT(ioc[IOSTAT_]);
                }
                ReplaceByIfStmt(stmt);
             }
}

void ReadWritePrint_Statement(SgStatement *stmt, int error_msg)
// READ, WRITE, PRINT statements

{           SgSymbol *sio;
            SgExpression *e,*iol;
            SgExpression *ioc[NUM__R];
            int IOtype, io_err;
            cur_st = stmt;
            send = 0;  
            // analizes IO control list and sets on ioc[]                       
            e = stmt->expr(1); // IO control
            io_err = IOcontrol(e,ioc,stmt->variant());           
            if(!io_err && error_msg){
               err("Illegal elements in control list", 185,stmt);
               return;
            }
            if((ioc[END_] || ioc[ERR_] || ioc[EOR_]) && error_msg) {
               err("END=, EOR= and ERR= specifiers are illegal in FDVM", 186,stmt);
               return;
            }
            
            if(ioc[UNIT_] && (ioc[UNIT_]->type()->variant() == T_STRING)) {
               SgKeywordValExp *kwe;
               if((kwe=isSgKeywordValExp(ioc[UNIT_])) && (!strcmp(kwe->value(),"*")))
                                                                       //"*" - system unit
                         ;
               else { // I/O to internal file           
                 if(ioc[UNIT_]->symbol() && HEADER(ioc[UNIT_]->symbol()) && error_msg)
                   Error("'%s' is distributed array", ioc[UNIT_]->symbol()->identifier(),   148,stmt);
                 if(error_msg)   
                   TestIOList(stmt->expr(0),stmt,error_msg);
                     //err("I/O to internal file is not supported in FDVM", stmt);
                 return;
               }  
	    }   
            
            // analizes format specifier and determines type of I/O
            if(ioc[FMT_]) {
            
               SgKeywordValExp * kwe;
               kwe = isSgKeywordValExp(ioc[FMT_]);
               if(kwe) // Format
                  if(!strcmp(kwe->value(),"*"))
                     IOtype = 1; // formatted IO, controlled by IO-list  
                  else {
                     IOtype = 0; // illegal format specifier ??
                     if(error_msg) 
                       err("Invalid format specification", 189,stmt);
                     return;
                  }
               else
                     IOtype = 2; // formatted IO, controlled by format
                                 // specification or NAMELIST
            }
            else
                     IOtype = 3; // unformatted IO
            if(ioc[NML_])
              IOtype = 2; // formatted IO, controlled by  NAMELIST

            //Any_IO_Statement(stmt);

           //looking through the IO-list
            iol = stmt->expr(0);
            if(!iol) {  // input list is absent 
              if(stmt->variant() != READ_STAT || !options.isOn(READ_ALL))  
                Replace_IO_Statement(ioc,stmt);
              return; 
            } 
             if((e = isSgArrayRefExp(iol->lhs())) &&  (HEADER(iol->lhs()->symbol()))) {
                                  // first item is distributed array refference
                if (iol->rhs() && error_msg)  {// there are other items in I/O-list
                  
                   err("Illegal I/O list ", 190,stmt);  
                   return;
                }
                if(ioc[IOSTAT_] && error_msg) {
                  err("IOSTAT= specifier is illegal in I/O of distributed array", 187,stmt);
                   return;
                 }
                if(!e->lhs())  //whole array
                   if(IOtype != 2) { 
                       sio = e->symbol();           
                           //buf_use[TypeIndex(sio->type()->baseType())] = 1; 
                       IO_ThroughBuffer(sio,stmt);
                       if(IN_COMPUTE_REGION && !inparloop && !in_checksection && error_msg)   
                         err("Illegal statement in the range of region (not implemented yet)", 576,stmt); 
                    }
                    else {
                       if( error_msg)
                         err("I/O of distributed array controlled by format specification or NAMELIST is not supported in FDVM", 191,stmt);
                         // illegal format specifier for I/O of distributed array
                       return; 
                    }
                else { 
                   if(error_msg)
                     err("Illegal I/O list item", 192,stmt);  
                   return;
                }   
             }
             else { // replicated variable list
	       if(!TestIOList(iol,stmt,error_msg))
                  return;  
               if (stmt->variant() == READ_STAT) {		   
                  if(!options.isOn(READ_ALL))
                     InsertSendInputList(iol,ioc[IOSTAT_],stmt);
               }
               else if(ioc[IOSTAT_] )
                     InsertSendIOSTAT(ioc[IOSTAT_]);
               
               if(stmt->variant() != READ_STAT || !options.isOn(READ_ALL))   
                  ReplaceByIfStmt(stmt);
                //if(IN_COMPUTE_REGION && !in_checksection)
                //  ChangeDistArrayRef(iol);
             }
             if(inparloop && (send || IN_COMPUTE_REGION) && error_msg)
               err("Illegal I/O statement in the range of parallel loop/region", 184,stmt);
             
}

void IO_ThroughBuffer(SgSymbol *ar, SgStatement *stmt)
{
 SgStatement *dost, *contst,*ifst;
 SgExpression *esize,*econd,*iodo, *iolist,*ubound,*are,*d, *eN[8];
 SgValueExp c1(1),c0(0);
 SgLabel *loop_lab; 
 //SgSymbol *sio;
 int i,l,rank,s,s0,N[8],itype,imem;
 int m = -1;
 int init,last,step;
 int M=0;
 
 contst = NULL;
 imem=ndvm;
 ReplaceContext(stmt);
 
 itype = TypeIndex(ar->type()->baseType());
 if(itype == -1)  //may be derived type 
 {
    Error("Illegal type's array in input-output statement: %s",ar->identifier(),999,stmt);
    return;
 } else
    buf_use[itype] = 1;
 l = rank = Rank(ar);  
 s = IOBufSize;  //SIZE_IO_BUF;
 for(i=1; i<=rank; i++) {
    //calculating size of i-th dimension
    esize = ReplaceParameter(ArrayDimSize(ar, i));
    eN[i] = NULL;
    if(esize && esize->variant()==STAR_RANGE)
    {
      Error("Assumed-size array: %s",ar->identifier(),162,stmt);
      return;
    }
    if(esize->isInteger())
      N[i] = esize->valueInteger();
    else 
      {N[i] = -1; eN[i] = esize;} //!! dummy argument 
    if((N[i] <= 0) && !eN[i])
    {
      Error("Array shape declaration error: '%s'", ar->identifier(),193, stmt);
      return;
    }
  }
    // calculating s
 for(i=1; i<=rank; i++) {
    if(eN[i]) {
      l=i-1;
      break;
    }
    s0 = s / N[i];
    if(!s0) { // s0 == 0
      l = i-1;  
      break;
    }
    else
      s = s0;
 }
 if(l==rank) { // generating assign statement: m = 1
   // m = ndvm;
   //doAssignStmtBefore(&c1.copy(),stmt); 
   M=1;
 }
 else
   m = ndvm++;

 if(l+1 <= rank) {
 // generating DO statement: DO label idvm01 = 0, N[l+1]-1, s 
          
   loop_lab = GetLabel();
   contst = new SgStatement(CONT_STAT);
   esize = eN[l+1] ? &(eN[l+1]->copy() - c1.copy()) : new SgValueExp(N[l+1]-1);
   dost= new SgForStmt(*loop_var[1], c0.copy(), *esize, *new SgValueExp(s), *contst);
   BIF_LABEL_USE(dost->thebif) = loop_lab->thelabel;
   (dost->lexNext())->setLabel(*loop_lab); 
 
   if(l+2 <= rank)   
     // generating DO nest:
     // DO label idvm02 = 0, N[rank]-1
     // DO label idvm03 = 0, N[rank-1]-1
     //   .  .  .
     // DO label idvm0j = 0, N[l+2]-1

                                       //for(i=rank; i>l+1; i--) {  //27.11.09
     for(i=l+2; i<=rank; i++) {
        esize = eN[i] ? &(eN[i]->copy() - c1.copy()) : new SgValueExp(N[i]-1);
        dost= new SgForStmt(*loop_var[rank-i+2], c0.copy(), *esize, *dost);  
                         
        BIF_LABEL_USE(dost->thebif) = loop_lab->thelabel;
     }

   cur_st->insertStmtAfter(*dost);

   for(i=l+1; i<=rank; i++)
      contst->lexNext()->extractStmt(); // extracting ENDDO

   if((N[l+1]<0) || (N[l+1]-(N[l+1]/s)*s)) {  
   // generating the construction
   // IF (Il+1 + s .LE. Nl+1) THEN 
   //    m = s
   // ELSE 
   //    m = Nl+1 - Il+1
   // ENDIF 
   // and then insert it before CONTINUE statement
     esize = eN[l+1] ? &(eN[l+1]->copy()) : new SgValueExp(N[l+1]);
     econd = & (( *new SgVarRefExp(*loop_var[1]) + *new SgValueExp(s)) <= *esize);
     ifst = new SgIfStmt(*econd,                                                                     *new SgAssignStmt(*DVM000(m),*new SgValueExp(s)),                   *new SgAssignStmt(*DVM000(m),*esize - *new SgVarRefExp(*loop_var[1])));
     contst -> insertStmtBefore(*ifst);
   }
   else 
     
     //dost->insertStmtBefore(*new SgAssignStmt(*DVM000(m),*new SgValueExp(s)));     
     M=s;
   //cur_st = ifst;   
   stmt->extractStmt();
   contst -> insertStmtBefore(*stmt);
   // transfering label over D0-statements
   BIF_LABEL(dost->thebif) = BIF_LABEL(stmt->thebif);
   BIF_LABEL(stmt->thebif) = NULL;
   //cur_st = stmt;
 }
 // creating implicit loop as element of I/O list:
 // (BUF(I0), I0= 1,N1*...*Nl*m)
 ubound = DVM000(m);
 N[0] = 1; 
 for(i=1; i<=l; i++)
      N[0] = N[0] * N[i];
 if(M)   // M= const
   ubound = new SgValueExp(N[0]*M);   
 else {
   ubound = DVM000(m);
   if(N[0]  != 1) 
   ubound = &( *ubound * (*new SgValueExp(N[0])) );   
 }   

 //   ubound = &( *ubound * (*new SgValueExp(N[0])));   
// iodo = new SgExpression(DDOT,&c1.copy(), ubound,NULL);
 iodo = & SgDDotOp(c1.copy(),*ubound);
 iodo = new SgExpression(SEQ,iodo,NULL,NULL);
 iodo = new  SgExpression(IOACCESS,NULL,iodo,loop_var[0]);
// iodo = new SgIOAccessExp(*loop_var[0], c1.copy(), *ubound);//Sage error
 iodo -> setLhs(new SgArrayRefExp(*bufIO[itype],                                                              *new SgVarRefExp(*loop_var[0])));
 iolist = new SgExprListExp(*iodo);
 // iolist -> setLhs(iodo);
// replacing I/O list in source I/O statement
 stmt -> setExpression(0,*iolist);
 //generating assign statement
 //dvm000(i) = ArrCpy(...)
 are = new SgArrayRefExp(*bufIO[Integer],c1.copy()); //!!! itype=>Integer (bufIO[itype])
 init = ndvm;
 //if(l+2 <= rank)
   for(i=2; i<(rank-l+1);i++ )
     doAssignStmtBefore(new SgVarRefExp(*loop_var[i]),stmt);
 if(l+1 <= rank)
   doAssignStmtBefore(new SgVarRefExp(*loop_var[1]),stmt);
 
 for(i=l; i; i-- )
  doAssignStmtBefore(new SgValueExp(-1),stmt); 
 last = ndvm;
 //if(l+2 <= rank) 
   for(i=2; i<(rank-l+1);i++ )
     doAssignStmtBefore(new SgVarRefExp(*loop_var[i]),stmt);
 if(l+1 <= rank) {
   d = new SgVarRefExp(*loop_var[1]);
   if(M != 1)  
     d = (M)? &(*d+(*new SgValueExp(M-1))) : &(*d+(*DVM000(m))-c1.copy()); 
   doAssignStmtBefore(d,stmt);
 }

 step = last+rank;
 if(l+1 <= rank) {
   ndvm = step + rank - l - 1;
   doAssignStmtBefore(&c1.copy(),stmt);
 }
 ndvm = step+rank;
 if(stmt->variant() == READ_STAT){ 
   doAssignStmtAfter (A_CopyTo_DA(are,HeaderRef(ar),init,last,step,2));
   if(dvm_debug) {
       if(contst)
         cur_st = contst;
       cur_st->insertStmtAfter(*D_Read(GetAddresDVM(HeaderRefInd(ar,1)))); 
   } 
 } else
   doAssignStmtBefore(DA_CopyTo_A(HeaderRef(ar),are,init,last,step,2),stmt);

// replace I/O statement by: IF(TstIO().NE.0) I/O-statement 
 ReplaceByIfStmt(stmt);

//calculating maximal number of used loop variables for I/O
 nio = (nio < (rank-l+1)) ? (rank-l+1) : nio;
 SET_DVM(imem);
} 

int IOcontrol(SgExpression *e, SgExpression *ioc[],int type)
// analizes IO_control list (e) and sets on ioc[]
{ SgKeywordValExp *kwe;
  SgExpression *ee,*el;
  int i;
  for(i=NUM__R; i; i--)
     ioc[i-1] = NULL;

  if(e->variant() == SPEC_PAIR) {
     if(type == PRINT_STAT)
        ioc[FMT_] = e->rhs();   
     else {
        // ioc[UNIT_] = e->rhs();
       kwe = isSgKeywordValExp(e->lhs());   
       if(!kwe)
          return(0);
       if     (!strcmp(kwe->value(),"unit"))
               ioc[UNIT_] = e->rhs();
       else if (!strcmp(kwe->value(),"fmt")) 
               ioc[FMT_]  = e->rhs();
       else
               return(0);
     }	       
     return(1);
  }
  
  if(e->variant() == EXPR_LIST){
    for(el=e; el; el = el->rhs()) {
       ee = el->lhs();
       if(ee->variant() != SPEC_PAIR)
          return(0); // IO_control list error
       kwe = isSgKeywordValExp(ee->lhs());   
       if(!kwe)
          return(0);
       if     (!strcmp(kwe->value(),"unit"))
               ioc[UNIT_] = ee->rhs();
       else if (!strcmp(kwe->value(),"fmt")) 
               ioc[FMT_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"nml")) 
               ioc[NML_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"rec")) 
               ioc[REC_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"iostat")) 
               ioc[IOSTAT_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"end")) 
               ioc[END_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"err")) 
               ioc[ERR_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"eor")) 
               ioc[EOR_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"size")) 
               ioc[SIZE_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"advance")) 
               ioc[ADVANCE_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"pos")) 
               ioc[POS_]  = ee->rhs();

       else
               return(0);
    }
    return(1);
  }    
  else
    return(0);
}    

int control_list_rw(SgExpression *e, SgExpression *ioc[])
// analizes IO_control list (e) and sets on ioc[]
{ SgKeywordValExp *kwe;
  SgExpression *ee,*el;
  int i;
  for(i=NUMB__RW; i; i--)
    ioc[i-1] = NULL;
  
  if(e->variant() == SPEC_PAIR) {
    kwe = isSgKeywordValExp(e->lhs());
    if (!kwe)
      return(0);
    if (!strcmp(kwe->value(),"unit"))
      ioc[UNIT_RW] = e->rhs();
    else if (!strcmp(kwe->value(),"fmt"))
      ioc[FMT_RW]  = e->rhs();
    else if (!strcmp(kwe->value(), "nml"))
      ioc[NML_RW] = e->rhs();
    else
      return(0);
    return(1);
  }
  
  if(e->variant() == EXPR_LIST){
    for(el=e; el; el = el->rhs()) {
      ee = el->lhs();
      if(ee->variant() != SPEC_PAIR)
        return(0); // IO_control list error
      kwe = isSgKeywordValExp(ee->lhs());
      if(!kwe)
        return(0);
      if     (!strcmp(kwe->value(),"unit"))
        ioc[UNIT_RW] = ee->rhs();
      else if (!strcmp(kwe->value(),"fmt"))
        ioc[FMT_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"nml"))
        ioc[NML_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"advance"))
        ioc[ADVANCE_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"async"))
        ioc[ASYNC_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"blank"))
        ioc[BLANK_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"decimal"))
        ioc[DECIMAL_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"delim"))
        ioc[DELIM_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"end"))
        ioc[END_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"err"))
        ioc[ERR_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"eor"))
        ioc[EOR_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"id"))
        ioc[ID_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"iomsg"))
        ioc[IOMSG_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"iostat"))
        ioc[IOSTAT_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"pad"))
        ioc[PAD_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"pos"))
        ioc[POS_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"rec"))
        ioc[REC_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"round"))
        ioc[ROUND_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"sign"))
        ioc[SIGN_RW]  = ee->rhs();
      else if (!strcmp(kwe->value(),"size"))
        ioc[SIZE_RW]  = ee->rhs();
      else
        return(0);
    }
    if (!ioc[UNIT_RW]) return(0);
    return(1);
  }
  else
    return(0);
}
    
int control_list1(SgExpression *e, SgExpression *ioc[])
// analizes control list (e) for statements BACKSPACE,REWIND and ENDFILE
// and sets on ioc[]
{ SgKeywordValExp *kwe;
  SgExpression *ee,*el;
  int i;
  for(i=NUM__R; i; i--)
     ioc[i-1] = NULL;

  if(e->variant() == SPEC_PAIR) {
     ioc[UNIT_] = e->rhs();   
     return(1);
  }
  
  if(e->variant() == EXPR_LIST){
    for(el=e; el; el = el->rhs()) {
       ee = el->lhs();
       if(ee->variant() != SPEC_PAIR)
          return(0); // IO_control list error
       kwe = isSgKeywordValExp(ee->lhs());   
       if(!kwe)
          return(0);
       if     (!strcmp(kwe->value(),"unit"))
               ioc[UNIT_] = ee->rhs();
       else if (!strcmp(kwe->value(),"iostat")) 
               ioc[IOSTAT_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"err")) 
               ioc[ERR_]  = ee->rhs();
       //else if (!strcmp(kwe->value(), "iomsg"))
       //		ioc[IOMSG_] = ee->rhs();
       else
               return(0);
    }
    return(1);
  }    
  else
    return(0);
}
    
int control_list_inquire (SgExpression *e, SgExpression *ioc[])
// analizes control list (e)  INQUIRE statement
// and sets on ioc[]
{
  SgKeywordValExp *kwe;
  int i;
  for(i=NUM__O+1; i; i--)
     ioc[i-1] = NULL;

  if(e->variant() == SPEC_PAIR && (kwe=isSgKeywordValExp(e->lhs())) && !strcmp(kwe->value(),"iolength")) {  // case of  INQUIRY (IOLENGTH = ...) outlist
    ioc[NUM__O] = e->rhs();
    return (1);   
  } else  
    return(control_list_open(e,ioc));  // control_list analisys
}

int control_list_open(SgExpression *e, SgExpression *ioc[])
// analizes control list (e) for OPEN,CLOSE and INQUIRE statements
// and sets on ioc[]
{ SgKeywordValExp *kwe;
  SgExpression *ee,*el;
  int i;
  for(i=NUM__O; i; i--)
     ioc[i-1] = NULL;

  if(e->variant() == SPEC_PAIR) {
     ioc[UNIT_] = e->rhs();   
     return(1);
  }
  if(e->variant() == EXPR_LIST){
    for(el=e; el; el = el->rhs()) {
       ee = el->lhs();
       if(ee->variant() != SPEC_PAIR)
          return(0); // IO_control list error
       kwe = isSgKeywordValExp(ee->lhs());   
       if(!kwe)
          return(0);
       if     (!strcmp(kwe->value(),"unit"))
               ioc[UNIT_] = ee->rhs();
       else if (!strcmp(kwe->value(),"file")) 
               ioc[FILE_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"status")) 
               ioc[STATUS_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"iostat")) 
               ioc[IOSTAT_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"access")) 
               ioc[ACCESS_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"err")) 
               ioc[ERR_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"form")) 
               ioc[FORM_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"recl")) 
               ioc[RECL_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"blank")) 
               ioc[BLANK_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"exist"))
               ioc[EXIST_] = ee->rhs();
       else if (!strcmp(kwe->value(),"opened")) 
               ioc[OPENED_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"number")) 
               ioc[NUMBER_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"named")) 
               ioc[NAMED_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"name")) 
               ioc[NAME_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"sequential")) 
               ioc[SEQUENTIAL_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"direct")) 
               ioc[DIRECT_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"nextrec")) 
               ioc[NEXTREC_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"formatted")) 
               ioc[FORMATTED_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"unformatted")) 
               ioc[UNFORMATTED_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"position")) 
               ioc[POSITION_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"action")) 
               ioc[ACTION_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"readwrite")) 
               ioc[READWRITE_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"read")) 
               ioc[READ_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"write")) 
               ioc[WRITE_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"delim")) 
               ioc[DELIM_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"pad")) 
               ioc[PAD_]  = ee->rhs();
       else if (!strcmp(kwe->value(),"convert")) 
               ioc[CONVERT_]  = ee->rhs();

       else
               return(0);
    }
    return(1);
  }    
  else
    return(0);
}        

void InsertSendIOSTAT(SgExpression * eios)
{int imem;
 SgType *t;
 imem = ndvm;  
 doAssignStmtAfter(GetAddresMem(eios));
 t = eios->symbol() ? Base_Type(eios->symbol()->type()) : SgTypeInt();//type of IOSTAT var
 doAssignStmtAfter(TypeLengthExpr(t)); //type size
 //doAssignStmtAfter(new SgValueExp(TypeSize(t))); 14.03.03
 doCallAfter(SendMemory(1,imem,imem+1));  //count of memory areas = 1
 if(dvm_debug)
   InsertNewStatementAfter(D_Read(DVM000(imem)),cur_st,cur_st->controlParent());
 SET_DVM(imem);
}

void InsertSendInquire(SgExpression * eioc[])
{int imem,j,i,icount;
 imem = ndvm;
 j=0;
 if(eioc[NUM__O]) {  // case of  INQUIRY (IOLENGTH = ...) outlist
   j=1;
   doAssignStmtAfter(GetAddresMem(eioc[NUM__O]));
   doAssignStmtAfter(TypeLengthExpr(eioc[NUM__O]->type())); 
 }  else {
   for (i=IOST_;i<ACTION_;i++)    
     if(eioc[i]) {
       doAssignStmtAfter(GetAddresMem(eioc[i]));
       j++;
     }
   for (i=IOST_;i<ACTION_;i++)    
     if(eioc[i])
       doAssignStmtAfter(TypeLengthExpr(eioc[i]->type())); 
       //doAssignStmtAfter(new SgValueExp(TypeSize(eioc[i]->type()))); 14.03.03
 }
 if(j) {
   icount = j;   //count of memory areas
   doCallAfter(SendMemory(icount,imem,imem+j));
   if(dvm_debug)
     for(i=0; i<j; i++)
       InsertNewStatementAfter(D_Read(DVM000(imem+i)),cur_st,cur_st->controlParent());
 }
 SET_DVM(imem);
}

int isDependence(SgExpression *e,SgExpression *eprev)
{
  if(!e || !eprev)
    return 0;
  if(ExpCompare(e, eprev))
    return 1;
  return (isDependence(e->lhs(),eprev) || isDependence(e->rhs(),eprev));
}

int ElementDependence(SgStatement *st_first, SgStatement *st, SgExpression *e)
{
  SgStatement *st_next = st_first;
  for(;st_next != st; st_next=st_next->lexNext())
     if(isDependence(e,st_next->expr(1)->lhs()->lhs()))   //st_next is dvm000(i)=getai(el), search for dependency between e and el
       return 1;
  return 0;
}

void SendList(SgStatement *st_first, SgExpression *iisize[], int imem, int j0, int nl)
{
  SgStatement *st;
  int i,j;
  if(j0==nl) return;
  for(j = j0,st=st_first; j<nl; j++,st = st->lexNext())
  {  
     if( j!=j0 && (ElementDependence(st_first,st,st->expr(1)->lhs()->lhs()) || ElementDependence(st_first,st,iisize[j])))
         break;
  }
  cur_st = st->lexPrev();
  for(i=j0;i<j;i++)
     doAssignStmtAfter(iisize[i]);
 
  doCallAfter(SendMemory(j-j0,imem+j0,imem+nl+j0));
 
  SendList(cur_st->lexNext(),iisize,imem,j,nl);
}

# define MAXLISTLEN  1000

void InsertSendInputList(SgExpression * input_list, SgExpression * io_stat,SgStatement *stmt)
{int imem,j,i,icount,iel;
 SgExpression *el,*ein,*iisize[MAXLISTLEN],*iinumb[MAXLISTLEN],*iielem[MAXLISTLEN];
 SgType *t;
 SgStatement *st_save = cur_st;
 imp_loop = NULL;

 if(dvm_debug)
   for(i=0;i<MAXLISTLEN;i++)
     iinumb[i] = NULL;

 imem = ndvm;
 j=0; 
 for (el=input_list;el;el=el->rhs()) {    
   ein = el->lhs();  // input list item
   if(j== MAXLISTLEN-2)
     err("Compiler bug (in InsertSendInputList)",0,stmt);
   if(isSgIOAccessExp(ein)) //implicit loop
   { if(!SpecialKindImplicitLoop(el->rhs(),ein,&j, iisize, iielem, iinumb, stmt))
        ImplicitLoop(ein,&j, iisize, iielem, iinumb, stmt); 
   }   
   else if(isSgArrayRefExp(ein) && !ein->lhs() && (ein->type()->variant()!=T_STRING)){//whole array 
      doAssignStmtAfter(GetAddresMem(FirstArrayElement(ein->symbol()))); 
      iisize[j] = InputItemLength(ein,stmt);
      if(dvm_debug){      
        iielem[j] = ElemLength(ein->symbol());
        iinumb[j] = NumbOfElem(iisize[j], iielem[j]);
      }
      j++;
   }
   else if(isSgArrayRefExp(ein)  && (ein->type()->variant()==T_ARRAY)){//section of array
      doAssignStmtAfter(GetAddresMem (ContinuousSection(ein) ? FirstElementOfSection(ein) : FirstArrayElement(ein->symbol()))); 
      iisize[j] = InputItemLength(ein,stmt);
      if(dvm_debug){      
        iielem[j] = ElemLength(ein->symbol());
        iinumb[j] = NumbOfElem(iisize[j], iielem[j]);
      }
      j++;
   
   } 
   else if(isSgRecordRefExp(ein) && ein->type()->variant() == T_ARRAY ) {  //structure reference of ArrayType
      SgExpression *ein_short = ArrayFieldLast(ein);  
      doAssignStmtAfter( GetAddresMem( isSgRecordRefExp(ein_short) ? FirstElementOfField(ein_short) : FirstElementOfSection(ein_short) ) );         
      iisize[j] = InputItemLength(ein_short,stmt);
      if(dvm_debug){      
        iielem[j] = ElemLength(isSgRecordRefExp(ein_short) ? RightMostField(ein_short)->symbol() : ein_short->symbol());
        iinumb[j] = NumbOfElem(iisize[j], iielem[j]);
      }
      j++;
         
   }
   else {      
      doAssignStmtAfter(GetAddresMem(ein->type()->variant()==T_ARRAY ? FirstElementOfSection(ein) : ein)); 
      iisize[j] = InputItemLength(ein,stmt);
      j++;      
   }  
 }
 if(io_stat) {
     doAssignStmtAfter(GetAddresMem(io_stat));
     t = io_stat->symbol() ? Base_Type(io_stat->symbol()->type()) : SgTypeInt();//type of IOSTAT var
     iisize[j] =  TypeLengthExpr(t); //new SgValueExp(TypeSize(t));
     j++;
 }

 SendList(st_save->lexNext(),iisize,imem,0,j); 

 if(dvm_debug){
   for(i=0;i<j;i++)
     if(iinumb[i] != NULL){ // input list item is array
       iel = ndvm;
       doAssignStmtAfter(iielem[i]); 
       doAssignStmtAfter(iinumb[i]); 
       InsertNewStatementAfter(D_ReadA(DVM000(imem+i),iel,iel+1),cur_st,cur_st->controlParent());
       SET_DVM(iel);
     } else
       InsertNewStatementAfter(D_Read(DVM000(imem+i)),cur_st,cur_st->controlParent());
 }
 SET_DVM(imem);
}

int SpecialKindImplicitLoop(SgExpression *el, SgExpression *ein, int *pj, SgExpression *iisize[], SgExpression *iielem[],SgExpression *iinumb[],SgStatement *stmt)
{
  SgExpression *ell, *e, *e1, *enumb, *elen, *bounds;
  SgSymbol *s;
  SgValueExp c1(1);

  if(el) return(0);  //number of input list items > 1
  ell = ein->lhs();
  if(ell->rhs()) return(0); //number of items of implicit loop list
  e = ell->lhs(); s = e->symbol();
  bounds = ein->rhs(); 
  if(bounds->rhs()) return(0);  //step of implicit loop is specified 
  if(isSgArrayRefExp(e)  && (e->type()->variant()!=T_STRING) && Rank(s)==1 && (isSgVarRefExp(e->lhs()->lhs())) && (e->lhs()->lhs()->symbol() == ein->symbol()) ) {
    e1 = &(e->copy()); 
    e1->lhs()->setLhs(bounds->lhs()->lhs()->copy());   
    doAssignStmtAfter(GetAddresMem(e1)); //initial address of array section
    enumb = &(bounds->lhs()->rhs()->copy() - bounds->lhs()->lhs()->copy() + c1);
    elen =  ElemLength(s);

    iisize[*pj] = &(*enumb * (*elen)); //array section length 
    if(dvm_debug) { 
       iielem[*pj] = elen; //ElemLength(s);
       iinumb[*pj] = enumb;
    }
    *pj = *pj+1;
    return (1);   
  }
  else
    return(0);   
         
}

void ImplicitLoop(SgExpression *ein, int *pj, SgExpression *iisize[], SgExpression *iielem[],SgExpression *iinumb[],SgStatement *stmt)
{
  SgExpression *ell, *e; 
  for (ell = ein->lhs();ell;ell=ell->rhs()){ //looking through item list of implicit loop
     e = ell->lhs();
     if(isSgIOAccessExp(e))
        ImplicitLoop(e,pj,iisize,iielem,iinumb,stmt); 
     else {
         if(isSgArrayRefExp(e)) {
             SgExpression *e1 ;
             SgSymbol *ar;
             int has_aster_or_1;

             if(!e->lhs() && e->type()->variant()==T_STRING) {//character object
               doAssignStmtAfter(GetAddresMem(e)); 
               iisize[*pj] = InputItemLength(e,stmt);
               *pj = *pj+1;   
               continue;
             }
             ar = e->symbol();
             has_aster_or_1 =  hasAsterOrOneInLastDim(ar); //testing last dimension : * or 1
             if(! has_aster_or_1) {
                if(isInSymbList(imp_loop,ar))
                  continue;
                else
                  imp_loop = AddToSymbList(imp_loop,ar);  
             }
             e1 = FirstArrayElement(ar);
             doAssignStmtAfter(GetAddresMem(e1)); //initial array address
             iisize[*pj] =ArrayLength(ar,stmt,0);// whole array length 
             if (has_aster_or_1)  //testing last dimension : * or 1
             {
                 if (ein->symbol() == lastDimInd(e->lhs()))
                     iisize[*pj] = CorrectLastOpnd(iisize[*pj], ar, ein->rhs(), stmt);
                 //correcting whole array length by implicit loop parameters
                 else
                     Error("Can not calculate array length: %s", ar->identifier(), 194, stmt);
             }

             if(dvm_debug) { 
               iielem[*pj] = ElemLength(ar);
               iinumb[*pj] = NumbOfElem(iisize[*pj], iielem[*pj]);
             }
             *pj = *pj+1;   
         }
         else if(e->variant() == ARRAY_OP) {//substring or substring of array element
             SgExpression *e1 ;
             if( !e->lhs()->lhs())  //substring 
             {
               doAssignStmtAfter(GetAddresMem(e->lhs())); 
               iisize[*pj] = InputItemLength(e->lhs(),stmt);
               *pj = *pj+1; 
               continue;   
             }
             //substring of array element  
             e1 = FirstArrayElement(e->lhs()->symbol());
             doAssignStmtAfter(GetAddresMem(e1)); //initial array address
             iisize[*pj] = ArrayLength(e->lhs()->symbol(),stmt,1); // whole array length 
             *pj = *pj+1;   
         }     
         else {
             doAssignStmtAfter(GetAddresMem(e)); 
             iisize[*pj] = InputItemLength(e,stmt);
             *pj = *pj+1;   
         }
     }
  }
}

/*
 * variant when substring is represented by ARRAY_REF node with 2 operands
 *
SgExpression * InputItemLength (SgExpression *e, SgStatement *stmt)
{
 if (isSgVarRefExp(e))
    return(new SgValueExp(TypeSize(e->type())));  
 if (isSgArrayRefExp(e)) 
    if(e->type()->variant()!=T_STRING) //whole array or array element of non-character type           
        if(e->lhs()) //array element
           return(new SgValueExp(TypeSize(e->symbol()->type()->baseType())));   
        else  //whole array
           return(ArrayLength(e->symbol(),stmt,1));   
    else { //variable, array element, substring or substring of array element of type CHARACTER 
      if(!(e->lhs())) //variable
        return(StringLengthExpr(e->symbol()->type(),e->symbol())); 
       //return(new SgValueExp(CharLength(e->symbol()->type()))); 14.03.03
     // e = e->lhs()->lhs(); //variant of e->lhs() is EXPR_LIST  
         
      if(!(e->rhs()) && (e->lhs()->lhs()->variant() != DDOT)) //array element of type CHARACTER 
           return(StringLengthExpr(e->symbol()->type()->baseType(),e->symbol()));   
              //return(new SgValueExp(CharLength(e->symbol()->type()->baseType())));   
     else
           return(SubstringLength(e)); 
   } 
  return(new SgValueExp(-1)); 
}

SgExpression *SubstringLength(SgExpression *sub)
{ //SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgExpression *e,*e1,*e2;
  SgType *t;
//err("Sorry, substring length calculating is not jet implemented",cur_st);
     if(sub->lhs()->lhs()->variant() == DDOT) {  //substring(sub has variant EXPR_LIST)
        e = sub->lhs()->lhs();     
        t=sub->symbol()->type();
     }
     else { //substring of array element
        e = sub->rhs();
        t=sub->symbol()->type()->baseType();
     } 
     if(e->lhs())
        e1 = &(e->lhs()->copy());
     else  
        e1 = &(c1.copy());

     if(e->rhs())
        e2 = &(e->rhs()->copy());
     else  
       e2 = StringLengthExpr(t,sub->symbol()); //new SgValueExp(CharLength(t)); 14.03.03
      return (&(*e2 - *e1 + c1));
}
*/


SgExpression * InputItemLength (SgExpression *e, SgStatement *stmt)
{
 if(isSgRecordRefExp(e))
 {   
     e = RightMostField(e); 
         //printf("FIELD: %s  %d ",(e->symbol() ? e->symbol()->identifier() : (char *)"----"),(e->type() ? e->type()->variant() : 0));
         //printf("  LINE   %d IN %s\n" ,stmt->lineNumber(),stmt->fileName() );  
 }
 if (isSgVarRefExp(e))
   return(TypeLengthExpr(e->type()));  
      //return(new SgValueExp(TypeSize(e->type()))); 14.03.03 
 if (isSgArrayRefExp(e))
 {
     if (e->symbol()->type()->variant() == T_STRING) // variable of type CHARACTER
         return(StringLengthExpr(e->symbol()->type(), e->symbol()));
     //return(new SgValueExp(CharLength(e->symbol()->type()))); 14.03.03
     else
     {
         if (e->lhs() && !isSgArrayType(e->type())) //array element
             return(TypeLengthExpr(e->symbol()->type()->baseType()));
         else if (e->lhs() && isSgArrayType(e->type())) //array section
             return(ContinuousSection(e) ? SectionLength(e, stmt, 1) : ArrayLength(e->symbol(), stmt, 1));
         else  //whole array
             return(ArrayLength(e->symbol(), stmt, 1));
     }
 }

 if (e->variant() == ARRAY_OP) //substring or substring of array element
           return(SubstringLength(e)); //substring

           return(new SgValueExp(-1)); 
}

SgExpression *SubstringLength(SgExpression *sub)
{ //SgSubscriptExp *sbe;
  SgValueExp c1(1);
  SgExpression *e,*e1,*e2;
  SgType *t;
  
//err("Sorry, substring length calculating is not jet implemented",cur_st);
     if(!sub->lhs()->lhs()){  //substring   
        t=sub->lhs()->symbol()->type();
        e = sub->rhs()->lhs(); // sub->rhs() has variant EXPR_LIST
     }
     else{                   //substring of array element
        t=sub->lhs()->symbol()->type()->baseType();
        e = sub->rhs(); 
     }  
     if(e->lhs())
        e1 = &(e->lhs()->copy());
     else  
        e1 = &(c1.copy());

     if(e->rhs())
        e2 = &(e->rhs()->copy());
     else  
       e2 = StringLengthExpr(t,sub->lhs()->symbol()); //new SgValueExp(CharLength(t));
      return (&(*e2 - *e1 + c1));
}

SgExpression *ArrayLength(SgSymbol *ar, SgStatement *stmt, int err)
{int i,rank;
 SgExpression *esize,*len; 
rank = Rank(ar);
len = TypeLengthExpr(ar->type()->baseType()); //length of one array element
  //len = new SgValueExp(TypeSize(ar->type()->baseType())); 14.03.03
for(i=1; i<=rank; i++) {
    //calculating size of i-th dimension
    esize = ReplaceParameter(ArrayDimSize(ar, i));
    if(err && esize && esize->variant()==STAR_RANGE)
      Error("Assumed-size array: %s",ar->identifier(),162,stmt);
    if(esize->isInteger())
      esize = new SgValueExp( esize->valueInteger());
    if(esize)
      len = &(*len * (*esize));
    
}
if (len->isInteger()) // calculating length if it is possible
    len = new SgValueExp( len->valueInteger()); 
return(len);
}

SgExpression *SectionLength(SgExpression *ea, SgStatement *stmt, int err)
{int i,rank;
 SgExpression *esize,*len, *el, *eup[MAX_DIMS], *ein[MAX_DIMS]; 
 //rank = ArraySectionRank(ea);
 rank = Rank(ea->symbol());  
 len = TypeLengthExpr(ea->symbol()->type()->baseType()); //length of one array element

  
 for(i=0,el=ea->lhs(); i<rank && el; i++,el=el->rhs()) {
    //calculating size of i-th dimension
    UpperBoundInTriplet(el->lhs(),ea->symbol(),i,eup);
    LowerBoundInTriplet(el->lhs(),ea->symbol(),i,ein);
    esize = &(*eup[i] - *ein[i] + *new SgValueExp(1));
       //if(err && esize && esize->variant()==STAR_RANGE)
       //  Error("Assumed-size array: %s",ar->identifier(),162,stmt);
       //if(esize->isInteger())
       //  esize = new SgValueExp( esize->valueInteger());
    if(esize)
      len = &(*len * (*esize));
    
}
     //if (len->isInteger()) // calculating length if it is possible
     //    len = new SgValueExp( len->valueInteger()); 
return(len);
}

SgExpression *ArrayLengthInElems(SgSymbol *ar, SgStatement *stmt, int err)
{int i,rank;
 SgExpression *esize,*len; 
rank = Rank(ar);
len =  new SgValueExp(1);
for(i=1; i<=rank; i++) {
    //calculating size of i-th dimension
    esize = ReplaceParameter(ArrayDimSize(ar, i));
    if(err && esize && esize->variant()==STAR_RANGE)
      Error("Assumed-size array: %s",ar->identifier(),162,stmt);
    if(esize->isInteger())
      esize = new SgValueExp( esize->valueInteger());
    if(esize)
      len = &(*len * (*esize));
    
}
if (len->isInteger()) // calculating length if it is possible
    len = new SgValueExp( len->valueInteger()); 
return(len);
}

SgExpression *NumbOfElem(SgExpression *es,SgExpression *el)
{SgExpression *e,*e1 = NULL,*ec;
 if(!es)
   return(NULL);
 if(es->isInteger())
   return(new SgValueExp( es->valueInteger() / el->valueInteger()));
                                                           //deleting on length of element
 ec = &es->copy();
 for(e=ec; e->variant() == MULT_OP; e=e->lhs())
    e1 = e;
 e1->setLhs(new SgValueExp(1)); //replace length of element by 1
 return(ec);
}

SgExpression *ElemLength(SgSymbol *ar)
{SgExpression *len;
len = TypeLengthExpr(ar->type()->baseType()); //length of one array element
//len = new SgValueExp(TypeSize(ar->type()->baseType()));  14.03.03
 return(len);
}

SgExpression *CorrectLastOpnd(SgExpression *len, SgSymbol *ar, SgExpression *bounds,SgStatement *stmt)
{SgExpression *elast;
 SgValueExp c1(1);
 if(!Rank(ar))
   return(len); //error situation 
 if(!bounds->rhs()){  //step of implicit loop is absent ,by default 1  
    elast=&(bounds->lhs()->rhs()->copy() - *Exprn(LowerBound(ar,Rank(ar)-1)) + c1);
                  //upper_bound_of_implicit_loop - lower_bound_of_last_dimension_of_array + 1
    if (elast->isInteger()) // calculating size if it is possible
      elast = new SgValueExp( elast->valueInteger()); 
    if(len->variant() == MULT_OP)       
      len->setRhs(elast); //replace last multiplicand of array length
    else
      len = &(*len * (*elast));//len is the length of array element,it is multiplied by elast
 }
 else // variant == SEQ,there is a step
    Error("Can not calculate array length: %s", ar->identifier(),194,stmt);
 if (len->isInteger()) // calculating length if it is possible
   len = new SgValueExp( len->valueInteger()); 
 return(len);
} 
            
SgSymbol *lastDimInd(SgExpression *el)
{//returns symbol of last subscript expression  if it is variable refference 
 //el - subscript list
 SgExpression *last = NULL;
 for(; el; el=el->rhs()) //search for last subscript
     last = el->lhs();
 if(isSgVarRefExp(last)) //is variable refference
    return(last->symbol());
 return(NULL);
}   

int hasAsterOrOneInLastDim(SgSymbol *ar)
{//is dummy argument or array in COMMON declared as a(n,n,*) or a(1)
 SgExpression *e;
 SgValueExp *ev;
 int rank;
 rank = Rank(ar);
 if(!rank)
   return(0);
 e=ArrayDimSize(ar,rank);
 if(e->variant()==STAR_RANGE)  
   return(1);
 if(rank==1 && (ev = isSgValueExp(e)) && ev->intValue() == 1)
    return(1);
 return(0);
}            

SgExpression *FirstArrayElement(SgSymbol *ar)
{//generating reference AR(L1,...,Ln), where Li - lower bound of i-th dimension
 int i;
 SgExpression *esl, *el, *e;
 el = NULL;
 for (i = Rank(ar); i; i--){
  esl = new SgExprListExp(*Exprn(LowerBound(ar,i-1)));
  esl->setRhs(el);
  el = esl;
 }
  e = new SgArrayRefExp(*ar);
  e->setLhs(el);
  return(e);
}
  
SgExpression *FirstElementOfSection(SgExpression *ea) 
{SgExpression *el, *ein[MAX_DIMS];
 int i,rank;
 SgExpression *esl, *e;
 SgSymbol * ar;
 ar = ea->symbol();
 rank = Rank(ar);
 if(!ea->lhs()) //whole array
   return(FirstArrayElement(ar));

 for(el=ea->lhs(),i=0; el && i<rank; el=el->rhs(),i++)    
     LowerBoundInTriplet(el->lhs(),ar,i, ein);
 el = NULL;
 for (i = rank; i; i--){
   esl = new SgExprListExp(*Exprn(ein[i-1]));
   esl->setRhs(el);
   el = esl;
 }
  e = new SgArrayRefExp(*ar);
  e->setLhs(el);
  return(e);
}

SgExpression *ArrayFieldLast(SgExpression *e)
{ 
  while(isSgRecordRefExp(e) && RightMostField(e)->type()->variant() != T_ARRAY)
      e=e->lhs();
  //e->unparsestdout(); printf("\n");
  return(e);
}

SgExpression *FirstElementOfField(SgExpression *e_RecRef)
{ 
  SgExpression *estr = &e_RecRef->copy();
  estr->setRhs(FirstElementOfSection(RightMostField(estr)) );
  return (estr);
}

int ArraySectionRank(SgExpression *ea)
{SgExpression *el;
 int rank;
 for(el=ea->lhs(),rank=0; el; el=el->rhs()) 
  if(el->lhs()->variant() == DDOT)
    rank++;
 return(rank);
}

int ContinuousSection(SgExpression *ea)
{ SgExpression *ei;

  ei = ea->lhs();
  if(ei->lhs()->variant() != DDOT)
     return(0);
  while(ei && isColon(ei->lhs()))
     ei = ei->rhs();
  if(!ei)      // (:,:,...:)
     return(1);
  //if(ei->lhs()->variant() == DDOT && ei->lhs()->lhs()->variant() == DDOT)   //there is step
  //   return (0);
  ei = ei->rhs();
  while(ei && ei->lhs()->variant() != DDOT)
     ei = ei->rhs();  
  if(!ei)
     return(1); 
  return(0);

}

int isColon(SgExpression *e)
{
  if(!e)
    return(0);
  if(e->variant() == DDOT && !e->lhs() && !e->rhs())
    return(1);
  return(0);
 
}


int hasEndErrControlSpecifier(SgStatement *stmt, SgExpression *ioEnd[] )
{
  SgExpression *el, *ee;
  SgExpression *e = stmt->expr(1);  //control list
  ioEnd[0] = ioEnd[1] = ioEnd[2] = NULL; 
  if(!e) return 0;
  if(e->variant() == EXPR_LIST){
    for(el=e; el; el = el->rhs()) {
       ee = el->lhs();
       if(ee->variant() != SPEC_PAIR)
          return 0; // IO_control list error
       SgKeywordValExp *kwe = isSgKeywordValExp(ee->lhs());   
       if(!kwe)
          return 0;
       if (!strcmp(kwe->value(),"iostat"))        
               return 0;
       else if (!strcmp(kwe->value(),"err")) 
               ioEnd[0]  =  el;  
       else if (!strcmp(kwe->value(),"end")) 
               ioEnd[1] =  el; 
       //else if (!strcmp(kwe->value(),"eor")) 
       //        ioEnd[2]  = el;  
       else
               continue;
    }
    if(ioEnd[0] || ioEnd[1] || ioEnd[2])
       return 1;
    else
       return 0;
  }    
  else
    return 0;
}

void ChangeSpecifierByIOSTAT(SgExpression *e)
{
  // e->variant() == SPEC_PAIR
  e->setLhs( new SgKeywordValExp("iostat"));
  e->setRhs( new SgVarRefExp(IOstatSymbol()) ) ;
}

void ChangeControlList(SgStatement *stmt, SgExpression *ioEnd[] )
{
  SgExpression *el;
  // replace one of the specifiers with IOSTAT
  for(el=stmt->expr(1); el; el=el->rhs())
     if(el==ioEnd[0] || el==ioEnd[1] || el==ioEnd[2]) 
     {
       ChangeSpecifierByIOSTAT(el->lhs());
       break; 
     }
  // delete others 
  while(el->rhs())  
  {
     if(el->rhs()==ioEnd[0] || el->rhs()==ioEnd[1] || el->rhs()==ioEnd[2]) 
     {
        el->setRhs(el->rhs()->rhs());
        continue;
     }
     else
        el=el->rhs();
  }
  return; 
}

void ReplaceStatementWithEndErrSpecifier(SgStatement *stmt, SgExpression *ioEnd[] )
{ 
  int i; 
  for(i=0; i<3; i++)
     if(ioEnd[i])
        doLogIfForIOstat(IOstatSymbol(),ioEnd[i]->lhs(),stmt);
  ChangeControlList(stmt,ioEnd);
}

/*--------------------------------------------------------------------------------------*/
/*      RTS2 interface                                                                  */
/*--------------------------------------------------------------------------------------*/

static inline int strcmpi(const char *s1, const char *s2) {
  size_t l1 = strlen(s1);
  size_t l2 = strlen(s2);
  size_t min_l = (l1 < l2? l1 : l2);
  char c1, c2;
  for (size_t i = 0; i < min_l; ++i) {
	c1 = tolower(s1[i]);
	c2 = tolower(s2[i]);
	if (c1 > c2) return 1;
	else if (c1 < c2) return -1;
  }
  if (l1 > min_l) return 1;
  else if (l2 > min_l) return -1;
  return 0;
}

const char *stringValuesOfArgs(int argNumber, SgStatement *stmt) {
  int variant = stmt->variant();
  
  if (variant == OPEN_STAT || variant == CLOSE_STAT) return openCloseArgStrings[argNumber];
  else if (variant == READ_STAT || variant == WRITE_STAT) return readWriteArgStrings[argNumber];
  else if (variant == ENDFILE_STAT || variant == REWIND_STAT || variant == BACKSPACE_STAT) return filePositionArgsStrings[argNumber];
  
  return NULL;
};

bool checkDefaultStringArg(SgExpression *arg, const char **possible_values, int count, int i, SgStatement *stmt, int error_msg) {
  
  // if default-string arg isn't a value expression, it can't be checked.
  if (!(arg && isSgValueExp(arg))) return true;
  SgValueExp *v = isSgValueExp(arg);
  
  char *string_val = v->stringValue();
  for (int string_arg_number = 0; string_arg_number < count; ++string_arg_number)
     if (!strcmpi(string_val, possible_values[string_arg_number])) return true;

  const char *stringArg = stringValuesOfArgs(i, stmt);
  if (error_msg)
     Error("Wrong value of '%s' argument in IO-statement", stringArg, 454, stmt);
  return false;
  
}

bool checkLabelRefArg(SgExpression *arg, SgStatement *stmt, int error_msg) {
  if (!arg) return true;
  SgLabelRefExp *lbl = isSgLabelRefExp(arg);
  if (!lbl) {
	if (error_msg)
	  err("Wrong type of label argument", 450, stmt);
	return false;
  }
  return true;
}

bool checkIntArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg) {
  if (!arg) return true;
  SgValueExp *val = isSgValueExp(arg);
  SgVarRefExp *var = isSgVarRefExp(arg);
  
  if (val && val->variant() == INT_VAL) return true;
  if (var && var->symbol()->type()->variant() == T_INT) return true;
  if (arg->type()->variant() == T_INT) return true;
  
  const char *stringArg = stringValuesOfArgs(i, stmt);
  if (error_msg)
    Error("Wrong type of '%s' argument in IO-statement", stringArg, 450, stmt);
  return false;
  
}

bool checkStringArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg) {
  if (!arg) return true;
  
  SgValueExp *val = isSgValueExp(arg);
  SgArrayRefExp *arr = isSgArrayRefExp(arg);
  if (val && val->variant() == STRING_VAL) return true;
  if (arr && arr->symbol()->type()->variant() == T_STRING) return true;
  if (arg->type()->variant() == T_STRING) return true;
  
  const char *stringArg = stringValuesOfArgs(i, stmt);
  if (error_msg)
    Error("Wrong type of '%s' argument in IO-statement", stringArg, 450, stmt);
  return false;
  
}

bool checkStringVarArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg) {
  if (!arg) return true;
  SgArrayRefExp *arr = isSgArrayRefExp(arg);
  if (!arr || arr->symbol()->type()->variant() != T_STRING) {
	const char *stringArg = stringValuesOfArgs(i, stmt);
	if (error_msg)
	  Error("Wrong type of '%s' argument in IO-statement", stringArg, 450, stmt);
	return false;
  }
  return true;
}

bool checkVarRefIntArg(SgExpression *arg, int i, SgStatement *stmt, int error_msg) {
  if (!arg) return true;
  SgVarRefExp *var = isSgVarRefExp(arg);
  
  if (!var || !(var->symbol()->type()->variant() == T_INT)) {
	const char *stringArg = stringValuesOfArgs(i, stmt);
	if (error_msg)
	  Error("Wrong type of '%s' argument in IO-statement", stringArg, 450, stmt);
	return false;
  }
  return true;
}

bool checkUnitAndNewUnit(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  if (ioc[UNIT_IO] && ioc[NEWUNIT_IO]) {
	if (error_msg)
	  err("Wrong combination of arguments: both unit and newunit arguments specified", 452, stmt);
	return false;
  }
  if (!ioc[UNIT_IO] && !ioc[NEWUNIT_IO]) {
	if (error_msg)
	  err("Neither unit nor newunit specified in OPEN statement", 451, stmt);
	return false;
  }
  return true;
}

// forbids sequential and direct access
bool checkAccessArg(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  // stream access is not a default value, so if access it omitted, there's an error
  if (!ioc[ACCESS_IO]) {
	if (error_msg)
	  err("Only stream access is allowed in parallel IO", 455, stmt);
	return false;
  }
  SgValueExp *access = isSgValueExp(ioc[ACCESS_IO]);
  if (!access) return true;
  if (!strcmpi(access->stringValue(), "stream")) return true;
  
  if (error_msg)
	err("Only stream access is allowed in parallel IO", 455, stmt);
  return false;
}

// forbids formatted input
bool checkFormArg(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  // if access is stream, default form argument value is formatted
  // if access isn't stream, this stmt is already treated as wrong
  if (!ioc[FORM_IO]) return true;
  SgValueExp *form = isSgValueExp(ioc[FORM_IO]);
  if (!form) return true;
  if (!strcmpi(form->stringValue(), "unformatted")) return true;
  
  if (error_msg)
	err("Formatted form is not allowed in parallel IO", 455, stmt);
  return false;
}

bool checkFormattedArgs(SgExpression **ioc, SgStatement *stmt, int error_msg) {
	/* if form specifier is omitted, it's considered to be unformatted. */
  SgExpression *form = ioc[FORM_IO];
  if (!form || (form && isSgValueExp(form) && !strcmpi(isSgValueExp(form)->stringValue(), "unformatted"))) {
    if (ioc[BLANK_IO] || ioc[DECIMAL_IO] || ioc[DELIM_IO] || ioc[ENCODING_IO] || ioc[PAD_IO] || ioc[ROUND_IO] || ioc[SIGN_IO])
    {
       if (error_msg)
           err("Formatted arguments used in unformatted IO.", 453, stmt);
       return false;
    }
  }
  return true;
}

bool checkStatusArg(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  if (!ioc[STATUS_IO]) return true;
  if (!isSgValueExp(ioc[STATUS_IO])) return true;
  char *string_val = isSgValueExp(ioc[STATUS_IO])->stringValue();
  
  if ((!strcmpi(string_val, "new") || !strcmpi(string_val, "replace")) && !ioc[FILE_IO]) {
	if (error_msg)
	  err("Wrong combination of arguments: if status argument is \"new\" or \"replace\", file argument shall be specified", 452, stmt);
	return false;
  }
  if (!strcmpi(string_val, "scratch") && ioc[FILE_IO]) {
	if (error_msg)
	  err("Wrong combination of arguments: if status argument is \"scratch\", file argument shall not be specified", 452, stmt);
	return false;
  }
  return true;
  
}

bool checkDvmModeArg(char const *io_modes_str, SgStatement *stmt, int error_msg) {
  
  if (!io_modes_str || !io_modes_str[0]) return true;
  bool l = false;
  bool p = false;
  for (int i = 0; *io_modes_str && i < 3; ++i) {
	if (io_modes_str[i] == 'l') l = true;
	else if (io_modes_str[i] == 'p') p = true;
  }
  if (l && p) {
	if (error_msg)
	  err("Wrong combination of arguments: local and parallel mode simultaneously used", 452, stmt);
	return false;
  }
  return true;
}

bool checkNewunitArgument(SgExpression **ioc, SgStatement *stmt, int error_msg) {
	/*
	 If the NEWUNIT= specifier appears in an OPEN statement, either the FILE= specifier shall appear, or the STATUS= specifier shall appear with a value of SCRATCH. The unit identified by a NEWUNIT value shall not be preconnected.
	 
	 newunit ==> (file xor status == 'scratch')
	 
	 !(newunit ==> (file xor status == 'scratch'))
	 !(!newunit || (file xor status == 'scratch'))
	 newunit && !(file xor status == 'scratch')
	 
	 a xor b = (!a^b || a^!b)
	 
	 newunit && !( (file && status != 'scratch') || (!file && status == 'scratch') )
	 newunit && !(file && status != 'scratch') && !(!file && status == 'scratch')
	 newunit && (!file || status == 'scratch') && (file || status != 'scratch')
	 
	 */
	
	SgExpression *newunit = ioc[NEWUNIT_IO];
	SgExpression *file = ioc[FILE_IO];
	SgExpression *status = ioc[STATUS_IO];
	
	bool status_scratch = (status && !isSgValueExp(status)) || (status && isSgValueExp(status) && !strcmpi(isSgValueExp(status)->stringValue(), "scratch"));
	bool status_not_scratch = !status || (status && isSgValueExp(status) && strcmpi(isSgValueExp(status)->stringValue(), "scratch"));
	
	if (newunit && (!file || status_scratch) && (file || status_not_scratch))	{
		if (error_msg)
			err("Wrong combination of arguments: newunit argument shall be specified together with either file argument, or with status argument equal to \"scratch\"", 452, stmt);
		return false;
	}

	return true;
	
}

bool checkFileArg(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  // FILE ARG If this specifier is omitted and the unit is not connected to a file, the STATUS= specifier shall be specified with a value of SCRATCH
  // !((file && !unit) -> status='scratch') = ((file && !unit) && !status='scratch')
  if (isSgVarRefExp(ioc[STATUS_IO])) return true;
  if (ioc[FILE_IO] && !ioc[UNIT_IO] && ioc[STATUS_IO] && isSgValueExp(ioc[STATUS_IO]) && strcmpi(isSgValueExp(ioc[STATUS_IO])->stringValue(), "scratch")) {
	if (error_msg)
	  err("Wrong combination of arguments: file argument specified, unit not specified and status isn't \"scratch\"", 452, stmt);
	return false;
  }
  return true;
}

bool checkReclArg(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  
  /*
   The value of the RECL= specifier shall be positive.
   This specifier shall not appear when a file is being connected for stream access.
   This specifier shall appear when a file is being connected for direct access.
   */
  
  SgExpression *recl = ioc[RECL_IO];
  SgExpression *access = ioc[ACCESS_IO];
  
  if (isSgVarRefExp(recl)) return true;
  if (recl && isSgValueExp(recl)->intValue() <= 0) {
	if (error_msg)
	  err("Wrong value of argument: recl argument should be positive", 455, stmt);
	return false;
  }
  if (isSgVarRefExp(access)) return true;
  if (recl && access && isSgValueExp(access) && !(strcmpi(isSgValueExp(access)->stringValue(), "stream"))) {
	if (error_msg)
	  err("Wrong combination of arguments: recl argument used with stream file", 452, stmt);
	return false;
  }
  if (!recl && access && isSgValueExp(access) && !(strcmpi(isSgValueExp(access)->stringValue(), "direct"))) {
	if (error_msg)
	  err("Wrong combination of arguments: recl argument should be used with direct file", 452, stmt);
	return false;
  }
  return true;
}

bool checkPosArg(SgExpression **ioc, SgStatement *stmt, int error_msg) {
	// The connection shall be for sequential or stream access.
	// error if is position is specefied, access is scecified and access is direct
	SgExpression *access = ioc[ACCESS_IO]; // default is sequantal, so, it's correct if it's omitted
	if (isSgValueExp(access)) return true;
	if (ioc[POSITION_IO]  && access && !strcmpi(isSgValueExp(access)->stringValue(), "direct")) {
		if (error_msg)
			err("Wrong combination of arguments: position argument may be specified only for direct and sequential access", 452, stmt);
		return false;
	}
	return true;
}

bool checkArgsClose(SgExpression **ioc, SgStatement *stmt, int error_msg) {
  
  bool correct = true;
  
  if (!checkIntArg(ioc[UNIT_IO], UNIT_IO, stmt, error_msg)) correct = false;
  if (!checkLabelRefArg(ioc[ERR_IO], stmt, error_msg)) correct = false;
  if (!checkVarRefIntArg(ioc[IOSTAT_IO], IOSTAT_IO, stmt, error_msg)) correct = false;
  if (!checkStringVarArg(ioc[IOMSG_IO], IOMSG_IO, stmt, error_msg)) correct = false;
  if (!checkStringArg(ioc[STATUS_IO], STATUS_IO, stmt, error_msg)) correct = false;
  
  if (!correct) return false;
  
  const char *pos_val_status[] = { "keep", "delete" };
  if (!checkDefaultStringArg(ioc[STATUS_IO], pos_val_status, 2, STATUS_IO, stmt, error_msg)) correct = false;
  return correct;
}

bool checkArgsOpen(SgExpression **ioc, SgStatement *stmt, int error_msg, char const *io_modes_str) {
	
	// for every argument we should check if it has a correct type
	// then check some special restricitions
	// then check that all the arguments have correct values
	bool correct = true;
	
	if (!checkLabelRefArg(ioc[ERR_IO], stmt, error_msg)) correct = false;
	
	if (!checkIntArg(ioc[UNIT_IO], UNIT_IO, stmt, error_msg)) correct = false;
	if (!checkIntArg(ioc[RECL_IO], RECL_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[ACCESS_IO], ACCESS_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[ACTION_IO], ACTION_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[ASYNC_IO], ASYNC_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[BLANK_IO], BLANK_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[DECIMAL_IO], DECIMAL_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[DELIM_IO], DELIM_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[ENCODING_IO], ENCODING_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[FILE_IO], FILE_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[FORM_IO], FORM_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[PAD_IO], PAD_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[POSITION_IO], POSITION_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[ROUND_IO], ROUND_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[SIGN_IO], SIGN_IO, stmt, error_msg)) correct = false;
	if (!checkStringArg(ioc[STATUS_IO], STATUS_IO, stmt, error_msg)) correct = false;
	
	// dvm io mode produces mistake!
	if (!checkStringArg(ioc[DVM_MODE_IO], DVM_MODE_IO, stmt, error_msg)) correct = false;
	if (!checkVarRefIntArg(ioc[IOSTAT_IO], IOSTAT_IO, stmt, error_msg)) correct = false;
	if (!checkVarRefIntArg(ioc[NEWUNIT_IO], NEWUNIT_IO, stmt, error_msg)) correct = false;
	if (!checkStringVarArg(ioc[IOMSG_IO], IOMSG_IO, stmt, error_msg)) correct = false;
	
	if (!correct) return false;
	
	/* FILE argument may have any value; it shouldn't checked */
	const int string_args[14] = { ACCESS_IO, ACTION_IO, ASYNC_IO, BLANK_IO, DECIMAL_IO, DELIM_IO, ENCODING_IO /*, FILE_IO */, FORM_IO, PAD_IO, POSITION_IO, ROUND_IO, SIGN_IO, STATUS_IO, DVM_MODE_IO };
	
	const char *pos_val_access[] = { "sequental", "direct", "stream" }; //3
	const char *pos_val_action[] = { "read", "write", "readwrite"}; //3
	const char *pos_val_async[] = { "yes", "no"}; // 2
	const char *pos_val_blank[] = { "null", "zero"}; // 2
	const char *pos_val_decimal[] = { "comma", "point"}; // 2
	const char *pos_val_delim[] = { "apostrophe", "quote", "none" }; // 3
	const char *pos_val_encoding[] = { "utf-8", "default"}; // 2
	const char *pos_val_form[] = { "formatted", "unformatted"}; // 2
	const char *pos_val_pad[] = { "yes", "no"}; // 2
	const char *pos_val_position[] = { "asis", "rewind", "append"}; // 3
	const char *pos_val_round[] = { "up", "down", "zero", "nearest", "compatible", "processor_defined" }; // 6
	const char *pos_val_sign[] = { "plus", "suppress", "processor_defined" }; // 3
	const char *pos_val_status[] = { "old", "new", "replace", "unknown" }; // 4
	
	const char **pos_values[] = {pos_val_access, pos_val_action, pos_val_async, pos_val_blank, pos_val_decimal, pos_val_delim, pos_val_encoding,
	 pos_val_form, pos_val_pad, pos_val_position, pos_val_round, pos_val_sign, pos_val_status };
	const int arg_count[] = { 3, 3, 2, 2, 2, 3, 2, 2, 2, 3, 6, 3, 4 };
	
	for (int i = 0; i < 13; ++i) {
		if (!checkDefaultStringArg(ioc[string_args[i]], pos_values[i], arg_count[i], string_args[i], stmt, error_msg))
			correct = false;
	}
	
	if (!checkAccessArg(ioc, stmt, error_msg)) correct = false;
	if (!checkFormArg(ioc, stmt, error_msg)) correct = false;
	if (!checkFormattedArgs(ioc, stmt, error_msg)) correct = false;
	if (!checkPosArg(ioc, stmt, error_msg)) correct = false;
	if (!checkUnitAndNewUnit(ioc, stmt, error_msg)) correct = false;
	if (!checkNewunitArgument(ioc, stmt, error_msg)) correct = false;
	if (!checkReclArg(ioc, stmt, error_msg)) correct = false;
	if (!checkStatusArg(ioc, stmt, error_msg)) correct = false;
	
	if (!checkDvmModeArg(io_modes_str, stmt, error_msg)) correct = false;
	return correct;
	
}

bool checkArgsEnfileRewind(SgExpression **ioc, SgStatement *stmt, int error_msg) {
	/*
	 DVMH_API void dvmh_ftn_endfile_(const DvmType *pUnit, const VarRef *pErrFlagRef, const VarRef *pIOStatRef, const StringVarRef *pIOMsg);
	 DVMH_API void dvmh_ftn_rewind_(const DvmType *pUnit, const VarRef *pErrFlagRef, const VarRef *pIOStatRef, const StringVarRef *pIOMsg);
	 */
  bool correct = true;
  
  if (stmt->variant() == BACKSPACE_STAT) {
    if (error_msg)
      warn("Backspace statement isn't implemented in new IO", 0, stmt); // FIXME: error number
    correct = false;
  }
	
	if (!checkIntArg(ioc[UNIT_], UNIT_, stmt, error_msg)) correct = false;
	if (!ioc[UNIT_]) {
		if (error_msg)
			err("Unit argument not specified in file position statement", 456, stmt);
		correct = false;
	}
	if (!checkLabelRefArg(ioc[ERR_], stmt, error_msg)) correct = false;
	if (!checkVarRefIntArg(ioc[IOSTAT_],IOSTAT_, stmt, error_msg)) correct = false;
	if (!checkStringVarArg(ioc[IOMSG_], IOMSG_, stmt, error_msg)) correct = false;
	return correct;
	
}

bool checkArgsRW(SgExpression **ioc, SgStatement *stmt, int error_msg) {
	
	bool correct = true;
  
  /* these arguments are forbidden in both new and old IO: blank, delim, decimal, eor, pad, sign */
  if (ioc[BLANK_RW] || ioc[DELIM_RW] || ioc[DECIMAL_RW] || ioc[EOR_RW] || ioc[PAD_RW] || ioc[SIGN_RW] || ioc[ROUND_RW])
  {
    if (error_msg)
      err("Arguments forbidden in both new and old IO used", 453, stmt); // FIXME: number or error?
    correct = false;
  }
  
  /* these arguments are forbidden only in new IO, so only warning should be showed */
  /* these arguments aren't added to argument, so it's unnessecary to care about what will be with them */
  if (ioc[FMT_RW] || ioc[NML_RW] || ioc[ADVANCE_RW] || ioc[REC_RW] || ioc[SIZE_RW]) {
    if (error_msg)
      warn("Arguments not allowed in new IO used", 453, stmt);  // FIXME: number or error?
    correct = false;
  }

	checkIntArg(ioc[UNIT_RW], UNIT_RW, stmt, error_msg);
	
	if (stmt->variant() == WRITE_STAT && ioc[END_RW]) {
		if (error_msg)
			err("Illegal elements in control list", 185, stmt);
		correct = false;
	}
	else if (!checkLabelRefArg(ioc[END_RW], stmt, error_msg)) correct = false;
	
	if (!checkLabelRefArg(ioc[ERR_RW], stmt, error_msg)) correct = false;
	if (!checkVarRefIntArg(ioc[IOSTAT_RW], IOSTAT_RW, stmt, error_msg)) correct = false;
	if (!checkStringVarArg(ioc[IOMSG_RW], IOMSG_RW, stmt, error_msg)) correct = false;
	if (!checkIntArg(ioc[POS_RW], POS_RW, stmt, error_msg)) correct = false;
	
	SgExprListExp *items = isSgExprListExp(isSgInputOutputStmt(stmt)->itemList());
	if (items == NULL) {
    if (ioc[NML_RW]) {
      if (error_msg)
        warn("Namelist argument is not supported in new IO", 457, stmt); // FIXME: error number
      return false; // further checking is unnecceasry, because there's no item to reading/writing
    }
    else {
      if (error_msg)
        err("Subject for reading/writing not specified", 457, stmt);
      return false; // further checking is unnecceasry, because there's no item to reading/writing
    }
	}
  
  if (stmt->variant() == READ_STAT) {
    for (int i = 0; i < items->length(); ++i) {
      SgExpression *item = items->elem(i);
      if (!(item->variant() == VAR_REF || item->variant() == ARRAY_REF || item->variant() == ARRAY_OP)) {
        if (error_msg)
          err("Wrong type of argument in IO-statement: reading item is not a variable", 450, stmt);
        correct = false;
      }
    }
  }
  /* array expressions are not yet implemented in new IO, but are allowed in old IO */
  else {
    for (int i = 0; i < items->length(); ++i) {
      SgExpression *item = items->elem(i);
      // forbidding array expressions such as A+B
      // substrings, array elements and sections are still allowed
      if (isSgArrayType(item->type()) && !item->symbol()) {
        if (error_msg)
          warn("Not implemented item type for writing in new IO: array expressions", 458, stmt);
        correct = false;
      }
    }
  }
  
  return correct;
}

SgStatement *IfConnected(SgStatement *stmt, SgExpression *unit, bool suitableForNewIO) 
{
  // generate If construct:  
  //     if (dvmh_ftn_connected ( unit,suitableForNewIO ) then
  //            CONTINUE
  //     else
  //            stmt
  //     endif

	SgValueExp one(1); 
        SgStatement *cp = stmt->controlParent();
        cur_st = stmt->lexNext();
	stmt->extractStmt();
	SgStatement *trueBody = new SgStatement(CONT_STAT); //CONTINUE statement
	SgStatement *falseBody = stmt;	
	SgExpression *failIfYes = suitableForNewIO ? ConstRef(0) : ConstRef(1); // ????????
        	
	SgIfStmt *ifst = new SgIfStmt(SgEqOp(*DvmhConnected(DvmType_Ref(unit), failIfYes), one), *trueBody, *falseBody);
	
	cur_st->insertStmtBefore(*ifst, *cp);
	
	cur_st = trueBody;

       if (stmt-> hasLabel()) {    // IO statement has label
       // the label of IO statement is transfered on IF statement  
          BIF_LABEL(stmt->thebif) = NULL; 
          ifst->setLabel(*stmt->label());  
       }
       char *cmnt=stmt-> comments();
       if (cmnt) {    // IO statement has preceeding comments
       // the comment of IO statement is transfered on IF statement  
          BIF_CMNT(stmt->thebif) = NULL;
          ifst -> setComments(cmnt);
       }

        return ifst; 
}

int control_list_open_new(SgExpression *e, SgExpression *ioc[])
// analizes control list (e) for OPEN
// and sets on ioc[]
{ SgKeywordValExp *kwe;
  SgExpression *ee,*el;
  int i;
  for(i=NUMB__CL; i; i--)
	ioc[i-1] = NULL;
  
  if(e->variant() == SPEC_PAIR) {
	kwe = isSgKeywordValExp(e->lhs());
	if (!kwe || !strcmp(kwe->value(), "unit"))
	  ioc[UNIT_IO] = e->rhs();
	else if (!strcmp(kwe->value(), "newunit"))
	  ioc[NEWUNIT_IO] = e->rhs();
	else return 0;
	
	return(1);
  }
  if(e->variant() == EXPR_LIST){
	for(el=e; el; el = el->rhs()) {
	  ee = el->lhs();
	  if(ee->variant() != SPEC_PAIR)
		return(0); // IO_control list error
	  kwe = isSgKeywordValExp(ee->lhs());
	  if(!kwe)
		return(0);
	  if     (!strcmp(kwe->value(),"unit"))
		ioc[UNIT_IO] = ee->rhs();
	  else if (!strcmp(kwe->value(),"access"))
		ioc[ACCESS_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"action"))
		ioc[ACTION_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"async"))
		ioc[ASYNC_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"blank"))
		ioc[BLANK_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"decimal"))
		ioc[DECIMAL_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"delim"))
		ioc[DELIM_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"encoding"))
		ioc[ENCODING_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"file"))
		ioc[FILE_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"form"))
		ioc[FORM_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"iostat"))
		ioc[IOSTAT_IO] = ee->rhs();
	  else if (!strcmp(kwe->value(),"iomsg"))
		ioc[IOMSG_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"newunit"))
		ioc[NEWUNIT_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"pad"))
		ioc[PAD_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"position"))
		ioc[POSITION_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"recl"))
		ioc[RECL_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"round"))
		ioc[ROUND_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"sign"))
		ioc[SIGN_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"status"))
		ioc[STATUS_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"err"))
		ioc[ERR_IO]  = ee->rhs();
	  else
		return(0);
	}
	return(1);
  }
  else
	return(0);
}

int control_list_close_new(SgExpression *e, SgExpression *ioc[])
// analizes control list (e) for CLOSE
// and sets on ioc[]
{ SgKeywordValExp *kwe;
  SgExpression *ee,*el;
  int i;
  for(i=NUMB__CL; i; i--)
	ioc[i-1] = NULL;
  
  if(e->variant() == SPEC_PAIR) {
	kwe = isSgKeywordValExp(e->lhs());
	if (!kwe || !strcmp(kwe->value(), "unit"))
	  ioc[UNIT_IO] = e->rhs();
	else return 0;
	return(1);
  }
  if(e->variant() == EXPR_LIST){
	for(el=e; el; el = el->rhs()) {
	  ee = el->lhs();
	  if(ee->variant() != SPEC_PAIR)
		return(0); // IO_control list error
	  kwe = isSgKeywordValExp(ee->lhs());
	  if(!kwe)
		return(0);
	  if     (!strcmp(kwe->value(),"unit"))
		ioc[UNIT_IO] = ee->rhs();
	  else if (!strcmp(kwe->value(),"iostat"))
		ioc[IOSTAT_IO] = ee->rhs();
	  else if (!strcmp(kwe->value(),"iomsg"))
		ioc[IOMSG_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"err"))
		ioc[ERR_IO]  = ee->rhs();
	  else if (!strcmp(kwe->value(),"status"))
		ioc[STATUS_IO]  = ee->rhs();
	  else
		return(0);
	}
	if (!ioc[UNIT_IO]) return(0);
	return(1);
  }
  else
	return(0);
  
}


//enum class ArgType : int { NUMBER = 0, STRING = 1, VAR = 2, STRINGVAR = 3 };
enum { NUMBER_ARG, STRING_ARG, VAR_ARG, STRING_VAR_ARG };
  
int addArgToCall(SgExpression *ioc[], int type, SgCallStmt *call, int arg)
{
  if (!ioc[arg])
	call->addArg(*ConstRef(0));
  else
	switch (type) {
	  case NUMBER_ARG:
		call->addArg(*DvmType_Ref(ioc[arg]));
		break;
	  case STRING_ARG:
		call->addArg(*DvmhString(ioc[arg]));
		break;
	  case VAR_ARG:
		call->addArg(*DvmhVariable(ioc[arg]));
		break;
	  case STRING_VAR_ARG:
		call->addArg(*DvmhStringVariable(ioc[arg]));
		break;
	  default:
		return 1;
	}
  return 0;
}

int addArgToCalls(SgExpression *ioc[], int type, SgCallStmt **calls, int ncalls, int arg) {
	
	if (!ioc[arg])
		for (int i = 0; i < ncalls; ++i)
			calls[i]->addArg(*ConstRef(0));
	else
		switch (type) {
			case NUMBER_ARG:
				for (int i = 0; i < ncalls; ++i)
					calls[i]->addArg(*DvmType_Ref(ioc[arg]));
				break;
			case STRING_ARG:
				for (int i = 0; i < ncalls; ++i)
					calls[i]->addArg(*DvmhString(ioc[arg]));
				break;
			case VAR_ARG:
				for (int i = 0; i < ncalls; ++i)
					calls[i]->addArg(*DvmhVariable(ioc[arg]));
				break;
			case STRING_VAR_ARG:
				for (int i = 0; i < ncalls; ++i)
					calls[i]->addArg(*DvmhStringVariable(ioc[arg]));
				break;
			default:
			  return 1;
		}
	return 0;
	
}
  
/* for inserting assignment dvm000(index) = 0 after cur_st. insertation is made only if cond = true */
void OccupyDvm000Elem(SgExpression *cond, int index) {
  
  if (cond) {
	SgValueExp *zero = new SgValueExp(0);
	SgStatement *ass = new SgAssignStmt (*DVM000(index), *zero);
	
	cur_st->lastNodeOfStmt()->insertStmtAfter(*ass, *cur_st->controlParent());
	cur_st = ass;
  }
}
  
/* for inserting if statement : if (dvm000(index) .ne. 0  goto  ... */
void InsertGotoStmt(SgExpression *err, int index) {
  
  if (err) {
	SgValueExp *zero = new SgValueExp(0);
	SgGotoStmt *gotostmt = new SgGotoStmt(*isSgLabelRefExp(err)->label());
	SgIfStmt *ifst = new SgIfStmt(SgNeqOp(*DVM000(index), *zero), *gotostmt);
	
	cur_st->lastNodeOfStmt()->insertStmtAfter(*ifst, *cur_st->controlParent());
	cur_st = ifst;
	
  }
}

void addRefArgToCall(SgExpression *ref_arg, SgCallStmt *call) {
  
  if (ref_arg) call->addArg(*DvmhVariable(DVM000(ndvm++)));
  else call->addArg(*ConstRef(0));
  return;
}

void addRefArgToCalls(SgExpression *err, SgCallStmt **calls, int ncalls, int *indeces) {
	for (int i = 0; i < ncalls; ++i) {
		indeces[i] = ndvm;
		addRefArgToCall(err, calls[i]);
	}
}


void Dvmh_Close(SgExpression *ioc[]) {
 
  /*
   DVMH_API void dvmh_ftn_close_(
   const DvmType *pUnit,
   const VarRef *pErrFlagRef,
   const VarRef *pIOStatRef,
   const StringVarRef *pIOMsg,
   const StringRef *pStatus);
   */
  SgStatement *continue_st = cur_st; //true body of IF construct
  fmask[FTN_CLOSE] = 2;
  SgCallStmt *close_call = new SgCallStmt(*fdvm[FTN_CLOSE]);
  
  int index_before = ndvm;
  
  addArgToCall(ioc, NUMBER_ARG, close_call, UNIT_IO);
  int index_err = ndvm;
  addRefArgToCall(ioc[ERR_IO], close_call);
  int index_iostat = ndvm;
  addRefArgToCall(ioc[IOSTAT_IO], close_call);
  addArgToCall(ioc, STRING_VAR_ARG, close_call, IOMSG_IO);
  addArgToCall(ioc, STRING_ARG, close_call, STATUS_IO);
  
  OccupyDvm000Elem(ioc[ERR_IO], index_err);
  OccupyDvm000Elem(ioc[IOSTAT_IO], index_iostat);
                 //InsertNewStatementAfter(close_call, cur_st, stmt->controlParent());
  doCallAfter(close_call);
  if (ioc[IOSTAT_IO]) doAssignTo_After(ioc[IOSTAT_IO], DVM000(index_iostat));
  InsertGotoStmt(ioc[ERR_IO], index_err);  
  continue_st->extractStmt();  
  SET_DVM(index_before);
  
  return;
}

void Dvmh_Open(SgExpression *ioc[], const char *io_modes_str)
{
  /*
   DVMH_API void dvmh_ftn_open_(
   const DvmType *pUnit,
   const StringRef *pAccess,
   const StringRef *pAction,
   const StringRef *pAsync,
   const StringRef *pBlank,
   const StringRef *pDecimal,
   const StringRef *pDelim,
   const StringRef *pEncoding,
   const StringRef *pFile,
   const StringRef *pForm,
   const VarRef *pErrFlagRef,
   const VarRef *pIOStatRef,
   const StringVarRef *pIOMsg,
   const VarRef *pNewUnitRef,
   const StringRef *pPad,
   const StringRef *pPosition,
   const DvmType *pRecl,
   const StringRef *pRound,
   const StringRef *pSign,
   const StringRef *pStatus,
   const StringRef *pDvmMode); */
  
  SgStatement *continue_st = cur_st; //true body of IF construct
  if (io_modes_str) ioc[DVM_MODE_IO] = new SgValueExp(io_modes_str);
  
  int index_before = ndvm;
  
  fmask[FTN_OPEN] = 2;
  SgCallStmt *open_call = new SgCallStmt(*fdvm[FTN_OPEN]);
  
  addArgToCall(ioc, NUMBER_ARG, open_call, UNIT_IO);
  addArgToCall(ioc, STRING_ARG, open_call, ACCESS_IO);
  addArgToCall(ioc, STRING_ARG, open_call, ACTION_IO);
  addArgToCall(ioc, STRING_ARG, open_call, ASYNC_IO);
  addArgToCall(ioc, STRING_ARG, open_call, BLANK_IO);
  addArgToCall(ioc, STRING_ARG, open_call, DECIMAL_IO);
  addArgToCall(ioc, STRING_ARG, open_call, DELIM_IO);
  addArgToCall(ioc, STRING_ARG, open_call, ENCODING_IO);
  addArgToCall(ioc, STRING_ARG, open_call, FILE_IO);
  addArgToCall(ioc, STRING_ARG, open_call, FORM_IO);
  
  int index_err = ndvm;
  addRefArgToCall(ioc[ERR_IO], open_call);
  int index_iostat = ndvm;
  addRefArgToCall(ioc[IOSTAT_IO], open_call);
  addArgToCall(ioc, STRING_VAR_ARG, open_call, IOMSG_IO);
  int index_newunit = ndvm;
  addRefArgToCall(ioc[NEWUNIT_IO], open_call);
  
  addArgToCall(ioc, STRING_ARG, open_call, PAD_IO);
  addArgToCall(ioc, STRING_ARG, open_call, POSITION_IO);
  addArgToCall(ioc, NUMBER_ARG, open_call, RECL_IO);
  addArgToCall(ioc, STRING_ARG, open_call, ROUND_IO);
  addArgToCall(ioc, STRING_ARG, open_call, SIGN_IO);
  addArgToCall(ioc, STRING_ARG, open_call, STATUS_IO);
  
  addArgToCall(ioc, STRING_ARG, open_call, DVM_MODE_IO);
  
  OccupyDvm000Elem(ioc[ERR_IO], index_err);
  OccupyDvm000Elem(ioc[IOSTAT_IO], index_iostat);
  OccupyDvm000Elem(ioc[NEWUNIT_IO], index_newunit);
  doCallAfter(open_call);
  if (ioc[IOSTAT_IO]) doAssignTo_After(ioc[IOSTAT_IO], DVM000(index_iostat));
  if (ioc[NEWUNIT_IO]) doAssignTo_After(ioc[NEWUNIT_IO], DVM000(index_newunit));
  InsertGotoStmt(ioc[ERR_IO], index_err);
  
  continue_st->extractStmt();
  
  SET_DVM(index_before);
  
  return;
  
}

void Dvmh_FilePosition(SgExpression *ioc[], int variant) {
  
  /*
   DVMH_API void dvmh_ftn_endfile_(const DvmType *pUnit, const VarRef *pErrFlagRef, const VarRef *pIOStatRef, const StringVarRef *pIOMsg);
   DVMH_API void dvmh_ftn_rewind_(const DvmType *pUnit, const VarRef *pErrFlagRef, const VarRef *pIOStatRef, const StringVarRef *pIOMsg);
   */
  
  SgStatement *continue_st = cur_st; //true body of IF construct
  
  SgCallStmt *call;
  if (variant == ENDFILE_STAT) {
	call = new SgCallStmt(*fdvm[FTN_ENDFILE]);
	fmask[FTN_ENDFILE] = 2;
  }
  else {
	call = new SgCallStmt(*fdvm[FTN_REWIND]);
	fmask[FTN_REWIND] = 2;
  }
  
  int index_before = ndvm;
  
  addArgToCall(ioc, NUMBER_ARG, call, UNIT_);
  int index_iostat = ndvm;
  addRefArgToCall(ioc[IOSTAT_], call);
  int index_err = ndvm;

  addRefArgToCall(ioc[ERR_], call);
  addArgToCall(ioc, STRING_VAR_ARG, call, IOMSG_);
	
  OccupyDvm000Elem(ioc[ERR_], index_err);
  OccupyDvm000Elem(ioc[IOSTAT_], index_iostat);
  doCallAfter(call);
  if (ioc[IOSTAT_]) doAssignTo_After(ioc[IOSTAT_], DVM000(index_iostat));
  InsertGotoStmt(ioc[ERR_], index_err);
  
  continue_st->extractStmt();
  
  SET_DVM(index_before);
  
  return;
  
}

SgExpression *ArrNoSubs(SgExpression *expr) {
	SgArrayRefExp *arr = isSgArrayRefExp(expr);
  // second part of conjunction is for excluding characters, that also are ArrayRefExp
	if (arr && isSgArrayType(expr->symbol()->type()))
    return new SgArrayRefExp(*arr->symbol());
	return expr;
}

void Dvmh_ReadWrite(SgExpression **ioc, SgStatement *stmt) {
  
  /*
   DVMH_API void dvmh_ftn_read_unf_(
   const DvmType *pUnit,
   const VarRef *pEndFlagRef,
   const VarRef *pErrFlagRef,
   const VarRef *pIOStatRef,
   const StringVarRef *pIOMsg,
   const DvmType *pPos,
   const DvmType dvmDesc[],
   const DvmType *pSpecifiedFlag,
   ...);
   */
  
  /* dvmh_ftn_write_unf() different from read by the absence of the flag pEnd.
   DVMH_API void dvmh_ftn_write_unf_(
   const DvmType *pUnit,
   const VarRef *pErrFlagRef,
   const VarRef *pIOStatRef,
   const StringVarRef *pIOMsg,
   const DvmType *pPos,
   const DvmType dvmDesc[],
   const DvmType *pSpecifiedRank, ...);
   */
  SgStatement *continue_st = cur_st; //true body of IF construct
  
  SgInputOutputStmt *io_stmt = isSgInputOutputStmt(stmt);
  SgExprListExp *items = isSgExprListExp(io_stmt->itemList());
  
  if (!items) return; // empty items case. for example, when namelist is used
  int ncalls = items->length();
  SgCallStmt *calls[1000]; //ncalls
  
  if (stmt->variant() == READ_STAT) {
    for (int i = 0; i < ncalls; ++i)
      calls[i] = new SgCallStmt(*fdvm[FTN_READ]);
    fmask[FTN_READ] = 2;
  }
  else {
    for (int i = 0; i < ncalls; ++i)
      calls[i] = new SgCallStmt(*fdvm[FTN_WRITE]);
    fmask[FTN_WRITE] = 2;
  }
  
  int index_before = ndvm;
  
  addArgToCalls(ioc, NUMBER_ARG, calls, ncalls, UNIT_RW);

  int *i_endf = new int[ncalls]; 
  int *i_errf = new int[ncalls]; 
  
  if (stmt->variant() == READ_STAT)
    addRefArgToCalls(ioc[END_RW], calls, ncalls, i_endf);
  addRefArgToCalls(ioc[ERR_RW], calls, ncalls, i_errf);
  
  int *i_iostat = new int[ncalls]; 
  addRefArgToCalls(ioc[IOSTAT_RW], calls, ncalls, i_iostat);
  
  addArgToCalls(ioc, STRING_VAR_ARG, calls, ncalls, IOMSG_RW);
  addArgToCalls(ioc, NUMBER_ARG, calls, ncalls, POS_RW);
  
  /*
   inserting arguments, describing variables and array
   for each arument:
   1) if it is dvm-array, adding sections
   2) if it is not-dvm array, insert data_enter before and data_exit after and adding sections
   3) if it is scalar expression, insert only data_enter and data_exit
   */
  
  for (int i_call = 0; i_call < ncalls; ++i_call) {
    SgExpression *item = items->elem(i_call);
    
    // Data_enter inserting and adding VarGenHeader argument for everything, that is not a dvm-array
    if (!(isSgArrayRefExp(item) && HEADER(item->symbol()))) {
      doCallAfter(DataEnter(ArrNoSubs(item), ConstRef_F95(0)));
      calls[i_call]->addArg(*VarGenHeader(ArrNoSubs(item)));
    }
    
    // array reference
    SgArrayRefExp *arr = isSgArrayRefExp(item);
    if (arr) {
      if (arr && HEADER(arr->symbol())) {
        //	it should be register_array(arr(1)), not register_array(arr)
        SgExprListExp *new_subs = new SgExprListExp(*new SgValueExp(1));
        SgArrayRefExp *new_array_ref = new SgArrayRefExp(*arr->symbol(), *new_subs);
        calls[i_call]->addArg(*Register_Array_H2(new_array_ref));
      }
      
      if (arr->numberOfSubscripts()) {
        int nsubs = arr->numberOfSubscripts();
        calls[i_call]->addArg(*ConstRef(nsubs));
        for (int i = nsubs-1; i >= 0; --i) {
          SgExpression *lbound;
          SgExpression *ubound;
          SgSubscriptExp *sub;
          // both bounds specified
          if ((sub = isSgSubscriptExp(arr->subscript(i)))) {
            lbound = sub->lbound();
            ubound = sub->ubound();
            lbound = (lbound? DvmType_Ref(lbound):  ConstRef_F95(-2147483648));
            ubound = (ubound? DvmType_Ref(ubound):  ConstRef_F95(-2147483648));
          }
          // only upper bound specified
          else {
            lbound = ubound = DvmType_Ref(arr->subscript(i));
          }
          calls[i_call]->addArg(*lbound);
          calls[i_call]->addArg(*ubound);
        }
      }
      else // array doesn't have subscript or it is an array expression
        calls[i_call]->addArg(*ConstRef(0));
    }
    else // it isn't array, anyhow it should be specified that there's no sections
      calls[i_call]->addArg(*ConstRef(0));
  }
  
	/* inserting function calling and goto statements in case of error occurring  */
	for (int i_call = 0; i_call < ncalls; ++i_call) {
		OccupyDvm000Elem(ioc[END_RW], i_endf[i_call]);
		OccupyDvm000Elem(ioc[ERR_RW], i_errf[i_call]);
		OccupyDvm000Elem(ioc[IOSTAT_RW], i_iostat[i_call]);
		doCallAfter(calls[i_call]);
		if (ioc[IOSTAT_RW]) doAssignTo_After(ioc[IOSTAT_RW], DVM000(i_iostat[i_call]));
		InsertGotoStmt(ioc[END_RW], i_endf[i_call]);
		InsertGotoStmt(ioc[ERR_RW], i_errf[i_call]);
	}
	
	/* for every not-dvm-array item, data_exit should be inserted */
  SgExpression *item;
	for (int i_call = 0; i_call < ncalls; ++i_call) {
		if (items) item = items->elem(i_call);
		else item = ConstRef(0);
    if (!(isSgArrayRefExp(item) && HEADER(item->symbol()))) {
      SgStatement *data_exit = DataExit(ArrNoSubs(ArrNoSubs(item)), 1);
      cur_st->lastNodeOfStmt()->insertStmtAfter(*data_exit, *cur_st->controlParent());
      cur_st = data_exit;
		}
	}
	
	continue_st->extractStmt();
	
	SET_DVM(index_before);
	
	return;
}



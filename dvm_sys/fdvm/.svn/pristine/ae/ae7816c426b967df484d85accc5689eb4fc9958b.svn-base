#include "dvm.h"
#include <stdexcept>

class Checkpoint {
  char *cpName; // checkpoint name
  char *serviceFilename; // service file name
  std::vector<SgExpression *> filenames; // filenames used for checkpointing
  SgExprListExp *variables; // variables list
  char defaultIOMode[5];
  
  static const char SERVICE_FILE_SUFFIX[10];
  
  SgSymbol *serviceUnitSymbol;
  SgSymbol *writeUnitSymbol;
  SgSymbol *currentFileSymbol;
  SgSymbol *lastFileSymbol;
  
  SgLabel *emptyServiceFileLabel;
  SgLabel *notExistingServiceFileLabel;
  
public:
  Checkpoint(char *cpName, std::vector<SgExpression *> filenames, SgExprListExp *variables, SgExpression *cpMode) {
    defaultIOMode[0] = 0;
    this->cpName = new char[strlen(cpName) + 1];
    strcpy(this->cpName, cpName);
    this->serviceFilename = new char[strlen(cpName) + strlen(SERVICE_FILE_SUFFIX) + 1];
    strcpy(this->serviceFilename, cpName);
    strcat(this->serviceFilename, SERVICE_FILE_SUFFIX);
    this->filenames = filenames;
    this->variables = variables;
    
    if (cpMode) {
      if (cpMode->variant() == ACC_LOCAL_OP) strcpy(defaultIOMode, "l");
      else if (cpMode->variant() == PARALLEL_OP) strcpy(defaultIOMode, "p");
      else throw new std::runtime_error("Unknown type of checkpoint mode");
    }
    else strcpy(defaultIOMode, "p");
  }
  
  void getNewLabels(int variant) {
    this->emptyServiceFileLabel = GetLabel();
    if (variant == WRITE_STAT) this->notExistingServiceFileLabel = GetLabel();
  }
  
  SgSymbol *getServiceUnitSymbol() {
    return this->serviceUnitSymbol;
  }
  
  SgSymbol *getWriteUnitSymbol() {
    return this->writeUnitSymbol;
  }
  
  SgSymbol *getCurrentFileSymbol() {
    return this->currentFileSymbol;
  }
  
  SgSymbol *getLastFileSymbol() {
    return this->lastFileSymbol;
  }
  
  void defineVariables();
  void createEmptyLastFilenameAssign();
  void createSaveFilenamesStatement();
  void createOpenServiceFileBeforeCp(int variant);
  void createReadServiceFileStatement(int variant);
  void createCloseServiceFileStatement(bool useLabel);
  void createCloseWriteFileStatement();
  void createOpenWriteFileStatement(bool isAsync);
  void createWriteOrReadStatement(int variant);
  void createWriteServiceFileStatement();
  void createOpenReadFileStatement();
  void createCheckFilenameStatement();
  void createOpenServiceFileAfterCp();
  void getNextFileStmt();
  void createSaveAsyncUnitStatement();
  void createCpWaitStatement(SgVarRefExp *statusVarRef);
  
};

const char Checkpoint::SERVICE_FILE_SUFFIX[10] = ".info.dat";

struct stringLessComparator {
  bool operator()(const char *a, const char *b) const {
    return strcmp(a, b) < 0;
  }
};

std::map<char *, Checkpoint *, stringLessComparator> checkpointMap;

void insertContinueStatement() {
  SgContinueStmt &continueStatement = *new SgContinueStmt();
  cur_st->lastNodeOfStmt()->insertStmtAfter(continueStatement, *cur_st->controlParent());
  cur_st = &continueStatement;
}

/* adds new checkpoint to checkpointMap
 example: !DVM$ CP_CREATE CP1, VARLIST(IT, B), FILES('jac_%02d.cp0','jac_%02d.cp1') [PARALLEL | LOCAL]
 */
void CP_Create_Statement(SgStatement *stmt, int error_msg)
{
  if (!options.isOn(IO_RTS)) {
    if (error_msg) warn("Checkpoints aren't supported without iO_RTS option", 462, stmt);
  }
  SgVarRefExp *cpNameExpr = isSgVarRefExp(stmt->expr(0));
  if (!cpNameExpr) return;
  char *cpName = cpNameExpr->symbol()->identifier();
  
  SgExprListExp *variablesExpr = isSgExprListExp(stmt->expr(1));
  
  SgExpression *filenamesAndCpModeExpr = stmt->expr(2);
  SgExprListExp *filenamesExpr = NULL;
  SgExpression *cpMode = NULL;
  std::vector<SgExpression *> filenames;
  if (isSgExprListExp(filenamesAndCpModeExpr)) {
    filenamesExpr = isSgExprListExp(filenamesAndCpModeExpr);
  }
  else if (filenamesAndCpModeExpr->variant() == ARRAY_OP) {
    filenamesExpr = isSgExprListExp(filenamesAndCpModeExpr->lhs());
    cpMode = filenamesAndCpModeExpr->rhs();
  }
  // else syntax error, no need to check
  
  for (int i = 0; i < filenamesExpr->length(); ++i) {
    SgValueExp *filename = isSgValueExp(filenamesExpr->elem(i));
    if (!filename) {
      if (error_msg) {
        err("Every filename in CP_CREATE statement should be character constant value", 463, stmt);
      }
      return;
    }
    size_t currentFilenameLength = strlen(filename->stringValue());
    if (currentFilenameLength >= 99) {
      if (error_msg) {
        err("Filename in CP_CREATE cannot be longer than 100 characters", 464, stmt);
      }
      return;
    }
    filenames.push_back(filenamesExpr->elem(i));
  }
  try {
    Checkpoint *checkpoint = new Checkpoint(cpName, filenames, variablesExpr, cpMode);
    checkpoint->defineVariables();
    if (checkpointMap.find(cpName) != checkpointMap.end()) {
      if (error_msg) {
        Error("Checkpoint with name %s already exists", cpName, 465, stmt);
      }
      return;
    }
    checkpointMap[cpName] = checkpoint;
    checkpoint->createSaveFilenamesStatement();
    checkpoint->createEmptyLastFilenameAssign();
  }
  catch(std::runtime_error error) {
    if (error_msg) {
      err(error.what(), 0, stmt);
    }
    return;
  }
  
}

/* fixme: delete from here! use the only enum for io.cpp and checkpoint.cpp */
enum {UNIT_IO, ACCESS_IO, ACTION_IO, ASYNC_IO, BLANK_IO, DECIMAL_IO, DELIM_IO, ENCODING_IO, ERR_IO, FILE_IO,
  FORM_IO, IOSTAT_IO, IOMSG_IO, NEWUNIT_IO, PAD_IO, POSITION_IO, RECL_IO, ROUND_IO, SIGN_IO, STATUS_IO, DVM_MODE_IO, NUMB__CL };
enum { UNIT_RW, FMT_RW, NML_RW, ADVANCE_RW, ASYNC_RW, BLANK_RW, DECIMAL_RW, DELIM_RW, END_RW, EOR_RW, ERR_RW, ID_RW,
  IOMSG_RW, IOSTAT_RW, PAD_RW, POS_RW, REC_RW, ROUND_RW, SIGN_RW, SIZE_RW, NUMB__RW };

void Checkpoint::defineVariables() {
  
  const int varLength =  300; //(int) (20 + strlen(this->cpName));
  char serviceUnitVarName[varLength];
  strcpy(serviceUnitVarName, "dvmh_service_unit_");
  strcat(serviceUnitVarName, this->cpName);
  
  char writeUnitVarName[varLength];
  strcpy(writeUnitVarName, "dvmh_write_unit_");
  strcat(writeUnitVarName, this->cpName);
  
  char currentFileVarName[varLength];
  strcpy(currentFileVarName, "dvmh_current_file_");
  strcat(currentFileVarName, this->cpName);
  
  char lastFileVarName[varLength];
  strcpy(lastFileVarName, "dvmh_last_file_");
  strcat(lastFileVarName, this->cpName);
  
  this->serviceUnitSymbol = new SgSymbol(VARIABLE_NAME, serviceUnitVarName);
  this->serviceUnitSymbol->setType(SgTypeInt());
  this->writeUnitSymbol = new SgSymbol(VARIABLE_NAME, writeUnitVarName);
  this->writeUnitSymbol->setType(SgTypeInt());
  
  SgStringLengthExp *lengthExpr = new SgStringLengthExp(*new SgValueExp(100));
  SgType *stringType = new SgType(T_STRING, lengthExpr, SgTypeChar());
  
  this->currentFileSymbol = new SgSymbol(VARIABLE_NAME, currentFileVarName);
  this->currentFileSymbol->setType(stringType);
  
  this->lastFileSymbol = new SgSymbol(VARIABLE_NAME, lastFileVarName);
  this->lastFileSymbol->setType(stringType);
  
  /* declare these variables for testing */
  cur_func->insertStmtAfter(*serviceUnitSymbol->makeVarDeclStmt());
  cur_func->insertStmtAfter(*writeUnitSymbol->makeVarDeclStmt());
  cur_func->insertStmtAfter(*currentFileSymbol->makeVarDeclStmt());
  cur_func->insertStmtAfter(*lastFileSymbol->makeVarDeclStmt());
  
}

void Checkpoint::createSaveFilenamesStatement() {
  
  /* generates dvmh_cp_save_filenames call:
   dvmh_cp_save_filenames(checkpoint_name, files_count, filename1, filename2, ...)
   */
  
  SgStatement *stmt = SaveCheckpointFilenames(new SgValueExp(this->cpName), this->filenames);
  SgStatement *cpCreateDir = cur_st;
  cur_st->insertStmtAfter(*stmt, *cur_st->controlParent());
  cur_st = stmt;
  cpCreateDir->extractStmt();
}

void Checkpoint::createEmptyLastFilenameAssign() {
  /*
   initialization dvmh_last_file variable. generating dvmh_last_file = ''&
   */
  SgVarRefExp *lastFilename = new SgVarRefExp(this->lastFileSymbol);
  SgValueExp *emptyString = new SgValueExp("");
  doAssignTo_After(lastFilename, emptyString);
}

void Checkpoint::createOpenServiceFileBeforeCp(int variant) {
  /* statement to be generated:
   open(newunit=service_unt, file=service_filename,
   access='stream', status='old', err=err_label, position='rewind', action='read')
   */
  
  SgExpression *ioc[NUMB__CL];
  for (int i = 0; i < NUMB__CL; ++i) {
    ioc[i] = NULL;
  }

  ioc[NEWUNIT_IO] = new SgVarRefExp(this->serviceUnitSymbol);
  ioc[ACCESS_IO] = new SgValueExp("STREAM");
  ioc[ACTION_IO] = new SgValueExp("READ");
  ioc[FILE_IO] = new SgValueExp(serviceFilename);
  ioc[POSITION_IO] = new SgValueExp("REWIND"); // for reading file
  ioc[STATUS_IO] = new SgValueExp("OLD");
  
  // if service file is opened for reading, error should occur.
  // if it is opened for saving checkpoint, not existing file is normal
  if (variant == WRITE_STAT) ioc[ERR_IO] = new SgLabelRefExp(*this->notExistingServiceFileLabel);
  
  insertContinueStatement();
  Dvmh_Open(ioc, defaultIOMode);
}

void Checkpoint::createOpenServiceFileAfterCp() {
  /* statement to be generated:
   open(newunit=service_unt, file=serviceFileName, access='stream', position='rewind', action='write')
   */
  
  SgExpression *ioc[NUMB__CL];
  for (int i = 0; i < NUMB__CL; ++i) {
    ioc[i] = NULL;
  }
  
  ioc[NEWUNIT_IO] = new SgVarRefExp(this->serviceUnitSymbol);
  ioc[ACCESS_IO] = new SgValueExp("STREAM");
  ioc[ACTION_IO] = new SgValueExp("WRITE");
  ioc[FILE_IO] = new SgValueExp(this->serviceFilename);
  ioc[POSITION_IO] = new SgValueExp("REWIND");
  ioc[STATUS_IO] = new SgValueExp("OLD");
  
  insertContinueStatement();
  Dvmh_Open(ioc, defaultIOMode);
}

void Checkpoint::createReadServiceFileStatement(int variant) {
  /* statement to be generated:
   read(unit = service_unt, end=200) last_filename
   end argument is used only for writing checkpoint.
   */
  
  SgExpression *ioc[NUMB__RW];
  for (int i = 0; i < NUMB__RW; ++i) {
    ioc[i] = NULL;
  }
  
  ioc[UNIT_RW] = new SgVarRefExp(this->serviceUnitSymbol);
  SgLabelRefExp *endLabelRef = new SgLabelRefExp(*this->emptyServiceFileLabel);
  ioc[END_RW] = endLabelRef;
  
  SgVarRefExp &lastFilenameExpr = *new SgVarRefExp(this->lastFileSymbol);
  SgExprListExp &itemsToRead = *new SgExprListExp(lastFilenameExpr);
  
  SgExprListExp &specList = *new SgExprListExp();
  
  SgSpecPairExp &specPairUnit = *new SgSpecPairExp(*new SgValueExp("unit"), *new SgVarRefExp(this->serviceUnitSymbol));
  specList.append(specPairUnit);
  if (variant == WRITE_STAT) {
    SgSpecPairExp &specPairEnd = *new SgSpecPairExp(*new SgValueExp("end"), *endLabelRef);
    specList.append(specPairEnd);
  }
  
  SgInputOutputStmt *ioStatement = new SgInputOutputStmt(READ_STAT, specList, itemsToRead);
  
  insertContinueStatement();
  Dvmh_ReadWrite(ioc, ioStatement);
  
}

void Checkpoint::createWriteServiceFileStatement() {
  /* statement to be generated:
   write(unit = service_unt) current_filename
   */
  SgExpression *ioc[NUMB__RW];
  for (int i = 0; i < NUMB__RW; ++i) {
    ioc[i] = NULL;
  }
  
  ioc[UNIT_RW] = new SgVarRefExp(this->serviceUnitSymbol);
  
  SgVarRefExp &currentFileExpr = *new SgVarRefExp(this->currentFileSymbol);
  SgExprListExp &itemsToWrite = *new SgExprListExp(currentFileExpr);
  
  SgSpecPairExp &specPairUnit = *new SgSpecPairExp(*new SgValueExp("unit"), *new SgVarRefExp(this->serviceUnitSymbol));
  SgExprListExp &specList = *new SgExprListExp();
  specList.append(specPairUnit);
  SgInputOutputStmt *ioStatement = new SgInputOutputStmt(WRITE_STAT, specList, itemsToWrite);
  
  insertContinueStatement();
  Dvmh_ReadWrite(ioc, ioStatement);
  
}

void Checkpoint::createCloseServiceFileStatement(bool useLabel) {
  /* statement to generate:
   [label]  close(unit = service_unit)
   */
  
  SgExpression *ioc[NUMB__CL];
  for (int i = 0; i < NUMB__CL; ++i)
    ioc[i] = NULL;
  ioc[UNIT_IO] = new SgVarRefExp(this->serviceUnitSymbol);
  
  insertContinueStatement();
  Dvmh_Close(ioc);
  
  if (useLabel) cur_st->setLabel(*this->emptyServiceFileLabel);
  
}

void Checkpoint::getNextFileStmt() {
  
  SgStatement *getNextFilenameStmt =
  GetNextFilename(new SgValueExp(this->cpName),
                  new SgVarRefExp(this->lastFileSymbol),
                  new SgVarRefExp(this->currentFileSymbol));
  doCallAfter(getNextFilenameStmt);
  cur_st->setLabel(*this->notExistingServiceFileLabel);
}

void Checkpoint::createCloseWriteFileStatement() {
  /* statement to generate:
    close(unit = write_unit)
   */
  
  SgExpression *ioc[NUMB__CL];
  for (int i = 0; i < NUMB__CL; ++i)
    ioc[i] = NULL;
  ioc[UNIT_IO] = new SgVarRefExp(this->writeUnitSymbol);
  
  insertContinueStatement();
  Dvmh_Close(ioc);
  
}

void Checkpoint::createOpenReadFileStatement() {
  /* statement to be generated:
   open(newunit = write_unt, file=last_filename, access='stream', status='old')
   */
  SgExpression *ioc[NUMB__CL];
  for (int i = 0; i < NUMB__CL; ++i) {
    ioc[i] = NULL;
  }
  
  ioc[NEWUNIT_IO] = new SgVarRefExp(this->writeUnitSymbol);
  ioc[FILE_IO] = new SgVarRefExp(this->lastFileSymbol);
  ioc[ACCESS_IO] = new SgValueExp("STREAM");
  ioc[STATUS_IO] = new SgValueExp("OLD");
  
  insertContinueStatement();
  Dvmh_Open(ioc, defaultIOMode);
  
}

void Checkpoint::createOpenWriteFileStatement(bool isAsync) {
  /* statement to be generated:
   open(newunit = write_unt, file=current_filename, access='stream', status='replace', dvmIoMode = defaultIOMode[+s])
   */
  SgExpression *ioc[NUMB__CL];
  for (int i = 0; i < NUMB__CL; ++i) {
    ioc[i] = NULL;
  }
  
  ioc[NEWUNIT_IO] = new SgVarRefExp(this->writeUnitSymbol);
  ioc[FILE_IO] = new SgVarRefExp(this->currentFileSymbol);
  ioc[ACCESS_IO] = new SgValueExp("STREAM");
  ioc[STATUS_IO] = new SgValueExp("REPLACE");
  ioc[ACTION_IO] = new SgValueExp("WRITE");
  
  insertContinueStatement();
  char *ioMode = new char[5];
  strcpy(ioMode, defaultIOMode);
  if (isAsync) strcat(ioMode, "s");
  Dvmh_Open(ioc, ioMode);
  
}

void Checkpoint::createWriteOrReadStatement(int variant) {
  SgExpression *ioc[NUMB__RW];
  for (int i = 0; i < NUMB__RW; ++i) {
    ioc[i] = NULL;
  }
  
  ioc[UNIT_RW] = new SgVarRefExp(this->writeUnitSymbol);
  
  SgSpecPairExp &specPairUnit = *new SgSpecPairExp(*new SgValueExp("unit"), *new SgVarRefExp(this->writeUnitSymbol));
  SgExprListExp &specList = *new SgExprListExp();
  specList.append(specPairUnit);
  SgInputOutputStmt *ioStatement = new SgInputOutputStmt(variant, specList, *this->variables);
  
  insertContinueStatement();
  Dvmh_ReadWrite(ioc, ioStatement);
  
}

void Checkpoint::createCheckFilenameStatement() {
  /* checks that filename was in current checkpoint declaration.
   generates dvmh_cp_check_filename(checkpoint_name, filename)
   */
  SgValueExp *cpNameExpr = new SgValueExp(this->cpName);
  SgVarRefExp *lastFileExpr = new SgVarRefExp(this->lastFileSymbol);
  SgStatement *checkFileStatement = CheckFilename(cpNameExpr, lastFileExpr);
  cur_st->insertStmtAfter(*checkFileStatement, *cur_st->controlParent());
  cur_st = checkFileStatement;
  
}

void Checkpoint::createSaveAsyncUnitStatement() {
  /* saves unit when cp_save is used in async mode
   generates dvmh_cp_save_async_unit(checkpoint_name, filename, unit)
   */
  SgValueExp *cpName = new SgValueExp(this->cpName);
  SgVarRefExp *currentFileExpr = new SgVarRefExp(this->currentFileSymbol);
  SgVarRefExp *writeUnitRef = new SgVarRefExp(this->writeUnitSymbol);
  
  SgStatement *cpSaveAsyncUnit = CpSaveAsyncUnit(cpName, currentFileExpr, writeUnitRef);
  cur_st->insertStmtAfter(*cpSaveAsyncUnit, *cur_st->controlParent());
  cur_st = cpSaveAsyncUnit;

}

void Checkpoint::createCpWaitStatement(SgVarRefExp *statusVarRef) {
  /* wait for all files to finish async saving and closing them
   generates dvmh_cp_wait(checkpoint_name, status_var)
   */
  SgStatement *initialCpWait = cur_st;
  SgStatement *cpWaitStmt = CpWait(new SgValueExp(this->cpName), statusVarRef);
  cur_st->insertStmtAfter(*cpWaitStmt);
  cur_st = cpWaitStmt;
  initialCpWait->extractStmt();
}

Checkpoint *getCheckpoint(SgStatement *stmt, int error_msg) {
  SgVarRefExp *checkpointVarRef = isSgVarRefExp(stmt->expr(0));
  char *checkpointName = new char[strlen(checkpointVarRef->symbol()->identifier()) + 1];
  strcpy(checkpointName, checkpointVarRef->symbol()->identifier());
  std::map<char *, Checkpoint *>::iterator checkpointIt = checkpointMap.find(checkpointName);
  if (checkpointIt == checkpointMap.end()) {
    if (error_msg) {
      Error("No created checkpoint with name %s found", checkpointName, 466, stmt);
    }
    return NULL;
  }
  return checkpointIt->second;
}

void CP_Save_Statement(SgStatement *stmt, int error_msg) {
  
  /*
   stmt->variant() == DVM_CP_SAVE_DIR
   stmt->expr(0) – имя-контр-точки
   stmt->expr(1) –  NULL или variant == ACC_ASYNC_OP
   */
  Checkpoint *checkpoint = getCheckpoint(stmt, error_msg);
  if (!checkpoint) return;
  
  bool isAsync = (stmt->expr(1) != NULL && stmt->expr(1)->variant() == ACC_ASYNC_OP);
  
  checkpoint->getNewLabels(WRITE_STAT);
  
  checkpoint->createOpenServiceFileBeforeCp(WRITE_STAT);
  checkpoint->createReadServiceFileStatement(WRITE_STAT);
  checkpoint->createCloseServiceFileStatement(true);
  
  checkpoint->getNextFileStmt();
  
  checkpoint->createOpenWriteFileStatement(isAsync);
  if (isAsync) checkpoint->createSaveAsyncUnitStatement();
  checkpoint->createWriteOrReadStatement(WRITE_STAT);
  if (!isAsync) checkpoint->createCloseWriteFileStatement();
  
  checkpoint->createOpenServiceFileAfterCp();
  checkpoint->createWriteServiceFileStatement();
  checkpoint->createCloseServiceFileStatement(false);
  
}

void CP_Load_Statement(SgStatement *stmt, int error_msg) {
  Checkpoint *checkpoint = getCheckpoint(stmt, error_msg);
  if (!checkpoint) return;
  
  checkpoint->getNewLabels(READ_STAT);
  
  checkpoint->createOpenServiceFileBeforeCp(READ_STAT);
  checkpoint->createReadServiceFileStatement(READ_STAT);
  checkpoint->createCloseServiceFileStatement(true);
  
  checkpoint->createCheckFilenameStatement();
  
  checkpoint->createOpenReadFileStatement();
  checkpoint->createWriteOrReadStatement(READ_STAT);
  checkpoint->createCloseWriteFileStatement();
  
}

void CP_Wait(SgStatement *stmt, int error_msg) {
  Checkpoint *checkpoint = getCheckpoint(stmt, error_msg);
  if (!checkpoint) return;
  
  SgVarRefExp *statusVarRef = isSgVarRefExp(stmt->expr(1));
  if (!statusVarRef || !(statusVarRef->symbol()->type()->variant() == T_INT)) {
    if (error_msg)
      err("Wrong type of STATUS argument in CP_WAIT-statement", 467, stmt);
    return;
  }
  
  checkpoint->createCpWaitStatement(statusVarRef);
  
}


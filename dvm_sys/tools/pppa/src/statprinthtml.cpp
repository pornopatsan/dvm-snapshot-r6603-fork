#include "statprinthtml.h"
#include <string.h>
#include <stdarg.h>

CStatPrintHtml::CStatPrintHtml(const char *name){
  table = 0;
  valid = TRUE;
  fs.open(name, std::fstream::out);
  if (!fs.is_open()) {
    valid = FALSE;
    sprintf(texterr, "Can't open file %s\n", name);
    return;
  }
  fs << "<!DOCTYPE html><html><head></head><body>\n";

}

void CStatPrintHtml::TextErr(char *t){
  strcpy(t, texterr);
}

BOOL CStatPrintHtml::Valid(){
  return valid;
}

int CStatPrintHtml::StartTable(){
  ++table;
  fs << "<table>";
  return 0;
}

int CStatPrintHtml::EndTable(){
  if (table > 0){
    --table;
    fs << "</table>";
  } else {
    sprintf(texterr,"EndTable вызван вне таблицы\n");
    return -1;
  }
  return 0;
}

template <typename T>
int CStatPrintHtml::StatPrint(T x,...){
  T t;
  va_list arglist;
  va_start(arglist, x);
  if (table) {
    fs << "  <tr>";
    fs << "    <td>" << x << "</td>\n";
    while (t = va_arg(arglist, T)){
      fs << "    <td>" << t << "</td>\n";
    }
    fs << "  </tr>\n";
  } else {
    fs << "    <td>" << x << "</td>\n";
    while (t = va_arg(arglist, T))
      fs << t;
  }
  va_end(arglist);
  return 0;
}

CStatPrintHtml::~CStatPrintHtml(){
  if (fs.is_open()){
    fs << "</body></html>";
    fs.close();
  }
}

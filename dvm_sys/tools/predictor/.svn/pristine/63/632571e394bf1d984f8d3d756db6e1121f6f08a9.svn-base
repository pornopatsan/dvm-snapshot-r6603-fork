#ifndef _PARSESTRING_H
#define _PARSESTRING_H

#include <string>
#include "Event.h"


//*************** for GNU only ****************//
#ifdef _UNIX_
	char *_itoa(int value, char *str, int radix);
	char *_ltoa(long value, char *str, int radix);
#endif

double atof( const std::string& str );
int atoi( const std::string& str );
long atol( const std::string& str );


//************* INTEGER PARAMETERS *************//

extern int ParamFromString(char* par_name, int& par_val, char* str);
extern int IndexParamFromString(char* par_name, int par_idx, int& par_val, char* str);
extern int SubParamFromString(char* par_name, int par_idx, char* sub_par, int& par_val, 
							  char* str);

extern int ParamFromStrArr(char* par_name, int& par_val, char** str_arr, int str_cnt);
extern int IndexParamFromStrArr(char* par_name, int par_idx, int& par_val, char** str_arr, 
								int str_cnt);
extern int SubParamFromStrArr(char* par_name, int par_idx, char* sub_par, int& par_val, 
							  char** str_arr, int str_cnt);

bool ParamFromString(const std::string& par_name, std::string::size_type& par_val, 
					 const std::string& str);

//************** LONG PARAMETERS *************//

extern int ParamFromString(char* par_name, long& par_val, char* str, int base);
extern int IndexParamFromString(char* par_name, int par_idx, long& par_val, char* str, int base);
extern int SubParamFromString(char* par_name, int par_idx, char* sub_par, long& par_val, char* str, int base);

extern int ParamFromStrArr(char* par_name, long& par_val, char** str_arr, int str_cnt, int base);
extern int IndexParamFromStrArr(char* par_name, int par_idx, long& par_val, char** str_arr, int str_cnt, int base);
extern int SubParamFromStrArr(char* par_name, int par_idx, char* sub_par, long& par_val, char** str_arr, int str_cnt, int base);

//************ FLOAT PARAMETERS *************//

extern int ParamFromString(char* par_name, double& par_val, char* str);
extern int IndexParamFromString(char* par_name, int par_idx, double& par_val, char* str);
extern int SubParamFromString(char* par_name, int par_idx, char* sub_par, double& par_val, char* str);

extern int ParamFromStrArr(char* par_name, double& par_val, char** str_arr, int str_cnt);
extern int IndexParamFromStrArr(char* par_name, int par_idx, double& par_val, char** str_arr, int str_cnt);
extern int SubParamFromStrArr(char* par_name, int par_idx, char* sub_par, double& par_val, char** str_arr, int str_cnt);

//************ STRING PARAMETERS *************//

extern int ParamFromString(char* par_name, char* par_val, char* str);
extern int IndexParamFromString(char* par_name, char* par_idx, char*& par_val, char* str);
extern int SubParamFromString(char* par_name, int par_idx, char* sub_par, char*& par_val, char* str);

extern int ParamFromStrArr(char* par_name, char*& par_val, char** str_arr, int str_cnt);
extern int IndexParamFromStrArr(char* par_name, int par_idx, char*& par_val, char** str_arr, int str_cnt);
extern int SubParamFromStrArr(char* par_name, int par_idx, char* sub_par, char*& par_val, char** str_arr, int str_cnt);

//************ SERVICE ROUTINES *************//

extern int StrToInt(char* str);
extern long StrToLong(char* str);
extern long StrToLong(char* str, int base);
extern double StrToDouble(char* str);

extern void IntToStr(char*& str, int val);
extern void LongToStr(char*& str, long val);
extern void DoubleToStr(char*& str, double val);

//-----------------------------------------------------------------------------------

bool ParamFromString(const std::string& par_name, std::string& par_val, const std::string& str);
bool ParamFromString(const std::string& par_name, int& par_val, const std::string& str);
bool ParamFromString(const std::string& par_name, long& par_val, const std::string& str);
bool ParamFromString(const std::string& par_name, double& par_val, const std::string& str);

int ModifierFromStrArr(char** str_arr, int str_cnt);

#endif

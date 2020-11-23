#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <fstream>

#include "ParseString.h"

using namespace std;

extern ofstream prot;

#ifdef _UNIX_

//char *_fcvt( double value, int count, int *dec, int *sign );

char *_itoa( int value, char *str, int radix )
{
	if (radix != 10) {
		prot << "Radix in the function '_itoa' no equal '10'" << endl;
		abort();
	}
	sprintf(str, "%d", value);
	return str;
}

char *_ltoa( long value, char *str, int radix )
{
	if (radix != 10) {
		prot << "Radix in the function '_ltoa' no equal '10'" << endl;
		abort();
	}
	sprintf(str, "%ld", value);
	return str;
}

#endif

//******* SERVICE ROUTINES

int StrToInt(char* str)
{ 
	return atoi(str);
}

long StrToLong(char* str, int base)
{ 
//====
	// единственный минус этой функции в том что возможно совпадения значений без перемешивания байтов и с ним 
	// так что в будущем!!!!! исправить это введением нового типа = hi + lo
	// и при условии что hi != 0 использовать его.
	long hi,lo;
	char tmp_hi[2*sizeof(long)+1];
  char tmp_lo[2*sizeof(long)+1];
	char *tmp;

	int i, sz=2*sizeof(long);

	i=strlen(str);
	if(str[strlen(str)-1]==';') i--;

	//разбивка строки str на две подстроки с удалением ';' и с добавлением маркера конца строки
	if(i>sz) { strncpy(tmp_hi,str,i-sz); tmp_hi[i-sz]=0;}
	else strcpy(tmp_hi,"");
	if(i>sz) { strncpy(tmp_lo,str+i-sz,sz); tmp_lo[sz]=0;}
  else { strncpy(tmp_lo,str,i); tmp_lo[i]=0;}
	
	hi=strtoul(tmp_hi,&tmp,base);
	lo=strtoul(tmp_lo,&tmp,base);

//	printf("STR->'%s'-> RES::%lx+%lx ",str,hi,lo);

	//смешиваем hi и lo по методу Димы Попова :)
	hi = (hi>>16) | (hi<<16);
	hi = ((hi<<8) & 0xff00ff00)|((hi>>8) & 0x00ff00ff);
	lo += hi;

//	printf("-> %lx\n",lo);
	return lo;

//was    return strtoul(str,&tmp,base);
//=***
};

double StrToDouble(char* str)
{ char* tmp;    
	return strtod(str,&tmp);
};


void IntToStr(char*& str, int val)
{ 
	_itoa(val,str,10);  
}

void LongToStr(char*& str, long val, int base)
{
	_ltoa(val,str,base);  
}

void DoubleToStr(char*& str, double val)
  {
#ifdef _UNIX_
	char st[256];
	sprintf(st, "%+f", val);
	int i = 0;
	int j = 0;
	while (st[++i] != 0) {
		if (st[i] == '.') {
			j = 1;
			continue;
		}
		st[i - j] = st[i];
	}
	st[i - j] = 0;

#else
	int		dec,
			sign;
    strcpy(str,_fcvt(val,6,&dec,&sign));
#endif
	prot << "DoubleToStr=(" << val << ')' << str << endl;
  };

//******* STRING PARAMETERS

int ParamFromString(char * par_name, char* par_val, char* str)
{
	char* pos;
    int len;
//   cout << "par_name=" << par_name << " str=" << str << endl;
    if (str==NULL) return 0;
    pos = strstr(str,par_name);
    if(pos==NULL) return 0;
    pos += strlen(par_name);
		while (pos[0]==' ') pos++; //====//
		if ((pos[0] != '=') && (pos[0] != '['))
			return ParamFromString(par_name, par_val, pos);
    len=strcspn(++pos," ");
    strncpy(par_val,pos,len);
    par_val[len]='\0';
    return 1;
}

int IndexParamFromString(char * par_name, int par_idx, char*& par_val, char* str)
  { char par_idx_name[64];
    char* idx_str;
    if(str==NULL) return 0;
    idx_str=(char*)malloc(sizeof(char)*8);
	assert(idx_str != NULL);
    IntToStr(idx_str,par_idx);
    strcpy(par_idx_name,par_name);
    strcat(par_idx_name,"[");
    strcat(par_idx_name,idx_str);
    strcat(par_idx_name,"]");
    free(idx_str);
    return ParamFromString(par_idx_name, par_val, str);
  };

int SubParamFromString(char * par_name, int par_idx, char* sub_par, char*& par_val, char* str)
  { char par_idx_name[64];
    char* idx_str;
    char* subpar_str;
    if(str==NULL) return 0;
    idx_str=(char*)malloc(sizeof(char)*8);
	assert(idx_str != NULL);
    IntToStr(idx_str,par_idx);
    strcpy(par_idx_name,par_name);
    strcat(par_idx_name,"[");
    strcat(par_idx_name,idx_str);
    strcat(par_idx_name,"]");
    free(idx_str);
    if((subpar_str=strstr(str,par_idx_name))==NULL) 
      return 0;
    else
      return ParamFromString(sub_par, par_val, subpar_str);
  };

int ParamFromStrArr(char * par_name, char*& par_val, char** str_arr, int str_cnt)
  { int i;
    int res = 0;
    if(str_cnt<=0) return 0;
    for(i=0; i<str_cnt; i++)
      if ((res=ParamFromString(par_name, par_val, str_arr[i])) != 0) break;
    return res;
  };

int IndexParamFromStrArr(char * par_name, int par_idx, char*& par_val, char** str_arr, int str_cnt)
  { int i;
    int res = 0;
    if(str_cnt<=0) return 0;
    for(i=0; i<str_cnt; i++) {
      if((res=IndexParamFromString(par_name, par_idx, par_val, str_arr[i])) != 0) break;
	}
    return res;
  };

int SubParamFromStrArr(char * par_name, int par_idx, char* sub_par, char*& par_val, char** str_arr, int str_cnt)
  { int i;
    int res = 0;
    if(str_cnt<=0) return 0;
    for(i=0; i<str_cnt; i++)
      if((res=SubParamFromString(par_name, par_idx, sub_par, par_val, str_arr[i])) != 0) break;
    return res;
  };

//******* INTEGER PARAMETERS

int ParamFromString(char * par_name, int& par_val, char* str)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=ParamFromString(par_name,buffer,str)) != 0)
      par_val=StrToInt(buffer);
    free(buffer);
    return res;
  };

int IndexParamFromString(char * par_name, int par_idx, int& par_val, char* str)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=IndexParamFromString(par_name,par_idx,buffer,str)) != 0)
      par_val=StrToInt(buffer);
    free(buffer);
    return res;
  };

int SubParamFromString(char * par_name, int par_idx, char* sub_par, int& par_val, char* str)
  { int res;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=SubParamFromString(par_name,par_idx,sub_par,buffer,str)) != 0)
      par_val=StrToInt(buffer);
    free(buffer);
    return res;
  };


int ParamFromStrArr(char * par_name, int& par_val, char** str_arr, int str_cnt)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=ParamFromStrArr(par_name,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToInt(buffer);
    free(buffer);
    return res;
  };

int IndexParamFromStrArr(char * par_name, int par_idx, int& par_val, char** str_arr, int str_cnt)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=IndexParamFromStrArr(par_name,par_idx,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToInt(buffer);
    free(buffer);
    return res;
  };

int SubParamFromStrArr(char * par_name, int par_idx, char* sub_par, int& par_val, char** str_arr, int str_cnt)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=SubParamFromStrArr(par_name,par_idx,sub_par,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToInt(buffer);
    free(buffer);
    return res;
  };

//******* LONG PARAMETERS

int ParamFromString(char * par_name, long& par_val, char* str, int base)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=ParamFromString(par_name,buffer,str)) != 0)
      par_val=StrToLong(buffer, base);
    free(buffer);
    return res;
  };

int IndexParamFromString(char * par_name, int par_idx, long& par_val, char* str, int base)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=IndexParamFromString(par_name,par_idx,buffer,str)) != 0)
      par_val=StrToLong(buffer, base);
    free(buffer);
    return res;
  };

int SubParamFromString(char * par_name, int par_idx, char* sub_par, long& par_val, char* str, int base)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=SubParamFromString(par_name,par_idx,sub_par,buffer,str)) != 0)
      par_val=StrToLong(buffer, base);
    free(buffer);
    return res;
  };


int ParamFromStrArr(char * par_name, long& par_val, char** str_arr, int str_cnt, int base)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=ParamFromStrArr(par_name,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToLong(buffer, base);
    free(buffer);
    return res;
  };

int IndexParamFromStrArr(char * par_name, int par_idx, long& par_val, char** str_arr, int str_cnt, int base)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);

    if((res=IndexParamFromStrArr(par_name,par_idx,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToLong(buffer, base);
    free(buffer);
    return res;
  };

int SubParamFromStrArr(char * par_name, int par_idx, char* sub_par, long& par_val, char** str_arr, int str_cnt, int base)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*16);
	assert(buffer != NULL);
    if((res=SubParamFromStrArr(par_name,par_idx,sub_par,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToLong(buffer, base);
    free(buffer);
    return res;
  };

//******* FLOAT PARAMETERS

int ParamFromString(char * par_name, double& par_val, char* str)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*32);
	assert(buffer != NULL);
    if((res=ParamFromString(par_name,buffer,str)) != 0)
      par_val=StrToDouble(buffer);
    free(buffer);
    return res;
  };

int IndexParamFromString(char * par_name, int par_idx, double& par_val, char* str)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*32);
	assert(buffer != NULL);
    if((res=IndexParamFromString(par_name,par_idx,buffer,str)) != 0)
      par_val=StrToDouble(buffer);
    free(buffer);
    return res;
  };

int SubParamFromString(char * par_name, int par_idx, char* sub_par, double& par_val, char* str)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*32);
	assert(buffer != NULL);
    if((res=SubParamFromString(par_name,par_idx,sub_par,buffer,str)) != 0)
      par_val=StrToDouble(buffer);
    free(buffer);
    return res;
  };


int ParamFromStrArr(char * par_name, double& par_val, char** str_arr, int str_cnt)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*32);
	assert(buffer != NULL);
    if((res=ParamFromStrArr(par_name,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToDouble(buffer);
    free(buffer);
    return res;
  };

int IndexParamFromStrArr(char * par_name, int par_idx, double& par_val, char** str_arr, int str_cnt)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*32);
	assert(buffer != NULL);
    if((res=IndexParamFromStrArr(par_name,par_idx,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToDouble(buffer);
    free(buffer);
    return res;
  };

int SubParamFromStrArr(char * par_name, int par_idx, char* sub_par, double& par_val, char** str_arr, int str_cnt)
  { int res = 0;
    char* buffer=(char*)malloc(sizeof(char)*32);
	assert(buffer != NULL);
    if((res=SubParamFromStrArr(par_name,par_idx,sub_par,buffer,str_arr,str_cnt)) != 0)
      par_val=StrToDouble(buffer);
    free(buffer);
    return res;
  };

//******* SPECIAL ROUTINES

//==================================================================================

double atof( const string& str )
{
	return atof(str.c_str());
}

int atoi( const string& str )
{
	return atoi(str.c_str());
}

long atol( const string& str )
{
	return atol(str.c_str());
}


bool ParamFromString(const string& par_name, string& par_val, const string& str)
{ 
	string::size_type i = str.find(par_name);
	if (i == string::npos) return false;
	i += par_name.size() + 1;
	i = str.find_first_not_of(' ', i);
	if (str[i] != '=') return false;
	i = str.find_first_not_of(' ', i + 1);
	string::size_type j = str.find_first_of(" ,;", i);
	if (j == string::npos) return false;
	par_val = str.substr(i, j-1);
	return true;
}

bool ParamFromString(const string& par_name, int& par_val, const string& str)
{
	string par;
	if (ParamFromString(par_name, par, str)) {
		par_val = atoi(par.c_str());
		return true;
	} else {
		par_val = 0;
		return false;
	}		
}

bool ParamFromString(const string& par_name, long& par_val, const string& str)
{
	string par;
	if (ParamFromString(par_name, par, str)) {
		par_val = atol(par.c_str());
		return true;
	} else {
		par_val = 0L;
		return false;
	}		
}


bool ParamFromString(const string& par_name, double& par_val, const string& str)
{
	string par;
	if (ParamFromString(par_name, par, str)) {
		par_val = atof(par.c_str());
		return true;
	} else {
		par_val = 0.;
		return false;
	}		
}

bool ParamFromString(const string& par_name, int ind, string& par_val, const string& str)
{ 
	string	index;
	string::size_type pos = 0;
	string::size_type i = str.find(par_name, pos);
	if (i == string::npos) return false;
	i += par_name.size() + 1;						// skip parameter name
	i = str.find_first_not_of(' ', i);				// skip spaces
	if (str[i] != '=') return false;
	i = str.find_first_not_of(' ', i + 1);
	string::size_type j = str.find_first_of(" ,;", i);
	if (j == string::npos) return false;
	par_val = str.substr(i, j-1);
	return true;
}


int ModifierFromStrArr(char** str_arr, int str_cnt)
{
    for (int i=0; i < str_cnt; i++) {
		if (strstr(str_arr[i], "(AMView)") != NULL)
			return 1;
		if (strstr(str_arr[i], "(DisArray)") != NULL)
			return 2;
    }
	return 0;
}


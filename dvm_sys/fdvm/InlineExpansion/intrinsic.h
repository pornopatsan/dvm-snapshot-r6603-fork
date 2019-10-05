intrinsic_type[ICHAR] = 1; 
intrinsic_type[CHAR]  = 7;
intrinsic_type[INT]   = 1; //
intrinsic_type[IFIX]  = 1;
intrinsic_type[IDINT] = 1;
intrinsic_type[FLOAT] = 3;
intrinsic_type[REAL]  = 3; //
intrinsic_type[SNGL]  = 3;
intrinsic_type[DBLE]  = 4; //
intrinsic_type[CMPLX] = 5; //
intrinsic_type[DCMPLX]= 6;
intrinsic_type[AINT]  = 3; //
intrinsic_type[DINT]  = 4;
intrinsic_type[ANINT] = 3; //
intrinsic_type[DNINT] = 4;
intrinsic_type[NINT]  = 1; //
intrinsic_type[IDNINT]= 1;
intrinsic_type[ABS]   =-1; //3
intrinsic_type[IABS]  = 1;
intrinsic_type[DABS]  = 4;
intrinsic_type[CABS]  = 5;
intrinsic_type[MOD]   =-1; //1
intrinsic_type[AMOD]  = 3;
intrinsic_type[DMOD]  = 4;
intrinsic_type[SIGN]  =-1; //3
intrinsic_type[ISIGN] = 1;
intrinsic_type[DSIGN] = 4;
intrinsic_type[DIM]   =-1; //3
intrinsic_type[IDIM]  = 1;
intrinsic_type[DDIM]  = 4;
intrinsic_type[MAX]   =-1;
intrinsic_type[MAX0]  = 1;
intrinsic_type[AMAX1] = 3;
intrinsic_type[DMAX1] = 4;
intrinsic_type[AMAX0] = 3;
intrinsic_type[MAX1]  = 1;
intrinsic_type[MIN]   =-1;  //
intrinsic_type[MIN0]  = 1;
intrinsic_type[AMIN1] = 3;
intrinsic_type[DMIN1] = 4;
intrinsic_type[AMIN0] = 3;
intrinsic_type[MIN1]  = 1;
intrinsic_type[LEN]   = 1;
intrinsic_type[INDEX] = 1;
intrinsic_type[AIMAG] =-1; //3
intrinsic_type[DIMAG] = 4;
intrinsic_type[CONJG] =-1; //5
intrinsic_type[DCONJG]= 6;
intrinsic_type[SQRT]  =-1; //3
intrinsic_type[DSQRT] = 4;
intrinsic_type[CSQRT] = 5;
intrinsic_type[EXP]   =-1; //3
intrinsic_type[DEXP]  = 4;
intrinsic_type[CEXP]  = 5;
intrinsic_type[LOG]   =-1; //
intrinsic_type[ALOG]  = 3;
intrinsic_type[DLOG]  = 4;
intrinsic_type[CLOG]  = 5;
intrinsic_type[LOG10] =-1; //
intrinsic_type[ALOG10]= 3;
intrinsic_type[DLOG10]= 4;
intrinsic_type[SIN]   =-1; //3
intrinsic_type[DSIN]  = 4;
intrinsic_type[CSIN]  = 5;
intrinsic_type[COS]   =-1; //3
intrinsic_type[DCOS]  = 4;
intrinsic_type[CCOS]  = 5;
intrinsic_type[TAN]   =-1; //3
intrinsic_type[DTAN]  = 4;
intrinsic_type[ASIN]  =-1; //3
intrinsic_type[DASIN] = 4;
intrinsic_type[ACOS]  =-1; //3
intrinsic_type[DACOS] = 4;
intrinsic_type[ATAN]  =-1; //3
intrinsic_type[DATAN] = 4;
intrinsic_type[ATAN2] =-1; //3
intrinsic_type[DATAN2]= 4;
intrinsic_type[SINH]  =-1; //3
intrinsic_type[DSINH] = 4;
intrinsic_type[COSH]  =-1; //3
intrinsic_type[DCOSH] = 4;
intrinsic_type[TANH]  =-1; //3
intrinsic_type[DTANH] = 4;
intrinsic_type[LGE]   = 2;
intrinsic_type[LGT]   = 2;
intrinsic_type[LLE]   = 2;
intrinsic_type[LLT]   = 2;
//intrinsic_type[] = ;
//intrinsic_type[] = ;


//{ICHAR, CHAR,INT,IFIX,IDINT,FLOAT,REAL,SNGL,DBLE,CMPLX,DCMPLX,AINT,DINT,ANINT,DNINT,NINT,IDNINT,ABS,IABS,DABS,CABS,
//      MOD,AMOD,DMOD, SIGN,ISIGN, DSIGN, DIM,IDIM,DDIM, MAX,MAX0, AMAX1,DMAX1, AMAX0,MAX1, MIN,MIN0,
//      AMIN1,DMIN1,AMIN0,MIN1,LEN,INDEX,AIMAG,DIMAG,CONJG,DCONJG,SQRT,DSQRT,CSQRT,EXP,DEXP.CEXP,LOG,ALOG,DLOG,CLOG,
//      LOG10,ALOG10,DLOG10,SIN,DSIN,CSIN,COS,DCOS,CCOS,TAN,DTAN,ASIN,DASIN,ACOS,DACOS,ATAN,DATAN,
//      ATAN2,DATAN2,SINH,DSINH,COSH,DCOSH,TANH,DTANH, LGE,LGT,LLE,LLT};
//universal: ANINT,NINT,ABS,  MOD,SIGN,DIM,MAX,MIN,SQRT,EXP,LOG,LOG10,SIN,COS,TAN,ASIN,ACOS,ATAN,ATAN2,SINH,COSH,TANH

//universal name - -1
//integer - 1
//logical - 2
//real - 3
//double precision - 4
//complex - 5
//complex*16 - 6
//character - 7

intrinsic_name[ICHAR] = "ichar"; 
intrinsic_name[CHAR]  = "char";
intrinsic_name[INT]   = "int"; //
intrinsic_name[IFIX]  = "ifix";
intrinsic_name[IDINT] = "idint";
intrinsic_name[FLOAT] = "float";
intrinsic_name[REAL]  = "real"; //
intrinsic_name[SNGL]  = "sngl";
intrinsic_name[DBLE]  = "dble"; //
intrinsic_name[CMPLX] = "cmplx"; //
intrinsic_name[DCMPLX]= "dcmplx";
intrinsic_name[AINT]  = "aint"; //
intrinsic_name[DINT]  = "dint";
intrinsic_name[ANINT] = "anint"; //
intrinsic_name[DNINT] = "dnint";
intrinsic_name[NINT]  = "nint"; //
intrinsic_name[IDNINT]= "idnint";
intrinsic_name[ABS]   = "abs"; //
intrinsic_name[IABS]  = "iabs";
intrinsic_name[DABS]  = "dabs";
intrinsic_name[CABS]  = "cabs";
intrinsic_name[MOD]   = "mod"; //
intrinsic_name[AMOD]  = "amod";
intrinsic_name[DMOD]  = "dmod";
intrinsic_name[SIGN]  = "sign"; //
intrinsic_name[ISIGN] = "isign";
intrinsic_name[DSIGN] = "dsign";
intrinsic_name[DIM]   = "dim"; //
intrinsic_name[IDIM]  = "idim";
intrinsic_name[DDIM]  = "ddim";
intrinsic_name[MAX]   = "max";
intrinsic_name[MAX0]  = "max0";
intrinsic_name[AMAX1] = "amax1";
intrinsic_name[DMAX1] = "dmax1";
intrinsic_name[AMAX0] = "amax0";
intrinsic_name[MAX1]  = "max1";
intrinsic_name[MIN]   = "min";  //
intrinsic_name[MIN0]  = "min0";
intrinsic_name[AMIN1] = "amin1";
intrinsic_name[DMIN1] = "dmin1";
intrinsic_name[AMIN0] = "amin0";
intrinsic_name[MIN1]  = "min1";
intrinsic_name[LEN]   = "len";
intrinsic_name[INDEX] = "index";
intrinsic_name[AIMAG] = "AIMAG"; //
intrinsic_name[DIMAG] = "DIMAG";
intrinsic_name[CONJG] = "conjg"; //
intrinsic_name[DCONJG]= "dconjg";
intrinsic_name[SQRT]  = "sqrt"; //
intrinsic_name[DSQRT] = "dsqrt";
intrinsic_name[CSQRT] = "csqrt";
intrinsic_name[EXP]   = "exp"; //
intrinsic_name[DEXP]  = "dexp";
intrinsic_name[CEXP]  = "cexp";
intrinsic_name[LOG]   = "log"; //
intrinsic_name[ALOG]  = "alog";
intrinsic_name[DLOG]  = "dlog";
intrinsic_name[CLOG]  = "clog";
intrinsic_name[LOG10] = "log10"; //
intrinsic_name[ALOG10]= "alog10";
intrinsic_name[DLOG10]= "dlog10";
intrinsic_name[SIN]   = "sin"; //
intrinsic_name[DSIN]  = "dsin";
intrinsic_name[CSIN]  = "csin";
intrinsic_name[COS]   = "cos"; //
intrinsic_name[DCOS]  = "dcos";
intrinsic_name[CCOS]  = "ccos";
intrinsic_name[TAN]   = "tan"; //
intrinsic_name[DTAN]  = "dtan";
intrinsic_name[ASIN]  = "asin"; //
intrinsic_name[DASIN] = "dasin";
intrinsic_name[ACOS]  = "acos"; //
intrinsic_name[DACOS] = "dacos";
intrinsic_name[ATAN]  = "atan"; //
intrinsic_name[DATAN] = "datan";
intrinsic_name[ATAN2] = "atan2"; //
intrinsic_name[DATAN2]= "datan2";
intrinsic_name[SINH]  = "sinh"; //
intrinsic_name[DSINH] = "dsinh";
intrinsic_name[COSH]  = "cosh"; //
intrinsic_name[DCOSH] = "dcosh";
intrinsic_name[TANH]  = "tanh"; //
intrinsic_name[DTANH] = "dtanh";
intrinsic_name[LGE]   = "lge";
intrinsic_name[LGT]   = "lgt";
intrinsic_name[LLE]   = "lle";
intrinsic_name[LLT]   = "llt";



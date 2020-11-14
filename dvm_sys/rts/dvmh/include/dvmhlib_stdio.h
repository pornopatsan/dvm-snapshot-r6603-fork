#ifndef DVMHLIB_STDIO_H
#define DVMHLIB_STDIO_H

#include <stdio.h>
#include <stdarg.h>

#include "dvmhlib_types.h"

#pragma GCC visibility push(default)
#ifdef __cplusplus
extern "C" {
#endif

extern FILE *dvmh_stdin, *dvmh_stdout, *dvmh_stderr;

int dvmh_remove(const char *filename);
int dvmh_rename(const char *oldFN, const char *newFN);
FILE *dvmh_tmpfile(void);
char *dvmh_tmpnam(char *s);

int dvmh_fclose(FILE *stream);
int dvmh_fflush(FILE *stream);
FILE *dvmh_fopen(const char *filename, const char *mode);
FILE *dvmh_freopen(const char *filename, const char *mode, FILE *stream);
void dvmh_setbuf(FILE *stream, char *buf);
int dvmh_setvbuf(FILE *stream, char *buf, int mode, size_t size);

int dvmh_fprintf(FILE *stream, const char *format, ...);
void dvmh_void_fprintf(FILE *stream, const char *format, ...);
int dvmh_fscanf(FILE *stream, const char *format, ...);
void dvmh_void_fscanf(FILE *stream, const char *format, ...);
int dvmh_printf(const char *format, ...);
void dvmh_void_printf(const char *format, ...);
int dvmh_scanf(const char *format, ...);
void dvmh_void_scanf(const char *format, ...);
int dvmh_vfprintf(FILE *stream, const char *format, va_list arg);
void dvmh_void_vfprintf(FILE *stream, const char *format, va_list arg);
int dvmh_vfscanf(FILE *stream, const char *format, va_list arg);
void dvmh_void_vfscanf(FILE *stream, const char *format, va_list arg);
int dvmh_vprintf(const char *format, va_list arg);
void dvmh_void_vprintf(const char *format, va_list arg);
int dvmh_vscanf(const char *format, va_list arg);
void dvmh_void_vscanf(const char *format, va_list arg);

int dvmh_fgetc(FILE *stream);
char *dvmh_fgets(char *s, int n, FILE *stream);
void dvmh_void_fgets(char *s, int n, FILE *stream);
int dvmh_fputc(int c, FILE *stream);
void dvmh_void_fputc(int c, FILE *stream);
int dvmh_fputs(const char *s, FILE *stream);
void dvmh_void_fputs(const char *s, FILE *stream);
int dvmh_getc(FILE *stream);
int dvmh_getchar(void);
char *dvmh_gets(char *s);
void dvmh_void_gets(char *s);
int dvmh_putc(int c, FILE *stream);
void dvmh_void_putc(int c, FILE *stream);
int dvmh_putchar(int c);
void dvmh_void_putchar(int c);
int dvmh_puts(const char *s);
void dvmh_void_puts(const char *s);
int dvmh_ungetc(int c, FILE *stream);
void dvmh_void_ungetc(int c, FILE *stream);

size_t dvmh_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t dvmh_fread_distrib(DvmType dvmDesc[], size_t size, size_t nmemb, FILE *stream);
void dvmh_void_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
void dvmh_void_fread_distrib(DvmType dvmDesc[], size_t size, size_t nmemb, FILE *stream);
size_t dvmh_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t dvmh_fwrite_distrib(const DvmType dvmDesc[], size_t size, size_t nmemb, FILE *stream);
void dvmh_void_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
void dvmh_void_fwrite_distrib(const DvmType dvmDesc[], size_t size, size_t nmemb, FILE *stream);

int dvmh_fgetpos(FILE *stream, fpos_t *pos);
int dvmh_fseek(FILE *stream, long offset, int whence);
void dvmh_void_fseek(FILE *stream, long offset, int whence);
int dvmh_fsetpos(FILE *stream, const fpos_t *pos);
long dvmh_ftell(FILE *stream);
void dvmh_rewind(FILE *stream);

void dvmh_clearerr(FILE *stream);
int dvmh_feof(FILE *stream);
int dvmh_ferror(FILE *stream);
void dvmh_perror(const char *s);

void dvmh_ftn_open_(const DvmType *pUnit, const DvmType *pAccessStr, const DvmType *pActionStr, const DvmType *pAsyncStr, const DvmType *pBlankStr,
        const DvmType *pDecimalStr, const DvmType *pDelimStr, const DvmType *pEncodingStr, const DvmType *pFileStr, const DvmType *pFormStr,
        const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pNewUnitRef, const DvmType *pPadStr,
        const DvmType *pPositionStr, const DvmType *pRecl, const DvmType *pRoundStr, const DvmType *pSignStr, const DvmType *pStatusStr,
        const DvmType *pDvmModeStr);
void dvmh_ftn_close_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pStatusStr);
DvmType dvmh_ftn_connected_(const DvmType *pUnit, const DvmType *pFailIfYes);

void dvmh_ftn_read_unf_(const DvmType *pUnit, const DvmType *pEndFlagRef, const DvmType *pErrFlagRef, const DvmType *pIOStatRef,
        const DvmType *pIOMsgStrRef, const DvmType *pPos, const DvmType dvmDesc[], const DvmType *pSpecifiedRank,
        /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);
void dvmh_ftn_write_unf_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef, const DvmType *pPos,
        const DvmType dvmDesc[], const DvmType *pSpecifiedRank, /* const DvmType *pIndexLow, const DvmType *pIndexHigh */...);

void dvmh_ftn_endfile_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef);
void dvmh_ftn_rewind_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef);
void dvmh_ftn_flush_(const DvmType *pUnit, const DvmType *pErrFlagRef, const DvmType *pIOStatRef, const DvmType *pIOMsgStrRef);

void dvmh_cp_save_filenames_(const DvmType* cpName, const DvmType *filesCount, ...);
void dvmh_cp_next_filename_(const DvmType* pCpName, const DvmType *pPrevFile, const DvmType *pCurrFileRef);
void dvmh_cp_check_filename_(const DvmType* pCpName, const DvmType *pFile);
void dvmh_cp_wait_(const DvmType* pCpName, const DvmType *pStatusVarRef);
void dvmh_cp_save_async_unit_(const DvmType* pCpName, const DvmType *pFile, const DvmType *pWriteUnit);

#ifdef __cplusplus
}
#endif
#pragma GCC visibility pop

#endif

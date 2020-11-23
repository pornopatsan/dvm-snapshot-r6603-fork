#ifndef CDVMH_DEBUG_HELPERS_H
#define CDVMH_DEBUG_HELPERS_H

#include <dvmhlib2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DVMH_DBG_READ_VAR( filename, line, rt_type, base, var ) \
( \
    dvmh_line_C( line, filename ),\
    dvmh_dbg_read_var_C( ( DvmType )rt_type, ( DvmType )&var, ( DvmType )base, #var ), \
    var \
)
    
#define DVMH_DBG_WRITE_VAR( filename, line, var, base, rt_type, expr ) \
( \
    dvmh_line_C( line, filename ), \
    dvmh_dbg_before_write_var_C( ( DvmType )rt_type, ( DvmType )&var, ( DvmType )base, #var ), \
    ( expr ), \
    dvmh_dbg_after_write_var_C(), \
    var \
)
    
#define DVMH_DBG_INIT_VAR( filename, line, var, base, rt_type, rhs ) \
( \
    dvmh_line_C( line, filename ), \
    dvmh_dbg_before_write_var_C( ( DvmType )rt_type, ( DvmType )&var, ( DvmType )base, #var ), \
    ( var = ( rhs ) ), \
    dvmh_dbg_after_write_var_C(), \
    rhs \
)
  
#define DVMH_DBG_LOOP_SEQ_START( filename, line, num ) \
( \
    dvmh_line_C( line, filename ), \
    dvmh_dbg_loop_seq_start_C( num ) \
)
    
#define DVMH_DBG_LOOP_END( filename, line, num ) \
( \
    dvmh_line_C( line, filename ), \
    dvmh_dbg_loop_end_C() \
)
    
// rest is:
//    DvmType rank - rank of the loop
//    AddrType addr1, ..., AddrType addrN - addresses of loop variables
//    DvmType type1, ..., DvmType typeN - types of loop variables
// ex: dvmh_dbg_loop_iter_Cdvmh_dbg_loop_iter_C( "file.cdv", 20, ( 1, ( AddrType )&i, ( AddrType )&j, rt_INT, rt_INT ) )
#define DVMH_DBG_LOOP_ITER( filename, line, rest ) \
( \
    dvmh_line_C( line, filename ), \
    dvmh_dbg_loop_iter_C rest \
)

// rest is:
//    DvmType num - number of the loop
//    DvmType rank - rank of the loop
//    DvmType start_ind1, ... , DvmType start_indN - start indices values (comma-separated)
//    DvmType end_ind1, ..., DvmType end_indN - end indices values (comma-separated)
//    DvmType step1, ..., DvmType stepN - indices steps (comma-separated)
// ex: dvmh_dbg_loop_par_start_C( "file.cdv", 20, ( 1, 2, 0, 0, N - 1, N - 1, 1, 1 ) )
#define DVMH_DBG_LOOP_PAR_START( filename, line, rest ) \
( \
    dvmh_line_C( line, filename ), \
    dvmh_dbg_loop_par_start_C rest \
)
    
#ifdef __cplusplus
}
#endif

#endif /* CDVMH_DEBUG_MACROS_H */


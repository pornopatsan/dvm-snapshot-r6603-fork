/*****************************/
/* all general functions     */
/*****************************/
#include "leak_detector.h"

#include "acc_data.h"
#include "dvm.h"

using std::string;
using std::set;

// copy input string to another buffer 
char *copyOfUnparse(const char *strUp)
{
    char *str;
    str = new char[strlen(strUp) + 1];
    strcpy(str, strUp);
    return str;
}

// convert "str " to "STR "
char* aks_strupr(const char *str)
{
    char *tmpstr = new char[strlen(str) + 1];
    tmpstr[0] = '\0';
    strcat(tmpstr, str);
    for (size_t i = 0; i < strlen(tmpstr); ++i)
    {
        if (tmpstr[i] <= 'z' && tmpstr[i] >= 'a')
            tmpstr[i] += 'A' - 'a';
    }
    return tmpstr;
}

// convert "STR" to "str"
char* aks_strlowr(const char *str)
{
    char *tmpstr = new char[strlen(str) + 1];
    tmpstr[0] = '\0';
    strcat(tmpstr, str);
    for (size_t i = 0; i < strlen(tmpstr); ++i)
    {
        if (tmpstr[i] <= 'Z' && tmpstr[i] >= 'A')
            tmpstr[i] -= 'A' - 'a';
    }
    return tmpstr;
}

void initIntrinsicFunctionNames()
{
    if (intrinsicF.size() != 0)
        return;

    intrinsicF.insert(string("abs"));
#ifdef __SPF
        intrinsicF.insert(string("adjustl"));
#endif
    intrinsicF.insert(string("and"));
#ifdef __SPF
    intrinsicF.insert(string("any"));
    intrinsicF.insert(string("associated"));
    intrinsicF.insert(string("allocated"));
#endif
    intrinsicF.insert(string("amod"));
    intrinsicF.insert(string("aimax0"));
    intrinsicF.insert(string("ajmax0"));
    intrinsicF.insert(string("akmax0"));
    intrinsicF.insert(string("aimin0"));
    intrinsicF.insert(string("ajmin0"));
    intrinsicF.insert(string("akmin0"));
    intrinsicF.insert(string("amax1"));
    intrinsicF.insert(string("amax0"));
    intrinsicF.insert(string("amin1"));
    intrinsicF.insert(string("amin0"));
    intrinsicF.insert(string("aimag"));
    intrinsicF.insert(string("alog"));
    intrinsicF.insert(string("alog10"));
    intrinsicF.insert(string("asin"));
    intrinsicF.insert(string("asind"));
    intrinsicF.insert(string("asinh"));
    intrinsicF.insert(string("acos"));
    intrinsicF.insert(string("acosd"));
    intrinsicF.insert(string("acosh"));
    intrinsicF.insert(string("atan"));
    intrinsicF.insert(string("atand"));
    intrinsicF.insert(string("atanh"));
    intrinsicF.insert(string("atan2"));
    intrinsicF.insert(string("atan2d"));
#ifdef __SPF
    intrinsicF.insert(string("aint"));
    intrinsicF.insert(string("anint"));
    intrinsicF.insert(string("achar"));
#endif
    intrinsicF.insert(string("babs"));
    intrinsicF.insert(string("bbits"));
    intrinsicF.insert(string("bbset"));
    intrinsicF.insert(string("bdim"));
    intrinsicF.insert(string("biand"));
    intrinsicF.insert(string("bieor"));
    intrinsicF.insert(string("bior"));
    intrinsicF.insert(string("bixor"));
    intrinsicF.insert(string("btest"));
    intrinsicF.insert(string("bbtest"));
    intrinsicF.insert(string("bbclr"));
    intrinsicF.insert(string("bitest"));
    intrinsicF.insert(string("bjtest"));
    intrinsicF.insert(string("bktest"));
    intrinsicF.insert(string("bessel_j0"));
    intrinsicF.insert(string("bessel_j1"));
    intrinsicF.insert(string("bessel_jn"));
    intrinsicF.insert(string("bessel_y0"));
    intrinsicF.insert(string("bessel_y1"));
    intrinsicF.insert(string("bessel_yn"));
    intrinsicF.insert(string("bmod"));
    intrinsicF.insert(string("bnot"));
    intrinsicF.insert(string("bshft"));
    intrinsicF.insert(string("bshftc"));
    intrinsicF.insert(string("bsign"));
    intrinsicF.insert(string("cos"));
    intrinsicF.insert(string("ccos"));
    intrinsicF.insert(string("cdcos"));
    intrinsicF.insert(string("cosd"));
    intrinsicF.insert(string("cosh"));
    intrinsicF.insert(string("cotan"));
    intrinsicF.insert(string("cotand"));
    intrinsicF.insert(string("ceiling"));
    intrinsicF.insert(string("cexp"));
    intrinsicF.insert(string("conjg"));
    intrinsicF.insert(string("csqrt"));
    intrinsicF.insert(string("clog"));
    intrinsicF.insert(string("clog10"));
    intrinsicF.insert(string("cdlog"));
    intrinsicF.insert(string("cdlog10"));
    intrinsicF.insert(string("csin"));
    intrinsicF.insert(string("cabs"));
    intrinsicF.insert(string("cdabs"));
    intrinsicF.insert(string("cdexp"));
    intrinsicF.insert(string("cdsin"));
    intrinsicF.insert(string("cdsqrt"));
    intrinsicF.insert(string("cdtan"));
    intrinsicF.insert(string("cmplx"));
    intrinsicF.insert(string("char"));
    intrinsicF.insert(string("ctan"));
    intrinsicF.insert(string("cpu_time"));
    intrinsicF.insert(string("dim"));
    intrinsicF.insert(string("ddim"));
    intrinsicF.insert(string("dble"));
    intrinsicF.insert(string("dfloat"));
    intrinsicF.insert(string("dfloti"));
    intrinsicF.insert(string("dflotj"));
    intrinsicF.insert(string("dflotk"));
#ifdef __SPF
    intrinsicF.insert(string("dint"));
    intrinsicF.insert(string("dvtime"));
#endif
    intrinsicF.insert(string("dmax1"));
    intrinsicF.insert(string("dmin1"));
    intrinsicF.insert(string("dmod"));
    intrinsicF.insert(string("dprod"));
    intrinsicF.insert(string("dreal"));
    intrinsicF.insert(string("dsign"));
    intrinsicF.insert(string("dshiftl"));
    intrinsicF.insert(string("dshiftr"));
    intrinsicF.insert(string("dabs"));
    intrinsicF.insert(string("dsqrt"));
    intrinsicF.insert(string("dexp"));
    intrinsicF.insert(string("dlog"));
    intrinsicF.insert(string("dlog10"));
    intrinsicF.insert(string("dsin"));
    intrinsicF.insert(string("dcos"));
    intrinsicF.insert(string("dcosd"));
    intrinsicF.insert(string("dtan"));
    intrinsicF.insert(string("dtand"));
    intrinsicF.insert(string("dasin"));
    intrinsicF.insert(string("dasind"));
    intrinsicF.insert(string("dasinh"));
    intrinsicF.insert(string("dacos"));
    intrinsicF.insert(string("dacosd"));
    intrinsicF.insert(string("dacosh"));
    intrinsicF.insert(string("datan"));
    intrinsicF.insert(string("datand"));
    intrinsicF.insert(string("datanh"));
    intrinsicF.insert(string("datan2"));
    intrinsicF.insert(string("datan2d"));
    intrinsicF.insert(string("derf"));
    intrinsicF.insert(string("derfc"));
    intrinsicF.insert(string("dsind"));
    intrinsicF.insert(string("dsinh"));
    intrinsicF.insert(string("dcosh"));
    intrinsicF.insert(string("dcotan"));
    intrinsicF.insert(string("dcotand"));
    intrinsicF.insert(string("dtanh"));
#ifdef __SPF
    intrinsicF.insert(string("dnint"));
#endif
    intrinsicF.insert(string("dcmplx"));
    intrinsicF.insert(string("dconjg"));
    intrinsicF.insert(string("dimag"));
    intrinsicF.insert(string("exp"));
    intrinsicF.insert(string("erf"));
    intrinsicF.insert(string("erfc"));
    intrinsicF.insert(string("erfc_scaled"));
#ifdef __SPF
    intrinsicF.insert(string("etime"));
#endif
    intrinsicF.insert(string("float"));    
    intrinsicF.insert(string("floati"));
    intrinsicF.insert(string("floatj"));
    intrinsicF.insert(string("floatk"));
    intrinsicF.insert(string("floor"));
#ifdef __SPF
    intrinsicF.insert(string("flush"));
#endif
    intrinsicF.insert(string("gamma"));
    intrinsicF.insert(string("habs"));
    intrinsicF.insert(string("hbclr"));
    intrinsicF.insert(string("hbits"));
    intrinsicF.insert(string("hbset"));
    intrinsicF.insert(string("hdim"));
    intrinsicF.insert(string("hiand"));
    intrinsicF.insert(string("hieor"));
    intrinsicF.insert(string("hior"));
    intrinsicF.insert(string("hixor"));
    intrinsicF.insert(string("hmod"));
    intrinsicF.insert(string("hnot"));
    intrinsicF.insert(string("hshft"));
    intrinsicF.insert(string("hshftc"));
    intrinsicF.insert(string("hsign"));
    intrinsicF.insert(string("htest"));
#ifdef __SPF
    intrinsicF.insert(string("huge"));
#endif
    intrinsicF.insert(string("hypot"));    
    intrinsicF.insert(string("iiabs"));
#ifdef __SPF
    intrinsicF.insert(string("iargc"));
#endif
    intrinsicF.insert(string("iiand"));
    intrinsicF.insert(string("iibclr"));
    intrinsicF.insert(string("iibits"));
    intrinsicF.insert(string("iibset"));
    intrinsicF.insert(string("iidim"));
    intrinsicF.insert(string("iieor"));
    intrinsicF.insert(string("iior"));
    intrinsicF.insert(string("iishft"));
    intrinsicF.insert(string("iishftc"));
    intrinsicF.insert(string("iisign"));
    intrinsicF.insert(string("iixor"));
    intrinsicF.insert(string("int"));
    intrinsicF.insert(string("idint"));
    intrinsicF.insert(string("ifix"));
    intrinsicF.insert(string("idim"));
    intrinsicF.insert(string("isign"));
    intrinsicF.insert(string("index"));
    intrinsicF.insert(string("iabs"));
    intrinsicF.insert(string("ibits"));
#ifdef __SPF
    intrinsicF.insert(string("idnint"));
    intrinsicF.insert(string("ichar"));
    intrinsicF.insert(string("iachar"));    
    intrinsicF.insert(string("isnan"));
#endif
    intrinsicF.insert(string("iand"));
    intrinsicF.insert(string("ior"));
    intrinsicF.insert(string("ibset"));
    intrinsicF.insert(string("ibclr"));
    intrinsicF.insert(string("ibchng"));
    intrinsicF.insert(string("ieor"));
    intrinsicF.insert(string("ilen"));
    intrinsicF.insert(string("imag"));
    intrinsicF.insert(string("imax0"));
    intrinsicF.insert(string("imax1"));
    intrinsicF.insert(string("imin0"));
    intrinsicF.insert(string("imin1"));
    intrinsicF.insert(string("imod"));
    intrinsicF.insert(string("inot"));
    intrinsicF.insert(string("isha"));
    intrinsicF.insert(string("ishc"));
    intrinsicF.insert(string("ishft"));
    intrinsicF.insert(string("ishftc"));
    intrinsicF.insert(string("ishl"));
    intrinsicF.insert(string("ixor"));
    intrinsicF.insert(string("jiabs"));
    intrinsicF.insert(string("jiand"));
    intrinsicF.insert(string("jibclr"));
    intrinsicF.insert(string("jibits"));
    intrinsicF.insert(string("jibset"));
    intrinsicF.insert(string("jidim"));
    intrinsicF.insert(string("jieor"));
    intrinsicF.insert(string("jior"));
    intrinsicF.insert(string("jishft"));
    intrinsicF.insert(string("jishftc"));
    intrinsicF.insert(string("jisign"));
    intrinsicF.insert(string("jixor"));
    intrinsicF.insert(string("jmax0"));
    intrinsicF.insert(string("jmax1"));
    intrinsicF.insert(string("jmin0"));
    intrinsicF.insert(string("jmin1"));
    intrinsicF.insert(string("jmod"));
    intrinsicF.insert(string("jnot"));
    intrinsicF.insert(string("kiabs"));
    intrinsicF.insert(string("kiand"));
    intrinsicF.insert(string("kibclr"));
    intrinsicF.insert(string("kibits"));
    intrinsicF.insert(string("kibset"));
    intrinsicF.insert(string("kidim"));
    intrinsicF.insert(string("kieor"));
    intrinsicF.insert(string("kior"));
    intrinsicF.insert(string("kishft"));
    intrinsicF.insert(string("kishftc"));
    intrinsicF.insert(string("kisign"));
    intrinsicF.insert(string("kmax0"));
    intrinsicF.insert(string("kmax1"));
    intrinsicF.insert(string("kmin0"));
    intrinsicF.insert(string("kmin1"));
    intrinsicF.insert(string("kmod"));
    intrinsicF.insert(string("knot"));
    intrinsicF.insert(string("len"));
#ifdef __SPF
    intrinsicF.insert(string("len_trim"));
#endif
    intrinsicF.insert(string("lge"));
    intrinsicF.insert(string("lgt"));
    intrinsicF.insert(string("lle"));
    intrinsicF.insert(string("llt"));
    intrinsicF.insert(string("log_gamma"));
    intrinsicF.insert(string("log"));
    intrinsicF.insert(string("log10"));
    intrinsicF.insert(string("lshft"));
    intrinsicF.insert(string("lshift"));
    intrinsicF.insert(string("max"));
    intrinsicF.insert(string("max0"));
    intrinsicF.insert(string("max1"));
    intrinsicF.insert(string("merge_bits"));
    intrinsicF.insert(string("min"));
#ifdef __SPF
    intrinsicF.insert(string("minval"));
    intrinsicF.insert(string("maxval"));
#endif
    intrinsicF.insert(string("min0"));
    intrinsicF.insert(string("min1"));
    intrinsicF.insert(string("mod"));
    intrinsicF.insert(string("modulo"));
    intrinsicF.insert(string("not"));
#ifdef __SPF
    intrinsicF.insert(string("nint"));
    intrinsicF.insert(string("null"));
#endif
    intrinsicF.insert(string("or"));
    intrinsicF.insert(string("popcnt"));
    intrinsicF.insert(string("poppar"));
    intrinsicF.insert(string("random_number"));
    intrinsicF.insert(string("real"));
    intrinsicF.insert(string("reshape"));
#ifdef __SPF      
    intrinsicF.insert(string("present"));    
    intrinsicF.insert(string("repeat"));
#endif
    intrinsicF.insert(string("rshft"));
    intrinsicF.insert(string("rshift"));
    intrinsicF.insert(string("sign"));
#ifdef __SPF
    intrinsicF.insert(string("size"));
    intrinsicF.insert(string("scan"));
    intrinsicF.insert(string("sizeof"));    
#endif
    intrinsicF.insert(string("sngl"));
    intrinsicF.insert(string("sqrt"));
    intrinsicF.insert(string("sin"));
    intrinsicF.insert(string("sind"));
    intrinsicF.insert(string("sinh"));
    intrinsicF.insert(string("shifta"));
    intrinsicF.insert(string("shiftl"));
    intrinsicF.insert(string("shiftr"));
    intrinsicF.insert(string("sum"));
    intrinsicF.insert(string("tan"));
    intrinsicF.insert(string("tand"));
    intrinsicF.insert(string("tanh"));
#ifdef __SPF
    intrinsicF.insert(string("tiny"));
#endif
    intrinsicF.insert(string("trailz"));
    intrinsicF.insert(string("trim"));
    intrinsicF.insert(string("xor"));
    intrinsicF.insert(string("wtime"));
    intrinsicF.insert(string("zabs"));
    intrinsicF.insert(string("zcos"));
    intrinsicF.insert(string("zexp"));
    intrinsicF.insert(string("zlog"));
    intrinsicF.insert(string("zsin"));
    intrinsicF.insert(string("zsqrt"));
    intrinsicF.insert(string("ztan"));

#ifdef __SPF
    //TODO: add all OMP functions
    intrinsicF.insert(string("omp_get_wtime"));
    intrinsicF.insert(string("omp_get_num_threads"));
    intrinsicF.insert(string("omp_destroy_lock"));
    intrinsicF.insert(string("omp_destroy_nest_lock"));
    intrinsicF.insert(string("omp_get_dynamic"));
    intrinsicF.insert(string("omp_get_max_threads"));
    intrinsicF.insert(string("omp_get_nested"));
    intrinsicF.insert(string("omp_get_num_procs"));
    intrinsicF.insert(string("omp_get_thread_num"));
    intrinsicF.insert(string("omp_init_lock"));
    intrinsicF.insert(string("omp_get_wtick"));
    intrinsicF.insert(string("omp_in_parallel"));
    intrinsicF.insert(string("omp_init_nest_lock"));
    intrinsicF.insert(string("omp_set_dynamic"));
    intrinsicF.insert(string("omp_set_lock"));
    intrinsicF.insert(string("omp_set_nest_lock"));
    intrinsicF.insert(string("omp_set_nested"));
    intrinsicF.insert(string("omp_set_num_threads"));
    intrinsicF.insert(string("omp_test_lock"));
    intrinsicF.insert(string("omp_test_nest_lock"));
    intrinsicF.insert(string("omp_unset_lock"));
    intrinsicF.insert(string("omp_unset_nest_lock"));

    //TODO: add all MPI functions
    intrinsicF.insert("mpi_address");
    intrinsicF.insert("mpi_allgather");
    intrinsicF.insert("mpi_allgatherv");
    intrinsicF.insert("mpi_allreduce");
    intrinsicF.insert("mpi_alltoall");
    intrinsicF.insert("mpi_alltoallv");
    intrinsicF.insert("mpi_barrier");
    intrinsicF.insert("mpi_bcast");
    intrinsicF.insert("mpi_bsend");
    intrinsicF.insert("mpi_bsend_init");
    intrinsicF.insert("mpi_buffer_attach");
    intrinsicF.insert("mpi_buffer_detach");
    intrinsicF.insert("mpi_cart_coords");
    intrinsicF.insert("mpi_cart_create");
    intrinsicF.insert("mpi_cart_get");
    intrinsicF.insert("mpi_cart_rank");
    intrinsicF.insert("mpi_cart_shift");
    intrinsicF.insert("mpi_cart_sub");
    intrinsicF.insert("mpi_cartdim_get");
    intrinsicF.insert("mpi_comm_create");
    intrinsicF.insert("mpi_comm_dup");
    intrinsicF.insert("mpi_comm_free");
    intrinsicF.insert("mpi_comm_group");
    intrinsicF.insert("mpi_comm_rank");
    intrinsicF.insert("mpi_comm_size");
    intrinsicF.insert("mpi_comm_split");
    intrinsicF.insert("mpi_dims_create");
    intrinsicF.insert("mpi_finalize");
    intrinsicF.insert("mpi_gather");
    intrinsicF.insert("mpi_gatherv");
    intrinsicF.insert("mpi_get_count");
    intrinsicF.insert("mpi_get_processor_name");
    intrinsicF.insert("mpi_graph_create");
    intrinsicF.insert("mpi_graph_get");
    intrinsicF.insert("mpi_graph_neighbors");
    intrinsicF.insert("mpi_graph_neighbors_count");
    intrinsicF.insert("mpi_graphdims_get");
    intrinsicF.insert("mpi_group_compare");
    intrinsicF.insert("mpi_group_difference");
    intrinsicF.insert("mpi_group_excl");
    intrinsicF.insert("mpi_group_free");
    intrinsicF.insert("mpi_group_incl");
    intrinsicF.insert("mpi_group_intersection");
    intrinsicF.insert("mpi_group_rank");
    intrinsicF.insert("mpi_group_size");
    intrinsicF.insert("mpi_group_translate_ranks");
    intrinsicF.insert("mpi_group_union");
    intrinsicF.insert("mpi_ibsend");
    intrinsicF.insert("mpi_init");
    intrinsicF.insert("mpi_initialized");
    intrinsicF.insert("mpi_iprobe");
    intrinsicF.insert("mpi_irecv");
    intrinsicF.insert("mpi_irsend");
    intrinsicF.insert("mpi_isend");
    intrinsicF.insert("mpi_issend");
    intrinsicF.insert("mpi_op_create");
    intrinsicF.insert("mpi_op_free");
    intrinsicF.insert("mpi_pack");
    intrinsicF.insert("mpi_pack_size");
    intrinsicF.insert("mpi_probe");
    intrinsicF.insert("mpi_recv");
    intrinsicF.insert("mpi_recv_init");
    intrinsicF.insert("mpi_reduce");
    intrinsicF.insert("mpi_reduce_scatter");
    intrinsicF.insert("mpi_request_free");
    intrinsicF.insert("mpi_rsend");
    intrinsicF.insert("mpi_rsend_init");
    intrinsicF.insert("mpi_scan");
    intrinsicF.insert("mpi_scatter");
    intrinsicF.insert("mpi_scatterv");
    intrinsicF.insert("mpi_send");
    intrinsicF.insert("mpi_send_init");
    intrinsicF.insert("mpi_sendrecv");
    intrinsicF.insert("mpi_sendrecv_replace");
    intrinsicF.insert("mpi_ssend");
    intrinsicF.insert("mpi_ssend_init");
    intrinsicF.insert("mpi_start");
    intrinsicF.insert("mpi_startall");
    intrinsicF.insert("mpi_test");
    intrinsicF.insert("mpi_testall");
    intrinsicF.insert("mpi_testany");
    intrinsicF.insert("mpi_testsome");
    intrinsicF.insert("mpi_topo_test");
    intrinsicF.insert("mpi_type_commit");
    intrinsicF.insert("mpi_type_contiguous");
    intrinsicF.insert("mpi_type_extent");
    intrinsicF.insert("mpi_type_free");
    intrinsicF.insert("mpi_type_hindexed");
    intrinsicF.insert("mpi_type_hvector");
    intrinsicF.insert("mpi_type_indexed");
    intrinsicF.insert("mpi_type_lb");
    intrinsicF.insert("mpi_type_size");
    intrinsicF.insert("mpi_type_struct");
    intrinsicF.insert("mpi_type_ub");
    intrinsicF.insert("mpi_type_vector");
    intrinsicF.insert("mpi_unpack");
    intrinsicF.insert("mpi_wait");
    intrinsicF.insert("mpi_waitall");
    intrinsicF.insert("mpi_waitany");
    intrinsicF.insert("mpi_waitsome");
    intrinsicF.insert("mpi_wtick");
    intrinsicF.insert("mpi_wtime");
#endif

    // set Types
    intrinsicDoubleT.insert(string("ddim"));
    intrinsicDoubleT.insert(string("dble"));
    intrinsicDoubleT.insert(string("dfloat"));
    intrinsicDoubleT.insert(string("dfloti"));
    intrinsicDoubleT.insert(string("dflotj"));
    intrinsicDoubleT.insert(string("dflotk"));
#ifdef __SPF
    intrinsicDoubleT.insert(string("dint"));
#endif
    intrinsicDoubleT.insert(string("dmax1"));
    intrinsicDoubleT.insert(string("dmin1"));
    intrinsicDoubleT.insert(string("dmod"));
    intrinsicDoubleT.insert(string("dprod"));
    intrinsicDoubleT.insert(string("dreal"));
    intrinsicDoubleT.insert(string("dsign"));
    intrinsicDoubleT.insert(string("dshiftl"));
    intrinsicDoubleT.insert(string("dshiftr"));
    intrinsicDoubleT.insert(string("dabs"));
    intrinsicDoubleT.insert(string("dsqrt"));
    intrinsicDoubleT.insert(string("dexp"));
    intrinsicDoubleT.insert(string("dlog"));
    intrinsicDoubleT.insert(string("dlog10"));
    intrinsicDoubleT.insert(string("dsin"));
    intrinsicDoubleT.insert(string("dcos"));
    intrinsicDoubleT.insert(string("dcosd"));
    intrinsicDoubleT.insert(string("dtan"));
    intrinsicDoubleT.insert(string("dtand"));
    intrinsicDoubleT.insert(string("dasin"));
    intrinsicDoubleT.insert(string("dasind"));
    intrinsicDoubleT.insert(string("dasinh"));
    intrinsicDoubleT.insert(string("dacos"));
    intrinsicDoubleT.insert(string("dacosd"));
    intrinsicDoubleT.insert(string("dacosh"));
    intrinsicDoubleT.insert(string("datan"));
    intrinsicDoubleT.insert(string("datand"));
    intrinsicDoubleT.insert(string("datanh"));
    intrinsicDoubleT.insert(string("datan2"));
    intrinsicDoubleT.insert(string("datan2d"));
    intrinsicDoubleT.insert(string("derf"));
    intrinsicDoubleT.insert(string("derfc"));
    intrinsicDoubleT.insert(string("dsind"));
    intrinsicDoubleT.insert(string("dsinh"));
    intrinsicDoubleT.insert(string("dcosh"));
    intrinsicDoubleT.insert(string("dcotan"));
    intrinsicDoubleT.insert(string("dcotand"));
    intrinsicDoubleT.insert(string("dtanh"));
#ifdef __SPF
    intrinsicDoubleT.insert(string("dnint"));
#endif
    intrinsicDoubleT.insert(string("dcmplx"));
    intrinsicDoubleT.insert(string("dconjg"));
    intrinsicDoubleT.insert(string("dimag"));

    intrinsicFloatT.insert(string("sngl"));
    intrinsicFloatT.insert(string("real"));   
    intrinsicFloatT.insert(string("float"));
}

//need to extend 
int getIntrinsicFunctionType(const char* name)
{
    if (!name)
        return 0;
        
    set<string>::iterator result = intrinsicF.find(name);
    if (result == intrinsicF.end())
        return 0;

    if (intrinsicDoubleT.find(name) != intrinsicDoubleT.end())
        return T_DOUBLE;
    else if (intrinsicFloatT.find(name) != intrinsicFloatT.end())
        return T_FLOAT;
    
    return 0;
}

int isIntrinsicFunctionName(const char *name)
{
    if (!name)
        return 0;

    int retval = 1;
    set<string>::iterator result = intrinsicF.find(name);

    if (result == intrinsicF.end())
        retval = 0;

    //check for dabs, dtan and etc.
    if (retval == 0 && name[0] == 'd')
    {
        string partName(name + 1);
        result = intrinsicF.find(partName);

        if (result != intrinsicF.end())
            retval = 1;
    }

    return retval;
}

SgSymbol *OriginalSymbol(SgSymbol *s)
{
    return((IS_BY_USE(s) ? (s)->moduleSymbol() : s));
}

#ifdef __SPF
extern "C" void addToCollection(const int line, const char *file, void *pointer, int type);
#endif

void addNumberOfFileToAttribute(SgProject *project)
{
    int numOfFiles = project->numberOfFiles();
    for (int i = 0; i < numOfFiles; ++i)
    {
        SgFile *currF = &(project->file(i));
        string t = currF->filename();
        int *num = new int[1];
#ifdef __SPF
        addToCollection(__LINE__, __FILE__, num, 2);
#endif
        num[0] = i;
        currF->addAttribute(SG_FILE_ATTR, num, sizeof(int));

        SgFile::addFile(std::make_pair(currF, i));

        // fill private info for all statements
        for (SgStatement *st = currF->firstStatement(); st; st = st->lexNext())
        {
            st->setFileId(i);
            st->setProject(project);
        }

        for (SgSymbol *sm = currF->firstSymbol(); sm; sm = sm->next())
        {
            sm->setFileId(i);
            sm->setProject(project);
        }
    }
}

#ifndef __SPF
// correct private list after CUDA kernel generation
void correctPrivateList(int flag)
{
    if (newVars.size() != 0)
    {
        if (flag == RESTORE)
        {
            if (private_list)
            {
                for (size_t i = 0; i < newVars.size(); ++i)
                    private_list = private_list->rhs();
            }
        }
        else if (flag == ADD)
        {
            for (size_t i = 0; i < newVars.size(); ++i)
            {
                SgExprListExp *e = new SgExprListExp(*new SgVarRefExp(*newVars[i]));
                e->setRhs(private_list);
                private_list = e;
            }
        }
    }
}

// create kernel call functions from HOST: skernel<<< specs>>>( args)
SgFunctionCallExp *cudaKernelCall(SgSymbol *skernel, SgExpression *specs, SgExpression *args = NULL)
{
    SgExpression *fe = new SgExpression(ACC_CALL_OP);
    fe->setSymbol(*skernel);
    fe->setRhs(*specs);
    if (args)
        fe->setLhs(*args);
    
    return (SgFunctionCallExp *)fe;
}

// create FORTRAN index type in kernel: integer*4 if rt_INT or 
// integer*8 if rt_LONG, rt_LLONG   
static SgType *FortranIndexType(int rtType)
{
    SgType *type = NULL;

    if (rtType == rt_INT)
    {
        SgExpression *le = new SgExpression(LEN_OP);
        le->setLhs(new SgValueExp(4));
        type = new SgType(T_INT, le, SgTypeInt());
    }
    else if (rtType == rt_LONG || rtType == rt_LLONG)
    {
        SgExpression *le = new SgExpression(LEN_OP);
        le->setLhs(new SgValueExp(8));
        type = new SgType(T_INT, le, SgTypeInt());
    }
    return type;
}

// create cuda index type in kernel for FORTRAN and C
SgType *indexTypeInKernel(int rt_Type)
{    
    SgType *ret = NULL;

    if (indexType_int == NULL)
    {
        s_indexType_int = new SgSymbol(TYPE_NAME, "__indexTypeInt", options.isOn(C_CUDA) ? *block_C_Cuda : *mod_gpu);
        s_indexType_int->setType(new SgDescriptType(*SgTypeInt(), BIT_TYPEDEF));
        if (options.isOn(C_CUDA))
            indexType_int = C_Derived_Type(s_indexType_int);
        else
        {
            SgExpression *le = new SgExpression(LEN_OP);
            le->setLhs(new SgValueExp(4));
            indexType_int = new SgType(T_INT, new SgVariableSymb("_int", *FortranIndexType(rt_INT), *mod_gpu), le, SgTypeInt());
        }
    }
    
    if (indexType_long == NULL)
    {
        s_indexType_long = new SgSymbol(TYPE_NAME, "__indexTypeLong", options.isOn(C_CUDA) ? *block_C_Cuda : *mod_gpu);
        s_indexType_long->setType(C_LongType());
        if (options.isOn(C_CUDA))
            indexType_long = C_Derived_Type(s_indexType_long);
        else
        {
            SgExpression *le = new SgExpression(LEN_OP);
            le->setLhs(new SgValueExp(8));
            indexType_long = new SgType(T_INT, new SgVariableSymb("_long", *FortranIndexType(rt_LONG), *mod_gpu), le, SgTypeInt());
        }
    }

    if (indexType_llong == NULL)
    {
        s_indexType_llong = new SgSymbol(TYPE_NAME, "__indexTypeLLong", options.isOn(C_CUDA) ? *block_C_Cuda : *mod_gpu);
        s_indexType_llong->setType(C_LongLongType());
        if (options.isOn(C_CUDA))
            indexType_llong = C_Derived_Type(s_indexType_llong);
        else
        {
            SgExpression *le = new SgExpression(LEN_OP);
            le->setLhs(new SgValueExp(8));
            indexType_llong = new SgType(T_INT, new SgVariableSymb("_llong", *FortranIndexType(rt_LLONG), *mod_gpu), le, SgTypeInt());            
        }
    }
    
    if (rt_Type == rt_INT)
        ret = indexType_int;
    else if (rt_Type == rt_LONG)
        ret = indexType_long;
    else if (rt_Type == rt_LLONG)
        ret = indexType_llong;

    return ret;
}

// declare DO variables of parallel loop nest in kernel by indexType: rt_INT, rt_LONG, rt_LLONG
void DeclareDoVars(SgType *indexType)
{
    SgStatement *st;
    SgExpression *vl, *el;

    // declare do_variables of parallel loop nest
    if (options.isOn(C_CUDA))
    {
        vl = &(dvm_parallel_dir->expr(2))->copy(); // do_variables list copy  
        for (el = vl; el; el = el->rhs())
            (el->lhs())->setSymbol(new SgVariableSymb(el->lhs()->symbol()->identifier(), *indexType, *kernel_st));
        st = Declaration_Statement(vl->lhs()->symbol());   // of CudaIndexType
        st->setExpression(0, *vl);
        kernel_st->insertStmtAfter(*st);
        st->addComment("// Local needs");
    }
    else // Fortran-Cuda
    {
        st = indexType->symbol()->makeVarDeclStmt();   // of CudaIndexType
        kernel_st->insertStmtAfter(*st);
        vl = dvm_parallel_dir->expr(2); // do_variables list
        st->setExpression(0, vl->copy());
        st->addComment("! Local needs\n");
    }
}


// create dvm coefficient:*0001, *0002 by indexType: rt_INT, rt_LONG, rt_LLONG
static SgExpression *dvm_coef(SgSymbol *ar, int i, SgType *indeTypeInKernel)
{
    SgVarRefExp *ret = NULL;
    if (options.isOn(C_CUDA))
    {
        SgSymbol *s_dummy_coef = new SgSymbol(VARIABLE_NAME, AR_COEFFICIENTS(ar)->sc[i]->identifier(), *indeTypeInKernel, *kernel_st);
        ret = new SgVarRefExp(*s_dummy_coef);
    }
    else
        ret = new SgVarRefExp(*(AR_COEFFICIENTS(ar)->sc[i]));
    return ret;
}

// create array list by indexType: rt_INT, rt_LONG, rt_LLONG
SgExpression *CreateArrayDummyList(SgType *indeTypeInKernel)
{
    symb_list *sl;
    SgExpression *ae, *coef_list, *edim;
    int n, d;
    SgExpression *arg_list = NULL;

    edim = new SgExprListExp();  // [] dimension

    for (sl = acc_array_list; sl; sl = sl->next)   // + base_ref + <array_coeffs>
    {
        SgSymbol *s_dummy;
        s_dummy = KernelDummyArray(sl->symb);
        if (options.isOn(C_CUDA))
            ae = new SgArrayRefExp(*s_dummy, *edim);
        else
            ae = new SgArrayRefExp(*s_dummy);
        ae->setType(s_dummy->type());   //for C_Cuda
        ae = new SgExprListExp(*ae);
        
        arg_list = AddListToList(arg_list, ae);
        coef_list = NULL;
        if (Rank(sl->symb) == 0)      //remote_access buffer may be of rank 0   
            continue;
        d = options.isOn(AUTO_TFM) ? 0 : 1;
        for (n = Rank(sl->symb) - d; n>0; n--)
        {
            ae = new SgExprListExp(*dvm_coef(sl->symb, n + 1, indeTypeInKernel));
            coef_list = AddListToList(coef_list, ae);
        }

        arg_list = AddListToList(arg_list, coef_list);
    }
    return(arg_list);

}


// create local parts of array list by indexType: rt_INT, rt_LONG, rt_LLONG
SgSymbol *KernelDummyLocalPart(SgSymbol *s, SgType *indeTypeInKernel)
{
    SgArrayType *typearray;
    SgType *type;

    // for C_Cuda
    typearray = new SgArrayType(*indeTypeInKernel);
    typearray->addDimension(NULL);
    type = typearray;

    return(new SgSymbol(VARIABLE_NAME, s->identifier(), *type, *kernel_st));

}

SgExpression *CreateLocalPartList(SgType *indeTypeInKernel)
{
    local_part_list *pl;
    SgExpression *ae;
    SgExpression *arg_list = NULL;
    for (pl = lpart_list; pl; pl = pl->next) // + <local_part>
    {
        if (options.isOn(C_CUDA))
            ae = new SgExprListExp(*new SgArrayRefExp(*KernelDummyLocalPart(pl->local_part, indeTypeInKernel), 
                                   *new SgExprListExp())); //<local_part>[]
        else
            ae = new SgExprListExp(*new SgArrayRefExp(*pl->local_part));
        arg_list = AddListToList(arg_list, ae);
    }
    return(arg_list);

}

// create two kernel calls (for rt_INT and rt_LLONG) in CUDA_handeler by base kernel function. 
// return if(rt_INT) kernel<<< >>>() else kernel2<<< >>>()
SgStatement* createKernelCallsInCudaHandler(SgFunctionCallExp *baseFunc, SgSymbol *s_loop_ref, SgSymbol *idxTypeInKernel, SgSymbol *s_blocks)
{
    SgStatement *stmt = NULL;
    std::string fcall_INT = baseFunc->symbol()->identifier();
    std::string fcall_LLONG = baseFunc->symbol()->identifier();
    fcall_INT += "_int";
    fcall_LLONG += "_llong";

    SgExpression *args = baseFunc->args();

    SgFunctionCallExp *funcCall_int = cudaKernelCall(new SgSymbol(VARIABLE_NAME, fcall_INT.c_str()), baseFunc->rhs());
    SgFunctionCallExp *funcCall_llong = cudaKernelCall(new SgSymbol(VARIABLE_NAME, fcall_LLONG.c_str()), baseFunc->rhs());
    
    while (args)
    {
        bool flag = false;
        if (args->lhs()->symbol())
        {
            if (strcmp(args->lhs()->symbol()->identifier(), "blocks_info") == 0)
            {
                funcCall_int->addArg(*new SgCastExp(*C_PointerType(indexTypeInKernel(rt_INT)), *args->lhs()));
                funcCall_llong->addArg(*new SgCastExp(*C_PointerType(indexTypeInKernel(rt_LLONG)), *args->lhs()));
                flag = true;
            }

            if (args->lhs()->getAttribute(0) != NULL)
            {
                SgAttribute *att = args->lhs()->getAttribute(0);
                if (att->getAttributeSize() == 777)
                {
                    funcCall_int->addArg(*new SgCastExp(*C_PointerType(indexTypeInKernel(rt_INT)), *args->lhs()));
                    funcCall_llong->addArg(*new SgCastExp(*C_PointerType(indexTypeInKernel(rt_LLONG)), *args->lhs()));
                    flag = true;
                    args->lhs()->deleteAttribute(0);
                }
            }
        }

        if (flag == false)
        {
            funcCall_int->addArg(*args->lhs());
            funcCall_llong->addArg(*args->lhs());
        }
        args = args->rhs();
    }

    if (options.isOn(RTC))
    {
        SgFunctionCallExp *rtc_FCall_INT = new SgFunctionCallExp(*createNewFunctionSymbol("loop_cuda_rtc_launch"));
        rtc_FCall_INT->addArg(*new SgVarRefExp(s_loop_ref));
        rtc_FCall_INT->addArg(*new SgValueExp(fcall_INT.c_str()));
        rtc_FCall_INT->addArg(*new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, fcall_INT.c_str())));
        rtc_FCall_INT->addArg(SgAddrOp(*new SgVarRefExp(s_blocks)));
        rtc_FCall_INT->addArg(*new SgValueExp(baseFunc->numberOfArgs()));

        RTC_FArgs.push_back(baseFunc->args());
        RTC_FCall.push_back(rtc_FCall_INT);

        SgFunctionCallExp *rtc_FCall_LLONG = new SgFunctionCallExp(*createNewFunctionSymbol("loop_cuda_rtc_launch"));
        rtc_FCall_LLONG->addArg(*new SgVarRefExp(s_loop_ref));
        rtc_FCall_LLONG->addArg(*new SgValueExp(fcall_LLONG.c_str()));
        rtc_FCall_LLONG->addArg(*new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, fcall_LLONG.c_str())));
        rtc_FCall_LLONG->addArg(SgAddrOp(*new SgVarRefExp(s_blocks)));
        rtc_FCall_LLONG->addArg(*new SgValueExp(baseFunc->numberOfArgs()));

        RTC_FArgs.push_back(baseFunc->args());
        RTC_FCall.push_back(rtc_FCall_LLONG);
    }

    if (options.isOn(RTC))
        stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(*idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT"))),
        *new SgCExpStmt(*RTC_FCall[RTC_FCall.size() - 2]), *new SgCExpStmt(*RTC_FCall[RTC_FCall.size() - 1]));
    else
        stmt = new SgIfStmt(SgEqOp(*new SgVarRefExp(*idxTypeInKernel), *new SgVarRefExp(*new SgSymbol(VARIABLE_NAME, "rt_INT"))),
        *new SgCExpStmt(*funcCall_int), *new SgCExpStmt(*funcCall_llong));
    return stmt;
}

static string getValue(SgExpression *exp)
{
    if (exp == NULL)
        return "";

    string ret = "";
    if (exp->symbol())
    {
        if (exp->symbol()->identifier())
            ret = "(" + string(exp->symbol()->identifier()) + ")";
    }
    else if (exp->variant() == INT_VAL)
    {
        char buf[256];
        sprintf(buf, "%d", exp->valueInteger());
        ret = "(" + string(buf) + ")";
    }
    else if (exp->variant() == ADD_OP)
        ret = "(+)";
    else if (exp->variant() == SUBT_OP)
        ret = "(-)";
    else if (exp->variant() == MULT_OP)
        ret = "(*)";
    else if (exp->variant() == DIV_OP)
        ret = "(/)";
    else if (exp->variant() == MOD_OP)
        ret = "(mod)";
    else if (exp->variant() == EXP_OP)
        ret = "(**)";
    else if (exp->variant() == KEYWORD_VAL)
        ret = "(" + string(((SgKeywordValExp*)exp)->value()) + ")";
    return ret;
}

static void recExpressionPrint(SgExpression* exp, const int lvl, const char* LR, const int currNum, int& allNum)
{
    if (exp)
    {
        SgExpression* lhs = exp->lhs();
        SgExpression* rhs = exp->rhs();
        int lNum, rNum;

        string vCurr = getValue(exp);
        string vL = getValue(lhs);
        string vR = getValue(rhs);

        if (lhs && rhs)
        {
            lNum = allNum + 1;
            rNum = allNum + 2;
            allNum += 2;
            printf("\"%d_%d_%s_%s_%s\" -> \"%d_%d_L_%s_%s\";\n", currNum, lvl, LR, tag[exp->variant()], vCurr.c_str(), lNum, lvl + 1, tag[lhs->variant()], vL.c_str());
            printf("\"%d_%d_%s_%s_%s\" -> \"%d_%d_R_%s_%s\";\n", currNum, lvl, LR, tag[exp->variant()], vCurr.c_str(), rNum, lvl + 1, tag[rhs->variant()], vR.c_str());
        }
        else if (lhs)
        {
            lNum = allNum + 1;
            allNum++;
            printf("\"%d_%d_%s_%s_%s\" -> \"%d_%d_L_%s_%s\";\n", currNum, lvl, LR, tag[exp->variant()], vCurr.c_str(), lNum, lvl + 1, tag[lhs->variant()], vL.c_str());
        }
        else if (rhs)
        {
            rNum = allNum + 1;
            allNum++;
            printf("\"%d_%d_%s_%s_%s\" -> \"%d_%d_R_%s_%s\";\n", currNum, lvl, LR, tag[exp->variant()], vCurr.c_str(), rNum, lvl + 1, tag[rhs->variant()], vR.c_str());
        }
        if (lhs)
            recExpressionPrint(lhs, lvl + 1, "L", lNum, allNum);
        if (rhs)
            recExpressionPrint(rhs, lvl + 1, "R", rNum, allNum);
    }
}

void recExpressionPrintFdvm(SgExpression *exp)
{
    printf("digraph G{\n");
    int allNum = 0;
    recExpressionPrint(exp, 0, "L", allNum, allNum);
    if (allNum == 0 && exp)
        printf("\"%d_%d_%s_%s_%s\";\n", allNum, 0, "L", tag[exp->variant()], getValue(exp).c_str());
    printf("}\n");
    fflush(NULL);
}
#endif
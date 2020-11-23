#include <string>
#include <fstream>

#include "Event.h"

using namespace std;
extern ofstream prot;

Event EventNameToID(const string& event_name)
  { 
//		cout<<"event='"<<event_name<<"'\n";
 
	//====
		if(event_name == "blkdiv_") return blkdiv_; //====// пока не обрабатывается
		if(event_name == "dvm_Init") return dvm_Init; 
		//=***

    if(event_name == "delrg_") return delrg_;
    if(event_name == "insred_") return insred_;
    if(event_name == "arrcpy_") return arrcpy_;
    if(event_name == "aarrcp_") return aarrcp_;
    if(event_name == "waitcp_") return waitcp_;
    if(event_name == "crtda_") return crtda_;
    if(event_name == "getam_") return getam_;
    if(event_name == "mapam_") return mapam_;
    if(event_name == "runam_") return runam_;
    if(event_name == "stopam_") return stopam_;
    if(event_name == "getamv_") return getamv_;
    if(event_name == "getamr_") return getamr_;
    if(event_name == "crtamv_") return crtamv_;
    if(event_name == "runam_") return runam_;
    if(event_name == "align_") return align_;
    if(event_name == "getps_") return getps_;
    if(event_name == "saverv_") return saverv_;
    if(event_name == "tstelm_") return tstelm_;
    if(event_name == "rwelm_") return rwelm_;
    if(event_name == "rlocel_") return rlocel_;
    if(event_name == "delda_") return delda_;
    if(event_name == "delobj_") return delobj_;
    if(event_name == "copelm_") return copelm_;
    if(event_name == "elmcpy_") return elmcpy_;
    if(event_name == "wlocel_") return wlocel_;
    if(event_name == "clocel_") return clocel_;
    if(event_name == "getlen_") return getlen_;
    if(event_name == "dvm_Init") return dvm_Init;
    if(event_name == "dvm_fopen") return dvm_fopen;
    if(event_name == "dvm_fclose") return dvm_fclose;
    if(event_name == "dvm_void_vfprintf") return dvm_void_vfprintf;
    if(event_name == "dvm_vfprintf") return dvm_vfprintf;
    if(event_name == "dvm_fwrite") return dvm_fwrite;
    if(event_name == "dvm_fread") return dvm_fread;
    if(event_name == "tron_") return tron_;
    if(event_name == "delamv_") return delamv_;
    if(event_name == "distr_") return distr_;
    if(event_name == "crtred_") return crtred_;
    if(event_name == "delred_") return delred_;
    if(event_name == "begbl_") return begbl_;
    if(event_name == "endbl_") return endbl_;
    if(event_name == "crtpl_") return crtpl_;
    if(event_name == "mappl_") return mappl_;
    if(event_name == "endpl_") return endpl_;
    if(event_name == "locind_") return locind_;
    if(event_name == "tstda_") return tstda_;
    if(event_name == "srmem_") return srmem_;
    if(event_name == "tstio_") return tstio_;
    if(event_name == "getrnk_") return getrnk_;
    if(event_name == "getsiz_") return getsiz_;
    if(event_name == "dvm_vscanf") return dvm_vscanf;
    if(event_name == "realn_")	return realn_;
    if(event_name == "redis_")	return redis_;
    if(event_name == "arrmap_") return arrmap_;
    if(event_name == "setpsw_") return setpsw_;
    if(event_name == "setind_") return setind_;
    if(event_name == "locsiz_") return locsiz_;
    if(event_name == "imlast_") return imlast_;
    if(event_name == "malign_") return malign_;
    if(event_name == "crtrg_")	return crtrg_;
    if(event_name == "mrealn_") return mrealn_;
    if(event_name == "strtrd_") return strtrd_;
    if(event_name == "waitrd_") return waitrd_;
    if(event_name == "amvmap_") return amvmap_;
    if(event_name == "exfrst_") return exfrst_;
	if(event_name == "across_") return across_;
    if(event_name == "dopl_")	return dopl_;
    if(event_name == "mdistr_") return mdistr_;
    if(event_name == "mredis_") return mredis_;
    if(event_name == "delarm_") return delarm_;
    if(event_name == "delmvm_") return delmvm_;
    if(event_name == "dvm_fscanf")	return dvm_fscanf;
    if(event_name == "dvm_scanf")	return dvm_scanf;
    if(event_name == "dvm_vfscanf") return dvm_vfscanf;
    if(event_name == "dvm_clearerr")return dvm_clearerr;
    if(event_name == "dvm_feof") return dvm_feof;
    if(event_name == "dvm_ferror") return dvm_ferror;
    if(event_name == "dvm_fflush") return dvm_fflush;
    if(event_name == "dvm_fgetc") return dvm_fgetc;
    if(event_name == "dvm_fgetpos") return dvm_fgetpos;
    if(event_name == "dvm_fgets") return dvm_fgets;
    if(event_name == "dvm_fputc") return dvm_fputc;
    if(event_name == "dvm_fputs") return dvm_fputs;
    if(event_name == "dvm_freopen") return dvm_freopen;
    if(event_name == "dvm_fseek") return dvm_fseek;
    if(event_name == "dvm_fsetpos") return dvm_fsetpos;
    if(event_name == "dvm_ftell") return dvm_ftell;
    if(event_name == "dvm_getc") return dvm_getc;
    if(event_name == "dvm_getchar") return dvm_getchar;
    if(event_name == "dvm_gets") return dvm_gets;
    if(event_name == "dvm_putc") return dvm_putc;
    if(event_name == "dvm_putchar") return dvm_putchar;
    if(event_name == "dvm_puts") return dvm_puts;
    if(event_name == "dvm_rewind") return dvm_rewind;
    if(event_name == "dvm_setbuf") return dvm_setbuf;
    if(event_name == "dvm_setvbuf") return dvm_setvbuf;
    if(event_name == "dvm_tmpfile") return dvm_tmpfile;
    if(event_name == "dvm_ungetc") return dvm_ungetc;
    if(event_name == "dvm_void_fprintf") return dvm_void_fprintf;
    if(event_name == "dvm_fprintf") return dvm_fprintf;
    if(event_name == "dvm_void_printf") return dvm_void_printf;
    if(event_name == "dvm_printf") return dvm_printf;
    if(event_name == "dvm_void_vprintf") return dvm_void_vprintf;
    if(event_name == "dvm_vprintf") return dvm_vprintf;
    if(event_name == "dvm_remove") return dvm_remove;
    if(event_name == "dvm_rename") return dvm_rename;
    if(event_name == "dvm_tmpnam") return dvm_tmpnam;
    if(event_name == "dvm_close") return dvm_close;
    if(event_name == "dvm_fstat") return dvm_fstat;
    if(event_name == "dvm_lseek") return dvm_lseek;
    if(event_name == "dvm_open") return dvm_open;
    if(event_name == "dvm_read") return dvm_read;
    if(event_name == "dvm_write") return dvm_write;
    if(event_name == "dvm_access") return dvm_access;
    if(event_name == "dvm_stat") return dvm_stat;
    if(event_name == "mps_Bcast") return mps_Bcast;
    if(event_name == "mps_Barrier") return mps_Barrier;
    if(event_name == "dvm_dfread") return dvm_dfread;
    if(event_name == "dvm_dfwrite") return dvm_dfwrite;
    if(event_name == "crtshg_") return crtshg_;
    if(event_name == "inssh_") return inssh_;
    if(event_name == "insshd_") return insshd_;
    if(event_name == "incsh_") return incsh_;
    if(event_name == "incshd_") return incshd_;
    if(event_name == "strtsh_") return strtsh_;
    if(event_name == "waitsh_") return waitsh_;
    if(event_name == "sendsh_") return sendsh_;
    if(event_name == "recvsh_") return recvsh_;
    if(event_name == "delshg_") return delshg_;
    if(event_name == "getind_") return getind_;
    if(event_name == "addhdr_") return addhdr_;
    if(event_name == "delhdr_") return delhdr_;
    if(event_name == "troff_") return troff_;
    if(event_name == "biof_") return biof_;
    if(event_name == "eiof_") return eiof_;

    if(event_name == "crtps_") return crtps_;
    if(event_name == "psview_") return psview_;
    if(event_name == "delps_") return crtps_;
    if(event_name == "setelw_") return setelw_;

    if(event_name == "dprstv_") return dprstv_;
    if(event_name == "dstv_") return dstv_;
    if(event_name == "dldv_") return dldv_;
    if(event_name == "dbegpl_") return dbegpl_;
    if(event_name == "dbegsl_") return dbegsl_;
    if(event_name == "dendl_") return dendl_;
    if(event_name == "diter_") return diter_;
    if(event_name == "drmbuf_") return drmbuf_;
    if(event_name == "dskpbl_") return dskpbl_;

    if(event_name == "binter_") return binter_;
    if(event_name == "einter_") return einter_;
    if(event_name == "bsloop_") return bsloop_;
    if(event_name == "bploop_") return bploop_;
    if(event_name == "enloop_") return enloop_;

    if(event_name == "dvm_exit") { return Event_dvm_exit;}

    if(event_name == "crtrbl_") return crtrbl_;
    if(event_name == "crtrbp_") return crtrbp_;
    if(event_name == "loadrb_") return loadrb_;
    if(event_name == "waitrb_") return waitrb_;
    if(event_name == "crtbg_") return crtbg_;
    if(event_name == "insrb_") return insrb_;
    if(event_name == "loadbg_") return loadbg_;
    if(event_name == "waitbg_") return waitbg_;


   return Unknown_Func;
  }

//------------------------------------------------------------------------------
#ifdef P_DEBUG
static std::string IDToEventName(const Event& event_id)
  { 
    if(event_id == delrg_) return "delrg_";
    if(event_id == insred_) return "insred_";
    if(event_id == arrcpy_) return "arrcpy_";
    if(event_id == aarrcp_) return "aarrcp_";
    if(event_id == waitcp_) return "waitcp_";
    if(event_id == crtda_) return "crtda_";
    if(event_id == getam_) return "getam_";
    if(event_id == mapam_) return "mapam_";
    if(event_id == runam_) return "runam_";
    if(event_id == stopam_) return "stopam_";
    if(event_id == getamv_) return "getamv_";
    if(event_id == getamr_) return "getamr_";
    if(event_id == crtamv_) return "crtamv_";
    if(event_id == runam_) return "runam_";
    if(event_id == align_) return "align_";
    if(event_id == getps_) return "getps_";
    if(event_id == saverv_) return "saverv_";
    if(event_id == tstelm_) return "tstelm_";
    if(event_id == rwelm_) return "rwelm_";
    if(event_id == rlocel_) return "rlocel_";
    if(event_id == delda_) return "delda_";
    if(event_id == delobj_) return "delobj_";
    if(event_id == copelm_) return "copelm_";
    if(event_id == elmcpy_) return "elmcpy_";
    if(event_id == wlocel_) return "wlocel_";
    if(event_id == clocel_) return "clocel_";
    if(event_id == getlen_) return "getlen_";
    if(event_id == dvm_fopen) return "dvm_fopen";
    if(event_id == dvm_fclose) return "dvm_fclose";
    if(event_id == dvm_void_vfprintf) return "dvm_void_vfprintf";
    if(event_id == dvm_vfprintf) return "dvm_vfprintf";
    if(event_id == dvm_fwrite) return "dvm_fwrite";
    if(event_id == dvm_fread) return "dvm_fread";
    if(event_id == tron_) return "tron_";
    if(event_id == delamv_) return "delamv_";
    if(event_id == distr_) return "distr_";
    if(event_id == crtred_) return "crtred_";
    if(event_id == delred_) return "delred_";
    if(event_id == begbl_) return "begbl_";
    if(event_id == endbl_) return "endbl_";
    if(event_id == crtpl_) return "crtpl_";
    if(event_id == mappl_) return "mappl_";
    if(event_id == endpl_) return "endpl_";
    if(event_id == locind_) return "locind_";
    if(event_id == tstda_) return "tstda_";
    if(event_id == srmem_) return "srmem_";
    if(event_id == tstio_) return "tstio_";
    if(event_id == getrnk_) return "getrnk_";
    if(event_id == getsiz_) return "getsiz_";
    if(event_id == dvm_vscanf) return "dvm_vscanf";
    if(event_id == realn_) return "realn_";
    if(event_id == redis_) return "redis_";
    if(event_id == arrmap_) return "arrmap_";
    if(event_id == setpsw_) return "setpsw_";
    if(event_id == setind_) return "setind_";
    if(event_id == locsiz_) return "locsiz_";
    if(event_id == imlast_) return "imlast_";
    if(event_id == malign_) return "malign_";
    if(event_id == crtrg_) return "crtrg_";
    if(event_id == mrealn_) return "mrealn_";
    if(event_id == strtrd_) return "strtrd_";
    if(event_id == waitrd_) return "waitrd_";
    if(event_id == amvmap_) return "amvmap_";
    if(event_id == exfrst_) return "exfrst_";
	if(event_id == across_) return "across_";
    if(event_id == dopl_)	return "dopl_";
    if(event_id == mdistr_) return "mdistr_";
    if(event_id == mredis_) return "mredis_";
    if(event_id == delarm_) return "delarm_";
    if(event_id == delmvm_) return "delmvm_";
    if(event_id == dvm_Init) return "dvm_Init";
    if(event_id == dvm_fscanf) return "dvm_fscanf";
    if(event_id == dvm_scanf) return "dvm_scanf";
    if(event_id == dvm_vfscanf) return "dvm_vfscanf";
    if(event_id == dvm_clearerr) return "dvm_clearerr";
    if(event_id == dvm_feof) return "dvm_feof";
    if(event_id == dvm_ferror) return "dvm_ferror";
    if(event_id == dvm_fflush) return "dvm_fflush";
    if(event_id == dvm_fgetc) return "dvm_fgetc";
    if(event_id == dvm_fgetpos) return "dvm_fgetpos";
    if(event_id == dvm_fgets) return "dvm_fgets";
    if(event_id == dvm_fputc) return "dvm_fputc";
    if(event_id == dvm_fputs) return "dvm_fputs";
    if(event_id == dvm_freopen) return "dvm_freopen";
    if(event_id == dvm_fseek) return "dvm_fseek";
    if(event_id == dvm_fsetpos) return "dvm_fsetpos";
    if(event_id == dvm_ftell) return "dvm_ftell";
    if(event_id == dvm_getc) return "dvm_getc";
    if(event_id == dvm_getchar) return "dvm_getchar";
    if(event_id == dvm_gets) return "dvm_gets";
    if(event_id == dvm_putc) return "dvm_putc";
    if(event_id == dvm_putchar) return "dvm_putchar";
    if(event_id == dvm_puts) return "dvm_puts";
    if(event_id == dvm_rewind) return "dvm_rewind";
    if(event_id == dvm_setbuf) return "dvm_setbuf";
    if(event_id == dvm_setvbuf) return "dvm_setvbuf";
    if(event_id == dvm_tmpfile) return "dvm_tmpfile";
    if(event_id == dvm_ungetc) return "dvm_ungetc";
    if(event_id == dvm_void_fprintf) return "dvm_void_fprintf";
    if(event_id == dvm_fprintf) return "dvm_fprintf";
    if(event_id == dvm_void_printf) return "dvm_void_printf";
    if(event_id == dvm_printf) return "dvm_printf";
    if(event_id == dvm_void_vprintf) return "dvm_void_vprintf";
    if(event_id == dvm_vprintf) return "dvm_vprintf";
    if(event_id == dvm_remove) return "dvm_remove";
    if(event_id == dvm_rename) return "dvm_rename";
    if(event_id == dvm_tmpnam) return "dvm_tmpnam";
    if(event_id == dvm_close) return "dvm_close";
    if(event_id == dvm_fstat) return "dvm_fstat";
    if(event_id == dvm_lseek) return "dvm_lseek";
    if(event_id == dvm_open) return "dvm_open";
    if(event_id == dvm_read) return "dvm_read";
    if(event_id == dvm_write) return "dvm_write";
    if(event_id == dvm_access) return "dvm_access";
    if(event_id == dvm_stat) return "dvm_stat";
    if(event_id == mps_Bcast) return "mps_Bcast";
    if(event_id == mps_Barrier) return "mps_Barrier";
    if(event_id == dvm_dfread) return "dvm_dfread";
    if(event_id == dvm_dfwrite) return "dvm_dfwrite";
    if(event_id == crtshg_) return "crtshg_";
    if(event_id == inssh_) return "inssh_";
    if(event_id == insshd_) return "insshd_";
    if(event_id == incsh_) return "incsh_";
    if(event_id == incshd_) return "incshd_";
    if(event_id == strtsh_) return "strtsh_";
    if(event_id == waitsh_) return "waitsh_";
    if(event_id == recvsh_) return "recvsh_";
    if(event_id == sendsh_) return "sendsh_";
    if(event_id == delshg_) return "delshg_";
    if(event_id == getind_) return "getind_";
    if(event_id == addhdr_) return "addhdr_";
    if(event_id == delhdr_) return "delhdr_";
    if(event_id == troff_) return "troff_";
    if(event_id == biof_) return "biof_";
    if(event_id == eiof_) return "eiof_";

    if(event_id == crtps_) return "crtps_";
    if(event_id == psview_) return "psview_";
    if(event_id == delps_) return "delps_";
    if(event_id == setelw_) return "setelw_";

    if(event_id == dprstv_) return "dprstv_";
    if(event_id == dstv_) return "dstv_";
    if(event_id == dldv_) return "dldv_";
    if(event_id == dbegpl_) return "dbegpl_";
    if(event_id == dbegsl_) return "dbegsl_";
    if(event_id == dendl_) return "dendl_";
    if(event_id == diter_) return "diter_";
    if(event_id == drmbuf_) return "drmbuf_";
    if(event_id == dskpbl_) return "dskpbl_";

    if(event_id == binter_) return "binter_";
    if(event_id == einter_) return "einter_";
    if(event_id == bsloop_) return "bsloop_";
    if(event_id == bploop_) return "bploop_";
    if(event_id == enloop_) return "enloop_";

    if(event_id == Event_dvm_exit) return "dvm_exit";

	if(event_id == crtrbl_) return "crtrbl_";
	if(event_id == crtrbp_) return "crtrbp_";
    if(event_id == loadrb_) return "loadrb_";
    if(event_id == waitrb_) return "waitrb_";
    if(event_id == crtbg_) return "crtbg_";
    if(event_id == insrb_) return "insrb_";
    if(event_id == loadbg_) return "loadbg_";
    if(event_id == waitbg_) return "waitbg_";


    return "Unknown_Func";
}

ostream& operator << (ostream& os, const Event& e)
{
	os << ' ' << IDToEventName(e) << ' '; 
	return os;
}

#endif

FuncType GetFuncType(Event func_id)
{  
    switch (func_id) {
		case Root_func:
			return __RootFunc;
		case crtda_ :
        case align_ :
        case delda_ :
        case realn_ :
        case arrcpy_ :
        case aarrcp_ :
        case waitcp_ :
			return __DArrayFunc;

        case binter_ :
        case bsloop_ :
        case bploop_ :
        case einter_ :
        case enloop_ :
			return __IntervalFunc;

        case crtshg_ :
        case inssh_ :
        case insshd_ :
        case incsh_ :
        case incshd_ :
        case delshg_ :
        case strtsh_ :
        case waitsh_ :
		case sendsh_ :
		case recvsh_ :
        case imlast_ :
        case exfrst_ :
		case across_ :
			return __ShadowFunc;

        case crtrg_ :
        case crtred_ :
        case insred_ :
        case delred_ :
        case delrg_ :
        case strtrd_ :
        case waitrd_ :
			return __ReductFunc;

        case crtpl_ :
        case mappl_ :
        case dopl_ :
        case endpl_ :
			return __ParLoopFunc;

		case crtps_ :
		case delps_ :
		case setelw_ :
		case psview_ :
        case getps_ :
        case getam_ :
        case mapam_ :
        case runam_ :
        case stopam_ :
        case getamr_ :
        case crtamv_ :
        case delamv_ :
        case blkdiv_ :
        case distr_ :
        case redis_ :
          return __MPS_AMFunc;

        case dvm_rewind :
        case dvm_tmpfile :
        case dvm_ungetc :
        case dvm_setbuf :
        case dvm_setvbuf :
        case dvm_remove :
        case dvm_rename :
        case dvm_tmpnam :
        case dvm_close :
        case dvm_fstat :
        case dvm_lseek :
        case dvm_access :
        case dvm_stat :
        case dvm_clearerr :
        case dvm_ferror :
        case dvm_fgetpos :
        case dvm_ftell :
        case dvm_getc :
        case dvm_open :
        case dvm_read :
        case dvm_write :
        case dvm_fflush :
        case dvm_fgetc :
        case dvm_feof :
        case dvm_getchar :
        case dvm_gets :
        case dvm_putc :
        case dvm_putchar :
        case dvm_puts :
        case dvm_vscanf :
        case dvm_fopen :
        case dvm_fclose :
        case dvm_void_vfprintf :
        case dvm_vfprintf :
        case dvm_fwrite :
        case dvm_fread :
        case dvm_fgets :
        case dvm_fputc :
        case dvm_fputs :
        case dvm_freopen :
        case dvm_fseek :
        case dvm_fsetpos :
        case dvm_fscanf :
        case dvm_scanf :
        case dvm_void_fprintf :
        case dvm_fprintf :
        case dvm_void_printf :
        case dvm_printf :
        case dvm_void_vprintf :
        case dvm_vprintf :
        case dvm_vfscanf :
        case biof_ :
        case eiof_ :
        case srmem_ :
			return __IOFunc;

        case tstio_ :
        case tstda_ :
        case locsiz_ :
        case getlen_ :
        case delobj_ :
        case tron_ :
        case troff_ :
        case clocel_ :
        case locind_ :
        case rlocel_ :
        case wlocel_ :
        case tstelm_ :
        case getrnk_ :
        case getsiz_ :
        case delmvm_ :
        case mdistr_ :
        case mredis_ :
        case amvmap_ :
        case setpsw_ :
        case malign_ :
        case mrealn_ :
        case delarm_ :
        case arrmap_ :
        case rwelm_ :
        case copelm_ :
        case elmcpy_ :
        case setind_ :
        case dvm_dfread :
        case dvm_dfwrite :
        case getind_ :
        case addhdr_ :
        case delhdr_ :
        case dprstv_ :
        case dstv_ :
        case dldv_ :
        case dbegpl_ :
        case dbegsl_ :
        case dendl_ :
        case diter_ :
        case drmbuf_ :
        case dskpbl_ :
        case begbl_ :
        case endbl_ :
        case getamv_ :
			return __RegularFunc;

        case crtrbl_ :
        case crtrbp_ :
        case loadrb_ :
        case waitrb_ :
        case crtbg_ :
        case insrb_ :
        case loadbg_ :
        case waitbg_ :
			return __RemAccessFunc;


        default : 
			return __UnknownFunc;
	}
}

#ifdef P_DEBUG
static std::string GetFuncTypeName(const FuncType ft)
{
	switch(ft) {
		case	__RootFunc:		return "__RootFunc";
		case	__DArrayFunc:	return "__DArrayFunc";
		case	__IntervalFunc:	return "__IntervalFunc";
		case	__IOFunc:		return "__IOFunc";
		case	__MPS_AMFunc:	return "__MPS_AMFunc";
		case	__ParLoopFunc:	return "__ParLoopFunc";
		case	__ReductFunc:	return "__ReductFunc";
		case	__RegularFunc:	return "__RegularFunc";
		case	__ShadowFunc:	return "__ShadowFunc";
		case	__RemAccessFunc:return "__RemAccessFunc";
		case	__UnknownFunc:	return "__UnknownFunc";
		default:				return "__UnknownFunc";
	}
}

std::ostream& operator << (std::ostream& os, const FuncType& ft)
{
	os << ' ' << GetFuncTypeName(ft) << ' '; 
	return os;
}

#endif


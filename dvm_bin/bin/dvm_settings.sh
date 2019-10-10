#!/bin/sh

export DVMVERS='5.0'
export PLATFORM='osx-10.11-clang'
export dvmbuild_real='Version 5.0, revision 6603, platform osx-10.11-clang, build 3'

if [ -z "$optcconv" ]; then export optcconv='-e2 -noCuda'; fi
if [ -z "$optfconv" ]; then export optfconv='-e2 -noCuda'; fi

if [ -z "$PCC" ]; then export PCC='mpicc -g -O2'; fi
if [ -z "$PCXX" ]; then export PCXX='mpicxx -g -O2'; fi
if [ -z "$PFORT" ]; then export PFORT='mpif77 -g -O2'; fi
unset MPIR_HOME
unset OMPI_CC
unset OMPI_CXX
unset OMPI_F77
unset OMPI_FC
unset CUDA_PATH
unset PGI_PATH
unset CLANG

export PLINKER='mpicxx -g'
export PFLINKER='mpif77 -g'
export Pld='ld'
unset CUDA_LIB
export ADD_LIBS="$USER_LIBS -lc++ -lm -lstdc++"
unset ADD_LIB_PATHS
if [ -z "$I_MPI_LINK" ]; then export I_MPI_LINK='opt_mt'; fi

unset MPIRUN_CMD
if [ -z "$dvmwait" ]; then export dvmwait=0; fi

if [ -z "$dvmsave" ]; then export dvmsave=0; fi
if [ -z "$dvmshow" ]; then export dvmshow=0; fi

if [ -z "$dvmrun" ]; then export dvmrun='runmpi'; fi
export OPTIONCHAR='-'
if [ -z "$dvmpar" ]; then export dvmpar="$dvmdir/par/.rel"; fi
if [ -z "$dvmout" ]; then export dvmout='off'; fi
if [ -z "$Pred_sys" ]; then export Pred_sys="$dvmdir/par/Predictor.par"; fi
if [ -z "$Pred_vis" ]; then export Pred_vis='firefox'; fi
if [ -z "$Doc_vis" ]; then export Doc_vis='firefox'; fi

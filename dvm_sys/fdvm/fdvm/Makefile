#echo#######################################################################
# Makefile for   Fortran DVM back-end
#
#echo#######################################################################
SAGEROOT =../Sage
CONFIG_ARCH=iris4d
LIBDIR = ../libsage
#LIBDIR = $(SAGEROOT)/lib/$(CONFIG_ARCH)
#LIBDIR1 =/usr/people/podd/oldsrc
LIBDIR1 = $(LIBDIR)
LIBINCLUDE = $(SAGEROOT)/lib/include
HINCLUDE = $(SAGEROOT)/h
DVMINCLUDE = ../include
INSTALLDEST = ../bin
INSTALL = /bin/cp


#HP-ALLOCA#LDLIBS  = -lPW#ENDIF#
#HP_CFLAGS#CEXTRA = -Aa#ENDIF#

CC = gcc
#USE_CC#CC=cc#ENDIF#

#CXX = DCC
CXX = g++
#USE_CFRONT#CXX=CC#ENDIF#

LOADER = $(CXX)

INCLUDE = -I. -I$(LIBINCLUDE) -I$(HINCLUDE) -I$(DVMINCLUDE)

#CFLAGS	= $(INCLUDE) -Wall -c # $(CEXTRA)
CFLAGS	= $(INCLUDE) -Wall -g -c # $(CEXTRA)
LDFLAGS	= 

LIBS = $(LIBDIR)/libSage++.a $(LIBDIR)/libsage.a $(LIBDIR)/libdb.a 
DVM = f_dvm
OBGS = dvm.o funcall.o stmt.o io.o help.o debug.o hpf.o omp.o ompdebug.o acc.o acc_analyzer.o acc_across_analyzer.o calls.o acc_f2c.o acc_f2c_handlers.o acc_across.o aks_structs.o aks_analyzeLoops.o acc_data.o acc_rtc.o acc_utilities.o parloop.o
# ***********************************************************
f: DVM

install: $(INSTALLDEST)/DVM

DVM: $(OBGS) 
	$(LOADER) $(LDFLAGS) $(OBGS)  $(LIBS) -o $(DVM)

acc.o: acc.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) acc.cpp
	
acc_across.o: acc_across.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h $(DVMINCLUDE)/aks_structs.h
	$(CXX) $(CFLAGS) acc_across.cpp

acc_across_analyzer.o: acc_across_analyzer.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h $(DVMINCLUDE)/acc_across_analyzer.h
	$(CXX) $(CFLAGS) acc_across_analyzer.cpp

acc_analyzer.o: acc_analyzer.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h $(DVMINCLUDE)/acc_analyzer.h
	$(CXX) $(CFLAGS) acc_analyzer.cpp

acc_data.o: acc_data.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) acc_data.cpp

acc_f2c.o: acc_f2c.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) acc_f2c.cpp

acc_f2c_handlers.o: acc_f2c_handlers.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) acc_f2c_handlers.cpp

acc_rtc.o: acc_rtc.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) acc_rtc.cpp
		
acc_utilities.o: acc_utilities.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) acc_utilities.cpp
	
aks_analyzeLoops.o: aks_analyzeLoops.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h $(DVMINCLUDE)/aks_structs.h
	$(CXX) $(CFLAGS) aks_analyzeLoops.cpp	
	
aks_structs.o: aks_structs.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h $(DVMINCLUDE)/aks_structs.h
	$(CXX) $(CFLAGS) aks_structs.cpp

calls.o: calls.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) calls.cpp
	
debug.o: debug.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) debug.cpp
	
dvm.o: dvm.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) dvm.cpp
	
funcall.o: funcall.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) funcall.cpp

help.o: help.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) help.cpp

hpf.o: hpf.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) hpf.cpp

io.o: io.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) io.cpp
	
omp.o: omp.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) omp.cpp

ompdebug.o: ompdebug.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) ompdebug.cpp

parloop.o: parloop.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) parloop.cpp

stmt.o: stmt.cpp $(DVMINCLUDE)/fdvm.h $(DVMINCLUDE)/libnum.h $(DVMINCLUDE)/libdvm.h \
	$(DVMINCLUDE)/dvm.h
	$(CXX) $(CFLAGS) stmt.cpp



$(INSTALLDEST)/DVM: DVM
		@echo Installing $(DVM) in $(INSTALLDEST)
		if [ -d $(INSTALLDEST) ] ; then true; \
                   else mkdir $(INSTALLDEST) ;fi
		$(INSTALL) $(DVM) $(INSTALLDEST)
test: tdvm.o

tdvm.o: tdvm.cpp
	$(CXX) -g -c tdvm.cpp 

clean:
	/bin/rm -f *.o *.dep $(DVM)

cleaninstall:
	/bin/rm -f *.o $(DVM)




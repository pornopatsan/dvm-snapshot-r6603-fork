#######################################################################
##                  pC++/Sage++  Copyright (C) 1993                  ##
##  Indiana University  University of Oregon  University of Rennes   ##
#######################################################################

# sage/Sage++/Makefile (PHB)

SHELL = /bin/sh
CONFIG_ARCH=iris4d

RANLIB_TEST = [ -f /usr/bin/ranlib ] || [ -f /bin/ranlib ]
#NO_RANLIB#RANLIB_TEST = (exit 1)#ENDIF#

# Shared library hack for HP-UX
LSX = .a
#HP_CFLAGS#CEXTRA = -Aa +z#ENDIF#
#HP_CFLAGS#LSX = .sl#ENDIF#

PCXX = ../bin/$(CONFIG_ARCH)/pc++

CC = gcc
#CC=cc

CXX     = #CC
#USE_CFRONT#CXX= CC#ENDIF#
#USE_DECCXX#CXX=cxx#ENDIF#
#USE_IBMXLC#CXX=xlC#ENDIF#
CXX=DCC#ENDIF##USE_SGIDCC#
CXX=g++
LOADER = $(CXX)
#INSTALLDEST = ../lib/$(CONFIG_ARCH)
INSTALLDEST = ../../libsage
INSTALL = /bin/cp
HDRS        = ../h
LIBINCLUDE  = ../lib/include
SAGEINCLUDE = -I$(HDRS) -I$(LIBINCLUDE)

# Directory in which include files can be found
INCLUDEDIR = ./h
INCLUDE = -I$(INCLUDEDIR) $(SAGEINCLUDE)

# -w don't issue warning now.
CFLAGS	= $(INCLUDE) -g -Wall -c $(CEXTRA)
LDFLAGS	= 
#BISON= /usr/freeware/bin/bison
BISON= bison
TOOLSage++_SRC	=  libSage++.cpp

TOOLSage++_HDR     = $(LIBINCLUDE)/macro.h $(LIBINCLUDE)/bif_node.def  $(LIBINCLUDE)/type.def  $(LIBINCLUDE)/symb.def $(LIBINCLUDE)/libSage++.h

TOOLSage++_OBJ	=   libSage++.o 

SUBDIR1 = extentions
SUBDIR  = ${SUBDIR1}

#all: $(TOOLSage++_OBJ) $(TOOLSage++_HDR)
#	@for i in ${SUBDIR1}; do (echo "*** COMPILING $$i DIRECTORY"; cd $$i;\
#	  $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" $@); done

libSage++.a: libSage++.o  $(TOOLSage++_HDR)
		/bin/rm -f libSage++.a
		ar qc libSage++.a libSage++.o
		@if $(RANLIB_TEST) ; then ranlib libSage++.a ; \
	           else echo "\tNOTE: ranlib not required" ; fi
libSage++.o: libSage++.cpp $(TOOLSage++_HDR)
		$(CXX) $(CFLAGS)  libSage++.cpp

libSage++.dep: libSage++.cpp $(TOOLSage++_HDR)
		$(PCXX) -deponly $(INCLUDE) libSage++.cpp -o libSage++.o

libSage++ : libSage++$(LSX)

clean:
	/bin/rm -f libSage++$(LSX) libSage++.dep libSage++.proj
	/bin/rm -f $(TOOLSage++_OBJ)
	/bin/rm -f extentions/sgCallGraph.o
	/bin/rm -f extentions/sgClassHierarchy.o

cleaninstall: clean

install:$(INSTALLDEST)/libSage++.a

#	@for i in ${SUBDIR1}; do (echo "*** COMPILING $$i DIRECTORY"; cd $$i;\
#	  $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" $@); done

$(INSTALLDEST)/libSage++.a: libSage++.a
		if [ -d $(INSTALLDEST) ] ; then true; \
                    else mkdir $(INSTALLDEST) ;fi
		$(INSTALL) libSage++.a $(INSTALLDEST)
		@if $(RANLIB_TEST) ; then ranlib $(INSTALLDEST)/libSage++.a ; \
	           else echo "\tNOTE: ranlib not required" ; fi

${SUBDIR}: FRC
	cd $@; $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" all
 
FRC:


#######################################################################
##                  pC++/Sage++  Copyright (C) 1993                  ##
##  Indiana University  University of Oregon  University of Rennes   ##
#######################################################################


# sage/Makefile (phb)

# Pete Beckman  (5/27/93)

#
# This makefile recursively calls MAKE in each subdirectory
#
# There are two configurations for this Makefile at the present time
# 1) Users/Developers of the Sage++ Compiler tools
# 2) Users/Developers of pC++, a Parallel C++ for Supercomputers
#

SHELL   = /bin/sh

CONFIG_ARCH=iris4d

CC	= gcc
#CC=cc#ENDIF##USE_CC#
#PTX#CC=cc#ENDIF#

CXX     = g++
#USE_CFRONT#CXX= CC#ENDIF#
#USE_DECCXX#CXX=cxx#ENDIF#
#USE_IBMXLC#CXX=xlC#ENDIF#
CXX=DCC#ENDIF##USE_SGIDCC#
CXX     = g++
LINKER	= $(CC)

#PTX#EXTRASRC=target/symmetry/src#ENDIF#
#SYMMETRY#EXTRASRC=target/symmetry/src#ENDIF#
#CM5#EXTRASRC=target/cm5/src#ENDIF#
#PARAGON#EXTRASRC=target/paragon/src#ENDIF#
#PARAGON_XDEV#EXTRASRC=target/paragon/src#ENDIF#
#KSR#EXTRASRC=target/ksr1/src#ENDIF#
#SP1#EXTRASRC=target/sp1/src#ENDIF#
#CS2#EXTRASRC=target/cs2/src#ENDIF#
EXTRASRC=target/sgimp/src#ENDIF##SGIMP#

# instr temporarily removed until libSage++ stable

# Several types of configurations....

# tools EVERYONE needs
BASIC = lib  Sage++

# Other Compiler Tools
SAGEXX = f2dep#ENDIF##SAGEXX#

# pC++ system
#PVM_INSTALLED#PVMTEMP=target/pvm/src#ENDIF#
TEMP = breezy instr dep2C++ target/uniproc/src $(PVMTEMP)
#PCXX#PCXX = $(TEMP) $(EXTRASRC) TestSuite#ENDIF#

# What to compile
SUBDIR1 = $(BASIC) 

# Subdirectories to make resursively
SUBDIR  = ${SUBDIR1}

all:
	@echo "*********** RECURSIVELY MAKING SUBDIRECTORIES ***********"
	@for i in ${SUBDIR1}; do (echo "*** COMPILING $$i DIRECTORY"; cd $$i;\
             $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" ); done
	@echo "***************** DONE ************************
#	@echo "To run the TestSuite code (in uniprocessor mode) type:"
#	@echo "cd TestSuite; make test"

clean:
	for i in ${SUBDIR1} Sage++; do (cd $$i; $(MAKE) "MAKE=$(MAKE)" clean); done

cleandist:	clean cleangood
cleaninstall:	clean cleangood
cleangood:
	@echo "Deleting *~ #* core *.a *.sl *.o *.dep"
	@find . \( -name \*~ -o -name \#\* -o -name core \) \
	   -exec /bin/rm {} \; -print
	@find . \( -name \*.a -o -name \*.sl -o -name \*.o -o -name \*.dep \) \
	   -exec /bin/rm {} \; -print
	@if [ ! -d bin/$(CONFIG_ARCH) ] ; then true; \
	      else /bin/rm -r bin/$(CONFIG_ARCH) ; fi
	@if [ ! -d lib/$(CONFIG_ARCH) ] ; then true; \
	      else /bin/rm -r lib/$(CONFIG_ARCH) ; fi
	@if [ ! -d target/pvm/lib ] ; then true; \
	      else /bin/rm -r target/pvm/lib ; fi

install:
	@echo "*********** RECURSIVELY MAKING SUBDIRECTORIES ***********"
	@for i in ${SUBDIR1}; do (echo "*** COMPILING $$i DIRECTORY"; cd $$i;\
             $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" install); done
	@echo "***************** DONE ************************"
#	@echo "To run the TestSuite code (in uniprocessor mode) type:"
#	@echo "cd TestSuite; make test"

.RECURSIVE: ${SUBDIR1}

${SUBDIR}: FRC
	cd $@; $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" all

FRC:


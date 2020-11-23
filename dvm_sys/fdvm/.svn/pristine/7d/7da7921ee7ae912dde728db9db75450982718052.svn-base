#######################################################################
##                  pC++/Sage++  Copyright (C) 1993                  ##
##  Indiana University  University of Oregon  University of Rennes   ##
#######################################################################


# sage/lib/Makefile (phb)

SHELL = /bin/sh
INSTALL = /bin/cp

# Flags passed down to Makefiles in subdirectories
MFLAGS	= 

CC = gcc
#CC=cc#ENDIF##USE_CC#

CXX = g++
CXX = /usr/WorkShop/usr/bin/DCC
LINKER = $(CC)

NOP = echo
#C90#EXTRAOBJ=alloca-c90.o#ENDIF#
#C90#NOP = @/bin/rm -f alloca-c90.o#ENDIF#

SUBDIR1	= oldsrc newsrc
# Subdirectories to make resursively
SUBDIR	=  ${SUBDIR1}

all:	${SUBDIR} $(EXTRAOBJ)

clean:	
	$(NOP)
	for i in ${SUBDIR1}; do (cd $$i; $(MAKE) "MAKE=$(MAKE)" clean); done

install: FRC $(EXTRAOBJ)
	@for i in ${SUBDIR1}; do (cd $$i; \
	     echo "   *** COMPILING $$i DIRECTORY";\
             $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" install); done

# If you are on a C90, you will need the gnu alloca()
alloca-c90.o:	alloca-c90.c
	$(CC) -c alloca-c90.c
	if [ -d c90 ] ; then true; \
	  else mkdir c90 ;fi
	$(INSTALL) alloca-c90.o c90

.RECURSIVE: ${SUBDIR}

${SUBDIR}:	FRC
	@echo "   *** COMPILING $@ DIRECTORY"; cd $@; \
          $(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "CXX=$(CXX)" "LINKER=$(LINKER)" all

FRC:


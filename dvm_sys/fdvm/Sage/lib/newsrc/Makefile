#######################################################################
##                  pC++/Sage++  Copyright (C) 1993                  ##
##  Indiana University  University of Oregon  University of Rennes   ##
#######################################################################


# sage/lib/newsrc/Makefile (phb)

LSX = .a

#HP_CFLAGS#CEXTRA = -Aa +z#ENDIF#
#HP_CFLAGS#LSX = .sl#ENDIF#

SHELL = /bin/sh
CONFIG_ARCH=iris4d

# ALPHA Sage new lib.a   modified by Pete Beckman (2/1/93)

RANLIB_TEST = [ -f /usr/bin/ranlib ] || [ -f /bin/ranlib ]
#NO_RANLIB#RANLIB_TEST = (exit 1)#ENDIF#

CC = gcc
#CC=cc#ENDIF##USE_CC#

CXX = g++
CXX = /usr/WorkShop/usr/bin/DCC
OLDHEADERS = ../../h

#INSTALLDEST = ../$(CONFIG_ARCH)
INSTALLDEST = ../../../libsage
INSTALL = /bin/cp

# Directory in which include file can be found
toolbox_include = ../include

INCLUDE = -I$(OLDHEADERS) -I../include
CFLAGS	= $(INCLUDE) -g -Wall $(CEXTRA)
LDFLAGS	= 
BISON= /usr/freeware/bin/bison
BISON= bison
TOOLBOX_SRC	=  comments.c low_level.c unparse.c toolsann.c annotate.tab.c

TOOLBOX_HDR     =  $(toolbox_include)/macro.h $(toolbox_include)/bif_node.def  $(toolbox_include)/type.def  $(toolbox_include)/symb.def 

TOOLBOX_OBJ	=  low_level.o  unparse.o 

TOOLBOX_OBJ_ANN	=  comments.o toolsann.o annotate.tab.o

all: libsage$(LSX) 

clean:
	/bin/rm -f *.o lib*$(LSX)

low_level.o: low_level.c $(TOOLBOX_HDR)

unparse.o:   unparse.c $(TOOLBOX_HDR) $(toolbox_include)/unparse.def $(toolbox_include)/unparseC++.def

main.o : main.c 

libsage : libsage$(LSX)

libsage.a:      $(TOOLBOX_OBJ) $(TOOLBOX_HDR)
		/bin/rm -f libsage.a
		ar qc libsage.a $(TOOLBOX_OBJ)
		@if $(RANLIB_TEST) ; then ranlib libsage.a ; \
	           else echo "\tNOTE: ranlib not required" ; fi

libsage.sl:     $(TOOLBOX_OBJ) $(TOOLBOX_HDR)
		/bin/rm -f libsage.sl
		ld -b -s -o libsage.sl $(TOOLBOX_OBJ)


install: $(INSTALLDEST)/libsage$(LSX) 

$(INSTALLDEST)/libsage$(LSX): libsage$(LSX)
	if [ -d $(INSTALLDEST) ] ; then true; else mkdir $(INSTALLDEST) ;fi
	$(INSTALL) libsage$(LSX) $(INSTALLDEST)/libsage$(LSX)
	@if $(RANLIB_TEST) ; then ranlib $(INSTALLDEST)/libsage$(LSX) ; \
           else echo "\tNOTE: ranlib not required" ; fi





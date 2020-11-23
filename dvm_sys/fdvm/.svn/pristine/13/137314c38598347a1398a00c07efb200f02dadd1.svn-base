#######################################################################
##                  pC++/Sage++  Copyright (C) 1993                  ##
##  Indiana University  University of Oregon  University of Rennes   ##
#######################################################################


# sage/lib/oldsrc/Makefile (phb)

LSX = .a

#HP_CFLAGS#CEXTRA = -Ae +z#ENDIF#
#HP_CFLAGS#LSX = .sl#ENDIF#

SHELL = /bin/sh
CONFIG_ARCH=iris4d

RANLIB_TEST = [ -f /usr/bin/ranlib ] || [ -f /bin/ranlib ]
#NO_RANLIB#RANLIB_TEST = (exit 1)#ENDIF#

# Directory with all the include headers
H = ../../h

#INSTALLDEST = ../$(CONFIG_ARCH)
INSTALLDEST = ../../../libsage
INSTALL = /bin/cp

CC = gcc
#CC=cc#ENDIF##USE_CC#

CXX = g++
CXX = /usr/WorkShop/usr/bin/DCC
LINKER = $(CC)

CFLAGS	= -g -Wall -I$H $(CEXTRA)

DEST	= ${HOME}/bin

EXTHDRS	= $H/bif.h $H/db.h $H/db.h $H/defs.h $H/dep.h \
	  $H/dep_str.h $H/list.h $H/ll.h $H/sets.h $H/symb.h \
	  $H/tag $H/vparse.h

OBJS	= anal_ind.o db.o db_unp.o db_unp_vpc.o  dbutils.o \
	  garb_coll.o glob_anal.o ker_fun.o list.o \
	  make_nodes.o mod_ref.o ndeps.o readnodes.o sets.o setutils.o \
	  symb_alg.o writenodes.o 

SRCS	= anal_ind.c db.c db_unp.c db_unp_vpc.c  dbutils.c \
	  garb_coll.c glob_anal.c ker_fun.c list.c \
	  make_nodes.c mod_ref.c ndeps.c readnodes.c sets.c setutils.c \
	  symb_alg.c writenodes.c


all:		$(OBJS) libdb$(LSX)

libdb.a:	$(OBJS)
		/bin/rm -f libdb.a
		ar qc libdb.a $(OBJS)
		@if $(RANLIB_TEST) ; then ranlib libdb.a ; \
		   else echo "\tNOTE: ranlib not required" ; fi

libdb.sl:	$(OBJS)
		/bin/rm -f libdb.sl
		ld -b -s -o libdb.sl $(OBJS)

clean:
		@/bin/rm -f $(OBJS) $(PROGRAM) *.dep libdb$(LSX)

index:
		ctags -wx $(HDRS) $(SRCS)

print:
		$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); ctags $(HDRS) $(SRCS)

install: $(INSTALLDEST)/libdb$(LSX)

$(INSTALLDEST)/libdb$(LSX): libdb$(LSX)
		if [ -d $(INSTALLDEST) ] ; then true; \
                    else mkdir $(INSTALLDEST) ;fi
		$(INSTALL) libdb$(LSX) $(INSTALLDEST)
		@if $(RANLIB_TEST) ; then ranlib $(INSTALLDEST)/libdb$(LSX) ; \
		   else echo "\tNOTE: ranlib not required" ; fi

###
anal_ind.o: $H/db.h $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
db.o:  $H/db.h $H/defs.h \
	$H/tag $H/bif.h $H/ll.h $H/symb.h $H/sets.h
db_unp.o: $H/db.h  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
db_unp_vpc.o:  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h $H/db.h $H/vparse.h
dbutils.o:  $H/db.h \
	$H/defs.h $H/tag $H/bif.h $H/ll.h $H/symb.h $H/sets.h
garb-coll.o:  $H/db.h $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
glob_anal.o:  $H/db.h $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
ker_fun.o:  $H/defs.h $H/tag $H/bif.h $H/ll.h \
	$H/symb.h $H/sets.h
list.o: $H/db.h  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h $H/list.h
make_nodes.o: $H/db.h  $H/defs.h $H/tag \
	$H/bif.h $H/ll.h $H/symb.h $H/sets.h
mod_ref.o:  $H/defs.h $H/tag $H/bif.h $H/ll.h \
	$H/symb.h $H/sets.h $H/vparse.h $H/db.h
ndeps.o:  $H/db.h $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
readnodes.o: $H/db.h  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h $H/dep_str.h \
	 $H/dep.h
sets.o: $H/db.h  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
setutils.o: $H/db.h  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
symb_alg.o: $H/db.h  $H/defs.h $H/tag $H/bif.h \
	$H/ll.h $H/symb.h $H/sets.h
writenodes.o: $H/db.h $H/defs.h $H/tag \
	$H/bif.h $H/ll.h $H/symb.h $H/sets.h $H/dep_str.h \
	$H/dep.h

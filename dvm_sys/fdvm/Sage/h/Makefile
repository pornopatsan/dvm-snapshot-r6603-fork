#######################################################################
##                  pC++/Sage++  Copyright (C) 1993                  ##
##  Indiana University  University of Oregon  University of Rennes   ##
#######################################################################


CC = gcc
CC = cc
CXX = g++
CXX = DCC

LINKER = $(CC)

all:	tag.h

tag.h:	head tag
	( cat head; \
	  sed < tag \
	    '/#defin/s/\([^ ]*\) \([^ 	]*\)\(.*\)/  tag \[ \2 \] = \"\2\";/')\
	  > tag.h

#######################################################################
##                             Copyright (C) 1999                    ##
##              Keldysh Institute of Appllied Mathematics            ##
#######################################################################

# Predictor/Makefile
#
# This makefile recursively calls MAKE in each subdirectory
#

# What to compile
SUBDIR= Rater Zlib Presage Trcutil

all: 
	@echo "****** RECURSIVELY MAKING SUBDIRECTORIES Predictor/: $(SUBDIR) ******"
	cd Rater/Src; $(MAKE) all
	cd Zlib/Src; $(MAKE) all
	cd Presage/Src;$(MAKE) all
	cd Trcutil; $(MAKE) all
	@echo "****** DONE MAKING dvm/: $(SUBDIR) ******"
debug: 
	@echo "****** RECURSIVELY MAKING SUBDIRECTORIES Predictor/: $(SUBDIR) ******"
	cd Rater/Src; $(MAKE) all
	cd Zlib/Src; $(MAKE) all
	cd Presage/Src; $(MAKE) all
	cd Trcutil; $(MAKE) all
	@echo "****** DONE MAKING dvm/: $(SUBDIR) ******"

release: 
	@echo "****** RECURSIVELY MAKING SUBDIRECTORIES Predictor/: $(SUBDIR) ******"
	cd Rater/Src; $(MAKE) "RELEASE=-DNDEBUG" all
	cd Zlib/Src; $(MAKE) "RELEASE=-DNDEBUG" all
	cd Presage/Src; $(MAKE) "RELEASE=-DNDEBUG" all
	cd Trcutil; $(MAKE) all
	@echo "****** DONE MAKING dvm/: $(SUBDIR) ******"

clean1:
	@echo "****** RECURSIVELY   CLEAN SUBDIRECTORIES Predictor/: $(SUBDIR) ******"
	cd Rater/Src; $(MAKE)  clean
	cd Zlib/Src; $(MAKE)  clean
	cd Presage/Src; $(MAKE) clean
	cd Trcutil; $(MAKE) clean
	@echo "****** DONE  CLEAN Predictor/: $(SUBDIR) ******"


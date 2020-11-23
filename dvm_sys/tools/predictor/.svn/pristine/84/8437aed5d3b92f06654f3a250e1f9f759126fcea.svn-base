.KEEP_STATE:
#############################################################################
#                                                                           #
#                                   RATER                                   #
#                                                                           #
#############################################################################

LIB	    = librater.a

INC_DIR = -I../Include -I../../Presage/Include

COPT    = -g -w $(INC_DIR) -DP_DEBUG
LINK	= ar r 

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) -c $(COPT) $*.cpp

MODULES  = \
	AMView.o		DimBound.o	RedGroup.o \
	AlignAxis.o		DistAxis.o	RedVar.o \
	BGroup.o		Space.o \
	Block.o		Ls.o		Vm.o RemAccessBuf.o \
	CommCost.o		ParLoop.o 	DArray.o \
	intersection.o	LoopBlock.o	LoopLS.o

################################## RATER ##################################

all: $(LIB)

$(LIB): $(MODULES) 
	$(LINK) $(LIB) $(MODULES)
	
clean:
	rm -f *.o $(LIB)

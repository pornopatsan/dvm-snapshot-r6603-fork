BINDIR=../../../testing

EXECUTABLES = st
COPT=-c -I../../Zlib/include
ZLIB	= zlib
ZLIBDIR	= ../../Zlib/Release

OBJS=\
	inter.o  \
	potensyn.o  \
	statread.o  \
	treeinter.o \
	statprintf.o \
    statlist.o \
    statinter.o \
    interface.o \
    stat.o \
    Source.o \

$(BINDIR)/$(EXECUTABLES): $(OBJS)
	clang++ -o $@ $(OBJS) -L$(ZLIBDIR) -l$(ZLIB)

all:  $(BINDIR)/$(EXECUTABLES)
	@echo "*** COMPILING EXECUTABLE $(EXECUTABLES) DONE"

clean:
	rm -f $(OBJS)
cleanall:
	rm -f $(OBJS)

potensyn.o:
	$(CXX) $(COPT) potensyn.cpp
inter.o:
	$(CXX) $(COPT) inter.cpp
statread.o:
	$(CXX) $(COPT) statread.cpp
treeinter.o:
	$(CXX) $(COPT) treeinter.cpp
statprintf.o:
	$(CXX) $(COPT) statprintf.cpp
statinter.o:
	$(CXX) $(COPT) statinter.cpp
statlist.o:
	$(CXX) $(COPT) statlist.cpp
interface.o:
	$(CXX) $(COPT) interface.cpp -fdeclspec
stat.o:
	$(CXX) $(COPT) stat.cpp
Source.o:
	$(CXX) $(COPT) ../../../testing/Source.cpp

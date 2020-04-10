BINDIR=../../../testing

EXECUTABLES = st
CXX=/usr/local/opt/llvm@9/bin/clang++ -g -O2
COPT=-c -I../../Zlib/include -g
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
    integration.o \
    integration.dll \
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
# statprintf.o:
# 	$(CXX) $(COPT) statprintf.cpp
statinter.o:
	$(CXX) $(COPT) -std=c++11 statinter.cpp
statlist.o:
	$(CXX) $(COPT) -std=c++11 statlist.cpp
interface.o:
	$(CXX) $(COPT) -std=c++11 interface.cpp -fdeclspec
stat.o:
	$(CXX) $(COPT) -std=c++11 stat.cpp
Source.o:
	$(CXX) $(COPT) -std=c++11 ../../../testing/Source.cpp -fdeclspec
integration.o:
	$(CXX) $(COPT) -std=c++11 integration.cpp -fdeclspec
integration.dll:
	clang++ -shared -o integration.dll *.o ../../Zlib/*.o

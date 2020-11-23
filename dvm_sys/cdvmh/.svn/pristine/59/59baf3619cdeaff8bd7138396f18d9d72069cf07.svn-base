Get the required tools:
1) Subversion. Source code control program. Get it from:
   http://subversion.apache.org/packages.html
2) CMake. This is used for generating Visual Studio solution and project files. Get it from: 
   http://www.cmake.org/cmake/resources/software.html
3) Visual Studio 2012 or later
4) Python. This is needed only if you will be running the tests (which is essential, if you will be developing for clang). 
   Get it from: http://www.python.org/download/

// all commands through Power Shell 
1) mkdir llvm_3_4
2) <PATH_SVN>/svn export http://llvm.org/svn/llvm-project/llvm/branches/release_34 llvm_3_4  (or run via SVN GUI) 

3) cd llvm_3_4/tools/
   mkdir clang
   cd ../../
   
4) <PATH_SVN>/svn export http://llvm.org/svn/llvm-project/cfe/branches/release_34 llvm_3_4/tools/clang (or run via SVN GUI) 

5) mkdir build_3_4
   cd build_3_4
   
6) cmake -G "Visual Studio 11" ../llvm_3_4 (or run via CmakeGui) [FOR Visual Studio 2012]

7) open LLVM.sln
8) build all projects 

9) cd CDVMH
10) open CDVMH.sln [FOR Visual Studio 2012]
11) build CDVMH converter
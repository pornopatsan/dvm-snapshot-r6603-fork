********************************************************************************
*                                                                              *
*              How to generate Microsoft Visual Studio solution?               *
*                                                                              *
********************************************************************************

It is possible to use CMake to generate Microsoft Visual Studio solution
and projects for CDVMH.

At first, you need to be shure that the following tools have been installed:
1) Subversion. Source code control program. Get it from:
   https://tortoisesvn.net/downloads.html
   Even more different tools: http://subversion.apache.org/packages.html
   At the process of installation make shure that subversion command line client
   will be installed. For TortoiseSVN do not forget to specify an appropriate
   option.
2) CMake. This is used for generating Visual Studio solution and project files.
   Get it from:
   http://www.cmake.org/
   Use version greater or equal 2.8.0.
3) Microsoft Visual Studio 2012 or later. Get it from:
   https://www.visualstudio.com
   The current version of CMake script has been tested for
   Microsoft Visual Studio 2015
4) Python. This is needed only if you will be running the tests
  (which is essential, if you will be developing for clang).
   Get it from: http://www.python.org/download/

Run 'cmake-gui':
1) In the field 'Where is the source code:'
   set path <path_to_dvmhrepo>/dvm/cdvmh-clang/trunk
2) In the field 'Where to build binaries:'
   set path to the directory in which the CDVMH solution should be located.
3) Push 'Configure' button.

New avaliable options will be higlighted.
If LLVM and Clang repositories have been already downloaded:
1) Set the LOCAL_LLVM flag.
2) In the field LLVM_SOURCE_DIR set path to a LLVM source tree.
If LLVM and Clang repositories have not been downloaded yet:
1) Set the DOWNLOAD_LLVM flag.
2) In the fields LLVM_REPO and CLANG_REPO set url of LLVM and Clang subversion
    repository respectively.
    By default the 3.4 versions of LLVM and Clang are used.
You can also set the CMAKE_INSTALL_PREFIX field. A special project INSTALL will
be generated. After building this project all targets will be copied
to the specified dir.

Now push 'Configure' button again and when configuration will be done push
'Generate' button.

The CDVMH solution will be built in the 'Where to build binaries:' directory.

********************************************************************************



# Setup the project

cmake_minimum_required(VERSION 2.8)
project(gtest)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")
set(CMAKE_CXX_STANDARD 11)
# The version number.
#set (superw_VERSION_MAJOR 1)

#----------------------------------------------------------------------------
#Set the necessary path variables and find the ROOT package
#set(ROOTSYS /users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.06.04)
# Tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH /nfs/gerda2/users/rizalinko/gerda_sw/install/root/linux-centos-7-x86_64/6.04.10 )

set(ROOT_CONFIG_DEBUG ON)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
set(CMAKE_MODULE_PATH  /nfs/gerda2/users/rizalinko/gerda_sw/install/root/linux-centos-7-x86_64/6.04.10/etc/cmake/)
list(APPEND CMAKE_PREFIX_PATH /users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.04.10 )

#CPPFLAGS=(shell root-config --cflags)
#LDFLAGS=(shell root-config --libs)
if(DEFINED ROOTSYS AND NOT DEFINED ROOT_DIR)
    set(ROOT_DIR  /nfs/gerda2/users/rizalinko/gerda_sw/install/root/linux-centos-7-x86_64/6.04.10/cmake)
endif()
#find_package(ROOT REQUIRED COMPONENTS TFile TStorage TObject Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore MathMore Minuit2 RooFitCore RooFit RooStats SPlot Thread Gui)
find_package(ROOT)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
#include(${ROOT_USE_FILE})
#----------------------------------------------------------------------------
#Set the necessary path variables for gada and bat

#include_directories(/nfs/gerda2/users/rizalinko/GerdaSoftware/install/gerda/linux-centos-7-x86_64/master/include/gerda-ada)
#link_directories(/nfs/gerda2/users/rizalinko/GerdaSoftware/install/gerda/linux-centos-7-x86_64/master/lib/)


link_directories(/nfs/gerda2/users/rizalinko/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/lib)
				#/nfs/gerda2/users/rizalinko/GerdaSoftware/install/bat/linux-centos-7-x86_64/rizalinko
include_directories(/nfs/gerda2/users/rizalinko/GerdaSoftware/install/bat/linux-centos-7-x86_64/0.9.4_1/include)
#include_directories(/nfs/gerda2/users/rizalinko/GerdaSoftware/install/gerda/linux-centos-7-x86_64/master/include/gelatio)
#include_directories(/nfs/gerda2/users/rizalinko/GerdaSoftware/install/gerda/linux-centos-7-x86_64/master/include/mgdo)
#----------------------------------------------------------------------------


link_directories(/nfs/gerda2/users/rizalinko/gerda_sw/install/root/linux-centos-7-x86_64/6.04.10/lib/)
#link_directories(${ROOT_LIBRARY_DIR})


message($ENV{SWMOD_INST_BASE})
message($ENV{SWMOD_INST_BASE})




if(ROOT_CONFIG_DEBUG)
    message(STATUS "ROOT_LIBRARY_DIR: " ${ROOT_LIBRARY_DIR})
endif(ROOT_CONFIG_DEBUG)


if(ROOT_CONFIG_DEBUG)
    foreach(lib ${ROOT_LIBRARIES})
        message(STATUS "Using root library: " ${lib})
    endforeach(lib)
endif(ROOT_CONFIG_DEBUG)
message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})
set(all_root_libs ${ROOT_LIBRARIES})





#include(RootNewMacros)
#set(ROOT_DIR $ENV{ROOTSYS}/etc/cmake )
#find_package(ROOT REQUIRED COMPONENTS RIO Net)
#set(ROOT_LIBRARY_DIR $ENV{ROOTSYS}/lib)
##list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}/build/)
#list(REMOVE_DUPLICATES ROOT_LIBRARIES)
#set(SOURCE_FILES main.cpp)

add_executable(gtest GammaCountAnalysis.cxx)
# GammaUtilities.h GammaTierHandler.h GammaSpectrum.h GammaProtoFunctions.h GammaLineFit.h GammaLineAnalysis.h GammaFitFunction.h GammaEvent.h GammaSpectrum.cxx GammaLineFit.cxx GammaTierHandler.cxx)
target_link_libraries(gtest ${all_root_libs}  libBATmodels.so libBAT.so )


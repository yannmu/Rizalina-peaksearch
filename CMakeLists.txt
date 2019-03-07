#----------------------------------------------------------------------------
# Setup the project

cmake_minimum_required(VERSION 2.8)
project(peaksearcht)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")
set(CMAKE_CXX_STANDARD 11)
# The version number.
#set (superw_VERSION_MAJOR 1)

#----------------------------------------------------------------------------
#Set the necessary path variables and find the ROOT package
#set(ROOTSYS /users/m/mingazheva/GerdaSoftware/install/root/linux-centos-7-x86_64/6.06.04)
# Tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH  /app/cern/root_v6.14.00/ )

set(ROOT_CONFIG_DEBUG ON)

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
set(CMAKE_MODULE_PATH  /app/cern/root_v6.14.00/cmake)
list(APPEND CMAKE_PREFIX_PATH  /app/cern/root_v6.14.00/ )

#CPPFLAGS=(shell root-config --cflags)
#LDFLAGS=(shell root-config --libs)
if(DEFINED ROOTSYS AND NOT DEFINED ROOT_DIR)
    set(ROOT_DIR /app/cern/root_v6.14.00/cmake)
endif()
#find_package(ROOT REQUIRED COMPONENTS TFile TStorage TObject Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore MathMore Minuit2 RooFitCore RooFit RooStats SPlot Thread Gui)
find_package(ROOT)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
#include(${ROOT_USE_FILE})
#----------------------------------------------------------------------------
#Set the necessary path variables for gada and bat

#include_directories(/home/atp/rizalina/sw/install/gerda/linux-opensuse-42.3-x86_64/master/include/gerda-ada)


#mb llib64
#link_directories(/home/atp/rizalina/sw/install/gerda/linux-opensuse-42.3-x86_64/master/lib)
link_directories(/disk/data1/atp/rizalina/sw/install/bat/linux-opensuse-15.0-x86_64/0.9.4_1/lib64)
				#/nfs/gerda2/users/rizalinko/GerdaSoftware/install/bat/linux-centos-7-x86_64/rizalinko
include_directories(/disk/data1/atp/rizalina/sw/install/bat/linux-opensuse-15.0-x86_64/0.9.4_1/include)
#include_directories(/home/atp/rizalina/sw/install/gerda/linux-opensuse-42.3-x86_64/master/include/gelatio)
#include_directories(/home/atp/rizalina/sw/install/gerda/linux-opensuse-42.3-x86_64/master/include/mgdo)
#----------------------------------------------------------------------------


link_directories( /app/cern/root_v6.14.00/lib/)
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

add_executable(peaksearcht GammaCountAnalysis.cxx)

#target_link_libraries(peaksearcht ${all_root_libs} libgerda-ada-core.so libGELATIOManagement.so libBATmodels.so libBAT.so )
target_link_libraries(peaksearcht ${all_root_libs} libBATmodels.so libBAT.so )

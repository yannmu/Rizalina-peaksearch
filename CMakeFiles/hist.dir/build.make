# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /disk/data1/atp/rizalina/sw/gamma-analysis_tests

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /disk/data1/atp/rizalina/sw/gamma-analysis_tests

# Include any dependencies generated for this target.
include CMakeFiles/hist.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/hist.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hist.dir/flags.make

CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o: CMakeFiles/hist.dir/flags.make
CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o: GammaCountAnalysis.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/disk/data1/atp/rizalina/sw/gamma-analysis_tests/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o -c /disk/data1/atp/rizalina/sw/gamma-analysis_tests/GammaCountAnalysis.cxx

CMakeFiles/hist.dir/GammaCountAnalysis.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hist.dir/GammaCountAnalysis.cxx.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /disk/data1/atp/rizalina/sw/gamma-analysis_tests/GammaCountAnalysis.cxx > CMakeFiles/hist.dir/GammaCountAnalysis.cxx.i

CMakeFiles/hist.dir/GammaCountAnalysis.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hist.dir/GammaCountAnalysis.cxx.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /disk/data1/atp/rizalina/sw/gamma-analysis_tests/GammaCountAnalysis.cxx -o CMakeFiles/hist.dir/GammaCountAnalysis.cxx.s

CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.requires:

.PHONY : CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.requires

CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.provides: CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.requires
	$(MAKE) -f CMakeFiles/hist.dir/build.make CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.provides.build
.PHONY : CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.provides

CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.provides.build: CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o


# Object files for target hist
hist_OBJECTS = \
"CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o"

# External object files for target hist
hist_EXTERNAL_OBJECTS =

hist: CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o
hist: CMakeFiles/hist.dir/build.make
hist: /app/cern/root_v6.14.00/lib/libCore.so
hist: /app/cern/root_v6.14.00/lib/libImt.so
hist: /app/cern/root_v6.14.00/lib/libRIO.so
hist: /app/cern/root_v6.14.00/lib/libNet.so
hist: /app/cern/root_v6.14.00/lib/libHist.so
hist: /app/cern/root_v6.14.00/lib/libGraf.so
hist: /app/cern/root_v6.14.00/lib/libGraf3d.so
hist: /app/cern/root_v6.14.00/lib/libGpad.so
hist: /app/cern/root_v6.14.00/lib/libTree.so
hist: /app/cern/root_v6.14.00/lib/libTreePlayer.so
hist: /app/cern/root_v6.14.00/lib/libRint.so
hist: /app/cern/root_v6.14.00/lib/libPostscript.so
hist: /app/cern/root_v6.14.00/lib/libMatrix.so
hist: /app/cern/root_v6.14.00/lib/libPhysics.so
hist: /app/cern/root_v6.14.00/lib/libMathCore.so
hist: /app/cern/root_v6.14.00/lib/libThread.so
hist: /app/cern/root_v6.14.00/lib/libMultiProc.so
hist: CMakeFiles/hist.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/disk/data1/atp/rizalina/sw/gamma-analysis_tests/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable hist"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hist.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hist.dir/build: hist

.PHONY : CMakeFiles/hist.dir/build

CMakeFiles/hist.dir/requires: CMakeFiles/hist.dir/GammaCountAnalysis.cxx.o.requires

.PHONY : CMakeFiles/hist.dir/requires

CMakeFiles/hist.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hist.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hist.dir/clean

CMakeFiles/hist.dir/depend:
	cd /disk/data1/atp/rizalina/sw/gamma-analysis_tests && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /disk/data1/atp/rizalina/sw/gamma-analysis_tests /disk/data1/atp/rizalina/sw/gamma-analysis_tests /disk/data1/atp/rizalina/sw/gamma-analysis_tests /disk/data1/atp/rizalina/sw/gamma-analysis_tests /disk/data1/atp/rizalina/sw/gamma-analysis_tests/CMakeFiles/hist.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hist.dir/depend

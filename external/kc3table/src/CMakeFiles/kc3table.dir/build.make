# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/cmake-gui

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nikos/Desktop/Monas/external/kc3table

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nikos/Desktop/Monas/external/kc3table/src

# Include any dependencies generated for this target.
include CMakeFiles/kc3table.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/kc3table.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kc3table.dir/flags.make

CMakeFiles/kc3table.dir/kc3table.cpp.o: CMakeFiles/kc3table.dir/flags.make
CMakeFiles/kc3table.dir/kc3table.cpp.o: kc3table.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/nikos/Desktop/Monas/external/kc3table/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/kc3table.dir/kc3table.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/kc3table.dir/kc3table.cpp.o -c /home/nikos/Desktop/Monas/external/kc3table/src/kc3table.cpp

CMakeFiles/kc3table.dir/kc3table.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kc3table.dir/kc3table.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/nikos/Desktop/Monas/external/kc3table/src/kc3table.cpp > CMakeFiles/kc3table.dir/kc3table.cpp.i

CMakeFiles/kc3table.dir/kc3table.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kc3table.dir/kc3table.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/nikos/Desktop/Monas/external/kc3table/src/kc3table.cpp -o CMakeFiles/kc3table.dir/kc3table.cpp.s

CMakeFiles/kc3table.dir/kc3table.cpp.o.requires:
.PHONY : CMakeFiles/kc3table.dir/kc3table.cpp.o.requires

CMakeFiles/kc3table.dir/kc3table.cpp.o.provides: CMakeFiles/kc3table.dir/kc3table.cpp.o.requires
	$(MAKE) -f CMakeFiles/kc3table.dir/build.make CMakeFiles/kc3table.dir/kc3table.cpp.o.provides.build
.PHONY : CMakeFiles/kc3table.dir/kc3table.cpp.o.provides

CMakeFiles/kc3table.dir/kc3table.cpp.o.provides.build: CMakeFiles/kc3table.dir/kc3table.cpp.o

# Object files for target kc3table
kc3table_OBJECTS = \
"CMakeFiles/kc3table.dir/kc3table.cpp.o"

# External object files for target kc3table
kc3table_EXTERNAL_OBJECTS =

kc3table: CMakeFiles/kc3table.dir/kc3table.cpp.o
kc3table: CMakeFiles/kc3table.dir/build.make
kc3table: CMakeFiles/kc3table.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable kc3table"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kc3table.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kc3table.dir/build: kc3table
.PHONY : CMakeFiles/kc3table.dir/build

CMakeFiles/kc3table.dir/requires: CMakeFiles/kc3table.dir/kc3table.cpp.o.requires
.PHONY : CMakeFiles/kc3table.dir/requires

CMakeFiles/kc3table.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kc3table.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kc3table.dir/clean

CMakeFiles/kc3table.dir/depend:
	cd /home/nikos/Desktop/Monas/external/kc3table/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nikos/Desktop/Monas/external/kc3table /home/nikos/Desktop/Monas/external/kc3table /home/nikos/Desktop/Monas/external/kc3table/src /home/nikos/Desktop/Monas/external/kc3table/src /home/nikos/Desktop/Monas/external/kc3table/src/CMakeFiles/kc3table.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/kc3table.dir/depend


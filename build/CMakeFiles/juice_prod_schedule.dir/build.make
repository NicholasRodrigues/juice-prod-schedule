# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.30.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.30.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/felipeduarte/juice-prod-schedule

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/felipeduarte/juice-prod-schedule/build

# Include any dependencies generated for this target.
include CMakeFiles/juice_prod_schedule.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/juice_prod_schedule.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/juice_prod_schedule.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/juice_prod_schedule.dir/flags.make

CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o: CMakeFiles/juice_prod_schedule.dir/flags.make
CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o: /Users/felipeduarte/juice-prod-schedule/src/algorithm.cpp
CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o: CMakeFiles/juice_prod_schedule.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/felipeduarte/juice-prod-schedule/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o -MF CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o.d -o CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o -c /Users/felipeduarte/juice-prod-schedule/src/algorithm.cpp

CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felipeduarte/juice-prod-schedule/src/algorithm.cpp > CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.i

CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felipeduarte/juice-prod-schedule/src/algorithm.cpp -o CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.s

CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o: CMakeFiles/juice_prod_schedule.dir/flags.make
CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o: /Users/felipeduarte/juice-prod-schedule/src/main.cpp
CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o: CMakeFiles/juice_prod_schedule.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/felipeduarte/juice-prod-schedule/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o -MF CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o.d -o CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o -c /Users/felipeduarte/juice-prod-schedule/src/main.cpp

CMakeFiles/juice_prod_schedule.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/juice_prod_schedule.dir/src/main.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felipeduarte/juice-prod-schedule/src/main.cpp > CMakeFiles/juice_prod_schedule.dir/src/main.cpp.i

CMakeFiles/juice_prod_schedule.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/juice_prod_schedule.dir/src/main.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felipeduarte/juice-prod-schedule/src/main.cpp -o CMakeFiles/juice_prod_schedule.dir/src/main.cpp.s

CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o: CMakeFiles/juice_prod_schedule.dir/flags.make
CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o: /Users/felipeduarte/juice-prod-schedule/src/parser.cpp
CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o: CMakeFiles/juice_prod_schedule.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/felipeduarte/juice-prod-schedule/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o -MF CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o.d -o CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o -c /Users/felipeduarte/juice-prod-schedule/src/parser.cpp

CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felipeduarte/juice-prod-schedule/src/parser.cpp > CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.i

CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felipeduarte/juice-prod-schedule/src/parser.cpp -o CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.s

CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o: CMakeFiles/juice_prod_schedule.dir/flags.make
CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o: /Users/felipeduarte/juice-prod-schedule/src/neighborhoods.cpp
CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o: CMakeFiles/juice_prod_schedule.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/felipeduarte/juice-prod-schedule/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o -MF CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o.d -o CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o -c /Users/felipeduarte/juice-prod-schedule/src/neighborhoods.cpp

CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felipeduarte/juice-prod-schedule/src/neighborhoods.cpp > CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.i

CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felipeduarte/juice-prod-schedule/src/neighborhoods.cpp -o CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.s

# Object files for target juice_prod_schedule
juice_prod_schedule_OBJECTS = \
"CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o" \
"CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o" \
"CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o" \
"CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o"

# External object files for target juice_prod_schedule
juice_prod_schedule_EXTERNAL_OBJECTS =

juice_prod_schedule: CMakeFiles/juice_prod_schedule.dir/src/algorithm.cpp.o
juice_prod_schedule: CMakeFiles/juice_prod_schedule.dir/src/main.cpp.o
juice_prod_schedule: CMakeFiles/juice_prod_schedule.dir/src/parser.cpp.o
juice_prod_schedule: CMakeFiles/juice_prod_schedule.dir/src/neighborhoods.cpp.o
juice_prod_schedule: CMakeFiles/juice_prod_schedule.dir/build.make
juice_prod_schedule: CMakeFiles/juice_prod_schedule.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/felipeduarte/juice-prod-schedule/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable juice_prod_schedule"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/juice_prod_schedule.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/juice_prod_schedule.dir/build: juice_prod_schedule
.PHONY : CMakeFiles/juice_prod_schedule.dir/build

CMakeFiles/juice_prod_schedule.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/juice_prod_schedule.dir/cmake_clean.cmake
.PHONY : CMakeFiles/juice_prod_schedule.dir/clean

CMakeFiles/juice_prod_schedule.dir/depend:
	cd /Users/felipeduarte/juice-prod-schedule/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/felipeduarte/juice-prod-schedule /Users/felipeduarte/juice-prod-schedule /Users/felipeduarte/juice-prod-schedule/build /Users/felipeduarte/juice-prod-schedule/build /Users/felipeduarte/juice-prod-schedule/build/CMakeFiles/juice_prod_schedule.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/juice_prod_schedule.dir/depend


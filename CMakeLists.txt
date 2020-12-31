cmake_minimum_required (VERSION 3.1.0)
project (keywords)
INCLUDE(FindPackageHandleStandardArgs)

# Options
OPTION(VERBOSE_DEBUG "Activate verbose display on standard error to facilitate debug" OFF)

# CMake configuration
SET(CMAKE_MODULE_PATH ".")

# General configuration
SET(CMAKE_C_FLAGS "-Werror -std=c++17 -Waddress -Warray-bounds -Wchar-subscripts -Wimplicit-int -Wimplicit-function-declaration -Wcomment -Wformat -Wnonnull -Wparentheses -Wpointer-sign -Wreorder -Wreturn-type -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable")
SET(CMAKE_CXX_FLAGS "-Werror -std=c++17 -Waddress -Warray-bounds -Wchar-subscripts -Wcomment -Wformat -Wnonnull -Wparentheses -Wreorder -Wreturn-type -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable")
SET(CMAKE_CXX_FLAGS_DEBUG
		"${GCC_DEBUG_FLAGS} -O0 -g -DDEBUG"
		CACHE STRING "Flags used by the C++ compiler during debug builds."
		FORCE )
SET(CMAKE_C_FLAGS_DEBUG
		"${GCC_DEBUG_FLAGS} -O0 -g -DDEBUG"
		CACHE STRING "Flags used by the C compiler during debug builds."
		FORCE )
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Profiling build type
SET(CMAKE_CXX_FLAGS_PROFILE
		"${GCC_DEBUG_FLAGS} -O0 -g -pg -fprofile-arcs -ftest-coverage -DDEBUG"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET(CMAKE_C_FLAGS_PROFILE
		"${GCC_DEBUG_FLAGS} -O0 -g -pg -fprofile-arcs -ftest-coverage -DDEBUG"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_PROFILE
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_PROFILE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PROFILE
    CMAKE_C_FLAGS_PROFILE
    CMAKE_EXE_LINKER_FLAGS_PROFILE
    CMAKE_SHARED_LINKER_FLAGS_PROFILE )

# Find Doxygen
Find_Package(Doxygen)
if (DOXYGEN_FOUND)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} doxyconf comment "Generating API documentation with Doxygen" verbatim)
endif (DOXYGEN_FOUND)

# Configure target
ADD_EXECUTABLE(keywords keywords.cpp dictionary.h dictionary.cpp graph.h graph.cpp)
ADD_EXECUTABLE(prepare prepare.cpp)

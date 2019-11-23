cmake_minimum_required(VERSION 2.8)
project(bF)

# project settings 
set(BF_ENABLE_LOG false)

if (BF_ENABLE_LOG) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LOG=1")
endif()

# add modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/cmake-argp/cmake" ${CMAKE_MODULE_PATH})

# set some project-wide flags
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-overflow -Wno-varargs -O3" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overflow -Wno-varargs -O3" )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-stdlib=libc++" COMPILER_SUPPORTS_LIBCPP)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

	if(COMPILER_SUPPORTS_LIBCPP)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif()
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

set( BINARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/pc CACHE PATH
        "Single Directory for all Binaries")

# find libs
find_package(argp)

# all source files
file(GLOB_RECURSE ALL_SRC src/*.cc)

# set includes
include_directories("src" "lib/cxx_argp" "${ARGP_INCLUDE_PATH}")

# compile bF
add_executable(bF ${ALL_SRC})

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive" )

# link final bF binary 
target_link_libraries(bF ${ARGP_LIBRARIES})

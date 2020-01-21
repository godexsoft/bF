cmake_minimum_required ( VERSION 3.15 )
project ( bF )

# default release build
if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif ()

# possible options
option ( BF_ENABLE_LOG  "Enables or disables logging support"  OFF )

MESSAGE ( STATUS "bF Options:" ) 
MESSAGE ( STATUS "----" ) 
MESSAGE ( STATUS "BF_ENABLE_LOG: " ${BF_ENABLE_LOG} )
MESSAGE ( STATUS "----" ) 

# add used modules and libs
set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/cmake-argp/cmake" ${CMAKE_MODULE_PATH} )
add_subdirectory ( ${CMAKE_SOURCE_DIR}/lib/libfmt EXCLUDE_FROM_ALL )

find_package ( argp )

# find all source code
file( GLOB_RECURSE ALL_SRC src/*.cc )

add_executable (
  bF               "${ALL_SRC}" )

target_compile_features (
  bF    PRIVATE    cxx_std_17
        PRIVATE    cxx_constexpr 
        PRIVATE    cxx_auto_type 
        PRIVATE    cxx_lambdas 
        PRIVATE    cxx_nullptr
        PRIVATE    cxx_variadic_templates )

target_include_directories (
  bF    PRIVATE    "include" 
        PRIVATE    "lib/libfmt/include"
        PRIVATE    "lib/cxx_argp"
        PRIVATE    "${ARGP_INCLUDE_PATH}" )
target_link_libraries (
  bF    PRIVATE    "${ARGP_LIBRARIES}"
        PRIVATE    fmt::fmt-header-only )

# add custom option-based flags
if ( BF_ENABLE_LOG ) 
  target_compile_options (
    bF  PRIVATE    "-DENABLE_LOG=1" )
endif ()


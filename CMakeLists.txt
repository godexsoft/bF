cmake_minimum_required ( VERSION 3.15 )
project ( bF )

# default to release build
if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
endif ()

# possible options
option ( BF_ENABLE_LOG   "Enables or disables logging support"  OFF )
option ( BF_ENABLE_8BIT  "Enables 8 bit cell size"  ON )
option ( BF_ENABLE_16BIT "Enables 16 bit cell size"  OFF )
option ( BF_ENABLE_32BIT "Enables 32 bit cell size"  OFF )
option ( BF_ENABLE_64BIT "Enables 64 bit cell size"  OFF )

MESSAGE ( STATUS "bF Options:" ) 
MESSAGE ( STATUS "----" ) 
MESSAGE ( STATUS "BF_ENABLE_LOG:   " ${BF_ENABLE_LOG} )
MESSAGE ( STATUS "BF_ENABLE_8BIT:  " ${BF_ENABLE_8BIT} )
MESSAGE ( STATUS "BF_ENABLE_16BIT: " ${BF_ENABLE_16BIT} )
MESSAGE ( STATUS "BF_ENABLE_32BIT: " ${BF_ENABLE_32BIT} )
MESSAGE ( STATUS "BF_ENABLE_64BIT: " ${BF_ENABLE_64BIT} )
MESSAGE ( STATUS "----" ) 

# add used modules and libs
add_subdirectory ( ${CMAKE_SOURCE_DIR}/lib/libfmt EXCLUDE_FROM_ALL )

# bF library
add_library ( libbF INTERFACE )

target_compile_features (
  libbF INTERFACE  cxx_std_17
        INTERFACE  cxx_constexpr 
        INTERFACE  cxx_auto_type 
        INTERFACE  cxx_lambdas 
        INTERFACE  cxx_nullptr
        INTERFACE  cxx_variadic_templates )

target_include_directories (
  libbF INTERFACE  "include" 
        INTERFACE  "lib/libfmt/include" )
target_link_libraries (
  libbF INTERFACE  "-lstdc++" 
        INTERFACE  fmt::fmt-header-only )

# add custom option-based flags for the library
if ( BF_ENABLE_LOG ) 
  target_compile_options (
    libbF   INTERFACE    "-DENABLE_LOG=1" )
endif ()

# bF app itself
file( GLOB_RECURSE ALL_SRC src/*.cc )
add_executable (
  bF    "${ALL_SRC}" )

target_include_directories (
  bF    PRIVATE    "lib/cxxopts/include" )        
target_link_libraries (
  bF    PRIVATE    libbF )

# add custom option-based flags for the example app
if ( BF_ENABLE_8BIT ) 
  target_compile_options (
    bF   PRIVATE    "-DENABLE_8BIT=1" )
endif ()

if ( BF_ENABLE_16BIT ) 
  target_compile_options (
    bF   PRIVATE    "-DENABLE_16BIT=1" )
endif ()

if ( BF_ENABLE_32BIT ) 
  target_compile_options (
    bF   PRIVATE    "-DENABLE_32BIT=1" )
endif ()

if ( BF_ENABLE_64BIT ) 
  target_compile_options (
    bF   PRIVATE    "-DENABLE_64BIT=1" )
endif ()

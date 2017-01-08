cmake_minimum_required(VERSION 3.2)
project(drift)
add_executable(drift_test
  context.hh
  drift_test.cc
  variant.hh
  private/ast.hh
  private/ast.cc
  private/compile_context.hh
  private/frame.hh
  private/text_parse.cc
  private/types.hh
  private/context.cc
  private/variant.cc)
set_property(TARGET drift_test PROPERTY CXX_STANDARD 14)
set_property(TARGET drift_test PROPERTY CXX_STANDARD_REQUIRED 14)


# Thanks: http://stackoverflow.com/a/33266748
include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

#enable_cxx_compiler_flag_if_supported("-Wall")
#enable_cxx_compiler_flag_if_supported("-Wextra")
#enable_cxx_compiler_flag_if_supported("-pedantic")
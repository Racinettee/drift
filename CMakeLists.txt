cmake_minimum_required(VERSION 3.2)
project(drift)
add_executable(drift_test
  drift_test.cc
  private/ast.hh
  private/ast.cc
  private/compile_context.hh
  private/text_parse.cc
  private/types.hh
  context.cc)
set_property(TARGET drift_test PROPERTY CXX_STANDARD 14)
set_property(TARGET drift_test PROPERTY CXX_STANDARD_REQUIRED 14)
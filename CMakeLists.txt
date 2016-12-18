cmake_minimum_required(VERSION 3.2)
project(drift)
add_executable(drift_test
  drift_test.cc
  text_parse.cc)
set_property(TARGET drift_test PROPERTY CXX_STANDARD 14)
set_property(TARGET drift_test PROPERTY CXX_STANDARD_REQUIRED 14)
#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "types.hh"

namespace drift
{
  struct compile_context
  {
    program_list program;
    inline void push_inst(op_t op)
    {
      program.push_back(op);
    }
  };
}
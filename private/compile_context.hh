#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "types.hh"

namespace drift
{
  struct compile_context
  {
    variable_mapping var_table;
    unsigned int persistent_index = 0;
    inline unsigned int add_variable(const std::wstring& name)
    {
      var_table[name] = persistent_index;
      return persistent_index++;
    }
    inline bool has_variable(const std::wstring& name)
    {
      return (var_table.find(name) != var_table.end());
    }
    inline unsigned int variable_index(const std::wstring& name)
    {
      return var_table.at(name);
    }
    program_list program;
    inline void push_inst(op_t op)
    {
      program.push_back(op);
    }
    template<class T> inline void push_literal(T literal)
    {
      size_t start = program.size();
      program.resize(program.size()+sizeof(T));
      std::copy((op_t*)&literal, (op_t*)&literal+sizeof(T), &program[start]);
    }
  };
}
#pragma once
#include <codecvt>
#include <locale>
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
    size_t bytes_count() const
    {
      return program.size();
    }
    template<class T> inline void write_value_bytes(size_t index, T value)
    {
      *reinterpret_cast<T*>(&program.at(index)) = value;
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8cnv;
    std::string to_string(const std::wstring& s)
    {
      return utf8cnv.to_bytes(s);
    }
    std::wstring to_wstring(const std::string& s)
    {
      return utf8cnv.from_bytes(s);
    }
  };
}
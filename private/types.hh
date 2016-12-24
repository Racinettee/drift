#pragma once
#include "../variant.hh"
namespace drift
{
  enum inst : unsigned char
  {
    push,
    pop,
    add,
    sub,
    mul,
    divide,
    end,
    num_literal,
    str_literal
  };

  typedef unsigned char op_t;

  typedef std::vector<op_t> program_list;

  typedef std::deque<std::shared_ptr<variant>> value_stack;

  //typedef std::unordered_map<std::wstring
}
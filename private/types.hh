#pragma once
#include <deque>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../variant.hh"
namespace drift
{
  enum inst : unsigned char
  {
    push,
    pop,
    store,
    add,
    sub,
    mul,
    divide,
    end,
    num_literal,
    str_literal,
    nil,
    neg
  };

  typedef unsigned char op_t;

  typedef unsigned int var_index;

  typedef std::vector<op_t> program_list;

  typedef std::deque<std::shared_ptr<variant>> value_stack;

  typedef const std::vector<std::shared_ptr<variant>>& args_t;

  typedef std::unordered_map<std::wstring, var_index> variable_mapping;

  typedef std::unordered_map<var_index, variant_ptr> variable_storage;
}
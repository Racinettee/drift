#pragma once
#include <deque>
#include <vector>
#include <memory>
#include <unordered_map>

namespace drift
{
  typedef unsigned int var_index;
}
#include "../variant.hh"
namespace drift
{
  enum inst : unsigned char
  {
    push,
    pop,
    store,
    load,
    add,
    sub,
    mul,
    divide,
    end,
    num_literal, // a double follows
    str_literal, // no implementation yet: refer to a location in a data segment?
    fnc_pointer, // two indexes follow
    nil,
    neg,
    less_than,
    greater_than,
    equals,
    nequals,
    jump_true,
    jump_false,
    call
  };

  typedef unsigned char op_t;

  typedef std::wstring string_t;

  typedef std::vector<op_t> program_list;

  typedef std::deque<std::shared_ptr<variant>> value_stack;

  typedef const std::vector<std::shared_ptr<variant>>& args_t;

  typedef std::unordered_map<string_t, var_index> variable_mapping;

  typedef std::unordered_map<var_index, variant_ptr> variable_storage;
}
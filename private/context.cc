#include <stack>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include "../context.hh"
#include "ast.hh"
#include "frame.hh"
#include "compile_context.hh"

namespace drift
{
  block_expr* parse(std::wistream&);

  context::context()
  {
    cc = new compile_context;
  }
  context::~context()
  {
    delete cc;
  }
  size_t context::code_size() const
  {
    return cc->program.size();
  }
  template<class T> static inline T get_datum(op_t* addr)
  {
    return *reinterpret_cast<T*>(addr);
  }
  void context::exec(const std::vector<shared_ptr<variant>>& args)
  {
    // Do not delete the last frame - it is owned by the context
    std::stack<frame*> stack_frames;
    stack_frames.push(&this->global_frame);
    // This will need to start off at an offset if there is a data segment ever<
    auto& ilist = this->cc->program;
    // The same goes for this line
    for(size_t i = 0; i < code_size(); i++)
    {
      switch(ilist[i])
      {
        default:
          throw std::runtime_error("Unknown or unimplemented instruction!");
        case inst::push:
          break;
        case inst::pop:
          stack.pop_back();
          break;
        case inst::store: {
          variant_ptr data = stack.back(); stack.pop_back();
          stack_frames.top()->set_var(get_datum<var_index>(&ilist[i+1]), data);
          i += sizeof(var_index);
          break;
        }
        case inst::load: {
          stack.push_back(stack_frames.top()->get_var(get_datum<var_index>(&ilist[i+1])));
          i += sizeof(var_index);
          break;
        }
        case inst::add: {
          variant_ptr right = stack.back(); stack.pop_back();
          variant_ptr left = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(*left + *right));
          break;
        }
        case inst::sub: {
          variant_ptr right = stack.back(); stack.pop_back();
          variant_ptr left = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(*left - *right));
          break;
        }
        case inst::mul: {
          variant_ptr right = stack.back(); stack.pop_back();
          variant_ptr left = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(*left * *right));
          break;
        }
        case inst::divide: {
          variant_ptr right = stack.back(); stack.pop_back();
          variant_ptr left = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(*left / *right));
          break;
        }
        case inst::nil:
          stack.push_back(shared_variant(null_variant()));
          break;
        case inst::neg: {
          variant_ptr operand = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(-(*operand)));
          break;
        }
        case inst::less_than: {
          variant_ptr right = stack.back(); stack.pop_back();
          variant_ptr left = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(*left < *right));
          break;
        }
        case inst::greater_than: {
          variant_ptr right = stack.back(); stack.pop_back();
          variant_ptr left = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(*left > *right));
          break;
        }
        case inst::jump_false: {
          variant_ptr bool_value = stack.back(); stack.pop_back();
          i += (!bool_value->boolean ? get_datum<unsigned int>(&ilist[i+1]) : sizeof(unsigned int));
          break;
        }
        case inst::end:
          break;
        case inst::num_literal: {
          stack.push_back(shared_variant(get_datum<double>(&ilist[i+1])));
          i += sizeof(double);
          break;
        }
      }
    }
  }
  
  variant_ptr context::operator()(const wstring& s)
  {
    try {
      wistringstream ss(s);
      block_expr* program = parse(ss);
      program->emit(cc);
      delete program;
      exec({});
      if(!stack.empty())
      {
        auto result = stack.back();
        stack.pop_back();
        return result;
      }
      return shared_variant(null_variant());
    }
    catch(exception& e) {
      wcout << e.what() << endl;
      return shared_variant(null_variant());
    }
  }
  variant_ptr context::operator[](const wstring& name)
  {
    if(!cc->has_variable(name))
      return shared_variant(null_variant());
    return global_frame.get_var(cc->get_variable(name));
  }

  variant_ptr context::load_file(const wstring& file)
  {
    try {
      wifstream fs{cc->to_string(file)};
      if(!fs.is_open())
        throw runtime_error("Could not open file: "s+cc->to_string(file));
      block_expr* program = parse(fs);
      program->emit(cc);
      delete program;
      exec({});
      if(!stack.empty())
      {
        auto result = stack.back();
        stack.pop_back();
        return result;
      }
      return shared_variant(null_variant());
    }
    catch(exception& e) {
      wcout << e.what() << endl;
      return shared_variant(null_variant());
    }
  }
}
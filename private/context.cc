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
  ast::block_expr* parse(std::wistream&);

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
  void context::exec(const std::vector<shared_ptr<variant>>& args, size_t starting_point)
  {
    // Do not delete the last frame - it is owned by the context
    std::stack<frame*> stack_frames;
    stack_frames.push(&this->global_frame);
    // This will need to start off at an offset if there is a data segment ever<
    auto& ilist = this->cc->program;
    // The same goes for this line
    for(size_t i = starting_point; i < code_size(); i++)
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
          i += (!bool_value->boolean ? get_datum<var_index>(&ilist[i+1]) : sizeof(var_index));
          break;
        }
        case inst::fnc_pointer: {
          var_index handle = get_datum<var_index>(&ilist[i + 1]);
          i += sizeof(var_index);
          var_index address = get_datum<var_index>(&ilist[i + 1]);
          i += sizeof(var_index);
          stack.push_back(shared_variant(function_pointer(handle, address, this)));
          break;
        }
        case inst::call: {
          var_index fn_ptr_id = get_datum<var_index>(&ilist[i + 1]);
          i += sizeof(var_index);
          variant_ptr fn = stack_frames.top()->get_var(fn_ptr_id);
          var_index address = fn->fn_ptr.address;
          stack_frames.push(new frame(static_cast<var_index>(i), stack_frames.top()));
          i = address-1; // This subtraction accounts for the offset the ast generates
          break;
        }
        case inst::end:
          if (stack_frames.size() > 1) {
            i = stack_frames.top()->return_index();
            delete stack_frames.top();
            stack_frames.pop();
            break;
          }
          return;
        case inst::num_literal: {
          stack.push_back(shared_variant(get_datum<double>(&ilist[i+1])));
          i += sizeof(double);
          break;
        }
        case inst::str_literal: {
          var_index str_size = get_datum<var_index>(&ilist[i + 1]);
          i += sizeof(var_index);
          stack.push_back(shared_variant(string_t((wchar_t*)&ilist[i + 1], str_size)));
          i += str_size*sizeof(wchar_t);
          break;
        }
      }
    }
  }
  
  object context::operator()(const wstring& s)
  {
    try {
      wistringstream ss(s);
      ast::block_expr* program = parse(ss);
      program->emit(cc);
      cc->run_deferred_routines();
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
  object context::operator[](const wstring& name)
  {
    if(!cc->has_variable(name))
      return shared_variant(null_variant());
    return global_frame.get_var(cc->get_variable(name));
  }

  object context::load_file(const wchar_t* file)
  {
    return load_file(wstring(file));
  }
  object context::load_file(const wstring& file)
  {
    try {
      wifstream fs{cc->to_string(file)};
      if(!fs.is_open())
        throw runtime_error("Could not open file: "s+cc->to_string(file));
      ast::block_expr* program = parse(fs);
      program->emit(cc);
      cc->push_inst(inst::end);
      cc->run_deferred_routines();
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
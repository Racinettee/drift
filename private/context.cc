#include <stack>
#include <string>
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
          stack_frames.top()->set_var(get_datum<unsigned int>(&ilist[i+1]), data);//*reinterpret_cast<unsigned int*>(&ilist[i+1]);
          i += sizeof(unsigned int);
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
        case inst::end:
          break;
        case inst::num_literal: {
          stack.push_back(shared_variant(get_datum<double>(&ilist[i+1]))); //*reinterpret_cast<double*>(&ilist[i+1])));
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
      auto result = stack.back();
      stack.pop_back();
      return result;
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
    return global_frame.get_var(cc->variable_index(name));
  }
}
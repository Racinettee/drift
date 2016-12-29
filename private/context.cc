#include <string>
#include <sstream>
#include <iostream>
using namespace std;

#include "ast.hh"
#include "../context.hh"
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

  void context::exec(const std::vector<shared_ptr<variant>>& args)
  {
    // This will need to start off at an offset if there is a data segment ever<
    auto& ilist = this->cc->program;
    // The same goes for this line
    for(size_t i = 0; i < code_size(); i++)
    {
      switch(ilist[i])
      {
        default:
          throw std::runtime_error("Unknown instruction!");
        case inst::push:
          break;
        case inst::pop:
          stack.pop_back();
          break;
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
        case inst::neg: {
          variant_ptr operand = stack.back(); stack.pop_back();
          stack.push_back(shared_variant(-(*operand)));
        }
        case inst::end:
          break;
        case inst::num_literal: {
          stack.push_back(shared_variant(*reinterpret_cast<double*>(&ilist[i+1])));
          i += +sizeof(double);
          break;
        }
      }
    }
  }
  
  shared_ptr<variant> context::operator()(const std::wstring& s)
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
    }
  }
}
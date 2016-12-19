#include <string>
#include <sstream>
#include <iostream>
using namespace std;

#include "ast.hh"
#include "context.hh"

namespace drift
{
  block_expr* parse(std::wistream&);
  
  shared_ptr<variant> context::operator()(const std::wstring& s)
  {
    try {
      wistringstream ss(s);
      block_expr* program = parse(ss);
      program->emit(this);
    }
    catch(exception& e) {
      wcout << e.what() << endl;
    }
  }
}
#include "context.hh"
using namespace std;

namespace drift
{
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
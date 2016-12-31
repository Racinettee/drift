#include <memory>
#include <iostream>
using namespace std;

#include "context.hh"
#include "variant.hh"

using namespace drift;

int main() try
{
  context ctxt;
  shared_ptr<variant> result = ctxt(
    L"-2.3*2\n"
    L"let init = 10");
  cout << result->num << endl;
  wcout << kind_string(ctxt[L"init"]) << L": "<< ctxt[L"init"]->num << endl;
}
catch(std::exception& e)
{
  puts(e.what());
}
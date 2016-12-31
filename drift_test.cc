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
    L"let init = -(10* -2)");
  cout << result->num << endl;
  variant_ptr init = ctxt[L"init"];
  wcout << kind_string(init) << L": "<< init->num << endl;
}
catch(std::exception& e)
{
  puts(e.what());
}
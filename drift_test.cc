#include <memory>
#include <iostream>
using namespace std;

#include "context.hh"
#include "variant.hh"
#include "private/types.hh"

using namespace drift;

int main() try
{
  context ctxt;
  shared_ptr<variant> result = ctxt(L"16 + 16");
  cout << result->num << endl;
}
catch(std::exception& e)
{
  puts(e.what());
}
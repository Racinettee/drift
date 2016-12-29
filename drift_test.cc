#include <memory>
#include <iostream>
using namespace std;

#include "context.hh"
#include "variant.hh"

using namespace drift;

int main() try
{
  context ctxt;
  shared_ptr<variant> result = ctxt(L"-2.3*2");
  cout << result->num << endl;
}
catch(std::exception& e)
{
  puts(e.what());
}
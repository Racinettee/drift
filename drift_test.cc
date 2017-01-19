#include <memory>
#include <iostream>
using namespace std;

#include "context.hh"
using namespace drift;

int main() try
{
  context ctxt;
  wcout << L"Result of test.drift: " << ctxt.load_file(L"testing/test.drift")->num << endl;
  auto initial = ctxt[L"result"];
  wcout << L"Details of variable 'result': " << kind_string(initial) << L": "<< initial->num << endl;
  return 0;
}
catch(std::exception& e)
{
  puts(e.what());
}
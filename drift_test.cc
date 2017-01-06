#include <memory>
#include <iostream>
using namespace std;

#include "context.hh"
using namespace drift;

int main() try
{
  context ctxt;
  ctxt.load_file(L"testing/test.drift");
  auto initial = ctxt[L"result"];
  wcout << kind_string(initial) << L": "<< initial->num << endl;
  return 0;
}
catch(std::exception& e)
{
  puts(e.what());
}
#include <memory>
#include <iostream>
using namespace std;

#include "context.hh"
using namespace drift;

int main() try
{
  context ctxt;
  wcout << L"Result of test.drift: " << ctxt.load_file(L"testing/test.drift") << endl;
  wcout << L"Value of variable 'stringvar': " << ctxt[L"stringvar"] << endl;
  wcout << L"Result of test method: " << ctxt[L"testmethod"]() << endl;

  return 0;
}
catch(std::exception& e)
{
  puts(e.what());
}
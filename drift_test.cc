#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <deque>
#include <new>
#include <locale>
#include <codecvt>
#include <initializer_list>
using namespace std;

#include "variant.hh"
#include "private/types.hh"

using namespace drift;

#include "context.hh"

int main() try
{
  using namespace std;

  context ctxt;

  shared_ptr<variant> result = ctxt(L"16 + 16");
  cout << result->num << endl;
}
catch(std::exception& e)
{
  puts(e.what());
}
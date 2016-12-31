#include <string>
#include "../variant.hh"
namespace
{
  wchar_t* kind_strings[] =
  {
    L"double",
    L"string",
    L"null"
  };
}
using namespace std;
namespace drift
{
  wstring kind_string(variant_ptr v)
  {
    return kind_strings[(int)v->kind];
  }
}
#include <locale>
#include <codecvt>
#include <fstream>
#include <string>
#include <cwtype>
using namespace std;

#include "ast.hh"
#include "drift_variant.hh"

namespace
{
  static inline atomic_expr* read_num(wchar_t first, std::wistream& input)
  {
    std::wstring result = first;
    while(!input.eof() && (iswdigit(input.peek()) || input.peek() == L'.'))
      result += input.get();
    // do some validation like check for to many dots in the number
    // ...

    return new atomic_expr(stod(result));
  }
  static inline expr* read_ident_or_kw(wchar_t first, std::wistream& input)
  {
    std::wstring result = first;
    while(!input.eof() && iswalnum(input.peek()))
      result += input.get();

    // Check if keyword, return new keywd expression else new ident expr
  }
}
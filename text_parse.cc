#include <locale>
#include <codecvt>
#include <fstream>
#include <string>
#include <cwctype>
using namespace std;

#include "ast.hh"
#include "variant.hh"

namespace
{
  static std::array<wchar_t*> keywords =
  {
    L"let",
    L"def",
    L"class"
  };
  static inline bool is_keyword(const wstring& kw)
  {
    for(auto element : keywords)
      if(kw == element)
        return true;
    return false;
  }
  static inline expr* read_ident_or_kw(wchar_t first, wistream& input)
  {
    std::wstring result = first;
    while(!input.eof() && iswalnum(input.peek()))
      result += input.get();

    // Check if keyword, return new keywd expression else new ident expr
    if(is_keyword(result))
      return new keyword_expr(result);
    return new identifier_expr(result);
  }
  static inline atomic_expr* read_num(wchar_t first, wistream& input)
  {
    std::wstring result = first;
    while(!input.eof() && (iswdigit(input.peek()) || input.peek() == L'.'))
      result += input.get();
    // do some validation like check for to many dots in the number
    // ...

    return new atomic_expr(stod(result));
  }
}
namespace drift
{
  block_expr* parse(std::wistream& input)
  {
    std::vector<expr*> results;

    while(!input.eof())
    {
      wchar_t first_char = input.get();

      if(iswdigit(first_char))
      {
        results.push_back(read_num(first_char, input));
      }
      else if(iswalnum(first_char))
      {
        results.push_back(read_ident_or_kw(first_char, input));
      }
    }
    return new block_expr(results);
  }
}
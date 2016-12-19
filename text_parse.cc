#include <locale>
#include <codecvt>
#include <fstream>
#include <string>
#include <sstream>
#include <cwctype>
#include <iostream>
using namespace std;

#include "ast.hh"
#include "variant.hh"
using namespace drift;

namespace
{
  void eat_whitespace(wistream& input)
  {
    wchar_t chr = input.peek();
    while(chr == L' ' || chr == L'\n' || chr == L'\t') {
      input.get();
      chr = input.peek();
    }
  }
  static const array<wchar_t*, 3> keywords =
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
  static const array<wchar_t, 5> arithmetic =
  {
    L'+',
    L'-',
    L'*',
    L'/',
    L'%',
    L'^',
    L'='
  };
  static inline is_arith(const wchar_t syntax)
  {
    for(wchar_t e : arithmetic)
      if(e == syntax)
        return true;
    return false;
  }
  static inline binary_arith* read_binop(wchar_t first_char, expr* left_op)
  {
    wstring oper = L"";
    oper += first_char;
  }
  static inline expr* read_ident_or_kw(wchar_t first, wistream& input)
  {
    std::wstring result = L"";
    result += first;
    while(!input.eof() && iswalnum(input.peek()))
      result += input.get();

    // Check if keyword, return new keywd expression else new ident expr
    if(is_keyword(result))
      return new keyword_expr(result);
    return new identifier_expr(result);
  }
  static inline atomic_expr* read_num(wchar_t first, wistream& input)
  {
    std::wstring result = L"";
    result += first;
    while(!input.eof() && (iswdigit(input.peek()) || input.peek() == L'.'))
      result += input.get();
    // do some validation like check for to many dots in the number
    // ...
    eat_whitespace(input);

    atomic_expr* numeric_expr = new atomic_expr(stod(result));

    if(is_arith(input.peek()))
      return read_binop(input.get(), numeric_expr);

    return numeric_expr;
  }
  static unary_operator* unary_op(const wchar_t op)
  {

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
      else if(is_arith(first_char))
      {
        // From this level it is likely a unary opeator
        results.push_back(unary_op(first_char, input));
      }
      else if(iswalnum(first_char))
      {
        results.push_back(read_ident_or_kw(first_char, input));
      }
    }
    return new block_expr(results);
  }
}
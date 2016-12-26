#include <locale>
#include <codecvt>
#include <fstream>
#include <string>
#include <sstream>
#include <cwctype>
#include <iostream>
using namespace std;

#include "ast.hh"
#include "../variant.hh"
using namespace drift;

namespace
{
  static inline void eat_whitespace(wistream& input)
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
  static const array<wchar_t, 7> arithmetic =
  {
    L'+',
    L'-',
    L'*',
    L'/',
    L'%',
    L'^',
    L'='
  };
  static inline bool is_arith(const wchar_t syntax)
  {
    for(wchar_t e : arithmetic)
      if(e == syntax)
        return true;
    return false;
  }
  static inline binary_arith* read_binop(wchar_t first_char, expr* left_op, wistream& input)
  {
    wstring oper = L"";
    oper += first_char;
    while(is_arith(input.peek()))
      oper += input.get();

    eat_whitespace(input);

    binary_arith* bin_node = new 

    return new binary_arith(oper, left_op, )
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
  // Read a number and return an expression
  // Will read the next bit of input:
  // If it is +,-,*,/ then it will descend into binop expr
  // If it is dot it will descend into member function call (when it exists)
  static inline expr* read_num(wchar_t first, wistream& input)
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
  static inline unary_operator* unary_op(const wchar_t op, wistream& input)
  {

  }
}
namespace drift
{
  expr* parse_expr(std::wistream& input)
  {
    wchar_t first_char = input.get();

    // Parse a numeric literal expr
    if(iswdigit(first_char))
    {
      return read_num(first_char, input);
    }
    // Parse unary operator expr
    else if(is_arith(first_char))
    {
      return unary_op(first_char, input);
    }
    // Identifier expression
    else if(iswalnum(first_char))
    {
      return read_ident_or_kw(first_char, input);
    }
    else if(input.peek() == L'(')
    {
      input.get();
      expr* exp = parse_expr(input);
      assert(input.get() == L')');
      return exp;
    }
    else if(input.peek() == L'{')
    {
      input.get();
      block_expr* block = parse(input);
      assert(input.get() == L'}');
      return block;
    }
    else
      throw std::runtime_error("Unknown expression");
  }
  block_expr* parse(std::wistream& input)
  {
    std::vector<expr*> results;

    while(!input.eof())
    {
      auto peek_char = input.peek();
      if(peek_char == L')' || peek_char == L'}')
        break;
      
      results.push_back(parse_expr(input));
    }
    return new block_expr(results);
  }
}
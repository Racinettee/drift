#include <locale>
#include <codecvt>
#include <fstream>
#include <string>
#include <sstream>
#include <cwctype>
#include <iostream>
using namespace std;

#include <assert.h>

#include "ast.hh"
#include "../variant.hh"
using namespace drift;

namespace drift
{
  block_expr* parse(std::wistream& input);
  expr* parse_expr(std::wistream& input);
}
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
      if(kw == wstring(element))
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

    expr* right_op = parse_expr(input);

    eat_whitespace(input);

    binary_arith* oper_node = new binary_arith(oper, left_op, right_op);

    if(is_arith(input.peek()))
      return read_binop(input.get(), oper_node, input);

    return oper_node;
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

    return new atomic_expr(stod(result));
  }
  static inline unary_operator* unary_op(const wchar_t op, wistream& input)
  {
    return new unary_operator(std::wstring(L"")+op, parse_expr(input));
  }
}
namespace drift
{
  expr* parse_expr(std::wistream& input)
  {
    wchar_t first_char = input.get();

    expr* result_expr = nullptr;

    // Parse a numeric literal expr
    if(iswdigit(first_char))
    {
      result_expr = read_num(first_char, input);
    }
    // Parse unary operator expr
    else if(is_arith(first_char))
    {
      result_expr = unary_op(first_char, input);
    }
    // Identifier expression
    else if(iswalnum(first_char))
    {
      result_expr = read_ident_or_kw(first_char, input);
    }
    else if(input.peek() == L'(')
    {
      input.get();
      result_expr = parse_expr(input);
      assert(input.get() == L')');
    }
    else if(input.peek() == L'{')
    {
      input.get();
      result_expr = parse(input);
      assert(input.get() == L'}');
    }
    else
      throw std::runtime_error("Unknown expression");
    
    eat_whitespace(input);

    if(is_arith(input.peek()))
      return read_binop(input.get(), result_expr, input);

    return result_expr;
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
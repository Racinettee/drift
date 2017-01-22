#include <array>
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
  ast::block_expr* parse(std::wistream& input);
  ast::expr* parse_expr(std::wistream& input, bool=true);
}
namespace
{
  using namespace drift::ast;
  static inline void eat_whitespace(wistream& input)
  {
    wchar_t chr = input.peek();
    while(chr == L' ' || chr == L'\n' || chr == L'\t') {
      input.get();
      chr = input.peek();
    }
  }
  static const array<const wchar_t*, 4> keywords =
  {
    L"let",
    L"def",
    L"class",
    L"if"
  };
  static inline bool is_keyword(const wstring& kw)
  {
    for(auto element : keywords)
      if(kw == wstring(element))
        return true;
    return false;
  }
  static const array<const wchar_t, 12> arithmetic =
  {
    L'+',
    L'-',
    L'*',
    L'/',
    L'%',
    L'^',
    L'=',
    L'&',
    L'|',
    L'>',
    L'<',
    L'!'
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

    expr* right_op = parse_expr(input, false);

    eat_whitespace(input);

    binary_arith* oper_node = new binary_arith(oper, left_op, right_op);

    if(is_arith(input.peek()))
      return read_binop(input.get(), oper_node, input);

    return oper_node;
  }
  static inline wstring get_ident(wchar_t first, wistream& input)
  {
    std::wstring result = L"";
    result += first;
    while(!input.eof() && iswalnum(input.peek()))
      result += input.get();
    return result;
  }
  static inline let_expr* handle_letexpr(wistream& input)
  {
    eat_whitespace(input);
    wchar_t first = input.get();
    assert(iswalpha(first));
    wstring ident = get_ident(first, input);
    eat_whitespace(input);
    expr* initialval_expr = nullptr;
    if(input.peek() == L'=')
      initialval_expr = parse_expr(input);
    return new let_expr(ident, initialval_expr);
  }
  static inline if_expr* handle_ifexpr(wistream& input)
  {
    eat_whitespace(input);
    assert(input.peek() == L'(');
    expr* conditional = parse_expr(input);
    eat_whitespace(input);
    assert(input.peek() == L'{');
    block_expr* body = parse(input);
    return new if_expr(conditional, body);
  }
  static inline def_expr* handle_def(wistream& input)
  {
    eat_whitespace(input);
    auto first_char = input.get();
    assert(iswalpha(first_char));
    auto ident = get_ident(first_char, input);
    // This is temporary - for the meantime just assert that opening and closing paren follow
    assert(input.get() == L'(');
    assert(input.get() == L')');
    eat_whitespace(input);
    assert(input.get() == L'{');
    block_expr* body = parse(input);
    assert(input.get() == L'}');
    return new def_expr(ident, body);
  }
  static inline expr* handle_keyword(wstring result, wistream& input)
  {
    if(result == L"let")
    {
      return handle_letexpr(input);
    }
    else if(result == L"if")
    {
      return handle_ifexpr(input);
    }
	  else if (result == L"def")
	  {
	    return handle_def(input);
	  }
    throw runtime_error("Cannot parse unknown keyword");
  }
  static inline arg_list* handle_args(wistream& input)
  {
    vector<expr*> arguments;
    arguments.push_back(parse_expr(input));
    eat_whitespace(input);
    while (input.peek() == ',')
    {
      input.get();
      arguments.push_back(parse_expr(input));
      eat_whitespace(input);
    }
    return new arg_list(arguments);
  }
  static inline expr* read_ident_or_kw(wchar_t first, wistream& input)
  {
    const wstring result = get_ident(first, input);

    // Check if keyword, return new keywd expression else new ident expr
    if(is_keyword(result))
      return handle_keyword(result, input);

    eat_whitespace(input);
    // By here we know its an identifier - check for some special cases:
    // = - do an assignment expression
    // . - do a member access - when it exists
    // () - do a method call
    if(input.peek() == L'=')
    {
      input.get();
      return new assign_expr(result, parse_expr(input));
    }
    if (input.peek() == L'(')
    {
      input.get();
      eat_whitespace(input);
      // We go to parse some args if we see that ')' isn't the next character
      arg_list* args = (input.peek() == L')' ? nullptr : handle_args(input));
      assert(input.get() == L')');
      return new function_call(result, args);
    }
    //
    // Final case - return an ident expr, pushes the value of the variable
    return new identifier_expr(result);
  }
  // Read a number and return an expression
  // Will read the next bit of input:
  // If it is +,-,*,/ then it will descend into binop expr
  // If it is dot it will descend into member function call (when it exists)
  static inline expr* read_num(wchar_t first, wistream& input)
  {
    wstring result = L"";
    result += first;
    while(!input.eof() && (iswdigit(input.peek()) || input.peek() == L'.'))
      result += input.get();

    return new atomic_expr(stod(result));
  }
  static inline expr* unary_op(const wchar_t op, wistream& input)
  {
    wstring oper = L"";
    oper += op;
    while(is_arith(input.peek()))
      oper += input.get();

    // If our unary operator is followed by a number, lets just create an atomic_expr from it
    if(iswdigit(input.peek()))
    {
      while(!input.eof() && (iswdigit(input.peek()) || input.peek() == L'.'))
        oper += input.get();
      return new atomic_expr(stod(oper));
    }
    // Otherwise we'll create a unary node - eg for '-' we'll use a neg command
    return new unary_operator(oper, parse_expr(input));
  }
  static inline atomic_expr* read_string(const wchar_t first_char, wistream& input)
  {
    string_t result = L"";
    wchar_t current;
    while ((current = input.get()) != first_char)
    {
      if (current == L'\\')
        current = input.get();
      result += current;
    }
    return new atomic_expr(result);
  }
}
namespace drift
{
  ast::expr* parse_expr(std::wistream& input, bool advance_binop)
  {
    eat_whitespace(input);
    wchar_t first_char = input.get();

    ast::expr* result_expr = nullptr;

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
    else if (first_char == L'\'' || first_char == L'"')
    {
	    result_expr = read_string(first_char, input);
    }
    // Identifier expression
    else if(iswalnum(first_char))
    {
      result_expr = read_ident_or_kw(first_char, input);
    }
    else if(first_char == L'(')
    {
      result_expr = parse_expr(input);
      assert(input.get() == L')');
    }
    else if(first_char == L'{')
    {
      result_expr = parse(input);
      assert(input.get() == L'}');
    }
    else
      throw std::runtime_error("Unknown expression");
    
    eat_whitespace(input);

    if(advance_binop)
      if(is_arith(input.peek()))
        return read_binop(input.get(), result_expr, input);

    return result_expr;
  }
  ast::block_expr* parse(std::wistream& input)
  {
    vector<expr*> results;

    while(!input.eof())
    {
      auto peek_char = input.peek();
      if(peek_char == L')' || peek_char == L'}')
        break;
      
      results.push_back(parse_expr(input));
    }
    return new ast::block_expr(results);
  }
}
#include "ast.hh"

#include <string>
using namespace std;

namespace drift
{
  void atomic_expr::emit(compile_context* cc)
  {

  }
  void unary_operator::emit(compile_context* cc)
  {

  }
  unary_operator::~unary_operator()
  {
    delete expression;
  }
  binary_arith::binary_arith(wstring op, expr* l, expr* r)
  {

  }
  binary_arith::~binary_arith()
  {
    delete left;
    delete right;
  }
  void binary_arith::emit(compile_context* cc)
  {

  }
  block_expr::~block_expr()
  {
    for(auto exp : expressions)
      delete exp;
  }
  void block_expr::emit(compile_context* cc)
  {

  }
  void keyword_expr::emit(compile_context* cc)
  {

  }
  void identifier_expr::emit(compile_context* cc)
  {

  }
}
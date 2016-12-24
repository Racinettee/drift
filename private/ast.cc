#include "ast.hh"

#include <string>
using namespace std;

namespace drift
{
  void atomic_expr::emit(context* cc)
  {

  }
  void unary_operator::emit(context* cc)
  {

  }
  unary_operator::~unary_operator()
  {
    delete symbol;
  }
  binary_arith::binary_arith(wstring op, expr* l, expr* r)
  {

  }
  void binary_arith::emit(context* cc)
  {

  }
  block_expr::~block_expr()
  {

  }
  void block_expr::emit(context* cc)
  {

  }
  void keyword_expr::emit(context* cc)
  {

  }
  void identifier_expr::emit(context* cc)
  {

  }
}
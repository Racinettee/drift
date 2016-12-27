#include "ast.hh"

#include <string>
using namespace std;

namespace drift
{
  void atomic_expr::emit(compile_context* cc)
  {
    switch(atom_kind)
    {
      case kind::dbl:
        // emit the bytes of value->num into cc's result vector
        cc->push_inst(inst::num_literal);
        for(op_t* dbl_cursor = (op_t*)&value->num; dbl_cursor < (op_t*)&value->num+sizeof(double); dbl_cursor++)
          cc->push_inst(*dbl_cursor);
        break;
      case kind::str:
        // emit the bytes into a data segment or something
        break;
    }
  }
  void unary_operator::emit(compile_context* cc)
  {

  }
  unary_operator::~unary_operator()
  {
    delete expression;
  }
  binary_arith::binary_arith(wstring op, expr* l, expr* r)
    : op(op), left(l), right(r)
  {

  }
  binary_arith::~binary_arith()
  {
    delete left;
    delete right;
  }
  void binary_arith::emit(compile_context* cc)
  {
    right->emit(cc);
    left->emit(cc);
    if(op == L"+")
      cc->program.push_back(inst::add);
    else if(op == L"-")
      cc->program.push_back(inst::sub);
    // else if etc....
  }
  block_expr::~block_expr()
  {
    for(auto exp : expressions)
      delete exp;
  }
  void block_expr::emit(compile_context* cc)
  {
    for(auto expr : expressions)
      expr->emit(cc);
  }
  void keyword_expr::emit(compile_context* cc)
  {

  }
  void identifier_expr::emit(compile_context* cc)
  {

  }
}
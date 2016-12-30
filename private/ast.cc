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
  unary_operator::unary_operator(wstring sym, expr* e)
    : symbol(sym), expression(e)
  {

  }
  unary_operator::~unary_operator()
  {
    delete expression;
  }
  void unary_operator::emit(compile_context* cc)
  {
    expression->emit(cc);
    if(symbol == L"-")
      cc->push_inst(inst::neg);
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
    left->emit(cc);
    right->emit(cc);
    if(op == L"+")
      cc->push_inst(inst::add);
    else if(op == L"-")
      cc->push_inst(inst::sub);
    else if(op == L"*")
      cc->push_inst(inst::mul);
    else if(op == L"/")
      cc->push_inst(inst::divide);
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
  let_expr::~let_expr()
  {
    delete initial;
  }
  void let_expr::emit(compile_context* cc)
  {
    unsigned int index = cc->add_variable(ident);
    if(initial)
      initial->emit(cc);
    else
      cc->push_inst(inst::nil);

    cc->push_inst(inst::store);
    for(op_t* idx_ptr = (op_t*)&index; idx_ptr < (op_t*)&index+sizeof(unsigned int); idx_ptr++)
      cc->push_inst(*idx_ptr);
  }
  void identifier_expr::emit(compile_context* cc)
  {

  }
}
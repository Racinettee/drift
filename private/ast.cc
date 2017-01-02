#include "ast.hh"

#include <string>
#include <locale>
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
        cc->push_literal<double>(value->num);
        break;
      case kind::str:
        // emit the bytes into a data segment or something
        break;
      case kind::null:
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
    delete right;
    delete left;
  }
  void binary_arith::emit(compile_context* cc)
  {
    using namespace std::string_literals;
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
    else if(op == L"<")
      cc->push_inst(inst::less_than);
    else if(op == L">")
      cc->push_inst(inst::greater_than);
    else if(op == L"==")
      cc->push_inst(inst::equals);
    else if(op == L"!=")
      cc->push_inst(inst::nequals);
    else
      throw runtime_error(cc->to_string(L"Unknown binary operator: "s + op));
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
  assign_expr::~assign_expr()
  {
    delete value;
  }
  void assign_expr::emit(compile_context* cc)
  {
    if(!cc->has_variable(ident))
      throw runtime_error("Variable: "s+cc->to_string(ident)+" has not been declared"s);
    value->emit(cc);
    cc->push_inst(inst::store);
    cc->push_literal<var_index>(cc->get_variable(ident));
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
    cc->push_literal<var_index>(index);
  }
  if_expr::~if_expr()
  {
    delete body;
    delete conditional;
  }
  void if_expr::emit(compile_context* cc)
  {
    // 1. Emit the conditional
    conditional->emit(cc);
    // 2. Jump if the condition is false to the index indicated by the following four bytes as uint
    cc->push_inst(inst::jump_false);
    // 3. Store the index of the aformentioned uint
    size_t dest_index = cc->bytes_count();
    // 4. Create storage for the uint
    cc->push_literal<unsigned int>(0);
    // 5. Emit the body
    body->emit(cc);
    // 6. Write uint now that we know where to go using an offset
    cc->write_value_bytes<unsigned int>(dest_index, cc->bytes_count() - dest_index);
  }
  void identifier_expr::emit(compile_context* cc)
  {

  }
}
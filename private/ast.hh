#pragma once
#include <string>
#include <vector>

#include "compile_context.hh"
#include "../variant.hh"

namespace drift
{
  struct expr
  {
    virtual void emit(compile_context*) = 0;
    virtual ~expr() = default;
  };
  struct atomic_expr : expr
  {
    typedef variant::element_kind kind;
    atomic_expr(double d): atom_kind(kind::dbl), value(shared_variant(d)) { }
    atomic_expr(std::wstring s): atom_kind(kind::str), value(shared_variant(s)) { }
    virtual void emit(compile_context*) override;
    std::shared_ptr<variant> value;
    kind atom_kind;
  };
  struct unary_operator : expr
  {
    unary_operator(std::wstring sym, expr* e);
    virtual ~unary_operator();
    // We will upcast to try and validate that symbol is a valid child node
    virtual void emit(compile_context*) override;
    std::wstring symbol;
    expr* expression;
  };
  struct binary_arith : expr
  {
    binary_arith(std::wstring, expr*, expr*);
    virtual ~binary_arith();
    virtual void emit(compile_context*) final;
    std::wstring op;
    expr* left;
    expr* right;
  };
  struct block_expr : expr
  {
    block_expr(std::vector<expr*> ee): expressions(ee) { }
    virtual ~block_expr();
    virtual void emit(compile_context*) override;
    std::vector<expr*> expressions;
  };
  struct assign_expr : expr
  {

  };
  struct let_expr : expr
  {
    let_expr(std::wstring ident, expr* init = nullptr): ident(ident), initial(init) { }
    virtual ~let_expr();
    virtual void emit(compile_context*) override;
    std::wstring ident;
    expr* initial;
  };
  struct identifier_expr : expr
  {
    identifier_expr(std::wstring ident): identifier(ident) { }
    virtual void emit(compile_context*) final;
    std::wstring identifier;
  };
}
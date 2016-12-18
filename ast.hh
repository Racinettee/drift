#pragma once
#include <string>
#include <vector>

#include "context.hh"
#include "variant.hh"

namespace drift
{
  struct expr
  {
    virtual void emit(context*) = 0;
    virtual ~expr() = default;
  };
  struct atomic_expr : expr
  {
    atomic_expr(double d): value(shared_variant(d)) { }
    atomic_expr(std::wstring s): value(shared_variant(s)) { }
    virtual void emit(context*) = 0;
    std::shared_ptr<variant> value;
  };
  struct binary_arith : expr
  {
    binary_arith(std::wstring, expr*, expr*);
    virtual void emit(context*) = 0;
    std::wstring op;
    expr* left;
    expr* right;
  };
  struct block_expr : expr
  {
    block_expr(std::vector<expr*> ee): expressions(ee) { }
    virtual ~block_expr();
    virtual void emit(context*) = 0;
    std::vector<expr*> expressions;
  };
  struct keyword_expr : expr
  {
    keyword_expr(std::wstring kw): keyword(kw) { }
    virtual void emit(context*) = 0;
    std::wstring keyword;
  };
  struct identifier_expr : expr
  {
    identifier_expr(std::wstring ident): identifier(ident) { }
    virtual void emit(context*) = 0;
    std::wstring identifier;
  };
}
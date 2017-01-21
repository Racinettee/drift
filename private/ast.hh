#pragma once
#include <string>
#include <vector>

#include "compile_context.hh"
#include "../variant.hh"

namespace drift
{
  namespace ast
  {
    struct expr
    {
      virtual void emit(compile_context*) = 0;
      virtual ~expr() = default;
    };
    struct atomic_expr : expr
    {
      typedef variant::element_kind kind;
      atomic_expr(double d) :
        value(shared_variant(d)), atom_kind(kind::dbl) { }
      atomic_expr(std::wstring s) :
        value(shared_variant(s)), atom_kind(kind::str) { }
      virtual void emit(compile_context*) override;
      variant_ptr value;
      kind atom_kind;
    };
    struct unary_operator : expr
    {
      unary_operator(std::wstring sym, expr* e) :
        symbol(sym), expression(e) { }
      virtual ~unary_operator();
      // We will upcast to try and validate that symbol is a valid child node
      virtual void emit(compile_context*) override;
      std::wstring symbol;
      expr* expression;
    };
    struct binary_arith : expr
    {
      binary_arith(std::wstring op, expr* l, expr* r) :
        op(op), left(l), right(r) { }
      virtual ~binary_arith();
      virtual void emit(compile_context*) final;
      std::wstring op;
      expr* left;
      expr* right;
    };
    struct block_expr : expr
    {
      block_expr(std::vector<expr*> ee) :
        expressions(ee) { }
      virtual ~block_expr();
      virtual void emit(compile_context*) override;
      std::vector<expr*> expressions;
    };
    struct assign_expr : expr
    {
      assign_expr(std::wstring ident, expr* value) :
        ident(ident), value(value) { }
      virtual ~assign_expr();
      virtual void emit(compile_context*) override;
      std::wstring ident;
      expr* value;
    };
    struct let_expr : expr
    {
      let_expr(std::wstring ident, expr* init = nullptr) :
        ident(ident), initial(init) { }
      virtual ~let_expr();
      virtual void emit(compile_context*) override;
      std::wstring ident;
      expr* initial;
    };
    struct if_expr : expr
    {
      if_expr(expr* cond, block_expr* body) :
        conditional(cond), body(body) { }
      virtual ~if_expr();
      virtual void emit(compile_context*) override;
      expr* conditional;
      block_expr* body;
    };
    struct identifier_expr : expr
    {
      identifier_expr(std::wstring ident) :
        identifier(ident) { }
      virtual void emit(compile_context*) final;
      std::wstring identifier;
    };
    struct def_expr : expr
    {
      def_expr(std::wstring name, block_expr* inst) :
        name(name), body(inst) { }
      virtual ~def_expr();
      virtual void emit(compile_context*) override;
      std::wstring name;
      block_expr* body;
    };
    struct arg_list : expr
    {
      arg_list(std::vector<expr*> args) : args(args) { }
      virtual ~arg_list();
      virtual void emit(compile_context*) override;
      std::vector<expr*> args;
    };
    struct function_call : expr
    {
      function_call(std::wstring name, arg_list* args) :
        name(name), args(args) { }
      virtual ~function_call();
      virtual void emit(compile_context*) override;
      std::wstring name;
      arg_list* args;
    };
  }
}
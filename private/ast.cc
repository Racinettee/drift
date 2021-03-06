#include "ast.hh"

#include <string>
#include <locale>
#include <iostream>
#include <sstream>
using namespace std;

namespace
{
  template<class Arg> static inline void error(ostringstream& ss, Arg arg)
  {
    ss << arg << " ";
  }
  template<class Arg, class... Args> static inline void error(ostringstream& ss, const Arg& arg, const Args&... args)
  {
    error(ss, arg);
    error(ss, args...);
  }
  template<class... Args> static inline void error(const Args&... args)
  {
    ostringstream ss("");
    ss << "Error: "s;
    error(ss, args...);
    throw runtime_error(ss.str());
  }
}

namespace drift
{
  namespace ast
  {
    void atomic_expr::emit(compile_context* cc)
    {
      switch (atom_kind)
      {
      case kind::dbl:
        // emit the bytes of value->num into cc's result vector
        cc->push_inst(inst::num_literal);
        cc->push_literal<double>(value->num);
        break;
      case kind::str:
        // emit the bytes into a data segment or something
        cc->push_inst(inst::str_literal);
        // The first four bytes before the string specify its size
        cc->push_literal<string_t>(value->str);
        break;
      case kind::null:
        break;
      }
    }
    unary_operator::~unary_operator()
    {
      delete expression;
    }
    void unary_operator::emit(compile_context* cc)
    {
      expression->emit(cc);
      if (symbol == L"-")
        cc->push_inst(inst::neg);
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
      if (op == L"+")
        cc->push_inst(inst::add);
      else if (op == L"-")
        cc->push_inst(inst::sub);
      else if (op == L"*")
        cc->push_inst(inst::mul);
      else if (op == L"/")
        cc->push_inst(inst::divide);
      else if (op == L"<")
        cc->push_inst(inst::less_than);
      else if (op == L">")
        cc->push_inst(inst::greater_than);
      else if (op == L"==")
        cc->push_inst(inst::equals);
      else if (op == L"!=")
        cc->push_inst(inst::nequals);
      else
        error(cc->to_string(L"Unknown binary operator: "s), cc->to_string(op));
      // else if etc....
    }
    block_expr::~block_expr()
    {
      for (auto exp : expressions)
        delete exp;
    }
    void block_expr::emit(compile_context* cc)
    {
      for (size_t i = 0U; i < expressions.size() - 1; i++) {
        auto expr = expressions[i];
        // This is checking for an effectless expression at block level
        if (dynamic_cast<atomic_expr*>(expr))
          continue;
        expr->emit(cc);
      }
      expressions.back()->emit(cc);
    }
    assign_expr::~assign_expr()
    {
      delete value;
    }
    void assign_expr::emit(compile_context* cc)
    {
      if (!cc->has_variable(ident))
        error("Variable: "s, cc->to_string(ident), " has not been declared"s);
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
      var_index index = cc->add_variable(ident);
      if (initial)
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
      cc->push_literal<var_index>(0U);
      // 5. Emit the body
      body->emit(cc);
      // 6. Write uint now that we know where to go using an offset
      cc->write_value_bytes<var_index>(dest_index, cc->bytes_count() - dest_index);
    }
    void identifier_expr::emit(compile_context* cc)
    {
      // Variable decl
      if (!cc->has_variable(identifier))
        error("Variable: "s, cc->to_string(identifier), " has not been declared"s);
      // Get the value on to the stack
      cc->push_inst(inst::load);
      cc->push_literal<var_index>(cc->get_variable(identifier));
    }
    def_expr::~def_expr()
    {
      delete body;
    }
    void def_expr::emit(compile_context* cc)
    {
      if (cc->has_variable(name))
        error("Redefinition of: ", cc->to_string(name));

      // Steps for setting up a function:
      // 1. Create a variable and get an index for it
      // 2. Set up a function pointer to go on the stack
      //    2.1 Push the fnc_pointer op_code
      //    2.2 Push the index
      //    2.3 Reserve space for the address (also pushing though)
      // 3. Defer filling in the address till the end
      //    In the deferred routine:
      //    3.1 Get the index of the newest byte
      //    3.2 Write the index to the reserved space
      //    3.3 Emit the functions body
      // This is required to keep the functions body from getting mixed up
      // With the body of the global scope instructions
      var_index func_index = cc->add_variable(name);

      cc->push_inst(inst::fnc_pointer);
      cc->push_literal<var_index>(func_index);
      // Create storage for the address - we have to come back here later and write the address of the function
      size_t func_address_index = cc->bytes_count();
      cc->push_literal<var_index>(0U);
      cc->push_inst(inst::store);
      cc->push_literal<var_index>(func_index);

      cc->defer_to_end([func_address_index, this](compile_context* cc) {
        size_t function_index = cc->bytes_count();
        cc->write_value_bytes<var_index>(func_address_index, function_index);
        body->emit(cc);
        cc->push_inst(inst::end);
      });
    }
    arg_list::~arg_list()
    {
      for (auto arg : args)
        delete arg;
    }
    void arg_list::emit(compile_context* cc)
    {
      for (auto arg : args)
        arg->emit(cc);
    }
    function_call::~function_call()
    {
      delete args;
    }
    void function_call::emit(compile_context* cc)
    {
      // Check that the function exists or has been declared call exists
      if (!cc->has_variable(name))
        error(cc->to_string(name), " is undefined");

      // Push arguments onto the stack
      if (args)
        args->emit(cc);

      cc->push_inst(inst::call); // Maybe we want to put a count of how many args were passed here or something...
      cc->push_literal<var_index>(cc->get_variable(name));
    }
  }
}
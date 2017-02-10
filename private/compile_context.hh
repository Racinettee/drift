#pragma once
#include <list>
#include <stack>
#include <locale>
#include <string>
#include <vector>
#include <codecvt>
#include <functional>
#include <unordered_map>

#include "types.hh"

namespace drift
{
  struct compile_context
  {
    struct dasm_State* dstate;
    program_list program;
    inline void push_inst(op_t op)
    {
      program.push_back(op);
    }
    template<class T> inline void push_literal(T literal)
    {
      size_t start = program.size();
      program.resize(program.size()+sizeof(T));
      *reinterpret_cast<T*>(&program[start]) = literal;
    }
    inline size_t bytes_count() const
    {
      return program.size();
    }
    template<class T> inline void write_value_bytes(size_t index, T value)
    {
      *reinterpret_cast<T*>(&program.at(index)) = value;
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8cnv;
    inline std::string to_string(const std::wstring& s)
    {
      return utf8cnv.to_bytes(s);
    }
    inline std::wstring to_wstring(const std::string& s)
    {
      return utf8cnv.from_bytes(s);
    }
    var_index persistent_index = 0;
    inline var_index new_index()
    {
      return persistent_index++;
    }
  private:
    class frame
    {
      compile_context* cc;
      frame* parent;
      variable_mapping variable_map;
    public:
      frame(compile_context* c, frame* p=nullptr): cc(c), parent(p) { }
      inline bool has_variable(const string_t& s)
      {
        return (variable_map.find(s) != variable_map.end() ? true : (parent ? parent->has_variable(s) : false));
      }
      inline var_index add_variable(const string_t& s)
      {
        var_index index = cc->new_index();
        variable_map[s] = index;
        return index;
      }
      inline var_index get_variable(const string_t& s)
      {
        using namespace std;
        using namespace std::string_literals;
        try {
          return variable_map.at(s);
        }
        catch(...) {
          try {
            return parent->get_variable(s);
          }
          catch(...) {
            throw std::runtime_error("Variable: "s+cc->to_string(s)+" undefined"s);
          }
        }
      }
    };
    std::stack<frame*> stack_frames;
  public:
    inline void push_frame()
    {
      stack_frames.push(new frame(this, !stack_frames.empty() ? stack_frames.top() : nullptr));
    }
    inline void pop_frame()
    {
      if(stack_frames.empty())
        throw std::runtime_error("pop_frame: cant pop empty stack");
      delete stack_frames.top();
      stack_frames.pop();
    }
    inline var_index add_variable(const string_t& name)
    {
      return stack_frames.top()->add_variable(name);
    }
    inline bool has_variable(const string_t& name)
    {
      return stack_frames.top()->has_variable(name);
    }
    inline var_index get_variable(const string_t& name)
    {
      return stack_frames.top()->get_variable(name);
    }
    typedef std::function<void(compile_context*)> deferred_routine;
    std::list<deferred_routine> deferred_routines;
    inline void defer_to_end(deferred_routine routine)
    {
      deferred_routines.push_back(routine);
    }
    inline void run_deferred_routines()
    {
      for (auto& routine : deferred_routines)
        routine(this);
      deferred_routines.clear();
    }
    compile_context(struct dasm_State* s)
    {
      dstate = s;
      push_frame();
    }
    ~compile_context()
    {
      pop_frame();
    }
  };
  template<> inline void compile_context::push_literal<string_t>(string_t literal)
  {
    // Push the size of the string here
    push_literal<var_index>(literal.size());
    size_t start = program.size();
    program.resize(program.size() + (literal.size() * sizeof(wchar_t)));
    std::copy(literal.begin(), literal.end(), (wchar_t*)&program[start]);
  }
}
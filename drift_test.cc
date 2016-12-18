#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <deque>
#include <new>
#include <locale>
#include <codecvt>
#include <initializer_list>
using namespace std;

#include "variant.hh"

enum inst : unsigned char
{
  push,
  pop,
  add,
  sub,
  mul,
  divide,
  end,
  num_literal,
  str_literal
};

using namespace drift;

typedef std::vector<inst> inst_list;
typedef std::deque<std::shared_ptr<variant>> value_stack;

value_stack stack; 
struct page // code page
{
  inst_list code;

  page(const inst_list& l): code(l) { }
  void exec(const std::vector<shared_ptr<variant>>& args);
  size_t size_bytes() const
  {
    return code.size();
  }
};

void page::exec(const std::vector<shared_ptr<variant>>& args)
{
  auto& ilist = this->code;

  for(size_t i = 0; i < size_bytes(); i++)
  {
    switch(ilist[i])
    {
      default:
        throw std::runtime_error("Unknown instruction!");
      case inst::push:
        break;
      case inst::pop:
        stack.pop_back();
        break;
      case inst::add: {
        variant_ptr right = stack.back(); stack.pop_back();
        variant_ptr left = stack.back(); stack.pop_back();
        stack.push_back(shared_variant(*left + *right));
        break;
      }
      case inst::sub:
        break;
      case inst::mul:
        break;
      case inst::divide:
        break;
      case inst::end:
        break;
      case inst::num_literal: {
        stack.push_back(shared_variant(*reinterpret_cast<double*>(&ilist[i+1])));
        i += +sizeof(double);
        break;
      }
    }
  }
}

int main() try
{
  using namespace std;

  unsigned char instructions[] = {
    inst::num_literal, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 48, 64,
    inst::num_literal, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 48, 64,
    inst::add};

  page code_page{std::vector<inst>((inst*)instructions, (inst*)instructions + sizeof instructions)};

  code_page.exec({});

  cout << (*stack.back()).num << endl;
}
catch(std::exception& e)
{
  puts(e.what());
}
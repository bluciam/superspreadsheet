#include <string>
#include <sstream>
#include <iostream>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>

namespace TL = TransLucid;

int
main(int argc, char *argv[])
{
  TL::Translator traductor;
  //traductor.loadLibrary(U"int");
  traductor.parse_header (
    U"dimension ustring<n>;;"
    U"infixl ustring<-> ustring<operator-> 5;;"
    U"infixl ustring<+> ustring<operator+> 5;;"
    U"infixl ustring<*> ustring<operator*> 10;;"
    U"library ustring<int>;;"
    U"dimension ustring<t>;;"
    U"dimension ustring<w>;;"
    U"dimension ustring<x>;;"
    U"dimension ustring<y>;;"
    U"dimension ustring<z>;;"
  );

  const size_t hmin = 5;
  const size_t vmin = 8;

  const size_t t_pivot =  8;
  const size_t w_pivot = 11;
  const size_t x_pivot =  5;
  const size_t y_pivot =  7;
  const size_t z_pivot = 42;

  const size_t horizontal_radius = 3;
  const size_t vertical_radius   = 3;

  for (size_t i = 0 ; i != 2*horizontal_radius+1  ; ++i)
  {
     for (size_t j = 0 ; j != 2*vertical_radius+1 ; ++j)
     {
       std::stringstream out;
       out << "0 @ [" ;
       out << "t:" << (i+t_pivot-horizontal_radius) << ", ";
       out << "w:" << (j+w_pivot-vertical_radius) << ", ";
       out << "x:" << x_pivot << ", ";
       out << "y:" << y_pivot << ", ";
       out << "z:" << z_pivot;
       out << "]";;
       std::string str = out.str();
       std::cout << str << std::endl;
       std::u32string expr32 (str.begin(), str.end());
       TL::HD* e = traductor.translate_expr(expr32);
     }
  }
  return 0;
}


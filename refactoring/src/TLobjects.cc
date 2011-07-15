/* TLobjects.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "TLobjects.h"

TLobjects::TLobjects()
{
  header = & traductor.header ( ) ;
  expression.clear();
  expression += '0';
}

TLobjects::TLobjects ( std::u32string header_str, std::u32string eqns_str )
{
  header = & traductor.header ( ) ;
  traductor.parse_header ( header_str );
  traductor.translate_and_add_equation_set ( eqns_str ) ;
}

TLobjects::~TLobjects()
{
}

Glib::ustring
TLobjects::calculate_expr (std::u32string tuple32)
{
  std::string s;
  TL::HD* cellContext;
  TL::TaggedConstant cellResult;
  // If translate_expr returns 0, there was an error in the translation.
  cellContext = traductor.translate_expr(tuple32);
  if (cellContext == 0)
  {
    s.clear(); s += "???";
    return s;
  }
  cellResult = (*cellContext)(TL::Tuple());
  if (cellResult.first.index() == TL::TYPE_INDEX_INTMP) {
    std::stringstream sout;
    sout << cellResult.first.value<TL::Intmp>().value();
    s = sout.str();
    std::cout << "Answer is " << s << std::endl;
  } else if (cellResult.first.index() == TL::TYPE_INDEX_SPECIAL) {
    s.clear(); s += "???";
    std::cout << "Answer is of SPECIAL type: " << std::endl; 
    cellResult.first.value<TL::Special>().print(std::cout);
    std::cout << std::endl;
  } else {
    s.clear(); s += "???";
    std::cout << "Answer is of wrong type" << std::endl;
  }
  return s;
}


// HERE! ??

int
TLobjects::get_time() 
{
    //ss << "The current time is " << (*our_system).theTime() << std::endl;

//   return (*our_system).theTime();
}

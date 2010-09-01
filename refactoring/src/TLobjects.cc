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

  // TODO: need to somehow add the dimension to the systems if ords.pivot
  // is not empty
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
  try 
  {
    cellContext = traductor.translate_expr(tuple32);
    cellResult = (*cellContext)(TL::Tuple());
  } 
  catch (...)
  {
    std::cout << "Did not translate expression or" << std::endl;
    std::cout << "Could not get the tuple from TaageddConstant." << std::endl;
    s.clear(); s += "???";
    return s;
  }
  if (cellResult.first.index() == TL::TYPE_INDEX_INTMP) {
    std::stringstream sout;
    sout << cellResult.first.value<TL::Intmp>().value();
    s = sout.str();
    std::cout << "Answer is " << s << std::endl;
  } else if (cellResult.first.index() == TL::TYPE_INDEX_SPECIAL) {
    s.clear(); s += "???";
    std::cout << "Answer is of SPECIAL type: ";
    cellResult.first.value<TL::Special>().print(std::cout);
    std::cout << std::endl;
  } else {
    s.clear(); s += "???";
    std::cout << "Answer is of wrong type" << std::endl;
  }
  return s;
}

int
TLobjects::get_time() 
{
}

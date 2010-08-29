#include "TLobjects.h"

TLobjects::TLobjects()
{
  expression.clear();
  expression += '0';
}

TLobjects::TLobjects ( std::u32string header, std::u32string eqns)
{
  traductor.parse_header ( header ) ;
  TL::equation_v eqns_set = traductor.translate_equation_set ( eqns ) ;
  std::cout << "Size of eqns vector = " << eqns_set.size() << std::endl;
}

TLobjects::~TLobjects()
{
}

Glib::ustring
TLobjects::calculate_expr (std::u32string tuple32)
{
  TL::HD* cellContext = traductor.translate_expr(tuple32);
  TL::TaggedConstant cellResult = (*cellContext)(TL::Tuple());
  std::string s;
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

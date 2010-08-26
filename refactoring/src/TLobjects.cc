#include "TLobjects.h"

TLobjects::TLobjects()
{
  expression.clear();
//  expression += '0';
  expression.assign("#x * #y") ;
  create_symbols();
}


TLobjects::~TLobjects()
{
}

bool
TLobjects::create_symbols()
{
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

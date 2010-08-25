#include <gtkmm.h> 
#include <map>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>


namespace TL = TransLucid;

class TLobjects
{
public:
  // Constructor and Destructor
  TLobjects();
  virtual ~TLobjects();

  // Members objects
  TL::Translator traductor;
  Glib::ustring expression;


  // Member methods
  bool create_symbols ();
  Glib::ustring calculate_expr (std::u32string expr); 
  int get_time ();


protected:
};

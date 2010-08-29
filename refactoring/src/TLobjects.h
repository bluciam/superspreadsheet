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
  TLobjects(std::u32string header, std::u32string eqns);
  virtual ~TLobjects();

  // Members objects
  TL::Translator traductor;
  Glib::ustring expression;
  TL::equation_v eqns_set;


  // Member methods
  Glib::ustring calculate_expr (std::u32string expr); 
  int get_time ();



protected:
};

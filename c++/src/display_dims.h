#include <gtkmm/window.h>
#include <gtkmm.h> 
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>

namespace TL = TransLucid;

class display_dims : public Gtk::ScrolledWindow
{
public:
//Constructor and Destructor
  display_dims(int row_range,
               int col_range,
               int h_min,
               int v_min,
               int type_sheet,
               std::map<Glib::ustring,int>& tuples,
               Glib::ustring* h_dim, Glib::ustring* v_dim,
               TL::Translator& traductor,
               Glib::ustring& expression);
  display_dims();  // To draw just one cell
  virtual ~display_dims();

protected:
  //Child widgets:
  Gtk::Table * table;
  Gtk::Label * label;
  Gtk::Frame * frame;

  //Signal handlers:
  // None needed?
};

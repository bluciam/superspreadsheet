#include <gtkmm/window.h>
#include <gtkmm.h> 


class display_dims : public Gtk::ScrolledWindow
{
public:
//Constructor and Destructor
  display_dims(guint row_range, guint col_range, guint h_min, guint v_min);
  virtual ~display_dims();

protected:
  //Child widgets:
  Gtk::Table * table;
  Gtk::Label * label;
  Gtk::Frame * frame;

  //Signal handlers:
  // None needed?
};

#include <gtkmm/window.h>
#include <gtkmm.h> 


class display_dims : public Gtk::ScrolledWindow
{
public:
//Constructor and Destructor
  display_dims(int row_range,
               int col_range,
               int h_min,
               int v_min,
               int type_sheet);
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

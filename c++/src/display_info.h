#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <iostream>
#include <gtkmm.h> 


class display_info : public Gtk::Frame
{
public:
//Constructor and Destructor
  display_info(std::map<Glib::ustring, int>* tuples,
               int* h_radius, int* v_radius,
               Glib::ustring* h_dim,
               Glib::ustring* v_dim);
  virtual ~display_info();

  Gtk::RadioButton hnodisplay,
                   vnodisplay,
                   * hdisplay,
                   * vdisplay;

protected:
  //Signal handlers:
  void on_h_nodim_toggled(Glib::ustring * h_dim);
  void on_v_nodim_toggled(Glib::ustring * v_dim);
  void on_h_toggled(Gtk::RadioButton * h_radio,
                    Glib::ustring    dim,
                    Glib::ustring    * h_dim,
                    Glib::ustring    * v_dim);
  void on_v_toggled(Gtk::RadioButton * v_radio,
                    Glib::ustring    dim,
                    Glib::ustring    * h_dim,
                    Glib::ustring    * v_dim);
  void on_h_spread_spin(int *);
  void on_v_spread_spin(int *);
  void on_dimension_pivot_changed(Gtk::Entry *, Glib::ustring,
                          std::map<Glib::ustring,int> * );
  void on_dimension_delete(
       Glib::ustring dim,
       std::map<Glib::ustring,int> * tuples,
       Glib::ustring    * h_dim,
       Glib::ustring    * v_dim,
       int * h_radius,
       int * v_radius);

  //Child widgets:
  Gtk::VBox * box;  // For packing label and spin buttons

  Gtk::Entry  * values;

  Gtk::SpinButton h_spread_spin,
                  v_spread_spin;
  Gtk::Adjustment h_spread_limits,
                  v_spread_limits;

  Gtk::Table   * table;

  Gtk::Label   * label;

  Gtk::Button* button;
};


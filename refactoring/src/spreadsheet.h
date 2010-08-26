#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <iostream>
#include <gtkmm.h> 
#include <map>
#include <Pivot.h>
#include <TLobjects.h>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>

namespace TL = TransLucid;

class spreadsheet : public Gtk::Window
{
public:
//Constructor and Destructor
  spreadsheet();
  virtual ~spreadsheet();

protected:
  //MAIN INTERFACE: Signal handlers:
  void on_del_dim(Glib::ustring dim);
  void on_add_dim();
  void on_redraw_clicked(Glib::ustring msg);
  void on_get_exprs();
  void on_file_name();
  void on_closebutton_clicked();


  void on_icon_pressed_exprs(Gtk::EntryIconPosition icon_pos, 
                            const GdkEventButton* event);
  void on_status_clicked(Glib::ustring msg);
  void on_infobar_status(int status);


  //MAIN INTERFACE: Child widgets:
  Gtk::VBox main_box;
  Gtk::HBox hbox_title,
            hbox_exprs,
            hbox_last;

  Gtk::HBox hbox_pivot_comp;
  Gtk::VBox vbox_new_dim;
  Gtk::VBox vbox_pivot;

  Gtk::VBox * box;  // For packing label and spin buttons in info

  Gtk::Button status_button,
              redraw_button,
              close_button;
  Gtk::Button * button;

  Gtk::Entry  exprs_entry, file_name;
  Gtk::Entry  new_dim_entry, new_pivot_entry;

  Gtk::InfoBar infoBar_status;

  Gtk::Label   label_status,
               * label,
               window_header;

  Gtk::Frame   content_frame,
               info_frame,
               * frame ;
  Gtk::HButtonBox last_box;
  Gtk::Statusbar status_bar;

  void display_pivot();

  void display_comp();
  void display_comp_all(int row_range, int col_range, int h_min, int v_min);
  void display_comp_row(int row_range, int h_min);
  void display_comp_col(int col_range, int v_min);
  void display_comp_cell();

  Gtk::Frame * display_comp_SW;
//  Gtk::ScrolledWindow * display_comp_SW;

  void on_h_nodim_toggled();
  void on_v_nodim_toggled();
  void on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim);
  void on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim);
  void on_h_spread_spin();
  void on_v_spread_spin();
  void on_dim_pivot_changed(Glib::ustring dim, Gtk::SpinButton * pivot_spin);

  Gtk::Table  *  table_comp,
              *  table_pivot;
  Gtk::RadioButton * hnodisplay,
                   * vnodisplay,
                   * hdisplay,
                   * vdisplay;

  Gtk::SpinButton * h_spread_spin,
                  * v_spread_spin;
  Gtk::Adjustment h_spread_limits,
                  v_spread_limits;

  Gtk::SpinButton * pivot_spin;
  Gtk::Adjustment * pivot_limits;

  TLobjects TLstuff;
  Pivot pivot;

  // Variables
  Glib::ustring drawn_v_dim;
  Glib::ustring drawn_h_dim;

};


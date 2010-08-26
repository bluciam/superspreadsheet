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
  void on_add_dimension(Glib::ustring msg);
  void on_add_OK(Glib::ustring msg);
  void on_del_dimension(Glib::ustring msg);
  void on_del_dim(Glib::ustring dim);
  void on_which_dimension(Glib::ustring dim);
  void on_cancel_edit(Glib::ustring msg);
  void on_icon_pressed_exprs(Gtk::EntryIconPosition icon_pos, 
                            const GdkEventButton* event);
  void on_status_clicked(Glib::ustring msg);
  void on_infobar_status(int status);
  void on_redraw_clicked(Glib::ustring msg);
  void on_get_exprs();
  void on_file_name();
  void on_closebutton_clicked();


  //MAIN INTERFACE: Child widgets:
  Gtk::VBox main_box;
  Gtk::HBox hbox_title,
            hbox_exprs,
            hbox_edit_dim,
            * hbox_del_dim,
            hbox_add_dim,
            hbox_last;
  Gtk::HPaned hpaned_content;
  Gtk::VBox * box;  // For packing label and spin buttons in info

  Gtk::Button status_button,
              cancel_button,
              redraw_button,
              close_button;
  Gtk::Button del_dim_button,
              add_dim_button,
              * button;

  Gtk::Entry  exprs_entry, file_name;
  Gtk::Entry  new_dim_entry, new_pivot_entry;

  Gtk::InfoBar infoBar_status;

  Gtk::Label   label_status,
               * label,
               window_header;

  Gtk::Frame   content_frame,
               info_frame,
               * frame,
               frame_edit_dim;
  Gtk::HButtonBox last_box;
  Gtk::Statusbar status_bar;

  // DIMS INFO INTERFACE: methods for displaying
  void display_pivot();
//  Gtk::ScrolledWindow * display_info_SW;
  Gtk::Frame * display_pivot_SW;

  // DIMS CONTENT INTERFACE: methods for displaying
  void display_comp();
  void display_comp_all(int row_range, int col_range, int h_min, int v_min);
  void display_comp_row(int row_range, int h_min);
  void display_comp_col(int col_range, int v_min);
  void display_comp_cell();


  Gtk::Frame * display_comp_SW;
//  Gtk::ScrolledWindow * display_comp_SW;

  // DIMS INFO INTERFACE: Signal handlers
  void on_h_nodim_toggled();
  void on_v_nodim_toggled();
  void on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim);
  void on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim);
  void on_h_spread_spin();
  void on_v_spread_spin();
  void on_dim_pivot_changed(Glib::ustring dim, Gtk::SpinButton * pivot_spin);
//  void on_dim_pivot_changed(Glib::ustring dim, Gtk::Entry * values);
  void on_dimension_delete(Glib::ustring dim);

  // DIMS INFO INTERFACE: entites
  Gtk::Entry  * values;
  Gtk::Table  *  table,
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


  // INTERACTION WITH TL: methods
//  void create_equations ();

  // INTERACTION WITH TL: entities
//  TL::Translator traductor;
//  Glib::ustring expression;

  TLobjects TLstuff;

  Pivot pivot;

  // Varaibles
  Glib::ustring drawn_v_dim;
  Glib::ustring drawn_h_dim;

};


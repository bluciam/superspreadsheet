/* spreadsheet.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

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
  //Child widgets:
  Gtk::VBox main_box;
  Gtk::HBox hbox_title,
            hbox_system,
            hbox_pivot_comp,
            hbox_last;
  Gtk::Statusbar status_bar;

  Gtk::VBox vbox_pivot;
  Gtk::ScrolledWindow * display_comp_SW;
 // Gtk::Frame * display_comp_SW;
  Gtk::VBox vbox_new_dim;
  Gtk::VBox * vbox;  // For packing label and spin buttons in info
  Gtk::HButtonBox last_box;

  Gtk::Table * table_comp,
             * table_pivot,
               table_system;

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

  Gtk::Button * button;
  Gtk::Button system_status_button,
              redraw_comp_button,
              close_button;

  Gtk::Entry  exprs_entry,
              eqns_entry,
              filename_eqns_entry,
              filename_header_entry,
              filename_expr_entry;

  Gtk::Entry  new_dim_entry,
              new_pivot_entry;

  Gtk::Label   label_system_status,
               * label,
               window_header;

  Gtk::Frame   pivot_frame,
               comp_frame,
               system_frame,
               * frame ; // for each cell of the table_comp

  Gtk::InfoBar infoBar_system_status;

  // Signal handlers:
  void on_reset_system();
  void on_get_expr();
  void on_filename_header();
  void on_filename_eqns();

  void on_h_nodim_toggled();
  void on_v_nodim_toggled();
  void on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim);
  void on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim);
  void on_h_spread_spin();
  void on_v_spread_spin();
  void on_dim_pivot_changed(Glib::ustring dim, Gtk::SpinButton * pivot_spin);

  void on_del_dim(Glib::ustring dim);
  void on_add_dim();

  void on_redraw_comp_clicked();
  // void on_redraw_comp_clicked(Glib::ustring msg);
  void on_closebutton_clicked();
  void on_system_status_clicked();
  void on_infobar_system_status(int status);


  // Other Methods:
  void display_pivot();
  void display_comp();
  void display_comp_all(int row_range, int col_range, int h_min, int v_min);
  void display_comp_row(int row_range, int h_min);
  void display_comp_col(int col_range, int v_min);
  void display_comp_cell();
  void display_comp_cell_nodims();

  // Data Structures and TL communications objects
  TLobjects * TLstuff;
  Pivot pivot;
  std::u32string header_32;
  std::u32string eqns_32;

};

#endif // SPREADSHEET_H

/* spreadsheet.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <iostream>
#include <gtkmm.h> 
#include <map>
#include <set>
#include <Pivot.h>
#include <List_uuid_n_content.h>
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
  Gtk::HBox title_hbox,
            system_hbox,
            new_equation_hbox,
            expressions_hbox,
            pivot_comp_hbox,
            last_hbox;
  Gtk::VBox pivot_vbox,
            new_dim_vbox,
            * vbox;  // For packing label and spin buttons in info

  Gtk::HButtonBox system_box,
                  equations_box,
                  last_box;

  Gtk::Statusbar status_bar;

  Gtk::ScrolledWindow * display_comp_SW;

  Gtk::Table * comp_table,
             * pivot_table,
               system_table,
             * expressions_table,
               equations_show_table,
//               expr_table,
             * equations_table;

  Gtk::Frame   pivot_frame,
               comp_frame,
               system_frame,
               expressions_frame,
               equations_frame,
               * frame ; // for each cell of the comp_table

  Gtk::RadioButton * hnodisplay,
                   * vnodisplay,
                   * hdisplay,
                   * vdisplay,
                   * act_exp_checkbutton;

  Gtk::SpinButton * h_spread_spin,
                  * v_spread_spin;
  Gtk::Adjustment h_spread_limits,
                  v_spread_limits;
  Gtk::SpinButton * pivot_spin;
  Gtk::Adjustment * pivot_limits;

  Gtk::Button * button;
  Gtk::Button tick_button,
              system_status_button,
              redraw_comp_button,
              close_button;

  Gtk::CheckButton * checkbutton,
                   * del_exp_checkbutton;

  Gtk::Entry  exprs_entry,
              eqn_entry,
              filename_eqns_entry,
              filename_header_entry,
              filename_expr_entry,
              * entry;
  Gtk::Entry  new_dim_entry,
              new_pivot_entry;

  Gtk::Label   label_system_status,
               * label,
               window_header;

  Gtk::InfoBar infoBar_system_status;

  // Signal handlers:
  void on_tick_time();
  void on_update_system();
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


  //TESTING FOR THE ENTITIES DISPLAY and other things
  List_uuid_n_content equations;
  void filling_equations_table();
  void on_delete_ticked_equations();
  void on_browse_headerfile();
  void on_browse_equationsfile();
  std::set<Glib::ustring> expressions;
  void filling_expressions_table();
  void on_change_expr(Gtk::Entry * entry);

  // Hooking up callbacks; to sort when the code is tested a bit more.
  void on_active_expr(Gtk::RadioButton * active, Glib::ustring expr);
  void on_delete_expr(Glib::ustring expr);

};

#endif // SPREADSHEET_H

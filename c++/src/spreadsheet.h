#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <iostream>
#include <gtkmm.h> // Needed for now for Stock Items


class spreadsheet : public Gtk::Window
{
//Constructor and Destructor
public:
  spreadsheet();
  virtual ~spreadsheet();

protected:
  //Signal handlers:
  void on_button_clicked();
  void on_dimname_clicked();
  void on_other_clicked(Glib::ustring msg);
  void on_v_nodim_toggled();
  void on_v_toggled(Gtk::RadioButton * radio);
  void on_h_nodim_toggled(), on_h_toggled();
  void on_icon_pressed_eqns(Gtk::EntryIconPosition icon_pos, 
                            const GdkEventButton* event);
  void on_commit_clicked();
  void on_infobar_commit(int commit);

  //Child widgets:
  Gtk::VBox main_box, vbox1, vbox2, vbox3;
  Gtk::HBox hbox1, hbox2, hbox3, hbox4;
  Gtk::Button* the_button;

  Gtk::RadioButton hnodisplay, vnodisplay;
  Gtk::RadioButton * hdisplay, * vdisplay;
//  Gtk::RadioButton* hdisplay;
//  Gtk::RadioButton* vdisplay;
//  Gtk::Button other_button;
  Gtk::Button quit_button,
              commit_button,
              close_button;
  Gtk::Entry  eqns_entry;
  Gtk::Entry * values;

  Gtk::Table * table;
  Gtk::ScrolledWindow scrollDimsWindow;

  Gtk::InfoBar InfoBar_commit;
  Gtk::Label   Label_commit;

  Gtk::Frame f_dim_title;
  Gtk::Label l_dim_title, ll_dim_title;
  Gtk::Label * label;
  Gtk::Label window_title;
};


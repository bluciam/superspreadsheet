#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <iostream>
#include <gtkmm.h> // Needed for now for Stock Items


class spreadsheet : public Gtk::Window
{
public:
//Constructor and Destructor
  spreadsheet();
  virtual ~spreadsheet();

protected:
  //Signal handlers:
  void on_closebutton_clicked();
  void on_v_nodim_toggled();
  void on_h_nodim_toggled();
  void on_v_toggled(Gtk::RadioButton *, Glib::ustring);
  void on_h_toggled(Gtk::RadioButton *, Glib::ustring);
  void on_icon_pressed_eqns(Gtk::EntryIconPosition icon_pos, 
                            const GdkEventButton* event);
  void on_commit_clicked(Glib::ustring msg);
  void on_infobar_commit(int commit);

  //Child widgets:
  Gtk::VBox main_box;
  Gtk::HBox hbox1,
            hbox2,
            hbox3,
            hbox4;

  Gtk::Button quit_button,
              commit_button,
              close_button;

  Gtk::Entry  eqns_entry,
              * values;

  Gtk::Table * table;
  Gtk::ScrolledWindow scrollDimsWindow;

  Gtk::InfoBar InfoBar_commit;

  Gtk::Label   Label_commit,
               * label,
               window_title;

  Gtk::Frame   * frame;
  Gtk::AspectFrame aspect_frame;
  Gtk::Alignment   center_frame;
  Gtk::RadioButton hnodisplay,
                   vnodisplay,
                   * hdisplay,
                   * vdisplay;
  Gtk::HButtonBox last_box;

  void create_equations ();
};


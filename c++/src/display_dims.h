#include <gtkmm/window.h>
#include <gtkmm.h> // Needed for now for Stock Items


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
};

/*
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


  Gtk::VBox main_box;
  Gtk::HBox hbox1,
            hbox3,
            hbox4;
  Gtk::HPaned hbox2;
  Gtk::VBox * box;  // For packing label and spin buttons

  Gtk::Button quit_button,
              commit_button,
              close_button;

  Gtk::Entry  eqns_entry,
              * values;

  Gtk::SpinButton h_spread_spin,
                  v_spread_spin;
  Gtk::Adjustment h_spread_limits,
                  v_spread_limits;
  int h_lower_limit,
      v_lower_limit,
      h_upper_limit,
      v_upper_limit;


  Gtk::InfoBar InfoBar_commit;

  Gtk::Label   Label_commit,
               * label,
               window_title;

  Gtk::Frame   * frame,
               * big_frame;
  Gtk::AspectFrame aspect_frame;
  Gtk::Alignment   center_frame;
  Gtk::RadioButton hnodisplay,
                   vnodisplay,
                   * hdisplay,
                   * vdisplay;
  Gtk::HButtonBox last_box;

  void create_equations ();
*/

#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <iostream>
#include <gtkmm.h> 
#include <display_dims.h>
#include <display_info.h>


class spreadsheet : public Gtk::Window
{
public:
//Constructor and Destructor
  spreadsheet();
  virtual ~spreadsheet();

protected:
  //Signal handlers:
  void on_closebutton_clicked();
  void on_icon_pressed_exprs(Gtk::EntryIconPosition icon_pos, 
                            const GdkEventButton* event);
  void on_commit_clicked(Glib::ustring msg);
  void on_redraw_clicked(Glib::ustring msg);
  void on_infobar_commit(int commit);

  void on_del_dimension(Glib::ustring msg);
  void on_which_dimension(Glib::ustring dim);
  void on_add_dimension(Glib::ustring msg);
  void on_add_OK(Glib::ustring msg);
  void on_cancel_edit(Glib::ustring msg);


  //Child widgets:
  Gtk::VBox main_box;
  Gtk::HBox hbox_title,
            hbox_exprs,
            hbox_edit_dim,
//            hbox_del_dim,
            * hbox_del_dim,
            hbox_add_dim,
            hbox_last;
  Gtk::HPaned hpaned_content;
  Gtk::VBox * box;  // For packing label and spin buttons in info
//  Gtk::HBox * hbox; 

  Gtk::Button quit_button,
              commit_button,
              cancel_button,
              redraw_button,
              close_button;
  Gtk::Button del_dim_button,
              add_dim_button,
              * button;

  Gtk::Entry  exprs_entry;
  Gtk::Entry  new_dim_entry, new_pivot_entry;

  display_dims * dimensions_sheet;
  display_info * info_sheet;


  Gtk::InfoBar InfoBar_commit;

  Gtk::Label   Label_commit,
               * label,
               window_title;

  Gtk::Frame   content_frame,
               info_frame,
//               * del_dim_frame,
//               add_dim_frame,
               edit_dim_frame;
  Gtk::HButtonBox last_box;

  //Functions dealing with TL entities
  void create_equations ();
};


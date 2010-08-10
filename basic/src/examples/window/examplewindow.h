#ifndef GTKMM_EXAMPLE_RANGEWIDGETS_H
#define GTKMM_EXAMPLE_RANGEWIDGETS_H

#include <gtkmm.h>

class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();
  virtual ~ExampleWindow();

protected:
  //Signal handlers:
  void on_checkbutton_toggled();
  void on_menu_position(Gtk::PositionType type);
  void on_menu_policy(Gtk::UpdateType type);
  void on_adjustment1_value_changed();
  void on_adjustment2_value_changed();
  void on_button_quit();

  //Child widgets:
  Gtk::VBox m_VBox_Top, m_VBox2, m_VBox_HScale;
  Gtk::HBox m_HBox_Scales, m_HBox_Digits, m_HBox_PageSize;

  Gtk::Adjustment m_adjustment, m_adjustment_digits, m_adjustment_pagesize;

  Gtk::VScale m_VScale;
  Gtk::HScale m_HScale, m_Scale_Digits, m_Scale_PageSize;

  Gtk::HSeparator m_Separator;

  Gtk::CheckButton m_CheckButton;

  Gtk::HScrollbar m_Scrollbar;

  Gtk::Menu m_Menu_Position, m_Menu_Policy;

  Gtk::Button m_Button_Quit;
};

#endif //GTKMM_EXAMPLE_RANGEWIDGETS_H


#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>

class spreadsheet : public Gtk::Window
{
public:
  spreadsheet();
  virtual ~spreadsheet();

protected:
  //Signal handlers:
  void on_button_clicked();

  //Child widgets:
  Gtk::VBox m_Box_Top, m_Box1, m_Box2;
  Gtk::RadioButton m_RadioButton1, m_RadioButton2, m_RadioButton3;
  Gtk::Button m_Button_Close;
};


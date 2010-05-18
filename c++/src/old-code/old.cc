#include "spreadsheet.h"


spreadsheet::spreadsheet() :
  m_Box1(false, 10),
  m_Box2(false, 10),
  m_RadioButton1("button1"),
  m_RadioButton2("button2"),
  m_RadioButton3("button3"),
  m_Button_Close("close")
{
  // Set title and border of the window
  set_title("The Super SpreadSheet, The S³");
  set_border_width(10);

  // Put radio buttons 2 and 3 in the same group as 1:
  Gtk::RadioButton::Group group = m_RadioButton1.get_group();
  m_RadioButton2.set_group(group);
  m_RadioButton3.set_group(group);

  // Add outer box to the window (because the window
  // can only contain a single widget)
  add(m_Box_Top);

  //Put the inner boxes and the separator in the outer box:
  m_Box_Top.pack_start(m_Box1);
  m_Box_Top.pack_start(m_Box2);

  // Set the inner boxes' borders
  m_Box2.set_border_width(10);
  m_Box1.set_border_width(10);

  // Put the radio buttons in Box1:
  m_Box1.pack_start(m_RadioButton1);
  m_Box1.pack_start(m_RadioButton2);
  m_Box1.pack_start(m_RadioButton3);

  // Set the second button active
  m_RadioButton2.set_active();

  // Put Close button in Box2:
  m_Box2.pack_start(m_Button_Close);

  // Make the button the default widget
  m_Button_Close.set_can_default();
  m_Button_Close.grab_default();

  // Connect the clicked signal of the button to
  // spreadsheet::on_button_clicked()
  m_Button_Close.signal_clicked().connect(sigc::mem_fun(*this,
              &spreadsheet::on_button_clicked) );

  // Show all children of the window
  show_all_children();
}

spreadsheet::~spreadsheet()
{
}

void spreadsheet::on_button_clicked()
{
  hide(); //to close the application.
}


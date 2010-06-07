#include <gtkmm/main.h>
#include "spreadsheet.h"

int main(int argc, char *argv[])
{
//  Gtk::Main::init_gtkmm_internals();

  Gtk::Main* kit = new Gtk::Main(argc, argv);
  spreadsheet* sss = new spreadsheet;

  //Shows the spreadsheet window and returns when it is closed.
  Gtk::Main::run(*sss);
  delete sss;
  delete kit;

//  Gtk::Main kit(argc, argv);
//  spreadsheet sss;
//  Gtk::Main::run(sss);


  return 0;
}


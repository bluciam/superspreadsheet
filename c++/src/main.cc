#include <gtkmm/main.h>
#include "spreadsheet.h"

int main(int argc, char *argv[])
{
  Gtk::Main* kit = new Gtk::Main(argc, argv);

  spreadsheet* sss = new spreadsheet;
  //Shows the spreadsheet window and returns when it is closed.
  Gtk::Main::run(*sss);
  delete sss;
  delete kit;

  return 0;
}


#include <gtkmm/main.h>
#include "spreadsheet.h"

int main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  spreadsheet spreadsheet;
  //Shows the spreadsheet window and returns when it is closed.
  Gtk::Main::run(spreadsheet);

  return 0;
}


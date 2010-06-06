#include "display_dims.h"
#include <cstdio>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>

// Constructor
display_dims::display_dims( guint row_range, guint col_range, guint h_min, guint v_min)
{

  // Begin second table, which is within a Gtk::ScrolledWindow
/*
  scrollDimsWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  scrollDimsWindow.set_size_request(400,200); // Set minimum size of widget
*/
 std::cout << "Checking on values in display_dims:\n h_min = " 
           << h_min << "\n v_min = " <<
            v_min << "\n row_range = " << row_range << "\n col_range "
            << col_range << std::endl;


  set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  set_size_request(400,200); // Set minimum size of widget

  table = Gtk::manage(new Gtk::Table(row_range + 1, col_range + 1, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);
  add(*table);

/*
  big_frame = Gtk::manage(new Gtk::Frame);
  big_frame->set_shadow_type(Gtk::SHADOW_IN);
  big_frame->add(scrollDimsWindow);
  scrollDimsWindow.add(*table);
  hbox2.pack2(*big_frame);
*/

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices"); 
  (*table).attach(*label, 0, 1, 0, 1);
//  int value = tuples[h_dim] - h_radius;    // h_min
  for (int i = 1 ; i != row_range + 1; ++i)
  {
    std::string s;
    std::stringstream out;
    out << h_min + i - 1;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s; 
    (*label).set_label(cell); 
    (*table).attach(*label, i, i+1, 0, 1);
  }

//   value = tuples[v_dim] - v_radius;     // v_min
  for (int i = 1 ; i != col_range + 1; ++i)
  {
    std::string s;
    std::stringstream out;
    out << v_min + i - 1;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s; 
    (*label).set_label(cell); 
    (*table).attach(*label, 0, 1, i, i+1);

  }

  /* For now just labels representing the values of each cell.
   */
  for (int i = 1 ; i != row_range + 1 ; ++i)
  {
     for (int j = 1 ; j != col_range + 1 ; ++j)
     {
       std::string s;
       std::stringstream out;
       out << i << " " << j;
       s = out.str();

       label = Gtk::manage(new Gtk::Label);
       frame = Gtk::manage(new Gtk::Frame);
       Glib::ustring cell = s; // Name to come from TL HD
       (*label).set_label(cell); 
       (*frame).add(*label);
       (*table).attach(*frame, i, i+1, j, j+1);
     }
  }
  // Show all children of the window
  show_all_children();
}


display_dims::~display_dims()
{
  // Trying to get rid of the problem of not deallocating memory.
  Glib::Error::register_cleanup();
  Glib::wrap_register_cleanup();
}



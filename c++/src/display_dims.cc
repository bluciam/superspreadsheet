#include "display_dims.h"
#include <cstdio>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>

// Constructor
display_dims::display_dims( int row_range,
                            int col_range,
                            int h_min,
                            int v_min,
                            int type_sheet)
{

 std::cout << "Checking on values in display_dims:\n h_min = " 
           << h_min << "\n v_min = "
           << v_min << "\n row_range = " << row_range << "\n col_range "
           << col_range << std::endl;

  set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  set_size_request(300,200); // Set minimum size of widget

  table = Gtk::manage(new Gtk::Table(row_range + 1, col_range + 1, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);
  add(*table);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices"); 
  (*table).attach(*label, 0, 1, 0, 1);
//  for (int i = 1 ; i != row_range + 1; ++i)
  if (type_sheet != 2) {   // only the vertical dimension
    for (int i = 0 ; i < row_range ; ++i)
    {
      std::string s;
      std::stringstream out;
      out << h_min + i ;
//      out << h_min + i - 1;
      s = out.str();
      label = Gtk::manage(new Gtk::Label);
      Glib::ustring cell = s; 
      (*label).set_label(cell); 
      (*table).attach(*label, i+1, i+2, 0, 1);
    }
  }

//  for (int i = 1 ; i != col_range + 1; ++i)
  if (type_sheet != 3) {   // only the horizontal  dimension
    for (int i = 0 ; i < col_range ; ++i)
    {
      std::string s;
      std::stringstream out;
      out << v_min + i ;
//      out << v_min + i - 1;
      s = out.str();
      label = Gtk::manage(new Gtk::Label);
      Glib::ustring cell = s; 
      (*label).set_label(cell); 
      (*table).attach(*label, 0, 1, i+1, i+2);
//    (*table).attach(*label, 0, 1, i, i+1);

    }
  }

//  for (int i = 1 ; i != row_range + 1 ; ++i)
  for (int i = 0 ; i != row_range  ; ++i)
  {
     for (int j = 0 ; j != col_range ; ++j)
//     for (int j = 1 ; j != col_range + 1 ; ++j)
     {
       std::string s;
       std::stringstream out;
       out << i << " " << j;
       s = out.str();

       label = Gtk::manage(new Gtk::Label);
       frame = Gtk::manage(new Gtk::Frame);
       Glib::ustring cell = s; 
       (*label).set_label(cell); 
       (*frame).add(*label);
       (*table).attach(*frame, i+1, i+2, j+1, j+2);
//       (*table).attach(*frame, i, i+1, j, j+1);
     }
  }
  // Show all children of the window
  show_all_children();

}

display_dims::display_dims( int type_sheet)
{

//  set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  table = Gtk::manage(new Gtk::Table( 2, 2, false));
  add(*table);
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices"); 
  (*table).attach(*label, 0, 1, 0, 1);

  std::string s;
  std::stringstream out;
  out << 0 << " " << 0;
  s = out.str();
  label = Gtk::manage(new Gtk::Label);
  frame = Gtk::manage(new Gtk::Frame);
  Glib::ustring cell = s; 
  (*label).set_label(cell); 
  (*frame).add(*label);
  (*table).attach(*frame, 1, 2, 1, 2);
  show_all_children();

}

display_dims::~display_dims()
{
  // Trying to get rid of the problem of not deallocating memory.
  Glib::Error::register_cleanup();
  Glib::wrap_register_cleanup();
}



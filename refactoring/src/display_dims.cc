#include <cstdio>
#include "display_dims.h"

// Constructor
display_dims::display_dims( int row_range,
                            int col_range,
                            int h_min,
                            int v_min,
                            int type_sheet,
			    std::map<Glib::ustring,int>& tuples,
			    Glib::ustring* h_dim, Glib::ustring* v_dim,
                            TL::Translator& traductor,
                            Glib::ustring& expression
                            ) // 1 = 2D, 2 = V only, 3 = H only
{

/*
 std::cout << "Checking on values in display_dims:\n h_min = " 
           << h_min << "\n v_min = "
           << v_min << "\n row_range = " << row_range << "\n col_range "
           << col_range << std::endl;
*/

  set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
//  set_size_request(300,200); // Set minimum size of widget

  table = Gtk::manage(new Gtk::Table(row_range + 1, col_range + 1, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);
  add(*table);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices"); 
  (*table).attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
  if (type_sheet != 2) {   // only the vertical dimension
    //  Drawing horizontal index 
    for (int i = 0 ; i != row_range ; ++i)
    {
      std::string s;
      std::stringstream out;
      out << h_min + i ;
      s = out.str();
      label = Gtk::manage(new Gtk::Label);
      Glib::ustring cell = s; 
      (*label).set_label(cell); 
      (*table).attach(*label, i+1, i+2, 0, 1, Gtk::FILL, Gtk::FILL);
    }
  }

  if (type_sheet != 3) {   // only the horizontal  dimension
    //  Drawing vertical index 
    for (int i = 0 ; i < col_range ; ++i)
    {
      std::string s;
      std::stringstream out;
      out << v_min + i ;
      s = out.str();
      label = Gtk::manage(new Gtk::Label);
      Glib::ustring cell = s; 
      (*label).set_label(cell); 
      (*table).attach(*label, 0, 1, i+1, i+2, Gtk::FILL, Gtk::FILL);

    }
  }

  // Drawing content
  for (int i = 0 ; i != row_range  ; ++i)
  {
     for (int j = 0 ; j != col_range ; ++j)
     {
       std::string s;
       std::stringstream out;
       out << (i+h_min) << " " << (j+v_min);
       s = out.str();

       label = Gtk::manage(new Gtk::Label);
       frame = Gtk::manage(new Gtk::Frame);
       Glib::ustring cell = s; 
       (*label).set_label(cell); 
       (*frame).add(*label);
       (*table).attach(*frame, i+1, i+2, j+1, j+2, Gtk::FILL, Gtk::FILL);
//       std::cout << "expression = " << expression << std::endl;
//       std::stringstream newout;
//       newout << expression;
//       newout << " @ [";
//       newout << *h_dim << ":" << (i+h_min) << ", ";
//       newout << *v_dim << ":" << (j+v_min);
//       for (std::map<Glib::ustring,int>::iterator mit = tuples.begin();
//            mit != tuples.end(); ++mit)
//       {
//         if (mit->first != *h_dim && mit->first != *v_dim)
//         {
//	   newout << ", " << mit->first << ":" << mit->second;
//         }
//       }
//       newout << "]";
//       std::cout << newout.str() << std::endl;
//       std::u32string tuple32;
//       for (std::string::iterator it = newout.str().begin();
//            it != newout.str().end(); ++it)
//       {
//         int i = *it;
//         char32_t c = i;
//         tuple32.push_back(c);
//       }
//       tuple32.push_back(0);
//       TL::HD* cellContext = traductor.translate_expr(tuple32);
       //std::cout << newout.str() << std::endl;
     }
  }
  // Show all children of the window
  show_all_children();
}

display_dims::display_dims()  // single cell
{

//  set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  table = Gtk::manage(new Gtk::Table( 2, 2, false));
  add(*table);
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices"); 
  (*table).attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL, 0,0);

  std::string s;
  std::stringstream out;
  out << 0 << " " << 0;
  s = out.str();
  label = Gtk::manage(new Gtk::Label);
  frame = Gtk::manage(new Gtk::Frame);
  Glib::ustring cell = s; 
  (*label).set_label(cell); 
  (*frame).add(*label);
  (*table).attach(*frame, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL);
  show_all_children();

}

display_dims::~display_dims()
{
  // Trying to get rid of the problem of not deallocating memory.
  Glib::Error::register_cleanup();
  Glib::wrap_register_cleanup();
}



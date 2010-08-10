#include "display_info.h"
#include <cstdio>
#include <map>


display_info::display_info(std::map<Glib::ustring, int>* tuples,
                           int* h_radius, 
                           int* v_radius,
                           Glib::ustring* h_dim,
                           Glib::ustring* v_dim ) :

  h_spread_limits(0.0, 0.0, 20.0, 1.0, 3.0, 0.0), // Gtk::Adjustment(
  v_spread_limits(0.0, 0.0, 20.0, 1.0, 3.0, 0.0)  //      initial_value, 
                                                  //      lower, upper,
                                                  //      step_increment, 
                                                  //      page_increment, 
                                                  //      page_size);
{

/*
std::cout << "Entering display_info.cc" << std::endl;
std::cout << "Data: dimensions = " << (*tuples).size()
          << "\n  h_radius " << (*h_radius)  
          << "\n  v_radius " << (*v_radius)  
          << "\n  h_dim " << (*h_dim)  
          << "\n  v_dim " << (*v_dim)  
          << std::endl;
for ( it=(*tuples).begin() ; it != (*tuples).end(); it++ )
  std::cout << (*it).first << " => " << (*it).second << std::endl;
*/

  std::map<Glib::ustring,int>::iterator it;

  h_spread_limits.set_value(*h_radius);
  h_spread_spin.set_adjustment(h_spread_limits);
  h_spread_spin.set_size_request(50, -1);
  h_spread_spin.set_numeric(true);
  h_spread_spin.set_alignment(1);

  v_spread_limits.set_value(*v_radius);
  v_spread_spin.set_adjustment(v_spread_limits);
  v_spread_spin.set_size_request(50, -1);
  v_spread_spin.set_numeric(true);
  v_spread_spin.set_alignment(1);

// Begin table
  table = Gtk::manage(new Gtk::Table(((*tuples).size()+2), 4, false));
  (*table).set_col_spacings(10);
  add(*table);

  // Column titles
  label = manage(new Gtk::Label);
  (*label).set_label("Dimension");
  (*table).attach((*label),0,1,0,1);

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(h_spread_spin);
  (*label).set_label("H dim\nradius");
  (*table).attach((*box),1,2,0,1);

  h_spread_spin.signal_value_changed().connect(
    sigc::bind(
      sigc::mem_fun(
                *this, &display_info::on_h_spread_spin ), h_radius ) );

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(v_spread_spin);
  (*label).set_label("V dim\nradius");
  (*table).attach((*box),2,3,0,1);

  v_spread_spin.signal_value_changed().connect(
    sigc::bind (
      sigc::mem_fun(
                *this, &display_info::on_v_spread_spin ), v_radius ) );

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Pivot");
  (*table).attach((*label),3,4,0,1);

/*
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Delete\ndim");
  (*table).attach((*label),4,5,0,1);
*/

  // First row: no dimension chosen
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("None");
  (*table).attach((*label),0,1,1,2);

  Gtk::RadioButton::Group hgroup = (hnodisplay).get_group();
  (*table).attach((hnodisplay),1,2,1,2);
  (hnodisplay).signal_toggled().connect(
    sigc::bind (
      sigc::mem_fun( *this, &display_info::on_h_nodim_toggled ), h_dim ) );

  Gtk::RadioButton::Group vgroup = (vnodisplay).get_group();
  (*table).attach((vnodisplay),2,3,1,2);
  (vnodisplay).signal_toggled().connect(
     sigc::bind (
       sigc::mem_fun( *this, &display_info::on_v_nodim_toggled ), v_dim ) );


//  it=(*tuples).begin();
//  (*h_dim) = (*(it++)).first;
//  (*v_dim) = (*it).first;

  /* Following rows: dimension names and radio buttons in two groups:
      H and V and the pivot value.
  */
  int i = 0; // Can't put this in the for loop, it explodes. 
  for ( it=(*tuples).begin(); it != (*tuples).end(); ++it, ++i )
  {
    int ii = i+3;

    label = Gtk::manage(new Gtk::Label);
    Glib::ustring dim = (*it).first;
    (*label).set_label(dim); 
    (*table).attach( (*label), 0, 1, i+2, ii);

    hdisplay = Gtk::manage(new Gtk::RadioButton);
    (*hdisplay).set_group(hgroup);
    (*table).attach( (*hdisplay), 1, 2, i+2, ii);
    (*hdisplay).signal_toggled().connect(
       sigc::bind( 
         sigc::mem_fun(*this, &display_info::on_h_toggled),
           hdisplay, dim, h_dim, v_dim ) );

    vdisplay = Gtk::manage(new Gtk::RadioButton);
    (*vdisplay).set_group(vgroup);
    (*table).attach((*vdisplay),2,3,i+2,ii);
    (*vdisplay).signal_toggled().connect(
       sigc::bind( 
         sigc::mem_fun(*this, &display_info::on_v_toggled),
            vdisplay, dim, h_dim, v_dim ) );

    if ((*h_dim) == dim) {
      (*hdisplay).set_active();
    } else if ((*v_dim) == dim) {
      (*vdisplay).set_active();
    }

    values = Gtk::manage(new Gtk::Entry);
    values->set_alignment(1);
    std::string s;
    std::stringstream out;
    out << (*it).second ;
    s = out.str();
    (*values).set_size_request(50, -1);
    (*values).set_text(s);
    (*table).attach((*values),3,4,i+2,ii);
//    values->signal_activate().connect( sigc::bind (
    values->signal_changed().connect( sigc::bind (
       sigc::mem_fun (
            *this, &display_info::on_dimension_pivot_changed ),
            values, dim, tuples ) );

/*
    button = Gtk::manage(new Gtk::Button(dim));
    (*table).attach((*button),4,5,i+2,ii);
    button->signal_clicked().connect( sigc::bind (
      sigc::mem_fun (    
            *this, &display_info::on_dimension_delete ),
            dim, tuples, h_dim, v_dim , h_radius, v_radius ) ) ;
*/

  }
// End table

  // Show all children of the window
  show_all_children();

}


display_info::~display_info()
{
  Glib::Error::register_cleanup();
  Glib::wrap_register_cleanup();

}

void
display_info::on_h_spread_spin(int * h_radius)
{
  (*h_radius) = h_spread_spin.get_value();
  std::cout << (*h_radius) << std::endl;
  h_spread_limits.set_value(*h_radius);
}

void
display_info::on_v_spread_spin(int * v_radius)
{
  (*v_radius) = v_spread_spin.get_value();
  std::cout << (*v_radius) << std::endl;
  v_spread_limits.set_value(*v_radius);
}

/*
void
display_info::on_dimension_delete( Glib::ustring dim,
                                   std::map<Glib::ustring,int> * tuples,
                                   Glib::ustring * h_dim,
                                   Glib::ustring * v_dim,
                                   int * h_radius,
                                   int * v_radius)
{
  (*tuples).erase (dim);
  this->remove();
  display_info * info_sheet = Gtk::manage(new display_info(
    (*tuples).size(), tuples, h_radius, v_radius, h_dim, v_dim ) );

  if ((*h_dim) == dim) hnodisplay.set_active();
  if ((*v_dim) == dim) vnodisplay.set_active();
 (*info_sheet).show();
}
*/

/*
  for ( it=tuples.begin() ; it != tuples.end(); it++ )
      std::cout << (*it).first << " => " << (*it).second << std::endl;
*/

void
display_info::on_dimension_pivot_changed(Gtk::Entry * values,
                                         Glib::ustring dim,
                                         std::map<Glib::ustring,int> * tuples)
{
  Glib::ustring s_pivot = values->get_text ();
  std::cout << s_pivot << std::endl;
  int pivot =  atoi(s_pivot.c_str()); 
  (*tuples)[dim] = pivot;

/*
  for ( it=tuples.begin() ; it != tuples.end(); it++ )
      std::cout << (*it).first << " => " << (*it).second << std::endl;
*/
}

void
display_info::on_h_nodim_toggled(Glib::ustring * h_dim)
{
  if ((hnodisplay).get_active()) {
    std::cout << "No dimension chosen for horizontal display."<< std::endl;
    (*h_dim) = "";
    std::cout << "h_dim = " << (*h_dim) << std::endl;
  }
}

void
display_info::on_v_nodim_toggled(Glib::ustring * v_dim)
{
  if ((vnodisplay).get_active()) {
    std::cout << "No dimension chosen for vertical display." << std::endl;
    (*v_dim) = "";
    std::cout << "v_dim = " << (*v_dim) << std::endl;
  }
}

void
display_info::on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim,
                           Glib::ustring * h_dim,
                           Glib::ustring * v_dim)
{
  if ((*v_radio).get_active()) { // When a dim is chosen
    if ((*h_dim) == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      vnodisplay.set_active();
    } else {
      (*v_dim) = dim; 
      std::cout << dim << " chosen for vertical display."<< std::endl;
    }
  } // When a dim is released
}

void
display_info::on_h_toggled(Gtk::RadioButton * h_radio,
                           Glib::ustring dim,
                           Glib::ustring * h_dim,
                           Glib::ustring * v_dim)
{
  if ((*h_radio).get_active()) { // When a dim is chosen
    if ((*v_dim) == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      hnodisplay.set_active();
    } else {
      (*h_dim) = dim; 
      std::cout << dim << " chosen for horizontal display."<< std::endl;
    }
  } // When a dim is released
}

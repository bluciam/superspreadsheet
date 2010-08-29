#include <cstdio>
#include <iostream>
#include <fstream>
#include "spreadsheet.h"

spreadsheet::spreadsheet() :
  // false: child widgets don't have the same width
  //    10: distance in pixels between widgets
  main_box  (false, 10),
  hbox_title (false, 10),
  hbox_exprs (false, 10),
  hbox_eqns (false, 10),
  hbox_last (false, 10),
  window_header("The TransLucid objects browser" ),

  table_system (3,3,false),

  // Gtk::Adjustment(
  //   initial_value, lower, upper, step_increment, page_increment, page_size);
  h_spread_limits(0.0, 0.0, 20.0, 1.0, 1.0, 0.0),
  v_spread_limits(0.0, 0.0, 20.0, 1.0, 1.0, 0.0),

  // last_button is Gtk::HButtonBox for equal spacing (30) between buttons.
  last_box(Gtk::BUTTONBOX_END, 30),

  status_button(" _Show Status ",true), 
  redraw_button(" _Redraw SpreadSheet  ",true), 
  close_button("_Quit", true)       // to use alt-C to close app

{

  // Set title and size of the SuperSpreadSheet main window
  set_title("The Super SpreadSheet, The S³");
  // Add outer box to the window since it may only contain a single widget 
  add(main_box);
  main_box.set_size_request(800,600);
  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox_title, false, false, 0);
//  main_box.pack_start(hbox_exprs, false, false, 0);
//  main_box.pack_start(hbox_eqns, false, false, 0);
//  main_box.pack_start(table_system,false,false,0);
  main_box.pack_start(hbox_system,false,false,0);
  main_box.pack_start(hbox_pivot_comp,true, true, 0);
  main_box.pack_start(hbox_last, false, false, 0);
  main_box.pack_start(status_bar, false, false, 0);

  // Set the boxes' spacing around the outside of the container to 5 pixels
  hbox_title.set_border_width(5); 
  hbox_exprs.set_border_width(5);
  hbox_eqns.set_border_width(5);
  hbox_last.set_border_width(5);

// Begin hbox_title 
  // hbox_title.pack_start(rule);
  hbox_title.pack_start(window_header);
// End   hbox_title 


// Begin hbox_exprs 

  TLstuff = new TLobjects();
  //  exprs_entry.set_max_length(50);
  label = Gtk::manage(new Gtk::Label("Expression"));
//  hbox_exprs.pack_start(*label);

(table_system).attach((*label),0,1,0,1,Gtk::FILL,Gtk::FILL);

  exprs_entry.set_text((*TLstuff).expression);
  exprs_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  exprs_entry.signal_icon_press().connect(
    sigc::mem_fun(*this, &spreadsheet::on_icon_pressed_exprs) );
  exprs_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_get_exprs) );
//  hbox_exprs.pack_start(exprs_entry);

(table_system).attach((exprs_entry),1,2,0,1,Gtk::FILL,Gtk::FILL);


  label = Gtk::manage(new Gtk::Label("  Or enter an expression file to load"));
  hbox_exprs.pack_start(*label);
  hbox_exprs.pack_start(filename_expr_entry);
  filename_expr_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_file_name) );

  button = Gtk::manage(new Gtk::Button("Or browse"));
  hbox_exprs.pack_start(*button);
// End hbox_exprs 

// Begin hbox_eqns 

hbox_system.pack_start(table_system);
  label = Gtk::manage(new Gtk::Label("Header file "));
(table_system).attach((*label),0,1,1,2,Gtk::FILL,Gtk::FILL);

//  hbox_eqns.pack_start(*label);
//  hbox_eqns.pack_start(filename_header_entry);
(table_system).attach((filename_header_entry),1,2,1,2,Gtk::FILL,Gtk::FILL);

  filename_header_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_filename_header) );

  label = Gtk::manage(new Gtk::Label("Equations file "));
(table_system).attach((*label),0,1,2,3,Gtk::FILL,Gtk::FILL);
//  hbox_eqns.pack_start(*label);
//  hbox_eqns.pack_start(filename_eqns_entry);
(table_system).attach((filename_eqns_entry),1,2,2,3,Gtk::FILL,Gtk::FILL);
  filename_eqns_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_filename_eqns) );

button = Gtk::manage(new Gtk::Button("Reset system"));
//(table_system).attach((*button),2,3,2,3,Gtk::FILL,Gtk::FILL);
hbox_system.pack_end(*button);
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_reset_system) );



// End hbox_eqns 

// Begin hbox_pivot_comp
  // Begin pivot_frame, vbox_pivot
    // Begin table_pivot
  display_pivot();
  pivot_frame.set_shadow_type(Gtk::SHADOW_IN);
  pivot_frame.add(((vbox_pivot)));
  hbox_pivot_comp.pack_start(pivot_frame,false,true,0);
    // End table_pivot

    // Begin vbox_new_dim
  Gtk::HBox * hbox = Gtk::manage(new Gtk::HBox);
  label = Gtk::manage(new Gtk::Label("Dim Name "));
  (*hbox).pack_start(*label); 
  (*hbox).pack_start(new_dim_entry); 
  vbox_new_dim.pack_start(*hbox, false, false, 0);

  hbox = Gtk::manage(new Gtk::HBox);
  label = Gtk::manage(new Gtk::Label("Pivot Value"));
  (*hbox).pack_start(*label); 
  (*hbox).pack_start(new_pivot_entry); 
  vbox_new_dim.pack_start(*hbox, false, false, 0);

  button = Gtk::manage(new Gtk::Button("_Add New Dimension",true));
  vbox_new_dim.pack_start(*button);
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_add_dim) );

  (vbox_pivot).pack_end(vbox_new_dim, false, false, 0);
    // End vbox_new_dim
  // End pivot_frame, vbox_pivot

  // Begin comp_frame
  display_comp();
  drawn_h_dim = pivot.h_dim;
  drawn_v_dim = pivot.v_dim;
  comp_frame.set_shadow_type(Gtk::SHADOW_IN);
  comp_frame.add(*display_comp_SW);
//  hbox_pivot_comp.pack_start(comp_frame,false,false);
  hbox_pivot_comp.pack_start(comp_frame,true,true);
  // End comp_frame
// End hbox_pivot_comp



// Begin hbox_last 
  // InfoBar is taken from example, not fully understood.
  Gtk::Container* infoBarContainer =
    dynamic_cast<Gtk::Container*>(infoBar_status.get_content_area());
  if (infoBarContainer) infoBarContainer->add(label_status);
  infoBar_status.add_button(Gtk::Stock::OK, 0);
  infoBar_status.set_message_type(Gtk::MESSAGE_INFO);
  infoBar_status.signal_response().connect(
    sigc::mem_fun(*this, &spreadsheet::on_infobar_status ) );
  hbox_last.pack_start(infoBar_status, Gtk::PACK_SHRINK);

//  last_box.add(status_button);
  status_button.signal_clicked().connect(
    sigc::bind<Glib::ustring> (
      sigc::mem_fun(
        *this, &spreadsheet::on_status_clicked), "Showing current status") );

//  last_box.add(redraw_button);
  redraw_button.signal_clicked().connect(
    sigc::bind<Glib::ustring> (
      sigc::mem_fun( *this, &spreadsheet::on_redraw_clicked),
                     "Redrawing the spreadsheet") );

  last_box.add(close_button);
  close_button.signal_clicked().connect (
    sigc::mem_fun( *this, &spreadsheet::on_closebutton_clicked) );

  hbox_last.pack_start(last_box);
  // Make the button the default widget. No idea what this 
  // does, as the explanation does not match the functionality.
  close_button.set_can_default();
  close_button.grab_default();
// End hbox_last 

  show_all_children();
  infoBar_status.hide(); // InfoBar shown only when status button is pressed
} // End spreadsheet::spreadsheet()



void
spreadsheet::on_add_dim()
{

  Glib::ustring dimname  = new_dim_entry.get_text();
  Glib::ustring dimpivot = new_pivot_entry.get_text();
  std::cout << "  Name: "  << dimname << std::endl;
  std::cout << "  Pivot: " << dimpivot << std::endl;
  if (dimname == "" && dimpivot == "")
  { 
    return;
  }
  else if (dimname == "" || dimpivot == "")
  { 
    new_dim_entry.set_text("");
    new_pivot_entry.set_text("");
    vbox_new_dim.show();
    return;
  }
  else
  {
    new_dim_entry.set_text("");
    new_pivot_entry.set_text("");
    int piv =  atoi(dimpivot.c_str());
    (pivot.ords)[dimname] = piv;

    (vbox_pivot).remove(*table_pivot);
    display_pivot();
    (vbox_pivot).show_all_children();
    (vbox_pivot).show();
    vbox_new_dim.show();
  }
}


void
spreadsheet::on_del_dim(Glib::ustring dim)
{

  (pivot.ords).erase (dim);
  if (dim == (pivot.h_dim)) {
    (pivot.h_dim.clear() ) ;
    (*hnodisplay).toggled();
  } else if (dim == (pivot.v_dim)) {
    (pivot.v_dim.clear() )  ;
    (*vnodisplay).toggled();
  } 


  (vbox_pivot).remove(*table_pivot);
  display_pivot();
  (vbox_pivot).show_all_children();
  (vbox_pivot).show();

  status_bar.push("Deleted dimension " + dim);
  std::cout << "Deleted dimension " << dim << std::endl;
//TODO: do I have to redraw?

}

void
spreadsheet::on_redraw_clicked(Glib::ustring msg)
{
  display_comp();
  drawn_h_dim = (pivot.h_dim);
  drawn_v_dim = (pivot.v_dim);
  comp_frame.remove();
  comp_frame.add(*display_comp_SW);
  (*display_comp_SW).show_all_children();
//  status_bar.push(msg);
}

void
spreadsheet::on_reset_system()
{
// TLstuff->reset(header, eqns);
 std::cout << "header : \""
           << std::string(header_32.begin(), header_32.end())
           << "\"" << std::endl;
 std::cout << "eqns : \""
           << std::string(eqns_32.begin(), eqns_32.end())
           << "\"" << std::endl;
 delete(TLstuff);
 try {
 TLstuff = new TLobjects(header_32, eqns_32);
 std::cout << "Something went right" << std::endl;
 }
 catch (...) {
 std::cout << "Something went wrong" << std::endl;
 }

}

void
spreadsheet::on_filename_header()
{

  Glib::ustring header_name_8  = filename_header_entry.get_text();
  std::string header_name_raw  = Glib::filename_from_utf8(header_name_8);

  std::string header_raw   = Glib::file_get_contents(header_name_raw);
  Glib::ustring header_8   = Glib::locale_to_utf8(header_raw);
  header_32 = std::u32string (header_8.begin(), header_8.end());
  std::cout << header_8 << std::endl;
  status_bar.push(header_8);

}

void
spreadsheet::on_filename_eqns()
{


Glib::ustring eqns_name_8  = filename_eqns_entry.get_text();
std::string eqns_name_raw  = Glib::filename_from_utf8(eqns_name_8);

std::string eqns_raw   = Glib::file_get_contents(eqns_name_raw);
Glib::ustring eqns_8   = Glib::locale_to_utf8(eqns_raw);
eqns_32 = std::u32string (eqns_8.begin(), eqns_8.end());
// eqns_32 (eqns_8.begin(), eqns_8.end());
  status_bar.push(eqns_8);
  std::cout << eqns_8 << std::endl;

/*
  std::ifstream eqns_file (filename_eqns_entry.get_text());
  Glib::ustring msg;
  if (eqns_file.is_open() ) 
  {
    msg = ("Equations file " + filename_eqns_entry.get_text() + " is open.");
  }
  else msg = ("Unable to open equations file " + filename_eqns_entry.get_text() );
  std::cout << msg << std::endl;
  status_bar.push(msg);
  eqns_file.close();
*/

}

void
spreadsheet::on_file_name()
{
  std::ifstream expr_file (filename_expr_entry.get_text());
  Glib::ustring msg;
  if (expr_file.is_open() ) 
  {
    msg = ("File " + filename_expr_entry.get_text() + " is open.");
  }
  else msg = ("Unable to open file " + filename_expr_entry.get_text() );
  std::cout << msg << std::endl;
  status_bar.push(msg);
  expr_file.close();
}

void
spreadsheet::on_get_exprs()
{
  (*TLstuff).expression = exprs_entry.get_text();
  status_bar.push("Expression \"" + (*TLstuff).expression + "\" entered.");
  std::cout << "Expression \"" + (*TLstuff).expression + "\" entered."
            << std::endl;
  redraw_button.clicked();
}

void
spreadsheet::on_icon_pressed_exprs(Gtk::EntryIconPosition /* icon_pos */,
                                  const GdkEventButton*  /* event */)
{
/* TODO: figure out how to produce a drop down list of existing equations
 * for now just selecting/highlighting text in it.
 * http://library.gnome.org/devel/gtkmm-tutorial/unstable/sec-text-entry.html.en#sec-text-entry-completion
 */
  exprs_entry.select_region(0, exprs_entry.get_text_length());
  std::cout << "Expression field clicked." << std::endl;
}

void
spreadsheet::on_status_clicked(Glib::ustring msg)
{
  Glib::ustring hd, vd, exp, dhd, dvd;
  if ((pivot.h_dim) == "")
    hd = "No horizontal dimension chosen.";
  else
    hd = "The horizontal dimension is " + (pivot.h_dim) + ".";
  if ((pivot.v_dim) == "")
    vd = "\nNo vertical dimension chosen.";
  else
    vd = "\nThe vertical dimension is " + (pivot.v_dim) + ".";
  if ((*TLstuff).expression == "")
    exp = "\nThere is no expression.";
  else
    exp = "\nExpression is " + (*TLstuff).expression + "." ;
  if (drawn_h_dim == "")
    dhd = "\nNo horizontal dimension drawn.";
  else
    dhd = "\nDrawn horizontal dimension is " + (drawn_h_dim)  + ".";
  if (drawn_v_dim == "")
    dvd = "\nNo vertical dimension drawn.";
  else
    dvd = "\nDrawn vertical dimension is " + (drawn_v_dim) + ".";
  Glib::ustring text = hd + vd + exp + dhd + dvd;
  label_status.set_text(text);
  infoBar_status.show(); // To show the widget when status is clicked.
  /* http://library.gnome.org/devel/gtkmm-tutorial/unstable/
   * sec-progressbar.html.en
   * to introduce progress bar instead of the Ok button.
   */
  status_bar.push(msg);
}

void
spreadsheet::on_infobar_status(int)
{
  infoBar_status.hide();
}

void
spreadsheet::on_closebutton_clicked()
{
  std::cout << "Quitting the S³..." << std::endl;
  hide(); //to close the application.
}

spreadsheet::~spreadsheet()
{
  /* Trying to get rid of the problem of not deallocating memory, but this is
     blowing up with display_comp class as a pointer.
  */
//  Glib::Error::register_cleanup();
//  Glib::wrap_register_cleanup();
}

void
spreadsheet::display_pivot()
{
  hnodisplay = Gtk::manage(new Gtk::RadioButton);
  vnodisplay = Gtk::manage(new Gtk::RadioButton);

  h_spread_spin = Gtk::manage(new Gtk::SpinButton);
  v_spread_spin = Gtk::manage(new Gtk::SpinButton);

  h_spread_limits.set_value(pivot.h_radius);
  h_spread_spin->set_adjustment(h_spread_limits);
  h_spread_spin->set_size_request(50, -1);
  h_spread_spin->set_numeric(true);
  h_spread_spin->set_alignment(1);

  v_spread_limits.set_value(pivot.v_radius);
  v_spread_spin->set_adjustment(v_spread_limits);
  v_spread_spin->set_size_request(50, -1);
  v_spread_spin->set_numeric(true);
  v_spread_spin->set_alignment(1);

// Begin table_pivot
  table_pivot = Gtk::manage(new Gtk::Table(((pivot.ords).size()+2), 6, false));
  (*table_pivot).set_col_spacings(10);

  // Column titles
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Dimension");
  (*table_pivot).attach((*label),0,1,0,1,Gtk::FILL,Gtk::FILL);

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(*v_spread_spin);
  (*label).set_label("V dim\nradius");
  (*table_pivot).attach((*box),1,2,0,1,Gtk::FILL,Gtk::FILL);
  v_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_v_spread_spin ) );

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(*h_spread_spin);
  (*label).set_label("H dim\nradius");
  (*table_pivot).attach((*box),2,3,0,1,Gtk::FILL,Gtk::FILL);
  h_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_spread_spin ) );

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Pivot");
  (*table_pivot).attach((*label),3,4,0,1,Gtk::FILL,Gtk::FILL);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Delete\ndimension?");
  (*table_pivot).attach((*label),4,5,0,1,Gtk::FILL,Gtk::FILL);

 // First row: no dimension chosen
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("None");
  (*table_pivot).attach((*label),0,1,1,2,Gtk::FILL,Gtk::FILL);

  Gtk::RadioButton::Group vgroup = (*vnodisplay).get_group();
  (*table_pivot).attach((*vnodisplay),1,2,1,2,Gtk::FILL,Gtk::FILL);
  (*vnodisplay).signal_toggled().connect(
     sigc::mem_fun( *this, &spreadsheet::on_v_nodim_toggled ) );

  Gtk::RadioButton::Group hgroup = (*hnodisplay).get_group();
  (*table_pivot).attach((*hnodisplay),2,3,1,2,Gtk::FILL,Gtk::FILL);
  (*hnodisplay).signal_toggled().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_nodim_toggled ) );

  /* Following rows: dimension names and radio buttons in two groups:
      H and V and the pivot value.
  */
  int i = 0; // Can't put this in the for loop, it explodes. 
  std::map<Glib::ustring,int>::iterator it;
  for ( it=(pivot.ords).begin(); it != (pivot.ords).end(); ++it, ++i )
  {
    int ii = i+3;

    label = Gtk::manage(new Gtk::Label);
    Glib::ustring dim = (*it).first;
    (*label).set_label(dim);
    (*table_pivot).attach( (*label), 0, 1, i+2, ii,Gtk::FILL,Gtk::FILL);

    vdisplay = Gtk::manage(new Gtk::RadioButton);
    (*vdisplay).set_group(vgroup);
    (*table_pivot).attach((*vdisplay), 1, 2, i+2, ii, Gtk::FILL,Gtk::FILL);
    (*vdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_v_toggled), vdisplay, dim ) );

    hdisplay = Gtk::manage(new Gtk::RadioButton);
    (*hdisplay).set_group(hgroup);
    (*table_pivot).attach( (*hdisplay), 2, 3, i+2, ii, Gtk::FILL,Gtk::FILL);
    (*hdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_h_toggled), hdisplay, dim ) );

    if ((pivot.h_dim) == dim) {
      (*hdisplay).set_active();
    } else if ((pivot.v_dim) == dim) {
      (*vdisplay).set_active();
    }

    pivot_spin = Gtk::manage(new Gtk::SpinButton);
    pivot_limits = Gtk::manage(new Gtk::Adjustment(
        (*it).second, -1000.0, 1000.0, 1.0, 3.0, 0.0) );
    pivot_spin->set_adjustment(*pivot_limits);
    pivot_spin->set_size_request(60, -1);
    pivot_spin->set_numeric(true);
    pivot_spin->set_alignment(1);
    pivot_spin->signal_value_changed().connect(
      sigc::bind (
        sigc::mem_fun( *this, &spreadsheet::on_dim_pivot_changed),
                       dim, pivot_spin ) );
    (*table_pivot).attach((*pivot_spin),3,4,i+2,ii,Gtk::FILL,Gtk::FILL);

    button = Gtk::manage(new Gtk::Button(dim));
    (*table_pivot).attach((*button),4,5,i+2,ii,Gtk::FILL,Gtk::FILL);
    button->signal_clicked().connect(
      sigc::bind (
         sigc::mem_fun(*this, &spreadsheet::on_del_dim), dim ) );
  }
// End table_pivot
  (vbox_pivot).pack_start(*table_pivot);
  (vbox_pivot).show();
  redraw_button.clicked();
}

void
spreadsheet::on_h_spread_spin()
{
  (pivot.h_radius) = h_spread_spin->get_value();
  std::stringstream sout;
  sout << pivot.h_radius;
  std::string s = sout.str();
  status_bar.push( "The new h_radius is " + s);
//  h_spread_limits.set_value(pivot.h_radius);
  redraw_button.clicked();
}

void
spreadsheet::on_v_spread_spin()
{
  (pivot.v_radius) = v_spread_spin->get_value();
  std::stringstream sout;
  sout << pivot.v_radius;
  std::string s = sout.str();
  status_bar.push( "The new v_radius is " + s);
  redraw_button.clicked();
}

void
spreadsheet::on_dim_pivot_changed(Glib::ustring dim, Gtk::SpinButton * pivot_spin)
{
  int new_pivot = pivot_spin->get_value ();
  std::stringstream sout;
  sout << new_pivot;
  std::string msg = "New pivot for dim " + dim + " is " + sout.str();
//std::cout << "New pivot for dim " << dim << " is " << new_pivot << std::endl;
  status_bar.push(msg);
  pivot.ords[dim] = new_pivot;
  redraw_button.clicked();
}

void
spreadsheet::on_v_nodim_toggled()
{
  if ((*vnodisplay).get_active()) {
    std::cout << "No dimension chosen for vertical display." << std::endl;
    pivot.v_dim.clear() ;
    std::cout << "v_dim = " << (pivot.v_dim) << std::endl;
    redraw_button.clicked();
  }
}

void
spreadsheet::on_h_nodim_toggled()
{
  if ((*hnodisplay).get_active()) {
    std::cout << "No dimension chosen for horizontal display."<< std::endl;
    pivot.h_dim.clear() ;
    std::cout << "h_dim = " << (pivot.h_dim) << std::endl;
    redraw_button.clicked();
  }
}

void
spreadsheet::on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim)
{
  if ((*v_radio).get_active()) { // When a dim is chosen
    if ((pivot.h_dim) == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      (*vnodisplay).set_active();
    } else {
      (pivot.v_dim) = dim;
      std::cout << dim << " chosen for vertical display."<< std::endl;
    }
  } // When a dim is released
  redraw_button.clicked();
}

void
spreadsheet::on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim)
{
  if ((*h_radio).get_active()) { // When a dim is chosen
    if ((pivot.v_dim) == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      (*hnodisplay).set_active();
    } else {
      (pivot.h_dim) = dim;
      std::cout << dim << " chosen for horizontal display."<< std::endl;
    }
  } // When a dim is released
  redraw_button.clicked();
}

void
spreadsheet::display_comp()
{
  display_comp_SW = Gtk::manage(new Gtk::ScrolledWindow);
  (*display_comp_SW).set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
//  display_comp_SW = Gtk::manage(new Gtk::Frame);
  int row_range = pivot.h_radius * 2 + 1 ;
  int col_range = pivot.v_radius * 2 + 1 ;

  if ( pivot.isOneCell() )
  {
    display_comp_cell();
  }
  else if ( pivot.isOneRow() )
  {
    int h_min = pivot.ords[(pivot.h_dim)] - pivot.h_radius ;
    display_comp_row(row_range, h_min);
  }
  else if ( pivot.isOneCol() )
  {
    int v_min = pivot.ords[(pivot.v_dim)] - pivot.v_radius ;
    display_comp_col(col_range, v_min);
  }
  else
  {
    int h_min = pivot.ords[(pivot.h_dim)] - pivot.h_radius ;
    int v_min = pivot.ords[(pivot.v_dim)] - pivot.v_radius ;
    display_comp_all(row_range, col_range, h_min, v_min);
  }
  (*display_comp_SW).show();
}


void 
spreadsheet::display_comp_all(int row_range, int col_range,
                              int h_min, int v_min)
{
  table_comp = Gtk::manage(new Gtk::Table(row_range + 1, col_range + 1, false));
  (*table_comp).set_col_spacings(10);
  (*table_comp).set_row_spacings(10);

  (*table_comp).set_row_spacing(0,30);
  (*table_comp).set_col_spacing(0,30);

  (*display_comp_SW).add(*table_comp);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label( pivot.v_dim + " \\ " + pivot.h_dim );
//  (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
//  (*table_comp).attach(*label, 0, 1, 0, 1,
//                       Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  (*table_comp).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

  //  Drawing horizontal index 
  for (int i = 0 ; i != row_range ; ++i) {
    std::string s;
    std::stringstream out;
    out << h_min + i ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table_comp).attach(*label, i+1, i+2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
//    (*table_comp).attach(*label, i+1, i+2, 0, 1, Gtk::FILL, Gtk::FILL);
  }

  //  Drawing vertical index 
  for (int j = 0 ; j < col_range ; ++j) {
    std::string s;
    std::stringstream out;
    out << v_min + j ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table_comp).attach(*label, 0, 1, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);
  }

  // Drawing content
  for (int i = 0 ; i != row_range ; ++i) {
    for (int j = 0 ; j != col_range ; ++j) {
    // Creating a string with expressions and context, calling TL for evaluation
      std::cout << "expression = " << (*TLstuff).expression << std::endl;
      std::stringstream newout;
      newout << "(";
      newout << (*TLstuff).expression;
      newout << ")";
      newout << " @ [DIM_TIME:0, ";
      newout << pivot.h_dim << ":" << (i+h_min) << ", ";
      newout << pivot.v_dim << ":" << (j+v_min);
      for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
           mit != pivot.ords.end(); ++mit) {
         if (mit->first != pivot.h_dim && mit->first != pivot.v_dim) {
           newout << ", " << mit->first << ":" << mit->second;
         }
      }
      newout << "]";
      std::string newout_str = newout.str();
      std::cout << newout_str << std::endl;
      std::u32string tuple32 (newout_str.begin(), newout_str.end());
      Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

      label = Gtk::manage(new Gtk::Label);
      frame = Gtk::manage(new Gtk::Frame);
      (*label).set_label(cell);
      (*frame).add(*label);
      (*table_comp).attach(*frame, i+1, i+2, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);
    }
  }

//  (*display_comp_SW).show();
}

void 
spreadsheet::display_comp_row(int row_range, int h_min)
{
  table_comp = Gtk::manage(new Gtk::Table(row_range + 1, 2, false));
  (*table_comp).set_col_spacings(10);
  (*table_comp).set_row_spacings(10);
  (*display_comp_SW).add(*table_comp);

  (*table_comp).set_row_spacing(0,30);
  (*table_comp).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label(pivot.h_dim);
//  (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*table_comp).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
//  (*table_comp).attach(*label, 0, 1, 0, 1,
//                       Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);

  for (int i = 0 ; i != row_range ; ++i) {
    //  Drawing horizontal index 
    std::string s;
    std::stringstream out;
    out << h_min + i ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table_comp).attach(*label, i+1, i+2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

    // Drawing content
    // Creating a string with expressions and context, calling TL for evaluation
    std::cout << "expression = " << (*TLstuff).expression << std::endl;
    std::stringstream newout;
    newout << "(";
    newout << (*TLstuff).expression;
    newout << ")";
    newout << " @ [DIM_TIME:0, ";
    newout << pivot.h_dim << ":" << (i+h_min) ;
    for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
         mit != pivot.ords.end(); ++mit) {
      if (mit->first != pivot.h_dim) {
        newout << ", " << mit->first << ":" << mit->second;
      }
    }
    newout << "]";
    std::string newout_str = newout.str();
    std::cout << newout_str << std::endl;
    std::u32string tuple32 (newout_str.begin(), newout_str.end());
    cell = (*TLstuff).calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label);
    frame = Gtk::manage(new Gtk::Frame);
    (*label).set_label(cell);
    (*frame).add(*label);
    (*table_comp).attach(*frame, i+1, i+2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
  }
//  (*display_comp_SW).show();
}

void 
spreadsheet::display_comp_col(int col_range, int v_min)
{
  table_comp = Gtk::manage(new Gtk::Table(2, col_range + 1, false));
  (*table_comp).set_col_spacings(10);
  (*table_comp).set_row_spacings(10);
  (*display_comp_SW).add(*table_comp);

  (*table_comp).set_row_spacing(0,30);
  (*table_comp).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label(pivot.v_dim);
//  (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*table_comp).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
//  (*table_comp).attach(*label, 0, 1, 0, 1,
//                       Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);

  for (int j = 0 ; j != col_range ; ++j)
  {
    //  Drawing vertical index 
    std::string s;
    std::stringstream out;
    out << v_min + j ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table_comp).attach(*label, 0, 1, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);

    // Drawing content
    // Creating a string with expressions and context, calling TL for evaluation
    std::cout << "expression = " << (*TLstuff).expression << std::endl;
    std::stringstream newout;
    newout << "(";
    newout << (*TLstuff).expression;
    newout << ")";
    newout << " @ [DIM_TIME:0, ";
    newout << pivot.v_dim << ":" << (j+v_min) ;
    for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
         mit != pivot.ords.end(); ++mit)
    {
      if (mit->first != pivot.v_dim)
      {
        newout << ", " << mit->first << ":" << mit->second;
      }
    }
    newout << "]";
    std::string newout_str = newout.str();
    std::cout << newout_str << std::endl;
    std::u32string tuple32 (newout_str.begin(), newout_str.end());
    cell = (*TLstuff).calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label);
    frame = Gtk::manage(new Gtk::Frame);
    (*label).set_label(cell);
    (*frame).add(*label);
    (*table_comp).attach(*frame, 1, 2, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);
  }
//  (*display_comp_SW).show();
}

void
spreadsheet::display_comp_cell()  // single cell
{
  table_comp = Gtk::manage(new Gtk::Table( 2, 2, false));
  (*display_comp_SW).add(*table_comp);

  (*table_comp).set_row_spacing(0,30);
  (*table_comp).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  ");
//   (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  // (*table_comp).attach(*label, 0, 1, 0, 1,
  //                      Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  (*table_comp).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

  // Creating a string with expressions and context, calling TL for evaluation
  std::stringstream newout;
  newout << "(";
  newout << (*TLstuff).expression;
  newout << ")";
  newout << " @ [DIM_TIME:0, ";
  std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
  newout << mit->first << ":" << mit->second;
  ++mit;
  for (mit; mit != pivot.ords.end(); ++mit)
  {
    newout << "," << mit->first << ":" << mit->second;
  }
  newout << "]";
  std::string newout_str = newout.str();
  std::cout << newout_str << std::endl;
  std::u32string tuple32 (newout_str.begin(), newout_str.end());
  Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

  label = Gtk::manage(new Gtk::Label);
  frame = Gtk::manage(new Gtk::Frame);
  (*label).set_label(cell);
  (*frame).add(*label);
  (*table_comp).attach(*frame, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
//  (*display_comp_SW).show();
}


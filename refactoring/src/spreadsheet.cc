/* spreadsheet.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <tl/parser_header_util.hpp>
#include "spreadsheet.h"
#include "HeaderDims_In_PivotOrds.h"
#include <boost/spirit/include/qi.hpp>

spreadsheet::spreadsheet() :
  // false: child widgets don't have the same width
  //    10: distance in pixels between widgets
  main_box  (false, 10),
  title_hbox (false, 10),
  last_hbox (false, 10),
  window_header("The TransLucid browser" ),
  system_table (3,3,false),

  // Gtk::Adjustment(
  //   initial_value, lower, upper, step_increment, page_increment, page_size);
  h_spread_limits(0.0, 0.0, 20.0, 1.0, 1.0, 0.0),
  v_spread_limits(0.0, 0.0, 20.0, 1.0, 1.0, 0.0),

  // last_button is Gtk::HButtonBox for equal spacing (30) between buttons.
  last_box(Gtk::BUTTONBOX_END, 30),
  system_status_button(" _Show Current System Status ", true), 
  redraw_comp_button(" _Redraw SpreadSheet  ", true), 
  tick_button("  _Tick Time  ", true),
  close_button("_Quit", true)       // to use alt-C to close app

{

//TODO: use delete for all of the pointers.
//TODO: catch all the errors froms TransLucid

  // Set title and size of the SuperSpreadSheet main window
  set_title("The Super SpreadSheet, The S³");
  // Add outer box to the window since it may only contain a single widget 
  add(main_box);
  // main_box.set_size_request(800,600);
  //Put the inner boxes in the outer box:
  main_box.pack_start(title_hbox, false, false, 0);
  main_box.pack_start(system_frame,false,false,5);
  main_box.pack_start(entities_hbox,false,false,0);
  main_box.pack_start(pivot_comp_hbox,true, true, 0);
  main_box.pack_start(last_hbox, false, false, 0);
  main_box.pack_start(status_bar, false, false, 0);

  // Set the boxes' spacing around the outside of the container to 5 pixels
  title_hbox.set_border_width(5); 
  system_hbox.set_border_width(5);
  entities_hbox.set_border_width(5);
  pivot_comp_hbox.set_border_width(5);
  last_hbox.set_border_width(5);

// Begin title_hbox 
  title_hbox.pack_start(window_header);
// End title_hbox 

// Begin system_hbox 
  TLstuff = new TLobjects();
  // Begin system_table
    // Expression
  label = Gtk::manage(new Gtk::Label("Expression"));
  (system_table).attach((*label),0,1,0,1,Gtk::FILL,Gtk::FILL);
  exprs_entry.set_text((*TLstuff).expression);
  exprs_entry.set_max_length(50);
  exprs_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_get_expr) );
  (system_table).attach((exprs_entry),1,2,0,1,Gtk::FILL,Gtk::FILL);
    // Header file
  label = Gtk::manage(new Gtk::Label("Header file "));
  (system_table).attach((*label),0,1,1,2,Gtk::FILL,Gtk::FILL);
  (system_table).attach((filename_header_entry),1,2,1,2,Gtk::FILL,Gtk::FILL);
  filename_header_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_filename_header) );
    // Equations file
  label = Gtk::manage(new Gtk::Label("Equations file "));
  (system_table).attach((*label),0,1,2,3,Gtk::FILL,Gtk::FILL);
  (system_table).attach((filename_eqns_entry),1,2,2,3,Gtk::FILL,Gtk::FILL);
  filename_eqns_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_filename_eqns) );
  // End system_table

    // Loaded Equations
  equations.ent_TreeView.signal_row_activated().connect(
    sigc::mem_fun(*this, &spreadsheet::on_treeview_row_activated) );
  button = Gtk::manage(new Gtk::Button("  _Delete ticked equations",true));
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_delete_equation) );
  entities_hbox.pack_start(equations.ent_TreeView);
  entities_hbox.pack_start(*button);


  // InfoBar is taken from example.
  Gtk::Container* infoBarSystemContainer =
    dynamic_cast<Gtk::Container*>(infoBar_system_status.get_content_area());
  if (infoBarSystemContainer) infoBarSystemContainer->add(label_system_status);
//  infoBar_system_status.add_button(Gtk::Stock::OK, 0);
  infoBar_system_status.add_button("Hide Status", 0);
  infoBar_system_status.set_message_type(Gtk::MESSAGE_INFO);
  infoBar_system_status.signal_response().connect(
    sigc::mem_fun(*this, &spreadsheet::on_infobar_system_status ) );
  
  system_status_button.signal_clicked().connect(
    sigc::mem_fun( *this, &spreadsheet::on_system_status_clicked) );

  button = Gtk::manage(new Gtk::Button("  _Update system  ", true));
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_update_system) );
  (tick_button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_tick_time) );
  tick_button.set_relief(Gtk::ReliefStyle::RELIEF_HALF);

  system_hbox.pack_start(system_table);
  system_hbox.pack_end(tick_button);
  system_hbox.pack_end(*button);
  system_hbox.pack_end(infoBar_system_status);
  system_hbox.pack_end(system_status_button);

  system_frame.set_shadow_type(Gtk::SHADOW_IN);
  system_frame.add(system_hbox);
// End system_hbox

// Begin pivot_comp_hbox
  // Begin pivot_frame, pivot_vbox
    // Begin pivot_table
  display_pivot();
  pivot_frame.set_shadow_type(Gtk::SHADOW_IN);
  pivot_frame.add(((pivot_vbox)));
  pivot_comp_hbox.pack_start(pivot_frame,false,true,0);
    // End pivot_table

    // Begin new_dim_vbox
  Gtk::HBox * hbox = Gtk::manage(new Gtk::HBox);
  label = Gtk::manage(new Gtk::Label("Dim Name "));
  (*hbox).pack_start(*label); 
  (*hbox).pack_start(new_dim_entry); 
  new_dim_vbox.pack_start(*hbox, false, false, 0);

  hbox = Gtk::manage(new Gtk::HBox);
  label = Gtk::manage(new Gtk::Label("Pivot Value"));
  (*hbox).pack_start(*label); 
  (*hbox).pack_start(new_pivot_entry); 
  new_dim_vbox.pack_start(*hbox, false, false, 0);

  button = Gtk::manage(new Gtk::Button("_Add New Dimension",true));
  new_dim_vbox.pack_start(*button);
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_add_dim) );

  (pivot_vbox).pack_end(new_dim_vbox, false, false, 0);
    // End new_dim_vbox
  // End pivot_frame, pivot_vbox

  // Begin comp_frame
  display_comp();
  comp_frame.set_shadow_type(Gtk::SHADOW_IN);
  comp_frame.add(*display_comp_SW);
//  pivot_comp_hbox.pack_start(comp_frame,false,false);
  pivot_comp_hbox.pack_start(comp_frame,true,true);
  // End comp_frame
// End pivot_comp_hbox

// Begin last_hbox 
  // last_box.add(redraw_comp_button);
  redraw_comp_button.signal_clicked().connect(
    sigc::mem_fun( *this, &spreadsheet::on_redraw_comp_clicked) );

  last_box.add(close_button);
  close_button.signal_clicked().connect (
    sigc::mem_fun( *this, &spreadsheet::on_closebutton_clicked) );

  last_hbox.pack_start(last_box);
  // Make the button the default widget. Not working as expected, gtk bug?.
  close_button.set_can_default();
  close_button.grab_default();
// End last_hbox 

  show_all_children();
  infoBar_system_status.hide(); 
} // End spreadsheet::spreadsheet()


spreadsheet::~spreadsheet()
{
  /* Trying to get rid of the problem of not deallocating memory, but this is
     blowing up with display_comp class as a pointer.
  */
//  Glib::Error::register_cleanup();
//  Glib::wrap_register_cleanup();
}

void
spreadsheet::on_get_expr()
{
  (*TLstuff).expression = exprs_entry.get_text();
  status_bar.push("Expression \"" + (*TLstuff).expression + "\" entered.");
  std::cout << "Expression \"" + (*TLstuff).expression + "\" entered."
            << std::endl;
  redraw_comp_button.clicked();
}

void
spreadsheet::on_filename_header()
{
  Glib::ustring header_name_8 = filename_header_entry.get_text();
  std::string header_name_raw = Glib::filename_from_utf8(header_name_8);
  std::string header_raw = Glib::file_get_contents(header_name_raw);
  Glib::ustring header_8 = Glib::locale_to_utf8(header_raw);
  header_32 = std::u32string (header_8.begin(), header_8.end());
  std::cout << header_8 << std::endl;
  status_bar.push("Header file read.");
}

void
spreadsheet::on_filename_eqns()
{
  Glib::ustring eqns_name_8 = filename_eqns_entry.get_text();
  std::string eqns_name_raw = Glib::filename_from_utf8(eqns_name_8);
  std::string eqns_raw = Glib::file_get_contents(eqns_name_raw);
  Glib::ustring eqns_8 = Glib::locale_to_utf8(eqns_raw);
  eqns_32 = std::u32string (eqns_8.begin(), eqns_8.end());
  status_bar.push("Equations file read.");
  std::cout << eqns_8 << std::endl;
}

void
spreadsheet::on_infobar_system_status(int)
{
  infoBar_system_status.hide();
}

void
spreadsheet::on_system_status_clicked()
{
  std::stringstream sout;
  sout << "Current expression is \"" << (*TLstuff).expression 
       << "\"" << std::endl;
  for (std::map<Glib::ustring,int>::iterator mit=(pivot.ords).begin(); 
       mit != (pivot.ords).end(); 
       ++mit )
  {
    sout << "Dimension \"" << (*mit).first << "\" = " << (*mit).second 
         << std::endl;
  }

  std::string text = sout.str();

  label_system_status.set_text(text);
  infoBar_system_status.show(); // To show the widget when status is clicked.
//  system_status_button.hide();
}

void
spreadsheet::on_update_system()
{
  status_bar.push("Updating system");
// TLstuff->update(header, eqns);
  std::cout << "Loading header : " << std::endl << "\"" << std::endl 
            << header_32 << "\"" << std::endl;
  std::cout << "loading equations : " << std::endl << "\"" << std::endl 
            << eqns_32 << "\"" << std::endl;
  try
  {
    (*TLstuff).traductor.parse_header ( header_32 ) ;
  }
  catch (...)
  {
    Glib::ustring msg = " Could not load and parse the header." ;
    std::cout << msg << std::endl;
    status_bar.push(msg);
  }
  (*TLstuff).traductor.header().dimension_symbols.
             for_each(HeaderDims_In_PivotOrds(pivot)); 
  try
  {
    // Add equations to TL system
    (*TLstuff).traductor.translate_and_add_equation_set ( eqns_32 ) ;
    // Read equations from TL system and add to TreeView widget
    TL::Translator::EquationIterator
         begin_eqs = (*TLstuff).traductor.beginEquations();
    TL::Translator::EquationIterator
         end_eqs = (*TLstuff).traductor.endEquations();
    // TODO what happens if adding an equation already added?
    for ( ; begin_eqs != end_eqs ; )
    {
      std::cout << begin_eqs.print() << std::endl;
      std::cout << begin_eqs.id() << std::endl;
      std::stringstream ss;
      ss << begin_eqs.id();
      std::string uuid_str = ss.str();
      equations.add_entity(uuid_str, begin_eqs.print());

      begin_eqs++;
    }
  }
  catch (...)
  {
    Glib::ustring msg =  " Could not translate and add the equations." ;
    std::cout << msg << std::endl;
    status_bar.push(msg);
  }
  (*TLstuff).traductor.header().dimension_symbols.
             for_each(HeaderDims_In_PivotOrds(pivot)); 
  // Redraw the pivot area to show new dimensions from header
  delete(pivot_table);
  display_pivot();
  (pivot_vbox).show_all_children();
 
  // Clering the fields to avoid reloading
  header_32.clear();
  eqns_32.clear();
  filename_eqns_entry.set_text("");
  filename_header_entry.set_text("");
}

void
spreadsheet::on_tick_time()
{
  int current_time = (*TLstuff).get_time();
  std::stringstream ss;
  ss << current_time;
  status_bar.push("Advancing time: current time = " + ss.str());


  // Reading equations from the TL system
  TL::Translator::EquationIterator
       begin_eqs = (*TLstuff).traductor.beginEquations();
  TL::Translator::EquationIterator
       end_eqs = (*TLstuff).traductor.endEquations();
  for ( ; begin_eqs != end_eqs ; )
  {
    std::cout << begin_eqs.print() << std::endl;
//    std::cout << begin_eqs.id() << std::endl;
    begin_eqs++;
  }
}

void
spreadsheet::on_delete_equation()
{
  Glib::ustring msg;
  int no_dels = 0;
  typedef Gtk::TreeModel::Children type_children;
  type_children children = equations.ent_refTreeModel->children();
  for(type_children::iterator iter = children.begin();
      iter != children.end(); )
  {
    Gtk::TreeModel::Row row = *iter;
    if (row[equations.theColumns.del_ent])
    {
    std::cout //<< "Deleting equation: UUID = " << row[equations.theColumns.uuid]
              << "Equation = " << row[equations.theColumns.content]
              << ", Delete status = " << row[equations.theColumns.del_ent]
              << std::endl;
    no_dels++ ;
    // TODO with the uuid, nned to erase the equation from the system
    iter = equations.ent_refTreeModel->erase(row);
    }
    else iter++; 
  }
  if (!no_dels) msg = "There are no equations to delete";
  else 
  {
    std::stringstream sout;
    sout << no_dels;
    Glib::ustring num = sout.str();
    msg = num + " equation(s) deleted";
  }
  status_bar.push(msg);


}

void
spreadsheet::on_treeview_row_activated( const Gtk::TreeModel::Path& path, 
                                        Gtk::TreeViewColumn* column)
{
  Gtk::TreeModel::iterator iter = equations.ent_refTreeModel->get_iter(path);
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    std::cout // << "Row activated: UUID = " << row[equations.theColumns.uuid]
              << ", Equation = " << row[equations.theColumns.content] 
              << ", Delete status = " << row[equations.theColumns.del_ent] 
              << std::endl;
  }
}

void
spreadsheet::on_add_dim()
{
  Glib::ustring dimname  = new_dim_entry.get_text();
  Glib::ustring dimpivot = new_pivot_entry.get_text();
  if (dimname == "" && dimpivot == "")
  { 
    return;
  }
  else if (dimname == "" || dimpivot == "")
  { 
    new_dim_entry.set_text("");
    new_pivot_entry.set_text("");
    new_dim_vbox.show();
    return;
  }
  else
  {
    new_dim_entry.set_text("");
    new_pivot_entry.set_text("");
    int piv =  atoi(dimpivot.c_str());
    (pivot.ords)[dimname] = piv;

    Glib::ustring dim_8 = ( "dimension ustring<" + dimname + ">;;" );
    std::u32string dim_32 = std::u32string (dim_8.begin(), dim_8.end());
    // std::cout << "dim_32 is \"" << dim_32 << "\"" << std::endl;

    Glib::ustring dim_a_8 = dimname ;
    std::u32string dim_a_32 = std::u32string (dim_a_8.begin(), dim_a_8.end());
    // std::cout << "dim_a_32 is \"" << dim_a_32 << "\"" << std::endl;
    try
    {
      TL::Parser::addDimensionSymbol((*TLstuff).traductor.header(), dim_a_32);
      // (*TLstuff).traductor.parse_header ( dim_32 ) ;
      std::cout << "Adding to the header \"" << dim_32 << "\"." << std::endl;
    }
    catch (...)
    {
      std::cout << "Could not parse dim_32" << std::endl;
    }
    std::cout << "  Name: "  << dimname << std::endl;
    std::cout << "  Pivot: " << dimpivot << std::endl;

    (pivot_vbox).remove(*pivot_table);
    delete(pivot_table);
    display_pivot();
    (pivot_vbox).show_all_children();
    // (pivot_vbox).show();
    // new_dim_vbox.show();
  }
}


void
spreadsheet::on_del_dim(Glib::ustring dim)
{
  (pivot.ords).erase (dim);
  #if 0
  // Only include when not allowing this to be added to system
  if ( (dim == "time") || (dim == "id") || (dim == "all" ) )
  {
    std::cout << "User cannot handle this dimension: \"" << dim 
              << "\"" << std::endl ;
    return;
  }
  #endif
  std::u32string dim_d_32 = std::u32string(dim.begin(), dim.end());
  TL::Parser::removeDimensionSymbol((*TLstuff).traductor.header(), dim_d_32);
  if (dim == (pivot.h_dim))
  {
    (pivot.h_dim.clear() ) ;
    (*hnodisplay).toggled();
  }
  else if (dim == (pivot.v_dim))
  {
    (pivot.v_dim.clear() )  ;
    (*vnodisplay).toggled();
  } 

  (pivot_vbox).remove(*pivot_table);
  delete(pivot_table);
  display_pivot();
  (pivot_vbox).show_all_children();
  (pivot_vbox).show();

  status_bar.push("Deleted dimension " + dim);
  std::cout << "Deleted dimension " << dim << std::endl;

}

void
spreadsheet::on_redraw_comp_clicked()
{
//  delete(display_comp_SW);
  delete(comp_table);
  display_comp();
  comp_frame.remove();
  comp_frame.add(*display_comp_SW);
  (*display_comp_SW).show_all_children();
}

void
spreadsheet::on_closebutton_clicked()
{
  std::cout << "Quitting the S³..." << std::endl;
  hide(); //to close the application.
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

// Begin pivot_table
  // When pivot.ords is empty it's size is zero
  pivot_table = Gtk::manage(new Gtk::Table(((pivot.ords).size()+2),6,false));
  (*pivot_table).set_col_spacings(10);

  // Column titles
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Dimension");
  (*pivot_table).attach((*label),0,1,0,1,Gtk::FILL,Gtk::FILL);

  vbox = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*vbox).add(*label);
  (*vbox).add(*v_spread_spin);
  (*label).set_label("V dim\nradius");
  (*pivot_table).attach((*vbox),1,2,0,1,Gtk::FILL,Gtk::FILL);
  v_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_v_spread_spin ) );

  vbox = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*vbox).add(*label);
  (*vbox).add(*h_spread_spin);
  (*label).set_label("H dim\nradius");
  (*pivot_table).attach((*vbox),2,3,0,1,Gtk::FILL,Gtk::FILL);
  h_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_spread_spin ) );

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Pivot");
  (*pivot_table).attach((*label),3,4,0,1,Gtk::FILL,Gtk::FILL);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Delete\ndimension?");
  (*pivot_table).attach((*label),4,5,0,1,Gtk::FILL,Gtk::FILL);

 // First row: no dimension chosen for computation display
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("None");
  (*pivot_table).attach((*label),0,1,1,2,Gtk::FILL,Gtk::FILL);

  Gtk::RadioButton::Group vgroup = (*vnodisplay).get_group();
  (*pivot_table).attach((*vnodisplay),1,2,1,2,Gtk::FILL,Gtk::FILL);
  (*vnodisplay).signal_toggled().connect(
     sigc::mem_fun( *this, &spreadsheet::on_v_nodim_toggled ) );

  Gtk::RadioButton::Group hgroup = (*hnodisplay).get_group();
  (*pivot_table).attach((*hnodisplay),2,3,1,2,Gtk::FILL,Gtk::FILL);
  (*hnodisplay).signal_toggled().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_nodim_toggled ) );

  /* Following rows: dimension names and radio buttons in two groups:
      H and V and the pivot value.
  */
  int i = 0; // Can't put this in the for loop, it explodes. 
  std::map<Glib::ustring,int>::iterator mit;
  // When pivot.ords is empty, the for loop doesn't start
  for ( mit=(pivot.ords).begin(); mit != (pivot.ords).end(); ++mit, ++i )
  {
    int ii = i+3;

    label = Gtk::manage(new Gtk::Label);
    Glib::ustring dim = (*mit).first;
    (*label).set_label(dim);
    (*pivot_table).attach( (*label), 0, 1, i+2, ii,Gtk::FILL,Gtk::FILL);

    vdisplay = Gtk::manage(new Gtk::RadioButton);
    (*vdisplay).set_group(vgroup);
    (*pivot_table).attach((*vdisplay), 1, 2, i+2, ii, Gtk::FILL,Gtk::FILL);
    (*vdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_v_toggled), vdisplay, dim ) );

    hdisplay = Gtk::manage(new Gtk::RadioButton);
    (*hdisplay).set_group(hgroup);
    (*pivot_table).attach( (*hdisplay), 2, 3, i+2, ii, Gtk::FILL,Gtk::FILL);
    (*hdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_h_toggled), hdisplay, dim ) );

    if ((pivot.h_dim) == dim)
    {
      (*hdisplay).set_active();
    }
    else if ((pivot.v_dim) == dim)
    {
      (*vdisplay).set_active();
    }

    pivot_spin = Gtk::manage(new Gtk::SpinButton);
    pivot_limits = Gtk::manage(new Gtk::Adjustment(
        (*mit).second, -1000.0, 1000.0, 1.0, 3.0, 0.0) );
    pivot_spin->set_adjustment(*pivot_limits);
    pivot_spin->set_size_request(60, -1);
    pivot_spin->set_numeric(true);
    pivot_spin->set_alignment(1);
    pivot_spin->signal_value_changed().connect(
      sigc::bind (
        sigc::mem_fun( *this, &spreadsheet::on_dim_pivot_changed),
                       dim, pivot_spin ) );
    (*pivot_table).attach((*pivot_spin),3,4,i+2,ii,Gtk::FILL,Gtk::FILL);

    button = Gtk::manage(new Gtk::Button(dim));
    (*pivot_table).attach((*button),4,5,i+2,ii,Gtk::FILL,Gtk::FILL);
    button->signal_clicked().connect(
      sigc::bind (
         sigc::mem_fun(*this, &spreadsheet::on_del_dim), dim ) );
  }
// End pivot_table
  (pivot_vbox).pack_start(*pivot_table);
  (pivot_vbox).show();
  redraw_comp_button.clicked();
}

void
spreadsheet::on_v_spread_spin()
{
  (pivot.v_radius) = v_spread_spin->get_value();
  std::stringstream sout;
  sout << pivot.v_radius;
  std::string s = sout.str();
  status_bar.push( "The new v_radius is " + s);
  redraw_comp_button.clicked();
}

void
spreadsheet::on_h_spread_spin()
{
  (pivot.h_radius) = h_spread_spin->get_value();
  std::stringstream sout;
  sout << pivot.h_radius;
  std::string s = sout.str();
  status_bar.push( "The new h_radius is " + s);
  redraw_comp_button.clicked();
}

void
spreadsheet::on_v_nodim_toggled()
{
  if ((*vnodisplay).get_active())
  {
    std::cout << "No dimension chosen for vertical display." << std::endl;
    pivot.v_dim.clear() ;
    std::cout << "v_dim = " << (pivot.v_dim) << std::endl;
    redraw_comp_button.clicked();
  }
}

void
spreadsheet::on_h_nodim_toggled()
{
  if ((*hnodisplay).get_active())
  {
    std::cout << "No dimension chosen for horizontal display."<< std::endl;
    pivot.h_dim.clear() ;
    std::cout << "h_dim = " << (pivot.h_dim) << std::endl;
    redraw_comp_button.clicked();
  }
}

void
spreadsheet::on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim)
{
  if ((*v_radio).get_active())
  { // When a dim is chosen
    if ((pivot.h_dim) == dim)
    {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      (*vnodisplay).set_active();
    }
    else
    {
      (pivot.v_dim) = dim;
      std::cout << dim << " chosen for vertical display."<< std::endl;
    }
  } // When a dim is released
  redraw_comp_button.clicked();
}

void
spreadsheet::on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim)
{
  if ((*h_radio).get_active()) { // When a dim is chosen
    if ((pivot.v_dim) == dim)
    {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      (*hnodisplay).set_active();
    }
    else
    {
      (pivot.h_dim) = dim;
      std::cout << dim << " chosen for horizontal display."<< std::endl;
    }
  } // When a dim is released
  redraw_comp_button.clicked();
}

void
spreadsheet::on_dim_pivot_changed(Glib::ustring dim,
                                  Gtk::SpinButton * pivot_spin)
{
  int new_pivot = pivot_spin->get_value ();
  std::stringstream sout;
  sout << new_pivot;
  std::string msg = "New pivot for dim \"" + dim + "\" is \"" + 
                    sout.str() + "\"";
  status_bar.push(msg);
  pivot.ords[dim] = new_pivot;
  redraw_comp_button.clicked();
}

void
spreadsheet::display_comp()
{
  display_comp_SW = Gtk::manage(new Gtk::ScrolledWindow);
  (*display_comp_SW).set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
//  display_comp_SW = Gtk::manage(new Gtk::Frame);

  if ((pivot.ords).empty())
  {
    display_comp_cell_nodims();
    (*display_comp_SW).show();
    return;
  }
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
  comp_table = Gtk::manage(new Gtk::Table(row_range + 1, col_range + 1, false));
  (*comp_table).set_col_spacings(10);
  (*comp_table).set_row_spacings(10);

  (*comp_table).set_row_spacing(0,30);
  (*comp_table).set_col_spacing(0,30);

  (*display_comp_SW).add(*comp_table);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label( pivot.v_dim + " \\ " + pivot.h_dim );
//  (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
//  (*comp_table).attach(*label, 0, 1, 0, 1,
//                       Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

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
    (*comp_table).attach(*label, i+1, i+2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
//    (*comp_table).attach(*label, i+1, i+2, 0, 1, Gtk::FILL, Gtk::FILL);
  }

  //  Drawing vertical index 
  for (int j = 0 ; j < col_range ; ++j)
  {
    std::string s;
    std::stringstream out;
    out << v_min + j ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*comp_table).attach(*label, 0, 1, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);
  }

  // Drawing content
  for (int i = 0 ; i != row_range ; ++i)
  {
    for (int j = 0 ; j != col_range ; ++j)
    {
    // Creating a string with expressions and context, calling TL for evaluation
      std::cout << "expression = " << (*TLstuff).expression << std::endl;
      std::stringstream newout;
      newout << "(";
      newout << (*TLstuff).expression;
      newout << ")";
      // newout << " @ [ ";
      newout << " @ [time:0, ";
      newout << pivot.h_dim << ":" << (i+h_min) << ", ";
      newout << pivot.v_dim << ":" << (j+v_min);
      for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
           mit != pivot.ords.end(); ++mit)
      {
         if (mit->first != pivot.h_dim && mit->first != pivot.v_dim)
         {
           newout << ", " << mit->first << ":" << mit->second;
         }
      }
      newout << "]";
      std::string newout_str = newout.str();
      // std::cout << newout_str << std::endl;
      std::u32string tuple32 (newout_str.begin(), newout_str.end());
      std::cout << tuple32 << std::endl;
      Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

      label = Gtk::manage(new Gtk::Label);
      frame = Gtk::manage(new Gtk::Frame);
      (*label).set_label(cell);
      (*frame).add(*label);
      (*comp_table).attach(*frame, i+1, i+2, j+1, j+2, Gtk::SHRINK,Gtk::SHRINK);
    }
  }
}

void 
spreadsheet::display_comp_row(int row_range, int h_min)
{
  comp_table = Gtk::manage(new Gtk::Table(row_range + 1, 2, false));
  (*comp_table).set_col_spacings(10);
  (*comp_table).set_row_spacings(10);
  (*display_comp_SW).add(*comp_table);

  (*comp_table).set_row_spacing(0,30);
  (*comp_table).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label(pivot.h_dim);
//  (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
//  (*comp_table).attach(*label, 0, 1, 0, 1,
//                       Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);

  for (int i = 0 ; i != row_range ; ++i)
  {
    //  Drawing horizontal index 
    std::string s;
    std::stringstream out;
    out << h_min + i ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*comp_table).attach(*label, i+1, i+2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

    // Drawing content
    // Creating a string with expressions and context, calling TL for evaluation
    std::cout << "expression = " << (*TLstuff).expression << std::endl;
    std::stringstream newout;
    newout << "(";
    newout << (*TLstuff).expression;
    newout << ")";
    newout << " @ [time:0, ";
    newout << pivot.h_dim << ":" << (i+h_min) ;
    for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
         mit != pivot.ords.end(); ++mit)
    {
      if (mit->first != pivot.h_dim)
      {
        newout << ", " << mit->first << ":" << mit->second;
      }
    }
    newout << "]";
    std::string newout_str = newout.str();
    std::u32string tuple32 (newout_str.begin(), newout_str.end());
    std::cout << tuple32 << std::endl;
    cell = (*TLstuff).calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label);
    frame = Gtk::manage(new Gtk::Frame);
    (*label).set_label(cell);
    (*frame).add(*label);
    (*comp_table).attach(*frame, i+1, i+2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
  }
}

void 
spreadsheet::display_comp_col(int col_range, int v_min)
{
  comp_table = Gtk::manage(new Gtk::Table(2, col_range + 1, false));
  (*comp_table).set_col_spacings(10);
  (*comp_table).set_row_spacings(10);
  (*display_comp_SW).add(*comp_table);

  (*comp_table).set_row_spacing(0,30);
  (*comp_table).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label(pivot.v_dim);
//  (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
//  (*comp_table).attach(*label, 0, 1, 0, 1,
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
    (*comp_table).attach(*label, 0, 1, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);

    // Drawing content
    // Creating a string with expressions and context, calling TL for evaluation
    std::cout << "expression = " << (*TLstuff).expression << std::endl;
    std::stringstream newout;
    newout << "(";
    newout << (*TLstuff).expression;
    newout << ")";
    newout << " @ [time:0, ";
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
    std::u32string tuple32 (newout_str.begin(), newout_str.end());
    std::cout << tuple32 << std::endl;
    cell = (*TLstuff).calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label);
    frame = Gtk::manage(new Gtk::Frame);
    (*label).set_label(cell);
    (*frame).add(*label);
    (*comp_table).attach(*frame, 1, 2, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);
  }
}

void
spreadsheet::display_comp_cell()  // single cell
{
  comp_table = Gtk::manage(new Gtk::Table( 2, 2, false));
  (*display_comp_SW).add(*comp_table);

  (*comp_table).set_row_spacing(0,30);
  (*comp_table).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  ");
//   (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  // (*comp_table).attach(*label, 0, 1, 0, 1,
  //                      Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

  // Creating a string with expressions and context, calling TL for evaluation
  std::stringstream newout;
  newout << "(";
  newout << (*TLstuff).expression;
  newout << ")";
  newout << " @ [time:0";
  // There is at least one element in pivot.ords otherwise control would be
  // given to display_comp_cel_nodim()
  for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
       mit != pivot.ords.end(); ++mit)
  {
    newout << "," << mit->first << ":" << mit->second;
  }
  newout << "]";
  std::string newout_str = newout.str();
  std::u32string tuple32 (newout_str.begin(), newout_str.end());
  std::cout << tuple32 << std::endl;
  Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

  label = Gtk::manage(new Gtk::Label);
  frame = Gtk::manage(new Gtk::Frame);
  (*label).set_label(cell);
  (*frame).add(*label);
  (*comp_table).attach(*frame, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
}

void
spreadsheet::display_comp_cell_nodims()  // single cell and no dimensions
{
  comp_table = Gtk::manage(new Gtk::Table( 2, 2, false));
  (*display_comp_SW).add(*comp_table);

  (*comp_table).set_row_spacing(0,30);
  (*comp_table).set_col_spacing(0,30);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  ");
//   (*label).set_size_request(10,10);
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  // (*comp_table).attach(*label, 0, 1, 0, 1,
  //                      Gtk::FILL|Gtk::EXPAND, Gtk::FILL|Gtk::EXPAND);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

  // Creating a string with expressions and context, calling TL for evaluation
  // std::stringstream newout;
  // newout << "(";
  // newout << (*TLstuff).expression;
  // newout << ")";
  // newout << " @ [time:0";
  // newout << "]";
  // std::string newout_str = newout.str();
  std::string newout_str = "(" + (*TLstuff).expression + ") @ [time:0]";
  // std::cout << newout_str << std::endl;
  std::u32string tuple32 (newout_str.begin(), newout_str.end());
  std::cout << tuple32 << std::endl;
  Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

  label = Gtk::manage(new Gtk::Label);
  frame = Gtk::manage(new Gtk::Frame);
  (*label).set_label(cell);
  (*frame).add(*label);
  (*comp_table).attach(*frame, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
}


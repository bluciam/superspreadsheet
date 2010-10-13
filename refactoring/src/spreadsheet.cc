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
  new_equation_hbox(false,10),
  new_expression_hbox(false,10),
  //  window_header("The TransLucid browser" ), \\Label
  system_table (3,3,false),
  equations_show_table(2,2,false),

  // Gtk::Adjustment(
  //   initial_value, lower, upper, step_increment, page_increment, page_size);
  h_spread_limits(0.0, 0.0, 20.0, 1.0, 1.0, 0.0),
  v_spread_limits(0.0, 0.0, 20.0, 1.0, 1.0, 0.0),

  // Gtk::HButtonBox (boxfor equal spacing (30) between buttons.
  last_box(Gtk::BUTTONBOX_END, 30),
  equations_box(Gtk::BUTTONBOX_END, 30),
  system_box(Gtk::BUTTONBOX_SPREAD, 30),
  system_status_button(" _Show Current System Status ", true), 
  redraw_comp_button(" _Redraw SpreadSheet  ", true), 
  tick_button(" Commit: _Tick Time  ", true),
  close_button("_Quit", true)       // to use alt-C to close app

{

  // Set title and size of the SuperSpreadSheet main window
  set_title("The Super SpreadSheet, The S³: The TransLucid Browser");
  add(main_box);
  // main_box.set_size_request(800,600);
  //Put the inner boxes in the outer box:
//  main_box.pack_start(title_hbox, false, false, 10);
  main_box.pack_start(system_box, false, false, 20);
  main_box.pack_start(system_frame,false,false,0);
  main_box.pack_start(expressions_frame,false,false,0);
  main_box.pack_start(equations_frame,false,false,5);
  main_box.pack_start(pivot_comp_hbox,true, true, 0);
  main_box.pack_start(last_hbox, false, false, 0);
  main_box.pack_start(status_bar, false, false, 0);

  // Set the boxes' spacing around the outside of the container to 5 pixels
//  title_hbox.set_border_width(5); 
  system_hbox.set_border_width(5);
  pivot_comp_hbox.set_border_width(5);
  last_hbox.set_border_width(5);
  pivot_vbox.set_border_width(5);
  new_dim_vbox.set_border_width(5);

// Begin title_hbox 
  title_hbox.pack_start(window_header);
// End title_hbox 


// Begin system_box
  system_status_button.signal_clicked().connect(
    sigc::mem_fun( *this, &spreadsheet::on_system_status_clicked) );
  button = Gtk::manage(new Gtk::Button("  _Update system  ", true));
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_update_system) );
  (tick_button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_tick_time) );
  tick_button.set_relief(Gtk::ReliefStyle::RELIEF_HALF);

  system_box.pack_start(tick_button, Gtk::PACK_SHRINK);
  system_box.pack_start(*button);
  system_box.pack_start(system_status_button);
// End system_box

// Begin system_frame :: system_hbox 
  TLstuff = new TLobjects();
  system_hbox.pack_start(system_table);
  system_hbox.pack_end(infoBar_system_status);
  // Begin system_table

    // Header file
  label = Gtk::manage(new Gtk::Label("Header file name"));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  (system_table).attach((*label),0,1,1,2,Gtk::FILL,Gtk::FILL,5);
  (system_table).attach((filename_header_entry),1,2,1,2,Gtk::FILL,Gtk::FILL,5);
  filename_header_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_filename_header) );
  button = Gtk::manage(new Gtk::Button("Or choose file"));
  (system_table).attach(*button,2,3,1,2,Gtk::FILL,Gtk::FILL,5); 
  button->signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_browse_headerfile) );
    // Equations file
  label = Gtk::manage(new Gtk::Label("Equations file name"));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  (system_table).attach((*label),0,1,2,3,Gtk::FILL,Gtk::FILL,5);
  (system_table).attach((filename_eqns_entry),1,2,2,3,Gtk::FILL,Gtk::FILL,5);
  filename_eqns_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_filename_eqns) );
  button = Gtk::manage(new Gtk::Button("Or choose file"));
  (system_table).attach(*button,2,3,2,3,Gtk::FILL,Gtk::FILL,5); 
  button->signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_browse_equationsfile) );
  // End system_table

  // InfoBar is taken from example.
  Gtk::Container* infoBarSystemContainer =
    dynamic_cast<Gtk::Container*>(infoBar_system_status.get_content_area());
  if (infoBarSystemContainer) infoBarSystemContainer->add(label_system_status);
  //  infoBar_system_status.add_button(Gtk::Stock::OK, 0);
  infoBar_system_status.add_button("Hide Status", 0);
  infoBar_system_status.set_message_type(Gtk::MESSAGE_INFO);
  infoBar_system_status.signal_response().connect(
    sigc::mem_fun(*this, &spreadsheet::on_infobar_system_status ) );

  system_frame.set_shadow_type(Gtk::SHADOW_IN);
  system_frame.add(system_hbox);
// End system_frame :: system_hbox


// Begin expressions_frame :: expressions_show_table
  // new_expression_hbox 
  (expressions_show_table).set_col_spacings(10);
  label = Gtk::manage(new Gtk::Label("New Expression"));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  expression_entry.set_width_chars(50);
  expression_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_new_expression) );
  new_expression_hbox.pack_start(*label,false,false,0);
  new_expression_hbox.pack_start(expression_entry,false,false,3);
  expressions_show_table.attach(new_expression_hbox, 0, 1, 0, 1,
                              Gtk::FILL,Gtk::FILL,15,10);
  // expression_table
  expressions.insert ((*TLstuff).expression);
  filling_expressions_table();
  expressions_frame.set_shadow_type(Gtk::SHADOW_IN);
  expressions_frame.add(expressions_show_table);
// End expressions_frame :: expressions_show_table

// Begin equations_frame
  // new_equation_hbox
  label = Gtk::manage(new Gtk::Label("New Equation   "));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  equation_entry.set_width_chars(50);
  equation_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_new_equation) );
  new_equation_hbox.pack_start(*label,false,false,0);
  new_equation_hbox.pack_start(equation_entry,false,false,3);
  equations_show_table.attach(new_equation_hbox, 0, 1, 0, 1,
                              Gtk::FILL,Gtk::FILL,15,10);
  (equations_show_table).set_col_spacings(10);
  // equations_table
  filling_equations_table();
  //equation_box
  button = Gtk::manage(new Gtk::Button("  _Delete ticked equations  ",true));
  (*button).signal_clicked().connect(
    sigc::mem_fun(*this, &spreadsheet::on_delete_ticked_equations) );
  equations_box.pack_end(*button, true, false, 3);
  equations_show_table.attach(equations_box,1,2,1,2,Gtk::FILL,Gtk::FILL,5);

  equations_frame.set_shadow_type(Gtk::SHADOW_IN);
  equations_frame.add(equations_show_table);
// End equations_frame

// Begin pivot_comp_hbox
  // Begin pivot_frame :: pivot_vbox
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
  // End pivot_frame :: pivot_vbox

  // Begin comp_frame
  display_comp();
  comp_frame.set_shadow_type(Gtk::SHADOW_IN);
  comp_frame.add(*display_comp_SW);
  pivot_comp_hbox.pack_start(comp_frame,true,true);
  // End comp_frame
// End pivot_comp_hbox

// Begin last_hbox 
  // last_box.add(redraw_comp_button); // Automatic when there is a change.
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
spreadsheet::on_browse_headerfile()
{
  Gtk::FileChooserDialog dialog("Please choose a header file",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  //Show the dialog and wait for a user response:
  int result = dialog.run();
  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Open clicked." << std::endl;
      std::string filename = dialog.get_filename();
      std::cout << "File selected: " <<  filename << std::endl;
      std::string header_raw = Glib::file_get_contents(filename);
      Glib::ustring header_8 = Glib::locale_to_utf8(header_raw);
      header_32 = std::u32string (header_8.begin(), header_8.end());
      filename_header_entry.set_text(filename);
      std::cout << header_8 << std::endl;
      status_bar.push("Header file read.");
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      status_bar.push("Choose file CANCELed.");
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
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
spreadsheet::on_browse_equationsfile()
{
  Gtk::FileChooserDialog dialog("Please choose an equations file",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  //Show the dialog and wait for a user response:
  int result = dialog.run();
  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Open clicked." << std::endl;
      std::string filename = dialog.get_filename();
      std::cout << "File selected: " <<  filename << std::endl;
      std::string eqns_raw = Glib::file_get_contents(filename);
      Glib::ustring eqns_8 = Glib::locale_to_utf8(eqns_raw);
      eqns_32 = std::u32string (eqns_8.begin(), eqns_8.end());
      filename_eqns_entry.set_text(filename);
      std::cout << eqns_8 << std::endl;
      status_bar.push("Header file read.");
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      status_bar.push("Choose file CANCELed.");
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

void
spreadsheet::filling_equations_table()
{
  int row_size = equations.entities.size() + 1 ;
  equations_table = Gtk::manage(new Gtk::Table(row_size,4,false));
  (*equations_table).set_col_spacings(10);
  if ((equations.entities.empty()))
    return;

  label = Gtk::manage(new Gtk::Label("  UUID  "));
  (*equations_table).attach((*label), 0, 1, 0, 1,Gtk::FILL,Gtk::FILL,5,8);
  label = Gtk::manage(new Gtk::Label("  Equations  "));
  (*equations_table).attach((*label), 1, 2, 0, 1,Gtk::FILL,Gtk::FILL,5,8);
  label = Gtk::manage(new Gtk::Label("Delete"));
  (*equations_table).attach((*label), 2, 3, 0, 1,Gtk::FILL,Gtk::FILL,5,8);
  if (!(equations.entities.empty()))
  {
  // Create a multimap to sort by equation. 
  std::multimap<Glib::ustring, TransLucid::uuid> temp_equations;
  for ( std::map<TransLucid::uuid,Glib::ustring>::iterator 
             mit=(equations.entities).begin();
             mit != (equations.entities).end(); ++mit 
      )
      temp_equations.insert(std::pair<Glib::ustring,TransLucid::uuid>(
                     (*mit).second , (*mit).first ));
  // Display the multimap in the equations table
  int i = 1;
  for ( std::multimap<Glib::ustring , TransLucid::uuid >::iterator 
             mit=(temp_equations).begin();
             mit != (temp_equations).end();
             ++mit , ++i )
  {
    std::stringstream ss;
    ss << (*mit).second;
    Glib::ustring uuid_str = ss.str();
    label = Gtk::manage(new Gtk::Label(uuid_str));
    (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_TOP);
    (*equations_table).attach((*label), 0, 1, i, i+1, Gtk::FILL,Gtk::FILL);
//    int size = (*mit).first.size();
    entry = Gtk::manage(new Gtk::Entry());
    (*entry).set_width_chars((*mit).first.size());
//    (*entry).set_width_chars(size);
    (*entry).set_text((*mit).first);
    (*equations_table).attach((*entry), 1, 2, i, i+1);

    checkbutton = Gtk::manage(new Gtk::CheckButton());
    (*checkbutton).set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_TOP);
    (*equations_table).attach((*checkbutton), 2, 3, i, i+1, 
                              Gtk::FILL,Gtk::FILL);
    (*checkbutton).signal_toggled().connect(
      sigc::bind(
        sigc::mem_fun(*this, &spreadsheet::on_tick_equation),
                      checkbutton, (*mit).second ) );
    }
  }
  equations_show_table.attach((*equations_table), 0, 1, 1, 2,
                              Gtk::FILL,Gtk::FILL,5,15);
}

void
spreadsheet::on_infobar_system_status(int)
{
  infoBar_system_status.hide();
}

void
spreadsheet::on_system_status_clicked()
{
  std::stringstream ss;
/*
  ss << "Current expression is \"" << (*TLstuff).expression 
       << "\"" << std::endl;
  for (std::map<Glib::ustring,int>::iterator mit=(pivot.ords).begin(); 
       mit != (pivot.ords).end(); 
       ++mit )
  {
    ss << "Dimension \"" << (*mit).first << "\" = " << (*mit).second 
         << std::endl;
  }
*/

  ss << "Loaded libraries in header: library ustring<int>" << std::endl;
  ss << "Available operators:" << std::endl;
  ss << "  infixl ustring<-> ustring<operator-> 5" << std::endl;
  ss << "  infixl ustring<+> ustring<operator+> 5" << std::endl;
  ss << "  infixl ustring<*> ustring<operator*> 10" << std::endl;
  std::string text = ss.str();

  label_system_status.set_text(text);
  infoBar_system_status.show(); // To show the widget when status is clicked.
  //  system_status_button.hide();
}

void
spreadsheet::on_update_system()
{
  status_bar.push("Updating system");
  std::cout << "Loading header : " << std::endl << "\"" << std::endl 
            << header_32 << "\"" << std::endl;
  std::cout << "loading equations : " << std::endl << "\"" << std::endl 
            << eqns_32 << "\"" << std::endl;
  try
  {
    (*TLstuff).traductor.parse_header ( header_32 ) ;
    // bool status = TLstuff -> add_header(string);
    // Benefit: try and catch local to TLstuff, not here.
  }
  catch (...)
  {
    Glib::ustring msg = " Could not load and parse the header." ;
    std::cout << msg << std::endl;
    status_bar.push(msg);
  }
  // Add header dimensions to the sss dimension structure.
  (*TLstuff).traductor.header().dimension_symbols.
             for_each(HeaderDims_In_PivotOrds(pivot)); 
  try
  {
    // Add equations to TL system
    (*TLstuff).traductor.translate_and_add_equation_set ( eqns_32 ) ;
    // Read equations from TL system and add to List_uuid_n_entity
    TL::Translator::EquationIterator
         begin_eqs = (*TLstuff).traductor.beginEquations();
    TL::Translator::EquationIterator
         end_eqs = (*TLstuff).traductor.endEquations();
    // TODO what happens if adding an equation already added?
    for ( ; begin_eqs != end_eqs ; )
    {
      std::cout << begin_eqs.print() << std::endl;
      std::cout << begin_eqs.id() << std::endl;
      equations.add_entity(begin_eqs.id(), begin_eqs.print());
      // Readraw the system_table using filling_equations_table()
      equations_show_table.remove(*equations_table);
      delete(equations_table);
      filling_equations_table();
      equations_show_table.show_all_children();
      begin_eqs++;
    }
  }
  catch (...)
  {
    Glib::ustring msg =  " Could not translate and add the equations." ;
    std::cout << msg << std::endl;
    status_bar.push(msg);
  }
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
}

void
spreadsheet::on_new_expression()
{
  Glib::ustring new_expr = expression_entry.get_text();
  (*TLstuff).expression = new_expr;
  status_bar.push("Expression \"" + new_expr + "\" entered.");
  std::cout << "Expression \"" + new_expr + "\" entered."
            << std::endl;
  expressions.insert (new_expr);
  expressions_show_table.remove(*expressions_table);
  delete(expressions_table);
  filling_expressions_table();
  expressions_show_table.show_all_children();
  expression_entry.set_text("");
}

void
spreadsheet::on_new_equation()
{
  Glib::ustring new_eqn = equation_entry.get_text();
  // TODO: update TLsystem somehow
  // TODO: verify validity of equation entered
  status_bar.push("Equation \"" + new_eqn + "\" entered.");
  std::cout << "Equation \"" + new_eqn + "\" entered."
            << std::endl;
  // TODO: add to equations.entities once I have a uuid
  //       equations.entities[uuid] = new_eqn;

/*
  TODO to put in once I know how to add an equations to the TLsystem
  equations_show_table.remove(*equations_table);
  delete(equations_table);
  filling_equations_table();
  equations_show_table.show_all_children();
  equation_entry.set_text("");
*/

}

void
spreadsheet::on_active_expr(Gtk::RadioButton * active, Glib::ustring expr)
{
  if ((*active).get_active())
  {
    (*TLstuff).expression = expr;
    redraw_comp_button.clicked();
  }
}

void
spreadsheet::on_change_expr(Gtk::Entry * entry)
{
  Glib::ustring new_value = entry->get_text();
//  std::cout << new_value << std::endl;
  expressions.insert (new_value);
  expressions_show_table.remove(*expressions_table);
  delete(expressions_table);
  filling_expressions_table();
  expressions_show_table.show_all_children();

}

void
spreadsheet::on_delete_expr(Gtk::RadioButton * active, Glib::ustring expr)
{
  expressions.erase (expr);
  if ((*active).get_active()) 
  {
    (*TLstuff).expression = "";
    redraw_comp_button.clicked();
  }
  expressions_show_table.remove(*expressions_table);
  delete(expressions_table);
  filling_expressions_table();
  expressions_show_table.show_all_children();
}

void
spreadsheet::filling_expressions_table()
{

  // TODO create the table with the right size
  expressions_table = Gtk::manage(new Gtk::Table());

  label = Gtk::manage(new Gtk::Label("Previous Expressions"));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  (*expressions_table).attach((*label),0,1,2,3,Gtk::FILL,Gtk::FILL,5);

  label = Gtk::manage(new Gtk::Label("Delete"));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  (*expressions_table).attach((*label),2,3,1,2,Gtk::FILL,Gtk::FILL,5);

  label = Gtk::manage(new Gtk::Label("Activate"));
  (*label).set_alignment(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER);
  (*expressions_table).attach((*label),3,4,1,2,Gtk::FILL,Gtk::FILL,5);
  act_exp_checkbutton = Gtk::manage(new Gtk::RadioButton);
  Gtk::RadioButton::Group active_group = (*act_exp_checkbutton).get_group();

  std::set<Glib::ustring>::iterator sit;
  int i = 2;
  for (sit = expressions.begin(); sit != expressions.end(); sit++, i++)
  {
    // Entry with the current expression as in the set::expressions.
    entry = Gtk::manage(new Gtk::Entry());
    (*entry).set_text(*sit);
    (*expressions_table).attach((*entry),1,2,i,i+1,Gtk::FILL,Gtk::FILL,5);
    entry->signal_activate().connect(
      sigc::bind(
        sigc::mem_fun(*this,&spreadsheet::on_change_expr), entry) );

    // RadioButon to "Activate" button
    act_exp_checkbutton = Gtk::manage(new Gtk::RadioButton);
    (*act_exp_checkbutton).set_group(active_group);
    (*act_exp_checkbutton).set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_TOP);
    if ((*sit) == (*TLstuff).expression)
    {
      (*act_exp_checkbutton).set_active();
      on_active_expr((act_exp_checkbutton), (*sit));
    }
    (*expressions_table).attach((*act_exp_checkbutton), 3, 4, i, i+1,
                                Gtk::FILL,Gtk::FILL,15);
    (*act_exp_checkbutton).signal_toggled().connect(
      sigc::bind(
        sigc::mem_fun(*this,&spreadsheet::on_active_expr),
                      act_exp_checkbutton, (*sit)));

    // CheckButton to "Delete" button
    checkbutton = Gtk::manage(new Gtk::CheckButton());
    (*checkbutton).set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_TOP);
    (*expressions_table).attach((*checkbutton), 2, 3, i, i+1,
                               Gtk::FILL,Gtk::FILL,15);
    (*checkbutton).signal_toggled().connect(
      sigc::bind(
        sigc::mem_fun(*this,&spreadsheet::on_delete_expr), 
                      act_exp_checkbutton, *sit));
  }
  expressions_show_table.attach((*expressions_table),0,1,1,2,
                                 Gtk::FILL,Gtk::FILL,5,15);
}

void
spreadsheet::on_tick_equation(Gtk::CheckButton * del, TransLucid::uuid uuid)
{
  if ((*del).get_active())
    equations_to_del.insert(uuid);
  else 
    equations_to_del.erase(uuid);
}

void
spreadsheet::on_delete_ticked_equations()
{
  std::set<TransLucid::uuid>::iterator sit;
  for (sit = equations_to_del.begin(); sit != equations_to_del.end(); sit++ )
  {
    std::cout << "To delete :" << *sit << std::endl;
    equations.del_entity(*sit);
    equations_show_table.remove(*equations_table);
    delete(equations_table);
    filling_equations_table();
    equations_show_table.show_all_children();
    // TODO: delete equations from TLsystem
  }
  redraw_comp_button.clicked();
  equations_to_del.clear();
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
  label = Gtk::manage(new Gtk::Label("Dimensions"));
  (*pivot_table).attach((*label),0,1,0,1,Gtk::FILL,Gtk::FILL);

  vbox = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label("V dim\nradius"));
  (*vbox).add(*label);
  (*vbox).add(*v_spread_spin);
  (*pivot_table).attach((*vbox),1,2,0,1,Gtk::FILL,Gtk::FILL);
  v_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_v_spread_spin ) );

  vbox = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label("H dim\nradius"));
  (*vbox).add(*label);
  (*vbox).add(*h_spread_spin);
  (*pivot_table).attach((*vbox),2,3,0,1,Gtk::FILL,Gtk::FILL);
  h_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_spread_spin ) );

  label = Gtk::manage(new Gtk::Label("Pivot"));
  (*pivot_table).attach((*label),3,4,0,1,Gtk::FILL,Gtk::FILL);

  label = Gtk::manage(new Gtk::Label("Delete\ndimension?"));
  (*pivot_table).attach((*label),4,5,0,1,Gtk::FILL,Gtk::FILL);

  // First row: no dimension chosen for computation display
  label = Gtk::manage(new Gtk::Label("None"));
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

    Glib::ustring dim = (*mit).first;
    label = Gtk::manage(new Gtk::Label(dim));
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
      (*hdisplay).set_active();
    else if ((pivot.v_dim) == dim)
      (*vdisplay).set_active();

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
  std::stringstream ss;
  ss << pivot.v_radius;
  status_bar.push( "The new v_radius is " + ss.str());
  redraw_comp_button.clicked();
}

void
spreadsheet::on_h_spread_spin()
{
  (pivot.h_radius) = h_spread_spin->get_value();
  std::stringstream ss;
  ss << pivot.h_radius;
  status_bar.push( "The new h_radius is " + ss.str());
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
  if ((*h_radio).get_active()) 
  { // When a dim is chosen
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
  std::stringstream ss;
  ss << new_pivot;
  std::string msg = "New pivot for dim \"" + dim + "\" is \"" + 
                    ss.str() + "\"";
  status_bar.push(msg);
  pivot.ords[dim] = new_pivot;
  redraw_comp_button.clicked();
}

void
spreadsheet::display_comp()
{
  display_comp_SW = Gtk::manage(new Gtk::ScrolledWindow);
  (*display_comp_SW).set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);

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
  (*comp_table).set_col_spacings(5);
  (*comp_table).set_row_spacings(5);
  (*display_comp_SW).add(*comp_table);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label( pivot.v_dim + " \\ " + pivot.h_dim ));
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK,5,5);

  //  Drawing horizontal index 
  for (int i = 0 ; i != row_range ; ++i)
  {
    std::stringstream ss;
    ss << h_min + i ;
    label = Gtk::manage(new Gtk::Label(ss.str()));
    (*comp_table).attach(*label, i+1, i+2, 0, 1, Gtk::SHRINK, Gtk::SHRINK,5,5);
  }

  //  Drawing vertical index 
  for (int j = 0 ; j < col_range ; ++j)
  {
    std::stringstream ss;
    ss << v_min + j ;
    label = Gtk::manage(new Gtk::Label(ss.str()));
    (*comp_table).attach(*label, 0, 1, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK,5,5);
  }

  // Drawing content
  for (int i = 0 ; i != row_range ; ++i)
  {
    for (int j = 0 ; j != col_range ; ++j)
    {
    // Creating a string with expressions and context, calling TL for evaluation
      std::cout << "expression = " << (*TLstuff).expression << std::endl;
      std::stringstream ss;
      ss << "(";
      ss << (*TLstuff).expression;
      ss << ")";
      ss << " @ [time:0, ";
      ss << pivot.h_dim << ":" << (i+h_min) << ", ";
      ss << pivot.v_dim << ":" << (j+v_min);
      for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
           mit != pivot.ords.end(); ++mit)
      {
         if (mit->first != pivot.h_dim && mit->first != pivot.v_dim)
         {
           ss << ", " << mit->first << ":" << mit->second;
         }
      }
      ss << "]";
      std::string ss_str = ss.str();
      std::u32string tuple32 (ss_str.begin(), ss_str.end());
      std::cout << tuple32 << std::endl;
      Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

      label = Gtk::manage(new Gtk::Label(cell));
      frame = Gtk::manage(new Gtk::Frame);
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

  (*comp_table).set_row_spacing(0,20);
  (*comp_table).set_col_spacing(0,20);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label(pivot.h_dim));
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK,5,5);

  for (int i = 0 ; i != row_range ; ++i)
  {
    //  Drawing horizontal index 
    std::stringstream ss;
    ss << h_min + i ;
    label = Gtk::manage(new Gtk::Label(ss.str()));
    (*comp_table).attach(*label, i+1, i+2, 0, 1, Gtk::SHRINK, Gtk::SHRINK,5,5);

    // Drawing content
    // Creating a string with expressions and context, calling TL for evaluation
    std::cout << "expression = " << (*TLstuff).expression << std::endl;
    ss.str("");
    ss << "(" << (*TLstuff).expression << ")";
    ss << " @ [time:0, ";
    ss << pivot.h_dim << ":" << (i+h_min) ;
    for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
         mit != pivot.ords.end(); ++mit)
    {
      if (mit->first != pivot.h_dim)
      {
        ss << ", " << mit->first << ":" << mit->second;
      }
    }
    ss << "]";
    std::string ss_str = ss.str();
    std::u32string tuple32 (ss_str.begin(), ss_str.end());
    std::cout << tuple32 << std::endl;
    Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label(cell));
    frame = Gtk::manage(new Gtk::Frame);
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

  (*comp_table).set_row_spacing(0,20);
  (*comp_table).set_col_spacing(0,20);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label(pivot.v_dim));
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK,5,5);

  for (int j = 0 ; j != col_range ; ++j)
  {
    //  Drawing vertical index 
    std::stringstream ss;
    ss << v_min + j ;
    label = Gtk::manage(new Gtk::Label(ss.str()));
    (*comp_table).attach(*label, 0, 1, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK,5,5);

    // Drawing content
    // Creating a string with expressions and context, calling TL for evaluation
    std::cout << "expression = " << (*TLstuff).expression << std::endl;
    ss.str("");
    ss << "(" << (*TLstuff).expression << ")";
    ss << " @ [time:0, ";
    ss << pivot.v_dim << ":" << (j+v_min) ;
    for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
         mit != pivot.ords.end(); ++mit)
    {
      if (mit->first != pivot.v_dim)
      {
        ss << ", " << mit->first << ":" << mit->second;
      }
    }
    ss << "]";
    std::string ss_str = ss.str();
    std::u32string tuple32 (ss_str.begin(), ss_str.end());
    std::cout << tuple32 << std::endl;
    Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label(cell));
    frame = Gtk::manage(new Gtk::Frame);
    (*frame).add(*label);
    (*comp_table).attach(*frame, 1, 2, j+1, j+2, Gtk::SHRINK, Gtk::SHRINK);
  }
}

void
spreadsheet::display_comp_cell()  // single cell
{
  comp_table = Gtk::manage(new Gtk::Table( 2, 2, false));
  (*display_comp_SW).add(*comp_table);

  (*comp_table).set_row_spacing(0,20);
  (*comp_table).set_col_spacing(0,20);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label("  "));
  (*label).set_label("  ");
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

  // Creating a string with expressions and context, calling TL for evaluation
  std::stringstream ss;
  ss << "(" << (*TLstuff).expression << ")";
  ss << " @ [time:0";
  // There is at least one element in pivot.ords otherwise control would be
  // given to display_comp_cel_nodim()
  for (std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
       mit != pivot.ords.end(); ++mit)
  {
    ss << "," << mit->first << ":" << mit->second;
  }
  ss << "]";
  std::string ss_str = ss.str();
  std::u32string tuple32 (ss_str.begin(), ss_str.end());
  std::cout << tuple32 << std::endl;
  Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

  label = Gtk::manage(new Gtk::Label(cell));
  frame = Gtk::manage(new Gtk::Frame);
  (*frame).add(*label);
  (*comp_table).attach(*frame, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
}

void
spreadsheet::display_comp_cell_nodims()  // single cell and no dimensions
{
  comp_table = Gtk::manage(new Gtk::Table( 2, 2, false));
  (*display_comp_SW).add(*comp_table);

  (*comp_table).set_row_spacing(0,20);
  (*comp_table).set_col_spacing(0,20);

  // Label cell, first cell
  label = Gtk::manage(new Gtk::Label("  "));
  (*label).set_width_chars(10);
  (*label).set_line_wrap(true);
  (*comp_table).attach(*label, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);

  std::string ss_str = "(" + (*TLstuff).expression + ") @ [time:0]";
  std::u32string tuple32 (ss_str.begin(), ss_str.end());
  std::cout << tuple32 << std::endl;
  Glib::ustring cell = (*TLstuff).calculate_expr(tuple32);

  label = Gtk::manage(new Gtk::Label(cell));
  frame = Gtk::manage(new Gtk::Frame);
  (*frame).add(*label);
  (*comp_table).attach(*frame, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
}


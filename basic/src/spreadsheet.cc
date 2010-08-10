#include <cstdio>
#include <map>
#include "spreadsheet.h"

std::map<Glib::ustring,int> tuples;
std::map<Glib::ustring,int>::iterator it;

Glib::ustring * v_dim = new Glib::ustring;
Glib::ustring * h_dim = new Glib::ustring;
int           * h_radius = new int;     // horizontal radius of display
int           * v_radius = new int;     // vertical radius of display

Glib::ustring drawn_v_dim;
Glib::ustring drawn_h_dim;

spreadsheet::spreadsheet() :
  main_box  (false, 10),   // false: child widgets don't have the same width
  hbox_title(false, 10),   //    10: pixels between widgets
  hbox_exprs (false, 10),
  hbox_last (false, 10),

  window_title("The S³ is displaying these dimensions ..."),
  last_box(Gtk::BUTTONBOX_END, 30),   // last_button is Gtk::HButtonBox for
                                      //   equal spacing (30) of buttons.
  del_dim_button("_Delete dimension", true),
  add_dim_button("_Add dimension", true),

  cancel_button("_Cancel",true),
  status_button(" _Show Status ",true), 
  redraw_button(" _Redraw SpreadSheet  ",true), 
  quit_button(Gtk::Stock::QUIT),
  close_button("_Quit", true)       // to use alt-C to close app
{
  (*h_radius) = 3;
  (*v_radius) = 4;

  tuples["x"] = 5;
  tuples["y"] = 7;
  tuples["w"] = 11;
  tuples["z"] = 42;
  tuples["t"] = 8;

  // Initial dimensions chosen for display by the programmer
  it=(tuples).begin();
  (*h_dim) = (*(it++)).first;
  (*v_dim) = (*it).first;

  expression = "0";

  // Connecting to TL code
  create_equations();

  // Set title and border of the window
  set_title("The Super SpreadSheet, The S³");
//  set_border_width(10);
//  set_default_size(800,400);

  // Add outer box to the window since the window may contain a single widget 
  add(main_box);

  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox_title);
  main_box.pack_start(hbox_exprs);
  main_box.pack_start(hpaned_content);
  main_box.pack_start(edit_dim_frame);
  main_box.pack_start(hbox_last);
  main_box.pack_start(status_bar);

  // Set the boxes' spacing around the outside of the container: 5 pixels
  hbox_title.set_border_width(5); 
  hbox_exprs.set_border_width(5);
  hbox_last.set_border_width(5);

// Begin hbox_title 
  hbox_title.pack_start(window_title);
// End   hbox_title 


// Begin hbox_exprs 
  //  exprs_entry.set_max_length(50);
  exprs_entry.set_text("Enter your expression");
  exprs_entry.set_text(exprs_entry.get_text() + " and press Enter.");
  exprs_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  exprs_entry.signal_icon_press().connect( sigc::mem_fun(*this,
             &spreadsheet::on_icon_pressed_exprs) );
  exprs_entry.signal_activate().connect( sigc::mem_fun(*this,
             &spreadsheet::on_get_exprs) );
  hbox_exprs.pack_start(exprs_entry);
// End hbox_exprs 


// Begin hpaned_content 
  // Begin first table
  info_sheet = Gtk::manage(new display_info(
    &tuples, h_radius, v_radius, h_dim, v_dim ) );

  info_frame.set_shadow_type(Gtk::SHADOW_IN);
  info_frame.add(*info_sheet);
  hpaned_content.pack1(info_frame,Gtk::FILL);
  // End first table


  // Begin second table
  dimensions_sheet = Gtk::manage(new display_dims(
    ((*h_radius) * 2 + 1 ),
    ((*v_radius) * 2 + 1 ),
    ( tuples[(*h_dim)] - (*h_radius) ),
    ( tuples[(*v_dim)] - (*v_radius) ),
    1,
    tuples, h_dim, v_dim, traductor, expression ));

    drawn_h_dim = (*h_dim);
    drawn_v_dim = (*v_dim);

  content_frame.set_shadow_type(Gtk::SHADOW_IN);
  content_frame.add(*dimensions_sheet);
  hpaned_content.pack2(content_frame);
  // End second table
// End hpaned_content 

// Begin hbox_edit_dim
  edit_dim_frame.add(hbox_edit_dim);
  hbox_edit_dim.set_border_width(5);
  hbox_edit_dim.pack_start(add_dim_button, Gtk::FILL, 2 );
  hbox_edit_dim.pack_start(del_dim_button, Gtk::FILL, 2 );

  del_dim_button.signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_del_dimension),
                     "Deleting dimension. Click on the chosen dimension.") );

  add_dim_button.signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_add_dimension),
                     "Adding dimension. Enter the appropriate values.") );

  hbox_add_dim.set_border_width(5);
  label = Gtk::manage(new Gtk::Label("Add new dimension:     "));
  hbox_add_dim.pack_start(*label);

  Gtk::HBox * hbox = Gtk::manage(new Gtk::HBox);
  label = Gtk::manage(new Gtk::Label("Name "));
  (*hbox).pack_start(*label); 
  (*hbox).pack_start(new_dim_entry); 
  hbox_add_dim.pack_start(*hbox);

  hbox = Gtk::manage(new Gtk::HBox);
  label = Gtk::manage(new Gtk::Label("Pivot "));
  (*hbox).pack_start(*label); 
  (*hbox).pack_start(new_pivot_entry); 
  hbox_add_dim.pack_start(*hbox);

  button = Gtk::manage(new Gtk::Button("_OK",true));
  hbox_add_dim.pack_start(*button);
  (*button).signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_add_OK),
                     "New values entered...") );

  hbox_add_dim.pack_start(cancel_button);
  (cancel_button).signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_cancel_edit),
                     "Cancelling adding dimensions..." ) );
// End hbox_edit_dim

// Begin hbox_last 
  // InfoBar is taken from example, not fully understood.
  Gtk::Container* infoBarContainer =
    dynamic_cast<Gtk::Container*>(infoBar_status.get_content_area());
  if (infoBarContainer) infoBarContainer->add(label_status);
  infoBar_status.add_button(Gtk::Stock::OK, 0);
//  label_status.set_text("Status is ");
  infoBar_status.set_message_type(Gtk::MESSAGE_INFO);
  infoBar_status.signal_response().connect(
    sigc::mem_fun(*this, &spreadsheet::on_infobar_status ) );
  hbox_last.pack_start(infoBar_status, Gtk::PACK_SHRINK);

  last_box.add(status_button);
  last_box.add(redraw_button);
  last_box.add(close_button);
  hbox_last.pack_start(last_box);

  // Make the button the default widget. No idea what this 
  // does, as the explanation does not match the functionality.
  close_button.set_can_default();
  close_button.grab_default();

  // Connecting the signal to the corresponding handlers.
  redraw_button.signal_clicked().connect(
    sigc::bind<Glib::ustring> (
      sigc::mem_fun( *this, &spreadsheet::on_redraw_clicked),"Redrawing...") );
  status_button.signal_clicked().connect(
    sigc::bind<Glib::ustring> (
      sigc::mem_fun(
        *this, &spreadsheet::on_status_clicked), "Showing current status") );
  close_button.signal_clicked().connect (
    sigc::mem_fun( *this, &spreadsheet::on_closebutton_clicked) );
  quit_button.signal_clicked().connect(
    sigc::ptr_fun( &Gtk::Main::quit));
// End hbox_last 


  // Show all children of the window
  show_all_children();
  infoBar_status.hide(); // InfoBar shown only when status button is pressed

}


void
spreadsheet::on_add_dimension(Glib::ustring msg)
{
  edit_dim_frame.remove();
  edit_dim_frame.add(hbox_add_dim);
  edit_dim_frame.show_all_children();
  std::cout << msg << std::endl;
  status_bar.push(msg);
}

void
spreadsheet::on_add_OK(Glib::ustring msg)
{
  std::cout << msg << std::endl;
  Glib::ustring dimname  = new_dim_entry.get_text();
  Glib::ustring dimpivot = new_pivot_entry.get_text();
  std::cout << "  Name: "  << dimname << std::endl;
  std::cout << "  Pivot: " << dimpivot << std::endl;
// TODO need to check that pivot is a number
//      if dimname exist already as a key
  if (dimname == "" && dimpivot == "") {        // empty fields
    cancel_button.clicked();
  } else if (dimname == "" || dimpivot == "") { // only one is empty
    new_dim_entry.set_text("");
    new_pivot_entry.set_text("");
    add_dim_button.clicked(); // for it to show, I need to clear the text...
  } else {
  new_dim_entry.set_text("");
  new_pivot_entry.set_text("");

  int pivot =  atoi(dimpivot.c_str());
  tuples[dimname] = pivot; 

  info_frame.remove();
  info_sheet = Gtk::manage(new display_info(
    &tuples, h_radius, v_radius, h_dim, v_dim ) );
  info_frame.add(*info_sheet);
  (*info_sheet).show();

  status_bar.push("Adding dimension name = \"" + dimname + 
                  "\" and pivot = \"" + dimpivot + "\"");
  edit_dim_frame.remove();
  edit_dim_frame.add(hbox_edit_dim);
  edit_dim_frame.show_all_children();
  }
}

void
spreadsheet::on_del_dimension(Glib::ustring msg)
{
  label = Gtk::manage(new Gtk::Label("Delete which dimension?"));
  hbox_del_dim = Gtk::manage(new Gtk::HBox());
  (*hbox_del_dim).pack_start(*label);
  for ( it=tuples.begin() ; it != tuples.end(); it++ ) {
    button = Gtk::manage(new Gtk::Button((*it).first));
    (*hbox_del_dim).pack_start(*button,Gtk::FILL,2);

    button->signal_clicked().connect( sigc::bind (
      sigc::mem_fun (
        *this, &spreadsheet::on_which_dimension ), ((*it).first) ) ) ;
  }
  button = Gtk::manage(new Gtk::Button("_Cancel",true));
  (*hbox_del_dim).pack_start(*button);
  (*button).signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_cancel_edit),
                     "Cancelling deleting dimensions..." ) );

  edit_dim_frame.remove();
  edit_dim_frame.add(*hbox_del_dim);
  edit_dim_frame.show_all_children();
  std::cout << msg << std::endl;
  status_bar.push(msg);
}

void
spreadsheet::on_which_dimension(Glib::ustring dim)
{
  edit_dim_frame.remove();
  edit_dim_frame.add(hbox_edit_dim);
  (tuples).erase (dim);

  if (dim == (*h_dim)) {
    (*h_dim) = "";
    (*info_sheet).hnodisplay.toggled();
  } else if (dim == (*v_dim)) {
    (*v_dim) = "";
    (*info_sheet).vnodisplay.toggled();
  } 

  info_frame.remove();
  info_sheet = Gtk::manage(new display_info(
    &tuples, h_radius, v_radius, h_dim, v_dim ) );
  info_frame.add(*info_sheet);
  (*info_sheet).show();
  edit_dim_frame.show_all_children();
  status_bar.push("Deleting dimension " + dim);
}

void
spreadsheet::on_cancel_edit(Glib::ustring msg)
{
  edit_dim_frame.remove();
  edit_dim_frame.add(hbox_edit_dim);
  std::cout << msg << std::endl;
  status_bar.push("Canceling the editing of dimensions.");
}

void
spreadsheet::on_redraw_clicked(Glib::ustring msg)
{
  std::cout << msg << std::endl;
  content_frame.remove();

  drawn_h_dim = (*h_dim);
  drawn_v_dim = (*v_dim);

  if (( (*h_dim) == "") && ((*v_dim) == "")) {
    dimensions_sheet = Gtk::manage(new display_dims());
  } else if ( (*h_dim) == "" ) {
  dimensions_sheet = Gtk::manage(new display_dims(
    1, ( (*v_radius) * 2 + 1 ), 0, ( tuples[(*v_dim)] - (*v_radius) ), 2,
    tuples, h_dim, v_dim, traductor, expression ));
  } else if ( (*v_dim) == "" ) {
  dimensions_sheet = Gtk::manage(new display_dims(
    ( (*h_radius) * 2 + 1 ), 1 , ( tuples[(*h_dim)] - (*h_radius) ), 0 , 3,
      tuples, h_dim, v_dim, traductor, expression));
  } else {
  dimensions_sheet = Gtk::manage(new display_dims(
    ( (*h_radius) * 2 + 1 ),
    ( (*v_radius) * 2 + 1 ),
    ( tuples[(*h_dim)] - (*h_radius) ),
    ( tuples[(*v_dim)] - (*v_radius) ),
    1,
    tuples, h_dim, v_dim, traductor, expression ));
  }

  content_frame.add(*dimensions_sheet);
  (*dimensions_sheet).show();
  status_bar.push("Redrawing the spreadsheet.");
}

void
spreadsheet::on_get_exprs()
{
  expression = exprs_entry.get_text();
  status_bar.push("Expression \"" + expression + "\" entered.");
  std::cout << "Expression \"" + expression + "\" entered." << std::endl;
  std::u32string expr32;
  for (Glib::ustring::iterator it = expression.begin();
       it != expression.end(); ++it)
  {
    int i = *it;
    char32_t c = i;
    expr32.push_back(c);
  }
  TL::HD *h = traductor.translate_expr(expr32);
  TL::TaggedConstant v = (*h)(TL::Tuple());
  mpz_class ival = v.first.value<TL::Intmp>().value();
  std::cout << "Result = \"" << ival << "\"." << std::endl;
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
  if ((*h_dim) == "")
    hd = "No horizontal dimension chosen.";
  else
    hd = "The horizontal dimension is " + (*h_dim);
  if ((*v_dim) == "")
    vd = "\nNo vertical dimension chosen.";
  else
    vd = "\nThe vertical dimension is " + (*v_dim);
  if (expression == "")
    exp = "\nThere is no expression.";
  else
    exp = "\nExpression is " + expression ;
  if (drawn_h_dim == "")
    dhd = "\nNo horizontal dimension drawn.";
  else
    dhd = "\nDrawn horizontal dimension is " + (drawn_h_dim) ;
  if (drawn_v_dim == "")
    dvd = "\nNo vertical dimension drawn.";
  else
    dvd = "\nDrawn vertical dimension is " + (drawn_v_dim);
  Glib::ustring text = hd + vd + exp + dhd + dvd;
  label_status.set_text(text);
  infoBar_status.show(); // To show the message when status is clicked.
  /* http://library.gnome.org/devel/gtkmm-tutorial/unstable/sec-progressbar.html.en
   * to introduce progress bar instead of the Ok button.
   */
//  std::cout <<  msg << std::endl;
  status_bar.push(msg);
}

void
spreadsheet::on_infobar_status(int)
{
  infoBar_status.hide();
}

void
spreadsheet::create_equations ()
{
  // TL::Translator traductor;
  // traductor.loadLibrary(U"int");
  traductor.parse_header (
    U"dimension ustring<n>;;"
    U"infixl ustring<-> ustring<operator-> 5;;"
    U"infixl ustring<+> ustring<operator+> 5;;"
    U"infixl ustring<*> ustring<operator*> 10;;"
    U"library ustring<int>;;"
    U"dimension ustring<t>;;"
    U"dimension ustring<w>;;"
    U"dimension ustring<x>;;"
    U"dimension ustring<y>;;"
    U"dimension ustring<z>;;"
  );
  traductor.translate_and_add_equation_set (
    U"fact | [n:0] = 1;;"
    U"fact = #n * (fact @ [n:#n-1]);;"
  );
  TL::HD* e = traductor.translate_expr(U"fact @ [n:5]");
  TL::TaggedConstant result = (*e)(TL::Tuple());
  std::cout << "fact(5) = "
            << result.first.value<TL::Intmp>().value() << std::endl;
  delete e;
}

/*
void
spreadsheet::create_equations ()
{
  namespace TL = TransLucid;
  TL::Translator traductor;
  traductor.loadLibrary(U"int");
  std::cout << "Loaded the library" << std::endl;
  TL::Parser::Header& header = traductor.header();
  TL::Parser::addBinaryOpSymbol (
    header, L"+", L"operator+", TL::Tree::ASSOC_LEFT, 5
  );
  TL::Parser::addBinaryOpSymbol (
    header, L"*", L"operator*", TL::Tree::ASSOC_LEFT, 10
  );
  TL::Parser::addBinaryOpSymbol (
    header, L"-", L"operator-", TL::Tree::ASSOC_LEFT, 5
  );
  std::cout << "Loaded the three operators" << std::endl;
  traductor.translate_and_add_equation_set(U" x = 5;; y = 6;;");
  std::cout << "Added the equations" << std::endl;

  TL::HD& system = traductor.system();
  std::cout << "Created a HD system" << std::endl;
  TL::TaggedConstant v = system
  (TL::Tuple(TL::tuple_t(
    {
      {TL::DIM_ID, TL::generate_string(U"x")}
    }
  )));
  std::cout << "Created a tuple" << std::endl;
  std::cout << v.first.index() << " and " << v.first.value<TL::Intmp>().value() << std::endl;
}
*/

void
spreadsheet::on_closebutton_clicked()
{
  std::cout << "Quitting the S³..." << std::endl;
  hide(); //to close the application.
}

spreadsheet::~spreadsheet()
{
  /* Trying to get rid of the problem of not deallocating memory, but this is
     blowing up with display_dims class as a pointer.
  */
//  Glib::Error::register_cleanup();
//  Glib::wrap_register_cleanup();
}

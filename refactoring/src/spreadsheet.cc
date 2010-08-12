#include <cstdio>
#include "spreadsheet.h"

spreadsheet::spreadsheet() :
  // false: child widgets don't have the same width
  //    10: distance in pixels between widgets
  main_box  (false, 10),
  hbox_title(false, 10),
  hbox_exprs (false, 10),
  hbox_last (false, 10),

  window_header("The S³ is displaying these dimensions ..."),
  // last_button is Gtk::HButtonBox for equal spacing (30) between buttons.

  h_spread_limits(0.0, 0.0, 20.0, 1.0, 3.0, 0.0),
  v_spread_limits(0.0, 0.0, 20.0, 1.0, 3.0, 0.0),
            // Gtk::Adjustment( initial_value, 
            //      lower, upper,
            //      step_increment, 
            //      page_increment, 
            //      page_size);

  last_box(Gtk::BUTTONBOX_END, 30),
  del_dim_button("_Delete dimension", true),
  add_dim_button("_Add dimension", true),

  cancel_button("_Cancel",true),
  status_button(" _Show Status ",true), 
  redraw_button(" _Redraw SpreadSheet  ",true), 
  quit_button(Gtk::Stock::QUIT),
  close_button("_Quit", true)       // to use alt-C to close app

{
  // Initializing dims info for display
  (h_radius) = new int(3);
  (v_radius) = new int(3);
//TODO: does not make any sense unless these are part of an equation
  tuples["x"] = 5;
  tuples["y"] = 7;
  tuples["w"] = 11;
  tuples["z"] = 42;
  tuples["t"] = 8;
  // Initial dimensions chosen for display by the programmer
  it=(tuples).begin();
  (h_dim) = new Glib::ustring((*(it++)).first);
  (v_dim) = new Glib::ustring((*it).first);

//  expression = "0";

  // Connecting to TL code
//  create_equations();

  // Set title and size of the SuperSpreadSheet main window
  set_title("The Super SpreadSheet, The S³");
//  set_default_size(800,400);

  // Add outer box to the window since it may only contain a single widget 
  add(main_box);

  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox_title, false, false, 0);
  main_box.pack_start(hbox_exprs, false, false, 0);
  main_box.pack_start(hpaned_content,true, true, 0);
  main_box.pack_start(frame_edit_dim, false, false, 0);
  main_box.pack_start(hbox_last, false, false, 0);
  main_box.pack_start(status_bar, false, false, 0);

  // Set the boxes' spacing around the outside of the container to 5 pixels
  hbox_title.set_border_width(5); 
  hbox_exprs.set_border_width(5);
  hbox_last.set_border_width(5);

// Begin hbox_title 
  hbox_title.pack_start(window_header);
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
  display_info();
  info_frame.set_shadow_type(Gtk::SHADOW_IN);
  info_frame.add(*display_info_SW);
//  hpaned_content.pack1(info_frame,Gtk::FILL);
  hpaned_content.pack1(info_frame,true,false);
//  hpaned_content.pack1(info_frame);
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
//  hpaned_content.pack2(content_frame);
  hpaned_content.pack2(content_frame,true,false);
  // End second table
// End hpaned_content 

// Begin frame_edit_dim
  frame_edit_dim.add(hbox_edit_dim);
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
// End frame_edit_dim

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

  show_all_children();
  infoBar_status.hide(); // InfoBar shown only when status button is pressed
} // End spreadsheet::spreadsheet()


void
spreadsheet::on_add_dimension(Glib::ustring msg)
{
  frame_edit_dim.remove();
  frame_edit_dim.add(hbox_add_dim);
  frame_edit_dim.show_all_children();
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
  display_info();
  info_frame.add(*display_info_SW);
  info_frame.show_all_children();
  (*display_info_SW).show();
  (*display_info_SW).show_all_children();

  status_bar.push("Adding dimension name = \"" + dimname + 
                  "\" and pivot = \"" + dimpivot + "\"");
  frame_edit_dim.remove();
  frame_edit_dim.add(hbox_edit_dim);
  frame_edit_dim.show_all_children();
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

  frame_edit_dim.remove();
  frame_edit_dim.add(*hbox_del_dim);
  frame_edit_dim.show_all_children();
  std::cout << msg << std::endl;
  status_bar.push(msg);
}

void
spreadsheet::on_which_dimension(Glib::ustring dim)
{
  frame_edit_dim.remove();
  frame_edit_dim.add(hbox_edit_dim);
  (tuples).erase (dim);

  if (dim == (*h_dim)) {
    (*h_dim) = "";
    (*hnodisplay).toggled();
  } else if (dim == (*v_dim)) {
    (*v_dim) = "";
    (*vnodisplay).toggled();
  } 

  info_frame.remove();
  display_info();
  info_frame.add(*display_info_SW);
  info_frame.show_all_children();
  (*display_info_SW).show();
  (*display_info_SW).show_all_children();

//  info_sheet = Gtk::manage(new display_info(
//    &tuples, h_radius, v_radius, h_dim, v_dim ) );
//  info_frame.add(*info_sheet);
//  (*info_sheet).show();
  frame_edit_dim.show_all_children();
  status_bar.push("Deleting dimension " + dim);
}

void
spreadsheet::on_cancel_edit(Glib::ustring msg)
{
  frame_edit_dim.remove();
  frame_edit_dim.add(hbox_edit_dim);
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


void
spreadsheet::display_info()
{

  hnodisplay = Gtk::manage(new Gtk::RadioButton);
  vnodisplay = Gtk::manage(new Gtk::RadioButton);

  h_spread_spin = Gtk::manage(new Gtk::SpinButton);
  v_spread_spin = Gtk::manage(new Gtk::SpinButton);

  h_spread_limits.set_value(*h_radius);
  h_spread_spin->set_adjustment(h_spread_limits);
  h_spread_spin->set_size_request(50, -1);
  h_spread_spin->set_numeric(true);
  h_spread_spin->set_alignment(1);

  v_spread_limits.set_value(*v_radius);
  v_spread_spin->set_adjustment(v_spread_limits);
  v_spread_spin->set_size_request(50, -1);
  v_spread_spin->set_numeric(true);
  v_spread_spin->set_alignment(1);

// Begin table
  display_info_SW = Gtk::manage(new Gtk::ScrolledWindow);
  table = Gtk::manage(new Gtk::Table(((tuples).size()+2), 4, false));

  (*table).set_col_spacings(10);
  (*display_info_SW).add(*table);

  // Column titles
  label = manage(new Gtk::Label);
  (*label).set_label("Dimension");
  (*table).attach((*label),0,1,0,1);

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(*h_spread_spin);
  (*label).set_label("H dim\nradius");
  (*table).attach((*box),1,2,0,1);

  h_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_spread_spin ) );

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(*v_spread_spin);
  (*label).set_label("V dim\nradius");
  (*table).attach((*box),2,3,0,1);

  v_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_v_spread_spin ) );

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Pivot");
  (*table).attach((*label),3,4,0,1);

 // First row: no dimension chosen
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("None");
  (*table).attach((*label),0,1,1,2);

  Gtk::RadioButton::Group hgroup = (*hnodisplay).get_group();
  (*table).attach((*hnodisplay),1,2,1,2);
  (*hnodisplay).signal_toggled().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_nodim_toggled ) );

  Gtk::RadioButton::Group vgroup = (*vnodisplay).get_group();
  (*table).attach((*vnodisplay),2,3,1,2);
  (*vnodisplay).signal_toggled().connect(
     sigc::mem_fun( *this, &spreadsheet::on_v_nodim_toggled ) );

  /* Following rows: dimension names and radio buttons in two groups:
      H and V and the pivot value.
  */
  int i = 0; // Can't put this in the for loop, it explodes. 
  for ( it=(tuples).begin(); it != (tuples).end(); ++it, ++i )
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
         sigc::mem_fun(*this, &spreadsheet::on_h_toggled), hdisplay, dim ) );

    vdisplay = Gtk::manage(new Gtk::RadioButton);
    (*vdisplay).set_group(vgroup);
    (*table).attach((*vdisplay),2,3,i+2,ii);
    (*vdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_v_toggled), vdisplay, dim ) );

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
    values->signal_changed().connect(
      sigc::bind (
        sigc::mem_fun (*this, &spreadsheet::on_dimension_pivot_changed), dim) );
  }

// End table
//  table->show();
  (*display_info_SW).show();
//  info_frame.add(*display_info_SW);
//  info_frame.show_all_children();

}

void
spreadsheet::on_h_spread_spin()
{
  (*h_radius) = h_spread_spin->get_value();
  std::cout << (*h_radius) << std::endl;
  h_spread_limits.set_value(*h_radius);
}

void
spreadsheet::on_v_spread_spin()
{
  (*v_radius) = v_spread_spin->get_value();
  std::cout << (*v_radius) << std::endl;
  v_spread_limits.set_value(*v_radius);
}


void
spreadsheet::on_dimension_pivot_changed(Glib::ustring dim)
{
  Glib::ustring s_pivot = values->get_text ();
  std::cout << s_pivot << std::endl;
  int pivot =  atoi(s_pivot.c_str());
  tuples[dim] = pivot;
}

void
spreadsheet::on_h_nodim_toggled()
{
  if ((*hnodisplay).get_active()) {
    std::cout << "No dimension chosen for horizontal display."<< std::endl;
    (*h_dim) = "";
    std::cout << "h_dim = " << (*h_dim) << std::endl;
  }
}

void
spreadsheet::on_v_nodim_toggled()
{
  if ((*vnodisplay).get_active()) {
    std::cout << "No dimension chosen for vertical display." << std::endl;
    (*v_dim) = "";
    std::cout << "v_dim = " << (*v_dim) << std::endl;
  }
}


void
spreadsheet::on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim)
{
  if ((*v_radio).get_active()) { // When a dim is chosen
    if ((*h_dim) == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      (*vnodisplay).set_active();
    } else {
      (*v_dim) = dim;
      std::cout << dim << " chosen for vertical display."<< std::endl;
    }
  } // When a dim is released
}

void
spreadsheet::on_h_toggled(Gtk::RadioButton * h_radio,
                           Glib::ustring dim)
{
  if ((*h_radio).get_active()) { // When a dim is chosen
    if ((*v_dim) == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      (*hnodisplay).set_active();
    } else {
      (*h_dim) = dim;
      std::cout << dim << " chosen for horizontal display."<< std::endl;
    }
  } // When a dim is released
}


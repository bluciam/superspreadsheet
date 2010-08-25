#include <cstdio>
#include <iostream>
#include <fstream>
#include "spreadsheet.h"

spreadsheet::spreadsheet() :
  // false: child widgets don't have the same width
  //    10: distance in pixels between widgets
  main_box  (false, 10),
  hbox_title(false, 10),
  hbox_exprs (false, 10),
  hbox_last (false, 10),
  hbox_edit_dim(false, 10),
  window_header("The TransLucid objects browser" ),

  // Gtk::Adjustment(
  //   initial_value, lower, upper, step_increment, page_increment, page_size);
  h_spread_limits(0.0, 0.0, 20.0, 1.0, 3.0, 0.0),
  v_spread_limits(0.0, 0.0, 20.0, 1.0, 3.0, 0.0),
  // last_button is Gtk::HButtonBox for equal spacing (30) between buttons.
  del_dim_button("_Delete dimension", true),
  add_dim_button("_Add dimension", true),

  last_box(Gtk::BUTTONBOX_END, 30),
  cancel_button("_Cancel",true),
  status_button(" _Show Status ",true), 
  redraw_button(" _Redraw SpreadSheet  ",true), 
  close_button("_Quit", true)       // to use alt-C to close app

{

  // Set title and size of the SuperSpreadSheet main window
  set_title("The Super SpreadSheet, The S³");
  // Add outer box to the window since it may only contain a single widget 
  add(main_box);
  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox_title, false, false, 0);
  main_box.pack_start(hbox_exprs );
//  main_box.pack_start(hbox_exprs, false, false, 0);
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
  label = Gtk::manage(new Gtk::Label("Enter an expression"));
  hbox_exprs.pack_start(*label);


  exprs_entry.set_text(TLstuff.expression);
  exprs_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  exprs_entry.signal_icon_press().connect(
    sigc::mem_fun(*this, &spreadsheet::on_icon_pressed_exprs) );
  exprs_entry.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_get_exprs) );
  hbox_exprs.pack_start(exprs_entry);

  label = Gtk::manage(new Gtk::Label("    Or enter a file to load"));
  hbox_exprs.pack_start(*label);
  hbox_exprs.pack_start(file_name);
  file_name.signal_activate().connect(
    sigc::mem_fun(*this, &spreadsheet::on_file_name) );

  button = Gtk::manage(new Gtk::Button("or browse"));
  hbox_exprs.pack_start(*button);
  
// End hbox_exprs 


// Begin hpaned_content 
  // Begin first table
  display_info();
  info_frame.set_shadow_type(Gtk::SHADOW_IN);
  info_frame.add(*display_info_SW);
  // void Gtk::Paned::pack1 (Widget& child, bool resize, bool shrink )
  hpaned_content.pack1(info_frame,true,false);
  // End first table

  // Begin second table
  display_dims();
  drawn_h_dim = pivot.h_dim;
  drawn_v_dim = pivot.v_dim;
  content_frame.set_shadow_type(Gtk::SHADOW_IN);
  content_frame.add(*display_dims_SW);
  // void Gtk::Paned::pack2 (Widget& child, bool resize, bool shrink )
  hpaned_content.pack2(content_frame,true,false);
  // End second table
// End hpaned_content 


// Begin frame_edit_dim
  /* In this frame there can be tree different frames:
  /  hbox_edit_dim, hbox_add_dim and hbox_del_dim.
  /  The first one contains two buttons: to add or del dimensions.
  /  When the add button is pressed, hbox_edit_dim is replaced by 
  /  hbox_add_dim which is constant and can be created just one, following.
  /  When the del buttin is pressed, hbox_edit_dim is replaced by 
  /  hbox_del_dim, which needs to be generating at each time depending on
  /  how many dimensions exists. This box is crated at each go in the
  /  call back  function, on_del_dimension()
 */
  frame_edit_dim.add(hbox_edit_dim);
  hbox_edit_dim.set_border_width(5);

  hbox_edit_dim.pack_start(add_dim_button, true, false, 0 );
  add_dim_button.signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_add_dimension),
                     "Adding dimension. Enter the appropriate values.") );

  hbox_edit_dim.pack_start(del_dim_button, true, false, 0 );
  del_dim_button.signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun( *this, &spreadsheet::on_del_dimension),
                     "Deleting dimension. Click on the chosen dimension.") );
  // hbox_add_dim
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
      sigc::mem_fun(*this, &spreadsheet::on_add_OK), "New values entered...") );

  hbox_add_dim.pack_start(cancel_button);
  (cancel_button).signal_clicked().connect(
    sigc::bind (
      sigc::mem_fun(*this, &spreadsheet::on_cancel_edit),
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
  status_button.signal_clicked().connect(
    sigc::bind<Glib::ustring> (
      sigc::mem_fun(
        *this, &spreadsheet::on_status_clicked), "Showing current status") );

  last_box.add(redraw_button);
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

  int piv =  atoi(dimpivot.c_str());
  (pivot.ords)[dimname] = piv; 

  info_frame.remove();
  display_info();
  info_frame.add(*display_info_SW);
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

  std::map<Glib::ustring,int>::iterator it;
  label = Gtk::manage(new Gtk::Label("Delete which dimension?"));
  hbox_del_dim = Gtk::manage(new Gtk::HBox());
  (*hbox_del_dim).pack_start(*label);
  for ( it=pivot.ords.begin() ; it != pivot.ords.end(); it++ ) {
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
                     "Cancelling delete dimensions..." ) );

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
  frame_edit_dim.show_all_children();

  (pivot.ords).erase (dim);
  if (dim == (pivot.h_dim)) {
    (pivot.h_dim.clear() ) ;
    (*hnodisplay).toggled();
  } else if (dim == (pivot.v_dim)) {
    (pivot.v_dim.clear() )  ;
    (*vnodisplay).toggled();
  } 

  info_frame.remove();
  display_info();
  info_frame.add(*display_info_SW);
  (*display_info_SW).show_all_children();
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
  display_dims();
  drawn_h_dim = (pivot.h_dim);
  drawn_v_dim = (pivot.v_dim);
  content_frame.remove();
  content_frame.add(*display_dims_SW);
  (*display_dims_SW).show_all_children();
  status_bar.push(msg);
}

void
spreadsheet::on_file_name()
{
  std::ifstream expr_file (file_name.get_text());
  Glib::ustring msg;
  if (expr_file.is_open() ) 
  {
    msg = ("File " + file_name.get_text() + " is open.");
  }
  else msg = ("Unable to open file " + file_name.get_text() );
  std::cout << msg << std::endl;
  status_bar.push(msg);
  
  expr_file.close();

}

void
spreadsheet::on_get_exprs()
{
  TLstuff.expression = exprs_entry.get_text();
  status_bar.push("Expression \"" + TLstuff.expression + "\" entered.");
  std::cout << "Expression \"" + TLstuff.expression + "\" entered." << std::endl;
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
  if (TLstuff.expression == "")
    exp = "\nThere is no expression.";
  else
    exp = "\nExpression is " + TLstuff.expression + "." ;
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
     blowing up with display_dims class as a pointer.
  */
//  Glib::Error::register_cleanup();
//  Glib::wrap_register_cleanup();
}

// DISPLAY_INFO
void
spreadsheet::display_info()
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

// Begin table
  display_info_SW = Gtk::manage(new Gtk::Frame);
//  display_info_SW = Gtk::manage(new Gtk::ScrolledWindow);
  table = Gtk::manage(new Gtk::Table(((pivot.ords).size()+2), 4, false));
  (*table).set_col_spacings(10);

  // Column titles
  label = manage(new Gtk::Label);
  (*label).set_label("Dimension");
  (*table).attach((*label),0,1,0,1,Gtk::FILL,Gtk::FILL);

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(*h_spread_spin);
  (*label).set_label("H dim\nradius");
  (*table).attach((*box),1,2,0,1,Gtk::FILL,Gtk::FILL);
  h_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_spread_spin ) );

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(*v_spread_spin);
  (*label).set_label("V dim\nradius");
  (*table).attach((*box),2,3,0,1,Gtk::FILL,Gtk::FILL);
  v_spread_spin->signal_value_changed().connect(
    sigc::mem_fun( *this, &spreadsheet::on_v_spread_spin ) );

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Pivot");
  (*table).attach((*label),3,4,0,1,Gtk::FILL,Gtk::FILL);

 // First row: no dimension chosen
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("None");
  (*table).attach((*label),0,1,1,2,Gtk::FILL,Gtk::FILL);

  Gtk::RadioButton::Group hgroup = (*hnodisplay).get_group();
  (*table).attach((*hnodisplay),1,2,1,2,Gtk::FILL,Gtk::FILL);
  (*hnodisplay).signal_toggled().connect(
    sigc::mem_fun( *this, &spreadsheet::on_h_nodim_toggled ) );

  Gtk::RadioButton::Group vgroup = (*vnodisplay).get_group();
  (*table).attach((*vnodisplay),2,3,1,2,Gtk::FILL,Gtk::FILL);
  (*vnodisplay).signal_toggled().connect(
     sigc::mem_fun( *this, &spreadsheet::on_v_nodim_toggled ) );

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
    (*table).attach( (*label), 0, 1, i+2, ii,Gtk::FILL,Gtk::FILL);

    hdisplay = Gtk::manage(new Gtk::RadioButton);
    (*hdisplay).set_group(hgroup);
    (*table).attach( (*hdisplay), 1, 2, i+2, ii,Gtk::FILL,Gtk::FILL);
    (*hdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_h_toggled), hdisplay, dim ) );

    vdisplay = Gtk::manage(new Gtk::RadioButton);
    (*vdisplay).set_group(vgroup);
    (*table).attach((*vdisplay),2,3,i+2,ii,Gtk::FILL,Gtk::FILL);
    (*vdisplay).signal_toggled().connect(
       sigc::bind(
         sigc::mem_fun(*this, &spreadsheet::on_v_toggled), vdisplay, dim ) );

    if ((pivot.h_dim) == dim) {
      (*hdisplay).set_active();
    } else if ((pivot.v_dim) == dim) {
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
    (*table).attach((*values),3,4,i+2,ii,Gtk::FILL,Gtk::FILL);
//    values->signal_activate().connect( sigc::bind (
    values->signal_changed().connect(
      sigc::bind (
        sigc::mem_fun (*this, &spreadsheet::on_dimension_pivot_changed),
                       dim, values ) );
  }
// End table
  (*display_info_SW).add(*table);
  (*display_info_SW).show();
}

void
spreadsheet::on_h_spread_spin()
{
  (pivot.h_radius) = h_spread_spin->get_value();
  std::cout << (pivot.h_radius) << std::endl;
//  status_bar.push(*h_radius);
  h_spread_limits.set_value(pivot.h_radius);
}

void
spreadsheet::on_v_spread_spin()
{
  (pivot.v_radius) = v_spread_spin->get_value();
  std::cout << (pivot.v_radius) << std::endl;
//  status_bar.push(*v_radius);
  v_spread_limits.set_value(pivot.v_radius);
}

void
spreadsheet::on_dimension_pivot_changed(Glib::ustring dim, Gtk::Entry * values)
{
  Glib::ustring s_pivot = values->get_text ();
  std::cout << s_pivot << std::endl;
  status_bar.push("Pivot is " + s_pivot);
  int piv =  atoi(s_pivot.c_str());
  pivot.ords[dim] = piv;
}

void
spreadsheet::on_h_nodim_toggled()
{
  if ((*hnodisplay).get_active()) {
    std::cout << "No dimension chosen for horizontal display."<< std::endl;
    pivot.h_dim.clear() ;
    std::cout << "h_dim = " << (pivot.h_dim) << std::endl;
  }
}

void
spreadsheet::on_v_nodim_toggled()
{
  if ((*vnodisplay).get_active()) {
    std::cout << "No dimension chosen for vertical display." << std::endl;
    pivot.v_dim.clear() ;
    std::cout << "v_dim = " << (pivot.v_dim) << std::endl;
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
}

// DISPLAY_DIMS
void
spreadsheet::display_dims()
{
  int row_range = pivot.h_radius * 2 + 1 ;
  int col_range = pivot.v_radius * 2 + 1 ;

  if ( pivot.isOneCell() )
  {
    display_dims_cell();
  }
  else if ( pivot.isOneRow() )
  {
    int h_min     = pivot.ords[(pivot.h_dim)] - pivot.h_radius ;
    display_dims_row(row_range, h_min);
  }
  else if ( pivot.isOneCol() )
  {
    int v_min     = pivot.ords[(pivot.v_dim)] - pivot.v_radius ;
    display_dims_col(col_range, v_min);
  }
  else
  {
    int h_min     = pivot.ords[(pivot.h_dim)] - pivot.h_radius ;
    int v_min     = pivot.ords[(pivot.v_dim)] - pivot.v_radius ;
    display_dims_all(row_range, col_range, h_min, v_min);
  }
}


void 
spreadsheet::display_dims_all(int row_range, int col_range,
                              int h_min, int v_min)
{
//  display_dims_SW = Gtk::manage(new Gtk::ScrolledWindow);
//  (*display_dims_SW).set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  display_dims_SW = Gtk::manage(new Gtk::Frame);
  table = Gtk::manage(new Gtk::Table(row_range + 1, col_range + 1, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);
  (*display_dims_SW).add(*table);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices");
  (*table).attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);

  //  Drawing horizontal index 
  for (int i = 0 ; i != row_range ; ++i) {
    std::string s;
    std::stringstream out;
    out << h_min + i ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table).attach(*label, i+1, i+2, 0, 1, Gtk::FILL, Gtk::FILL);
  }

  //  Drawing vertical index 
  for (int i = 0 ; i < col_range ; ++i) {
    std::string s;
    std::stringstream out;
    out << v_min + i ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table).attach(*label, 0, 1, i+1, i+2, Gtk::FILL, Gtk::FILL);
  }

  // Drawing content
  for (int i = 0 ; i != row_range ; ++i) {
    for (int j = 0 ; j != col_range ; ++j) {

      std::cout << "expression = " << TLstuff.expression << std::endl;
      std::stringstream newout;

      newout << "(";
      newout << TLstuff.expression;
      newout << ")";
      newout << " @ [";
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

      Glib::ustring cell = TLstuff.calculate_expr(tuple32);
      label = Gtk::manage(new Gtk::Label);
      frame = Gtk::manage(new Gtk::Frame);
      (*label).set_label(cell);
      (*frame).add(*label);
      (*table).attach(*frame, i+1, i+2, j+1, j+2, Gtk::FILL, Gtk::FILL);
    }
  }
  (*display_dims_SW).show();
}

void 
spreadsheet::display_dims_row(int row_range, int h_min)
{
//  display_dims_SW = Gtk::manage(new Gtk::ScrolledWindow);
//  (*display_dims_SW).set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  display_dims_SW = Gtk::manage(new Gtk::Frame);
  table = Gtk::manage(new Gtk::Table(row_range + 1, 2, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);
  (*display_dims_SW).add(*table);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices");
  (*table).attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);

  for (int i = 0 ; i != row_range ; ++i) {
    //  Drawing horizontal index 
    std::string s;
    std::stringstream out;
    out << h_min + i ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table).attach(*label, i+1, i+2, 0, 1, Gtk::FILL, Gtk::FILL);

    // Drawing content
    std::cout << "expression = " << TLstuff.expression << std::endl;
    std::stringstream newout;
    newout << "(";
    newout << TLstuff.expression;
    newout << ")";
    newout << " @ [";
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

    cell = TLstuff.calculate_expr(tuple32);
    label = Gtk::manage(new Gtk::Label);
    frame = Gtk::manage(new Gtk::Frame);
    (*label).set_label(cell);
    (*frame).add(*label);
    (*table).attach(*frame, i+1, i+2, 1, 2, Gtk::FILL, Gtk::FILL);
  }
  (*display_dims_SW).show();
}

void 
spreadsheet::display_dims_col(int col_range, int v_min)
{
//  display_dims_SW = Gtk::manage(new Gtk::ScrolledWindow);
//  (*display_dims_SW).set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  display_dims_SW = Gtk::manage(new Gtk::Frame);
  table = Gtk::manage(new Gtk::Table(2, col_range + 1, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);
  (*display_dims_SW).add(*table);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices");
  (*table).attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);

  for (int j = 0 ; j != col_range ; ++j)
  {
    //  Drawing horizontal index 
    std::string s;
    std::stringstream out;
    out << v_min + j ;
    s = out.str();
    label = Gtk::manage(new Gtk::Label);
    Glib::ustring cell = s;
    (*label).set_label(cell);
    (*table).attach(*label, 0, 1, j+1, j+2, Gtk::FILL, Gtk::FILL);

    // Drawing content
    std::cout << "expression = " << TLstuff.expression << std::endl;
    std::stringstream newout;
    newout << "(";
    newout << TLstuff.expression;
    newout << ")";
    newout << " @ [";
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
    cell = TLstuff.calculate_expr(tuple32);

    label = Gtk::manage(new Gtk::Label);
    frame = Gtk::manage(new Gtk::Frame);
    (*label).set_label(cell);
    (*frame).add(*label);
    (*table).attach(*frame, 1, 2, j+1, j+2, Gtk::FILL, Gtk::FILL);
  }
  (*display_dims_SW).show();
}

void
spreadsheet::display_dims_cell()  // single cell
{
//  display_dims_SW = Gtk::manage(new Gtk::ScrolledWindow);
  display_dims_SW = Gtk::manage(new Gtk::Frame);
  table = Gtk::manage(new Gtk::Table( 2, 2, false));
  (*display_dims_SW).add(*table);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("  Dim\nIndices");
  (*table).attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL, 0,0);

  std::stringstream newout;
  newout << "(";
  newout << TLstuff.expression;
  newout << ")";
  newout << " @ [";
  std::map<Glib::ustring,int>::iterator mit = pivot.ords.begin();
  newout << mit->first << ":" << mit->second;
  std::cout << "(" << mit->first << ")  (" << mit->second << ")" << " is the first element " << std::endl;
//  ++mit;
  mit++;
  for (mit; mit != pivot.ords.end(); ++mit)
  {
    newout << "," << mit->first << ":" << mit->second;
  }
  newout << "]";
  std::string newout_str = newout.str();
  std::cout << newout_str << std::endl;
  std::u32string tuple32 (newout_str.begin(), newout_str.end());
  Glib::ustring cell = TLstuff.calculate_expr(tuple32);

  label = Gtk::manage(new Gtk::Label);
  frame = Gtk::manage(new Gtk::Frame);
  (*label).set_label(cell);
  (*frame).add(*label);
  (*table).attach(*frame, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL);
  (*display_dims_SW).show();
}


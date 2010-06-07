#include "spreadsheet.h"
#include <cstdio>
#include <map>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>
// #include <display_dims.h>


Glib::ustring v_dim, h_dim; // To globally tag the 2 chosen dims
int           num_dims;     // Number of dims available for display 
int           h_radius;     // horizontal radius of display
int           v_radius;     // vertical radius of display
int           row_range;
int           col_range;

spreadsheet::spreadsheet() :
  main_box(false, 10), // false: child widgets don't have the same width
  hbox1(false, 10),    //    10: pixels between widgets
  hbox3(false, 10),
  hbox4(false, 10),
  extra_box(false, 10),

  // Gtk::Adjustment(float initial_value, float lower, float upper,
  //      float step_increment, float page_increment, float page_size);
  // The initial_value is to be the radius, for now 0 with upper limit of 10. 
  h_spread_limits(0.0, 0.0, 10.0, 1.0, 3.0, 0.0),
  v_spread_limits(0.0, 0.0, 10.0, 1.0, 3.0, 0.0),

//  dimensions_sheet(5,5,5,5),

  window_title("The S³ is displaying these dimensions ..."),
  // last_button is Gtk::HButtonBox for equal spacing (30) of buttons.
  last_box(Gtk::BUTTONBOX_END, 30),

  commit_button("  Redraw SpreadSheet  "), 
  quit_button(Gtk::Stock::QUIT),
  close_button("_Close", true) // to use alt-C to close app
{
  num_dims = 4; 
  h_radius = 11;
  v_radius = 4;
  row_range = h_radius * 2 + 1;
  col_range = v_radius * 2 + 1;

  std::map<Glib::ustring,int> tuples;
  std::map<Glib::ustring,int>::iterator it;
  
  tuples["x"] = 4;
  tuples["y"] = 7;
  tuples["w"] = 11;
  tuples["z"] = 42;
/*
for ( it=tuples.begin() ; it != tuples.end(); it++ )
    std::cout << (*it).first << " => " << (*it).second << std::endl;
*/

  it=tuples.begin();
  h_dim = (*(it++)).first;
  v_dim = (*it).first;

  // Set title and border of the window
  set_title("The Super SpreadSheet, The S³");
//  set_border_width(10);
//  set_default_size(800,400);

  // Add outer box to the window since the window may contain a single widget 
  add(main_box);

  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox1);
  main_box.pack_start(hbox3);
  main_box.pack_start(hbox2);
  main_box.pack_start(extra_box);

  // Set the boxes' spacing around the outside of the container: 10 pixels
  hbox1.set_border_width(5); 
  hbox3.set_border_width(5);
  hbox4.set_border_width(5);

// Begin hbox1 
  hbox1.pack_start(window_title);
// End   hbox1 


// Begin hbox2 

  h_spread_limits.set_value(h_radius);
  v_spread_limits.set_value(v_radius);
  h_spread_spin.set_adjustment(h_spread_limits);
  v_spread_spin.set_adjustment(v_spread_limits);
  h_spread_spin.set_size_request(50, -1);
  h_spread_spin.set_numeric(true);
  h_spread_spin.set_wrap(true);
  h_spread_spin.set_alignment(1);
  v_spread_spin.set_size_request(50, -1);
  v_spread_spin.set_numeric(true);
  v_spread_spin.set_wrap(true);
  v_spread_spin.set_alignment(1);

  // Begin table
  table = Gtk::manage(new Gtk::Table(num_dims, 4, false));
  (*table).set_col_spacings(10);

  big_frame = Gtk::manage(new Gtk::Frame);
  big_frame->set_shadow_type(Gtk::SHADOW_IN);
  big_frame->set_size_request(300,-1);
  big_frame->add(*table);
  hbox2.pack1(*big_frame);

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

  box = Gtk::manage(new Gtk::VBox);
  label = Gtk::manage(new Gtk::Label);
  (*box).add(*label);
  (*box).add(v_spread_spin);
  (*label).set_label("V dim\nradius");
  (*table).attach((*box),2,3,0,1);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Pivot");
  (*table).attach((*label),3,4,0,1);

  // First row: no dimension chosen
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("None");
  (*table).attach((*label),0,1,1,2);

  Gtk::RadioButton::Group hgroup = (hnodisplay).get_group();
  (*table).attach((hnodisplay),1,2,1,2);
  (hnodisplay).signal_toggled().connect(sigc::mem_fun(*this,
              &spreadsheet::on_h_nodim_toggled) );

  Gtk::RadioButton::Group vgroup = (vnodisplay).get_group();
  (*table).attach((vnodisplay),2,3,1,2);
  (vnodisplay).signal_toggled().connect(sigc::mem_fun(*this,
              &spreadsheet::on_v_nodim_toggled) );


  /* Following rows: radio buttons in two groups: H and V
     and the pivot value
  */
  int i = 0; // Can't put this in the for loop, it explodes. 
  for ( it=tuples.begin(); it != tuples.end(); ++it, ++i )
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

    // Initial dimensions chosen for display by the programmer
    if (i == 0) (*hdisplay).set_active();
    else if (i == 1) (*vdisplay).set_active();

    values = Gtk::manage(new Gtk::Entry);
    values->set_alignment(1);
    std::string s;
    std::stringstream out;
    out << (*it).second ;
    s = out.str();
    (*values).set_size_request(50, -1);
    (*values).set_text(s);
    (*table).attach((*values),3,4,i+2,ii);

/* TODO: attach signal when the text is changed and update 
         dim_tuples[i].pivot.
   signal is changed ...
   values->signal_action_changed().connect( sigc::mem_fun(*this,
              &spreadsheet::on_dimension_pivot_changed) );
*/
  }
  // End table
  std::cout << "h dim = " << h_dim << " and v dim = " << v_dim << std::endl;

  // Begin display of dimension values
  int h_min = tuples[h_dim] - h_radius;
  int v_min = tuples[v_dim] - v_radius;

  std::cout << "Checking on values in spreadsheet:\n h_min = " 
            << h_min << "\n v_min = " <<
            v_min << "\n row_range = " << row_range << "\n col_range "
            << col_range << std::endl;
  dimensions_sheet = Gtk::manage(new display_dims(row_range, col_range, h_min, v_min));

  big_frame = Gtk::manage(new Gtk::Frame);
  big_frame->set_shadow_type(Gtk::SHADOW_IN);
  big_frame->add(*dimensions_sheet);
  hbox2.pack2(*big_frame);




  // End display of dimension values


// End hbox2 

// Begin hbox3 

  //  eqns_entry.set_max_length(50);
  eqns_entry.set_text("hello");
  eqns_entry.set_text(eqns_entry.get_text() + " world");
  eqns_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  eqns_entry.signal_icon_press().connect( sigc::mem_fun(*this,
             &spreadsheet::on_icon_pressed_eqns) );
  hbox3.pack_start(eqns_entry);

// End hbox3 


// Begin hbox4 
  // Filling up the last horizontal box with new eqn field, commit and quit 


/* Trying to add a message when "commiting changes" is taking place.
 * Taken from example, not fully understood.
 */
  Gtk::Container* infoBarContainer =
    dynamic_cast<Gtk::Container*>(InfoBar_commit.get_content_area());
  if (infoBarContainer)
    infoBarContainer->add(Label_commit);
  // Add an ok button to the InfoBar:
  InfoBar_commit.add_button(Gtk::Stock::OK, 0);
  Label_commit.set_text("Commiting changes...");
  InfoBar_commit.set_message_type(Gtk::MESSAGE_INFO);
  InfoBar_commit.signal_response().connect(sigc::mem_fun(*this,
                 &spreadsheet::on_infobar_commit ) );
  hbox4.pack_start(InfoBar_commit, Gtk::PACK_SHRINK);


  last_box.add(commit_button);
  last_box.add(close_button);
  last_box.add(quit_button);
  hbox4.pack_start(last_box);
  main_box.pack_start(hbox4);


  // Make the button the default widget. from Widget. No idea what this 
  // does as the explanation does not match the functionality.
  close_button.set_can_default();
  close_button.grab_default();

  // Connecting the signal to the corresponding handlers.
  commit_button.signal_clicked().connect(
         sigc::bind<Glib::ustring> (
             sigc::mem_fun(
                *this,
                &spreadsheet::on_commit_clicked), 
                "Hi :-)") );
  close_button.signal_clicked().connect (
             sigc::mem_fun(
                *this,
                &spreadsheet::on_closebutton_clicked) );
  quit_button.signal_clicked().connect(
             sigc::ptr_fun(&Gtk::Main::quit));

// End hbox4 


  // Show all children of the window
  show_all_children();
  InfoBar_commit.hide(); // InfoBar shown only when commit button is pressed

  create_equations();

}


spreadsheet::~spreadsheet()
{
  // Trying to get rid of the problem of not deallocating memory.
//  Glib::Error::register_cleanup();
//  Glib::wrap_register_cleanup();
}


void
spreadsheet::on_closebutton_clicked()
{
  std::cout << "Qutting the S³..." << std::endl;
  hide(); //to close the application.
}

void
spreadsheet::on_commit_clicked(Glib::ustring msg)
{
//  (*dimensions_sheet).hide();
  big_frame->remove();
  

  InfoBar_commit.show(); // To show the message when commit is clicked.
  // Extra handling when known what to do.
  /* http://library.gnome.org/devel/gtkmm-tutorial/unstable/sec-progressbar.html.en
   * to introduce progress bar instead of the Ok button.
   */
  std::cout << msg << std::endl;
//  (*dimensions_sheet).hide();
//  delete dimensions_sheet;
//  hbox2.remove(*big_frame);


  dimensions_sheet = Gtk::manage(new display_dims(6, 6, 6, 6));
//  big_frame = Gtk::manage(new Gtk::Frame);
//  big_frame->set_shadow_type(Gtk::SHADOW_IN);
  big_frame->add(*dimensions_sheet);
  std::cout << "End of on_commit_clicked."  << std::endl;
Gtk::Button* pButton = new Gtk::Button("_Something", true);
//  extra_box.pack_start(*pButton);
//  extra_box.pack_start(*dimensions_sheet);

  hbox2.pack2(*big_frame);
  (*this).show_all_children();

//  show_all_children();
//  (*dimensions_sheet).show();
}

void
spreadsheet::on_infobar_commit(int)
{
  // Hide the commit InfoBar:
//  (*this).show_all_children();
//  (*dimensions_sheet).show();
  InfoBar_commit.hide();
  std::cout << "End of on_infobar_commit."  << std::endl;
}

void
spreadsheet::on_h_nodim_toggled()
{
  if ((hnodisplay).get_active()) {
    std::cout << "No dimension chosen for horizontal display."<< std::endl;
    h_dim = "";
  }
}

void
spreadsheet::on_v_nodim_toggled()
{
  if ((vnodisplay).get_active()) {
    std::cout << "No dimension chosen for vertical display." << std::endl;
    v_dim = "";
  }
//  std::cout << "The Button was clicked: state="
//      << (vnodisplay.get_active() ? "true" : "false")
//      << std::endl;
}

void
spreadsheet::on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim)
{
  if ((*v_radio).get_active()) { // When a dim is chosen
    if (h_dim == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      vnodisplay.set_active();
    } else {
      v_dim = dim; 
      std::cout << dim << " chosen for vertical display."<< std::endl;
    }
  } // When a dim is released
}

void
spreadsheet::on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim)
{
  if ((*h_radio).get_active()) { // When a dim is chosen
    if (v_dim == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
      hnodisplay.set_active();
    } else {
      h_dim = dim; 
      std::cout << dim << " chosen for horizontal display."<< std::endl;
    }
  } // When a dim is released
}

void
spreadsheet::on_icon_pressed_eqns(Gtk::EntryIconPosition /* icon_pos */,
                                  const GdkEventButton*  /* event */)
{
/* TODO: figure out how to produce a drop down list of existing equations
 * for now just selecting/highlighting text in it.
 * http://library.gnome.org/devel/gtkmm-tutorial/unstable/sec-text-entry.html.en#sec-text-entry-completion
 */
  eqns_entry.select_region(0, eqns_entry.get_text_length());
  std::cout << "Equation field clicked." << std::endl;
}


void
spreadsheet::create_equations ()
{
  using namespace TransLucid;
  Translator traductor;
 // traductor.loadLibrary(U"int");
  traductor.parse_header (
    U"dimension ustring<n>;;"
    U"infixl ustring<-> ustring<operator-> 5;;"
    U"infixl ustring<*> ustring<operator*> 10;;"
    U"library ustring<int>;;"
  );
  traductor.translate_and_add_equation_set (
    U"fact | [n:0] = 1;;"
    U"fact = #n * (fact @ [n:#n-1]);;"
  );
  HD* e = traductor.translate_expr(U"fact @ [n:20]");
  TaggedConstant result = (*e)(Tuple());
  std::cout << result.first.value<Intmp>().value() << std::endl;
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

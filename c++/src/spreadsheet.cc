#include "spreadsheet.h"
#include <cstdio>
//#include <tl/translator.hpp>
//#include <tl/utility.hpp>
//#include <tl/parser_util.hpp>


Glib::ustring dim_col, dim_row; // To globally tag the 2 chosen dims
int           num_dims;         // Number of dims available for display 
int           h_spread,    // horizontal spread in radio terms from pivot
              v_spread;    // vertical spread in radio terms from pivot
struct tuple {
  Glib::ustring dim;
  int           pivot;
}; 

spreadsheet::spreadsheet() :
  main_box(false, 10), // false: child widget don't have the same width
  hbox1(false, 10),    // 10: pixels between widgets
  hbox2(false, 10),
  hbox3(false, 10),
  hbox4(false, 10),

  window_title("The S³ is displaying these dimensions ..."),
  aspect_frame("", Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 16, false),
  center_frame(0.5, 0.5, 0, 0),
  last_box(Gtk::BUTTONBOX_END, 20),

  commit_button("Commit your changes"), 
  quit_button(Gtk::Stock::QUIT),
  close_button("_Close", true) // to use alt-C to close app
{
 /* These three variables are comming from TL HD's of the choosen
  * dimensions to display and their ranges. Here hard-coded.
  */
  num_dims = 4; 
  h_spread = 4;
  v_spread = 2;
  int row_range = h_spread * 2 +1;
  int col_range = v_spread * 2 +1;
  tuple dim_tuples [num_dims];
  dim_tuples[0].dim = "x";
  dim_tuples[0].pivot = 4;
  dim_tuples[1].dim = "y";
  dim_tuples[1].pivot = 7;
  dim_tuples[2].dim = "w";
  dim_tuples[2].pivot = 11;
  dim_tuples[3].dim = "z";
  dim_tuples[3].pivot = 42;


  // Set title and border of the window
  set_title("The Super SpreadSheet, The S³");
//  set_border_width(10);
//  set_default_size(800,400);

  // Add outer box to the window since the window may contain a single widget 
  add(main_box);

  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox1);
  main_box.pack_start(hbox2);
  main_box.pack_start(hbox3);
//  main_box.pack_start(hbox4);

 /* Temporary solution to the fact that hbox4 grows ridiculously tall
  * and haven't found a way to limit the height.
  */ 
//  aspect_frame.add(hbox4);
//  center_frame.add(aspect_frame);


/* last version say 1.00
  center_frame.add(hbox4);
  main_box.pack_start(center_frame);
*/ 

/*
  it might be better to use the arguments in pack_start():
void pack_start(Gtk::Widget& child, PackOptions options = PACK_EXPAND_WIDGET, guint padding = 0);
    * PACK_SHRINK: Space is contracted to the child widget size. The widget will take up just-enough space and never expand.
    * PACK_EXPAND_PADDING: Extra space is filled with padding. The widgets will be spaced out evenly, but their sizes won't change - there will be empty space between the widgets instead.
    * PACK_EXPAND_WIDGET: Extra space is taken up by increasing the child widget size, without changing the amount of space between widgets.



*/

  // Set the boxes' spacing around the outside of the container: 10 pixels
  hbox1.set_border_width(10); 
  hbox2.set_border_width(10);
  hbox3.set_border_width(10);
  hbox4.set_border_width(10);

// Begin hbox1 
  hbox1.pack_start(window_title);
// End   hbox1 

// Begin hbox2 
  // Begin first table
  table = Gtk::manage(new Gtk::Table(num_dims, 4, false));
  (*table).set_col_spacings(10);
  hbox2.pack_start(*table);

  // Column titles
  label = manage(new Gtk::Label);
  (*label).set_label("Dimension\nName");
  (*table).attach((*label),0,1,0,1);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Horizontal\ndimension");
  (*table).attach((*label),1,2,0,1);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Vertical\ndimension");
  (*table).attach((*label),2,3,0,1);

  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Value");
  (*table).attach((*label),3,4,0,1);

  // First row: no dimension chosen
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("No dimension");
  (*table).attach((*label),0,1,1,2);

  Gtk::RadioButton::Group hgroup = (hnodisplay).get_group();
  (*table).attach((hnodisplay),1,2,1,2);
  (hnodisplay).signal_toggled().connect(sigc::mem_fun(*this,
              &spreadsheet::on_h_nodim_toggled) );

  Gtk::RadioButton::Group vgroup = (vnodisplay).get_group();
  (*table).attach((vnodisplay),2,3,1,2);
  (vnodisplay).signal_toggled().connect(sigc::mem_fun(*this,
              &spreadsheet::on_v_nodim_toggled) );

  for (int i = 0; i != num_dims; ++i) {
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    int ii = i+1;

    label = Gtk::manage(new Gtk::Label);
    Glib::ustring dim = "Dimension " + s; // Name to come from TL HD
    (*label).set_label(dim_tuples[i].dim); 
    (*table).attach((*label),0,1,i+2,ii+2);

    hdisplay = Gtk::manage(new Gtk::RadioButton);
    (*hdisplay).set_group(hgroup);
    (*table).attach((*hdisplay),1,2,i+2,ii+2);
    (*hdisplay).signal_toggled().connect(
       sigc::bind( 
         sigc::mem_fun(*this, &spreadsheet::on_h_toggled), hdisplay, dim ) );

    vdisplay = Gtk::manage(new Gtk::RadioButton);
    (*vdisplay).set_group(vgroup);
    (*table).attach((*vdisplay),2,3,i+2,ii+2);
    (*vdisplay).signal_toggled().connect(
       sigc::bind( 
         sigc::mem_fun(*this, &spreadsheet::on_v_toggled), vdisplay, dim ) );

/* TODO: need to connect this text entry to a handler, 
 * but for now don't know what to do with it.
 */
    values = Gtk::manage(new Gtk::Entry);
    (*table).attach((*values),3,4,i+2,ii+2);

  }
  // End first table

  // Begin second table, which is within a Gtk::ScrolledWindow

  scrollDimsWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  scrollDimsWindow.set_size_request(400,400); // Set minimum size of widget

//  get_vbox()->pack_start(scrollDimsWindow); //not sure what this does.

  table = Gtk::manage(new Gtk::Table(row_range, col_range, false));
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);

  scrollDimsWindow.add(*table);

  /* For now just labels representing the values of each cell.
   * Values to be taken from TL HD.
   */
  for(int i = 0; i != row_range ; ++i)
  {
     for(int j = 0; j != col_range ; ++j)
     {
       std::string s;
       std::stringstream out;
       out << i << " " << j;
       s = out.str();

       label = Gtk::manage(new Gtk::Label);
       frame = Gtk::manage(new Gtk::Frame);
       Glib::ustring cell = "Cell " + s; // Name to come from TL HD
       (*label).set_label(cell); 
       (*frame).add(*label);
       (*table).attach(*frame, i, i + 1, j, j + 1);
     }
  }

  hbox2.pack_start(scrollDimsWindow);

// End hbox2 

// Begin hbox3 
//     Trying something: putting this all in hbox4

/* Trying to add a meesage when "commiting changes" is taking place.
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

// End hbox3 


// Begin hbox4 
  // Filling up the last horizontal box with new eqn field, commit and quit 

//  eqns_entry.set_max_length(50);
  eqns_entry.set_text("hello");
  eqns_entry.set_text(eqns_entry.get_text() + " world");
  eqns_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  eqns_entry.signal_icon_press().connect( sigc::mem_fun(*this,
             &spreadsheet::on_icon_pressed_eqns) );
// TODO: The actual text entry needs to be handled.


  hbox3.pack_start(eqns_entry);
/* last version say 1.00
  hbox4.pack_start(commit_button);
  hbox4.pack_start(close_button);
  hbox4.pack_start(quit_button);
*/
  last_box.add(commit_button);
  last_box.add(close_button);
  last_box.add(quit_button);
  hbox4.pack_start(last_box);
  main_box.pack_start(hbox4);


//  hbox4.set_size_request(400,50); // need the maximum height only. not working

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

//  create_equations(); 

}


spreadsheet::~spreadsheet()
{
  // Trying to get rid of the problem of not deallocating memory.
  Glib::Error::register_cleanup();
  Glib::wrap_register_cleanup();
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
  InfoBar_commit.show(); // To show the message when commit is clicked.
  // Extra handling when known what to do.
  /* http://library.gnome.org/devel/gtkmm-tutorial/unstable/sec-progressbar.html.en
   * to introduce progress bar instead of the Ok button.
   */
  std::cout << msg << std::endl;
}

void
spreadsheet::on_infobar_commit(int)
{
  // Hide the commit InfoBar:
  InfoBar_commit.hide();
}

void
spreadsheet::on_h_nodim_toggled()
{
  if ((hnodisplay).get_active()) 
  std::cout << "No dimension chosen for horizontal display."<< std::endl;
}

void
spreadsheet::on_v_nodim_toggled()
{
  if ((vnodisplay).get_active()) 
  std::cout << "No dimension chosen for vertical display." << std::endl;
//  std::cout << "The Button was clicked: state="
//      << (vnodisplay.get_active() ? "true" : "false")
//      << std::endl;
}

// Need to know how to hook a (* widget) here after it has been reused in 
// the main. 
void
spreadsheet::on_v_toggled(Gtk::RadioButton * v_radio, Glib::ustring dim)
{
  if ((*v_radio).get_active()) { // When a dim is chosen
    if (dim_row == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
//      (*v_radio).toggled();
      vnodisplay.set_active();
    } else {
      dim_col = dim; 
      std::cout << dim << " chosen for vertical display."<< std::endl;
    }
  } // When a dim is released
}

void
spreadsheet::on_h_toggled(Gtk::RadioButton * h_radio, Glib::ustring dim)
{
  if ((*h_radio).get_active()) { // When a dim is chosen
    if (dim_col == dim) {
      std::cout << "Cannot display the same dimension in both directions."
                << std::endl;
//      (*h_radio).toggled();
      hnodisplay.set_active();
    } else {
      dim_row = dim; 
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

/*
void
spreadsheet::create_equations ()
{
  using namespace TransLucid;
  Translator traductor;
  traductor.loadLibrary(U"int");
  traductor.parse_header (
    U"dimension ustring<n>;;"
    U"infixl ustring<-> ustring<operator-> 5;;"
    U"infixl ustring<*> ustring<operator*> 10;;"
    U"library ustring<int>;;"
  );
  traductor.translate_and_add_equation_set (
    U"fac | [n:0] = 1;;"
    U"fact = #n * (fact @ [n:#n-1]);;"
  );
  HD* e = traductor.translate_expr(U"fact @ [n:20]");
  TaggedConstant result = (*e)(Tuple());
  std::cout << result.first.value<Intmp>().value() << std::endl;
  delete e;
}
*/

/*
void
spreadsheet::create_equations ()
{
  namespace TL = TransLucid;
  TL::Translator traductor;
//  traductor.loadLibrary(U"int");
//  std::cout << "Loaded the library" << std::endl;
  TL::Parser::HeaderStruct& header = traductor.header();
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


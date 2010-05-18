#include "spreadsheet.h"
#include <cstdio>

spreadsheet::spreadsheet() :
  main_box(false, 10), // false: child widget don't have the same width
  hbox1(false, 10),    // 10: pixels between widgets
  hbox2(false, 10),
  hbox3(false, 10),
  hbox4(false, 10),

  window_title("The S³ is displaying these dimensions ..."),

  commit_button("Commit your changes"), 
  quit_button(Gtk::Stock::QUIT),
  close_button("_Close", true) // to use alt-C to close app
{
  int dimnumber = 10;

  // Set title and border of the window
  set_title("The Super SpreadSheet, The S³");
  set_border_width(10);
//  set_default_size(800,400);

  // Add outer box to the window since the window may contain a single widget 
  add(main_box);

  //Put the inner boxes in the outer box:
  main_box.pack_start(hbox1);
  main_box.pack_start(hbox2);
  main_box.pack_start(hbox3);
  main_box.pack_start(hbox4);

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
  table = new Gtk::Table(dimnumber, 4, false);
  (*table).set_col_spacings(10);
  hbox2.pack_start(*table);

  // Column titles
  label = Gtk::manage(new Gtk::Label);
  (*label).set_label("Dimension\nName");
  (*table).attach((*label),0,1,0,1);

  label = new Gtk::Label;
  (*label).set_label("Horizontal\ndimension");
  (*table).attach((*label),1,2,0,1);

  label = new Gtk::Label;
  (*label).set_label("Vertical\ndimension");
  (*table).attach((*label),2,3,0,1);

  label = new Gtk::Label;
  (*label).set_label("Value");
  (*table).attach((*label),3,4,0,1);

  // First row: no dimension chosen
  label = new Gtk::Label;
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

  for (int i = 0; i != dimnumber; ++i) {
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    int ii = i+1;

    label = new Gtk::Label;
    (*label).set_label("Dimension " + s); // Name to come from TL HD
    (*table).attach((*label),0,1,i+2,ii+2);

    hdisplay = new Gtk::RadioButton;
    (*hdisplay).set_group(hgroup);
    (*table).attach((*hdisplay),1,2,i+2,ii+2);

    vdisplay = new Gtk::RadioButton;
    (*vdisplay).set_group(vgroup);
    (*table).attach((*vdisplay),2,3,i+2,ii+2);
// TODO: need to find a way to hook to the signal handler and be able
// to check the state of th widget.

//    (*vdisplay).signal_toggled().connect(
//       sigc::bind<Gtk::RadioButton>( 
//         sigc::mem_fun(*this, &spreadsheet::on_v_toggled), (*vdisplay) ) );

// TODO: need to connect this text entry to a handler, don't know which
// or what and how.
    values = new Gtk::Entry;
    (*table).attach((*values),3,4,i+2,ii+2);

  }
  // End first table

  // Begin second table, which is within a Gtk::ScrolledWindow

  scrollDimsWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
  scrollDimsWindow.set_size_request(400,400); // Set minimum size of widget

//  get_vbox()->pack_start(scrollDimsWindow); //not sure what this does.

  table = new Gtk::Table(10, 10, false);
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);

  scrollDimsWindow.add(*table);

  /* This simply creates a grid of toggle buttons on the table
   * to demonstrate the scrolled window. 
   * Taken from examples for tutorial. 
   * TODO: should change to just labels. Not sure what has to be done. */
  for(int i = 0; i < 10; i++)
  {
     for(int j = 0; j < 10; j++)
     {
        char buffer[32];
        sprintf(buffer, "button (%d,%d)\n", i, j);
        Gtk::Button* pButton = Gtk::manage(new Gtk::ToggleButton(buffer));
        (*table).attach(*pButton, i, i + 1, j, j + 1);
     }
  }

  hbox2.pack_start(scrollDimsWindow);

// End hbox2 

// Begin hbox3 

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

  hbox3.pack_end(InfoBar_commit, Gtk::PACK_SHRINK);

// End hbox3 


// Begin hbox4 
  // Filling up the last horizontal box with new eqn field, commit and quit 

//  eqns_entry.set_max_length(50);
  eqns_entry.set_text("hello");
  eqns_entry.set_text(eqns_entry.get_text() + " world");
//  eqns_entry.select_region(0, eqns_entry.get_text_length());
  eqns_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  eqns_entry.signal_icon_press().connect( sigc::mem_fun(*this,
             &spreadsheet::on_icon_pressed_eqns) );
// TODO: The actual text entry needs to be handled.


  hbox4.pack_start(eqns_entry);
  hbox4.pack_start(commit_button);
  hbox4.pack_start(close_button);
  hbox4.pack_start(quit_button);

  hbox4.set_size_request(400,50); // need the maximum height only. not working

  // Make the button the default widget. from Widget. No idea what this 
  // does as the explanation does not match the functionality.
  close_button.set_can_default();
  close_button.grab_default();

  // Connecting the signal to the corresponding handlers.
  commit_button.signal_clicked().connect(sigc::bind (sigc::mem_fun(*this,
                &spreadsheet::on_commit_clicked), "Hi :-)") );
  close_button.signal_clicked().connect(sigc::mem_fun(*this,
               &spreadsheet::on_closebutton_clicked) );
  quit_button.signal_clicked().connect(sigc::ptr_fun(&Gtk::Main::quit));

// End hbox4 


  // Show all children of the window
  show_all_children();
  InfoBar_commit.hide(); // InfoBar shown only when commit button is pressed
}


spreadsheet::~spreadsheet()
{
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
//void
//spreadsheet::on_v_toggled(Gtk::RadioButon * radio)
//{
//  std::cout << "Some dimension chosen for vertical display."<< std::endl;
//}

void
spreadsheet::on_v_toggled()
{
  std::cout << "Some dimension chosen for vertical display."<< std::endl;
}

void
spreadsheet::on_h_toggled()
{
  std::cout << "Some dimension chosen for horizontal display."<< std::endl;
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



#include "spreadsheet.h"
#include <cstdio>


spreadsheet::spreadsheet() :
  main_box(false, 10),
  hbox1(false, 10),
  hbox2(false, 10),
  hbox3(false, 10),
  hbox4(false, 10),
//  the_button("Any name") = new Gtk::Button,
//  the_button("Any name"),
  f_dim_title("Frame for Dimensions Displayed"),
  l_dim_title("Label for Dimensions Displayed"),
  ll_dim_title("Another Label for Dimensions Displayed"),
  window_title("The S³ is displaying these dimensions ..."),

  commit_button("Commit your changes"), 
  quit_button(Gtk::Stock::QUIT),
//  other_button("Other Button"),
  close_button("_Close", true) // to use alt-C to close app
{
  int dimnumber = 10;

  // Set title and border of the window
  set_title("The Super SpreadSheet, The S³");
  set_border_width(10);

  // Add outer box to the window (because the window
  // can only contain a single widget)
  add(main_box);

  //Put the inner boxes and the separator in the outer box:
  main_box.pack_start(hbox1);
  main_box.pack_start(hbox2);
  main_box.pack_start(hbox3);
  main_box.pack_start(hbox4);

  table = new Gtk::Table(dimnumber, 4, false);
  (*table).set_col_spacings(10);

  hbox2.pack_start(*table);
  hbox2.pack_start(vbox2);
  hbox2.pack_start(vbox3);

  // Set the inner boxes' borders
  vbox1.set_border_width(10);
  vbox2.set_border_width(10);
  vbox3.set_border_width(10);

  hbox1.set_border_width(10);
  hbox2.set_border_width(10);
  hbox3.set_border_width(10);
  hbox4.set_border_width(10);

// Begin hbox1 

  hbox1.pack_start(window_title);
//  (other_button).signal_clicked().connect(sigc::bind<Glib::ustring>(
//                 sigc::mem_fun(*this, &spreadsheet::on_other_clicked),
//                 "Hello" ));

  label = new Gtk::Label;
  (*label).set_label("Name");
  (*table).attach((*label),0,1,0,1);

  label = new Gtk::Label;
  (*label).set_label("Horizontal\ndimension");
  (*table).attach((*label),1,2,0,1);
//  vbox2.pack_start(*headers);

  label = new Gtk::Label;
  (*label).set_label("Vertical\ndimension");
  (*table).attach((*label),2,3,0,1);
//  vbox3.pack_start(*headers);

  label = new Gtk::Label;
  (*label).set_label("Value");
  (*table).attach((*label),3,4,0,1);

  label = new Gtk::Label;
  (*label).set_label("No dimension");
  (*table).attach((*label),0,1,1,2);

//  hdisplay = new Gtk::RadioButton;
  Gtk::RadioButton::Group hgroup = (hnodisplay).get_group();
  (*table).attach((hnodisplay),1,2,1,2);
  (hnodisplay).signal_toggled().connect(sigc::mem_fun(*this,
              &spreadsheet::on_h_nodim_toggled) );
//  vbox2.pack_start(*hdisplay);
//  vbox2.add(*hdisplay);

//  vdisplay = new Gtk::RadioButton;
  Gtk::RadioButton::Group vgroup = (vnodisplay).get_group();
  (*table).attach((vnodisplay),2,3,1,2);
//  vbox3.pack_start(*vdisplay);
  (vnodisplay).signal_toggled().connect(sigc::mem_fun(*this,
              &spreadsheet::on_v_nodim_toggled) );



// Need a table otherwise they don't line up.
  for (int i = 0; i != dimnumber; ++i) {
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    int ii = i+1;

    label = new Gtk::Label;
    (*label).set_label("Dimension " + s); // Name to come from TL HD
    (*table).attach((*label),0,1,i+2,ii+2);

//    (*the_button).set_label("Dimension " + s); // Name to come from TL HD
//    vbox1.pack_start(*the_button);
//    (*the_button).signal_clicked().connect(sigc::bind<Glib::ustring>( 
//                  sigc::mem_fun(*this, &spreadsheet::on_other_clicked), 
//                  (*the_button).get_label() ));

    hdisplay = new Gtk::RadioButton;
    (*hdisplay).set_group(hgroup);
    (*table).attach((*hdisplay),1,2,i+2,ii+2);
//    vbox2.pack_start(*hdisplay);

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


// End hbox1 


// Begin hbox2 
//  f_dim_title.add(l_dim_title);
//  hbox2.pack_start(ll_dim_title);
//  hbox2.pack_start(f_dim_title);

//  scrollDimsWindow.set_border_width(100);
  scrollDimsWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);

//  get_vbox()->pack_start(scrollDimsWindow); //not sure what this does.

  table = new Gtk::Table(10, 10, false);
  (*table).set_col_spacings(10);
  (*table).set_row_spacings(10);

  scrollDimsWindow.add(*table);

  /* this simply creates a grid of toggle buttons on the table
   * to demonstrate the scrolled window. */
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
  // Filling up the last horizontal box with new eqn field, commit and quit 

// Trying to add a meesage when "commiting changes" is taking place.
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
//  eqns_entry.set_max_length(50);
  eqns_entry.set_text("hello");
  eqns_entry.set_text(eqns_entry.get_text() + " world");
//  eqns_entry.select_region(0, eqns_entry.get_text_length());
  eqns_entry.set_icon_from_stock(Gtk::Stock::INDEX );
  eqns_entry.signal_icon_press().connect( sigc::mem_fun(*this,
             &spreadsheet::on_icon_pressed_eqns) );
// TODO: The actual text entry need to be handled.


  hbox4.pack_start(eqns_entry);


//This any button is to be replaced with eqns browser
//  the_button = new Gtk::Button;
//  (*the_button).set_label("Any Name");
//  (*the_button).signal_clicked().connect(sigc::mem_fun(*this,
//               &spreadsheet::on_dimname_clicked), "Hello" );

//  hbox3.pack_start(*the_button);
  hbox4.pack_start(commit_button);
  hbox4.pack_start(close_button);
  hbox4.pack_start(quit_button);

// TODO: Need a signal handler for the commit button

  // Make the button the default widget. from Widget. No idea what this 
  // does as the explanation does not match the functionality
  close_button.set_can_default();
  close_button.grab_default();

  // Connect the clicked signal of the close_button to
  // spreadsheet::on_button_clicked()
  commit_button.signal_clicked().connect(sigc::mem_fun(*this,
                &spreadsheet::on_commit_clicked) );
  close_button.signal_clicked().connect(sigc::mem_fun(*this,
               &spreadsheet::on_button_clicked) );
  quit_button.signal_clicked().connect(sigc::ptr_fun(&Gtk::Main::quit));

// End hbox4 



  // Show all children of the window
  show_all_children();
  InfoBar_commit.hide();
}

spreadsheet::~spreadsheet()
{
}

void spreadsheet::on_button_clicked()
{
  std::cout << "Qutting the S³..." << std::endl;
  hide(); //to close the application.
}

void spreadsheet::on_commit_clicked()
{
  InfoBar_commit.show(); //to show the message
}

void spreadsheet::on_dimname_clicked()
{
  std::cout << "Any button has been clicked." << std::endl;
}

void spreadsheet::on_other_clicked(Glib::ustring msg)
{
  std::cout << msg << " been clicked." << std::endl;
}

void spreadsheet::on_v_nodim_toggled()
{
  if ((vnodisplay).get_active()) 
  std::cout << "No dimension chosen for vertical display." << std::endl;

//  std::cout << "The Button was clicked: state="
//      << (vnodisplay.get_active() ? "true" : "false")
//      << std::endl;

}

//void spreadsheet::on_v_toggled(Gtk::RadioButon * radio)
//{
//  std::cout << "Some dimension chosen for vertical display."<< std::endl;
//}

void spreadsheet::on_h_nodim_toggled()
{
  if ((hnodisplay).get_active()) 
  std::cout << "No dimension chosen for horizontal display."<< std::endl;
}

void spreadsheet::on_h_toggled()
{
  std::cout << "Some dimension chosen for horizontal display."<< std::endl;
}

void spreadsheet::on_icon_pressed_eqns(Gtk::EntryIconPosition /* icon_pos */,
                                       const GdkEventButton* /* event */)
{
// TODO: figure out how to produce a drop down list of existing equations
// for now just selecting text in it.
  eqns_entry.select_region(0, eqns_entry.get_text_length());
  std::cout << "Equation field clicked." << std::endl;

}
void spreadsheet::on_infobar_commit(int)
{
  // Clear the message and hide the info bar:
//  Label_commit.set_text("");
  InfoBar_commit.hide();
}



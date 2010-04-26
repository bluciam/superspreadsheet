/* Beginnings of the Hyper Spreadsheet
   Blanca Mancilla and John Plaice
   Thu Apr 22 16:27:28 CEST 2010
#include <string.h>

*/

#include <gtk/gtk.h>
#include <stdlib.h>
typedef struct
{
  /* Assuming dimnames no bigger than 30 characters */
  char dimA_displayed[30];
  char dimB_displayed[30];
  char dimA_chosen[30];
  char dimB_chosen[30];
} userEntries;
userEntries thisEntry;

/* Callback function to display the content of a widget.
 * The data passed to this function is printed to stdout. */
static void 
callback_display (GtkWidget* widget, gpointer  data)
{
  g_print ("You press the widget %s.\n", (gchar *) data);
}

/* Callback function for the state-of-the-system field.
 * For now, the data is passed into stdout until I learn else. */
static void
callback_state (GtkWidget* widget, gpointer data)
{
  g_print ("The state of your system is ... %s.\n", (gchar *) data);
}

static void
choose_dim (GtkWidget* widget, gpointer dim)
{
  static int i = 0;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) 
  {
    printf ("Dimension chosen: %s.\n", (gchar *) dim);
  } else {
    printf ("Dimension released: %s.\n", (gchar *) dim);
  }

}

static void
entering_eqns (GtkWidget* widget, GtkWidget* entry)
{
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Equation entered: %s\n", entry_text);
}


/* Callback funtion for delele event from the window manager.
 * We only want to quit with the Quit button to do proper finalization. */
static gboolean
delete_event(GtkWidget* widget, GdkEvent*  event, gpointer data)
{
  g_print ("Press Quit to quit. \n");
  return TRUE;
}

/* Callback function for the Quit button to quit */
static void
quitting (GtkWidget* widget, gpointer data)
{
  g_print ("Quitting from %s.\n", (gchar *) data);
  gtk_main_quit ();
}


/* Main */

int
main (int argc, char *argv[])
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *box;
  GtkWidget *box1;
  GtkWidget *table;
  GtkWidget *table_bot;
  GtkWidget *entry;
  GtkWidget *separator;

  /* This is called in all GTK applications. Arguments are parsed
   * from the command line and are returned to the application. */
  gtk_init (&argc, &argv);

  /* Create the main window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  /* Call to set the title of the main window. */
  gtk_window_set_title (GTK_WINDOW (window), "The Super Spreadsheet!");

  /* Handler for delete_event. */
  g_signal_connect (G_OBJECT (window), "delete_event",
		    G_CALLBACK (delete_event), NULL);

  /* Sets the border width of the window. 
   * Perhaps later the width should variable. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 200);

  /* Main box (not really visible) to pack all the widgets into.
   * It will be used as a tool to arrange widgets. */
  box = gtk_vbox_new (FALSE, 0);

  /* Put the box into the main window. */
  gtk_container_add (GTK_CONTAINER (window), box);

/* Begin State button */
  /* Creates the button to show the state of the system at
   * the to top tof the window, called "State".   */
  button = gtk_button_new_with_label
    ("State: Dimensions displayed (dimA, dimB), etc");
    
  /* When the "State" button is clicked, we call the "callback_state"
   * function with a pointer to "state" as its argument */
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (callback_state), (gpointer) "asleep");

  /* Instead of gtk_container_add, we pack this button into the invisible
   * box, which has been packed into the window. */
  gtk_box_pack_start (GTK_BOX(box), button, TRUE, TRUE, 0);

  /* Always remember this step, this tells GTK that our preparation for
   * this button is complete, and it can now be displayed. */
  gtk_widget_show (button);
/* End State button */

/* Begin table in the middle with spreadsheet table in it. */
  table_bot = gtk_table_new (1, 3, FALSE);

/* Begin "Dimension Browser" widget */
  box1 = gtk_vbox_new (FALSE, 0);

  button = gtk_button_new_with_label ("Choose two dimensions");
  gtk_widget_show (button);
  gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);

  int   dimnumber = 10;
  char* dimname[dimnumber]; /* To hold the dimension names comming from ...? */
  for (int i = 0; i != dimnumber; ++i) {
    dimname[i] = (char*) malloc(10);
    sprintf(dimname[i],"Dimension%2d", i);
    button = gtk_check_button_new_with_label (dimname[i]);
    g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (choose_dim),
                      (gpointer) dimname[i] );
    gtk_box_pack_start(GTK_BOX (box1), button, TRUE, FALSE, 0);
    gtk_widget_show (button);
  }
  gtk_table_attach_defaults (GTK_TABLE (table_bot), box1, 0, 1, 0, 1);
  gtk_widget_show (box1);
/* End "Dimension Browser" widget */

/* Begin The SpreadSheet */
  int row = 10;
  int col = 10;
  char* cellnames[row][col];  /* To name each indvidual cell */
  table = gtk_table_new (row, col, FALSE);

  for (int i = 0; i != row; ++i) {
    int ii = i+1;
    for (int j = 0; j != col; ++j) {
      cellnames[i][j] = (char*) malloc(10);
      /* Name and creation of *this* cell */
      sprintf(cellnames[i][j],"Cell%2d%2d", i, j);
      button = gtk_button_new_with_label (cellnames[i][j]);
      g_signal_connect (G_OBJECT (button),
                        "clicked",
                        G_CALLBACK (callback_display),
                        (gpointer) (cellnames[i][j]));
      gtk_widget_show (button);
      int jj = j+1;
      gtk_table_attach (GTK_TABLE (table), button, i, ii, j, jj, 
                        GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL , 10, 10);
    }
  }
  gtk_widget_show (table);
  gtk_table_attach_defaults (GTK_TABLE (table_bot), table, 1, 2, 0, 1);
/* End The SpreadSheet */

/* Begin "Equation Browser" widget */
  button = gtk_button_new_with_label ("Equation Browser");
  /* When the quit button is clicked, we call the "quitting" function
   * with a pointer to "quit" as its argument */
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (quitting), (gpointer) "Equation Browser");
  gtk_table_attach_defaults (GTK_TABLE (table_bot), button, 2, 3, 0, 1);
  gtk_widget_show (button);

  gtk_box_pack_start(GTK_BOX (box), table_bot, TRUE, TRUE, 0);
  gtk_widget_show (table_bot);
/* End "Equation Browser" widget */
/* End table in the middle with spreadsheet table in it. */

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (box), separator, FALSE, TRUE, 0);
  gtk_widget_show (separator);


/* Begin bottom table with equation field and commit and quit buttons */
  table_bot = gtk_table_new (1, 3, FALSE);

/* Begin Equations field */
  entry = gtk_entry_new ();
  gtk_entry_set_max_length (GTK_ENTRY (entry), 150);
  g_signal_connect (G_OBJECT (entry), "activate", 
                    G_CALLBACK (entering_eqns), (gpointer) entry);
  gtk_entry_set_text (GTK_ENTRY (entry), "Type in your TransLucid equation");
  gtk_table_attach_defaults (GTK_TABLE (table_bot), entry, 0, 1, 0, 1);
  gtk_widget_show (entry);
/* End Equations field */

/* Begin Commit button */
  button = gtk_button_new_with_label ("Commit changes");

  /* The callback function is callback_display, for now simply to
   * show the contents of the widget. */
  g_signal_connect (G_OBJECT (button),
                    "clicked",
		    G_CALLBACK (callback_display),
                    (gpointer) "Commit your changes");
  gtk_table_attach_defaults (GTK_TABLE (table_bot), button, 1, 2, 0, 1);
  gtk_widget_show (button);
/* End Commit button */

/* Begin Quit button */
  /* Creates the quit button with the label "Quit". */
  button = gtk_button_new_with_label ("Quit");
    
  /* When the quit button is clicked, we call the "quitting" function
   * with a pointer to "quit" as its argument */
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (quitting), (gpointer) "quit");
  gtk_table_attach_defaults (GTK_TABLE (table_bot), button, 2, 3, 0, 1);
  gtk_widget_show (button);
/* Begin Quit button */

  gtk_box_pack_start(GTK_BOX (box), table_bot, TRUE, TRUE, 0);
  gtk_widget_show (table_bot);
/* End bottom table with equation field and commit and quit buttons */


  /* The order in which we show the buttons is not really important, but I
   * recommend showing the window last, so it all pops up at once. */
  gtk_widget_show (box);
  gtk_widget_show (window);
    
  /* Rest in gtk_main and wait for the fun to begin! */
  gtk_main ();

  return 0;
}

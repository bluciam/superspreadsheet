/* Beginnings of the Hyper Spreadshhet
   Blanca Mancilla and John Plaice
   Thu Apr 22 16:27:28 CEST 2010
*/

#include <gtk/gtk.h>
#include <stdlib.h>

/* Callback function for any cell in the spreadshetl. 
 * For now, the data passed to this function is printed to stdout. */
static void callback_cell( GtkWidget *widget,
                           gpointer   data )
{
    g_print ("You are in cell %s.\n", (gchar *) data);
}

/* Callback function for the state-of-the-system field.
 * For now, the data is passed into stdout until I learn else. */
static void callback_state( GtkWidget *widget,
                            gpointer   data )
{
    g_print ("The state of your system is ... asleep. %s.\n", (gchar *) data);
}

/* Callback funtion for delele event from the window manager.
 * We only want to quit with the Quit button to do proper finalization. */
static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    g_print ("Press Quit to quit. \n");
    return TRUE;
}

/* Callback function for the Quit button to quit */
static void quitting( GtkWidget *widget,
                      gpointer   data )
{
    gtk_main_quit ();
}


/* Main */

int main( int   argc,
          char *argv[] )
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *table;
    GtkWidget *table_bot;

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
     * Perhaps later the width is variable. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 200);

    /* Main box (not really visible) to pack all the widgets into.
     * It will be used as a tool to arrange widgets. */
    box = gtk_vbox_new (FALSE, 0);

    /* Put the box into the main window. */
    gtk_container_add (GTK_CONTAINER (window), box);

/* Begin State button */
    /* Creates the button to show the state of the system at
     * the to top tof the window, called "State".   */
    button = gtk_button_new_with_label ("State");
    
    /* When the "State" button is clicked, we call the "callback_state"
     * function with a pointer to "state" as its argument */
    g_signal_connect (G_OBJECT (button), "clicked",
		      G_CALLBACK (callback_state), (gpointer) "state");

    /* Instead of gtk_container_add, we pack this button into the invisible
     * box, which has been packed into the window. */
    gtk_box_pack_start (GTK_BOX(box), button, TRUE, TRUE, 0);

    /* Always remember this step, this tells GTK that our preparation for
     * this button is complete, and it can now be displayed. */
    gtk_widget_show (button);
/* End State button */


/* Begin The SpreadSheet */
    int row = 10;
    int col = 10;
    char* cellnames[10][10];  /* To name each indvidual cell */
    table = gtk_table_new (row, col, FALSE);

    for (int i = 0; i != row; ++i) {
       int ii = i+1;
       for (int j = 0; j != col; ++j) {
         cellnames[i][j] = (char*) malloc(10);
         /* Name and creation of *this* cell */
         sprintf(cellnames[i][j],"cell%2d%2d", i, j);
         button = gtk_button_new_with_label (cellnames[i][j]);
         g_signal_connect (G_OBJECT (button), "clicked",
		           G_CALLBACK (callback_cell),
                           (gpointer) cellnames[i][j]);
         gtk_widget_show (button);
         int jj = j+1;
         gtk_table_attach (GTK_TABLE (table), button, i, ii, j, jj, 
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL , 10, 10);

      /* g_print ("%s\n", cellnames[i][j]); */
      /* g_print ("You are in cell left = %d, right = %d, top = %d, bottom = %d. \n", i, ii, j, jj); */
      /* gtk_table_attach_defaults (GTK_TABLE (table), button, i, ii, j, jj);*/
       }
    }
    gtk_widget_show (table);
    gtk_box_pack_start(GTK_BOX (box), table, TRUE, TRUE, 0);
/* End The SpreadSheet */


/* Begin bottom box with equation field and quit button */
    /* Bottom box (not really visible) to pack equations and quit button. */
    table_bot = gtk_table_new (1, 2, FALSE);

    /* Create the equation field, now as a button. */
    button = gtk_button_new_with_label ("Equations in TL");

    /* Call the same callback function as for state with a different argument,
     * passing a pointer to "equations" instead. */
    g_signal_connect (G_OBJECT (button), "clicked",
		      G_CALLBACK (callback_cell), (gpointer) "equations");
    gtk_table_attach_defaults (GTK_TABLE (table_bot), button, 0, 1, 0, 1);
    gtk_widget_show (button);

    /* Creates the quit button with the label "Quit". */
    button = gtk_button_new_with_label ("Quit");
    
    /* When the quit button is clicked, we call the "quitting" function
     * with a pointer to "quit" as its argument */
    g_signal_connect (G_OBJECT (button), "clicked",
		      G_CALLBACK (quitting), (gpointer) "quit");
    g_signal_connect (G_OBJECT (button), "clicked",
		      G_CALLBACK (quitting), (gpointer) "quit");
    gtk_table_attach_defaults (GTK_TABLE (table_bot), button, 1, 2, 0, 1);
    gtk_widget_show (button);

    gtk_box_pack_start(GTK_BOX (box), table_bot, TRUE, TRUE, 0);
    gtk_widget_show (table_bot);

/* End bottom box with equation field and quit button */

    /* The order in which we show the buttons is not really important, but I
     * recommend showing the window last, so it all pops up at once. */
    gtk_widget_show (box);
    gtk_widget_show (window);
    
    /* Rest in gtk_main and wait for the fun to begin! */
    gtk_main ();

    return 0;
}

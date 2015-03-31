/*
 *  * Compile me with:
 *   *   gcc -o tut tut.c $(pkg-config --cflags --libs gtk+-2.0 gmodule-2.0)
 *    */

#include <gtk/gtk.h>


const char *glade_search_path[] = {"./preferences/prefs.glade", "/usr/local/share/varnam/ibus/prefs.glade", "/usr/share/varnam/ibus/prefs.glade"};

static gboolean
load_glade_file (GtkBuilder *builder)
{
  GError     *error = NULL;
  size_t i;
  for (i = 0; i < (sizeof glade_search_path / sizeof *glade_search_path); i++) {
    if (g_file_test (glade_search_path[i], G_FILE_TEST_IS_REGULAR) &&
        gtk_builder_add_from_file (builder, glade_search_path[i], &error)) {
      return TRUE;
    } 
    g_warning ("FAILED %s", glade_search_path[i]);
  }

  return FALSE;
}

int
main( int    argc,
    char **argv )
{
  GtkBuilder *builder;
  GtkWidget  *window;

  /* Init GTK+ */
  gtk_init( &argc, &argv );

  /* Create new GtkBuilder object */
  builder = gtk_builder_new();

  if (!load_glade_file (builder)) {
    g_warning( "%s", "Faild to find prefs.glade");
    return( 1 );
  }

  /* Get main window pointer from UI */
  window = GTK_WIDGET( gtk_builder_get_object( builder, "ibus-varnam-preferences" ) );

  /* Connect signals */
  gtk_builder_connect_signals( builder, NULL );

  /* Destroy builder, since we don't need it anymore */
  g_object_unref( G_OBJECT( builder ) );

  /* Show window. All other widgets are automatically shown by GtkBuilder */
  gtk_widget_show( window );

  /* Start main loop */
  gtk_main();

  return( 0 );
}

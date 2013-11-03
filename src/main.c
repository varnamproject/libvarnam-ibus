/**
  The MIT License (MIT)

  Copyright (c) Navaneeth K.N

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
**/

#include <stdio.h>
#include <ibus.h>
#include "engine.h"

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

/* command line options */
static gboolean ibus = FALSE;
static gboolean verbose = FALSE;
static gchar *langCode = NULL;

static FILE *logfile = NULL;
static void
glib_log_handler (const gchar *logDomain, GLogLevelFlags logLevel, const gchar *message, gpointer data)
{
  GString *logs;

  if (logfile == NULL) {
    logs = g_string_new (g_get_home_dir ());
    g_string_append (logs, "/.varnam/ibus-engine/logs");
    if (g_mkdir_with_parents (logs->str, 0755) == -1) {
      g_printerr ("Failed to create logs directory: %s. Logging to file will be disabled\n", logs->str);
    }

    g_string_append_printf (logs, "/logs-%s.txt", langCode);

    logfile = fopen (logs->str, "a");
    if (logfile == NULL) {
      g_printerr ("Failed to create logs: %s. Logging to file will be disabled\n", logs->str);
    }

    g_string_free (logs, TRUE);
  }

  if (logfile != NULL) {
    fprintf (logfile, "%s %s", logDomain, message);
    fflush (logfile);
  }
  g_print ("%s %s", logDomain, message);
}


static const GOptionEntry entries[] =
{
  { "ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus, "component is executed by ibus", NULL },
  { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "verbose", NULL },
  { "language", 'l', 0, G_OPTION_ARG_STRING, &langCode, "Language code for this engine", NULL },
  { NULL },
};

static void
ibus_disconnected_cb (IBusBus  *bus,
    gpointer  user_data)
{
  ibus_quit ();
}

static void
init (void)
{
  ibus_init ();

  bus = ibus_bus_new ();
  g_object_ref_sink (bus);
  g_signal_connect (bus, "disconnected", G_CALLBACK (ibus_disconnected_cb), NULL);

  factory = ibus_factory_new (ibus_bus_get_connection (bus));
  g_object_ref_sink (factory);
  ibus_factory_add_engine (factory, "varnam", IBUS_TYPE_VARNAM_ENGINE);

  if (ibus) {
    ibus_bus_request_name (bus, "org.freedesktop.IBus.Varnam", 0);
  }
  else {
    IBusComponent *component;
    component = ibus_component_new ("org.freedesktop.IBus.Varnam",
        "Varnam",
        "0.1.0",
        "MIT",
        "Navaneeth K.N <navaneethkn@gmail.com>",
        "http://varnamproject.com",
        "",
        "ibus-varnam");
    ibus_component_add_engine (component,
        ibus_engine_desc_new ("varnam",
          "Varnam",
          "Varnam",
          "ko",
          "MIT",
          "Navaneeth K.N <navaneethkn@gmail.com>",
          "icons/ibus-enchant.svg",
          "us"));
    ibus_bus_register_component (bus, component);
  }

  /* Initialize varnam handle */
  varnam_engine_init_handle (langCode);
}

int main(int argc, char **argv)
{
  GError *error = NULL;
  GOptionContext *context;

  g_log_set_default_handler (glib_log_handler, NULL);

  /* Parse the command line */
  context = g_option_context_new ("- ibus engine");
  g_option_context_add_main_entries (context, entries, "ibus-varnam");

  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("Option parsing failed: %s\n", error->message);
    g_error_free (error);
    return (-1);
  }

  if (langCode == NULL) {
    g_printerr ("Language code is not set\n");
    return (-1);
  }

  /* Go */
  init ();
  ibus_main ();

  return 0;
}

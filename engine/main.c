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
#include <engine-util.h>

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;

/* command line options */
static gboolean ibus = FALSE;
static gboolean verbose = FALSE;
static gchar *langCode = NULL;

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
  GString *engineName, *busName;
  engineName = g_string_new ("Varnam.");
  g_string_append (engineName, langCode);
  busName = g_string_new ("org.freedesktop.IBus.Varnam.");
  g_string_append (busName, langCode);

  ibus_init ();

  bus = ibus_bus_new ();
  g_object_ref_sink (bus);
  g_signal_connect (bus, "disconnected", G_CALLBACK (ibus_disconnected_cb), NULL);

  factory = ibus_factory_new (ibus_bus_get_connection (bus));
  g_object_ref_sink (factory);
  ibus_factory_add_engine (factory, engineName->str, IBUS_TYPE_VARNAM_ENGINE);

  if (ibus) {
    ibus_bus_request_name (bus, busName->str, 0);
  }
  else {
    IBusComponent *component;
    component = ibus_component_new (busName->str,
        "Varnam input engine",
        "0.1",
        "MIT",
        "Navaneeth K.N <navaneethkn@gmail.com>",
        "http://varnamproject.com",
        "",
        "ibus-varnam");
    ibus_component_add_engine (component,
        ibus_engine_desc_new (engineName->str,
          engineName->str,
          "Varnam input engine",
          langCode,
          "MIT",
          "Navaneeth K.N <navaneethkn@gmail.com>",
          "icons/ibus-enchant.svg",
          "us"));
    ibus_bus_register_component (bus, component);
  }

  /* Initialize varnam handle */
  varnam_engine_init_handle (langCode);
  g_string_free (engineName, TRUE);
  g_string_free (busName, TRUE);
}

int main(int argc, char **argv)
{
  GError *error = NULL;
  GOptionContext *context;

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
    g_printerr("usage : <command> -l <language_code>\n");
    g_printerr("eg : ibus-engine-varnam -l ml\n");
    return (-1);
  }

  enable_logging ("engine", langCode);

  /* Go */
  init ();
  ibus_main ();

  return 0;
}

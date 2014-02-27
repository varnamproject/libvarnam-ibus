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
#include <glib.h>
#include "engine-util.h"

static gchar *langCode = NULL;
static gchar *componentName = NULL;
static FILE *logfile = NULL;
static void
glib_log_handler (const gchar *logDomain, GLogLevelFlags logLevel, const gchar *message, gpointer data)
{
  GString *logs;

  if (logfile == NULL) {
    logs = ibus_varnam_engine_get_config_dir ();
    if (logs == NULL)
      return;

    g_string_append (logs, "/logs");
    if (g_mkdir_with_parents (logs->str, 0755) == -1) {
      g_printerr ("Failed to create logs directory: %s. Logging to file will be disabled\n", logs->str);
    }

    g_string_append_printf (logs, "/%s-logs-%s.txt", componentName, langCode);
    logfile = fopen (logs->str, "a");
    if (logfile == NULL) {
      g_printerr ("Failed to create logs: %s. Logging to file will be disabled\n", logs->str);
      g_string_free (logs, TRUE);
      return;
    }
    g_string_free (logs, TRUE);
  }

  fprintf (logfile, "%s %s", logDomain, message);
  fflush (logfile);
  g_print ("%s %s", logDomain, message);
}

void
enable_logging (gchar *cname, gchar *lcode)
{
  componentName = cname;
  langCode = lcode;
  g_log_set_default_handler (glib_log_handler, NULL);
}

GString*
ibus_varnam_engine_get_config_dir()
{
  GString *configDir;
  configDir = g_string_new (g_get_user_config_dir ());
  g_string_append (configDir, "/varnam/ibus-engine");
  if (g_mkdir_with_parents (configDir->str, 0755) == -1) {
    g_printerr ("Failed to create config directory: %s\n", configDir->str);
  }

  return configDir;
}

GString*
ibus_varnam_engine_get_data_dir()
{
  GString *dataDir;
  dataDir = g_string_new (g_get_user_data_dir ());
  g_string_append (dataDir, "/varnam/suggestions");
  if (g_mkdir_with_parents (dataDir->str, 0755) == -1) {
    g_printerr ("Failed to create config directory: %s\n", dataDir->str);
  }

  return dataDir;
}

void
ibus_varnam_engine_persist_key_file (GKeyFile *keyFile, GString *filePath)
{
 gchar *keyFileContents;
 FILE *f = NULL;
 f = fopen (filePath->str, "w");
 if (f == NULL) {
   g_printerr ("Failed to persist key file to: %s\n", filePath->str);
 }
 else {
   keyFileContents = g_key_file_to_data (keyFile, NULL, NULL);
   fprintf (f, "%s", keyFileContents);
   fclose (f);
   g_free (keyFileContents);
 }
}


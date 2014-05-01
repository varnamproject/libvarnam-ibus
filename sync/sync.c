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

#include <stdlib.h>
#include <glib.h>
#include <varnam.h>
#include <deps/sqlite3.h>
#include <engine-util.h>

static gchar *langCode = NULL;
static gchar *learnFromDir = NULL;

static const GOptionEntry entries[] =
{
  { "language", 'l', 0, G_OPTION_ARG_STRING, &langCode, "Language code for this engine", NULL },
  { "learn-from-dir", 'f', 0, G_OPTION_ARG_STRING, &learnFromDir, "If set, learns from all the files in the directory", NULL },
  { NULL },
};

int main (int argc, char **argv)
{
  GOptionContext *context;
  GError *error = NULL;

  /* Parse the command line */
  context = g_option_context_new ("- varnam sync");
  g_option_context_add_main_entries (context, entries, "varnam-sync");
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("Option parsing failed: %s\n", error->message);
    g_error_free (error);
    return (-1);
  }

  if (langCode == NULL) {
    g_printerr ("Language code is not set\n");
    g_printerr("usage : varnam-sync -l <language code>\n");
    return (-1);
  }

  enable_logging ("sync", langCode);

  if (learnFromDir != NULL) {
    /* make sure we have the varnam data directory created properly */
    GString *dataDir = ibus_varnam_engine_get_data_dir();
    g_message ("Using: %s\n", dataDir->str);
    g_message ("Learning from: %s\n", learnFromDir);

    varnam *handle = NULL;
    char *msg;
    int rc = varnam_init_from_lang (langCode, &handle, &msg);
    if (rc != VARNAM_SUCCESS) {
      g_message ("Failed to initialize varnam. %s\n", msg);
      return -1;
    }

    GDateTime *startTime = g_date_time_new_now_local ();
    int i = 0;
    while (true) {
      GString *fileName = g_string_new ("");
      g_string_printf (fileName, "%d.txt", i++);
      gchar *fullPath = g_build_path ("/", learnFromDir, fileName->str, NULL);

      gboolean fileExists = g_file_test (fullPath, G_FILE_TEST_IS_REGULAR);
      if (!fileExists) {
        break;
      }

      g_message ("Learning from: %s\n", fullPath);
      vlearn_status status;
      rc = varnam_learn_from_file (handle, fullPath, &status, NULL, NULL);
      if (rc != VARNAM_SUCCESS) {
        g_message ("Failed to learn from: %s. %s", fullPath, varnam_get_last_error (handle));
      }
      else {
        g_message ("Learned from: %s. Total words = %d, Failed = %d\n", fullPath, status.total_words, status.failed);
      }

      if (i == 6) {
        /* Learning only 5 for performance reasons */
        break;
      }
    }

    GDateTime *endTime = g_date_time_new_now_local ();
    GTimeSpan elapsedTime = g_date_time_difference (endTime, startTime);
    g_message ("Finished sync. Took %" G_GINT64_FORMAT " microseconds\n", elapsedTime);
    varnam_destroy (handle);
  }

  return 0;
}

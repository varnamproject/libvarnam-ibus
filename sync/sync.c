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

static gchar *langCode = NULL;

static const GOptionEntry entries[] =
{
  { "language", 'l', 0, G_OPTION_ARG_STRING, &langCode, "Language code for this engine", NULL },
  { NULL },
};


static gboolean
upload_word (const gchar *word, const gchar *lang, const gchar *host)
{
  GString *postData, *url, *commandLine;
  GError *error = NULL;
  gboolean spawned;

  postData = g_string_new ("");
  g_string_printf (postData, "\"lang=%s&text=%s\"", lang, word);
  url = g_string_new ("");
  g_string_printf (url, "%s/api/learn", host);

  commandLine = g_string_new ("");
  g_string_printf (commandLine, "curl --silent --data %s %s --output /dev/null", postData->str, url->str);

  spawned = g_spawn_command_line_async (commandLine->str, &error);
  if (!spawned) {
    g_print ("Failed to learn: %s. %s\n", word, error->message);
  }

  g_string_free (postData, TRUE);
  g_string_free (url, TRUE);
  g_string_free (commandLine, TRUE);

  return spawned;
}

int main (int argc, char **argv)
{
  GOptionContext *context;
  GError *error = NULL;
  gboolean uploaded;

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
    return (-1);
  }


  uploaded = upload_word ("hey", "ml", "http://192.168.1.2:3000");
  if (!uploaded) {
    g_print ("Pooy failed\n");
  }
  return 0;
}

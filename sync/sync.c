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

static void
upload_word (const gchar *word, const gchar *lang, const gchar *host)
{
  GString *postData, *url;
  gchar *argv[4];
  GError *error = NULL;
  gboolean spawned;

  postData = g_string_new ("");
  g_string_printf (postData, "\"lang=%s&text=%s\"", lang, word);
  url = g_string_new ("");
  g_string_printf (url, "%s/learn", host);

  argv[0] = "curl";
  argv[1] = "--data";
  argv[2] = postData->str;
  argv[3] = url->str;

  spawned = g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
  if (!spawned) {
    g_print ("Failed to learn: %s. %s\n", word, error->message);
  }

  g_string_free (postData, TRUE);
  g_string_free (url, TRUE);
}

int main (int argc, char **argv)
{
  upload_word ("hey", "ml", "http://192.168.1.3:8082");
  return 0;
}

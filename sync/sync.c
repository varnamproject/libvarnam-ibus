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

#define TIMESTAMP_FILE "timestamps"
#define UPLOAD_QUERY "SELECT w.word FROM WORDS w, patterns_content p WHERE w.id = p.word_id AND p.learned = 1 and w.learned_on > date('%d-%s-%s') GROUP BY w.word ORDER BY word"

static gchar *langCode = NULL;

static const GOptionEntry entries[] =
{
  { "language", 'l', 0, G_OPTION_ARG_STRING, &langCode, "Language code for this engine", NULL },
  { NULL },
};

/*TODO: Need to handle errors */
static gboolean
upload_word (const gchar *word, const gchar *lang)
{
  GString *postData, *url, *commandLine;
  GError *error = NULL;
  gboolean spawned;

  postData = g_string_new ("");
  g_string_printf (postData, "\"lang=%s&text=%s\"", lang, word);
  url = g_string_new ("http://192.168.1.3:3000/api/learn");

  commandLine = g_string_new ("");
  g_string_printf (commandLine, "curl --silent --data %s %s --output /dev/null", postData->str, url->str);

  spawned = g_spawn_command_line_async (commandLine->str, &error);
  if (!spawned) {
    g_print ("Failed to upload: %s. %s\n", word, error->message);
  }

  g_string_free (postData, TRUE);
  g_string_free (url, TRUE);
  g_string_free (commandLine, TRUE);

  return spawned;
}

static GString*
get_timestamp_file_path(void)
{
  GString *configDir;
  configDir = ibus_varnam_engine_get_config_dir ();
  if (configDir == NULL)
    return NULL;

  g_string_append_printf (configDir, "/%s", TIMESTAMP_FILE);
  return configDir;
}

static GKeyFile* 
get_timestamp_file(void)
{
  GString *timestampFilePath;
  GKeyFile *timestampFile;

  timestampFilePath = get_timestamp_file_path ();
  if (timestampFilePath == NULL)
    return NULL;

  timestampFile = g_key_file_new ();
  g_key_file_load_from_file (timestampFile, timestampFilePath->str, G_KEY_FILE_KEEP_COMMENTS, NULL);
  g_string_free (timestampFilePath, TRUE);
  return timestampFile;
}

static gint64
get_last_upload_timestamp(void) 
{
  gint64 timestamp = 0;
  GKeyFile *timestampFile;

  timestampFile = get_timestamp_file ();
  if (timestampFile == NULL)
    return 0;

  timestamp = g_key_file_get_int64 (timestampFile, "timestamps", "upload", NULL);
  g_key_file_free (timestampFile);
  return timestamp;
}

static int 
callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i = 0; i < argc; i++){
    upload_word (argv[i], langCode);
  }
  return 0;
}

static gboolean
upload_words_learned_after(gint64 lastUploadedTimestamp, GError **error)
{
  GString *query, *twoDigitMonth, *twoDigitDay;
  GDateTime *lastUploaded;
  sqlite3 *db;
  int rc;
  char *errMsg;
  gint year, month, day;

  rc = sqlite3_open ("/home/parallels/.varnam/suggestions/ml-unicode.vst.learnings", &db);
  if (rc) {
    g_print ("Can't open varnam learnings file. %s\n", sqlite3_errmsg (db));
    sqlite3_close (db);
    return FALSE;
  }

  lastUploaded = g_date_time_new_from_unix_local (lastUploadedTimestamp);
  g_date_time_get_ymd (lastUploaded, &year, &month, &day);

  /* glib gives month and date in single digit format where SQLIte needs it in two digits */
  twoDigitMonth = g_string_new ("");
  twoDigitDay = g_string_new ("");
  g_string_printf (twoDigitMonth, (month < 10) ? "0%d" : "%d", month);
  g_string_printf (twoDigitDay, (day < 10) ? "0%d" : "%d", day);

  query = g_string_new ("");
  g_string_printf (query, UPLOAD_QUERY, year, twoDigitMonth->str, twoDigitDay->str);

  rc = sqlite3_exec (db, query->str, callback, 0, &errMsg);
  if (rc != SQLITE_OK) {
    g_print ("Failed to execute the query. %s\n", errMsg);
    sqlite3_free (errMsg);
    sqlite3_close (db);
    return FALSE;
  }

  g_string_free (query, TRUE);
  g_string_free (twoDigitMonth, TRUE);
  g_string_free (twoDigitDay, TRUE);
  sqlite3_close (db);
  return TRUE;
}

static void
perform_upload(void)
{
  gint64 lastUploaded;
  GDateTime *now;
  GKeyFile *timestampFile;
  GString *timestampFilePath;

  lastUploaded = get_last_upload_timestamp ();
  if (lastUploaded != 0) {
    /* uploading all the items that are learned after the last upload */
    g_print ("here");
    upload_words_learned_after (lastUploaded, NULL);
  }
  else {
    /* No last upload. Writing current timestamp to the file */
    timestampFile = get_timestamp_file ();
    if (timestampFile != NULL) {
      now = g_date_time_new_now_local ();
      g_key_file_set_int64 (timestampFile, "timestamps", "upload", g_date_time_to_unix (now));
      timestampFilePath = get_timestamp_file_path ();
      if (timestampFilePath != NULL) {
        ibus_varnam_engine_persist_key_file (timestampFile, timestampFilePath);
      }

      g_date_time_unref (now);
      g_key_file_free (timestampFile);
      g_string_free (timestampFilePath, TRUE);
    }
  }
}

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
    return (-1);
  }

  perform_upload ();


  /*uploaded = upload_word ("hey", "ml", "http://192.168.1.2:3000");*/
  /*if (!uploaded) {*/
    /*g_print ("Pooy failed\n");*/
  /*}*/
  return 0;
}

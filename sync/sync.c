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

static gboolean
upload_word (const gchar *word, const gchar *lang)
{
  GString *postData, *url, *commandLine;
  GError *error = NULL;
  gboolean spawned;
  gint exitStatus;
  gchar *stdOut = NULL, *stdErr = NULL;

  postData = g_string_new ("");
  g_string_printf (postData, "\"lang=%s&text=%s\"", lang, word);
  url = g_string_new ("http://192.168.1.3:3000/api/learn");

  commandLine = g_string_new ("");
  g_string_printf (commandLine, "curl --fail --data %s %s --output /dev/null", postData->str, url->str);

  spawned = g_spawn_command_line_sync (commandLine->str, &stdOut, &stdErr, &exitStatus, &error);
  if (!spawned || exitStatus != 0) {
    g_print ("Failed to upload: %s. %s. %s. %s\n", word, error == NULL ? "" : error->message, stdOut, stdErr);
  }

  g_string_free (postData, TRUE);
  g_string_free (url, TRUE);
  g_string_free (commandLine, TRUE);
  g_free (stdOut);
  g_free (stdErr);

  return spawned && exitStatus == 0;
}

static gboolean
is_the_same_day (gint64 timestamp)
{
  GDateTime *now, *then;
  gint year, month, date, year1, month1, date1;

  then = g_date_time_new_from_unix_local (timestamp);
  now = g_date_time_new_now_local ();
  if (now == NULL)
    return FALSE;

  g_date_time_get_ymd (now, &year, &month, &date);
  g_date_time_get_ymd (then, &year1, &month1, &date1);

  return year == year1 && month == month1 && date == date1;
}

static gboolean
download_words (gint64 timestamp, GString **downloadedFilePath)
{
  GDateTime *lastDownloaded, *now;
  gint year, month, date, exitStatus;
  GString *url, *commandLine, *outputFile;
  GError *error = NULL;
  gboolean spawned;
  gchar *stdOut = NULL, *stdErr = NULL;

  lastDownloaded = g_date_time_new_from_unix_local (timestamp);
  if (lastDownloaded == NULL) {
    g_print ("Incorrect timestamp: %" G_GINT64_FORMAT  "\n", timestamp);
    return FALSE;
  }

  outputFile = g_string_new ("");
  now = g_date_time_new_now_local ();
  g_string_printf (outputFile, "%s/%" G_GINT64_FORMAT ".txt", g_get_tmp_dir (), g_date_time_to_unix (now));
  g_date_time_unref (now);

  g_date_time_get_ymd (lastDownloaded, &year, &month, &date);
  url = g_string_new ("");
  g_string_printf (url, "http://www.varnamproject.com/words/%s/%d/%d/%d", langCode, year, month, date);

  commandLine = g_string_new ("");
  g_string_printf (commandLine, "curl --fail -o %s %s", outputFile->str, url->str);

  spawned = g_spawn_command_line_sync (commandLine->str, &stdOut, &stdErr, &exitStatus, &error);
  if (!spawned || exitStatus != 0) {
    g_print ("Failed to download: %s\n, command: %s\n, output file: %s\n, error message: %s\n, stdout: %s\n, stderr: %s.\n", 
        url->str, commandLine->str, outputFile->str,  error == NULL ? "NULL" : error->message, stdOut, stdErr);
  }

  g_string_free (url, TRUE);
  g_string_free (commandLine, TRUE);

  *downloadedFilePath = outputFile;
  return spawned && exitStatus == 0;
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

static gint64
get_last_download_timestamp(void) 
{
  gint64 timestamp = 0;
  GKeyFile *timestampFile;

  timestampFile = get_timestamp_file ();
  if (timestampFile == NULL)
    return 0;

  timestamp = g_key_file_get_int64 (timestampFile, "timestamps", "download", NULL);
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
upload_words_learned_after(gint64 lastUploadedTimestamp)
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
write_current_timestamp_to_file(const char *key)
{
  GKeyFile *timestampFile;
  GString *timestampFilePath;
  GDateTime *now;

  timestampFile = get_timestamp_file ();
  if (timestampFile != NULL) {
    now = g_date_time_new_now_local ();
    g_key_file_set_int64 (timestampFile, "timestamps", key, g_date_time_to_unix (now));
    timestampFilePath = get_timestamp_file_path ();
    if (timestampFilePath != NULL) {
      ibus_varnam_engine_persist_key_file (timestampFile, timestampFilePath);
    }

    g_date_time_unref (now);
    g_key_file_free (timestampFile);
    g_string_free (timestampFilePath, TRUE);
  }
}

static void
perform_upload(void)
{
  gint64 lastUploaded;

  lastUploaded = get_last_upload_timestamp ();
  if (lastUploaded != 0) {
    /* uploading all the items that are learned after the last upload */
    upload_words_learned_after (lastUploaded);
  }

  write_current_timestamp_to_file ("upload");
}

static void
perform_download(void)
{
  gint64 lastDownloaded;
  GString *downloadedFile;
  gboolean downloaded;
  varnam *handle;
  char *msg;
  int rc;
  vlearn_status learnStatus;

  lastDownloaded = get_last_download_timestamp ();
  if (lastDownloaded != 0 && !is_the_same_day (lastDownloaded)) {
    downloaded = download_words (lastDownloaded, &downloadedFile);
    if (!downloaded) {
      g_print ("Failed to download the latest words");
      return;
    }

    rc = varnam_init ("/usr/local/share/varnam/vst/ml-unicode.vst", &handle, &msg);
    if (rc != VARNAM_SUCCESS) {
      g_print ("Error initializing varnam. %s\n", msg);
      return;
    }

    rc = varnam_config (handle, VARNAM_CONFIG_ENABLE_SUGGESTIONS, "/home/parallels/.varnam/suggestions/ml-unicode.vst.learnings");
    if (rc != VARNAM_SUCCESS) {
      g_print ("Error configuring suggestions. %s\n", msg);
      return;
    }

    rc = varnam_learn_from_file (handle, downloadedFile->str, &learnStatus, NULL, NULL);
    if (rc != VARNAM_SUCCESS) {
      g_print ("Error learning from file. %s\n", varnam_get_last_error (handle));
    }

    varnam_destroy (handle);
  }

  write_current_timestamp_to_file ("download");
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
  perform_download ();

  return 0;
}

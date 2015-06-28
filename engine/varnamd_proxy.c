
/*
 * Contains functions to handle communication to varnamd
 */

#include <curl/curl.h>
#include <glib.h>
#include "varnamd_proxy.h"

static const char* VARNAMD_PORT = "8123";
static const char* VARNAMD_URL = "http://localhost:8123";
static gboolean proxy_initialized = FALSE;

static gboolean
is_varnamd_running();

static gboolean
start_varnamd();

/* Initializes the proxy instance
 *
 * This routine pings varnamd on the default port
 * If varnamd is not running, it starts varnamd
 * If varnamd is running, it establishes the communication channel
 */
void
varnamd_proxy_init()
{
	if (!is_varnamd_running()) {
		g_message("varnamd is not running. Launching a new instance\n");
		gboolean started = start_varnamd();
		if (!started) {
			g_message("Failed to start varnamd. Sync will be disabled\n");
		} else {
			proxy_initialized = TRUE;
		}
	} else {
		proxy_initialized = TRUE;
	}
}

static gboolean
is_varnamd_running()
{
	CURL *curl;
  CURLcode res;
	gboolean running = FALSE;

	curl = curl_easy_init();
	if(curl) {
		GString *url = g_string_new(VARNAMD_URL);
		g_string_append(url, "/status");

		curl_easy_setopt(curl, CURLOPT_URL, url->str);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		res = curl_easy_perform(curl);
		if(res == CURLE_OK) {
			running = TRUE;
		}
		g_string_free(url, TRUE);
	}
	else {
    g_message ("Failed to init curl\n");
	}

	curl_easy_cleanup(curl);
	return running;
}

static gboolean
start_varnamd()
{
	gchar* argv[] = {"varnamd", "-p", VARNAMD_PORT, "-enable-internal-apis", "-log-to-file=true", NULL};
	GError *error = NULL;
	GPid pid;
	gboolean started = g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, &pid, &error);
	if (started) {
		g_message ("Started varnamd. Pid: %d\n", pid);
	} else {
    g_message ("Failed to start varnamd. %s\n", error->message);
	}

	return started;
}

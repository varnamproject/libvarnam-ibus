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

#include <string.h>
#include <varnam.h>
#include "engine.h"

typedef struct _IBusVarnamEngine IBusVarnamEngine;
typedef struct _IBusVarnamEngineClass IBusVarnamEngineClass;

struct _IBusVarnamEngine {
  IBusEngine parent;
  GString *preedit;
  gint cursor_pos;
  IBusLookupTable *table;
};

struct _IBusVarnamEngineClass {
  IBusEngineClass parent;
};

/* functions prototype */
/*static void	ibus_varnam_engine_class_init	(IBusVarnamEngineClass	*klass);*/
/*static void	ibus_varnam_engine_init		(IBusVarnamEngine		*engine);*/
static void	ibus_varnam_engine_destroy (IBusVarnamEngine *engine);
static gboolean ibus_varnam_engine_process_key_event (IBusEngine  *engine, guint keyval, guint keycode, guint modifiers);

/*[>static void ibus_varnam_engine_focus_in    (IBusEngine             *engine);<]*/
/*static void ibus_varnam_engine_focus_out   (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_reset       (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_disable     (IBusEngine             *engine);*/
/*static void ibus_engine_set_cursor_location (IBusEngine             *engine,*/
                                             /*gint                    x,*/
                                             /*gint                    y,*/
                                             /*gint                    w,*/
                                             /*gint                    h);*/
/*static void ibus_varnam_engine_set_capabilities*/
                                            /*(IBusEngine             *engine,*/
                                             /*guint                   caps);*/
/*static void ibus_varnam_engine_page_up     (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_page_down   (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_cursor_up   (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_cursor_down (IBusEngine             *engine);*/
/*static void ibus_enchant_property_activate  (IBusEngine             *engine,*/
                                             /*const gchar            *prop_name,*/
                                             /*gint                    prop_state);*/
/*static void ibus_varnam_engine_property_show*/
											/*(IBusEngine             *engine,*/
                                             /*const gchar            *prop_name);*/
/*static void ibus_varnam_engine_property_hide*/
											/*(IBusEngine             *engine,*/
                                             /*const gchar            *prop_name);*/


G_DEFINE_TYPE (IBusVarnamEngine, ibus_varnam_engine, IBUS_TYPE_ENGINE)

static varnam *handle = NULL;
void varnam_engine_init_handle (const gchar *langCode)
{
  char *msg;
  int rc;

  if (handle == NULL) {
    rc = varnam_init_from_lang (langCode, &handle, &msg);
    if (rc != VARNAM_SUCCESS) {
      g_message ("Error initializing varnam. %s\n", msg);
      handle = NULL;
    }
  }
}

static void
ibus_varnam_engine_class_init (IBusVarnamEngineClass *klass)
{
  IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

  ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_varnam_engine_destroy;
  engine_class->process_key_event = ibus_varnam_engine_process_key_event;
}

static void
ibus_varnam_engine_init (IBusVarnamEngine *engine)
{
  engine->preedit = g_string_new ("");
  engine->cursor_pos = 0;
  engine->table = ibus_lookup_table_new (9, 0, TRUE, TRUE);
  g_object_ref_sink (engine->table);
}

static void
ibus_varnam_engine_destroy (IBusVarnamEngine *engine)
{
  if (engine->preedit) {
    g_string_free (engine->preedit, TRUE);
    engine->preedit = NULL;
  }

  if (engine->table) {
    g_object_unref (engine->table);
    engine->table = NULL;
  }

  ((IBusObjectClass *) ibus_varnam_engine_parent_class)->destroy ((IBusObject *) engine);
}

static void
ibus_varnam_engine_update_lookup_table (IBusVarnamEngine *engine)
{
  varray *words;
  vword *word;
  int rc, i;

  if (engine->preedit->len == 0) {
    ibus_engine_hide_lookup_table ((IBusEngine *) engine);
    return;
  }

  ibus_lookup_table_clear (engine->table);

  rc = varnam_transliterate (handle, engine->preedit->str, &words);
  if (rc != VARNAM_SUCCESS) {
    g_message ("Error transliterating: %s. %s\n", engine->preedit->str, varnam_get_last_error (handle));
    ibus_engine_hide_lookup_table ((IBusEngine *) engine);
    return;
  }

  for (i = 0; i < varray_length (words); i++) {
    word = varray_get (words, i);
    ibus_lookup_table_append_candidate (engine->table, ibus_text_new_from_string (word->text));
  }
  ibus_lookup_table_append_candidate (engine->table, ibus_text_new_from_string (engine->preedit->str));

  ibus_engine_update_lookup_table ((IBusEngine *) engine, engine->table, TRUE);
}

static void
ibus_varnam_engine_update_preedit (IBusVarnamEngine *engine)
{
  IBusText *text;

  text = ibus_text_new_from_static_string (engine->preedit->str);
  ibus_engine_update_preedit_text ((IBusEngine *) engine,
                                   text,
                                   engine->cursor_pos,
                                   TRUE);
}

static void
ibus_varnam_engine_clear_state (IBusVarnamEngine *engine)
{
  g_string_assign (engine->preedit, "");
  engine->cursor_pos = 0;
  ibus_varnam_engine_update_preedit (engine);
  ibus_lookup_table_clear (engine->table);
  ibus_engine_hide_lookup_table ((IBusEngine*) engine);
}

static gboolean
ibus_varnam_engine_commit (IBusVarnamEngine *engine, IBusText *text, gboolean shouldLearn)
{
  int rc;
  if (shouldLearn) {
    rc = varnam_learn (handle, ibus_text_get_text (text));
    if (rc != VARNAM_SUCCESS) {
      g_message ("Failed to learn: %s\n", ibus_text_get_text (text));
    }
  }

   /* Not releasing candidate because it is a borrowed reference */
  ibus_engine_commit_text ((IBusEngine *) engine, text);
  ibus_varnam_engine_clear_state (engine);
  return TRUE;
}

static IBusText*
ibus_varnam_engine_get_candidate (IBusVarnamEngine *engine)
{
  if (ibus_lookup_table_get_number_of_candidates (engine->table) == 0)
    return NULL;

  return ibus_lookup_table_get_candidate (engine->table, ibus_lookup_table_get_cursor_pos (engine->table));
}

#define is_alpha(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

static gboolean
is_word_break (guint keyval)
{
  if (keyval == 46 || keyval == 44 || keyval == 63 || keyval == 33 || keyval == 40 || keyval == 41 || keyval == 34 || keyval == 59 || keyval == 39)
    return TRUE;
  return FALSE;
}

static gboolean
ibus_varnam_engine_process_key_event (IBusEngine *engine,
                                       guint       keyval,
                                       guint       keycode,
                                       guint       modifiers)
{
  IBusText *text, *tmp;
  IBusVarnamEngine *varnamEngine = (IBusVarnamEngine *) engine;

  if (modifiers & IBUS_RELEASE_MASK)
    return FALSE;

  modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

  if (modifiers != 0) {
    if (varnamEngine->preedit->len == 0)
      return FALSE;
    else
      return TRUE;
  }
 
  switch (keyval) {
    case IBUS_space:
      text = ibus_varnam_engine_get_candidate (varnamEngine);
      if (text == NULL) {
        tmp = ibus_text_new_from_printf ("%s ", varnamEngine->preedit->str);
        return ibus_varnam_engine_commit (varnamEngine, tmp, FALSE);
      }
      tmp = ibus_text_new_from_printf ("%s ", ibus_text_get_text (text));
      return ibus_varnam_engine_commit (varnamEngine, tmp, TRUE);

    case IBUS_Return:
      text = ibus_varnam_engine_get_candidate (varnamEngine);
      if (text == NULL) {
        tmp = ibus_text_new_from_static_string (varnamEngine->preedit->str);
        ibus_varnam_engine_commit (varnamEngine, tmp, FALSE);
        return FALSE;
      }
      return ibus_varnam_engine_commit (varnamEngine, text, TRUE);

    case IBUS_Escape:
      if (varnamEngine->preedit->len == 0)
        return FALSE;

      tmp = ibus_text_new_from_static_string (varnamEngine->preedit->str);
      ibus_varnam_engine_commit (varnamEngine, tmp, FALSE);
      return FALSE;

    case IBUS_Left:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos > 0) {
        varnamEngine->cursor_pos --;
        ibus_varnam_engine_update_preedit (varnamEngine);
      }
      return TRUE;

    case IBUS_Right:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos < varnamEngine->preedit->len) {
        varnamEngine->cursor_pos ++;
        ibus_varnam_engine_update_preedit (varnamEngine);
      }
      return TRUE;

    case IBUS_Up:
      if (varnamEngine->preedit->len == 0)
        return FALSE;

      ibus_lookup_table_cursor_up (varnamEngine->table);
      ibus_engine_update_lookup_table (engine, varnamEngine->table, TRUE);
      return TRUE;

    case IBUS_Down:
      if (varnamEngine->preedit->len == 0)
        return FALSE;

      ibus_lookup_table_cursor_down (varnamEngine->table);
      ibus_engine_update_lookup_table (engine, varnamEngine->table, TRUE);

      return TRUE;

    case IBUS_BackSpace:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos > 0) {
        varnamEngine->cursor_pos --;
        g_string_erase (varnamEngine->preedit, varnamEngine->cursor_pos, 1);
        ibus_varnam_engine_update_preedit (varnamEngine);
        ibus_varnam_engine_update_lookup_table (varnamEngine);
      }
      return TRUE;

    case IBUS_Delete:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos < varnamEngine->preedit->len) {
        g_string_erase (varnamEngine->preedit, varnamEngine->cursor_pos, 1);
        ibus_varnam_engine_update_preedit (varnamEngine);
        ibus_varnam_engine_update_lookup_table (varnamEngine);
      }
      return TRUE;
  }

  if (is_word_break (keyval)) {
    text = ibus_varnam_engine_get_candidate (varnamEngine);
    if (text != NULL) {
      tmp = ibus_text_new_from_printf ("%s%c", ibus_text_get_text (text), keyval);
      return ibus_varnam_engine_commit (varnamEngine, tmp, TRUE);
    }

    return FALSE;
  }

  if (keyval <= 128) {
    if (varnamEngine->preedit->len == 0) {
      tmp = ibus_text_new_from_static_string ("");
      ibus_engine_commit_text ((IBusEngine *) varnamEngine, tmp);
    }
    g_string_insert_c (varnamEngine->preedit, varnamEngine->cursor_pos, keyval);
    varnamEngine->cursor_pos ++;
    ibus_varnam_engine_update_preedit (varnamEngine);
    ibus_varnam_engine_update_lookup_table (varnamEngine);
    return TRUE;
  }

  return FALSE;
}

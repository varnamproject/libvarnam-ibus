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

#include "engine.h"

typedef struct _IBusVarnamEngine IBusVarnamEngine;
typedef struct _IBusVarnamEngineClass IBusVarnamEngineClass;

struct _IBusVarnamEngine {
	IBusEngine parent;

    /* members */
  GString *preedit;
  gint cursor_pos;

  IBusLookupTable *table;
};

struct _IBusVarnamEngineClass {
	IBusEngineClass parent;
};

/* functions prototype */
static void	ibus_varnam_engine_class_init	(IBusVarnamEngineClass	*klass);
static void	ibus_varnam_engine_init		(IBusVarnamEngine		*engine);
static void	ibus_varnam_engine_destroy		(IBusVarnamEngine		*engine);
static gboolean 
			ibus_varnam_engine_process_key_event
                                            (IBusEngine             *engine,
                                             guint               	 keyval,
                                             guint               	 keycode,
                                             guint               	 modifiers);
/*static void ibus_varnam_engine_focus_in    (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_focus_out   (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_reset       (IBusEngine             *engine);*/
/*static void ibus_varnam_engine_enable      (IBusEngine             *engine);*/
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

static void ibus_varnam_engine_commit_string
                                            (IBusVarnamEngine      *engine,
                                             const gchar            *string);
static void ibus_varnam_engine_update      (IBusVarnamEngine      *engine);

G_DEFINE_TYPE (IBusVarnamEngine, ibus_varnam_engine, IBUS_TYPE_ENGINE)

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
  /* Init varnam here */
    /*if (broker == NULL) {*/
        /*broker = enchant_broker_init ();*/
        /*dict = enchant_broker_request_dict (broker, "en");*/
    /*}*/

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
ibus_varnam_engine_update_lookup_table (IBusVarnamEngine *enchant)
{
    /*gchar ** sugs;*/
    /*gint n_sug, i;*/
    /*gboolean retval;*/

    /*if (enchant->preedit->len == 0) {*/
        /*ibus_engine_hide_lookup_table ((IBusEngine *) enchant);*/
        /*return;*/
    /*}*/

    /*ibus_lookup_table_clear (enchant->table);*/
    
    /*[> Do varnam call here <]*/
    /*[>sugs = enchant_dict_suggest (dict,<]*/
                                 /*[>enchant->preedit->str,<]*/
                                 /*[>enchant->preedit->len,<]*/
                                 /*[>&n_sug);<]*/

    /*if (sugs == NULL || n_sug == 0) {*/
        /*ibus_engine_hide_lookup_table ((IBusEngine *) enchant);*/
        /*return;*/
    /*}*/

    /*for (i = 0; i < n_sug; i++) {*/
        /*ibus_lookup_table_append_candidate (enchant->table, ibus_text_new_from_string (sugs[i]));*/
    /*}*/

    /*ibus_engine_update_lookup_table ((IBusEngine *) enchant, enchant->table, TRUE);*/
}

static void
ibus_varnam_engine_update_preedit (IBusVarnamEngine *engine)
{
    /*IBusText *text;*/
    /*gint retval;*/

    /*text = ibus_text_new_from_static_string (engine->preedit->str);*/
    /*text->attrs = ibus_attr_list_new ();*/
    
    /*ibus_attr_list_append (text->attrs,*/
                           /*ibus_attr_underline_new (IBUS_ATTR_UNDERLINE_SINGLE, 0, enchant->preedit->len));*/

    /*if (enchant->preedit->len > 0) {*/
        /*retval = enchant_dict_check (dict, enchant->preedit->str, enchant->preedit->len);*/
        /*if (retval != 0) {*/
            /*ibus_attr_list_append (text->attrs,*/
                               /*ibus_attr_foreground_new (0xff0000, 0, enchant->preedit->len));*/
        /*}*/
    /*}*/
    
    /*ibus_engine_update_preedit_text ((IBusEngine *)enchant,*/
                                     /*text,*/
                                     /*enchant->cursor_pos,*/
                                     /*TRUE);*/

}

/* commit preedit to client and update preedit */
static gboolean
ibus_varnam_engine_commit_preedit (IBusVarnamEngine *engine)
{
  if (engine->preedit->len == 0)
    return FALSE;

  ibus_varnam_engine_commit_string (engine, engine->preedit->str);
  g_string_assign (engine->preedit, "");
  engine->cursor_pos = 0;

  ibus_varnam_engine_update (engine);

  return TRUE;
}


static void
ibus_varnam_engine_commit_string (IBusVarnamEngine *engine,
                                  const gchar       *string)
{
    IBusText *text;
    text = ibus_text_new_from_static_string (string);
    ibus_engine_commit_text ((IBusEngine *) engine, text);
}

static void
ibus_varnam_engine_update (IBusVarnamEngine *engine)
{
    ibus_varnam_engine_update_preedit (engine);
    ibus_engine_hide_lookup_table ((IBusEngine *) engine);
}

#define is_alpha(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

static gboolean 
ibus_varnam_engine_process_key_event (IBusEngine *engine,
                                       guint       keyval,
                                       guint       keycode,
                                       guint       modifiers)
{
  IBusText *text;
  IBusVarnamEngine *varnamEngine = (IBusVarnamEngine *) engine;

  g_print ("In process key event: %s\n", varnamEngine->preedit->str);

  if (modifiers & IBUS_RELEASE_MASK)
    return FALSE;

  modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

  /*if (modifiers == IBUS_CONTROL_MASK && keyval == IBUS_s) {*/
    /*ibus_varnam_engine_update_lookup_table (enchant);*/
    /*return TRUE;*/
  /*}*/

  if (modifiers != 0) {
    if (varnamEngine->preedit->len == 0)
      return FALSE;
    else
      return TRUE;
  }


  switch (keyval) {
    case IBUS_space:
      g_string_append (varnamEngine->preedit, " ");
      return ibus_varnam_engine_commit_preedit (varnamEngine);
    case IBUS_Return:
      return ibus_varnam_engine_commit_preedit (varnamEngine);

    case IBUS_Escape:
      if (varnamEngine->preedit->len == 0)
        return FALSE;

      g_string_assign (varnamEngine->preedit, "");
      varnamEngine->cursor_pos = 0;
      ibus_varnam_engine_update (varnamEngine);
      return TRUE;        

    case IBUS_Left:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos > 0) {
        varnamEngine->cursor_pos --;
        ibus_varnam_engine_update (varnamEngine);
      }
      return TRUE;

    case IBUS_Right:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos < varnamEngine->preedit->len) {
        varnamEngine->cursor_pos ++;
        ibus_varnam_engine_update (varnamEngine);
      }
      return TRUE;

    case IBUS_Up:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos != 0) {
        varnamEngine->cursor_pos = 0;
        ibus_varnam_engine_update (varnamEngine);
      }
      return TRUE;

    case IBUS_Down:
      if (varnamEngine->preedit->len == 0)
        return FALSE;

      if (varnamEngine->cursor_pos != varnamEngine->preedit->len) {
        varnamEngine->cursor_pos = varnamEngine->preedit->len;
        ibus_varnam_engine_update (varnamEngine);
      }

      return TRUE;

    case IBUS_BackSpace:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos > 0) {
        varnamEngine->cursor_pos --;
        g_string_erase (varnamEngine->preedit, varnamEngine->cursor_pos, 1);
        ibus_varnam_engine_update (varnamEngine);
      }
      return TRUE;

    case IBUS_Delete:
      if (varnamEngine->preedit->len == 0)
        return FALSE;
      if (varnamEngine->cursor_pos < varnamEngine->preedit->len) {
        g_string_erase (varnamEngine->preedit, varnamEngine->cursor_pos, 1);
        ibus_varnam_engine_update (varnamEngine);
      }
      return TRUE;
  }

  if (is_alpha (keyval)) {
    g_string_insert_c (varnamEngine->preedit,
        varnamEngine->cursor_pos,
        keyval);

    varnamEngine->cursor_pos ++;
    ibus_varnam_engine_update (varnamEngine);

    return TRUE;
  }

  return FALSE;
}

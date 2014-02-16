/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp2txt.c
 * 
 * Copyright (C) 2014 Hodong Kim <hodong@cogno.org>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <glib-object.h>
#include "hwp.h"
#include "hwp2txt.h"

/** HwpToTxt class ***********************************************************/
static void hwp_to_txt_iface_init (HwpListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HwpToTxt, hwp_to_txt, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (HWP_TYPE_LISTENER, hwp_to_txt_iface_init))

/**
 * hwp_to_txt_new:
 *
 * Creates a new #HwpToTxt.
 *
 * Return value: a new #HwpToTxt
 *
 * Since: 0.0.1
 */
HwpToTxt *hwp_to_txt_new ()
{
  return g_object_new (HWP_TYPE_TO_TXT, NULL);
}

static void hwp_to_txt_init (HwpToTxt *hwp_to_txt)
{
}

static void hwp_to_txt_finalize (GObject *object)
{
  HwpToTxt *hwp2txt = HWP_TO_TXT (object);
  if (hwp2txt->output_stream)
    g_object_unref (hwp2txt->output_stream);

  G_OBJECT_CLASS (hwp_to_txt_parent_class)->finalize (object);
}

static void hwp_to_txt_class_init (HwpToTxtClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hwp_to_txt_finalize;
}

/* callback */
void extract_text (HwpListener  *listener,
                   HwpParagraph *paragraph,
                   gpointer      user_data,
                   GError      **error)
{
  HwpToTxt *hwp2txt = HWP_TO_TXT (listener);

  GString *string = paragraph->string;
  if (string) {
    if (hwp2txt->output_stream) {
      g_output_stream_write (hwp2txt->output_stream, string->str, string->len,
                             NULL, error);
      gchar lf = '\n';
      g_output_stream_write (hwp2txt->output_stream, &lf, 1, NULL, error);
    } else {
      printf ("%s\n", string->str);
    }
  }
}

static void hwp_to_txt_iface_init (HwpListenerInterface *iface)
{
  iface->paragraph = extract_text;
}

void hwp_to_txt_convert (HwpToTxt *hwp2txt,
                         char     *in_filename,
                         char     *out_filename,
                         GError  **error)
{
  HwpHWP5File *hwpfile = hwp_hwp5_file_new_for_path (in_filename, error);

  if (*error)
    return;

  if (out_filename) {
    GFile *file = g_file_new_for_path (out_filename);
    hwp2txt->output_stream =
      (GOutputStream *) g_file_create (file, G_FILE_CREATE_NONE, NULL, error);

    g_object_unref (file);

    if (*error)
      return;
  }

  HwpHWP5Parser *parser;
  parser = hwp_hwp5_parser_new (HWP_LISTENER (hwp2txt), NULL);
  hwp_hwp5_parser_parse (parser, HWP_HWP5_FILE (hwpfile), error);
  g_object_unref (hwpfile);
  g_object_unref (parser);
}
/*****************************************************************************/

/** main **/
int main (int argc, char *argv[])
{
  char **in_filenames = NULL;
  char  *out_filename = NULL;

  GOptionEntry entries[] =
  {
    { "output",         'o', 0, G_OPTION_ARG_FILENAME,       &out_filename,
      "output txt file", "TEXT_FILE"},
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &in_filenames,
      NULL,              "HWP_FILE" },
    {NULL}
  };

  GError  *error = NULL;
  GOptionContext *context;
  HwpPage *page;

#if (!GLIB_CHECK_VERSION(2, 35, 0))
  g_type_init();
#endif

  context = g_option_context_new (NULL);
  g_option_context_set_summary (context, "Convert hwp file to text file");
  g_option_context_add_main_entries (context, entries, NULL);

  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    g_print ("option parsing failed: %s\n", error->message);
    g_option_context_free (context);
    return 1;
  }

  if (!in_filenames)
    goto CATCH;

  int count = 0;
  while (in_filenames[count])
  { count++; }

  if (count != 1)
    goto CATCH;

  g_option_context_free (context);

  /* main procedure */
  HwpToTxt *hwp2txt = hwp_to_txt_new ();
  hwp_to_txt_convert (hwp2txt, in_filenames[0], out_filename, &error);
  g_object_unref (hwp2txt);

  g_strfreev (in_filenames);
  g_free (out_filename);

  if (error) {
    fprintf (stderr, "%s\n", error->message);
    return 1;
  }

  return 0;

  CATCH:
  {
    char *help_msg = g_option_context_get_help (context, FALSE, NULL);
    printf ("%s", help_msg);
    g_free (help_msg);
    g_option_context_free (context);
    return 1;
  }
}

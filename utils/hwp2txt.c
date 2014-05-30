/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp2txt.c
 *
 * Copyright (C) 2014 Hodong Kim <hodong@cogno.org>
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <glib-object.h>
#include "hwp.h"

/* HwpToTxt class ***********************************************************/
#define HWP_TYPE_TO_TXT             (hwp_to_txt_get_type ())
#define HWP_TO_TXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TO_TXT, HwpToTxt))
#define HWP_TO_TXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TO_TXT, HwpToTxtClass))
#define HWP_IS_TO_TXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TO_TXT))
#define HWP_IS_TO_TXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TO_TXT))
#define HWP_TO_TXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TO_TXT, HwpToTxtClass))

typedef struct _HwpToTxt      HwpToTxt;
typedef struct _HwpToTxtClass HwpToTxtClass;

struct _HwpToTxtClass
{
  GObjectClass parent_class;
};

struct _HwpToTxt
{
  GObject        parent_instance;
  GOutputStream *output_stream;
};

GType hwp_to_txt_get_type (void) G_GNUC_CONST;

static void hwp_to_txt_iface_init (HwpListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HwpToTxt, hwp_to_txt, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (HWP_TYPE_LISTENER, hwp_to_txt_iface_init))

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

HwpToTxt *hwp_to_txt_new ()
{
  return g_object_new (HWP_TYPE_TO_TXT, NULL);
}

void hwp_to_txt_convert (HwpToTxt  *hwp2txt,
                         char      *in_filename,
                         char      *out_filename,
                         GError   **error)
{
  HwpFile *hwpfile = hwp_file_new_for_path (in_filename, error);

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

  HwpParser *parser = hwp_parser_new (HWP_LISTENER (hwp2txt), NULL);
  hwp_parser_parse (parser, hwpfile, error);
  g_object_unref (parser);
  g_object_unref (hwpfile);
}

void listen_paragraph (HwpListener   *listener,
                       HwpParagraph  *paragraph,
                       gpointer       user_data,
                       GError       **error)
{
  HwpToTxt *hwp2txt = HWP_TO_TXT (listener);

  const gchar *text = hwp_paragraph_get_text (paragraph);
  if (text == NULL)
    text = "\n";

  if (hwp2txt->output_stream)
  {
    g_output_stream_write (hwp2txt->output_stream,
                           text, strlen (text), NULL, error);
    g_output_stream_write (hwp2txt->output_stream,
                           (gchar *) '\n', 1, NULL, error);
  }
  else
  {
    printf ("%s\n", text);
  }
}

static void hwp_to_txt_iface_init (HwpListenerInterface *iface)
{
  iface->paragraph = listen_paragraph;
}

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

#if (!GLIB_CHECK_VERSION(2, 35, 0))
  g_type_init();
#endif

  context = g_option_context_new (NULL);
  g_option_context_set_summary (context, "Convert hwp file to text file");
  g_option_context_add_main_entries (context, entries, NULL);
  g_slice_free (GOptionEntry, entries);

  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    fprintf (stderr, "option parsing failed: %s\n", error->message);
    g_option_context_free (context);
    goto FAIL;
  }

  if (!in_filenames)
  {
    char *help_msg = g_option_context_get_help (context, FALSE, NULL);
    printf ("%s", help_msg);
    g_free (help_msg);
    goto FAIL;
  }

  int count = 0;
  while (in_filenames[count])
  { count++; }

  if (count != 1)
  {
    char *help_msg = g_option_context_get_help (context, FALSE, NULL);
    printf ("%s", help_msg);
    g_free (help_msg);
    goto FAIL;
  }

  g_option_context_free (context);

  if (g_file_test (out_filename, G_FILE_TEST_EXISTS))
  {
    fprintf (stderr, "%s file exist\n", out_filename);
    goto FAIL;
  }

  HwpToTxt *hwp2txt = hwp_to_txt_new ();
  hwp_to_txt_convert (hwp2txt, in_filenames[0], out_filename, &error);
  g_object_unref (hwp2txt);

  if (error)
  {
    fprintf (stderr, "%s\n", error->message);
    goto FAIL;
  }

  g_strfreev (in_filenames);
  g_free (out_filename);

  return 0;

  FAIL:

  g_clear_error (&error);
  g_strfreev (in_filenames);
  g_free (out_filename);
  return 1;
}

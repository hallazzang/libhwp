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
#include "hwp-txt-converter.h"

void convert(char *in_filename, char *out_filename, GError **error)
{
  HwpHWP5File *file = hwp_hwp5_file_new_for_path (in_filename, error);

  if (*error)
    g_error ("%s", (*error)->message);

  HwpTxtConverter *converter;
  HwpHWP5Parser   *parser;
  converter = hwp_txt_converter_new ();
  parser = hwp_hwp5_parser_new (HWP_LISTENER (converter), NULL);
  hwp_hwp5_parser_parse (parser, HWP_HWP5_FILE (file), error);
  g_object_unref (file);
  g_object_unref (parser);

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

  if (in_filenames) {
    int count = 0;
    for (count = 0; in_filenames[count]; count++)
    {}

    if (count == 1) {
      g_option_context_free (context);

      convert(in_filenames[0], out_filename, &error);
      return 0;
    }
  }

  char *help_msg = NULL;
  help_msg = g_option_context_get_help (context, FALSE, NULL);
  printf ("%s", help_msg);
  g_option_context_free (context);
  return 1;
}
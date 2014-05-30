/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp2svg.c
 *
 * Copyright (C) 2013-2014 Hodong Kim <hodong@cogno.org>
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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <glib-object.h>
#include <cairo-svg.h>
#include "hwp.h"

int main (int argc, char **argv)
{
  char **in_filenames = NULL;
  char  *out_filename = NULL;

  GOptionEntry entries[] =
  {
    { "output",         'o', 0, G_OPTION_ARG_FILENAME,       &out_filename,
      "output directory", "SVG_DIRECTORY"},
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
  g_option_context_set_summary (context, "Convert hwp file to svg files");
  g_option_context_add_main_entries (context, entries, NULL);
  g_slice_free (GOptionEntry, entries);

  if (!g_option_context_parse (context, &argc, &argv, &error))
  {
    g_print ("option parsing failed: %s\n", error->message);
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

  if (!out_filename)
  {
    char *p = NULL;
    /* basename 은 확장자를 포함합니다. */
    char *basename = g_path_get_basename (in_filenames[0]);

    if ((p = rindex (basename, '.'))) {
      int len = strlen (basename) - strlen (p);
      /* filebase 는 확장자를 포함하지 않습니다. */
      char *filebase = g_strndup (basename, len);
      out_filename = g_strconcat (filebase, "_FILES", NULL);
      g_free (filebase);
    } else {
      out_filename = g_strconcat (basename, "_FILES", NULL);
    }
    g_free (basename);
  }

  if (g_file_test (out_filename, G_FILE_TEST_EXISTS)) {
    fprintf (stderr, "%s file exist\n", out_filename);
    goto FAIL;
  }

  HwpPage *page;
  gdouble  width = 0.0, height = 0.0;
  HwpFile *file = hwp_file_new_for_path (in_filenames[0], &error);

  if (error)
  {
    fprintf (stderr, "%s\n", error->message);
    goto FAIL;
  }

  HwpDocument *document = hwp_file_get_document (file, &error);
  g_object_unref (file);

  if (document == NULL)
  {
    fprintf (stderr, "%s: invalid hwp file\n", in_filenames[0]);
    goto FAIL;
  }

  guint n_pages = hwp_document_get_n_pages (document);

  if (n_pages < 1) {
    fprintf (stderr, "There is no page");
    g_object_unref (document);
    goto FAIL;
  }

  mkdir (out_filename, S_IRWXU);
  cairo_surface_t *surface;
  cairo_t *cr;

  for (guint i = 0; i < n_pages; i++)
  {
    page = hwp_document_get_page (document, i);
    hwp_page_get_size (page, &width, &height);
    char *filename = g_strdup_printf ("%s/%d.svg", out_filename, i);
    surface = cairo_svg_surface_create (filename, width, height);
    g_free (filename);
    cr = cairo_create (surface);

    hwp_page_render (page, cr);
    g_object_unref (page);
    cairo_show_page (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
  }

  g_object_unref (document);
  g_strfreev (in_filenames);
  g_free (out_filename);

  return 0;

  FAIL:

  g_clear_error (&error);
  g_strfreev (in_filenames);
  g_free (out_filename);
  return 1;
}

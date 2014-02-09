/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp2svg.c
 * 
 * Copyright (C) 2013-2014 Hodong Kim <hodong@cogno.org>
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

/* TODO */
/* output directory and output name */

#include <stdio.h>
#include <glib-object.h>
#include <cairo-svg.h>
#include "hwp.h"

void convert(char *in_filename, char *out_filename, GError **error)
{
  HwpPage *page;
  gdouble  width = 0.0, height = 0.0;
  HwpFile     *file     = hwp_file_new_for_path (in_filename, error);
  HwpDocument *document = hwp_file_get_document (file,    error);

  if (*error)
    g_error ("%s", (*error)->message);

  guint n_pages = hwp_document_get_n_pages (document);

  if (n_pages < 1) {
    g_set_error_literal (error,
                         HWP_FILE_ERROR,
                         HWP_FILE_ERROR_INVALID,
                         "There is no page");
    return;
  }

  cairo_surface_t *surface;
  cairo_t *cr;
  char *filename;

  for (guint i = 0; i < n_pages; i++) {
    hwp_page_get_size (page, &width, &height);
    filename = g_strdup_printf("page%d.svg", i);
    surface = cairo_svg_surface_create (filename, width, height);
    cr = cairo_create (surface);

    page = hwp_document_get_page (document, i);
    hwp_render_page (cr, page);
    cairo_show_page (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
  }

  g_object_unref (document);
  g_object_unref (file);
}

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

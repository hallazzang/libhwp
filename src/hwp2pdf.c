/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp2pdf.c
 * 
 * Copyright (C) 2013 Hodong Kim <cogniti@gmail.com>
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
#include <cairo-pdf.h>
#include "ghwp.h"

int main (int argc, char **argv)
{
    GError *error = NULL;
    GHWPPage *page;
    gdouble width = 0.0, height = 0.0;
    
    if (argc < 3) {
        puts ("Usage: hwp2pdf file.hwp file.pdf");
        return -1;
    }

#if (!GLIB_CHECK_VERSION(2, 35, 0))
    g_type_init();
#endif

    GHWPFile *file = ghwp_file_new_from_filename (argv[1], &error);

    if (error)
        g_error ("%s", error->message);

    GHWPDocument *document = ghwp_file_get_document (file, &error);

    guint n_pages = ghwp_document_get_n_pages (document);

    if (n_pages < 1) {
        puts ("There is no page");
        return -1;
    }

    cairo_surface_t *surface = cairo_pdf_surface_create (argv[2], 0.0, 0.0);
    cairo_t *cr = cairo_create (surface);

    for (guint i = 0; i < n_pages; i++) {
        page = ghwp_document_get_page (document, i);
        ghwp_page_get_size (page, &width, &height);
        cairo_pdf_surface_set_size (surface, width, height);
        ghwp_page_render (page, cr);
        cairo_show_page (cr);
    }

    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    g_object_unref (document);
    g_object_unref (file);
    return 0;
}

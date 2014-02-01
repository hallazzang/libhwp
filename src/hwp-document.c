/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-document.c
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include <string.h>
#include <stdio.h>

#include "hwp-document.h"
#include "hwp-file.h"
#include "hwp-models.h"
#include "hwp-listener.h"
#include <pango/pangocairo.h>

static void hwp_document_listener_iface_init (HwpListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HwpDocument, hwp_document, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (HWP_TYPE_LISTENER, hwp_document_listener_iface_init))

/**
 * hwp_document_new_from_file:
 * @filename: the path of a file, in the GLib filename encoding
 * @error: location to store the error occurring, or %NULL to ignore
 * 
 * Creates a new #HwpDocument.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #G_FILE_ERROR
 * domains.
 * 
 * Returns: a new #HwpDocument, or %NULL
 *
 * Since: 0.0.1
 */
HwpDocument *hwp_document_new_from_file (const gchar *filename, GError **error)
{
  g_return_val_if_fail (filename != NULL, NULL);

  HwpFile *file = hwp_file_new_for_path (filename, error);

  if (file == NULL || *error) {
    return NULL;
  }

  return hwp_file_get_document (file, error);
}

/**
 * hwp_document_get_n_pages:
 * @document: a #HwpDocument
 *
 * Gets the number of pages in a notebook.
 *
 * Return value: the number of pages in the document
 *
 * Since: 0.0.1
 */
guint hwp_document_get_n_pages (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), 0U);
    return document->pages->len;
}

/**
 * hwp_document_get_page:
 * @document: a #HwpDocument
 * @n_page: the index of the page to get
 *
 * Returns a #HwpPage representing the page at index
 *
 * Returns: (transfer none): a #HwpPage
 *     DO NOT FREE the page.
 *
 * Since: 0.0.1
 */
HwpPage *hwp_document_get_page (HwpDocument *doc, gint n_page)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (doc), NULL);
    HwpPage *page = g_array_index (doc->pages, HwpPage *, (guint) n_page);
    return g_object_ref (page);
}

/**
 * hwp_document_new:
 * 
 * Creates a new #HwpDocument.
 * 
 * Return value: a new #HwpDocument
 *
 * Since: 0.0.1
 */
HwpDocument *hwp_document_new (void)
{
    return (HwpDocument*) g_object_new (HWP_TYPE_DOCUMENT, NULL);
}

/**
 * hwp_document_get_title:
 * @document: A #HwpDocument
 *
 * Returns the document's title
 *
 * Return value: a new allocated string containing the title
 *               of @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_title (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->title);
}

/**
 * hwp_document_get_keywords:
 * @document: A #HwpDocument
 *
 * Returns the keywords associated to the document
 *
 * Return value: a new allocated string containing keywords associated
 *               to @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_keywords (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->keywords);
}

/**
 * hwp_document_get_subject:
 * @document: A #HwpDocument
 *
 * Returns the subject of the document
 *
 * Return value: a new allocated string containing the subject
 *               of @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_subject (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->subject);
}

/**
 * hwp_document_get_creator:
 * @document: A #HwpDocument
 *
 * Returns the creator of the document.
 *
 * Return value: a new allocated string containing the creator
 *               of @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_creator (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->creator);
}

/**
 * hwp_document_get_creation_date:
 * @document: A #HwpDocument
 *
 * Returns the date the document was created as seconds since the Epoch
 *
 * Return value: the date the document was created, or -1
 *
 * Since: 0.0.1
 */
GTime hwp_document_get_creation_date (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), (GTime)-1);
    return document->creation_date;
}

/**
 * hwp_document_get_modification_date:
 * @document: A #HwpDocument
 *
 * Returns the date the document was most recently modified as seconds since the Epoch
 *
 * Return value: the date the document was most recently modified, or -1
 *
 * Since: 0.0.1
 */
GTime hwp_document_get_modification_date (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), (GTime)-1);
    return document->mod_date;
}

/**
 * hwp_document_get_hwp_format:
 * @document: A #HwpDocument
 *
 * Returns the HWP format of @document as a string (e.g. HWP v5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP format
 *               of @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_format (HwpDocument *document)
{
    gchar *format;

    g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);

    format = g_strdup_printf ("HWP v%s",
        hwp_document_get_hwp_version_string (document));
    return format;
}

/**
 * hwp_document_get_hwp_version_string:
 * @document: A #HwpDocument
 *
 * Returns the HWP version of @document as a string (e.g. 5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP version
 *               of @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_hwp_version_string (HwpDocument *document)
{
    g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);
    return g_strdup_printf ("%d.%d.%d.%d", document->major_version,
                                           document->minor_version,
                                           document->micro_version,
                                           document->extra_version);
}

/**
 * hwp_document_get_hwp_version:
 * @document: A #HwpDocument
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
 *
 * Returns: the major and minor and micro and extra HWP version numbers
 *
 * Since: 0.0.1
 */
void hwp_document_get_hwp_version (HwpDocument *document,
                                   guint8      *major_version,
                                   guint8      *minor_version,
                                   guint8      *micro_version,
                                   guint8      *extra_version)
{
  g_return_if_fail (HWP_IS_DOCUMENT (document));
  if (major_version) *major_version = document->major_version;
  if (minor_version) *minor_version = document->minor_version;
  if (micro_version) *micro_version = document->micro_version;
  if (extra_version) *extra_version = document->extra_version;
}

static void hwp_document_finalize (GObject *obj)
{
  HwpDocument *doc = HWP_DOCUMENT(obj);
  g_free (doc->prv_text);
  g_array_free (doc->paragraphs, TRUE);
  g_array_free (doc->pages, TRUE);

  G_OBJECT_CLASS (hwp_document_parent_class)->finalize (obj);
}

static void hwp_document_class_init (HwpDocumentClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_document_finalize;
}

static void hwp_document_init (HwpDocument *doc)
{
  doc->paragraphs = g_array_new (TRUE, TRUE, sizeof (HwpParagraph *));
  doc->pages      = g_array_new (TRUE, TRUE, sizeof (HwpPage *));
}

void document_version (HwpListener *listener,
                       guint8       major_version,
                       guint8       minor_version,
                       guint8       micro_version,
                       guint8       extra_version,
                       gpointer     user_data,
                       GError     **error)
{
  HwpDocument *document   = (HwpDocument *) listener;
  document->major_version = major_version;
  document->minor_version = minor_version;
  document->micro_version = micro_version;
  document->extra_version = extra_version;
}

void hwp_document_paginate (HwpDocument *document, PangoLayout *layout)
{
  /* TODO */
}

void object (HwpListener *listener,
             GObject     *object,
             gpointer     user_data,
             GError     **error)
{
  if (HWP_IS_PARAGRAPH (object)) {
    HwpParagraph *paragraph = HWP_PARAGRAPH (object);
    HwpText *hwp_text = hwp_paragraph_get_hwp_text (paragraph);
    HwpDocument *document  = (HwpDocument *) listener;
    if (hwp_text) {
      printf("%s\n", hwp_text->text);
      PangoFontMap *fontmap = pango_cairo_font_map_get_default ();
      PangoContext *context = pango_font_map_create_context (fontmap);
      PangoLayout *layout = pango_layout_new (context);
      pango_layout_set_width (layout, 595 * PANGO_SCALE);
      pango_layout_set_wrap  (layout, PANGO_WRAP_WORD_CHAR);
      pango_layout_set_text (layout, hwp_text->text, -1);
      hwp_document_paginate (document, layout);
    }
  }
}

static void hwp_document_listener_iface_init (HwpListenerInterface *iface)
{
  iface->document_version = document_version;
  iface->object           = object;
}

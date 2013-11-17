/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * ghwp-document.c
 *
 * Copyright (C) 2012-2013 Hodong Kim <cogniti@gmail.com>
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

#include "ghwp-document.h"
#include "ghwp-file.h"
#include "ghwp-models.h"
#include "ghwp-listener.h"
#include <pango/pangocairo.h>

static void ghwp_document_listener_iface_init (GHWPListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GHWPDocument, ghwp_document, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (GHWP_TYPE_LISTENER,ghwp_document_listener_iface_init))

/**
 * ghwp_document_new_from_uri:
 * @uri: uri of the file to load
 * @error: (allow-none): Return location for an error, or %NULL
 * 
 * Creates a new #GHWPDocument.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #GHWP_ERROR and #G_FILE_ERROR
 * domains.
 * 
 * Return value: A newly created #GHWPDocument, or %NULL
 *
 * Since: 0.1
 */
GHWPDocument *ghwp_document_new_from_uri (const gchar *uri, GError **error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar        *filename = g_filename_from_uri (uri, NULL, error);
    GHWPDocument *document = ghwp_document_new_from_filename (filename, error);
    g_free (filename);
    return document;
}

/**
 * Since: 0.1
 */
GHWPDocument *
ghwp_document_new_from_filename (const gchar *filename, GError **error)
{
    g_return_val_if_fail (filename != NULL, NULL);

    GHWPFile *file = ghwp_file_new_from_filename (filename, error);

    if (file == NULL || *error) {
        return NULL;
    }

    return ghwp_file_get_document (file, error);
}

/**
 * Since: 0.1
 */
guint ghwp_document_get_n_pages (GHWPDocument *doc)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (doc), 0U);
    return doc->pages->len;
}

/**
 * ghwp_document_get_page:
 * @doc: a #GHWPDocument
 * @n_page: the index of the page to get
 *
 * Returns a #GHWPPage representing the page at index
 *
 * Returns: (transfer none): a #GHWPPage
 *     DO NOT FREE the page.
 *
 * Since: 0.1
 */
GHWPPage *ghwp_document_get_page (GHWPDocument *doc, gint n_page)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (doc), NULL);
    GHWPPage *page = g_array_index (doc->pages, GHWPPage *, (guint) n_page);
    return g_object_ref (page);
}

/**
 * ghwp_document_new:
 * 
 * Creates a new #GHWPDocument.
 * 
 * Return value: A newly created #GHWPDocument
 *
 * Since: 0.1
 */
GHWPDocument *ghwp_document_new (void)
{
    return (GHWPDocument*) g_object_new (GHWP_TYPE_DOCUMENT, NULL);
}

/**
 * ghwp_document_get_title:
 * @document: A #GHWPDocument
 *
 * Returns the document's title
 *
 * Return value: a new allocated string containing the title
 *               of @document, or %NULL
 *
 * Since: 0.2
 */
gchar *ghwp_document_get_title (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->title);
}

/**
 * ghwp_document_get_keywords:
 * @document: A #GHWPDocument
 *
 * Returns the keywords associated to the document
 *
 * Return value: a new allocated string containing keywords associated
 *               to @document, or %NULL
 *
 * Since: 0.2
 */
gchar *ghwp_document_get_keywords (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->keywords);
}

/**
 * ghwp_document_get_subject:
 * @document: A #GHWPDocument
 *
 * Returns the subject of the document
 *
 * Return value: a new allocated string containing the subject
 *               of @document, or %NULL
 *
 * Since: 0.2
 */
gchar *ghwp_document_get_subject (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->subject);
}

/**
 * ghwp_document_get_creator:
 * @document: A #GHWPDocument
 *
 * Returns the creator of the document.
 *
 * Return value: a new allocated string containing the creator
 *               of @document, or %NULL
 *
 * Since: 0.2
 */
gchar *ghwp_document_get_creator (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);
    return g_strdup (document->creator);
}

/**
 * ghwp_document_get_creation_date:
 * @document: A #GHWPDocument
 *
 * Returns the date the document was created as seconds since the Epoch
 *
 * Return value: the date the document was created, or -1
 *
 * Since: 0.2
 */
GTime ghwp_document_get_creation_date (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), (GTime)-1);
    return document->creation_date;
}

/**
 * ghwp_document_get_modification_date:
 * @document: A #GHWPDocument
 *
 * Returns the date the document was most recently modified as seconds since the Epoch
 *
 * Return value: the date the document was most recently modified, or -1
 *
 * Since: 0.2
 */
GTime ghwp_document_get_modification_date (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), (GTime)-1);
    return document->mod_date;
}

/**
 * ghwp_document_get_hwp_format:
 * @document: A #GHWPDocument
 *
 * Returns the HWP format of @document as a string (e.g. HWP v5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP format
 *               of @document, or %NULL
 *
 * Since: 0.2
 */
gchar *ghwp_document_get_format (GHWPDocument *document)
{
    gchar *format;

    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);

    format = g_strdup_printf ("HWP v%s",
        ghwp_document_get_hwp_version_string (document));
    return format;
}

/**
 * ghwp_document_get_hwp_version_string:
 * @document: A #GHWPDocument
 *
 * Returns the HWP version of @document as a string (e.g. 5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP version
 *               of @document, or %NULL
 *
 * Since: 0.2
 */
gchar *ghwp_document_get_hwp_version_string (GHWPDocument *document)
{
    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);
    return g_strdup_printf ("%d.%d.%d.%d", document->major_version,
                                           document->minor_version,
                                           document->micro_version,
                                           document->extra_version);
}

/**
 * ghwp_document_get_hwp_version:
 * @document: A #GHWPDocument
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
 *
 * Returns: the major and minor and micro and extra HWP version numbers
 *
 * Since: 0.2
 */
void ghwp_document_get_hwp_version (GHWPDocument *document,
                                    guint8       *major_version,
                                    guint8       *minor_version,
                                    guint8       *micro_version,
                                    guint8       *extra_version)
{
    g_return_if_fail (GHWP_IS_DOCUMENT (document));
    if (major_version) *major_version = document->major_version;
    if (minor_version) *minor_version = document->minor_version;
    if (micro_version) *micro_version = document->micro_version;
    if (extra_version) *extra_version = document->extra_version;
}

static void ghwp_document_finalize (GObject *obj)
{
    GHWPDocument *doc = GHWP_DOCUMENT(obj);
    g_free (doc->prv_text);
    g_array_free (doc->paragraphs, TRUE);
    g_array_free (doc->pages, TRUE);
    g_object_unref (doc->summary_info);
    G_OBJECT_CLASS (ghwp_document_parent_class)->finalize (obj);
}

static void ghwp_document_class_init (GHWPDocumentClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_document_finalize;
}

static void ghwp_document_init (GHWPDocument *doc)
{
    doc->paragraphs = g_array_new (TRUE, TRUE, sizeof (GHWPParagraph *));
    doc->pages      = g_array_new (TRUE, TRUE, sizeof (GHWPPage *));
}

void listen_document_version (GHWPListener *listener,
                              guint8        major_version,
                              guint8        minor_version,
                              guint8        micro_version,
                              guint8        extra_version,
                              gpointer      user_data,
                              GError      **error)
{
  GHWPDocument *document  = (GHWPDocument *) listener;
  document->major_version = major_version;
  document->minor_version = minor_version;
  document->micro_version = micro_version;
  document->extra_version = extra_version;
}

void ghwp_document_paginate (GHWPDocument *document, PangoLayout *layout)
{
  /* TODO */
}

void listen_object (GHWPListener *listener,
                    GObject      *object,
                    gpointer      user_data,
                    GError      **error)
{
  if (GHWP_IS_PARAGRAPH (object)) {
    GHWPParagraph *paragraph = GHWP_PARAGRAPH (object);
    GHWPText *ghwp_text = ghwp_paragraph_get_ghwp_text (paragraph);
    GHWPDocument *document  = (GHWPDocument *) listener;
    if (ghwp_text) {
      printf("%s\n", ghwp_text->text);
      PangoFontMap *fontmap = pango_cairo_font_map_get_default ();
      PangoContext *context = pango_font_map_create_context (fontmap);
      PangoLayout *layout = pango_layout_new (context);
      pango_layout_set_width (layout, 595 * PANGO_SCALE);
      pango_layout_set_wrap  (layout, PANGO_WRAP_WORD_CHAR);
      pango_layout_set_text (layout, ghwp_text->text, -1);
      ghwp_document_paginate (document, layout);
    }
  }
}

static void ghwp_document_listener_iface_init (GHWPListenerInterface *iface)
{
  iface->document_version = listen_document_version;
  iface->object           = listen_object;
}

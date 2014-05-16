/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-document.c
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
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
#include <math.h>
#include <pango/pango.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

static void hwp_document_listener_iface_init (HwpListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HwpDocument, hwp_document, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (HWP_TYPE_LISTENER, hwp_document_listener_iface_init))

static cairo_status_t hwp_document_write_pdf_to_mem (void *closure,
                                                     const unsigned char *data,
                                                     unsigned int length)
{
  GByteArray *pdf_data = HWP_DOCUMENT (closure)->pdf_data;
  g_byte_array_append (pdf_data, data, length);

  return CAIRO_STATUS_SUCCESS;
}

static void hwp_document_create_poppler_document (HwpDocument *document)
{
  g_return_if_fail (document->paragraphs->len > 0);

  HwpParagraph *paragraph0 = g_ptr_array_index (document->paragraphs, 0);
  HwpParagraph *paragraph = NULL;
  guint8 major_version;
  hwp_document_get_hwp_version (document, &major_version, NULL, NULL, NULL);
  /* 아직 secd 작업하지 못한 hwp3, hwp ml 문서를 위해 */
  if (paragraph0->secd == NULL)
    paragraph0->secd = hwp_secd_new ();

  HwpSecd *secd = paragraph0->secd;

  gdouble x0 = secd->page_left_margin_in_points;
  gdouble y0 = secd->page_top_margin_in_points;

  cairo_surface_t *surface;
  surface = cairo_pdf_surface_create_for_stream (hwp_document_write_pdf_to_mem,
                                                 document,
                                                 secd->page_width_in_points,
                                                 secd->page_height_in_points);

  cairo_t *cr = cairo_create (surface);
  PangoLayout *layout = pango_cairo_create_layout (cr);

  pango_layout_set_width (layout,
                          (secd->page_width_in_points -
                           secd->page_left_margin_in_points -
                           secd->page_right_margin_in_points) * PANGO_SCALE);

  pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
  pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);

  cairo_move_to (cr, x0, y0);

  for (guint i = 0; i < document->paragraphs->len; i++)
  {
    paragraph = g_ptr_array_index (document->paragraphs, i);
    PangoAttrList *attrs = pango_attr_list_new ();

    for (guint j = 0; j < paragraph->m_len; j++)
    {
      HwpCharShape *char_shape = g_ptr_array_index (document->char_shapes,
                                                    paragraph->m_id[j]);
      gdouble font_size_in_points = char_shape->height_in_points;

      HwpFaceName *hwp_face_name = g_ptr_array_index (document->face_names,
                                                      char_shape->face_id[0]); /* 한국어의 경우 0 */
      gchar *face_name = hwp_face_name->font_name;

      HwpTextAttributes *text_attrs = g_ptr_array_index (paragraph->text_attrs, j);
      PangoAttribute *family = pango_attr_family_new (face_name);
      family->start_index = text_attrs->start_index;
      family->end_index   = text_attrs->end_index;

      PangoAttribute *size;
      size = pango_attr_size_new (font_size_in_points * PANGO_SCALE);
      size->start_index = text_attrs->start_index;
      size->end_index = text_attrs->end_index;

      pango_attr_list_insert (attrs, family);
      pango_attr_list_insert (attrs, size);
    } /* for (guint j = 0; j < paragraph->m_len; j++) */

    if (paragraph->text)
      pango_layout_set_text (layout, paragraph->text, -1);
    else
      continue;

    /* hwp5의 경우 attrs 적용합니다. */
    if (major_version == 5)
      pango_layout_set_attributes (layout, attrs);

    pango_attr_list_unref (attrs);

    pango_cairo_update_layout (cr, layout);
    PangoLayoutIter *iter = pango_layout_get_iter (layout);

    do {
      PangoRectangle ink_rect;
      PangoRectangle logical_rect;
      double x = 0.0, y = 0.0;

      PangoLayoutLine *line = pango_layout_iter_get_line_readonly (iter);
      pango_layout_iter_get_line_extents (iter, &ink_rect, &logical_rect);
      cairo_get_current_point (cr, &x, &y);

      if (y + logical_rect.height / PANGO_SCALE >=
          secd->page_height_in_points -
          secd->page_bottom_margin_in_points -
          secd->page_footer_margin_in_points)
      {
        cairo_show_page (cr);
        cairo_move_to (cr, x0, y0);
      }

      cairo_rel_move_to (cr, 0, logical_rect.height / PANGO_SCALE);
      pango_cairo_show_layout_line (cr, line);
    } while (pango_layout_iter_next_line (iter));

    pango_layout_iter_free (iter);
  } /* for (guint j = 0; j < paragraph->m_len; j++) */

  g_object_unref (layout);
  cairo_destroy (cr);
  cairo_surface_finish (surface);
  cairo_surface_destroy (surface);

  document->poppler_document =
    poppler_document_new_from_data ((char *) document->pdf_data->data,
                                    document->pdf_data->len,
                                    NULL, NULL);
}

/**
 * hwp_document_new_from_file:
 * @filename: the path of a file, in the GLib filename encoding
 * @error: location to store the error occurring, or %NULL to ignore
 * 
 * Creates a new #HwpDocument.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #HWP_FILE_ERROR
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

  if (!file)
    return NULL;

  HwpDocument *document = hwp_file_get_document (file, error);
  g_object_unref (file);

  if (!document)
    return NULL;

  return document;
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
  g_return_val_if_fail (HWP_IS_DOCUMENT (document), 0);

  if (!document->poppler_document)
    hwp_document_create_poppler_document (document);

  return poppler_document_get_n_pages (document->poppler_document);
}

/**
 * hwp_document_get_page:
 * @document: a #HwpDocument
 * @n_page: the index of the page to get
 *
 * Returns a #HwpPage representing the page at index
 *
 * Returns: (transfer full): a new #HwpPage or %NULL on error.
 * Free the returned object with g_object_unref ().
 *
 * Since: 0.0.1
 */

HwpPage *hwp_document_get_page (HwpDocument *document, gint n_page)
{
  g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);

  if (!document->poppler_document)
    hwp_document_create_poppler_document (document);

  HwpPage *page = hwp_page_new ();
  page->poppler_page = poppler_document_get_page (document->poppler_document,
                                                  n_page);
  return page;
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
  return g_object_new (HWP_TYPE_DOCUMENT, NULL);
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

  return document->info ? g_strdup (document->info->title) : NULL;
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

  return document->info ? g_strdup (document->info->keywords) : NULL;
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

  return document->info ? g_strdup (document->info->subject) : NULL;
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

  return document->info ? g_strdup (document->info->creator) : NULL;
}

/**
 * hwp_document_get_creation_date:
 * @document: A #HwpDocument
 *
 * Returns the date the document was created as seconds since the Epoch
 *
 * Return value: the date the document was created, or 0
 *
 * Since: 0.0.1
 */
GTime hwp_document_get_creation_date (HwpDocument *document)
{
  g_return_val_if_fail (HWP_IS_DOCUMENT (document), (GTime) 0);

  return document->info ? document->info->creation_date : 0;
}

/**
 * hwp_document_get_modification_date:
 * @document: A #HwpDocument
 *
 * Returns the date the document was most recently modified as seconds since the Epoch
 *
 * Return value: the date the document was most recently modified, or 0
 *
 * Since: 0.0.1
 */
GTime hwp_document_get_modification_date (HwpDocument *document)
{
  g_return_val_if_fail (HWP_IS_DOCUMENT (document), (GTime) 0);

  return document->info ? document->info->mod_date : 0;
}

/**
 * hwp_document_get_format:
 * @document: A #HwpDocument
 *
 * Returns the HWP format of @document as a string (e.g. HWP 5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP format
 *               of @document, or %NULL
 *
 * Since: 0.0.1
 */
gchar *hwp_document_get_format (HwpDocument *document)
{
  g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);

  return g_strdup_printf ("HWP %s",
                          hwp_document_get_hwp_version_string (document));
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

static void hwp_document_finalize (GObject *object)
{
  HwpDocument *document = HWP_DOCUMENT(object);

  g_ptr_array_free (document->char_shapes, TRUE);
  g_ptr_array_free (document->face_names, TRUE);
  g_ptr_array_free (document->paragraphs, TRUE);
  g_free (document->prv_text);
  g_object_unref (document->info);
  g_byte_array_free (document->pdf_data, TRUE);

  G_OBJECT_CLASS (hwp_document_parent_class)->finalize (object);
}

static void hwp_document_class_init (HwpDocumentClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_document_finalize;
}

static void hwp_document_init (HwpDocument *document)
{
  document->pdf_data = g_byte_array_new ();

  document->char_shapes =
    g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_char_shape_free);

  document->face_names =
    g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_face_name_free);

  document->paragraphs = g_ptr_array_new_with_free_func (g_object_unref);
}

/* callback */
static void hwp_document_listen_version (HwpListener *listener,
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

/**
 * hwp_document_add_paragraph:
 * @document: A #HwpDocument
 * @paragraph: A #HwpParagraph
 *
 * Since: 0.0.1
 */
void hwp_document_add_paragraph (HwpDocument *document, HwpParagraph *paragraph)
{
  g_return_if_fail (HWP_IS_DOCUMENT  (document));
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));

  g_ptr_array_add (document->paragraphs, paragraph);
}

/**
 * hwp_document_add_char_shape:
 * @document: A #HwpDocument
 * @char_shape: A #HwpCharShape
 *
 * Since: 0.0.3
 */
void
hwp_document_add_char_shape (HwpDocument *document, HwpCharShape *char_shape)
{
  g_return_if_fail (HWP_IS_DOCUMENT (document));
  g_return_if_fail (char_shape != NULL);

  g_ptr_array_add (document->char_shapes, char_shape);
}

/**
 * hwp_document_add_face_name:
 * @document: A #HwpDocument
 * @face_name: A #HwpFaceName
 *
 * Since: 0.0.3
 */
void hwp_document_add_face_name (HwpDocument *document, HwpFaceName *face_name)
{
  g_return_if_fail (HWP_IS_DOCUMENT (document));
  g_return_if_fail (face_name != NULL);

  g_ptr_array_add (document->face_names, face_name);
}

/* callback */
static void hwp_document_listen_paragraph (HwpListener  *listener,
                                           HwpParagraph *paragraph,
                                           gpointer      user_data,
                                           GError      **error)
{
  HwpDocument *document = HWP_DOCUMENT (listener);
  hwp_document_add_paragraph (document, paragraph);
}

static void hwp_document_listen_summary_info (HwpListener    *listener,
                                              HwpSummaryInfo *info,
                                              gpointer        user_data,
                                              GError        **error)
{
  HwpDocument *document = HWP_DOCUMENT (listener);

  document->info = info;
}

static void hwp_document_listen_char_shape (HwpListener  *listener,
                                            HwpCharShape *char_shape,
                                            gpointer      user_data,
                                            GError      **error)
{
  HwpDocument *document = HWP_DOCUMENT (listener);
  hwp_document_add_char_shape (document, char_shape);
}

static void hwp_document_listen_face_name (HwpListener  *listener,
                                           HwpFaceName  *face_name,
                                           gpointer      user_data,
                                           GError      **error)
{
  HwpDocument *document = HWP_DOCUMENT (listener);
  hwp_document_add_face_name (document, face_name);
}

static void hwp_document_listener_iface_init (HwpListenerInterface *iface)
{
  iface->document_version = hwp_document_listen_version;
  iface->char_shape       = hwp_document_listen_char_shape;
  iface->face_name        = hwp_document_listen_face_name;
  iface->paragraph        = hwp_document_listen_paragraph;
  iface->summary_info     = hwp_document_listen_summary_info;
}

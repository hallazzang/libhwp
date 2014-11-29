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
#include "hwp-listenable.h"
#include <math.h>
#include <pango/pango.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

static void
hwp_document_listenable_iface_init (HwpListenableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HwpDocument, hwp_document, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (HWP_TYPE_LISTENABLE,
                         hwp_document_listenable_iface_init))

static cairo_status_t hwp_document_write_pdf_to_mem (void *closure,
                                                     const unsigned char *data,
                                                     unsigned int length)
{
  GByteArray *pdf_data = HWP_DOCUMENT (closure)->pdf_data;
  g_byte_array_append (pdf_data, data, length);

  return CAIRO_STATUS_SUCCESS;
}

static void hwp_document_add_page (HwpDocument *document, HwpPage *page)
{
  g_ptr_array_add (document->pages, page);
}

static void hwp_document_set_attrs_to_pango_layout (HwpDocument  *document,
                                                    HwpParagraph *paragraph,
                                                    PangoLayout  *layout)
{
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

    PangoAttribute *size, *foreground;
    size = pango_attr_size_new (font_size_in_points * PANGO_SCALE);
    size->start_index = text_attrs->start_index;
    size->end_index = text_attrs->end_index;

    foreground = pango_attr_foreground_new (char_shape->text_color.red,
                                            char_shape->text_color.green,
                                            char_shape->text_color.blue);

    pango_attr_list_insert (attrs, family);
    pango_attr_list_insert (attrs, size);
    pango_attr_list_insert (attrs, foreground);
  } /* for (guint j = 0; j < paragraph->m_len; j++) */

  guint8 major_version;
  hwp_document_get_hwp_version (document, &major_version, NULL, NULL, NULL);

  /* hwp5의 경우 attrs 적용합니다. */
  if (major_version == 5)
    pango_layout_set_attributes (layout, attrs);

  pango_attr_list_unref (attrs);
}

static void
hwp_document_paginate (HwpDocument *document, gdouble width, gdouble height)
{
  PangoFontMap *fontmap = pango_cairo_font_map_get_default ();
  PangoContext *context = pango_font_map_create_context (fontmap);

  gdouble x = 0.0;
  gdouble y = 0.0;

  for (guint i = 0; i < document->paragraphs->len; i++)
  {
    HwpParagraph *paragraph = hwp_document_get_paragraph (document, i);

    if (paragraph->text)
    {
      PangoLayout *layout = pango_layout_new (context);
      g_ptr_array_add (document->layouts, layout);
      HwpParaShape *para_shape = g_ptr_array_index (document->para_shapes,
                                                    paragraph->para_shape_id);
      /* 줄 간격 종류 한글 2007 이하 버전에서 사용 */
      /* int a1 = para_shape->prop1 & ((1 << 0) +
                                      (1 << 1)); */
      /* 정렬 방식 */
      int alignment = para_shape->prop1 & ((1 << 2) +
                                           (1 << 3) +
                                           (1 << 4));
      /* 줄 나눔 기준 영어 단위 */
      int wrap = para_shape->prop1 & ((1 << 5) +
                                      (1 << 6));
      /* 줄 나눔 기준 한글 단위 */
      /* int a4 = para_shape->prop1 & (1 << 7); */
      /* 편집 용지의 줄 격자 사용 여부 */
      /* int a5 = para_shape->prop1 & (1 << 8); */
      /* 공백 최소값 */
      /* int a6 = para_shape->prop1 & ((1 << 9) +
                                       (1 << 10) +
                                       (1 << 11) +
                                       (1 << 12) +
                                       (1 << 13) +
                                       (1 << 14) +
                                       (1 << 15)); */
      /* 외톨이줄 보호 여부 */
      /* int a7 = para_shape->prop1 & (1 << 16); */
      /* 다음 문단과 함께 여부 */
      /* int a8 = para_shape->prop1 & (1 << 17); */
      /* 문단 보호 여부 */
      /* int a9 = para_shape->prop1 & (1 << 18); */
      /* 문단 앞에서 항상 쪽 나눔 여부 */
      /* int a10 = para_shape->prop1 & (1 << 19); */
      /* 세로 정렬 */
      /* int a11 = para_shape->prop1 & ((1 << 20) +
                                        (1 << 21)); */
      /* 글꼴에 어울리는 줄 높이 여부 */
      /* int a12 = para_shape->prop1 & (1 << 22); */
      /* 문단 머리 모양 종류 */
      /* int a13 = para_shape->prop1 & ((1 << 23) +
                                        (1 << 24)); */
      /* 문단 문단 수준 */
      /* int a14 = para_shape->prop1 & ((1 << 25) +
                                        (1 << 26) +
                                        (1 << 27)); */
      /* 문단 테두리 연결 여부 */
      /* int a15 = para_shape->prop1 & (1 << 28); */

      switch (wrap)
      {
        case 0:
          pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
          break;
        case 1 << 5:
          pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
          break;
        case 2 << 5:
          pango_layout_set_wrap (layout, PANGO_WRAP_CHAR);
          break;
        default:
          break;
      }

      switch (alignment)
      {
        /* case 0: */
        case 1 << 2:
          pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
          break;
        case 2 << 2:
          pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
          break;
        case 3 << 2:
          pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
          break;
        /* case 4 << 2: */
        /* case 5 << 2: */
        default:
          pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
          break;
      }

      pango_layout_set_width (layout, width * PANGO_SCALE);
      pango_layout_set_text (layout, paragraph->text, -1);

      hwp_document_set_attrs_to_pango_layout (document, paragraph, layout);

      PangoLayoutIter *iter = pango_layout_get_iter (layout);

      do {
        PangoRectangle ink_rect0;
        PangoRectangle logical_rect0;
        PangoLayoutLine *line = pango_layout_iter_get_line_readonly (iter);
        pango_layout_iter_get_line_extents (iter, &ink_rect0, &logical_rect0);
        y = y + logical_rect0.height / PANGO_SCALE;

        guint page_index = (guint) floor (y / height);

        if ((int) page_index > (int) (document->pages->len - 1))
          hwp_document_add_page (document, hwp_page_new ());

        HwpLayout *hwp_layout = hwp_layout_new ();
        hwp_layout->data = pango_layout_line_ref (line);
        hwp_layout->type = 'p';
        hwp_layout->x = x;
        hwp_layout->y = y - page_index * height;

        /* 페이지 넘어가는 경우 y값 보정 */
        if (hwp_layout->y < logical_rect0.height / PANGO_SCALE)
        {
          y = page_index * height + logical_rect0.height / PANGO_SCALE;
          hwp_layout->y = logical_rect0.height / PANGO_SCALE;
        }

        HwpPage *page = g_ptr_array_index (document->pages, page_index);
        hwp_page_add_layout (page, hwp_layout);
      } while (pango_layout_iter_next_line (iter));

      pango_layout_iter_free (iter);
    } /* if (paragraph->text) */

    if (paragraph->table)
    {
      GPtrArray    *row  = NULL;
      HwpTableCell *cell = NULL;
      gdouble       max  = 0.0;
      x = x + paragraph->table->left_margin * 0.01;
      y = y + paragraph->table->top_margin  * 0.01;
      gdouble backup_x = x;
      /* each row */
      for (guint i = 0; i < paragraph->table->rows->len; i++)
      {
        row = g_ptr_array_index (paragraph->table->rows, i);
        double backup_y = 0.0;

        /* cell in each row */
        for (guint j = 0; j < row->len; j++)
        {
          cell = g_ptr_array_index (row, j);
          HwpParagraph *c_paragraph = NULL;
          backup_y = y;
          gdouble sum = 0.0;

          guint page_index = (guint) floor (y / height);

          if ((int) page_index > (int) (document->pages->len - 1))
            hwp_document_add_page (document, hwp_page_new ());

          HwpLayout *c_line = hwp_layout_new ();
          c_line->data = hwp_point_new (x +  cell->width * 0.01,
                                        y - page_index * height);
          c_line->type = 'l';
          c_line->x = x;
          c_line->y = y - page_index * height;
          /* TODO: 페이지 넘어가는 경우 y값 보정 */
          HwpPage *page = g_ptr_array_index (document->pages, page_index);
          hwp_page_add_layout (page, c_line);
          page = NULL;

          x = x + cell->left_margin * 0.01;
          y = y + cell->top_margin  * 0.01;

          for (guint k = 0; k < cell->paragraphs->len; k++)
          {
            c_paragraph = g_ptr_array_index (cell->paragraphs, k);

            PangoLayout *layout = pango_layout_new (context);
            /* document->layouts: array for g_object_unref (layout) */
            g_ptr_array_add (document->layouts, layout);
            pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
            pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
            pango_layout_set_width (layout,
              (cell->width - cell->left_margin - cell->right_margin) * 0.01 * PANGO_SCALE);
            pango_layout_set_text (layout, c_paragraph->text, -1);

            hwp_document_set_attrs_to_pango_layout (document, c_paragraph, layout);

            PangoLayoutIter *iter = pango_layout_get_iter (layout);
            PangoRectangle ink_rect;
            PangoRectangle logical_rect;

            do {
              PangoLayoutLine *line = pango_layout_iter_get_line_readonly (iter);
              pango_layout_iter_get_line_extents (iter, &ink_rect, &logical_rect);

              guint page_index = (guint) floor (y / height);

              if ((int) page_index > (int) (document->pages->len - 1))
                hwp_document_add_page (document, hwp_page_new ());

              y = y + logical_rect.height / PANGO_SCALE;
              HwpLayout *hwp_layout = hwp_layout_new ();
              hwp_layout->data = pango_layout_line_ref (line);
              hwp_layout->type = 'p';
              hwp_layout->x = x;
              hwp_layout->y = y - page_index * height;
              /* TODO: 페이지 넘어가는 경우 y값 보정 */
              HwpPage *page = g_ptr_array_index (document->pages, page_index);
              hwp_page_add_layout (page, hwp_layout);
            } while (pango_layout_iter_next_line (iter));

            pango_layout_iter_free (iter);
            pango_layout_get_extents (layout, &ink_rect, &logical_rect);
            sum = sum + logical_rect.height;
          } /* for (guint k = 0; k < cell->paragraphs->len; k++) */

          if (max < sum)
            max = sum;

          sum = 0.0;
          x = x - cell->left_margin * 0.01 + cell->width * 0.01;
          y = backup_y;
        }

        x = backup_x;
        /* FIXME */
        y = backup_y + max / PANGO_SCALE + 10 +
              (cell->bottom_margin) * 0.01;
        max = 0.0;
      } /* for (guint i = 0; i < paragraph->table->rows->len; i++) */
      y = y + paragraph->table->bottom_margin  * 0.01;
    } /* if (paragraph->table) */
  } /* for (guint i = 0; i < document->paragraphs->len; i++) */

  g_object_unref (context);
}

static void hwp_document_render (HwpDocument *document, cairo_t *cr)
{
  for (guint j = 0; j < document->pages->len; j++)
  {
    HwpPage *page0 = g_ptr_array_index (document->pages, j);
    for (guint i = 0; i < page0->layouts->len; i++)
    {
      HwpLayout *hwp_layout = g_ptr_array_index (page0->layouts, i);
      cairo_move_to (cr, hwp_layout->x, hwp_layout->y);
      switch (hwp_layout->type)
      {
        case 'p': /* pango layout line */
          pango_cairo_show_layout_line (cr, hwp_layout->data);
          break;
        case 'l': /* line */
          cairo_line_to (cr, ((HwpPoint *) hwp_layout->data)->x,
                             ((HwpPoint *) hwp_layout->data)->y);
          cairo_stroke (cr);
          break;
        default:
          break;
      }
    }
    cairo_show_page (cr);
  }
}

static void hwp_document_create_poppler_document (HwpDocument *document)
{
  g_return_if_fail (document->paragraphs->len > 0);

  HwpParagraph *paragraph0 = hwp_document_get_paragraph (document, 0);
  HwpSecd *secd;
  if (paragraph0->secd)
    secd = paragraph0->secd;
  /* 아직 secd 작업하지 못한 hwp3, hwp ml 문서를 위해 */
  else
    secd = hwp_secd_new ();

  gdouble width  = secd->page_width_in_points -
                     secd->page_left_margin_in_points -
                     secd->page_right_margin_in_points;
  gdouble height = secd->page_height_in_points -
                     secd->page_top_margin_in_points -
                     secd->page_bottom_margin_in_points -
                     secd->page_header_margin_in_points -
                     secd->page_footer_margin_in_points;

  hwp_document_paginate (document, width, height);

  cairo_surface_t *surface;
  surface = cairo_pdf_surface_create_for_stream (hwp_document_write_pdf_to_mem,
                                                 document,
                                                 secd->page_width_in_points,
                                                 secd->page_height_in_points);
  cairo_t *cr = cairo_create (surface);
  cairo_translate (cr, secd->page_left_margin_in_points,
                       secd->page_top_margin_in_points);

  hwp_document_render (document, cr);

  cairo_destroy (cr);
  cairo_surface_finish (surface);
  cairo_surface_destroy (surface);

  document->poppler_document =
    poppler_document_new_from_data ((char *) document->pdf_data->data,
                                    document->pdf_data->len, NULL, NULL);

  for (guint i = 0; i < document->pages->len; i++)
  {
    HwpPage *page = g_ptr_array_index (document->pages, i);
    page->poppler_page = poppler_document_get_page (document->poppler_document, i);
  }
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

  return document->pages->len;
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

  return g_object_ref (g_ptr_array_index (document->pages, n_page));
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

  if (document->major_version == 3)
    return g_strdup_printf ("%d.%d", document->major_version,
                                     document->minor_version);
  else
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
  g_ptr_array_free (document->para_shapes, TRUE);
  g_ptr_array_free (document->face_names, TRUE);
  g_ptr_array_free (document->bin_data, TRUE);
  g_ptr_array_free (document->paragraphs, TRUE);
  g_ptr_array_free (document->pages, TRUE);
  g_ptr_array_free (document->layouts, TRUE);
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

  document->para_shapes =
    g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_para_shape_free);

  document->face_names =
    g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_face_name_free);

  document->bin_data =
    g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_bin_data_free);

  document->paragraphs = g_ptr_array_new_with_free_func (g_object_unref);
  document->pages      = g_ptr_array_new_with_free_func (g_object_unref);
  document->layouts    = g_ptr_array_new_with_free_func (g_object_unref);
}

/* callback */
static void hwp_document_listen_version (HwpListenable *listenable,
                                         guint8         major_version,
                                         guint8         minor_version,
                                         guint8         micro_version,
                                         guint8         extra_version,
                                         gpointer       user_data,
                                         GError       **error)
{
  HwpDocument *document   = (HwpDocument *) listenable;
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
 * hwp_document_get_paragraph:
 * @document: a #HwpDocument
 * @index: the index of the paragraph to get
 *
 * Returns a #HwpParagraph representing the paragraph at index
 *
 * Returns: (transfer none): a #HwpParagraph or %NULL on error.
 *
 * Since: 0.1
 */
HwpParagraph *hwp_document_get_paragraph (HwpDocument *document, guint index)
{
  g_return_val_if_fail (HWP_IS_DOCUMENT (document), NULL);

  if (index < document->paragraphs->len)
    return g_ptr_array_index (document->paragraphs, index);
  else
    return NULL;
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
 * hwp_document_add_para_shape:
 * @document: A #HwpDocument
 * @para_shape: A #HwpParaShape
 *
 * Since: 0.1.5
 */
void
hwp_document_add_para_shape (HwpDocument *document, HwpParaShape *para_shape)
{
  g_return_if_fail (HWP_IS_DOCUMENT (document));
  g_return_if_fail (para_shape != NULL);

  g_ptr_array_add (document->para_shapes, para_shape);
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

void hwp_document_add_bin_data (HwpDocument *document, HwpBinData *bin_data)
{
  g_return_if_fail (HWP_IS_DOCUMENT (document));
  g_return_if_fail (bin_data != NULL);

  g_ptr_array_add (document->bin_data, bin_data);
}

/* callback */
static void hwp_document_listen_paragraph (HwpListenable *listenable,
                                           HwpParagraph  *paragraph,
                                           gpointer       user_data,
                                           GError       **error)
{
  HwpDocument *document = HWP_DOCUMENT (listenable);
  hwp_document_add_paragraph (document, paragraph);
}

static void
hwp_document_listen_summary_info (HwpListenable  *listenable,
                                  HwpSummaryInfo *info,
                                  gpointer        user_data,
                                  GError        **error)
{
  HwpDocument *document = HWP_DOCUMENT (listenable);

  document->info = info;
}

static void hwp_document_listen_char_shape (HwpListenable *listenable,
                                            HwpCharShape  *char_shape,
                                            gpointer       user_data,
                                            GError       **error)
{
  HwpDocument *document = HWP_DOCUMENT (listenable);
  hwp_document_add_char_shape (document, char_shape);
}

static void hwp_document_listen_para_shape (HwpListenable *listenable,
                                            HwpParaShape  *para_shape,
                                            gpointer       user_data,
                                            GError       **error)
{
  HwpDocument *document = HWP_DOCUMENT (listenable);
  hwp_document_add_para_shape (document, para_shape);
}

static void hwp_document_listen_face_name (HwpListenable *listenable,
                                           HwpFaceName   *face_name,
                                           gpointer       user_data,
                                           GError       **error)
{
  HwpDocument *document = HWP_DOCUMENT (listenable);
  hwp_document_add_face_name (document, face_name);
}

static void hwp_document_listen_bin_data (HwpListenable *listenable,
                                          HwpBinData    *bin_data,
                                          gpointer       user_data,
                                          GError       **error)
{
  HwpDocument *document = HWP_DOCUMENT (listenable);
  hwp_document_add_bin_data (document, bin_data);
}

static void
hwp_document_listenable_iface_init (HwpListenableInterface *iface)
{
  iface->document_version = hwp_document_listen_version;
  iface->char_shape       = hwp_document_listen_char_shape;
  iface->para_shape       = hwp_document_listen_para_shape;
  iface->face_name        = hwp_document_listen_face_name;
  iface->bin_data         = hwp_document_listen_bin_data;
  iface->paragraph        = hwp_document_listen_paragraph;
  iface->summary_info     = hwp_document_listen_summary_info;
}

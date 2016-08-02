/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-models.c
 * This file is part of the libhwp project.
 *
 * Copyright (C) 2012-2016 Hodong Kim <cogniti@gmail.com>
 *
 * The libhwp is dual licensed under the LGPL v3+ or Apache License 2.0
 *
 * The libhwp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The libhwp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program;  If not, see <http://www.gnu.org/licenses/>.
 *
 * Or,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This software has been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include <glib/gprintf.h>

#include "hwp-models.h"
#include "hwp-hwp5-parser.h"

/* HwpSummaryInfo **********************************************************/

G_DEFINE_TYPE (HwpSummaryInfo, hwp_summary_info, G_TYPE_OBJECT);

/**
 * hwp_summary_info_new:
 *
 * Creates a new #HwpSummaryInfo
 *
 * Returns: a new #HwpSummaryInfo
 *
 * Since: 0.0.1
 */
HwpSummaryInfo *hwp_summary_info_new (void)
{
  return (HwpSummaryInfo *) g_object_new (HWP_TYPE_SUMMARY_INFO, NULL);
}

static void hwp_summary_info_init (HwpSummaryInfo *hwp_summary_info)
{
}

static void hwp_summary_info_finalize (GObject *object)
{
  HwpSummaryInfo *info = HWP_SUMMARY_INFO (object);

  g_free (info->title);
  g_free (info->format);
  g_free (info->author);
  g_free (info->subject);
  g_free (info->keywords);
  g_free (info->layout);
  g_free (info->start_mode);
  g_free (info->permissions);
  g_free (info->ui_hints);
  g_free (info->creator);
  g_free (info->producer);
  g_free (info->linearized);
  g_free (info->security);
  g_free (info->paper_size);
  g_free (info->license);
  /* hwp info */
  g_free (info->desc);
  g_free (info->last_saved_by);
  /* version of hanword */
  g_free (info->hanword_version);

  G_OBJECT_CLASS (hwp_summary_info_parent_class)->finalize (object);
}

static void hwp_summary_info_class_init (HwpSummaryInfoClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hwp_summary_info_finalize;
}

/* HwpParagraph ************************************************************/

G_DEFINE_TYPE (HwpParagraph, hwp_paragraph, G_TYPE_OBJECT);

static void hwp_paragraph_finalize (GObject *object)
{
  HwpParagraph *paragraph = HWP_PARAGRAPH (object);

  if (paragraph->text)
    g_free (paragraph->text);

  if (paragraph->table)
    g_object_unref (paragraph->table);

  if (paragraph->secd)
    hwp_secd_free (paragraph->secd);

  if (paragraph->m_pos)
    g_free (paragraph->m_pos);

  if (paragraph->m_id)
    g_free (paragraph->m_id);

  if (paragraph->text_attrs)
    g_ptr_array_free (paragraph->text_attrs, TRUE);

  G_OBJECT_CLASS (hwp_paragraph_parent_class)->finalize (object);
}

static void hwp_paragraph_class_init (HwpParagraphClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_paragraph_finalize;
}

static void hwp_paragraph_init (HwpParagraph *paragraph)
{
}

/**
 * hwp_paragraph_new:
 *
 * Creates a new #HwpParagraph
 *
 * Returns: a new #HwpParagraph
 *
 * Since: 0.0.1
 */
HwpParagraph *hwp_paragraph_new (void)
{
  return g_object_new (HWP_TYPE_PARAGRAPH, NULL);
}

/**
 * hwp_paragraph_set_text:
 * @paragraph: a #HwpParagraph
 * @text: a valid UTF-8 string
 *
 * Since: 0.0.4
 */
void hwp_paragraph_set_text (HwpParagraph *paragraph, const gchar *text)
{
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
  paragraph->text = g_strdup (text);
}

/**
 * hwp_paragraph_get_text:
 * @paragraph: a #HwpParagraph
 *
 * Return value: (transfer none): the text in the paragraph
 *
 * Since: 0.0.4
 */
const char *hwp_paragraph_get_text (HwpParagraph *paragraph)
{
  g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), NULL);
  return paragraph->text;
}

/**
 * hwp_paragraph_set_secd:
 * @paragraph: a #HwpParagraph
 * @secd: a #HwpSecd
 *
 * Since: 0.0.3
 */
void hwp_paragraph_set_secd (HwpParagraph *paragraph, HwpSecd *secd)
{
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
  g_return_if_fail (secd != NULL);
  paragraph->secd = secd;
}

/**
 * hwp_paragraph_set_table:
 * @paragraph: a #HwpParagraph
 * @table: a #HwpTable
 *
 * Since: 0.0.1
 */
void hwp_paragraph_set_table (HwpParagraph *paragraph, HwpTable *table)
{
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
  g_return_if_fail (HWP_IS_TABLE (table));
  paragraph->table = table;
}

/**
 * hwp_paragraph_get_table:
 * @paragraph: a #HwpParagraph
 *
 * Return value: (transfer none): A #HwpTable, or %NULL
 *
 * Since: 0.0.1
 */
HwpTable *hwp_paragraph_get_table (HwpParagraph *paragraph)
{
  g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), NULL);
  return paragraph->table;
}

/* HwpTable ****************************************************************/

G_DEFINE_TYPE (HwpTable, hwp_table, G_TYPE_OBJECT);

/**
 * hwp_table_new:
 *
 * Creates a new #HwpTable
 *
 * Returns: a new #HwpTable
 *
 * Since: 0.0.1
 */
HwpTable *hwp_table_new (void)
{
  return (HwpTable *) g_object_new (HWP_TYPE_TABLE, NULL);
}

#ifdef HWP_ENABLE_DEBUG
#include <stdio.h>

void hexdump(guint8 *data, guint16 data_len)
{
  int i = 0;

  printf("data_len = %d\n", data_len);
  printf("-----------------------------------------------\n");
  printf("00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n");
  printf("-----------------------------------------------\n");

  for (i = 0; i < data_len; i++) {
    if ( (i != 0) && (i % 16 == 0))
      printf("\n");
    printf("%02x ", data[i]);
  }
  printf("\n-----------------------------------------------\n");
}
#endif

static void hwp_table_init (HwpTable *table)
{
  table->rows = g_ptr_array_new_with_free_func ((GDestroyNotify) g_ptr_array_unref);
}

static void hwp_table_finalize (GObject *object)
{
  HwpTable *table = HWP_TABLE(object);

  if (table->row_sizes)
    g_free (table->row_sizes);

  if (table->zones)
    g_free (table->zones);

  g_ptr_array_unref (table->rows);

  G_OBJECT_CLASS (hwp_table_parent_class)->finalize (object);
}

static void hwp_table_class_init (HwpTableClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_table_finalize;
}

/**
 * hwp_table_get_last_cell:
 * @table: a #HwpTable
 *
 * Return value: (transfer none): A #HwpTableCell, or %NULL
 *
 * Since: 0.0.1
 */
HwpTableCell *hwp_table_get_last_cell (HwpTable *table)
{
  g_return_val_if_fail (HWP_IS_TABLE (table), NULL);

  GPtrArray *row = g_ptr_array_index (table->rows, table->rows->len - 1);
  return g_ptr_array_index (row, row->len - 1);
}

/**
 * hwp_table_add_cell:
 * @table: a #HwpTable
 * @cell: a #HwpTableCell
 * @index: row index
 *
 * Since: 0.1.2
 */
void hwp_table_add_cell (HwpTable *table, HwpTableCell *cell, guint index)
{
  g_return_if_fail (HWP_IS_TABLE (table));
  g_return_if_fail (HWP_IS_TABLE_CELL (cell));

  if (index < table->rows->len)
  {
    GPtrArray *row = g_ptr_array_index (table->rows, index);
    g_ptr_array_add (row, cell);
  }
  else
  {
    g_warning ("hwp_table_add_cell: out of index");
  }
}

/* HwpTableCell ************************************************************/

G_DEFINE_TYPE (HwpTableCell, hwp_table_cell, G_TYPE_OBJECT);

static void hwp_table_cell_init (HwpTableCell *cell)
{
  cell->paragraphs = g_ptr_array_new_with_free_func (g_object_unref);
}

static void hwp_table_cell_finalize (GObject *object)
{
  HwpTableCell *cell = HWP_TABLE_CELL (object);
  g_ptr_array_unref (cell->paragraphs);

  G_OBJECT_CLASS (hwp_table_cell_parent_class)->finalize (object);
}

static void hwp_table_cell_class_init (HwpTableCellClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_table_cell_finalize;
}

/**
 * hwp_table_cell_new:
 *
 * Creates a new #HwpTableCell
 *
 * Returns: a new #HwpTableCell
 *
 * Since: 0.0.1
 */
HwpTableCell *hwp_table_cell_new (void)
{
  return (HwpTableCell *) g_object_new (HWP_TYPE_TABLE_CELL, NULL);
}

/**
 * hwp_table_cell_get_last_paragraph:
 * @cell: a #HwpTableCell
 *
 * Return value: (transfer none): A #HwpParagraph, or %NULL
 *
 * Since: 0.0.1
 */
HwpParagraph *hwp_table_cell_get_last_paragraph (HwpTableCell *cell)
{
  g_return_val_if_fail (HWP_IS_TABLE_CELL (cell), NULL);

  return g_ptr_array_index (cell->paragraphs, cell->paragraphs->len - 1);
}

/**
 * hwp_table_cell_add_paragraph:
 * @cell: a #HwpTableCell
 * @paragraph: a#HwpParagraph
 *
 * Since: 0.0.1
 */
void hwp_table_cell_add_paragraph (HwpTableCell *cell, HwpParagraph *paragraph)
{
  g_return_if_fail (HWP_IS_TABLE_CELL (cell));
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
  g_ptr_array_add (cell->paragraphs, paragraph);
}

G_DEFINE_BOXED_TYPE (HwpSecd, hwp_secd, hwp_secd_copy, hwp_secd_free)

/**
 * hwp_secd_new:
 *
 * Creates a new #HwpSecd
 *
 * Returns: a new #HwpSecd
 *
 * Since: 0.0.3
 */
HwpSecd *hwp_secd_new ()
{
  HwpSecd *secd = g_slice_new0 (HwpSecd);

  /* dafault 값 설정 */
  secd->page_width_in_points = 595.28;
  secd->page_height_in_points = 841.88;
  secd->page_left_margin_in_points = 85.04;
  secd->page_right_margin_in_points = 85.04;
  secd->page_top_margin_in_points = 56.68;
  secd->page_bottom_margin_in_points = 42.52;
  secd->page_header_margin_in_points = 42.52;
  secd->page_footer_margin_in_points = 42.52;
  secd->page_gutter_margin_in_points = 0.0;
  secd->page_prop = 0.0;

  return secd;
}

/**
 * hwp_secd_copy:
 * @secd: a #HwpSecd
 *
 * Since: 0.0.3
 */
HwpSecd *hwp_secd_copy (HwpSecd *secd)
{
  g_return_val_if_fail (secd != NULL, NULL);

  return g_slice_dup (HwpSecd, secd);
}

/**
 * hwp_secd_free:
 * @secd: a #HwpSecd
 *
 * Since: 0.0.3
 */
void hwp_secd_free (HwpSecd *secd)
{
  if (secd)
    g_slice_free (HwpSecd, secd);
}

G_DEFINE_BOXED_TYPE (HwpFaceName, hwp_face_name,
                     hwp_face_name_copy, hwp_face_name_free)

HwpFaceName *hwp_face_name_new ()
{
  return g_slice_new0 (HwpFaceName);
}

HwpFaceName *hwp_face_name_copy (HwpFaceName *face_name)
{
  g_return_val_if_fail (face_name != NULL, NULL);

  return g_slice_dup (HwpFaceName, face_name);
}

void hwp_face_name_free (HwpFaceName *face_name)
{
  g_free (face_name->font_name);
  g_slice_free (HwpFaceName, face_name);
}

G_DEFINE_BOXED_TYPE (HwpCharShape, hwp_char_shape,
                     hwp_char_shape_copy, hwp_char_shape_free)

HwpCharShape *hwp_char_shape_new ()
{
  return g_slice_new0 (HwpCharShape);
}

HwpCharShape *hwp_char_shape_copy (HwpCharShape *char_shape)
{
  g_return_val_if_fail (char_shape != NULL, NULL);

  return g_slice_dup (HwpCharShape, char_shape);
}

void hwp_char_shape_free (HwpCharShape *char_shape)
{
  g_slice_free (HwpCharShape, char_shape);
}

G_DEFINE_BOXED_TYPE (HwpParaShape, hwp_para_shape,
                     hwp_para_shape_copy, hwp_para_shape_free)

HwpParaShape *hwp_para_shape_new ()
{
  return g_slice_new0 (HwpParaShape);
}

HwpParaShape *hwp_para_shape_copy (HwpParaShape *para_shape)
{
  g_return_val_if_fail (para_shape != NULL, NULL);

  return g_slice_dup (HwpParaShape, para_shape);
}

void hwp_para_shape_free (HwpParaShape *para_shape)
{
  g_slice_free (HwpParaShape, para_shape);
}

G_DEFINE_BOXED_TYPE (HwpBinData, hwp_bin_data,
                     hwp_bin_data_copy, hwp_bin_data_free)

HwpBinData *hwp_bin_data_new ()
{
  return g_slice_new0 (HwpBinData);
}

HwpBinData *hwp_bin_data_copy (HwpBinData *bin_data)
{
  g_return_val_if_fail (bin_data != NULL, NULL);

  return g_slice_dup (HwpBinData, bin_data);
}

void hwp_bin_data_free (HwpBinData *bin_data)
{
  g_free (bin_data->format);
  g_slice_free (HwpBinData, bin_data);
}

G_DEFINE_BOXED_TYPE (HwpCommonProperty, hwp_common_property,
                     hwp_common_property_copy, hwp_common_property_free)

HwpCommonProperty *hwp_common_property_new ()
{
  return g_slice_new0 (HwpCommonProperty);
}

HwpCommonProperty *hwp_common_property_copy (HwpCommonProperty *property)
{
  g_return_val_if_fail (property != NULL, NULL);

  return g_slice_dup (HwpCommonProperty, property);
}

void hwp_common_property_free (HwpCommonProperty *property)
{
  g_slice_free (HwpCommonProperty, property);
}

G_DEFINE_BOXED_TYPE (HwpPoint, hwp_point, hwp_point_copy, hwp_point_free)

/**
 * hwp_point_new:
 * @x: x coordinate
 * @y: y coordinate
 *
 * Creates a new #HwpPoint
 *
 * Returns: a new #HwpPoint, use hwp_point_free() to free it
 */
HwpPoint *hwp_point_new (gdouble x, gdouble y)
{
  HwpPoint *point = g_slice_new0 (HwpPoint);
  point->x = x;
  point->y = y;
  return point;
}

/**
 * hwp_point_copy:
 * @point: a #HwpPoint to copy
 *
 * Creates a copy of @point
 *
 * Returns: a new allocated copy of @point
 */
HwpPoint *hwp_point_copy (HwpPoint *point)
{
  g_return_val_if_fail (point != NULL, NULL);

  return g_slice_dup (HwpPoint, point);
}

/**
 * hwp_point_free:
 * @point: a #HwpPoint
 *
 * Frees the given #HwpPoint
 */
void hwp_point_free (HwpPoint *point)
{
  g_slice_free (HwpPoint, point);
}

/* HwpRectangle type */

G_DEFINE_BOXED_TYPE (HwpRectangle, hwp_rectangle,
                     hwp_rectangle_copy, hwp_rectangle_free)

/**
 * hwp_rectangle_new:
 *
 * Creates a new #HwpRectangle
 *
 * Returns: a new #HwpRectangle, use hwp_rectangle_free() to free it
 */
HwpRectangle *hwp_rectangle_new (void)
{
  return g_slice_new0 (HwpRectangle);
}

/**
 * hwp_rectangle_copy:
 * @rectangle: a #HwpRectangle to copy
 *
 * Creates a copy of @rectangle
 *
 * Returns: a new allocated copy of @rectangle
 */
HwpRectangle *hwp_rectangle_copy (HwpRectangle *rectangle)
{
  g_return_val_if_fail (rectangle != NULL, NULL);

  return g_slice_dup (HwpRectangle, rectangle);
}

/**
 * hwp_rectangle_free:
 * @rectangle: a #HwpRectangle
 *
 * Frees the given #HwpRectangle
 */
void hwp_rectangle_free (HwpRectangle *rectangle)
{
  g_slice_free (HwpRectangle, rectangle);
}

/* HwpColor type */
G_DEFINE_BOXED_TYPE (HwpColor, hwp_color, hwp_color_copy, hwp_color_free)

/**
 * hwp_color_new:
 *
 * Creates a new #HwpColor
 *
 * Returns: a new #HwpColor, use hwp_color_free() to free it
 */
HwpColor *hwp_color_new (void)
{
  return (HwpColor *) g_new0 (HwpColor, 1);
}

/**
 * hwp_color_copy:
 * @color: a #HwpColor to copy
 *
 * Creates a copy of @color
 *
 * Returns: a new allocated copy of @color
 */
HwpColor *hwp_color_copy (HwpColor *color)
{
  HwpColor *new_color;

  new_color = g_new (HwpColor, 1);
  *new_color = *color;

  return new_color;
}

/**
 * hwp_color_free:
 * @color: a #HwpColor
 *
 * Frees the given #HwpColor
 */
void hwp_color_free (HwpColor *color)
{
  g_free (color);
}

/* HwpTextAttributes type */

G_DEFINE_BOXED_TYPE (HwpTextAttributes, hwp_text_attributes,
                     hwp_text_attributes_copy, hwp_text_attributes_free)

/**
 * hwp_text_attributes_new:
 *
 * Creates a new #HwpTextAttributes
 *
 * Returns: a new #HwpTextAttributes, use hwp_text_attributes_free() to free it
 *
 * Since: 0.0.4
 */
HwpTextAttributes *hwp_text_attributes_new (void)
{
  return (HwpTextAttributes *) g_slice_new0 (HwpTextAttributes);
}

/**
 * hwp_text_attributes_copy:
 * @text_attrs: a #HwpTextAttributes to copy
 *
 * Creates a copy of @text_attrs
 *
 * Returns: a new allocated copy of @text_attrs
 *
 * Since: 0.0.4
 */
HwpTextAttributes *hwp_text_attributes_copy (HwpTextAttributes *text_attrs)
{
  HwpTextAttributes *attrs;

  attrs = g_slice_dup (HwpTextAttributes, text_attrs);
  attrs->font_name = g_strdup (text_attrs->font_name);
  return attrs;
}

/**
 * hwp_text_attributes_free:
 * @text_attrs: a #HwpTextAttributes
 *
 * Frees the given #HwpTextAttributes
 *
 * Since: 0.0.4
 */
void hwp_text_attributes_free (HwpTextAttributes *text_attrs)
{
  g_free (text_attrs->font_name);
  g_slice_free (HwpTextAttributes, text_attrs);
}

/**
 * hwp_page_free_text_attributes:
 * @list: (element-type HwpTextAttributes): A list of
 *   #HwpTextAttributes<!-- -->s
 *
 * Frees a list of #HwpTextAttributes<!-- -->s allocated by
 * hwp_page_get_text_attributes().
 *
 * Since: 0.0.4
 */
void hwp_page_free_text_attributes (GList *list)
{
  if (G_UNLIKELY (list == NULL))
    return;

  g_list_foreach (list, (GFunc) hwp_text_attributes_free, NULL);
  g_list_free (list);
}

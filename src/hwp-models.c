/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-models.c
 *
 * Copyright (C) 2013-2014 Hodong Kim <hodong@cogno.org>
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

#include <glib/gprintf.h>

#include "hwp-models.h"
#include "hwp-hwp5-parser.h"

/* HwpSummaryInfo **********************************************************/

G_DEFINE_TYPE (HwpSummaryInfo, hwp_summary_info, G_TYPE_OBJECT);

/**
 * hwp_summary_info_new:
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

/**
 * hwp_paragraph_new:
 * Since: 0.0.1
 */
HwpParagraph *hwp_paragraph_new (void)
{
  return g_object_new (HWP_TYPE_PARAGRAPH, NULL);
}

static void hwp_paragraph_finalize (GObject *object)
{
  HwpParagraph *paragraph = HWP_PARAGRAPH (object);

  if (paragraph->text)
    g_free ((char *) paragraph->text);

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
 * hwp_paragraph_set_text:
 * @paragraph: a #HwpParagraph
 * @text: a valid UTF-8 string
 *
 * Since: 0.0.4
 */
void hwp_paragraph_set_text (HwpParagraph *paragraph, const gchar *text)
{
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
  paragraph->text = text;
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
  table->cells = g_ptr_array_new_with_free_func (g_object_unref);
}

static void hwp_table_finalize (GObject *object)
{
  HwpTable *table = HWP_TABLE(object);

  if (table->row_sizes)
    g_free (table->row_sizes);

  if (table->zones)
    g_free (table->zones);

  g_ptr_array_unref (table->cells);

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
  return g_ptr_array_index (table->cells, table->cells->len - 1);
}

/**
 * hwp_table_add_cell:
 * Since: 0.0.1
 */
void hwp_table_add_cell (HwpTable *table, HwpTableCell *cell)
{
  g_return_if_fail (HWP_IS_TABLE (table));
  g_return_if_fail (HWP_IS_TABLE_CELL (cell));
  g_ptr_array_add (table->cells, cell);
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
 * Since: 0.0.1
 */
void hwp_table_cell_add_paragraph (HwpTableCell *cell, HwpParagraph *paragraph)
{
  g_return_if_fail (HWP_IS_TABLE_CELL (cell));
  g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
  g_ptr_array_add (cell->paragraphs, paragraph);
}

HWP_DEFINE_BOXED_TYPE (HwpSecd, hwp_secd, hwp_secd_copy, hwp_secd_free)

/**
 * hwp_secd_new:
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
 * Since: 0.0.3
 */
HwpSecd *hwp_secd_copy (HwpSecd *secd)
{
  g_return_val_if_fail (secd != NULL, NULL);

  return g_slice_dup (HwpSecd, secd);
}

/**
 * hwp_secd_free:
 * Since: 0.0.3
 */
void hwp_secd_free (HwpSecd *secd)
{
  if (secd)
    g_slice_free (HwpSecd, secd);
}

HWP_DEFINE_BOXED_TYPE (HwpFaceName,
                       hwp_face_name,
                       hwp_face_name_copy,
                       hwp_face_name_free)

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
  g_slice_free (HwpFaceName, face_name);
}

HWP_DEFINE_BOXED_TYPE (HwpCharShape,
                       hwp_char_shape,
                       hwp_char_shape_copy,
                       hwp_char_shape_free)

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

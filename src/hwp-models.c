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

/** HwpText *****************************************************************/

G_DEFINE_TYPE (HwpText, hwp_text, G_TYPE_OBJECT);

HwpText *hwp_text_new (const gchar *text)
{
    g_return_val_if_fail (text != NULL, NULL);
    HwpText *hwp_text = (HwpText *) g_object_new (HWP_TYPE_TEXT, NULL);
    hwp_text->text = g_strdup (text);
    return hwp_text;
}

/**
 * hwp_text_append:
 * @hwp_text: a #HwpText
 * @text: #gchar
 *
 * Return value: (transfer none): A #HwpText, or %NULL
 *
 * Since: 0.0.1
 */
HwpText *hwp_text_append (HwpText *hwp_text, const gchar *text)
{
    g_return_val_if_fail (HWP_IS_TEXT (hwp_text), NULL);

    gchar *tmp;
    tmp = g_strdup (hwp_text->text);
    g_free (hwp_text->text);
    hwp_text->text = g_strconcat (tmp, text, NULL);
    g_free (tmp);
    return hwp_text;
}

static void hwp_text_finalize (GObject *object)
{
  HwpText *hwp_text = HWP_TEXT(object);
  if (hwp_text->text)
    g_free (hwp_text->text);

  G_OBJECT_CLASS (hwp_text_parent_class)->finalize (object);
}

static void hwp_text_class_init (HwpTextClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_text_finalize;
}

static void hwp_text_init (HwpText *hwp_text)
{
}

/** HwpParagraph ************************************************************/

G_DEFINE_TYPE (HwpParagraph, hwp_paragraph, G_TYPE_OBJECT);

HwpParagraph *hwp_paragraph_new (void)
{
    return (HwpParagraph *) g_object_new (HWP_TYPE_PARAGRAPH, NULL);
}

static void hwp_paragraph_finalize (GObject *obj)
{
    G_OBJECT_CLASS (hwp_paragraph_parent_class)->finalize (obj);
}

static void hwp_paragraph_class_init (HwpParagraphClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_paragraph_finalize;
}

static void hwp_paragraph_init (HwpParagraph *paragraph)
{
}

void
hwp_paragraph_set_hwp_text (HwpParagraph *paragraph, HwpText *hwp_text)
{
    g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
    g_return_if_fail (HWP_IS_TEXT (hwp_text));
    paragraph->hwp_text = hwp_text;
}

/**
 * hwp_paragraph_get_hwp_text:
 * @paragraph: a #HwpParagraph
 *
 * Return value: (transfer none): A #HwpText, or %NULL
 *
 * Since: 0.0.1
 */
HwpText *hwp_paragraph_get_hwp_text (HwpParagraph *paragraph)
{
    g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), NULL);
    return paragraph->hwp_text;
}

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

/** HwpTable ****************************************************************/

G_DEFINE_TYPE (HwpTable, hwp_table, G_TYPE_OBJECT);

HwpTable *hwp_table_new (void)
{
    return g_object_new (HWP_TYPE_TABLE, NULL);
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
    table->cells = g_array_new (TRUE, TRUE, sizeof (HwpTableCell *));
}

static void hwp_table_finalize (GObject *object)
{
  HwpTable *table = HWP_TABLE(object);

  if (table->row_sizes)
    g_free (table->row_sizes);

  if (table->zones)
    g_free (table->zones);

  g_array_free (table->cells, TRUE);

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
    return g_array_index (table->cells, HwpTableCell *,
                          table->cells->len - 1);
}

void hwp_table_add_cell (HwpTable *table, HwpTableCell *cell)
{
    g_return_if_fail (HWP_IS_TABLE (table));
    g_return_if_fail (HWP_IS_TABLE_CELL (cell));
    g_array_append_val (table->cells, cell);
}

/** HwpTableCell ************************************************************/

G_DEFINE_TYPE (HwpTableCell, hwp_table_cell, G_TYPE_OBJECT);

static void hwp_table_cell_init (HwpTableCell *cell)
{
    cell->paragraphs = g_array_new (TRUE, TRUE, sizeof (HwpParagraph *));
    cell->layouts    = g_array_new (TRUE, TRUE, sizeof (PangoLayout *));
}

static void hwp_table_cell_finalize (GObject *object)
{
    HwpTableCell *cell = HWP_TABLE_CELL(object);
    g_array_free (cell->paragraphs, TRUE);
    g_array_free (cell->layouts,    TRUE);
    G_OBJECT_CLASS (hwp_table_cell_parent_class)->finalize (object);
}

static void hwp_table_cell_class_init (HwpTableCellClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_table_cell_finalize;
}

HwpTableCell *hwp_table_cell_new (void)
{
    return g_object_new (HWP_TYPE_TABLE_CELL, NULL);
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
    return g_array_index (cell->paragraphs, HwpParagraph *,
                          cell->paragraphs->len - 1);
}

void
hwp_table_cell_add_paragraph (HwpTableCell *cell, HwpParagraph *paragraph)
{
    g_return_if_fail (HWP_IS_TABLE_CELL (cell));
    g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
    g_array_append_val (cell->paragraphs, paragraph);
}

void
hwp_table_cell_add_pango_layout (HwpTableCell *cell, PangoLayout *layout)
{
    g_return_if_fail (HWP_IS_TABLE_CELL (cell));
    g_return_if_fail (PANGO_IS_LAYOUT (layout));
    g_array_append_val (cell->layouts, layout);
}

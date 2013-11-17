/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-models.c
 *
 * Copyright (C) 2013 Hodong Kim <cogniti@gmail.com>
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
#include "hwp-parser.h"

#define _g_free0(var) (var = (g_free (var), NULL))

/** HWPText *****************************************************************/

G_DEFINE_TYPE (HWPText, hwp_text, G_TYPE_OBJECT);

HWPText *hwp_text_new (const gchar *text)
{
    g_return_val_if_fail (text != NULL, NULL);
    HWPText *hwp_text = (HWPText *) g_object_new (HWP_TYPE_TEXT, NULL);
    hwp_text->text = g_strdup (text);
    return hwp_text;
}

HWPText *hwp_text_append (HWPText *hwp_text, const gchar *text)
{
    g_return_val_if_fail (HWP_IS_TEXT (hwp_text), NULL);

    gchar *tmp;
    tmp = g_strdup (hwp_text->text);
    g_free (hwp_text->text);
    hwp_text->text = g_strconcat (tmp, text, NULL);
    g_free (tmp);
    return hwp_text;
}

static void hwp_text_finalize (GObject *obj)
{
    HWPText *hwp_text = HWP_TEXT(obj);
    _g_free0 (hwp_text->text);
    G_OBJECT_CLASS (hwp_text_parent_class)->finalize (obj);
}

static void hwp_text_class_init (HWPTextClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_text_finalize;
}

static void hwp_text_init (HWPText *hwp_text)
{
}

/** HWPParagraph ************************************************************/

G_DEFINE_TYPE (HWPParagraph, hwp_paragraph, G_TYPE_OBJECT);

HWPParagraph *hwp_paragraph_new (void)
{
    return (HWPParagraph *) g_object_new (HWP_TYPE_PARAGRAPH, NULL);
}

static void hwp_paragraph_finalize (GObject *obj)
{
    G_OBJECT_CLASS (hwp_paragraph_parent_class)->finalize (obj);
}

static void hwp_paragraph_class_init (HWPParagraphClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_paragraph_finalize;
}

static void hwp_paragraph_init (HWPParagraph *paragraph)
{
}

void
hwp_paragraph_set_hwp_text (HWPParagraph *paragraph, HWPText *hwp_text)
{
    g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
    g_return_if_fail (HWP_IS_TEXT (hwp_text));
    paragraph->hwp_text = hwp_text;
}

HWPText *hwp_paragraph_get_hwp_text (HWPParagraph *paragraph)
{
    g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), NULL);
    return paragraph->hwp_text;
}

void hwp_paragraph_set_table (HWPParagraph *paragraph, HWPTable *table)
{
    g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
    g_return_if_fail (HWP_IS_TABLE (table));
    paragraph->table = table;
}

HWPTable *hwp_paragraph_get_table (HWPParagraph *paragraph)
{
    g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), NULL);
    return paragraph->table;
}

/** HWPTable ****************************************************************/

G_DEFINE_TYPE (HWPTable, hwp_table, G_TYPE_OBJECT);

HWPTable *hwp_table_new (void)
{
    return g_object_new (HWP_TYPE_TABLE, NULL);
}

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

static void hwp_table_init (HWPTable *table)
{
    table->cells = g_array_new (TRUE, TRUE, sizeof (HWPTableCell *));
}

static void hwp_table_finalize (GObject *object)
{
    HWPTable *table = HWP_TABLE(object);
    _g_free0 (table->row_sizes);
    _g_free0 (table->zones);
    g_array_free (table->cells, TRUE);
    G_OBJECT_CLASS (hwp_table_parent_class)->finalize (object);
}

static void hwp_table_class_init (HWPTableClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_table_finalize;
}

HWPTableCell *hwp_table_get_last_cell (HWPTable *table)
{
    g_return_val_if_fail (HWP_IS_TABLE (table), NULL);
    return g_array_index (table->cells, HWPTableCell *,
                          table->cells->len - 1);
}

void hwp_table_add_cell (HWPTable *table, HWPTableCell *cell)
{
    g_return_if_fail (HWP_IS_TABLE (table));
    g_return_if_fail (HWP_IS_TABLE_CELL (cell));
    g_array_append_val (table->cells, cell);
}

/** HWPTableCell ************************************************************/

G_DEFINE_TYPE (HWPTableCell, hwp_table_cell, G_TYPE_OBJECT);

static void hwp_table_cell_init (HWPTableCell *cell)
{
    cell->paragraphs = g_array_new (TRUE, TRUE, sizeof (HWPParagraph *));
    cell->layouts    = g_array_new (TRUE, TRUE, sizeof (PangoLayout *));
}

static void hwp_table_cell_finalize (GObject *object)
{
    HWPTableCell *cell = HWP_TABLE_CELL(object);
    g_array_free (cell->paragraphs, TRUE);
    g_array_free (cell->layouts,    TRUE);
    G_OBJECT_CLASS (hwp_table_cell_parent_class)->finalize (object);
}

static void hwp_table_cell_class_init (HWPTableCellClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_table_cell_finalize;
}

HWPTableCell *hwp_table_cell_new (void)
{
    return g_object_new (HWP_TYPE_TABLE_CELL, NULL);
}

HWPParagraph *hwp_table_cell_get_last_paragraph (HWPTableCell *cell)
{
    g_return_val_if_fail (HWP_IS_TABLE_CELL (cell), NULL);
    return g_array_index (cell->paragraphs, HWPParagraph *,
                          cell->paragraphs->len - 1);
}

void
hwp_table_cell_add_paragraph (HWPTableCell *cell, HWPParagraph *paragraph)
{
    g_return_if_fail (HWP_IS_TABLE_CELL (cell));
    g_return_if_fail (HWP_IS_PARAGRAPH (paragraph));
    g_array_append_val (cell->paragraphs, paragraph);
}

void
hwp_table_cell_add_pango_layout (HWPTableCell *cell, PangoLayout *layout)
{
    g_return_if_fail (HWP_IS_TABLE_CELL (cell));
    g_return_if_fail (PANGO_IS_LAYOUT (layout));
    g_array_append_val (cell->layouts, layout);
}

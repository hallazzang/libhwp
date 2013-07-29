/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-models.c
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

#include "ghwp-models.h"

#define _g_free0(var) (var = (g_free (var), NULL))

/** GHWPText *****************************************************************/

G_DEFINE_TYPE (GHWPText, ghwp_text, G_TYPE_OBJECT);

GHWPText *ghwp_text_new (const gchar *text)
{
    g_return_val_if_fail (text != NULL, NULL);
    GHWPText *ghwp_text = (GHWPText *) g_object_new (GHWP_TYPE_TEXT, NULL);
    ghwp_text->text = g_strdup (text);
    return ghwp_text;
}

GHWPText *ghwp_text_append (GHWPText *ghwp_text, const gchar *text)
{
    g_return_val_if_fail (GHWP_IS_TEXT (ghwp_text), NULL);

    gchar *tmp;
    tmp = g_strdup (ghwp_text->text);
    g_free (ghwp_text->text);
    ghwp_text->text = g_strconcat (tmp, text, NULL);
    g_free (tmp);
    return ghwp_text;
}

static void ghwp_text_finalize (GObject *obj)
{
    GHWPText *ghwp_text = GHWP_TEXT(obj);
    _g_free0 (ghwp_text->text);
    G_OBJECT_CLASS (ghwp_text_parent_class)->finalize (obj);
}

static void ghwp_text_class_init (GHWPTextClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_text_finalize;
}

static void ghwp_text_init (GHWPText *ghwp_text)
{
}

/** GHWPParagraph ************************************************************/

G_DEFINE_TYPE (GHWPParagraph, ghwp_paragraph, G_TYPE_OBJECT);

GHWPParagraph *ghwp_paragraph_new (void)
{
    return (GHWPParagraph *) g_object_new (GHWP_TYPE_PARAGRAPH, NULL);
}

static void ghwp_paragraph_finalize (GObject *obj)
{
    G_OBJECT_CLASS (ghwp_paragraph_parent_class)->finalize (obj);
}

static void ghwp_paragraph_class_init (GHWPParagraphClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_paragraph_finalize;
}

static void ghwp_paragraph_init (GHWPParagraph *paragraph)
{
}

void
ghwp_paragraph_set_ghwp_text (GHWPParagraph *paragraph, GHWPText *ghwp_text)
{
    g_return_if_fail (GHWP_IS_PARAGRAPH (paragraph));
    g_return_if_fail (GHWP_IS_TEXT (ghwp_text));
    paragraph->ghwp_text = ghwp_text;
}

GHWPText *ghwp_paragraph_get_ghwp_text (GHWPParagraph *paragraph)
{
    g_return_val_if_fail (GHWP_IS_PARAGRAPH (paragraph), NULL);
    return paragraph->ghwp_text;
}

void ghwp_paragraph_set_table (GHWPParagraph *paragraph, GHWPTable *table)
{
    g_return_if_fail (GHWP_IS_PARAGRAPH (paragraph));
    g_return_if_fail (GHWP_IS_TABLE (table));
    paragraph->table = table;
}

GHWPTable *ghwp_paragraph_get_table (GHWPParagraph *paragraph)
{
    g_return_val_if_fail (GHWP_IS_PARAGRAPH (paragraph), NULL);
    return paragraph->table;
}

/** GHWPTable ****************************************************************/

G_DEFINE_TYPE (GHWPTable, ghwp_table, G_TYPE_OBJECT);

GHWPTable *ghwp_table_new (void)
{
    return g_object_new (GHWP_TYPE_TABLE, NULL);
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

GHWPTable *ghwp_table_new_from_context (GHWPContext *context)
{
    g_return_val_if_fail (GHWP_IS_CONTEXT (context), NULL);
    GHWPTable *table = ghwp_table_new ();

    context_read_uint32 (context, &table->flags);
    context_read_uint16 (context, &table->n_rows);
    context_read_uint16 (context, &table->n_cols);
    context_read_uint16 (context, &table->cell_spacing);
    context_read_uint16 (context, &table->left_margin);
    context_read_uint16 (context, &table->right_margin);
    context_read_uint16 (context, &table->top_margin);
    context_read_uint16 (context, &table->bottom_margin);

    table->row_sizes = g_malloc0_n (table->n_rows, 2);

    for (guint i = 0; i < table->n_rows; i++) {
        context_read_uint16 (context, &(table->row_sizes[i]));
    }

    context_read_uint16 (context, &table->border_fill_id);

    if (ghwp_document_check_version (context->document, 5, 0, 0, 7)) {
        context_read_uint16 (context, &table->valid_zone_info_size);

        table->zones = g_malloc0_n (table->valid_zone_info_size, 2);

        for (guint i = 0; i < table->valid_zone_info_size; i++) {
            context_read_uint16 (context, &(table->zones[i]));
        }
    }

    if (context->data_count != context->data_len) {
        g_warning ("%s:%d: TABLE data size mismatch at %s\n",
            __FILE__, __LINE__,
            ghwp_document_get_hwp_version_string(context->document));
    }
    return table;
}

static void ghwp_table_init (GHWPTable *table)
{
    table->cells = g_array_new (TRUE, TRUE, sizeof (GHWPTableCell *));
}

static void ghwp_table_finalize (GObject *object)
{
    GHWPTable *table = GHWP_TABLE(object);
    _g_free0 (table->row_sizes);
    _g_free0 (table->zones);
    g_array_free (table->cells, TRUE);
    G_OBJECT_CLASS (ghwp_table_parent_class)->finalize (object);
}

static void ghwp_table_class_init (GHWPTableClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_table_finalize;
}

GHWPTableCell *ghwp_table_get_last_cell (GHWPTable *table)
{
    g_return_val_if_fail (GHWP_IS_TABLE (table), NULL);
    return g_array_index (table->cells, GHWPTableCell *,
                          table->cells->len - 1);
}

void ghwp_table_add_cell (GHWPTable *table, GHWPTableCell *cell)
{
    g_return_if_fail (GHWP_IS_TABLE (table));
    g_return_if_fail (GHWP_IS_TABLE_CELL (cell));
    g_array_append_val (table->cells, cell);
}

/** GHWPTableCell ************************************************************/

G_DEFINE_TYPE (GHWPTableCell, ghwp_table_cell, G_TYPE_OBJECT);

static void ghwp_table_cell_init (GHWPTableCell *cell)
{
    cell->paragraphs = g_array_new (TRUE, TRUE, sizeof (GHWPParagraph *));
    cell->layouts    = g_array_new (TRUE, TRUE, sizeof (PangoLayout *));
}

static void ghwp_table_cell_finalize (GObject *object)
{
    GHWPTableCell *cell = GHWP_TABLE_CELL(object);
    g_array_free (cell->paragraphs, TRUE);
    g_array_free (cell->layouts,    TRUE);
    G_OBJECT_CLASS (ghwp_table_cell_parent_class)->finalize (object);
}

static void ghwp_table_cell_class_init (GHWPTableCellClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_table_cell_finalize;
}

GHWPTableCell *ghwp_table_cell_new (void)
{
    return g_object_new (GHWP_TYPE_TABLE_CELL, NULL);
}

GHWPTableCell *ghwp_table_cell_new_from_context (GHWPContext *context)
{
    g_return_val_if_fail (GHWP_IS_CONTEXT (context), NULL);

    GHWPTableCell *table_cell = ghwp_table_cell_new ();
    /* 표 60 LIST_HEADER */
    context_read_uint16 (context, &table_cell->n_paragraphs);
    context_read_uint32 (context, &table_cell->flags);
    context_read_uint16 (context, &table_cell->unknown1);
    /* 표 75 cell property */
    context_read_uint16 (context, &table_cell->col_addr);
    context_read_uint16 (context, &table_cell->row_addr);
    context_read_uint16 (context, &table_cell->col_span);
    context_read_uint16 (context, &table_cell->row_span);

    context_read_uint32 (context, &table_cell->width);
    context_read_uint32 (context, &table_cell->height);

    context_read_uint16 (context, &table_cell->left_margin);
    context_read_uint16 (context, &table_cell->right_margin);
    context_read_uint16 (context, &table_cell->top_margin);
    context_read_uint16 (context, &table_cell->bottom_margin);

    context_read_uint16 (context, &table_cell->border_fill_id);
    /* unknown */
    context_read_uint32 (context, &table_cell->unknown2);
/*    printf("%d %d %d\n%d %d %d %d\n%d %d\n%d %d %d %d\n%d\n",*/
/*        table_cell->n_paragraphs, table_cell->flags, table_cell->unknown,*/

/*        table_cell->col_addr,*/
/*        table_cell->row_addr,*/
/*        table_cell->col_span,*/
/*        table_cell->row_span,*/

/*        table_cell->width, table_cell->height,*/

/*        table_cell->left_margin,*/
/*        table_cell->right_margin,*/
/*        table_cell->top_margin,*/
/*        table_cell->bottom_margin,*/

/*        table_cell->border_fill_id);*/

    if (context->data_count != context->data_len) {
        g_warning ("%s:%d: table cell size mismatch\n", __FILE__, __LINE__);
    }

    return table_cell;
}

GHWPParagraph *ghwp_table_cell_get_last_paragraph (GHWPTableCell *cell)
{
    g_return_val_if_fail (GHWP_IS_TABLE_CELL (cell), NULL);
    return g_array_index (cell->paragraphs, GHWPParagraph *,
                          cell->paragraphs->len - 1);
}

void
ghwp_table_cell_add_paragraph (GHWPTableCell *cell, GHWPParagraph *paragraph)
{
    g_return_if_fail (GHWP_IS_TABLE_CELL (cell));
    g_return_if_fail (GHWP_IS_PARAGRAPH (paragraph));
    g_array_append_val (cell->paragraphs, paragraph);
}

void
ghwp_table_cell_add_pango_layout (GHWPTableCell *cell, PangoLayout *layout)
{
    g_return_if_fail (GHWP_IS_TABLE_CELL (cell));
    g_return_if_fail (PANGO_IS_LAYOUT (layout));
    g_array_append_val (cell->layouts, layout);
}

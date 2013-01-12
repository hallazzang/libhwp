/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-models.c
 *
 * Copyright (C) 2013  Hodong Kim <cogniti@gmail.com>
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

#include "ghwp-models.h"
#include <gio/gio.h>

#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

/** TextSpan *****************************************************************/

G_DEFINE_TYPE (TextSpan, text_span, G_TYPE_OBJECT);

TextSpan* text_span_new (const gchar *text)
{
    g_return_val_if_fail (text != NULL, NULL);
    TextSpan *textspan = (TextSpan*) g_object_new (TEXT_TYPE_SPAN, NULL);
    textspan->text = g_strdup (text);
    return textspan;
}

static void text_span_finalize (GObject *obj)
{
    TextSpan *textspan = TEXT_SPAN(obj);
    _g_free0 (textspan->text);
    G_OBJECT_CLASS (text_span_parent_class)->finalize (obj);
}

static void text_span_class_init (TextSpanClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = text_span_finalize;
}

static void text_span_init (TextSpan *textspan)
{
}

/** GHWPParagraph ************************************************************/

G_DEFINE_TYPE (GHWPParagraph, ghwp_paragraph, G_TYPE_OBJECT);

GHWPParagraph* ghwp_paragraph_new (void)
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

/** GHWPTable ****************************************************************/

G_DEFINE_TYPE (GHWPTable, ghwp_table, G_TYPE_OBJECT);

GHWPTable *ghwp_table_new (void)
{
    return (GHWPTable *) g_object_new (GHWP_TYPE_TABLE, NULL);
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
    GHWPTable *table = ghwp_table_new ();
    int        i;

    context_read_uint32 (context, &table->flags);
    context_read_uint16 (context, &table->row_count);
    context_read_uint16 (context, &table->col_count);
    context_read_uint16 (context, &table->cell_spacing);
    context_read_uint16 (context, &table->left_margin);
    context_read_uint16 (context, &table->right_margin);
    context_read_uint16 (context, &table->top_margin);
    context_read_uint16 (context, &table->bottom_margin);

/*    printf("%d %d %d %d %d %d %d %d\n", table->flags,*/
/*                                        table->row_count,*/
/*                                        table->col_count,*/
/*                                        table->cell_spacing,*/
/*                                        table->left_margin,*/
/*                                        table->right_margin,*/
/*                                        table->top_margin,*/
/*                                        table->bottom_margin);*/

    table->row_sizes = g_malloc0_n (table->row_count, 2);

    for (i = 0; i < table->row_count; i++) {
        context_read_uint16 (context, &(table->row_sizes[i]));
    }

    context_read_uint16 (context, &table->border_fill_id);
    context_read_uint16 (context, &table->valid_zone_info_size);

    table->zones = g_malloc0_n (table->valid_zone_info_size, 2);

    for (i = 0; i < table->valid_zone_info_size; i++) {
        context_read_uint16 (context, &(table->zones[i]));
    }

    if (context->data_count != context->data_len) {
        g_warning ("%s:%d: table size mismatch\n", __FILE__, __LINE__);
    }
    return table;
}

static void
ghwp_table_init (GHWPTable *ghwp_table)
{
}

static void
ghwp_table_finalize (GObject *object)
{
    GHWPTable *table = GHWP_TABLE(object);
    _g_free0 (table->row_sizes);
    _g_free0 (table->zones);
    G_OBJECT_CLASS (ghwp_table_parent_class)->finalize (object);
}

static void
ghwp_table_class_init (GHWPTableClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_table_finalize;
}

/** GHWPTableCell ************************************************************/

G_DEFINE_TYPE (GHWPTableCell, ghwp_table_cell, G_TYPE_OBJECT);

static void
ghwp_table_cell_init (GHWPTableCell *ghwp_table_cell)
{
}

static void
ghwp_table_cell_finalize (GObject *object)
{
    G_OBJECT_CLASS (ghwp_table_cell_parent_class)->finalize (object);
}

static void
ghwp_table_cell_class_init (GHWPTableCellClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_table_cell_finalize;
}

GHWPTableCell *ghwp_table_cell_new (void)
{
    return (GHWPTableCell *) g_object_new (GHWP_TYPE_TABLE_CELL, NULL);
}

GHWPTableCell *ghwp_table_cell_new_from_context (GHWPContext *context)
{
    GHWPTableCell *table_cell = ghwp_table_cell_new ();
    /* 표 60 */
    context_read_uint16 (context, &table_cell->n_paragraphs);
    context_read_uint32 (context, &table_cell->flags);
    context_read_uint16 (context, &table_cell->unknown);
    /* 표 75 */
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

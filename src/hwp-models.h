/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-table.c
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

#ifndef __GHWP_MODELS_H__
#define __GHWP_MODELS_H__

#include <glib-object.h>
#include <pango/pango-layout.h>

G_BEGIN_DECLS

typedef struct _GHWPText      GHWPText;
typedef struct _GHWPTable     GHWPTable;
typedef struct _GHWPTableCell GHWPTableCell;

/** GHWPParagraph ************************************************************/

#define GHWP_TYPE_PARAGRAPH             (hwp_paragraph_get_type ())
#define GHWP_PARAGRAPH(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PARAGRAPH, GHWPParagraph))
#define GHWP_PARAGRAPH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PARAGRAPH, GHWPParagraphClass))
#define GHWP_IS_PARAGRAPH(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PARAGRAPH))
#define GHWP_IS_PARAGRAPH_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PARAGRAPH))
#define GHWP_PARAGRAPH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PARAGRAPH, GHWPParagraphClass))

typedef struct _GHWPParagraph      GHWPParagraph;
typedef struct _GHWPParagraphClass GHWPParagraphClass;

struct _GHWPParagraph
{
    GObject    parent_instance;
    GHWPText  *hwp_text;
    GHWPTable *table;
};

struct _GHWPParagraphClass
{
    GObjectClass parent_class;
};

GType          hwp_paragraph_get_type      (void) G_GNUC_CONST;
GHWPParagraph *hwp_paragraph_new           (void);
void           hwp_paragraph_set_hwp_text (GHWPParagraph *paragraph,
                                             GHWPText      *hwp_text);
GHWPText      *hwp_paragraph_get_hwp_text (GHWPParagraph *paragraph);
GHWPTable     *hwp_paragraph_get_table     (GHWPParagraph *paragraph);
void           hwp_paragraph_set_table     (GHWPParagraph *paragraph,
                                             GHWPTable     *table);

/** GHWPText *****************************************************************/

#define GHWP_TYPE_TEXT             (hwp_text_get_type ())
#define GHWP_TEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_TEXT, GHWPText))
#define GHWP_TEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_TEXT, GHWPTextClass))
#define GHWP_IS_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_TEXT))
#define GHWP_IS_TEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_TEXT))
#define GHWP_TEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_TEXT, GHWPTextClass))

typedef struct _GHWPText        GHWPText;
typedef struct _GHWPTextClass   GHWPTextClass;
typedef struct _GHWPTextPrivate GHWPTextPrivate;

struct _GHWPText
{
    GObject          parent_instance;
    GHWPTextPrivate *priv;
    gchar           *text;
};

struct _GHWPTextClass
{
    GObjectClass parent_class;
};

GType     hwp_text_get_type (void) G_GNUC_CONST;
GHWPText *hwp_text_new      (const     gchar *text);
GHWPText *hwp_text_append   (GHWPText *hwp_text, const gchar *text);

/** GHWPTable ****************************************************************/

#define GHWP_TYPE_TABLE             (hwp_table_get_type ())
#define GHWP_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_TABLE, GHWPTable))
#define GHWP_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_TABLE, GHWPTableClass))
#define GHWP_IS_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_TABLE))
#define GHWP_IS_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_TABLE))
#define GHWP_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_TABLE, GHWPTableClass))

typedef struct _GHWPTable      GHWPTable;
typedef struct _GHWPTableClass GHWPTableClass;

struct _GHWPTableClass
{
    GObjectClass parent_class;
};

struct _GHWPTable
{
    GObject  parent_instance;
    guint32  flags;
    guint16  n_rows; /* 행 개수 */
    guint16  n_cols; /* 열 개수 */
    guint16  cell_spacing; /* 셀과 셀 사이의 간격 hwpuint */
    /* margin */
    guint16  left_margin;
    guint16  right_margin;
    guint16  top_margin;
    guint16  bottom_margin;

    guint16 *row_sizes;
    guint16  border_fill_id;
    guint16  valid_zone_info_size;
    guint16 *zones;

    GArray  *cells;
};

GType          hwp_table_get_type         (void) G_GNUC_CONST;
GHWPTable     *hwp_table_new              (void);
GHWPTableCell *hwp_table_get_last_cell    (GHWPTable     *table);
void           hwp_table_add_cell         (GHWPTable     *table,
                                            GHWPTableCell *cell);

/** GHWPTableCell ************************************************************/

#define GHWP_TYPE_TABLE_CELL             (hwp_table_cell_get_type ())
#define GHWP_TABLE_CELL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_TABLE_CELL, GHWPTableCell))
#define GHWP_TABLE_CELL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_TABLE_CELL, GHWPTableCellClass))
#define GHWP_IS_TABLE_CELL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_TABLE_CELL))
#define GHWP_IS_TABLE_CELL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_TABLE_CELL))
#define GHWP_TABLE_CELL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_TABLE_CELL, GHWPTableCellClass))

typedef struct _GHWPTableCell      GHWPTableCell;
typedef struct _GHWPTableCellClass GHWPTableCellClass;

struct _GHWPTableCell
{
    GObject parent_instance;
    /* 표 60 list header */
    guint16 n_paragraphs;
    guint32 flags;
    guint16 unknown1;
    /* 표 75 cell property */
    guint16 col_addr;
    guint16 row_addr;
    guint16 col_span;
    guint16 row_span;

    guint32 width;  /* hwpunit */
    guint32 height; /* hwpunit */

    guint16 left_margin;
    guint16 right_margin;
    guint16 top_margin;
    guint16 bottom_margin;

    guint16 border_fill_id;

    guint32 unknown2;

    /* private use */
    gdouble _y;
    GArray *paragraphs;
    GArray *layouts;
};

struct _GHWPTableCellClass
{
    GObjectClass parent_class;
};

GType          hwp_table_cell_get_type           (void) G_GNUC_CONST;
GHWPTableCell *hwp_table_cell_new                (void);
GHWPParagraph *hwp_table_cell_get_last_paragraph (GHWPTableCell    *cell);
void           hwp_table_cell_add_paragraph      (GHWPTableCell    *cell,
                                                   GHWPParagraph    *paragraph);
void           hwp_table_cell_add_pango_layout   (GHWPTableCell    *cell,
                                                   PangoLayout      *layout);

G_END_DECLS

#endif /* __GHWP_MODELS_H__ */

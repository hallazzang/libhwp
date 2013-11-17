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

#ifndef __HWP_MODELS_H__
#define __HWP_MODELS_H__

#include <glib-object.h>
#include <pango/pango-layout.h>

G_BEGIN_DECLS

typedef struct _HWPText      HWPText;
typedef struct _HWPTable     HWPTable;
typedef struct _HWPTableCell HWPTableCell;

/** HWPParagraph ************************************************************/

#define HWP_TYPE_PARAGRAPH             (hwp_paragraph_get_type ())
#define HWP_PARAGRAPH(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_PARAGRAPH, HWPParagraph))
#define HWP_PARAGRAPH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_PARAGRAPH, HWPParagraphClass))
#define HWP_IS_PARAGRAPH(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_PARAGRAPH))
#define HWP_IS_PARAGRAPH_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_PARAGRAPH))
#define HWP_PARAGRAPH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_PARAGRAPH, HWPParagraphClass))

typedef struct _HWPParagraph      HWPParagraph;
typedef struct _HWPParagraphClass HWPParagraphClass;

struct _HWPParagraph
{
    GObject    parent_instance;
    HWPText  *hwp_text;
    HWPTable *table;
};

struct _HWPParagraphClass
{
    GObjectClass parent_class;
};

GType          hwp_paragraph_get_type      (void) G_GNUC_CONST;
HWPParagraph *hwp_paragraph_new           (void);
void           hwp_paragraph_set_hwp_text (HWPParagraph *paragraph,
                                             HWPText      *hwp_text);
HWPText      *hwp_paragraph_get_hwp_text (HWPParagraph *paragraph);
HWPTable     *hwp_paragraph_get_table     (HWPParagraph *paragraph);
void           hwp_paragraph_set_table     (HWPParagraph *paragraph,
                                             HWPTable     *table);

/** HWPText *****************************************************************/

#define HWP_TYPE_TEXT             (hwp_text_get_type ())
#define HWP_TEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TEXT, HWPText))
#define HWP_TEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TEXT, HWPTextClass))
#define HWP_IS_TEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TEXT))
#define HWP_IS_TEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TEXT))
#define HWP_TEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TEXT, HWPTextClass))

typedef struct _HWPText        HWPText;
typedef struct _HWPTextClass   HWPTextClass;
typedef struct _HWPTextPrivate HWPTextPrivate;

struct _HWPText
{
    GObject          parent_instance;
    HWPTextPrivate *priv;
    gchar           *text;
};

struct _HWPTextClass
{
    GObjectClass parent_class;
};

GType     hwp_text_get_type (void) G_GNUC_CONST;
HWPText *hwp_text_new      (const     gchar *text);
HWPText *hwp_text_append   (HWPText *hwp_text, const gchar *text);

/** HWPTable ****************************************************************/

#define HWP_TYPE_TABLE             (hwp_table_get_type ())
#define HWP_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TABLE, HWPTable))
#define HWP_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TABLE, HWPTableClass))
#define HWP_IS_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TABLE))
#define HWP_IS_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TABLE))
#define HWP_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TABLE, HWPTableClass))

typedef struct _HWPTable      HWPTable;
typedef struct _HWPTableClass HWPTableClass;

struct _HWPTableClass
{
    GObjectClass parent_class;
};

struct _HWPTable
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
HWPTable     *hwp_table_new              (void);
HWPTableCell *hwp_table_get_last_cell    (HWPTable     *table);
void           hwp_table_add_cell         (HWPTable     *table,
                                            HWPTableCell *cell);

/** HWPTableCell ************************************************************/

#define HWP_TYPE_TABLE_CELL             (hwp_table_cell_get_type ())
#define HWP_TABLE_CELL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TABLE_CELL, HWPTableCell))
#define HWP_TABLE_CELL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TABLE_CELL, HWPTableCellClass))
#define HWP_IS_TABLE_CELL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TABLE_CELL))
#define HWP_IS_TABLE_CELL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TABLE_CELL))
#define HWP_TABLE_CELL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TABLE_CELL, HWPTableCellClass))

typedef struct _HWPTableCell      HWPTableCell;
typedef struct _HWPTableCellClass HWPTableCellClass;

struct _HWPTableCell
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

struct _HWPTableCellClass
{
    GObjectClass parent_class;
};

GType          hwp_table_cell_get_type           (void) G_GNUC_CONST;
HWPTableCell *hwp_table_cell_new                (void);
HWPParagraph *hwp_table_cell_get_last_paragraph (HWPTableCell    *cell);
void           hwp_table_cell_add_paragraph      (HWPTableCell    *cell,
                                                   HWPParagraph    *paragraph);
void           hwp_table_cell_add_pango_layout   (HWPTableCell    *cell,
                                                   PangoLayout      *layout);

G_END_DECLS

#endif /* __HWP_MODELS_H__ */

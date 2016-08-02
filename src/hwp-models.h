/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-models.h
 *
 * Copyright (C) 2012-2016 Hodong Kim <cogniti@gmail.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This software has been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_MODELS_H__
#define __HWP_MODELS_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* HwpSummaryInfo **********************************************************/

#define HWP_TYPE_SUMMARY_INFO             (hwp_summary_info_get_type ())
#define HWP_SUMMARY_INFO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_SUMMARY_INFO, HwpSummaryInfo))
#define HWP_SUMMARY_INFO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_SUMMARY_INFO, HwpSummaryInfoClass))
#define HWP_IS_SUMMARY_INFO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_SUMMARY_INFO))
#define HWP_IS_SUMMARY_INFO_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_SUMMARY_INFO))
#define HWP_SUMMARY_INFO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_SUMMARY_INFO, HwpSummaryInfoClass))

typedef struct _HwpSummaryInfo       HwpSummaryInfo;
typedef struct _HwpSummaryInfoClass  HwpSummaryInfoClass;

struct _HwpSummaryInfo
{
  GObject parent_instance;

  char  *title;
  char  *format;
  char  *author;
  char  *subject;
  char  *keywords;
  char  *layout;
  char  *start_mode;
  char  *permissions;
  char  *ui_hints;
  char  *creator;
  char  *producer;
  GTime  creation_date;
  GTime  mod_date;
  char  *linearized;
  char  *security;
  char  *paper_size;
  char  *license;
  /* hwp info */
  char  *desc;
  GTime  last_printed;
  char  *last_saved_by;
  /* version of hanword */
  char  *hanword_version;
};

/**
 * HwpSummaryInfoClass:
 * @parent_class: the parent class
 *
 * The class structure for the <structname>HwpSummaryInfo</structname> type.
 */
struct _HwpSummaryInfoClass
{
  GObjectClass parent_class;
};

GType hwp_summary_info_get_type (void) G_GNUC_CONST;

HwpSummaryInfo *hwp_summary_info_new (void);

/****************************************************************************/

typedef struct _HwpTable     HwpTable;
typedef struct _HwpTableCell HwpTableCell;
typedef struct _HwpSecd      HwpSecd;

/* HwpParagraph ************************************************************/

#define HWP_TYPE_PARAGRAPH             (hwp_paragraph_get_type ())
#define HWP_PARAGRAPH(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_PARAGRAPH, HwpParagraph))
#define HWP_PARAGRAPH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_PARAGRAPH, HwpParagraphClass))
#define HWP_IS_PARAGRAPH(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_PARAGRAPH))
#define HWP_IS_PARAGRAPH_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_PARAGRAPH))
#define HWP_PARAGRAPH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_PARAGRAPH, HwpParagraphClass))

typedef struct _HwpParagraph      HwpParagraph;
typedef struct _HwpParagraphClass HwpParagraphClass;

struct _HwpParagraph
{
  GObject    parent_instance;

  guint32    n_chars;
  guint32    control_mask;
  guint16    para_shape_id;
  guint8     para_style_id;
  guint8     column_type;
  guint16    n_char_shapes;
  guint16    n_range_tags;
  guint16    n_aligns;
  guint32    para_instance_id;
  guint16    track; /* 변경 추적 병합 문단 여부 (5.0.3.2 버전 이상) */

  char      *text;
  HwpTable  *table;
  HwpSecd   *secd;

  guint32   *m_pos;
  guint32   *m_id;
  guint16    m_len;
  GPtrArray *text_attrs;
};

/**
 * HwpParagraphClass:
 * @parent_class: the parent class
 *
 * The class structure for the <structname>HwpParagraph</structname> type.
 */
struct _HwpParagraphClass
{
  GObjectClass parent_class;
};

GType         hwp_paragraph_get_type   (void) G_GNUC_CONST;
HwpParagraph *hwp_paragraph_new        (void);
void          hwp_paragraph_set_text   (HwpParagraph *paragraph,
                                        const gchar  *text);
const char   *hwp_paragraph_get_text   (HwpParagraph *paragraph);
HwpTable     *hwp_paragraph_get_table  (HwpParagraph *paragraph);
void          hwp_paragraph_set_table  (HwpParagraph *paragraph,
                                        HwpTable     *table);
void          hwp_paragraph_set_secd   (HwpParagraph *paragraph,
                                        HwpSecd      *secd);

/* HwpTable ****************************************************************/

#define HWP_TYPE_TABLE             (hwp_table_get_type ())
#define HWP_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TABLE, HwpTable))
#define HWP_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TABLE, HwpTableClass))
#define HWP_IS_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TABLE))
#define HWP_IS_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TABLE))
#define HWP_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TABLE, HwpTableClass))

typedef struct _HwpTable      HwpTable;
typedef struct _HwpTableClass HwpTableClass;

struct _HwpTable
{
  GObject    parent_instance;
  guint32    flags;
  guint16    n_rows; /* 행 개수 */
  guint16    n_cols; /* 열 개수 */
  guint16    cell_spacing; /* 셀과 셀 사이의 간격 hwpuint */
  /* margin */
  guint16    left_margin;
  guint16    right_margin;
  guint16    top_margin;
  guint16    bottom_margin;

  guint16   *row_sizes;
  guint16    border_fill_id;
  guint16    valid_zone_info_size;
  guint16   *zones;

  GPtrArray *rows;
};

/**
 * HwpTableClass:
 * @parent_class: the parent class
 *
 * The class structure for the <structname>HwpTable</structname> type.
 */
struct _HwpTableClass
{
  GObjectClass parent_class;
};

GType         hwp_table_get_type         (void) G_GNUC_CONST;
HwpTable     *hwp_table_new              (void);
HwpTableCell *hwp_table_get_last_cell    (HwpTable     *table);
void          hwp_table_add_cell         (HwpTable     *table,
                                          HwpTableCell *cell,
                                          guint         index);

/* HwpTableCell ************************************************************/

#define HWP_TYPE_TABLE_CELL             (hwp_table_cell_get_type ())
#define HWP_TABLE_CELL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TABLE_CELL, HwpTableCell))
#define HWP_TABLE_CELL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TABLE_CELL, HwpTableCellClass))
#define HWP_IS_TABLE_CELL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TABLE_CELL))
#define HWP_IS_TABLE_CELL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TABLE_CELL))
#define HWP_TABLE_CELL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TABLE_CELL, HwpTableCellClass))

typedef struct _HwpTableCell      HwpTableCell;
typedef struct _HwpTableCellClass HwpTableCellClass;

struct _HwpTableCell
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
  GPtrArray *paragraphs;
};

/**
 * HwpTableCellClass:
 * @parent_class: the parent class
 *
 * The class structure for the <structname>HwpTableCell</structname> type.
 */
struct _HwpTableCellClass
{
  GObjectClass parent_class;
};

GType         hwp_table_cell_get_type           (void) G_GNUC_CONST;
HwpTableCell *hwp_table_cell_new                (void);
HwpParagraph *hwp_table_cell_get_last_paragraph (HwpTableCell *cell);
void          hwp_table_cell_add_paragraph      (HwpTableCell *cell,
                                                 HwpParagraph *paragraph);

/**
 * HwpSecd:
 * @page_width_in_points: page width in points
 * @page_height_in_points: page height in points
 * @page_left_margin_in_points: page left margin in points
 * @page_right_margin_in_points: page right margin in points
 * @page_top_margin_in_points:page top margin in points
 * @page_bottom_margin_in_points: page bottom margin in points
 * @page_header_margin_in_points: page header margin in points
 * @page_footer_margin_in_points: page footer margin in points
 * @page_gutter_margin_in_points: page gutter margin in points
 * @page_prop: page property
 *
 * The structure for the <structname>HwpSecd</structname> type.
 */
typedef struct _HwpSecd HwpSecd;
struct _HwpSecd
{
  gdouble page_width_in_points;
  gdouble page_height_in_points;
  gdouble page_left_margin_in_points;
  gdouble page_right_margin_in_points;
  gdouble page_top_margin_in_points;
  gdouble page_bottom_margin_in_points;
  gdouble page_header_margin_in_points;
  gdouble page_footer_margin_in_points;
  gdouble page_gutter_margin_in_points;
  gdouble page_prop;
};

GType    hwp_secd_get_type (void) G_GNUC_CONST;
HwpSecd *hwp_secd_new      (void);
HwpSecd *hwp_secd_copy     (HwpSecd *secd);
void     hwp_secd_free     (HwpSecd *secd);

/**
 * HwpColor:
 * @red: the red componment of color
 * @green: the green component of color
 * @blue: the blue component of color
 *
 * A #HwpColor describes a RGB color. Color components
 * are values between 0 and 65535
 */
typedef struct _HwpColor           HwpColor;
struct _HwpColor
{
  guint16 red;
  guint16 green;
  guint16 blue;
};

#define HWP_TYPE_COLOR                 (hwp_color_get_type ())

GType     hwp_color_get_type (void) G_GNUC_CONST;
HwpColor *hwp_color_new      (void);
HwpColor *hwp_color_copy     (HwpColor *color);
void      hwp_color_free     (HwpColor *color);

/**
 * HwpCharShape:
 * @face_id: face id
 * @ratio: ratio
 * @char_spacing: character spacing
 * @rel_size: relative size
 * @char_offset: character offset
 * @height_in_points: height in points
 * @prop: property
 * @space_between_shadows1: space between shadows1
 * @space_between_shadows2: space between shadows2
 * @text_color: text color
 * @underline_color: underline color
 * @shade_color: shade color
 * @shadow_color: shadow color
 * @border_fill_id: border fill id, if the version of hwp document file is the
 *                  same as or newer than 5.0.2.1
 * @strike_through_color: strike-through color, if the version of hwp document
 *                        file is the same as or newer than 5.0.3.0
 *
 * The structure for the <structname>HwpCharShape</structname> type.
 */
typedef struct _HwpCharShape HwpCharShape;
struct _HwpCharShape
{
  guint16  face_id[7];
  guint8   ratio[7];               /* 장평 */
  guint8   char_spacing[7];        /* 자간 */
  guint8   rel_size[7];            /* 상대 크기 */
  guint8   char_offset[7];         /* 글자 위치 */
  gdouble  height_in_points;       /* guint32 / 7200.0 * 72 */
  guint32  prop;                   /* 속성 */
  guint8   space_between_shadows1; /* 그림자 간격 */
  guint8   space_between_shadows2; /* 그림자 간격 */
  HwpColor text_color;             /* 글자 색 */
  HwpColor underline_color;        /* 밑줄 색 */
  HwpColor shade_color;            /* 음영 색 */
  HwpColor shadow_color;           /* 그림자 색 */
  guint16  border_fill_id;         /* 글자 테두리/배경 5.0.2.1 이상 */
  HwpColor strike_through_color;   /* 취소선 색        5.0.3.0 이상 */
};

GType         hwp_char_shape_get_type (void) G_GNUC_CONST;
HwpCharShape *hwp_char_shape_new      (void);
HwpCharShape *hwp_char_shape_copy     (HwpCharShape *char_shape);
void          hwp_char_shape_free     (HwpCharShape *char_shape);

/**
 * HwpParaShape:
 * @prop1: property1
 * @left_margin: left margin
 * @right_margin: right margin
 * @indent_margin: indent margin
 * @prev_margin: prev margin
 * @next_margin: next margin
 * @line_spacing1: if version < 5.0.2.5
 * @tabdef_id: tabdef id
 * @numbering_id: numbering id
 * @border_fill_id: border fill id
 * @border_offset_left: border offset left
 * @border_offset_right: border offset right
 * @border_offset_top: border offset top
 * @border_offset_bottom: border offset bottom
 * @prop2: property2 if version >= 5.0.1.7
 * @prop3: property3 if version >= 5.0.2.5
 * @line_spacing2:  if version >= 5.0.2.5
 *
 * The structure for the <structname>HwpParaShape</structname> type.
 */
typedef struct _HwpParaShape HwpParaShape;
struct _HwpParaShape
{
  guint32 prop1;
  gint32  left_margin;
  gint32  right_margin;
  gint32  indent_margin;
  gint32  prev_margin;
  gint32  next_margin;
  gint32  line_spacing1; /* version < 5.0.2.5 */
  guint16 tabdef_id;
  guint16 numbering_id;
  guint16 border_fill_id;
  gint16  border_offset_left;
  gint16  border_offset_right;
  gint16  border_offset_top;
  gint16  border_offset_bottom;
  guint32 prop2;  /* version >= 5.0.1.7 */
  guint32 prop3;  /* version >= 5.0.2.5 */
  guint32 line_spacing2;  /* version >= 5.0.2.5 */
};

GType         hwp_para_shape_get_type (void) G_GNUC_CONST;
HwpParaShape *hwp_para_shape_new      (void);
HwpParaShape *hwp_para_shape_copy     (HwpParaShape *para_shape);
void          hwp_para_shape_free     (HwpParaShape *para_shape);

/**
 * HwpFaceName:
 * @prop1: property
 * @len1: length1
 * @font_name: font name
 *
 * The structure for the <structname>HwpFaceName</structname> type.
 */
typedef struct _HwpFaceName HwpFaceName;
struct _HwpFaceName
{
  guint8  prop1;
  guint16 len1;
  gchar  *font_name;
};

GType        hwp_face_name_get_type (void) G_GNUC_CONST;
HwpFaceName *hwp_face_name_new      (void);
HwpFaceName *hwp_face_name_copy     (HwpFaceName *face_name);
void         hwp_face_name_free     (HwpFaceName *face_name);

/**
 * HwpBinData:
 * @id: id
 * @format: format
 *
 * The structure for the <structname>HwpBinData</structname> type.
 */
typedef struct _HwpBinData HwpBinData;
struct _HwpBinData
{
  guint16 id;
  gchar  *format;
};

GType       hwp_bin_data_get_type (void) G_GNUC_CONST;
HwpBinData *hwp_bin_data_new      (void);
HwpBinData *hwp_bin_data_copy     (HwpBinData *bin_data);
void        hwp_bin_data_free     (HwpBinData *bin_data);

/**
 * HwpCommonProperty:
 * @ctrl_id: control id
 * @prop: property
 * @y_offset: y offset
 * @x_offset: x offset
 * @width: width
 * @height: height
 * @z_order: z order
 * @margin1: margin1
 * @margin2: margin2
 * @margin3: margin3
 * @margin4: margin4
 * @instance_id: instance id
 * @len: length
 *
 * The structure for the <structname>HwpCommonProperty</structname> type.
 */
typedef struct _HwpCommonProperty HwpCommonProperty;
struct _HwpCommonProperty
{
  guint32 ctrl_id;
  guint32 prop;
  guint32 y_offset;
  guint32 x_offset;
  guint32 width;
  guint32 height;
  guint32 z_order;
  guint16 margin1;
  guint16 margin2;
  guint16 margin3;
  guint16 margin4;
  guint32 instance_id;
  guint32 len;
};

GType              hwp_common_property_get_type (void) G_GNUC_CONST;
HwpCommonProperty *hwp_common_property_new      (void);
HwpCommonProperty *hwp_common_property_copy     (HwpCommonProperty *prop);
void               hwp_common_property_free     (HwpCommonProperty *prop);

typedef struct _HwpPoint           HwpPoint;
typedef struct _HwpRectangle       HwpRectangle;
typedef struct _HwpTextAttributes  HwpTextAttributes;
typedef struct _HwpLayout          HwpLayout;

/* A point on a page */
#define HWP_TYPE_POINT             (hwp_point_get_type ())
/**
 * HwpPoint:
 * @x: x coordinate
 * @y: y coordinate
 */
struct _HwpPoint
{
  gdouble x;
  gdouble y;
};

GType     hwp_point_get_type (void) G_GNUC_CONST;
HwpPoint *hwp_point_new      (gdouble x, gdouble y);
HwpPoint *hwp_point_copy     (HwpPoint  *point);
void      hwp_point_free     (HwpPoint  *point);

/* A rectangle on a page */
#define HWP_TYPE_RECTANGLE             (hwp_rectangle_get_type ())
/**
 * HwpRectangle:
 * @x1: x coordinate of lower left corner
 * @y1: y coordinate of lower left corner
 * @x2: x coordinate of upper right corner
 * @y2: y coordinate of upper right corner
 *
 * A #HwpRectangle is used to describe
 * locations on a page and bounding boxes
 */
struct _HwpRectangle
{
  gdouble x1;
  gdouble y1;
  gdouble x2;
  gdouble y2;
};

GType         hwp_rectangle_get_type (void) G_GNUC_CONST;
HwpRectangle *hwp_rectangle_new      (void);
HwpRectangle *hwp_rectangle_copy     (HwpRectangle *rectangle);
void          hwp_rectangle_free     (HwpRectangle *rectangle);

/* Text attributes. */
#define HWP_TYPE_TEXT_ATTRIBUTES       (hwp_text_attributes_get_type ())
/**
 * HwpTextAttributes:
 * @font_name: font name
 * @font_size: font size
 * @is_underlined: if text is underlined
 * @color: a #HwpColor, the foreground color
 * @start_index: start position this text attributes apply
 * @end_index: end position this text text attributes apply, the character at end_index is not included
 *
 * A #HwpTextAttributes is used to describe text attributes of a range of text
 *
 * Since: 0.0.4
 */
struct _HwpTextAttributes
{
  gchar   *font_name;
  gdouble  font_size;
  gboolean is_underlined;
  HwpColor color;

  gint     start_index;
  gint     end_index;
};

GType              hwp_text_attributes_get_type (void) G_GNUC_CONST;
HwpTextAttributes *hwp_text_attributes_new      (void);
HwpTextAttributes *hwp_text_attributes_copy     (HwpTextAttributes *text_attrs);
void               hwp_text_attributes_free     (HwpTextAttributes *text_attrs);

G_END_DECLS

#endif /* __HWP_MODELS_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp5-parser.c
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
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

#include "config.h"
#include <glib/gi18n-lib.h>
#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>
#include <gsf/gsf-msole-utils.h>
#include <stdio.h>
#include "gsf-input-stream.h"
#include "hwp-hwp5-parser.h"

G_DEFINE_TYPE (HwpHWP5Parser, hwp_hwp5_parser, G_TYPE_OBJECT);

static void hwp_hwp5_parser_parse_paragraph (HwpHWP5Parser *parser,
                                             HwpHWP5File   *file,
                                             GError       **error);

gboolean parser_skip (HwpHWP5Parser *parser, guint16 count)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), FALSE);

  gboolean is_success = FALSE;
  /*
   * FIXME: g_input_stream_skip 가 작동하지 않아 아래처럼 처리했다.
   * g_input_stream_skip를 제대로 작동시키려면 컨버터 스트림을 손봐야 한다.
   */
  guint8 *buf = g_malloc (count);
  is_success = g_input_stream_read_all (parser->stream, buf, (gsize) count,
                                        &parser->priv->bytes_read,
                                        NULL, NULL);
  g_free (buf);

  if ((is_success == FALSE) || (parser->priv->bytes_read != (gsize) count))
  {
    g_warning ("%s:%d:skip size mismatch\n", __FILE__, __LINE__);
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }

  parser->data_count += count;
  return TRUE;
}

gboolean parser_read_uint16 (HwpHWP5Parser *parser, guint16 *i, GError **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), FALSE);

#ifdef HWP_ENABLE_DEBUG
  g_assert (parser->data_count <= parser->data_len - 2);
#endif
  g_return_val_if_fail (parser->data_count <= parser->data_len - 2, FALSE);

  g_input_stream_read_all (parser->stream, i, 2,
                           &parser->priv->bytes_read, NULL, error);
  if (*error || (parser->priv->bytes_read != 2)) {
    *i = 0;
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }

  *i = GUINT16_FROM_LE(*i);
  parser->data_count += 2;
  return TRUE;
}

gboolean parser_read_uint32 (HwpHWP5Parser *parser, guint32 *i, GError **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), FALSE);
  g_return_val_if_fail (parser->data_count <= parser->data_len - 4, FALSE);

  g_input_stream_read_all (parser->stream, i, 4,
                           &parser->priv->bytes_read, NULL, error);
  if (*error || (parser->priv->bytes_read != 4)) {
    *i = 0;
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }
  *i = GUINT32_FROM_LE(*i);
  parser->data_count += 4;
  return TRUE;
}

/**
 * hwp_hwp5_parser_pull:
 * @parser: #HwpHWP5Parser
 * @error: #GError
 *
 * On a successful pull, %TURE is returned.
 *
 * If we reached the end of the stream %FALSE is returned and error is not set.
 *
 * If there is an error during the operation
 * %FALSE is returned and error is set to indicate the error status.
 *
 * Returns: %TRUE on success, %FALSE if there was an error or end-of-stream
 *
 * Since: 0.0.1
 */
gboolean hwp_hwp5_parser_pull (HwpHWP5Parser *parser, GError **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), FALSE);

  if (parser->state == HWP_PARSE_STATE_PASSING) {
    parser->state = HWP_PARSE_STATE_NORMAL;
    return TRUE;
  }

  if (parser->data_len - parser->data_count > 0)
    parser_skip (parser, parser->data_len - parser->data_count);

  if (g_input_stream_is_closed (parser->stream))
    return FALSE;

  /* 4바이트 읽기 */
  g_input_stream_read_all (parser->stream, &parser->priv->header, 4,
                           &parser->priv->bytes_read, NULL, error);
  if (*error) {
    if (!g_input_stream_is_closed (parser->stream))
      g_input_stream_close (parser->stream, NULL, NULL);

    return FALSE;
  }
  /* if invalid or end-of-stream */
  if (parser->priv->bytes_read != 4) {
    /* if not end-of-stream */
    if (parser->priv->bytes_read != 0)
      g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                           _("File corrupted or invalid file"));

    if (!g_input_stream_is_closed (parser->stream))
      g_input_stream_close (parser->stream, NULL, NULL);

    return FALSE;
  }

  /* Converts a guint32 value from little-endian to host byte order. */
  parser->priv->header = GUINT32_FROM_LE(parser->priv->header);
  /* 4바이트 헤더 디코딩하기 */
  parser->tag_id   = (guint16) ( parser->priv->header        & 0x3ff);
  parser->level    = (guint16) ((parser->priv->header >> 10) & 0x3ff);
  parser->data_len = (guint16) ((parser->priv->header >> 20) & 0xfff);
  /* 비정상 */
  if (parser->data_len == 0) {
    g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                         _("File corrupted"));
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }
  /* data_len == 0xfff 이면 다음 4바이트는 data_len 이다 */
  if (parser->data_len == 0xfff) {
    g_input_stream_read_all (parser->stream, &parser->data_len, 4,
                             &parser->priv->bytes_read, NULL, error);
    if (*error) {
      g_input_stream_close (parser->stream, NULL, NULL);
      return FALSE;
    }

    /* 비정상 */
    if (parser->priv->bytes_read != 4) {
      g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                           _("File corrupted"));
      g_input_stream_close (parser->stream, NULL, NULL);
      return FALSE;
    }

    parser->data_len = GUINT32_FROM_LE(parser->data_len);
  }

#ifdef HWP_ENABLE_DEBUG
  printf ("%d", parser->level);
  for (int i = 0; i < parser->level; i++)
    printf ("    ");
  printf ("%s\n", hwp_get_tag_name(parser->tag_id));
#endif

  parser->data_count = 0;

  return TRUE;
}

/**
 * hwp_hwp5_parser_new:
 * @listener:
 * @user_data
 *
 * Returns:
 *
 * Since: 0.0.1
 */
HwpHWP5Parser *hwp_hwp5_parser_new (HwpListener *listener, gpointer user_data)
{
  g_return_val_if_fail (HWP_IS_LISTENER (listener), NULL);

  HwpHWP5Parser *parser = g_object_new (HWP_TYPE_HWP5_PARSER, NULL);
  parser->listener      = listener;
  parser->user_data     = user_data;

  return parser;
}

static void hwp_hwp5_parser_parse_doc_info (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint8 id_mappings_len;

  if (hwp_hwp5_parser_check_version (parser, 5, 0, 0, 7))
    id_mappings_len = 16;
  else
    id_mappings_len = 14;

  guint32 *id_mappings = g_malloc0_n (id_mappings_len, sizeof(guint32));

  parser->stream = file->doc_info_stream;

  while (hwp_hwp5_parser_pull (parser, error)) {
    switch (parser->tag_id) {
    case HWP_TAG_DOCUMENT_PROPERTIES:
      break;
    case HWP_TAG_ID_MAPPINGS:
      for (guint8 i = 0; i < id_mappings_len; i++)
        parser_read_uint32 (parser, &id_mappings[i], error);
      g_free (id_mappings);
      break;
    case HWP_TAG_BIN_DATA:

      break;
    case HWP_TAG_FACE_NAME:
      break;
    case HWP_TAG_BORDER_FILL:
      break;
    case HWP_TAG_CHAR_SHAPE:
      break;
    case HWP_TAG_TAB_DEF:
      break;
    case HWP_TAG_NUMBERING:
      break;
    case HWP_TAG_BULLET:
      break;
    case HWP_TAG_PARA_SHAPE:
      break;
    case HWP_TAG_STYLE:
      break;
    case HWP_TAG_DOC_DATA:
      break;
    case HWP_TAG_FORBIDDEN_CHAR:
      break;
    case HWP_TAG_COMPATIBLE_DOCUMENT:
      break;
    case HWP_TAG_LAYOUT_COMPATIBILITY:
      break;
    default:
      printf("%s:%d: %s not implemented\n", __FILE__, __LINE__,
          hwp_get_tag_name (parser->tag_id));
      break;
    }
  }
}

static void hwp_hwp5_parser_parse_section_definition (HwpHWP5Parser *parser,
                                                      GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
        parser->state = HWP_PARSE_STATE_PASSING;
        break;
    }

    g_assert (parser->level == 2);

    switch (parser->tag_id) {
    case HWP_TAG_PAGE_DEF:
      break;
    case HWP_TAG_FOOTNOTE_SHAPE:
      break;
    case HWP_TAG_PAGE_BORDER_FILL:
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
        __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

/* 머리말 */
static void hwp_hwp5_parser_parse_header (HwpHWP5Parser *parser,
                                          HwpHWP5File   *file,
                                          GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_LIST_HEADER:
      break;
    case HWP_TAG_PARA_HEADER:
        hwp_hwp5_parser_parse_paragraph (parser, file, error);
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
        __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

/* 각주 */
static void hwp_hwp5_parser_parse_footnote (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_LIST_HEADER:
      break;
    case HWP_TAG_PARA_HEADER:
      hwp_hwp5_parser_parse_paragraph (parser, file, error);
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
        __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_tcmt (HwpHWP5Parser *parser,
                                        HwpHWP5File   *file,
                                        GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_LIST_HEADER:
      break;
    case HWP_TAG_PARA_HEADER:
      hwp_hwp5_parser_parse_paragraph (parser, file, error);
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
          __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

/*
 *         col 0   col 1
 *       +-------+-------+
 * row 0 |  00   |   01  |
 *       +-------+-------+
 * row 1 |  10   |   11  |
 *       +-------+-------+
 * row 2 |  20   |   21  |
 *       +-------+-------+
 *
 * <table> ::= { <list-header> <para-header>+ }+
 *
 * para-header
 *  ...
 *  ctrl-header (id:tbl)
 *   table: row-count, col-count
 *   list-header (00)
 *   ...
 *   list-header (01)
 *   ...
 *   list-header (10)
 *   ...
 *   list-header (11)
 *   ...
 *   list-header (20)
 *   ...
 *   list-header (21)
 */

static HwpTable *hwp_hwp5_parser_get_table (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);
  g_return_val_if_fail (HWP_IS_HWP5_FILE (file), NULL);

  HwpTable *table = hwp_table_new ();

  parser_read_uint32 (parser, &table->flags, error);
  parser_read_uint16 (parser, &table->n_rows, error);
  parser_read_uint16 (parser, &table->n_cols, error);
  parser_read_uint16 (parser, &table->cell_spacing, error);
  parser_read_uint16 (parser, &table->left_margin, error);
  parser_read_uint16 (parser, &table->right_margin, error);
  parser_read_uint16 (parser, &table->top_margin, error);
  parser_read_uint16 (parser, &table->bottom_margin, error);

  table->row_sizes = g_malloc0_n (table->n_rows, 2);

  for (guint i = 0; i < table->n_rows; i++) {
    parser_read_uint16 (parser, &(table->row_sizes[i]), error);
  }

  parser_read_uint16 (parser, &table->border_fill_id, error);

  if (hwp_hwp5_file_check_version (file, 5, 0, 0, 7)) {
    parser_read_uint16 (parser, &table->valid_zone_info_size, error);

    table->zones = g_malloc0_n (table->valid_zone_info_size, 2);

    for (guint i = 0; i < table->valid_zone_info_size; i++) {
      parser_read_uint16 (parser, &(table->zones[i]), error);
    }
  }

  if (parser->data_count != parser->data_len) {
    g_warning ("%s:%d: TABLE data size mismatch at %s\n",
      __FILE__, __LINE__,
      hwp_hwp5_file_get_hwp_version_string(HWP_FILE (file)));
  }

  return table;
}

static HwpTableCell *hwp_hwp5_parser_get_table_cell (HwpHWP5Parser *parser,
                                                     GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);

  HwpTableCell *table_cell = hwp_table_cell_new ();
  /* 표 60 LIST_HEADER */
  parser_read_uint16 (parser, &table_cell->n_paragraphs, error);
  parser_read_uint32 (parser, &table_cell->flags, error);
  parser_read_uint16 (parser, &table_cell->unknown1, error);
  /* 표 75 cell property */
  parser_read_uint16 (parser, &table_cell->col_addr, error);
  parser_read_uint16 (parser, &table_cell->row_addr, error);
  parser_read_uint16 (parser, &table_cell->col_span, error);
  parser_read_uint16 (parser, &table_cell->row_span, error);

  parser_read_uint32 (parser, &table_cell->width, error);
  parser_read_uint32 (parser, &table_cell->height, error);

  parser_read_uint16 (parser, &table_cell->left_margin, error);
  parser_read_uint16 (parser, &table_cell->right_margin, error);
  parser_read_uint16 (parser, &table_cell->top_margin, error);
  parser_read_uint16 (parser, &table_cell->bottom_margin, error);

  parser_read_uint16 (parser, &table_cell->border_fill_id, error);
  /* unknown */
  parser_read_uint32 (parser, &table_cell->unknown2, error);

#ifdef HWP_ENABLE_DEBUG
  printf("%d %d %d\n%d %d %d %d\n%d %d\n%d %d %d %d\n%d\n",
         table_cell->n_paragraphs, table_cell->flags, table_cell->unknown1,

         table_cell->col_addr,
         table_cell->row_addr,
         table_cell->col_span,
         table_cell->row_span,

         table_cell->width, table_cell->height,

         table_cell->left_margin,
         table_cell->right_margin,
         table_cell->top_margin,
         table_cell->bottom_margin,

         table_cell->border_fill_id);
#endif

  if (parser->data_count != parser->data_len) {
    g_warning ("%s:%d: table cell size mismatch\n", __FILE__, __LINE__);
  }

  return table_cell;
}

static void hwp_hwp5_parser_parse_table (HwpHWP5Parser *parser,
                                         HwpHWP5File   *file,
                                         GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  HwpTable     *table     = NULL;
  HwpTableCell *cell      = NULL;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_TABLE:
      table = hwp_hwp5_parser_get_table (parser, file, error);
      break;
    case HWP_TAG_LIST_HEADER: /* cell */
      cell = hwp_hwp5_parser_get_table_cell (parser, error);
      hwp_table_add_cell (table, cell);

      break;
    case HWP_TAG_PARA_HEADER:
      parser->state = HWP_PARSE_STATE_INSIDE_TABLE;
      hwp_hwp5_parser_parse_paragraph (parser, file, error);
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
        __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
  /* TODO */
  /* add table to where ? */
}

static GString *hwp_hwp5_parser_get_string (HwpHWP5Parser *parser, GError **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);

  gunichar2 ch; /* guint16 */
  GString  *string = g_string_new (NULL);
  guint     i;

  for (i = 0; i < parser->data_len; i = i + 2)
  {
    parser_read_uint16 (parser, &ch, error);
    switch (ch) {
    case 0:
      break;
    case 1:
    case 2:
    case 3:
    case 4: /* inline */
    case 5: /* inline */
    case 6: /* inline */
    case 7: /* inline */
    case 8: /* inline */
      i = i + 14;
      parser_skip (parser, 14);
      break;
    case 9: /* inline */ /* tab */
      i = i + 14;
      parser_skip (parser, 14);
      g_string_append_unichar (string, ch);
      break;
    case 10:
      break;
    case 11:
    case 12:
      i = i + 14;
      parser_skip (parser, 14);
      break;
    case 13:
      break;
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19: /* inline */
    case 20: /* inline */
    case 21:
    case 22:
    case 23:
      i = i + 14;
      parser_skip (parser, 14);
      break;
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
      break;
    default:
      g_string_append_unichar (string, ch);
      break;
    } /* switch */
  } /* for */

  if (parser->data_count != parser->data_len) {
    g_string_free (string, TRUE);
    return NULL;
  }

  return string;
}

static void hwp_hwp5_parser_parse_shape_component (HwpHWP5Parser *parser,
                                                   HwpHWP5File   *file,
                                                   GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_SHAPE_COMPONENT:
      hwp_hwp5_parser_parse_shape_component (parser, file, error);
      break;
    case HWP_TAG_LIST_HEADER:
      break;
    case HWP_TAG_PARA_HEADER:
      hwp_hwp5_parser_parse_paragraph (parser, file, error);
      break;
    case HWP_TAG_SHAPE_COMPONENT_PICTURE:
      break;
    case HWP_TAG_SHAPE_COMPONENT_LINE:
      break;
    case HWP_TAG_SHAPE_COMPONENT_POLYGON:
      break;
    case HWP_TAG_SHAPE_COMPONENT_RECTANGLE:
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
        __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_bookmark (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_CTRL_DATA:
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
          __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_eqedid (HwpHWP5Parser *parser,
                                          HwpHWP5File   *file,
                                          GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_EQEDIT:
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
          __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_ctrl_header (HwpHWP5Parser *parser,
                                               HwpHWP5File   *file,
                                               GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  parser_read_uint32 (parser, &parser->ctrl_id, error);
#ifdef HWP_ENABLE_DEBUG
  printf (" \"%c%c%c%c\"\n",
    (gchar) (parser->ctrl_id >> 24 & 0xff),
    (gchar) (parser->ctrl_id >> 16 & 0xff),
    (gchar) (parser->ctrl_id >>  8 & 0xff),
    (gchar) (parser->ctrl_id >>  0 & 0xff));
#endif
  switch (parser->ctrl_id) {
  case CTRL_ID_SECTION_DEF:
    hwp_hwp5_parser_parse_section_definition (parser, error);
    break;
  case CTRL_ID_NEW_NUM:
    break;
  case CTRL_ID_COLUMN_DEF:
    break;
  case CTRL_ID_HEADEDR: /* 머리말 */
    hwp_hwp5_parser_parse_header (parser, file, error);
    break;
  case CTRL_ID_AUTO_NUM:
    break;
  case CTRL_ID_TABLE:
    hwp_hwp5_parser_parse_table (parser, file, error);
    break;
  case CTRL_ID_FOOTNOTE: /* 각주 */
    hwp_hwp5_parser_parse_footnote (parser, file, error);
    break;
  case CTRL_ID_PAGE_HIDE: /* 페이지 감추기 pghd */
    break;
  case CTRL_ID_DRAWING_SHAPE_OBJECT:
    hwp_hwp5_parser_parse_shape_component (parser, file, error);
    break;
  case CTRL_ID_TCMT: /* 숨은 설명 */
    hwp_hwp5_parser_parse_tcmt (parser, file, error);
    break;
  case CTRL_ID_TCPS:
    break;
  case CTRL_ID_EQEDID:
    hwp_hwp5_parser_parse_eqedid (parser, file, error);
    break;
  case FIELD_BOOKMARK:
    hwp_hwp5_parser_parse_bookmark (parser, file, error);
    break;
  default:
    g_warning ("%s:%d:\"%c%c%c%c\":%s not implemented",
        __FILE__, __LINE__,
        (gchar) (parser->ctrl_id >> 24 & 0xff),
        (gchar) (parser->ctrl_id >> 16 & 0xff),
        (gchar) (parser->ctrl_id >>  8 & 0xff),
        (gchar) (parser->ctrl_id >>  0 & 0xff),
        hwp_get_ctrl_name (parser->ctrl_id));
    break;
  } /* switch (parser->ctrl_id) */
}

static void hwp_hwp5_parser_parse_paragraph (HwpHWP5Parser *parser,
                                             HwpHWP5File   *file,
                                             GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  guint16 level = parser->level;

  HwpParagraph *paragraph = hwp_paragraph_new ();
  GString      *string    = NULL;

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id) {
    case HWP_TAG_PARA_TEXT:
      string = hwp_hwp5_parser_get_string (parser, error);
      hwp_paragraph_set_string (paragraph, string);
      string = NULL;
      break;
    case HWP_TAG_PARA_CHAR_SHAPE:
      break;
    case HWP_TAG_PARA_LINE_SEG:
      break;
    case HWP_TAG_CTRL_HEADER:
      hwp_hwp5_parser_parse_ctrl_header (parser, file, error);
      break;
    default:
      g_warning ("%s:%d:%s not implemented",
          __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */

  /* FIXME 내포된 문단이 먼저 넘어간 후 이곳 문단이 넘어가는 버그가 있다. */
  /* 모델링을 어떻게 할지 정한 후에 제대로 모델링을 하게 되면 그 버그는 자연히 없어진다. */
  if (iface->paragraph)
    iface->paragraph (parser->listener, paragraph, parser->user_data, error);

  return;
}

static void hwp_hwp5_parser_parse_section (HwpHWP5Parser *parser,
                                           HwpHWP5File   *file,
                                           GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  while (hwp_hwp5_parser_pull (parser, error))
  {
    parser_skip (parser, parser->data_len);
    g_assert (parser->level == 0);

    switch (parser->tag_id) {
    case HWP_TAG_PARA_HEADER:
      hwp_hwp5_parser_parse_paragraph (parser, file, error);
      break;
    default:
        g_warning ("%s:%d:%s not implemented",
            __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
        break;
    }  /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_sections (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  for (guint i = 0; i < file->section_streams->len; i++)
  {
    GInputStream *stream = g_array_index (file->section_streams,
                                          GInputStream *, i);
    parser->stream = stream;
    hwp_hwp5_parser_parse_section (parser, file, error);
  }
}

static void hwp_hwp5_parser_parse_body_text (HwpHWP5Parser *parser,
                                             HwpHWP5File   *file,
                                             GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  hwp_hwp5_parser_parse_sections (parser, file, error);
}

/* 알려지지 않은 것을 감지하기 위해 이렇게 작성함 */
static void metadata_hash_func (gpointer k, gpointer v, gpointer user_data)
{
  gchar          *name = (gchar          *) k;
  GsfDocProp     *prop = (GsfDocProp     *) v;
  HwpSummaryInfo *info = (HwpSummaryInfo *) user_data;

  GValue const *value = gsf_doc_prop_get_val (prop);

  if ( g_str_equal(name, GSF_META_NAME_CREATOR) ) {
    info->creator = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_DATE_MODIFIED) ) {
    GsfTimestamp *ts = g_value_get_boxed (value);
    info->mod_date = (GTime) ts->timet;
  } else if ( g_str_equal(name, GSF_META_NAME_DESCRIPTION) ) {
    info->desc = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_KEYWORDS) ) {
    info->keywords = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_SUBJECT) ) {
    info->subject = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_TITLE) ) {
    info->title = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_LAST_PRINTED) ) {
    GsfTimestamp *ts   = g_value_get_boxed (value);
    info->last_printed = (GTime) ts->timet;
  } else if ( g_str_equal(name, GSF_META_NAME_LAST_SAVED_BY) ) {
    info->last_saved_by = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_DATE_CREATED) ) {
    GsfTimestamp *ts    = g_value_get_boxed (value);
    info->creation_date = (GTime) ts->timet;
  /* hwp 문서를 저장할 때 사용된 한컴 워드프로세서의 내부 버전 */
  } else if ( g_str_equal(name, GSF_META_NAME_REVISION_COUNT) ) {
    info->hanword_version = g_strdup (g_value_get_string (value));
  } else if ( g_str_equal(name, GSF_META_NAME_PAGE_COUNT) ) {
    /* not correct n_pages == 0 ?? */
    g_value_get_int (value);
  } else {
    g_warning("%s:%d:%s not implemented\n", __FILE__, __LINE__, name);
  }
}

static void hwp_hwp5_parser_parse_summary_info (HwpHWP5Parser *parser,
                                                HwpHWP5File   *file,
                                                GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);
  if (!iface->summary_info)
    return;

  GsfInputStream *gis;
  gssize          size;
  guint8         *buf = NULL;
  GsfInput       *summary;
  GsfDocMetaData *meta;

  gis  = g_object_ref (file->summary_info_stream);
  size = gsf_input_stream_size (gis);
  buf  = g_malloc (size);

  g_input_stream_read (G_INPUT_STREAM (gis), buf, size, NULL, error);

  if (*error) {
    g_free (buf);
    g_object_unref (gis);
    g_warning ("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
    return;
  }

  /* from gsf-msole-utils.c */
  guint8 component_guid [] = {
    0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
    0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
  };

  /*
   * This code forces sections[i].type to COMPONENT_PROP.
   * For more information, please refer to gsf_doc_meta_data_read_from_msole
   * in gsf-msole-utils.c.c
   */
  if (size >= sizeof(component_guid) + 28) {
    memcpy (buf + 28, component_guid, sizeof(component_guid));
  } else {
    g_free (buf);
    g_object_unref (file->summary_info_stream);
    g_object_unref (gis);
    g_set_error (error, HWP_FILE_ERROR, HWP_FILE_ERROR_INVALID,
                 "%s:%d: file corrupted\n", __FILE__, __LINE__);
    return;
  }

  summary = gsf_input_memory_new (buf, size, FALSE);
  meta = gsf_doc_meta_data_new ();

#ifdef HAVE_GSF_DOC_META_DATA_READ_FROM_MSOLE
  /* since libgsf 1.14.24 */
  gsf_doc_meta_data_read_from_msole (meta, summary);
#else
  /* NOTE gsf_msole_metadata_read: deprecated since libgsf 1.14.24 */
  gsf_msole_metadata_read (summary, meta);
#endif

  HwpSummaryInfo *info = hwp_summary_info_new ();
  gsf_doc_meta_data_foreach (meta, metadata_hash_func, info);
  iface->summary_info (parser->listener, info, parser->user_data, error);

  g_free (buf);
  g_object_unref (meta);
  g_object_unref (summary);
  g_object_unref (gis);
}

static void hwp_hwp5_parser_parse_prv_text (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  GsfInputStream *gis   = g_object_ref (file->prv_text_stream);
  gssize          size  = gsf_input_stream_size (gis);
  guchar         *buf   = g_new (guchar, size);

  g_input_stream_read ((GInputStream*) gis, buf, size, NULL, error);

  if (*error) {
    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
    g_free (buf);
    g_object_unref (gis);
    return;
  }

  /* g_convert() can be used to convert a byte buffer of UTF-16 data of
     ambiguous endianess. */
  gchar *prv_text = g_convert ((const gchar*) buf, (gssize) size,
                               "UTF-8", "UTF-16LE", NULL, NULL, error);

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);
  if (iface->prv_text)
    iface->prv_text (HWP_LISTENER (parser->listener), prv_text, parser->user_data, error);

  if (*error) {
    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
    g_free (prv_text);
    g_free (buf);
    g_object_unref (gis);
    return;
  }

  g_free (buf);
  g_free (prv_text);
  g_object_unref (gis);
}

gboolean hwp_hwp5_parser_check_version (HwpHWP5Parser *parser,
                                        guint8         major,
                                        guint8         minor,
                                        guint8         micro,
                                        guint8         extra)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), FALSE);

  return (parser->major_version >  major) ||
         (parser->major_version == major &&
          parser->minor_version >  minor) ||
         (parser->major_version == major &&
          parser->minor_version == minor &&
          parser->micro_version >  micro) ||
         (parser->major_version == major &&
          parser->minor_version == minor &&
          parser->micro_version == micro &&
          parser->extra_version >= extra);
}

void hwp_hwp5_parser_parse_file_header (HwpHWP5Parser *parser,
                                        HwpHWP5File   *file,
                                        GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);

  if (iface->document_version)
    iface->document_version (parser->listener,
                             file->major_version,
                             file->minor_version,
                             file->micro_version,
                             file->extra_version,
                             parser->user_data,
                             error);

  parser->major_version = file->major_version;
  parser->minor_version = file->minor_version;
  parser->micro_version = file->micro_version;
  parser->extra_version = file->extra_version;
}

/**
 * hwp_hwp5_parser_parse:
 * @parser:a #HwpHWP5Parser
 * @file: a #HwpHWP5File
 * @error: a #GError
 *
 * Since: 0.0.1
 */
void hwp_hwp5_parser_parse (HwpHWP5Parser *parser,
                            HwpHWP5File   *file,
                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  hwp_hwp5_parser_parse_file_header    (parser, file, error);
  hwp_hwp5_parser_parse_doc_info       (parser, file, error);
  hwp_hwp5_parser_parse_body_text      (parser, file, error);
  hwp_hwp5_parser_parse_summary_info   (parser, file, error);
/*  _hwp_hwp5_parser_parse_bin_data       (parser, file, error); */
  hwp_hwp5_parser_parse_prv_text       (parser, file, error);
/*  _hwp_hwp5_parser_parse_prv_image      (parser, file, error); */
/*  _hwp_hwp5_parser_parse_doc_options    (parser, file, error); */
/*  _hwp_hwp5_parser_parse_scripts        (parser, file, error); */
/*  _hwp_hwp5_parser_parse_xml_template   (parser, file, error); */
/*  _hwp_hwp5_parser_parse_doc_history    (parser, file, error); */
}

static void hwp_hwp5_parser_init (HwpHWP5Parser *parser)
{
  parser->priv = G_TYPE_INSTANCE_GET_PRIVATE (parser,
                                              HWP_TYPE_HWP5_PARSER,
                                              HwpHWP5ParserPrivate);
  parser->state = HWP_PARSE_STATE_NORMAL;
}

static void hwp_hwp5_parser_finalize (GObject *object)
{
  HwpHWP5Parser *parser = HWP_HWP5_PARSER (object);
  if (G_IS_INPUT_STREAM (parser->stream)) {
    g_input_stream_close (parser->stream, NULL, NULL);
    g_object_unref (parser->stream);
  }

  G_OBJECT_CLASS (hwp_hwp5_parser_parent_class)->finalize (object);
}

static void hwp_hwp5_parser_class_init (HwpHWP5ParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  g_type_class_add_private (klass, sizeof (HwpHWP5ParserPrivate));
  object_class->finalize = hwp_hwp5_parser_finalize;
}

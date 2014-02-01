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

static HwpParagraph *
hwp_hwp5_parser_get_paragraph (HwpHWP5Parser *parser, HwpHWP5File *file);

gboolean parser_skip (HwpHWP5Parser *parser, guint16 count)
{
  g_return_val_if_fail (parser != NULL, FALSE);

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

gboolean parser_read_uint16 (HwpHWP5Parser *parser, guint16 *i)
{
    g_return_val_if_fail (parser != NULL, FALSE);
    g_return_val_if_fail (parser->data_count <= parser->data_len - 2, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (parser->stream, i, 2,
                                          &parser->priv->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (parser->priv->bytes_read != 2) ||
        (parser->priv->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (parser->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT16_FROM_LE(*i);
    parser->data_count += 2;
    return TRUE;
}

gboolean parser_read_uint32 (HwpHWP5Parser *parser, guint32 *i)
{
    g_return_val_if_fail (parser != NULL, FALSE);
    g_return_val_if_fail (parser->data_count <= parser->data_len - 4, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (parser->stream, i, 4,
                                          &parser->priv->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (parser->priv->bytes_read != 4) ||
        (parser->priv->bytes_read == 0))
    {
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

  gboolean is_success = TRUE;

  if (parser->data_len - parser->data_count > 0)
      parser_skip (parser, parser->data_len - parser->data_count);

  /* 4바이트 읽기 */
  is_success = g_input_stream_read_all (parser->stream,
                                        &parser->priv->header,
                                        (gsize) 4,
                                        &parser->priv->bytes_read,
                                        NULL, error);

  if (is_success == FALSE) {
    if (!g_input_stream_is_closed(parser->stream))
      g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }
  /* 스트림의 끝, 에러가 아님 */
  if (parser->priv->bytes_read == ((gsize) 0)) {
    if (!g_input_stream_is_closed(parser->stream))
      g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }
  /* 비정상 */
  if (parser->priv->bytes_read != ((gsize) 4)) {
    g_error("error %s %d", __FILE__, __LINE__);
    g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                         _("File corrupted or invalid file"));
    if (!g_input_stream_is_closed(parser->stream))
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
      g_error("error %s %d", __FILE__, __LINE__);
      g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                           _("File corrupted"));
      g_input_stream_close (parser->stream, NULL, NULL);
      return FALSE;
  }
  /* data_len == 0xfff 이면 다음 4바이트는 data_len 이다 */
  if (parser->data_len == 0xfff) {
      g_error("0xfff %s %d", __FILE__, __LINE__);
      is_success = g_input_stream_read_all (parser->stream,
                                            &parser->data_len, (gsize) 4,
                                            &parser->priv->bytes_read,
                                            NULL, error);

      if (is_success == FALSE) {
          /* g_input_stream_read_all이 에러를 설정했으므로
           * 따로 에러 설정할 필요 없다. */
          g_input_stream_close (parser->stream, NULL, NULL);
          return FALSE;
      }

      /* 비정상 */
      if (parser->priv->bytes_read != ((gsize) 4)) {
          g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                               _("File corrupted"));
          g_input_stream_close (parser->stream, NULL, NULL);
          return FALSE;
      }

      parser->data_len = GUINT32_FROM_LE(parser->data_len);
  }

  parser->data_count = 0;

  return TRUE;
}

/*HwpHWP5Parser *hwp_hwp5_parser_new (GInputStream *stream)*/
/*{*/
/*    g_return_val_if_fail (stream != NULL, NULL);*/
/*    HwpHWP5Parser *parser = g_object_new (HWP_TYPE_HWP5_PARSER, NULL);*/
/*    parser->stream = g_object_ref (stream);*/
/*    return parser;*/
/*}*/

/**
 * Since: TODO
 */
HwpHWP5Parser *hwp_hwp5_parser_new (HwpListener *listener, gpointer user_data)
{
  HwpHWP5Parser *parser = g_object_new (HWP_TYPE_HWP5_PARSER, NULL);
  parser->listener      = listener;
  parser->user_data     = user_data;

  return parser;
}

static void parse_doc_info (HwpHWP5Parser *parser,
                            HwpHWP5File   *file,
                            GError       **error)
{
    g_return_if_fail (HWP_IS_HWP5_FILE (file));

/*    guint32 id_mappings[16] = {0}; */ /* 반드시 초기화 해야 한다. */
/*    int i;*/

/*    GInputStream *stream  = file->doc_info_stream;
    HwpHWP5Parser  *parser = hwp_hwp5_parser_new (stream);
    while (hwp_hwp5_parser_pull (parser, error)) {
        switch (parser->tag_id) {
        case HWP_TAG_DOCUMENT_PROPERTIES:*/
            /* TODO */
/*            break;
        case HWP_TAG_ID_MAPPINGS:*/
/*            for (i = 0; i < sizeof(id_mappings); i = i + sizeof(guint32)) {*/
/*                memcpy(&id_mappings[i], &(parser->data[i]), sizeof(guint32));*/
/*                id_mappings[i] = GUINT16_FROM_LE(id_mappings[i]);*/
/*                printf("%d\n", id_mappings[i]);*/
/*            }*/
/*            break;
        default:
            printf("%s:%d: %s not implemented\n", __FILE__, __LINE__,
                _hwp_get_tag_name (parser->tag_id));
            break;
        }
    }

    g_object_unref (parser);*/
}

static void parse_section_definition (HwpHWP5Parser *parser)
{
    GError *error = NULL;

    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < 2) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }

        printf ("%s\n", hwp_get_tag_name(parser->tag_id));
        g_assert (parser->level == 2);
        switch (parser->tag_id) {
        case HWP_TAG_PAGE_DEF:
            break;
        case HWP_TAG_FOOTNOTE_SHAPE:
            break;
        case HWP_TAG_PAGE_BORDER_FILL:
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
            break;
        } /* switch */
    } /* while */
}

/* 머리말 */
static void parse_header (HwpHWP5Parser *parser, HwpHWP5File *file)
{
    GError *error = NULL;

    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < 2) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }
        printf ("%s\n", hwp_get_tag_name(parser->tag_id));

        g_assert (parser->level == 2);

        switch (parser->tag_id) {
        case HWP_TAG_LIST_HEADER:
            break;
        case HWP_TAG_PARA_HEADER:
            hwp_hwp5_parser_get_paragraph (parser, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
            break;
        } /* switch */
    } /* while */
}

/* 각주 */
static void parse_footnote (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  GError *error = NULL;
  guint16 level = parser->level + 1;
  printf("level = %d\n", level);

    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < level) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }
        printf ("%s\n", hwp_get_tag_name(parser->tag_id));

        g_assert (parser->level == level);

        switch (parser->tag_id) {
        case HWP_TAG_LIST_HEADER:
            break;
        case HWP_TAG_PARA_HEADER:
            hwp_hwp5_parser_get_paragraph (parser, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
            break;
        } /* switch */
    } /* while */
}

static void parse_tcmt (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  GError *error = NULL;
  guint16 level = parser->level + 1;
  printf("level = %d\n", level);

    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < level) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }
        printf ("%s\n", hwp_get_tag_name(parser->tag_id));

        g_assert (parser->level == level);

        switch (parser->tag_id) {
        case HWP_TAG_LIST_HEADER:
            break;
        case HWP_TAG_PARA_HEADER:
            hwp_hwp5_parser_get_paragraph (parser, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
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

static HwpTable *
hwp_hwp5_parser_get_table (HwpHWP5Parser *parser, HwpHWP5File *file)
{
    g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);
    HwpTable *table = hwp_table_new ();

    parser_read_uint32 (parser, &table->flags);
    parser_read_uint16 (parser, &table->n_rows);
    parser_read_uint16 (parser, &table->n_cols);
    parser_read_uint16 (parser, &table->cell_spacing);
    parser_read_uint16 (parser, &table->left_margin);
    parser_read_uint16 (parser, &table->right_margin);
    parser_read_uint16 (parser, &table->top_margin);
    parser_read_uint16 (parser, &table->bottom_margin);

    table->row_sizes = g_malloc0_n (table->n_rows, 2);

    for (guint i = 0; i < table->n_rows; i++) {
        parser_read_uint16 (parser, &(table->row_sizes[i]));
    }

    parser_read_uint16 (parser, &table->border_fill_id);

    if (hwp_hwp5_file_check_version (file, 5, 0, 0, 7)) {
        parser_read_uint16 (parser, &table->valid_zone_info_size);

        table->zones = g_malloc0_n (table->valid_zone_info_size, 2);

        for (guint i = 0; i < table->valid_zone_info_size; i++) {
            parser_read_uint16 (parser, &(table->zones[i]));
        }
    }

    if (parser->data_count != parser->data_len) {
        g_warning ("%s:%d: TABLE data size mismatch at %s\n",
            __FILE__, __LINE__,
            hwp_hwp5_file_get_hwp_version_string(HWP_FILE (file)));
    }
    return table;
}

static HwpTableCell *hwp_hwp5_parser_get_table_cell (HwpHWP5Parser *parser)
{
    g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);

    HwpTableCell *table_cell = hwp_table_cell_new ();
    /* 표 60 LIST_HEADER */
    parser_read_uint16 (parser, &table_cell->n_paragraphs);
    parser_read_uint32 (parser, &table_cell->flags);
    parser_read_uint16 (parser, &table_cell->unknown1);
    /* 표 75 cell property */
    parser_read_uint16 (parser, &table_cell->col_addr);
    parser_read_uint16 (parser, &table_cell->row_addr);
    parser_read_uint16 (parser, &table_cell->col_span);
    parser_read_uint16 (parser, &table_cell->row_span);

    parser_read_uint32 (parser, &table_cell->width);
    parser_read_uint32 (parser, &table_cell->height);

    parser_read_uint16 (parser, &table_cell->left_margin);
    parser_read_uint16 (parser, &table_cell->right_margin);
    parser_read_uint16 (parser, &table_cell->top_margin);
    parser_read_uint16 (parser, &table_cell->bottom_margin);

    parser_read_uint16 (parser, &table_cell->border_fill_id);
    /* unknown */
    parser_read_uint32 (parser, &table_cell->unknown2);
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

    if (parser->data_count != parser->data_len) {
        g_warning ("%s:%d: table cell size mismatch\n", __FILE__, __LINE__);
    }

    return table_cell;
}

static void parse_table (HwpHWP5Parser *parser, HwpHWP5File *file)
{
    GError *error = NULL;
    guint16 level = parser->level + 1;
    printf("level = %d\n", level);
    HwpTable     *table     = NULL;
    HwpTableCell *cell      = NULL;
    HwpParagraph *paragraph = NULL;
  
    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < level) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }
        printf ("%s\n", hwp_get_tag_name(parser->tag_id));

        g_assert (parser->level == level);

        switch (parser->tag_id) {
        case HWP_TAG_TABLE:
            table = hwp_hwp5_parser_get_table (parser, file);
            break;
        case HWP_TAG_LIST_HEADER: /* cell */
            cell = hwp_hwp5_parser_get_table_cell (parser);
            hwp_table_add_cell (table, cell);
            break;
        case HWP_TAG_PARA_HEADER:
            parser->state = HWP_PARSE_STATE_INSIDE_TABLE;
            paragraph = hwp_hwp5_parser_get_paragraph (parser, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
            break;
        } /* switch */
    } /* while */
    /* TODO */
    /* add table to where ? */
}

static gchar *hwp_hwp5_parser_get_text (HwpHWP5Parser *parser)
{
    g_return_val_if_fail (parser != NULL, NULL);
    gunichar2 ch; /* guint16 */
    GString  *text = g_string_new (NULL);
    guint     i;

    for (i = 0; i < parser->data_len; i = i + 2)
    {
        parser_read_uint16 (parser, &ch);
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
            parser_skip(parser, 14);;
            break;
        case 9: /* inline */ /* tab */
            i = i + 14;
            parser_skip(parser, 14);;
            g_string_append_unichar(text, ch);
            break;
        case 10:
            break;
        case 11:
        case 12:
            i = i + 14;
            parser_skip(parser, 14);;
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
            parser_skip(parser, 14);;
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
            g_string_append_unichar(text, ch);
            break;
        } /* switch */
    } /* for */

    if (parser->data_count != parser->data_len) {
        g_string_free(text, TRUE);
        return NULL;
    }

    return g_string_free(text, FALSE);
}

static void parse_shape_component (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  GError *error = NULL;
  guint16 level = parser->level + 1;
  printf("level = %d\n", level);

    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < level) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }
        printf ("%s\n", hwp_get_tag_name(parser->tag_id));

        g_assert (parser->level == level);

        switch (parser->tag_id) {
        case HWP_TAG_SHAPE_COMPONENT_PICTURE:
          break;
        case HWP_TAG_SHAPE_COMPONENT_LINE:
          break;
        case HWP_TAG_SHAPE_COMPONENT_POLYGON:
          break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
            break;
        } /* switch */
    } /* while */
}

static void
parse_drawing_shape_object (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  GError *error = NULL;
  guint16 level = parser->level + 1;
  printf("level = %d\n", level);

    while (hwp_hwp5_parser_pull(parser, &error)) {
        if (parser->level < level) {
            parser->state = HWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", parser->level);
        for (int i = 0; i < parser->level; i++) {
            printf (" ");
        }
        printf ("%s\n", hwp_get_tag_name(parser->tag_id));

        g_assert (parser->level == level);

        switch (parser->tag_id) {
        case HWP_TAG_SHAPE_COMPONENT:
          parse_shape_component (parser, file);
          break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
            break;
        } /* switch */
    } /* while */
}

static HwpParagraph *
hwp_hwp5_parser_get_paragraph (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  GError *error = NULL;
  guint16 level = parser->level + 1;
  printf("level = %d\n", level);
  HwpParagraph *paragraph = hwp_paragraph_new ();
  HwpText      *hwp_text = NULL;
  gchar         *text      = NULL;

  while (hwp_hwp5_parser_pull(parser, &error)) {
    if (parser->level < level) {
        parser->state = HWP_PARSE_STATE_PASSING;
        break;
    }

    printf ("%d", parser->level);
    for (int i = 0; i < parser->level; i++) {
      printf (" ");
    }
    printf ("%s\n", hwp_get_tag_name(parser->tag_id));

    g_assert (parser->level == level);

    switch (parser->tag_id) {
    case HWP_TAG_PARA_TEXT:
      text = hwp_hwp5_parser_get_text (parser);
      hwp_text = hwp_text_new (text);
      hwp_paragraph_set_hwp_text (paragraph, hwp_text);
      printf ("%s\n", text);
      g_free (text);
      text = NULL;
      break;
    case HWP_TAG_PARA_CHAR_SHAPE:
      break;
    case HWP_TAG_PARA_LINE_SEG:
      break;
    case HWP_TAG_CTRL_HEADER:
      parser_read_uint32 (parser, &parser->ctrl_id);
      printf (" \"%c%c%c%c\"\n",
        (gchar) (parser->ctrl_id >> 24 & 0xff),
        (gchar) (parser->ctrl_id >> 16 & 0xff),
        (gchar) (parser->ctrl_id >>  8 & 0xff),
        (gchar) (parser->ctrl_id >>  0 & 0xff));
      switch (parser->ctrl_id) {
      case CTRL_ID_SECTION_DEF:
        parse_section_definition (parser);
        break;
      case CTRL_ID_NEW_NUM:
        break;
      case CTRL_ID_COLUMN_DEF:
        break;
      case CTRL_ID_HEADEDR: /* 머리말 */
        parse_header (parser, file);
        break;
      case CTRL_ID_AUTO_NUM:
        break;
      case CTRL_ID_TABLE:
        parse_table (parser, file);
        break;
      case CTRL_ID_FOOTNOTE: /* 각주 */
        parse_footnote (parser, file);
        break;
      case CTRL_ID_PAGE_HIDE:
        break;
      case CTRL_ID_DRAWING_SHAPE_OBJECT:
        parse_drawing_shape_object (parser, file);
        break;
      case CTRL_ID_TCMT: /* 숨은 설명 */
        parse_tcmt (parser, file);
        break;
      case CTRL_ID_TCPS:
        break;
      default:
        g_error ("%s:%d:\"%c%c%c%c\":%s not implemented",
            __FILE__, __LINE__,
            (gchar) (parser->ctrl_id >> 24 & 0xff),
            (gchar) (parser->ctrl_id >> 16 & 0xff),
            (gchar) (parser->ctrl_id >>  8 & 0xff),
            (gchar) (parser->ctrl_id >>  0 & 0xff),
            hwp_get_ctrl_name (parser->ctrl_id));
        break;
      }
      break;
    default:
      g_error ("%s:%d:%s not implemented",
          __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
      break;
    } /* switch */
  } /* while */
  return paragraph;
}

static void parse_section (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  GError       *error     = NULL;
  HwpParagraph *paragraph = NULL;

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);

  while (hwp_hwp5_parser_pull(parser, &error))
  {
    printf ("%d", parser->level);
    for (int i = 0; i < parser->level; i++) {
        printf (" ");
    }
    printf ("%s\n", hwp_get_tag_name(parser->tag_id));
    parser_skip (parser, parser->data_len);
    g_assert (parser->level == 0);

    switch (parser->tag_id) {
    case HWP_TAG_PARA_HEADER:
      paragraph = hwp_hwp5_parser_get_paragraph (parser, file);
      if (iface->object) {
        iface->object (parser->listener,
                       G_OBJECT (paragraph),
                       parser->user_data,
                       &error);
        /* 변수를 재사용할 수 있으므로 NULL 대입 */
        paragraph = NULL;
      }
      break;
    default:
        g_error ("%s:%d:%s not implemented",
            __FILE__, __LINE__, hwp_get_tag_name (parser->tag_id));
        break;
    }  /* switch */
  } /* while */
}

static void parse_sections (HwpHWP5Parser *parser, HwpHWP5File *file)
{
  for (guint i = 0; i < file->section_streams->len; i++)
  {
    GInputStream *stream = g_array_index (file->section_streams,
                                          GInputStream *, i);
    parser->stream = stream;
    parse_section (parser, file);
  }
}

static void parse_body_text (HwpHWP5Parser *parser,
                             HwpHWP5File *file,
                             GError **error)
{
    g_return_if_fail (HWP_IS_HWP5_FILE (file));

    parse_sections (parser, file);
}

static void parse_view_text (HwpHWP5Parser *parser, HwpHWP5File *file)
{
    g_return_if_fail (HWP_IS_HWP5_FILE (file));

    parse_sections (parser, file);
}

/* 알려지지 않은 것을 감지하기 위해 이렇게 작성함 */
static void metadata_hash_func (gpointer k, gpointer v, gpointer user_data)
{
  gchar         *name   = (gchar         *) k;
  GsfDocProp    *prop   = (GsfDocProp    *) v;
  HwpHWP5Parser *parser = (HwpHWP5Parser *) user_data;
  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser);
  GValue const *value = gsf_doc_prop_get_val (prop);

  if ( g_str_equal(name, GSF_META_NAME_CREATOR) ) {
    if (iface->creator) {
      gchar *creator = (gchar *) g_value_get_string (value);
      iface->creator (parser->listener, creator, parser->user_data, NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_DATE_MODIFIED) ) {
    if (iface->mod_date) {
      GsfTimestamp *ts = g_value_get_boxed (value);
      GTime mod_date = (GTime) ts->timet;
      iface->mod_date (parser->listener, mod_date, parser->user_data, NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_DESCRIPTION) ) {
    if (iface->desc) {
      gchar *desc = (gchar *) g_value_get_string (value);
      iface->desc (parser->listener, desc, parser->user_data, NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_KEYWORDS) ) {
    if (iface->keywords) {
      gchar *keywords = (gchar *) g_value_get_string (value);
      iface->keywords (parser->listener, keywords, parser->user_data, NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_SUBJECT) ) {
    if (iface->subject) {
      gchar *subject = (gchar *) g_value_get_string (value);
      iface->subject (parser->listener, subject, parser->user_data, NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_TITLE) ) {
    if (iface->title) {
      gchar *title = (gchar *) g_value_get_string (value);
      iface->title (parser->listener, title, parser->user_data, NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_LAST_PRINTED) ) {
    if (iface->last_printed) {
      GsfTimestamp *ts    = g_value_get_boxed (value);
      GTime last_printed   = (GTime) ts->timet;
      iface->last_printed (parser->listener,
                           last_printed,
                           parser->user_data,
                           NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_LAST_SAVED_BY) ) {
    if (iface->last_saved_by) {
      gchar *last_saved_by = (gchar *) g_value_get_string (value);
      iface->last_saved_by (parser->listener,
                            last_saved_by,
                            parser->user_data,
                            NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_DATE_CREATED) ) {
    if (iface->creation_date) {
      GsfTimestamp *ts    = g_value_get_boxed (value);
      GTime creation_date  = (GTime) ts->timet;
      iface->creation_date (parser->listener,
                            creation_date,
                            parser->user_data,
                            NULL);
    }
  /* hwp 문서를 저장할 때 사용된 한컴 워드프로세서의 내부 버전 */
  } else if ( g_str_equal(name, GSF_META_NAME_REVISION_COUNT) ) {
    if (iface->hanword_version) {
      gchar *hanword_version = (gchar *) g_value_get_string (value);
      iface->hanword_version (parser->listener,
                              hanword_version,
                              parser->user_data,
                              NULL);
    }
  } else if ( g_str_equal(name, GSF_META_NAME_PAGE_COUNT) ) {
    /* not correct n_pages == 0 ?? */
    /* g_value_get_int (value); */
  } else {
    g_warning("%s:%d:%s not implemented\n", __FILE__, __LINE__, name);
  }
}

static void parse_summary_info (HwpHWP5Parser *parser,
                                HwpHWP5File   *file,
                                GError       **error)
{
    g_return_if_fail (HWP_IS_HWP5_FILE (file));

    GsfInputStream *gis;
    gssize          size;
    guint8         *buf = NULL;
    GsfInputMemory *summary;
    GsfDocMetaData *meta;

    gis  = g_object_ref (file->summary_info_stream);
    size = gsf_input_stream_size (gis);
    buf  = g_malloc(size);

    g_input_stream_read ((GInputStream*) gis, buf, (gsize) size, NULL, error);

    if (*error != NULL) {
        buf = (g_free (buf), NULL);
        g_object_unref (gis);
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        g_clear_error (error);
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
    buf = (g_free (buf), NULL);
    g_object_unref (file->summary_info_stream);
    g_object_unref (gis);
    g_warning("%s:%d: file corrupted\n", __FILE__, __LINE__);
    return;
  }

    summary = (GsfInputMemory*) gsf_input_memory_new (buf, size, FALSE);

    meta = gsf_doc_meta_data_new ();

#ifdef HAVE_GSF_DOC_META_DATA_READ_FROM_MSOLE
    /* since libgsf 1.14.24 */
    gsf_doc_meta_data_read_from_msole (meta, (GsfInput*) summary);
#else
    /* NOTE gsf_msole_metadata_read: deprecated since libgsf 1.14.24 */
    gsf_msole_metadata_read ((GsfInput*) summary, meta);
#endif

    gsf_doc_meta_data_foreach (meta, metadata_hash_func, NULL);

    g_object_unref (meta);
    g_object_unref (summary);
    buf = (g_free (buf), NULL);
    g_object_unref (gis);
}

static void parse_prv_text (HwpHWP5Parser *parser,
                            HwpHWP5File   *file,
                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_FILE (file));

  GsfInputStream *gis   = g_object_ref (file->prv_text_stream);
  gssize          size  = gsf_input_stream_size (gis);
  guchar         *buf   = g_new (guchar, size);

  g_input_stream_read ((GInputStream*) gis, buf, size, NULL, error);

  if (*error) {
    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
    g_clear_error (error);
    buf = (g_free (buf), NULL);
    g_object_unref (gis);
    return;
  }

  /* g_convert() can be used to convert a byte buffer of UTF-16 data of
     ambiguous endianess. */
  gchar *prv_text = g_convert ((const gchar*) buf, (gssize) size,
                               "UTF-8", "UTF-16LE", NULL, NULL, error);

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser);
  if (iface->prv_text)
    iface->prv_text (HWP_LISTENER (parser), prv_text, parser->user_data, error);

  if (error) {
    g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
    g_free (prv_text);
    g_clear_error (error);
    buf = (g_free (buf), NULL);
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

/**
 * hwp_hwp5_parser_parse:
 * @parser:
 * @file:
 * @error:
 *
 * Since: 0.0.1
 */
void hwp_hwp5_parser_parse (HwpHWP5Parser *parser,
                            HwpHWP5File   *file,
                            GError       **error)
{
  g_return_if_fail (parser != NULL);
  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);
  if (iface->document_version)
    iface->document_version (parser->listener,
                             file->major_version,
                             file->minor_version,
                             file->micro_version,
                             file->extra_version,
                             parser->user_data,
                             error);

/*  parse_file_header    (parser, file, error); */
/*  parse_doc_info       (parser, file, error);*/
  parse_body_text      (parser, file, error);
/*  parse_view_text      (parser, file, error); */
/*  parse_summary_info   (parser, file, error);*/
/*    parse_bin_data       (parser, file, error); */
/*  parse_prv_text       (parser, file, error);*/
/*    parse_prv_image      (parser, file, error); */
/*    parse_doc_options    (parser, file, error); */
/*    parse_scripts        (parser, file, error); */
/*    parse_xml_template   (parser, file, error); */
/*    parse_doc_history    (parser, file, error); */
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

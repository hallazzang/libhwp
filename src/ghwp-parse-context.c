/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * ghwp-parse.c
 *
 * Copyright (C) 2012-2013 Hodong Kim <cogniti@gmail.com>
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
#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>
#include <gio/gio.h>

#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <gsf/gsf-doc-meta-data.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "gsf-input-stream.h"
#include "ghwp-file-v5.h"
#include "ghwp-listener.h"
#include "ghwp-models.h"
#include "ghwp-parse-context.h"

G_DEFINE_TYPE (GHWPParseContext, ghwp_parse_context, G_TYPE_OBJECT)

gboolean context_skip (GHWPParseContext *context, guint16 count)
{
  g_return_val_if_fail (context != NULL, FALSE);

  gboolean is_success = FALSE;
  /*
   * FIXME: g_input_stream_skip 가 작동하지 않아 아래처럼 처리했다.
   * g_input_stream_skip를 제대로 작동시키려면 컨버터 스트림을 손봐야 한다.
   */
  guint8 *buf = g_malloc (count);
  is_success = g_input_stream_read_all (context->stream, buf, (gsize) count,
                                        &context->priv->bytes_read, 
                                        NULL, NULL);
  g_free (buf);

  if ((is_success == FALSE) || (context->priv->bytes_read != (gsize) count))
  {
    g_warning ("%s:%d:skip size mismatch\n", __FILE__, __LINE__);
    g_input_stream_close (context->stream, NULL, NULL);
    return FALSE;
  }

  context->data_count += count;
  return TRUE;
}

gboolean context_read_uint16 (GHWPParseContext *context, guint16 *i)
{
    g_return_val_if_fail (context != NULL, FALSE);
    g_return_val_if_fail (context->data_count <= context->data_len - 2, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (context->stream, i, 2,
                                          &context->priv->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (context->priv->bytes_read != 2) ||
        (context->priv->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT16_FROM_LE(*i);
    context->data_count += 2;
    return TRUE;
}

gboolean context_read_uint32 (GHWPParseContext *context, guint32 *i)
{
    g_return_val_if_fail (context != NULL, FALSE);
    g_return_val_if_fail (context->data_count <= context->data_len - 4, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (context->stream, i, 4,
                                          &context->priv->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (context->priv->bytes_read != 4) ||
        (context->priv->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT32_FROM_LE(*i);
    context->data_count += 4;
    return TRUE;
}

/* 에러일 경우 FALSE 반환, error 설정,
 * 성공일 경우 TRUE 반환,
 * end-of-stream 일 경우 FALSE 반환, error 설정 안 함 */
gboolean ghwp_parse_context_pull (GHWPParseContext *context, GError **error)
{
  g_return_val_if_fail (GHWP_IS_PARSE_CONTEXT (context), FALSE);

  if (context->state == GHWP_PARSE_STATE_PASSING) {
      context->state = GHWP_PARSE_STATE_NORMAL;
      return TRUE;
  }

  gboolean is_success = TRUE;

  if (context->data_len - context->data_count > 0)
      context_skip (context, context->data_len - context->data_count);

  /* 4바이트 읽기 */
  is_success = g_input_stream_read_all (context->stream,
                                        &context->priv->header,
                                        (gsize) 4,
                                        &context->priv->bytes_read,
                                        NULL, error);

  if (is_success == FALSE) {
    if (!g_input_stream_is_closed(context->stream))
      g_input_stream_close (context->stream, NULL, NULL);
    return FALSE;
  }
  /* 스트림의 끝, 에러가 아님 */
  if (context->priv->bytes_read == ((gsize) 0)) {
    if (!g_input_stream_is_closed(context->stream))
      g_input_stream_close (context->stream, NULL, NULL);
    return FALSE;
  }
  /* 비정상 */
  if (context->priv->bytes_read != ((gsize) 4)) {
    g_error("error %s %d", __FILE__, __LINE__);
    g_set_error_literal (error, GHWP_ERROR, GHWP_ERROR_INVALID,
                         _("File corrupted or invalid file"));
    if (!g_input_stream_is_closed(context->stream))
      g_input_stream_close (context->stream, NULL, NULL);
    return FALSE;
  }

  /* Converts a guint32 value from little-endian to host byte order. */
  context->priv->header = GUINT32_FROM_LE(context->priv->header);
  /* 4바이트 헤더 디코딩하기 */
  context->tag_id   = (guint16) ( context->priv->header        & 0x3ff);
  context->level    = (guint16) ((context->priv->header >> 10) & 0x3ff);
  context->data_len = (guint16) ((context->priv->header >> 20) & 0xfff);
  /* 비정상 */
  if (context->data_len == 0) {
      g_error("error %s %d", __FILE__, __LINE__);
      g_set_error_literal (error, GHWP_ERROR, GHWP_ERROR_INVALID,
                           _("File corrupted"));
      g_input_stream_close (context->stream, NULL, NULL);
      return FALSE;
  }
  /* data_len == 0xfff 이면 다음 4바이트는 data_len 이다 */
  if (context->data_len == 0xfff) {
      g_error("0xfff %s %d", __FILE__, __LINE__);
      is_success = g_input_stream_read_all (context->stream,
                                            &context->data_len, (gsize) 4,
                                            &context->priv->bytes_read,
                                            NULL, error);

      if (is_success == FALSE) {
          /* g_input_stream_read_all이 에러를 설정했으므로
           * 따로 에러 설정할 필요 없다. */
          g_input_stream_close (context->stream, NULL, NULL);
          return FALSE;
      }

      /* 비정상 */
      if (context->priv->bytes_read != ((gsize) 4)) {
          g_set_error_literal (error, GHWP_ERROR, GHWP_ERROR_INVALID,
                               _("File corrupted"));
          g_input_stream_close (context->stream, NULL, NULL);
          return FALSE;
      }

      context->data_len = GUINT32_FROM_LE(context->data_len);
  }

  context->data_count = 0;

  return TRUE;
}

/*GHWPParseContext *ghwp_parse_context_new (GInputStream *stream)*/
/*{*/
/*    g_return_val_if_fail (stream != NULL, NULL);*/
/*    GHWPParseContext *context = g_object_new (GHWP_TYPE_PARSE_CONTEXT, NULL);*/
/*    context->stream = g_object_ref (stream);*/
/*    return context;*/
/*}*/

/**
 * Since: TODO
 */
GHWPParseContext *
ghwp_parse_context_new (GHWPListener *listener, gpointer user_data)
{
  GHWPParseContext *context = g_object_new (GHWP_TYPE_PARSE_CONTEXT, NULL);
  context->listener           = listener;
  context->user_data        = user_data;

  return context;
}

static void parse_doc_info (GHWPFileV5 *file, GHWPDocument *document)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

/*    guint32 id_mappings[16] = {0}; */ /* 반드시 초기화 해야 한다. */
/*    int i;*/

/*    GInputStream *stream  = file->doc_info_stream;
    GHWPParseContext  *context = ghwp_parse_context_new (stream);
    while (ghwp_parse_context_pull (context, error)) {
        switch (context->tag_id) {
        case GHWP_TAG_DOCUMENT_PROPERTIES:*/
            /* TODO */
/*            break;
        case GHWP_TAG_ID_MAPPINGS:*/
/*            for (i = 0; i < sizeof(id_mappings); i = i + sizeof(guint32)) {*/
/*                memcpy(&id_mappings[i], &(context->data[i]), sizeof(guint32));*/
/*                id_mappings[i] = GUINT16_FROM_LE(id_mappings[i]);*/
/*                printf("%d\n", id_mappings[i]);*/
/*            }*/
/*            break;
        default:
            printf("%s:%d: %s not implemented\n", __FILE__, __LINE__,
                _ghwp_get_tag_name (context->tag_id));
            break;
        }
    }

    g_object_unref (context);*/
}

static void parse_section_definition (GHWPParseContext *context)
{
    GError *error = NULL;

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < 2) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }

        printf ("%s\n", ghwp_get_tag_name(context->tag_id));
        g_assert (context->level == 2);
        switch (context->tag_id) {
        case GHWP_TAG_PAGE_DEF:
            break;
        case GHWP_TAG_FOOTNOTE_SHAPE:
            break;
        case GHWP_TAG_PAGE_BORDER_FILL:
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
            break;
        } /* switch */
    } /* while */
}

static void parse_paragraph (GHWPParseContext *context, GHWPFileV5 *file);

/* 머리말 */
static void parse_header (GHWPParseContext *context, GHWPFileV5 *file)
{
    GError *error = NULL;

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < 2) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }
        printf ("%s\n", ghwp_get_tag_name(context->tag_id));

        g_assert (context->level == 2);

        switch (context->tag_id) {
        case GHWP_TAG_LIST_HEADER:
            break;
        case GHWP_TAG_PARA_HEADER:
            parse_paragraph (context, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
            break;
        } /* switch */
    } /* while */
}

/* 각주 */
static void parse_footnote (GHWPParseContext *context, GHWPFileV5 *file)
{
  GError *error = NULL;
  guint16 level = context->level + 1;
  printf("level = %d\n", level);

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < level) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }
        printf ("%s\n", ghwp_get_tag_name(context->tag_id));

        g_assert (context->level == level);

        switch (context->tag_id) {
        case GHWP_TAG_LIST_HEADER:
            break;
        case GHWP_TAG_PARA_HEADER:
            parse_paragraph (context, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
            break;
        } /* switch */
    } /* while */
}

static void parse_tcmt (GHWPParseContext *context, GHWPFileV5 *file)
{
  GError *error = NULL;
  guint16 level = context->level + 1;
  printf("level = %d\n", level);

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < level) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }
        printf ("%s\n", ghwp_get_tag_name(context->tag_id));

        g_assert (context->level == level);

        switch (context->tag_id) {
        case GHWP_TAG_LIST_HEADER:
            break;
        case GHWP_TAG_PARA_HEADER:
            parse_paragraph (context, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
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

static GHWPTable *
ghwp_parse_context_get_table (GHWPParseContext *context, GHWPFileV5 *file)
{
    g_return_val_if_fail (GHWP_IS_PARSE_CONTEXT (context), NULL);
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

    if (ghwp_file_v5_check_version (file, 5, 0, 0, 7)) {
        context_read_uint16 (context, &table->valid_zone_info_size);

        table->zones = g_malloc0_n (table->valid_zone_info_size, 2);

        for (guint i = 0; i < table->valid_zone_info_size; i++) {
            context_read_uint16 (context, &(table->zones[i]));
        }
    }

    if (context->data_count != context->data_len) {
        g_warning ("%s:%d: TABLE data size mismatch at %s\n",
            __FILE__, __LINE__,
            ghwp_file_v5_get_hwp_version_string(GHWP_FILE (file)));
    }
    return table;
}

static void parse_table (GHWPParseContext *context, GHWPFileV5 *file)
{
    GError *error = NULL;
    guint16 level = context->level + 1;
    printf("level = %d\n", level);
    GHWPTable     *table     = NULL;
    GHWPTableCell *cell      = NULL;
    GHWPParagraph *paragraph = NULL;

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < level) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }
        printf ("%s\n", ghwp_get_tag_name(context->tag_id));

        g_assert (context->level == level);

        switch (context->tag_id) {
        case GHWP_TAG_TABLE:
            table = ghwp_parse_context_get_table (context, file);
            break;
        case GHWP_TAG_LIST_HEADER: /* cell */
            cell = ghwp_table_cell_new_from_context (context);
            ghwp_table_add_cell (table, cell);
            break;
        case GHWP_TAG_PARA_HEADER:
            context->state = GHWP_PARSE_STATE_INSIDE_TABLE;
            paragraph = ghwp_paragraph_new ();
            parse_paragraph (context, file);
            break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
            break;
        } /* switch */
    } /* while */
    /* TODO */
    /* add table to where ? */
}

static gchar *ghwp_parse_context_get_text (GHWPParseContext *context)
{
    g_return_val_if_fail (context != NULL, NULL);
    gunichar2 ch; /* guint16 */
    GString  *text = g_string_new("");
    guint     i;

    for (i = 0; i < context->data_len; i = i + 2)
    {
        context_read_uint16 (context, &ch);
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
            context_skip(context, 14);;
            break;
        case 9: /* inline */ /* tab */
            i = i + 14;
            context_skip(context, 14);;
            g_string_append_unichar(text, ch);
            break;
        case 10:
            break;
        case 11:
        case 12:
            i = i + 14;
            context_skip(context, 14);;
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
            context_skip(context, 14);;
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

    if (context->data_count != context->data_len) {
        g_string_free(text, TRUE);
        return NULL;
    }

    return g_string_free(text, FALSE);
}

static void parse_shape_component (GHWPParseContext *context, GHWPFileV5 *file)
{
  GError *error = NULL;
  guint16 level = context->level + 1;
  printf("level = %d\n", level);

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < level) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }
        printf ("%s\n", ghwp_get_tag_name(context->tag_id));

        g_assert (context->level == level);

        switch (context->tag_id) {
        case GHWP_TAG_SHAPE_COMPONENT_PICTURE:
          break;
        case GHWP_TAG_SHAPE_COMPONENT_LINE:
          break;
        case GHWP_TAG_SHAPE_COMPONENT_POLYGON:
          break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
            break;
        } /* switch */
    } /* while */
}

static void
parse_drawing_shape_object (GHWPParseContext *context, GHWPFileV5 *file)
{
  GError *error = NULL;
  guint16 level = context->level + 1;
  printf("level = %d\n", level);

    while (ghwp_parse_context_pull(context, &error)) {
        if (context->level < level) {
            context->state = GHWP_PARSE_STATE_PASSING;
            break;
        }

        printf ("%d", context->level);
        for (int i = 0; i < context->level; i++) {
            printf (" ");
        }
        printf ("%s\n", ghwp_get_tag_name(context->tag_id));

        g_assert (context->level == level);

        switch (context->tag_id) {
        case GHWP_TAG_SHAPE_COMPONENT:
          parse_shape_component (context, file);
          break;
        default:
            g_error ("%s:%d:%s not implemented",
                __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
            break;
        } /* switch */
    } /* while */
}

static void parse_paragraph (GHWPParseContext *context, GHWPFileV5 *file)
{
  GError *error = NULL;
  guint16 level = context->level + 1;
  printf("level = %d\n", level);
  gchar *text = NULL;

  while (ghwp_parse_context_pull(context, &error)) {
    if (context->level < level) {
        context->state = GHWP_PARSE_STATE_PASSING;
        break;
    }

    printf ("%d", context->level);
    for (int i = 0; i < context->level; i++) {
      printf (" ");
    }
    printf ("%s\n", ghwp_get_tag_name(context->tag_id));

    g_assert (context->level == level);

    switch (context->tag_id) {
    case GHWP_TAG_PARA_TEXT:
      text = ghwp_parse_context_get_text (context);
      if (text != NULL || *text != '\0')
        printf("%s\n", text);
      else
        printf("(null)\n");

      g_free (text);
      text = NULL;
      break;
    case GHWP_TAG_PARA_CHAR_SHAPE:
      break;
    case GHWP_TAG_PARA_LINE_SEG:
      break;
    case GHWP_TAG_CTRL_HEADER:
      context_read_uint32 (context, &context->ctrl_id);
      printf (" \"%c%c%c%c\"\n",
        (gchar) (context->ctrl_id >> 24 & 0xff),
        (gchar) (context->ctrl_id >> 16 & 0xff),
        (gchar) (context->ctrl_id >>  8 & 0xff),
        (gchar) (context->ctrl_id >>  0 & 0xff));
      switch (context->ctrl_id) {
      case CTRL_ID_SECTION_DEF:
        parse_section_definition (context);
        break;
      case CTRL_ID_NEW_NUM:
        break;
      case CTRL_ID_COLUMN_DEF:
        break;
      case CTRL_ID_HEADEDR: /* 머리말 */
        parse_header (context, file);
        break;
      case CTRL_ID_AUTO_NUM:
        break;
      case CTRL_ID_TABLE:
        parse_table (context, file);
        break;
      case CTRL_ID_FOOTNOTE: /* 각주 */
        parse_footnote (context, file);
        break;
      case CTRL_ID_PAGE_HIDE:
        break;
      case CTRL_ID_DRAWING_SHAPE_OBJECT:
        parse_drawing_shape_object (context, file);
        break;
      case CTRL_ID_TCMT: /* 숨은 설명 */
        parse_tcmt (context, file);
        break;
      case CTRL_ID_TCPS:
        break;
      default:
        g_error ("%s:%d:\"%c%c%c%c\":%s not implemented",
            __FILE__, __LINE__,
            (gchar) (context->ctrl_id >> 24 & 0xff),
            (gchar) (context->ctrl_id >> 16 & 0xff),
            (gchar) (context->ctrl_id >>  8 & 0xff),
            (gchar) (context->ctrl_id >>  0 & 0xff),
            ghwp_get_ctrl_name (context->ctrl_id));
        break;
      }
      break;
    default:
      g_error ("%s:%d:%s not implemented",
          __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
      break;
    } /* switch */
  } /* while */
}

static void parse_section (GHWPParseContext *context,
                           GHWPFileV5       *file)
{
  GError *error = NULL;

  while (ghwp_parse_context_pull(context, &error))
  {
    printf ("%d", context->level);
    for (int i = 0; i < context->level; i++) {
        printf (" ");
    }
    printf ("%s\n", ghwp_get_tag_name(context->tag_id));
    context_skip (context, context->data_len);
    g_assert (context->level == 0);

    switch (context->tag_id) {
    case GHWP_TAG_PARA_HEADER:
/*        context->listener->paragraph();*/
        parse_paragraph (context, file);
        break;
    default:
        g_error ("%s:%d:%s not implemented",
            __FILE__, __LINE__, ghwp_get_tag_name (context->tag_id));
        break;
    }  /* switch */
  } /* while */
}

static void parse_sections (GHWPParseContext *context, GHWPFileV5 *file)
{
  for (guint i = 0; i < file->section_streams->len; i++)
  {
    GInputStream *stream = g_array_index (file->section_streams,
                                          GInputStream *, i);
    context->stream = stream;
    parse_section  (context, file);
  }
}

static void parse_body_text (GHWPParseContext *context, GHWPFileV5 *file)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    parse_sections (context, file);
}

static void parse_view_text (GHWPParseContext *context, GHWPFileV5 *file)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    parse_sections (context, file);
}

/* 알려지지 않은 것을 감지하기 위해 이렇게 작성함 */
static void metadata_hash_func (gpointer k, gpointer v, gpointer user_data)
{
    gchar        *name  = (gchar        *) k;
    GsfDocProp   *prop  = (GsfDocProp   *) v;
    GHWPDocument *doc   = (GHWPDocument *) user_data;
    GValue const *value = gsf_doc_prop_get_val (prop);

    if ( g_str_equal(name, GSF_META_NAME_CREATOR) ) {
        doc->creator = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_DATE_MODIFIED) ) {
        GsfTimestamp *ts    = g_value_get_boxed (value);
        doc->mod_date = (GTime) ts->timet;
    } else if ( g_str_equal(name, GSF_META_NAME_DESCRIPTION) ) {
        doc->desc = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_KEYWORDS) ) {
        doc->keywords = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_SUBJECT) ) {
        doc->subject = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_TITLE) ) {
        doc->title = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_LAST_PRINTED) ) {
        GsfTimestamp *ts    = g_value_get_boxed (value);
        doc->last_printed   = (GTime) ts->timet;
    } else if ( g_str_equal(name, GSF_META_NAME_LAST_SAVED_BY) ) {
        doc->last_saved_by = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_DATE_CREATED) ) {
        GsfTimestamp *ts    = g_value_get_boxed (value);
        doc->creation_date  = (GTime) ts->timet;
    /* hwp 문서를 저장할 때 사용된 한컴 워드프로세서의 내부 버전 */
    } else if ( g_str_equal(name, GSF_META_NAME_REVISION_COUNT) ) {
        doc->hanword_version = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_PAGE_COUNT) ) {
        /* not correct n_pages == 0 ?? */
        /* g_value_get_int (value); */
    } else {
        g_warning("%s:%d:%s not implemented\n", __FILE__, __LINE__, name);
    }
}

static void parse_summary_info (GHWPFileV5 *file, GHWPDocument *document)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    GsfInputStream *gis;
    gssize          size;
    guint8         *buf = NULL;
    GsfInputMemory *summary;
    GsfDocMetaData *meta;
    GError         *error = NULL;

    gis  = g_object_ref (file->summary_info_stream);
    size = gsf_input_stream_size (gis);
    buf  = g_malloc(size);

    g_input_stream_read ((GInputStream*) gis, buf, (gsize) size, NULL, &error);

    if (error != NULL) {
        buf = (g_free (buf), NULL);
        g_object_unref (gis);
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        g_clear_error (&error);
        return;
    }

    /* changwoo's solution, thanks to changwoo.
     * https://groups.google.com/d/msg/libhwp/gFDD7UMCXBc/tyR3wOXoRIoJ
     * https://github.com/changwoo/gnome-hwp-support/blob/master/properties/props-data.c
     * Trick the libgsf's MSOLE property set listener, by changing
     * its GUID. The \005HwpSummaryInformation is compatible with
     * the summary property set.
     */
    guint8 component_guid [] = {
        0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
        0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
    };

    if (size >= sizeof(component_guid) + 28) {
        memcpy (buf + 28, component_guid, (gsize) sizeof(component_guid));
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

    gsf_doc_meta_data_foreach (meta, metadata_hash_func, document);

    document->summary_info = g_object_ref (meta);
    g_object_unref (meta);
    g_object_unref (summary);
    buf = (g_free (buf), NULL);
    g_object_unref (gis);
}

static void parse_prv_text (GHWPFileV5 *file, GHWPDocument *document)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    GsfInputStream *gis   = g_object_ref (file->prv_text_stream);
    gssize          size  = gsf_input_stream_size (gis);
    guchar         *buf   = g_new (guchar, size);
    GError         *error = NULL;

    g_input_stream_read ((GInputStream*) gis, buf, size, NULL, &error);

    if (error) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        g_free (document->prv_text);
        g_clear_error (&error);
        buf = (g_free (buf), NULL);
        g_object_unref (gis);
        return;
    }

    /* g_convert() can be used to convert a byte buffer of UTF-16 data of
       ambiguous endianess. */
    document->prv_text = g_convert ((const gchar*) buf, (gssize) size,
                               "UTF-8", "UTF-16LE", NULL, NULL, &error);

    if (error) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        g_free (document->prv_text);
        g_clear_error (&error);
        buf = (g_free (buf), NULL);
        g_object_unref (gis);
        return;
    }

    buf = (g_free (buf), NULL);
    g_object_unref (gis);
}

gboolean ghwp_parse_context_check_version (GHWPParseContext *context,
                                           guint8            major,
                                           guint8            minor,
                                           guint8            micro,
                                           guint8            extra)
{
    g_return_val_if_fail (GHWP_IS_PARSE_CONTEXT (context), FALSE);

    return (context->major_version >  major)   ||
           (context->major_version == major &&
            context->minor_version >  minor)   ||
           (context->major_version == major &&
            context->minor_version == minor &&
            context->micro_version >  micro)   ||
           (context->major_version == major &&
            context->minor_version == minor &&
            context->micro_version == micro &&
            context->extra_version >= extra);
}

/*
 * Since: TODO
 */
void ghwp_parse_context_parse (GHWPParseContext *context,
                               GHWPFileV5       *file,
                               GError          **error)
{
  g_return_if_fail (context != NULL);
  GHWPListenerInterface *iface = GHWP_LISTENER_GET_IFACE (context->listener);
  if (iface->document_version)
    iface->document_version (context->listener,
                             file->major_version,
                             file->minor_version,
                             file->micro_version,
                             file->extra_version,
                             context->user_data,
                             error);

/*    parse_doc_info       (file, document);*/
  parse_body_text      (context, file);
/*    parse_view_text      (file, document);*/
/*    parse_summary_info   (file, document);*/
/*    parse_bin_data       (file, document);*/
/*    parse_prv_text       (file, document);*/
/*    parse_prv_image      (file, document);*/
/*    parse_doc_options    (file, document);*/
/*    parse_scripts        (file, document);*/
/*    parse_xml_template   (file, document);*/
/*    parse_doc_history    (file, document);*/
}

static void ghwp_parse_context_init (GHWPParseContext * context)
{
  context->state = GHWP_PARSE_STATE_NORMAL;
  context->priv  = G_TYPE_INSTANCE_GET_PRIVATE (context,
                                                GHWP_TYPE_PARSE_CONTEXT,
                                                GHWPParseContextPrivate);
}

static void ghwp_parse_context_finalize (GObject *obj)
{
  GHWPParseContext *context = GHWP_PARSE_CONTEXT(obj);
  if (G_IS_INPUT_STREAM (context->stream)) {
    g_input_stream_close (context->stream, NULL, NULL);
    g_object_unref (context->stream);
  }
  G_OBJECT_CLASS (ghwp_parse_context_parent_class)->finalize (obj);
}

static void ghwp_parse_context_class_init (GHWPParseContextClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (GHWPParseContextPrivate));
    object_class->finalize = ghwp_parse_context_finalize;
}

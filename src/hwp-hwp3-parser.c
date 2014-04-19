/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-parser.c
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

#include "hwp-hwp3-parser.h"
#include "hwp-hwp3-file.h"
#include "hwp-charset.h"
#include <math.h>
#include <stdlib.h>

G_DEFINE_TYPE (HwpHWP3Parser, hwp_hwp3_parser, G_TYPE_OBJECT);

gboolean hwp_hwp3_parser_read (HwpHWP3Parser *parser, void *buffer, gsize count)
{
  g_return_val_if_fail (parser != NULL, FALSE);

  gboolean is_success = FALSE;
  is_success = g_input_stream_read_all (parser->stream, buffer, count,
                                        &parser->bytes_read,
                                        NULL, NULL);
  if ((is_success == FALSE) || (parser->bytes_read == 0))
  {
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }

  return TRUE;
}

gboolean hwp_hwp3_parser_read_uint8 (HwpHWP3Parser *parser, guint8 *i)
{
  g_return_val_if_fail (parser != NULL, FALSE);

  gboolean is_success = FALSE;
  is_success = g_input_stream_read_all (parser->stream, i, 1,
                                        &parser->bytes_read,
                                        NULL, NULL);
  if ((is_success == FALSE) || (parser->bytes_read != 1))
  {
    *i = 0;
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }

  return TRUE;
}

gboolean hwp_hwp3_parser_read_uint16 (HwpHWP3Parser *parser, guint16 *i)
{
  g_return_val_if_fail (parser != NULL, FALSE);

  gboolean is_success = FALSE;
  is_success = g_input_stream_read_all (parser->stream, i, 2,
                                        &parser->bytes_read,
                                        NULL, NULL);
  if ((is_success == FALSE) || (parser->bytes_read != 2))
  {
    *i = 0;
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }
  *i = GUINT16_FROM_LE(*i);

  return TRUE;
}

gboolean hwp_hwp3_parser_read_uint32 (HwpHWP3Parser *parser, guint32 *i)
{
  g_return_val_if_fail (parser != NULL, FALSE);

  gboolean is_success = FALSE;
  is_success = g_input_stream_read_all (parser->stream, i, 4,
                                        &parser->bytes_read,
                                        NULL, NULL);
  if ((is_success == FALSE) || (parser->bytes_read != 4))
  {
    *i = 0;
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }
  *i = GUINT32_FROM_LE(*i);

  return TRUE;
}

gboolean hwp_hwp3_parser_skip (HwpHWP3Parser *parser, guint16 count)
{
  g_return_val_if_fail (parser != NULL, FALSE);

  gboolean is_success = FALSE;
  guint8  *buf        = g_malloc (count);

  is_success = g_input_stream_read_all (parser->stream, buf, (gsize) count,
                                        &parser->bytes_read,
                                        NULL, NULL);
  g_free (buf);

  if ((is_success == FALSE) || (parser->bytes_read != (gsize) count))
  {
    g_warning ("%s:%d:skip size mismatch\n", __FILE__, __LINE__);
    g_input_stream_close (parser->stream, NULL, NULL);
    return FALSE;
  }

  return TRUE;
}

static void _hwp_hwp3_parser_parse_signature (HwpHWP3Parser *parser,
                                              HwpHWP3File   *file,
                                              GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_PARSER (parser));
  gchar *signature = g_malloc(30);
  hwp_hwp3_parser_read (parser, signature, 30);
  g_free (signature);
}

static void _hwp_hwp3_parser_parse_doc_info (HwpHWP3Parser *parser,
                                             HwpHWP3File   *file,
                                             GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));
  /* 문서 정보 128 bytes */
  /* 암호 여부 */
  hwp_hwp3_parser_skip (parser, 96);
  hwp_hwp3_parser_read_uint16 (parser, &(file->is_crypt));

  /* offset: 124 압축 여부, 0이면 비압축 그외 압축 */
  hwp_hwp3_parser_skip (parser, 26);
  hwp_hwp3_parser_read_uint8 (parser, &(file->is_compress));
  /* sub revision */
  hwp_hwp3_parser_read_uint8 (parser, &(file->rev));
  file->major_version = 3;
  file->minor_version = 0;
  file->micro_version = 0;
  file->extra_version = file->rev;

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);
  if (iface->document_version)
    iface->document_version (parser->listener,
                             file->major_version,
                             file->minor_version,
                             file->micro_version,
                             file->extra_version,
                             parser->user_data,
                             error);

  /* 정보 블럭 길이 */
  hwp_hwp3_parser_read_uint16 (parser, &(file->info_block_len));
}

static void _hwp_hwp3_parser_parse_summary_info (HwpHWP3Parser *parser,
                                                 HwpHWP3File   *file,
                                                 GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));

  GString *string[9];
  guint16  c;
  guint8   count = 0;
  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);

  for (guint i = 0; i < 9; i++)
  {
    count = 0;
    string[i] = g_string_new (NULL);

    while (count < 112)
    {
      hwp_hwp3_parser_read_uint16 (parser, &c);
      count += 2;

      if (c != 0)
      {
        gchar *str = hwp_hnchar_to_utf8 (c);
        g_string_append (string[i], str);
        g_free (str);
      } else {
        hwp_hwp3_parser_skip (parser, 112 - count);
        break;
      }
    }
  } /* for */

  if (iface->summary_info) {
    HwpSummaryInfo *info = hwp_summary_info_new ();

    info->title = g_string_free (string[0], FALSE);
    info->subject = g_string_free (string[1], FALSE);
    info->creator = g_string_free (string[2], FALSE);

    /* str format: "2001년 10월 11일 목요일, 20시 48분" */
    gchar *s = g_string_free (string[3], FALSE);
    GRegex *regex;
    regex = g_regex_new ("^([0-9]{4})년 ([0-9]{1,2})월 ([0-9]{1,2})일 .요일, ([0-9]{1,2})시 ([0-9]{1,2})분", 0, 0, error);
    GMatchInfo *match_info;
    g_regex_match (regex, s, 0, &match_info);
    gint match_num = g_match_info_get_match_count (match_info);

    gchar *year   = NULL;
    gchar *month  = NULL;
    gchar *day    = NULL;
    gchar *hour   = NULL;
    gchar *minute = NULL;

    if (match_num == 6) {
      year   = g_match_info_fetch (match_info, 1);
      month  = g_match_info_fetch (match_info, 2);
      day    = g_match_info_fetch (match_info, 3);
      hour   = g_match_info_fetch (match_info, 4);
      minute = g_match_info_fetch (match_info, 5);



      GDateTime *datetime = g_date_time_new_local (atoi (year),
                                                   atoi (month),
                                                   atoi (day),
                                                   atoi (hour),
                                                   atoi (minute),
                                                   0.0);

      g_free (year);
      g_free (month);
      g_free (day);
      g_free (hour);
      g_free (minute);

      GTime time = g_date_time_to_unix (datetime);

      info->mod_date = time;
    }

    g_match_info_free (match_info);
    g_regex_unref (regex);

    iface->summary_info (parser->listener, info, parser->user_data, error);

  }
  /* TODO */
  /* 4 ~ 8 */
}

static void _hwp_hwp3_parser_parse_info_block (HwpHWP3Parser *parser,
                                               HwpHWP3File   *file,
                                               GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));

  GInputStream *stream = file->priv->stream;
  g_input_stream_skip (stream, file->info_block_len, NULL, error);
}

static void _hwp_hwp3_parser_parse_font_names (HwpHWP3Parser *parser,
                                               HwpHWP3File   *file,
                                               GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));
  guint16 n_fonts;
  guint8 *buffer = NULL;
  gsize bytes_read;
  GInputStream *stream = HWP_HWP3_FILE (file)->priv->stream;

  for (guint8 i = 0; i < 7; i++)
  {
    g_input_stream_read_all (stream, &n_fonts, 2, &bytes_read, NULL, NULL);
    buffer = g_malloc (40 * n_fonts);
    g_input_stream_read_all (stream, buffer, 40 * n_fonts, &bytes_read, NULL, NULL);
    gchar *fontname = g_convert ((const gchar*) buffer, bytes_read,
                                 "UTF-8", "JOHAB", NULL, NULL, error);
    g_free (buffer);
    g_free (fontname);
  }
}

static void _hwp_hwp3_parser_parse_styles (HwpHWP3Parser *parser,
                                           HwpHWP3File   *file,
                                           GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));
  guint16 n_styles;
  guint8 *buffer = NULL;
  gsize bytes_read;
  GInputStream *stream = HWP_HWP3_FILE (file)->priv->stream;
  g_input_stream_read_all (stream, &n_styles, 2, &bytes_read, NULL, error);

  for (guint16 i = 0; i < n_styles; i++)
  {
    buffer = g_malloc (20 + 31 + 187);
    g_input_stream_read_all (stream, buffer, 20 + 31 + 187, &bytes_read, NULL, error);
    gchar *stylename = g_convert ((const gchar*) buffer, 20,
                                  "UTF-8", "JOHAB", NULL, NULL, error);
    g_free (buffer);
    g_free (stylename);
  }
}

static gboolean _hwp_hwp3_parser_parse_paragraph (HwpHWP3Parser *parser,
                                                  HwpHWP3File   *file,
                                                  GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP3_FILE (file), FALSE);

  /* 문단 정보 */
  guint8  prev_paragraph_shape;
  guint16 n_chars;
  guint16 n_lines;
  guint8  char_shape_included;
  guint8  flag;


  hwp_hwp3_parser_read_uint8  (parser, &prev_paragraph_shape);
  hwp_hwp3_parser_read_uint16 (parser, &n_chars);
  hwp_hwp3_parser_read_uint16 (parser, &n_lines);
  hwp_hwp3_parser_read_uint8  (parser, &char_shape_included);

  hwp_hwp3_parser_skip (parser, 1 + 4 + 1 + 31);
  /* 여기까지 43 바이트 */

  if (prev_paragraph_shape == 0 && n_chars > 0)
    hwp_hwp3_parser_skip (parser, 187);

  /* 빈문단이면 FALSE 반환 */
  if (n_chars == 0)
    return FALSE;

  /* 줄 정보 */
  hwp_hwp3_parser_skip (parser, n_lines * 14);

  /* 글자 모양 정보 */
  if (char_shape_included != 0) {
    for (guint16 i = 0; i < n_chars; i++) {
      hwp_hwp3_parser_read_uint8 (parser, &flag);
      if (flag != 1) {
        hwp_hwp3_parser_skip (parser, 31);
      }
    }
  }

  HwpParagraph *paragraph = hwp_paragraph_new ();
  GString      *string    = g_string_new (NULL);

  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);

  /* 글자들 */
  guint16 n_chars_read = 0;
  guint16 c;

  while (n_chars_read < n_chars)
  {
    hwp_hwp3_parser_read_uint16 (parser, &c);
    n_chars_read += 1;

    if (c == 6) {
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6 + 34);
      continue;
    } else if (c == 9) { /* tab */
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6);
      g_string_append (string, "\t");
      continue;
    } else if (c == 10) { /* table */
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6);
      /* 테이블 식별 정보 84 바이트 */
      hwp_hwp3_parser_skip (parser, 80);

      guint16 n_cells;
      hwp_hwp3_parser_read_uint16 (parser, &n_cells);

      hwp_hwp3_parser_skip (parser, 2);
      hwp_hwp3_parser_skip (parser, 27 * n_cells);

      /* <셀 문단 리스트>+ */
      for (guint16 i = 0; i < n_cells; i++) {
        /* <셀 문단 리스트> ::= <셀 문단>+ <빈문단> */
        while(_hwp_hwp3_parser_parse_paragraph (parser, file, error))
        {
        }
      }

      /* <캡션 문단 리스트> ::= <캡션 문단>+ <빈문단> */
      while(_hwp_hwp3_parser_parse_paragraph(parser, file, error))
      {
      }
      continue;
    } else if (c == 11) {
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6);
      guint32 len;
      hwp_hwp3_parser_read_uint32 (parser, &len);
      hwp_hwp3_parser_skip (parser, 344);
      hwp_hwp3_parser_skip (parser, len);
      /* <캡션 문단 리스트> ::= <캡션 문단>+ <빈문단> */
      while(_hwp_hwp3_parser_parse_paragraph(parser, file, error))
      {
      }
      continue;
    } else if (c == 13) { /* 글자들 끝 */
      g_string_append (string, "\n");
      continue;
    } else if (c == 16) {
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6);
      hwp_hwp3_parser_skip (parser, 10);
      /* <문단 리스트> ::= <문단>+ <빈문단> */
      while(_hwp_hwp3_parser_parse_paragraph (parser, file, error))
      {
      }
      continue;
    } else if (c == 17) { /* 각주/미주 */
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6);
      hwp_hwp3_parser_skip (parser, 14);
      while(_hwp_hwp3_parser_parse_paragraph (parser, file, error))
      {
      }
      continue;
    } else if (c == 18 || c == 19 || c == 20 || c == 21) {
      n_chars_read += 3;
      hwp_hwp3_parser_skip (parser, 6);
      continue;
    } else if (c == 23) { /*글자 겹침 */
      n_chars_read += 4;
      hwp_hwp3_parser_skip (parser, 8);
      continue;
    } else if (c == 24 || c == 25) {
      n_chars_read += 2;
      hwp_hwp3_parser_skip (parser, 4);
      continue;
    } else if (c == 28) { /* 개요 모양/번호 */
      n_chars_read += 31;
      hwp_hwp3_parser_skip (parser, 62);
      continue;
    } else if (c == 30 || c == 31) {
      n_chars_read += 1;
      hwp_hwp3_parser_skip (parser, 2);
      continue;
    } else if (c >= 0x0020 && c <= 0xffff) {
      gchar *tmp = hwp_hnchar_to_utf8 (c);
      g_string_append (string, tmp);
      g_free (tmp);
      continue;
    } else {
      g_warning ("special character: %04x", c);
    } /* if */
  } /* while */

  /* FIXME 다음 내포된 ;문단이 먼저 넘어간 후 이곳 문단이 넘어가는 버그가 있다. */
  hwp_paragraph_set_string (paragraph, string);

  if (iface->paragraph)
    iface->paragraph (parser->listener,
                      paragraph,
                      parser->user_data,
                      error);

  return TRUE;
}

static void _hwp_hwp3_parser_parse_paragraphs (HwpHWP3Parser *parser,
                                               HwpHWP3File   *file,
                                               GError       **error)
{
  /* <문단 리스트> ::= <문단>+ <빈문단> */
  while(_hwp_hwp3_parser_parse_paragraph (parser, file, error))
  {
  }
}

static void _hwp_hwp3_parser_parse_supplementary_info_block1 (HwpHWP3Parser *parser,
                                                              HwpHWP3File   *file,
                                                              GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));
}

static void _hwp_hwp3_parser_parse_supplementary_info_block2 (HwpHWP3Parser *parser,
                                                              HwpHWP3File   *file,
                                                              GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));
}

/**
 * Since: 0.0.1
 */
void hwp_hwp3_parser_parse (HwpHWP3Parser *parser,
                            HwpHWP3File   *file,
                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));

  parser->stream = file->priv->stream;

  _hwp_hwp3_parser_parse_signature (parser, file, error);
  _hwp_hwp3_parser_parse_doc_info (parser, file, error);
  _hwp_hwp3_parser_parse_summary_info (parser, file, error);
  _hwp_hwp3_parser_parse_info_block (parser, file, error);

  if (file->is_compress) {
    GZlibDecompressor *zd;
    GInputStream      *cis;

    zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
    cis = g_converter_input_stream_new (file->priv->stream, G_CONVERTER (zd));
    g_filter_input_stream_set_close_base_stream (G_FILTER_INPUT_STREAM (cis),
                                                 TRUE);
    g_object_unref (file->priv->stream);
    file->priv->stream = g_object_ref (cis);
    g_object_unref (zd);
  }

  parser->stream = file->priv->stream;

  _hwp_hwp3_parser_parse_font_names (parser, file, error);
  _hwp_hwp3_parser_parse_styles (parser, file, error);
  _hwp_hwp3_parser_parse_paragraphs (parser, file, error);
  _hwp_hwp3_parser_parse_supplementary_info_block1 (parser, file, error);
  _hwp_hwp3_parser_parse_supplementary_info_block2 (parser, file, error);
}

/**
 * Since: 0.0.1
 */
HwpHWP3Parser *hwp_hwp3_parser_new (HwpListener *listener,
                                    gpointer     user_data)
{
  g_return_val_if_fail (HWP_IS_LISTENER (listener), NULL);

  HwpHWP3Parser *parser = g_object_new (HWP_TYPE_HWP3_PARSER, NULL);
  parser->listener      = listener;
  parser->user_data     = user_data;

  return parser;
}

static void hwp_hwp3_parser_init (HwpHWP3Parser *parser)
{
}

static void hwp_hwp3_parser_finalize (GObject *object)
{
  HwpHWP3Parser *parser = HWP_HWP3_PARSER (object);
  g_object_unref (parser->stream);

  G_OBJECT_CLASS (hwp_hwp3_parser_parent_class)->finalize (object);
}

static void hwp_hwp3_parser_class_init (HwpHWP3ParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hwp_hwp3_parser_finalize;
}

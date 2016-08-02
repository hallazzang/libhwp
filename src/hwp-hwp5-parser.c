/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp5-parser.c
 * This file is part of the libhwp project.
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
 * along with this program;  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This software has been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include <gsf/gsf-doc-meta-data.h>
#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-utils.h>
#include <stdio.h>
#include "hwp-hwp5-parser.h"
#include "hwp-charset.h"

G_DEFINE_TYPE (HwpHWP5Parser, hwp_hwp5_parser, G_TYPE_OBJECT);

static HwpParagraph *hwp_hwp5_parser_build_paragraph (HwpHWP5Parser *parser,
                                                      HwpHWP5File   *file,
                                                      GError       **error);

#define WARNING_TAG_NOT_IMPLEMENTED(tag_id) \
{ \
  const gchar *name = hwp_get_tag_name ((tag_id)); \
  if (name) { \
    g_warning ("%s:%d: %s not implemented\n", __FILE__, __LINE__, name); \
  } else { \
    g_warning ("%s:%d: unknown tag id: %d is not implemented\n", \
               __FILE__, __LINE__, (tag_id)); \
  } \
}

#define WARNING_CTRL_NOT_IMPLEMENTED(ctrl_id) \
{ \
  const gchar *name = hwp_get_ctrl_name ((ctrl_id)); \
  if (name) { \
    g_warning ("%s:%d: %s not implemented\n", __FILE__, __LINE__, name); \
  } else { \
    g_warning ("%s:%d:\"%c%c%c%c\" not implemented, please implement CTRL_ID", \
               __FILE__, __LINE__, \
               (gchar) ((ctrl_id) >> 24 & 0xff), \
               (gchar) ((ctrl_id) >> 16 & 0xff), \
               (gchar) ((ctrl_id) >>  8 & 0xff), \
               (gchar) ((ctrl_id) >>  0 & 0xff)); \
  } \
}

gboolean parser_skip (HwpHWP5Parser *parser, guint32 count)
{
  gboolean is_success = FALSE;
  gsize bytes_read = 0;

  guint8 *buf = g_malloc (count);
  is_success = g_input_stream_read_all (parser->stream, buf, count,
                                        &bytes_read, NULL, NULL);
  g_free (buf);

  if ((is_success == FALSE) || (bytes_read != count))
  {
    g_warning ("%s:%d: count:%d, bytes_read:%" G_GSIZE_FORMAT
               " skip size mismatch\n", __FILE__, __LINE__, count, bytes_read);
    return FALSE;
  }

  parser->data_pos += count;
  return TRUE;
}

gboolean parser_read_bytes (HwpHWP5Parser *parser,
                            void          *buffer,
                            guint32        count,
                            GError       **error)
{
#ifdef HWP_ENABLE_DEBUG
  g_assert (parser->data_pos + count <= parser->data_len);
#endif
  g_return_val_if_fail (parser->data_pos + count <= parser->data_len, FALSE);

  gboolean is_success = FALSE;
  gsize bytes_read = 0;

  is_success = g_input_stream_read_all (parser->stream, buffer, count,
                                        &bytes_read, NULL, NULL);

  if ((is_success == FALSE) || (bytes_read != count))
  {
    g_warning ("%s:%d: size mismatch\n", __FILE__, __LINE__);
    return FALSE;
  }

  parser->data_pos += count;
  return TRUE;
}

gboolean parser_read_uint8 (HwpHWP5Parser *parser, guint8 *i, GError **error)
{
#ifdef HWP_ENABLE_DEBUG
  g_assert (parser->data_pos + 1 <= parser->data_len);
#endif
  g_return_val_if_fail (parser->data_pos + 1 <= parser->data_len, FALSE);

  gsize bytes_read = 0;
  g_input_stream_read_all (parser->stream, i, 1,
                           &bytes_read, NULL, error);
  if (*error || (bytes_read != 1)) {
    *i = 0;
    return FALSE;
  }

  parser->data_pos += 1;
  return TRUE;
}

gboolean parser_read_uint16 (HwpHWP5Parser *parser, guint16 *i, GError **error)
{
#ifdef HWP_ENABLE_DEBUG
  g_assert (parser->data_pos + 2 <= parser->data_len);
#endif
  g_return_val_if_fail (parser->data_pos + 2 <= parser->data_len, FALSE);

  gsize bytes_read = 0;
  g_input_stream_read_all (parser->stream, i, 2,
                           &bytes_read, NULL, error);
  if (*error || (bytes_read != 2)) {
    *i = 0;
    return FALSE;
  }

  *i = GUINT16_FROM_LE(*i);
  parser->data_pos += 2;
  return TRUE;
}

gboolean parser_read_int16 (HwpHWP5Parser *parser, gint16 *i, GError **error)
{
#ifdef HWP_ENABLE_DEBUG
  g_assert (parser->data_pos + 2 <= parser->data_len);
#endif
  g_return_val_if_fail (parser->data_pos + 2 <= parser->data_len, FALSE);

  gsize bytes_read = 0;
  g_input_stream_read_all (parser->stream, i, 2,
                           &bytes_read, NULL, error);
  if (*error || (bytes_read != 2)) {
    *i = 0;
    return FALSE;
  }

  *i = GINT16_FROM_LE(*i);
  parser->data_pos += 2;
  return TRUE;
}

gboolean parser_read_uint32 (HwpHWP5Parser *parser, guint32 *i, GError **error)
{
  g_return_val_if_fail (parser->data_pos + 4 <= parser->data_len, FALSE);

  gsize bytes_read = 0;
  g_input_stream_read_all (parser->stream, i, 4,
                           &bytes_read, NULL, error);
  if (*error || (bytes_read != 4)) {
    *i = 0;
    return FALSE;
  }
  *i = GUINT32_FROM_LE(*i);
  parser->data_pos += 4;
  return TRUE;
}

gboolean parser_read_int32 (HwpHWP5Parser *parser, gint32 *i, GError **error)
{
  g_return_val_if_fail (parser->data_pos + 4 <= parser->data_len, FALSE);

  gsize bytes_read = 0;
  g_input_stream_read_all (parser->stream, i, 4,
                           &bytes_read, NULL, error);
  if (*error || (bytes_read != 4)) {
    *i = 0;
    return FALSE;
  }
  *i = GINT32_FROM_LE(*i);
  parser->data_pos += 4;
  return TRUE;
}

gboolean
parser_read_color (HwpHWP5Parser *parser, HwpColor *color, GError **error)
{
  guint32 colorref;
  parser_read_uint32 (parser, &colorref, error);
  color->red   = (colorref & 0x000000ff >>  0) * 0xff;
  color->green = (colorref & 0x0000ff00 >>  8) * 0xff;
  color->blue  = (colorref & 0x00ff0000 >> 16) * 0xff;

  if (*error)
    return FALSE;

  return TRUE;
}

/**
 * hwp_hwp5_parser_pull:
 * @parser: #HwpHWP5Parser
 * @error: #GError
 *
 * On a successful pull, %TRUE is returned.
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

  if (parser->data_len > parser->data_pos)
  {
    parser_skip (parser, parser->data_len - parser->data_pos);
  }

  /* 4바이트 읽기 */
  gsize bytes_read = 0;
  g_input_stream_read_all (parser->stream, &parser->header, 4,
                           &bytes_read, NULL, error);
  if (*error) {
    g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
    return FALSE;
  }

  /* if invalid or end-of-stream */
  if (bytes_read != 4) {
    /* if not end-of-stream */
    if (bytes_read != 0)
      g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                           _("File corrupted or invalid file"));

    return FALSE;
  }

  parser->header = GUINT32_FROM_LE(parser->header);
  /* 4바이트 헤더 디코딩하기 */
  parser->tag_id   = (guint16) ( parser->header        & 0x3ff);
  parser->level    = (guint16) ((parser->header >> 10) & 0x3ff);
  parser->data_len = (guint32) ((parser->header >> 20) & 0xfff);
  /* 비정상 */
  if (parser->data_len == 0)
    goto FAIL;
  /* data_len == 0xfff 이면 다음 4바이트는 data_len 이다 */
  if (parser->data_len == 0xfff)
  {
    g_input_stream_read_all (parser->stream, &parser->data_len, 4,
                             &bytes_read, NULL, error);
    if (*error) {
      g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
      return FALSE;
    }
    /* 비정상 */
    if (bytes_read != 4) {
      g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                           _("File corrupted"));
      return FALSE;
    }
    parser->data_len = GUINT32_FROM_LE(parser->data_len);
  }

#ifdef HWP_ENABLE_DEBUG
  printf ("%d", parser->level);

  const gchar *name = hwp_get_tag_name (parser->tag_id);
  if (name)
  {
    for (int i = 0; i < parser->level; i++)
      printf ("    ");

    printf ("%s\n", name);
  }
  else
  {
    printf ("\nunknown tag id: %d\n", parser->tag_id);
  }
#endif

  parser->data_pos = 0;

  return TRUE;

  FAIL:
  g_set_error_literal (error, HWP_ERROR, HWP_ERROR_INVALID,
                       _("File corrupted"));
  return FALSE;
}

/**
 * hwp_hwp5_parser_new:
 * @listenable: a #HwpListenable
 * @user_data: a #gpointer
 *
 * Returns:
 *
 * Since: 0.0.1
 */
HwpHWP5Parser *hwp_hwp5_parser_new (HwpListenable *listenable,
                                    gpointer       user_data)
{
  g_return_val_if_fail (HWP_IS_LISTENABLE (listenable), NULL);

  HwpHWP5Parser *parser = g_object_new (HWP_TYPE_HWP5_PARSER, NULL);
  parser->listenable    = listenable;
  parser->user_data     = user_data;

  return parser;
}

static void hwp_hwp5_parser_parse_doc_info (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  HwpListenableInterface *iface;
  iface = HWP_LISTENABLE_GET_IFACE (parser->listenable);

  parser->stream = file->doc_info_stream;

  while (hwp_hwp5_parser_pull (parser, error))
  {
    switch (parser->tag_id) {
    case HWP_TAG_DOCUMENT_PROPERTIES:
      break;
    case HWP_TAG_ID_MAPPINGS:
      {
        guint8 id_mappings_len;
        id_mappings_len = parser->data_len / sizeof(guint32);

        guint32 *id_mappings = g_malloc0_n (id_mappings_len, sizeof(guint32));

        for (guint8 i = 0; i < id_mappings_len; i++)
          parser_read_uint32 (parser, &id_mappings[i], error);

        g_free (id_mappings);
      }
      break;
    case HWP_TAG_BIN_DATA:
      {
        HwpBinData *bin_data = hwp_bin_data_new ();
        guint16 flag, len;
        parser_read_uint16 (parser, &flag, error);

        /* type */
        switch (flag & 3)
        {
          case 0:
            {
              /* gchar *type = "link"; */
              parser_read_uint16 (parser, &len, error);
              guint8 *buf = malloc (len * 2);
              parser_read_bytes (parser, buf, len * 2, error);
              gchar *abs_path = g_convert ((const gchar *) buf,
                                           len * 2,
                                           "UTF-8", "UTF-16LE", NULL, NULL,
                                           error);
              g_free (buf);
              g_free (abs_path);

              parser_read_uint16 (parser, &len, error);
              buf = malloc (len * 2);
              parser_read_bytes (parser, buf, len * 2, error);
              gchar *rel_path = g_convert ((const gchar *) buf, len * 2,
                                           "UTF-8", "UTF-16LE", NULL, NULL,
                                           error);
              g_free (buf);
              g_free (rel_path);
            }
            break;
          case 1:
            {
              /* gchar *type = "embedding"; */
              parser_read_uint16 (parser, &bin_data->id, error);
              parser_read_uint16 (parser, &len, error);
              guint8 *buf = malloc (len * 2);
              parser_read_bytes (parser, buf, len * 2, error);
              bin_data->format = g_convert ((const gchar *) buf, len * 2,
                                            "UTF-8", "UTF-16LE", NULL, NULL,
                                            error);
              g_free (buf);
            }
            break;
          case 2:
            {
              /* gchar *type = "storage"; */
              parser_read_uint16 (parser, &bin_data->id, error);
              parser_read_uint16 (parser, &len, error);
              guint8 *buf = malloc (len * 2);
              parser_read_bytes (parser, buf, len * 2, error);
              bin_data->format = g_convert ((const gchar *) buf, len * 2,
                                            "UTF-8", "UTF-16LE", NULL, NULL,
                                            error);
              g_free (buf);
            }
            break;
          default:
            g_warning ("%s:%d: unknown type: %d", __FILE__, __LINE__, flag & 3);
            break;
        }

        /* compress_policy */
        switch (flag & 0x30)
        {
          case 0:
            /* gchar *compress_policy = "default"; */
            break;
          case 0x10:
            /* gchar *compress_policy = "force_compress"; */
            break;
          case 0x20:
            /* gchar *compress_policy = "force_plain"; */
            break;
          default:
            g_warning ("%s:%d: unknown compress_policy: %d",
              __FILE__, __LINE__, flag & 0x30);
            break;
        }

        /* status */
        switch (flag & 0xc8)
        {
          case 0:
            /* gchar *status = "not yet accessed"; */
            break;
          case 0x40:
            /* gchar *status = "access success and file found"; */
            break;
          case 0x80:
            /* gchar *status = "access fail and error"; */
            break;
          case 0xc0:
            /* gchar *status = "access fail and ignore"; */
            break;
          default:
            g_warning ("%s:%d: unknown status: %d",
              __FILE__, __LINE__, flag & 0xc8);
            break;
        }

        if (iface->bin_data)
          iface->bin_data (parser->listenable,
                           bin_data,
                           parser->user_data,
                           error);
        else
          hwp_bin_data_free (bin_data);
      }
      break;
    case HWP_TAG_FACE_NAME:
      {
        HwpFaceName *hwp_face_name = hwp_face_name_new ();

        parser_read_uint8 (parser, &hwp_face_name->prop1, error);
        parser_read_uint16 (parser, &hwp_face_name->len1, error);

        GString *gstr = g_string_new ("");
        gunichar2 unichar;

        for (guint16 i = 0; i < hwp_face_name->len1; i++)
        {
          parser_read_uint16 (parser, &unichar, error);
          g_string_append_unichar (gstr, unichar);
        }

        hwp_face_name->font_name = g_string_free (gstr, FALSE);

        if (iface->face_name)
          iface->face_name (parser->listenable,
                            hwp_face_name,
                            parser->user_data,
                            error);
        else
          hwp_face_name_free (hwp_face_name);
      }
      break;
    case HWP_TAG_BORDER_FILL:
      break;
    case HWP_TAG_CHAR_SHAPE:
      {
        HwpCharShape *char_shape = hwp_char_shape_new ();

        for (guint8 i = 0; i < 7; i++)
        {
          parser_read_uint16(parser, &char_shape->face_id[i], error);
          parser_read_uint8 (parser, &char_shape->ratio[i], error);
          parser_read_uint8 (parser, &char_shape->char_spacing[i], error);
          parser_read_uint8 (parser, &char_shape->rel_size[i], error);
          parser_read_uint8 (parser, &char_shape->char_offset[i], error);
        }

        guint32 height;
        parser_read_uint32 (parser, &height, error);
        char_shape->height_in_points = height / 7200.0 * 72;
        parser_read_uint32 (parser, &char_shape->prop, error);
        parser_read_uint8  (parser, &char_shape->space_between_shadows1, error);
        parser_read_uint8  (parser, &char_shape->space_between_shadows2, error);
        parser_read_color  (parser, &char_shape->text_color,      error);
        parser_read_color  (parser, &char_shape->underline_color, error);
        parser_read_color  (parser, &char_shape->shade_color,     error);
        parser_read_color  (parser, &char_shape->shadow_color,    error);

        if (hwp_hwp5_parser_check_version (parser, 5, 0, 2, 1))
          parser_read_uint16 (parser, &char_shape->border_fill_id, error);

        if (hwp_hwp5_parser_check_version (parser, 5, 0, 3, 0))
          parser_read_color (parser, &char_shape->strike_through_color, error);

        if (iface->char_shape)
          iface->char_shape (parser->listenable,
                             char_shape,
                             parser->user_data,
                             error);
        else
          hwp_char_shape_free (char_shape);
      }
      break;
    case HWP_TAG_TAB_DEF:
      {
        guint32 prop, pos;
        guint16 count;
        guint8  type, leader;
        parser_read_uint32 (parser, &prop, error);
        parser_read_uint16 (parser, &count, error);
        parser_skip (parser, 2); /* dummy */

        for (guint16 i = 0; i < count; i++)
        {
          parser_read_uint32 (parser, &pos, error);
          parser_read_uint8  (parser, &type, error);
          parser_read_uint8  (parser, &leader, error);
          parser_skip (parser, 2); /* dummy */
        }
      }
      break;
    case HWP_TAG_NUMBERING:
      {
        guint16 len, start_num;
        for (guint8 i = 0; i < 7; i++)
        {
          parser_skip(parser, 12);
          parser_read_uint16 (parser, &len, error);
          guint8 *buf = malloc (len * 2);
          parser_read_bytes (parser, buf, len * 2, error);

          if (buf)
          {
            gchar *numbering = g_convert ((const gchar *) buf, len * 2,
                                          "UTF-8", "UTF-16LE", NULL, NULL,
                                          error);
            g_free (buf);
            g_free (numbering);
          }
        }

        parser_read_uint16 (parser, &start_num, error);

        if (hwp_hwp5_parser_check_version (parser, 5, 0, 2, 5))
        {
          for (guint8 i = 0; i < 7; i++)
          {
            parser_skip(parser, 4);
          }
        }
      }
      break;
    case HWP_TAG_BULLET:
      break;
    case HWP_TAG_PARA_SHAPE:
      {
        HwpParaShape *para_shape = hwp_para_shape_new ();

        parser_read_uint32 (parser, &para_shape->prop1, error);
        parser_read_int32  (parser, &para_shape->left_margin, error);
        parser_read_int32  (parser, &para_shape->right_margin, error);
        parser_read_int32  (parser, &para_shape->indent_margin, error);
        parser_read_int32  (parser, &para_shape->prev_margin, error);
        parser_read_int32  (parser, &para_shape->next_margin, error);

        if (!hwp_hwp5_parser_check_version (parser, 5, 0, 2, 5))
          parser_read_int32  (parser, &para_shape->line_spacing1, error);

        parser_read_uint16 (parser, &para_shape->tabdef_id, error);
        parser_read_uint16 (parser, &para_shape->numbering_id, error);
        parser_read_uint16 (parser, &para_shape->border_fill_id, error);
        parser_read_int16  (parser, &para_shape->border_offset_left, error);
        parser_read_int16  (parser, &para_shape->border_offset_right, error);
        parser_read_int16  (parser, &para_shape->border_offset_top, error);
        parser_read_int16  (parser, &para_shape->border_offset_bottom, error);

        if (hwp_hwp5_parser_check_version (parser, 5, 0, 1, 7))
          parser_read_uint32 (parser, &para_shape->prop2, error);

        if (hwp_hwp5_parser_check_version (parser, 5, 0, 2, 5))
          parser_read_uint32 (parser, &para_shape->prop3, error);

        if (hwp_hwp5_parser_check_version (parser, 5, 0, 2, 5))
          parser_read_uint32 (parser, &para_shape->line_spacing2, error);

        if (iface->para_shape)
          iface->para_shape (parser->listenable,
                             para_shape,
                             parser->user_data,
                             error);
        else
          hwp_para_shape_free (para_shape);
      }
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
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    }
  }
}

static HwpSecd *
hwp_hwp5_parser_build_section_definition (HwpHWP5Parser *parser,
                                          HwpHWP5File   *file,
                                          GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);

  guint16 level = parser->level;
  HwpSecd *secd = hwp_secd_new ();

  while (hwp_hwp5_parser_pull (parser, error)) {
    if (parser->level <= level) {
        parser->state = HWP_PARSE_STATE_PASSING;
        break;
    }

    g_assert (parser->level == 2);

    switch (parser->tag_id) {
    case HWP_TAG_PAGE_DEF:
      {
        guint32 tmp;
        /* TODO 이 경우 gdouble = guint32 / 7200.0 *72 형변환은 어떻게 되는가? */
        parser_read_uint32 (parser, &tmp, error);
        secd->page_width_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_height_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_left_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_right_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_top_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_bottom_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_header_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_footer_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_gutter_margin_in_points = tmp / 7200.0 * 72;
        parser_read_uint32 (parser, &tmp, error);
        secd->page_prop = tmp;
      }
      break;
    case HWP_TAG_FOOTNOTE_SHAPE:
      break;
    case HWP_TAG_PAGE_BORDER_FILL:
      break;
    case HWP_TAG_LIST_HEADER:
      break;
    case HWP_TAG_PARA_HEADER:
      hwp_hwp5_parser_build_paragraph (parser, file, error);
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
  return secd;
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
      hwp_hwp5_parser_build_paragraph (parser, file, error);
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
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
      hwp_hwp5_parser_build_paragraph (parser, file, error);
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
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
      hwp_hwp5_parser_build_paragraph (parser, file, error);
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
}

static HwpTable *hwp_hwp5_parser_get_table (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);
  g_return_val_if_fail (HWP_IS_HWP5_FILE (file), NULL);

  HwpTable *table = hwp_table_new ();

  parser_read_uint32 (parser, &table->flags, error);
  parser_read_uint16 (parser, &table->n_rows, error);

  for (guint i = 0; i < table->n_rows; i++)
    g_ptr_array_add (table->rows, g_ptr_array_new_with_free_func ((GDestroyNotify) g_object_unref));

  parser_read_uint16 (parser, &table->n_cols, error);
  parser_read_uint16 (parser, &table->cell_spacing, error);
  parser_read_uint16 (parser, &table->left_margin, error);
  parser_read_uint16 (parser, &table->right_margin, error);
  parser_read_uint16 (parser, &table->top_margin, error);
  parser_read_uint16 (parser, &table->bottom_margin, error);

  table->row_sizes = g_malloc0_n (table->n_rows, 2);

  for (guint i = 0; i < table->n_rows; i++)
    parser_read_uint16 (parser, &(table->row_sizes[i]), error);

  parser_read_uint16 (parser, &table->border_fill_id, error);

  if (hwp_hwp5_file_check_version (file, 5, 0, 1, 0))
  {
    parser_read_uint16 (parser, &table->valid_zone_info_size, error);
    table->zones = g_malloc0_n (table->valid_zone_info_size, 10);

    for (guint i = 0; i < table->valid_zone_info_size; i++)
      parser_read_bytes (parser, &(table->zones[i]), 10, error);
  }

  if (parser->data_pos != parser->data_len)
  {
    g_warning ("%s:%d: TABLE data size mismatch at %s\n", __FILE__, __LINE__,
      hwp_hwp5_file_get_hwp_version_string (HWP_FILE (file)));
  }

  return table;
}

static HwpTableCell *hwp_hwp5_parser_get_table_cell (HwpHWP5Parser *parser,
                                                     GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);

  HwpTableCell *table_cell = hwp_table_cell_new ();
/*
 * list-header (cell) data_len = 46, ver: 5.0.2.2
 * list-header (cell) data_len = 47, ver: 5.0.3.4
 */
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

  if (parser->data_len == 30) /* FIXME */
    goto FINALLY;

  parser_read_uint16 (parser, &table_cell->bottom_margin, error);
  parser_read_uint16 (parser, &table_cell->border_fill_id, error);
  /* unknown */
  parser_read_uint32 (parser, &table_cell->unknown2, error);

  FINALLY:
#ifdef HWP_ENABLE_DEBUG
  printf ("%d %d %d\n%d %d %d %d\n%d %d\n%d %d %d %d\n%d\n",
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

  if (parser->data_pos != parser->data_len) {
    g_warning ("%s:%d: table cell size mismatch, ver %d.%d.%d.%d\n", __FILE__, __LINE__,
      parser->major_version,
      parser->minor_version,
      parser->micro_version,
      parser->extra_version);
  }

  return table_cell;
}

static HwpTable *hwp_hwp5_parser_build_table (HwpHWP5Parser *parser,
                                              HwpHWP5File   *file,
                                              GError       **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_PARSER (parser), NULL);
  g_return_val_if_fail (HWP_IS_HWP5_FILE (file), NULL);

  guint16 level = parser->level;

  HwpTable     *table     = NULL;
  HwpTableCell *cell      = NULL;
  HwpParagraph *paragraph = NULL;

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
#ifdef HWP_ENABLE_DEBUG
      printf ("list-header (cell) data_len = %d, ver: %s\n",
        parser->data_len, hwp_hwp5_file_get_hwp_version_string (HWP_FILE (file)));
#endif
      cell = hwp_hwp5_parser_get_table_cell (parser, error);
      hwp_table_add_cell (table, cell, cell->row_addr);
      break;
    case HWP_TAG_PARA_HEADER:
      paragraph = hwp_hwp5_parser_build_paragraph (parser, file, error);
      if (paragraph)
        hwp_table_cell_add_paragraph (cell, paragraph);

      paragraph = NULL;
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */

  return table;
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
      hwp_hwp5_parser_build_paragraph (parser, file, error);
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
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
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
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_bokm (HwpHWP5Parser *parser,
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
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_clickhere (HwpHWP5Parser *parser,
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
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
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
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_form (HwpHWP5Parser *parser,
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
    case HWP_TAG_FORM_OBJECT:
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_footer (HwpHWP5Parser *parser,
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
      hwp_hwp5_parser_build_paragraph (parser, file, error);
      break;
    default:
      WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
      break;
    } /* switch */
  } /* while */
}

static void hwp_hwp5_parser_parse_ctrl_header (HwpHWP5Parser *parser,
                                               HwpHWP5File   *file,
                                               HwpParagraph  *paragraph,
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
    {
      HwpSecd *secd = NULL;
      secd = hwp_hwp5_parser_build_section_definition (parser, file, error);
      if (secd)
        hwp_paragraph_set_secd (paragraph, secd);
    }
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
    {
      HwpCommonProperty *prop = hwp_common_property_new ();

      prop->ctrl_id = parser->ctrl_id;
      parser_read_uint32 (parser, &prop->prop, error);
      parser_read_uint32 (parser, &prop->y_offset, error);
      parser_read_uint32 (parser, &prop->x_offset, error);
      parser_read_uint32 (parser, &prop->width, error);
      parser_read_uint32 (parser, &prop->height, error);
      parser_read_uint32 (parser, &prop->z_order, error);
      parser_read_uint16 (parser, &prop->margin1, error);
      parser_read_uint16 (parser, &prop->margin2, error);
      parser_read_uint16 (parser, &prop->margin3, error);
      parser_read_uint16 (parser, &prop->margin4, error);
      parser_read_uint32 (parser, &prop->instance_id, error);
      parser_read_uint32 (parser, &prop->len, error);

      if (parser->data_pos < parser->data_len)
        g_warning ("%s:%d: remained %d bytes\n",
          __FILE__, __LINE__, parser->data_len - parser->data_pos);

      hwp_common_property_free (prop);

      HwpTable *table = NULL;
      table = hwp_hwp5_parser_build_table (parser, file, error);

      if (table)
        hwp_paragraph_set_table (paragraph, table);
    }
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
  case CTRL_ID_FORM:
    hwp_hwp5_parser_parse_form (parser, file, error);
    break;
  case CTRL_ID_FOOTER:
    hwp_hwp5_parser_parse_footer (parser, file, error);
    break;
  case CTRL_ID_BOKM: /* 책갈피 */
    hwp_hwp5_parser_parse_bokm (parser, file, error);
    break;
  case FIELD_BOOKMARK:
    hwp_hwp5_parser_parse_bookmark (parser, file, error);
    break;
  case FIELD_CLICKHERE:
    hwp_hwp5_parser_parse_clickhere (parser, file, error);
    break;
  case FIELD_HYPERLINK:
    break;
  default:
    WARNING_CTRL_NOT_IMPLEMENTED (parser->ctrl_id);
    break;
  } /* switch (parser->ctrl_id) */
}

static HwpParagraph *hwp_hwp5_parser_build_paragraph (HwpHWP5Parser *parser,
                                                      HwpHWP5File   *file,
                                                      GError       **error)
{
  guint16 level = parser->level;

  HwpParagraph *paragraph = hwp_paragraph_new ();
  gchar        *raw_text  = NULL;

  parser_read_uint32 (parser, &paragraph->n_chars, error);
  if (paragraph->n_chars & 0x80000000)
    paragraph->n_chars &= 0x7fffffff;

  parser_read_uint32 (parser, &paragraph->control_mask, error);
  parser_read_uint16 (parser, &paragraph->para_shape_id, error);
  parser_read_uint8  (parser, &paragraph->para_style_id, error);
  parser_read_uint8  (parser, &paragraph->column_type, error);
  parser_read_uint16 (parser, &paragraph->n_char_shapes, error);
  parser_read_uint16 (parser, &paragraph->n_range_tags, error);
  parser_read_uint16 (parser, &paragraph->n_aligns, error);
  parser_read_uint32 (parser, &paragraph->para_instance_id, error);

  if (hwp_hwp5_parser_check_version (parser, 5, 0, 3, 2))
    parser_read_uint16 (parser, &paragraph->track, error);

  while (hwp_hwp5_parser_pull (parser, error))
  {
    if (parser->level <= level)
    {
      parser->state = HWP_PARSE_STATE_PASSING;
      break;
    }

    g_assert (parser->level == level + 1);

    switch (parser->tag_id)
    {
      case HWP_TAG_PARA_TEXT:
        if (raw_text)
          g_free (raw_text);

        raw_text = g_malloc (parser->data_len);
        parser_read_bytes (parser, raw_text, parser->data_len, error);
        parser->data_pos += parser->data_len;
#ifdef HWP_ENABLE_DEBUG
        printf ("paragraph->n_chars:%d\n", paragraph->n_chars);
        printf ("parser->data_len:%d\n", parser->data_len);
#endif
        break;
      case HWP_TAG_PARA_CHAR_SHAPE:
        {
          paragraph->m_len = parser->data_len / 8;
          paragraph->m_pos = g_malloc (4 * paragraph->m_len);
          paragraph->m_id  = g_malloc (4 * paragraph->m_len);
          paragraph->text_attrs = g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_text_attributes_free);

          for (guint i = 0; i < paragraph->m_len; i++)
          {
            parser_read_uint32 (parser, &paragraph->m_pos[i], error);
            parser_read_uint32 (parser, &paragraph->m_id[i], error);

#ifdef HWP_ENABLE_DEBUG
            printf ("m_pos[%d]:%d\n", i, paragraph->m_pos[i]);
#endif
          }

          gint prev_index = 0;
          GString *string = g_string_new ("");

          for (guint j = 0; j < paragraph->m_len; j++)
          {
            GString *substring = g_string_new ("");
            guint32 pos1, pos2;
            pos1 = paragraph->m_pos[j];

            if (j + 1 == paragraph->m_len)
              pos2 = paragraph->n_chars;
            else
              pos2 = paragraph->m_pos[j+1];

            for (guint i = pos1 * 2; i < pos2 * 2; i = i + 2)
            {
              if (!raw_text)
                break;

              gunichar2 c = GSF_LE_GET_GUINT16(raw_text + i);
              switch (c)
              {
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
                  break;
                case 9: /* inline */ /* tab */
                  i = i + 14;
                  g_string_append_unichar (substring, c);
                  break;
                case 10:
                  break;
                case 11:
                case 12:
                  i = i + 14;
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
                  /* HyPUA code */
                  if (c >= 0xe0bc && c <= 0xf8f7)
                  {
                    const gunichar2 *unichar2;
                    unichar2 = hyc2uni_page14[c-0xe0bc];
                    for (int j = 0; j < 3; j++)
                    {
                      if (unichar2[j] == 0)
                        break;

                      g_string_append_unichar (substring, unichar2[j]);
                    }
                  }
                  else
                  {
                    g_string_append_unichar (substring, c);
                  }
                  break;
              } /* switch */
            } /* for (guint i = pos1 * 2; i < pos2 * 2; i = i + 2) */

            HwpTextAttributes *text_attrs = hwp_text_attributes_new ();
            text_attrs->start_index = prev_index;
            /* The character at end_index is not included */
            text_attrs->end_index = text_attrs->start_index + strlen (substring->str);
            g_ptr_array_add (paragraph->text_attrs, text_attrs);
#ifdef HWP_ENABLE_DEBUG
            printf ("start:%d ~ end:%d:text:%s\n",
                    text_attrs->start_index,
                    text_attrs->end_index,
                    substring->str);
#endif
            prev_index = text_attrs->end_index;
            g_string_append (string, g_string_free (substring, FALSE));
          } /* for (guint j = 0; j < paragraph->m_len; j++) */
          paragraph->text = g_string_free (string, FALSE);
        }
        break;
      case HWP_TAG_PARA_LINE_SEG:
        break;
      case HWP_TAG_CTRL_HEADER:
        hwp_hwp5_parser_parse_ctrl_header (parser, file, paragraph, error);
        break;
      case HWP_TAG_MEMO_LIST:
        break;
      /* TODO: HWP_TAG_LIST_HEADER
         계층화를 위해 HWP_TAG_MEMO_LIST 를 파싱하는 부분에서 처리할 필요가 있습니다. */
      case HWP_TAG_LIST_HEADER: /* memo list 가 사용합니다 */
        break;
      /* TODO: HWP_TAG_PARA_HEADER
         계층화를 위해 HWP_TAG_MEMO_LIST 를 파싱하는 부분에서 처리할 필요가 있습니다. */
      case HWP_TAG_PARA_HEADER: /* memo list 가 사용합니다. */
        hwp_hwp5_parser_build_paragraph (parser, file, error);
        break;
      default:
        WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
        break;
    } /* switch */
  } /* while */

  g_free (raw_text);
  raw_text = NULL;
  return paragraph;
}

static void hwp_hwp5_parser_parse_section (HwpHWP5Parser *parser,
                                           HwpHWP5File   *file,
                                           GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  HwpListenableInterface *iface;
  iface = HWP_LISTENABLE_GET_IFACE (parser->listenable);
  HwpParagraph *paragraph = NULL;

  while (hwp_hwp5_parser_pull (parser, error))
  {
    switch (parser->tag_id)
    {
      case HWP_TAG_PARA_HEADER:
        paragraph = hwp_hwp5_parser_build_paragraph (parser, file, error);
        break;
      default:
        WARNING_TAG_NOT_IMPLEMENTED (parser->tag_id);
        break;
    }  /* switch */

    if (paragraph)
    {
      /* call callback function */
      if (iface->paragraph)
        iface->paragraph (parser->listenable,
                          paragraph,
                          parser->user_data,
                          error);
      else
        g_object_unref (paragraph);

      paragraph = NULL;
    }
  } /* while */
}

static void hwp_hwp5_parser_parse_sections (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  for (guint i = 0; i < file->section_streams->len; i++)
  {
    parser->stream = g_ptr_array_index (file->section_streams, i);
    hwp_hwp5_parser_parse_section (parser, file, error);
    if (*error)
      break;
  }
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

  HwpListenableInterface *iface;
  iface = HWP_LISTENABLE_GET_IFACE (parser->listenable);
  if (!iface->summary_info)
    return;

  gssize          size;
  guint8         *buf = NULL;
  const guint8   *ret = NULL;
  GsfInput       *summary;
  GsfDocMetaData *meta;

  size = gsf_input_size (file->summary_info_stream);
  buf = g_malloc (size);
  ret = gsf_input_read (file->summary_info_stream, size, buf);

  if (ret == NULL)
  {
    g_warning ("%s:%d\n", __FILE__, __LINE__);
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
  iface->summary_info (parser->listenable,
                       info,
                       parser->user_data,
                       error);
  g_free (buf);
  g_object_unref (meta);
  g_object_unref (summary);
}

static void hwp_hwp5_parser_parse_prv_text (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error)
{
  gsf_off_t    size = gsf_input_size (file->prv_text_stream);
  const guint8 *buf = gsf_input_read (file->prv_text_stream, size, NULL);

  if (buf == NULL)
    goto FAIL;

  /* g_convert() can be used to convert a byte buffer of UTF-16 data of
     ambiguous endianess. */
  gchar *prv_text = g_convert ((const gchar *) buf, (gssize) size,
                               "UTF-8", "UTF-16LE", NULL, NULL, error);

  if (*error)
    goto FAIL;

  HwpListenableInterface *iface;
  iface = HWP_LISTENABLE_GET_IFACE (parser->listenable);

  if (iface->prv_text)
    iface->prv_text (HWP_LISTENABLE (parser->listenable),
                     prv_text,
                     parser->user_data,
                     error);
  else
    g_free (prv_text);

  return;

  FAIL:
  if (*error)
    g_warning("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
  else
    g_warning("%s:%d\n", __FILE__, __LINE__);
}

/**
 * hwp_hwp5_parser_check_version:
 * @parser: a #HwpHWP5Parser
 * @major: the major version to check for
 * @minor: the minor version to check for
 * @micro: the micro version to check for
 * @extra: the extra version to check for
 *
 * Checks the version of the HWP document file
 *
 * <example>
 * <title>Checking the version of the HWP document file</title>
 * <programlisting>
 *   if (hwp_hwp5_parser_check_version (parser, 5, 0, 0, 7))
 *     g_print ("HWP document file version is 5.0.0.7 or above");
 * </programlisting>
 * </example>
 *
 * Returns: %TRUE if the version of the hwp document file is the same as
 * or newer than the passed-in version
 *
 * Since: 0.0.1
 */
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

static void hwp_hwp5_parser_parse_file_header (HwpHWP5Parser *parser,
                                               HwpHWP5File   *file,
                                               GError       **error)
{
  g_return_if_fail (HWP_IS_HWP5_PARSER (parser) && HWP_IS_HWP5_FILE (file));

  HwpListenableInterface *iface;
  iface = HWP_LISTENABLE_GET_IFACE (parser->listenable);

  if (iface->document_version)
    iface->document_version (parser->listenable,
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

  if (*error) {
    g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
    return;
  }

  hwp_hwp5_parser_parse_doc_info       (parser, file, error);

  if (*error) {
    g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
    return;
  }

  hwp_hwp5_parser_parse_sections       (parser, file, error);

  if (*error) {
    g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
    return;
  }

  hwp_hwp5_parser_parse_summary_info   (parser, file, error);
/*  _hwp_hwp5_parser_parse_bin_data       (parser, file, error); */

  if (*error) {
    g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
    return;
  }

  hwp_hwp5_parser_parse_prv_text       (parser, file, error);

  if (*error) {
    g_warning ("%s:%d:%s\n", __FILE__, __LINE__, (*error)->message);
    return;
  }
/*  _hwp_hwp5_parser_parse_prv_image      (parser, file, error); */
/*  _hwp_hwp5_parser_parse_doc_options    (parser, file, error); */
/*  _hwp_hwp5_parser_parse_scripts        (parser, file, error); */
/*  _hwp_hwp5_parser_parse_xml_template   (parser, file, error); */
/*  _hwp_hwp5_parser_parse_doc_history    (parser, file, error); */
}

static void hwp_hwp5_parser_init (HwpHWP5Parser *parser)
{
  parser->state = HWP_PARSE_STATE_NORMAL;
}

static void hwp_hwp5_parser_finalize (GObject *object)
{
  G_OBJECT_CLASS (hwp_hwp5_parser_parent_class)->finalize (object);
}

static void hwp_hwp5_parser_class_init (HwpHWP5ParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = hwp_hwp5_parser_finalize;
}

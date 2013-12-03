/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-parser.c
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

#include "hwp-hwp3-parser.h"

G_DEFINE_TYPE (HWPHWP3Parser, hwp_hwp3_parser, G_TYPE_OBJECT);

HWPHWP3Parser *hwp_hwp3_parser_new (GInputStream *stream)
{
    g_return_val_if_fail (stream != NULL, NULL);
    HWPHWP3Parser *parser = g_object_new (HWP_TYPE_HWP3_PARSER, NULL);
    parser->stream = g_object_ref (stream);
    return parser;
}

gboolean hwp_hwp3_parser_read_uint8 (HWPHWP3Parser *parser, guint8 *i)
{
    g_return_val_if_fail (parser != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (parser->stream, i, 1,
                                          &parser->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (parser->bytes_read != 1) ||
        (parser->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (parser->stream, NULL, NULL);
        return FALSE;
    }

    return TRUE;
}

gboolean hwp_hwp3_parser_read_uint16 (HWPHWP3Parser *parser, guint16 *i)
{
    g_return_val_if_fail (parser != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (parser->stream, i, 2,
                                          &parser->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (parser->bytes_read != 2) ||
        (parser->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (parser->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT16_FROM_LE(*i);

    return TRUE;
}

gboolean hwp_hwp3_parser_read_uint32 (HWPHWP3Parser *parser, guint32 *i)
{
    g_return_val_if_fail (parser != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (parser->stream, i, 4,
                                          &parser->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (parser->bytes_read != 4) ||
        (parser->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (parser->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT32_FROM_LE(*i);
    
    return TRUE;
}

gboolean hwp_hwp3_parser_read (HWPHWP3Parser *parser, void *buffer, gsize count)
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

gboolean hwp_hwp3_parser_skip (HWPHWP3Parser *parser, guint16 count)
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

static void hwp_hwp3_parser_init (HWPHWP3Parser *parser)
{
  parser->priv = G_TYPE_INSTANCE_GET_PRIVATE (parser,
                                              HWP_TYPE_HWP3_PARSER,
                                              HWPHWP3ParserPrivate);
}

static void
hwp_hwp3_parser_finalize (GObject *object)
{
  HWPHWP3Parser *parser = HWP_HWP3_PARSER (object);
  g_object_unref (parser->stream);

  G_OBJECT_CLASS (hwp_hwp3_parser_parent_class)->finalize (object);
}

static void hwp_hwp3_parser_class_init (HWPHWP3ParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  g_type_class_add_private (klass, sizeof (HWPHWP3ParserPrivate));
  object_class->finalize = hwp_hwp3_parser_finalize;
}

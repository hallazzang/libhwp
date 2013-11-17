/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-context-v3.c
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

#include "hwp-context-v3.h"

G_DEFINE_TYPE (GHWPContextV3, hwp_context_v3, G_TYPE_OBJECT);

GHWPContextV3 *hwp_context_v3_new (GInputStream *stream)
{
    g_return_val_if_fail (stream != NULL, NULL);
    GHWPContextV3 *context = g_object_new (GHWP_TYPE_CONTEXT_V3, NULL);
    context->stream = g_object_ref (stream);
    return context;
}

gboolean hwp_context_v3_read_uint8 (GHWPContextV3 *context, guint8 *i)
{
    g_return_val_if_fail (context != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (context->stream, i, 1,
                                          &context->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (context->bytes_read != 1) ||
        (context->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }

    return TRUE;
}

gboolean hwp_context_v3_read_uint16 (GHWPContextV3 *context, guint16 *i)
{
    g_return_val_if_fail (context != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (context->stream, i, 2,
                                          &context->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (context->bytes_read != 2) ||
        (context->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT16_FROM_LE(*i);

    return TRUE;
}

gboolean hwp_context_v3_read_uint32 (GHWPContextV3 *context, guint32 *i)
{
    g_return_val_if_fail (context != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (context->stream, i, 4,
                                          &context->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) ||
        (context->bytes_read != 4) ||
        (context->bytes_read == 0))
    {
        *i = 0;
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }
    *i = GUINT32_FROM_LE(*i);
    
    return TRUE;
}

gboolean hwp_context_v3_read (GHWPContextV3 *context, void *buffer, gsize count)
{
    g_return_val_if_fail (context != NULL, FALSE);

    gboolean is_success = FALSE;
    is_success = g_input_stream_read_all (context->stream, buffer, count,
                                          &context->bytes_read,
                                          NULL, NULL);
    if ((is_success == FALSE) || (context->bytes_read == 0))
    {
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }

    return TRUE;
}

gboolean hwp_context_v3_skip (GHWPContextV3 *context, guint16 count)
{
    g_return_val_if_fail (context != NULL, FALSE);

    gboolean is_success = FALSE;
    guint8  *buf        = g_malloc (count);

    is_success = g_input_stream_read_all (context->stream, buf, (gsize) count,
                                          &context->bytes_read,
                                          NULL, NULL);
    g_free (buf);

    if ((is_success == FALSE) || (context->bytes_read != (gsize) count))
    {
        g_warning ("%s:%d:skip size mismatch\n", __FILE__, __LINE__);
        g_input_stream_close (context->stream, NULL, NULL);
        return FALSE;
    }

    return TRUE;
}

static void hwp_context_v3_init (GHWPContextV3 *hwp_context_v3)
{

}

static void hwp_context_v3_finalize (GObject *object)
{
    GHWPContextV3 *context = GHWP_CONTEXT_V3(object);
    g_object_unref (context->stream);
	G_OBJECT_CLASS (hwp_context_v3_parent_class)->finalize (object);
}

static void hwp_context_v3_class_init (GHWPContextV3Class *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = hwp_context_v3_finalize;
}

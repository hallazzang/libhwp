/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * gsf-input-stream.c
 *
 * Copyright (C) 2012-2013 Hodong Kim <hodong@cogno.org>
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

#include <glib.h>
#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <gsf/gsf-infile-msole.h>

#include "gsf-input-stream.h"

G_DEFINE_TYPE (GsfInputStream, gsf_input_stream, G_TYPE_INPUT_STREAM);

GsfInputStream *gsf_input_stream_new (GsfInput *input)
{
  g_return_val_if_fail (GSF_IS_INPUT (input), NULL);
  GsfInputStream *gis = g_object_new (GSF_TYPE_INPUT_STREAM, NULL);
  gis->priv->input = g_object_ref (input);
  return gis;
}

gssize gsf_input_stream_read (GInputStream *base,
                              void         *buffer,
                              gsize         buffer_len,
                              GCancellable *cancellable,
                              GError      **error)
{
  GsfInputStream *gis = GSF_INPUT_STREAM (base);
  gint64    remaining = gsf_input_remaining (gis->priv->input);
  gsf_input_read (gis->priv->input, MIN (remaining, buffer_len), buffer);

  return (gssize) (remaining - gsf_input_remaining (gis->priv->input));
}

gboolean gsf_input_stream_close (GInputStream *base,
                                 GCancellable *cancellable,
                                 GError      **error)
{
    /* FIXME: currently do nothing */
    return TRUE;
}

gssize gsf_input_stream_size (GsfInputStream *stream)
{
    g_return_val_if_fail (GSF_IS_INPUT_STREAM (stream), 0L);
    return (gssize) gsf_input_size (stream->priv->input);
}

static void gsf_input_stream_finalize (GObject *obj)
{
    GsfInputStream *gis = GSF_INPUT_STREAM (obj);
    g_object_unref (gis->priv->input);
    G_OBJECT_CLASS (gsf_input_stream_parent_class)->finalize (obj);
}

static void gsf_input_stream_class_init (GsfInputStreamClass *klass)
{
    GObjectClass      *object_class = G_OBJECT_CLASS (klass);
    GInputStreamClass *parent_class = G_INPUT_STREAM_CLASS (klass);
    g_type_class_add_private (klass, sizeof (GsfInputStreamPrivate));
    parent_class->read_fn  = gsf_input_stream_read;
    parent_class->close_fn = gsf_input_stream_close;
    object_class->finalize = gsf_input_stream_finalize;
}

static void gsf_input_stream_init (GsfInputStream *stream)
{
    stream->priv = G_TYPE_INSTANCE_GET_PRIVATE (stream,
                                                GSF_TYPE_INPUT_STREAM,
                                                GsfInputStreamPrivate);
}

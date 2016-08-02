/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * gsf-input-stream.h
 * This file is part of the libhwp project.
 *
 * Copyright (C) 2012-2016 Hodong Kim <cogniti@gmail.com>
 *
 * The libhwp is dual licensed under the LGPL v3+ or Apache License 2.0
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
 *
 * Or,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __GSF_INPUT_STREAM_H__
#define __GSF_INPUT_STREAM_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GSF_TYPE_INPUT_STREAM             (gsf_input_stream_get_type ())
#define GSF_INPUT_STREAM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSF_TYPE_INPUT_STREAM, GsfInputStream))
#define GSF_INPUT_STREAM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GSF_TYPE_INPUT_STREAM, GsfInputStreamClass))
#define GSF_IS_INPUT_STREAM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSF_TYPE_INPUT_STREAM))
#define GSF_IS_INPUT_STREAM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSF_TYPE_INPUT_STREAM))
#define GSF_INPUT_STREAM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSF_TYPE_INPUT_STREAM, GsfInputStreamClass))

typedef struct _GsfInputStream        GsfInputStream;
typedef struct _GsfInputStreamClass   GsfInputStreamClass;
typedef struct _GsfInputStreamPrivate GsfInputStreamPrivate;

struct _GsfInputStream
{
  GInputStream           parent_instance;
  GsfInputStreamPrivate *priv;
};

struct _GsfInputStreamClass
{
  GInputStreamClass parent_class;
};

struct _GsfInputStreamPrivate
{
  GsfInput* input;
};

GType           gsf_input_stream_get_type (void) G_GNUC_CONST;
GsfInputStream *gsf_input_stream_new      (GsfInput       *input);
gssize          gsf_input_stream_read     (GInputStream   *base,
                                           void           *buffer,
                                           gsize           buffer_len,
                                           GCancellable   *cancellable,
                                           GError        **error);
gboolean        gsf_input_stream_close    (GInputStream   *base,
                                           GCancellable   *cancellable,
                                           GError        **error);
gssize          gsf_input_stream_size     (GsfInputStream *stream);

G_END_DECLS

#endif /* __GSF_INPUT_STREAM_H__ */

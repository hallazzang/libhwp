/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * ghwp-parse.h
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

#ifndef __GHWP_PARSE_CONTEXT_H__
#define __GHWP_PARSE_CONTEXT_H__

#include <glib-object.h>
#include <gio/gio.h>

#include "ghwp-file-v5.h"
#include "ghwp-listener.h"

G_BEGIN_DECLS

#define GHWP_TYPE_PARSE_CONTEXT             (ghwp_parse_context_get_type ())
#define GHWP_PARSE_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PARSE_CONTEXT, GHWPParseContext))
#define GHWP_PARSE_CONTEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PARSE_CONTEXT, GHWPParseContextClass))
#define GHWP_IS_PARSE_CONTEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PARSE_CONTEXT))
#define GHWP_IS_PARSE_CONTEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PARSE_CONTEXT))
#define GHWP_PARSE_CONTEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PARSE_CONTEXT, GHWPParseContextClass))

typedef struct _GHWPParseContext        GHWPParseContext;
typedef struct _GHWPParseContextClass   GHWPParseContextClass;
typedef struct _GHWPParseContextPrivate GHWPParseContextPrivate;

struct _GHWPParseContextClass
{
  GObjectClass parent_class;
};

struct _GHWPParseContext
{
  GObject                  parent_instance;
  GHWPParseContextPrivate *priv;

  GHWPListener              *listener;
  gpointer                 user_data;
  GInputStream            *stream;
  /* from record header */
  guint16                  tag_id;
  guint16                  level;
  guint16                  data_len;
  /* for sanity checking */
  guint16                  data_count;
  /* for parsing */
  guint8                   state;
  guint32                  ctrl_id;
  /* for version check */
  guint8                   major_version;
  guint8                   minor_version;
  guint8                   micro_version;
  guint8                   extra_version;
};

struct _GHWPParseContextPrivate
{
  guint32  header;
  gsize    bytes_read;
  gboolean ret;
};

GType             ghwp_parse_context_get_type (void) G_GNUC_CONST;
GHWPParseContext *ghwp_parse_context_new      (GHWPListener       *listener,
                                               gpointer          user_data);
void              ghwp_parse_context_parse    (GHWPParseContext *context,
                                               GHWPFileV5       *file,
                                               GError          **error);
gboolean     ghwp_parse_context_pull     (GHWPParseContext  *context, GError **error);
gboolean     context_read_uint16   (GHWPParseContext  *context,
                                    guint16      *i);
gboolean     context_read_uint32   (GHWPParseContext  *context,
                                    guint32      *i);
gboolean     context_skip          (GHWPParseContext  *context,
                                    guint16       count);

G_END_DECLS

#endif /* __GHWP_PARSE_CONTEXT_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-parser.h
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

#ifndef __GHWP_PARSER_H__
#define __GHWP_PARSER_H__

#include <glib-object.h>
#include <gio/gio.h>

#include "hwp-file-v5.h"
#include "hwp-listener.h"

G_BEGIN_DECLS

#define GHWP_TYPE_PARSER             (hwp_parser_get_type ())
#define GHWP_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PARSER, GHWPParser))
#define GHWP_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PARSER, GHWPParserClass))
#define GHWP_IS_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PARSER))
#define GHWP_IS_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PARSER))
#define GHWP_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PARSER, GHWPParserClass))

typedef struct _GHWPParser        GHWPParser;
typedef struct _GHWPParserClass   GHWPParserClass;
typedef struct _GHWPParserPrivate GHWPParserPrivate;

struct _GHWPParserClass
{
  GObjectClass parent_class;
};

struct _GHWPParser
{
  GObject                  parent_instance;
  GHWPParserPrivate *priv;

  GHWPListener            *listener;
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

struct _GHWPParserPrivate
{
  guint32  header;
  gsize    bytes_read;
  gboolean ret;
};

GType             hwp_parser_get_type (void) G_GNUC_CONST;
GHWPParser *hwp_parser_new      (GHWPListener       *listener,
                                               gpointer          user_data);
void              hwp_parser_parse    (GHWPParser *parser,
                                               GHWPFileV5       *file,
                                               GError          **error);
gboolean     hwp_parser_pull     (GHWPParser  *parser, GError **error);
gboolean     parser_read_uint16   (GHWPParser  *parser,
                                    guint16      *i);
gboolean     parser_read_uint32   (GHWPParser  *parser,
                                    guint32      *i);
gboolean     parser_skip          (GHWPParser  *parser,
                                    guint16       count);

G_END_DECLS

#endif /* __GHWP_PARSER_H__ */

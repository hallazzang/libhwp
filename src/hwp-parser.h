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

#ifndef __HWP_PARSER_H__
#define __HWP_PARSER_H__

#include <glib-object.h>
#include <gio/gio.h>

#include "hwp-file-v5.h"
#include "hwp-listener.h"

G_BEGIN_DECLS

#define HWP_TYPE_PARSER             (hwp_parser_get_type ())
#define HWP_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_PARSER, HWPParser))
#define HWP_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_PARSER, HWPParserClass))
#define HWP_IS_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_PARSER))
#define HWP_IS_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_PARSER))
#define HWP_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_PARSER, HWPParserClass))

typedef struct _HWPParser        HWPParser;
typedef struct _HWPParserClass   HWPParserClass;
typedef struct _HWPParserPrivate HWPParserPrivate;

struct _HWPParserClass
{
  GObjectClass parent_class;
};

struct _HWPParser
{
  GObject                  parent_instance;
  HWPParserPrivate *priv;

  HWPListener            *listener;
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

struct _HWPParserPrivate
{
  guint32  header;
  gsize    bytes_read;
  gboolean ret;
};

GType             hwp_parser_get_type (void) G_GNUC_CONST;
HWPParser *hwp_parser_new      (HWPListener       *listener,
                                               gpointer          user_data);
void              hwp_parser_parse    (HWPParser *parser,
                                               HWPFileV5       *file,
                                               GError          **error);
gboolean     hwp_parser_pull     (HWPParser  *parser, GError **error);
gboolean     parser_read_uint16   (HWPParser  *parser,
                                    guint16      *i);
gboolean     parser_read_uint32   (HWPParser  *parser,
                                    guint32      *i);
gboolean     parser_skip          (HWPParser  *parser,
                                    guint16       count);

G_END_DECLS

#endif /* __HWP_PARSER_H__ */

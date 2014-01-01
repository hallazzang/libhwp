/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp5-parser.h
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
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

#ifndef __HWP_HWP5_PARSER_H__
#define __HWP_HWP5_PARSER_H__

#include <glib-object.h>
#include <gio/gio.h>

#include "hwp-hwp5-file.h"
#include "hwp-listener.h"

G_BEGIN_DECLS

#define HWP_TYPE_HWP5_PARSER             (hwp_hwp5_parser_get_type ())
#define HWP_HWP5_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_HWP5_PARSER, HwpHWP5Parser))
#define HWP_HWP5_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_HWP5_PARSER, HwpHWP5ParserClass))
#define HWP_IS_HWP5_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_HWP5_PARSER))
#define HWP_IS_HWP5_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_HWP5_PARSER))
#define HWP_HWP5_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_HWP5_PARSER, HwpHWP5ParserClass))

typedef struct _HwpHWP5Parser        HwpHWP5Parser;
typedef struct _HwpHWP5ParserClass   HwpHWP5ParserClass;
typedef struct _HwpHWP5ParserPrivate HwpHWP5ParserPrivate;

struct _HwpHWP5ParserClass
{
  GObjectClass parent_class;
};

struct _HwpHWP5Parser
{
  GObject                  parent_instance;
	HwpHWP5ParserPrivate    *priv;

  HwpListener             *listener;
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

struct _HwpHWP5ParserPrivate
{
  guint32  header;
  gsize    bytes_read;
  gboolean ret;
};

GType          hwp_hwp5_parser_get_type    (void) G_GNUC_CONST;
HwpHWP5Parser *hwp_hwp5_parser_new         (HwpListener   *listener,
                                            gpointer       user_data);
void           hwp_hwp5_parser_parse       (HwpHWP5Parser *parser,
                                            HwpHWP5File   *file,
                                            GError       **error);
gboolean       hwp_hwp5_parser_pull        (HwpHWP5Parser *parser,
                                            GError       **error);
gboolean       hwp_hwp5_parser_read_uint16 (HwpHWP5Parser *parser,
                                            guint16       *i);
gboolean       hwp_hwp5_parser_read_uint32 (HwpHWP5Parser *parser,
                                            guint32       *i);
gboolean       hwp_hwp5_parser_skip        (HwpHWP5Parser *parser,
                                            guint16        count);

G_END_DECLS

#endif /* __HWP_HWP5_PARSER_H__ */

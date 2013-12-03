/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-parser.h
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

#ifndef __HWP_HWP3_PARSER_H__
#define __HWP_HWP3_PARSER_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HWP_TYPE_HWP3_PARSER             (hwp_hwp3_parser_get_type ())
#define HWP_HWP3_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_HWP3_PARSER, HWPHWP3Parser))
#define HWP_HWP3_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_HWP3_PARSER, HWPHWP3ParserClass))
#define HWP_IS_HWP3_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_HWP3_PARSER))
#define HWP_IS_HWP3_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_HWP3_PARSER))
#define HWP_HWP3_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_HWP3_PARSER, HWPHWP3ParserClass))

typedef struct _HWPHWP3Parser        HWPHWP3Parser;
typedef struct _HWPHWP3ParserClass   HWPHWP3ParserClass;
typedef struct _HWPHWP3ParserPrivate HWPHWP3ParserPrivate;

struct _HWPHWP3ParserClass
{
  GObjectClass parent_class;
};

struct _HWPHWP3Parser
{
  GObject               parent_instance;
  HWPHWP3ParserPrivate *priv;

  GInputStream         *stream;
  gsize                 bytes_read;
};

struct _HWPHWP3ParserPrivate
{
};

GType          hwp_hwp3_parser_get_type    (void) G_GNUC_CONST;
HWPHWP3Parser *hwp_hwp3_parser_new         (GInputStream  *stream);
gboolean       hwp_hwp3_parser_read_uint8  (HWPHWP3Parser *parser,
                                            guint8        *i);
gboolean       hwp_hwp3_parser_read_uint16 (HWPHWP3Parser *parser,
                                            guint16       *i);
gboolean       hwp_hwp3_parser_read_uint32 (HWPHWP3Parser *parser,
                                            guint32       *i);
gboolean       hwp_hwp3_parser_read        (HWPHWP3Parser *parser,
                                            void          *buffer,
                                            gsize          count);
gboolean       hwp_hwp3_parser_skip        (HWPHWP3Parser *parser,
                                            guint16        count);
G_END_DECLS

#endif /* __HWP_HWP3_PARSER_H__ */

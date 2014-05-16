/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * hwp-parser.h
 * Copyright (C) 2014 Hodong Kim <hodong@cogno.org>
 *
 * hwp-parser.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * hwp-parser.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_PARSER_H__
#define __HWP_PARSER_H__

#include <glib-object.h>
#include "hwp-listener.h"
#include "hwp-file.h"

G_BEGIN_DECLS

#define HWP_TYPE_PARSER             (hwp_parser_get_type ())
#define HWP_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_PARSER, HwpParser))
#define HWP_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_PARSER, HwpParserClass))
#define HWP_IS_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_PARSER))
#define HWP_IS_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_PARSER))
#define HWP_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_PARSER, HwpParserClass))

typedef struct _HwpParser      HwpParser;
typedef struct _HwpParserClass HwpParserClass;

struct _HwpParser
{
  GObject parent_instance;

  HwpListener *listener;
  gpointer     user_data;
};

struct _HwpParserClass
{
  GObjectClass parent_class;
};

GType hwp_parser_get_type (void) G_GNUC_CONST;

HwpParser *hwp_parser_new   (HwpListener *listener,
                             gpointer     user_data);
void       hwp_parser_parse (HwpParser   *parser,
                             HwpFile     *file,
                             GError     **error);

G_END_DECLS

#endif /* __HWP_PARSER_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-parser.h
 *
 * Copyright (C) 2013-2014 Hodong Kim <hodong@cogno.org>
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

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_HWP3_PARSER_H__
#define __HWP_HWP3_PARSER_H__

#include <glib-object.h>
#include <gio/gio.h>
#include "hwp-listenable.h"
#include "hwp-hwp3-file.h"

G_BEGIN_DECLS

#define HWP_TYPE_HWP3_PARSER             (hwp_hwp3_parser_get_type ())
#define HWP_HWP3_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_HWP3_PARSER, HwpHWP3Parser))
#define HWP_HWP3_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_HWP3_PARSER, HwpHWP3ParserClass))
#define HWP_IS_HWP3_PARSER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_HWP3_PARSER))
#define HWP_IS_HWP3_PARSER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_HWP3_PARSER))
#define HWP_HWP3_PARSER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_HWP3_PARSER, HwpHWP3ParserClass))

typedef struct _HwpHWP3Parser        HwpHWP3Parser;
typedef struct _HwpHWP3ParserClass   HwpHWP3ParserClass;
typedef struct _HwpHWP3ParserPrivate HwpHWP3ParserPrivate;

struct _HwpHWP3Parser
{
  GObject        parent_instance;

  HwpListenable *listenable;
  GInputStream  *stream;
  gsize          bytes_read;
  gpointer       user_data;
};

/**
 * HwpHWP3ParserClass:
 * @parent_class: the parent class
 *
 * The class structure for the <structname>HwpHWP3ParserClass</structname> type.
 */
struct _HwpHWP3ParserClass
{
  GObjectClass parent_class;
};

GType          hwp_hwp3_parser_get_type    (void) G_GNUC_CONST;
HwpHWP3Parser *hwp_hwp3_parser_new         (HwpListenable *listenable,
                                            gpointer       user_data);
void           hwp_hwp3_parser_parse       (HwpHWP3Parser *parser,
                                            HwpHWP3File   *file,
                                            GError       **error);

G_END_DECLS

#endif /* __HWP_HWP3_PARSER_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-parser.h
 * This file is part of the libhwp project.
 *
 * Copyright (C) 2013-2016 Hodong Kim <cogniti@gmail.com>
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

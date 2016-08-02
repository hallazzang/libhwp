/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * hwp-parser.h
 * This file is part of the libhwp project.
 *
 * Copyright (C) 2014-2016 Hodong Kim <cogniti@gmail.com>
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

#ifndef __HWP_PARSER_H__
#define __HWP_PARSER_H__

#include <glib-object.h>
#include "hwp-listenable.h"
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

  HwpListenable *listenable;
  gpointer       user_data;
};

/**
 * HwpParserClass:
 * @parent_class: the parent class
 *
 * The class structure for the <structname>HwpParser</structname> type.
 */
struct _HwpParserClass
{
  GObjectClass parent_class;
};

GType hwp_parser_get_type (void) G_GNUC_CONST;

HwpParser *hwp_parser_new   (HwpListenable *listenable,
                             gpointer       user_data);
void       hwp_parser_parse (HwpParser     *parser,
                             HwpFile       *file,
                             GError       **error);

G_END_DECLS

#endif /* __HWP_PARSER_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-parser.c
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

#include "hwp-parser.h"
#include "hwp-hwp5-file.h"
#include "hwp-hwp5-parser.h"
#include "hwp-hwp3-file.h"
#include "hwp-hwp3-parser.h"
#include "hwp-hwpml-file.h"
#include "hwp-hwpml-parser.h"

G_DEFINE_TYPE (HwpParser, hwp_parser, G_TYPE_OBJECT);

static void hwp_parser_init (HwpParser *hwp_parser)
{
}

static void hwp_parser_finalize (GObject *object)
{
  G_OBJECT_CLASS (hwp_parser_parent_class)->finalize (object);
}

static void hwp_parser_class_init (HwpParserClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = hwp_parser_finalize;
}

/**
 * hwp_parser_new:
 * @listenable: a #HwpListenable
 * @user_data: a #gpointer
 *
 * Returns: a new #HwpParser
 *
 * Since: 0.1
 */
HwpParser *hwp_parser_new (HwpListenable *listenable,
                           gpointer       user_data)
{
  g_return_val_if_fail (HWP_IS_LISTENABLE (listenable), NULL);

  HwpParser *parser  = g_object_new (HWP_TYPE_PARSER, NULL);
  parser->listenable = listenable;
  parser->user_data  = user_data;

  return parser;
}

/**
 * hwp_parser_parse:
 * @parser:a #HwpParser
 * @file: a #HwpFile
 * @error: a #GError
 *
 * Since: 0.1
 */
void hwp_parser_parse (HwpParser *parser, HwpFile *file, GError **error)
{
  g_return_if_fail (HWP_IS_PARSER (parser) && HWP_IS_FILE (file));

  if (HWP_IS_HWP5_FILE (file))
  {
    HwpHWP5Parser *parser5;
    parser5 = hwp_hwp5_parser_new (parser->listenable, parser->user_data);
    hwp_hwp5_parser_parse (parser5, HWP_HWP5_FILE (file), error);
    g_object_unref (parser5);
  }
  else if (HWP_IS_HWPML_FILE (file))
  {
    HwpHWPMLParser *parser_ml;
    parser_ml = hwp_hwpml_parser_new (parser->listenable, parser->user_data);
    hwp_hwpml_parser_parse (parser_ml, HWP_HWPML_FILE (file), error);
    g_object_unref (parser_ml);
  }
  else if (HWP_IS_HWP3_FILE (file))
  {
    HwpHWP3Parser *parser3;
    parser3 = hwp_hwp3_parser_new (parser->listenable, parser->user_data);
    hwp_hwp3_parser_parse (parser3, HWP_HWP3_FILE (file), error);
    g_object_unref (parser3);
  }
}

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * ghwp-parser.c
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

#include "ghwp-parser.h"

G_DEFINE_INTERFACE (GHWPParser, ghwp_parser, G_TYPE_OBJECT)

static void ghwp_parser_default_init (GHWPParserInterface *klass)
{
}

void ghwp_parser_document_version (GHWPParser *parser,
                                   guint8      major_version,
                                   guint8      minor_version,
                                   guint8      micro_version,
                                   guint8      extra_version,
                                   gpointer    user_data,
                                   GError    **error)
{
  GHWPParserInterface *iface = GHWP_PARSER_GET_IFACE (parser);

  if (!iface->document_version)
    return;

  iface->document_version(parser,
                          major_version,
                          minor_version,
                          micro_version,
                          extra_version,
                          user_data,
                          error);
}

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * ghwp-parser.h
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

#include "ghwp-document.h"
#include "ghwp-file-v5.h"

G_BEGIN_DECLS

/**
 * GHWPParseState:
 *
 * This type indicates the current state of parsing.
 *
 * Since: 0.2
 */
typedef enum {
  GHWP_PARSE_STATE_NORMAL,
  GHWP_PARSE_STATE_PASSING,
  GHWP_PARSE_STATE_INSIDE_TABLE
} GHWPParseState;

/* GHWPParserInterface ********************************************************/

#define GHWP_TYPE_PARSER            (ghwp_parser_get_type ())
#define GHWP_PARSER(o)              (G_TYPE_CHECK_INSTANCE_CAST ((o), GHWP_TYPE_PARSER, GHWPParser))
#define GHWP_IS_PARSER(o)           (G_TYPE_CHECK_INSTANCE_TYPE ((o), GHWP_TYPE_PARSER))
#define GHWP_PARSER_GET_IFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GHWP_TYPE_PARSER, GHWPParserInterface))

typedef struct _GHWPParser          GHWPParser; /* dummy typedef */
typedef struct _GHWPParserInterface GHWPParserInterface;

/**
 * Since: TODO
 */
struct _GHWPParserInterface
{
  GTypeInterface            base_iface;

  void (*document_version) (GHWPParser        *parser,
                            guint8            major_version,
                            guint8            minor_version,
                            guint8            micro_version,
                            guint8            extra_version,
                            gpointer          user_data,
                            GError          **error);
};

GType ghwp_parser_get_type         (void) G_GNUC_CONST;
void  ghwp_parser_document_version (GHWPParser *parser,
                                   guint8     major_version,
                                   guint8     minor_version,
                                   guint8     micro_version,
                                   guint8     extra_version,
                                   gpointer   user_data,
                                   GError   **error);

G_END_DECLS

#endif /* __GHWP_PARSER_H__ */

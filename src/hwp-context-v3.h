/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-context-v3.h
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

#ifndef _GHWP_CONTEXT_V3_H_
#define _GHWP_CONTEXT_V3_H_

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GHWP_TYPE_CONTEXT_V3             (hwp_context_v3_get_type ())
#define GHWP_CONTEXT_V3(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_CONTEXT_V3, GHWPContextV3))
#define GHWP_CONTEXT_V3_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_CONTEXT_V3, GHWPContextV3Class))
#define GHWP_IS_CONTEXT_V3(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_CONTEXT_V3))
#define GHWP_IS_CONTEXT_V3_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_CONTEXT_V3))
#define GHWP_CONTEXT_V3_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_CONTEXT_V3, GHWPContextV3Class))

typedef struct _GHWPContextV3Class GHWPContextV3Class;
typedef struct _GHWPContextV3      GHWPContextV3;

struct _GHWPContextV3Class
{
	GObjectClass parent_class;
};

struct _GHWPContextV3
{
	GObject       parent_instance;
    GInputStream *stream;
    gsize         bytes_read;
};

GType          hwp_context_v3_get_type    (void) G_GNUC_CONST;
GHWPContextV3 *hwp_context_v3_new         (GInputStream  *stream);
gboolean       hwp_context_v3_read_uint8  (GHWPContextV3 *context,
                                            guint8        *i);
gboolean       hwp_context_v3_read_uint16 (GHWPContextV3 *context,
                                            guint16       *i);
gboolean       hwp_context_v3_read_uint32 (GHWPContextV3 *context,
                                            guint32       *i);
gboolean       hwp_context_v3_read        (GHWPContextV3 *context,
                                            void          *buffer,
                                            gsize          count);
gboolean       hwp_context_v3_skip        (GHWPContextV3 *context,
                                            guint16        count);
G_END_DECLS

#endif /* _GHWP_CONTEXT_V3_H_ */

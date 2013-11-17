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

#ifndef _HWP_CONTEXT_V3_H_
#define _HWP_CONTEXT_V3_H_

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HWP_TYPE_CONTEXT_V3             (hwp_context_v3_get_type ())
#define HWP_CONTEXT_V3(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_CONTEXT_V3, HWPContextV3))
#define HWP_CONTEXT_V3_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_CONTEXT_V3, HWPContextV3Class))
#define HWP_IS_CONTEXT_V3(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_CONTEXT_V3))
#define HWP_IS_CONTEXT_V3_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_CONTEXT_V3))
#define HWP_CONTEXT_V3_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_CONTEXT_V3, HWPContextV3Class))

typedef struct _HWPContextV3Class HWPContextV3Class;
typedef struct _HWPContextV3      HWPContextV3;

struct _HWPContextV3Class
{
	GObjectClass parent_class;
};

struct _HWPContextV3
{
	GObject       parent_instance;
    GInputStream *stream;
    gsize         bytes_read;
};

GType          hwp_context_v3_get_type    (void) G_GNUC_CONST;
HWPContextV3 *hwp_context_v3_new         (GInputStream  *stream);
gboolean       hwp_context_v3_read_uint8  (HWPContextV3 *context,
                                            guint8        *i);
gboolean       hwp_context_v3_read_uint16 (HWPContextV3 *context,
                                            guint16       *i);
gboolean       hwp_context_v3_read_uint32 (HWPContextV3 *context,
                                            guint32       *i);
gboolean       hwp_context_v3_read        (HWPContextV3 *context,
                                            void          *buffer,
                                            gsize          count);
gboolean       hwp_context_v3_skip        (HWPContextV3 *context,
                                            guint16        count);
G_END_DECLS

#endif /* _HWP_CONTEXT_V3_H_ */

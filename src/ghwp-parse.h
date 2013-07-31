/*
 * ghwp-parse.h
 *
 * Copyright (C) 2012-2013  Hodong Kim <cogniti@gmail.com>
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

#ifndef __GHWP_CONTEXT_H__
#define __GHWP_CONTEXT_H__

#include <glib-object.h>
#include <gio/gio.h>

#include "ghwp-document.h"

G_BEGIN_DECLS

#define GHWP_TYPE_CONTEXT             (ghwp_context_get_type ())
#define GHWP_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_CONTEXT, GHWPContext))
#define GHWP_CONTEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_CONTEXT, GHWPContextClass))
#define GHWP_IS_CONTEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_CONTEXT))
#define GHWP_IS_CONTEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_CONTEXT))
#define GHWP_CONTEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_CONTEXT, GHWPContextClass))

/*typedef enum*/
/*{*/
/*    GHWP_PARSE_ERROR_UNKNOWN_TAG*/
/*} GHWPParseError;*/

typedef enum
{
    STATE_NORMAL,
    STATE_PASSING,
    STATE_INSIDE_TABLE
} GHWPParseState;

typedef struct _GHWPContext        GHWPContext;
typedef struct _GHWPContextClass   GHWPContextClass;
typedef struct _GHWPContextPrivate GHWPContextPrivate;

struct _GHWPContext {
    GObject             parent_instance;
    GHWPContextPrivate *priv;
    GInputStream       *stream;
    GHWPDocument       *document;
    /* from record header */
    guint16             tag_id;
    guint16             level;
    guint16             data_len;
    /* for checking */
    guint16             data_count;
    /* for parsing */
    guint8              state;
    guint32             ctrl_id;
};

struct _GHWPContextClass {
    GObjectClass parent_class;
};

struct _GHWPContextPrivate {
    guint32  header;
    gsize    bytes_read;
    gboolean ret;
};

GType        ghwp_context_get_type (void) G_GNUC_CONST;
GHWPContext *ghwp_context_new      (GInputStream *stream);
gboolean     ghwp_context_pull     (GHWPContext  *context, GError **error);
gboolean     context_read_uint16   (GHWPContext  *context,
                                    guint16      *i);
gboolean     context_read_uint32   (GHWPContext  *context,
                                    guint32      *i);
gboolean     context_skip          (GHWPContext  *context,
                                    guint16       count);

G_END_DECLS

#endif /* __GHWP_CONTEXT_H__ */

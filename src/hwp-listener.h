/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-listener.h
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

#ifndef __GHWP_LISTENER_H__
#define __GHWP_LISTENER_H__

#include <glib-object.h>

#include "hwp-models.h"

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

/* GHWPListenerInterface ********************************************************/

#define GHWP_TYPE_LISTENER            (hwp_listener_get_type ())
#define GHWP_LISTENER(o)              (G_TYPE_CHECK_INSTANCE_CAST ((o), GHWP_TYPE_LISTENER, GHWPListener))
#define GHWP_IS_LISTENER(o)           (G_TYPE_CHECK_INSTANCE_TYPE ((o), GHWP_TYPE_LISTENER))
#define GHWP_LISTENER_GET_IFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GHWP_TYPE_LISTENER, GHWPListenerInterface))

typedef struct _GHWPListener          GHWPListener; /* dummy typedef */
typedef struct _GHWPListenerInterface GHWPListenerInterface;

/**
 * Since: TODO
 */
struct _GHWPListenerInterface
{
  GTypeInterface            base_iface;

  void (*document_version) (GHWPListener *listener,
                            guint8        major_version,
                            guint8        minor_version,
                            guint8        micro_version,
                            guint8        extra_version,
                            gpointer      user_data,
                            GError      **error);
  void (*object)           (GHWPListener *listener,
                            GObject      *object,
                            gpointer      user_data,
                            GError      **error);
};

GType hwp_listener_get_type         (void) G_GNUC_CONST;
/*void  hwp_listener_document_version (GHWPListener *listener,*/
/*                                      guint8        major_version,*/
/*                                      guint8        minor_version,*/
/*                                      guint8        micro_version,*/
/*                                      guint8        extra_version,*/
/*                                      gpointer      user_data,*/
/*                                      GError      **error);*/

G_END_DECLS

#endif /* __GHWP_LISTENER_H__ */

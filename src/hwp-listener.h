/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-listener.h
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
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

#ifndef __HWP_LISTENER_H__
#define __HWP_LISTENER_H__

#include <glib-object.h>

#include "hwp-models.h"

G_BEGIN_DECLS

/**
 * HwpParseState:
 *
 * This type indicates the current state of parsing.
 *
 * Since: 0.2
 */
typedef enum {
  HWP_PARSE_STATE_NORMAL,
  HWP_PARSE_STATE_PASSING,
  HWP_PARSE_STATE_INSIDE_TABLE
} HwpParseState;

/* HwpListenerInterface ********************************************************/

#define HWP_TYPE_LISTENER            (hwp_listener_get_type ())
#define HWP_LISTENER(o)              (G_TYPE_CHECK_INSTANCE_CAST ((o), HWP_TYPE_LISTENER, HwpListener))
#define HWP_IS_LISTENER(o)           (G_TYPE_CHECK_INSTANCE_TYPE ((o), HWP_TYPE_LISTENER))
#define HWP_LISTENER_GET_IFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), HWP_TYPE_LISTENER, HwpListenerInterface))

typedef struct _HwpListener          HwpListener; /* dummy typedef */
typedef struct _HwpListenerInterface HwpListenerInterface;

/**
 * Since: TODO
 */
struct _HwpListenerInterface
{
  GTypeInterface             base_iface;

  void (* text)             (HwpListener *listener,
                             const gchar *text,
                             gsize        text_len,
                             gpointer     user_data,
                             GError     **error);
  void (* document_version) (HwpListener *listener,
                             guint8       major_version,
                             guint8       minor_version,
                             guint8       micro_version,
                             guint8       extra_version,
                             gpointer     user_data,
                             GError     **error);
  void (* object)           (HwpListener *listener,
                             GObject     *object,
                             gpointer     user_data,
                             GError     **error);
  void (* prv_text)         (HwpListener *listener,
                             const gchar *prv_text,
                             gpointer     user_data,
                             GError     **error);
  /* summary information */
  void (* creator)          (HwpListener *listener,
                             const gchar *creator,
                             gpointer     user_data,
                             GError     **error);
  void (* mod_date)         (HwpListener *listener,
                             GTime        mod_date,
                             gpointer     user_data,
                             GError     **error);
  void (* desc)             (HwpListener *listener,
                             const gchar *desc,
                             gpointer     user_data,
                             GError     **error);
  void (* keywords)         (HwpListener *listener,
                             const gchar *keywords,
                             gpointer     user_data,
                             GError     **error);
  void (* subject)          (HwpListener *listener,
                             const gchar *subject,
                             gpointer     user_data,
                             GError     **error);
  void (* title)            (HwpListener *listener,
                             const gchar *title,
                             gpointer     user_data,
                             GError     **error);
  void (* last_printed)     (HwpListener *listener,
                             GTime        last_printed,
                             gpointer     user_data,
                             GError     **error);
  void (* last_saved_by)    (HwpListener *listener,
                             const gchar *last_saved_by,
                             gpointer     user_data,
                             GError     **error);
  void (* creation_date)    (HwpListener *listener,
                             GTime        creation_date,
                             gpointer     user_data,
                             GError     **error);
  void (* hanword_version)  (HwpListener *listener,
                             const gchar *hanword_version,
                             gpointer     user_data,
                             GError     **error);
};

GType hwp_listener_get_type         (void) G_GNUC_CONST;
/*void  hwp_listener_document_version (HwpListener *listener,*/
/*                                      guint8        major_version,*/
/*                                      guint8        minor_version,*/
/*                                      guint8        micro_version,*/
/*                                      guint8        extra_version,*/
/*                                      gpointer      user_data,*/
/*                                      GError      **error);*/

G_END_DECLS

#endif /* __HWP_LISTENER_H__ */
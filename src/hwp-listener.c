/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-listener.c
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

#include "hwp-listener.h"

G_DEFINE_INTERFACE (GHWPListener, hwp_listener, G_TYPE_OBJECT)

static void hwp_listener_default_init (GHWPListenerInterface *klass)
{
}

/*void hwp_listener_document_version (GHWPListener *listener,*/
/*                                     guint8        major_version,*/
/*                                     guint8        minor_version,*/
/*                                     guint8        micro_version,*/
/*                                     guint8        extra_version,*/
/*                                     gpointer      user_data,*/
/*                                     GError      **error)*/
/*{*/
/*  GHWPListenerInterface *iface = GHWP_LISTENER_GET_IFACE (listener);*/

/*  if (!iface->document_version)*/
/*    return;*/

/*  iface->document_version(listener,*/
/*                          major_version,*/
/*                          minor_version,*/
/*                          micro_version,*/
/*                          extra_version,*/
/*                          user_data,*/
/*                          error);*/
/*}*/

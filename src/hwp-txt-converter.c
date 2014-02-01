/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-txt-converter.c
 * 
 * Copyright (C) 2014 Hodong Kim <hodong@cogno.org>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hwp-txt-converter.h"
#include "hwp-listener.h"

static void hwp_txt_listener_iface_init (HwpListenerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HwpTxtConverter, hwp_txt_converter, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (HWP_TYPE_LISTENER, hwp_txt_listener_iface_init))

/**
 * hwp_txt_converter_new:
 * 
 * Creates a new #HwpTxtConverter.
 * 
 * Return value: a new #HwpTxtConverter
 *
 * Since: 0.0.1
 */
HwpTxtConverter *hwp_txt_converter_new ()
{
    return (HwpTxtConverter *) g_object_new (HWP_TYPE_TXT_CONVERTER, NULL);
}

static void
hwp_txt_converter_init (HwpTxtConverter *hwp_txt_converter)
{

	
	/* TODO: Add initialization code here */
}

static void
hwp_txt_converter_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (hwp_txt_converter_parent_class)->finalize (object);
}

static void
hwp_txt_converter_class_init (HwpTxtConverterClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = hwp_txt_converter_finalize;
}

void text (HwpListener *listener,
           const gchar *text,
           gsize        text_len,
           gpointer     user_data,
           GError     **error)
{
  printf ("%s", text);
}

static void hwp_txt_listener_iface_init (HwpListenerInterface *iface)
{
  iface->text = text;
}
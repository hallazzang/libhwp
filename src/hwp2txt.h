/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-txt-converter.h
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

#ifndef __HWP_TXT_CONVERTER_H__
#define __HWP_TXT_CONVERTER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HWP_TYPE_TXT_CONVERTER             (hwp_txt_converter_get_type ())
#define HWP_TXT_CONVERTER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TXT_CONVERTER, HwpTxtConverter))
#define HWP_TXT_CONVERTER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TXT_CONVERTER, HwpTxtConverterClass))
#define HWP_IS_TXT_CONVERTER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TXT_CONVERTER))
#define HWP_IS_TXT_CONVERTER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TXT_CONVERTER))
#define HWP_TXT_CONVERTER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TXT_CONVERTER, HwpTxtConverterClass))

typedef struct _HwpTxtConverter      HwpTxtConverter;
typedef struct _HwpTxtConverterClass HwpTxtConverterClass;

struct _HwpTxtConverterClass
{
	GObjectClass parent_class;
};

struct _HwpTxtConverter
{
	GObject parent_instance;
};

GType hwp_txt_converter_get_type (void) G_GNUC_CONST;

HwpTxtConverter *hwp_txt_converter_new ();

G_END_DECLS

#endif /* __HWP_TXT_CONVERTER_H__ */
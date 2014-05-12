/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-to-txt.h
 * 
 * Copyright (C) 2014 Hodong Kim <hodong@cogno.org>
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HWP_TO_TXT_H__
#define __HWP_TO_TXT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HWP_TYPE_TO_TXT             (hwp_to_txt_get_type ())
#define HWP_TO_TXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_TO_TXT, HwpToTxt))
#define HWP_TO_TXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_TO_TXT, HwpToTxtClass))
#define HWP_IS_TO_TXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_TO_TXT))
#define HWP_IS_TO_TXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_TO_TXT))
#define HWP_TO_TXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_TO_TXT, HwpToTxtClass))

typedef struct _HwpToTxt      HwpToTxt;
typedef struct _HwpToTxtClass HwpToTxtClass;

struct _HwpToTxtClass
{
  GObjectClass parent_class;
};

struct _HwpToTxt
{
  GObject parent_instance;

  GOutputStream *output_stream;
};

GType hwp_to_txt_get_type (void) G_GNUC_CONST;

HwpToTxt *hwp_to_txt_new ();

G_END_DECLS

#endif /* __HWP_TO_TXT_H__ */

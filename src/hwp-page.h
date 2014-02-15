/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-page.h
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

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_PAGE_H__
#define __HWP_PAGE_H__

#include <glib-object.h>
#include <cairo.h>

#include "hwp-enums.h"

G_BEGIN_DECLS

#define HWP_TYPE_PAGE             (hwp_page_get_type ())
#define HWP_PAGE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_PAGE, HwpPage))
#define HWP_PAGE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_PAGE, HwpPageClass))
#define HWP_IS_PAGE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_PAGE))
#define HWP_IS_PAGE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_PAGE))
#define HWP_PAGE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_PAGE, HwpPageClass))

typedef struct _HwpPage       HwpPage;
typedef struct _HwpPageClass  HwpPageClass;

struct _HwpPage
{
  GObject parent_instance;
  GArray *paragraphs;
};

struct _HwpPageClass
{
  GObjectClass parent_class;
};

GType    hwp_page_get_type    (void) G_GNUC_CONST;
HwpPage *hwp_page_new         (void);
void     hwp_page_get_size    (HwpPage *page,
                               gdouble *width,
                               gdouble *height);
gboolean  hwp_page_render     (HwpPage *page,    cairo_t *cr);

G_END_DECLS

#endif /* __HWP_PAGE_H__ */

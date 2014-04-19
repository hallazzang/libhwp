/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-page.h
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_PAGE_H__
#define __HWP_PAGE_H__

#include <glib-object.h>
#include <cairo.h>

#include "hwp-enums.h"
#include <poppler.h>

G_BEGIN_DECLS

#define HWP_TYPE_PAGE             (hwp_page_get_type ())
#define HWP_PAGE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_PAGE, HwpPage))
#define HWP_PAGE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_PAGE, HwpPageClass))
#define HWP_IS_PAGE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_PAGE))
#define HWP_IS_PAGE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_PAGE))
#define HWP_PAGE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_PAGE, HwpPageClass))

typedef struct _HwpPage       HwpPage;
typedef struct _HwpPageClass  HwpPageClass;

typedef struct _HwpRectangle  HwpRectangle;
typedef struct _HwpColor      HwpColor;

struct _HwpPage
{
  GObject parent_instance;

  PopplerPage *poppler_page;
};

struct _HwpPageClass
{
  GObjectClass parent_class;
};

GType    hwp_page_get_type   (void) G_GNUC_CONST;
HwpPage *hwp_page_new        (void);
void     hwp_page_get_size   (HwpPage          *page,
                              gdouble          *width,
                              gdouble          *height);
void     hwp_page_render     (HwpPage          *page,
                              cairo_t          *cr);
void
hwp_page_render_selection    (HwpPage          *page,
                              cairo_t          *cairo,
                              HwpRectangle     *selection,
                              HwpRectangle     *old_selection,
                              HwpSelectionStyle style,
                              HwpColor         *glyph_color,
                              HwpColor         *background_color);
char *
hwp_page_get_selected_text   (HwpPage          *page,
                              HwpSelectionStyle style,
                              HwpRectangle     *selection);
cairo_region_t *
hwp_page_get_selected_region (HwpPage          *page,
                              gdouble           scale,
                              HwpSelectionStyle style,
                              HwpRectangle     *selection);
GList *hwp_page_find_text    (HwpPage          *page,
                              const char       *text);
void
hwp_page_render_for_printing (HwpPage          *page,
                              cairo_t          *cairo);

/*
 * A convenience macro for boxed type implementations, which defines a
 * type_name_get_type() function registering the boxed type.
 */
#define HWP_DEFINE_BOXED_TYPE(TypeName, type_name, copy_func, free_func)          \
GType                                                                                 \
type_name##_get_type (void)                                                           \
{                                                                                     \
        static volatile gsize g_define_type_id__volatile = 0;                         \
	if (g_once_init_enter (&g_define_type_id__volatile)) {                        \
	        GType g_define_type_id =                                              \
		    g_boxed_type_register_static (g_intern_static_string (#TypeName), \
		                                  (GBoxedCopyFunc) copy_func,         \
						  (GBoxedFreeFunc) free_func);        \
		g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);    \
	}                                                                             \
	return g_define_type_id__volatile;                                            \
}

/* A rectangle on a page */
#define HWP_TYPE_RECTANGLE             (hwp_rectangle_get_type ())
/**
 * HwpRectangle:
 * @x1: x coordinate of lower left corner
 * @y1: y coordinate of lower left corner
 * @x2: x coordinate of upper right corner
 * @y2: y coordinate of upper right corner
 *
 * A #HwpRectangle is used to describe
 * locations on a page and bounding boxes
 */
struct _HwpRectangle
{
  gdouble x1;
  gdouble y1;
  gdouble x2;
  gdouble y2;
};

GType         hwp_rectangle_get_type (void) G_GNUC_CONST;
HwpRectangle *hwp_rectangle_new      (void);
HwpRectangle *hwp_rectangle_copy     (HwpRectangle *rectangle);
void          hwp_rectangle_free     (HwpRectangle *rectangle);

/* A color in RGB */
#define HWP_TYPE_COLOR                 (hwp_color_get_type ())

/**
 * HwpColor:
 * @red: the red componment of color
 * @green: the green component of color
 * @blue: the blue component of color
 *
 * A #HwpColor describes a RGB color. Color components
 * are values between 0 and 65535
 */
struct _HwpColor
{
  guint16 red;
  guint16 green;
  guint16 blue;
};

GType     hwp_color_get_type (void) G_GNUC_CONST;
HwpColor *hwp_color_new      (void);
HwpColor *hwp_color_copy     (HwpColor *color);
void      hwp_color_free     (HwpColor *color);

G_END_DECLS

#endif /* __HWP_PAGE_H__ */

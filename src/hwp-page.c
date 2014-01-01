/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-page.c
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

#include "hwp-page.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "hwp-models.h"

G_DEFINE_TYPE (HwpPage, hwp_page, G_TYPE_OBJECT);

/**
 * Since: 0.1
 */
void hwp_page_get_size (HwpPage *page,
                         gdouble  *width,
                         gdouble  *height)
{
    g_return_if_fail (HWP_IS_PAGE (page));

    *width  = 595.0;
    *height = 842.0;
}

static void hwp_show_layout (cairo_t *cr, HwpLayout *layout)
{
    cairo_move_to (cr, layout->x, layout->y);
    pango_cairo_show_layout (cr, layout->pango_layout);
}

/**
 * Since: 0.1
 */
gboolean hwp_page_render (HwpPage *page, cairo_t *cr)
{
    g_return_val_if_fail (HWP_IS_PAGE (page) && cr, FALSE);

    HwpLayout *layout;

    for (guint i = 0; i < page->layouts->len; i++) {
        layout = g_array_index (page->layouts, HwpLayout *, i);
        hwp_show_layout (cr, layout);
    }

    return TRUE;
}

/**
 * Since: 0.1
 */
HwpPage *hwp_page_new (void)
{
    return (HwpPage *) g_object_new (HWP_TYPE_PAGE, NULL);
}

/**
 * Since: TODO
 */
void
hwp_page_render_selection (HwpPage           *page,
                            cairo_t            *cr,
                            HwpRectangle      *selection,
                            HwpRectangle      *old_selection,
                            HwpSelectionStyle  style, 
                            HwpColor          *glyph_color,
                            HwpColor          *background_color)
{
    g_return_if_fail (page != NULL);
    /* TODO */
}

/**
 * Since: TODO
 */
char *
hwp_page_get_selected_text (HwpPage          *page,
                             HwpSelectionStyle style,
                             HwpRectangle     *selection)
{
    g_return_val_if_fail (page != NULL, NULL);
    /* TODO */
    return NULL;
}

/**
 * Since: TODO
 */
cairo_region_t *
hwp_page_get_selection_region (HwpPage          *page,
                                gdouble            scale,
                                HwpSelectionStyle style,
                                HwpRectangle     *selection)
{
    g_return_val_if_fail (page != NULL, NULL);
    /* TODO */
    return NULL;
}

/**
 * hwp_rectangle_free:
 * @rectangle: a #HwpRectangle
 *
 * Frees the given #HwpRectangle
 *
 * Since: 0.2
 */
void hwp_rectangle_free (HwpRectangle *rectangle)
{
    g_return_if_fail (rectangle != NULL);
    g_slice_free (HwpRectangle, rectangle);
}

static void hwp_page_finalize (GObject *obj)
{
    HwpPage *page = HWP_PAGE (obj);
    g_array_free (page->paragraphs, TRUE);
    g_array_free (page->layouts,    TRUE);
    G_OBJECT_CLASS (hwp_page_parent_class)->finalize (obj);
}

static void hwp_page_class_init (HwpPageClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = hwp_page_finalize;
}

static void hwp_page_init (HwpPage *page)
{
    page->paragraphs = g_array_new (TRUE, TRUE, sizeof (HwpParagraph *));
    page->layouts    = g_array_new (TRUE, TRUE, sizeof (HwpLayout   *));
}

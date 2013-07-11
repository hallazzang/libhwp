/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-page.c
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

#include "ghwp-page.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>

G_DEFINE_TYPE (GHWPPage, ghwp_page, G_TYPE_OBJECT);

void ghwp_page_get_size (GHWPPage *page,
                         gdouble  *width,
                         gdouble  *height)
{
    g_return_if_fail (page != NULL);
    *width  = 595.0;
    *height = 842.0;
}

PangoContext *
ghwp_cairo_create_pango_context (cairo_t *cr)
{
    PangoFontMap *fontmap;
    PangoContext *context;

    fontmap = pango_cairo_font_map_get_default ();
    context = pango_font_map_create_context (fontmap);
    pango_cairo_update_context (cr, context);
    pango_cairo_context_set_resolution (context, 0);
    return context;
}

static PangoLayout *
ghwp_create_pango_layout (PangoContext * context)
{
    PangoFontDescription *desc;
    PangoLayout *layout;

    pango_context_set_language (context, pango_language_from_string ("ko-KR"));
    pango_context_set_base_dir (context, PANGO_DIRECTION_LTR);
    pango_context_set_base_gravity (context, PANGO_GRAVITY_SOUTH);

    desc = pango_font_description_copy (pango_context_get_font_description (context));

    pango_font_description_set_family_static (desc, "HCR Batang");
    pango_font_description_set_style   (desc, PANGO_STYLE_NORMAL);
    pango_font_description_set_variant (desc, PANGO_VARIANT_NORMAL);
    pango_font_description_set_weight  (desc, PANGO_WEIGHT_NORMAL);
    pango_font_description_set_stretch (desc, PANGO_STRETCH_NORMAL);
    pango_font_description_set_size    (desc, 14 * PANGO_SCALE);

    layout = pango_layout_new (context);
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);

    pango_layout_set_text (layout, NULL, 0);
    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);

    return layout;
}

gboolean ghwp_page_render (GHWPPage *page, cairo_t *cr)
{
    g_return_val_if_fail (page != NULL, FALSE);
    g_return_val_if_fail (cr   != NULL, FALSE);
    cairo_save (cr);
    static double y = 0;
    gint i;
    PangoLayout *layout;
    GHWPParagraph *paragraph;
    PangoContext *context;

    context = ghwp_cairo_create_pango_context (cr);
    layout = ghwp_create_pango_layout (context);

    for (i = 0; i < page->paragraphs->len; i++) {
        paragraph = g_array_index (page->paragraphs, GHWPParagraph *, i);
        pango_layout_set_text (layout, paragraph->ghwp_text->text, -1);
        pango_cairo_update_layout (cr, layout);
        pango_cairo_show_layout   (cr, layout);
        cairo_move_to(cr, 0, y);
        y += 30;
    }

    g_object_unref (layout);
    g_object_unref (context);
    cairo_restore (cr);

    return TRUE;
}

GHWPPage *ghwp_page_new (void)
{
    return (GHWPPage *) g_object_new (GHWP_TYPE_PAGE, NULL);
}

static void ghwp_page_finalize (GObject *obj)
{
    GHWPPage *page = GHWP_PAGE(obj);
    g_array_free (page->paragraphs, TRUE);
    G_OBJECT_CLASS (ghwp_page_parent_class)->finalize (obj);
}

static void ghwp_page_class_init (GHWPPageClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_page_finalize;
}

static void ghwp_page_init (GHWPPage *page)
{
    page->paragraphs = g_array_new (TRUE, TRUE, sizeof (GHWPParagraph *));
}

/* experimental */
void
ghwp_page_render_selection (GHWPPage           *page,
                            cairo_t            *cr,
                            GHWPRectangle      *selection,
                            GHWPRectangle      *old_selection,
                            GHWPSelectionStyle  style, 
                            GHWPColor          *glyph_color,
                            GHWPColor          *background_color)
{
    g_return_if_fail (page != NULL);
    /* TODO */
}

/* experimental */
char *
ghwp_page_get_selected_text (GHWPPage          *page,
                             GHWPSelectionStyle style,
                             GHWPRectangle     *selection)
{
    g_return_val_if_fail (page != NULL, NULL);
    /* TODO */
    return NULL;
}

/* experimental */
cairo_region_t *
ghwp_page_get_selection_region (GHWPPage          *page,
                                gdouble            scale,
                                GHWPSelectionStyle style,
                                GHWPRectangle     *selection)
{
    g_return_val_if_fail (page != NULL, NULL);
    /* TODO */
    return NULL;
}

/**
 * ghwp_rectangle_free:
 * @rectangle: a #GHWPRectangle
 *
 * Frees the given #GHWPRectangle
 */
void
ghwp_rectangle_free (GHWPRectangle *rectangle)
{
    g_return_if_fail (rectangle != NULL);
    g_slice_free (GHWPRectangle, rectangle);
}

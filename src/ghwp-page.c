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

#include <math.h>
#include <pango/pangocairo.h>
static void
draw_text (cairo_t *cr)
{
#define RADIUS 150
#define N_WORDS 10
#define FONT "Sans Bold 27"
  PangoLayout *layout;
  PangoFontDescription *desc;
  int i;
  /* Center coordinates on the middle of the region we are drawing
   */
  cairo_translate (cr, RADIUS, RADIUS);
  /* Create a PangoLayout, set the font and text */
  layout = pango_cairo_create_layout (cr);
  pango_layout_set_text (layout, "Text", -1);
  desc = pango_font_description_from_string (FONT);
  pango_layout_set_font_description (layout, desc);
  pango_font_description_free (desc);
  /* Draw the layout N_WORDS times in a circle */
  for (i = 0; i < N_WORDS; i++)
    {
      int width, height;
      double angle = (360. * i) / N_WORDS;
      double red;
      cairo_save (cr);
      /* Gradient from red at angle == 60 to blue at angle == 240 */
      red   = (1 + cos ((angle - 60) * G_PI / 180.)) / 2;
      cairo_set_source_rgb (cr, red, 0, 1.0 - red);
      cairo_rotate (cr, angle * G_PI / 180.);
      /* Inform Pango to re-layout the text with the new transformation */
      pango_cairo_update_layout (cr, layout);
      pango_layout_get_size (layout, &width, &height);
      cairo_move_to (cr, - ((double)width / PANGO_SCALE) / 2, - RADIUS);
      pango_cairo_show_layout (cr, layout);
      cairo_restore (cr);
    }
  /* free the layout object */
  g_object_unref (layout);
}

gboolean ghwp_page_render (GHWPPage *page, cairo_t *cr)
{
    g_return_val_if_fail (page != NULL, FALSE);
    g_return_val_if_fail (cr   != NULL, FALSE);

    draw_text (cr);
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

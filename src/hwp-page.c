/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-page.c
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

#include "hwp-page.h"
#include "hwp-models.h"

G_DEFINE_TYPE (HwpPage, hwp_page, G_TYPE_OBJECT);

/**
 * hwp_page_get_size:
 * @page: #HwpPage
 * @width:
 * @height:
 *
 * Returns:
 *
 * Since: 0.0.1
 */
void hwp_page_get_size (HwpPage *page, gdouble *width, gdouble *height)
{
  g_return_if_fail (HWP_IS_PAGE (page));

  poppler_page_get_size (page->poppler_page, width, height);
}

/**
 * hwp_page_get_selected_text:
 * @page: a #HwpPage
 * @style: a #HwpSelectionStyle
 * @selection: the #HwpRectangle including the text
 *
 * Retrieves the contents of the specified @selection as text.
 *
 * Return value: a pointer to the contents of the @selection
 *               as a string
 * Since: 0.0.3
 */
char *hwp_page_get_selected_text (HwpPage           *page,
                                  HwpSelectionStyle  style,
                                  HwpRectangle      *selection)
{
  g_return_val_if_fail (HWP_IS_PAGE (page), NULL);
  g_return_val_if_fail (selection != NULL, NULL);

  return poppler_page_get_selected_text (POPPLER_PAGE (page->poppler_page),
                                         (PopplerSelectionStyle) style,
                                         (PopplerRectangle *) selection);
}

/**
 * hwp_page_get_text:
 * @page: a #HwpPage
 *
 * Retrieves the text of @page.
 *
 * Return value: a pointer to the text of the @page
 *               as a string
 * Since: 0.0.4
 */
char *hwp_page_get_text (HwpPage *page)
{
  g_return_val_if_fail (HWP_IS_PAGE (page), NULL);

  return poppler_page_get_text (POPPLER_PAGE (page->poppler_page));
}

/**
 * hwp_page_get_selected_region:
 * @page: a #HwpPage
 * @scale: scale specified as pixels per point
 * @style: a #HwpSelectionStyle
 * @selection: start and end point of selection as a rectangle
 *
 * Returns a region containing the area that would be rendered by
 * hwp_page_render_selection().
 * The returned region must be freed with cairo_region_destroy()
 *
 * Return value: (transfer full): a cairo_region_t
 *
 * Since: 0.0.3
 */
cairo_region_t *
hwp_page_get_selected_region (HwpPage          *page,
                              gdouble           scale,
                              HwpSelectionStyle style,
                              HwpRectangle     *selection)
{
  return poppler_page_get_selected_region (page->poppler_page,
                                           scale,
                                           (PopplerSelectionStyle)  style,
                                           (PopplerRectangle *) selection);
}

/**
 * hwp_page_render_selection:
 * @page: the #HwpPage for which to render selection
 * @cairo: cairo context to render to
 * @selection: start and end point of selection as a rectangle
 * @old_selection: previous selection
 * @style: a #HwpSelectionStyle
 * @glyph_color: color to use for drawing glyphs
 * @background_color: color to use for the selection background
 *
 * Render the selection specified by @selection for @page to
 * the given cairo context.  The selection will be rendered, using
 * @glyph_color for the glyphs and @background_color for the selection
 * background.
 *
 * If non-NULL, @old_selection specifies the selection that is already
 * rendered to @cairo, in which case this function will (some day)
 * only render the changed part of the selection.
 *
 * Since: 0.0.3
 */
void hwp_page_render_selection (HwpPage          *page,
                                cairo_t          *cairo,
                                HwpRectangle     *selection,
                                HwpRectangle     *old_selection,
                                HwpSelectionStyle style,
                                HwpColor         *glyph_color,
                                HwpColor         *background_color)
{
  poppler_page_render_selection (page->poppler_page,
                                 cairo,
                                 (PopplerRectangle *) selection,
                                 (PopplerRectangle *) old_selection,
                                 (PopplerSelectionStyle) style,
                                 (PopplerColor *) glyph_color,
                                 (PopplerColor *) background_color);
}
/**
 * hwp_page_new:
 *
 * Returns: a #HwpPage
 *
 * Since: 0.0.1
 */
HwpPage *hwp_page_new (void)
{
  return g_object_new (HWP_TYPE_PAGE, NULL);
}

static void hwp_page_finalize (GObject *object)
{
  HwpPage *page = HWP_PAGE (object);

  if (page->poppler_page)
    g_object_unref (page->poppler_page);

  g_ptr_array_free (page->layouts, TRUE);

  G_OBJECT_CLASS (hwp_page_parent_class)->finalize (object);
}

static void hwp_page_class_init (HwpPageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_page_finalize;
}

static void hwp_page_init (HwpPage *page)
{
  page->layouts = g_ptr_array_new_with_free_func ((GDestroyNotify) hwp_layout_free);
}

/**
 * hwp_page_render:
 * @page: the page to render from
 * @cr: cairo context to render to
 *
 * Render the page to the given cairo context. This function
 * is for rendering a page that will be displayed. If you want
 * to render a page that will be printed use
 * hwp_page_render_for_printing() instead
 *
 * Since: 0.0.3
 */
void hwp_page_render (HwpPage *page, cairo_t *cr)
{
  g_return_if_fail (HWP_IS_PAGE (page));
  g_return_if_fail (cr);

  poppler_page_render (page->poppler_page, cr);
}

/**
 * hwp_page_render_for_printing:
 * @page: a #HwpPage
 * @cairo: a #cairo_t
 *
 * Since: 0.0.3
 */
void hwp_page_render_for_printing (HwpPage *page,
                                   cairo_t *cairo)
{
  g_return_if_fail (HWP_IS_PAGE (page));

  poppler_page_render_for_printing (page->poppler_page, cairo);
}

/**
 * hwp_page_find_text:
 * @page: a #HwpPage
 * @text: the text to search for (UTF-8 encoded)
 *
 * A #GList of rectangles for each occurance of the text on the page.
 * The coordinates are in HWP points.
 *
 * Return value: (element-type HwpRectangle) (transfer full): a #GList of #HwpRectangle,
 */
GList *hwp_page_find_text (HwpPage *page, const char *text)
{
  return poppler_page_find_text (page->poppler_page, text);
}

/* HwpRectangle type */

HWP_DEFINE_BOXED_TYPE (HwpRectangle, hwp_rectangle,
                       hwp_rectangle_copy,
                       hwp_rectangle_free)

/**
 * hwp_rectangle_new:
 *
 * Creates a new #HwpRectangle
 *
 * Returns: a new #HwpRectangle, use hwp_rectangle_free() to free it
 */
HwpRectangle *hwp_rectangle_new (void)
{
  return g_slice_new0 (HwpRectangle);
}

/**
 * hwp_rectangle_copy:
 * @rectangle: a #HwpRectangle to copy
 *
 * Creates a copy of @rectangle
 *
 * Returns: a new allocated copy of @rectangle
 */
HwpRectangle *hwp_rectangle_copy (HwpRectangle *rectangle)
{
  g_return_val_if_fail (rectangle != NULL, NULL);

  return g_slice_dup (HwpRectangle, rectangle);
}

/**
 * hwp_rectangle_free:
 * @rectangle: a #HwpRectangle
 *
 * Frees the given #HwpRectangle
 */
void hwp_rectangle_free (HwpRectangle *rectangle)
{
  g_slice_free (HwpRectangle, rectangle);
}

/* HwpColor type */
HWP_DEFINE_BOXED_TYPE (HwpColor, hwp_color, hwp_color_copy, hwp_color_free)

/**
 * hwp_color_new:
 *
 * Creates a new #HwpColor
 *
 * Returns: a new #HwpColor, use hwp_color_free() to free it
 */
HwpColor *hwp_color_new (void)
{
  return (HwpColor *) g_new0 (HwpColor, 1);
}

/**
 * hwp_color_copy:
 * @color: a #HwpColor to copy
 *
 * Creates a copy of @color
 *
 * Returns: a new allocated copy of @color
 */
HwpColor *hwp_color_copy (HwpColor *color)
{
  HwpColor *new_color;

  new_color = g_new (HwpColor, 1);
  *new_color = *color;

  return new_color;
}

/**
 * hwp_color_free:
 * @color: a #HwpColor
 *
 * Frees the given #HwpColor
 */
void hwp_color_free (HwpColor *color)
{
  g_free (color);
}

/* HwpTextAttributes type */

HWP_DEFINE_BOXED_TYPE (HwpTextAttributes, hwp_text_attributes,
                       hwp_text_attributes_copy,
                       hwp_text_attributes_free)

/**
 * hwp_text_attributes_new:
 *
 * Creates a new #HwpTextAttributes
 *
 * Returns: a new #HwpTextAttributes, use hwp_text_attributes_free() to free it
 *
 * Since: 0.0.4
 */
HwpTextAttributes *hwp_text_attributes_new (void)
{
  return (HwpTextAttributes *) g_slice_new0 (HwpTextAttributes);
}

/**
 * hwp_text_attributes_copy:
 * @text_attrs: a #HwpTextAttributes to copy
 *
 * Creates a copy of @text_attrs
 *
 * Returns: a new allocated copy of @text_attrs
 *
 * Since: 0.0.4
 */
HwpTextAttributes *hwp_text_attributes_copy (HwpTextAttributes *text_attrs)
{
  HwpTextAttributes *attrs;

  attrs = g_slice_dup (HwpTextAttributes, text_attrs);
  attrs->font_name = g_strdup (text_attrs->font_name);
  return attrs;
}

/**
 * hwp_text_attributes_free:
 * @text_attrs: a #HwpTextAttributes
 *
 * Frees the given #HwpTextAttributes
 *
 * Since: 0.0.4
 */
void hwp_text_attributes_free (HwpTextAttributes *text_attrs)
{
  g_free (text_attrs->font_name);
  g_slice_free (HwpTextAttributes, text_attrs);
}

/**
 * hwp_page_get_text_attributes:
 * @page: A #HwpPage
 *
 * Obtains the attributes of the text as a GList of #HwpTextAttributes.
 * This list must be freed with hwp_page_free_text_attributes() when done.
 *
 * Each list element is a #HwpTextAttributes struct where start_index and
 * end_index indicates the range of text (as returned by hwp_page_get_text())
 * to which text attributes apply.
 *
 * Return value: (element-type HwpTextAttributes) (transfer full): A #GList of #HwpTextAttributes
 *
 * Since: 0.0.4
 */
GList *hwp_page_get_text_attributes (HwpPage *page)
{
  g_return_val_if_fail (HWP_IS_PAGE (page), NULL);
  return poppler_page_get_text_attributes (page->poppler_page);
}

/**
 * hwp_page_get_text_layout:
 * @page: A #HwpPage
 * @rectangles: (out) (array length=n_rectangles) (transfer container): return location for an array of #HwpRectangle
 * @n_rectangles: (out): length of returned array
 *
 * Obtains the layout of the text as a list of #HwpRectangle
 * This array must be freed with g_free () when done.
 *
 * The position in the array represents an offset in the text returned by
 * hwp_page_get_text()
 *
 * Return value: %TRUE if the page contains text, %FALSE otherwise
 *
 * Since: 0.0.4
 */
gboolean hwp_page_get_text_layout (HwpPage       *page,
                                   HwpRectangle **rectangles,
                                   guint         *n_rectangles)
{
  g_return_val_if_fail (HWP_IS_PAGE (page), FALSE);
  return poppler_page_get_text_layout (page->poppler_page,
                                       (PopplerRectangle **) rectangles,
                                       n_rectangles);
}

/**
 * hwp_page_free_text_attributes:
 * @list: (element-type HwpTextAttributes): A list of
 *   #HwpTextAttributes<!-- -->s
 *
 * Frees a list of #HwpTextAttributes<!-- -->s allocated by
 * hwp_page_get_text_attributes().
 *
 * Since: 0.0.4
 */
void hwp_page_free_text_attributes (GList *list)
{
  if (G_UNLIKELY (list == NULL))
    return;

  g_list_foreach (list, (GFunc) hwp_text_attributes_free, NULL);
  g_list_free (list);
}

/**
 * hwp_page_add_layout:
 * @page: #HwpPage
 * @layout: #HwpLayout
 *
 * Since: 0.1.2
 *
 */
void hwp_page_add_layout (HwpPage *page, HwpLayout *layout)
{
  g_ptr_array_add (page->layouts, layout);
}

/* HwpLayout type */

HWP_DEFINE_BOXED_TYPE (HwpLayout, hwp_layout, hwp_layout_copy, hwp_layout_free)

/**
 * hwp_layout_new:
 *
 * Creates a new #HwpLayout
 *
 * Returns: a new #HwpLayout, use hwp_layout_free() to free it
 */
HwpLayout *hwp_layout_new (void)
{
  return g_slice_new0 (HwpLayout);
}

/**
 * hwp_layout_copy:
 * @layout: a #HwpLayout to copy
 *
 * Creates a copy of @layout
 *
 * Returns: a new allocated copy of @layout
 */
HwpLayout *hwp_layout_copy (HwpLayout *layout)
{
  g_return_val_if_fail (layout != NULL, NULL);

  return g_slice_dup (HwpLayout, layout);
}

/**
 * hwp_layout_free:
 * @layout: a #HwpLayout
 *
 * Frees the given #HwpLayout
 */
void hwp_layout_free (HwpLayout *layout)
{
  g_slice_free (HwpLayout, layout);
}

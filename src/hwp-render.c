/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-render.c
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

#include "hwp-render.h"
#include "hwp-page.h"
#include "hwp-models.h"

/**
 * hwp_render_page:
 * @cr:
 * @page:
 *
 * Returns:
 *
 * Since: 0.1
 */
gboolean hwp_render_page (cairo_t *cr, HwpPage *page)
{
  g_return_val_if_fail (HWP_IS_PAGE (page), FALSE);
  g_return_val_if_fail (cr, FALSE);

  HwpParagraph *paragraph;

  for (guint i = 0; i < page->paragraphs->len; i++)
  {
    paragraph = g_array_index (page->paragraphs, HwpParagraph *, i);
    hwp_render_paragraph (cr, paragraph);
  }

  return TRUE;
}

/**
 * hwp_render_paragraph:
 * @cr:
 * @paragraph:
 *
 * Returns:
 *
 * Since: 0.1
 */
gboolean hwp_render_paragraph (cairo_t *cr, HwpParagraph *paragraph)
{
  g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), FALSE);
  g_return_val_if_fail (cr, FALSE);

  hwp_render_text (cr, paragraph->string->str);

  return TRUE;
}

/**
 * hwp_render_text:
 * @cr:
 * @text:
 *
 * Returns:
 *
 * Since: 0.1
 */
gboolean hwp_render_text (cairo_t *cr, const char *text)
{
  g_return_val_if_fail (cr,   FALSE);
  g_return_val_if_fail (text, FALSE);

  return TRUE;
}

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
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
 * hwp_page_get_size:
 * @page: #HwpPage
 * @width:
 * @hegiht:
 *
 * Returns:
 *
 * Since: 0.1
 */
void hwp_page_get_size (HwpPage *page, gdouble *width, gdouble *height)
{
  g_return_if_fail (HWP_IS_PAGE (page));

  *width  = 595.0;
  *height = 842.0;
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
  g_array_free (page->paragraphs, TRUE);
  G_OBJECT_CLASS (hwp_page_parent_class)->finalize (object);
}

static void hwp_page_class_init (HwpPageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize     = hwp_page_finalize;
}

static void hwp_page_init (HwpPage *page)
{
  page->paragraphs = g_array_new (TRUE, TRUE, sizeof (HwpParagraph *));
}

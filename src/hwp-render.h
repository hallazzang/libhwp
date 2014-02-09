/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-render.h
 *
 * Copyright (C) 2014 Hodong Kim <hodong@cogno.org>
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

#ifndef __HWP_RENDER_H__
#define __HWP_RENDER_H__

#include <glib-object.h>
#include <cairo.h>
#include "hwp-page.h"
#include "hwp-models.h"

G_BEGIN_DECLS

gboolean hwp_render_page      (cairo_t *cr, HwpPage      *page);
gboolean hwp_render_paragraph (cairo_t *cr, HwpParagraph *paragraph);
gboolean hwp_render_text      (cairo_t *cr, const gchar  *text);

G_END_DECLS

#endif /* __HWP_RENDER_H__ */

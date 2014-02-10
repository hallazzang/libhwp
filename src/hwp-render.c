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

#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library ft_lib;
static FT_Face    ft_face;
/*한 번만 초기화, 로드 */
static void once_ft_init_and_new (void)
{
  static gsize ft_init = 0;

  if (g_once_init_enter (&ft_init))
  {
    FT_Init_FreeType (&ft_lib);
    FT_New_Face (ft_lib, "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
                 0, &ft_face);
    g_once_init_leave (&ft_init, (gsize)1);
  }
}

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
  g_return_val_if_fail (cr, FALSE);
  g_return_val_if_fail (HWP_IS_PAGE (page), FALSE);

  cairo_save (cr);

  HwpParagraph *paragraph;

  double x = 20.0, y = 40.0;

  for (guint i = 0; i < page->paragraphs->len; i++)
  {
    paragraph = g_array_index (page->paragraphs, HwpParagraph *, i);
    hwp_render_paragraph (cr, paragraph, x, y);
  }

  cairo_restore (cr);

  return TRUE;
}

/**
 * hwp_render_paragraph:
 * @cr:
 * @paragraph:
 * @x:
 * @y:
 *
 * Returns:
 *
 * Since: 0.1
 */
gboolean
hwp_render_paragraph (cairo_t *cr, HwpParagraph *paragraph, double x, double y)
{
  g_return_val_if_fail (cr, FALSE);
  g_return_val_if_fail (HWP_IS_PARAGRAPH (paragraph), FALSE);

  guint         j, k;
  HwpTable     *table;
  HwpTableCell *cell;

  cairo_glyph_t        *glyphs = NULL; /* NULL로 지정하면 자동 할당됨 */
  cairo_scaled_font_t  *scaled_font;
  cairo_font_face_t    *font_face;
  cairo_matrix_t        font_matrix;
  cairo_matrix_t        ctm;
  cairo_font_options_t *font_options;
  cairo_text_extents_t  extents;

  /* create scaled font */
  once_ft_init_and_new(); /*한 번만 초기화, 로드*/
  font_face = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
  cairo_matrix_init_identity (&font_matrix);
  cairo_matrix_scale (&font_matrix, 12.0, 12.0);
  cairo_get_matrix (cr, &ctm);
  font_options = cairo_font_options_create ();
  cairo_get_font_options (cr, font_options);
  scaled_font = cairo_scaled_font_create (font_face,
                                         &font_matrix, &ctm, font_options);
  cairo_font_options_destroy (font_options);

  cairo_set_scaled_font(cr, scaled_font); /* 요 문장 없으면 fault 떨어짐 */
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

  x = 20.0;

  /* draw text */
  if (paragraph->string && !g_str_equal(paragraph->string->str, "\n\r"))
    hwp_render_text (cr, extents, glyphs, scaled_font,
                     paragraph->string->str, &x, &y);

  /* draw table */
  table = hwp_paragraph_get_table (paragraph);
  if (table != NULL)
  {
    for (j = 0; j < table->cells->len; j++)
    {
      cell = g_array_index(table->cells, HwpTableCell *, j);
      for (k = 0; k < cell->paragraphs->len; k++)
      {
        paragraph = g_array_index(cell->paragraphs, HwpParagraph *, k);
        if (paragraph->string)
        {
          /* FIXME x, y 좌표 */
          x = 40.0 + (595.5 - 40.0) / table->n_cols * cell->col_addr;
          if (cell->col_addr != 0)
              y -= 16.0;
          hwp_render_text (cr, extents, glyphs, scaled_font,
                           paragraph->string->str, &x, &y);
        }
      }
    }
  }

  cairo_glyph_free (glyphs);
  cairo_scaled_font_destroy (scaled_font);

  return TRUE;
}

/**
 * hwp_render_text:
 * @cr:
 * @extents:
 * @glyphs:
 * @scaled_font:
 * @text:
 * @x:
 * @y:
 *
 * Returns:
 *
 * Since: 0.1
 */
gboolean hwp_render_text (cairo_t             *cr,
                          cairo_text_extents_t extents,
                          cairo_glyph_t       *glyphs,
                          cairo_scaled_font_t *scaled_font,
                          const gchar         *text,
                          double              *x,
                          double              *y)
{
  g_return_val_if_fail (cr,   FALSE);
  g_return_val_if_fail (text, FALSE);

  gchar *ch = NULL;
  int    num_glyphs;
  guint  j;
  for (j = 0; j < g_utf8_strlen(text, -1); j++)
  {
    ch = g_utf8_substring(text, j, j+1);

    cairo_scaled_font_text_to_glyphs (scaled_font, *x, *y, ch, -1,
                                      &glyphs, &num_glyphs,
                                      NULL, NULL, NULL);
    g_free (ch);
    cairo_glyph_extents(cr, glyphs, num_glyphs, &extents);

    if (*x >= 595.0 - extents.x_advance - 20.0) {
      glyphs[0].x  = 20.0;
      glyphs[0].y += 16.0;
      *x  = 20.0 + extents.x_advance;
      *y += 16.0;
    }
    else {
      *x += extents.x_advance;
    }

    cairo_show_glyphs (cr, glyphs, num_glyphs);
  }

  *y += 18.0;

  return TRUE;
}

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-charset.c
 *
 * Copyright (C) 2013-2014 Hodong Kim <hodong@cogno.org>
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

#include <glib.h>
#include "hwp-charset.h"
#include <stdio.h>

/**
 * hwp_hnchar_to_utf8:
 * @c: a hnc character code
 *
 * Converts a single character to UTF-8.
 *
 * Return value: a pointer to a newly allocated UTF-8 string.
 *               This value must be freed with g_free(). If an
 *               error occurs, %NULL will be returned.
 *
 * Since: 0.0.1
 */
gchar *hwp_hnchar_to_utf8 (guint16 c)
{
  GString *string = g_string_new (NULL);
   /* ASCII printable characters */
  if (c >= 0x0020 && c <= 0x007e) {
      g_string_append_unichar (string, c);
  } else if (c >= 0x007f && c <= 0x3fff) {
      g_string_append_unichar (string, hnc2uni_page0[c]);
  /* 1수준 한자 4888자 */
  } else if (c >= 0x4000 && c <= 0x5317) {
      g_string_append_unichar (string, hnc2uni_page4[c-0x4000]);
  /* 2수준 한자 */
  } else if (c >= 0x5318 && c <= 0x7fff) {
      g_string_append_unichar (string, hnc2uni_page5[c-0x5318]);
  /* 한글 영역 */
  } else if (c >= 0x8000 && c <= 0xffff) {
    const gunichar2 *unichar2;
    unichar2 = hnc2uni_page8[c-0x8000];
    for (int i = 0; i < 3; i++) {
      if (unichar2[i] == 0)
        break;
      g_string_append_unichar (string, unichar2[i]);
    }
  } else {
    g_warning ("%04x: out of hnc code range", c);
  }
  return g_string_free (string, FALSE);
}

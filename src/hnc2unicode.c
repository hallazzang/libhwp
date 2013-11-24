/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hnc2unicode.h
 *
 * Copyright (C) 2013 Hodong Kim <cogniti@gmail.com>
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
#include "hnc2unicode.h"
#include <stdio.h>

/**
 * hwp_hnchar_to_utf8:
 * @c: a hnc character code
 *
 * Converts a single character to UTF-8.
 *
 * Return value: a pointer to a newly allocated UTF-8.
 *               This value must be freed with g_free(). If an
 *               error occurs, %NULL will be returned.
 *
 * Since: 0.2.1
 */
gchar *hwp_hnchar_to_utf8 (guint16 c)
{
  GString *string = g_string_new (NULL);
   /* ASCII printable characters */
  if (c >= 0x0020 && c <= 0x007e) {
      g_string_append_unichar (string, c);
      return g_string_free (string, FALSE);
  } else if (c >= 0x007f && c <= 0x3fff) {
      g_string_append_unichar (string, hnc2uni_page0[c]);
      return g_string_free (string, FALSE);
  /* 1수준 한자 4888자 */
  } else if (c >= 0x4000 && c <= 0x5317) {
      g_string_append_unichar (string, hnc2uni_page4[c-0x4000]);
      return g_string_free (string, FALSE);
  /* 2수준 한자 */
  } else if (c >= 0x5318 && c <= 0x7fff) {
      g_string_append_unichar (string, hnc2uni_page5[c-0x5318]);
      return g_string_free (string, FALSE);
  /* 한글 영역 */
  } else if (c >= 0x8000 && c <= 0xffff) {
      guint8 l = (c & 0x7c00) >> 10; /* 초성 */
      guint8 v = (c & 0x03e0) >> 5;  /* 중성 */
      guint8 t = (c & 0x001f);       /* 종성 */

      /* 조합형 현대 한글 음절(11172)을 유니코드로 변환 */
      if (L_MAP[l] != NONE && V_MAP[v] != NONE && T_MAP[t] != NONE) {
          guint16 syllable = 0xac00 + (L_MAP[l] * 21 * 28) +
                              (V_MAP[v] * 28) + T_MAP[t];
          g_string_append_unichar (string, syllable);
          return g_string_free (string, FALSE);
      /* 초성만 존재하는 경우 유니코드 한글 호환 자모로 변환 */
      } else if ((HNC_L1[l] != FILL) &&
                 (HNC_V1[v] == FILL  || HNC_V1[v] == NONE) &&
                 (HNC_T1[t] == FILL)) {
          g_string_append_unichar (string, HNC_L1[l]);
          return g_string_free (string, FALSE);
      /* 중성만 존재하는 경우 유니코드 한글 호환 자모로 변환 */
      } else if ((HNC_L1[l] == FILL) &&
                 (HNC_V1[v] != FILL  && HNC_V1[v] != NONE) &&
                 (HNC_T1[t] == FILL)) {
          g_string_append_unichar (string, HNC_V1[v]);
          return g_string_free (string, FALSE);
      /* 종성만 존재하는 경우 유니코드 한글 호환 자모로 변환 */
      } else if ((HNC_L1[l] == FILL) &&
                 (HNC_V1[v] == FILL  || HNC_V1[v] == NONE) &&
                 (HNC_T1[t] != FILL)) {
          g_string_append_unichar (string, HNC_T1[t]);
          return g_string_free (string, FALSE);
      /* 초성과 중성만 존재하는 조합형 옛한글의 경우 */
      } else if ((HNC_L1[l] != FILL) &&
                 (HNC_V1[v] != FILL  && HNC_V1[v] != NONE) &&
                 (HNC_T1[t] == FILL)) {
          g_string_append_unichar (string, HNC_L2[l]);
          g_string_append_unichar (string, HNC_V2[v]);
          return g_string_free (string, FALSE);
      /* 초성, 중성, 종성 모두 존재하는 조합형 옛한글의 경우 */
      } else if ((HNC_L1[l] != FILL) &&
                 (HNC_V1[v] != FILL  && HNC_V1[v] != NONE) &&
                 (HNC_T1[t] != FILL)) {
          g_string_append_unichar (string, HNC_L2[l]);
          g_string_append_unichar (string, HNC_V2[v]);
          g_string_append_unichar (string, HNC_T2[t]);
          return g_string_free (string, FALSE);
      /* 완성형 옛한글 한양PUA */
      } else if (v == 0) {
        const gunichar2 *unichar2;
        unichar2 = hnc2uni_page8[c - 0x8000];
        for (int i = 0; i < 3; i++) {
          if (i == 0)
            break;
          g_string_append_unichar (string, unichar2[i]);
        }
        return g_string_free (string, FALSE);
      } else {
          g_warning ("HNC code: %04x", c);
          g_string_free (string, TRUE);
          return NULL;
      }
  } else {
      g_warning ("%04x: out of hnc code range", c);
      g_string_free (string, TRUE);
      return NULL;
  }
}

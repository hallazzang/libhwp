/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
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

#ifndef _HNC2UNICODE_H_
#define _HNC2UNICODE_H_

#include <glib.h>

#define NONE 0xfd
#define FILL 0xff

/* HNC 조합형 11172 음절 --> 유니코드 변환용 표 */
static const unsigned short L_MAP[] = {
    NONE, NONE,    0,    1,    2,    3,    4,    5,
       6,    7,    8,    9,   10,   11,   12,   13,
      14,   15,   16,   17,   18,   19,   20, NONE,
    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE
};

static const unsigned short V_MAP[] = {
    NONE, NONE, NONE,    0,    1,    2,    3,    4,
    NONE, NONE,    5,    6,    7,    8,    9,   10,
    NONE, NONE,   11,   12,   13,   14,   15,   16,
    NONE, NONE,   17,   18,   19,   20, NONE, NONE
};

static const unsigned short T_MAP[] = {
    NONE,    0,    1,    2,    3,    4,    5,    6,
       7,    8,    9,   10,   11,   12,   13,   14,
      15,   16, NONE,   17,   18,   19,   20,   21,
      22,   23,   24,   25,   26,   27, NONE, NONE
};

/* HNC 자모 변환용 표 */
static const unsigned int HNC_L1[] = {
    0x3172,   FILL, 0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139,
    0x3141, 0x3142, 0x3143, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149,
    0x314a, 0x314b, 0x314c, 0x314d, 0x314e, 0x3173, 0x3144, 0x3176,
    0x3178, 0x317a, 0x317c, 0x317d, 0x317e, 0x317f, 0x3181, 0x3185
};

static const unsigned int HNC_V1[] = {
      NONE,   NONE,   FILL, 0x314f, 0x3150, 0x3151, 0x3152, 0x3153,
      NONE,   NONE, 0x3154, 0x3155, 0x3156, 0x3157, 0x3158, 0x3159,
      NONE, 0x3189, 0x315a, 0x315b, 0x315c, 0x315d, 0x315e, 0x315f,
    0x318a, 0x318c, 0x3160, 0x3161, 0x3162, 0x3163, 0x318d, 0x318e
};

static const unsigned int HNC_T1[] = {
    0x316d,   FILL, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136,
    0x3137, 0x3139, 0x313a, 0x313b, 0x313c, 0x313d, 0x313e, 0x313f,
    0x3140, 0x3141, 0x3178, 0x3142, 0x3144, 0x3145, 0x3146, 0x3147,
    0x3148, 0x314a, 0x314b, 0x314c, 0x314d, 0x314e, 0x317f, 0x3181
};

/* HNC 옛한글 조합용 표 */
static const unsigned int HNC_L2[] = {
    0x111e, 0x115f, 0x1100, 0x1101, 0x1102, 0x1103, 0x1104, 0x1105,
    0x1106, 0x1107, 0x1108, 0x1109, 0x110a, 0x110b, 0x110c, 0x110d,
    0x110e, 0x110f, 0x1110, 0x1111, 0x1112, 0x1120, 0x1121, 0x1127,
    0x112b, 0x112d, 0x112f, 0x1132, 0x1136, 0x1140, 0x114c, 0x1158
};

static const unsigned int HNC_V2[] = {
      NONE,   NONE, 0x1160, 0x1161, 0x1162, 0x1163, 0x1164, 0x1165,
      NONE,   NONE, 0x1166, 0x1167, 0x1168, 0x1169, 0x116a, 0x116b,
      NONE, 0x1188, 0x116c, 0x116d, 0x116e, 0x116f, 0x1170, 0x1171,
    0x1191, 0x1194, 0x1172, 0x1173, 0x1174, 0x1175, 0x119e, 0x11a1
};

static const unsigned int HNC_T2[] = {
    0x11d9,   NONE, 0x11a8, 0x11a9, 0x11aa, 0x11ab, 0x11ac, 0x11ad,
    0x11ae, 0x11af, 0x11b0, 0x11b1, 0x11b2, 0x11b3, 0x11b4, 0x11b5,
    0x11b6, 0x11b7, 0x11e6, 0x11b8, 0x11b9, 0x11ba, 0x11bb, 0x11bc,
    0x11bd, 0x11be, 0x11bf, 0x11c0, 0x11c1, 0x11c2, 0x11eb, 0x11f0
};

gchar *hnc_to_utf8 (guint16 c);

#endif /* _HNC2UNICODE_H_ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-file-v3.h
 *
 * Copyright (C) 2013 Hodong Kim <hodong@cogno.org>
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

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#ifndef __HWP_FILE_V3_H__
#define __HWP_FILE_V3_H__

#include <glib-object.h>

#include "hwp-file.h"
#include "hwp-models.h"

G_BEGIN_DECLS

#define HWP_TYPE_FILE_V3             (hwp_file_v3_get_type ())
#define HWP_FILE_V3(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_FILE_V3, HWPFileV3))
#define HWP_FILE_V3_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_FILE_V3, HWPFileV3Class))
#define HWP_IS_FILE_V3(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_FILE_V3))
#define HWP_IS_FILE_V3_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_FILE_V3))
#define HWP_FILE_V3_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_FILE_V3, HWPFileV3Class))

typedef struct _HWPFileV3        HWPFileV3;
typedef struct _HWPFileV3Class   HWPFileV3Class;
typedef struct _HWPFileV3Private HWPFileV3Private;

struct _HWPFileV3Class {
  HWPFileClass parent_class;
};

struct _HWPFileV3 {
  HWPFile           parent_instance;
  HWPFileV3Private *priv;

  HWPDocument      *document;
  guint16           is_crypt;
  guint8            is_compress;
  guint8            rev;
  guint16           info_block_len;
  HWPPage          *page;
};

struct _HWPFileV3Private {
  GInputStream *stream;
};

GType        hwp_file_v3_get_type               (void) G_GNUC_CONST;
HWPFileV3   *hwp_file_v3_new_for_path           (const gchar *path,
                                                 GError     **error);
gchar       *hwp_file_v3_get_hwp_version_string (HWPFile     *file);
void         hwp_file_v3_get_hwp_version        (HWPFile     *file,
                                                 guint8      *major_version,
                                                 guint8      *minor_version,
                                                 guint8      *micro_version,
                                                 guint8      *extra_version);
HWPDocument *hwp_file_v3_get_document           (HWPFile     *file,
                                                 GError     **error);

G_END_DECLS

#endif /* __HWP_FILE_V3_H__ */
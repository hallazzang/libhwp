/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-file.h
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

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_HWP3_FILE_H__
#define __HWP_HWP3_FILE_H__

#include <glib-object.h>

#include "hwp-file.h"
#include "hwp-models.h"

G_BEGIN_DECLS

#define HWP_TYPE_HWP3_FILE             (hwp_hwp3_file_get_type ())
#define HWP_HWP3_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_HWP3_FILE, HwpHWP3File))
#define HWP_HWP3_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_HWP3_FILE, HwpHWP3FileClass))
#define HWP_IS_HWP3_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_HWP3_FILE))
#define HWP_IS_HWP3_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_HWP3_FILE))
#define HWP_HWP3_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_HWP3_FILE, HwpHWP3FileClass))

typedef struct _HwpHWP3File        HwpHWP3File;
typedef struct _HwpHWP3FileClass   HwpHWP3FileClass;
typedef struct _HwpHWP3FilePrivate HwpHWP3FilePrivate;

struct _HwpHWP3FileClass
{
  HwpFileClass parent_class;
};

struct _HwpHWP3File
{
  HwpFile             parent_instance;
  HwpHWP3FilePrivate *priv;

  guint8              major_version;
  guint8              minor_version;
  guint8              micro_version;
  guint8              extra_version;

  guint16             is_crypt;
  guint8              is_compress;
  guint8              rev;
  guint16             info_block_len;
};

struct _HwpHWP3FilePrivate
{
  GInputStream *stream;
};

GType        hwp_hwp3_file_get_type               (void) G_GNUC_CONST;

HwpHWP3File *hwp_hwp3_file_new_for_path           (const gchar *path,
                                                   GError     **error);
gchar       *hwp_hwp3_file_get_hwp_version_string (HwpFile     *file);
void         hwp_hwp3_file_get_hwp_version        (HwpFile     *file,
                                                   guint8      *major_version,
                                                   guint8      *minor_version,
                                                   guint8      *micro_version,
                                                   guint8      *extra_version);
HwpDocument *hwp_hwp3_file_get_document           (HwpFile     *file,
                                                   GError     **error);

G_END_DECLS

#endif /* __HWP_HWP3_FILE_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp5-file.h
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

#ifndef __HWP_HWP5_FILE_H__
#define __HWP_HWP5_FILE_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-infile-msole.h>

#include "hwp-file.h"

G_BEGIN_DECLS

#define HWP_TYPE_HWP5_FILE             (hwp_hwp5_file_get_type ())
#define HWP_HWP5_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_HWP5_FILE, HwpHWP5File))
#define HWP_HWP5_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_HWP5_FILE, HwpHWP5FileClass))
#define HWP_IS_HWP5_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_HWP5_FILE))
#define HWP_IS_HWP5_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_HWP5_FILE))
#define HWP_HWP5_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_HWP5_FILE, HwpHWP5FileClass))

typedef struct _HwpHWP5File        HwpHWP5File;
typedef struct _HwpHWP5FileClass   HwpHWP5FileClass;
typedef struct _HwpHWP5FilePrivate HwpHWP5FilePrivate;

struct _HwpHWP5File
{
  HwpFile             parent_instance;
  HwpHWP5FilePrivate *priv;

  GArray             *section_streams;
  GInputStream       *prv_text_stream;
  GInputStream       *prv_image_stream;
  GInputStream       *file_header_stream;
  GInputStream       *doc_info_stream;
  GInputStream       *summary_info_stream;

  gchar              *signature;
  guint8              major_version;
  guint8              minor_version;
  guint8              micro_version;
  guint8              extra_version;
  gboolean            is_compress;
  gboolean            is_encrypt;
  gboolean            is_distribute;
  gboolean            is_script;
  gboolean            is_drm;
  gboolean            is_xml_template;
  gboolean            is_history;
  gboolean            is_sign;
  gboolean            is_certificate_encrypt;
  gboolean            is_sign_spare;
  gboolean            is_certificate_drm;
  gboolean            is_ccl;
};

struct _HwpHWP5FileClass
{
  HwpFileClass parent_class;
};

struct _HwpHWP5FilePrivate
{
  GsfInfile    *olefile;
  GInputStream *section_stream;
};

GType        hwp_hwp5_file_get_type               (void) G_GNUC_CONST;
HwpHWP5File *hwp_hwp5_file_new_for_path           (const gchar *path,
                                                   GError     **error);
gchar       *hwp_hwp5_file_get_hwp_version_string (HwpFile     *file);
void         hwp_hwp5_file_get_hwp_version        (HwpFile     *file,
                                                   guint8      *major_version,
                                                   guint8      *minor_version,
                                                   guint8      *micro_version,
                                                   guint8      *extra_version);
gboolean     hwp_hwp5_file_check_version          (HwpHWP5File *file,
                                                   guint8       major,
                                                   guint8       minor,
                                                   guint8       micro,
                                                   guint8       extra);
HwpDocument *hwp_hwp5_file_get_document           (HwpFile     *file,
                                                   GError     **error);

G_END_DECLS

#endif /* __HWP_HWP5_FILE_H__ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-file-v5.h
 *
 * Copyright (C) 2012-2013 Hodong Kim <cogniti@gmail.com>
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

#ifndef _HWP_FILE_V5_H_
#define _HWP_FILE_V5_H_

#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-infile-msole.h>

#include "hwp-file.h"

G_BEGIN_DECLS

#define HWP_TYPE_FILE_V5             (hwp_file_v5_get_type ())
#define HWP_FILE_V5(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_FILE_V5, HWPFileV5))
#define HWP_FILE_V5_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_FILE_V5, HWPFileV5Class))
#define HWP_IS_FILE_V5(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_FILE_V5))
#define HWP_IS_FILE_V5_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_FILE_V5))
#define HWP_FILE_V5_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_FILE_V5, HWPFileV5Class))

typedef struct _HWPFileV5        HWPFileV5;
typedef struct _HWPFileV5Class   HWPFileV5Class;
typedef struct _HWPFileV5Private HWPFileV5Private;

struct _HWPFileV5
{
    HWPFile           parent_instance;
    HWPFileV5Private *priv;

    GArray            *section_streams;
    GInputStream      *prv_text_stream;
    GInputStream      *prv_image_stream;
    GInputStream      *file_header_stream;
    GInputStream      *doc_info_stream;
    GInputStream      *summary_info_stream;

    gchar   *signature;
    guint8   major_version;
    guint8   minor_version;
    guint8   micro_version;
    guint8   extra_version;
    gboolean is_compress;
    gboolean is_encrypt;
    gboolean is_distribute;
    gboolean is_script;
    gboolean is_drm;
    gboolean is_xml_template;
    gboolean is_history;
    gboolean is_sign;
    gboolean is_certificate_encrypt;
    gboolean is_sign_spare;
    gboolean is_certificate_drm;
    gboolean is_ccl;
};

struct _HWPFileV5Class
{
    HWPFileClass parent_class;
};

struct _HWPFileV5Private
{
    GsfInfileMSOle *olefile;
    GInputStream   *section_stream;
};

GType         hwp_file_v5_get_type               (void) G_GNUC_CONST;
HWPFileV5   *hwp_file_v5_new_from_uri           (const gchar *uri,
                                                   GError     **error);
HWPFileV5   *hwp_file_v5_new_from_filename      (const gchar *filename,
                                                   GError     **error);
gchar        *hwp_file_v5_get_hwp_version_string (HWPFile    *file);
void          hwp_file_v5_get_hwp_version        (HWPFile    *file,
                                                   guint8      *major_version,
                                                   guint8      *minor_version,
                                                   guint8      *micro_version,
                                                   guint8      *extra_version);
gboolean      hwp_file_v5_check_version          (HWPFileV5 *file,
                                                   guint8      major,
                                                   guint8      minor,
                                                   guint8      micro,
                                                   guint8      extra);
HWPDocument *hwp_file_v5_get_document           (HWPFile    *file,
                                                   GError     **error);

G_END_DECLS

#endif /* _HWP_FILE_V5_H_ */

/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file-v3.h
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

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#ifndef _GHWP_FILE_V3_H_
#define _GHWP_FILE_V3_H_

#include <glib-object.h>

#include "ghwp.h"

G_BEGIN_DECLS

#define GHWP_TYPE_FILE_V3             (ghwp_file_v3_get_type ())
#define GHWP_FILE_V3(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_FILE_V3, GHWPFileV3))
#define GHWP_FILE_V3_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_FILE_V3, GHWPFileV3Class))
#define GHWP_IS_FILE_V3(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_FILE_V3))
#define GHWP_IS_FILE_V3_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_FILE_V3))
#define GHWP_FILE_V3_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_FILE_V3, GHWPFileV3Class))

typedef struct _GHWPFileV3Class   GHWPFileV3Class;
typedef struct _GHWPFileV3Private GHWPFileV3Private;

struct _GHWPFileV3Class 
{
    GHWPFileClass parent_class;
};

struct _GHWPFileV3
{
    GHWPFile           parent_instance;
    GHWPFileV3Private *priv;

    GHWPDocument      *document;
    guint16            is_crypt;
    guint8             is_compress;
    guint8             rev;
    guint16            info_block_len;
    GHWPPage          *page;
};

struct _GHWPFileV3Private
{
    GInputStream *stream;
};

GType         ghwp_file_v3_get_type               (void) G_GNUC_CONST;
GHWPFileV3   *ghwp_file_v3_new_from_uri           (const gchar *uri,
                                                   GError     **error);
GHWPFileV3   *ghwp_file_v3_new_from_filename      (const gchar *filename,
                                                   GError     **error);
gchar        *ghwp_file_v3_get_hwp_version_string (GHWPFile    *file);
void          ghwp_file_v3_get_hwp_version        (GHWPFile    *file,
                                                   guint8      *major_version,
                                                   guint8      *minor_version,
                                                   guint8      *micro_version,
                                                   guint8      *extra_version);
GHWPDocument *ghwp_file_v3_get_document           (GHWPFile    *file,
                                                   GError     **error);

G_END_DECLS

#endif /* _GHWP_FILE_V3_H_ */

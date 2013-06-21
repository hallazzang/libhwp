/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file-v3.c
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

#include "ghwp-file-v3.h"

G_DEFINE_TYPE (GHWPFileV3, ghwp_file_v3, GHWP_TYPE_FILE);

GHWPFileV3 *ghwp_file_v3_new_from_uri (const gchar *uri, GError **error)
{
    return NULL;
}

GHWPFileV3 *ghwp_file_v3_new_from_filename (const gchar *filename,
                                            GError     **error)
{
    return NULL;
}

gchar *ghwp_file_v3_get_hwp_version_string (GHWPFile *file)
{
    return NULL;
}

void ghwp_file_v3_get_hwp_version (GHWPFile *file,
                                   guint8   *major_version,
                                   guint8   *minor_version,
                                   guint8   *micro_version,
                                   guint8   *extra_version)
{

}

GHWPDocument *ghwp_file_v3_get_document (GHWPFile *file, GError **error)
{
    return NULL;
}

static void ghwp_file_v3_init (GHWPFileV3 *file)
{
    file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file, GHWP_TYPE_FILE_V3,
                                                    GHWPFileV3Private);
}

static void ghwp_file_v3_finalize (GObject *object)
{
    G_OBJECT_CLASS (ghwp_file_v3_parent_class)->finalize (object);
}

static void ghwp_file_v3_class_init (GHWPFileV3Class *klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (GHWPFileV3Private));
    GHWP_FILE_CLASS (klass)->get_document = ghwp_file_v3_get_document;
    GHWP_FILE_CLASS (klass)->get_hwp_version_string = ghwp_file_v3_get_hwp_version_string;
    GHWP_FILE_CLASS (klass)->get_hwp_version = ghwp_file_v3_get_hwp_version;
    object_class->finalize = ghwp_file_v3_finalize;
}

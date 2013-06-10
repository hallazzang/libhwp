/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file.c
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

#include <glib.h>
#include <glib-object.h>
#include <string.h>

#include "ghwp-file.h"
#include "ghwp-file-v5.h"
#include "ghwp-file-v3.h"

G_DEFINE_TYPE (GHWPFile, ghwp_file, G_TYPE_OBJECT);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

#define GHWP_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_FILE, GHWPFilePrivate))

static void      ghwp_file_finalize (GObject* obj);

GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar    *filename = g_filename_from_uri (uri, NULL, error);
    GHWPFile *file     = ghwp_file_new_from_filename (filename, error);
    _g_free0 (filename);

    return file;
}


GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);

    /* check signature */
	static guint8 const signature_ole[] =
		{ 0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };

	static guint8 const signature_v3[] =
        { 0x48, 0x57, 0x50, 0x20, 0x44, 0x6f, 0x63, 0x75, 0x6d, 0x65,
          0x6e, 0x74, 0x20, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x56, 0x33,
          0x2e, 0x30, 0x30, 0x20, 0x1a, 0x01, 0x02, 0x03, 0x04, 0x05 };

    GFile            *file   = g_file_new_for_path (filename);
    GFileInputStream *stream = g_file_read(file, NULL, error);

    if (!stream)
        return NULL;

    gsize bytes_read = 0;
    guint8 *buffer = g_malloc((gsize)30);
    g_input_stream_read_all (G_INPUT_STREAM(stream), buffer, (gsize) 30,
                             &bytes_read, NULL, error);

    if (bytes_read != 30) {
        g_free(buffer);
        return NULL;
    }

    if ( memcmp(buffer, signature_ole, sizeof(signature_ole)) == 0) {
        /* hwp v5 */
        g_free(buffer);
        g_object_unref(stream);
        return GHWP_FILE (ghwp_file_v5_new_from_filename (filename, error));
    } else if ( memcmp(buffer, signature_v3, sizeof(signature_v3)) == 0) {
        /* hwp v3 */
        g_free(buffer);
        g_object_unref(stream);
        return NULL;
/*        return (GHWP_FILE) ghwp_file_v3_new_from_filename (filename, error);*/
    } else {
        /* not a valid hwp file */
        *error = g_error_new (ghwp_error_quark(), 0, "not a valid hwp file");
        g_free(buffer);
        g_object_unref(stream);
        return NULL;
    }
}

static void ghwp_file_class_init (GHWPFileClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (GHWPFilePrivate));
    object_class->finalize = ghwp_file_finalize;
}


static void ghwp_file_init (GHWPFile * file)
{
    file->priv = GHWP_FILE_GET_PRIVATE (file);
}

static void ghwp_file_finalize (GObject* obj)
{
    GHWPFile *file = GHWP_FILE(obj);
    _g_object_unref0 (file->priv->olefile);
    _g_object_unref0 (file->prv_text_stream);
    _g_object_unref0 (file->prv_image_stream);
    _g_object_unref0 (file->file_header_stream);
    _g_object_unref0 (file->doc_info_stream);
    _g_array_free0 (file->section_streams);
    _g_object_unref0 (file->priv->section_stream);
    _g_object_unref0 (file->summary_info_stream);
    g_free (file->signature);
    G_OBJECT_CLASS (ghwp_file_parent_class)->finalize (obj);
}
